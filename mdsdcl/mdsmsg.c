#include        <stdarg.h>
#include        <stdio.h>
#ifdef vms
#include        <lib$routines.h>
#include        <ssdef.h>
#include        <time.h>
#else
#include        <sys/time.h>
#endif

#define CREATE_STS_TEXT
#include        "mdsdcl.h"
#include        "clidef.h"
#include        "tcldef.h"
#include        "ccldef.h"

/**********************************************************************
* MDSMSG.C --
*
* Routine for printing status information ...
*
* History:
*  05-Jan-1998  TRG  Create.
*
************************************************************************/



	/*****************************************************************
	 * statusText:
	 *****************************************************************/
static char  *statusText(	/* Return: addr of "status" string	*/
    int   sts			/* <r> sts value			*/
   )
   {
    int   i,k;
    int   facility;
    int   max;
    char  *facilityText;
    struct stsText  *stsText;
    static char  text[32];

    facility = sts >> 16;
    if (facility == MDSDCL_FACILITY)
       {
        stsText = mdsdcl_stsText;	/* point to array		*/
        facilityText = "MDSDCL_FACILITY";
        max = sizeof(mdsdcl_stsText)/sizeof(mdsdcl_stsText[0]);
       }
    else if (facility == CLI_FACILITY)
       {
        stsText = cli_stsText;		/* point to array		*/
        facilityText = "CLI_FACILITY";
        max = sizeof(cli_stsText)/sizeof(cli_stsText[0]);
       }
    else if (facility == CCL_FACILITY)
       {
        stsText = ccl_stsText;		/* point to array		*/
        facilityText = "CCL_FACILITY";
        max = sizeof(ccl_stsText)/sizeof(ccl_stsText[0]);
       }
    else if (facility == TCL_FACILITY)
       {
        stsText = tcl_stsText;		/* point to array		*/
        facilityText = "TCL_FACILITY";
        max = sizeof(tcl_stsText)/sizeof(tcl_stsText[0]);
       }
    else
       {
        facilityText = "???";
        max = 0;
       }

    for (i=0 ; i<max ; i++)
       {
        if (sts == stsText[i].stsL_num)
            return(stsText[i].stsA_name);	/*---------> return	*/
       }

    sprintf(text,"%s %s : %08X",facilityText,(sts&1)?"Success":"Error",sts);
    return(text);
   }



	/*****************************************************************
	 * mdsMsg:
	 *****************************************************************/
int   mdsMsg(			/* Return: sts provided by user		*/
    int   sts			/* <r> status code			*/
   ,char  fmt[]			/* <r> format statement			*/
   , ...			/* <r:opt> arguments to fmt[]		*/
   )
   {
    int   i,k;
    char  text[256];
    va_list  ap;		/* arg ptr				*/

    sprintf(text,"%s: ",pgmname());
    k = strlen(text);
    va_start(ap,fmt);		/* initialize "ap"			*/
    vsprintf(text+k,fmt,ap);
    if (sts)
        fprintf(stderr,"%s\n    sts=%s\n\n",text,statusText(sts));
    else
        fprintf(stderr,"%s\n",text);
    return(sts);
   }
