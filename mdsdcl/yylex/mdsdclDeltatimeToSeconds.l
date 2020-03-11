%option outfile="mdsdcl/mdsdclDeltatimeToSeconds.c"
%option nostack noyymore noyywrap noyylineno nodefault
%option nounput noinput noyy_push_state noyy_pop_state noyy_top_state
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

#include <string.h>
#include <stdio.h>
#define YYSTYPE char *
#define YYLTYPE void
static int idx;
static int times[3];
static int day, hour, min, sec;
static int error;

%}

%x has_day
number [[:digit:]]+

%%
[[:blank:]]+ ;
^[[:blank:]]*/{number}[[:blank:]]+({number}|\:) BEGIN has_day;
<has_day>{number} day=atoi(yytext); BEGIN INITIAL;
{number} times[idx]=atoi(yytext);
\: if (idx>2) yyterminate(); idx++;
. error=1; yyterminate();

%%

int mdsdclDeltatimeToSeconds(char *deltatime) {
    error=0;
    idx=0;
    memset(times,0,sizeof(times));
    day=0;
    hour=0;
    min=0;
    sec=0;
    yy_scan_string(deltatime);
    yylex();
    yy_delete_buffer(YY_CURRENT_BUFFER);
    if (error)
      return -1;
    switch (idx) {
    case 0: sec=times[0]; break;
    case 1: min=times[0]; sec=times[1]; break;
    case 2: hour=times[0]; min=times[1]; sec=times[2]; break;
    };
    return day * 24 * 60 *60 + hour * 60 * 60 + min * 60 + sec; 
}
