#include        "tclsysdef.h"
#include        <mdsshr.h>

/***********************************************************************
* TCL_SET_CURRENT.C --
*
* TclSetCurrent:  set current Experiment and Shot
*
* History:
*  15-Jan-1998  TRG  Created.  Ported from original code.
*
************************************************************************/


	/*****************************************************************
	 * TclSetCurrent:
	 *****************************************************************/
int TclSetCurrent()
   {
    int   ishot;
    int   sts;
    char  *experiment;
    static DYNAMIC_DESCRIPTOR(dsc_experiment);
    static DYNAMIC_DESCRIPTOR(dsc_shot);

    cli_get_value("EXPERIMENT",&dsc_experiment);
    experiment = dsc_experiment.dscA_pointer;
    if (cli_present("INCREMENT") & 1)
       {
        ishot = MdsGetCurrentShotId(experiment);
        ishot++;
        sts = MdsSetCurrentShotId(experiment,ishot);
       }
    else
       {
        cli_get_value("SHOT",&dsc_shot);
        sts = (sscanf(dsc_shot.dscA_pointer,"%d",&ishot) == 1) ? 1 : 0;
        if (sts & 1)
            sts = MdsSetCurrentShotId(experiment,ishot);
       }

    if ((sts & 1) != 1)
        MdsMsg(sts,0);
    return sts;
   }
