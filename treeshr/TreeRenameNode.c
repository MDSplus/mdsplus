/*------------------------------------------------------------------------------

		Name: TreeRenameNode

		Type:   C function

		Author:	Josh Stillerman
			MIT Plasma Fusion Center

		Date:   26-FEB-1988

		Purpose: Rename a node in a tree.

------------------------------------------------------------------------------

	Call sequence: status = TreeRenameNode(nid_ptr, newnamedsc_ptr)

------------------------------------------------------------------------------
   Copyright (c) 1987
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

  This routine modifies the name of an existing node in a tree.  It does NOT
  move the node from its location in the tree.
+-----------------------------------------------------------------------------*/

#include <string.h>
#include <stdlib.h>
#include <mdsdescrip.h>
#include <treeshr.h>
#include "treeshrp.h"
#include <usagedef.h>
#include <ncidef.h>

#define __toupper(c) (((c) >= 'a' && (c) <= 'z') ? (c) & 0xDF : (c))

static int FixParentState(PINO_DATABASE *dblist, NODE *parent_ptr, NODE *child_ptr);

extern void *DBID;

int TreeRenameNode(int nid, char *newname)
{
  return _TreeRenameNode(DBID, nid, newname);
}

int       _TreeRenameNode(void *dbid, int nid, char *newname)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID       *nid_ptr = (NID *)&nid;
  NODE     *pptr,
             *nptr,
             *newnode,
             *oldnode_ptr;
  char *newnode_name = 0;
  SEARCH_TYPE newnode_type;
  int       status;
  static char blank = ' ';
  static int name_size = sizeof(oldnode_ptr->name);
  char *upcase_name = malloc(strlen(newname)+1);
  int i;
/*****************************************************
  Make sure that the tree is open and OK and editable
*****************************************************/
  if (!(IS_OPEN_FOR_EDIT(dblist)))
    return TreeNOEDIT;

/**************************
   Convert to upper case.
***************************/
  for (i=0;i<(int)strlen(newname);i++)
  {
      upcase_name[i] = __toupper(newname[i]);
  }
  upcase_name[i] = 0;
/****************************************************
  make sure that the new node is not already there
***************************************************/
  status = TreeFindNode(upcase_name, &i);
  if (status & 1)
    return TreeALREADY_THERE;

/******************************************************
  Make sure the new node's parent is in the tree
******************************************************/
  status = TreeFindParent(dblist, upcase_name, &newnode, &newnode_name, &newnode_type);
  if (!(status & 1))
    return status;
  if ((newnode_type != BROTHER_TYPE_NOWILD) && (newnode_type != MEMBER_TYPE_NOWILD))
  {
    status = TreeINVPATH;
    goto cleanup;
  }
/************************************************
  Make sure that the node being renamed is not
  an ancestor of the destination. (This check
  insures that you are not renameing the node
  off into space.)
************************************************/
  nid_to_node(dblist, (nid_ptr), oldnode_ptr);
  for (nptr = newnode; nptr; nptr = parent_of(nptr))
    if (nptr == oldnode_ptr)
  {
    status = TreeINVPATH;
    goto cleanup;
  }

/************************************************
  Make sure that a node with a non-STRUCTURE usage is
  not being renamed into a son.
 ************************************************/
  if (newnode_type == BROTHER_TYPE_NOWILD)
    if (oldnode_ptr->usage != TreeUSAGE_STRUCTURE)
  {
    status = TreeINVPATH;
    goto cleanup;
  }

/************************************************
 OK so far so disconnect the old node
*************************************************/
  pptr = parent_of(oldnode_ptr);
  if (child_of(pptr) == oldnode_ptr)
    if (oldnode_ptr->brother)
      {link_it(pptr->child,brother_of(oldnode_ptr), pptr);}
    else
      pptr->child = 0;
  else
  {
    for (nptr = child_of(pptr); nptr && (brother_of(nptr) != oldnode_ptr); nptr = brother_of(nptr));
    if (nptr)
      if (oldnode_ptr->brother)
        {link_it(nptr->brother,brother_of(oldnode_ptr), nptr);}
      else
	nptr->brother = 0;
    else if (member_of(pptr) == oldnode_ptr)
      if (oldnode_ptr->brother)
        {link_it(pptr->member,brother_of(oldnode_ptr), pptr);}
      else
	pptr->member = 0;
    else
    {
      for (nptr = member_of(pptr); nptr && (brother_of(nptr) != oldnode_ptr); nptr = brother_of(nptr));
      if (nptr)
	if (oldnode_ptr->brother)
          {link_it(nptr->brother,brother_of(oldnode_ptr), nptr);}
	else
	  nptr->brother = 0;
      else
      {
        status = TreeINVTREE;
        goto cleanup;
      }
    }
  }

/***********************************************
 Next we must connect this node up to its new
 destination.
***********************************************/
  memcpy(oldnode_ptr->name,newnode_name,strlen(newnode_name));
  if (strlen(newnode_name) < sizeof(oldnode_ptr->name))
    memset(oldnode_ptr->name+strlen(newnode_name),32,sizeof(oldnode_ptr->name)-strlen(newnode_name));
  if (newnode_type == BROTHER_TYPE_NOWILD)
    status = TreeInsertChild(newnode, oldnode_ptr, dblist->tree_info->header->sort_children);
  else
    status = TreeInsertMember(newnode, oldnode_ptr, dblist->tree_info->header->sort_members);

  if (status & 1)
    status = FixParentState(dblist, newnode, oldnode_ptr);

  if (status & 1)
    dblist->modified = 1;

 cleanup:

  if (upcase_name) free(upcase_name);
  if (newnode_name) free(newnode_name);
  return status;
}


static int FixParentState(PINO_DATABASE *dblist, NODE *parent_ptr, NODE *child_ptr)
{
  int       status = 1;
  NID       parent_nid;
  NID       child_nid;
  int       parent_state;
  int       child_parent_state;
  static int retlen;
  static unsigned int child_flags;
  static NCI_ITM child_itm_list[] = {{sizeof(unsigned int), NciGET_FLAGS, (unsigned char *) &child_flags, &retlen},
				     {0, NciEND_OF_LIST}};
  node_to_nid(dblist, parent_ptr, (&parent_nid));
  node_to_nid(dblist, child_ptr, (&child_nid));

/***************************************************
  Note that parent_state and child_parent_state
  are normal positive boolean logic but the bits
  in the flag longword and the parent state argument
  to SET_PARENT_STATE are negative boolean logic.
****************************************************/
  parent_state = _TreeIsOn(dblist, *(int *)&parent_nid) & 1;
  TreeGetNci(*(int *)&child_nid, child_itm_list);
  child_parent_state = ((child_flags & NciM_PARENT_STATE) == 0);
  if (child_parent_state != parent_state)
    status = SetParentState(dblist, child_ptr, !parent_state);

  return status;
}
