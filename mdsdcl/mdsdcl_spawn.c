#include        <stdio.h>
#include        "mdsdcl.h"
#if defined(vms)
#include        <lib$routines.h>
#include        <sys/clidef.h>
#include        <ssdef.h>
#elif defined(_WIN32)
#else
#include        <errno.h>
#include        <signal.h>
#include        <sys/wait.h>
#endif

/***********************************************************************
* MDSDCL_SPAWN.C --
*
************************************************************************/


#ifdef vms
int   sys$clref();
int   sys$waitfr();
#endif

extern int LibSpawn();

#if !(defined(_WIN32) || defined(vms))
static void  child_done(	/* Return: meaningless sts		*/
    int   sig			/* <r> signal number			*/
   )
   {
    if (sig == SIGCHLD)
        fprintf(stdout,"--> Process completed\n");
    else
        fprintf(stderr,"--> child_done: *NOTE*  sig=0x%08X\n",sig);

    return;
   }
#endif



	/**************************************************************
	 * mdsdcl_spawn:
	 **************************************************************/
int   mdsdcl_spawn(			/* Return: status		*/
    struct _mdsdcl_ctrl  *ctrl		/* <r> control structure	*/
   )
   {
    int   k;
    int   flags;
    int   ccflag;
    int   keypadFlag;
    int   lnmFlag;
    int   notifyFlag;
    int   symbolsFlag;
    int   waitFlag;
    int   sts;
    static DYNAMIC_DESCRIPTOR(dsc_cmd);
    static DYNAMIC_DESCRIPTOR(dsc_input);
    static DYNAMIC_DESCRIPTOR(dsc_output);
    static DYNAMIC_DESCRIPTOR(dsc_process_name);
    static DYNAMIC_DESCRIPTOR(dsc_prompt);
    static DYNAMIC_DESCRIPTOR(dsc_cli);
#if !defined(vms)
    char  cmdstring[256] = "";
    struct descriptor cmdstring_d = {0,DTYPE_T,CLASS_S,0};
#ifndef _WIN32
    pid_t  pid,xpid;
#endif
#endif

    flags = 0;
    str_free1_dx(&dsc_cmd);
    str_free1_dx(&dsc_input);
    str_free1_dx(&dsc_output);
    str_free1_dx(&dsc_process_name);
    str_free1_dx(&dsc_prompt);
    str_free1_dx(&dsc_cli);

    cli_get_value("COMMAND",&dsc_cmd);
    cli_get_value("CLI",&dsc_cli);
    cli_get_value("INPUT",&dsc_input);
    cli_get_value("OUTPUT",&dsc_output);
    cli_get_value("PROCESS",&dsc_process_name);
    cli_get_value("PROMPT",&dsc_prompt);
    ccflag = cli_present("CARRIAGE_CONTROL") & 1;
    keypadFlag = cli_present("KEYPAD") & 1;
    lnmFlag = cli_present("LOGICAL_NAMES") & 1;
    notifyFlag = cli_present("NOTIFY") & 1;
    symbolsFlag = cli_present("SYMBOLS") & 1;
    waitFlag = cli_present("WAIT") & 1;

#if defined(vms)
    if (!ccflag)
        flags |= CLI$M_NOCONTROL;
    if (!keypadFlag)
        flags |= CLI$M_NOKEYPAD;
    if (!lnmFlag)
        flags |= CLI$M_NOLOGNAM;
    if (notifyFlag)
        flags |= CLI$M_NOTIFY;
    if (!symbolsFlag)
        flags |= CLI$M_NOCLISYM;

    flags |= CLI$M_NOWAIT;
    if (waitFlag)
       {
		int   efn;
        lib$get_ef(&efn);
        sys$clref(efn);
        sts = lib$spawn(&dsc_cmd,&dsc_input,&dsc_output,&flags,
                    &dsc_process_name,0,0,&efn,0,0,&dsc_prompt,&dsc_cli);
        if (sts & 1)
            sys$waitfr(efn);	/* wait for spawned cmd to complete	*/
        lib$free_ef(&efn);
       }
    else
       {
        sts = lib$spawn(&dsc_cmd,&dsc_input,&dsc_output,&flags,
                    &dsc_process_name,0,0,0,0,0,&dsc_prompt,&dsc_cli);
       }
    if (~sts & 1)
        lib$signal(sts,0);
#else				/*---------------- unix ----------------*/
    k = 0;
    if (dsc_cmd.dscA_pointer)
    {
        k += sprintf(cmdstring+k,"%s",dsc_cmd.dscA_pointer);
        if (cmdstring[0] == '"' && cmdstring[strlen(cmdstring)-1] == '"')
	{
          k = sprintf(cmdstring,"%s",dsc_cmd.dscA_pointer+1);
          k--;
          cmdstring[k]='\0';
        }
    }
    if (dsc_input.dscA_pointer)
        k += sprintf(cmdstring+k," <%s",dsc_input.dscA_pointer);
    if (dsc_output.dscA_pointer)
        k += sprintf(cmdstring+k," >%s",dsc_output.dscA_pointer);
    cmdstring_d.dscW_length = k;
    cmdstring_d.dscA_pointer = cmdstring;
    sts = LibSpawn(&cmdstring_d,waitFlag,notifyFlag);
#endif
    return(sts == 0);
   }
