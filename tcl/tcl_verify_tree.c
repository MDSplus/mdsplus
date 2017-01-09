#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <mdsshr.h>
#include <treeshr.h>

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
EXPORT int TclVerifyTree(void *ctx __attribute__ ((unused)), char **error, char **output __attribute__ ((unused)))
{				/* Returns: status                        */
  int sts;

  sts = TreeVerify();
  if (~sts & 1) {
    char *msg = MdsGetMsg(sts);
    *error = malloc(strlen(msg)+100);
    sprintf(*error,"Error: problem verifying tree\n"
	    "Error message was: %s\n", msg);
  }
  return (sts);
}
