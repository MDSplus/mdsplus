%{

#define YY_USER_INIT yyin=descr_file;
#define YY_PRESERVE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <mds_stdarg.h>
#include "gen_device.h"
#include "gen_device.tab.h"
int curr_row = 1;
char *curr_ptr;
extern FILE *descr_file;
extern char *file_name;
/*
extern int yyback();
extern int yyinitio();
extern int yylook();
extern int yywrap();
*/
extern int TdiCompile();
void Error();
%}

blank		[\t ]
num		[0-9]
integer		[+\-]?{num}+
character	[a-zA-Z_$]
expo		[Ee][+\-]?{integer}
real		[+\-]?(([0-9]+(".")?[0-9]*)|("."[0-9]+))({expo})?
other		[()\[\]\*,:;=><\+\{\}\.\~\!\-\^\/\%\#\&\|\'\?\\\"]
string		\"({character}|{num}|{blank})*\"
%%
device		{return DEVICE;}
model		{return MODEL;}
library		{return LIBRARY;}
state		{return STATE;}
options		{return OPTIONS;}
write_once	{return WRITE_ONCE;}
on		{return ON;}
off		{return OFF;}
usage		{return USAGE;}
structure	{return STRUCTURE;}
action		{return ACTION;}
dispatch	{return DISPATCH;}
numeric		{return NUMERIC;}
signal		{return SIGNAL;}
task		{return TASK;}
text		{return TEXT;}
window		{return WINDOW;}
axis		{return AXIS;}
subtree		{return SUBTREE;}
any		{return ANY;}
tags		{return TAGS;}
with		{return WITH;}
undefined	{return UNDEFINED;}
datum		{return DATUM;}
value		{return VALUE;}
operation	{return OPERATION;}
routine		{return ROUTINE;}
time_out	{return TIME_OUT;}
sequence	{return SEQUENCE;}
phase		{return PHASE;}
server_class	{return SERVER_CLASS;}
completion_event {return COMPLETION_EVENT;}
int		{return INTEGER; }
float		{return FLOAT; }
string		{return STRING; }
valid		{return VALID; }
if		{return IF; }
in		{return IN; }
error_message	{return ERROR_MESSAGE; }
is		{return IS; }
requires	{return REQUIRES; }
array		{return ARRAY; }
compressible    {return COMPRESSIBLE; }
no_write_model	{return NO_WRITE_MODEL; }
no_write_shot	{return NO_WRITE_SHOT; }
include_in_pulse {return INCLUDE_IN_PULSE; }
{blank}		;
{character}({character}|{num})*	{curr_ptr = malloc(strlen(yytext)+1); strcpy(curr_ptr, yytext); gen_devicelval.ptr = curr_ptr; 
						return IDENTIFIER; }
[\.\:]{character}({character}|{num})*	{curr_ptr = malloc(strlen(yytext)+1); strcpy(curr_ptr, yytext); gen_devicelval.ptr = curr_ptr; 
						return PATH_ITEM; }
{integer}	{curr_ptr = malloc(strlen(yytext)+1); strcpy(curr_ptr, yytext); gen_devicelval.ptr = curr_ptr; return INT_NUMBER; }        
{real}		{curr_ptr = malloc(strlen(yytext)+1); strcpy(curr_ptr, yytext); gen_devicelval.ptr = curr_ptr; return FLOAT_NUMBER; }
{string}	{curr_ptr = malloc(strlen(yytext)+1); strcpy(curr_ptr, &yytext[1]); curr_ptr[strlen(yytext) - 2] = '\0';
				gen_devicelval.ptr = curr_ptr; return STRING_WORD; }
\n		{curr_row++; }
{other}		{return yytext[0]; }
.		{Error(" "); }
%%






char* DevSkipExpr(int *type)
{
    static char buffer[1024];
    char word[512], a;
    struct descriptor
	curr_d = {0, DTYPE_T, CLASS_S, 0};
    struct descriptor_xd
	xd = {0, DTYPE_DSC, CLASS_XD, 0, 0};
    int i = 0, j, prev_i, status;
    curr_d.pointer = buffer;
    while(1)
    {
	j = 0;
	do {	
	   a = buffer[i++] = word[j++] = input();
	   if(i >= 1024) Error("Expression buffer overflow");
	} while(a && ((a <= 'z')&&(a >= 'A')||(a >= '0') && (a <= '9'))&&(a != ';'));
	word[j-1] = '\0';
	if(!a || !strcmp(word, "state")||!strcmp(word, "options")||!strcmp(word, "usage")||!strcmp(word, "tags")||(a == ';'))
	{
	    i--;
	    if(!a || a== ';')
	    {
		unput(buffer[i]);
		buffer[i] = '\0';
	    }
	    else
	    {
		for(; i >= prev_i; i--)
		    unput(buffer[i]);
		buffer[prev_i] = '\0';
	    }
	    curr_d.length = strlen(buffer);
	    status = TdiCompile(&curr_d, &xd MDS_END_ARG);
	    if(!(status & 1)) Error("Error in expression");
	    if((xd.pointer->class != CLASS_S)&& (xd.pointer->class != CLASS_D))
		*type = EXP;
	    else
		switch(xd.pointer->dtype)  {
			case DTYPE_L :
			case DTYPE_W : *type = INT; break;
			case DTYPE_F : 
			case DTYPE_G : *type = FLO; break;
			case DTYPE_T : *type = STR; break;
			default		   : *type = EXP;
		}
	    MdsFree1Dx(&xd,0);
	    return buffer;
	}
	else
	{
	    do {
		a = buffer[i++] = input();
		if(i >= 1024) Error("Expression buffer overflow");
		} while ((a == ' ')||(a == '\t')||(a == '\n')); 
	    i--;
	    prev_i = i;
	    unput(buffer[i]);
	}
    }
}

void Error(char *message)
{
    static int first = 1;
    extern int wrong;

    wrong = 1;
    printf("Syntax error at line %d %s\n", curr_row, message);
    exit(-1);
}


/* void yyerror YY_ARGS((char *fmt, ...)) */
void gen_deviceerror (char *fmt, ...)
{
    Error(yytext);
}

int yywrap()
{
    return 1;
} 

