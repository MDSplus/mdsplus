#include        "tclsysdef.h"
#include        <mdsshr.h>

/**********************************************************************
* TCL_SHOW_VERSION.C --
*
* TclShowVersion:  Display MDSplus version
*
* History:
*  13-Sep-2002  TWF  Create.  
*
************************************************************************/


/***************************************************************
 * TclShowVersion:
 ***************************************************************/
int TclShowVersion()
   {
     TclTextOut(MdsRelease());
     return 1;
   }
