#include        "tclsysdef.h"
#include        <mdsshr.h>

/***********************************************************************
* TCL_SET_CURRENT.C --
*
* TclSetCurrent:  set current Experiment and Shot
*
* History:
*  15-Jan-1998  TRG  Created.  Ported from original code.
*
************************************************************************/

#ifdef vms
#define TdiExecute  TDI$EXECUTE
#endif

extern int TdiExecute();

	/*****************************************************************
	 * TclSetCurrent:
	 *****************************************************************/
int TclSetCurrent(void *ctx)
{
  int sts;
  char *experiment=0;
  char *shotasc=0;
  int shot;
  
  DESCRIPTOR_LONG(dsc_shot, &shot);
  
  cli_get_value(ctx, "EXPERIMENT", &experiment);
  if (cli_present(ctx, "INCREMENT") & 1) {
    shot = TreeGetCurrentShotId(experiment);
    shot++;
    sts = TreeSetCurrentShotId(experiment, shot);
  } else {
    cli_get_value(ctx, "SHOT", &shotasc);
    shot=atoi(shotasc);
    if (shotasc)
      free(shotasc);
    sts = TreeSetCurrentShotId(experiment, shot);
  }

  if ((sts & 1) != 1)
    MdsMsg(sts, 0);
  if (experiment)
    free(experiment);
  return sts;
}
