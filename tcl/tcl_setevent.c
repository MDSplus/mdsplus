#include        "tclsysdef.h"

/**********************************************************************
* TCL_SETEVENT.C --
*
* Set MDSplus event.
*
* History:
*  6-Sep-2000  TWF  Create.  Ported from original mdsPlus code.
*
***********************************************************************/

	/***************************************************************
	 * TclSetevent:
	 * Close tree file(s).
	 ***************************************************************/
int TclSetEvent(void *ctx)
{
  char *event = 0;
  int status;
  cli_get_value(ctx, "EVENT", &event);
  status = MDSEvent(event, 0, 0);
  if (event)
    free(event);
  return status;
}
