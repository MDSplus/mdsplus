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

#include <mdsplus/mdsconfig.h>
#include <string.h>
#include <stdlib.h>
#include <mdsdescrip.h>
#include <treeshr.h>
#include "treeshrp.h"
#include <usagedef.h>
#include <ncidef.h>
#include <ctype.h>

static int FixParentState(PINO_DATABASE * dblist, NODE * parent_ptr, NODE * child_ptr);

extern void **TreeCtx();

int TreeRenameNode(int nid, char const *newname)
{
  return _TreeRenameNode(*TreeCtx(), nid, newname);
}

int _TreeRenameNode(void *dbid, int nid, char const *newname)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid_ptr = (NID *) & nid;
  NODE *pptr, *nptr, *newnode, *oldnode_ptr;
  char *newnode_name = 0;
  SEARCH_TYPE newnode_type;
  int status;
  char *upcase_name;
  int i;
/*****************************************************
  Make sure that the tree is open and OK and editable
*****************************************************/
  if (!(IS_OPEN_FOR_EDIT(dblist)))
    return TreeNOEDIT;
  upcase_name = strdup(newname);
/**************************
   Convert to upper case.
***************************/
  for (i = 0; i < (int)strlen(newname); i++) {
    upcase_name[i] = (char)toupper(newname[i]);
  }
  upcase_name[i] = 0;
/****************************************************
  make sure that the new node is not already there
***************************************************/
  status = _TreeFindNode(dbid, upcase_name, &i);
  if (status & 1) {
    status = TreeALREADY_THERE;
    goto cleanup;
  }
/******************************************************
  Make sure the new node's parent is in the tree
******************************************************/
  status = TreeFindParent(dblist, upcase_name, &newnode, &newnode_name, &newnode_type);
  if (!(status & 1))
    return status;
  if ((newnode_type != BROTHER_TYPE_NOWILD) && (newnode_type != MEMBER_TYPE_NOWILD)) {
    status = TreeINVPATH;
    goto cleanup;
  }
/************************************************
  Make sure that the node being renamed is not
  an ancestor of the destination. (This check
  insures that you are not renameing the node
  off into space.)
************************************************/
  oldnode_ptr = nid_to_node(dblist, (nid_ptr));
  for (nptr = newnode; nptr; nptr = parent_of(dblist, nptr))
    if (nptr == oldnode_ptr) {
      status = TreeINVPATH;
      goto cleanup;
    }

/************************************************
  Make sure that a node with a non-STRUCTURE usage is
  not being renamed into a son.
 ************************************************/
  if (newnode_type == BROTHER_TYPE_NOWILD)
    if (oldnode_ptr->usage != TreeUSAGE_STRUCTURE) {
      status = TreeINVPATH;
      goto cleanup;
    }

/************************************************
 OK so far so disconnect the old node
*************************************************/
  pptr = parent_of(dblist, oldnode_ptr);
  if (child_of(dblist, pptr) == oldnode_ptr)
    if (oldnode_ptr->brother) {
      pptr->child = node_offset(brother_of(dblist, oldnode_ptr), pptr);
    } else
      pptr->child = 0;
  else {
    for (nptr = child_of(dblist, pptr); nptr && (brother_of(dblist, nptr) != oldnode_ptr);
	 nptr = brother_of(dblist, nptr)) ;
    if (nptr)
      if (oldnode_ptr->brother) {
	nptr->brother = node_offset(brother_of(dblist, oldnode_ptr), nptr);
      } else
	nptr->brother = 0;
    else if (member_of(pptr) == oldnode_ptr)
      if (oldnode_ptr->brother) {
	pptr->member = node_offset(brother_of(dblist, oldnode_ptr), pptr);
      } else
	pptr->member = 0;
    else {
      for (nptr = member_of(pptr); nptr && (brother_of(dblist, nptr) != oldnode_ptr);
	   nptr = brother_of(dblist, nptr)) ;
      if (nptr)
	if (oldnode_ptr->brother) {
	  nptr->brother = node_offset(brother_of(dblist, oldnode_ptr), nptr);
	} else
	  nptr->brother = 0;
      else {
	status = TreeINVTREE;
	goto cleanup;
      }
    }
  }

/***********************************************
 Next we must connect this node up to its new
 destination.
***********************************************/
  memcpy(oldnode_ptr->name, newnode_name, strlen(newnode_name));
  if (strlen(newnode_name) < sizeof(oldnode_ptr->name))
    memset(oldnode_ptr->name + strlen(newnode_name), 32,
	   sizeof(oldnode_ptr->name) - strlen(newnode_name));
  if (newnode_type == BROTHER_TYPE_NOWILD)
    status = TreeInsertChild(newnode, oldnode_ptr, dblist->tree_info->header->sort_children);
  else
    status = TreeInsertMember(newnode, oldnode_ptr, dblist->tree_info->header->sort_members);

  if (status & 1)
    status = FixParentState(dblist, newnode, oldnode_ptr);

  if (status & 1)
    dblist->modified = 1;

 cleanup:

  if (upcase_name)
    free(upcase_name);
  if (newnode_name)
    free(newnode_name);
  return status;
}

static int FixParentState(PINO_DATABASE * dblist, NODE * parent_ptr, NODE * child_ptr)
{
  int status = 1;
  NID parent_nid;
  NID child_nid;
  int parent_state;
  int child_parent_state;
  int retlen;
  unsigned int child_flags;
  NCI_ITM child_itm_list[] =
      { {sizeof(unsigned int), NciGET_FLAGS, &child_flags, &retlen},
	{0, NciEND_OF_LIST, 0, 0}
  };
  node_to_nid(dblist, parent_ptr, (&parent_nid));
  node_to_nid(dblist, child_ptr, (&child_nid));

/***************************************************
  Note that parent_state and child_parent_state
  are normal positive boolean logic but the bits
  in the flag longword and the parent state argument
  to SET_PARENT_STATE are negative boolean logic.
****************************************************/
  parent_state = _TreeIsOn(dblist, *(int *)&parent_nid) & 1;
  status = _TreeGetNci(dblist, *(int *)&child_nid, child_itm_list);
  if (status & 1) {
    child_parent_state = ((child_flags & NciM_PARENT_STATE) == 0);
    if (child_parent_state != parent_state)
      status = SetParentState(dblist, child_ptr, !parent_state);
  }

  return status;
}
