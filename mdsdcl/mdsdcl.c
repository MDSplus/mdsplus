#include        <stdio.h>
#include        <stdlib.h>
#include        "mdsdcl.h"

/*********************************************************************
* MDSDCL.C --
*
* The MDSDCL command line interpreter, main routine.
*
* History:
*  04-Dec-1997  TRG  Create.
*
**********************************************************************/


extern int   mdsdcl_do_command();



	/***************************************************************
	 * main:
	 ***************************************************************/
main(
    int   argc
   ,char  *argv[]
   )
   {
    int   i,k;
    int   sts;
    static DYNAMIC_DESCRIPTOR(dsc_cmdline);

		/*=======================================================
		 * Command-line arguments ?
		 *======================================================*/
    str_free1_dx(&dsc_cmdline);
    if (argc > 1)
       {
        for (i=1 ; i<argc ; i++)
           {
            if (i > 1)
                str_append(&dsc_cmdline," ");
            str_append(&dsc_cmdline,argv[i]);
            if (IS_QUALIFIER_CHARACTER(*argv[i]) && strlen(argv[i])==1)
               {
                if ((i+1) < argc)
                    str_append(&dsc_cmdline,argv[++i]);
               }
           }
        sts = mdsdcl_do_command(dsc_cmdline.dscA_pointer);
        if (strncmp(dsc_cmdline.dscA_pointer,"set command",11))
            exit(sts);
        str_free1_dx(&dsc_cmdline);
       }

		/*=======================================================
		 * No command-line arguments.  Normal interactive session
		 *======================================================*/
    sts = CLI_STS_NORMAL;
    for ( ; ; )
       {
        sts = mdsdcl_do_command(0);
        if (sts == CLI_STS_EOF)
            exit(0);

        if ((~sts & 01) && sts!=CLI_STS_NOCOMD)
            fprintf(stderr,"--> error status %08X\n\n\n",sts);
       }
   }						/*  */
