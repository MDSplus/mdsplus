#include        "clisysdef.h"

		/* Filename: mdsdcl_tables.c
		 * created by cdu :  07-Apr-1998 13:01:40
		 ********************************************************/

extern int   MDSDCL_ACTIVATE_IMAGE();

static struct cduVerb  ACTIVATE_IMAGE = {	/* Syntax def	*/
        "ACTIVATE_IMAGE",0x0002,0,0,MDSDCL_ACTIVATE_IMAGE,0,0
       };

static struct cduValue  v01q01value = {
        0x3000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v01qualifiers[2] = {
        "IMAGE",0x2000,0,&ACTIVATE_IMAGE,&v01q01value,0
       ,0				/* null entry at end	*/
       };
extern int   MDSDCL_ATTACH();

static struct cduValue  v02p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v02params[2] = {
        "P1",0x1000,"PROCESS","Process",&v02p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  ATTACH_PID = {	/* Syntax def	*/
        "ATTACH_PID",0x0002,0,0,MDSDCL_ATTACH,0,0
       };

static struct cduValue  v02q01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v02qualifiers[2] = {
        "IDENTIFICATION",0x2000,0,&ATTACH_PID,&v02q01value,0
       ,0				/* null entry at end	*/
       };
extern int   MDSDCL_CREATE_LIBRARY();

static struct cduValue  CREATE_LIBRARY_p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  CREATE_LIBRARY_p02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  CREATE_LIBRARY_params[3] = {
        "P1",0x1000,0,0,&CREATE_LIBRARY_p01value,0
       ,"P2",0x1000,"LIBRARY","Library",&CREATE_LIBRARY_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  CREATE_LIBRARY = {	/* Syntax def	*/
        "CREATE_LIBRARY",0x0000,0,0,MDSDCL_CREATE_LIBRARY,CREATE_LIBRARY_params,0
       };

static struct cduKeyword  CREATE_KEYWORDS[2] = {	/* "Type" def	*/
        "LIBRARY",0x4000,0,&CREATE_LIBRARY,0,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v03p01value = {
        0x2001,0,CREATE_KEYWORDS, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v03params[2] = {
        "P1",0x1000,0,"What",&v03p01value,0
       ,0				/* null entry at end	*/
       };
extern int   MDSDCL_NOOP();

static struct cduValue  v04p01value = {
        0x0020,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v04params[2] = {
        "P1",0x1000,0,0,&v04p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v04q01value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v04q02value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v04q03value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v04q04value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v04q05value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v04q06value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v04q07value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v04q08value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v04q09value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v04q10value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v04qualifiers[13] = {
        "TABLES",0x2000,0,0,&v04q01value,0
       ,"HELPLIB",0x2000,0,0,&v04q02value,0
       ,"AUXHELPLIB",0x2000,0,0,&v04q03value,0
       ,"PROMPT",0x2000,0,0,&v04q04value,0
       ,"DEF_FILE",0x2000,0,0,&v04q05value,0
       ,"KEY_DEFS",0x2000,0,0,&v04q06value,0
       ,"DEF_LIBRARIES",0x2000,0,0,&v04q07value,0
       ,"COMMAND",0x2000,0,0,&v04q08value,0
       ,"INI_DEF_FILE",0x2000,0,0,&v04q09value,0
       ,"LIBRARY",0x2000,0,0,&v04q10value,0
       ,"CLOSE",0x2000,0,0,0,0
       ,"KEEP",0x2000,0,0,0,0
       ,0				/* null entry at end	*/
       };
extern int   mdsdcl_debug();
extern int   mdsdcl_define();

static struct cduValue  v06p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v06params[2] = {
        "P1",0x1000,0,"Macro",&v06p01value,0
       ,0				/* null entry at end	*/
       };
extern int   MDSDCL_DEFINE_KEY();

static struct cduValue  DEFINE_KEY_p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  DEFINE_KEY_p02value = {
        0x2008,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  DEFINE_KEY_params[3] = {
        "P1",0x1000,"KEY","Key name",&DEFINE_KEY_p01value,0
       ,"P2",0x1000,0,"equivalence-name",&DEFINE_KEY_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  DEFINE_KEY_q03value = {
        0x1000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  DEFINE_KEY_q06value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier DEFINE_KEY_qualifiers[8] = {
        "ECHO",0x2001,0,0,0,0
       ,"ERASE",0x2000,0,0,0,0
       ,"IF_STATE",0x2000,0,0,&DEFINE_KEY_q03value,0
       ,"LOCK_STATE",0x2000,0,0,0,0
       ,"LOG",0x2001,0,0,0,0
       ,"SET_STATE",0x2000,0,0,&DEFINE_KEY_q06value,0
       ,"TERMINATE",0x2000,0,0,0,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  DEFINE_KEY = {	/* Syntax def	*/
        "DEFINE_KEY",0x0000,0,0,MDSDCL_DEFINE_KEY,DEFINE_KEY_params,DEFINE_KEY_qualifiers
       };
extern int   mdsdcl_define_symbol();

static struct cduValue  DEFINE_SYMBOL_p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  DEFINE_SYMBOL_p02value = {
        0x2008,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  DEFINE_SYMBOL_params[3] = {
        "P1",0x1000,"SYMBOL","Symbol name",&DEFINE_SYMBOL_p01value,0
       ,"P2",0x1000,"VALUE","Value",&DEFINE_SYMBOL_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  DEFINE_SYMBOL_REST_p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  DEFINE_SYMBOL_REST_p02value = {
        0x2020,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  DEFINE_SYMBOL_REST_params[3] = {
        "P1",0x1000,"SYMBOL","Symbol name",&DEFINE_SYMBOL_REST_p01value,0
       ,"P2",0x1000,"VALUE","Value",&DEFINE_SYMBOL_REST_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  DEFINE_SYMBOL_REST = {	/* Syntax def	*/
        "DEFINE_SYMBOL_REST",0x0000,0,0,mdsdcl_define_symbol,DEFINE_SYMBOL_REST_params,0
       };

static struct cduQualifier DEFINE_SYMBOL_qualifiers[2] = {
        "REST_OF_LINE",0x2000,0,&DEFINE_SYMBOL_REST,0,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  DEFINE_SYMBOL = {	/* Syntax def	*/
        "DEFINE_SYMBOL",0x0000,0,0,mdsdcl_define_symbol,DEFINE_SYMBOL_params,DEFINE_SYMBOL_qualifiers
       };

static struct cduQualifier v06qualifiers[4] = {
        "KEY",0x2000,0,&DEFINE_KEY,0,0
       ,"SYMBOL",0x2000,0,&DEFINE_SYMBOL,0,0
       ,"EDIT",0x2000,0,0,0,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v07p01value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v07params[2] = {
        "P1",0x1000,0,0,&v07p01value,0
       ,0				/* null entry at end	*/
       };
extern int   MDSDCL_DELETE_KEY();

static struct cduValue  DELETE_KEY_p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  DELETE_KEY_params[2] = {
        "P1",0x1000,"KEY","Key",&DELETE_KEY_p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  DELETE_KEY_ALL_q03value = {
        0x1000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier DELETE_KEY_ALL_qualifiers[4] = {
        "ALL",0x2001,0,0,0,0
       ,"LOG",0x2001,0,0,0,0
       ,"STATE",0x2000,0,0,&DELETE_KEY_ALL_q03value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  DELETE_KEY_ALL = {	/* Syntax def	*/
        "DELETE_KEY_ALL",0x0002,0,0,MDSDCL_DELETE_KEY,0,DELETE_KEY_ALL_qualifiers
       };

static struct cduValue  DELETE_KEY_q03value = {
        0x1000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier DELETE_KEY_qualifiers[4] = {
        "ALL",0x2000,0,&DELETE_KEY_ALL,0,0
       ,"LOG",0x2001,0,0,0,0
       ,"STATE",0x2000,0,0,&DELETE_KEY_q03value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  DELETE_KEY = {	/* Syntax def	*/
        "DELETE_KEY",0x0000,0,0,MDSDCL_DELETE_KEY,DELETE_KEY_params,DELETE_KEY_qualifiers
       };

static struct cduQualifier v07qualifiers[2] = {
        "KEY",0x2000,0,&DELETE_KEY,0,0
       ,0				/* null entry at end	*/
       };
extern int   mdsdcl_do_macro();

static struct cduValue  v08p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v08p02value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v08p03value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v08p04value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v08p05value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v08p06value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v08p07value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v08p08value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v08params[9] = {
        "P1",0x1000,"NAME","Macro",&v08p01value,0
       ,"P2",0x1000,"P1",0,&v08p02value,0
       ,"P3",0x1000,"P2",0,&v08p03value,0
       ,"P4",0x1000,"P3",0,&v08p04value,0
       ,"P5",0x1000,"P4",0,&v08p05value,0
       ,"P6",0x1000,"P5",0,&v08p06value,0
       ,"P7",0x1000,"P6",0,&v08p07value,0
       ,"P8",0x1000,"P7",0,&v08p08value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v08q03value = {
        0x0004,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v08qualifiers[4] = {
        "MACRO",0x2001,0,0,0,0
       ,"INDIRECT",0x2000,0,0,0,0
       ,"REPEAT",0x2000,0,0,&v08q03value,0
       ,0				/* null entry at end	*/
       };
extern int   mdsdcl_exit();
extern int   MDSDCL_FINISH();
extern int   MDSDCL_HELP();

static struct cduValue  v11p01value = {
        0x0020,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v11params[2] = {
        "P1",0x1000,0,0,&v11p01value,0
       ,0				/* null entry at end	*/
       };
extern int   mdsdcl_init_timer();
static struct cduKeyword  INIT_KEYWORDS[2];	/* Prototype only	*/

static struct cduValue  INIT_TIMER_p01value = {
        0x2001,0,INIT_KEYWORDS, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  INIT_TIMER_params[2] = {
        "P1",0x1000,0,"What",&INIT_TIMER_p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  INIT_TIMER = {	/* Syntax def	*/
        "INIT_TIMER",0x0004,0,0,mdsdcl_init_timer,INIT_TIMER_params,0
       };

static struct cduKeyword  INIT_KEYWORDS[2] = {	/* "Type" def	*/
        "TIMER",0x4000,0,&INIT_TIMER,0,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v12p01value = {
        0x2001,0,INIT_KEYWORDS, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v12params[2] = {
        "P1",0x1000,0,"What",&v12p01value,0
       ,0				/* null entry at end	*/
       };
extern int   MDSDCL_MODIFY();

static struct cduValue  v13p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v13params[2] = {
        "P1",0x1000,0,"Macro",&v13p01value,0
       ,0				/* null entry at end	*/
       };
extern int   MDSDCL_SAVE_MACRO();

static struct cduValue  v14p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v14params[2] = {
        "P1",0x1000,0,"Macro",&v14p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v14q01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v14q02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v14qualifiers[3] = {
        "NAME",0x2000,0,0,&v14q01value,0
       ,"LIBRARY",0x2000,0,0,&v14q02value,0
       ,0				/* null entry at end	*/
       };
extern int   mdsdcl_set_command();

static struct cduValue  SET_COMMAND_p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  SET_COMMAND_p02value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SET_COMMAND_params[3] = {
        "P1",0x1000,0,0,&SET_COMMAND_p01value,0
       ,"P2",0x1000,"TABLE",0,&SET_COMMAND_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  SET_COMMAND_q01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  SET_COMMAND_q02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  SET_COMMAND_q03value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier SET_COMMAND_qualifiers[4] = {
        "HELPLIB",0x2000,0,0,&SET_COMMAND_q01value,0
       ,"PROMPT",0x2000,0,0,&SET_COMMAND_q02value,0
       ,"DEF_FILE",0x2000,0,0,&SET_COMMAND_q03value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SET_COMMAND = {	/* Syntax def	*/
        "SET_COMMAND",0x0000,0,0,mdsdcl_set_command,SET_COMMAND_params,SET_COMMAND_qualifiers
       };
extern int   MDSDCL_SET_KEY();

static struct cduValue  SET_KEY_p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SET_KEY_params[2] = {
        "P1",0x1000,0,0,&SET_KEY_p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  SET_KEY_q02value = {
        0x0000,"DEFAULT",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };
extern int   MDSDCL_SET_KEYBOARD_APPLICATION();

static struct cduQualifier SET_KEY_APPLICATION_qualifiers[2] = {
        "APPLICATION",0x2001,0,0,0,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SET_KEY_APPLICATION = {	/* Syntax def	*/
        "SET_KEY_APPLICATION",0x0000,0,0,MDSDCL_SET_KEYBOARD_APPLICATION,0,SET_KEY_APPLICATION_qualifiers
       };

static struct cduQualifier SET_KEY_NOAPPLICATION_qualifiers[2] = {
        "APPLICATION",0x2000,0,0,0,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SET_KEY_NOAPPLICATION = {	/* Syntax def	*/
        "SET_KEY_NOAPPLICATION",0x0000,0,0,MDSDCL_SET_KEYBOARD_APPLICATION,0,SET_KEY_NOAPPLICATION_qualifiers
       };

static struct cduQualifier SET_KEY_qualifiers[5] = {
        "LOG",0x2001,0,0,0,0
       ,"STATE",0x2000,0,0,&SET_KEY_q02value,0
       ,"APPLICATION",0x2004,0,&SET_KEY_APPLICATION,0,0
       ,"NUMERIC",0x2004,0,&SET_KEY_NOAPPLICATION,0,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SET_KEY = {	/* Syntax def	*/
        "SET_KEY",0x0000,0,0,MDSDCL_SET_KEY,SET_KEY_params,SET_KEY_qualifiers
       };
extern int   MDSDCL_SET_INTERRUPT();
static struct cduKeyword  SET_KEYWORDS[7];	/* Prototype only	*/

static struct cduValue  SET_INTERRUPT_p01value = {
        0x2001,0,SET_KEYWORDS, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SET_INTERRUPT_params[2] = {
        "P1",0x1000,0,0,&SET_INTERRUPT_p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SET_INTERRUPT = {	/* Syntax def	*/
        "SET_INTERRUPT",0x0004,0,0,MDSDCL_SET_INTERRUPT,SET_INTERRUPT_params,0
       };
extern int   MDSDCL_SET_LIBRARY();

static struct cduValue  SET_LIBRARY_p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  SET_LIBRARY_p02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SET_LIBRARY_params[3] = {
        "P1",0x1000,0,0,&SET_LIBRARY_p01value,0
       ,"P2",0x1000,"LIBRARY","Library",&SET_LIBRARY_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  SET_LIBRARY_CLOSE_p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SET_LIBRARY_CLOSE_params[2] = {
        "P1",0x1000,0,0,&SET_LIBRARY_CLOSE_p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduQualifier SET_LIBRARY_CLOSE_qualifiers[2] = {
        "CLOSE",0x2001,0,0,0,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SET_LIBRARY_CLOSE = {	/* Syntax def	*/
        "SET_LIBRARY_CLOSE",0x0000,0,0,MDSDCL_SET_LIBRARY,SET_LIBRARY_CLOSE_params,SET_LIBRARY_CLOSE_qualifiers
       };

static struct cduQualifier SET_LIBRARY_qualifiers[2] = {
        "CLOSE",0x2000,0,&SET_LIBRARY_CLOSE,0,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SET_LIBRARY = {	/* Syntax def	*/
        "SET_LIBRARY",0x0000,0,0,MDSDCL_SET_LIBRARY,SET_LIBRARY_params,SET_LIBRARY_qualifiers
       };
extern int   mdsdcl_set_prompt();

static struct cduValue  SET_PROMPT_p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  SET_PROMPT_p02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SET_PROMPT_params[3] = {
        "P1",0x1000,0,0,&SET_PROMPT_p01value,0
       ,"P2",0x1000,"PROMPT","Prompt",&SET_PROMPT_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SET_PROMPT = {	/* Syntax def	*/
        "SET_PROMPT",0x0000,0,0,mdsdcl_set_prompt,SET_PROMPT_params,0
       };
extern int   mdsdcl_set_verify();

static struct cduValue  SET_VERIFY_p01value = {
        0x2001,0,SET_KEYWORDS, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SET_VERIFY_params[2] = {
        "P1",0x1000,0,0,&SET_VERIFY_p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SET_VERIFY = {	/* Syntax def	*/
        "SET_VERIFY",0x0004,0,0,mdsdcl_set_verify,SET_VERIFY_params,0
       };

static struct cduKeyword  SET_KEYWORDS[7] = {	/* "Type" def	*/
        "COMMAND",0x4000,0,&SET_COMMAND,0,0
       ,"KEY",0x4000,0,&SET_KEY,0,0
       ,"INTERRUPT",0x4002,0,&SET_INTERRUPT,0,0
       ,"LIBRARY",0x4000,0,&SET_LIBRARY,0,0
       ,"PROMPT",0x4000,0,&SET_PROMPT,0,0
       ,"VERIFY",0x4002,0,&SET_VERIFY,0,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v15p01value = {
        0x2001,0,SET_KEYWORDS, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v15params[2] = {
        "P1",0x1000,0,"What",&v15p01value,0
       ,0				/* null entry at end	*/
       };
extern int   MDSDCL_SHOW_KEY();

static struct cduValue  SHOW_KEY_p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  SHOW_KEY_p02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SHOW_KEY_params[3] = {
        "P1",0x1000,0,0,&SHOW_KEY_p01value,0
       ,"P2",0x1000,"KEY","Key",&SHOW_KEY_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  SHOW_KEY_ALL_p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SHOW_KEY_ALL_params[2] = {
        "P1",0x1000,0,0,&SHOW_KEY_ALL_p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  SHOW_KEY_ALL_q04value = {
        0x1000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier SHOW_KEY_ALL_qualifiers[5] = {
        "ALL",0x2001,0,0,0,0
       ,"BRIEF",0x2000,0,0,0,0
       ,"FULL",0x2000,0,0,0,0
       ,"STATE",0x2000,0,0,&SHOW_KEY_ALL_q04value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SHOW_KEY_ALL = {	/* Syntax def	*/
        "SHOW_KEY_ALL",0x0000,0,0,0,SHOW_KEY_ALL_params,SHOW_KEY_ALL_qualifiers
       };

static struct cduValue  SHOW_KEY_DIRECTORY_p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SHOW_KEY_DIRECTORY_params[2] = {
        "P1",0x1000,0,0,&SHOW_KEY_DIRECTORY_p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduQualifier SHOW_KEY_DIRECTORY_qualifiers[3] = {
        "ALL",0x2000,0,0,0,0
       ,"DIRECTORY",0x2001,0,0,0,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SHOW_KEY_DIRECTORY = {	/* Syntax def	*/
        "SHOW_KEY_DIRECTORY",0x0000,0,0,0,SHOW_KEY_DIRECTORY_params,SHOW_KEY_DIRECTORY_qualifiers
       };

static struct cduValue  SHOW_KEY_q05value = {
        0x1000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier SHOW_KEY_qualifiers[6] = {
        "ALL",0x2000,0,&SHOW_KEY_ALL,0,0
       ,"BRIEF",0x2000,0,0,0,0
       ,"DIRECTORY",0x2000,0,&SHOW_KEY_DIRECTORY,0,0
       ,"FULL",0x2000,0,0,0,0
       ,"STATE",0x2000,0,0,&SHOW_KEY_q05value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SHOW_KEY = {	/* Syntax def	*/
        "SHOW_KEY",0x0000,0,0,MDSDCL_SHOW_KEY,SHOW_KEY_params,SHOW_KEY_qualifiers
       };
extern int   MDSDCL_SHOW_LIBRARY();

static struct cduValue  SHOW_LIBRARY_p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SHOW_LIBRARY_params[2] = {
        "P1",0x1000,0,0,&SHOW_LIBRARY_p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SHOW_LIBRARY = {	/* Syntax def	*/
        "SHOW_LIBRARY",0x0000,0,0,MDSDCL_SHOW_LIBRARY,SHOW_LIBRARY_params,0
       };
extern int   mdsdcl_show_macro();

static struct cduValue  SHOW_MACRO_p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  SHOW_MACRO_p02value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SHOW_MACRO_params[3] = {
        "P1",0x1000,0,0,&SHOW_MACRO_p01value,0
       ,"P2",0x1000,"MACRO",0,&SHOW_MACRO_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduQualifier SHOW_MACRO_qualifiers[2] = {
        "FULL",0x2000,0,0,0,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SHOW_MACRO = {	/* Syntax def	*/
        "SHOW_MACRO",0x0000,0,0,mdsdcl_show_macro,SHOW_MACRO_params,SHOW_MACRO_qualifiers
       };
extern int   mdsdcl_show_timer();

static struct cduVerb  SHOW_TIMER = {	/* Syntax def	*/
        "SHOW_TIMER",0x0000,0,0,mdsdcl_show_timer,0,0
       };
extern int   MDSDCL_SHOW_VM();

static struct cduValue  SHOW_VM_p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SHOW_VM_params[2] = {
        "P1",0x1000,0,0,&SHOW_VM_p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduQualifier SHOW_VM_qualifiers[3] = {
        "FULL",0x2000,0,0,0,0
       ,"CHECK",0x2000,0,0,0,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SHOW_VM = {	/* Syntax def	*/
        "SHOW_VM",0x0000,0,0,MDSDCL_SHOW_VM,SHOW_VM_params,SHOW_VM_qualifiers
       };

static struct cduKeyword  SHOW_KEYWORDS[6] = {	/* "Type" def	*/
        "KEY",0x4000,0,&SHOW_KEY,0,0
       ,"LIBRARY",0x4000,0,&SHOW_LIBRARY,0,0
       ,"MACRO",0x4000,0,&SHOW_MACRO,0,0
       ,"TIMER",0x4000,0,&SHOW_TIMER,0,0
       ,"VM",0x4000,0,&SHOW_VM,0,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v16p01value = {
        0x2001,0,SHOW_KEYWORDS, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v16params[2] = {
        "P1",0x1000,0,"What",&v16p01value,0
       ,0				/* null entry at end	*/
       };
extern int   mdsdcl_spawn();

static struct cduValue  v17p01value = {
        0x0020,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v17params[2] = {
        "P1",0x1000,"COMMAND",0,&v17p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v17q02value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v17q03value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v17q07value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v17q08value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v17q09value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v17qualifiers[12] = {
        "CARRIAGE_CONTROL",0x2001,0,0,0,0
       ,"CLI",0x2000,0,0,&v17q02value,0
       ,"INPUT",0x2000,0,0,&v17q03value,0
       ,"KEYPAD",0x2001,0,0,0,0
       ,"LOGICAL_NAMES",0x2001,0,0,0,0
       ,"NOTIFY",0x2000,0,0,0,0
       ,"OUTPUT",0x2000,0,0,&v17q07value,0
       ,"PROCESS",0x2000,0,0,&v17q08value,0
       ,"PROMPT",0x2000,0,0,&v17q09value,0
       ,"SYMBOLS",0x2001,0,0,0,0
       ,"WAIT",0x2001,0,0,0,0
       ,0				/* null entry at end	*/
       };
extern int   mdsdcl_type();

static struct cduValue  v18p01value = {
        0x0020,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v18params[2] = {
        "P1",0x1000,0,0,&v18p01value,0
       ,0				/* null entry at end	*/
       };
extern int   mdsdcl_wait();

static struct cduValue  v19p01value = {
        0x2040,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v19params[2] = {
        "P1",0x1000,"DELTA_TIME","Deltatime",&v19p01value,0
       ,0				/* null entry at end	*/
       };

struct cduVerb  mdsdcl_tables[20] = {
        "ACTIVATE",0x0000,0,0,0,0,v01qualifiers
       ,"ATTACH",0x0000,0,0,MDSDCL_ATTACH,v02params,v02qualifiers
       ,"CREATE",0x0000,0,0,0,v03params,0
       ,"DCMD",0x0000,0,0,MDSDCL_NOOP,v04params,v04qualifiers
       ,"DEBUG",0x0000,0,0,mdsdcl_debug,0,0
       ,"DEFINE",0x0000,0,0,mdsdcl_define,v06params,v06qualifiers
       ,"DELETE",0x0000,0,0,0,v07params,v07qualifiers
       ,"DO",0x0000,0,0,mdsdcl_do_macro,v08params,v08qualifiers
       ,"EXIT",0x0000,0,0,mdsdcl_exit,0,0
       ,"FINISH",0x0000,0,0,MDSDCL_FINISH,0,0
       ,"HELP",0x0000,0,0,MDSDCL_HELP,v11params,0
       ,"INIT",0x0000,0,0,0,v12params,0
       ,"MODIFY",0x0000,0,0,MDSDCL_MODIFY,v13params,0
       ,"SAVE",0x0000,0,0,MDSDCL_SAVE_MACRO,v14params,v14qualifiers
       ,"SET",0x0000,0,0,0,v15params,0
       ,"SHOW",0x0000,0,0,0,v16params,0
       ,"SPAWN",0x0000,0,0,mdsdcl_spawn,v17params,v17qualifiers
       ,"TYPE",0x0000,0,0,mdsdcl_type,v18params,0
       ,"WAIT",0x0000,0,0,mdsdcl_wait,v19params,0
       ,0				/* null entry at end	*/
       };
