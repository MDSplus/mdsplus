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


#ifdef vms
#define TdiExecute  TDI$EXECUTE
#endif

extern int   TdiExecute();


	/*****************************************************************
	 * TclSetCurrent:
	 *****************************************************************/
int TclSetCurrent()
   {
    int   sts;
    char  *experiment;
    static int   shot;
    static DESCRIPTOR_LONG(dsc_shot,&shot);
    static DYNAMIC_DESCRIPTOR(dsc_experiment);
    static DYNAMIC_DESCRIPTOR(dsc_asciiShot);

    cli_get_value("EXPERIMENT",&dsc_experiment);
    experiment = dsc_experiment.dscA_pointer;
    if (cli_present("INCREMENT") & 1)
       {
        shot = TreeGetCurrentShotId(experiment);
        shot++;
        sts = TreeSetCurrentShotId(experiment,shot);
       }
    else
       {
        cli_get_value("SHOT",&dsc_asciiShot);
#ifdef vms
        dsc_asciiShot.dscB_class = CLASS_S;	/* vms: malloc vs str$	*/
        sts = TdiExecute(&dsc_asciiShot,&dsc_shot MDS_END_ARG);
        dsc_asciiShot.dscB_class = CLASS_D;
#else
        sts = TdiExecute(&dsc_asciiShot,&dsc_shot MDS_END_ARG);
#endif
        if (sts & 1)
            sts = TreeSetCurrentShotId(experiment,shot);
       }

    if ((sts & 1) != 1)
        MdsMsg(sts,0);
    return sts;
   }
