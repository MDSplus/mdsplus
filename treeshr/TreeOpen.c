/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define _GNU_SOURCE
#include "treeshrp.h" /* must be first or off_t wrong */
#include <ctype.h>
#include <libroutines.h>
#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <stdlib.h>
#include <string.h>
#include <strroutines.h>
#include <treeshr.h>

#include <mdsplus/mdsconfig.h>
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif

#ifdef _WIN32
#include <io.h>
#include <windows.h>
inline static char *strndup(const char *src, size_t n)
{
  size_t len = strnlen(src, n);
  char *dst = memcpy(malloc(len + 1), src, len);
  dst[len] = '\0';
  return dst;
}
#else
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#endif
#include <errno.h>
#include <fcntl.h>
#include <ncidef.h>
#include <usagedef.h>

#ifdef HAVE_SYS_RESOURCE_H
#ifdef __APPLE__
#include <sys/time.h>
#endif
#include <sys/resource.h>
#endif

//#define DEBUG
#include <mdsmsg.h>

int treeshr_errno = 0;
extern int MDSEventCan();
static void RemoveBlanksAndUpcase(char *out, char const *in);
static int close_top_tree(PINO_DATABASE *dblist, int call_hook);
static int ConnectTree(PINO_DATABASE *dblist, char *tree, NODE *parent,
                       char *subtree_list);
static int CreateDbSlot(PINO_DATABASE **dblist, char *tree, int shot,
                        int editting);
static int MapFile(int fd, TREE_INFO *info, int nomap);
static int GetVmForTree(TREE_INFO *info, int nomap);
static int MapTree(TREE_INFO *info, TREE_INFO *root, int edit_flag);
static void SubtreeNodeConnect(PINO_DATABASE *dblist, NODE *parent,
                               NODE *subtreetop);
extern void TreeCleanupConnections();
extern void **TreeCtx();

int TreeClose(char const *tree, int shot)
{
  return _TreeClose(TreeCtx(), tree, shot);
}

int TreeEditing() { return _TreeEditing(*TreeCtx()); }

int TreeGetStackSize() { return _TreeGetStackSize(*TreeCtx()); }

int TreeIsOpen() { return _TreeIsOpen(*TreeCtx()); }

int TreeOpen(char const *tree, int shot, int read_only)
{
  return _TreeOpen(TreeCtx(), tree, shot, read_only);
}

int TreeSetStackSize(int size) { return _TreeSetStackSize(TreeCtx(), size); }

void TreeRestoreContext(void *ctx) { _TreeRestoreContext(TreeCtx(), ctx); }

void *TreeSaveContext() { return _TreeSaveContext(*TreeCtx()); }

int TreeOpenEdit(char const *tree, int shot)
{
  return _TreeOpenEdit(TreeCtx(), tree, shot);
}

int TreeOpenNew(char const *tree, int shot)
{
  return _TreeOpenNew(TreeCtx(), tree, shot);
}

EXPORT char *TreePath(char const *tree, char *tree_lower_out)
{
  size_t i;
  char pathname[32];
  char *path;
  char tree_lower[sizeof(TREE_NAME) + 1];
  for (i = 0; i < 12 && tree[i]; ++i)
    tree_lower[i] = tolower(tree[i]);
  tree_lower[i] = '\0';
  strcpy(pathname, tree_lower);
  strcat(pathname, TREE_PATH_SUFFIX);
  if (tree_lower_out)
    strcpy(tree_lower_out, tree_lower);
  path = TranslateLogical(pathname);
  if (path == NULL)
    path = TranslateLogical(TREE_DEFAULT_PATH);
  if (path)
  {
    // remove trailing spaces
    for (i = strlen(path); i > 0 && (path[i - 1] == ' ' || path[i - 1] == 9);
         i--)
      path[i - 1] = '\0';
    if (!*path)
    {
      // ignore empty path
      free(path);
      path = NULL;
    }
  }
  return path;
}

static char *ReplaceAliasTrees(char *tree_in)
{
  size_t buflen = strlen(tree_in) + 1;
  char *ans = calloc(1, buflen);
  char *tree = strtok(tree_in, ",");
  size_t i;
  while (tree)
  {
    char *treepath = TreePath(tree, 0);
    if (treepath && (strlen(treepath) > 5) &&
        (strncmp(treepath, "ALIAS", 5) == 0))
      tree = &treepath[5];
    if ((buflen - strlen(ans)) < (strlen(tree) + 1))
    {
      buflen += (strlen(tree) + 1);
      ans = realloc(ans, buflen);
    }
    if (strlen(ans) == 0)
      strcpy(ans, tree);
    else
    {
      strcat(ans, ",");
      strcat(ans, tree);
    }
    free(treepath);
    tree = strtok(0, ",");
  }
  free(tree_in);
  for (i = 0; i < buflen; ++i)
    ans[i] = (char)toupper(ans[i]);
  return ans;
}

static void free_top_db(PINO_DATABASE **dblist)
{
  if ((*dblist)->next)
  {
    PINO_DATABASE *db = *dblist;
    *dblist = (*dblist)->next;
    db->next = NULL;
    TreeFreeDbid(db);
  }
}

EXPORT int _TreeOpen(void **dbid, char const *tree_in, int shot_in,
                     int read_only_flag)
{
  int status;
  int shot;
  char *tree = strdup(tree_in);
  char *subtree_list = 0;
  char *comma_ptr;

  RemoveBlanksAndUpcase(tree, tree_in);
  tree = ReplaceAliasTrees(tree);
  if ((comma_ptr = strchr(tree, ',')) != 0)
  {
    subtree_list = strdup(tree);
    *comma_ptr = '\0';
  }

  /**************************************************
   To open a tree we need only to connect up the MAIN
   tree which will in turn link up any subtrees.
  **************************************************/

  shot = shot_in ? shot_in : TreeGetCurrentShotId(tree);
  if (shot < -1)
    status = TreeINVSHOT;
  else if (shot == 0)
    status = TreeNOCURRENT;
  else
  {
    char *path = TreePath(tree, 0);
    if (path)
    {
      PINO_DATABASE **dblist = (PINO_DATABASE **)dbid;
      int db_slot_status = CreateDbSlot(dblist, tree, shot, 0);
      if (db_slot_status == TreeSUCCESS || db_slot_status == TreeALREADY_OPEN)
      {
        status = ConnectTree(*dblist, tree, 0, subtree_list);
        if (status == TreeUNSUPTHICKOP)
          if (strlen(path) > 2 && path[strlen(path) - 2] == ':' &&
              path[strlen(path) - 1] == ':')
            status = ConnectTreeRemote(*dblist, tree, subtree_list, path);
        if (status == TreeSUCCESS || status == TreeNOTALLSUBS)
        {
          if (db_slot_status == TreeSUCCESS)
            (*dblist)->default_node = (*dblist)->tree_info->root;
          (*dblist)->open = 1;
          (*dblist)->open_readonly = read_only_flag != 0;
        }
        else
          free_top_db(dblist);
      }
      else
      {
        status = db_slot_status;
      }
      free(path);
    }
    else
      status = TreeNOPATH;
  }
  free(subtree_list);
  free(tree);
  return status;
}

static void RemoveBlanksAndUpcase(char *out, char const *in)
{
  while (*in)
  {
    char c = *in++;
    if (!isspace(c))
      *out++ = (char)toupper(c);
  }
  *out = 0;
}

int _TreeClose(void **dbid, char const *tree, int shot)
{
  PINO_DATABASE **dblist = (PINO_DATABASE **)dbid;
  int status;
  PINO_DATABASE *db;
  PINO_DATABASE *prev_db;
  status = TreeNOT_OPEN;

  // printf("TREE CLOSE\n");
  if (dblist && *dblist)
  {
    if (tree)
    {
      size_t i;
      char uptree[sizeof(TREE_NAME) + 1] = {0};
      size_t len = strlen(tree);
      for (i = 0; i < 12 && i < len; ++i)
        uptree[i] = (char)toupper(tree[i]);
      status = TreeNOT_OPEN;
      if (!shot)
        shot = TreeGetCurrentShotId(tree);
      for (prev_db = 0, db = *dblist; db ? db->open : 0;
           prev_db = db, db = db->next)
      {
        if ((shot == db->shotid) && (strcmp(db->main_treenam, uptree) == 0))
        {
          if (prev_db)
          {
            prev_db->next = db->next;
            db->next = *dblist;
            *dblist = db;
          }
          status = TreeSUCCESS;
          break;
        }
      }
    }
    else
      status = TreeSUCCESS;
    if (STATUS_OK)
    {
      if ((*dblist)->modified)
      {
        status = TreeWRITEFIRST;
      }
      else if ((*dblist)->open)
      {
        status = close_top_tree(*dblist, 1);
        if (STATUS_OK)
          free_top_db(dblist);
      }
      else
        status = TreeNOT_OPEN;
    }
  }
  TreeCleanupConnections();
  return status;
}

static int close_top_tree(PINO_DATABASE *dblist, int call_hook)
{
  int status = TreeSUCCESS;
  if (!dblist)
    return status;
  if (dblist->dispatch_table)
  {
    static int (*ServerFreeDispatchTable)() = NULL;
    status = LibFindImageSymbol_C("MdsServerShr", "ServerFreeDispatchTable",
                                  &ServerFreeDispatchTable);
    if (STATUS_OK)
    {
      status = ServerFreeDispatchTable(dblist->dispatch_table);
      dblist->dispatch_table = NULL;
    }
  }
  if (dblist->remote)
  {
    status = CloseTreeRemote(dblist, call_hook);
    if (status == TreeNOT_OPEN) /**** Remote server might have already closed
                                   the tree ****/
      status = TreeSUCCESS;
    return status;
  }
  TREE_INFO *previous_info, *local_info = dblist->tree_info;
  if (local_info)
  {
    /************************************************
     We check the BLOCKID just to make sure that what
     we were passed in indeed was a tree info block.
     This is a safety check because if we just assumed
     the block was correct, mapping and freeing random
     pages of memory would cause serious problems that
     may not show up immediately.
    *************************************************/

    if (local_info->blockid == TreeBLOCKID)
    {

      /******************************************************
       If the edit pointer field of this tree is non-zero,
       the tree has been opened for editting. The edit pointer
       points to a structure containing the number of virtual
       memory pages that were dynamically allocated for each
       section of the tree. If any of the tree sections had
       virtual memory allocated, this memory must be freed.
      *****************************************************/

      if (local_info->edit)
      {
        if (local_info->edit->header_pages)
          free(local_info->header);
        if (local_info->edit->node_vm_size)
          free(local_info->node);
        if (local_info->edit->tags_pages)
          free(local_info->tags);
        if (local_info->edit->tag_info_pages)
          free(local_info->tag_info);
        if (local_info->edit->external_pages)
          free(local_info->external);
        DELETED_NID *next, *dnid;
        for (dnid = local_info->edit->deleted_nid_list; dnid; dnid = next)
        {
          next = dnid->next;
          free(dnid);
        }
        free(local_info->edit);
      }
      /********************************************************
      For each tree in the linked list, first the pages must be
      remapped to the pagefile using sys$cretva before they can
      be freed for general use. This automatically unmaps the
      tree section file pages. Once the section is unmapped the
      I/O channel can be deassigned. Then the virtual memory
      can be deallocated using lib$free_vm_page.
      Finally the tree information block memory can be released.
      *********************************************************/

      while (local_info)
      {
        if (local_info->blockid == TreeBLOCKID)
        {
          if (local_info->rundown_id)
            MDSEventCan(local_info->rundown_id);
          if (local_info->channel)
            MDS_IO_CLOSE(local_info->channel);
          if (local_info->section_addr[0])
          {
#ifndef _WIN32
            if (local_info->mapped)
              munmap(local_info->section_addr[0],
                     (size_t)local_info->alq * 512);
#endif
            free(local_info->vm_addr);
          }
          TreeWait(local_info);
          if (local_info->data_file)
          {
            MdsFree1Dx(local_info->data_file->data, NULL);
            if (local_info->data_file->get)
              MDS_IO_CLOSE(local_info->data_file->get);
            if (local_info->data_file->put)
              MDS_IO_CLOSE(local_info->data_file->put);
            free(local_info->data_file);
            local_info->data_file = NULL;
          }
          if (local_info->nci_file)
          {
            if (local_info->nci_file->get)
              MDS_IO_CLOSE(local_info->nci_file->get);
            if (local_info->nci_file->put)
              MDS_IO_CLOSE(local_info->nci_file->put);
            free(local_info->nci_file);
            local_info->nci_file = NULL;
          }
          if (call_hook)
          {
            TreeCallHookFun("TreeHook", "CloseTree", local_info->treenam,
                            local_info->shot, NULL);
            TreeCallHook(CloseTree, local_info, 0);
          }
          free(local_info->filespec);
          free(local_info->treenam);
          if (local_info->has_lock)
            pthread_rwlock_destroy(&local_info->lock);
          previous_info = local_info;
          local_info = local_info->next_info;
          free(previous_info);
        }
      }
      dblist->tree_info = NULL;
    }
    else
      status = TreeINVTREE;
  }
  if (STATUS_OK)
  {
    dblist->open = 0;
    dblist->open_for_edit = 0;
    dblist->modified = 0;
    dblist->remote = 0;
    free(dblist->experiment);
    dblist->experiment = NULL;
    free(dblist->main_treenam);
    dblist->main_treenam = NULL;
  }
  return status;
}

int _TreeIsOpen(void *dbid)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  return IS_OPEN_FOR_EDIT(dblist) ? TreeOPEN_EDIT
                                  : (IS_OPEN(dblist) ? TreeOPEN : TreeNOT_OPEN);
}

int _TreeEditing(void *dbid)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  return IS_OPEN_FOR_EDIT(dblist) ? TreeSUCCESS : TreeNOEDIT;
}

static int ConnectTree(PINO_DATABASE *dblist, char *tree, NODE *parent,
                       char *subtree_list)
{
  /***********************************************
    If the parent's usage is not subtree then
    just return success.
  ************************************************/
  if (parent && parent->usage != TreeUSAGE_SUBTREE)
    return TreeSUCCESS;
  /***********************************************
    If there is a treelist (canditates) then if
    this tree is not in it then just return
    success notinlist.
  ************************************************/
  if (subtree_list)
  {
    char *found;
    char *tmp_list = malloc(strlen(subtree_list) + 3);
    char *tmp_tree = malloc(strlen(tree) + 3);
    strcpy(tmp_list, ",");
    strcat(tmp_list, subtree_list);
    strcat(tmp_list, ",");
    strcpy(tmp_tree, ",");
    strcat(tmp_tree, tree);
    strcat(tmp_tree, ",");
    found = strstr(tmp_list, tmp_tree);
    free(tmp_list);
    free(tmp_tree);
    if (!found)
      return TreeNOT_IN_LIST;
  }

  int status = TreeSUCCESS;
  int ext_status;
  int i;
  TREE_INFO *info;
  TREE_INFO *iptr;

  /***********************************************
    Get virtual memory for the tree
    information structure and zero the structure.
  ***********************************************/

  for (info = dblist->tree_info; info && strcmp(tree, info->treenam);
       info = info->next_info)
    ;
  if (!info)
  {
    info = calloc(1, sizeof(TREE_INFO));
    if (info)
    {

      /***********************************************
      Next we map the file and if successful copy
      the tree name (blank filled) into the info block.
      ***********************************************/
      info->has_lock = !TreeUsingPrivateCtx();
      if (info->has_lock)
        pthread_rwlock_init(&info->lock, NULL);
      info->flush = (dblist->shotid == -1);
      info->treenam = strdup(tree);
      info->shot = dblist->shotid;
      status = MapTree(info, dblist->tree_info, 0);
      if (STATUS_NOT_OK && (status == TreeFILE_NOT_FOUND ||
                            treeshr_errno == TreeFILE_NOT_FOUND))
      {
        TreeCallHookFun("TreeHook", "RetrieveTree", info->treenam, info->shot,
                        NULL);
        status = TreeCallHook(RetrieveTree, info, 0);
        if (STATUS_OK)
          status = MapTree(info, dblist->tree_info, 0);
      }
      if (status == TreeSUCCESS)
      {
        TreeCallHookFun("TreeHook", "OpenTree", tree, info->shot, NULL);
        TreeCallHook(OpenTree, info, 0);

        /**********************************************
         If this is the main tree the root node is the
         first node in the tree. If it is a subtree we
         must make the parent/subtree node connections
         and the referencing node is replaced by
         the root node of the subtree.
        **********************************************/

        info->root = info->node;
        if (parent == 0 || !dblist->tree_info)
        {
          dblist->tree_info = info;
          dblist->remote = 0;
        }
        else
        {
          for (iptr = dblist->tree_info; iptr->next_info;
               iptr = iptr->next_info)
            ;
          iptr->next_info = info;
          SubtreeNodeConnect(dblist, parent, info->node);
        }

        /***********************************************
         For each of the external references (subtrees),
         We make a string descriptor for the subtree
         name using the node name of the referencing node
         and then recursively call this routine to connect
         the subtree(s).
        *************************************************/
      }
      if (STATUS_NOT_OK && info)
      {
        free(info->treenam);
        free(info);
        info = 0;
      }
    }
  }
  if (info)
  {
    for (i = 0; i < info->header->externals; i++)
    {
      NODE *external_node = info->node + swapint32(&info->external[i]);
      char *subtree = strncpy(calloc(1, sizeof(NODE_NAME) + 1),
                              external_node->name, sizeof(NODE_NAME));
      subtree[sizeof(NODE_NAME)] = '\0';
      char *blank = strchr(subtree, ' ');
      if (blank)
        *blank = '\0';
      ext_status = ConnectTree(dblist, subtree, external_node, subtree_list);
      free(subtree);
      if (IS_NOT_OK(ext_status))
      {
        status = TreeNOTALLSUBS;
        if (treeshr_errno == TreeCANCEL)
          break;
      }
    }
  }
  return status;
}

#define move_to_top(prev_db, db) \
  if (prev_db)                   \
  {                              \
    prev_db->next = db->next;    \
    db->next = *dblist;          \
    *dblist = db;                \
  }

EXPORT int _TreeNewDbid(void **dblist)
{
  PINO_DATABASE *db = (PINO_DATABASE *)calloc(1, sizeof(PINO_DATABASE));
  if (db)
  {
    MDSDBG("Created DB %" PRIxPTR "\n", (uintptr_t)db);
    db->timecontext.start.dtype = DTYPE_DSC;
    db->timecontext.start.class = CLASS_XD;
    db->timecontext.end.dtype = DTYPE_DSC;
    db->timecontext.end.class = CLASS_XD;
    db->timecontext.delta.dtype = DTYPE_DSC;
    db->timecontext.delta.class = CLASS_XD;
    db->stack_size = DEFAULT_STACK_LIMIT;
    db->next = *(PINO_DATABASE **)dblist;
    *dblist = (void *)db;
    return TreeSUCCESS;
  }
  return TreeFAILURE;
}

static int CreateDbSlot(PINO_DATABASE **dblist, char *tree, int shot,
                        int editting)
{
  int status;
  PINO_DATABASE *db;
  PINO_DATABASE *prev_db;
  PINO_DATABASE *saved_prev_db = NULL;
  PINO_DATABASE *useable_db = 0;
  int count;
  int stack_size = DEFAULT_STACK_LIMIT;
  enum options
  {
    MOVE_TO_TOP,
    CLOSE,
    ERROR_DIRTY,
    OPEN_NEW
  };
  enum options option = OPEN_NEW;

  if (*dblist)
    stack_size = (*dblist)->stack_size;
  for (prev_db = 0, db = *dblist; db ? db->open : 0;
       prev_db = db, db = db->next)
  {
    if (db->shotid == shot)
    {
      if (strcmp(db->main_treenam, tree) == 0)
      {
        if (editting)
        {
          if (db->open_for_edit)
          {
            option = MOVE_TO_TOP;
            break;
          }
          else
          {
            option = CLOSE;
            break;
          }
        }
        else
        {
          if (db->open_for_edit)
          {
            if (db->modified)
            {
              option = ERROR_DIRTY;
              break;
            }
            else
            {
              option = CLOSE;
              break;
            }
          }
          else
          {
            option = MOVE_TO_TOP;
            break;
          }
        }
      }
    }
  }
  switch (option)
  {
  case MOVE_TO_TOP:
    move_to_top(prev_db, db);
    status = TreeALREADY_OPEN;
    break;

  case CLOSE:
    move_to_top(prev_db, db);
    close_top_tree(*dblist, 1);
    status = TreeSUCCESS;
    break;
  case ERROR_DIRTY:
    status = TreeWRITEFIRST;
    treeshr_errno = TreeWRITEFIRST;
    break;
  default:
    for (count = 0, prev_db = 0, db = *dblist; db;
         count++, prev_db = db, db = db->next)
    {
      if (!db->open)
      {
        move_to_top(prev_db, db);
        status = TreeSUCCESS;
        break;
      }
      else if (!(db->open_for_edit && db->modified))
      {
        saved_prev_db = prev_db, useable_db = db;
      }
    }
    if (!db)
    {
      if (count >= stack_size)
      {
        if (useable_db)
        {
          move_to_top(saved_prev_db, useable_db);
          close_top_tree(*dblist, 1);
          status = TreeSUCCESS;
        }
        else
        {
          status = TreeMAXOPENEDIT;
          treeshr_errno = TreeMAXOPENEDIT;
        }
      }
      else
        status = _TreeNewDbid((void **)dblist);
    }
  }
  if (status == TreeSUCCESS)
  {
    (*dblist)->shotid = shot;
    (*dblist)->setup_info = (shot == -1);
    free((*dblist)->experiment);
    (*dblist)->experiment = strdup(tree);
    free((*dblist)->main_treenam);
    (*dblist)->main_treenam = strdup(tree);
    (*dblist)->stack_size = stack_size;
  }
  return status;
}

int _TreeGetStackSize(void *dbid)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  return dblist->stack_size;
}

int _TreeSetStackSize(void **dbid, int size)
{
  PINO_DATABASE *dblist = *(PINO_DATABASE **)dbid;
  int new_size = size > 0 ? size : 1;
  int old_size = dblist->stack_size;
  for (; dblist; dblist = dblist->next)
  {
    dblist->stack_size = new_size;
    if (dblist && dblist->remote)
      SetStackSizeRemote(dblist, new_size);
  }
  return old_size;
}

static char *GetFname(char *tree, int shot)
/*********************************************
 allows to define a customized folder mapping.
 DTYPE_T <expt>_tree_filename(LONG)
 *********************************************/
{
  int status = 1;
  static char *ans = 0;
  struct descriptor_d fname = {0, DTYPE_T, CLASS_D, 0};
  char expression[128];
  struct descriptor expression_d = {0, DTYPE_T, CLASS_S, 0};
  if (ans)
  {
    free(ans);
    ans = 0;
  }
  expression_d.length =
      (unsigned short)sprintf(expression, "%s_tree_filename(%d)", tree, shot);
  expression_d.pointer = expression;
  static int (*TdiExecute)() =
      NULL; // LibFindImageSymbol_C is a NOP if TdiExecute is already set
  status = LibFindImageSymbol_C("TdiShr", "TdiExecute", &TdiExecute);
  if (STATUS_OK)
    status = (*TdiExecute)(&expression_d, &fname MDS_END_ARG);
  if (STATUS_OK)
  {
    ans = strncpy(malloc((size_t)fname.length + 2), fname.pointer,
                  fname.length);
    ans[fname.length] = '+';
    ans[fname.length + 1] = 0;
  }
  else
  {
    ans = strdup("ErRoR");
  }
  StrFree1Dx(&fname);
  return (ans);
}

static void str_shift(char *in, int num)
{
  char *p1 = in, *p2 = in + num;
  while (*p2 != 0)
    *p1++ = *p2++;
  *p1 = 0;
}

EXPORT char *MaskReplace(char *path_in, char *tree, int shot)
{
  char *path = strdup(path_in);
  char ShotMask[13];
  char *tilde;
  char *fname;
  int replace_tilde = 0;
  unsigned int i;
  if (shot > 0)
    sprintf(ShotMask, "%012u", shot);
  else
    memset(ShotMask, 'X', 12);
  for (tilde = strchr(path, '~'); tilde != 0; tilde = strchr(path, '~'))
  {
    char *tmp, *tmp2;
    switch (tilde[1])
    {
    case 'a':
      tilde[0] = ShotMask[11];
      str_shift(&tilde[1], 1);
      break;
    case 'b':
      tilde[0] = ShotMask[10];
      str_shift(&tilde[1], 1);
      break;
    case 'c':
      tilde[0] = ShotMask[9];
      str_shift(&tilde[1], 1);
      break;
    case 'd':
      tilde[0] = ShotMask[8];
      str_shift(&tilde[1], 1);
      break;
    case 'e':
      tilde[0] = ShotMask[7];
      str_shift(&tilde[1], 1);
      break;
    case 'f':
      tilde[0] = ShotMask[6];
      str_shift(&tilde[1], 1);
      break;
    case 'g':
      tilde[0] = ShotMask[5];
      str_shift(&tilde[1], 1);
      break;
    case 'h':
      tilde[0] = ShotMask[4];
      str_shift(&tilde[1], 1);
      break;
    case 'i':
      tilde[0] = ShotMask[3];
      str_shift(&tilde[1], 1);
      break;
    case 'j':
      tilde[0] = ShotMask[2];
      str_shift(&tilde[1], 1);
      break;
    case 'n':
      fname = GetFname(tree, shot);
      const size_t fname_len = strlen(fname);
      if ((strlen(tilde + 2) > 1) ||
          ((strlen(tilde + 2) == 1) && (tilde[2] != '\\')))
      {
        tmp = strdup(tilde + 2);
        tmp2 = strcpy(malloc(strlen(path) + 1 + fname_len), path);
        strcpy(tmp2 + (tilde - path) + fname_len, tmp);
        free(tmp);
        memcpy(tmp2 + (tilde - path), fname, fname_len);
      }
      else
      {
        tmp2 = strcpy(malloc(strlen(path) + 1 + fname_len), path);
        memcpy(tmp2 + (tilde - path), fname, fname_len + 1);
      }
      free(path);
      path = tmp2;
      break;
    case 't':
      tmp = strdup(tilde + 2);
      const size_t tree_len = strlen(tree);
      tmp2 = strcpy(malloc(strlen(path) + 1 + tree_len), path);
      strcpy(tmp2 + (tilde - path) + tree_len, tmp);
      free(tmp);
      memcpy(tmp2 + (tilde - path), tree, tree_len);
      free(path);
      path = tmp2;
      break;
    default:
      tilde[0] = 1;
      replace_tilde = (char)1;
      break;
    }
  }
  if (replace_tilde)
  {
    for (i = 0; i < strlen(path); i++)
      if (path[i] == (char)1)
        path[i] = '~';
  }
  return path;
}

#ifdef HAVE_SYS_RESOURCE_H
static void init_rlimit_once()
{
  struct rlimit rlp;
  getrlimit(RLIMIT_NOFILE, &rlp);
  if (rlp.rlim_cur < rlp.rlim_max)
  {
    rlp.rlim_cur = rlp.rlim_max;
    setrlimit(RLIMIT_NOFILE, &rlp);
  }
}
#endif
int OpenOne(TREE_INFO *info, TREE_INFO *root, tree_type_t type, int new,
            int edit_flag, char **filespec, int *fd_out)
{
/*
 * search for tree files unless filespec is preset with thick client def, i.e.
 * ends with "::"
 */
#ifdef HAVE_SYS_RESOURCE_H
  RUN_FUNCTION_ONCE(init_rlimit_once);
#endif
  int status, fd;
  INIT_AND_FREE_ON_EXIT(char *, treepath);
  fd = -1;
  if (root && root->filespec && root->speclen > 2 &&
      root->filespec[root->speclen - 1] == ':' &&
      root->filespec[root->speclen - 2] == ':')
  {
    treepath = strndup(root->filespec, root->speclen);
  }
  status = MDS_IO_OPEN_ONE(treepath, info->treenam, info->shot, type, new,
                           edit_flag, filespec, &info->speclen, &fd);
  FREE_NOW(treepath);
  if (STATUS_OK)
  {
    if (new &&fd == -1)
      status = TreeFCREATE;
    else
    {
#ifndef _WIN32
      info->mapped = (MDS_IO_ID(fd) == -1);
#ifdef __APPLE__
      /* from python-mmap Issue #11277: fsync(2) is not enough on OS X - a
           special, OS X specific fcntl(2) is necessary to force DISKSYNC and
           get around mmap(2) bug */
      if (info->mapped && fd != -1)
        (void)fcntl(fd, F_FULLFSYNC);
#endif
#endif
      if (fd == -1)
        status = edit_flag ? TreeFOPENW : TreeFOPENR;
    }
  }
  *fd_out = fd;
  if (fd >= 0 && type == TREE_TREEFILE_TYPE)
    info->channel = fd;
  return status;
}

static int MapTree(TREE_INFO *info, TREE_INFO *root, int edit_flag)
{
  /******************************************
  First we need to open the tree file.
  If successful, we create and map a global
  section on the tree file.
  *******************************************/
  int status, fd, nomap = 0;
  status = OpenOne(info, root, TREE_TREEFILE_TYPE, 0, edit_flag,
                   &info->filespec, &fd);
  if (STATUS_OK)
  {
    info->alq = (int)(MDS_IO_LSEEK(fd, 0, SEEK_END) / 512);
    if (info->alq < 1)
    {
      if (info->filespec)
        fprintf(stderr, "Corrupted/truncated tree file: %s\n",
                info->filespec);
      else
        fprintf(stderr,
                "Corrupted/truncated tree file: <filespec undefined>\n");
      MDS_IO_CLOSE(fd);
      status = TreeFILE_NOT_FOUND;
    }
    else
    {
      MDS_IO_LSEEK(fd, 0, SEEK_SET);
      status = TreeSUCCESS;
      nomap = !info->mapped;
    }
  }
  if (status == TreeSUCCESS)
    status = MapFile(fd, info, nomap);
  return status;
}

#ifdef WORDS_BIGENDIAN
static void SwapBytesInt(char *in)
{
  char tmp = in[0];
  in[0] = in[3];
  in[3] = tmp;
  tmp = in[1];
  in[1] = in[2];
  in[2] = tmp;
}

void FixupHeader(TREE_HEADER *hdr)
{
  char flags = ((char *)hdr)[1];
  hdr->sort_children = (flags & 1) != 0;
  hdr->sort_members = (flags & 2) != 0;
  hdr->versions_in_model = (flags & 4) != 0;
  hdr->versions_in_pulse = (flags & 8) != 0;
  SwapBytesInt((char *)&hdr->free);
  SwapBytesInt((char *)&hdr->tags);
  SwapBytesInt((char *)&hdr->externals);
  SwapBytesInt((char *)&hdr->nodes);
}
#else
#define FixupHeader(arg)
#endif

static int MapFile(int fd, TREE_INFO *info, int nomap)
{
  int status;

  /********************************************
  First we get virtual memory for the tree to
  be mapped into.
  ********************************************/

  status = GetVmForTree(info, nomap);
  if (status == TreeSUCCESS)
  {
    if (nomap)
    {
      status = (MDS_IO_READ(fd, (void *)info->section_addr[0],
                            (size_t)info->alq * 512) == (512 * info->alq))
                   ? TreeSUCCESS
                   : TreeTREEFILEREADERR;
    }
#ifndef _WIN32
    else
    {
#ifndef MAP_FILE
#define MAP_FILE 0
#endif
      info->section_addr[0] =
          mmap(0, (size_t)info->alq * 512, PROT_READ | PROT_WRITE,
               MAP_FILE | MAP_PRIVATE, MDS_IO_FD(info->channel), 0);
      status = info->section_addr[0] != (void *)-1 ? TreeSUCCESS
                                                   : TreeTREEFILEREADERR;
      if (STATUS_NOT_OK)
      {
        perror("Error mapping file");
        info->mapped = 0;
      }
    }
#endif
    /***********************************************
    If we successfully mapped the file, see if
    we got all of the file and the return addresses
    match what we asked for.
    If ok, fill in the addresses of the various
    parts of the tree based on the information in
    the tree header.
    ************************************************/

    if (STATUS_OK)
    {
      info->blockid = TreeBLOCKID;
      info->header = (TREE_HEADER *)info->section_addr[0];
      FixupHeader(info->header);
      info->node = (NODE *)(info->section_addr[0] +
                            (((int)sizeof(TREE_HEADER) + 511) / 512) * 512);
      info->tags =
          (int *)(((char *)info->node) +
                  ((info->header->nodes * (int)sizeof(NODE) + 511) / 512) *
                      512);
      info->tag_info =
          (TAG_INFO *)(((char *)info->tags) +
                       ((info->header->tags * 4 + 511) / 512) * 512);
      info->external =
          (int *)(((char *)info->tag_info) +
                  ((info->header->tags * (int)sizeof(TAG_INFO) + 511) / 512) *
                      512);
      TreeEstablishRundownEvent(info);
      status = TreeSUCCESS;
    }
    else
      free(info->vm_addr);
  }
  return status;
}

static int GetVmForTree(TREE_INFO *info, int nomap)
{
  int status = TreeSUCCESS;
  info->vm_pages = info->alq;
  if (nomap)
  {
    info->vm_addr = calloc(512, (size_t)info->vm_pages);
    if (info->vm_addr)
      info->section_addr[0] = info->vm_addr;
    else
      status = TreeMEMERR;
  }
  else
    info->vm_addr = 0;
  return status;
}

static void SubtreeNodeConnect(PINO_DATABASE *dblist, NODE *parent,
                               NODE *subtreetop)
{
  NID child_nid, parent_nid, brother_nid = {0, 0};
  NODE *brother = brother_of(dblist, parent);
  parent->usage = TreeUSAGE_SUBTREE_REF;
  subtreetop->usage = TreeUSAGE_SUBTREE_TOP;
  node_to_nid(dblist, subtreetop, &child_nid);
  node_to_nid(dblist, parent_of(dblist, parent), &parent_nid);
  if (brother)
    node_to_nid(dblist, brother_of(dblist, parent), &brother_nid);
  parent->child = *(int *)&child_nid;
  subtreetop->parent = *(int *)&parent_nid;
  subtreetop->brother = *(int *)&brother_nid;
  memcpy(subtreetop->name, parent->name, sizeof(subtreetop->name));
  return;
}

struct context
{
  char *expt;
  int shot;
  int edit;
  int readonly;
  int defnid;
};

void *_TreeSaveContext(void *dbid)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  struct context *ctx = NULL;
  if (IS_OPEN(dblist))
  {
    ctx = malloc(sizeof(struct context));
    ctx->expt =
        strdup(dblist->experiment);
    ctx->shot = dblist->shotid;
    ctx->edit = dblist->open_for_edit;
    ctx->readonly = dblist->open_readonly;
    _TreeGetDefaultNid(dbid, &ctx->defnid);
  }
  return (void *)ctx;
}

void _TreeRestoreContext(void **dbid, void *ctxin)
{
  if (ctxin)
  {
    struct context *ctx = (struct context *)ctxin;
    if (ctx->edit)
      _TreeOpenEdit(dbid, ctx->expt, ctx->shot);
    else
      _TreeOpen(dbid, ctx->expt, ctx->shot, ctx->readonly);
    _TreeSetDefaultNid(*dbid, ctx->defnid);
    free(ctx->expt);
    free(ctx);
  }
}

int TreeReopenDatafile(struct tree_info *info)
{
  int status = 1, reopen_get, reopen_put, stv;
  WRLOCKINFO(info);
  if (info->data_file)
  {
    reopen_get = info->data_file->get > 0;
    reopen_put = info->data_file->put > 0;
    if (reopen_get)
      MDS_IO_CLOSE(info->data_file->get);
    if (reopen_put)
      MDS_IO_CLOSE(info->data_file->put);
    free(info->data_file);
    info->data_file = NULL;
    if (reopen_get)
      status = _TreeOpenDatafileR(info);
    if (STATUS_OK && reopen_put)
      status = _TreeOpenDatafileW(info, &stv, 0);
  }
  UNLOCKINFO(info);
  return status;
}

int TreeReopenNci(struct tree_info *info)
{
  int status = 1;
  WRLOCKINFO(info);
  if (info->nci_file)
  {
    int reopen_get = info->nci_file->get;
    int reopen_put = info->nci_file->put;
    free(info->nci_file);
    info->nci_file = 0;
    if (reopen_get)
    {
      status = _TreeOpenNciR(info);
      MDS_IO_CLOSE(reopen_get);
    }
    if (reopen_put)
    {
      if (STATUS_OK)
        status = _TreeOpenNciW(info, 0);
      MDS_IO_CLOSE(reopen_put);
    }
  }
  UNLOCKINFO(info);
  return status;
}

int _TreeOpenEdit(void **dbid, char const *tree_in, int shot_in)
{
  TREE_INFO *info;
  size_t const treesize = strlen(tree_in) + 1;
  char *tree = (char *)alloca(treesize);
  int shot;
  int status;

  RemoveBlanksAndUpcase(tree, tree_in);
  shot = shot_in ? shot_in : TreeGetCurrentShotId(tree);
  if (shot < -1)
    status = TreeINVSHOT;
  else if (shot == 0)
    status = TreeNOCURRENT;
  else
  {
    PINO_DATABASE **dblist = (PINO_DATABASE **)dbid;
    status = CreateDbSlot(dblist, tree, shot, 1);
    if (status == TreeSUCCESS)
    {
      info = (TREE_INFO *)calloc(1, sizeof(TREE_INFO));
      if (info)
      {
        info->flush = ((*dblist)->shotid == -1);
        info->treenam = strdup(tree);
        info->shot = (*dblist)->shotid;
        status = MapTree(info, (*dblist)->tree_info, 1);
        if (STATUS_NOT_OK && (status == TreeFILE_NOT_FOUND ||
                              treeshr_errno == TreeFILE_NOT_FOUND))
        {
          TreeCallHookFun("TreeHook", "RetrieveTree", tree, info->shot, NULL);
          status = MapTree(info, (*dblist)->tree_info, 1);
          if (STATUS_NOT_OK && (status == TreeFILE_NOT_FOUND ||
                                treeshr_errno == TreeFILE_NOT_FOUND))
          {
            status = TreeCallHook(RetrieveTree, info, 0);
            if (STATUS_OK)
              status = MapTree(info, (*dblist)->tree_info, 1);
          }
        }
        if (STATUS_OK)
        {
          TreeCallHookFun("TreeHook", "OpenTreeEdit", tree, info->shot, NULL);
          TreeCallHook(OpenTreeEdit, info, 0);
          info->edit = (TREE_EDIT *)calloc(1, sizeof(TREE_EDIT));
          if (info->edit)
          {
            info->root = info->node;
            status = TreeOpenNciW(info, 0);
            if (STATUS_OK)
            {
              (*dblist)->tree_info = info;
              (*dblist)->open = 1;
              (*dblist)->open_for_edit = 1;
              (*dblist)->open_readonly = 0;
              (*dblist)->default_node = info->root;
            }
            else
              free(info->edit);
          }
        }
        if (STATUS_NOT_OK)
        {
          free(info->treenam);
          free(info);
        }
      }
      else
        status = TreeMEMERR;
      if (STATUS_NOT_OK)
        free_top_db(dblist);
    }
  }
  TreeCleanupConnections();
  return status;
}

int _TreeOpenNew(void **dbid, char const *tree_in, int shot_in)
{
  int fd_keepalive = -1; // in case tree is remote
  TREE_INFO *info;
  char *tree = strdup(tree_in);
  int shot;
  int status;

  RemoveBlanksAndUpcase(tree, tree_in);
  shot = shot_in ? shot_in : TreeGetCurrentShotId(tree);
  if (shot < -1)
    status = TreeINVSHOT;
  else if (shot == 0)
    status = TreeNOCURRENT;
  else
  {
    PINO_DATABASE **dblist = (PINO_DATABASE **)dbid;
    status = CreateDbSlot(dblist, tree, shot, 1);
    if (status == TreeSUCCESS)
    {
      info = (TREE_INFO *)calloc(1, sizeof(TREE_INFO));
      if (info)
      {
        int fd;
        info->flush = ((*dblist)->shotid == -1);
        info->treenam = strdup(tree);
        info->shot = (*dblist)->shotid;
        status = OpenOne(info, (*dblist)->tree_info, TREE_TREEFILE_TYPE, 1, 0,
                         &info->filespec, &fd);
        if (fd > -1)
        {
          fd_keepalive = fd;
          info->channel = -2;
          status = OpenOne(info, (*dblist)->tree_info, TREE_NCIFILE_TYPE, 1, 0,
                           NULL, &fd);

          if (fd > -1)
          {
            MDS_IO_CLOSE(fd_keepalive);
            fd_keepalive = fd;
            status = OpenOne(info, (*dblist)->tree_info, TREE_DATAFILE_TYPE, 1,
                             0, NULL, &fd);
            if (fd > -1)
            {
              MDS_IO_CLOSE(fd_keepalive);
              fd_keepalive = fd;
            }
          }
        }
        if (STATUS_OK)
        {
          TreeCallHookFun("TreeHook", "OpenTreeEdit", info->treenam,
                          info->shot, NULL);
          TreeCallHook(OpenTreeEdit, info, 0);
          info->edit = (TREE_EDIT *)calloc(1, sizeof(TREE_EDIT));
          if (info->edit)
          {
            info->blockid = TreeBLOCKID;
            info->header = calloc(1, 512);
            info->header->free = -1;
            info->edit->header_pages = 1;
            (*dblist)->tree_info = info;
            (*dblist)->open = 1;
            (*dblist)->open_for_edit = 1;
            (*dblist)->open_readonly = 0;
            info->root = info->node;
            (*dblist)->default_node = info->root;
            TreeOpenNciW(info, 0);
            info->edit->first_in_mem = 0;
            status = TreeExpandNodes(*dblist, 0, 0);
            // strncpy(info->node->name, "TOP         ",
            // sizeof(info->node->name));
            strcpy(info->node->name, "TOP");
            memset(info->node->name + 3, ' ', sizeof(info->node->name) - 3);
            info->node->parent = 0;
            info->node->child = 0;
            info->node->member = 0;
            info->node->usage = TreeUSAGE_SUBTREE;
            (info->node + 1)->child = 0;
            bitassign(1, info->edit->nci->flags, NciM_INCLUDE_IN_PULSE);
            info->tags = calloc(1, 512);
            info->edit->tags_pages = 1;
            info->tag_info = calloc(sizeof(TAG_INFO), 512);
            info->edit->tag_info_pages = sizeof(TAG_INFO);
            info->external = calloc(1, 512);
            info->edit->external_pages = 1;
            info->edit->first_in_mem = 0;
            info->header->version = 1;
            info->header->sort_children = 1;
            info->header->sort_members = 1;
            info->header->free = sizeof(NODE);
          }
        }
        else
        {
          free(info->treenam);
          free(info);
        }
        free(tree);
      }
      else
        status = TreeMEMERR;
      if (STATUS_NOT_OK)
        free_top_db(dblist);
    }
  }
  if (STATUS_OK)
  {
    _TreeWriteTree(dbid, 0, 0);
    MDS_IO_CLOSE(fd_keepalive);
  }
  TreeCleanupConnections();
  return status;
}

void TreeFreeDbid(void *dbid)
{
  if (dbid)
  {
    MDSDBG("Destroyed DB %" PRIxPTR "\n", (uintptr_t)dbid);
    PINO_DATABASE *db = (PINO_DATABASE *)dbid;
    TreeFreeDbid(db->next);
    close_top_tree(db, 1);
    free_xd(&db->timecontext.start);
    free_xd(&db->timecontext.end);
    free_xd(&db->timecontext.delta);
    free(db->delete_list);
    free(db);
  }
}

EXPORT int _TreeFileName(void *dbid, char *tree, int shot,
                         struct descriptor_xd *out_ptr)
{
  int status;
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  if (tree)
  {
    int fd;
    TREE_INFO info;
    info.treenam = tree;
    info.shot = shot;
    status = OpenOne(&info, dblist ? dblist->tree_info : NULL,
                     TREE_TREEFILE_TYPE, 0, 0, &info.filespec, &fd);
    if (STATUS_OK)
    {
      MDS_IO_CLOSE(fd);
      if (info.filespec)
      {
        struct descriptor dsc = {(unsigned short)strlen(info.filespec),
                                 DTYPE_T, CLASS_S, info.filespec};
        status = MdsCopyDxXd(&dsc, out_ptr);
        free(info.filespec);
      }
    }
  }
  else
  {
    if (IS_OPEN(dblist))
    {
      TREE_INFO *info = dblist->tree_info;
      struct descriptor dsc = {(uint16_t)strlen(info->filespec), DTYPE_T,
                               CLASS_S, info->filespec};
      status = MdsCopyDxXd(&dsc, out_ptr);
    }
    else
      status = TreeNOT_OPEN;
  }
  return status;
}

EXPORT int TreeFileName(char *tree, int shot, struct descriptor_xd *out_ptr)
{
  return _TreeFileName(*TreeCtx(), tree, shot, out_ptr);
}
