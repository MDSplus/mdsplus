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
    char  cmdstring[256];
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
        k += sprintf(cmdstring+k,"%s",dsc_cmd.dscA_pointer);
    if (dsc_input.dscA_pointer)
        k += sprintf(cmdstring+k," <%s",dsc_input.dscA_pointer);
    if (dsc_output.dscA_pointer)
        k += sprintf(cmdstring+k," >%s",dsc_output.dscA_pointer);
#ifndef _WIN32
    signal(SIGCHLD,notifyFlag ? child_done : SIG_DFL);
    pid = fork();
    if (!pid)
       {		/*-------------> child process: execute cmd	*/
        static char  *arglist[4];
		char  *p;

        printf("\nChild process: cmdstring='%s'\n",cmdstring);
        arglist[0] = nonblank(cmdstring);
        p = blank(arglist[0]);
        if (p)
           {
            *p = '\0';
            p = nonblank(p+1);
           }
        arglist[1] = p;
        printf("  arglist[0] = %s\n",arglist[0]);
        printf("  arglist[1] = %s\n",arglist[1]?arglist[1]:"Null");
        printf("\n");
        sts = execvp(arglist[0],arglist);

        printf("\nChild process:  try again ...\n");
        arglist[3] = 0;
        arglist[2] = arglist[1];
        arglist[1] = arglist[0];
        arglist[0] = "sh";
        printf("  arglist[0] = %s\n",arglist[0]);
        printf("  arglist[1] = %s\n",arglist[1]?arglist[1]:"Null");
        printf("  arglist[2] = %s\n",arglist[2]?arglist[2]:"Null");
        printf("\n");
        sts = execvp("/bin/sh",arglist);

        fprintf(stderr,"\n+++ *ERR* should never have gotten here!\n");
        perror("from inside mdsdcl_spawn()");
        fprintf(stderr,"\n");
        exit(1);
       }
			/*-------------> parent process ...		*/
    if (pid == -1)
       {
        fprintf(stderr,"Error %d from fork()\n",errno);
        return(0);
       }
    if (waitFlag)
       {
        for ( ; ; )
           {
            xpid = wait(&sts);
            if (xpid == pid)
                break;
           }
       }
#else
	sts = system(cmdstring) == 0;
#endif
#endif
    return(sts);
   }
