/*------------------------------------------------------------------------------

		Name:   TreeVerify

		Type:   C function

		Author:	TOM FREDIAN

		Date:   26-FEB-1990

		Purpose: Verify tree consistency

------------------------------------------------------------------------------

	Call sequence:

int TreeVerify( )

------------------------------------------------------------------------------
   Copyright (c) 1990
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------*/
#include <stdio.h>
#include <mdsdescrip.h>
#include <ncidef.h>
#include <treeshr.h>
#include <usagedef.h>
#include "treeshrp.h"

static int maxnodes;
static int nodecount;
static int countnodes(PINO_DATABASE * dblist, NODE * node);
static int countfree(PINO_DATABASE * dblist, NODE * node);

extern void **TreeCtx();

int TreeVerify()
{
  return _TreeVerify(*TreeCtx());
}

int _TreeVerify(void *dbid)
{
  int status;
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  if (dblist && dblist->tree_info) {
    NODE *firstempty = (dblist->tree_info->header->free == -1) ? (NODE *) 0 :
	(NODE *) ((char *)dblist->tree_info->node + dblist->tree_info->header->free);
    nodecount = 0;
    maxnodes = dblist->tree_info->header->nodes;
    if (countnodes(dblist, dblist->tree_info->node)) {
      int allocated = nodecount;
      printf("Node summary:\n");
      printf("  Allocated = %d/%d\n", nodecount, maxnodes);
      if (countfree(dblist, firstempty)) {
	int free = nodecount - allocated;
	int other = maxnodes - nodecount;
	printf("  Free      = %d/%d\n", free, maxnodes);
	printf("  Other     = %d/%d\n", other, maxnodes);
      }
    }
    status = TreeNORMAL;
  } else
    status = TreeNOT_OPEN;
  return status;
}

static int countnodes(PINO_DATABASE * dblist, NODE * node)
{
  if (node) {
    nodecount++;
    if (nodecount > maxnodes) {
      printf("Too many nodes found - exceeds total nodes %d\n", maxnodes);
      return 0;
    }
    switch (node->usage) {
    case TreeUSAGE_SUBTREE_REF:
      break;
    case TreeUSAGE_SUBTREE_TOP:
      break;
    default:
      if (member_of(node)) {
	if (parent_of(dblist, member_of(node)) != node)
	  printf("Bad node linkage\n");
	countnodes(dblist, member_of(node));
      }
      if (child_of(dblist, node)) {
	if (parent_of(dblist, child_of(dblist, node)) != node)
	  printf("Bad node linkage\n");
	countnodes(dblist, child_of(dblist, node));
      }
    }
    if (brother_of(dblist, node)) {
      if (parent_of(dblist, brother_of(dblist, node)) != parent_of(dblist, node))
	printf("Bad node linkage\n");
      countnodes(dblist, brother_of(dblist, node));
    }
  }
  return 1;
}

static int countfree(PINO_DATABASE * dblist, NODE * node)
{
  NODE *lnode;
  for (lnode = node; lnode; lnode = parent_of(dblist, lnode)) {
    nodecount++;
    if (nodecount > maxnodes) {
      printf("Too many nodes found - exceeds total nodes %d\n", maxnodes);
      return 0;
    }
    if (lnode->parent)
      if (child_of(dblist, parent_of(dblist, lnode)) != lnode)
	printf("Bad node linkage\n");
  }
  return 1;
}
