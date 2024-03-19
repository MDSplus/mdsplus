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
#include "treeshrp.h" /* must be first or off_t wrong */
#include <fcntl.h>
#include <ncidef.h>
#include <string.h>
#include <treeshr.h>

extern void **TreeCtx();

extern int SetNciRemote();
extern int TreeFlushOffRemote();
extern int TreeFlushResetRemote();
extern int TreeTurnOnRemote();
extern int TreeTurnOffRemote();

int TreeSetNci(int nid, NCI_ITM *nci_itm_ptr)
{
  return _TreeSetNci(*TreeCtx(), nid, nci_itm_ptr);
}

int TreeFlushOff(int nid) { return _TreeFlushOff(*TreeCtx(), nid); }

int TreeFlushReset(int nid) { return _TreeFlushReset(*TreeCtx(), nid); }

int TreeTurnOn(int nid) { return _TreeTurnOn(*TreeCtx(), nid); }

int TreeTurnOff(int nid) { return _TreeTurnOff(*TreeCtx(), nid); }

int TreeSetUsage(int nid_in, unsigned char usage)
{
  return _TreeSetUsage(*TreeCtx(), nid_in, usage);
}

static int set_node_parent_state(PINO_DATABASE *db, NODE *node, NCI *nci,
                                 unsigned int state)
{
  TREE_INFO *info;
  int node_num;
  int status;
  for (info = db->tree_info;
       info &&
       ((node < info->node) || (node > (info->node + info->header->nodes)));
       info = info->next_info)
    ;
  if (!info)
    return TreeNNF;
  node_num = (int)(node - info->node);
  int locked = 0;
  status = tree_get_and_lock_nci(info, node_num, nci, &locked);
  if (STATUS_OK)
  {
    bitassign(state, nci->flags, NciM_PARENT_STATE);
    status = tree_put_nci(info, node_num, nci, &locked);
  }
  return status;
}

int tree_set_parent_state(PINO_DATABASE *db, NODE *node, unsigned int state)
{
  int status;
  NCI nci;
  NODE *lnode;
  status = TreeSUCCESS;
  for (lnode = node; lnode && STATUS_OK; lnode = brother_of(db, lnode))
  {
    status = set_node_parent_state(db, lnode, &nci, state);
    if (STATUS_OK && (!(nci.flags & NciM_STATE)) && (lnode->child))
      status = tree_set_parent_state(db, child_of(db, lnode), state);
    if (STATUS_OK && (!(nci.flags & NciM_STATE)) && (lnode->member))
      status = tree_set_parent_state(db, member_of(lnode), state);
  }
  return status;
}

int tree_lock_nci(TREE_INFO *info, int readonly, int nodenum, int *deleted,
                  int *locked)
{
  int status = TreeSUCCESS;
  int deleted_loc, *deleted_ptr = deleted ? deleted : &deleted_loc;
  if (!info->header->readonly)
  {
    if (*locked == 0)
    { // acquire lock
      status = MDS_IO_LOCK(
          readonly ? info->nci_file->get : info->nci_file->put, nodenum * 42,
          42, readonly ? MDS_IO_LOCK_RD : MDS_IO_LOCK_WRT, deleted_ptr);
      if (STATUS_OK)
      {
        if (!*deleted_ptr)
          *locked = 3; // lock acquired and increment
        else if (!deleted)
          *locked = 2; // lock not acquired but caller did not provide
                       // deleted; increment w/o lock
      }
    }
    else
    { // increment and simulate
      if (deleted)
        *deleted = FALSE;
      *locked += 2;
    }
  }
  return status;
}
void tree_unlock_nci(TREE_INFO *info, int readonly, int nodenum, int *locked)
{
  if (!info->header->readonly)
  {
#ifdef DEBUG
    if (*locked < 2)
      fprintf(stderr, "ERROR: tree_unlock_nci and *locked invalid: %d\n",
              *locked);
#endif
    *locked -= 2; // decrement
    if (*locked == 1)
    { // last lock and lock acquired -> release lock
      MDS_IO_LOCK(readonly ? info->nci_file->get : info->nci_file->put,
                  nodenum * 42, 42, MDS_IO_LOCK_NONE, 0);
      *locked = 0;
    }
  }
}

/*------------------------------------------------------------------------------

                Name: TreeSetNci

                Type:   C function

                Author:	Josh Stillerman
                        MIT Plasma Fusion Center
                Date:   19-MAY-1988

                Purpose: Set the Characteristics of a Node.

------------------------------------------------------------------------------

        Call sequence:

  int TreeSetNci(int nid, NCI_ITM *nci_itm_ptr)
  int TreeFlushOff( int nid)
  int TreeFlushReset( int nid)

        Description:

+-----------------------------------------------------------------------------*/

int _TreeSetNci(void *dbid, int nid_in, NCI_ITM *nci_itm_ptr)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID *nid_ptr = (NID *)&nid_in;
  int status;
  int node_number;
  TREE_INFO *tree_info;
  NCI_ITM *itm_ptr;
  NCI nci;
  int putnci = 0;

  if (!(IS_OPEN(dblist)))
    return TreeNOTOPEN;
  if (dblist->open_readonly)
    return TreeREADONLY;
  if (dblist->remote)
    return SetNciRemote(dbid, nid_in, nci_itm_ptr);
  nid_to_tree_nidx(dblist, nid_ptr, tree_info, node_number);
  if (!tree_info)
    return TreeNNF;
  status = TreeCallHook(PutNci, tree_info, nid_in);
  if (status && STATUS_NOT_OK)
    return status;
  int locked = FALSE;
  RETURN_IF_NOT_OK(
      tree_get_and_lock_nci(tree_info, node_number, &nci, &locked));
  for (itm_ptr = nci_itm_ptr; itm_ptr->code != NciEND_OF_LIST && STATUS_OK;
       itm_ptr++)
  {
    switch (itm_ptr->code)
    {
    case NciSET_FLAGS:
      nci.flags |= *(unsigned int *)itm_ptr->pointer;
      putnci = 1;
      break;
    case NciCLEAR_FLAGS:
      nci.flags &= ~(*(unsigned int *)itm_ptr->pointer);
      putnci = 1;
      break;
    case NciSTATUS:
      putnci = 1;
      nci.status = *(unsigned int *)itm_ptr->pointer;
      break;
    case NciUSAGE:
    {
      NODE *node_ptr;

      /**************************************************
       First we must check to make sure we are editting
       a valid tree.
      **************************************************/

      if (!(IS_OPEN_FOR_EDIT(dblist)))
        status = TreeNOEDIT;

      node_ptr = nid_to_node(dblist, nid_ptr);
      if (node_ptr->usage != *(unsigned char *)itm_ptr->pointer)
      {
        node_ptr->usage = *(unsigned char *)itm_ptr->pointer;
        dblist->modified = 1;
      }
      status = TreeSUCCESS;
      break;
    }
    case NciCOMPRESSION_METHOD:
      putnci = 1;
      nci.compression_method = *(unsigned char *)itm_ptr->pointer;
      break;
    default:
      status = TreeILLEGAL_ITEM;
      break;
    }
  }
  if (STATUS_OK && putnci)
    status = tree_put_nci(tree_info, node_number, &nci, &locked);
  else
    tree_unlock_nci(tree_info, 0, node_number, &locked);
  return status;
}

EXPORT int TreeSetNciItm(int nid, int code, int value)
{
  NCI_ITM itm[] = {{0, 0, 0, 0}, {0, 0, 0, 0}};
  itm[0].buffer_length = (short)sizeof(int);
  itm[0].code = (short)code;
  itm[0].pointer = (void *)&value;
  return TreeSetNci(nid, itm);
}

int _TreeFlushOff(void *dbid, int nid)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID *nid_ptr = (NID *)&nid;
  TREE_INFO *tree_info;
  if (!(IS_OPEN(dblist)))
    return TreeNOTOPEN;
  if (dblist->remote)
    return TreeFlushOffRemote(dbid, nid);
  nid_to_tree(dblist, nid_ptr, tree_info);
  if (!tree_info)
    return TreeNNF;
  tree_info->flush = 0;
  return TreeSUCCESS;
}

int _TreeFlushReset(void *dbid, int nid)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID *nid_ptr = (NID *)&nid;
  TREE_INFO *tree_info;
  if (!(IS_OPEN(dblist)))
    return TreeNOTOPEN;
  if (dblist->remote)
    return TreeFlushResetRemote(dbid, nid);
  nid_to_tree(dblist, nid_ptr, tree_info);
  if (!tree_info)
    return TreeNNF;
  if ((tree_info->flush == 0) && (dblist->shotid == -1))
  {
    tree_info->flush = 1;
    TreeWait(tree_info);
  }
  return TreeSUCCESS;
}

/*------------------------------------------------------------------------------

                Name: tree_get_and_lock_nci

                Type:   C function

                Author:	Josh Stillerman
                        MIT Plasma Fusion Center

                Date:    22-MAR-1988

                Purpose: Routine which returns an characteristics record for a
node locking the record so that it can be updated.

------------------------------------------------------------------------------

        Call sequence:
               STATUS = tree_get_and_lock_nci(info, node_num, nci, &locked)

        Description:

+-----------------------------------------------------------------------------*/

int tree_get_and_lock_nci(TREE_INFO *info, int node_num, NCI *nci,
                          int *locked)
{
  if (info->header->readonly)
    return TreeREADONLY_TREE;

  int status = TreeSUCCESS;

  /******************************************
    If the tree is not open for edit then
    if the characteristic file is not open
    (or not open for write)
    open the characteristic file for writting
    if OK so far then
    fill in the gab and read the record
  ******************************************/
  if ((info->edit == 0) || (node_num < info->edit->first_in_mem))
  {
    int deleted = TRUE;
    char nci_bytes[42];
    if ((info->nci_file == 0) || (info->nci_file->put == 0))
      RETURN_IF_NOT_OK(TreeOpenNciW(info, 0));
    while (STATUS_OK)
    {
      RETURN_IF_NOT_OK(tree_lock_nci(info, 0, node_num, &deleted, locked));
      if (!deleted)
        break;
      status = TreeReopenNci(info);
    }
    if (STATUS_OK)
    {
      MDS_IO_LSEEK(info->nci_file->put, node_num * sizeof(nci_bytes),
                   SEEK_SET);
      if (MDS_IO_READ(info->nci_file->put, nci_bytes, sizeof(nci_bytes)) ==
          sizeof(nci_bytes))
      {
        TreeSerializeNciIn(nci_bytes, nci);
        status = TreeSUCCESS;
      }
      else
        status = TreeNCIREAD;
    }
    if (STATUS_NOT_OK)
      tree_unlock_nci(info, 0, node_num, locked);
  }
  else
  {
    /********************************************
     Otherwise the tree is open for edit so
     the characteristics are just a memory reference
     away.
    *********************************************/
    *locked += 2; // simulate lock
    memcpy(nci, info->edit->nci + node_num - info->edit->first_in_mem,
           sizeof(struct nci));
  }

  return status;
}

/*------------------------------------------------------------------------------

                Name: TreeOpenNciW

                Type:   C function

                Author:	Josh Stillerman
                        MIT Plasma Fusion Center

                Date:   22-MAR-1988

                Purpose: Open a tree characteristics file for write access.

------------------------------------------------------------------------------

        Call sequence:
                       status = TreeOpenNciW(info_ptr)

        Description:

+-----------------------------------------------------------------------------*/
static char *tree_to_characteristic(
    const char *tree,
    const int tmpfile)
{ // replace .tree with .characteristics or
  // .characteristics# if tmpfile
  const size_t baselen = strlen(tree) - sizeof("tree") + 1;
  const size_t namelen = baselen + (tmpfile ? sizeof("characteristics#") - 1
                                            : sizeof("characteristics") - 1);
  char *const filename = memcpy(malloc(namelen + 1), tree, baselen);
  memcpy(filename + baselen, "characteristics#", namelen - baselen);
  filename[namelen] = '\0';
  return filename;
}
int TreeOpenNciW(TREE_INFO *info, int tmpfile)
{
  WRLOCKINFO(info);
  int status = _TreeOpenNciW(info, tmpfile);
  UNLOCKINFO(info);
  return status;
}
int _TreeOpenNciW(TREE_INFO *info, int tmpfile)
{
  int status;

  /****************************************************
    If there is no characterisitics file block allocated then
      Allocate one
  *****************************************************/

  if (!info->nci_file)
  {
    status =
        ((info->nci_file = (struct nci_file *)malloc(sizeof(NCI_FILE))) != NULL)
            ? TreeSUCCESS
            : TreeFAILURE;
    if (STATUS_OK)
    {
      char *filename = tree_to_characteristic(info->filespec, tmpfile);
      memset(info->nci_file, 0, sizeof(NCI_FILE));
      info->nci_file->get = MDS_IO_OPEN(
          filename, tmpfile ? O_RDWR | O_CREAT | O_TRUNC | O_EXCL : O_RDONLY,
          0664);
      status = (info->nci_file->get == -1) ? TreeFAILURE : TreeSUCCESS;
      if (info->nci_file->get == -1)
        info->nci_file->get = 0;
      if (STATUS_OK)
      {
        info->nci_file->put = MDS_IO_OPEN(filename, O_RDWR, 0);
        status = (info->nci_file->put == -1) ? TreeFAILURE : TreeSUCCESS;
        if (info->nci_file->put == -1)
          info->nci_file->put = 0;
      }
      free(filename);
    }
  }
  else
  {
    /*******************************************
      Else the file was open for read access so
      open it for write access.
    *******************************************/
    char *filename = tree_to_characteristic(info->filespec, tmpfile);
    /********* this will never happen ***********/
    if (info->nci_file->put)
      MDS_IO_CLOSE(info->nci_file->put);
    info->nci_file->put = MDS_IO_OPEN(
        filename, (tmpfile ? O_RDWR | O_CREAT | O_TRUNC : O_RDWR), 0664);
    status = (info->nci_file->put == -1) ? TreeFAILURE : TreeSUCCESS;
    if (info->nci_file->put == -1)
      info->nci_file->put = 0;
    free(filename);
  }
  if (STATUS_OK)
  {
    if (info->edit)
    {
      info->edit->first_in_mem =
          (int)MDS_IO_LSEEK(info->nci_file->put, 0, SEEK_END) / 42;
    }
  }
  if (STATUS_OK)
    TreeCallHook(OpenNCIFileWrite, info, 0);
  return status;
}

/*------------------------------------------------------------------------------

                Name: tree_put_nci

                Type:   C function

                Author:	Josh Stillerman
                        MIT Plasma Fusion Center

                Date:    22-MAR-1988

                Purpose: Routine which puts an attribute record into a
                        tree.

------------------------------------------------------------------------------

        Call sequence:
               STATUS = tree_put_nci(info_ptr, node_num, nci_ptr, &locked)

        Description:
                Ensures unlock if is on File

+-----------------------------------------------------------------------------*/

int tree_put_nci(TREE_INFO *info, int node_num, NCI *nci, int *locked)
{
  int status = TreeSUCCESS;
  /***************************************
    If the tree is not open for edit
  ****************************************/

  if ((info->edit == 0) || (node_num < info->edit->first_in_mem))
  {

    /**********************
     Update the NCI record
    ***********************/
    if (!*locked)
      RETURN_IF_NOT_OK(tree_lock_nci(info, 0, node_num, NULL, locked));
    char nci_bytes[42] = {0};
    TreeSerializeNciOut(nci, nci_bytes);
    MDS_IO_LSEEK(info->nci_file->put, sizeof(nci_bytes) * node_num, SEEK_SET);
    status = (MDS_IO_WRITE(info->nci_file->put, nci_bytes, sizeof(nci_bytes)) ==
              sizeof(nci_bytes))
                 ? TreeSUCCESS
                 : TreeFAILURE;
    tree_unlock_nci(info, 0, node_num, locked);
  }

  /****************************
    Else it is open for edit so
    the characteristics are just a
    memory reference away.
  *****************************/

  else
  {
    memcpy(info->edit->nci + (node_num - info->edit->first_in_mem), nci,
           sizeof(*nci));
    if (!*locked)
      *locked -= 2; // simulate unlock
  }
  return status;
}

/*------------------------------------------------------------------------------

                Name: TreeTurnOn

                Type:   C function

                Author:	Josh Stillerman
                        MIT Plasma Fusion Center

                Date:   28-MAR-1988

                Purpose: Turn on a sub tree.

------------------------------------------------------------------------------

        Call sequence:
                        int TreeTurnOn()
                        status = TreeTurnOn(nid);

        Description:

+-----------------------------------------------------------------------------*/

int _TreeTurnOn(void *dbid, int nid_in)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID *nid = (NID *)&nid_in;
  int status;
  int node_num;
  TREE_INFO *info;
  NCI nci;
  NODE *node;
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  if (dblist->remote)
    return TreeTurnOnRemote(dbid, nid_in);
  nid_to_tree_nidx(dblist, nid, info, node_num);
  if (!info)
    return TreeNNF;
  int locked = 0;
  RETURN_IF_NOT_OK(tree_get_and_lock_nci(info, node_num, &nci, &locked));
  if (nci.flags & NciM_STATE)
  {
    bitassign(0, nci.flags, NciM_STATE);
    RETURN_IF_NOT_OK(tree_put_nci(info, node_num, &nci, &locked));
    if (!(nci.flags & NciM_PARENT_STATE))
    {
      node = nid_to_node(dblist, nid);
      if (node->child)
        status = tree_set_parent_state(dblist, child_of(dblist, node), 0);
      if (node->member)
        status = tree_set_parent_state(dblist, member_of(node), 0);
    }
    else
      status = TreePARENT_OFF;
  }
  else
  {
    tree_unlock_nci(info, 0, node_num, &locked);
    status = TreeALREADY_ON;
  }
  return status;
}

/*------------------------------------------------------------------------------

                Name: TreeTurnOff

                Type:   C function

                Author:	Josh Stillerman
                        MIT Plasma Fusion Center

                Date:   28-MAR-1988

                Purpose: Turn off a sub tree.

------------------------------------------------------------------------------

        Call sequence:
                        int TreeTurnOff()
                        status = TreeTurnOff(nid);

        Description:

+-----------------------------------------------------------------------------*/

int _TreeTurnOff(void *dbid, int nid_in)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID *nid = (NID *)&nid_in;
  int status;
  int node_num;
  TREE_INFO *info;
  NCI nci;
  NODE *node;
  if (!IS_OPEN(dblist))
    return TreeNOT_OPEN;
  if (dblist->remote)
    return TreeTurnOffRemote(dbid, nid_in);
  nid_to_tree_nidx(dblist, nid, info, node_num);
  if (!info)
    return TreeNNF;
  int locked = 0;
  RETURN_IF_NOT_OK(tree_get_and_lock_nci(info, node_num, &nci, &locked));
  if (!(nci.flags & NciM_STATE))
  {
    bitassign(1, nci.flags, NciM_STATE);
    RETURN_IF_NOT_OK(tree_put_nci(info, node_num, &nci, &locked));
    if (!(nci.flags & NciM_PARENT_STATE))
    {
      node = nid_to_node(dblist, nid);
      if (node->child)
        status = tree_set_parent_state(dblist, child_of(dblist, node), 1);
      if (node->member)
        status = tree_set_parent_state(dblist, member_of(node), 1);
    }
  }
  else
  {
    tree_unlock_nci(info, 0, node_num, &locked);
    status = TreeALREADY_OFF;
  }
  return status;
}

int _TreeSetUsage(void *dbid, int nid_in, unsigned char usage)
{
  NCI_ITM itm[] = {{0, 0, 0, 0}, {0, 0, 0, 0}};
  itm[0].buffer_length = (short)sizeof(usage);
  itm[0].code = (short)NciUSAGE;
  itm[0].pointer = (void *)&usage;
  return _TreeSetNci(dbid, nid_in, itm);
}
