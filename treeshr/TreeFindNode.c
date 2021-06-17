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
#include "treeshrp.h"
#include <ctype.h>
#include <mdsplus/mdsconfig.h>
#include <mdsplus/mdsplus.h>
#include <stdlib.h>
#include <string.h>
#include <strroutines.h>
#include <treeshr.h>

extern void **TreeCtx();
/*
 * Exported routines
 */
EXPORT int TreeFindNode(char const *path, int *outnid);
EXPORT int TreeFindNodeRelative(char const *path, int startnid, int *outnid);
EXPORT int _TreeFindNode(void *dbid, char const *path, int *outnid);
EXPORT int _TreeFindNodeRelative(void *dbid, char const *path, int startnid,
                                 int *outnid);

EXPORT int TreeFindNodeWild(char const *path, int *nid_out, void **ctx_inout,
                            int usage_mask);
EXPORT int TreeFindNodeWildRelative(char const *path, int startnid,
                                    int *nid_out, void **ctx_inout,
                                    int usage_mask);
EXPORT int _TreeFindNodeWild(void *dbid, char const *path, int *nid_out,
                             void **ctx_inout, int usage_mask);
EXPORT int _TreeFindNodeWildRelative(void *dbid, char const *path, int startnid,
                                     int *nid_out, void **ctx_inout,
                                     int usage_mask);

EXPORT int TreeFindNodeEnd(void **ctx_in);
EXPORT int _TreeFindNodeEnd(void *dbid, void **ctx);
/*
 * Static internal routines
 */
static NODELIST *Search(PINO_DATABASE *dblist, SEARCH_CTX *ctx,
                        SEARCH_TERM *term, NODE *start, NODELIST **tail);
static NODELIST *Find(PINO_DATABASE *dblist, SEARCH_TERM *term, NODE *start,
                      NODELIST **tail);
static NODELIST *AddNodeList(NODELIST *list, NODELIST **tail, NODE *node);
static void FreeNodeList(NODELIST *list);
static int match(char *first, char *second);
static void FreeSearchCtx(SEARCH_CTX *ctx);
static NODELIST *ConcatenateNodeLists(NODELIST *start_list, NODELIST **tail,
                                      NODELIST *end_list, NODELIST *end_tail);
static NODELIST *FindChildren(PINO_DATABASE *dblist, SEARCH_TERM *term,
                              NODE *start, NODELIST **tail);
static NODELIST *FindMembers(PINO_DATABASE *dblist, SEARCH_TERM *term,
                             NODE *start, NODELIST **tail);
static NODELIST *FindMembersOrChildren(PINO_DATABASE *dblist, SEARCH_TERM *term,
                                       NODE *start, NODELIST **tail);
static NODELIST *FindTagWild(PINO_DATABASE *dblist, SEARCH_TERM *term,
                             NODELIST **tail);
static NODELIST *Filter(NODELIST *list, int mask);
/*
 * External routines (not exported) defined here
 */
/*
 * External routines (not exported) referenced here
 */
extern int WildParse(char const *path, SEARCH_CTX *ctx, int *wild);
extern void FreeSearchTerms(SEARCH_TERM *terms);
extern char *_TreeGetPath(void *dbid, int nid_in);

/* trim trailing space form str into buf
 */
static size_t trim_cpy(char *buf, char *str, size_t len)
{
  size_t i;
  for (i = 0; i < len; i++)
  {
    if (str[i] == ' ')
      break;
    else
      buf[i] = str[i];
  }
  buf[i] = '\0';
  return i;
}
static inline int is_wild2(const char *const str)
{
  const char *p = str;
  while (*p != '\0' && *p != '*' && *p != '%')
    p++;
  return *p;
}
static inline int is_wild3(const char *const str)
{
  const char *p = str;
  while (*p != '\0' && *p != '*' && *p != '?' && *p != '%')
    p++;
  return *p;
}

EXPORT int TreeFindNode(char const *path, int *outnid)
{
  return _TreeFindNode(*TreeCtx(), path, outnid);
}

EXPORT int TreeFindNodeRelative(char const *path, int start, int *outnid)
{
  return _TreeFindNodeRelative(*TreeCtx(), path, start, outnid);
}

EXPORT int TreeFindNodeWild(char const *path, int *nid_out, void **ctx_inout,
                            int usage_mask)
{
  return _TreeFindNodeWild(*TreeCtx(), path, nid_out, ctx_inout, usage_mask);
}

EXPORT int TreeFindNodeWildRelative(char const *path, int start, int *nid_out,
                                    void **ctx_inout, int usage_mask)
{
  return _TreeFindNodeWildRelative(*TreeCtx(), path, start, nid_out, ctx_inout,
                                   usage_mask);
}

EXPORT int TreeFindNodeEnd(void **ctx_in)
{
  return _TreeFindNodeEnd(*TreeCtx(), ctx_in);
}

EXPORT int _TreeFindNodeWild(void *dbid, char const *path, int *nid_out,
                             void **ctx_inout, int usage_mask)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  int startnid = node_to_nid(dblist, dblist->default_node, NULL);
  int status = _TreeFindNodeWildRelative(dbid, path, startnid, nid_out,
                                         ctx_inout, usage_mask);

  return status;
}

EXPORT int _TreeFindNodeWildRelative(void *dbid, char const *path, int startnid,
                                     int *nid_out, void **ctx_inout,
                                     int usage_mask)
{
  int status = TreeSUCCESS;
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  int wild = 0;
  SEARCH_CTX *ctx;
  int not_first_time = 1;

  if (!IS_OPEN(dblist))
    return TreeNOT_OPEN;
  if (dblist->remote)
    return FindNodeWildRemote(dbid, path, nid_out, ctx_inout, usage_mask);
  if (*ctx_inout == NULL)
  {
    not_first_time = 0;
    ctx = (SEARCH_CTX *)calloc(1, sizeof(SEARCH_CTX));
    status = WildParse(path, ctx, &wild);
    if (STATUS_NOT_OK)
      return (status);
    *ctx_inout = ctx;
    NODELIST *tail = NULL;
    ctx->default_node = dblist->default_node;

    NODE *startn = nid_to_node(dblist, (NID *)&startnid);

    ctx->answers = Search(dblist, ctx, ctx->terms, startn, &tail);
    ctx->answers = Filter(ctx->answers, usage_mask);

    if (ctx->answers && (ctx->answers->node == ctx->default_node) && wild)
    {
      NODELIST *tmp = ctx->answers;
      ctx->answers = ctx->answers->next;
      free(tmp);
    }
  }
  else
  {
    ctx = *ctx_inout;
  }
  if (ctx->answers)
  {
    NODELIST *tmp;
    tmp = ctx->answers->next;
    *nid_out = node_to_nid(dblist, ctx->answers->node, (NID *)nid_out);
    free(ctx->answers);
    ctx->answers = tmp;
  }
  else
  {
    status = (not_first_time) ? TreeNMN : TreeNNF;
  }
  return status;
}

EXPORT int _TreeFindNode(void *dbid, char const *path, int *outnid)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  int startnid = node_to_nid(dblist, dblist->default_node, NULL);
  int status = _TreeFindNodeRelative(dblist, path, startnid, outnid);
  return status;
}

EXPORT int _TreeFindNodeRelative(void *dbid, char const *path, int startnid,
                                 int *outnid)
{
  int status = TreeSUCCESS;
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  int wild = 0;
  SEARCH_CTX ctx = {0};
  NODELIST *answer = NULL;
  NODELIST *tail = NULL;

  if (!IS_OPEN(dblist))
  {
    return TreeNOT_OPEN;
  }
  if (dblist->remote)
    return FindNodeRemote(dblist, path, outnid);

  if (path && (path[0] == '\n'))
  {
    status = TreeNNF;
    goto done;
  }

  status = WildParse(path, &ctx, &wild);
  if (STATUS_NOT_OK)
    goto done;
  if (wild)
  {
    status = TreeNOWILD;
    goto done;
  }
  // ctx.default_node = dblist->default_node;

  NODE *startn = nid_to_node(dblist, (NID *)&startnid);

  answer = Search(dblist, &ctx, ctx.terms, startn, &tail);

  if (answer)
  {
    *outnid = node_to_nid(dblist, answer->node, (NID *)outnid);
    FreeNodeList(answer);
  }
  else
  {
    status = TreeNNF;
  }
done:
  FreeSearchCtx(&ctx);
  return (STATUS_OK) ? TreeSUCCESS : status;
}

EXPORT int _TreeFindNodeEnd(void *dbid __attribute__((unused)), void **ctx)
{
  if (ctx)
  {
    if (*ctx)
    {
      FreeSearchCtx(*ctx);
      free(*ctx);
      *ctx = NULL;
    }
  }
  return TreeSUCCESS;
}

static void FreeSearchCtx(SEARCH_CTX *ctx)
{
  free(ctx->wildcard);
  FreeSearchTerms(ctx->terms);
}
static NODELIST *Search(PINO_DATABASE *dblist, SEARCH_CTX *ctx,
                        SEARCH_TERM *term, NODE *start, NODELIST **tail)
{
  NODELIST *nodes = term ? Find(dblist, term, start, tail) : NULL;
  if (nodes)
  {
    NODELIST *more_nodes = NULL;
    NODELIST *more_tail = NULL;
    if (term->next)
    {
      NODELIST *these_nodes = NULL;
      NODELIST *n;
      for (n = nodes; n; n = n->next)
      {
        NODELIST *these_tail = NULL;
        these_nodes = Search(dblist, ctx, term->next, n->node, &these_tail);
        if (these_nodes)
        {
          more_nodes = ConcatenateNodeLists(more_nodes, &more_tail, these_nodes,
                                            these_tail);
        }
      }
      FreeNodeList(nodes);
      *tail = more_tail;
      return (more_nodes);
    }
    else
    {
      return nodes;
    }
  }
  else
  {
    return NULL;
  }
}

static NODELIST *Find(PINO_DATABASE *dblist, SEARCH_TERM *term, NODE *start,
                      NODELIST **tail)
{
  NODELIST *answer = NULL;
  if (((term->search_type == CHILD) || (term->search_type == MEMBER)) &&
      (!is_wild3(term->term)))
  {
    term->search_type = CHILD_OR_MEMBER;
  }
  char trimmed[sizeof(NODE_NAME) + 1];
  switch (term->search_type)
  {
  case (CHILD):
  {
    NODE *n = child_of(dblist, start);
    for (; n; n = brother_of(dblist, n))
    {
      trim_cpy(trimmed, n->name, sizeof(NODE_NAME));
      if (match(term->term, trimmed))
        answer = AddNodeList(answer, tail, n);
    }
    break;
  }
  case (MEMBER):
  {
    NODE *n;
    for (n = member_of(start); n; n = brother_of(dblist, n))
    {
      trim_cpy(trimmed, n->name, sizeof(NODE_NAME));
      if (match(term->term, trimmed))
        answer = AddNodeList(answer, tail, n);
    }
    break;
  }
  case (CHILD_OR_MEMBER):
  {
    NODE *n;
    for (n = member_of(start); n; n = brother_of(dblist, n))
    {
      trim_cpy(trimmed, n->name, sizeof(NODE_NAME));
      if (match(term->term, trimmed))
        answer = AddNodeList(answer, tail, n);
    }
    n = child_of(dblist, start);
    for (; n; n = brother_of(dblist, n))
    {
      trim_cpy(trimmed, n->name, sizeof(NODE_NAME));
      if (match(term->term, trimmed))
        answer = AddNodeList(answer, tail, n);
    }
    break;
  }
  case (CHILD_SEARCH):
  {
    answer = FindChildren(dblist, term, start, tail);
    break;
  }
  case (MEMBER_SEARCH):
  {
    answer = FindMembers(dblist, term, start, tail);
    break;
  }
  case (CHILD_OR_MEMBER_SEARCH):
  {
    answer = FindMembersOrChildren(dblist, term, start, tail);
    break;
  }
  case (TAG):
  case (TAG_TREE):
  {
    answer = FindTagWild(dblist, term, tail);
    break;
  }
  case (PARENT):
  {
    answer = AddNodeList(answer, tail, parent_of(dblist, start));
    break;
  }
  case (ANCESTOR):
  {
    NODE *parent = parent_of(dblist, start);
    if (parent)
    {
      trim_cpy(trimmed, parent->name, sizeof(NODE_NAME));
      char *search_term = (strlen(term->term)) ? term->term : "*";
      if (match(search_term, trimmed))
        answer = AddNodeList(answer, tail, parent);
    }
    break;
  }
  case (ANCESTOR_SEARCH):
  {
    NODE *parent = parent_of(dblist, start);
    while (parent)
    {
      trim_cpy(trimmed, parent->name, sizeof(NODE_NAME));
      char *search_term = (strlen(term->term)) ? term->term : "*";
      if (match(search_term, trimmed))
        answer = AddNodeList(answer, tail, parent);
      parent = parent_of(dblist, parent);
    }
    break;
  }

  default:
  {
    printf("Search for type %d not implemented\n", term->search_type);
    return NULL;
    break;
  }
  }
  return (answer);
}

static NODELIST *FindTags(PINO_DATABASE *dblist, TREE_INFO *info, int treenum,
                          char *tagname, NODELIST **tail)
{
  NODELIST *answer = NULL;
  TAG_INFO *tptr = info->tag_info;
  int i;
  NID nid;
  int tag_wild = is_wild2(tagname);
  *tail = NULL;
  NODE *n;
  nid.tree = treenum;
  if (match(tagname, "TOP"))
  {
    nid.node = 0;
    n = nid_to_node(dblist, &nid);
    if (n->usage == TreeUSAGE_SUBTREE_REF)
      n = child_of(dblist, n);
    answer = AddNodeList(answer, tail, n);
  }
  else
  {
    char trimmed[sizeof(TAG_NAME) + 1];
    for (i = 0; i < info->header->tags; i++)
    {
      trim_cpy(trimmed, tptr[i].name, sizeof(TAG_NAME));
      if (match(tagname, trimmed))
      {
        nid.node = tptr[i].node_idx;
        n = nid_to_node(dblist, &nid);
        if (n->usage == TreeUSAGE_SUBTREE_REF)
          n = child_of(dblist, n);
        answer = AddNodeList(answer, tail, n);
        if (!tag_wild)
          break;
      }
    }
  }
  return (answer);
}

static TREE_INFO *GetDefaultTreeInfo(PINO_DATABASE *dblist, int *treenum)
{
  TREE_INFO *info;
  int i = 0;
  NID nid;
  node_to_nid(dblist, dblist->default_node, &nid);
  *treenum = nid.tree;
  for (info = dblist->tree_info; i < *treenum; info = info->next_info, i++)
    ;
  return info;
}

static NODELIST *FindTagWild(PINO_DATABASE *dblist, SEARCH_TERM *term,
                             NODELIST **tail)
{
  NODELIST *answer = NULL;
  TREE_INFO *tinfo;
  int treenum;
  char *tag_term = strdup(term->term);
  if (term->search_type == TAG_TREE)
  {
    int tree_wild;
    char *treename = tag_term;
    char *tagname = strstr(tag_term, "::") + 2;
    *(strstr(tag_term, "::")) = '\0';
    tree_wild = is_wild2(treename);
    if (treename[0] == '\\')
      treename++;
    for (treenum = 0, tinfo = dblist->tree_info; tinfo;
         tinfo = tinfo->next_info, treenum++)
    {
      if (match(treename, tinfo->treenam))
      {
        NODELIST *tag_tail = NULL;
        answer = ConcatenateNodeLists(
            answer, tail, FindTags(dblist, tinfo, treenum, tagname, &tag_tail),
            tag_tail);
        if (!tree_wild)
          break;
      }
    }
  }
  else
  {
    TREE_INFO *default_tinfo = GetDefaultTreeInfo(dblist, &treenum);
    char *tagname = tag_term;
    int tag_wild;
    if (tagname[0] == '\\')
      tagname++;
    tag_wild = is_wild2(tagname);
    answer = FindTags(dblist, default_tinfo, treenum, tagname, tail);
    if ((!answer) || tag_wild)
    {
      for (treenum = 0, tinfo = dblist->tree_info; tinfo;
           tinfo = tinfo->next_info)
      {
        if (tinfo != default_tinfo)
        {
          NODELIST *tag_tail = NULL;
          answer = ConcatenateNodeLists(
              answer, tail,
              FindTags(dblist, tinfo, treenum, tagname, &tag_tail), tag_tail);
          if (answer && (!tag_wild))
            break;
        }
        treenum++;
      }
    }
  }
  free(tag_term);
  return answer;
}

static NODELIST *FindMembers(PINO_DATABASE *dblist, SEARCH_TERM *term,
                             NODE *start, NODELIST **tail)
{
  NODELIST *answer = NULL;
  NODELIST *queue = NULL;
  NODELIST *que_tail = NULL;
  char trimmed[sizeof(NODE_NAME) + 1];
  *tail = NULL;
  queue = AddNodeList(queue, &que_tail, start);
  NODE *n;
  NODELIST *this;
  while (queue)
  {
    this = queue;
    queue = queue->next;
    if (!queue)
      que_tail = NULL;
    if ((n = member_of(this->node)))
    {
      for (; n; n = brother_of(dblist, n))
      {
        trim_cpy(trimmed, n->name, sizeof(NODE_NAME));
        char *search_term = (strlen(term->term)) ? term->term : "*";
        queue = AddNodeList(queue, &que_tail, n);
        if (match(search_term, trimmed))
          answer = AddNodeList(answer, tail, n);
      }
    }
    free(this);
  }
  return answer;
}

static NODELIST *FindChildren(PINO_DATABASE *dblist, SEARCH_TERM *term,
                              NODE *start, NODELIST **tail)
{
  NODELIST *answer = NULL;
  NODELIST *queue = NULL;
  NODELIST *que_tail = NULL;
  char trimmed[sizeof(NODE_NAME) + 1];
  *tail = NULL;
  queue = AddNodeList(queue, &que_tail, start);
  NODE *n;
  NODELIST *this;
  while (queue)
  {
    this = queue;
    queue = queue->next;
    if (!queue)
      que_tail = NULL;
    if ((n = child_of(dblist, this->node)))
    {
      for (; n; n = brother_of(dblist, n))
      {
        trim_cpy(trimmed, n->name, sizeof(NODE_NAME));
        char *search_term = (strlen(term->term)) ? term->term : "*";
        queue = AddNodeList(queue, &que_tail, n);
        if (match(search_term, trimmed))
          answer = AddNodeList(answer, tail, n);
      }
    }
    free(this);
  }
  return answer;
}

static NODELIST *FindMembersOrChildren(PINO_DATABASE *dblist, SEARCH_TERM *term,
                                       NODE *start, NODELIST **tail)
{
  NODELIST *answer = NULL;
  NODELIST *queue = NULL;
  NODELIST *que_tail = NULL;
  char trimmed[sizeof(NODE_NAME) + 1];
  *tail = NULL;
  queue = AddNodeList(queue, &que_tail, start);
  answer = AddNodeList(answer, tail, start);
  NODE *n;
  NODELIST *this;
  while (queue)
  {
    this = queue;
    queue = queue->next;
    if (!queue)
      que_tail = NULL;
    if ((n = descendant_of(dblist, this->node)))
    {
      for (; n; n = sibling_of(dblist, n))
      {
        trim_cpy(trimmed, n->name, sizeof(NODE_NAME));
        char *search_term = (strlen(term->term)) ? term->term : "*";
        queue = AddNodeList(queue, &que_tail, n);
        if (match(search_term, trimmed))
          answer = AddNodeList(answer, tail, n);
      }
    }
    free(this);
  }
  return answer;
}

static NODELIST *AddNodeList(NODELIST *head, NODELIST **tail, NODE *node)
{
  NODELIST *this_one = malloc(sizeof(NODELIST));
  this_one->node = node;
  this_one->next = NULL;
  if (*tail)
  {
    (*tail)->next = this_one;
  }
  else
  {
    head = this_one;
  }
  *tail = this_one;
  return (head);
}

static void FreeNodeList(NODELIST *list)
{
  NODELIST *ptr;
  for (ptr = list; ptr;)
  {
    NODELIST *nxt = ptr->next;
    free(ptr);
    ptr = nxt;
  }
}

static int match(char *first, char *second)
{
  // If we reach at the end of both strings, we are done
  if (*first == '\0' && *second == '\0')
    return 1;

  // Make sure that the characters after '*' are present
  // in second string. This function assumes that the first
  // string will not contain two consecutive '*'
  if (*first == '*' && *(first + 1) != '\0' && *second == '\0')
    return 0;

  // If the first string contains '?', or current characters
  // of both strings match
  if (*first == '?' || *first == '%' || *first == *second)
    return match(first + 1, second + 1);

  // If there is *, then there are two possibilities
  // a) We consider current character of second string
  // b) We ignore current character of second string.
  if (*first == '*')
    return match(first + 1, second) || match(first, second + 1);
  return 0;
}

static NODELIST *ConcatenateNodeLists(NODELIST *start_list, NODELIST **tail,
                                      NODELIST *end_list, NODELIST *end_tail)
{
  NODELIST *answer;

  if (end_list)
  {
    if (start_list)
    {
      (*tail)->next = end_list;
      answer = start_list;
    }
    else
    {
      answer = end_list;
    }
    *tail = end_tail;
  }
  else
  {
    answer = start_list;
  }
  return answer;
}

static NODELIST *Filter(NODELIST *list, int usage_mask)
{
  NODELIST *ptr;
  NODELIST *previous = list;
  NODELIST *answer = list;

  /* printf("In Filter() list %s and mask %d \n", *list, usage_mask); */

  for (ptr = list; ptr;)
  {
    if (!((1 << ((ptr->node->usage == TreeUSAGE_SUBTREE_TOP)
                     ? TreeUSAGE_SUBTREE
                     : ptr->node->usage)) &
          usage_mask))
    {
      NODELIST *tmp = ptr;
      if (ptr == answer)
      {
        answer = ptr->next;
        previous = answer;
        ptr = ptr->next;
      }
      else
      {
        previous->next = ptr->next;
        ptr = ptr->next;
      }
      free(tmp);
    }
    else
    {
      previous = ptr;
      ptr = ptr->next;
    }
  }
  return answer;
}
extern int TreeFindParent(PINO_DATABASE *dblist, char *name, NODE **node,
                          char **new_name, int *child)
{
  NODE *start = dblist->default_node;
  int status = TreeSUCCESS;
  *node = NULL;
  char *parent_name = strdup(name);
  char *last_dot = strrchr(parent_name, '.');
  char *last_colon = strrchr(parent_name, ':');
  if ((last_dot == NULL) && (last_colon == NULL))
  {
    *node = start;
    *new_name = strdup(name);
    *child = 0;
  }
  else
  {
    NID nid;
    *child = 0;
    if (last_dot > last_colon)
    {
      *child = 1;
      *new_name = strdup(last_dot + 1);
      *last_dot = '\0';
    }
    else
    {
      *child = 0;
      *new_name = strdup(last_colon + 1);
      *last_colon = '\0';
    }
    if (strlen(parent_name) == 0)
    {
      status = TreeSUCCESS;
      *node = dblist->default_node;
    }
    else
    {
      status = _TreeFindNode(dblist, parent_name, (int *)&nid);
      if (STATUS_OK)
      {
        *node = nid_to_node(dblist, &nid);
      }
    }
  }
  if (!*node)
  {
    status = TreeNNF;
  }
  free(parent_name);
  return status;
}
