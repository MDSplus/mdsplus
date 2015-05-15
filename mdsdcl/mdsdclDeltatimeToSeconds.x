%{
#include <string.h>
#include <stdio.h>
#define YYSTYPE char *
#define YYLTYPE void
static int idx;
static int times[3];
static int day, hour, min, sec;
static int error;

%}

%option noyywrap nounput noinput outfile="mdsdclDeltatimeToSeconds.c"
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
