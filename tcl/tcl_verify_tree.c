#include        "tclsysdef.h"
#include <string.h>

/***********************************************************************
* TCL_VERIFY_TREE.C --
*
* Call TreeVerify routine.
* History:
*  13-Mar-1998  TRG  Create.  Port from original mdsPlus code.
*
************************************************************************/


	/***************************************************************
	 * TclVerifyTree:
	 ***************************************************************/
int TclVerifyTree(void *ctx, char **error, char **output)
{				/* Returns: status                        */
  int sts;

  sts = TreeVerify();
  if (~sts & 1) {
    char *msg = MdsGetMsg(sts);
    *error = malloc(strlen(msg)+100);
    sprintf(*error,"Error: problem verify tree\n"
	    "Error message was: %s\n", msg);
  }
  return (sts);
}
