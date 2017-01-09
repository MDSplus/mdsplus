#include <string.h>
#include <stdlib.h>

#include <dcl.h>
#include <mdsshr.h>
#include <treeshr.h>

#include "tcl_p.h"

/**********************************************************************
* TCL_CLOSE.C --
*
* Close tree file(s).
*
* History:
*  12-Mar-1998  TRG  Create.  Ported from original mdsPlus code.
*
***********************************************************************/

	/***************************************************************
	 * TclClose:
	 * Close tree file(s).
	 ***************************************************************/
EXPORT int TclClose(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int sts;
  char *exp = 0;
  char *shotidstr = 0;
  int shotid;

  if (cli_get_value(ctx, "FILE", &exp) & 1) {
    cli_get_value(ctx, "SHOTID", &shotidstr);
    sts = tclStringToShot(shotidstr, &shotid, error);
    if (sts & 1)
      sts = TreeClose(exp, shotid);
  } else {
    int doall = cli_present(ctx, "ALL") & 1;
    while ((sts = TreeClose(0, 0)) & 1 && doall) ;
    if (doall && sts == TreeNOT_OPEN)
      sts = TreeNORMAL;
  }
  if (sts == TreeWRITEFIRST) {
    if (cli_present(ctx, "CONFIRM") & 1)
      sts = TreeQuitTree(0, 0);
    else
      *error = strdup("This tree has been modified. Either use the WRITE\n"
		      "command before closing to save modifications or\n"
		      "use CLOSE/CONFIRM to discard changes.\n\n");
  }
  if (sts & 1)
    TclNodeTouched(0, tree);
  if (exp)
    free(exp);
  if (shotidstr)
    free(shotidstr);
  return sts;
}
