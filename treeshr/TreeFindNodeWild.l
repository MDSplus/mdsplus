%{
#include <STATICdef.h>
#include <stdio.h>
#include <ctype.h>
#include <treeshr_messages.h>
#include "treeshrp.h"

STATIC_ROUTINE void addSearchTerm(yyscan_t scanner, int type, char *str);

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
%option reentrant 
%option prefix="yytreepath"
%option extra-type="SEARCH_CTX *"
%%
^\\{alphaplus}{1,12}:{2}{alphaplus}{1,23}	{
  addSearchTerm(yyscanner, TAG_TREE, yyget_text( yyscanner ));
}
^\\{alphaplus}{1,23} {
  addSearchTerm(yyscanner, TAG, yyget_text( yyscanner ));
}
{child_search} {
  addSearchTerm(yyscanner, CHILD_SEARCH, yyget_text( yyscanner ));
}
{member_search} {
  addSearchTerm(yyscanner, MEMBER_SEARCH, yyget_text( yyscanner ));
}
{childstart}{0,1}{ancestor_search}{alphaplus}{0,12} {
  addSearchTerm(yyscanner, ANCESTOR_SEARCH, yyget_text( yyscanner ));
}
{childstart}{0,1}{child_or_member_search}|{old_child_or_member_search} {
  addSearchTerm(yyscanner, CHILD_OR_MEMBER_SEARCH, yyget_text( yyscanner ));
}
{childstart}{0,1}{parent} {
  addSearchTerm(yyscanner, PARENT, yyget_text( yyscanner ));
}
{childstart}{0,1}{ancestor}{alphaplus}{0,12} {
  addSearchTerm(yyscanner, ANCESTOR, yyget_text( yyscanner ));
}
:{0,1}{alphaplus}{1,12} {
  addSearchTerm(yyscanner, MEMBER, yyget_text( yyscanner ));
}
{childstart}{alphaplus}{1,12} {
  addSearchTerm(yyscanner, CHILD, yyget_text( yyscanner ));
}
{child_or_memberstart}{alphaplus}{1,12} {
  addSearchTerm(yyscanner, CHILD_OR_MEMBER, yyget_text( yyscanner ));
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
/*
STATIC_ROUTINE const char  *SearchTypeName(enum yytreepathtokentype typ) 
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
*/

STATIC_ROUTINE SEARCH_TERM *SquishSearches(SEARCH_TERM *terms) {
  SEARCH_TERM *ptr = terms;
  SEARCH_TERM *tmp;
  for (;ptr && ptr->next; ptr = ptr->next) {
    if (((ptr->search_type == CHILD_SEARCH) && (ptr->next->search_type == CHILD)) ||
       ((ptr->search_type == MEMBER_SEARCH) && (ptr->next->search_type == MEMBER)) ||
       ((ptr->search_type == CHILD_OR_MEMBER_SEARCH) && (ptr->next->search_type == CHILD_OR_MEMBER))) {
      char *str = calloc(strlen(ptr->term) + strlen(ptr->next->term) + 1, sizeof(char));
      strcpy(str, ptr->term);
      strcat(str, ptr->next->term);
      free(ptr->term);
      free(ptr->next->term);
      ptr->term = str;
      tmp = ptr->next;
      ptr->next=tmp->next;
      free(tmp);
    }
  }
  return terms;
}

EXPORT void FreeSearchTerms(SEARCH_TERM *terms)
{
  SEARCH_TERM *ptr = terms;
  SEARCH_TERM *next;
  while (ptr){
     next = ptr->next;
     free(ptr->term);
     free(ptr);
     ptr = next;
  }
}

EXPORT int WildParse(char const *path, SEARCH_CTX *ctx, int *wild) 
{
//  static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
  char *wild_path = strdup(path);
  char *p;
  int status;
  yyscan_t scanner;

  while ((p = strstr(wild_path, "***"))) {
      *p++ = '~';
      *p++ = '~';
      *p++ = '~';
  }

  yytreepathlex_init_extra(ctx, &scanner);
  ctx->wildcard = strdup(wild_path);

  yytreepath_scan_string(wild_path, scanner);

  while((status = yytreepathlex(scanner)) > 0) ;
  if (status == 0) {
    /* now merge in trailing search clauses */
    ctx->terms=SquishSearches(ctx->terms);
    /* check if any terms are wild */
    *wild = strchr(wild_path, '*') || 
            strchr(wild_path, '%') || 
            strstr(wild_path, "~~~") || 
            strstr(wild_path, "...") || 
            strstr(wild_path, ":::") || 
            strstr(wild_path, "^^^") ;
  }
  yytreepathlex_destroy(scanner);
  free(wild_path);

//  PrintCtx(ctx);

  return(status ==0) ? TreeNORMAL : 0;
}

STATIC_ROUTINE void addSearchTerm(yyscan_t scanner, int type, char *str)
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

  if (yyget_extra(scanner)->terms==NULL)
    yyget_extra(scanner)->terms=ptr;
  else {
    for (p=yyget_extra(scanner)->terms; p->next; p = p->next);
    p->next = ptr;
  }
}

