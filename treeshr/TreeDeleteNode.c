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



#include <treeshr.h>
#include "treeshrp.h"
#include <string.h>
#include <stdlib.h>

extern void *DBID;

static unsigned char *TREE_DELETE_LIST = 0;

static void check_nid(PINO_DATABASE *dblist, NID *nid, int *count);

int TreeDeleteNodeGetNid(int *nid)
{
  return _TreeDeleteNodeGetNid(DBID, nid);
}

int TreeDeleteNodeInitialize(int nid, int *count, int reset)
{
  return(_TreeDeleteNodeInitialize(DBID, nid, count, reset));
}

void TreeDeleteNodeExecute()
{
  _TreeDeleteNodeExecute(DBID);
}
  

int _TreeDeleteNodeInitialize(void *dbid, int nidin, int *count, int reset)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID *nid = (NID *)&nidin;
  static int list_vm = 0;
  int       vm_needed;
  int       status;
  if (!IS_OPEN_FOR_EDIT(dblist))
    return TreeNOEDIT;
  vm_needed = dblist->tree_info->header->nodes / 8 + 4;
  if (vm_needed != list_vm)
  {
    unsigned char *old_list = TREE_DELETE_LIST;
    TREE_DELETE_LIST = malloc(vm_needed);
    if (!TREE_DELETE_LIST)
      return TreeFAILURE;
    if (reset)
    {
      memset(TREE_DELETE_LIST,0,vm_needed);
      if (count)
	*count = 0;
    }
    else
    {
      memcpy(TREE_DELETE_LIST,old_list,(list_vm < vm_needed) ? list_vm : vm_needed);
      if (vm_needed > list_vm)
        memset(TREE_DELETE_LIST+list_vm,0,vm_needed-list_vm);
    }
    if (list_vm)
      free(list_vm);
    list_vm = vm_needed;
  }
  else if (reset)
  {
    memset(TREE_DELETE_LIST,0,list_vm);
    if (count)
      *count = 0;
  }
  if (nid)
    check_nid(dblist, nid, count);
  return 1;
}

static int getbit(int bitnum)
{
  return TREE_DELETE_LIST[bitnum/8] & (1 << (bitnum % 8));
}

static void setbit(int bitnum)
{
  TREE_DELETE_LIST[bitnum/8] |= (1 << (bitnum % 8));
}

static void check_nid(PINO_DATABASE *dblist, NID *nid, int *count)
{
  int       bitnum = nid->node;
  if (!getbit(bitnum))
  {
    NODE     *node;
    NODE     *descendent;
    nid_to_node(dblist, nid, node);
    if (count)
      (*count)++;
    setbit(bitnum);
    for (descendent = member_of(node); descendent; descendent = brother_of(descendent))
    {
      NID       nid;
      node_to_nid(dblist, descendent, (&nid));
      check_nid(dblist, &nid, count);
    }
    for (descendent = child_of(node); descendent; descendent = brother_of(descendent))
    {
      NID       nid;
      node_to_nid(dblist, descendent, (&nid));
      check_nid(dblist, &nid, count);
    }
    if (node->conglomerate_elt)
    {
      NID       elt_nid;
      NODE     *elt_node;
      unsigned short elt_num = 1;
      elt_nid.node = nid->node - node->conglomerate_elt + 1;
      elt_nid.tree = nid->tree;
      nid_to_node(dblist, (&elt_nid), elt_node);
      for (; elt_node->conglomerate_elt == elt_num; elt_nid.node++, elt_num++, elt_node++)
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

extern void       _TreeDeleteNodeExecute(void *dbid)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  static NID       nid = {0, 0};
  NODE     *node;
  NODE     *prevnode = 0;
  NODE     *parent;
  static NCI empty_nci;
  NODE     *firstempty = (dblist->tree_info->header->free == -1) ? (NODE *) 0 :
	  (NODE *) ((char *) dblist->tree_info->node + dblist->tree_info->header->free);

  TREE_EDIT *edit = dblist->tree_info->edit;
  static int sizeof_nci = sizeof(NCI);
  static int sizeof_nodename = sizeof(node->name);
/*------------------------------------------------------------------------------

 Executable:                                                                  */

  nid.node = 0;
  while (_TreeDeleteNodeGetNid(dbid, (int*)&nid) & 1)
  {
    int       found = 0;
    _TreeRemoveNodesTags(dbid, *(int *)&nid);
    _TreeSetNoSubtree(dbid, *(int *)&nid);
    nid_to_node(dblist, (&nid), node);
    parent = parent_of(node);
    if (child_of(parent) == node)
    {
      found = 1;
      if (node->brother)
      {
        link_it(parent->child,brother_of(node),parent);
      }
      else
	parent->child = 0;
    }
    else if (parent->child)
    {
      NODE     *bro;
      for (bro = child_of(parent); bro->brother && (brother_of(bro) != node); bro = brother_of(bro));
      if (brother_of(bro) == node)
      {
	found = 1;
	if (node->brother)
	{
          link_it(bro->brother,brother_of(node),bro);
        }
	else
	  bro->brother = 0;
      }
    }
    if (!found)
    {
      if (member_of(parent) == node)
      {
	if (node->brother)
	{
	  link_it(parent->member,brother_of(node), parent);
        }
	else
	  parent->member = 0;
      }
      else if (parent->member)
      {
	NODE     *bro;
	for (bro = member_of(parent); bro->brother && (brother_of(bro) != node); bro = brother_of(bro));
	if (brother_of(bro) == node)
	{
	  found = 1;
	  if (node->brother)
	  {
	    link_it(bro->brother,brother_of(node), bro);
          }
	  else
	    bro->brother = 0;
	}
      }
    }
    if (nid.node < edit->first_in_mem)
    {
      NCI       old_nci;
      int       nidx = nid.node;
      TreeGetNciLw(dblist->tree_info, nidx, &old_nci);
      TreePutNci(dblist->tree_info, nidx, &empty_nci, 1);
    }
    else
      memcpy(edit->nci + nid.node - edit->first_in_mem, &empty_nci, sizeof(struct nci));
    memcpy(node->name,"deleted node",sizeof(node->name));
    node->conglomerate_elt = 0;
    node->member = 0;
    node->brother = 0;
    node->usage = 0;
    if (prevnode)
    {
      link_it(prevnode->parent, node, prevnode);
      node->child = -prevnode->parent;
    }
    else
    {
      link_it(dblist->tree_info->header->free,node, dblist->tree_info->node);
      node->child = 0;
    }
    if (firstempty)
    {
      link_it(node->parent, firstempty, node);
    }
    else
      node->parent = 0;
    if (firstempty)
      firstempty->child = -node->parent;
    prevnode = node;
  }
  dblist->modified = 1;
  _TreeDeleteNodeInitialize(dbid, 0, 0, 1);
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

int       _TreeDeleteNodeGetNid(void *dbid, int *innid)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID       *nid = (NID *)innid;
  int       i;
  int       status = TreeNORMAL;
  int       found;
  for (i = nid->node + 1;i<dblist->tree_info->header->nodes && ((found = getbit(i)) == 0);i++);
  if (found)
    nid->node = i;
  else
    status = TreeNMN;
  return status;
}
