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

#include <mdsdescrip.h>
#include "treeshrp.h"
#include <stdio.h>

static int maxnodes;
static int nodecount;
static int countnodes(NODE *node);
static int countfree(NODE *node);

extern void *DBID;

int TreeVerify()
{
  return _TreeVerify(DBID);
}

int _TreeVerify(void *dbid)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NODE     *firstempty = (dblist->tree_info->header->free == -1) ? (NODE *) 0 :
		   (NODE *) ((char *) dblist->tree_info->node + 
			     dblist->tree_info->header->free);
  NODE     *node;
  nodecount = 0;
  maxnodes = dblist->tree_info->header->nodes;
  if (countnodes(dblist->tree_info->node))
  {
    int       allocated = nodecount;
    printf("Node summary:\n");
    printf("  Allocated = %d/%d\n", nodecount, maxnodes);
    if (countfree(firstempty))
    {
      int       free = nodecount - allocated;
      int       other = maxnodes - nodecount;
      printf("  Free      = %d/%d\n", free, maxnodes);
      printf("  Other     = %d/%d\n", other, maxnodes);
    }
  }
  return 1;
}

static int countnodes(NODE *node)
{
  if (node)
  {
    nodecount++;
    if (nodecount > maxnodes)
    {
      printf("Too many nodes found - exceeds total nodes %d\n", maxnodes);
      return 0;
    }
    if (node->member)
    {
      if (parent_of(member_of(node)) != node)
	printf("Bad node linkage\n");
      countnodes(member_of(node));
    }
    if (node->child)
    {
      if (parent_of(child_of(node)) != node)
	printf("Bad node linkage\n");
      countnodes(child_of(node));
    }
    if (node->brother)
    {
      if (parent_of(brother_of(node)) != parent_of(node))
	printf("Bad node linkage\n");
      countnodes(brother_of(node));
    }
  }
  return 1;
}

static int countfree(NODE *node)
{
  NODE     *lnode;
  for (lnode = node; lnode; lnode = parent_of(lnode))
  {
    nodecount++;
    if (nodecount > maxnodes)
    {
      printf("Too many nodes found - exceeds total nodes %d\n", maxnodes);
      return 0;
    }
    if (lnode->parent)
      if (child_of(parent_of(lnode)) != lnode)
	printf("Bad node linkage\n");
  }
  return 1;
}
