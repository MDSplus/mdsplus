#include        <mdstypes.h>
#include	<config.h>
#include        "tclsysdef.h"
#include        <ncidef.h>
#include        <usagedef.h>

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
int TclSetView()
   {
    int   status;
    void  *ctx = 0;
    _int64 viewDate=-1;
    static DYNAMIC_DESCRIPTOR(dsc_viewdate_string);
    cli_get_value("DATE",&dsc_viewdate_string);
	StrUpcase(&dsc_viewdate_string,&dsc_viewdate_string);
    if (strcmp(dsc_viewdate_string.dscA_pointer,"NOW")==0) {
      status = TreeSetViewDate(&viewDate);
    }
    else if ((status = LibConvertDateString(dsc_viewdate_string.dscA_pointer,&viewDate)) & 1) {
      status = TreeSetViewDate(&viewDate);
    }
    if (!(status & 1)) TclTextOut("Bad time, use dd-mon-yyyy hh:mm:ss format. All fields required!");
    return status;
   }
