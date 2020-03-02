%option prefix="dcl_" outfile="cmdParseLex.c" header-file="dcllex.h"
%option nostack noyymore noyywrap noyylineno nodefault
%option unput noinput noyy_push_state noyy_pop_state noyy_top_state
%option interactive reentrant bison-bridge bison-locations case-insensitive
%{
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

#define yyterminate() return(END)
#include <string.h>
#include <stdio.h>
#define YYLTYPE void
#include "dcl_p.h"
#include <dcl.h>
#include "dclyacc.h"
int debug=0;
char *restOfLine=0;
%}

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
