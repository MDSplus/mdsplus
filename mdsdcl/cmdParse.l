%option prefix="dcl_" outfile="mdsdcl/cmdParseLex.c" header-file="mdsdcl/dcllex.h"
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
//#define DEBUG
#include <mdsmsg.h>
#define DBGM(RETURN,prefix)	MDSDBG("%-9s -> %-12s : '%s'", #RETURN, #prefix, yytext)
%}

%x start verb qualifier qualval qualval_list rest_of_line parameter

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

#		return(COMMENT);
!		return(COMMENT);

@[^[:blank:]]+ {
 DBGM(CMDFILE,verb);
 yylval->str=strdup(yytext+1);
 BEGIN verb;
 return(CMDFILE);
 }

{name} {
 DBGM(VERB,verb);
 yylval->str=strdup(yytext);
 BEGIN verb;
 return(VERB);
 }

<verb,qualifier>\/{name} {
 DBGM(QUALIFIER,qualifier);
 yylval->str=strdup(yytext+1);
 BEGIN qualifier;
 return(QUALIFIER);
 }

<qualifier>=[[:blank:]]*\( {
 DBGM(list,qualval_list);
 BEGIN qualval_list;
 return(EQUALS);
 }

<qualifier>= {
 DBGM(EQUALS,qualval);
 BEGIN qualval;
 return(EQUALS);
 }

<qualval_list>\) {
 DBGM(listend,verb);
 BEGIN verb;
 }

<qualval_list>{qualval} {
 DBGM(VALUE,qualval_list);
 yylval->str=strdup(yytext);
 return(VALUE);
 }

<qualval>{qualval} {
 DBGM(VALUE,verb);
 yylval->str=strdup(yytext);
 BEGIN verb;
 return(VALUE);
 }

<verb,qualifier>!		yyterminate();

<*>,		DBGM(COMMA,*);return(COMMA);

<*>[[:blank:]]+ DBGM(white,*);

<verb,qualifier>[^[:blank:]] {
 DBGM(start,rest_of_line);
 unput(yytext[0]);
 BEGIN rest_of_line;
 }

<rest_of_line>.* {
 DBGM(store,parameter);
 int i;
 yylval->str = strdup(yytext);
 for(i=strlen(yylval->str)-1;i>=0;i--)
   unput(yylval->str[i]);
 BEGIN parameter;
 }

<*>(EOL|\n)	MDSDBG("end       parameter    : 0x%02x",(int)yytext[0]);BEGIN parameter;

<parameter>{value} {
 DBGM(P_VALUE,verb);
 dclValuePtr value=malloc(sizeof(dclValue));
 value->value=strdup(yytext);
 value->restOfLine=yylval->str;
 BEGIN verb;
 yylval->pvalue=value;
 return(P_VALUE);
 }

<*>.		MDSDBG("any       *           : 0x%02x",(int)yytext[0]);

%%
