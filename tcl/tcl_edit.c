#include        "tclsysdef.h"

/***********************************************************************
* TCL_EDIT.C --
*
* Open a tree for edit.
*
* History:
*  14-Apr-1998  TRG  Create.  Ported from original MDSplus code.
*
************************************************************************/



	/***************************************************************
	 * TclEdit:
	 * Open tree for edit
	 ***************************************************************/
int TclEdit()
   {
    int   shot;
    int   sts;
    static DYNAMIC_DESCRIPTOR(dsc_filnam);
    static DYNAMIC_DESCRIPTOR(dsc_asciiShot);

    cli_get_value("FILE",&dsc_filnam);
    cli_get_value("SHOTID",&dsc_asciiShot);
    sscanf(dsc_asciiShot.dscA_pointer,"%d",&shot);
    if (cli_present("NEW") & 1)
        sts = TreeOpenNew(dsc_filnam.dscA_pointer,shot);
    else
        sts = TreeOpenEdit(dsc_filnam.dscA_pointer,shot);
    if (sts & 1)
        TclNodeTouched(0,tree);
    else
       {
        sts = MdsMsg(sts,"Error opening tree-file %s for EDIT",
                dsc_filnam.dscA_pointer);
#ifdef vms
        lib$signal(sts,0);
#endif
       }
    return sts;
   }
