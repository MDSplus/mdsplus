#include        "ccldef.h"

/***********************************************************************
* CCL.C --
*
* The CCL main program
*
* History:
*  07-Jan-1998  TRG  Create.
*
************************************************************************/


#ifdef vms
static char  set_command[] = "\
set command ccl_commands\
  /prompt=\"CCL> \"\
  /def_file=\"*.ccl\"\
";
#else
static char  set_command[] = "\
set command ccl_commands\
  -prompt=\"CCL> \"\
  -def_file=\"*.ccl\"\
";
#endif


	/****************************************************************
	 * main:
	 * CCL main program ...
	 ****************************************************************/
main(				/* CCL main program			*/
    int   argc			/* <r> arg count			*/
   ,char  *argv[]		/* <r> arguments array			*/
   )
   {
    int   i,k;
    int   sts;

    set_pgmname(argv[0]);
    sts = mdsdcl_do_command(set_command);

    if (~sts & 1)
        exit(mdsMsg(sts,"Error in startup"));

    for ( ; ; )
       {
        sts = mdsdcl_do_command(0);
        if (sts == CLI_STS_EOF)
            exit(0);
       }
   }
