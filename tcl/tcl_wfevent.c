#include        "tclsysdef.h"
/**********************************************************************
* TCL_WFEVENT.C --
*
* Wait for an event
*
* History:
*  06-Sep-2000  TWF  Create.  Ported from original mdsPlus code.
*
***********************************************************************/

	/***************************************************************
	 * TclWfevent
	 * Wait for MDSplus event
	 ***************************************************************/

int TclWfevent(void *ctx)
{
  char *event = 0;
  cli_get_value(ctx, "EVENT", &event);
  if (event) {
    MDSWfevent(event, 0, 0, 0);
    free(event);
  }
  return 1;
}
