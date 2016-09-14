#include <string.h>
#include <stdlib.h>

#include <dcl.h>
#include <mdstypes.h>
#include <libroutines.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <ncidef.h>
#include <usagedef.h>
#include <strroutines.h>
#include <libroutines.h>

/**********************************************************************
* TCL_SET_VIEW.C --
*
* TclSetView:  Set view date context.
*
* History:
*  10-AUG-2006  TWF  Create
*
************************************************************************/

	/****************************************************************
	 * TclSetView:
	 ****************************************************************/
EXPORT int TclSetView(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int status;
  int64_t viewDate = -1;
  char *viewDateStr = 0;
  cli_get_value(ctx, "DATE", &viewDateStr);
  if ((strcasecmp(viewDateStr, "NOW") == 0) ||
      ((status = LibConvertDateString(viewDateStr, &viewDate)) & 1)) {
    status = TreeSetViewDate(&viewDate);
  }
  if (!(status & 1)) {
    char *msg = MdsGetMsg(status);
    *error = malloc(strlen(msg) + 200);
    sprintf(*error, "Error: Bad time specified, use dd-mon-yyy hh:mm:ss format.\n"
	    "All fields required!\n" "Error message was: %s\n", msg);
  }
  if (viewDateStr)
    free(viewDateStr);
  return status;
}
