#include <string.h>
#include <stdlib.h>

#include <dcl.h>
#include <mdsshr.h>
#include <treeshr.h>

#include "tcl_p.h"

/***********************************************************************
* TCL_EDIT.C --
*
* Open a tree for edit.
*
* History:
*  14-Apr-1998  TRG  Create.  Ported from original MDSplus code.
*
************************************************************************/

	/***************************************************************
	 * TclEdit:
	 * Open tree for edit
	 ***************************************************************/
EXPORT int TclEdit(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int shot;
  int sts;
  char *filnam = 0;
  char *asciiShot = 0;

  cli_get_value(ctx, "FILE", &filnam);
  cli_get_value(ctx, "SHOTID", &asciiShot);
  sscanf(asciiShot, "%d", &shot);
  if (cli_present(ctx, "NEW") & 1)
    sts = TreeOpenNew(filnam, shot);
  else
    sts = TreeOpenEdit(filnam, shot);
  if (sts & 1)
    TclNodeTouched(0, tree);
  else {
    char *msg = MdsGetMsg(sts);
    *error = malloc(strlen(msg) + 100);
    sprintf(*error, "Error: Unable to edit shot number %d of the '%s' tree\nError msg was: %s\n",
	    shot, filnam, msg);
  }
  if (filnam)
    free(filnam);
  if (asciiShot)
    free(asciiShot);
  return sts;
}
