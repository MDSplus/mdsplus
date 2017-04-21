#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <dcl.h>

/**********************************************************************
* TCL_DELETE_PULSE.C --
*
* TclDeletePulse:  Delete a whole pulse
*
* History:
*  27-Apr-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/

EXPORT int TclDeletePulse(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  char *asciiShot = 0;
  int shot;
  int sts;
  int delete_all = cli_present(ctx, "ALL") & 1;

  cli_get_value(ctx, "SHOT", &asciiShot);
  sscanf(asciiShot, "%d", &shot);
  free(asciiShot);
  sts = TreeDeletePulseFile(shot, delete_all);
  if (~sts & 1) {
    char *msg = MdsGetMsg(sts);
    *error = malloc(strlen(msg) + 100);
    sprintf(*error, "Error: Could not delete pulse file %d\n" "Error message was: %s\n", shot, msg);
  }
  return sts;
}
