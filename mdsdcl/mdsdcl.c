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
    mdsdcl_main(argc,argv);
   }						/*  */
