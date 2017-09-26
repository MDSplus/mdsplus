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

		Name: TreeFindTagWild

		Type:   C function

		Author:	Josh Stillerman
			MIT Plasma Fusion Center

		Date:   29-MAR-1991

		Purpose: Find all of the tags matching a wildcard tag specification.

------------------------------------------------------------------------------

	Call sequence:

char *tag = TreeFindTagWild(char *wild, [nid_ptr], void **ctx)
TreeFindTagEnd(void **ctx);
------------------------------------------------------------------------------
   Copyright (c) 1991
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

+-----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <treeshr.h>
#include "treeshrp.h"
#include <strroutines.h>

extern int MdsCopyDxXd();
extern char *FindTagWildRemote();
extern int FindTagEndRemote();

extern void **TreeCtx();
typedef struct tag_search {
  int next_tag;
  TREE_INFO *this_tree_info;
  struct descriptor_d search_tag;
  struct descriptor_d search_tree;
  unsigned char top_match;
  unsigned char remote;
  char *remote_tag;
  int socket;
} TAG_SEARCH;

static TAG_SEARCH *NewTagSearch(char *tagnam_ptr);

static int NextTagTree(PINO_DATABASE * dblist, TAG_SEARCH * ctx);

char *_TreeFindTagWild(void *dbid, char *wild, int *nidout, void **ctx_inout);

char *TreeFindTagWild(char *wild, int *nidout, void **ctx_inout)
{
  return _TreeFindTagWild(*TreeCtx(), wild, nidout, ctx_inout);
}

int TreeFindTagWildDsc(char *wild, int *nidout, void **ctx_inout, struct descriptor_xd *name)
{
  int status;
  char *ans = _TreeFindTagWild(*TreeCtx(), wild, nidout, ctx_inout);
  if (ans) {
    static struct descriptor tag = { 0, DTYPE_T, CLASS_S, 0 };
    tag.length = (unsigned short)strlen(ans);
    tag.pointer = ans;
    MdsCopyDxXd(&tag, name);
    status = 1;
  } else
    status = 0;
  return status;
}

char *_TreeFindTagWild(void *dbid, char *wild, int *nidout, void **ctx_inout)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  /***************************
    check that there is a tree
    open.
   ****************************/
  if (!IS_OPEN(dblist))
    return NULL;
  if (dblist->remote)
    return FindTagWildRemote(dblist, wild, nidout, ctx_inout);
  else {
    NID *nid_ptr = (NID *) nidout;
    TAG_SEARCH **ctx = (TAG_SEARCH **) ctx_inout;
    int status = 1;
    unsigned char found, done;
    static char answer[128];

    /**********************************
    If this is the first time then
    allocate a context block and fill
    it in with a parse of the tagname.
    ***********************************/
    if (*ctx == (TAG_SEARCH *) 0) {
      *ctx = NewTagSearch(wild);
      if (*ctx == (TAG_SEARCH *) 0)
	status = TreeNMT;
      else
	status = NextTagTree(dblist, *ctx);
    }

    /*************************************
     Loop looking for a tag that matches
    **************************************/
    for (found = 0, done = 0; (status & 1) && !found && !done;) {

    /*************************************
      if out of tags in this tree then
      see if there is another one
    **************************************/
      if ((*ctx)->next_tag >= (*ctx)->this_tree_info->header->tags) {
	status = NextTagTree(dblist, *ctx);
	if (status & 1)
	  (*ctx)->next_tag = -1;
	else {
	  done = 1;
	  break;
	}
      } else {

    /**********************************************
      else if this is the first time for this tree
	try to return the \TOP tag.
	otherwise - move on to next tag for next
	time through the loop.
    ***********************************************/
	if ((*ctx)->next_tag == -1) {
	  if ((*ctx)->top_match) {
	    done = 1;
	    found = 1;
	  } else
	    ((*ctx)->next_tag)++;
	} else {

      /****************************************
	 Else
	   loop looking for a tag that matches
      *****************************************/
	  for (; !done && ((*ctx)->next_tag < (*ctx)->this_tree_info->header->tags);) {
	    unsigned short len;
	    static struct descriptor_s s_tag_dsc = { sizeof(TAG_NAME), DTYPE_T, CLASS_S, 0 };
	    static struct descriptor_d tag_dsc = { 0, DTYPE_T, CLASS_D, 0 };
	    s_tag_dsc.pointer =
		(char *)(*ctx)->this_tree_info->tag_info[swapint
							 ((char *)&(*ctx)->this_tree_info->
							  tags[(*ctx)->next_tag])].name;
	    StrTrim((struct descriptor *)&tag_dsc, (struct descriptor *)&s_tag_dsc, &len);
	    if (StrMatchWild((struct descriptor *)&tag_dsc,
			     (struct descriptor *)&((*ctx)->search_tag)) & 1) {
	      done = 1;
	      found = 1;
	    } else
	      ((*ctx)->next_tag)++;
	  }
	}
      }
    }
/********************************************
  If done and found then fill in the answer
*********************************************/
    if (found) {
      NODE *nptr = (*ctx)->this_tree_info->node;
      static char tagname[sizeof(TAG_NAME) + 1];
      if ((*ctx)->next_tag != -1) {
	static struct descriptor_s s_tag_name = { sizeof(TAG_NAME), DTYPE_T, CLASS_S, 0 };
	static struct descriptor_s tag_name = { sizeof(TAG_NAME), DTYPE_T, CLASS_S, tagname };
	unsigned short len;
	s_tag_name.pointer =
	    (char *)(*ctx)->this_tree_info->tag_info[swapint
						     ((char *)&(*ctx)->this_tree_info->
						      tags[(*ctx)->next_tag])].name;
	StrTrim((struct descriptor *)&tag_name, (struct descriptor *)&s_tag_name, &len);
	tagname[len] = '\0';
	nptr +=
	    swapint(&(*ctx)->this_tree_info->tag_info[swapint
						      ((char *)&(*ctx)->
						       this_tree_info->tags[(*ctx)->next_tag])].
		    node_idx);
      } else
	strcpy(tagname, "TOP");
      strcpy(answer, "\\");
      strcat(answer, (*ctx)->this_tree_info->treenam);
      strcat(answer, "::");
      strcat(answer, tagname);
      if (nid_ptr)
	node_to_nid(dblist, nptr, nid_ptr);
      ((*ctx)->next_tag)++;
      status = 1;
    } else {
      TreeFindTagEnd(ctx_inout);
      status = TreeNMT;
    }
    return (status & 1) ? answer : NULL;
  }
}

/****************************************
  Routine to free the tag search context
*****************************************/
void TreeFindTagEnd(void **ctx_inout)
{
  TAG_SEARCH **ctx = (TAG_SEARCH **) ctx_inout;
  if (*ctx) {
    if ((*ctx)->remote)
      FindTagEndRemote(ctx_inout);
    else {
      StrFree1Dx(&((*ctx)->search_tag));
      StrFree1Dx(&((*ctx)->search_tree));
      free(*ctx);
      *ctx = (TAG_SEARCH *) 0;
    }
  }
}

/*****************************************************
  Routine to return a new tag search data structure.
******************************************************/
static TAG_SEARCH *NewTagSearch(char *tagnam_ptr)
{
  static DESCRIPTOR(top, "TOP");
  TAG_SEARCH *ctx = (TAG_SEARCH *) malloc(sizeof(TAG_SEARCH));
  static struct descriptor_d empty = { 0, DTYPE_T, CLASS_D, 0 };
  struct descriptor tag_dsc = { 0, DTYPE_T, CLASS_S, 0 };
  unsigned short tree_len;
  char *cptr;
  static unsigned short one = 1;
  tag_dsc.length = (unsigned short)strlen(tagnam_ptr);
  tag_dsc.pointer = tagnam_ptr;
  ctx->search_tag = empty;
  ctx->search_tree = empty;
  ctx->next_tag = -1;
  ctx->this_tree_info = 0;
  ctx->remote = 0;
  if (*(char *)tag_dsc.pointer == '\\') {
    tag_dsc.length--;
    tag_dsc.pointer++;
  }
  if ((cptr = strstr(tagnam_ptr, "::")) != 0) {
    tree_len = (unsigned short)(cptr - tagnam_ptr);
    StrCopyR((struct descriptor *)&ctx->search_tree, &tree_len, tag_dsc.pointer);
    tag_dsc.length = (unsigned short)(tag_dsc.length - tree_len - 2);
    tag_dsc.pointer += (tree_len + 2);
  } else
    StrCopyR((struct descriptor *)&ctx->search_tree, &one, "*");
  if (tag_dsc.length)
    StrCopyDx((struct descriptor *)&ctx->search_tag, &tag_dsc);
  else
    StrCopyR((struct descriptor *)&ctx->search_tag, &one, "*");
  StrUpcase((struct descriptor *)&ctx->search_tree, (struct descriptor *)&ctx->search_tree);
  StrUpcase((struct descriptor *)&ctx->search_tag, (struct descriptor *)&ctx->search_tag);
  ctx->top_match = StrMatchWild((struct descriptor *)&top, (struct descriptor *)&ctx->search_tag) & 1;
  return ctx;
}

/********************************************
  Routine to return the next subtree in this
  tree which matches the wildcard tree spec.
*********************************************/
static int NextTagTree(PINO_DATABASE * dblist, TAG_SEARCH * ctx)
{
  unsigned char found = 0;
  if (ctx->this_tree_info == (TREE_INFO *) 0)
    ctx->this_tree_info = dblist->tree_info;
  else
    ctx->this_tree_info = ctx->this_tree_info->next_info;
  for (found = 0; !found && ctx->this_tree_info;
       ctx->this_tree_info = ctx->this_tree_info->next_info) {
    struct descriptor treenam = { 0, DTYPE_T, CLASS_S, 0 };
    treenam.length = (unsigned short)strlen(ctx->this_tree_info->treenam);
    treenam.pointer = ctx->this_tree_info->treenam;
    if (StrMatchWild((struct descriptor *)&treenam, (struct descriptor *)&ctx->search_tree) & 1) {
      found = 1;
      break;
    }
  }
  if (found)
    return 1;
  else
    return TreeNMT;
}
