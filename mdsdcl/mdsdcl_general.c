#include        <stdio.h>
#include        "mdsdcl.h"
#ifdef vms
#include        <lib$routines.h>
#include        <ssdef.h>
#include        <time.h>
#endif

/**********************************************************************
* MDSDCL_GENERAL.C --
*
* General-purpose routines, too short to bother with separate files.
*
* Includes:
*    mdsdcl_debug()
*    mdsdcl_exit()
*    mdsdcl_init_timer()
*    mdsdcl_show_timer()
*    mdsdcl_set_prompt()
*
* History:
*  05-Nov-1997  TRG  Create.
*
************************************************************************/



	/****************************************************************
	 * mdsdcl_debug:
	 ****************************************************************/
mdsdcl_debug()
   {
#ifdef vms
    lib$signal(SS$_DEBUG);
#else
    printf("\n--> DEBUG implemented for VMS only\n\n");
#endif
    printf("\n--> Want CLI debug messages?  <N>  ");
    cli_debug(yesno(0));
    return(1);
   }



	/****************************************************************
	 * mdsdcl_exit:
	 ****************************************************************/
mdsdcl_exit()
   {
    exit(0);
	return 1;
   }



	/****************************************************************
	 * mdsdcl_init_timer:
	 * mdsdcl_show_timer:
	 ****************************************************************/
mdsdcl_init_timer()
   {
    init_timer();
    return(1);
   }


mdsdcl_show_timer()
   {
    show_timer();
    return(1);
   }



	/**************************************************************
	 * mdsdcl_set_prompt:
	 **************************************************************/
int   mdsdcl_set_prompt(	/* Returns: status			*/
   struct _mdsdcl_ctrl  *ctrl	/* <m> the "ctrl" structure		*/
   )
   {
    return(cli_get_value("PROMPT",&ctrl->prompt));
   }



	/****************************************************************
	 * mdsdcl_type:
	 ****************************************************************/
int   mdsdcl_type()
   {
    int   sts;
    static DYNAMIC_DESCRIPTOR(dsc_text);

    sts = cli_get_value("P1",&dsc_text);
    printf("%s\n",(sts&1) ? dsc_text.dscA_pointer : " ");
    fflush(stdout);
    str_free1_dx(&dsc_text);
    return(1);
   }



	/****************************************************************
	 * mdsdcl_set_verify:
	 ****************************************************************/
mdsdcl_set_verify(		/* Returns: status			*/
   struct _mdsdcl_ctrl  *ctrl	/* <m> the "ctrl" structure		*/
   )
   {
    ctrl->verify = cli_present("VERIFY") & 1;
    return(1);
   }



	/****************************************************************
	 * mdsdcl_define_symbol:
	 ****************************************************************/
int   mdsdcl_define_symbol()
   {
    int   k;
    int   sts;
    char  *p;
    static DYNAMIC_DESCRIPTOR(dsc_name);
    static DYNAMIC_DESCRIPTOR(dsc_value);

    sts = cli_get_value("SYMBOL",&dsc_name);
    if (~sts & 1)
        return(sts);

    sts = cli_get_value("VALUE",&dsc_value);
    if (~sts & 1)
        return(cli_error(sts,"*ERR* getting value for symbol"));

#ifdef vms
    sts = lib$set_symbol(&dsc_name,&dsc_value);
    if (~sts & 1)
        sts = MDSDCL_STS_ERROR;
#else
    k = strlen(dsc_name.dscA_pointer) + strlen(dsc_value.dscA_pointer);
    p = malloc(k+2);
    if (!p)
        exit(cli_error(0,"*ERR* Out of space"));
    sprintf(p,"%s=%s",dsc_name.dscA_pointer,dsc_value.dscA_pointer);
    sts = putenv(p);
    if (sts)
       {
        perror("error from setenv");
        sts = MDSDCL_STS_ERROR;
       }
#endif

    return(sts);
   }
