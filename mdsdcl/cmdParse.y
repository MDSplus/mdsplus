%output  "mdsdcl/cmdParse.c"
%defines "mdsdcl/dclyacc.h"
%{
#define _GNU_SOURCE // required by rhel5 to include stpcpy
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mdsdcl_messages.h>
#include <mdsplus/mdsconfig.h>

#include "mdsdclthreadstatic.h"

#define YYLTYPE void *
#define yylex dcl_lex
#include "dcl_p.h"
#include "dcllex.h"

#define YYERROR_VERBOSE

static void yyerror(YYLTYPE *yyloc_param, yyscan_t yyscanner, dclCommandPtr *dclcmd, char **error, const char *s);
%}
%define api.pure full
%define api.value.type {union YYSTYPE}
%defines
%lex-param {YYLTYPE * yylloc_param}
%lex-param {yyscan_t yyscanner}

%parse-param {YYLTYPE * yylloc_param}
%parse-param {yyscan_t yyscanner}
%parse-param {dclCommandPtr *dclcmd}
%parse-param {char **error}

%token CMDFILE
%token VERB
%token QUALIFIER
%token EQUALS
%token VALUE
%token P_VALUE
%token COMMA
%token END
%token COMMENT

%type <cmd> command
%type <value_list> value_list
%type <value_list> pvalue_list
%type <qualifier> qualifier
%type <str> QUALIFIER VERB VALUE CMDFILE
%type <pvalue> P_VALUE
%start command

%%

command:
CMDFILE {
  $$=memset(malloc(sizeof(dclCommand)),0,sizeof(dclCommand));
  $$->verb=strdup("DO");
  $$->qualifier_count=1;
  $$->qualifiers=malloc(sizeof(dclQualifierPtr));
  $$->qualifiers[0]=memset(malloc(sizeof(dclQualifier)),0,sizeof(dclQualifier));
  $$->qualifiers[0]->name=strdup("INDIRECT");
  $$->parameter_count=1;
  $$->parameters=malloc(sizeof(dclParameterPtr));
  $$->parameters[0]=memset(malloc(sizeof(dclParameter)),0,sizeof(dclParameter));
  $$->parameters[0]->value_count=1;
  $$->parameters[0]->values=malloc(sizeof(char *));
  $$->parameters[0]->values[0]=strdup($CMDFILE);
  free($CMDFILE);
}
|VERB {
  $$=memset(malloc(sizeof(dclCommand)),0,sizeof(dclCommand));
  $$->verb=$VERB;
}
|COMMENT {YYACCEPT;}
|command qualifier {
  $qualifier->position=$$->parameter_count;
  if ($$->qualifier_count == 0)
    $$->qualifiers=malloc(sizeof(dclQualifierPtr));
  else
    $$->qualifiers=realloc($$->qualifiers,sizeof(dclQualifierPtr)*($$->qualifier_count+1));
  $$->qualifiers[$$->qualifier_count]=$qualifier;
  $$->qualifier_count++;
}
|command pvalue_list {
  dclParameterPtr param=memset(malloc(sizeof(dclParameter)),0,sizeof(dclParameter));
  param->value_count=$pvalue_list->count;
  param->values=$pvalue_list->values;
  param->restOfLine=$pvalue_list->restOfLine;
  free($pvalue_list);
  if ($$->parameter_count == 0)
    $$->parameters=malloc(sizeof(dclParameterPtr));
  else
    $$->parameters=realloc($$->parameters,sizeof(dclParameterPtr)*($$->parameter_count+1));
  $$->parameters[$$->parameter_count]=param;
  $$->parameter_count++;
}
|command END {
  *dclcmd=$$;
  YYACCEPT;
}

qualifier:
QUALIFIER {
  $$=memset(malloc(sizeof(dclQualifier)),0,sizeof(dclQualifier));
  $$->name=$QUALIFIER;
}
| qualifier EQUALS value_list {
  $$->value_count=$value_list->count;
  $$->values=$value_list->values;
  free($value_list);
}

value_list:
VALUE {
  char *value=$VALUE;
  $$=malloc(sizeof(dclValueList));
  $$->restOfLine=0;
  $$->count=1;
  $$->values=malloc(sizeof(char *));
  if (value[0]=='"' && value[strlen(value)-1]=='"') {
    char *nval;
    size_t i;
    value[strlen(value)-1]='\0';
    nval=strdup(value+1);
    free(value);
    for (i=0;i<strlen(nval);i++) {
      if (nval[i]=='"' && nval[i+1]=='"') {
	size_t j;
	for (j=i+1;j<strlen(nval);j++) {
	  nval[j]=nval[j+1];
	}
	i++;
      }
    }
    value=nval;
  }
  $$->values[0]=value;
}
| value_list COMMA VALUE {
  $$->values=realloc($$->values,sizeof(char *)*($$->count+1));
  $$->values[$$->count]=$VALUE;
  $$->count++;
}

pvalue_list:
P_VALUE {
  dclValuePtr dclvalue=$P_VALUE;
  char *value=dclvalue->value;
  $$=malloc(sizeof(dclValueList));
  $$->restOfLine=dclvalue->restOfLine;
  $$->count=1;
  $$->values=malloc(sizeof(char *));
  if (value[0]=='"' && value[strlen(value)-1]=='"') {
    char *nval;
    char *dq;
    value[strlen(value)-1]='\0';
    dq=nval=strdup(value+1);
    free(value);
    while((dq=strstr(dq,"\"\""))) {
      memmove(dq, dq+1, strlen(dq+1)+1);
      dq++;
    }
    value=nval;
  }
  free(dclvalue);
  $$->values[0]=value;
}
| pvalue_list COMMA P_VALUE {
  dclValuePtr dclvalue=$P_VALUE;
  free(dclvalue->restOfLine);
  $$->values=realloc($$->values,sizeof(char *)*($$->count+1));
  $$->values[$$->count]=dclvalue->value;
  $$->count++;
  free(dclvalue);
}

%%
static void yyerror(YYLTYPE *yyloc_param __attribute__ ((unused)),
		    yyscan_t yyscanner __attribute__ ((unused)),
		    dclCommandPtr *dclcmd __attribute__ ((unused)), char **error, const char *s) {
  if (error) {
    *error = realloc(*error, strlen(s)+16);
    sprintf(*error ,"mdsdcl: %s\n", s);
  }
}

EXPORT int mdsdcl_do_command_extra_args(char const* command, char **prompt, char **error, char **output, char *(*getline)(), void *getlineInfo) {
  dclCommandPtr dclcmd=0;
  YYLTYPE *yyloc_param=0;
  yyscan_t yyscanner;
  YY_BUFFER_STATE cmd_state;
  int result,status=MdsdclIVVERB;
  if (error && *error) {
    free(*error);
    *error = NULL;
  }
  if (output && *output) {
    free(*output);
    *output = NULL;
  }
  static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock(&lock);
  pthread_cleanup_push((void*)pthread_mutex_unlock,(void*)&lock);
  dcl_lex_init(&yyscanner);
  cmd_state = dcl__scan_string (command, yyscanner);
  result=yyparse (yyloc_param, yyscanner, &dclcmd, error);
  dcl__delete_buffer (cmd_state, yyscanner);
  dcl_lex_destroy(yyscanner);
  pthread_cleanup_pop(1);
  if (result==0) {
    if (dclcmd) {
      dclcmd->command_line=strdup(command);
      status=cmdExecute(dclcmd,prompt,error,output,getline, getlineInfo);
    } else
      status = 1;
  }
  return status;
}
