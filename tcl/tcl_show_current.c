#include        "tclsysdef.h"
#include        <mdsshr.h>

/**********************************************************************
* TCL_SHOW_CURRENT.C --
*
* TclShowCurrent:  Display current shotid.
*
* History:
*  27-Apr-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/

	/***************************************************************
	 * TclShowCurrent:
	 ***************************************************************/
int TclShowCurrent(void *ctx)
{
  int shot;
  char text[80];
  char *experiment = 0;

  cli_get_value(ctx, "EXPERIMENT", &experiment);
  shot = TreeGetCurrentShotId(experiment);
  if (shot) {
    sprintf(text, "Current shot is %d", shot);
    TclTextOut(text);
  } else
    MdsMsg(0, "Failed to get shotid!");
  if (experiment)
    free(experiment);
  return 1;
}
