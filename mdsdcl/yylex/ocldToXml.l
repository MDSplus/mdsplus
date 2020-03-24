%option outfile="mdsdcl/ocldToXml.c"
%option nostack noyymore noyywrap noyylineno
%option nounput noinput noyy_push_state noyy_pop_state noyy_top_state
%option case-insensitive
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

#define YYSTYPE  char *
#include <string.h>
#include <stdio.h>
#define YYLTYPE void
static inverb=0;
static intype=0;
static insyntax=0;
static void endSection();
%}

%x module verb parameter parameter_prompt parameter_label parameter_value parameter_value_type parameter_value_default type
%x routine keyword keyword_params keyword_value keyword_value_type keyword_value_default keyword_syntax syntax
%x qualifier qualifier_value qualifier_value_type qualifier_value_default qualifier_syntax


name [[:alpha:]]+[[:alnum:]_]+
quoted_value \"[^\"]*\"
value [^,\)]+
define_verb define[[:blank:]]+verb[[:blank:]]+
define_type define[[:blank:]]+type[[:blank:]]+
define_syntax define[[:blank:]]+syntax[[:blank:]]+

%%
<*>^\![^\n]*\n ;
module BEGIN module;
<module>{name} printf("<module name=\"%s\">\n",yytext); BEGIN INITIAL;
{define_verb} endSection(); BEGIN verb;
<verb>{name} printf("  <verb name=\"%s\">\n",yytext); inverb=1; BEGIN INITIAL;
routine[[:blank:]]+ BEGIN routine;
noqualifiers[[:blank:]\n]+
<routine>{name} printf("    <routine name=\"%s\"/>\n",yytext); BEGIN INITIAL;
parameter[[:blank:]]+ BEGIN parameter;
<parameter>{name} printf("    <parameter name=\"%s\"",yytext);
<parameter>,?[[:blank:]]*prompt[[:blank:]]*\= BEGIN parameter_prompt;
<parameter_prompt>\"[^\".]+\" printf(" prompt=\"%.*s\"",strlen(yytext)-2,yytext+1); BEGIN parameter;
<parameter>,?[[:blank:]]*label[[:blank:]]*\= BEGIN parameter_label;
<parameter_label>[[:alpha:]_]+ printf(" label=\"%s\"",yytext); BEGIN parameter;
<parameter>,?[[:blank:]]*value[[:blank:]]*\( BEGIN parameter_value;
<parameter>,?[[:blank:]]*value
<parameter_value>required+ printf(" required=\"True\"");
<parameter_value>list printf(" list=\"True\"");
<parameter_value>type[[:blank:]]*\= BEGIN parameter_value_type;
<parameter_value>default[[:blank:]]*\= BEGIN parameter_value_default;
<parameter_value_type>{name} printf(" type=\"%s\"",yytext); BEGIN parameter_value;
<parameter_value_default>{quoted_value} printf(" default=\"%.*s\"",strlen(yytext)-2,yytext+1); BEGIN parameter_value;
<parameter_value_default>{value} printf(" default=\"%s\"",yytext); BEGIN parameter_value;
<parameter_value>\) BEGIN parameter;
<parameter>\n printf("/>\n"); BEGIN INITIAL;
{define_type} endSection(); BEGIN type;
<type>{name} printf("  <type name=\"%s\">\n",yytext); intype=1; BEGIN INITIAL;
keyword[[:blank:]]+ BEGIN keyword;
<keyword>{name} printf("\"    <keyword name=\"%s\"",yytext); BEGIN keyword_params;
<keyword_params>,?[[:blank:]]*default[[:blank:]]* printf(" defaulted=\"True\"");
<keyword_params>,?[[:blank:]]*value[[:blank:]]*\( BEGIN keyword_value;
<keyword_value>type[[:blank:]]*\= BEGIN keyword_value_type;
<keyword_value>default[[:blank:]]*\= BEGIN parameter_value_default;
<keyword_value_type>{name} printf(" type=\"%s\"",yytext); BEGIN keyword_value;
<keyword_value_default>{quoted_value} printf(" default=\"%.*s\"",strlen(yytext)-2,yytext+1); BEGIN keyword_value;
<keyword_value_default>{value} printf(" default=\"%s\"",yytext); BEGIN keyword_value;
<keyword_params>,?[[:blank:]]*syntax[[:blank:]]*\= BEGIN keyword_syntax;
<keyword_syntax>{name} printf(" syntax=\"%s\"",yytext); BEGIN keyword_params;
<keyword_value>\) BEGIN keyword_params;
<keyword_params>\n printf("/>\"\n"); BEGIN INITIAL;
define[[:blank:]]+syntax[[:blank:]]+ endSection(); BEGIN syntax;
<syntax>{name} printf("  <syntax name=\"%s\">\n",yytext); insyntax=1; BEGIN INITIAL;
qualifier BEGIN qualifier;
<qualifier>{name} printf("    <qualifier name=\"%s\"",yytext);
<qualifier>,?[[:blank:]]*default printf(" defaulted=\"True\"");
<qualifier>,?[[:blank:]]*nonnegatable printf(" nonnegatable=\"True\"");
<qualifier>,?[[:blank:]]*value\( BEGIN qualifier_value;
<qualifier>,?[[:blank:]]*value ;
<qualifier>,?[[:blank:]]*syntax[[:blank:]]*\= BEGIN qualifier_syntax;
<qualifier_syntax>{name} printf(" syntax=\"%s\"",yytext); BEGIN qualifier;
<qualifier_value>,?[[:blank:]]*required printf(" required=\"True\"");
<qualifier_value>,?list printf(" list=\"True\"");
<qualifier_value>,?[[:blank:]]*type[[:blank:]]*\= BEGIN qualifier_value_type;
<qualifier_value>,?[[:blank:]]*default[[:blank:]]*\= BEGIN qualifier_value_default;
<qualifier_value_type>{name} printf(" type=\"%s\"",yytext); BEGIN qualifier_value;
<qualifier_value_default>{quoted_value} printf(" default=\"%.*s\"",strlen(yytext)-2,yytext+1); BEGIN qualifier_value;
<qualifier_value_default>{value} printf(" default=\"%s\"",yytext); BEGIN qualifier_value;
<qualifier_value>\) BEGIN qualifier;
<qualifier>\n printf("/>\n"); BEGIN INITIAL;
<*>. ;
%%

static void endSection() {
  if (inverb) {
     printf("\"  </verb>\n");
     inverb=0;
    }
  if (intype) {
     printf("\"  </type>\n");
     intype=0;
     }
  if (insyntax) {
     printf("\"  </syntax>\n");
     insyntax=0;
     }
}

int main(int argc, char **argv) {
    char *infile=argv[1];
    char *tablename=argv[2];
    char *outfile=strcpy(malloc(strlen(tablename)+5),tablename);
    strcat(outfile,".xml");
    freopen(infile,"r",stdin);
    freopen(outfile,"w",stdout);
    printf("<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n");
    yylex();
    endSection();
    printf("</module>\n");
    return 0;
    }
