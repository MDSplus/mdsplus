#ifdef __vms
#define vms  1
#endif
#include        "clisysdef.h"

		/* Filename: tcl_commands.hh
		 * created by cdu :  11-Mar-1998 16:47:46
		 ********************************************************/

extern struct cduKeyword  TCL_ADD_TYPE[3];
extern struct cduKeyword  TCL_DEFINE_TYPE[2];
extern struct cduKeyword  TCL_CREATE_TYPE[2];
extern struct cduKeyword  TCL_DELETE_TYPE[3];
extern struct cduKeyword  TCL_REMOVE_TYPE[2];
extern struct cduKeyword  TCL_RUNDOWN_TYPE[2];
extern struct cduKeyword  TCL_SHOW_TYPE[6];
extern struct cduKeyword  TCL_STOP_TYPE[2];
extern struct cduKeyword  TCL_SET_TYPE[6];
extern struct cduKeyword  TCL_START_TYPE[2];
extern int   TclDispatch_abort_server();
extern int   TclDispatch_stop_server();

static struct cduValue  STOP_SERVER_p01value = {
        0x2001,0,TCL_STOP_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  STOP_SERVER_p02value = {
        0x3000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  STOP_SERVER_params[3] = {
        "P1",0x1000,0,"What",&STOP_SERVER_p01value,0
       ,"P2",0x1000,"SERVER","Server",&STOP_SERVER_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  STOP_SERVER = {	/* Syntax def	*/
        "STOP_SERVER",0x0000,0,0,TclDispatch_stop_server,STOP_SERVER_params,0
       };

struct cduKeyword  TCL_STOP_TYPE[2] = {	/* "Type" def	*/
        "SERVER",0x4000,0,&STOP_SERVER,0,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  ABORT_SERVER_p01value = {
        0x2001,0,TCL_STOP_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  ABORT_SERVER_p02value = {
        0x3000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  ABORT_SERVER_params[3] = {
        "P1",0x1000,0,"What",&ABORT_SERVER_p01value,0
       ,"P2",0x1000,"SERVER","Server",&ABORT_SERVER_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  ABORT_SERVER = {	/* Syntax def	*/
        "ABORT_SERVER",0x0000,0,0,TclDispatch_abort_server,ABORT_SERVER_params,0
       };

static struct cduKeyword  abort_type[2] = {	/* "Type" def	*/
        "SERVER",0x4000,0,&ABORT_SERVER,0,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v01p01value = {
        0x2001,0,abort_type, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v01params[2] = {
        "P1",0x1000,0,"What",&v01p01value,0
       ,0				/* null entry at end	*/
       };
extern int   TclAddNode();

static struct cduValue  ADD_NODE_p01value = {
        0x2001,0,TCL_ADD_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  ADD_NODE_p02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  ADD_NODE_params[3] = {
        "P1",0x1000,0,"What",&ADD_NODE_p01value,0
       ,"P2",0x1000,"NODE","Node",&ADD_NODE_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  ADD_NODE_q01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  t04k01value = {
        0x0004,"0",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  t04k02value = {
        0x0004,"1",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  t04k03value = {
        0x0004,"2",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  t04k04value = {
        0x0004,"3",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  t04k05value = {
        0x0004,"4",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  t04k06value = {
        0x0004,"5",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  t04k07value = {
        0x0004,"6",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  t04k08value = {
        0x0004,"7",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  t04k09value = {
        0x0004,"8",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  t04k10value = {
        0x0004,"9",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  t04k11value = {
        0x0004,"10",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  t04k12value = {
        0x0004,"11",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  t04k13value = {
        0x0004,"12",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduKeyword  USAGE_TYPE[14] = {	/* "Type" def	*/
        "ANY",0x4000,0,0,&t04k01value,0
       ,"STRUCTURE",0x4000,0,0,&t04k02value,0
       ,"ACTION",0x4000,0,0,&t04k03value,0
       ,"DEVICE",0x4000,0,0,&t04k04value,0
       ,"DISPATCH",0x4000,0,0,&t04k05value,0
       ,"NUMERIC",0x4000,0,0,&t04k06value,0
       ,"SIGNAL",0x4000,0,0,&t04k07value,0
       ,"TASK",0x4000,0,0,&t04k08value,0
       ,"TEXT",0x4000,0,0,&t04k09value,0
       ,"WINDOW",0x4000,0,0,&t04k10value,0
       ,"AXIS",0x4000,0,0,&t04k11value,0
       ,"SUBTREE",0x4000,0,0,&t04k12value,0
       ,"COMPOUND_DATA",0x4000,0,0,&t04k13value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  ADD_NODE_q02value = {
        0x0001,"ANY",USAGE_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  ADD_NODE_q03value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier ADD_NODE_qualifiers[4] = {
        "MODEL",0x2000,0,0,&ADD_NODE_q01value,0
       ,"USAGE",0x2001,0,0,&ADD_NODE_q02value,0
       ,"QUALIFIERS",0x2000,0,0,&ADD_NODE_q03value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  ADD_NODE = {	/* Syntax def	*/
        "ADD_NODE",0x0000,0,0,TclAddNode,ADD_NODE_params,ADD_NODE_qualifiers
       };
extern int   TclAddTag();

static struct cduValue  ADD_TAG_p01value = {
        0x2001,0,TCL_ADD_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  ADD_TAG_p02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  ADD_TAG_p03value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  ADD_TAG_params[4] = {
        "P1",0x1000,0,"What",&ADD_TAG_p01value,0
       ,"P2",0x1000,"NODE","Node",&ADD_TAG_p02value,0
       ,"P3",0x1000,"TAG","Tag",&ADD_TAG_p03value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  ADD_TAG = {	/* Syntax def	*/
        "ADD_TAG",0x0000,0,0,TclAddTag,ADD_TAG_params,0
       };

struct cduKeyword  TCL_ADD_TYPE[3] = {	/* "Type" def	*/
        "NODE",0x4000,0,&ADD_NODE,0,0
       ,"TAG",0x4000,0,&ADD_TAG,0,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v02p01value = {
        0x2001,0,TCL_ADD_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v02params[2] = {
        "P1",0x1000,0,"What",&v02p01value,0
       ,0				/* null entry at end	*/
       };
extern int   TclCleanDatafile();

static struct cduValue  v03p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v03params[2] = {
        "P1",0x1000,"FILE","File",&v03p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v03q01value = {
        0x0004,"-1",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v03qualifiers[3] = {
        "SHOTID",0x2005,0,0,&v03q01value,0
       ,"OVERRIDE",0x2000,0,0,0,0
       ,0				/* null entry at end	*/
       };
extern int   TclClose();

static struct cduValue  v04p01value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v04params[2] = {
        "P1",0x1000,"FILE",0,&v04p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v04q01value = {
        0x0004,"-1",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v04qualifiers[3] = {
        "SHOTID",0x2005,0,0,&v04q01value,0
       ,"ALL",0x2000,0,0,0,0
       ,0				/* null entry at end	*/
       };
extern int   TclCompressDatafile();

static struct cduValue  v05p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v05params[2] = {
        "P1",0x1000,"FILE","File",&v05p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v05q01value = {
        0x0004,"-1",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v05qualifiers[3] = {
        "SHOTID",0x2005,0,0,&v05q01value,0
       ,"OVERRIDE",0x2000,0,0,0,0
       ,0				/* null entry at end	*/
       };
extern int   TclCreatePulse();

static struct cduValue  CREATE_PULSE_p01value = {
        0x2001,0,TCL_CREATE_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  CREATE_PULSE_p02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  CREATE_PULSE_params[3] = {
        "P1",0x1000,0,"What",&CREATE_PULSE_p01value,0
       ,"P2",0x1000,"SHOT","Shot number",&CREATE_PULSE_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  CREATE_PULSE_q01value = {
        0x3000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  CREATE_PULSE_q02value = {
        0x3000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier CREATE_PULSE_qualifiers[6] = {
        "INCLUDE",0x2000,0,0,&CREATE_PULSE_q01value,0
       ,"EXCLUDE",0x2000,0,0,&CREATE_PULSE_q02value,0
       ,"CONDITIONAL",0x2000,0,0,0,0
       ,"NOMAIN",0x2004,0,0,0,0
       ,"DISPATCH",0x2000,0,0,0,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  CREATE_PULSE = {	/* Syntax def	*/
        "CREATE_PULSE",0x0000,0,0,TclCreatePulse,CREATE_PULSE_params,CREATE_PULSE_qualifiers
       };

struct cduKeyword  TCL_CREATE_TYPE[2] = {	/* "Type" def	*/
        "PULSE",0x4000,0,&CREATE_PULSE,0,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v06p01value = {
        0x2001,0,TCL_CREATE_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v06params[2] = {
        "P1",0x1000,0,"What",&v06p01value,0
       ,0				/* null entry at end	*/
       };
extern int   TclDecompile();

static struct cduValue  v07p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v07params[2] = {
        "P1",0x1000,"PATH",0,&v07p01value,0
       ,0				/* null entry at end	*/
       };
extern int   TclCreatePulse_server();

static struct cduValue  DEFINE_SERVER_p01value = {
        0x2001,0,TCL_DEFINE_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  DEFINE_SERVER_p02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  DEFINE_SERVER_params[3] = {
        "P1",0x1000,0,"WHAT",&DEFINE_SERVER_p01value,0
       ,"P2",0x1000,0,0,&DEFINE_SERVER_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  DEFINE_SERVER_q01value = {
        0x3000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier DEFINE_SERVER_qualifiers[2] = {
        "TREE",0x2000,0,0,&DEFINE_SERVER_q01value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  DEFINE_SERVER = {	/* Syntax def	*/
        "DEFINE_SERVER",0x0000,0,0,TclCreatePulse_server,DEFINE_SERVER_params,DEFINE_SERVER_qualifiers
       };

struct cduKeyword  TCL_DEFINE_TYPE[2] = {	/* "Type" def	*/
        "SERVER",0x4000,0,&DEFINE_SERVER,0,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v08p01value = {
        0x2001,0,TCL_DEFINE_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v08params[2] = {
        "P1",0x1000,0,"WHAT",&v08p01value,0
       ,0				/* null entry at end	*/
       };
extern int   TclDeleteNode();

static struct cduValue  DELETE_NODE_p01value = {
        0x2001,0,TCL_DELETE_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  DELETE_NODE_p02value = {
        0x3000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  DELETE_NODE_params[3] = {
        "P1",0x1000,0,"What",&DELETE_NODE_p01value,0
       ,"P2",0x1000,"NODE","Node",&DELETE_NODE_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduQualifier DELETE_NODE_qualifiers[3] = {
        "LOG",0x2000,0,0,0,0
       ,"CONFIRM",0x2001,0,0,0,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  DELETE_NODE = {	/* Syntax def	*/
        "DELETE_NODE",0x0000,0,0,TclDeleteNode,DELETE_NODE_params,DELETE_NODE_qualifiers
       };
extern int   TclDeletePulse();

static struct cduValue  DELETE_PULSE_p01value = {
        0x2001,0,TCL_DELETE_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  DELETE_PULSE_p02value = {
        0x2004,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  DELETE_PULSE_params[3] = {
        "P1",0x1000,0,"What",&DELETE_PULSE_p01value,0
       ,"P2",0x1000,"SHOT","Shot number",&DELETE_PULSE_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduQualifier DELETE_PULSE_qualifiers[2] = {
        "ALL",0x2000,0,0,0,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  DELETE_PULSE = {	/* Syntax def	*/
        "DELETE_PULSE",0x0000,0,0,TclDeletePulse,DELETE_PULSE_params,DELETE_PULSE_qualifiers
       };

struct cduKeyword  TCL_DELETE_TYPE[3] = {	/* "Type" def	*/
        "NODE",0x4000,0,&DELETE_NODE,0,0
       ,"PULSE",0x4000,0,&DELETE_PULSE,0,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v09p01value = {
        0x2001,0,TCL_DELETE_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v09params[2] = {
        "P1",0x1000,0,"What",&v09p01value,0
       ,0				/* null entry at end	*/
       };
extern int   TclDirectory();

static struct cduValue  v10p01value = {
        0x1000,"*,.*",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v10params[2] = {
        "P1",0x1000,"NODE",0,&v10p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v10q03value = {
        0x1001,0,USAGE_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };
extern int   TclDirectoryTag();

static struct cduValue  DIR_TAG_SYNTAX_p01value = {
        0x1000,"*::*",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  DIR_TAG_SYNTAX_params[2] = {
        "P1",0x1000,"TAG",0,&DIR_TAG_SYNTAX_p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  DIR_TAG_SYNTAX = {	/* Syntax def	*/
        "DIR_TAG_SYNTAX",0x0000,0,0,TclDirectoryTag,DIR_TAG_SYNTAX_params,0
       };

static struct cduQualifier v10qualifiers[5] = {
        "FULL",0x2000,0,0,0,0
       ,"PATH",0x2000,0,0,0,0
       ,"USAGE",0x2000,0,0,&v10q03value,0
       ,"TAG",0x2000,0,&DIR_TAG_SYNTAX,0,0
       ,0				/* null entry at end	*/
       };
extern int   TclDispatch();

static struct cduValue  v11p01value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v11params[2] = {
        "P1",0x1000,"NODE",0,&v11p01value,0
       ,0				/* null entry at end	*/
       };
extern int   TclDispatch_build();

static struct cduValue  DISPATCH_BUILD_q01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier DISPATCH_BUILD_qualifiers[2] = {
        "MONITOR",0x2000,0,0,&DISPATCH_BUILD_q01value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  DISPATCH_BUILD = {	/* Syntax def	*/
        "DISPATCH_BUILD",0x0000,0,0,TclDispatch_build,0,DISPATCH_BUILD_qualifiers
       };
extern int   TclDispatch_check();

static struct cduQualifier DISPATCH_CHECK_qualifiers[2] = {
        "RESET",0x2000,0,0,0,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  DISPATCH_CHECK = {	/* Syntax def	*/
        "DISPATCH_CHECK",0x0000,0,0,TclDispatch_check,0,DISPATCH_CHECK_qualifiers
       };
extern int   TclDispatch_phase();

static struct cduValue  DISPATCH_PHASE_p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  DISPATCH_PHASE_params[2] = {
        "P1",0x1000,"PHASE_NAME","Phase",&DISPATCH_PHASE_p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  DISPATCH_PHASE_q03value = {
        0x0004,"1",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  DISPATCH_PHASE_q05value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier DISPATCH_PHASE_qualifiers[6] = {
        "PHASE",0x2000,0,0,0,0
       ,"NOACTION",0x2000,0,0,0,0
       ,"SYNCH",0x2005,0,0,&DISPATCH_PHASE_q03value,0
       ,"LOG",0x2001,0,0,0,0
       ,"MONITOR",0x2000,0,0,&DISPATCH_PHASE_q05value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  DISPATCH_PHASE = {	/* Syntax def	*/
        "DISPATCH_PHASE",0x0000,0,0,TclDispatch_phase,DISPATCH_PHASE_params,DISPATCH_PHASE_qualifiers
       };
extern int   TclDispatch_close();

static struct cduValue  DISPATCH_CLOSE_q02value = {
        0x3000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier DISPATCH_CLOSE_qualifiers[3] = {
        "CLOSE",0x2000,0,0,0,0
       ,"SERVER",0x2000,0,0,&DISPATCH_CLOSE_q02value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  DISPATCH_CLOSE = {	/* Syntax def	*/
        "DISPATCH_CLOSE",0x0000,0,0,TclDispatch_close,0,DISPATCH_CLOSE_qualifiers
       };
extern int   TclDispatch_command();

static struct cduValue  DISPATCH_COMMAND_p01value = {
        0x0020,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  DISPATCH_COMMAND_params[2] = {
        "P1",0x1000,0,0,&DISPATCH_COMMAND_p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  DISPATCH_COMMAND_q02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  DISPATCH_COMMAND_q04value = {
        0x0000,"TCL",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier DISPATCH_COMMAND_qualifiers[5] = {
        "COMMAND",0x2000,0,0,0,0
       ,"SERVER",0x2000,0,0,&DISPATCH_COMMAND_q02value,0
       ,"WAIT",0x2000,0,0,0,0
       ,"TABLE",0x2005,0,0,&DISPATCH_COMMAND_q04value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  DISPATCH_COMMAND = {	/* Syntax def	*/
        "DISPATCH_COMMAND",0x0000,0,0,TclDispatch_command,DISPATCH_COMMAND_params,DISPATCH_COMMAND_qualifiers
       };

static struct cduQualifier v11qualifiers[7] = {
        "WAIT",0x2001,0,0,0,0
       ,"BUILD",0x2000,0,&DISPATCH_BUILD,0,0
       ,"CHECK",0x2000,0,&DISPATCH_CHECK,0,0
       ,"PHASE",0x2000,0,&DISPATCH_PHASE,0,0
       ,"CLOSE",0x2000,0,&DISPATCH_CLOSE,0,0
       ,"COMMAND",0x2000,0,&DISPATCH_COMMAND,0,0
       ,0				/* null entry at end	*/
       };
extern int   TclDoNode();

static struct cduValue  v12p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v12params[2] = {
        "P1",0x1000,"NODE","Node",&v12p01value,0
       ,0				/* null entry at end	*/
       };
extern int   TclDoMethod();

static struct cduValue  DO_METHOD_p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  DO_METHOD_p02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  DO_METHOD_params[3] = {
        "P1",0x1000,"OBJECT","Object",&DO_METHOD_p01value,0
       ,"P2",0x1000,"METHOD","Method",&DO_METHOD_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  DO_METHOD_q01value = {
        0x3000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  DO_METHOD_q02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier DO_METHOD_qualifiers[4] = {
        "ARGUMENT",0x2000,0,0,&DO_METHOD_q01value,0
       ,"IF",0x2000,0,0,&DO_METHOD_q02value,0
       ,"OVERRIDE",0x2000,0,0,0,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  DO_METHOD = {	/* Syntax def	*/
        "DO_METHOD",0x0000,0,0,TclDoMethod,DO_METHOD_params,DO_METHOD_qualifiers
       };

static struct cduQualifier v12qualifiers[2] = {
        "METHOD",0x2000,0,&DO_METHOD,0,0
       ,0				/* null entry at end	*/
       };
extern int   TclEdit();

static struct cduValue  v13p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v13params[2] = {
        "P1",0x1000,"FILE","File",&v13p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v13q01value = {
        0x0004,"-1",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v13qualifiers[3] = {
        "SHOTID",0x2005,0,0,&v13q01value,0
       ,"NEW",0x2000,0,0,0,0
       ,0				/* null entry at end	*/
       };
extern int   TclLink();

static struct cduValue  v14p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v14p02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v14params[3] = {
        "P1",0x1000,"SOURCE","Source experiment",&v14p01value,0
       ,"P2",0x1000,"DESTINATION","Destination experiment",&v14p02value,0
       ,0				/* null entry at end	*/
       };
extern int   TreeMarkIncludes();
extern int   TclPutExpression();

static struct cduValue  v16p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v16p02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v16params[3] = {
        "P1",0x1000,"NODE","Node",&v16p01value,0
       ,"P2",0x1000,"VALUE","Value",&v16p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  PUT_EXTENDED_p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  PUT_EXTENDED_params[2] = {
        "P1",0x1000,"NODE","Node",&PUT_EXTENDED_p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  PUT_EXTENDED_q04value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier PUT_EXTENDED_qualifiers[5] = {
        "EXTENDED",0x2001,0,0,0,0
       ,"SYMBOLS",0x2000,0,0,0,0
       ,"LF",0x2001,0,0,0,0
       ,"EOF",0x2000,0,0,&PUT_EXTENDED_q04value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  PUT_EXTENDED = {	/* Syntax def	*/
        "PUT_EXTENDED",0x0000,0,0,TclPutExpression,PUT_EXTENDED_params,PUT_EXTENDED_qualifiers
       };

static struct cduQualifier v16qualifiers[2] = {
        "EXTENDED",0x2000,0,&PUT_EXTENDED,0,0
       ,0				/* null entry at end	*/
       };
extern int   TclCommand();

static struct cduValue  v17p01value = {
        0x0020,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v17params[2] = {
        "P1",0x1000,0,0,&v17p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v17q01value = {
        0x1000,"TCLCOMMANDS",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v17q02value = {
        0x0000,"TCLHELP",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v17q03value = {
        0x0000,"MDSDCL,DATABASE_COMMANDS",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v17q04value = {
        0x0000,"TCL>",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v17q05value = {
        0x0000,"*.TCL",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v17q06value = {
        0x0000,"TCLKEYS",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v17q07value = {
        0x0000,"TCL$LIBRARY",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v17q08value = {
        0x0000,"TCLINIT",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v17q09value = {
        0x0000,"SYS$LOGIN:*.TCL",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v17q10value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v17qualifiers[12] = {
        "TABLES",0x2001,0,0,&v17q01value,0
       ,"HELPLIB",0x2001,0,0,&v17q02value,0
       ,"AUXHELPLIB",0x2001,0,0,&v17q03value,0
       ,"PROMPT",0x2001,0,0,&v17q04value,0
       ,"DEF_FILE",0x2001,0,0,&v17q05value,0
       ,"KEY_DEFS",0x2001,0,0,&v17q06value,0
       ,"DEF_LIBRARIES",0x2001,0,0,&v17q07value,0
       ,"COMMAND",0x2001,0,0,&v17q08value,0
       ,"INI_DEF_FILE",0x2001,0,0,&v17q09value,0
       ,"LIBRARY",0x2000,0,0,&v17q10value,0
       ,"DEBUG",0x2000,0,0,0,0
       ,0				/* null entry at end	*/
       };
extern int   TclRemoveTag();

static struct cduValue  REMOVE_TAG_p01value = {
        0x2001,0,TCL_REMOVE_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  REMOVE_TAG_p02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  REMOVE_TAG_params[3] = {
        "P1",0x1000,0,"What",&REMOVE_TAG_p01value,0
       ,"P2",0x1000,"TAG","Tag",&REMOVE_TAG_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  REMOVE_TAG = {	/* Syntax def	*/
        "REMOVE_TAG",0x0000,0,0,TclRemoveTag,REMOVE_TAG_params,0
       };

struct cduKeyword  TCL_REMOVE_TYPE[2] = {	/* "Type" def	*/
        "TAG",0x4000,0,&REMOVE_TAG,0,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v18p01value = {
        0x2001,0,TCL_REMOVE_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v18params[2] = {
        "P1",0x1000,0,"What",&v18p01value,0
       ,0				/* null entry at end	*/
       };
extern int   TclRename();

static struct cduValue  v19p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v19p02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v19params[3] = {
        "P1",0x1000,"SOURCE",0,&v19p01value,0
       ,"P2",0x1000,"DEST",0,&v19p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduQualifier v19qualifiers[2] = {
        "LOG",0x2000,0,0,0,0
       ,0				/* null entry at end	*/
       };
extern int   TclRundownTree();

static struct cduValue  RUNDOWN_TREE_p01value = {
        0x2001,0,TCL_RUNDOWN_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  RUNDOWN_TREE_p02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  RUNDOWN_TREE_params[3] = {
        "P1",0x1000,0,"What",&RUNDOWN_TREE_p01value,0
       ,"P2",0x1000,"FILE","File",&RUNDOWN_TREE_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  RUNDOWN_TREE_q01value = {
        0x0000,"-1",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier RUNDOWN_TREE_qualifiers[4] = {
        "SHOTID",0x2005,0,0,&RUNDOWN_TREE_q01value,0
       ,"FORCE_EXIT",0x2000,0,0,0,0
       ,"SUBTREES",0x2000,0,0,0,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  RUNDOWN_TREE = {	/* Syntax def	*/
        "RUNDOWN_TREE",0x0000,0,0,TclRundownTree,RUNDOWN_TREE_params,RUNDOWN_TREE_qualifiers
       };

struct cduKeyword  TCL_RUNDOWN_TYPE[2] = {	/* "Type" def	*/
        "TREE",0x4000,0,&RUNDOWN_TREE,0,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v20p01value = {
        0x2001,0,TCL_RUNDOWN_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v20params[2] = {
        "P1",0x1000,0,"What",&v20p01value,0
       ,0				/* null entry at end	*/
       };
extern int   TclSetCurrent();

static struct cduValue  SET_CURRENT_p01value = {
        0x2001,0,TCL_SET_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  SET_CURRENT_p02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  SET_CURRENT_p03value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SET_CURRENT_params[4] = {
        "P1",0x1000,0,"What",&SET_CURRENT_p01value,0
       ,"P2",0x1000,"EXPERIMENT","Experiment",&SET_CURRENT_p02value,0
       ,"P3",0x1000,"SHOT",0,&SET_CURRENT_p03value,0
       ,0				/* null entry at end	*/
       };

static struct cduQualifier SET_CURRENT_qualifiers[2] = {
        "INCREMENT",0x2000,0,0,0,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SET_CURRENT = {	/* Syntax def	*/
        "SET_CURRENT",0x0000,0,0,TclSetCurrent,SET_CURRENT_params,SET_CURRENT_qualifiers
       };
extern int   TclSetDefault();

static struct cduValue  SET_DEFAULT_p01value = {
        0x2001,0,TCL_SET_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  SET_DEFAULT_p02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SET_DEFAULT_params[3] = {
        "P1",0x1000,0,"What",&SET_DEFAULT_p01value,0
       ,"P2",0x1000,"NODE","Node",&SET_DEFAULT_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SET_DEFAULT = {	/* Syntax def	*/
        "SET_DEFAULT",0x0000,0,0,TclSetDefault,SET_DEFAULT_params,0
       };
extern int   TclSetNode();

static struct cduValue  SET_NODE_p01value = {
        0x2001,0,TCL_SET_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  SET_NODE_p02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SET_NODE_params[3] = {
        "P1",0x1000,0,"What",&SET_NODE_p01value,0
       ,"P2",0x1000,"NODE","Node",&SET_NODE_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduQualifier SET_NODE_qualifiers[12] = {
        "SUBTREE",0x2000,0,0,0,0
       ,"ON",0x2000,0,0,0,0
       ,"OFF",0x2000,0,0,0,0
       ,"WRITE_ONCE",0x2000,0,0,0,0
       ,"COMPRESS_ON_PUT",0x2000,0,0,0,0
       ,"DO_NOT_COMPRESS",0x2000,0,0,0,0
       ,"SHOT_WRITE",0x2000,0,0,0,0
       ,"MODEL_WRITE",0x2000,0,0,0,0
       ,"INCLUDED",0x2000,0,0,0,0
       ,"LOG",0x2000,0,0,0,0
       ,"ESSENTIAL",0x2000,0,0,0,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SET_NODE = {	/* Syntax def	*/
        "SET_NODE",0x0000,0,0,TclSetNode,SET_NODE_params,SET_NODE_qualifiers
       };
extern int   TclDispatch_set_server();

static struct cduValue  SET_SERVER_p01value = {
        0x2001,0,TCL_SET_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  SET_SERVER_p02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SET_SERVER_params[3] = {
        "P1",0x1000,0,"What",&SET_SERVER_p01value,0
       ,"P2",0x1000,"SERVER","Server",&SET_SERVER_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  t11k01value = {
        0x0004,"0",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  t11k02value = {
        0x0004,"1",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  t11k03value = {
        0x0004,"2",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduKeyword  logging_type[4] = {	/* "Type" def	*/
        "NONE",0x4000,0,0,&t11k01value,0
       ,"ACTIONS",0x4001,0,0,&t11k02value,0
       ,"STATISTICS",0x4000,0,0,&t11k03value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  SET_SERVER_q01value = {
        0x0001,"ACTIONS",logging_type, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier SET_SERVER_qualifiers[2] = {
        "LOG",0x2004,0,0,&SET_SERVER_q01value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SET_SERVER = {	/* Syntax def	*/
        "SET_SERVER",0x0000,0,0,TclDispatch_set_server,SET_SERVER_params,SET_SERVER_qualifiers
       };
extern int   TclSetTree();

static struct cduValue  SET_TREE_p01value = {
        0x2001,0,TCL_SET_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  SET_TREE_p02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SET_TREE_params[3] = {
        "P1",0x1000,0,"What",&SET_TREE_p01value,0
       ,"P2",0x1000,"FILE","File",&SET_TREE_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  SET_TREE_q01value = {
        0x0000,"-1",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier SET_TREE_qualifiers[3] = {
        "SHOTID",0x2005,0,0,&SET_TREE_q01value,0
       ,"READONLY",0x2000,0,0,0,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SET_TREE = {	/* Syntax def	*/
        "SET_TREE",0x0000,0,0,TclSetTree,SET_TREE_params,SET_TREE_qualifiers
       };

struct cduKeyword  TCL_SET_TYPE[6] = {	/* "Type" def	*/
        "CURRENT",0x4000,0,&SET_CURRENT,0,0
       ,"DEFAULT",0x4000,0,&SET_DEFAULT,0,0
       ,"NODE",0x4000,0,&SET_NODE,0,0
       ,"SERVER",0x4000,0,&SET_SERVER,0,0
       ,"TREE",0x4000,0,&SET_TREE,0,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v21p01value = {
        0x2001,0,TCL_SET_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v21params[2] = {
        "P1",0x1000,0,"What",&v21p01value,0
       ,0				/* null entry at end	*/
       };
extern int   TclSetEvent();

static struct cduValue  v22p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v22params[2] = {
        "P1",0x1000,"EVENT","Event",&v22p01value,0
       ,0				/* null entry at end	*/
       };
extern int   TclShowCurrent();

static struct cduValue  SHOW_CURRENT_p01value = {
        0x2001,0,TCL_SET_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  SHOW_CURRENT_p02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SHOW_CURRENT_params[3] = {
        "P1",0x1000,0,"What",&SHOW_CURRENT_p01value,0
       ,"P2",0x1000,"EXPERIMENT","Experiment",&SHOW_CURRENT_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SHOW_CURRENT = {	/* Syntax def	*/
        "SHOW_CURRENT",0x0000,0,0,TclShowCurrent,SHOW_CURRENT_params,0
       };
extern int   TclShowData();

static struct cduValue  SHOW_DATA_p01value = {
        0x2001,0,TCL_SHOW_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  SHOW_DATA_p02value = {
        0x3000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SHOW_DATA_params[3] = {
        "P1",0x1000,0,"What",&SHOW_DATA_p01value,0
       ,"P2",0x1000,"NODE","Node",&SHOW_DATA_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SHOW_DATA = {	/* Syntax def	*/
        "SHOW_DATA",0x0000,0,0,TclShowData,SHOW_DATA_params,0
       };
extern int   TclShowDB();

static struct cduValue  SHOW_DB_p01value = {
        0x2001,0,TCL_SHOW_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SHOW_DB_params[2] = {
        "P1",0x1000,0,"What",&SHOW_DB_p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SHOW_DB = {	/* Syntax def	*/
        "SHOW_DB",0x0000,0,0,TclShowDB,SHOW_DB_params,0
       };
extern int   TclShowDefault();

static struct cduValue  SHOW_DEFAULT_p01value = {
        0x2001,0,TCL_SHOW_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SHOW_DEFAULT_params[2] = {
        "P1",0x1000,0,"What",&SHOW_DEFAULT_p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SHOW_DEFAULT = {	/* Syntax def	*/
        "SHOW_DEFAULT",0x0000,0,0,TclShowDefault,SHOW_DEFAULT_params,0
       };
extern int   TclDispatch_show_server();

static struct cduValue  SHOW_SERVER_p01value = {
        0x2001,0,TCL_STOP_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  SHOW_SERVER_p02value = {
        0x3000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SHOW_SERVER_params[3] = {
        "P1",0x1000,0,"What",&SHOW_SERVER_p01value,0
       ,"P2",0x1000,"SERVER","Server",&SHOW_SERVER_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduQualifier SHOW_SERVER_qualifiers[3] = {
        "OUTPUT",0x2001,0,0,0,0
       ,"FULL",0x2000,0,0,0,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SHOW_SERVER = {	/* Syntax def	*/
        "SHOW_SERVER",0x0000,0,0,TclDispatch_show_server,SHOW_SERVER_params,SHOW_SERVER_qualifiers
       };

struct cduKeyword  TCL_SHOW_TYPE[6] = {	/* "Type" def	*/
        "CURRENT",0x4000,0,&SHOW_CURRENT,0,0
       ,"DATA",0x4000,0,&SHOW_DATA,0,0
       ,"DB",0x4000,0,&SHOW_DB,0,0
       ,"DEFAULT",0x4000,0,&SHOW_DEFAULT,0,0
       ,"SERVER",0x4000,0,&SHOW_SERVER,0,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v23p01value = {
        0x2001,0,TCL_SHOW_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v23params[2] = {
        "P1",0x1000,0,"What",&v23p01value,0
       ,0				/* null entry at end	*/
       };
extern int   TclDispatch_start_server();

static struct cduValue  START_SERVER_p01value = {
        0x2001,0,TCL_START_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  START_SERVER_p02value = {
        0x3000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  START_SERVER_params[3] = {
        "P1",0x1000,0,"What",&START_SERVER_p01value,0
       ,"P2",0x1000,"SERVER","Server",&START_SERVER_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  START_SERVER = {	/* Syntax def	*/
        "START_SERVER",0x0000,0,0,TclDispatch_start_server,START_SERVER_params,0
       };

struct cduKeyword  TCL_START_TYPE[2] = {	/* "Type" def	*/
        "SERVER",0x4000,0,&START_SERVER,0,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v24p01value = {
        0x2001,0,TCL_START_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v24params[2] = {
        "P1",0x1000,0,"What",&v24p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v25p01value = {
        0x2001,0,TCL_STOP_TYPE, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v25params[2] = {
        "P1",0x1000,0,"What",&v25p01value,0
       ,0				/* null entry at end	*/
       };
extern int   TclVerifyTree();
extern int   TclWfevent();

static struct cduValue  v27p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v27params[2] = {
        "P1",0x1000,"EVENT","Event",&v27p01value,0
       ,0				/* null entry at end	*/
       };
extern int   TclWrite();

static struct cduValue  v28p01value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v28params[2] = {
        "P1",0x1000,"FILE",0,&v28p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v28q01value = {
        0x0004,"-1",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v28qualifiers[2] = {
        "SHOTID",0x2005,0,0,&v28q01value,0
       ,0				/* null entry at end	*/
       };

struct cduVerb  tcl_commands[29] = {
        "ABORT",0x0000,0,0,0,v01params,0
       ,"ADD",0x0000,0,0,0,v02params,0
       ,"CLEAN",0x0000,0,0,TclCleanDatafile,v03params,v03qualifiers
       ,"CLOSE",0x0000,0,0,TclClose,v04params,v04qualifiers
       ,"COMPRESS",0x0000,0,0,TclCompressDatafile,v05params,v05qualifiers
       ,"CREATE",0x0000,0,0,0,v06params,0
       ,"DECOMPILE",0x0000,0,0,TclDecompile,v07params,0
       ,"DEFINE",0x0000,0,0,0,v08params,0
       ,"DELETE",0x0000,0,0,0,v09params,0
       ,"DIRECTORY",0x0000,0,0,TclDirectory,v10params,v10qualifiers
       ,"DISPATCH",0x0000,0,0,TclDispatch,v11params,v11qualifiers
       ,"DO",0x0000,0,0,TclDoNode,v12params,v12qualifiers
       ,"EDIT",0x0000,0,0,TclEdit,v13params,v13qualifiers
       ,"LINK",0x0000,0,0,TclLink,v14params,0
       ,"MARK",0x0000,0,0,TreeMarkIncludes,0,0
       ,"PUT",0x0000,0,0,TclPutExpression,v16params,v16qualifiers
       ,"TCL",0x0000,0,0,TclCommand,v17params,v17qualifiers
       ,"REMOVE",0x0000,0,0,0,v18params,0
       ,"RENAME",0x0000,0,0,TclRename,v19params,v19qualifiers
       ,"RUNDOWN",0x0000,0,0,0,v20params,0
       ,"SET",0x0000,0,0,0,v21params,0
       ,"SETEVENT",0x0000,0,0,TclSetEvent,v22params,0
       ,"SHOW",0x0000,0,0,0,v23params,0
       ,"START",0x0000,0,0,0,v24params,0
       ,"STOP",0x0000,0,0,0,v25params,0
       ,"VERIFY",0x0000,0,0,TclVerifyTree,0,0
       ,"WFEVENT",0x0000,0,0,TclWfevent,v27params,0
       ,"WRITE",0x0000,0,0,TclWrite,v28params,v28qualifiers
       ,0				/* null entry at end	*/
       };
