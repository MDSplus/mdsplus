#ifdef __vms
#define vms  1
#endif
#include        "clisysdef.h"

		/* Filename: ccl_commands.hh
		 * created by cdu :  10-Feb-1998 16:57:07
		 ********************************************************/

extern struct cduKeyword  CCL_show_keywords[4];
extern struct cduKeyword  CCL_set_keywords[4];
extern int   ccl_finish();

static struct cduValue  v01p01value = {
        0x0020,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v01params[2] = {
        "P1",0x0000,0,0,&v01p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v01q01value = {
        0x1000,"CCL_COMMANDS",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v01q02value = {
        0x0000,"CCLHELP",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v01q03value = {
        0x0000,"MDSDCL",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v01q04value = {
        0x0000,"CCL>",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v01q05value = {
        0x0000,"*.CCL",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v01q06value = {
        0x0000,"CCLKEYS",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v01q07value = {
        0x0000,"CCL$LIBRARY",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v01q08value = {
        0x0000,"CCLINIT",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v01q09value = {
        0x0000,"SYS$LOGIN:*.CCL",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v01q10value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v01qualifiers[13] = {
        "TABLES",0x0001,0,0,&v01q01value,0
       ,"HELPLIB",0x0001,0,0,&v01q02value,0
       ,"AUXHELPLIB",0x0001,0,0,&v01q03value,0
       ,"PROMPT",0x0001,0,0,&v01q04value,0
       ,"DEF_FILE",0x0001,0,0,&v01q05value,0
       ,"KEY_DEFS",0x0001,0,0,&v01q06value,0
       ,"DEF_LIBRARIES",0x0001,0,0,&v01q07value,0
       ,"COMMAND",0x0001,0,0,&v01q08value,0
       ,"INI_DEF_FILE",0x0001,0,0,&v01q09value,0
       ,"LIBRARY",0x0000,0,0,&v01q10value,0
       ,"DEBUG",0x0000,0,0,0,0
       ,"CLOSE",0x0000,0,0,0,0
       ,0				/* null entry at end	*/
       };
extern int   ccl_pio();

static struct cduValue  v03p01value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v03params[2] = {
        "P1",0x0000,"MODULE",0,&v03p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v03q01value = {
        0x0000,"0",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v03q02value = {
        0x0000,"0",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v03q03value = {
        0x3000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v03q04value = {
        0x0000,"1",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v03q09value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v03qualifiers[10] = {
        "ADDRESS",0x0005,0,0,&v03q01value,0
       ,"FUNCTION",0x0005,0,0,&v03q02value,0
       ,"DATA",0x0004,0,0,&v03q03value,0
       ,"COUNT",0x0005,0,0,&v03q04value,0
       ,"HEX",0x0004,0,0,0,0
       ,"BINARY",0x0004,0,0,0,0
       ,"OCTAL",0x0004,0,0,0,0
       ,"DECIMAL",0x0005,0,0,0,0
       ,"MEMORY",0x0004,0,0,&v03q09value,0
       ,0				/* null entry at end	*/
       };
extern int   ccl_qrep();

static struct cduValue  v04p01value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v04params[2] = {
        "P1",0x0000,"MODULE",0,&v04p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v04q01value = {
        0x0000,"0",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v04q02value = {
        0x0000,"0",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v04q03value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v04q04value = {
        0x3000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v04q09value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v04qualifiers[10] = {
        "ADDRESS",0x0005,0,0,&v04q01value,0
       ,"FUNCTION",0x0005,0,0,&v04q02value,0
       ,"COUNT",0x0004,0,0,&v04q03value,0
       ,"DATA",0x0004,0,0,&v04q04value,0
       ,"HEX",0x0004,0,0,0,0
       ,"BINARY",0x0004,0,0,0,0
       ,"OCTAL",0x0004,0,0,0,0
       ,"DECIMAL",0x0005,0,0,0,0
       ,"MEMORY",0x0004,0,0,&v04q09value,0
       ,0				/* null entry at end	*/
       };
extern int   ccl_qscan();

static struct cduValue  v05p01value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v05params[2] = {
        "P1",0x0000,"MODULE",0,&v05p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v05q01value = {
        0x0000,"0",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v05q02value = {
        0x0000,"0",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v05q03value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v05q04value = {
        0x3000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v05q09value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v05qualifiers[10] = {
        "ADDRESS",0x0005,0,0,&v05q01value,0
       ,"FUNCTION",0x0005,0,0,&v05q02value,0
       ,"COUNT",0x0004,0,0,&v05q03value,0
       ,"DATA",0x0004,0,0,&v05q04value,0
       ,"HEX",0x0004,0,0,0,0
       ,"BINARY",0x0004,0,0,0,0
       ,"OCTAL",0x0004,0,0,0,0
       ,"DECIMAL",0x0005,0,0,0,0
       ,"MEMORY",0x0004,0,0,&v05q09value,0
       ,0				/* null entry at end	*/
       };
extern int   ccl_qstop();

static struct cduValue  v06p01value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v06params[2] = {
        "P1",0x0000,"MODULE",0,&v06p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v06q01value = {
        0x0000,"0",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v06q02value = {
        0x0000,"0",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v06q03value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v06q04value = {
        0x3000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v06q09value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v06qualifiers[10] = {
        "ADDRESS",0x0005,0,0,&v06q01value,0
       ,"FUNCTION",0x0005,0,0,&v06q02value,0
       ,"COUNT",0x0004,0,0,&v06q03value,0
       ,"DATA",0x0004,0,0,&v06q04value,0
       ,"HEX",0x0004,0,0,0,0
       ,"BINARY",0x0004,0,0,0,0
       ,"OCTAL",0x0004,0,0,0,0
       ,"DECIMAL",0x0005,0,0,0,0
       ,"MEMORY",0x0004,0,0,&v06q09value,0
       ,0				/* null entry at end	*/
       };
extern int   ccl_stop();

static struct cduValue  v07p01value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v07params[2] = {
        "P1",0x0000,"MODULE",0,&v07p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v07q01value = {
        0x0000,"0",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v07q02value = {
        0x0000,"0",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v07q03value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v07q04value = {
        0x3000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v07q09value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v07qualifiers[11] = {
        "ADDRESS",0x0005,0,0,&v07q01value,0
       ,"FUNCTION",0x0005,0,0,&v07q02value,0
       ,"COUNT",0x0004,0,0,&v07q03value,0
       ,"DATA",0x0004,0,0,&v07q04value,0
       ,"HEX",0x0004,0,0,0,0
       ,"BINARY",0x0004,0,0,0,0
       ,"OCTAL",0x0004,0,0,0,0
       ,"DECIMAL",0x0005,0,0,0,0
       ,"MEMORY",0x0004,0,0,&v07q09value,0
       ,"ENHANCED",0x0000,0,0,0,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v08p01value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v08params[2] = {
        "P1",0x0000,"MODULE",0,&v08p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v08q01value = {
        0x0000,"0",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v08q02value = {
        0x0000,"0",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v08q03value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v08q04value = {
        0x3000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v08q09value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v08qualifiers[11] = {
        "ADDRESS",0x0005,0,0,&v08q01value,0
       ,"FUNCTION",0x0005,0,0,&v08q02value,0
       ,"COUNT",0x0004,0,0,&v08q03value,0
       ,"DATA",0x0004,0,0,&v08q04value,0
       ,"HEX",0x0004,0,0,0,0
       ,"BINARY",0x0004,0,0,0,0
       ,"OCTAL",0x0004,0,0,0,0
       ,"DECIMAL",0x0005,0,0,0,0
       ,"MEMORY",0x0004,0,0,&v08q09value,0
       ,"ENHANCED",0x0001,0,0,0,0
       ,0				/* null entry at end	*/
       };
extern int   ccl_dclst();
extern int   ccl_lpio();

static struct cduValue  v10p01value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v10params[2] = {
        "P1",0x0000,"MODULE",0,&v10p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v10q01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v10q02value = {
        0x0000,"0",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v10q03value = {
        0x0000,"0",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v10q04value = {
        0x3000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v10q10value = {
        0x0000,"24",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v10qualifiers[11] = {
        "STEP",0x0004,0,0,&v10q01value,0
       ,"ADDRESS",0x0005,0,0,&v10q02value,0
       ,"FUNCTION",0x0005,0,0,&v10q03value,0
       ,"DATA",0x0004,0,0,&v10q04value,0
       ,"COUNT",0x0005,0,0,0,0
       ,"HEX",0x0004,0,0,0,0
       ,"BINARY",0x0004,0,0,0,0
       ,"OCTAL",0x0005,0,0,0,0
       ,"DECIMAL",0x0005,0,0,0,0
       ,"MEMORY",0x0005,0,0,&v10q10value,0
       ,0				/* null entry at end	*/
       };
extern int   ccl_lqscan();

static struct cduValue  v11p01value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v11params[2] = {
        "P1",0x0000,"MODULE",0,&v11p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v11q01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v11q02value = {
        0x0000,"0",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v11q03value = {
        0x0000,"0",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v11q04value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v11q05value = {
        0x3000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v11q10value = {
        0x0000,"24",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v11qualifiers[11] = {
        "STEP",0x0004,0,0,&v11q01value,0
       ,"ADDRESS",0x0005,0,0,&v11q02value,0
       ,"FUNCTION",0x0005,0,0,&v11q03value,0
       ,"COUNT",0x0004,0,0,&v11q04value,0
       ,"DATA",0x0004,0,0,&v11q05value,0
       ,"HEX",0x0004,0,0,0,0
       ,"BINARY",0x0004,0,0,0,0
       ,"OCTAL",0x0004,0,0,0,0
       ,"DECIMAL",0x0005,0,0,0,0
       ,"MEMORY",0x0005,0,0,&v11q10value,0
       ,0				/* null entry at end	*/
       };
extern int   ccl_lstop();

static struct cduValue  v12p01value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v12params[2] = {
        "P1",0x0000,"MODULE",0,&v12p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v12q01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v12q02value = {
        0x0000,"0",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v12q03value = {
        0x0000,"0",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v12q04value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v12q05value = {
        0x3000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v12q10value = {
        0x0000,"24",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v12qualifiers[11] = {
        "STEP",0x0004,0,0,&v12q01value,0
       ,"ADDRESS",0x0005,0,0,&v12q02value,0
       ,"FUNCTION",0x0005,0,0,&v12q03value,0
       ,"COUNT",0x0004,0,0,&v12q04value,0
       ,"DATA",0x0004,0,0,&v12q05value,0
       ,"HEX",0x0004,0,0,0,0
       ,"BINARY",0x0004,0,0,0,0
       ,"OCTAL",0x0004,0,0,0,0
       ,"DECIMAL",0x0005,0,0,0,0
       ,"MEMORY",0x0005,0,0,&v12q10value,0
       ,0				/* null entry at end	*/
       };
extern int   ccl_execute();
extern int   ccl_rdata();

static struct cduValue  v14q01value = {
        0x0000,"1",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v14q02value = {
        0x0000,"16383",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v14q03value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v14qualifiers[5] = {
        "STEP",0x0005,0,0,&v14q01value,0
       ,"COUNT",0x0005,0,0,&v14q02value,0
       ,"MEMORY",0x0004,0,0,&v14q03value,0
       ,"DATA",0x0000,0,0,0,0
       ,0				/* null entry at end	*/
       };
extern int   ccl_wdata();

static struct cduValue  v15q01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v15q02value = {
        0x0000,"16383",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v15q03value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v15q04value = {
        0x3000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v15qualifiers[9] = {
        "STEP",0x0004,0,0,&v15q01value,0
       ,"COUNT",0x0005,0,0,&v15q02value,0
       ,"MEMORY",0x0004,0,0,&v15q03value,0
       ,"DATA",0x0004,0,0,&v15q04value,0
       ,"HEX",0x0004,0,0,0,0
       ,"BINARY",0x0004,0,0,0,0
       ,"OCTAL",0x0004,0,0,0,0
       ,"DECIMAL",0x0005,0,0,0,0
       ,0				/* null entry at end	*/
       };
extern int   ccl_lamwait();

static struct cduValue  v16p01value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v16params[2] = {
        "P1",0x0000,"MODULE",0,&v16p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v16q01value = {
        0x0000,"32767",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v16qualifiers[2] = {
        "TIMEOUT",0x0005,0,0,&v16q01value,0
       ,0				/* null entry at end	*/
       };
extern int   ccl_set_xandq();

static struct cduValue  SET_XANDQ_p01value = {
        0x2001,0,CCL_set_keywords, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SET_XANDQ_params[2] = {
        "P1",0x0000,0,0,&SET_XANDQ_p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduKeyword  XANDQ_KEYWORDS[4] = {	/* "Type" def	*/
        "ANY",0x0000,0,0,0,0
       ,"NO",0x0000,0,0,0,0
       ,"YES",0x0000,0,0,0,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  SET_XANDQ_q01value = {
        0x0001,"ANY",XANDQ_KEYWORDS, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  SET_XANDQ_q02value = {
        0x0001,"ANY",XANDQ_KEYWORDS, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier SET_XANDQ_qualifiers[3] = {
        "X",0x0001,0,0,&SET_XANDQ_q01value,0
       ,"Q",0x0001,0,0,&SET_XANDQ_q02value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SET_XANDQ = {	/* Syntax def	*/
        "SET_XANDQ",0x0000,0,0,ccl_set_xandq,SET_XANDQ_params,SET_XANDQ_qualifiers
       };
extern int   ccl_set_memory();

static struct cduValue  SET_MEMORY_p01value = {
        0x2001,0,CCL_set_keywords, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  SET_MEMORY_16_q01value = {
        0x0000,"16",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier SET_MEMORY_16_qualifiers[2] = {
        "MEMORY",0x0005,0,0,&SET_MEMORY_16_q01value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SET_MEMORY_16 = {	/* Syntax def	*/
        "SET_MEMORY_16",0x0000,0,0,0,0,SET_MEMORY_16_qualifiers
       };

static struct cduValue  SET_MEMORY_24_q01value = {
        0x0000,"24",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier SET_MEMORY_24_qualifiers[2] = {
        "MEMORY",0x0005,0,0,&SET_MEMORY_24_q01value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SET_MEMORY_24 = {	/* Syntax def	*/
        "SET_MEMORY_24",0x0000,0,0,0,0,SET_MEMORY_24_qualifiers
       };

static struct cduKeyword  memory_keywords[3] = {	/* "Type" def	*/
        "16",0x0000,0,&SET_MEMORY_16,0,0
       ,"24",0x0000,0,&SET_MEMORY_24,0,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  SET_MEMORY_p02value = {
        0x0001,0,memory_keywords, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SET_MEMORY_params[3] = {
        "P1",0x0000,0,"What",&SET_MEMORY_p01value,0
       ,"P2",0x0000,0,0,&SET_MEMORY_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SET_MEMORY = {	/* Syntax def	*/
        "SET_MEMORY",0x0000,0,0,ccl_set_memory,SET_MEMORY_params,0
       };
extern int   ccl_set_module();

static struct cduValue  SET_MODULE_p01value = {
        0x2001,0,CCL_set_keywords, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  SET_MODULE_p02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SET_MODULE_params[3] = {
        "P1",0x0000,0,0,&SET_MODULE_p01value,0
       ,"P2",0x0000,"MODULE","Module",&SET_MODULE_p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SET_MODULE = {	/* Syntax def	*/
        "SET_MODULE",0x0000,0,0,ccl_set_module,SET_MODULE_params,0
       };

struct cduKeyword  CCL_set_keywords[4] = {	/* "Type" def	*/
        "XANDQ",0x0000,0,&SET_XANDQ,0,0
       ,"MEMORY",0x0000,0,&SET_MEMORY,0,0
       ,"MODULE",0x0000,0,&SET_MODULE,0,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v17p01value = {
        0x2001,0,CCL_set_keywords, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v17params[2] = {
        "P1",0x0000,0,"What",&v17p01value,0
       ,0				/* null entry at end	*/
       };
extern int   ccl_show_data();

static struct cduValue  SHOW_DATA_p01value = {
        0x2001,0,CCL_show_keywords, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SHOW_DATA_params[2] = {
        "P1",0x0000,0,0,&SHOW_DATA_p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  SHOW_DATA_q01value = {
        0x0000,"1",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  SHOW_DATA_q02value = {
        0x0000,"1",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier SHOW_DATA_qualifiers[8] = {
        "START",0x0005,0,0,&SHOW_DATA_q01value,0
       ,"END",0x0005,0,0,&SHOW_DATA_q02value,0
       ,"BINARY",0x0004,0,0,0,0
       ,"DECIMAL",0x0005,0,0,0,0
       ,"OCTAL",0x0004,0,0,0,0
       ,"HEX",0x0004,0,0,0,0
       ,"MEMORY",0x0004,0,0,0,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SHOW_DATA = {	/* Syntax def	*/
        "SHOW_DATA",0x0000,0,0,ccl_show_data,SHOW_DATA_params,SHOW_DATA_qualifiers
       };
extern int   ccl_show_module();

static struct cduValue  SHOW_MODULE_p01value = {
        0x2001,0,CCL_show_keywords, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SHOW_MODULE_params[2] = {
        "P1",0x0000,0,0,&SHOW_MODULE_p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SHOW_MODULE = {	/* Syntax def	*/
        "SHOW_MODULE",0x0000,0,0,ccl_show_module,SHOW_MODULE_params,0
       };
extern int   ccl_show_status();

static struct cduValue  SHOW_STATUS_p01value = {
        0x2001,0,CCL_show_keywords, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SHOW_STATUS_params[2] = {
        "P1",0x0000,0,0,&SHOW_STATUS_p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SHOW_STATUS = {	/* Syntax def	*/
        "SHOW_STATUS",0x0000,0,0,ccl_show_status,SHOW_STATUS_params,0
       };

struct cduKeyword  CCL_show_keywords[4] = {	/* "Type" def	*/
        "DATA",0x0000,0,&SHOW_DATA,0,0
       ,"MODULE",0x0000,0,&SHOW_MODULE,0,0
       ,"STATUS",0x0000,0,&SHOW_STATUS,0,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v18p01value = {
        0x2001,0,CCL_show_keywords, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v18params[2] = {
        "P1",0x0000,0,"What",&v18p01value,0
       ,0				/* null entry at end	*/
       };
extern int   ccl_plot_data();

static struct cduValue  v19q01value = {
        0x0000,"1",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v19q02value = {
        0x0000,"32767",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v19q03value = {
        0x0000,"TT:",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v19q04value = {
        0x0000,"RETRO",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v19qualifiers[5] = {
        "START",0x0005,0,0,&v19q01value,0
       ,"END",0x0005,0,0,&v19q02value,0
       ,"DEVICE",0x0005,0,0,&v19q03value,0
       ,"DEVTYPE",0x0005,0,0,&v19q04value,0
       ,0				/* null entry at end	*/
       };

struct cduVerb  ccl_commands[20] = {
        "CCL",0x0000,0,0,ccl_finish,v01params,v01qualifiers
       ,"FINISH",0x0000,0,0,ccl_finish,0,0
       ,"PIO",0x0000,0,0,ccl_pio,v03params,v03qualifiers
       ,"QREP",0x0000,0,0,ccl_qrep,v04params,v04qualifiers
       ,"QSCAN",0x0000,0,0,ccl_qscan,v05params,v05qualifiers
       ,"QSTOP",0x0000,0,0,ccl_qstop,v06params,v06qualifiers
       ,"STOP",0x0000,0,0,ccl_stop,v07params,v07qualifiers
       ,"FSTOP",0x0000,0,0,ccl_stop,v08params,v08qualifiers
       ,"DCLST",0x0000,0,0,ccl_dclst,0,0
       ,"LPIO",0x0000,0,0,ccl_lpio,v10params,v10qualifiers
       ,"LQSCAN",0x0000,0,0,ccl_lqscan,v11params,v11qualifiers
       ,"LSTOP",0x0000,0,0,ccl_lstop,v12params,v12qualifiers
       ,"EXECUTE",0x0000,0,0,ccl_execute,0,0
       ,"RDATA",0x0000,0,0,ccl_rdata,0,v14qualifiers
       ,"WDATA",0x0000,0,0,ccl_wdata,0,v15qualifiers
       ,"LAMWAIT",0x0000,0,0,ccl_lamwait,v16params,v16qualifiers
       ,"SET",0x0000,0,0,0,v17params,0
       ,"SHOW",0x0000,0,0,0,v18params,0
       ,"PLOT",0x0000,0,0,ccl_plot_data,0,v19qualifiers
       ,0				/* null entry at end	*/
       };
