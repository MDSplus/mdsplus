#include        <mdstypes.h>
#include	<config.h>
#include        "tclsysdef.h"
#include        <ncidef.h>
#include        <usagedef.h>
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
int TclSetView(void *ctx)
{
  int status;
  int64_t viewDate = -1;
  char *viewDateStr = 0;
  cli_get_value(ctx, "DATE", &viewDateStr);
  if ((strcasecmp(viewDateStr,"NOW")==0) ||
      ((status = LibConvertDateString(viewDateStr, &viewDate)) & 1)) {
    status = TreeSetViewDate(&viewDate);
  }
  if (!(status & 1))
    TclTextOut("Bad time, use dd-mon-yyyy hh:mm:ss format. All fields required!");
  if (viewDateStr)
    free(viewDateStr);
  return status;
}
