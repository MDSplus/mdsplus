#ifdef __vms
#ifndef vms
#define vms    1
#endif
#endif

#ifndef __TCLDEF_H
#define __TCLDEF_H    1

#include        <stdio.h>
#include        <stdlib.h>

#include        "mdsdcldef.h"
#include        "clidef.h"


/***********************************************************************
* TCLDEF.H --
*
************************************************************************/

#define TCL_STS(N)     (TCL_FACILITY<<16)+N
#define TCL_STS_NORMAL            TCL_STS(1)
#define TCL_STS_FAILED_ESSENTIAL  TCL_STS(2)

#ifdef CREATE_STS_TEXT
struct stsText  tcl_stsText[] = {
        STS_TEXT(TCL_STS_NORMAL,"Normal successful completion")
       ,STS_TEXT(TCL_STS_FAILED_ESSENTIAL,"Essential action failed")
       };
#endif



	/*****************************************************************
	 * Function prototypes:  user functions
	 *****************************************************************/
int TclSetTree();

#endif		/* __TCLDEF_H	*/
