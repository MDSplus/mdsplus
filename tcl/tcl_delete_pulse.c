#include        "tclsysdef.h"

/**********************************************************************
* TCL_DELETE_PULSE.C --
*
* TclDeletePulse:  Delete a whole pulse
*
* History:
*  27-Apr-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/


int TclDeletePulse()
   {
    static DYNAMIC_DESCRIPTOR(dsc_asciiShot);
    int shot;
    int sts;
    int delete_all = cli_present("ALL") & 1;

    cli_get_value("SHOT",&dsc_asciiShot);
    sscanf(dsc_asciiShot.dscA_pointer,"%d",&shot);
    sts = TreeDeletePulseFile(shot,delete_all);
    if (~sts & 1)
        sts = MdsMsg(sts,"Couldn't delete pulse file %d",shot);
    return sts;
   }
