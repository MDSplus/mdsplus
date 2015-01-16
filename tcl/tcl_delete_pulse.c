#include        "tclsysdef.h"

/**********************************************************************
* TCL_DELETE_PULSE.C --
*
* TclDeletePulse:  Delete a whole pulse
*
* History:
*  27-Apr-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/

int TclDeletePulse(void *ctx)
{
  char *asciiShot=0;
  int shot;
  int sts;
  int delete_all = cli_present(ctx, "ALL") & 1;

  cli_get_value(ctx, "SHOT", &asciiShot);
  sscanf(asciiShot, "%d", &shot);
  free(asciiShot);
  sts = TreeDeletePulseFile(shot, delete_all);
  if (~sts & 1)
    sts = MdsMsg(sts, "Couldn't delete pulse file %d", shot);
  return sts;
}
