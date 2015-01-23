#include        "tclsysdef.h"

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
int TclClose(void *ctx)
{
  int sts;
  static const char promptWritefirst[] =
      "This tree has been modified, write it before closing? [Y]: ";
  char *exp=0;
  char *shotidstr=0;
  int shotid=-2;

  if (cli_get_value(ctx, "FILE", &exp) & 1) {
    cli_get_value(ctx, "SHOTID", &shotidstr);
    sscanf(shotidstr, "%d", &shotid);
    sts = TreeClose(exp, shotid);
  } else {
    int doall = cli_present(ctx, "ALL") & 1;
    while ((sts = TreeClose(0, 0)) & 1 && doall) ;
    if (doall && sts == TreeNOT_OPEN)
      sts = TreeNORMAL;
  }
  if (sts == TreeWRITEFIRST) {
    if (cli_present(ctx, "CONFIRM") == CLI_STS_NEGATED)
      sts = TreeQuitTree(0, 0);
    else {
      printf("Deal with asking to write first\n");
      /*
      printf(promptWritefirst);
      if (yesno(1)) {
	sts = TreeWriteTree(0, 0);
	if (sts & 1) {
	  TreeClose(0, 0);
	}
      } else
	sts = TreeQuitTree(0, 0);
      */
    }
  }
  if (sts & 1)
    TclNodeTouched(0, tree);
  else
    MdsMsg(sts, "TclClose: *WARN* unexpected status");
  if (exp)
    free(exp);
  if (shotidstr)
    free(shotidstr);
  return sts;
}
