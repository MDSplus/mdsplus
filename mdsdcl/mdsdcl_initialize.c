#include        <stdio.h>
#include        <stdlib.h>
#include        "mdsdcl.h"
#ifdef vms
#include        <ssdef.h>
#include        <unistd.h>
#include        <lib$routines.h>
#endif
#ifdef _WIN32
#include <io.h>
#define isatty(a) _isatty(a)
#endif

/***********************************************************************
* MDSDCL_INITIALIZE.C --
*
************************************************************************/


extern struct cduVerb  mdsdcl_tables[];


static DYNAMIC_DESCRIPTOR(dsc_emptyString);


	/****************************************************************
	 * mdsdcl_initialize:
	 ****************************************************************/
int   mdsdcl_initialize(		/* Return: status		*/
    struct _mdsdcl_ctrl  *ctrl		/* <m> the control structure	*/
   )
   {
    int   i,k;
    int   depth;
    struct _mdsdcl_io  *io;

    if (ctrl->tbladr[0])
        return(1);

    clear_buffer(ctrl,sizeof(*ctrl));

    ctrl->prompt = dsc_emptyString;
    ctrl->helplib = dsc_emptyString;
    ctrl->def_file = dsc_emptyString;
    str_copy_dx(&ctrl->def_file,"*.mds");

    ctrl->macro.list = 0;
    ctrl->macro.current = dsc_emptyString;
    ctrl->macro.saved = dsc_emptyString;
    for (i=0 ; i<7 ; i++)
        ctrl->macro.parameter[i] = dsc_emptyString;

		/*======================================================
		 * initialize io levels ...
		 *=====================================================*/
    k = sizeof(ctrl->ioLevel)/sizeof(ctrl->ioLevel[0]);
    for (depth=0 ; depth<k ; depth++)
       {
        io = ctrl->ioLevel + depth;
        io->last_command = dsc_emptyString;
        for (i=0 ; i<8 ; i++)
            io->ioParameter[i] = dsc_emptyString;
       }
    ctrl->ioLevel[0].fp = stdin;
    ctrl->ioLevel[0].isatty = isatty(fileno(stdin));

		/*======================================================
		 * initialize tables ...
		 *=====================================================*/
    ctrl->tbladr[0] = mdsdcl_tables;
    ctrl->tables = 1;
    return(1);
   }
