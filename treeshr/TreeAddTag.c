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

		Name: TREE$ADD_TAG

		Type:   C function

		Author:	Thomas W. Fredian
			MIT Plasma Fusion Center

		Date:   28-DEC-1987

		Purpose: Insert a tag into a tree file

------------------------------------------------------------------------------

	Call sequence: status = TREE$ADD_TAG(nid_ptr,tagnam_ptr)

------------------------------------------------------------------------------
   Copyright (c) 1987
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

+-----------------------------------------------------------------------------*/
#include <STATICdef.h>
#include <stdlib.h>
#include <string.h>
#include <mdsdescrip.h>
#include <treeshr.h>
#include "treeshrp.h"
#include <ctype.h>

extern void **TreeCtx();

#ifdef min
#undef min
#endif
#define min(a,b)  ( (a) <= (b) ? (a) : (b) )
#ifdef max
#undef max
#endif
#define max(a,b)  ( (a) >= (b) ? (a) : (b) )

int TreeAddTag(int nid, char const *tagnam)
{
  return _TreeAddTag(*TreeCtx(), nid, tagnam);
}

int _TreeAddTag(void *dbid, int nid_in, char const *tagnam)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid_ptr = (NID *) & nid_in;
  int status;
  NODE *node_ptr;
  int tags;
  int pages_needed;
  int pages_allocated;
  int *new_tags_ptr;
  int *old_tags_ptr;
  int tidx;
  int newtag_idx;
  TAG_INFO *new_tag_info_ptr;
  TAG_INFO tag_info;
  size_t len;
  char tag[24];
  size_t i;
  int tmp;

/************************************************
First we make sure tree is open for editting and
the tag name specified does not already exist.
************************************************/

  status = IS_OPEN_FOR_EDIT(dblist) ? TreeNORMAL : TreeNOEDIT;
  if (!(status & 1))
    return status;

  len = strlen(tagnam);
  if (len < 1 || len > 24)
    return TreeTAGNAMLEN;

  for (i = 0; i < len; i++)
    tag[i] = (char)toupper(tagnam[i]);
  for (i = len; i < 24; i++)
    tag[i] = ' ';

  /* Check to make sure tagname is legal (starts with alpha followed by zero or
     more alphanumerics or underscores.
   */

  if ((tag[0] < 'A') || (tag[0] > 'Z'))
    return TreeINVTAG;
  for (i = 1; i < len; i++) {
    if (((tag[i] < 'A') || (tag[i] > 'Z')) && ((tag[i] < '0') || (tag[i] > '9')) && (tag[i] != '_'))
      return TreeINVTAG;
  }

  node_ptr = nid_to_node(dblist, nid_ptr);

/************************************************
 We must determine where to insert the new tag
 in the alphabetically sorted tag index list.
***************************************************/

  tags = dblist->tree_info->header->tags;
  old_tags_ptr = dblist->tree_info->tags;
  if (tags > 0) {
    newtag_idx = 0;
    for (tidx = 0; tidx < tags; tidx++) {
      int idx = swapint((char *)(old_tags_ptr + tidx));
      char *defined_tag = (char *)(dblist->tree_info->tag_info + idx)->name;
      int cmp = strncmp(tag, defined_tag, sizeof(TAG_NAME));
      if (cmp == 0)
	return TreeDUPTAG;
      else if (cmp < 0) {
	newtag_idx = tidx;
	break;
      }
    }
    if (newtag_idx == 0)
      newtag_idx = tidx;
  } else
    newtag_idx = 0;

/*******************************************************
 Next we will determine if we need to allocate more space
 for the new tag index. Tag index space is allocated in
 even 512 byte chunks.
********************************************************/

  pages_needed = ((tags + 1) * 4 + 511) / 512;
  pages_allocated = max((tags * 4 + 511) / 512, dblist->tree_info->edit->tags_pages);
  if (pages_needed > pages_allocated) {

  /********************************************************
   If we need to allocate more pages for tag indices we
   get more memory then copy the tag indices that will
   precede the new one to the new memory, then copy the
   trailing indices to the new memory leaving room for
   the new tag and then finally add the new index to
   the table. The new index will be the total tag count
   since the tag information block is simply appended
   to the end of the existing tag information blocks.
  ********************************************************/

    new_tags_ptr = calloc(512,(size_t)pages_needed);
    if (!(new_tags_ptr)) {
      return TreeMEMERR;
    }
    memcpy(new_tags_ptr, old_tags_ptr, (size_t)newtag_idx * sizeof(int));
    memcpy(new_tags_ptr + newtag_idx + 1, old_tags_ptr + newtag_idx,
	   (size_t)(tags - newtag_idx) * sizeof(int));
    *(new_tags_ptr + newtag_idx) = swapint((char *)&tags);
    if (dblist->tree_info->edit->tags_pages > 0)
      free(old_tags_ptr);
    dblist->tree_info->tags = new_tags_ptr;
    dblist->tree_info->edit->tags_pages = pages_needed;
  } else {

  /********************************************************
   If we don't need to allocate more pages for tag indices we
   copy only the trailing indices to the existing table
   leaving room for the new tag and then add the new index to
   the table. The new index will be the total tag count
   since the tag information block is simply appended
   to the end of the existing tag information blocks.
  ********************************************************/

    memmove(old_tags_ptr + newtag_idx + 1, old_tags_ptr + newtag_idx,
	    (size_t)(tags - newtag_idx) * sizeof(int));
    *(old_tags_ptr + newtag_idx) = swapint((char *)&tags);	/* Load new */
  }

/*********************************************
 Next we will fill in a tag information block
 for the new tag.
*********************************************/

  memcpy(tag_info.name, tag, sizeof(tag));
  tmp = (int)(node_ptr - dblist->tree_info->node);
  tag_info.node_idx = swapint((char *)&tmp);
  tag_info.tag_link = node_ptr->tag_link;

/*******************************************************
 Next we will determine if we need to allocate more space
 for the new tag information block. Tag information space
 is allocated in multiples of 512 byte chunks.
********************************************************/

  pages_needed = ((tags + 1) * (int)sizeof(TAG_INFO) + 511) / 512;
  pages_allocated = max((tags * (int)sizeof(TAG_INFO) + 511) / 512,
			dblist->tree_info->edit->tag_info_pages);
  if (pages_needed > pages_allocated) {

  /*******************************************************
   If we need to get more memory we should get more than
   we need so we don't need to expand as often.

   Once we get the memory we will copy the existing tag
   information blocks into the new memory and then append
   the new one to the end.
  *******************************************************/

    pages_needed = pages_needed + 31;
    new_tag_info_ptr = calloc(512,(size_t)pages_needed);
    if (!new_tag_info_ptr)
      return TreeMEMERR;

    memcpy(new_tag_info_ptr, dblist->tree_info->tag_info, (size_t)tags * sizeof(TAG_INFO));
    *(new_tag_info_ptr + tags) = tag_info;	/* Load new */
    if (dblist->tree_info->edit->tag_info_pages > 0)
      free(dblist->tree_info->tag_info);
    dblist->tree_info->tag_info = new_tag_info_ptr;
    dblist->tree_info->edit->tag_info_pages = pages_needed;
  } else {

  /***************************************************
   If we don't have to expand, just add the new tag
   information block at the end.
  ***************************************************/

    *(dblist->tree_info->tag_info + tags) = tag_info;	/* Load new */
  }

/******************************************
 To finish up we need to load the tag index
 into the node's tag link and increment the
 number of defined tags in the tree.
******************************************/

  tags++;
  node_ptr->tag_link = swapint((char *)&tags);
  dblist->tree_info->header->tags = tags;
  dblist->modified = 1;
  return TreeNORMAL;

}
