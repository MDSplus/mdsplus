
/*------------------------------------------------------------------------------

		Name: TreeRemoveNodesTags

		Type:   C function

		Author:	Thomas W. Fredian
			MIT Plasma Fusion Center

		Date:   30-DEC-1987

		Purpose: Remove all of the Tags from a node

------------------------------------------------------------------------------

	Call sequence:
	status = TreeRemoveNodesTags(int nid);

------------------------------------------------------------------------------
   Copyright (c) 1987
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:


+-----------------------------------------------------------------------------*/

#include <treeshr.h>
#include "treeshrp.h"
extern void *DBID;

static int       _TreeRemoveNodesTags(void *dbid, int nid);
static void      _RemoveTagIdx(PINO_DATABASE *dblist, int tagidx);

int TreeRemoveTag(char *name) { return _TreeRemoveTag(DBID, name); }
int TreeRemoveNodesTags(int nid) { return _TreeRemoveNodesTags(DBID, nid);}

static int       _TreeRemoveNodesTags(void *dbid, int nid)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NODE *node;
  NID *nid_ptr = (NID *)&nid;
  int tagidx, next_tag;

  int       status;
  
  if (!(IS_OPEN_FOR_EDIT(dblist)))
    return TreeNOEDIT;

/**********************************
 For every tag found for this node
   Remove it from tree
***********************************/

  status = TreeTNF;
  nid_to_node(dblist, nid_ptr, node);
  for (tagidx=node->tag_link; tagidx !=0; tagidx = next_tag) {
    next_tag = dblist->tree_info->tag_info[tagidx-1].tag_link;
    _RemoveTagIdx(dblist, tagidx);
  }
  node->tag_link=0;
  dblist->modified = 1;
  return status;
}

/*
 *  Routine to remove a tag from the tree given its name
 */
static int _TreeRemoveTag(PINO_DATABASE  *dblist, char *name)
{
  int idx;
  int status;
  if ((status = TreeFindTag(name, "\0", &idx))&1)
    _RemoveTagIdx(dblist, idx);
  return status;
}
 
/*
 *  Routine to remove a tag from the tree given its tag index
 */
static void _RemoveTagIdx(PINO_DATABASE  *dblist, int tagidx)
{
  TAG_INFO *info_ptr, *remove_info_ptr;
  NODE     *node_ptr;
  int      *tags_ptr, *this_tags_ptr;

  /****************************************
   First we must remove the linked list of
   tag info blocks connected to the node via
   tag links.
  *****************************************/

    remove_info_ptr = dblist->tree_info->tag_info + tagidx - 1;
    node_ptr = dblist->tree_info->node;
    if (node_ptr->tag_link == tagidx)
      node_ptr->tag_link = remove_info_ptr->tag_link;
    else
    {
      for (info_ptr = dblist->tree_info->tag_info + node_ptr->tag_link - 1;
           (info_ptr->tag_link != tagidx) && (info_ptr->tag_link >= 0);
           info_ptr = dblist->tree_info->tag_info + info_ptr->tag_link - 1);
      if (info_ptr->tag_link == tagidx)
        info_ptr->tag_link = remove_info_ptr->tag_link;
    }

  /*********************************************
   Next we will fix up all the tag links in the
   tree. Those links pointing to tags following
   the tag to be removed will be decremented by
   one. Both node tag links and tag info tag links
   must be corrected.
  ************************************************/

  for (info_ptr = dblist->tree_info->tag_info;
       info_ptr < dblist->tree_info->tag_info + dblist->tree_info->header->tags; 
       info_ptr++)
      if (info_ptr->tag_link >= tagidx)
	info_ptr->tag_link--;
    for (node_ptr = dblist->tree_info->node;
	 node_ptr < dblist->tree_info->node + dblist->tree_info->header->nodes; 
	 node_ptr++)
      if (node_ptr->tag_link >= tagidx)
	node_ptr->tag_link--;

  /*************************************************
    Next we must fix up all the tag index pointers
    much the same way as the tag links above.
  **************************************************/

    for (tags_ptr = dblist->tree_info->tags;
	 tags_ptr < dblist->tree_info->tags + dblist->tree_info->header->tags; 
	 tags_ptr++)
    {
      if (*tags_ptr == (tagidx-1)) this_tags_ptr = tags_ptr;
      if (*tags_ptr >= tagidx)
	(*tags_ptr)--;
    }

  /*******************************************
   Finally we need to move all the tag indexes
   and tag info blocks to fill in the hole left
   by the removed tag and then decrement the
   total tag count.
  ********************************************/
    if (dblist->tree_info->header->tags > 0) {
      int bytes;
      if( (bytes = (dblist->tree_info->header->tags - (this_tags_ptr - dblist->tree_info->tags)) * sizeof(int)) > 0) 
	memcpy(this_tags_ptr, this_tags_ptr+1, bytes);
      if( (bytes = (dblist->tree_info->header->tags - tagidx) * sizeof(TAG_INFO)) > 0) 
	memcpy(&dblist->tree_info->tag_info[tagidx-1], &dblist->tree_info->tag_info[tagidx], bytes);
    }
    dblist->tree_info->header->tags--;
}

