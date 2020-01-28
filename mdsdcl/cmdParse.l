%{
#define yyterminate() return(END)
#include <string.h>
#include <stdio.h>
#define YYLTYPE void
#include "dcl_p.h"
#include <dcl.h>
#include "cmdParse.tab.h"
int debug=0;
char *restOfLine=0;
%}

%option noinput reentrant bison-bridge bison-locations nodefault noyywrap yylineno outfile="cmdParseLex.c" header-file="dcllex.h"
%option case-insensitive interactive prefix="dcl_"

%x command verb qualifier qualifier_with_value qualifier_with_value_list qualval qualval_list rest_of_line parameter

name [[:alpha:][:alnum:]_]+
quoted_exc \"!\"
quoted_quote \"\"
quoted_value \"({quoted_exc}|{quoted_quote}|[^\"])*\"
unquoted_value_1 ({quoted_exc}|{quoted_quote}|[^[:blank:]\",=\/])+
unquoted_value_2 ({quoted_exc}|{quoted_quote}|[^[\"])*
unquoted_value_3 ({quoted_exc}|{quoted_quote}|[^[:blank:]\",=\/\(\)])+
value {unquoted_value_1}|{quoted_value}
qualval {unquoted_value_3}|{quoted_value}

%%

^[[:blank:]]*! {
  return(COMMENT);
  }

^[[:blank:]]*/{name} {
 if (debug) printf("Begin command\n");
 BEGIN command;
 }

^[[:blank:]]*@[^[:blank:]]+ {
 if (debug) printf("got DO/INDIRECT %s\n",yytext);
 yylval->str=strdup(yytext);
 BEGIN verb;
 return(CMDFILE);
 }
 
 /* <command>{name}/[[:blank:]/] {*/
<command>{name} {
 if (debug) printf("got verb %s\n",yytext);
 yylval->str=strdup(yytext);
 BEGIN verb;
 return(VERB);
 }

<verb>\/{name}/[[:blank:]]*=[[:blank:]]* {
 if (debug) printf("got qualifier with value %s\n",yytext);
 yylval->str=strdup(yytext+1);
 BEGIN qualifier_with_value;
 return(QUALIFIER);
 }

<verb>\/{name} {
 if (debug) printf("got qualifier %s\n",yytext);
 yylval->str=strdup(yytext+1);
 return(QUALIFIER);
 }

<qualifier_with_value>[[:blank:]]*=[[:blank:]]*\( {
 if (debug) printf("got a qualval_list\n");
 BEGIN qualval_list;
 return(EQUALS);
 }

<qualifier_with_value>[[:blank:]]*=[[:blank:]]* {
 if (debug) printf("got a qualval nolist\n");
 BEGIN qualval;
 return(EQUALS);
 }

<qualval_list>{qualval}/[[:blank:]]*\) {
 if (debug) printf("got last qualval |%s|\n",yytext);
 yylval->str=strdup(yytext);
 return(VALUE);
 }

<qualval_list>[[:blank:]]*\) {
 BEGIN verb;
 }

<qualval_list>{qualval}/[[:blank:]]*, {
 if (debug) printf("got a qualval=%s\n",yytext);
 yylval->str=strdup(yytext);
 return(VALUE);
 }

<qualval>{qualval} {
 if (debug) printf("got qualval |%s|\n",yytext);
 yylval->str=strdup(yytext);
 BEGIN verb;
 return(VALUE);
 }

<verb>! {
  yyterminate();
  }

<verb>[^[:blank:]] {BEGIN rest_of_line;
   unput(yytext[0]);
 }

<rest_of_line>.* {int i;
		 restOfLine = strdup(yytext);
		 for(i=strlen(restOfLine)-1;i>=0;i--)
		   unput(restOfLine[i]);
		 BEGIN parameter;
		 }
		 
<rest_of_line>EOL BEGIN parameter;

<parameter>{value} {
 dclValuePtr value=malloc(sizeof(dclValue));
 value->value=strdup(yytext);
 value->restOfLine=restOfLine;
 restOfLine=0;
 BEGIN verb;
 yylval->pvalue=value;
 return(PVALUE_);}

<*>, return(COMMA);

<*>[[:blank:]] ;

<*>.

%%
