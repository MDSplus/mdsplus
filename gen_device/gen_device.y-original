%{
#include <stdio.h>
#include <usagedef.h>
#include "gen_device.h"
InDescr  * type;
extern void yyerror();
extern NodeDescr *bottom_node_list;
extern NodeDescr *DevConcat();
extern char      *DevConcatString();
extern void       DevFreeNodeList();
extern void       DevGenAdd();
extern void       DevGenH();
extern void       DevGenOperation();
extern void       DevGenPartName();
extern InDescr   *DevAddInSet();
extern InDescr   *DevInConcat();
extern InDescr   *DevNewAction();
extern InDescr   *DevNewConvert();
extern InDescr   *DevNewFlag();
extern InDescr   *DevNewInElement();
extern InDescr   *DevNewInElementNoConv();
extern InDescr   *DevNewInElementErr();
extern InDescr   *DevNewMember();
extern InDescr   *DevNewNode();
extern InDescr   *DevNewRange();
extern InDescr   *DevNewSet();
extern InDescr   *DevSetType();
extern InDescr   *DevSkipExpr();

#define DEV_ADD_IN_SET DevAddInSet
#define DEV_CONCAT DevConcat
#define DEV_CONCAT_STRING DevConcatString
#define DEV_FREE_NODE_LIST DevFreeNodeList
#define DEV_GEN_ADD DevGenAdd
#define DEV_GEN_H DevGenH
#define DEV_GEN_OPERATION DevGenOperation
#define DEV_GEN_PART_NAME DevGenPartName
#define DEV_IN_CONCAT DevInConcat
#define DEV_NEW_ACTION DevNewAction
#define DEV_NEW_CONVERT DevNewConvert
#define DEV_NEW_FLAG DevNewFlag
#define DEV_NEW_IN_ELEMENT DevNewInElement
#define DEV_NEW_IN_ELEMENT_NOCONV DevNewInElementNoConv
#define DEV_NEW_IN_ELEMENT_ERR DevNewInElementErr
#define DEV_NEW_MEMBER DevNewMember
#define DEV_NEW_NODE DevNewNode
#define DEV_NEW_RANGE DevNewRange
#define DEV_NEW_SET DevNewSet
#define DEV_SET_TYPE DevSetType
#define DEV_SKIP_EXPR DevSkipExpr

%}
%start gen_device

%union  {
  char *ptr;
  int  status;
  int  option;
  InDescr *dptr;
  NodeDescr *nptr;
}
%token DEVICE MODEL IDENTIFIER LIBRARY INTEGER STATE OPTIONS WRITE_ONCE ON OFF USAGE STRUCTURE ACTION DISPATCH FLOAT_NUMBER
%token NUMERIC SIGNAL TASK TEXT WINDOW AXIS SUBTREE ANY TAGS WITH UNDEFINED DATUM VALUE STRING FLOAT INT_NUMBER IN
%token OPERATION TIME_OUT SEQUENCE PHASE SERVER_CLASS COMPLETION_EVENT ACTION ON OPERATION REQUIRES VALID IF ERROR_MESSAGE IS
%token STRING_WORD PATH_ITEM ROUTINE ARRAY COMPRESSIBLE NO_WRITE_MODEL NO_WRITE_SHOT INCLUDE_IN_PULSE
%% 
gen_device
	: device_decl operations				{ DEV_FREE_NODE_LIST(bottom_node_list); }
        ;
	
device_decl
	: DEVICE MODEL '=' IDENTIFIER LIBRARY '=' IDENTIFIER body   
								{ DEV_GEN_ADD($4, $7, $8); 
								  DEV_GEN_H($4, $7, $8);
								  DEV_GEN_PART_NAME($4, $7, $8); 
								  printf("Parsing interface description\n");
								  bottom_node_list = (NodeDescr *)$8.ptr;}
	;

body	: members						{ $$ = $1; }
	| nodes							{ $$ = $1; }
	| members nodes						{ $$.nptr = DEV_CONCAT($1, $2); }
	;

nodes
	: node							{ $$ = $1; }
	| nodes node						{ $$.nptr = DEV_CONCAT($1, $2); }
	;

node	: node_def members					{ $$.nptr = DEV_CONCAT($1, $2); }
	;

node_def
	: INT_NUMBER IDENTIFIER flags				{ $$.dptr = DEV_NEW_NODE($1, $2, $3); }
	;

flags   : state options usage tags				{ $$.dptr = DEV_NEW_FLAG($1, $2, $3, $4); }
	;

state	:							{ $$.ptr = 0; }
	| STATE '=' statedef					{ $$ = $3; }
	;

options	:							{ $$.option = 0; }
	| OPTIONS '=' option_list				{ $$.option = $3.option; }
	;

option_list
	: option						{ $$.option = $1.option; }
	| option_list ',' option				{ $$.option = $1.option | $3.option; }
	;

option
	: WRITE_ONCE						{ $$.option = OPT_WRITE_ONCE; }
	| COMPRESSIBLE						{ $$.option = OPT_COMPRESSIBLE; }
	| NO_WRITE_MODEL					{ $$.option = OPT_NO_WRITE_MODEL; }
	| NO_WRITE_SHOT						{ $$.option = OPT_NO_WRITE_SHOT; }
	| INCLUDE_IN_PULSE					{ $$.option = OPT_INCLUDE_IN_PULSE; }
	;


statedef
	: ON							{ $$.option = 1; }			
	| OFF							{ $$.option = 0; }
	;

usage	:							{ $$.ptr = 0; }
	| USAGE '=' usagedef					{ $$ = $3; }
	;

usagedef
	: STRUCTURE						{ $$.ptr = "TreeUSAGE_STRUCTURE"; }
	| ACTION						{ $$.ptr = "TreeUSAGE_ACTION"; }
	| DEVICE						{ $$.ptr = "TreeUSAGE_DEVICE"; }
	| DISPATCH						{ $$.ptr = "TreeUSAGE_DISPATCH"; }
	| NUMERIC						{ $$.ptr = "TreeUSAGE_NUMERIC"; }
	| SIGNAL						{ $$.ptr = "TreeUSAGE_SIGNAL"; }
	| TASK							{ $$.ptr = "TreeUSAGE_TASK"; }
	| TEXT							{ $$.ptr = "TreeUSAGE_TEXT"; }
	| WINDOW						{ $$.ptr = "TreeUSAGE_WINDOW"; }
	| AXIS							{ $$.ptr = "TreeUSAGE_AXIS"; }
	| SUBTREE						{ $$.ptr = "TreeUSAGE_SUBTREE"; }
	| ANY							{ $$.ptr = "TreeUSAGE_ANY";  }
	;

tags	:							{ $$.dptr = 0; }
	| TAGS '=' IDENTIFIER					{ $$ = $3; }
	;


members	:							{ $$.dptr = 0; }
	| WITH '{' member_list '}'				{ $$ = $3; }
	;

member_list 
	: member						{ $$ = $1; }
	| member_list member					{ $$.nptr = DEV_CONCAT($1, $2);  }
	;

member  : path ':' spec flags ';'				{ $$.dptr = DEV_NEW_MEMBER($1, $3, $4, type); }
	;

spec	: undefined						{ $$ = $1; type = (InDescr *)UND; }
	| datum							{ $$ = $1; }
	| method						{ $$ = $1; type = (InDescr *)ACT;}
	;

undefined
	: UNDEFINED						{ type = (InDescr *)UND; }
	;

datum   : DATUM VALUE '=' expr					{ $$ = $4; }
	;


expr	:							{ $$.dptr = DEV_SKIP_EXPR(&type); }
	;

method  : ACTION OPERATION '=' IDENTIFIER timout sequence phase class completion			
								{ $$.dptr = DEV_NEW_ACTION($4, $5, $6, $7, $8, $9); }
	;

timout	:							{ $$.status = 0; }
	| TIME_OUT '=' INTEGER					{ $$ = $3; }
	;


sequence
	: SEQUENCE '=' INT_NUMBER				{ $$ = $3; }
	;

phase	: PHASE '=' IDENTIFIER					{ $$ = $3; }
	;

class	: SERVER_CLASS '=' IDENTIFIER				{ $$ = $3; }
	;

completion
	:							{ $$.dptr = 0; }
	| COMPLETION_EVENT '=' '<' IDENTIFIER '>'		{ $$ = $4; }
	;



operations
	:
	| operations operation
	;

operation 
	: OPERATION IDENTIFIER require_list			{ DEV_GEN_OPERATION($2, $3, 1); }	
	| ROUTINE IDENTIFIER require_list			{ DEV_GEN_OPERATION($2, $3, 0); }	
	;

require_list
	: REQUIRES '{' in_elements '}'				{ $$ = $3; }	
	;

in_elements
	: in_element						{ $$ = $1; }
	| in_elements in_element				{ $$.dptr = DEV_IN_CONCAT($1, $2); }
	;

in_element
	: path opt_error ';'					{ $$.dptr = DEV_NEW_IN_ELEMENT_NOCONV($1, $2); }
	| path conversion ';'					{ $$.dptr = DEV_NEW_IN_ELEMENT($1, $2); }
	;

opt_error
	:							{ $$.dptr = 0; }
	| ERROR_MESSAGE IS IDENTIFIER				{ $$ = $3; }
	;

conversion
	: ':' convert						{ $$ = $2; }
	;


convert
	: int_convert						{ $$ = $1; }						
	| float_convert						{ $$ = $1; }
	| string_convert					{ $$ = $1; }
	| int_array_convert					{ $$ = $1; }
	| float_array_convert					{ $$ = $1; }
	;


int_convert
	: INTEGER opt_int_validity				{ $$.dptr = DEV_SET_TYPE($2, INT); }
	| INTEGER opt_error					{ $$.dptr = DEV_SET_TYPE(DEV_NEW_IN_ELEMENT_ERR($2), INT); }
	;

int_array_convert
	: INTEGER ARRAY opt_error				{$$.dptr = DEV_SET_TYPE(DEV_NEW_IN_ELEMENT_ERR($3), INT_ARRAY); }
	;

opt_int_validity
	: VALID IF IN '[' int_range_set ']' ERROR_MESSAGE IS IDENTIFIER
								{ $$.dptr = DEV_NEW_CONVERT($5, $9); }
	;

int_range_set
	: int_range						{ $$ = $1; }
	| int_set						{ $$ = $1; }
	| int_set_key						{ $$ = $1; }
	;

int_range
	: INT_NUMBER '-' INT_NUMBER				{ $$.dptr = DEV_NEW_RANGE($1, $3); }
	;

int_set
	: INT_NUMBER						{ $$.dptr = DEV_NEW_SET($1, 0, 0); }
	| int_set ',' INT_NUMBER				{ $$.dptr = DEV_ADD_IN_SET($1, $3, 0, 0); }
	;

int_set_key
	: INT_NUMBER '-' '>' INT_NUMBER				{ $$.dptr = DEV_NEW_SET($1, $4, 1);  }
	| int_set_key ',' INT_NUMBER '-' '>' INT_NUMBER		{ $$.dptr = DEV_ADD_IN_SET($1, $3, $6, 1); }
	;

float_convert
	: FLOAT opt_float_validity				{ $$.dptr = DEV_SET_TYPE($2, FLO); }
	| FLOAT opt_error					{ $$.dptr = DEV_SET_TYPE(DEV_NEW_IN_ELEMENT_ERR($2), FLO); }
	;

float_array_convert
	: FLOAT ARRAY opt_error					{ $$.dptr = DEV_SET_TYPE(DEV_NEW_IN_ELEMENT_ERR($3), FLO_ARRAY); }
	;


opt_float_validity
	: VALID IF IN '[' float_range_set ']' ERROR_MESSAGE IS IDENTIFIER
								{ $$.dptr = DEV_NEW_CONVERT($5, $9); }
	;


float_range_set
	: float_range						{ $$ = $1; }
	| float_set						{ $$ = $1; }
	| float_set_key						{ $$ = $1; }
	;

float_range
	: FLOAT_NUMBER '-' FLOAT_NUMBER				{$$.dptr = DEV_NEW_RANGE($1, $3); }
	;

float_set
	: FLOAT_NUMBER							{ $$.dptr = DEV_NEW_SET($1, 0, 0); }
	| float_set ',' FLOAT_NUMBER					{ $$.dptr = DEV_ADD_IN_SET($1, $3, 0, 0); }
	;


float_set_key
	: FLOAT_NUMBER '-' '>' INT_NUMBER			{ $$.dptr = DEV_NEW_SET($1, $4, 1);  }
	| float_set_key ',' FLOAT_NUMBER '-' '>' INT_NUMBER	{ $$.dptr = DEV_ADD_IN_SET($1, $3, $6, 1); }
	;


string_convert
	: STRING opt_string_validity				{ $$.dptr = DEV_SET_TYPE($2, STR); }
	| STRING opt_error					{ $$.dptr = DEV_SET_TYPE(DEV_NEW_IN_ELEMENT_ERR($2), STR); }
	;

opt_string_validity
	: VALID IF IN '[' string_set ']' ERROR_MESSAGE IS IDENTIFIER
								{ $$.dptr = DEV_NEW_CONVERT($5, $9, STR); }
	| VALID IF IN '[' string_set_key ']' ERROR_MESSAGE IS IDENTIFIER
								{ $$.dptr = DEV_NEW_CONVERT($5, $9, STR); }
	;

string_set
	: STRING_WORD						{ $$.dptr = DEV_NEW_SET($1, 0, 0); }
	| string_set ',' STRING_WORD				{ $$.dptr = DEV_ADD_IN_SET($1, $3, 0, 0); }
	;


string_set_key
	: STRING_WORD '-' '>' INT_NUMBER				{ $$.dptr = DEV_NEW_SET($1, $4, 1);  }
	| string_set_key ',' STRING_WORD '-' '>' INT_NUMBER		{ $$.dptr = DEV_ADD_IN_SET($1, $3, $6, 1); }
	;


path
	: IDENTIFIER path_items						{ $$.ptr = DEV_CONCAT_STRING($1, $2); }
	| PATH_ITEM path_items						{ $$.ptr = DEV_CONCAT_STRING($1, $2); }
	;


path_items
	:								{$$.ptr = 0; }
	| path_items PATH_ITEM						{$$.ptr = DEV_CONCAT_STRING($1, $2); }
	;


