#include <mdsshr.h>
#include <string.h>
#include <stdlib.h>
#include <treeshr.h>
#define EMPTY_NODE
#define EMPTY_NCI
#include <ncidef.h>
#include "treeshrp.h"
#include <mds_stdarg.h>
#include <usagedef.h>
#include <libroutines.h>
#include <strroutines.h>

#define max(a,b) ((a) > (b) ? (a) : (b))



#define node_to_node_number(node_ptr) node_ptr - dblist->tree_info->node
#define __toupper(c) (((c) >= 'a' && (c) <= 'z') ? (c) & 0xDF : (c))

extern void *DBID;

int TreeAddNode(char *name, int *nid_out, char usage)
{
  return _TreeAddNode(DBID, name, nid_out, usage);
}

int TreeAddConglom(char *path, char *congtype, int *nid)
{
  return _TreeAddConglom(DBID, path, congtype, nid);
}

int TreeEndConglomerate()
{
  return _TreeEndConglomerate(DBID);
}

int TreeQuitTree(char *exp_ptr, int shotid)
{
  return _TreeQuitTree(&DBID, exp_ptr, shotid);
}

int TreeSetSubtree(int nid)
{
  return _TreeSetSubtree(DBID, nid);
}

int TreeStartConglomerate(int size)
{
  return _TreeStartConglomerate(DBID, size);
}

int TreeWriteTree(char *exp_ptr, int shotid)
{
  return _TreeWriteTree(&DBID, exp_ptr, shotid);
}

int       _TreeAddNode(void *dbid, char *name, int *nid_out, char usage)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID	   *nid_ptr = 0;
  int       status;
  NODE     *parent;
  NODE     *new_ptr;
  char     *node_name;
  SEARCH_TYPE node_type;
  NID       nid;
  int       arg_count;
  static NCI nci;
  static char blank = ' ';
  short    *conglom_size;
  short    *conglom_index;
  char *upcase_name;
  int i;
  int len = strlen(name);
/*****************************************************
  Make sure that the tree is open and OK and editable
*****************************************************/
  if (!IS_OPEN_FOR_EDIT(dblist))
    return TreeNOEDIT;

  upcase_name = (char *)malloc(len+1);
  for (i=0;i<len;i++) upcase_name[i] = __toupper(name[i]);
  upcase_name[len]='\0';

/******************************************************
   See if the node's parent is already in the tree
   if not it is an error.
******************************************************/
  status = TreeFindParent(dblist, upcase_name, &parent, &node_name, &node_type);
  if (status & 1)
  {
  /****************************************************
    make sure that the node is not already there
  *****************************************************/
    status = TreeFindNode(upcase_name, (int *)&nid);
    free(upcase_name);
    if (status & 1)
      status = TreeALREADY_THERE;
    else if (status == TreeNNF)
    {
    /***********************************************
      If a conglomerate is being built make sure that
      it has not grown to big and increment the
      conglomerate node number.
    ************************************************/
      status = 1;
      conglom_size = &dblist->tree_info->edit->conglomerate_size;
      conglom_index = &dblist->tree_info->edit->conglomerate_index;
      if (*conglom_size)
      {
	(*conglom_index)++;
	if (*conglom_index > *conglom_size)
	  status = _TreeEndConglomerate((void *)dblist);
      }
    /************************************************
      If OK so far so grab a new node, Fill in the name
      and insert it into the list of brothers.
    *************************************************/
      if ((status & 1) && (node_type == BROTHER_TYPE_NOWILD) || (node_type == MEMBER_TYPE_NOWILD))
      {
	status = TreeNewNode(dblist, &new_ptr, &parent);
	if (status & 1)
	{
	  int       i;
          strncpy(new_ptr->name,node_name,sizeof(new_ptr->name));
          for (i=strlen(node_name);i<sizeof(new_ptr->name);i++)
	    new_ptr->name[i]=' ';
          free(node_name);
	  new_ptr->child = 0;
	  new_ptr->conglomerate_elt = *conglom_index;
	  if (node_type == BROTHER_TYPE_NOWILD)
	  {
	    status = TreeInsertChild(parent, new_ptr, dblist->tree_info->header->sort_children);
	    new_ptr->usage = TreeUSAGE_STRUCTURE;
	  }
	  else
	  {
	    status = TreeInsertMember(parent, new_ptr, dblist->tree_info->header->sort_members);
            new_ptr->usage = usage;
	  }
          node_to_nid(dblist, new_ptr, ((NID *)nid_out));
	}
	if (status & 1)
	{
	  static NCI new_nci;
	  NCI       scratch_nci;
	  NID       nid;
	  NID       parent_nid;
	  node_to_nid(dblist, parent, (&parent_nid));
	  node_to_nid(dblist, new_ptr, (&nid));
	  status = TreeGetNciLw(dblist->tree_info, nid.node, &scratch_nci);
	  if (status & 1)
	  {
            if (_TreeIsOn(dblist,*(int *)&parent_nid) & 1)
              new_nci.flags &= ~NciM_PARENT_STATE;
            else
              new_nci.flags |= NciM_PARENT_STATE;
	    /*
	    new_nci.NCI_FLAG_WORD.NCI_FLAGS.parent_state = !(_TreeIsOn(dblist,*(int *)&parent_nid) & 1);
	    */
	    status = TreePutNci(dblist->tree_info, nid.node, &new_nci, 1);
	  }
	}
      }
      else
	status = TreeINVPATH;
    }
  }
  if (status & 1)
    dblist->modified = 1;
  return status;
}

int TreeInsertChild(NODE *parent_ptr,NODE *child_ptr,int  sort)
{
  int status;
  int done = 0;
  NODE *pre_ptr;
  NODE *tmp_ptr;
  status = 1;                                                                        /* Assume success */
  link_it(child_ptr->parent,parent_ptr,child_ptr);                          /* fill in the parent pointer */
  child_ptr->brother = 0;                                                     /* Assume it will be only child */
  if (parent_ptr->child == 0)                                                 /* If first child */
  {
    link_it(parent_ptr->child,child_ptr,parent_ptr);                        /*   hook it up */
  }
  else                                                                               /* else */
  {
    if (sort)
    {
      for(pre_ptr = 0, tmp_ptr = child_of(parent_ptr);                                /*   for all children < this one */
          tmp_ptr && (strncmp((const char *)tmp_ptr->name, (const char *)child_ptr->name, 12) < 0);
          pre_ptr = tmp_ptr,tmp_ptr = brother_of(tmp_ptr));
      if (pre_ptr == 0)                                                               /*   if this will be first child */
      {
        link_it(child_ptr->brother,child_of(parent_ptr),child_ptr);          /*     make bro old first child */
        link_it(parent_ptr->child,child_ptr,parent_ptr);                     /*     make it first child  */
      }
      else                                                                            /*   else */
      {
        if (pre_ptr->brother == 0)                                             /*     if it will be last child */
          child_ptr->brother = 0;                                              /*       it has no brother */
        else                                                                          /*     else */
	{
          link_it(child_ptr->brother,brother_of(pre_ptr),child_ptr);         /*       its bro is the previous's bro */
        }
        link_it(pre_ptr->brother,child_ptr,pre_ptr);                         /*     the previous's bro is this one */
      }
    }
    else
    {
      for(tmp_ptr = child_of(parent_ptr);tmp_ptr->brother;                    /*    Find last child */
          tmp_ptr = brother_of(tmp_ptr));
      link_it(tmp_ptr->brother,child_ptr,tmp_ptr);                           /*   make this child its brother */
    }
  }
  return status;                                                                     /* return the status */
}

int TreeInsertMember(NODE *parent_ptr,NODE *member_ptr,int  sort)
{
      static int status;
      int done = 0;
      static NODE *tmp_ptr;
      static NODE *pre_ptr;
/*------------------------------------------------------------------------------

 Executable:
*/
      status = 1;                                                                        /* Assume success */
      link_it(member_ptr->parent,parent_ptr,member_ptr);                        /* fill in the parent pointer */
      member_ptr->brother = 0;                                                    /* Assume it will be only member*/
      if (parent_ptr->member == 0)                                                /* If first member*/
      {
        link_it(parent_ptr->member,member_ptr,parent_ptr);                      /*   hook it up */
      }
      else                                                                               /* else */
      {
        if (sort)
        {
         for(pre_ptr = 0, tmp_ptr = member_of(parent_ptr);                               /*   for all members < this one */
            tmp_ptr && (strncmp((const char *)tmp_ptr->name, (const char *)member_ptr->name, 12) < 0);
            pre_ptr = tmp_ptr, tmp_ptr = brother_of(tmp_ptr));
         if (pre_ptr == 0)                                                               /*   if this will be first child */
         {
           link_it(member_ptr->brother,member_of(parent_ptr),member_ptr);       /*     make bro old first child */
           link_it(parent_ptr->member,member_ptr,parent_ptr);                   /*     make it first child  */
         }
         else                                                                            /*   else */
         {
           if (pre_ptr->brother == 0)                                             /*     if it will be last child */
             member_ptr->brother = 0;                                             /*       it has no brother */
           else                                                                          /*     else */
	   {
             link_it(member_ptr->brother,brother_of(pre_ptr),member_ptr);       /*       its bro is the previous's bro */
           }
           link_it(pre_ptr->brother,member_ptr,pre_ptr);                        /*     the previous's bro is this one */
         }
        }
        else
        {
         for(tmp_ptr = member_of(parent_ptr);                                            /*    Find last member */
            tmp_ptr->brother;tmp_ptr = brother_of(tmp_ptr));
         link_it(tmp_ptr->brother,member_ptr,tmp_ptr);                          /*   make this child its brother */
        }
      }
      return status;                                                                     /* return the status */
    }


int       TreeNewNode(PINO_DATABASE *db_ptr, NODE **node_ptrptr, NODE **trn_node_ptrptr)
{
  int       status = 1;
  NODE     *node_ptr;
  TREE_INFO *info_ptr = db_ptr->tree_info;
  TREE_HEADER *header_ptr = info_ptr->header;
/********************************
  See if we need to expand the
  free list.
*********************************/

  if (header_ptr->free == -1)
    status = TreeExpandNodes(db_ptr, 1, &trn_node_ptrptr);

  if (status & 1)
  {

  /**************************************
    Use the first node on the free list.
  **************************************/

    node_ptr = (NODE *) ((char *) info_ptr->node + header_ptr->free);

  /*************************************
    Remove it from the free list
  *************************************/

    if (node_ptr->parent)
    {
      header_ptr->free += node_ptr->parent;
      (parent_of(node_ptr))->child = 0;
    }
    else
      header_ptr->free = -1;

  /***********************************
    Return the node
  ************************************/

    *node_ptrptr = node_ptr;
  }
  return status;
}

#define EXTEND_NODES 512

int       TreeExpandNodes(PINO_DATABASE *db_ptr, int num_fixup, NODE ***fixup_nodes)
{
  int      *saved_node_numbers;
  int       status = 1;
  NODE    **node_ptrptr;
  NODE     *node_ptr;
  NODE     *ptr;
  TREE_INFO *info_ptr;
  TREE_HEADER *header_ptr;
  TREE_EDIT *edit_ptr;
  int       i;
  NCI      *nciptr;
  static NCI empty_nci;
  int       vm_bytes;
  int       nodes;
  int       ncis;
  static NODE *empty_node_array = 0;
  static NCI *empty_nci_array = 0;
  static int empty_node_size = sizeof(NODE) * EXTEND_NODES; 
  static int empty_nci_size = sizeof(NCI) * EXTEND_NODES;

  if (!empty_node_array)
  {
    /*
    int       uic_code = JPI$_UIC;
    lib$getjpi(&uic_code, 0, 0, &empty_nci.nci$l_owner_identifier, 0, 0);
    */
    empty_node_array = (NODE *) malloc(empty_node_size);
    if (empty_node_array == NULL) return 0;
    empty_nci_array = (NCI *) malloc(empty_nci_size);
    if (empty_nci_array == NULL) return 0;
    for (i = 0; i < EXTEND_NODES; i++)
    {
      empty_node_array[i] = empty_node;
      empty_nci_array[i] = empty_nci;
    }
    empty_node_array[EXTEND_NODES - 1].parent = 0;
  }

/****************************************
   First get pointers to some usefull data
   structures out of the db_ptr.
*****************************************/
  info_ptr = db_ptr->tree_info;
  header_ptr = info_ptr->header;
  edit_ptr = info_ptr->edit;

/***************************************
  If necessary, get new memory for the
  nodes and characteristics
****************************************/
  nodes = EXTEND_NODES*10 + header_ptr->nodes;
  ncis = nodes - edit_ptr->first_in_mem;
  ncis = ncis > 0 ? ncis : 0;
  vm_bytes = nodes * sizeof(NODE) + ncis * sizeof(NCI);
  if (vm_bytes > edit_ptr->node_vm_size)
  {
    vm_bytes = (10 * EXTEND_NODES + header_ptr->nodes) * (sizeof(NODE) + sizeof(NCI));
    ptr = (NODE *)malloc(vm_bytes);
    if (ptr != NULL)
    {
      int       old_node_bytes = header_ptr->nodes * sizeof(NODE);
      int       old_nci_bytes = (header_ptr->nodes - edit_ptr->first_in_mem) * sizeof(NCI);
      old_nci_bytes = old_nci_bytes > 0 ? old_nci_bytes : 0;
    /****************************************
       Get the nids of some important nodes
       and all of the users ones so we can
       fix them up after the addresses of the
       nodes change. find out how many the
       caller wanted and get memory to hold
       their node numbers ...
    ****************************************/
      saved_node_numbers = (int *)malloc((num_fixup + 2) * sizeof(int));
      for (i = 0; i < num_fixup; i++)
	saved_node_numbers[i] = *fixup_nodes[i] - info_ptr->node;
      saved_node_numbers[i++] = info_ptr->root - info_ptr->node;
      saved_node_numbers[i++] = db_ptr->default_node - info_ptr->node;
      nciptr = (NCI *) (ptr + 10 * EXTEND_NODES + header_ptr->nodes);
    /***************************************
      copy the nodes and ncis
    ****************************************/
      if (old_node_bytes)
        memcpy(ptr,info_ptr->node,old_node_bytes);
      if (old_nci_bytes)
        memcpy(nciptr, edit_ptr->nci, old_nci_bytes);
      if (edit_ptr->node_vm_size)
        free(info_ptr->node);
      edit_ptr->node_vm_size = vm_bytes;
      info_ptr->node = ptr;
      edit_ptr->nci = nciptr;
    /***************************************
     loop changing the callers node numbers
     back into node pointers.  Then fix up
     the node pointers in the db and
     tree_info.
    ****************************************/

      for (i = 0; i < num_fixup; i++)
	*fixup_nodes[i] = info_ptr->node + saved_node_numbers[i];
      info_ptr->root = info_ptr->node + saved_node_numbers[i++];
      db_ptr->default_node = info_ptr->node + saved_node_numbers[i++];
      free(saved_node_numbers);

    }
    else
      return status;
  }
  memcpy(info_ptr->node + header_ptr->nodes, empty_node_array, empty_node_size);
  if (ncis)
    memcpy(edit_ptr->nci + header_ptr->nodes - edit_ptr->first_in_mem, empty_nci_array, empty_nci_size);
/******************************************
  If the free list was empty then make
  the free list pointer point to the new
  nodes created.
  otherwise
  make the free list pointer point to
  the same node number it used to point
  to and make the parent pointer of the
  old last node in the free list point
  to the first free node in the freelist.
*******************************************/
  if (header_ptr->free == -1)
  {
    header_ptr->free = header_ptr->nodes * sizeof(NODE);
    node_ptr = (NODE *) ((char *) info_ptr->node + header_ptr->free);
    node_ptr->child = 0;
  }
  else
  {
    for (node_ptr = (NODE *) ((char *) info_ptr->node + header_ptr->free);
	 node_ptr->parent; node_ptr = parent_of(node_ptr));
    node_ptr->parent = (int) (info_ptr->node + header_ptr->nodes) - (int) node_ptr;
    (info_ptr->node + header_ptr->nodes)->child = -node_ptr->parent;
  }
  header_ptr->nodes += EXTEND_NODES;
  return status;
}

int _TreeAddConglom(void *dbid, char *path, char *congtype, int *nid)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  int       status;
  struct descriptor expdsc = {0, DTYPE_T, CLASS_S, 0};
  char exp[256];
  static void *arglist[4] = {(void *)3};
  static DESCRIPTOR(tdishr,"TdiShr");
  static DESCRIPTOR(tdiexecute,"TdiExecute");
  static int addstatus = 0;
  static DESCRIPTOR_LONG(statdsc,&addstatus);
  int (*addr)();
  if (!IS_OPEN_FOR_EDIT(dblist))
    return TreeNOEDIT;
  if (path[0] == '\\')
    sprintf(exp,"%s__add('\\%s',_nid)",congtype,path);
  else
    sprintf(exp,"%s__add('%s',_nid)",congtype,path);
  status = LibFindImageSymbol(&tdishr,&tdiexecute,&addr);
  if (status & 1)
  {
    void *old_dbid = DBID;
    expdsc.length = strlen(exp);
    expdsc.pointer = exp;
    arglist[1] = &expdsc;
    arglist[2] = &statdsc;
    arglist[3] = MdsEND_ARG;
    DBID = dbid;
    status = LibCallg(arglist,addr);
    DBID = old_dbid;
    if (status & 1)
    {
      status = addstatus;
      if (status & 1)
      {
        status = _TreeFindNode(dbid, path, nid);
      }
    }
  }
  return status;
}

#define set_parent(nod_ptr, new_par_ptr) \
    (nod_ptr)->parent = (char *)new_par_ptr - (char *)nod_ptr
#define set_child(nod_ptr, new_child_ptr) \
    (nod_ptr)->child = (char *)new_child_ptr - (char *)nod_ptr

int _TreeStartConglomerate(void *dbid, int size)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  int       i;
  int       status;
  TREE_INFO *info_ptr;
  TREE_HEADER *header_ptr;
  NODE     *next_node_ptr;
  NODE     *starting_node_ptr;
  NODE     *this_node_ptr;
/*****************************************************
  Make sure that the tree is open and OK and editable
*****************************************************/
  if (!(IS_OPEN_FOR_EDIT(dblist)))
    return TreeNOEDIT;

/******************************************************
  First get some useful pointers to the tree data
  structures. Make sure that there is at least one free
  node.
******************************************************/
  info_ptr = dblist->tree_info;
  header_ptr = info_ptr->header;
  if (header_ptr->free == -1)
    status = TreeExpandNodes(dblist, 0, 0);
  this_node_ptr = (NODE *) ((char *) info_ptr->node + header_ptr->free);

/******************************************************
  Loop until a big enough chunk of contigous nodes
  is found looking for it.  If nessesary expand the
  node pool and continue looking.
******************************************************/
  for (status = 1, i = 0; (status & 1) && i < size - 1;)
  {
    if (i == 0)
      starting_node_ptr = this_node_ptr;
    if (this_node_ptr->parent)
    {
      next_node_ptr = parent_of(this_node_ptr);
      if (next_node_ptr - this_node_ptr != 1)
	i = 0;
      else
	i++;
      this_node_ptr = next_node_ptr;
    }
    else
    {
      NODE **fixups[3];
      fixups[0] = &this_node_ptr;
      fixups[1] = &starting_node_ptr;
      fixups[2] = &next_node_ptr;
      status = TreeExpandNodes(dblist, 3, fixups);
    }
  }

/***************************************************
  if OK so far then make sure that the free list
  pointer points to this block of nodes.  If
  nessesary move the pointers around to  make it so.
****************************************************/
  if (status & 1)
  {
    if (starting_node_ptr->child != 0)
    {
      if (parent_of(this_node_ptr))
      {
	set_parent(child_of(starting_node_ptr), parent_of(this_node_ptr));
	set_child(parent_of(this_node_ptr), child_of(starting_node_ptr));
      }
      else
	(child_of(starting_node_ptr))->parent = 0;
      set_parent(this_node_ptr, (NODE *) ((char *) info_ptr->node + header_ptr->free));
      set_child((NODE *) ((char *) info_ptr->node + header_ptr->free), this_node_ptr);
      header_ptr->free = (char *) starting_node_ptr - (char *) info_ptr->node;
      starting_node_ptr->child = 0;
    }
    info_ptr->edit->conglomerate_index = 0;
    info_ptr->edit->conglomerate_size = size;
    info_ptr->edit->conglomerate_setup = dblist->setup_info;
    dblist->setup_info = 0;
  }
  return status;
}

int _TreeEndConglomerate(void *dbid)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  int       status;
  unsigned short conglom_size;
  unsigned short conglom_index;
/*****************************************************
  Make sure that the tree is open and OK and editable
*****************************************************/
  if (!(IS_OPEN_FOR_EDIT(dblist)))
    return TreeNOEDIT;

  conglom_size = dblist->tree_info->edit->conglomerate_size;
  conglom_index = dblist->tree_info->edit->conglomerate_index;
  dblist->tree_info->edit->conglomerate_size = 0;
  dblist->tree_info->edit->conglomerate_index = 0;
  dblist->setup_info = dblist->tree_info->edit->conglomerate_setup;
  if (conglom_size < conglom_index)
    status = TreeCONGLOMFULL;
  else if (conglom_size > conglom_index)
    status = TreeCONGLOM_NOT_FULL;
  else
    status = 1;
  return status;
}

static void trim_excess_nodes(TREE_INFO *info_ptr);

static int one = 1;
static int zero = 0;

int _TreeWriteTree(void **dbid, char *exp_ptr, int shotid)
{
  PINO_DATABASE **dblist = (PINO_DATABASE **)dbid;
  int       status;
  int       header_pages;
  int       node_pages;
  int       tags_pages;
  int       tag_info_pages;
  int       external_pages;
  int       offset;
  PINO_DATABASE *db;
  PINO_DATABASE *prev_db;
  status = TreeNOT_OPEN;
  if (dblist)
  {
    if (exp_ptr)
    {
      char uptree[13];
      int i;
      int shot;
      int len = strlen(exp_ptr);
      for (i=0;i<12 && i<len;i++)
      uptree[i] = __toupper(exp_ptr[i]);
      uptree[i]='\0';
      shot = (shotid == 0) ? MdsGetCurrentShotId(exp_ptr) : shotid;
      status = TreeNOT_OPEN;
      for (prev_db = 0, db = (*dblist); db ? db->open : 0; prev_db = db, db = db->next)
      {
	if ((shot == db->shotid) && (strcmp(exp_ptr, db->main_treenam) == 0))
	{
	  if (prev_db)
	  {
	    prev_db->next = db->next;
	    db->next = (*dblist);
	    *dblist = db;
	  }
	  status = (*dblist)->open_for_edit ? TreeNORMAL : TreeNOT_OPEN;
	  break;
	}
      }
    }
    else
      status = (*dblist)->open_for_edit ? TreeNORMAL : TreeNOT_OPEN;
    if (status & 1)
    {
    /**************************************
     Compute number of pages to allocate for
     each part of the tree file.
     Create a file of the correct size then
     write out each part of the tree file.
     Close the file.
     Write out the characteristics file.
     **************************************/

      FILE *ntreef;
      TREE_INFO *info_ptr = (*dblist)->tree_info;
      char *nfilenam = strcpy(malloc(strlen(info_ptr->filespec)+2),info_ptr->filespec);
      /*      trim_excess_nodes(info_ptr); */
      header_pages = (sizeof(TREE_HEADER) + 511) / 512;
      node_pages = (info_ptr->header->nodes * sizeof(NODE) + 511) / 512;
      tags_pages = (info_ptr->header->tags * 4 + 511) / 512;
      tag_info_pages = (info_ptr->header->tags * sizeof(TAG_INFO) + 511) / 512;
      external_pages = (info_ptr->header->externals * 4 + 511) / 512;

      strcat(nfilenam,"#");
      ntreef = fopen(nfilenam,"wb");
      if (ntreef)
      {
        size_t num;
        num = fwrite(info_ptr->header,512,header_pages,ntreef);
        if (num != header_pages) goto error_exit;
        num = fwrite(info_ptr->node,512,node_pages,ntreef);
        if (num != node_pages) goto error_exit;
        num = fwrite(info_ptr->tags,512,tags_pages,ntreef);
        if (num != tags_pages) goto error_exit;
        num = fwrite(info_ptr->tag_info,512,tag_info_pages,ntreef);
        if (num != tag_info_pages) goto error_exit;
        num = fwrite(info_ptr->external,512,external_pages,ntreef);
        if (num != external_pages) goto error_exit;

	status = TreeWriteNci(info_ptr);
	if ((status & 1) == 0)
	  goto error_exit;

        remove(info_ptr->filespec);
        rename(nfilenam,info_ptr->filespec);
	(*dblist)->modified = 0;
        TreeCallHook(WriteTree, info_ptr);
      }
    }
  }
  return status;

error_exit:
  return status & 1 ? TreeFAILURE : status;
}

static void trim_excess_nodes(TREE_INFO *info_ptr)
{
  int      *nodecount_ptr = &info_ptr->header->nodes;
  int      *free_ptr = &info_ptr->header->free;
  NODE     *node_ptr;
  NODE     *nodes_ptr = info_ptr->node;
  NODE     *last_node_ptr = nodes_ptr + *nodecount_ptr - 1;
  NODE     *free_node_ptr;
  int       nodes;
  int       node_pages;
  int       length = sizeof(node_ptr->name);
  for (node_ptr = last_node_ptr;
       strncmp((const char *) node_ptr->name, (const char *) empty_node.name, length) == 0; node_ptr--);
  node_pages = ((node_ptr - nodes_ptr + 1) * sizeof(NODE) + 511) / 512;
  nodes = max(info_ptr->edit->first_in_mem, (node_pages * 512) / sizeof(NODE));
  if (nodes < *nodecount_ptr)
  {
    for (node_ptr = &nodes_ptr[nodes]; (*free_ptr != -1) && (node_ptr <= last_node_ptr); node_ptr++)
    {
      if (node_ptr == (NODE *) ((char *) nodes_ptr + *free_ptr))
      {
	if (node_ptr->parent)
	{
	  *free_ptr += node_ptr->parent;
	  (parent_of(node_ptr))->child = 0;
	}
	else
	  *free_ptr = -1;
      }
      else
      {
        NODE *p = parent_of(node_ptr);
        NODE *c = child_of(node_ptr);
	if (p)
	{
          link_it(p->child,c,p);
        }
	if (c)
	{
          link_it(c->parent,p,c);
        }
      }
    }
    *nodecount_ptr = nodes;
  }
  return;
}

int TreeWriteNci(TREE_INFO *info)
{
  int       status = 1;
  if (info->edit->first_in_mem)
  {
    if (info->header->nodes > info->edit->first_in_mem)
    {
      status = TreeFAILURE;
      if (!fseek(info->nci_file->put,info->edit->first_in_mem * sizeof(struct nci),SEEK_SET))
      {
        size_t num = fwrite(info->edit->nci,sizeof(struct nci),info->header->nodes - info->edit->first_in_mem,info->nci_file->put);
        if (num == (info->header->nodes - info->edit->first_in_mem))
	{
          info->edit->first_in_mem = info->header->nodes;
          status = TreeNORMAL;
        }
      }
    }
  }
  return status;
}

int _TreeSetSubtree(void *dbid, int nid)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID *nid_ptr = (NID *)&nid;
  NODE     *node_ptr;
  int       node_idx;
  int       numext;
  int       pages_needed;
  int       pages_allocated;
  int       i;
  int       status;
  int      *new_external_ptr;

/**************************************************
 First we must check to make sure we are editting
 a valid tree and that the node we are going to make
 into a subtree reference does not already have any
 children.
**************************************************/

  if (!(IS_OPEN_FOR_EDIT(dblist)))
    return TreeNOEDIT;

  nid_to_node(dblist, nid_ptr, node_ptr);
  if (node_ptr->child != 0)
    return TreeNOTCHILDLESS;
  if (node_ptr->member != 0)
    return TreeNOTMEMBERLESS;
  if (!(TreeIsChild(node_ptr) & 1))
    return TreeNOTSON;

/***************************************************
 Check to see if this node is already marked as a
 subtree reference.   If so just check the usage.
***************************************************/

  node_idx = node_ptr - dblist->tree_info->node;
  for (i = 0; i < dblist->tree_info->header->externals; i++) {
    if (*(dblist->tree_info->external + i) == node_idx) {
      if (node_ptr->usage != TreeUSAGE_SUBTREE) {
        node_ptr->usage = TreeUSAGE_SUBTREE;
        dblist->modified = 1;
       }     
      return TreeNORMAL;
    }
  }
/*****************************************************
 Next see if we need to expand the externals list
 memory allocation to add a new one.
 If so get another page and move the existing externals
 into the new memory.
******************************************************/

  numext = dblist->tree_info->header->externals + 1;
  pages_needed = (numext * 4 + 511) / 512;
  pages_allocated = max((numext * 4 + 507) / 512, dblist->tree_info->edit->external_pages);
  if (pages_needed > pages_allocated)
  {
    new_external_ptr = malloc(pages_needed * 512);
    status = new_external_ptr == 0 ? TreeFAILURE : TreeNORMAL;
    if ((status & 1) != 1)
    {
      return status;
    }
    memcpy(new_external_ptr, dblist->tree_info->external,(numext - 1) * 4);
    if (dblist->tree_info->edit->external_pages > 0)
      free(dblist->tree_info->external);
    dblist->tree_info->edit->external_pages = pages_needed;
    dblist->tree_info->external = new_external_ptr;
  }

/*****************************************************
 Finally, load the node index into the externals list
 and increment the number of externals.
*****************************************************/

  *(dblist->tree_info->external + numext - 1) = node_idx;
  dblist->tree_info->header->externals++;
  dblist->modified = 1;

/******************************************
  Set the usage of this node to be SUBTREE
*******************************************/
  node_ptr->usage = TreeUSAGE_SUBTREE;

  return TreeNORMAL;
}


int _TreeSetNosubtree(void *dbid, int nid)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID *nid_ptr = (NID *)nid;
  NODE     *node_ptr;
  int       node_idx;
  int       ext_idx;
  int       i;
  int       status;
/*****************************************************
  Make sure that the tree is open and OK and editable
*****************************************************/
  if (!(IS_OPEN_FOR_EDIT(dblist)))
    return TreeNOEDIT;

/************************************
 Find the node in the externals list.
*************************************/

  nid_to_node(dblist, nid_ptr, node_ptr);
  node_idx = node_ptr - dblist->tree_info->node;
  for (ext_idx = 0; ext_idx < dblist->tree_info->header->externals; ext_idx++)
    if (*(dblist->tree_info->external + ext_idx) == node_idx)
      break;
  if (ext_idx >= dblist->tree_info->header->externals)
    return TreeNORMAL;

/**************************************
 Decrement the number of externals and
 fill up the hole left by this removal.
 *************************************/

  dblist->tree_info->header->externals--;
  for (i = ext_idx; i < dblist->tree_info->header->externals; i++)
    *(dblist->tree_info->external + i) =
      *(dblist->tree_info->external + i + 1);
  dblist->modified = 1;

/*******************************
  Set this node usage STRUCTURE
********************************/
  node_ptr->usage = TreeUSAGE_STRUCTURE;

  return TreeNORMAL;
}

int _TreeQuitTree(void **dbid, char *exp_ptr, int shotid)
{
  PINO_DATABASE **dblist = (PINO_DATABASE **)dbid;
  int       status;
  PINO_DATABASE *db;
  PINO_DATABASE *prev_db;
  status = TreeNOT_OPEN;
  if (*dblist)
  {
    if (exp_ptr)
    {
      char uptree[13];
      int i;
      int shot;
      int len = strlen(exp_ptr);
      for (i=0;i<12 && i<len;i++)
      uptree[i] = __toupper(exp_ptr[i]);
      uptree[i]='\0';
      shot = (shotid == 0) ? MdsGetCurrentShotId(exp_ptr) : shotid;
      status = TreeNOT_OPEN;
      for (prev_db = 0, db = (*dblist); db ? db->open : 0; prev_db = db, db = db->next)
      {
	if ((shot == db->shotid) && (strcmp(exp_ptr, db->main_treenam) == 0))
	{
	  if (prev_db)
	  {
	    prev_db->next = db->next;
	    db->next = (*dblist);
	    *dblist = db;
	  }
	  status = (*dblist)->open_for_edit ? TreeNORMAL : TreeNOT_OPEN;
	  break;
	}
      }
    }
    else
      status = (*dblist)->open_for_edit ? TreeNORMAL : TreeNOT_OPEN;
    if ((status & 1) && ((*dblist)->open))
    {
      (*dblist)->modified = 0;
      status = _TreeClose(dbid,0,0);
    }
  }
  return status;
}
