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

STATIC_ROUTINE int IsMember(PINO_DATABASE * db, NODE * node);
STATIC_ROUTINE int Compare(char *string, int len, char *matchstring, int mlen);
STATIC_ROUTINE int CompareWild(char *string, int len, char *matchstring, int mlen);
STATIC_ROUTINE NODE *Pop(SEARCH_CONTEXT * search);
STATIC_ROUTINE void Push(SEARCH_CONTEXT * search, NODE * node);
STATIC_ROUTINE int Parse(SEARCH_CONTEXT * ctx, int wild);
STATIC_ROUTINE void ParseAction(SEARCH_CONTEXT * ctx, char **tree, int *treelen, int tokencnt,
				char *tokenptr, SEARCH_TYPE param);
STATIC_ROUTINE int TreeSearch(PINO_DATABASE * db, SEARCH_CONTEXT * ctx, int idx,
			      NODE ** node_in_out);
STATIC_ROUTINE char *AbsPath(void *dbid, char const *inpath, int nid_in);
STATIC_ROUTINE char *Treename(PINO_DATABASE * dblist, int nidin);
STATIC_ROUTINE int BsearchCompare(const void *this_one, const void *compare_one);

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

EXPORT char *TreeFindNodeTags(int nid_in, void **ctx_ptr)
{
  return _TreeFindNodeTags(*TreeCtx(), nid_in, ctx_ptr);
}

EXPORT char *TreeAbsPath(char const *inpath)
{
  return _TreeAbsPath(*TreeCtx(), inpath);
}

EXPORT int TreeFindTag(const char *tagnam, const char *treename, int *tagidx)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) * TreeCtx();
  NODE *nodeptr;
  return _TreeFindTag(*TreeCtx(), dblist->default_node, (short)strlen(treename), treename,
		      (short)strlen(tagnam), tagnam, &nodeptr, tagidx);
}

#define isident(b) ( ((*(b) >= 'A') && (*(b) <= 'Z')) || \
                     ((*(b) >= 'a') && (*(b) <= 'z')) || \
                     ((*(b) >= '0') && (*(b) <= '9')) || \
                      (*(b) == '$') || (*(b) == '_')   )
#define isast(b)   ( (*(b) == '*') )
#define iswild(b)  ( (*(b) == '%') || isast(b) )
#define isup(b)    ( (*(b) == '^') )
#define isdot(b)   ( (*(b) == '.') )
#define iscolon(b) ( (*(b) == ':') )
#define isslash(b) ( (*(b) == '\\') )
#define ishyphen(b) ( (*(b) == '-') )
#define not_eos(b) ( b < end_ptr )
#define do_action(x) {ctx++,ParseAction(ctx,&tree,&treelen,tokencnt,tokenptr,(x));}
#define compare(node)      Compare(node->name,12,search->string,search->len)
#define compare_wild(node) CompareWild(node->name,12,search->string,search->len)
#define FirstChild(node) (member_of(node) ? member_of(node) : child_of(db, node))
#define FirstChildOfParent(node) (parent_of(db, node) ? child_of(db, parent_of(db, node)) : child_of(db, node))
#define SiblingOf(node) (brother_of(db, node) ? brother_of(db, node) : ((parent_of(db, node) && IsMember(db, node)) ? child_of(db, parent_of(db, node)) : 0))
#define match_usage \
( (ctx[ctx->level+1].type != EOL) || \
  ((1<<((node->usage == TreeUSAGE_SUBTREE_TOP) ? TreeUSAGE_SUBTREE : node->usage)) & usage_mask) )

EXPORT int _TreeFindNode(void *dbid, char const *path, int *outnid)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid = (NID *) outnid;
  int status = TreeNORMAL;
  NODE *node;
  SEARCH_CONTEXT *ctx;
  int current_level;
  size_t i;
  size_t len = strlen(path);
  if (!IS_OPEN(dblist))
    return TreeNOT_OPEN;
  if (dblist->remote)
    return FindNodeRemote(dblist, path, outnid);
  ctx = calloc(sizeof(SEARCH_CONTEXT), MAX_SEARCH_LEVELS);
  ctx->type = EOL;
  ctx->string = strdup(path);
  for (i = 0; i < len && path[i] != ' '; i++)
    ctx->string[i] = (char)toupper(path[i]);
  ctx->string[i] = 0;
  status = Parse(ctx, 0);
  if STATUS_OK {
    node = dblist->default_node;
    ctx[0].node = node;
    for (current_level = 1; ctx[current_level].type != EOL && (status & 1); current_level++)
      status = TreeSearch(dblist, ctx, current_level, &node);
  }
  _TreeFindNodeEnd(dbid, (void **)&ctx);
  if STATUS_OK
    node_to_nid(dblist, node, nid);
  return status;
}

EXPORT int _TreeFindNodeWild(void *dbid, char const *path, int *nid_out, void **ctx_inout, int usage_mask)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid = (NID *) nid_out;
  SEARCH_CONTEXT *ctx = *(SEARCH_CONTEXT **) ctx_inout;
  int status = TreeNORMAL;
  NODE *node; /*************** static????????? ***********/
  SEARCH_CONTEXT *not_first_time = ctx;
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  if (dblist->remote)
    return FindNodeWildRemote(dbid, path, nid_out, ctx_inout, usage_mask);

/********************************************
  If there is no context now then need to
  parse and set up stuff for a new search.
**********************************************/

  if (ctx == 0) {
    *ctx_inout = malloc(sizeof(SEARCH_CONTEXT) * MAX_SEARCH_LEVELS);
    ctx = *(SEARCH_CONTEXT **) ctx_inout;
    if (ctx) {
      size_t i;
      size_t len = strlen(path);
      memset(ctx, 0, sizeof(SEARCH_CONTEXT) * MAX_SEARCH_LEVELS);
      ctx->type = EOL;
      ctx->string = malloc(len + 1);
      for (i = 0; i < len && path[i] != ' '; i++)
	ctx->string[i] = (char)toupper(path[i]);
      ctx->string[i] = 0;
      status = Parse(ctx, 1);
    } else
      status = TreeMEMERR;
    if (status == TreeNORMAL) {
      node = dblist->default_node;
      ctx->level = 1;
      ctx[0].node = node;
    }
  } else {
  /********************************************
    Otherwise this is an old search.  So start
    it where the other search left off (check for
    the '.' case.
  **********************************************/

    for (ctx->level = 1; ctx[ctx->level].type != EOL; ctx->level++) ;
    if (ctx->level > 1) {
      (ctx->level)--;
      node = ctx[ctx->level].node;
    } else
      status = TreeNMN;
  }

/********************************************
  If OK so far then
    For each level starting as above until
    failure or success do.
**********************************************/
  if (status == TreeNORMAL) {
    for (; ctx[ctx->level].type != EOL;) {
    /********************************************
      Look for this thing.  If OK then move
      on to next thing, otherwise move back to
      previous one.
    ********************************************/

      while (((status = TreeSearch(dblist, ctx, ctx->level, &node)) == TreeNORMAL)
	     && !match_usage) ;
      if (status == TreeNORMAL)
	(ctx->level)++;
      else {
	ctx[ctx->level].node = 0;
	(ctx->level)--;
	if (ctx->level >= 0)
	  node = ctx[ctx->level].node;
      }
    }
  }
  if (status == TreeNORMAL) {
    node_to_nid(dblist, node, nid);
  } else if ((status == TreeNNF) && not_first_time)
    status = TreeNMN;
  return status;
}

STATIC_ROUTINE int TreeSearch(PINO_DATABASE * db, SEARCH_CONTEXT * ctx, int idx,
			      NODE ** node_in_out)
{
  int tagidx;
  NODE *node = *node_in_out;
  SEARCH_CONTEXT *search = ctx + idx;
  switch (search->type) {
  case BROTHER_TYPE_NOWILD:
    {
      for (node = *node_in_out, node = search->node ? brother_of(db, node) : child_of(db, node);
	   node && !compare(node); node = brother_of(db, node)) ;
      if (node == 0)
	for (node = *node_in_out, node = search->node ? brother_of(db, node) : member_of(node);
	     node && !compare(node); node = brother_of(db, node)) ;
      break;
    }
  case BROTHER_TYPE:
    {
      for (node = search->node ? brother_of(db, node) : child_of(db, node);
	   node && !compare_wild(node); node = brother_of(db, node)) ;
      break;
    }
  case MEMBER_TYPE_NOWILD:
    {
      for (node = *node_in_out, node = search->node ? brother_of(db, node) : member_of(node);
	   node && !compare(node); node = brother_of(db, node)) ;
      if (node == 0)
	for (node = *node_in_out, node = search->node ? brother_of(db, node) : child_of(db, node);
	     node && !compare(node); node = brother_of(db, node)) ;
      break;
    }
  case MEMBER_TYPE:
    {
      for (node = search->node ? brother_of(db, node) : member_of(node);
	   node && !compare_wild(node); node = brother_of(db, node)) ;
      break;
    }
  case SON_MEMBER_TYPE_NOWILD:
    {
      for (node = search->node ? SiblingOf(node) : FirstChild(node); node && !compare(node);
	   node = SiblingOf(node)) ;
      break;
    }
  case SON_MEMBER_TYPE:
    {
      for (node = search->node ? SiblingOf(node) : FirstChild(node); node && !compare_wild(node);
	   node = SiblingOf(node)) ;
      break;
    }
  case TAG_TYPE:
    {
      if (search->node)
	node = 0;
      else
	_TreeFindTag(db, node, search->tag_tree_name_len, search->tag_tree_name, search->len,
		     search->string, &node, &tagidx);
      break;
    }
  case ANCESTOR_TYPE:
    {
      for (node = parent_of(db, node); node && !compare(node); node = parent_of(db, node)) ;
      break;
    }
  case PARENT_TYPE:
    {
      node = search->node ? 0 : parent_of(db, node);
      break;
    }
  case SON_BROTHER_TYPE:
    {
      if (search->node == 0) {
	if (child_of(db, node)) {
	  search->stop = node;
	  node = child_of(db, node);
	}
      } else if (brother_of(db, node)) {
	node = brother_of(db, node);
      } else {
	int looking_for_child = 1;
	node = FirstChildOfParent(node);
	while (node && (node != search->stop)) {
	  if (looking_for_child) {
	    if (child_of(db, node)) {
	      node = child_of(db, node);
	      break;
	    } else if (brother_of(db, node))
	      node = brother_of(db, node);
	    else {
	      looking_for_child = 0;
	      node = parent_of(db, node);
	    }
	  } else {
	    if (brother_of(db, node)) {
	      looking_for_child = 1;
	      node = brother_of(db, node);
	    } else
	      node = parent_of(db, node);
	  }
	}
	if (node == search->stop)
	  node = 0;
      }
      break;
    }
  case SON_MEMBER_BROTHER_TYPE:
    {
      if (search->node == 0) {
	if ((search + 1)->type == EOL)
	  node = FirstChild(node);
	search->stack = 0;
      } else if (SiblingOf(node))
	node = SiblingOf(node);
      else if (search->stack) {
	node = Pop(search);
	node = FirstChild(node);
      } else
	node = 0;
      if (node) {
	if (FirstChild(node))
	  Push(search, node);
      }
      break;
    }
  default:
    break;
  }
  *node_in_out = node;
  search->node = node;
  return node ? TreeNORMAL : TreeNNF;
}

/*************************************
  Pop(sctx) -
    pops the first node off of the que
  of nodes to be looked at.  When the Q
  is empty returns 0.
***************************************/
STATIC_ROUTINE NODE *Pop(SEARCH_CONTEXT * search)
{
  NODE *ans;
  NODELIST *ptr;
  if (search->stack) {
    ans = search->stack->node;
    ptr = search->stack;
    search->stack = ptr->next;
    free(ptr);
  } else
    ans = 0;
  return ans;
}

/****************************************************
   Push(sctx, node) -
     pushes a node onto the que of places that need 
   to be explored in the future of this *** search.
*****************************************************/
STATIC_ROUTINE void Push(SEARCH_CONTEXT * search, NODE * node)
{
  NODELIST *node_list = malloc(sizeof(NODELIST));
  node_list->next = 0;
  node_list->node = node;
  if (search->stack == 0)
    search->stack = node_list;
  else {
    NODELIST *end_of_list;
    for (end_of_list = search->stack; end_of_list->next; end_of_list = end_of_list->next) ;
    end_of_list->next = node_list;
  }
}

STATIC_ROUTINE int Compare(char *string, int len, char *matchstring, int mlen)
{
  int i;
  int match;
  if (mlen > len)
    match = 0;
  else if ((mlen == len) || (string[mlen] == ' ')) {
    for (i = 0; i < mlen && string[i] == matchstring[i]; i++) ;
    match = i == mlen;
  } else
    match = 0;
  return match;
}

STATIC_ROUTINE int CompareWild(char *string, int len, char *matchstring, int mlen)
{
  int i;
  int j;
  int match = 1;
  int slen;
  for (slen = len; slen && string[slen - 1] == ' '; slen--) ;
  for (i = 0, j = 0; i < mlen && match; i++, j++) {
    if (matchstring[i] == '*') {
      if (i != (mlen - 1)) {
	if (j < slen)
	  for (;
	       j < slen
	       && !(match = CompareWild(&string[j], slen - j, &matchstring[i + 1], mlen - (i + 1)));
	       j++) ;
	else
	  for (; i < mlen && (match = matchstring[i] == '*'); i++) ;
      }
      j = slen;
      break;
    } else
      match = j < slen && ((matchstring[i] == '%') || (matchstring[i] == string[j]));
  }
  return match && j == slen;
}

STATIC_ROUTINE int IsMember(PINO_DATABASE * db, NODE * node)
{
  NODE *n = 0;
  if (parent_of(db, node))
    for (n = member_of(parent_of(db, node)); n && n != node; n = brother_of(db, n)) ;
  return n == node;
}

STATIC_ROUTINE int Parse(SEARCH_CONTEXT * ctx, int wild)
{
  int status;
  char *char_ptr = ctx->string;
  char *end_ptr = ctx->string + strlen(ctx->string);
  int wild_ast;
  char *tree = NULL;
  int treelen = 0;
  SEARCH_TYPE state = NONE;
  int tokencnt = 1;
  char *tokenptr = char_ptr;

/**************************************************
 If the string is empty then assume error otherwise
 assume success
***************************************************/

  if (not_eos(char_ptr))
    status = TreeNORMAL;
  else
    return TreeEMPTY;

/**************************************************
 For each character in
 path name check for the
 various types of path name tokens.
***************************************************/

  while (not_eos(char_ptr)) {

  /********************************************
   See if the character is an alphanumeric.
   If current parse state is none, go to ident
   state and load the TOKEN starting address.
   Increment the tokencnt.
  ********************************************/

    if (isident(char_ptr)) {
      switch (state) {
      case NONE:
	state = MEMBER_TYPE_NOWILD;
	tokencnt = 1;
	tokenptr = char_ptr;
	break;

      case MEMBER_START:
	state = MEMBER_TYPE_NOWILD;
	tokencnt = 1;
	tokenptr = char_ptr;
	break;

      case ASTASTAST_TYPE:
	state = SON_MEMBER_TYPE_NOWILD;
	tokencnt = 1;
	tokenptr = char_ptr;
	break;

      case BROTHER_START:
	state = BROTHER_TYPE_NOWILD;
	tokencnt = 1;
	tokenptr = char_ptr;
	break;

      case BROTHER_TYPE_NOWILD:
      case MEMBER_TYPE_NOWILD:
	if (++tokencnt > 12)
	  return TreePARSEERR;
	break;

      case TAG_TYPE:
	if (++tokencnt > 24)
	  return TreePARSEERR;
	break;

      default:
	tokencnt++;
	break;
      }
    }

  /************************************************
   See if the character is a wild card character.
   If current parse state is none, zero the token
   size count and load the token starting address.
   If current parse state is none or ident set
   the state to wild. If current parse state is
   wild or up its ok but don't need to do anything.
   Otherwise we must return parse failure.
  *************************************************/

    else if (isast(char_ptr) && wild) {
      wild_ast = 1;
      if (not_eos(char_ptr + 2))
	if (isast(char_ptr + 1) && isast(char_ptr + 2)) {
	  wild_ast = 0;
	  switch (state) {
	  case NONE:
	  case ASTASTAST_TYPE:
	    {
	      do_action(SON_MEMBER_BROTHER_TYPE);
	      state = ASTASTAST_TYPE;
	      char_ptr += 2;
	      break;
	    }
	  case MEMBER_START:
	    {
	      tokencnt = 1;
	      tokenptr = char_ptr;
	      do_action(MEMBER_TYPE);
	      state = NONE;
	      char_ptr--;
	      break;
	    }
	  case BROTHER_START:
	    {
	      tokencnt = 1;
	      tokenptr = char_ptr;
	      do_action(BROTHER_TYPE);
	      state = NONE;
	      char_ptr--;
	      break;
	    }
	  default:
	    {
	      do_action(state);
	      state = NONE;
	      char_ptr--;
	      break;
	    }
	  }
	}
      if (wild_ast) {
	switch (state) {
	case NONE:
	case MEMBER_START:
	  tokencnt = 0;
	  tokenptr = char_ptr;
	  MDS_ATTR_FALLTHROUGH
	case MEMBER_TYPE_NOWILD:
	  state = MEMBER_TYPE;
	  break;
	case BROTHER_START:
	  tokencnt = 0;
	  tokenptr = char_ptr;
	  MDS_ATTR_FALLTHROUGH
	case BROTHER_TYPE_NOWILD:
	  state = BROTHER_TYPE;
	  break;

	case ASTASTAST_TYPE:
	  tokencnt = 0;
	  tokenptr = char_ptr;
	  MDS_ATTR_FALLTHROUGH
	case SON_MEMBER_TYPE_NOWILD:
	  state = SON_MEMBER_TYPE;
	  break;
	case BROTHER_TYPE:
	case MEMBER_TYPE:
	case SON_MEMBER_TYPE:
	case ANCESTOR_TYPE:
	  break;
	default:
	  return TreeNNF;
	}
      }
      tokencnt++;
    }

  /************************************************
   See if the character is a wild card character.
   If current parse state is none, zero the token
   size count and load the token starting address.
   If current parse state is none or ident set
   the state to wild. If current parse state is
   wild or up its ok but don't need to do anything.
   Otherwise we must return parse failure.
  *************************************************/

    else if (iswild(char_ptr) && wild) {
      switch (state) {
      case NONE:
      case MEMBER_START:
	tokencnt = 0;
	tokenptr = char_ptr;
	MDS_ATTR_FALLTHROUGH
      case MEMBER_TYPE_NOWILD:
	state = MEMBER_TYPE;
	break;
      case BROTHER_START:
	tokencnt = 0;
	tokenptr = char_ptr;
	MDS_ATTR_FALLTHROUGH
      case BROTHER_TYPE_NOWILD:
	state = BROTHER_TYPE;
	break;
      case ASTASTAST_TYPE:
	tokencnt = 0;
	tokenptr = char_ptr;
	MDS_ATTR_FALLTHROUGH
      case SON_MEMBER_TYPE_NOWILD:
	state = SON_MEMBER_TYPE;
	break;
      case BROTHER_TYPE:
      case MEMBER_TYPE:
      case SON_MEMBER_TYPE:
      case ANCESTOR_TYPE:
	break;
      default:
	return TreePARSEERR;
      }
      tokencnt++;
    }

  /*************************************************
   See if the charcter is a period. If the current
   parse state is none, check to see if it is an
   ... sequence and if it is ... do parse action
   otherwise do nothing.
   If in another parse state, do the parse action,
   set the state to none and back up the character
   pointer to reparse the period.
   *************************************************/

    else if (isdot(char_ptr))
      if (state == NONE) {
	if (not_eos(char_ptr + 1)) {
	  if (isdot(char_ptr + 1) && not_eos(char_ptr + 2)) {
	    if (isdot(char_ptr + 2)) {
	      do_action(SON_BROTHER_TYPE);
	      char_ptr += 2;
	    } else
	      return TreePARSEERR;
	  } else {
	    state = BROTHER_START;
	  }
	} else
	  return TreePARSEERR;
      } else {
	if (state != ASTASTAST_TYPE) {
	  do_action(state);
	}
	state = NONE;
	char_ptr--;
      }

  /**************************************************
   See if it is a \ character.
   If current state is none and there is at least
   one alphanumeric following the \, then start a tag
   state.
   Otherwise return a parsing error.
  ***************************************************/

    else if (isslash(char_ptr))
      if (state == NONE) {
	if (not_eos(char_ptr + 1)) {
	  if (isident(char_ptr + 1)) {
	    state = TAG_TYPE;
	    tokencnt = 0;
	    tokenptr = char_ptr + 1;
	  } else
	    return TreePARSEERR;
	} else
	  return TreePARSEERR;
      } else
	return TreePARSEERR;

  /***************************************************
   See if it a colon.
   See if it is a member or tag colon.
   If current state is tag and there is another colon
   and an ident, do the treenam action and start the
   token over for the remainder of the tag name. Any
   other condition should return a parsing error.
  ***************************************************/

    else if (iscolon(char_ptr))
      switch (state) {
      case TAG_TYPE:
	if (not_eos(char_ptr + 2)) {
	  if (iscolon(char_ptr + 1) && isident(char_ptr + 2)) {
	    tokencnt += 2;
	    do_action(TAG_TREE_TYPE);
	    ctx--;
	    tokencnt = 0;
	    tokenptr = char_ptr + 2;
	    char_ptr++;
	  } else {
	    do_action(state);
	    state = NONE;
	    char_ptr--;
	  }
	} else {
	  do_action(state);
	  state = NONE;
	  char_ptr--;
	}
	break;

      case NONE:
      case ASTASTAST_TYPE:
	if (not_eos(char_ptr + 1))
	  state = MEMBER_START;
	else
	  return TreePARSEERR;
	break;
      case MEMBER_START:
	return TreePARSEERR;
	break;
      case BROTHER_START:
	return TreePARSEERR;
	break;
      default:
	do_action(state);
	state = NONE;
	char_ptr--;
	break;
      }

  /***************************************************
   See if it is a carat character ^.
   If current state is none and there is at least one
   alphanumeric or wild character following, start an
   up state. Any other condition should return a parse
   error.
  ****************************************************/

    else if (isup(char_ptr))
      if (state == NONE || state == ASTASTAST_TYPE) {
	if (not_eos(char_ptr + 1)) {
	  if (isident(char_ptr + 1) || iswild(char_ptr + 1)) {
	    state = ANCESTOR_TYPE;
	    tokencnt = 0;
	    tokenptr = char_ptr + 1;
	  } else
	    return TreePARSEERR;
	} else
	  return TreePARSEERR;
      } else
	return TreePARSEERR;

  /****************************************************
   See if it is a hyphen character. If current state
   is none then do the hyphen action. Any other condition
   should return a parsing error.
  ******************************************************/

    else if (ishyphen(char_ptr))
      if (state == NONE || state == ASTASTAST_TYPE) {
	do_action(PARENT_TYPE);
      } else if (state == BROTHER_START) {
	do_action(PARENT_TYPE);
	state = NONE;
      } else
	return TreePARSEERR;
    else
      return TreePARSEERR;
    char_ptr++;
  }
  if ((state != MEMBER_START) && (state != NONE) && (state != ASTASTAST_TYPE)
      && (state != BROTHER_START))
    do_action(state);
  ctx++;
  ctx->type = EOL;
  return status;
}

STATIC_ROUTINE void ParseAction(SEARCH_CONTEXT * ctx, char **tree, int *treelen, int tokencnt,
				char *tokenptr, SEARCH_TYPE param)
{
  switch (param) {

  case TAG_TREE_TYPE:
    {				/*   case tree name in tag */
      *treelen = tokencnt - 2;	/*      save away the tree name for later */
      *tree = tokenptr;
      break;
    }

  default:
    {
      ctx->len = tokencnt>0x7fff ? 0x7fff : (short)tokencnt;
      ctx->string = tokenptr;
      ctx->type = param;
      ctx->node = 0;
      ctx->stop = 0;
      if (param == TAG_TYPE) {	/* if it is a tag search */
	ctx->tag_tree_name_len = (short)*treelen;	/*   fill in the saved tree name stuff */
	ctx->tag_tree_name = *tree;
      }
    }
  }
  return;
}

EXPORT int _TreeFindNodeEnd(void *dbid, void **ctx_in)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  SEARCH_CONTEXT *ctx = (SEARCH_CONTEXT *) * ctx_in;
  int status = TreeNORMAL;
  if (dblist && dblist->remote)
    return FindNodeEndRemote(dblist, ctx_in);
  if (ctx) {
    NODELIST *ptr;
    int i;
    if (ctx->string)
      free(ctx->string);
    for (i = 0; i < MAX_SEARCH_LEVELS; i++)
      if ((ctx[i].type == SON_MEMBER_BROTHER_TYPE) && ctx[i].stack)
	for (ptr = ctx[i].stack; ptr;) {
	  NODELIST *tmp = ptr;
	  ptr = ptr->next;
	  free(tmp);
	}
    free(ctx);
    *ctx_in = NULL;
  } else
    status = TreeNO_CONTEXT;

  return status;
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
  char *answer = NULL;
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
      answer = strncpy(malloc(i + 1), name, i);
      answer[i] = '\0';
      *ctx = swapint((char *)&(info_ptr->tag_info + *ctx - 1)->tag_link);
      if (*ctx == 0)
	*ctx = -1;
    } else
      *ctx = 0;
  } else
    *ctx = 0;
  return answer;
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
  char *answer = NULL;
  char *tmppath = NULL;
  SEARCH_CONTEXT ctx[MAX_SEARCH_LEVELS];
  memset(ctx, 0, sizeof(SEARCH_CONTEXT) * MAX_SEARCH_LEVELS);
  FREE_ON_EXIT(ctx->string);
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  _TreeSetDefaultNid(dbid, nid_in);
  FREE_ON_EXIT(tmppath);
  const char *pathptr;
  size_t len;
  size_t i;
  if (strlen(inpath)) {
    int nid;
    if (_TreeFindNode(dbid, inpath, &nid) & 1)
      pathptr = tmppath = _TreeGetPath(dbid, nid);
    else pathptr = inpath;
  } else
    pathptr = tmppath = _TreeGetPath(dbid, nid_in);
  len = strlen(pathptr);
  ctx->type = EOL;
  ctx->string = malloc(len + 1);
  for (i = 0; i < len && pathptr[i] != ' '; i++)
    ctx->string[i] = (char)toupper(pathptr[i]);
  ctx->string[i] = 0;
  if (Parse(ctx, 0) & 1) {
    if (ctx[1].type == TAG_TYPE) {
      if (ctx[1].tag_tree_name_len) {
	answer = ctx->string;
	ctx->string = NULL;
      } else {
	answer = malloc(16 + strlen(pathptr));
	strcpy(answer, "\\");
	strcat(answer, Treename(dblist, nid_in));
	strcat(answer, "::");
	strcat(answer, pathptr + 1);
      }
    } else if (ctx[1].type == PARENT_TYPE) {
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
  }
  FREE_NOW(ctx->string);
  FREE_NOW(tmppath);
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

EXPORT int TreeFindParent(PINO_DATABASE * dblist, char *path_ptr, NODE ** node_ptrptr, char **namedsc_ptr,
		   SEARCH_TYPE * type_ptr)
{
  SEARCH_CONTEXT ctx[MAX_SEARCH_LEVELS];
  int status;
  NODE *node_ptr;
  int ctxidx;
  size_t len = strlen(path_ptr);
  size_t i;
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  status = TreeNORMAL;
  memset(ctx, 0, sizeof(SEARCH_CONTEXT) * MAX_SEARCH_LEVELS);
  ctx->type = EOL;
  ctx->string = strcpy(malloc(len + 1), path_ptr);
  for (i = 0; i < len && path_ptr[i] != ' '; i++)
    ctx->string[i] = (char)toupper(path_ptr[i]);
  ctx->string[i] = 0;
  status = Parse(ctx, 0);
  if (status & 1) {
    node_ptr = dblist->default_node;
    ctxidx = 1;
    for (; ctx[ctxidx + 1].type != EOL;) {
    /********************************************
       Look for this thing.  If OK then move
       on to next thing, otherwise move back to
       previous one.
    ********************************************/

      status = TreeSearch(dblist, ctx, ctxidx, &node_ptr);
      if (status & 1)
	ctxidx++;
      else {
	ctx[ctxidx].node = NULL;
	ctxidx--;
	if (ctxidx >= 0)
	  node_ptr = ctx[ctxidx].node;
      }
    }
    if (status & 1) {
      *namedsc_ptr = strcpy(malloc(strlen(ctx[ctxidx].string) + 1), ctx[ctxidx].string);
      *type_ptr = ctx[ctxidx].type;
      *node_ptrptr = node_ptr;
    } else {
      *namedsc_ptr = NULL;
      *type_ptr = NONE;
      *node_ptrptr = NULL;
    }
  }
  if (ctx->string)
    free(ctx->string);
  return status;
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
