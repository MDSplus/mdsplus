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

STATIC_ROUTINE int _TFN (void *dbid, char const *path, int *outnid);
STATIC_ROUTINE int _TFNWild(void *dbid,  char const *path, int *nid_out, void **ctx_inout, int usage_mask);
STATIC_ROUTINE NODELIST *Search(PINO_DATABASE *dblist, SEARCH_CTX *ctx, SEARCH_TERM *term, NODE *start);
STATIC_ROUTINE NODELIST *Find(PINO_DATABASE *dblist, SEARCH_TERM *term, NODE *start);
STATIC_ROUTINE NODELIST *AddNodeList(NODELIST *list, NODE *node);
STATIC_ROUTINE void FreeNodeList(NODELIST *list);
STATIC_ROUTINE int match(char *first, char *second);
STATIC_ROUTINE int _TFNEnd(void *dbid, void **ctx);
STATIC_ROUTINE int _TFN(void *dbid, char const *path, int *outnid);
STATIC_ROUTINE void FreeSearchCtx(SEARCH_CTX *ctx);
extern void FreeSearchTerms(SEARCH_TERM *terms);
STATIC_ROUTINE NODELIST *ConcatenateNodeLists(NODELIST *start_list, NODELIST *end_list);
STATIC_ROUTINE char *Trim(char *str);
STATIC_ROUTINE NODELIST *FindChildren(PINO_DATABASE *dblist, SEARCH_TERM *term, NODE *start);

EXPORT int WildParse(char const *path, SEARCH_CTX *ctx, int *wild);


EXPORT int TFN(char const *path, int *outnid)
{
  return _TFN(*TreeCtx(), path, outnid);
}

EXPORT int TFNWild(char const *path, int *nid_out, void **ctx_inout, int usage_mask)
{
   return _TFNWild(*TreeCtx(), path, nid_out, ctx_inout, usage_mask);
}

EXPORT int TFNEnd(void **ctx_in)
{
  return _TFNEnd(*TreeCtx(), ctx_in);
}
STATIC_ROUTINE int _TFNWild(void *dbid, char const *path, int *nid_out, void **ctx_inout, int usage_mask)
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

STATIC_ROUTINE int _TFN(void *dbid, char const *path, int *outnid)
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
    printf("No wildcards allowed in TreeFindNode\n");
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
static int _TFNEnd(void *dbid __attribute__ ((unused)), void **ctx)
{
  FreeSearchCtx(*ctx);
  free(ctx);
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


STATIC_ROUTINE NODELIST *Find(PINO_DATABASE *dblist, SEARCH_TERM *term, NODE *start)
{
  NODELIST *answer = NULL;
  switch (term->search_type) {
/*
    case (CHILD) : {
      NODE *n;
      char *match_str = (term->term[0]=='.') ? term->term+1 : term->term;
      for (n=child_of(dblist, start); n; n = brother_of(dblist, n)) {
        char *trimmed = Trim(n->name);
        if (match(match_str, trimmed)) {
          answer = AddNodeList(answer, n);
        }
        free(trimmed);
      }
      break;
    }
    case (MEMBER) : {
      NODE *n;
      char *match_str = (term->term[0]==':') ? term->term+1 : term->term;
      for (n=member_of(start); n; n = brother_of(dblist, n)) {
        char *trimmed = Trim(n->name);
        if (match(match_str, trimmed)) {
          answer = AddNodeList(answer, n);
        }
        free(trimmed);
      }
      break;
    }
*/
    case (CHILD) : 
    case (MEMBER) : 
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
    case (PARENT) :
    {
      answer = AddNodeList(answer, parent_of(dblist, start));
      break;
    }
/*
 * make this seperate and recursive
 *
    case (CHILD_SEARCH) :
    {
      NODE n;
      char *match_str=term->term+3;
      for (n=child_of(dblist, start); n; n = brother_of(dblist, n) {
        
      }
    }   
 */
    default: {
      printf("Search for type %d not implimented\n", term->search_type);
      return NULL;
      break;
    }
  }
  return(answer);
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
  if(list->next) {
    FreeNodeList(list->next);
  }
  free(list);
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
    if (isspace(*p)) *p='\0';
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
/*STATIC_ROUTINE int MatchWild(char *string, char *matchstring)
{
  char *sptr=string;
  char *mptr=matchstring;
  int match=1;
  while(*sptr && *mptr && match) {
    if (*sptr != *mptr) {
      if (*mptr == '*')
    }
  }
  int i;
  int j;
  int match = 1;
  int slen;
  for (slen = len; slen && string[slen - 1] == ' '; slen--) ;
  for (i = 0, j = 0; i < mlen && match; i++, j++) {
    if (matchstring[i] == '*') {
      if (i != (mlen - 1)) {
        if (j < slen) {
          for (;
            j < slen
            && !(match = MatchWild(&string[j], slen - j, &matchstring[i + 1], mlen - (i + 1)));
            j++) ;
        } else {
          for (; i < mlen && (match = matchstring[i] == '*'); i++) ;
        }
      }
      j = slen;
      break;
              } else {
       match = j < slen && ((matchstring[i] == '%') || (matchstring[i] == string[j]));
     }
   }
   return match && j == slen;
}
*/
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

