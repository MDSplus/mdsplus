%option outfile="mdsdcl/mdsdclDeltatimeToSeconds.c"
%option nostack noyymore noyywrap noyylineno nodefault
%option nounput noinput noyy_push_state noyy_pop_state noyy_top_state
%option never-interactive
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

typedef struct {
	int day;
	int error;
	int idx;
	int times[3];
} lex_t;

#define YYSTYPE		char *
#define YYLTYPE		lex_t
#define YY_DECL	int	yylex(lex_t* var)
#define YY_NO_UNPUT

%}

%%

[[:blank:]]+	;
[[:digit:]]+	if (var->idx==0) var->day=var->times[0]; var->times[var->idx]=atoi(yytext);
":"		if (var->idx>=2) yyterminate(); var->idx++;
.		var->error=1; yyterminate();
\n		yyterminate();

%%

int mdsdclDeltatimeToSeconds(const char *const deltatime) {
    lex_t lex = {0};
    int hour=0;
    int min=0;
    int sec=0;
    yy_scan_string(deltatime);
    yylex(&lex);
    yy_delete_buffer(YY_CURRENT_BUFFER);
    if (lex.error)
      return -1;
    switch (lex.idx) {
    case 0: sec=lex.times[0]; break;
    case 1: min=lex.times[0]; sec=lex.times[1]; break;
    case 2: hour=lex.times[0]; min=lex.times[1]; sec=lex.times[2]; break;
    };
    return ((lex.day * 24 + hour) * 60 + min) * 60 + sec;
}

#ifdef TEST_MAIN
static int test(const char* expr, const int val)
{
	const int res = mdsdclDeltatimeToSeconds(expr);
	if (res != val)
	{
		fprintf(stderr, "ERROR: %12d != %12d : '%s'\n", res, val, expr);
		return 0;
	}
	return 1;
}

int main(int narg, char args[])
{
	int res = 0;
	res = (res << 1) | test(" 1 ",                  1); // s
	res = (res << 1) | test(" 1 : 2 ",             62); // m:s
	res = (res << 1) | test(" 1 : 2 : 3 ",       3723); // H:m:s
	res = (res << 1) | test(" 1 : 2 : 3 : 4 ",   3723); // H:m:s:<ignored>
	res = (res << 1) | test(" 1 2 ",            86402); // day=1 + s
	res = (res << 1) | test(" 1 2 : 3 ",        86523); // day=1 + m:s
	res = (res << 1) | test(" 1 2 : 3 : 4 ",    93784); // day + H:m:s
	res = (res << 1) | test(" 1 2 : 3 : 4 : 5 ",93784); // day + H:m:s:<ignored>
	return res;
}
#endif
