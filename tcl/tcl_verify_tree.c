#include        "tclsysdef.h"

/***********************************************************************
* TCL_VERIFY_TREE.C --
*
* Call TreeVerifyTree routine.
* History:
*  13-Mar-1998  TRG  Create.  Port from original mdsPlus code.
*
************************************************************************/



	/***************************************************************
	 * TclVerifyTree:
	 ***************************************************************/
int   TclVerifyTree()		/* Returns: status			*/
   {
    int   sts;

    sts = TreeVerifyTree();
    if (~sts & 1)
        mdsMsg(sts,"TclVerifyTree: *WARN* status from TreeVerifyTree");
    return(sts);
   }
