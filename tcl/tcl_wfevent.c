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

int TclWfevent()
{
    static DYNAMIC_DESCRIPTOR(event);
    cli_get_value("EVENT",&event);
    MDSWfevent(event.dscA_pointer,0,0,0);
    return 1;
}
