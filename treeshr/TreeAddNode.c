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
#define EMPTY_NODE
#define EMPTY_NCI
#include "treeshrp.h"		/*must be first or off_t is misdefined */
#include <STATICdef.h>

#include <string.h>
#include <stdlib.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <string.h>
#include <stdlib.h>
#include <treeshr.h>
#include <treeshr_messages.h>
#include <errno.h>
#include <fcntl.h>
#include <ncidef.h>
#include <mds_stdarg.h>
#include <usagedef.h>
#include <libroutines.h>
#include <strroutines.h>
#include <ctype.h>

#ifdef max
#undef max
#endif
#define max(a,b) ((a) > (b) ? (a) : (b))

#define node_to_node_number(node_ptr) node_ptr - dblist->tree_info->node

extern void **TreeCtx();
STATIC_ROUTINE int TreeNewNode(PINO_DATABASE * db_ptr, NODE ** node_ptrptr,
			       NODE ** trn_node_ptrptr);
STATIC_ROUTINE int TreeWriteNci(TREE_INFO * info);

int TreeAddNode(char const *name, int *nid_out, char usage)
{
  return _TreeAddNode(*TreeCtx(), name, nid_out, usage);
}

int TreeAddConglom(char const *path, char const *congtype, int *nid)
{
  return _TreeAddConglom(*TreeCtx(), path, congtype, nid);
}

int TreeEndConglomerate()
{
  return _TreeEndConglomerate(*TreeCtx());
}

int TreeQuitTree(char const *exp_ptr, int shotid)
{
  return _TreeQuitTree(TreeCtx(), exp_ptr, shotid);
}

int TreeSetNoSubtree(int nid)
{
  return _TreeSetNoSubtree(*TreeCtx(), nid);
}

int TreeSetSubtree(int nid)
{
  return _TreeSetSubtree(*TreeCtx(), nid);
}

int TreeStartConglomerate(int size)
{
  return _TreeStartConglomerate(*TreeCtx(), size);
}

int TreeWriteTree(char const *exp_ptr, int shotid)
{
  return _TreeWriteTree(TreeCtx(), exp_ptr, shotid);
}

int64_t TreeGetDatafileSize()
{
  return _TreeGetDatafileSize(*TreeCtx());
}

int _TreeAddNode(void *dbid, char const *name, int *nid_out, char usage)
{
  INIT_STATUS_AS TreeNORMAL;
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NODE *parent;
  NODE *new_ptr = NULL;
  char *node_name;
  SEARCH_TYPE node_type;
  int nid;
  short *conglom_size;
  short *conglom_index;
  char *upcase_name;
  size_t i;
  size_t len = strlen(name);
/*****************************************************
  Make sure that the tree is open and OK and editable
*****************************************************/
  if (!IS_OPEN_FOR_EDIT(dblist))
    return TreeNOEDIT;
  upcase_name = (char *)malloc(len + 1);
  for (i = 0; i < len; i++)
    upcase_name[i] = (char)toupper(name[i]);
  upcase_name[len] = '\0';

/******************************************************
   See if the node's parent is already in the tree
   if not it is an error.
******************************************************/
  status = TreeFindParent(dblist, upcase_name, &parent, &node_name, &node_type);
  if STATUS_OK {
  /****************************************************
    make sure that the node is not already there
  *****************************************************/
    status = _TreeFindNode(dbid, upcase_name, &nid);
    if STATUS_OK
      status = TreeALREADY_THERE;
    else if (status == TreeNNF) {
    /***********************************************
      If a conglomerate is being built make sure that
      it has not grown to big and increment the
      conglomerate node number.
    ************************************************/
      status = TreeNORMAL;
      conglom_size = &dblist->tree_info->edit->conglomerate_size;
      conglom_index = &dblist->tree_info->edit->conglomerate_index;
      if (*conglom_size) {
	(*conglom_index)++;
	if (*conglom_index > *conglom_size)
	  status = _TreeEndConglomerate((void *)dblist);
      }
    /************************************************
      If OK so far so grab a new node, Fill in the name
      and insert it into the list of brothers.
    *************************************************/
      if (STATUS_OK && ((node_type == BROTHER_TYPE_NOWILD) || (node_type == MEMBER_TYPE_NOWILD))) {
	status = TreeNewNode(dblist, &new_ptr, &parent);
	if STATUS_OK {
	  size_t i;
	  short idx = *conglom_index;
	  strncpy(new_ptr->name, node_name, sizeof(new_ptr->name));
	  for (i = strlen(node_name); i < sizeof(new_ptr->name); i++)
	    new_ptr->name[i] = ' ';
	  new_ptr->child = 0;
	  LoadShort(idx, &new_ptr->conglomerate_elt);
	  if (node_type == BROTHER_TYPE_NOWILD || usage == TreeUSAGE_STRUCTURE
	      || usage == TreeUSAGE_SUBTREE) {
	    status = TreeInsertChild(parent, new_ptr, dblist->tree_info->header->sort_children);
	    new_ptr->usage = usage == TreeUSAGE_SUBTREE ? TreeUSAGE_SUBTREE : TreeUSAGE_STRUCTURE;
	  } else {
	    status = TreeInsertMember(parent, new_ptr, dblist->tree_info->header->sort_members);
	    new_ptr->usage = (unsigned char)(((usage <= TreeUSAGE_MAXIMUM) && (usage >= 0)) ? usage : TreeUSAGE_ANY);
	  }
	  *nid_out = node_to_nid(dblist, new_ptr, 0);
	}
	if STATUS_OK {
	  NCI new_nci;
	  NCI scratch_nci;
	  NID nid;
	  int parent_nid;
	  memset(&new_nci, 0, sizeof(NCI));
	  parent_nid = node_to_nid(dblist, parent, 0);
	  node_to_nid(dblist, new_ptr, &nid);
	  status = TreeGetNciLw(dblist->tree_info, nid.node, &scratch_nci);
	  if STATUS_OK {
	    if (_TreeIsOn(dblist, *(int *)&parent_nid) & 1)
	      new_nci.flags &= (unsigned)~NciM_PARENT_STATE;
	    else
	      new_nci.flags |= NciM_PARENT_STATE;
	    new_nci.flags |= NciM_COMPRESS_ON_PUT;
	    status = TreePutNci(dblist->tree_info, nid.node, &new_nci, 1);
	    TreeUnLockNci(dblist->tree_info, 0, nid.node);
	  }
	}
      } else
	status = TreeINVPATH;
    }
    free(node_name);
  }
  if STATUS_OK
    dblist->modified = 1;
  free(upcase_name);
  return status;
}

int TreeInsertChild(NODE * parent_ptr, NODE * child_ptr, int sort)
{
  INIT_STATUS_AS TreeNORMAL;
  NODE *pre_ptr;
  NODE *tmp_ptr;
  child_ptr->parent = node_offset(parent_ptr, child_ptr);	/* fill in the parent pointer */
  child_ptr->brother = 0;	/* Assume it will be only child */
  if (parent_ptr->child == 0) {	/* If first child */
    parent_ptr->child = node_offset(child_ptr, parent_ptr);
  } else {			/* else */

    if (sort) {
      for (pre_ptr = 0, tmp_ptr = child_of(0, parent_ptr);	/*   for all children < this one */
	   tmp_ptr && (strncmp((const char *)tmp_ptr->name, (const char *)child_ptr->name, 12) < 0);
	   pre_ptr = tmp_ptr, tmp_ptr = brother_of(0, tmp_ptr)) ;
      if (pre_ptr == 0) {	/*   if this will be first child */
	child_ptr->brother = node_offset(child_of(0, parent_ptr), child_ptr);	/*     make bro old first child */
	parent_ptr->child = node_offset(child_ptr, parent_ptr);	/*     make it first child  */
      } else {			/*   else */

	if (pre_ptr->brother == 0)	/*     if it will be last child */
	  child_ptr->brother = 0;	/*       it has no brother */
	else {			/*     else */

	  child_ptr->brother = node_offset(brother_of(0, pre_ptr), child_ptr);	/*       its bro is the previous's bro */
	}
	pre_ptr->brother = node_offset(child_ptr, pre_ptr);	/*     the previous's bro is this one */
      }
    } else {
      for (tmp_ptr = child_of(0, parent_ptr); tmp_ptr->brother;	/*    Find last child */
	   tmp_ptr = brother_of(0, tmp_ptr)) ;
      tmp_ptr->brother = node_offset(child_ptr, tmp_ptr);	/*   make this child its brother */
    }
  }
  return status;		/* return the status */
}

int TreeInsertMember(NODE * parent_ptr, NODE * member_ptr, int sort)
{
  NODE *tmp_ptr;
  NODE *pre_ptr;
/*------------------------------------------------------------------------------
 Executable:
*/
  member_ptr->parent = node_offset(parent_ptr, member_ptr);	/* fill in the parent pointer */
  member_ptr->brother = 0;	/* Assume it will be only member */
  if (parent_ptr->member == 0) {	/* If first member */
    parent_ptr->member = node_offset(member_ptr, parent_ptr);	/*   hook it up */
  } else {			/* else */
    if (sort) {
      for (pre_ptr = 0, tmp_ptr = member_of(parent_ptr);	/*   for all members < this one */
	   tmp_ptr
	   && (strncmp((const char *)tmp_ptr->name, (const char *)member_ptr->name, 12) < 0);
	   pre_ptr = tmp_ptr, tmp_ptr = brother_of(0, tmp_ptr)) ;
      if (pre_ptr == 0) {	/*   if this will be first child */
	member_ptr->brother = node_offset(member_of(parent_ptr), member_ptr);	/*     make bro old first child */
	parent_ptr->member = node_offset(member_ptr, parent_ptr);	/*     make it first child  */
      } else {			/*   else */
	if (pre_ptr->brother == 0)	/*     if it will be last child */
	  member_ptr->brother = 0;	/*       it has no brother */
	else {			/*     else */
	  member_ptr->brother = node_offset(brother_of(0, pre_ptr), member_ptr);	/*       its bro is the previous's bro */
	}
	pre_ptr->brother = node_offset(member_ptr, pre_ptr);	/*     the previous's bro is this one */
      }
    } else {
      for (tmp_ptr = member_of(parent_ptr);	/*    Find last member */
	   tmp_ptr->brother; tmp_ptr = brother_of(0, tmp_ptr)) ;
      tmp_ptr->brother = node_offset(member_ptr, tmp_ptr);	/*   make this child its brother */
    }
  }
  return TreeNORMAL;		/* return the status */
}

STATIC_ROUTINE int TreeNewNode(PINO_DATABASE * db_ptr, NODE ** node_ptrptr, NODE ** trn_node_ptrptr)
{
  INIT_STATUS_AS TreeNORMAL;
  NODE *node_ptr;
  TREE_INFO *info_ptr = db_ptr->tree_info;
  TREE_HEADER *header_ptr = info_ptr->header;
/********************************
  See if we need to expand the
  free list.
*********************************/
  if (header_ptr->free == -1)
    status = TreeExpandNodes(db_ptr, 1, &trn_node_ptrptr);
  if STATUS_OK {

  /**************************************
    Use the first node on the free list.
  **************************************/

    node_ptr = (NODE *) ((char *)info_ptr->node + header_ptr->free);

  /*************************************
    Remove it from the free list
  *************************************/

    if (node_ptr->parent) {
      header_ptr->free += swapint((char *)&node_ptr->parent);
      (parent_of(0, node_ptr))->child = 0;
    } else
      header_ptr->free = -1;

  /***********************************
    Return the node
  ************************************/
    memset(node_ptr, 0, sizeof(*node_ptr));

    *node_ptrptr = node_ptr;
  }
  return status;
}

#define EXTEND_NODES 512

int TreeExpandNodes(PINO_DATABASE * db_ptr, int num_fixup, NODE *** fixup_nodes)
{
  INIT_STATUS_AS TreeNORMAL;
  int *saved_node_numbers;
  NODE *node_ptr;
  NODE *ptr;
  TREE_INFO *info_ptr;
  TREE_HEADER *header_ptr;
  TREE_EDIT *edit_ptr;
  int i;
  NCI *nciptr;
  STATIC_CONSTANT NCI empty_nci;
  int vm_bytes;//TODO: 2GB limit
  int nodes;
  int ncis;
  STATIC_THREADSAFE NODE *empty_node_array = 0;
  STATIC_THREADSAFE NCI *empty_nci_array = 0;
  STATIC_CONSTANT size_t empty_node_size = sizeof(NODE) * EXTEND_NODES;
  STATIC_CONSTANT size_t empty_nci_size = sizeof(NCI) * EXTEND_NODES;

  if (!empty_node_array) {
    /*
       int       uic_code = JPI$_UIC;
       lib$getjpi(&uic_code, 0, 0, &empty_nci.nci$l_owner_identifier, 0, 0);
     */
    empty_node.parent = sizeof(NODE);
    empty_node.child = -(int)sizeof(NODE);;
    empty_node_array = (NODE *) malloc(empty_node_size);
    if (empty_node_array == NULL)
      return MDSplusERROR;
    empty_nci_array = (NCI *) malloc(empty_nci_size);
    if (empty_nci_array == NULL)
      return MDSplusERROR;
    for (i = 0; i < EXTEND_NODES; i++) {
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
  nodes = EXTEND_NODES * 10 + header_ptr->nodes;
  ncis = nodes - edit_ptr->first_in_mem;
  ncis = ncis > 0 ? ncis : 0;
  ncis = ncis + EXTEND_NODES * 10;
  vm_bytes = nodes * (int)sizeof(NODE) + ncis * (int)sizeof(NCI);
  if (vm_bytes > edit_ptr->node_vm_size) {
    ptr = (NODE *) malloc((size_t)vm_bytes);
    if (ptr != NULL) {
      int old_node_bytes = header_ptr->nodes * (int)sizeof(NODE);
      int old_nci_bytes = (header_ptr->nodes - edit_ptr->first_in_mem) * (int)sizeof(NCI);
      old_nci_bytes = old_nci_bytes > 0 ? old_nci_bytes : 0;
    /****************************************
       Get the nids of some important nodes
       and all of the users ones so we can
       fix them up after the addresses of the
       nodes change. find out how many the
       caller wanted and get memory to hold
       their node numbers ...
    ****************************************/
      saved_node_numbers = (int *)malloc((size_t)(num_fixup + 2) * sizeof(int));
      for (i = 0; i < num_fixup; i++)
	saved_node_numbers[i] = (int)(*fixup_nodes[i] - info_ptr->node);
      saved_node_numbers[i++] = (int)(info_ptr->root - info_ptr->node);
      saved_node_numbers[i++] = (int)(db_ptr->default_node - info_ptr->node);
      nciptr = (NCI *) (ptr + (10 * EXTEND_NODES + nodes));
    /***************************************
      copy the nodes and ncis
    ****************************************/
      if (old_node_bytes)
	memcpy(ptr, info_ptr->node, (size_t)old_node_bytes);
      if (old_nci_bytes)
	memcpy(nciptr, edit_ptr->nci, (size_t)old_nci_bytes);
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
    } else
      return status;
  }
  memcpy(info_ptr->node + header_ptr->nodes, empty_node_array, empty_node_size);
  if (ncis) {
    int nci_offset = header_ptr->nodes - edit_ptr->first_in_mem;
    if (nci_offset < 0)
      nci_offset = 0;
    memcpy(edit_ptr->nci + nci_offset, empty_nci_array, empty_nci_size);
  }
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
  if (header_ptr->free == -1) {
    header_ptr->free = header_ptr->nodes * (int)sizeof(NODE);
    node_ptr = (NODE *) ((char *)info_ptr->node + header_ptr->free);
    node_ptr->child = 0;
  } else {
    int tmp;
    for (node_ptr = (NODE *) ((char *)info_ptr->node + header_ptr->free);
	 node_ptr->parent; node_ptr = parent_of(0, node_ptr)) ;
    node_ptr->parent = node_offset((info_ptr->node + header_ptr->nodes), node_ptr);
    tmp = -swapint((char *)&node_ptr->parent);
    (info_ptr->node + header_ptr->nodes)->child = swapint((char *)&tmp);
  }
  header_ptr->nodes += EXTEND_NODES;
  return status;
}

int _TreeAddConglom(void *dbid, char const *path, char const *congtype, int *nid)
{
  INIT_STATUS_AS TreeNORMAL, addstatus = TreeNORMAL;
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  struct descriptor expdsc = { 0, DTYPE_T, CLASS_S, 0 };
  char exp[256];
  void *arglist[4] = { (void *)3 };
  STATIC_CONSTANT DESCRIPTOR(tdishr, "TdiShr");
  STATIC_CONSTANT DESCRIPTOR(tdiexecute, "TdiExecute");
  DESCRIPTOR_LONG(statdsc, 0);
  STATIC_THREADSAFE int (*addr) ();
  statdsc.pointer = (char *)&addstatus;
  if (!IS_OPEN_FOR_EDIT(dblist))
    return TreeNOEDIT;
  if (path[0] == '\\')
    sprintf(exp, "DevAddDevice('\\%s', '%s')", path, congtype);
  else
    sprintf(exp, "DevAddDevice('%s', '%s')", path, congtype);
  if (addr == 0)
    status = LibFindImageSymbol(&tdishr, &tdiexecute, &addr);
  if STATUS_OK {
    void *old_dbid = *TreeCtx();
    expdsc.length = (unsigned short)strlen(exp);
    expdsc.pointer = exp;
    arglist[1] = &expdsc;
    arglist[2] = &statdsc;
    arglist[3] = MdsEND_ARG;
    *TreeCtx() = dbid;
    status = (int)((char *)LibCallg(arglist, addr) - (char *)0);
    *TreeCtx() = old_dbid;
    if STATUS_OK {
      status = addstatus;
      if STATUS_OK {
	status = _TreeFindNode(dbid, path, nid);
      }
    }
  }
  return status;
}

#define set_parent(nod_ptr, new_par_ptr) \
  (nod_ptr)->parent = (int)((char *)new_par_ptr - (char *)nod_ptr)
#define set_child(nod_ptr, new_child_ptr) \
  (nod_ptr)->child = (int)((char *)new_child_ptr - (char *)nod_ptr)

int _TreeStartConglomerate(void *dbid, int size)
{
  if (size>0x7fff) return TreeBUFFEROVF;
  INIT_STATUS_AS TreeNORMAL;
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  int i;
  TREE_INFO *info_ptr;
  TREE_HEADER *header_ptr;
  NODE *next_node_ptr = NULL;
  NODE *starting_node_ptr = NULL;
  NODE *this_node_ptr;
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
  this_node_ptr = (NODE *) ((char *)info_ptr->node + header_ptr->free);

/******************************************************
  Loop until a big enough chunk of contigous nodes
  is found looking for it.  If nessesary expand the
  node pool and continue looking.
******************************************************/
  for (i = 0; STATUS_OK && i < size - 1;) {
    if (i == 0)
      starting_node_ptr = this_node_ptr;
    if (this_node_ptr->parent) {
      next_node_ptr = parent_of(0, this_node_ptr);
      if (next_node_ptr - this_node_ptr != 1)
	i = 0;
      else
	i++;
      this_node_ptr = next_node_ptr;
    } else {
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
  if STATUS_OK {
    if (starting_node_ptr && starting_node_ptr->child != 0) {
      if (parent_of(0, this_node_ptr)) {
	set_parent(child_of(0, starting_node_ptr), parent_of(0, this_node_ptr));
	set_child(parent_of(0, this_node_ptr), child_of(0, starting_node_ptr));
      } else
	(child_of(0, starting_node_ptr))->parent = 0;
      set_parent(this_node_ptr, (NODE *) ((char *)info_ptr->node + header_ptr->free));
      set_child((NODE *) ((char *)info_ptr->node + header_ptr->free), this_node_ptr);
      header_ptr->free = (int)((char *)starting_node_ptr - (char *)info_ptr->node);
      starting_node_ptr->child = 0;
    }
    info_ptr->edit->conglomerate_index = 0;
    info_ptr->edit->conglomerate_size = (short)size;
    info_ptr->edit->conglomerate_setup = dblist->setup_info;
    dblist->setup_info = 0;
  }
  return status;
}

int _TreeEndConglomerate(void *dbid)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  short conglom_size,conglom_index;
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
    return TreeCONGLOMFULL;
  else if (conglom_size > conglom_index)
    return TreeCONGLOM_NOT_FULL;
  else
    return TreeNORMAL;
}

STATIC_ROUTINE void trim_excess_nodes(TREE_INFO * info_ptr);

#ifdef WORDS_BIGENDIAN
STATIC_ROUTINE TREE_HEADER *HeaderOut(TREE_HEADER * hdr) {
  TREE_HEADER *ans = (TREE_HEADER *) malloc(sizeof(TREE_HEADER));
  ((char *)ans)[1] = (char)((hdr->sort_children ? 1 : 0) | (hdr->sort_members ? 2 : 0));
  ans->free = swapint((char *)&hdr->free);
  ans->tags = swapint((char *)&hdr->tags);
  ans->externals = swapint((char *)&hdr->externals);
  ans->nodes = swapint((char *)&hdr->nodes);
  return ans;
}

STATIC_ROUTINE void FreeHeaderOut(TREE_HEADER * hdr) {
  free(hdr);
}

#else
#define HeaderOut(in) in
#define FreeHeaderOut(in)
#endif

int64_t _TreeGetDatafileSize(void *dbid)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  TREE_INFO *info = dblist->tree_info;
  if ((!info->data_file) || info->data_file->get == 0) {
    if IS_NOT_OK(TreeOpenDatafileR(info))
      return -1;
  }
  return MDS_IO_LSEEK(info->data_file->get, 0, SEEK_END);
}

int _TreeWriteTree(void **dbid, char const *exp_ptr, int shotid)
{
    INIT_STATUS_AS TreeNOT_OPEN;
    PINO_DATABASE **dblist = (PINO_DATABASE **) dbid;
    size_t header_pages;
    size_t node_pages;
    size_t tags_pages;
    size_t tag_info_pages;
    size_t external_pages;
    PINO_DATABASE *db;
    PINO_DATABASE *prev_db;
    if (dblist) {
        if (exp_ptr) {
            char uptree[13];
            size_t i;
            int shot;
            size_t len = strlen(exp_ptr);
            for (i = 0; i < 12 && i < len; i++)
                uptree[i] = (char)toupper(exp_ptr[i]);
            uptree[i] = '\0';
            shot = (shotid == 0) ? TreeGetCurrentShotId(uptree) : shotid;
            status = TreeNOT_OPEN;
            for (prev_db = 0, db = (*dblist); db ? db->open : 0; prev_db = db, db = db->next) {
                if ((shot == db->shotid) && (strcmp(uptree, db->main_treenam) == 0)) {
                    if (prev_db) {
                        prev_db->next = db->next;
                        db->next = (*dblist);
                        *dblist = db;
                    }
                    status = IS_OPEN_FOR_EDIT(*dblist) ? TreeNORMAL : TreeNOT_OPEN;
                    break;
                }
            }
        } else
            status = IS_OPEN_FOR_EDIT(*dblist) ? TreeNORMAL : TreeNOT_OPEN;
        if STATUS_OK {
            /**************************************
            Compute number of pages to allocate for
            each part of the tree file.
            Create a file of the correct size then
            write out each part of the tree file.
            Close the file.
            Write out the characteristics file.
            **************************************/
            int ntreefd;
            TREE_INFO *info_ptr = (*dblist)->tree_info;
            char *nfilenam = strcpy(malloc(strlen(info_ptr->filespec) + 2), info_ptr->filespec);
	    _TreeDeleteNodesWrite(*dbid);
            trim_excess_nodes(info_ptr);
            header_pages = (sizeof(TREE_HEADER) + 511u) / 512u;
            node_pages = ((size_t)info_ptr->header->nodes * sizeof(NODE) + 511u) / 512u;
            tags_pages = ((size_t)info_ptr->header->tags * 4u + 511u) / 512u;
            tag_info_pages = ((size_t)info_ptr->header->tags * sizeof(TAG_INFO) + 511u) / 512u;
            external_pages = ((size_t)info_ptr->header->externals * 4u + 511u) / 512u;
            strcat(nfilenam, "#");
            ntreefd = MDS_IO_OPEN(nfilenam, O_WRONLY | O_CREAT | O_TRUNC, 0664);
            if (ntreefd != -1) {
                status = MDSplusERROR;
                ssize_t num;
                TREE_HEADER *header = HeaderOut(info_ptr->header);
                num = MDS_IO_WRITE(ntreefd, header, 512 * header_pages);
                FreeHeaderOut(header);
                status = TreeWRITETREEERR;
                if (num != (ssize_t)(header_pages * 512)) {
                    goto error_exit;
                }
                num = MDS_IO_WRITE(ntreefd, info_ptr->node, 512 * node_pages);
                if (num != (ssize_t)(node_pages * 512))
                    goto error_exit;
                num = MDS_IO_WRITE(ntreefd, info_ptr->tags, 512 * tags_pages);
                if (num != (ssize_t)(tags_pages * 512))
                    goto error_exit;
                num = MDS_IO_WRITE(ntreefd, info_ptr->tag_info, 512 * tag_info_pages);
                if (num != (ssize_t)(tag_info_pages * 512))
                    goto error_exit;
                num = MDS_IO_WRITE(ntreefd, info_ptr->external, 512 * external_pages);
                if (num != (ssize_t)(external_pages * 512))
                    goto error_exit;
                status = TreeWriteNci(info_ptr);
                if STATUS_NOT_OK
                    goto error_exit;
                status = TreeNORMAL;
                if (info_ptr->channel > -1)
                    status = MDS_IO_CLOSE(info_ptr->channel);
                info_ptr->channel = -2;
                MDS_IO_REMOVE(info_ptr->filespec);
                if (MDS_IO_CLOSE(ntreefd) == -1) {
                    status = TreeCLOSEERR;
                    goto error_exit;
                }
                if (MDS_IO_RENAME(nfilenam, info_ptr->filespec) == -1) {
                    status = TreeMOVEERROR;
                    goto error_exit;
                }
                info_ptr->channel = MDS_IO_OPEN(info_ptr->filespec, O_RDWR, 0);
                if (info_ptr->channel == -1) {
                    status = TreeOPENEDITERR;
                    goto error_exit;
                }
                MDS_IO_LOCK(info_ptr->channel, 1, 1, MDS_IO_LOCK_RD | MDS_IO_LOCK_NOWAIT, 0);
                status = TreeNORMAL;
                (*dblist)->modified = 0;
                TreeCallHook(WriteTree, info_ptr, 0);
            } else {
                (*dblist)->modified = 0;
                status = TreeFCREATE;
            }
error_exit:
            if (nfilenam)
                free(nfilenam);
        }
    }
    return status;
}

STATIC_ROUTINE void trim_excess_nodes(TREE_INFO * info_ptr)
{
  int *nodecount_ptr = (int *)&info_ptr->header->nodes;
  int *free_ptr = (int *)&info_ptr->header->free;
  NODE *node_ptr;
  NODE *nodes_ptr = info_ptr->node;
  NODE *last_node_ptr = nodes_ptr + *nodecount_ptr - 1;
  int nodes;
  size_t node_pages;
  size_t length = sizeof(node_ptr->name);
  for (node_ptr = last_node_ptr;
       strncmp((const char *)node_ptr->name, (const char *)empty_node.name, length) == 0;
       node_ptr--) ;
  node_pages = (((size_t)(node_ptr - nodes_ptr + 1) * sizeof(NODE) + 511) / 512);
  nodes = max(info_ptr->edit->first_in_mem, (int)((node_pages * 512) / sizeof(NODE)));
  if (nodes < *nodecount_ptr) {
    for (node_ptr = &nodes_ptr[nodes]; (*free_ptr != -1) && (node_ptr <= last_node_ptr); node_ptr++) {
      if (node_ptr == (NODE *) ((char *)nodes_ptr + *free_ptr)) {
	if (node_ptr->parent) {
	  *free_ptr += swapint((char *)&node_ptr->parent);
	  (parent_of(0, node_ptr))->child = 0;
	} else
	  *free_ptr = -1;
      } else {
	NODE *p = parent_of(0, node_ptr);
	NODE *c = child_of(0, node_ptr);
	if (p) {
	  p->child = node_offset(c, p);
	}
	if (c) {
	  c->parent = node_offset(p, c);
	}
      }
    }
    *nodecount_ptr = nodes;
  }
}

STATIC_ROUTINE int TreeWriteNci(TREE_INFO * info)
{
  INIT_STATUS_AS TreeNORMAL;
  if (info->header->nodes > info->edit->first_in_mem) {
    int numnodes = info->header->nodes - info->edit->first_in_mem;
    int i;
    NCI nci;
    char nci_bytes[42];
    int nbytes = (int)sizeof(nci_bytes);
    int offset;
    for (i = 0, offset = info->edit->first_in_mem * nbytes; i < numnodes && STATUS_OK;
	 i++, offset += nbytes) {
      MDS_IO_LSEEK(info->nci_file->put, offset, SEEK_SET);
      memcpy(&nci, &info->edit->nci[i], sizeof(nci));
      TreeSerializeNciOut(&nci, nci_bytes);
      status =
	  (MDS_IO_WRITE(info->nci_file->put, nci_bytes, (size_t)nbytes) ==
	   nbytes) ? TreeNORMAL : TreeNCIWRITE;
      if STATUS_OK
	info->edit->first_in_mem++;
    }
  }
  return status;
}

int _TreeSetSubtree(void *dbid, int nid)
{
  INIT_STATUS_AS TreeNORMAL;
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid_ptr = (NID *) & nid;
  NODE *node_ptr;
  int node_idx;
  int numext;
  int pages_needed;
  int pages_allocated;
  int i;
  int *new_external_ptr;

/**************************************************
 First we must check to make sure we are editting
 a valid tree and that the node we are going to make
 into a subtree reference does not already have any
 children.
**************************************************/

  if (!(IS_OPEN_FOR_EDIT(dblist)))
    return TreeNOEDIT;

  node_ptr = nid_to_node(dblist, nid_ptr);
  if (node_ptr->child != 0)
    return TreeNOTCHILDLESS;
  if (node_ptr->member != 0)
    return TreeNOTMEMBERLESS;
  if (!(TreeIsChild(dblist, node_ptr) & 1))
    return TreeNOTSON;

/***************************************************
 Check to see if this node is already marked as a
 subtree reference.   If so just check the usage.
***************************************************/

  node_idx = (int)(node_ptr - dblist->tree_info->node);
  for (i = 0; i < dblist->tree_info->header->externals; i++) {
    if (swapint((char *)&dblist->tree_info->external[i]) == node_idx) {
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
  if (pages_needed > pages_allocated) {
    new_external_ptr = malloc((size_t)pages_needed * 512u);
    status = new_external_ptr == 0 ? TreeMEMERR : TreeNORMAL;
    if STATUS_NOT_OK {
      return status;
    }
    memcpy(new_external_ptr, dblist->tree_info->external, (size_t)((numext - 1) * 4));
    if (dblist->tree_info->edit->external_pages > 0)
      free(dblist->tree_info->external);
    dblist->tree_info->edit->external_pages = pages_needed;
    dblist->tree_info->external = new_external_ptr;
  }

/*****************************************************
 Finally, load the node index into the externals list
 and increment the number of externals.
*****************************************************/

  *(dblist->tree_info->external + numext - 1) = swapint((char *)&node_idx);
  dblist->tree_info->header->externals++;
  dblist->modified = 1;

/******************************************
  Set the usage of this node to be SUBTREE
*******************************************/
  node_ptr->usage = TreeUSAGE_SUBTREE;

  return TreeNORMAL;
}

int _TreeSetNoSubtree(void *dbid, int nid)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid_ptr = (NID *) & nid;
  NODE *node_ptr;
  int node_idx;
  int ext_idx;
  int i;
/*****************************************************
  Make sure that the tree is open and OK and editable
*****************************************************/
  if (!(IS_OPEN_FOR_EDIT(dblist)))
    return TreeNOEDIT;

/************************************
 Find the node in the externals list.
*************************************/

  node_ptr = nid_to_node(dblist, nid_ptr);
  node_idx = (int)(node_ptr - dblist->tree_info->node);
  for (ext_idx = 0; ext_idx < dblist->tree_info->header->externals; ext_idx++)
    if (swapint((char *)&dblist->tree_info->external[ext_idx]) == node_idx)
      break;
  if (ext_idx >= dblist->tree_info->header->externals)
    return TreeNORMAL;

/**************************************
 Decrement the number of externals and
 fill up the hole left by this removal.
 *************************************/

  dblist->tree_info->header->externals--;
  for (i = ext_idx; i < dblist->tree_info->header->externals; i++)
    *(dblist->tree_info->external + i) = swapint((char *)&dblist->tree_info->external[i + 1]);
  dblist->modified = 1;

/*******************************
  Set this node usage STRUCTURE
********************************/
  node_ptr->usage = TreeUSAGE_STRUCTURE;

  return TreeNORMAL;
}

int _TreeQuitTree(void **dbid, char const *exp_ptr, int shotid)
{
  INIT_STATUS_AS TreeNOT_OPEN;
  PINO_DATABASE **dblist = (PINO_DATABASE **) dbid;
  PINO_DATABASE *db;
  PINO_DATABASE *prev_db;
  if (*dblist) {
    if (exp_ptr) {
      char uptree[13];
      size_t i;
      int shot;
      size_t len = strlen(exp_ptr);
      for (i = 0; i < 12 && i < len; i++)
	uptree[i] = (char)toupper(exp_ptr[i]);
      uptree[i] = '\0';
      shot = (shotid == 0) ? TreeGetCurrentShotId(uptree) : shotid;
      status = TreeNOT_OPEN;
      for (prev_db = 0, db = (*dblist); db ? db->open : 0; prev_db = db, db = db->next) {
	if ((shot == db->shotid) && (strcmp(uptree, db->main_treenam) == 0)) {
	  if (prev_db) {
	    prev_db->next = db->next;
	    db->next = (*dblist);
	    *dblist = db;
	  }
	  status = (*dblist)->open_for_edit ? TreeNORMAL : TreeNOT_OPEN;
	  break;
	}
      }
    } else
      status = (*dblist)->open_for_edit ? TreeNORMAL : TreeNOT_OPEN;
    if (STATUS_OK && ((*dblist)->open)) {
      _TreeDeleteNodesDiscard(*dbid);
      (*dblist)->modified = 0;
      status = _TreeClose(dbid, 0, 0);
    }
  }
  return status;
}
