#include <string.h>
#include <treeshr.h>
#define EMPTY_NODE
#define EMPTY_NCI
#include "treeshrp.h"
#include <mds_stdarg.h>
#include <usagedef.h>
#include <libroutines.h>
#include <strroutines.h>

#define node_to_node_number(node_ptr) node_ptr - dblist->tree_info->node
#define __toupper(c) (((c) >= 'a' && (c) <= 'z') ? (c) & 0xDF : (c))

extern void *DBID;

int       _TreeAddNode(void *dbid, char *name, int *nid_out, char usage);

int TreeAddNode(char *name, int *nid_out, char usage)
{
  return _TreeAddNode(DBID, name, nid_out, usage);
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
	  status = TreeEndConglomerate(dblist);
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
          strncpy(new_ptr->name,name,sizeof(new_ptr->name));
          for (i=strlen(name);i<sizeof(new_ptr->name);i++)
	    new_ptr->name[i]=' ';
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
	    new_nci.NCI_FLAG_WORD.NCI_FLAGS.parent_state = !(_TreeIsOn(dblist,*(int *)&parent_nid) & 1);
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
  child_ptr->parent = link_of(parent_ptr,child_ptr);                          /* fill in the parent pointer */
  child_ptr->brother = 0;                                                     /* Assume it will be only child */
  if (parent_ptr->child == 0)                                                 /* If first child */
    parent_ptr->child = link_of(child_ptr,parent_ptr);                        /*   hook it up */
  else                                                                               /* else */
  {
    if (sort)
    {
      for(pre_ptr = 0, tmp_ptr = child_of(parent_ptr);                                /*   for all children < this one */
          tmp_ptr && (strncmp((const char *)tmp_ptr->name, (const char *)child_ptr->name, 12) < 0);
          pre_ptr = tmp_ptr,tmp_ptr = brother_of(tmp_ptr));
      if (pre_ptr == 0)                                                               /*   if this will be first child */
      {
        child_ptr->brother = link_of(child_of(parent_ptr),child_ptr);          /*     make bro old first child */
        parent_ptr->child = link_of(child_ptr,parent_ptr);                     /*     make it first child  */
      }
      else                                                                            /*   else */
      {
        if (pre_ptr->brother == 0)                                             /*     if it will be last child */
          child_ptr->brother = 0;                                              /*       it has no brother */
        else                                                                          /*     else */
          child_ptr->brother = link_of(brother_of(pre_ptr),child_ptr);         /*       its bro is the previous's bro */
        pre_ptr->brother = link_of(child_ptr,pre_ptr);                         /*     the previous's bro is this one */
      }
    }
    else
    {
      for(tmp_ptr = child_of(parent_ptr);tmp_ptr->brother;                    /*    Find last child */
          tmp_ptr = brother_of(tmp_ptr));
      tmp_ptr->brother = link_of(child_ptr,tmp_ptr);                           /*   make this child its brother */
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
      member_ptr->parent = link_of(parent_ptr,member_ptr);                        /* fill in the parent pointer */
      member_ptr->brother = 0;                                                    /* Assume it will be only member*/
      if (parent_ptr->member == 0)                                                /* If first member*/
        parent_ptr->member = link_of(member_ptr,parent_ptr);                      /*   hook it up */
      else                                                                               /* else */
      {
        if (sort)
        {
         for(pre_ptr = 0, tmp_ptr = member_of(parent_ptr);                               /*   for all members < this one */
            tmp_ptr && (strncmp((const char *)tmp_ptr->name, (const char *)member_ptr->name, 12) < 0);
            pre_ptr = tmp_ptr, tmp_ptr = brother_of(tmp_ptr));
         if (pre_ptr == 0)                                                               /*   if this will be first child */
         {
           member_ptr->brother = link_of(member_of(parent_ptr),member_ptr);       /*     make bro old first child */
           parent_ptr->member = link_of(member_ptr,parent_ptr);                   /*     make it first child  */
         }
         else                                                                            /*   else */
         {
           if (pre_ptr->brother == 0)                                             /*     if it will be last child */
             member_ptr->brother = 0;                                             /*       it has no brother */
           else                                                                          /*     else */
             member_ptr->brother = link_of(brother_of(pre_ptr),member_ptr);       /*       its bro is the previous's bro */
           pre_ptr->brother = link_of(member_ptr,pre_ptr);                        /*     the previous's bro is this one */
         }
        }
        else
        {
         for(tmp_ptr = member_of(parent_ptr);                                            /*    Find last member */
            tmp_ptr->brother;tmp_ptr = brother_of(tmp_ptr));
         tmp_ptr->brother = link_of(member_ptr,tmp_ptr);                          /*   make this child its brother */
        }
      }
      return status;                                                                     /* return the status */
    }


int       TreeEndConglomerate(PINO_DATABASE *dblist)
{
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
