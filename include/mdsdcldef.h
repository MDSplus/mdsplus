#ifdef __vms
#ifndef vms
#define vms    1
#endif
#endif
#ifdef __unix
#ifndef unix
#define unix    1
#endif
#endif

#define _DESCRIPTOR_PREFIXES   1
#include        <mdsdescrip.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        "facility_list.h"
#include        "dasutil.h"

/**********************************************************************
* MDSDCLDEF.H --
*
* Structure definitions etc for MDSDCL ...
*
***********************************************************************/

#ifndef __MDSDCLDEF
#define __MDSDCLDEF   1

#define MAX_TABLES     15
#define MAX_DEPTH      4	/* Max depth of indirection, incl macros */

struct _mdsdcl_io  {
    FILE  *fp;			/* File ptr.  0 for macros		*/
    int   isatty;		/* Flag: T if input is a tty		*/
    int   lineno;		/* current line number in file		*/
    struct _mdsdcl_macro  *ioMacro;
    struct descriptor  last_command;	/* Dynamic descriptor		*/
    struct descriptor  ioParameter[8];	/* Dynamic descriptors		*/
   };
struct _mdsdcl_macro  {
    char  *name;			/* macro name <upper case>	*/
    int   isOpen;			/* 1 if macro is in use		*/
    int   maxLines;			/* max num lines allocated	*/
    int   numLines;			/* num lines used		*/
    char  **lines;			/* ptr to lines[maxLines] ptrs	*/
   };
struct _mdsdcl_ctrl  {
    struct cduVerb  *tbladr[MAX_TABLES];
    int tables;
    struct descriptor prompt;
    struct descriptor helplib;
    struct descriptor def_file;
    struct  {
        int   maxMacros;		/* dim of macro.list[]		*/
        int   numMacros;		/* num macros defined in list[] */
        struct _mdsdcl_macro  *list;	/* top of macro list[]		*/
        struct descriptor current;
        struct descriptor saved;
        struct descriptor parameter[7];
        int max_param;
       } macro;
/*    int keyboard_id;		/* for VMS SMG routines		*/
/*    int sys_input;		/* for VMS SMG routines		*/
/*    int pasteboard_id;		/* for VMS SMG routines		*/
    int key_table_id;		/* for VMS SMG routines		*/
    struct  {
        struct descriptor name;
        struct descriptor lock_name;
        int lksb[2];
        int index;
       } library;
/*    struct descriptor last_command;			/*  */
    struct  {
        int index;
        struct descriptor name;
       } read_lib[8];
    int   depth;		/* Current depth within ioLevel[]	*/
    struct _mdsdcl_io  ioLevel[MAX_DEPTH + 1];
    char verify;
    char end_block;
    char filler[2];
   };

	/***************************************************************
	 * Function prototypes:
	 ***************************************************************/
int   mdsdcl_main(		/* The real main program		*/
    int   argc			/* <r> argument count			*/
   ,char  *argv[]		/* <r> arg list				*/
   );
int   mdsdcl_dcl_parse(			/* Returns CLI_STS_xxx status	*/
    void  *command			/* <r:opt> command string	*/
   ,struct _mdsdcl_ctrl  *ctrl		/* <m> control structure	*/
   ,int   tabidx			/* <r> cmd table idx, 1-based	*/
   );
int   mdsdcl_do_command(
    void  *command		/* <r:opt> command -- cstring or dsc	*/
   );
int   MdsMsg(			/* Return: sts provided by user		*/
    int   sts			/* <r> status code			*/
   ,char  fmt[]			/* <r> format statement			*/
   , ...			/* <r:opt> arguments to fmt[]		*/
   );
int   mdsdcl_initialize(		/* Return: status		*/
    struct _mdsdcl_ctrl  *ctrl		/* <m> the control structure	*/
   );
void  mdsdcl_insert_symbols(		/* Returns: void		*/
    struct descriptor  *dsc_cmdline	/* <m> command line		*/
   ,struct descriptor  *dsc_prmList	/* <r> parameter list		*/
   );
int   mdsdcl_get_input(			/* Return: status		*/
    void  *uprompt			/* <r> dsc or c-string		*/
   ,struct descriptor  *dsc_cmd		/* <w> new command line		*/
   );
int   mdsdcl_get_input_nosymbols(	/* Return: status		*/
    void  *uprompt			/* <r> dsc or c-string		*/
   ,struct descriptor  *dsc_cmd		/* <w> new command line		*/
   );
int   mdsdcl_close_indirect_all();


	/****************************************************************
	 * Status codes:
	 ****************************************************************/
#define MDSDCL_STS(N)   (MDSDCL_FACILITY<<16)+N

#define MDSDCL_STS_SUCCESS         MDSDCL_STS(1)
#define MDSDCL_STS_INDIRECT_DONE   MDSDCL_STS(3)

#define MDSDCL_STS_ERROR           MDSDCL_STS(2)
#define MDSDCL_STS_INDIRECT_ERROR  MDSDCL_STS(4)
#define MDSDCL_STS_INDIRECT_EOF    MDSDCL_STS(6)


#ifdef CREATE_STS_TEXT
static struct stsText  mdsdcl_stsText[] = {
        STS_TEXT(MDSDCL_STS_SUCCESS,"Normal successful completion")
       ,STS_TEXT(MDSDCL_STS_INDIRECT_DONE,"Success from indirect command")

       ,STS_TEXT(MDSDCL_STS_ERROR,"Generic MDSDCL Error")
       ,STS_TEXT(MDSDCL_STS_INDIRECT_ERROR,"Error from indirect command")
       ,STS_TEXT(MDSDCL_STS_INDIRECT_EOF,"End-Of-File in indirect command")
       };
#endif

#endif		/* __MDSDCLDEF	*/
