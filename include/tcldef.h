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
#define TCL_STS_SUCCESS      TCL_STS(1)

#ifdef CREATE_STS_TEXT
struct stsText  tcl_stsText[] = {
        STS_TEXT(TCL_STS_SUCCESS)
       };
#endif



	/*****************************************************************
	 * Function prototypes:  user functions
	 *****************************************************************/
int TclSetTree();

#endif		/* __TCLDEF_H	*/
