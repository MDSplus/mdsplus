#include        "tclsysdef.h"

/***********************************************************************
* TCL_VERIFY_TREE.C --
*
* Call TreeVerify routine.
* History:
*  13-Mar-1998  TRG  Create.  Port from original mdsPlus code.
*
************************************************************************/


#ifdef vms
#define TreeVerify  TREE$VERIFY_TREE
int   TREE$VERIFY_TREE();
#endif



	/***************************************************************
	 * TclVerifyTree:
	 ***************************************************************/
int   TclVerifyTree()		/* Returns: status			*/
   {
    int   sts;

    sts = TreeVerify();
    if (~sts & 1)
        MdsMsg(sts,0);
    return(sts);
   }
