#include        <stdio.h>
#include        <stdlib.h>
#include        "mdsdcl.h"

/*********************************************************************
* MDSDCL.C --
*
* The MDSDCL command line interpreter, main routine.
*
* History:
*  06-Apr-2001  TRG  Revised "@" processing removes "makeCmdlineMacro".
*  26-Apr-2000  TRG  Treat cmdline as special-case "macro", via
*                     makeCmdlineMacro.  Will allow processing of "@".
*  04-Dec-1997  TRG  Create.
*
**********************************************************************/


extern int   mdsdcl_do_command();
extern struct _mdsdcl_ctrl  *mdsdcl_ctrl_address();


#define CMD_PREP    1		/* Table-initialization command		*/

static struct cmd_struct  cmd0[2] = {
        0,   (void *)(sizeof(cmd0)/sizeof(cmd0[0]))
       ,CMD_PREP,          "PREP"
       };



	/***************************************************************
	 * main:  Mdsdcl main program
	 ***************************************************************/
int  main(
    int   argc
   ,char  *argv[]
   )
   {
    int   i,k;
    int   sts;
    char  *p;
    struct _mdsdcl_ctrl  *ctrl;
    static DYNAMIC_DESCRIPTOR(dsc_cmdline);

    set_pgmname(argv[0]);
    ctrl = mdsdcl_ctrl_address();

		/*=======================================================
		 * Command-line arguments ?
		 *======================================================*/
    str_free1_dx(&dsc_cmdline);
    if (argc > 1)
       {
		/*--------------------------------------------------------
		 * First, check for "PREP" command string, used to setup
		 *  an extra command table, like for TCL or CCL ...
		 *-------------------------------------------------------*/
        i = 1;
        if (IS_QUALIFIER_CHARACTER(*argv[i]))
           {
            str_copy_dx(&dsc_cmdline,argv[i]);
            if (dsc_cmdline.dscW_length == 1)
                str_append(&dsc_cmdline,argv[++i]);

            p = (char *)dsc_cmdline.dscA_pointer + 1;
            k = cmd_lookup(&p,cmd0,0,0,0);
            if (!k || (++i >= argc))
                exit(MdsMsg(0,"Bad command line"));

			/*------------------------------------------------
			 * The only legitimate command is "PREP" ...
			 *-----------------------------------------------*/
            str_copy_dx(&dsc_cmdline,argv[i++]);
            sts = mdsdcl_do_command(dsc_cmdline.dscA_pointer);
            if (~sts & 1)
                exit(sts);
            str_free1_dx(&dsc_cmdline);
           }

		/*--------------------------------------------------------
		 * Now handle the rest of the command line ...
		 *-------------------------------------------------------*/
        if (i < argc)
           {
            k = i;			/* save for comparison below	*/
            for ( ; i<argc ; i++)
               {
                if (i > k)
                    str_append(&dsc_cmdline," ");
                str_append(&dsc_cmdline,argv[i]);
                if (IS_QUALIFIER_CHARACTER(*argv[i]) && strlen(argv[i])==1)
                   {
                    if ((i+1) < argc)
                        str_append(&dsc_cmdline,argv[++i]);
                   }
               }
            sts = mdsdcl_do_command(&dsc_cmdline);
            for ( ; ctrl->depth > 0 ; )
               {
                sts = mdsdcl_do_command(0);
                {
                 struct _mdsdcl_io  *io;

                 io = ctrl->ioLevel + ctrl->depth;	/* for debug only*/
                }
               }
            exit(0);
           }
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
       }
   }						/*  */
