#include        <stdio.h>
#include        "mdsdcl.h"
#ifdef vms
#include        <ssdef.h>
#include        <stdarg.h>
#include        <lib$routines.h>
#else
#include        <stdlib.h>
#include        <sys/stat.h>
#endif

/***********************************************************************
* MDSDCL_SHOW_VM.C --
*
* Include new command table in mdsdcl.
*
* History:
*  11-Apr-2001  TWF  Create.
*
************************************************************************/


	/****************************************************************
	 * mdsdcl_show_vm:
	 ****************************************************************/

#ifndef _tolower
#define _tolower(c) (((c) >= 'A' && (c) <= 'Z') ? (c) | 0x20 : (c))
#endif

int   MDSDCL_SHOW_VM(		/* Return: status		*/
    struct _mdsdcl_ctrl  *ctrl		/* <m> the control structure	*/
   )
   {
#ifdef malloc
     int full = cli_present("FULL") & 1;
     MdsShowVM(full);
#else
     MDSprintf("System not built with memcheck enabled. SHOW VM command is unavailable.\n");
#endif
    return(1);
   }
