#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        "mdsdcl.h"
#ifdef vms
#include        <lib$routines.h>
#include        <ssdef.h>
#endif

/**********************************************************************
* MDSDCL_DCL_PARSE.C --
*
* History:
*  08-Nov-1997  TRG  Create.
*
************************************************************************/


extern int   mdsdcl_get_input();



	/****************************************************************
	 * mdsdcl_dcl_parse:
	 *****************************************************************/
int   mdsdcl_dcl_parse(		/* Returns CLI_STS_xxx status	*/
    void  *command			/* <r:opt> command string	*/
   ,struct _mdsdcl_ctrl  *ctrl		/* <m> control structure	*/
   ,int   tabidx			/* <r> cmd table idx, 1-based	*/
   )
   {
    int   indirect_flag;
    int   nbytes;
    int   sts;
    char  *p;
    struct _mdsdcl_io  *io;
    struct descriptor  *dsc;
    static char  *cmd;			/* addr of private cmd string	*/
    static int   maxcmd;		/* current length of cmd[]	*/
    static char  doIndirect[16];
#ifdef vms
    extern mdsdcl$dcl_parse_handler();
#endif

#ifdef vms
    if (tabidx > 1)
        lib$establish(lib$sig_to_ret);
    else
        lib$establish(mdsdcl$dcl_parse_handler);
#endif

    if (!doIndirect[0])
       {			/* first time ...			*/
        sprintf(doIndirect,"DO %cINDIRECT ",QUALIFIER_CHARACTER);
       }

		/*-------------------------------------------------------
		 * Copy optional "command" to cmd, checking for '@' ...
		 *------------------------------------------------------*/
    if (command && (is_cdescr(command) || is_ddescr(command)))
       {
        dsc = command;
        p = nonblank(dsc->dscA_pointer);
       }
    else
        p = nonblank(command);

    if (!p)
       {		/* command string is null ...			*/
        sts = cli_dcl_parse(0,ctrl->tbladr[tabidx-1],
            mdsdcl_get_input,mdsdcl_get_input,&ctrl->prompt);

        dsc = cli_addr_cmdline_dsc();
        io = ctrl->ioLevel + ctrl->depth;
        str_copy_dx(&io->last_command,dsc);
       }
    else
       {		/* else, p is start of command string ...	*/
        nbytes = 0;
        if (indirect_flag = (*p == '@'))
           {
            p = nonblank(p+1);		/* skip the '@'			*/
            nbytes = sizeof(doIndirect) - 1;
           }
        nbytes += strlen(p);

        if (nbytes > maxcmd)
           {		/*----- allocate enough space for cmd[] ------*/
            maxcmd = (nbytes>79) ? nbytes : 79;
            if (cmd)
                free(cmd);
            cmd = malloc(maxcmd);
            if (!cmd)
               {
                fprintf(stderr,"mdsdcl_dcl_parse: Out of space!\n");
                exit(0);
               }
            }

        sprintf(cmd,"%s%s",indirect_flag?doIndirect:"",p?p:"");

        sts = cli_dcl_parse(cmd,ctrl->tbladr[tabidx-1],
            mdsdcl_get_input,mdsdcl_get_input,&ctrl->prompt);
       }
    return(sts);
   }
