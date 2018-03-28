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
#include <STATICdef.h>
#include <mdsplus/mdsconfig.h>
#include <stdlib.h>
#include <string.h>
#include <mdsplus/mdsplus.h>
#include <strroutines.h>
#include <treeshr.h>
#include "treeshrp.h"
#include <ctype.h>

extern void **TreeCtx();

#define min(a,b) (((a) < (b)) ? (a) : (b))

STATIC_ROUTINE int BsearchCompare(const void *this_one, const void *compare_one);

EXPORT char *TreeFindNodeTags(int nid_in, void **ctx_ptr)
{
  return _TreeFindNodeTags(*TreeCtx(), nid_in, ctx_ptr);
}


EXPORT int TreeFindTag(const char *tagnam, const char *treename, int *tagidx)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) * TreeCtx();
  NODE *nodeptr;
  return _TreeFindTag(*TreeCtx(), dblist->default_node, (short)strlen(treename), treename,
		      (short)strlen(tagnam), tagnam, &nodeptr, tagidx);
}

EXPORT int TreeFindNodeTagsDsc(int nid_in, void **ctx_ptr, struct descriptor *tag)
{
  int status;
  char *tagname = TreeFindNodeTags(nid_in, ctx_ptr);
  if (tagname) {
    struct descriptor tagd = { 0, DTYPE_T, CLASS_S, 0 };
    tagd.length = (unsigned short)strlen(tagname);
    tagd.pointer = tagname;
    StrCopyDx(tag, &tagd);
    status = 1;
  } else
    status = 0;
  return status;
}

EXPORT char *_TreeFindNodeTags(void *dbid, int nid_in, void **ctx_ptr)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid = (NID *) & nid_in;
  TREE_INFO *info_ptr;
  NODE *node_ptr;
  int *ctx = (int *)ctx_ptr;
  if (!(IS_OPEN(dblist)))
    return NULL;
  if (dblist->remote)
    return FindNodeTagsRemote(dblist, nid_in, ctx_ptr);

/*******************************************************
 First we must find the tree to which the node belongs.
 The tags associated with a node are linked together
 in a linked list. The tags are indexed by TAG_LINK
 fields in the node structure and the tag information
 structure. The TAG_LINK is an index into the array of
 tag information blocks of the tree using 1 to designate
 the first tag. Since a node may have more than one tag
 this routine must be called repetitively to return all
 the tag names. A context input/output argument of the
 routine is used to maintain the context between subsequent
 calls.
**********************************************************/

  nid_to_tree(dblist, nid, info_ptr);
  if (info_ptr) {

  /*********************************************************
   If the context argument is zero then begin at the nodes
   tag link. If the context argument is -1 then this must
   just be the last of a sequence of calls and we must just
   return NO MORE TAGS. Otherwise we assume the context pointer
   is the tag index of the next tag to be returned to the called.
  **********************************************************/

    if (*ctx == 0) {
      node_ptr = nid_to_node(dblist, nid);
      *ctx = swapint((char *)&node_ptr->tag_link);
    } else if (*ctx == -1) {
      *ctx = 0;
    }
    if ((*ctx > 0) && (*ctx <= info_ptr->header->tags)) {
      unsigned int i;
      char *name = (char *)(info_ptr->tag_info + *ctx - 1)->name;
      for (i = 0; i < sizeof(TAG_NAME) && name[i] != ' '; i++) ;
      char *answer = strncpy(malloc(i + 1), name, i);
      answer[i] = '\0';
      *ctx = swapint((char *)&(info_ptr->tag_info + *ctx - 1)->tag_link);
      if (*ctx == 0)
	*ctx = -1;
      return answer;
    } else
      *ctx = 0;
  } else
    *ctx = 0;
  return NULL;
}

struct tag_search {
  TAG_NAME tag;
  TREE_INFO *info;
};

EXPORT int _TreeFindTag(PINO_DATABASE * db, NODE * default_node, short treelen, const char *tree,
		 short taglen, const char *tagnam, NODE ** nodeptr, int *tagidx)
{
  int len = min(taglen, (short)sizeof(TAG_NAME));
  int i;
  int *idx;
  int status;
  struct tag_search tsearch;
  memset(tsearch.tag, 32, sizeof(TAG_NAME));
  for (i = 0; i < len; i++)
    tsearch.tag[i] = (char)toupper(tagnam[i]);
  *nodeptr = NULL;
/********************************************
 To locate a tag we must first find which tree
 is being referenced. If the tree name is not
 explicitly provided, the tree is determined
 by the current default node.
*********************************************/

  for (tsearch.info = db->tree_info; tsearch.info; tsearch.info = tsearch.info->next_info) {
    if (tree) {
      size_t len = strlen(tsearch.info->treenam);
      if ((len == (size_t) treelen) && strncmp(tsearch.info->treenam, tree, len) == 0)
	break;
    } else if ((default_node >= tsearch.info->node) &&
	       (default_node < tsearch.info->node + tsearch.info->header->nodes))
      break;
  }
  if (tsearch.info == NULL)
    return TreeTNF;

	  /***********************************************
	  If the tag name specified is the reserved name
	  "TOP", then return the root pointer of the
	  tree being referenced.
	  Otherwise we must look up the tagname in the
	  tagname table of the tree.
      ***********************************************/
  if (taglen == 3 && strncmp(tagnam, "TOP", 3) == 0) {
    *nodeptr = tsearch.info->root;
    *tagidx = 0;
    return TreeNORMAL;
  } else {
    /******************************************************
     To look up the tag in the tag table we will use a
     binary search. If there are no tags defined, just return
     TAG NOT FOUND. If there is only one tag defined, the
     binary search won't work correctly so we must just test
     to single tag directly. If there is more than one tag
     we will just use the C binary search routine to find the
     tag in the table.
    ********************************************************/
    switch (tsearch.info->header->tags) {
    case 0:
      status = TreeTNF;
      break;
    case 1:
      if (BsearchCompare((void *)&tsearch, (void *)tsearch.info->tags) == 0) {
	*nodeptr = tsearch.info->node + swapint((char *)&tsearch.info->tag_info->node_idx);
	*tagidx = 1;
	return TreeNORMAL;
      } else
	status = TreeTNF;
      break;
    default:
      if ((idx = bsearch((const void *)&tsearch, (const void *)tsearch.info->tags,
			 (size_t)tsearch.info->header->tags, sizeof(int), BsearchCompare)) != 0) {
	*nodeptr =
	    tsearch.info->node +
	    swapint((char *)&(tsearch.info->tag_info + swapint((char *)idx))->node_idx);
	*tagidx = swapint((char *)idx) + 1;
	return TreeNORMAL;
      } else
	status = TreeTNF;
      break;
    }
    if (status == TreeTNF && tree == 0) {
      char *tag;
      void *ctx = 0;
      int nid;
      NODE *node;
      NID *nidptr = (NID *) & nid;
      unsigned int i;
      for (i = 0; i < sizeof(tsearch.tag); i++) {
	if (tsearch.tag[i] == ' ') {
	  tsearch.tag[i] = '\0';
	  break;
	}
      }
      tag = _TreeFindTagWild(db, tsearch.tag, &nid, &ctx);
      if (tag) {
	status = TreeNORMAL;
	node = nid_to_node(db, nidptr);
	*nodeptr = node;
      }
      TreeFindTagEnd(&ctx);
    }
  }
  return status;
}

STATIC_ROUTINE int BsearchCompare(const void *this_one, const void *compare_one)
{
  struct tag_search *tsearch = (struct tag_search *)this_one;
  char *tag = (tsearch->info->tag_info + swapint((char *)compare_one))->name;

/******************************************
 This routine is called by bsearch during
 binary searches of the tag table. The first
 argument is the info block and tag name to check and the
 second argument is the index into the tag
 info block array.
*******************************************/

  return strncmp((char *)tsearch->tag, (char *)tag, sizeof(TAG_NAME));
}

EXPORT int TreeAbsPathDsc(char *inpath, struct descriptor *outpath)
{
  char *ans_c = TreeAbsPath(inpath);
  if (ans_c == 0)
    return 0;
  else {
    struct descriptor ans_d = { 0, DTYPE_T, CLASS_S, 0 };
    ans_d.length = (unsigned short)strlen(ans_c);
    ans_d.pointer = ans_c;
    StrCopyDx(outpath, &ans_d);
    free(ans_c);
  }
  return 1;
}
