%{
  #include <stdio.h>
  #include <stdlib.h>
  int yydebug=0;
  #define YYLTYPE void *
  #define yylex dcl_lex
  #include "dcl_p.h"
  #include <dcl.h>
  #include "dcllex.h"
  static void yyerror(YYLTYPE *yyloc_param, yyscan_t yyscanner, dclCommandPtr *dclcmd, char *s);
%}
%define api.pure full
%define api.value.type {union YYSTYPE}
%defines
%lex-param {YYLTYPE * yylloc_param}
%lex-param {yyscan_t yyscanner}

%parse-param {YYLTYPE * yylloc_param}
%parse-param {yyscan_t yyscanner}
%parse-param {dclCommandPtr *dclcmd};

%token CMDFILE
%token VERB
%token QUALIFIER
%token EQUALS
%token VALUE
%token PVALUE
%token COMMA
%token END

%type <cmd> command
%type <value_list> value_list
%type <value_list> pvalue_list
%type <qualifier> qualifier
%type <str> QUALIFIER VERB VALUE CMDFILE
%type <pvalue> PVALUE

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
  $$->parameters[0]->values[0]=strdup($CMDFILE+1);
  free($CMDFILE);
}
|VERB {
  $$=memset(malloc(sizeof(dclCommand)),0,sizeof(dclCommand));
  $$->verb=$VERB;
}
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
    char *dq;
    value[strlen(value)-1]='\0';
    dq=nval=strdup(value+1);
    free(value);
    while((dq=strstr(dq,"\"\""))) {
      dq[1]='\0';
      dq=dq+1;
      strcat(nval,dq+1);      
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
PVALUE {
  dclValuePtr dclvalue=$PVALUE;
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
      dq[1]='\0';
      dq=dq+1;
      strcat(nval,dq+1);      
    }
    value=nval;
  }
  free(dclvalue);
  $$->values[0]=value;
}
| pvalue_list COMMA PVALUE {
  dclValuePtr dclvalue=$PVALUE;
  free(dclvalue->restOfLine);
  $$->values=realloc($$->values,sizeof(char *)*($$->count+1));
  $$->values[$$->count]=dclvalue->value;
  $$->count++;
  free(dclvalue);
}

%%
static void yyerror(YYLTYPE *yyloc_param, yyscan_t yyscanner, dclCommandPtr *dclcmd, char *s) {
   fprintf (stderr, "%s\n", s);
}

int mdsdcl_do_command_extra_args(char const* command, char **prompt, char **output, char **error) {
  dclCommandPtr dclcmd=0;
  YYLTYPE *yyloc_param=0;
  yyscan_t yyscanner;
  YY_BUFFER_STATE cmd_state;
  int result,status=CLI_STS_IVVERB;
  dcl_lex_init(&yyscanner);
  cmd_state = dcl__scan_string (command, yyscanner);
  result=yyparse (yyloc_param, yyscanner, &dclcmd);
  if (result==0) {
    dclcmd->command_line=strdup(command);
    status=cmdExecute(dclcmd,prompt,output,error);
  }
  dcl__delete_buffer (cmd_state, yyscanner);
  dcl_lex_destroy(yyscanner);
  return status;
}
  
