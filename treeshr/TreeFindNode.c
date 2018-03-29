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
*/#include <mdsplus/mdsconfig.h>
#include <stdlib.h>
#include <string.h>
#include <mdsplus/mdsplus.h>
#include <strroutines.h>
#include <treeshr.h>
#include "treeshrp.h"
#include <ctype.h>

extern void **TreeCtx();
/*
 * Exported routines
 */
EXPORT char *TreeAbsPath(char const *inpath);
EXPORT int TreeFindNode(char const *path, int *outnid);
EXPORT int TreeFindNodeWild(char const *path, int *nid_out, void **ctx_inout, int usage_mask);
EXPORT int TreeFindNodeEnd(void **ctx_in);
EXPORT int _TreeFindNodeWild(void *dbid, char const *path, int *nid_out, void **ctx_inout, int usage_mask);
EXPORT int _TreeFindNode (void *dbid, char const *path, int *outnid);
EXPORT int _TreeFindNodeWild(void *dbid,  char const *path, int *nid_out, void **ctx_inout, int usage_mask);
EXPORT int _TreeFindNodeEnd(void *dbid, void **ctx);
/*
 * Static internal routines
 */
STATIC_ROUTINE NODELIST *RemoveTail(NODELIST *list);
STATIC_ROUTINE int NotIn(NODELIST *list, NODE *node);
STATIC_ROUTINE char *AbsPath(void *dbid, char const *inpath, int nid_in);
STATIC_ROUTINE NODELIST *Search(PINO_DATABASE *dblist, SEARCH_CTX *ctx, SEARCH_TERM *term, NODE *start);
STATIC_ROUTINE NODELIST *Find(PINO_DATABASE *dblist, SEARCH_TERM *term, NODE *start);
STATIC_ROUTINE NODELIST *AddNodeList(NODELIST *list, NODE *node);
STATIC_ROUTINE void FreeNodeList(NODELIST *list);
STATIC_ROUTINE int match(char *first, char *second);
STATIC_ROUTINE void FreeSearchCtx(SEARCH_CTX *ctx);
STATIC_ROUTINE NODELIST *ConcatenateNodeLists(NODELIST *start_list, NODELIST *end_list);
STATIC_ROUTINE char *Trim(char *str);
STATIC_ROUTINE NODELIST *FindChildren(PINO_DATABASE *dblist, SEARCH_TERM *term, NODE *start);
STATIC_ROUTINE NODELIST *FindMembers(PINO_DATABASE *dblist, SEARCH_TERM *term, NODE *start);
STATIC_ROUTINE NODELIST *FindMembersOrChildren(PINO_DATABASE *dblist, SEARCH_TERM *term, NODE *start);
STATIC_ROUTINE NODELIST *FindTagWild(PINO_DATABASE *dblist, SEARCH_TERM *term);
STATIC_ROUTINE NODELIST *Filter(NODELIST *list, int mask);
STATIC_ROUTINE char *Treename(PINO_DATABASE * dblist, int nid_in);
/*
 * External routines (not exported) defined here
 */
/*
 * External routines (not exported) referenced here
 */
extern int WildParse(char const *path, SEARCH_CTX *ctx, int *wild);
extern void FreeSearchTerms(SEARCH_TERM *terms);
extern char *_TreeGetPath(void *dbid, int nid_in);

EXPORT int TreeFindNode(char const *path, int *outnid)
{
  return _TreeFindNode(*TreeCtx(), path, outnid);
}

EXPORT int TreeFindNodeWild(char const *path, int *nid_out, void **ctx_inout, int usage_mask)
{
   return _TreeFindNodeWild(*TreeCtx(), path, nid_out, ctx_inout, usage_mask);
}

EXPORT int TreeFindNodeEnd(void **ctx_in)
{
  return _TreeFindNodeEnd(*TreeCtx(), ctx_in);
}
EXPORT char *TreeAbsPath(char const *inpath)
{
  return _TreeAbsPath(*TreeCtx(), inpath);
}
EXPORT int _TreeFindNodeWild(void *dbid, char const *path, int *nid_out, void **ctx_inout, int usage_mask)
{
  int status = TreeNORMAL;
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  int wild = 0;
  SEARCH_CTX *ctx;

  if (!IS_OPEN(dblist))
    return TreeNOT_OPEN;
//  if (dblist->remote)
//    return FindNodeRemote(dblist, path, outnid);
  if (*ctx_inout == NULL) {
    ctx = (SEARCH_CTX *)calloc(1, sizeof(SEARCH_CTX));
    status = WildParse(path, ctx, &wild);
    if STATUS_NOT_OK
      return(status);
    *ctx_inout = ctx;
    ctx->default_node = dblist->default_node;
    ctx->answers = Search(dblist, ctx, ctx->terms, dblist->default_node);
    ctx->answers = Filter(ctx->answers, usage_mask); 
 }
  else {
    ctx = *ctx_inout;
  }
  if (ctx->answers) {
    NODELIST *tmp;
    tmp = ctx->answers->next;
    *nid_out = node_to_nid(dblist, ctx->answers->node, (NID *)nid_out);
    free(ctx->answers);
    ctx->answers = tmp;    
  }
  else {
    status = TreeNMN;
  }
  return status;
}

EXPORT int _TreeFindNode(void *dbid, char const *path, int *outnid)
{
  int status = TreeNORMAL;
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  int wild = 0;
  SEARCH_CTX ctx;
  NODELIST *answer;

  if (!IS_OPEN(dblist))
    return TreeNOT_OPEN;
  if (dblist->remote)
    return FindNodeRemote(dblist, path, outnid);
  status = WildParse(path, &ctx, &wild);
  if STATUS_NOT_OK 
    return(status);
  if (wild) {
    return 0; /* should be TreeNOWILD */
  }
  ctx.default_node = dblist->default_node;
//  ctx.terms->start_node = ctx.default_node;
  answer = Search(dblist, &ctx, ctx.terms, dblist->default_node);
  if (answer) {
    *outnid = node_to_nid(dblist, answer->node, (NID *)outnid);
  }
  else {
    status = TreeNNF;
  }
  FreeSearchCtx(&ctx);
  return status;
}
EXPORT int _TreeFindNodeEnd(void *dbid __attribute__ ((unused)), void **ctx)
{
  FreeSearchCtx(*ctx);
  free(*ctx);
  *ctx = NULL;
  return TreeNORMAL;
}
STATIC_ROUTINE void FreeSearchCtx(SEARCH_CTX *ctx)
{
  free(ctx->wildcard);
  FreeSearchTerms(ctx->terms);
}
STATIC_ROUTINE NODELIST *Search(PINO_DATABASE *dblist, SEARCH_CTX *ctx, SEARCH_TERM *term, NODE *start)
{
  NODELIST *nodes = Find(dblist, term, start);
  if (nodes) {
    NODELIST *more_nodes = NULL;    
    if(term->next) {
      NODELIST *these_nodes = NULL;
      NODELIST *n;
      for (n=nodes; n; n=n->next) {
        these_nodes = Search(dblist, ctx, term->next, n->node);
        if (these_nodes) {
          more_nodes = ConcatenateNodeLists(more_nodes, these_nodes);
        }
      }
      FreeNodeList(nodes);
      return(more_nodes);
    }
    else{
      return nodes;
    }
  }
  else{
    return NULL;
  } 
}

STATIC_ROUTINE inline int Wild(char *term) 
{
  return (strchr(term, '*') || strchr(term, '?'));
}

STATIC_ROUTINE NODELIST *Find(PINO_DATABASE *dblist, SEARCH_TERM *term, NODE *start)
{
  NODELIST *answer = NULL;
  if (((term->search_type==CHILD) || (term->search_type==MEMBER)) && (! Wild(term->term))) {
    term->search_type = CHILD_OR_MEMBER;
  }
  switch (term->search_type) {
    case (CHILD) : {
      NODE *n;
      for (n=child_of(dblist, start); n; n = brother_of(dblist, n)) {
        char *trimmed = Trim(n->name);
        if (match(term->term, trimmed)) {
          answer = AddNodeList(answer, n);
        }
        free(trimmed);
      }
      break;
    }
    case (MEMBER) : {
      NODE *n;
      for (n=member_of(start); n; n = brother_of(dblist, n)) {
        char *trimmed = Trim(n->name);
        if (match(term->term, trimmed)) {
          answer = AddNodeList(answer, n);
        }
        free(trimmed);
      }
       break;
    }
    case (CHILD_OR_MEMBER) : {
      NODE *n;
      for (n=member_of(start); n; n = brother_of(dblist, n)) {
        char *trimmed = Trim(n->name);
        if (match(term->term, trimmed)) {
          answer = AddNodeList(answer, n);
        }
        free(trimmed);
      }
      for (n=child_of(dblist, start); n; n = brother_of(dblist, n)) {
        char *trimmed = Trim(n->name);
        if (match(term->term, trimmed)) {
          answer = AddNodeList(answer, n);
        }
        free(trimmed);
      }
      break;
    }
    case (CHILD_SEARCH) : {
      answer = FindChildren(dblist, term, start);
      break;
    }
    case (MEMBER_SEARCH) : {
      answer = FindMembers(dblist, term, start);
      break;
    }
    case (CHILD_OR_MEMBER_SEARCH) : {
      answer = FindMembersOrChildren(dblist, term, start);
      break;
    }
    case (TAG):
    case (TAG_TREE): {
      answer = FindTagWild(dblist, term);
      break;
    }
    case (PARENT) :
    {
      answer = AddNodeList(answer, parent_of(dblist, start));
      break;
    }
    case (ANCESTOR): {
      NODE *parent = parent_of(dblist, start);
      if (parent) {
        char *trimmed = Trim(parent->name);
        char *search_term = (strlen(term->term)) ? term->term : "*";
        if (match(search_term, trimmed))
          answer = AddNodeList(answer, parent);
        free(trimmed);
      }
      break;
    }
    case (ANCESTOR_SEARCH): {
      NODE *parent = parent_of(dblist, start);
      while (parent) {
        char *trimmed = Trim(parent->name);
        char *search_term = (strlen(term->term)) ? term->term : "*";
        if (match(search_term, trimmed))
          answer = AddNodeList(answer, parent);
        free(trimmed);
        parent = parent_of(dblist, parent);
      }
      break;
    }

    default: {
      printf("Search for type %d not implimented\n", term->search_type);
      return NULL;
      break;
    }
  }
  return(answer);
}

STATIC_ROUTINE NODELIST *FindTags(PINO_DATABASE *dblist, TREE_INFO *info, int treenum, char *tagname)
{
  NODELIST *answer = NULL;
  TAG_INFO *tptr = info->tag_info;
  int i;
  NID nid;
  nid.tree = treenum;
  if(match(tagname, "TOP")) {
    NID nid ={treenum, 0};
    answer = AddNodeList(answer, nid_to_node(dblist, &nid));
  } 
  for (i=0; i < *info->tags; i++) {
    char *trimmed = Trim(tptr[i].name);
    if(match(tagname, trimmed)) {
      nid.node = tptr[i].node_idx;
      answer = AddNodeList(answer, nid_to_node(dblist, &nid));
    }
    free(trimmed);
  }
  return(answer);
}

STATIC_ROUTINE TREE_INFO *GetDefaultTreeInfo(PINO_DATABASE *dblist, int *treenum)
{
  TREE_INFO *info;
  int  i = 0;
  NID nid;
  node_to_nid(dblist, dblist->default_node, &nid);
  *treenum = nid.tree; 
  for (info=dblist->tree_info; i < *treenum; info = info->next_info, i++);
  return info;

}

STATIC_ROUTINE NODELIST *FindTagWild(PINO_DATABASE *dblist, SEARCH_TERM *term)
{
  NODELIST *answer = NULL;
  TREE_INFO *tinfo;
  int treenum;
  char *tag_term = strdup(term->term);
  if (term->search_type == TAG_TREE) {
    char *treename=tag_term;
    char *tagname = strstr(tag_term, "::")+2;
    *(strstr(tag_term, "::")) = '\0';
    if (treename[0] == '\\') treename++;
    for (treenum = 0, tinfo=dblist->tree_info; tinfo; tinfo = tinfo->next_info, treenum++) {
      if(match(treename, tinfo->treenam)) {
        answer = ConcatenateNodeLists(answer, FindTags(dblist, tinfo, treenum, tagname));
      }
    }
  }
  else {
    TREE_INFO *default_tinfo = GetDefaultTreeInfo(dblist, &treenum);
    char *tagname = tag_term;
    if (tagname[0] == '\\') tagname++;
    answer = FindTags(dblist, default_tinfo, treenum, tagname);
    for (treenum=0, tinfo=dblist->tree_info; tinfo; tinfo = tinfo->next_info) {
      if (tinfo != default_tinfo) {
        answer = ConcatenateNodeLists(answer, FindTags(dblist, tinfo, treenum, tagname));
      }
      treenum++;
    }
  }
  free(tag_term);
  return answer;
}
STATIC_ROUTINE NODELIST *FindMembersOrChildren(PINO_DATABASE *dblist, SEARCH_TERM *term, NODE *start)
{
  NODELIST *answer = NULL;
  NODELIST *toDo = NULL;
  NODELIST *nptr;
  NODE *n;
  for (n=member_of(start); n; n=brother_of(dblist, n)) {
    toDo=AddNodeList(toDo, n);
    char *trimmed = Trim(n->name);
    if (match((strlen(term->term))?term->term:"*", trimmed)) {
      answer = AddNodeList(answer, n);
    }
    free(trimmed);
  }
  for (n=child_of(dblist, start); n; n=brother_of(dblist, n)) {
    toDo=AddNodeList(toDo, n);
    char *trimmed = Trim(n->name);
    if (match((strlen(term->term))?term->term:"*", trimmed)) {
      answer = AddNodeList(answer, n);
    }
    free(trimmed);
  }

  for (nptr=toDo; nptr; nptr=nptr->next)
    answer = ConcatenateNodeLists(answer, FindMembersOrChildren(dblist, term, nptr->node));
  FreeNodeList(toDo);
  return answer;
}
STATIC_ROUTINE NODELIST *RemoveTail(NODELIST *list)
{
  NODELIST *ptr = list;
  NODELIST *previous = NULL;
  while(ptr->next) {
    previous = ptr;
    ptr = ptr->next;
  }
  free(ptr);
  if (previous) previous->next = NULL;
  return (previous) ? list : NULL;
}

STATIC_ROUTINE NODELIST *FindMembers(PINO_DATABASE *dblist, SEARCH_TERM *term, NODE *start)
{
  NODELIST *answer = NULL;
  NODELIST *toDo = NULL;
  NODELIST *visited = NULL;
  NODELIST *ptr;
  NODE *n;

  toDo = AddNodeList(toDo, member_of(start));
  ptr = toDo;
  while(ptr) {
    if (NotIn(visited, ptr->node)) {
      char *trimmed = Trim(ptr->node->name);
      visited = AddNodeList(visited, ptr->node);
      if (match(term->term, trimmed)) {
        answer = AddNodeList(answer, ptr->node);
      }
      free(trimmed);
      if ((n=brother_of(dblist, ptr->node))) {
        toDo = AddNodeList(toDo, n);
      }
      ptr = ptr->next;
      if (! ptr)
        ptr = toDo;
    }
    else if ((n = member_of(ptr->node))) {
      toDo->node = n;
      ptr = toDo;
    }
    else {
      NODELIST *tmp;
      tmp = toDo;
      toDo = toDo->next;
      free(tmp);
      ptr = toDo;
    }
  }
  FreeNodeList(visited);
  return answer;
}

STATIC_ROUTINE int NotIn(NODELIST *list, NODE *node)
{
  NODELIST *ptr = list;
  while (ptr) {
    if (ptr->node == node) {
      return(0);
    }
    ptr = ptr->next;
  }
  return 1;
}

STATIC_ROUTINE NODELIST *FindChildren(PINO_DATABASE *dblist, SEARCH_TERM *term, NODE *start) 
{
  NODELIST *answer = NULL;
  NODELIST *toDo = NULL;
  NODELIST *nptr;
  NODE *n;
  for (n=child_of(dblist, start); n; n=brother_of(dblist, n)) {
    toDo=AddNodeList(toDo, n);
    char *trimmed = Trim(n->name);
    if (match((strlen(term->term))?term->term:"*", trimmed)) {
      answer = AddNodeList(answer, n);
    }
    free(trimmed);
  }
  for (nptr=toDo; nptr; nptr=nptr->next)
    answer = ConcatenateNodeLists(answer, FindChildren(dblist, term, nptr->node));
  FreeNodeList(toDo);
  return answer;
}


STATIC_ROUTINE NODELIST *AddNodeList(NODELIST *list, NODE *node)
{
  NODELIST *this_one = malloc(sizeof(NODELIST));
  this_one->node = node;
  this_one->next = NULL;
  if (list) {
    NODELIST *p;
    for (p = list; p->next; p = p->next);
    p->next = this_one;
  }
  else {
    list = this_one;
  }
  return (list);
}

STATIC_ROUTINE void FreeNodeList(NODELIST *list)
{
  NODELIST *ptr;
  for (ptr=list; ptr;)
  {
    NODELIST *nxt = ptr->next;
    free(ptr);
    ptr=nxt;
  }
}
/*
 * Trim trailing spaces from input nodename
 *
 * Note there will not be a \0
 * so allocate 12+1 and copy it in first
 */
STATIC_ROUTINE char *Trim(char *str)
{
  char *ans = calloc(sizeof(NODE_NAME)+1, 1);
  char *p;
  strncpy(ans, str, sizeof(NODE_NAME));
  for (p=ans; *p; p++) {
    if (isspace(*p)) {
      *p='\0';
      break;
    }
  }
  return ans;
}

STATIC_ROUTINE int match(char *first, char *second)
{
    // If we reach at the end of both strings, we are done
    if (*first == '\0' && *second == '\0')
        return 1;
 
    // Make sure that the characters after '*' are present
    // in second string. This function assumes that the first
    // string will not contain two consecutive '*'
    if (*first == '*' && *(first+1) != '\0' && *second == '\0')
        return 0;
 
    // If the first string contains '?', or current characters
    // of both strings match
    if (*first == '?' || *first == *second)
        return match(first+1, second+1);
 
    // If there is *, then there are two possibilities
    // a) We consider current character of second string
    // b) We ignore current character of second string.
    if (*first == '*')
        return match(first+1, second) || match(first, second+1);
    return 0;
}

STATIC_ROUTINE NODELIST  *ConcatenateNodeLists(NODELIST *start_list, NODELIST *end_list)
{
  NODELIST *p;
  NODELIST *answer;

  if (start_list) {
    for (p=start_list; p->next; p = p->next);
    p->next = end_list;
    answer = start_list;
  } else {
    answer = end_list;
  }
  return answer;
}

STATIC_ROUTINE NODELIST *Filter(NODELIST *list, int usage_mask) {
  NODELIST *ptr;
  NODELIST *previous=list;
  NODELIST *answer=list;
  for (ptr=list; ptr; previous = ptr, ptr = ptr->next) {
    if(!((1<<((ptr->node->usage == TreeUSAGE_SUBTREE_TOP) ? TreeUSAGE_SUBTREE : ptr->node->usage)) & usage_mask)) {
      NODELIST *tmp=ptr;
      if (ptr == answer) {
        answer = ptr->next;
        previous = answer;
      }
      else {
        previous->next=ptr->next;
        ptr = previous;
      }
      free(tmp);
    }
  }
  return answer;
}
extern int TreeFindParent(PINO_DATABASE *dblist, char *name, NODE **node, char **new_name, int *child)
{
  NODE *start = dblist->default_node;
  int status = TreeNORMAL;
  *node = NULL;
  char *parent_name = strdup(name);
  char *last_dot = strrchr(parent_name, '.');
  char *last_colon = strrchr(parent_name, ':');
  if ((last_dot == NULL) && (last_colon == NULL)) {
    *node = start;
    *new_name = strdup(name);
    *child=0;
  }
  else {
    NID nid;
    *child = 0;
    if (last_dot > last_colon)
    {
      *child  = 1;
      *new_name = strdup(last_dot+1);
      *last_dot = '\0';
    }
    else {
      *child = 0;
      *new_name = strdup(last_colon+1);
      *last_colon = '\0';
    }
    if (strlen(parent_name) == 0) {
      status = TreeNORMAL;
      *node = dblist->default_node;
    }
    else {
      status = _TreeFindNode(dblist, parent_name, (int *)&nid);
      if (status & 1) {
        *node = nid_to_node(dblist, &nid);
      }
    }
  }
  if (! *node) {
    status = TreeNNF;
  }
  free(parent_name);
  return status;
}
EXPORT char *_TreeAbsPath(void *dbid, char const *inpath)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  int nid;
  char *answer;
  if (!IS_OPEN(dblist))
    return NULL;
  if (dblist->remote)
    return AbsPathRemote(dblist, inpath);
  _TreeGetDefaultNid(dbid, &nid);
  answer = AbsPath(dbid, inpath, nid);
  _TreeSetDefaultNid(dbid, nid);
  return answer;
}

STATIC_ROUTINE char *AbsPath(void *dbid, char const *inpath, int nid_in)
{
  char *answer = NULL, *pathptr = NULL;
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  _TreeSetDefaultNid(dbid, nid_in);
  size_t len = strlen(inpath);
  if (len) {
    int nid;
    if (_TreeFindNode(dbid, inpath, &nid) & 1) {
      pathptr = _TreeGetPath(dbid, nid);
      len = strlen(pathptr);
    } else pathptr = strdup(inpath);
  } else {
    pathptr = _TreeGetPath(dbid, nid_in);
    len = strlen(pathptr);
  }
  if (pathptr[0] == '\\') {
    if (strstr(pathptr, "::")) {
      answer = strdup(pathptr);
    }
    else {
      char *treename = Treename(dblist, nid_in);
      char *answer = (char *)malloc(strlen(treename) + strlen(pathptr) + 2 + 1);
      strcpy(answer, "\\");
      strcat(answer, treename);
      strcat(answer, "::");
      strcat(answer, pathptr+1);
    }
  }
  else if ((strstr(pathptr, "-") == pathptr) || (strstr(pathptr, ".-") == pathptr)) {
      int nid;
      NODE node;
      NODE *nodeptr = &node;
      NID *nidptr = (NID *) & nid;
      NID *nidinptr = (NID *) & nid_in;
      if (nid_in) {
        nodeptr = nid_to_node(dblist, nidinptr);
        node_to_nid(dblist, parent_of(dblist, nodeptr), nidptr);
        answer = AbsPath(dbid, &pathptr[2], nid);
      } else
        answer = NULL;
  } else {
    char *tmp = _TreeGetPath(dbid, nid_in);
    answer = strcpy(malloc(strlen(tmp) + strlen(pathptr) + 2), tmp);
    free(tmp);
    switch (pathptr[0]) {
    case '.':
    case ':':
      strcat(answer, pathptr);
      break;
    default:
      strcat(answer, ":");
      strcat(answer, pathptr);
      break;
    }
  }
  free(pathptr);
  return answer;
}
STATIC_ROUTINE char *Treename(PINO_DATABASE * dblist, int nid_in)
{
  TREE_INFO *info;
  NID nid = *(NID *) & nid_in;
  unsigned int treenum;
  for (info = dblist->tree_info, treenum = 0; info && treenum < nid.tree; info = info->next_info) ;
  return info ? info->treenam : "";
}

