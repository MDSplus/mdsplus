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
int TclSetEvent()
   {
    static DYNAMIC_DESCRIPTOR(event);

    cli_get_value("EVENT",&event);
    return MDSEvent(event.dscA_pointer,0,0);
   }
