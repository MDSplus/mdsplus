/*------------------------------------------------------------------------------

		Name:   TreeDeleteNodeInitialize

		Type:   C function

		Author:	TOM FREDIAN

		Date:    8-JAN-1990

		Purpose: Construct a list of nodes that will be deleted
			 if specified node is deleted.

------------------------------------------------------------------------------

	Call sequence:

int TreeDeleteNodeInitialize(NID *nid,int *count,reset)

------------------------------------------------------------------------------
   Copyright (c) 1990
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------*/
#include <STATICdef.h>
#include <string.h>
#include <stdlib.h>
#include <treeshr.h>
#include "treeshrp.h"

extern void **TreeCtx();

static unsigned char *TREE_DELETE_LIST = 0;

STATIC_ROUTINE void check_nid(PINO_DATABASE * dblist, NID * nid, int *count);

int TreeDeleteNodeGetNid(int *nid)
{
  return _TreeDeleteNodeGetNid(*TreeCtx(), nid);
}

int TreeDeleteNodeInitialize(int nid, int *count, int reset)
{
  return (_TreeDeleteNodeInitialize(*TreeCtx(), nid, count, reset));
}

void TreeDeleteNodeExecute()
{
  _TreeDeleteNodeExecute(*TreeCtx());
}

int _TreeDeleteNodeInitialize(void *dbid, int nidin, int *count, int reset)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid = (NID *) & nidin;
  static int list_vm = 0;
  int vm_needed;
  if (!IS_OPEN_FOR_EDIT(dblist))
    return TreeNOEDIT;
  vm_needed = dblist->tree_info->header->nodes / 8 + 4;
  if (vm_needed != list_vm) {
    unsigned char *old_list = TREE_DELETE_LIST;
    TREE_DELETE_LIST = malloc(vm_needed);
    if (!TREE_DELETE_LIST)
      return TreeMEMERR;
    if (reset) {
      memset(TREE_DELETE_LIST, 0, vm_needed);
      if (count)
	*count = 0;
    } else {
      memcpy(TREE_DELETE_LIST, old_list, (list_vm < vm_needed) ? list_vm : vm_needed);
      if (vm_needed > list_vm)
	memset(TREE_DELETE_LIST + list_vm, 0, vm_needed - list_vm);
    }
    if (list_vm)
      free(old_list);
    list_vm = vm_needed;
  } else if (reset) {
    memset(TREE_DELETE_LIST, 0, list_vm);
    if (count)
      *count = 0;
  }
  if (nidin)
    check_nid(dblist, nid, count);
  return TreeNORMAL;
}

STATIC_ROUTINE int getbit(int bitnum)
{
  return TREE_DELETE_LIST[bitnum / 8] & (1 << (bitnum % 8));
}

STATIC_ROUTINE void setbit(int bitnum)
{
  TREE_DELETE_LIST[bitnum / 8] |= (1 << (bitnum % 8));
}

STATIC_ROUTINE void check_nid(PINO_DATABASE * dblist, NID * nid, int *count)
{
  int bitnum = nid->node;
  if (!getbit(bitnum)) {
    NODE *node;
    NODE *descendent;
    node = nid_to_node(dblist, nid);
    if (count)
      (*count)++;
    setbit(bitnum);
    for (descendent = member_of(node); descendent; descendent = brother_of(0, descendent)) {
      NID nid;
      node_to_nid(dblist, descendent, (&nid));
      check_nid(dblist, &nid, count);
    }
    for (descendent = child_of(0, node); descendent; descendent = brother_of(0, descendent)) {
      NID nid;
      node_to_nid(dblist, descendent, (&nid));
      check_nid(dblist, &nid, count);
    }
    if (swapshort((char *)&node->conglomerate_elt)) {
      NID elt_nid;
      NODE *elt_node;
      unsigned short elt_num = 1;
      elt_nid.node = nid->node - swapshort((char *)&node->conglomerate_elt) + 1;
      elt_nid.tree = nid->tree;
      elt_node = nid_to_node(dblist, &elt_nid);
      for (; swapshort((char *)&elt_node->conglomerate_elt) == elt_num;
	   elt_nid.node++, elt_num++, elt_node++)
	check_nid(dblist, &elt_nid, count);
    }
  }
}

/*------------------------------------------------------------------------------

		Name:   TreeDeleteNodeExecute

		Type:   C function

		Author:	TOM FREDIAN

		Date:    8-JAN-1990

		Purpose: Delete nodes selected for delete

------------------------------------------------------------------------------

	Call sequence:

void TreeDeleteNodeExecute( )

------------------------------------------------------------------------------
   Copyright (c) 1990
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------*/

extern void _TreeDeleteNodeExecute(void *dbid)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  static NID nid;
  NODE *node;
  NODE *parent;
  static NCI empty_nci;

  TREE_EDIT *edit = dblist->tree_info->edit;
  static int zero = 0;
/*------------------------------------------------------------------------------

 Executable:                                                                  */

  nid.tree = 0;
  nid.node = 0;
  while (_TreeDeleteNodeGetNid(dbid, (int *)&nid) & 1) {
    int found = 0;
    _TreeRemoveNodesTags(dbid, *(int *)&nid);
    _TreeSetNoSubtree(dbid, *(int *)&nid);
    node = nid_to_node(dblist, &nid);
    parent = parent_of(0, node);
    if (child_of(0, parent) == node) {
      found = 1;
      if (node->brother) {
	parent->child = node_offset(brother_of(0, node), parent);
      } else
	parent->child = 0;
    } else if (parent->child) {
      NODE *bro;
      for (bro = child_of(0, parent); bro->brother && (brother_of(0, bro) != node);
	   bro = brother_of(0, bro)) ;
      if (brother_of(0, bro) == node) {
	found = 1;
	if (node->brother) {
	  bro->brother = node_offset(brother_of(0, node), bro);
	} else
	  bro->brother = 0;
      }
    }
    if (!found) {
      if (member_of(parent) == node) {
	if (node->brother) {
	  parent->member = node_offset(brother_of(0, node), parent);
	} else
	  parent->member = 0;
      } else if (parent->member) {
	NODE *bro;
	for (bro = member_of(parent); bro->brother && (brother_of(0, bro) != node);
	     bro = brother_of(0, bro)) ;
	if (brother_of(0, bro) == node) {
	  found = 1;
	  if (node->brother) {
	    bro->brother = node_offset(brother_of(0, node), bro);
	  } else
	    bro->brother = 0;
	}
      }
    }
    if ((int)nid.node < edit->first_in_mem) {
      DELETED_NID *dnid = malloc(sizeof(DELETED_NID));
      dnid->next=edit->deleted_nid_list;
      dnid->nid=nid;
      edit->deleted_nid_list=dnid;
    }
    else
      memcpy(edit->nci + nid.node - edit->first_in_mem, &empty_nci, sizeof(struct nci));
    memcpy(node->name, "deleted node", sizeof(node->name));
    LoadShort(zero, &node->conglomerate_elt);
    node->member = 0;
    node->brother = 0;
    node->usage = 0;
  }
  dblist->modified = 1;
  _TreeDeleteNodeInitialize(dbid, 0, 0, 1);
}

void _TreeDeleteNodesWrite(void *dbid) {
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  static NID nid;
  NODE *node;
  NODE *prevnode = 0;
  static NCI empty_nci;
  NODE *firstempty = (dblist->tree_info->header->free == -1) ? (NODE *) 0 :
      (NODE *) ((char *)dblist->tree_info->node + dblist->tree_info->header->free);

  TREE_EDIT *edit = dblist->tree_info->edit;
  DELETED_NID *dnid,*next;
  NCI old_nci;
  int nidx;
  for (dnid=edit->deleted_nid_list,edit->deleted_nid_list=0; dnid; dnid=next) {
    next=dnid->next;
    nid=dnid->nid;
    free(dnid);
    node = nid_to_node(dblist, &nid);
    if (prevnode) {
      int tmp;
      prevnode->parent = node_offset(node, prevnode);
      tmp = -swapint((char *)&prevnode->parent);
      node->child = swapint((char *)&tmp);
    } else {
      int tmp;
      tmp = node_offset(node, dblist->tree_info->node);
      dblist->tree_info->header->free = swapint((char *)&tmp);
      node->child = 0;
    }
    prevnode = node;
    if (firstempty) {
      int tmp;
      node->parent = node_offset(firstempty, node);
      tmp = -swapint((char *)&node->parent);
      firstempty->child = swapint((char *)&tmp);
    } else
      node->parent = 0;
    nidx = nid.node;
    TreeGetNciLw(dblist->tree_info, nidx, &old_nci);
    TreePutNci(dblist->tree_info, nidx, &empty_nci, 1);
    TreeUnLockNci(dblist->tree_info, 0, nidx);
  }
}

/*------------------------------------------------------------------------------

		Name:   TreeDeleteNodeGetNid

		Type:   C function

		Author:	TOM FREDIAN

		Date:    8-JAN-1990

		Purpose: Get next nid in to-be-deleted list

------------------------------------------------------------------------------

	Call sequence:

int TreeDeleteNodeGetNid(NID *nid)

------------------------------------------------------------------------------
   Copyright (c) 1990
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------*/

int _TreeDeleteNodeGetNid(void *dbid, int *innid)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid = (NID *) innid;
  int i;
  int status = TreeNORMAL;
  int found = 0;
  if (TREE_DELETE_LIST != NULL)
    for (i = nid->node + 1; i < dblist->tree_info->header->nodes && ((found = getbit(i)) == 0);
	 i++) ;
  if (found)
    nid->node = i;
  else
    status = TreeNMN;
  return status;
}
