#include <string.h>
#include <stdlib.h>

#include <dcl.h>
#include <mdsshr.h>
#include <mdsdcl_messages.h>

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

EXPORT int TclWfevent(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  char *event = 0;
  char *timeout = 0;
  int seconds = 0;
  int status;
  cli_get_value(ctx, "EVENT", &event);
  cli_get_value(ctx, "TIMEOUT", &timeout);
  seconds = atoi(timeout);
  free(timeout);
  if (event) {
    if (seconds > 0) {
      status = MDSWfeventTimed(event, 0, 0, 0,seconds);
      if (!(status & 1))
	*error = strdup("Timeout\n");
    } else {
      status = MDSWfevent(event, 0, 0, 0);
    }
    free(event);
  }
  return 1;
}
