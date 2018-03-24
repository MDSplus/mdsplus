%{
#include <STATICdef.h>
#include <stdio.h>
#include <ctype.h>
#include "treeshrp.h"
static SEARCH_TERM *THE_LIST = NULL;

STATIC_ROUTINE void upcaseAndTrim(char *str)
{
  char *p=str;
  for (p=str; *p; p++) {
    *p = (isspace(*p) ? '\0' : toupper(*p));
  }
}

STATIC_ROUTINE int isPunctuation(char c)
{
  return ((c == '.') || (c == ':') || (c=='-') || (c=='~') | (c=='^'));
}
STATIC_ROUTINE void addSearchTerm(int type, char *str)
{
  SEARCH_TERM *ptr, *p;
//  printf("addSearchTerm\n");
  ptr=malloc(sizeof(SEARCH_TERM));
  ptr->search_type = type;
  ptr->term = NULL;
  ptr->next = NULL;
  if (str) {
    char *cptr = str;
    while(*cptr) {
      if (isPunctuation(*cptr)) {
        cptr++;
      } else {
       break; 
     }
    }
    ptr->term = strdup(cptr);
  }
  upcaseAndTrim(ptr->term);
  if (THE_LIST==NULL)
    THE_LIST=ptr;
  else {
    for (p=THE_LIST; p->next; p = p->next);
    p->next = ptr;
  }
}

%}

alphaplus     [a-zA-Z_0-9\*\?]
nodestart     [:a-zA-Z\*\?]
childstart    \.
child_or_memberstart ~
parent        \-
ancestor      \^
child_or_member ~
child_search \.\.\.
member_search :::
ancestor_search \^\^\^
child_or_member_search ~~~
old_child_or_member_search \*\*\*
newline \n

%option noinput
%option nounput
%option noyywrap
%option prefix="yytreepath"
%%
^\\{alphaplus}{1,12}:{2}{alphaplus}{1,23}	{
//  printf ("Found a tag tree /%s/\n", yytreepathtext);
  addSearchTerm(TAG_TREE, yytreepathtext);
}
^\\{alphaplus}{1,23} {
//  printf ("Found a tag (no tree) /%s/ returning %d\n", yytreepathtext, TAG);
  addSearchTerm(TAG, yytreepathtext);
}
{child_search} {
//  printf ("Found a Child Search\n");
  addSearchTerm(CHILD_SEARCH, yytreepathtext);
}
{member_search} {
//  printf ("Found a Member Search\n");
  addSearchTerm(MEMBER_SEARCH, yytreepathtext);
}
{ancestor_search} {
//  printf ("Found a Ancestor Search\n");
  addSearchTerm(ANCESTOR_SEARCH, yytreepathtext);
}
{child_or_member_search}|{old_child_or_member_search} {
//  printf ("Found a Child or Member Search\n");
  addSearchTerm(CHILD_OR_MEMBER_SEARCH, yytreepathtext);
}
{childstart}{0,1}{parent} {
//  printf ("Found a Parent\n");
  addSearchTerm(PARENT, yytreepathtext);
}
{ancestor} {
//  printf ("Found a Ancestor\n");
  addSearchTerm(ANCESTOR, yytreepathtext);
}
:{0,1}{alphaplus}{1,12} {
//  printf ("Found MEMBER /%s/\n", yytreepathtext);
  addSearchTerm(MEMBER, yytreepathtext);
}
{childstart}{alphaplus}{1,12} {
//  printf ("Found a CHILD /%s/\n", yytreepathtext);
  addSearchTerm(CHILD, yytreepathtext);
}
{child_or_memberstart}{alphaplus}{1,12} {
//  printf ("Found a child or member /%s/\n", yytreepathtext);
  addSearchTerm(CHILD_OR_MEMBER, yytreepathtext);
}
{newline} {
  return (0);
}
<<EOF>> {
  return (0);
}
. {
  return (-1);
}
%%
STATIC_ROUTINE const char  *SearchTypeName(enum yytokentype typ) 
{
  const char * names[] = {
    "TAG_TREE",
    "TAG", 
    "CHILD",
    "MEMBER",
    "CHILD_OR_MEMBER",
    "ANCESTOR",
    "ANCESTOR_SEARCH",
    "CHILD_SEARCH",
    "MEMBER_SEARCH",
    "CHILD_OR_MEMBER_SEARCH",
    "PARENT",
    "EOL"
  };
  return names[typ-TAG_TREE];
}


STATIC_ROUTINE void PrintCtx(SEARCH_CTX *ctx) 
{
  SEARCH_TERM *ptr;
  printf ("Print Context for %s\n", ctx->wildcard);
  for ( ptr=ctx->terms; 
        ptr; 
        ptr=ptr->next) {
    printf("\t%s -%s-\n", SearchTypeName(ptr->search_type), ptr->term);
  }
}

STATIC_ROUTINE SEARCH_TERM *SquishSearches(SEARCH_TERM *terms) {
  SEARCH_TERM *ptr = terms;
  for (;ptr->next; ptr = ptr->next)
    if ((ptr->search_type == ANCESTOR_SEARCH) ||
        (ptr->search_type == CHILD_SEARCH) ||
        (ptr->search_type == MEMBER_SEARCH) ||
        (ptr->search_type == CHILD_OR_MEMBER_SEARCH) )

    {
      char *ans = (char *)malloc(strlen(ptr->term)+strlen(ptr->next->term)+1);
      strcpy(ans, ptr->term);
      strcat(ans, ptr->next->term);
      free(ptr->term);
      free(ptr->next->term);
      ptr->term = ans;
      ptr->next = ptr->next->next;
    }
    return ptr;
}

EXPORT void FreeSearchTerms(SEARCH_TERM *terms)
{
  SEARCH_TERM *ptr = terms;
  SEARCH_TERM *next;
  while (ptr){
     next = ptr->next;
     free(ptr->term);
     ptr = next;
  }
}

EXPORT int WildParse(char const *path, SEARCH_CTX *ctx, int *wild) 
{
  char *wild_path = strdup(path);
  char *p;
  int status;
//  if (THE_LIST) {
//    FreeSearchTerms(THE_LIST);
//  }
  /* change *** to ~~~ */
  while ((p = strstr(wild_path, "***"))) {
      *p++ = '~';
      *p++ = '~';
      *p++ = '~';
  }
  ctx->wildcard = strdup(wild_path);
  yytreepath_scan_string(wild_path);
  while((status = yytreepathlex()) > 0) ;
  if (status == 0) {
    ctx->terms=THE_LIST;
    /* now merge in trailing search clauses */
//    ctx->terms=SquishSearches(ctx->terms);
    /* check if any terms are wild */
    *wild = strchr(wild_path, '*') || 
            strchr(wild_path, '%') || 
            strstr(wild_path, "~~~") || 
            strstr(wild_path, "...") || 
            strstr(wild_path, ":::") || 
            strstr(wild_path, "^^^") ;
  }
  free(wild_path);
  PrintCtx(ctx);

  THE_LIST =NULL;
  return(status ==0);
}
