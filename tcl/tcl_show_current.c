#include        "tclsysdef.h"
#include        <mdsshr.h>

/**********************************************************************
* TCL_SHOW_CURRENT.C --
*
* TclShowCurrent:  Display current shotid.
*
* History:
*  27-Apr-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/


	/***************************************************************
	 * TclShowCurrent:
	 ***************************************************************/
int TclShowCurrent()
   {
    int   shot;
    char  text[80];
    static DYNAMIC_DESCRIPTOR(dsc_experiment);

    cli_get_value("EXPERIMENT",&dsc_experiment);
    shot = TreeGetCurrentShotId(dsc_experiment.dscA_pointer);
    if (shot)
       {
        sprintf(text,"Current shot is %d",shot);
        TclTextOut(text);
       }
    else
        MdsMsg(0,"Failed to get shotid!");
    return 1;
   }
