#include        "tclsysdef.h"
#include <setjmp.h>

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

static void EventOccurred(void *astprm, int len, char *data)
{
  longjmp(astprm,1);

}

int TclWfevent()
{
    jmp_buf environment;
    int eventid=-1;
    float forever = 1E6;
    static DYNAMIC_DESCRIPTOR(event);
    cli_get_value("EVENT",&event);
    if (setjmp(environment) == 0)
    {
      MDSEventAst(event.dscA_pointer, EventOccurred, environment, &eventid);
      LibWait(&forever);
    }
    else if (eventid >= 0)
    {
      MDSEventCan(eventid);
    }
    return 1;
}
