#include <string.h>
#include <stdlib.h>

#include <dcl.h>
#include <mdsshr.h>
#include <treeshr.h>

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
EXPORT int TclShowCurrent(void *ctx, char **error, char **output)
{
  int shot;
  char *experiment = 0;

  cli_get_value(ctx, "EXPERIMENT", &experiment);
  shot = TreeGetCurrentShotId(experiment);
  if (shot) {
    *output = malloc(100);
    sprintf(*output, "Current shot is %d\n", shot);
  } else {
    *error = strdup("Failed to get shotid.\n");
  }
  if (experiment)
    free(experiment);
  return 1;
}
