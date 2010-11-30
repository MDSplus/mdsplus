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


STATIC_CONSTANT char *cvsrev = "@(#)$RCSfile$ $Revision$ $Date$";

extern void **TreeCtx();

static unsigned char *TREE_DELETE_LIST = 0;

STATIC_ROUTINE void check_nid(PINO_DATABASE *dblist, NID *nid, int *count);

int TreeDeleteNodeGetNid(int *nid)
{
  return _TreeDeleteNodeGetNid(*TreeCtx(), nid);
}

int TreeDeleteNodeInitialize(int nid, int *count, int reset)
{
  return(_TreeDeleteNodeInitialize(*TreeCtx(), nid, count, reset));
}

void TreeDeleteNodeExecute()
{
  _TreeDeleteNodeExecute(*TreeCtx());
}
  

int _TreeDeleteNodeInitialize(void *dbid, int nidin, int *count, int reset)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID *nid = (NID *)&nidin;
  static int list_vm = 0;
  int       vm_needed;
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
      free(old_list);
    list_vm = vm_needed;
  }
  else if (reset)
  {
    memset(TREE_DELETE_LIST,0,list_vm);
    if (count)
      *count = 0;
  }
  if (nidin)
    check_nid(dblist, nid, count);
  return TreeNORMAL;
}

STATIC_ROUTINE int getbit(int bitnum)
{
  return TREE_DELETE_LIST[bitnum/8] & (1 << (bitnum % 8));
}

STATIC_ROUTINE void setbit(int bitnum)
{
  TREE_DELETE_LIST[bitnum/8] |= (1 << (bitnum % 8));
}

STATIC_ROUTINE void check_nid(PINO_DATABASE *dblist, NID *nid, int *count)
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
    if (swapshort((char *)&node->conglomerate_elt))
    {
      NID       elt_nid;
      NODE     *elt_node;
      unsigned short elt_num = 1;
      elt_nid.node = nid->node - swapshort((char *)&node->conglomerate_elt) + 1;
      elt_nid.tree = nid->tree;
      nid_to_node(dblist, (&elt_nid), elt_node);
      for (; swapshort((char *)&elt_node->conglomerate_elt) == elt_num; elt_nid.node++, elt_num++, elt_node++)
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
  static NID       nid;
  NODE     *node;
  NODE     *prevnode = 0;
  NODE     *parent;
  static NCI empty_nci;
  NODE     *firstempty = (dblist->tree_info->header->free == -1) ? (NODE *) 0 :
	  (NODE *) ((char *) dblist->tree_info->node + dblist->tree_info->header->free);

  TREE_EDIT *edit = dblist->tree_info->edit;
  static int zero = 0;
/*------------------------------------------------------------------------------

 Executable:                                                                  */

  nid.tree = 0;
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
      if (node->INFO.TREE_INFO.brother)
      {
        link_it(parent->INFO.TREE_INFO.child,brother_of(node),parent);
      }
      else
	parent->INFO.TREE_INFO.child = 0;
    }
    else if (parent->INFO.TREE_INFO.child)
    {
      NODE     *bro;
      for (bro = child_of(parent); bro->INFO.TREE_INFO.brother && (brother_of(bro) != node); bro = brother_of(bro));
      if (brother_of(bro) == node)
      {
	found = 1;
	if (node->INFO.TREE_INFO.brother)
	{
          link_it(bro->INFO.TREE_INFO.brother,brother_of(node),bro);
        }
	else
	  bro->INFO.TREE_INFO.brother = 0;
      }
    }
    if (!found)
    {
      if (member_of(parent) == node)
      {
	if (node->INFO.TREE_INFO.brother)
	{
	  link_it(parent->INFO.TREE_INFO.member,brother_of(node), parent);
        }
	else
	  parent->INFO.TREE_INFO.member = 0;
      }
      else if (parent->INFO.TREE_INFO.member)
      {
	NODE     *bro;
	for (bro = member_of(parent); bro->INFO.TREE_INFO.brother && (brother_of(bro) != node); bro = brother_of(bro));
	if (brother_of(bro) == node)
	{
	  found = 1;
	  if (node->INFO.TREE_INFO.brother)
	  {
	    link_it(bro->INFO.TREE_INFO.brother,brother_of(node), bro);
          }
	  else
	    bro->INFO.TREE_INFO.brother = 0;
	}
      }
    }
    if ((int)nid.node < edit->first_in_mem)
    {
      NCI       old_nci;
      int       nidx = nid.node;
      TreeGetNciLw(dblist->tree_info, nidx, &old_nci);
      TreePutNci(dblist->tree_info, nidx, &empty_nci, 1);
    }
    else
      memcpy(edit->nci + nid.node - edit->first_in_mem, &empty_nci, sizeof(struct nci));
    memcpy(node->name,"deleted node",sizeof(node->name));
    LoadShort(zero,&node->conglomerate_elt);
    node->INFO.TREE_INFO.member = 0;
    node->INFO.TREE_INFO.brother = 0;
    node->usage = 0;
    if (prevnode)
    {
      int tmp;
      link_it(prevnode->parent, node, prevnode);
      tmp = -swapint((char *)&prevnode->parent);
      node->INFO.TREE_INFO.child = swapint((char *)&tmp);
    }
    else
    {
      int tmp;
      link_it(tmp,node, dblist->tree_info->node);
      dblist->tree_info->header->free = swapint((char *)&tmp);
      node->INFO.TREE_INFO.child = 0;
    }
    if (firstempty)
    {
      int tmp;
      link_it(node->parent, firstempty, node);
      tmp = -swapint((char *)&node->parent);
      firstempty->INFO.TREE_INFO.child = swapint((char *)&tmp);
    }
    else
      node->parent = 0;
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
  int       found=0;
  if (TREE_DELETE_LIST != NULL)
    for (i = nid->node + 1;i<dblist->tree_info->header->nodes && ((found = getbit(i)) == 0);i++);
  if (found)
    nid->node = i;
  else
    status = TreeNMN;
  return status;
}
