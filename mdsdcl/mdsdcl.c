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
    int   sts;
#define AT_START  2

    termio_init(argc,argv);

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
