#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <dcl.h>
#include <mdsshr.h>
#include <treeshr.h>

/**********************************************************************
* TCL_WRITE.C --
*
* Write tree file(s).
*
* History:
*  12-Mar-1998  TRG  Create.  Ported from original mdsPlus code.
*
***********************************************************************/

	/***************************************************************
	 * TclWrite:
	 ***************************************************************/
EXPORT int TclWrite(void *ctx, char **error __attribute__ ((unused)), char **output)
{
  int sts;
  char *exp = 0;
  if (cli_get_value(ctx, "FILE", &exp) & 1) {
    char *shotidStr = 0;
    int shotid;
    cli_get_value(ctx, "SHOTID", &shotidStr);
    shotid = atoi(shotidStr);
    sts = TreeWriteTree(exp, shotid);
    if (exp)
      free(exp);
    if (shotidStr)
      free(shotidStr);
  } else {
    sts = TreeWriteTree(0, 0);
  }

  if (~sts & 1) {
    char *msg = MdsGetMsg(sts);
    *output = malloc(strlen(msg) + 100);
    sprintf(*output, "Error: Unable to write tree\n" "Error message was: %s\n", msg);
  }
  return sts;
}
