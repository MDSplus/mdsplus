#include <config.h>
#ifdef HAVE_WINDOWS_H
#include        <winsock2.h>
#endif

#include        <stdio.h>
#include        "mdsdcl.h"
#ifdef vms
#include        <ssdef.h>
#include        <lib$routines.h>
#endif
#include <STATICdef.h>
#include "mdsdclthreadsafe.h"

/*********************************************************************
* MDSDCL_DO_COMMAND.C --
*
* Effectively, the main MDSDCL routine.  Called again and again
* from within main().
*
* History:
*  25-Feb-2003  TRG  If mdsdcl_dcl_parse() returns CLI_STS_NOCOMD (i.e.
*                     blank line) try again -- do not return.
*  13-Apr-2001  TRG  Remove check for indirect files.
*                    Report "No such command" if final sts = CLI_STS_IVQUAL.
*  06-Apr-2001  TRG  Revised handling of indirect commands.
*                    Restore "verify" display to within mdsdcl_do_command.
*  06-May-1998  TRG  Remove "verify" display:  moved to "get_input".
*  10-Nov-1997  TRG  Adapted from original VMS-only source code.
*
**********************************************************************/



#ifdef vms
extern sys$setast();
extern smg$set_out_of_band_asts();
#endif



	/****************************************************************
	 * displayCmdline:
	 ****************************************************************/
static void  displayCmdline(
    char  *cmdline		/* <r> the command line			*/
   )
   {
    int   i;
    struct _mdsdcl_ctrl  *ctrl = &MdsdclGetThreadStatic()->ctrl;

    if (!nonblank(cmdline))
        return;
    for (i=0 ; i<ctrl->depth ; i++)
        printf("==");
    printf("> %s\n\r",cmdline);
    return;
   }

STATIC_THREADSAFE pthread_mutex_t mdsdclMutex;
STATIC_THREADSAFE int             mdsdclMutex_initialized=0;

	/****************************************************************
	 * mdsdcl_do_command:
	 * Effectively, the main MDSDCL routine ...
	 ****************************************************************/
int mdsdcl_do_command(
    void  *command		/* <r:opt> command -- cstring or dsc	*/
   )
   {
    int tblidx;
    int sts,stsParse;
    int tried_indirect = 0;
    struct _mdsdcl_io  *io;
    static char  doMacro[12];
    static DYNAMIC_DESCRIPTOR(dsc_cmd);
    struct _mdsdcl_ctrl  *ctrl = &MdsdclGetThreadStatic()->ctrl;
#ifdef vms
    extern int   MDSDCL$MSG_TO_RET();
    extern int   MDSDCL$OUT_OF_BAND_AST();
#endif

	/*---------------------------------------------------------------
	 * Executable:
	 *--------------------------------------------------------------*/

    LockMdsShrMutex(&mdsdclMutex,&mdsdclMutex_initialized);
    if (!ctrl->tbladr[0])  mdsdcl_initialize(ctrl);
    tblidx = ctrl->tables;

    if (!doMacro[0])
       {			/* first time ...			*/
        sprintf(doMacro,"DO %cMACRO ",'/');
       }

    io = ctrl->ioLevel + ctrl->depth;
    if (command)
       {
        str_trim(&io->last_command,command);
        str_copy_dx(&dsc_cmd,&io->last_command);
        mdsdcl_insert_symbols(io->ioParameter,&dsc_cmd);
       }
    else
       {
        str_free1_dx(&io->last_command);
        str_free1_dx(&dsc_cmd);
       }

		/*-------------------------------------------------------
		 * Try each table in turn ...
		 *------------------------------------------------------*/
    for ( ; tblidx>0 ; tblidx--)
       {
        do {
            stsParse = mdsdcl_dcl_parse(&dsc_cmd,ctrl,tblidx);
           }  while(stsParse == CLI_STS_NOCOMD);
				/* 25-Feb-03: ignore blank lines -- TRG	*/
        if (~stsParse & 1)
           {
	     if ((stsParse == CLI_STS_EOF) || (stsParse == CLI_STS_NOCOMD)) {
	        UnlockMdsShrMutex(&mdsdclMutex);
                return(stsParse);
	     }	else if (stsParse == MDSDCL_STS_INDIRECT_EOF) {
	        UnlockMdsShrMutex(&mdsdclMutex);
                return(stsParse);	/*--------------------> return	*/
	     }
           }

        io = ctrl->ioLevel + ctrl->depth;
        if (!dsc_cmd.dscA_pointer)
            str_copy_dx(&dsc_cmd,&io->last_command);

        if (stsParse & 1)
           {			/* Try to dispatch the routine ...	*/
            sts = cli_dispatch(ctrl);
            if (sts != CLI_STS_INVROUT)
               {
                if (ctrl->verify && sts != 3)
                    displayCmdline(dsc_cmd.dscA_pointer);
                if (~sts & 1)
                   {
                    if (dsc_cmd.dscA_pointer)
                        MdsMsg(0,"--> failed on line '%s'",
                                dsc_cmd.dscA_pointer);
                    mdsdcl_close_indirect_all();
                   }
		 UnlockMdsShrMutex(&mdsdclMutex);
                return(sts);		/*--------------------> return	*/
               }
           }
        }
		/*-------------------------------------------------------
		 * Command not found in any table:  try it as a macro ...
		 *------------------------------------------------------*/
    if (ctrl->verify)
        displayCmdline(dsc_cmd.dscA_pointer);
    tblidx = 1;			/* i.e., the table for DO/MACRO		*/
    str_prefix(&dsc_cmd,doMacro);
    sts = mdsdcl_dcl_parse(&dsc_cmd,ctrl,tblidx);
    if (sts & 1)
       {			/* Try to dispatch the macro ...	*/
        sts = cli_dispatch(ctrl);
       }
    else if (sts == CLI_STS_IVQUAL)
       {
        MdsMsg(0,"No such command");	/* CLI_STS_IVQUAL msg is suppressed
					 *  by cli_process_qualifier() */
       }

    if (~sts & 1)
        mdsdcl_close_indirect_all();
    UnlockMdsShrMutex(&mdsdclMutex);

    return(sts);			/*--------------------> return	*/
   }



	/***************************************************************
	 * mdsdcl_ctrl_address:
	 ***************************************************************/
struct _mdsdcl_ctrl  *mdsdcl_ctrl_address()
   {
     return(&MdsdclGetThreadStatic()->ctrl);
   }
