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

                Purpose: Find all of the tags matching a wildcard tag
specification.

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

#include <strroutines.h>
#include <treeshr.h>

#include "treeshrp.h"
#include "treethreadstatic.h"

extern int MdsCopyDxXd();
extern char *FindTagWildRemote();
extern int FindTagEndRemote();

extern void **TreeCtx();

typedef struct tag_search
{
  int next_tag;
  TREE_INFO *this_tree_info;
  mdsdsc_d_t search_tag;
  mdsdsc_d_t search_tree;
  unsigned char top_match;
  unsigned char remote;
  char answer[1 + sizeof(TREE_NAME) + 2 + sizeof(TAG_NAME) + 1];
  int socket;
} TAG_SEARCH;

static inline TAG_SEARCH *newTagSearch(char *tagnam_ptr);
static int nextTagTree(PINO_DATABASE *dblist, TAG_SEARCH *ctx);

char *_TreeFindTagWild(void *dbid, char *wild, int *nidout, void **ctx_inout);

char *TreeFindTagWild(char *wild, int *nidout, void **ctx_inout)
{
  return _TreeFindTagWild(*TreeCtx(), wild, nidout, ctx_inout);
}

int _TreeFindTagWildDsc(void *dbid, char *wild, int *nidout, void **ctx_inout,
                        mdsdsc_xd_t *name)
{
  char *ans_c = _TreeFindTagWild(dbid, wild, nidout, ctx_inout);
  if (!ans_c)
    return TreeFAILURE;
  DESCRIPTOR_FROM_CSTRING(ans_d, ans_c);
  MdsCopyDxXd(&ans_d, name);
  return TreeSUCCESS;
}
int TreeFindTagWildDsc(char *wild, int *nidout, void **ctx_inout,
                       mdsdsc_xd_t *name)
{
  return _TreeFindTagWildDsc(*TreeCtx(), wild, nidout, ctx_inout, name);
}

static int findtag(PINO_DATABASE *dblist, TAG_SEARCH **ctx)
{
  int found, done, status;
  mdsdsc_s_t s_tag_dsc = {sizeof(TAG_NAME), DTYPE_T, CLASS_S, 0};
  mdsdsc_d_t tag_dsc = {0, DTYPE_T, CLASS_D, 0};
  FREED_ON_EXIT(&tag_dsc);
  for (found = 0, done = 0, status = 1; STATUS_OK && !found && !done;)
  {
    /*************************************
     if out of tags in this tree then
     see if there is another one
   **************************************/
    if ((*ctx)->next_tag >= (*ctx)->this_tree_info->header->tags)
    {
      status = nextTagTree(dblist, *ctx);
      if (STATUS_OK)
        (*ctx)->next_tag = -1;
      else
      {
        done = 1;
        break;
      }
    }
    else
    {
      /**********************************************
        else if this is the first time for this tree
        try to return the \TOP tag.
        otherwise - move on to next tag for next
        time through the loop.
      ***********************************************/
      if ((*ctx)->next_tag == -1)
      {
        if ((*ctx)->top_match)
        {
          done = 1;
          found = 1;
        }
        else
          ((*ctx)->next_tag)++;
      }
      else
      {

        /****************************************
           Else
             loop looking for a tag that matches
        *****************************************/
        for (; !done &&
               ((*ctx)->next_tag < (*ctx)->this_tree_info->header->tags);)
        {
          unsigned short len;
          s_tag_dsc.pointer =
              (char *)(*ctx)
                  ->this_tree_info
                  ->tag_info[swapint32(
                      &(*ctx)->this_tree_info->tags[(*ctx)->next_tag])]
                  .name;
          StrTrim((mdsdsc_t *)&tag_dsc, (mdsdsc_t *)&s_tag_dsc, &len);
          if (IS_OK(StrMatchWild((mdsdsc_t *)&tag_dsc,
                                 (mdsdsc_t *)&((*ctx)->search_tag))))
          {
            done = 1;
            found = 1;
          }
          else
            ((*ctx)->next_tag)++;
        }
      }
    }
  }
  FREED_NOW(&tag_dsc);
  return found;
}

char *_TreeFindTagWild(void *dbid, char *wild, int *nidout, void **ctx_inout)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  /***************************
    check that there is a tree
    open.
   ****************************/
  if (!IS_OPEN(dblist))
    return NULL;
  if (dblist->remote)
    return FindTagWildRemote(dblist, wild, nidout, ctx_inout);
  NID *nid_ptr = (NID *)nidout;
  TAG_SEARCH **ctx = (TAG_SEARCH **)ctx_inout;
  /**********************************
  If this is the first time then
  allocate a context block and fill
  it in with a parse of the tagname.
  ***********************************/
  int status;
  if (*ctx == (TAG_SEARCH *)0)
  {
    *ctx = newTagSearch(wild);
    if (*ctx == (TAG_SEARCH *)0)
      return NULL;
    else
      status = nextTagTree(dblist, *ctx);
  }
  else
    status = TreeSUCCESS;

  /*************************************
   Loop looking for a tag that matches
  **************************************/
  /********************************************
    If done and found then fill in the answer
  *********************************************/
  if (STATUS_OK && findtag(dblist, ctx))
  {
    NODE *nptr = (*ctx)->this_tree_info->node;
    char tagname[sizeof(TAG_NAME) + 1];
    if ((*ctx)->next_tag != -1)
    {
      mdsdsc_s_t s_tag_name = {sizeof(TAG_NAME), DTYPE_T, CLASS_S, 0};
      mdsdsc_s_t tag_name = {sizeof(TAG_NAME), DTYPE_T, CLASS_S, tagname};
      unsigned short len;
      s_tag_name.pointer =
          (char *)(*ctx)
              ->this_tree_info
              ->tag_info[swapint32(
                  &(*ctx)->this_tree_info->tags[(*ctx)->next_tag])]
              .name;
      StrTrim((mdsdsc_t *)&tag_name, (mdsdsc_t *)&s_tag_name, &len);
      tagname[len] = '\0';
      nptr +=
          swapint32(&(*ctx)
                         ->this_tree_info
                         ->tag_info[swapint32(
                             &(*ctx)->this_tree_info->tags[(*ctx)->next_tag])]
                         .node_idx);
    }
    else
      strcpy(tagname, "TOP");
    strcpy((*ctx)->answer, "\\");
    strcat((*ctx)->answer, (*ctx)->this_tree_info->treenam);
    strcat((*ctx)->answer, "::");
    strcat((*ctx)->answer, tagname);
    if (nid_ptr)
      node_to_nid(dblist, nptr, nid_ptr);
    ((*ctx)->next_tag)++;
  }
  else
  {
    TreeFindTagEnd(ctx_inout);
    status = TreeNMT;
  }
  return STATUS_OK ? (*ctx)->answer : NULL;
}

/****************************************
  Routine to free the tag search context
*****************************************/
void TreeFindTagEnd(void **ctx_inout)
{
  if (!*ctx_inout)
    return;
  TAG_SEARCH **ctx = (TAG_SEARCH **)ctx_inout;
  if ((*ctx)->remote)
    FindTagEndRemote(ctx_inout);
  else
  {
    StrFree1Dx(&((*ctx)->search_tag));
    StrFree1Dx(&((*ctx)->search_tree));
    free(*ctx);
    *ctx = NULL;
  }
}

/*****************************************************
  Routine to return a new tag search data structure.
******************************************************/
static inline TAG_SEARCH *newTagSearch(char *tagnam_ptr)
{
  const DESCRIPTOR(top, "TOP");
  length_t tree_len;
  TAG_SEARCH *ctx = (TAG_SEARCH *)malloc(sizeof(TAG_SEARCH));
  if (!ctx)
    return NULL;
  ctx->search_tag = (mdsdsc_d_t){0, DTYPE_T, CLASS_D, 0};
  ctx->search_tree = (mdsdsc_d_t){0, DTYPE_T, CLASS_D, 0};
  ctx->next_tag = -1;
  ctx->this_tree_info = 0;
  ctx->remote = 0;
  ctx->answer[0] = '\0';
  DESCRIPTOR_FROM_CSTRING(tag_dsc, tagnam_ptr);
  if (*(char *)tag_dsc.pointer == '\\')
  {
    tag_dsc.length--;
    tag_dsc.pointer++;
  }
  char *cptr = strstr(tagnam_ptr, "::");
  const length_t one = 1;
  if (cptr)
  {
    tree_len = (length_t)(cptr - tagnam_ptr);
    StrCopyR((mdsdsc_t *)&ctx->search_tree, &tree_len, tag_dsc.pointer);
    tag_dsc.length = (length_t)(tag_dsc.length - tree_len - 2);
    tag_dsc.pointer += (tree_len + 2);
  }
  else
    StrCopyR((mdsdsc_t *)&ctx->search_tree, &one, "*");
  if (tag_dsc.length)
    StrCopyDx((mdsdsc_t *)&ctx->search_tag, &tag_dsc);
  else
    StrCopyR((mdsdsc_t *)&ctx->search_tag, &one, "*");
  StrUpcase((mdsdsc_t *)&ctx->search_tree, (mdsdsc_t *)&ctx->search_tree);
  StrUpcase((mdsdsc_t *)&ctx->search_tag, (mdsdsc_t *)&ctx->search_tag);
  ctx->top_match =
      IS_OK(StrMatchWild((mdsdsc_t *)&top, (mdsdsc_t *)&ctx->search_tag));
  return ctx;
}

/********************************************
  Routine to return the next subtree in this
  tree which matches the wildcard tree spec.
*********************************************/
static int nextTagTree(PINO_DATABASE *dblist, TAG_SEARCH *ctx)
{
  if (!ctx->this_tree_info)
    ctx->this_tree_info = dblist->tree_info;
  else
    ctx->this_tree_info = ctx->this_tree_info->next_info;
  for (; ctx->this_tree_info;
       ctx->this_tree_info = ctx->this_tree_info->next_info)
  {
    DESCRIPTOR_FROM_CSTRING(treenam, ctx->this_tree_info->treenam);
    if (IS_OK(StrMatchWild(&treenam, (mdsdsc_t *)&ctx->search_tree)))
      return TreeSUCCESS;
  }
  return TreeNMT;
}
