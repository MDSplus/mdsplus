#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dcl.h>
#include <mdsshr.h>
#include <treeshr.h>
#include "tcl_p.h"

/**********************************************************************
* TCL_CLEAN_DATAFILE.C --
*
* TclCleanDatafile:  Clean a datafile (delete unused space, etc???)
*
* History:
*  14-Apr-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/

extern int TdiExecute();

	/****************************************************************
	 * TclCleanDatafile:
	 ****************************************************************/
EXPORT int TclCleanDatafile(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int sts;
  int shot;
  char *filnam = 0;
  char *asciiShot = 0;
  cli_get_value(ctx, "FILE", &filnam);
  cli_get_value(ctx, "SHOTID", &asciiShot);
  sts = tclStringToShot(asciiShot, &shot, error);
  if (sts & 1) {
    sts = TreeCleanDatafile(filnam, shot);
    if (!(sts & 1)) {
      char *msg = MdsGetMsg(sts);
      *error = malloc(strlen(msg) + strlen(filnam) + 100);
      sprintf(*error, "Error: Problem cleaning tree '%s' shot '%d'\nError message was: %s\n",
	      filnam, shot, msg);
    }
  }
  if (filnam)
    free(filnam);
  if (asciiShot)
    free(asciiShot);
  return sts;
}
