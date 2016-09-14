#include <string.h>
#include <stdlib.h>

#include <dcl.h>
#include <mdsshr.h>
#include <treeshr.h>

#include "tcl_p.h"


/***********************************************************************
* TCL_SET_CURRENT.C --
*
* TclSetCurrent:  set current Experiment and Shot
*
* History:
*  15-Jan-1998  TRG  Created.  Ported from original code.
*
************************************************************************/


extern int TdiExecute();

	/*****************************************************************
	 * TclSetCurrent:
	 *****************************************************************/
EXPORT int TclSetCurrent(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int sts;
  char *experiment = 0;
  char *shotasc = 0;
  int shot;

  cli_get_value(ctx, "EXPERIMENT", &experiment);
  if (cli_present(ctx, "INCREMENT") & 1) {
    shot = TreeGetCurrentShotId(experiment);
    shot++;
    sts = TreeSetCurrentShotId(experiment, shot);
  } else {
    cli_get_value(ctx, "SHOT", &shotasc);
    sts = tclStringToShot(shotasc,&shot, error);
    if (shotasc)
      free(shotasc);
    sts = TreeSetCurrentShotId(experiment, shot);
  }

  if (((sts & 1) != 1) && (*error == NULL)) {
    char *msg = MdsGetMsg(sts);
    *error = malloc(strlen(msg)+100);
    sprintf(*error, "Error: Unable to change current shot\n"
	    "Error message was: %s\n",msg);
  }
  if (experiment)
    free(experiment);
  return sts;
}
