#include        <stdio.h>
#include        "mdsdcl.h"
#ifdef vms
#include        <ssdef.h>
#include        <lib$routines.h>
#endif

/*********************************************************************
* MDSDCL_DO_COMMAND.C --
*
* Effectively, the main MDSDCL routine.  Called again and again
* from within main().
*
* History:
*  01-May-2000  TRG  Check for "@" indirect command in "command" param.
*  06-May-1998  TRG  Remove "verify" display:  moved to "get_input".
*  10-Nov-1997  TRG  Adapted from original VMS-only source code.
*
**********************************************************************/


struct _mdsdcl_ctrl  MDSDCL_COMMON;
static struct _mdsdcl_ctrl *ctrl = &MDSDCL_COMMON;

#ifdef vms
extern sys$setast();
extern smg$set_out_of_band_asts();
#endif



	/****************************************************************
	 * mdsdcl_do_command:
	 * Effectively, the main MDSDCL routine ...
	 ****************************************************************/
int mdsdcl_do_command(
    void  *command		/* <r:opt> command -- cstring or dsc	*/
   )
   {
    int   startDepth;
    int   sts;
    int   tblidx;
    int tried_indirect = 0;
    char  *p;
    struct _mdsdcl_io  *io;
    static char  doMacro[12];
    static DYNAMIC_DESCRIPTOR(dsc_cmd);
#ifdef vms
    extern int   MDSDCL$MSG_TO_RET();
    extern int   MDSDCL$OUT_OF_BAND_AST();
#endif

	/*---------------------------------------------------------------
	 * Executable:
	 *--------------------------------------------------------------*/
#ifdef vms
    lib$establish(MDSDCL$MSG_TO_RET);
    sys$setast(1);
/*    smg$set_out_of_band_asts(&ctrl->pasteboard_id,&0x4000008,
/*                        MDSDCL$OUT_OF_BAND_AST,ctrl);	/*  */
#endif

    if (!doMacro[0])
       {			/* first time ...			*/
        if (!ctrl->tbladr[0])  mdsdcl_initialize(ctrl);
        sprintf(doMacro,"DO %cMACRO ",QUALIFIER_CHARACTER);
       }

    io = ctrl->ioLevel + ctrl->depth;
    if (command)
       {
        str_trim(&io->last_command,command);
        str_copy_dx(&dsc_cmd,&io->last_command);
        mdsdcl_insert_symbols(io->ioParameter,&dsc_cmd);

			/*=================================================
			 * If "@" appears in optional "command" param,
			 *  create and execute a fake macro so that it
			 *  will be handled correctly ...
			 *================================================*/
        p = dsc_cmd.dscA_pointer;
        if (*p == '@')
           {
            makeCmdlineMacro("__CMDLINE__",p);
            startDepth = ctrl->depth;
            sts = mdsdcl_do_command("__CMDLINE__");
            for ( ; ctrl->depth > startDepth ; )
                sts = mdsdcl_do_command(0);
            return(sts);
           }
       }
    else
       {
        str_free1_dx(&io->last_command);
        str_free1_dx(&dsc_cmd);
       }

		/*-------------------------------------------------------
		 * Try each table in turn ...
		 *------------------------------------------------------*/
    tblidx = ctrl->tables;
    for ( ; tblidx>0 ; tblidx--)
       {
        sts = mdsdcl_dcl_parse(&dsc_cmd,ctrl,tblidx);
        if (~sts & 1)
           {
            if ((sts == CLI_STS_EOF) || (sts == CLI_STS_NOCOMD))
                return(sts);		/*--------------------> return	*/
            if (sts == MDSDCL_STS_INDIRECT_EOF)
                return(sts);		/*--------------------> return	*/
           }

        io = ctrl->ioLevel + ctrl->depth;
        if (!dsc_cmd.dscA_pointer)
            str_copy_dx(&dsc_cmd,&io->last_command);

        if (sts & 1)
           {			/* Try to dispatch the routine ...	*/
            sts = cli_dispatch(ctrl);
            if (sts != CLI_STS_INVROUT)
               {
                if (~sts & 1)
                   {
                    if (dsc_cmd.dscA_pointer)
                        MdsMsg(0,"--> failed on line '%s'",
                                dsc_cmd.dscA_pointer);
                    mdsdcl_close_indirect_all();
                   }
                return(sts);		/*--------------------> return	*/
               }
           }
        }
		/*-------------------------------------------------------
		 * Command not found in any table:  try it as a macro ...
		 *------------------------------------------------------*/
    tblidx = 1;
    str_prefix(&dsc_cmd,doMacro);
    sts = mdsdcl_dcl_parse(&dsc_cmd,ctrl,tblidx);
    if (sts & 1)
       {			/* Try to dispatch the macro ...	*/
        sts = cli_dispatch(ctrl);
       }
    if (~sts & 1)
        mdsdcl_close_indirect_all();

    return(sts);			/*--------------------> return	*/
   }



	/***************************************************************
	 * mdsdcl_ctrl_address:
	 ***************************************************************/
struct _mdsdcl_ctrl  *mdsdcl_ctrl_address()
   {
    return(ctrl);
   }
