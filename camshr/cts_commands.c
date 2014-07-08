#include        "clisysdef.h"

		/* Filename: cts_commands.c
		 * created by cdu :  02-Jun-2014 15:59:11
		 ********************************************************/

extern int   Assign();

static struct cduValue  v01p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v01p02value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v01params[3] = {
        "P1",0x1000,"PHY_NAME","Physical name:",&v01p01value,0
       ,"P2",0x1000,"LOG_NAME","Logical name:",&v01p02value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v01q01value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v01qualifiers[2] = {
        "COMMENT",0x2000,0,0,&v01q01value,0
       ,0				/* null entry at end	*/
       };
extern int   Autoconfig();
extern int   Noop();

static struct cduValue  v03p01value = {
        0x0020,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v03params[2] = {
        "P1",0x1000,0,0,&v03p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduValue  v03q01value = {
        0x1000,"CTS_COMMANDS",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v03q02value = {
        0x0000,"CTSHELP",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v03q03value = {
        0x0000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v03q04value = {
        0x0000,"CTS>",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v03q05value = {
        0x0000,"*.CTS",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v03q06value = {
        0x0000,"CTSKEYS",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v03q07value = {
        0x0000,"CTS$LIBRARY",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v03q08value = {
        0x0000,"CTSINIT",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v03q09value = {
        0x0000,"SYS$LOGIN:*.CTS",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduValue  v03q10value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduQualifier v03qualifiers[12] = {
        "TABLES",0x2001,0,0,&v03q01value,0
       ,"HELPLIB",0x2001,0,0,&v03q02value,0
       ,"AUXHELPLIB",0x2000,0,0,&v03q03value,0
       ,"PROMPT",0x2001,0,0,&v03q04value,0
       ,"DEF_FILE",0x2001,0,0,&v03q05value,0
       ,"KEY_DEFS",0x2001,0,0,&v03q06value,0
       ,"DEF_LIBRARIES",0x2001,0,0,&v03q07value,0
       ,"COMMAND",0x2001,0,0,&v03q08value,0
       ,"INI_DEF_FILE",0x2001,0,0,&v03q09value,0
       ,"LIBRARY",0x2000,0,0,&v03q10value,0
       ,"DEBUG",0x2000,0,0,0,0
       ,0				/* null entry at end	*/
       };
extern int   Deassign();

static struct cduValue  v04p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v04params[2] = {
        "P1",0x1000,"MODULE","Module:",&v04p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduQualifier v04qualifiers[2] = {
        "PHYSICAL",0x2000,0,0,0,0
       ,0				/* null entry at end	*/
       };
extern int   Help();
extern int   ResetHighway();

static struct cduValue  v06p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v06params[2] = {
        "P1",0x1000,"MODULE","Highway:",&v06p01value,0
       ,0				/* null entry at end	*/
       };
extern int   SetCrate();

static struct cduValue  v07p01value = {
        0x0000,"*",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v07params[2] = {
        "P1",0x1000,"CRATE",0,&v07p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduQualifier v07qualifiers[4] = {
        "ONLINE",0x2000,0,0,0,0
       ,"OFFLINE",0x2000,0,0,0,0
       ,"QUIET",0x2000,0,0,0,0
       ,0				/* null entry at end	*/
       };
extern int   ShowModule();

static struct cduValue  v08p01value = {
        0x0000,"*",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v08params[2] = {
        "P1",0x1000,"MODULE",0,&v08p01value,0
       ,0				/* null entry at end	*/
       };
extern int   ShowCrate();

static struct cduValue  SHOW_CRATE_p01value = {
        0x0000,"*",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SHOW_CRATE_params[2] = {
        "P1",0x1000,"MODULE",0,&SHOW_CRATE_p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SHOW_CRATE = {	/* Syntax def	*/
        "SHOW_CRATE",0x0000,0,0,ShowCrate,SHOW_CRATE_params,0
       };

static struct cduValue  SHOW_HIGHWAY_p01value = {
        0x0000,"*",0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  SHOW_HIGHWAY_params[2] = {
        "P1",0x1000,"MODULE",0,&SHOW_HIGHWAY_p01value,0
       ,0				/* null entry at end	*/
       };

static struct cduVerb  SHOW_HIGHWAY = {	/* Syntax def	*/
        "SHOW_HIGHWAY",0x0000,0,0,ShowModule,SHOW_HIGHWAY_params,0
       };

static struct cduQualifier v08qualifiers[5] = {
        "CRATE",0x2000,0,&SHOW_CRATE,0,0
       ,"HIGHWAY",0x2000,0,&SHOW_HIGHWAY,0,0
       ,"PHYSICAL",0x2000,0,0,0,0
       ,"FORMAT",0x2000,0,0,0,0
       ,0				/* null entry at end	*/
       };
extern int   AddCrate();

static struct cduValue  v09p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v09params[2] = {
        "P1",0x1000,"PHY_NAME","Physical name:",&v09p01value,0
       ,0				/* null entry at end	*/
       };
extern int   DelCrate();

static struct cduValue  v10p01value = {
        0x2000,0,0, {0,DSC_K_DTYPE_T,DSC_K_CLASS_D,0} ,0
       };

static struct cduParam  v10params[2] = {
        "P1",0x1000,"PHY_NAME","Physical name:",&v10p01value,0
       ,0				/* null entry at end	*/
       };

struct cduVerb  cts_commands[11] = {
        "ASSIGN",0x0000,0,0,Assign,v01params,v01qualifiers
       ,"AUTOCONFIG",0x0000,0,0,Autoconfig,0,0
       ,"CTS",0x0000,0,0,Noop,v03params,v03qualifiers
       ,"DEASSIGN",0x0000,0,0,Deassign,v04params,v04qualifiers
       ,"HELP",0x0000,0,0,Help,0,0
       ,"RESET",0x0000,0,0,ResetHighway,v06params,0
       ,"SET",0x0000,0,0,SetCrate,v07params,v07qualifiers
       ,"SHOW",0x0000,0,0,ShowModule,v08params,v08qualifiers
       ,"ADDCRATE",0x0000,0,0,AddCrate,v09params,0
       ,"DELCRATE",0x0000,0,0,DelCrate,v10params,0
       ,0				/* null entry at end	*/
       };
