#include        "tclsysdef.h"


/*------------------------------------------------------------------------

		Name: TclSetTree

		Type:   C function

		Author:	Thomas W. Fredian
			MIT Plasma Fusion Center

		Date:    4-JAN-1988

		Purpose: Open a tree file for perusing

--------------------------------------------------------------------------

	Call sequence:

--------------------------------------------------------------------------
   Copyright (c) 1988
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:


+-------------------------------------------------------------------------*/


#define READONLY    1


	/***************************************************************
	 * TclSetTree:
	 **************************************************************/
int TclSetTree()
   {
    int   shot;
    int   sts;
    char  *filnam;
    static DYNAMIC_DESCRIPTOR(dsc_filnam);
    static DYNAMIC_DESCRIPTOR(dsc_ascShot);

		/*--------------------------------------------------------
		 * Executable ...
		 *-------------------------------------------------------*/
    cli_get_value("FILE",&dsc_filnam);
    cli_get_value("SHOTID",&dsc_ascShot);
    sts = (sscanf(dsc_ascShot.dscA_pointer,"%d",&shot)==1) ? 1 : 0;
    if (sts & 1)
       {
        filnam = dsc_filnam.dscA_pointer;
        if (cli_present("READONLY") & 1)
            sts = TreeOpen(filnam,shot,READONLY);
        else
            sts = TreeOpen(filnam,shot,0);
       }
    if (sts & 1)
        TclNodeTouched(0,tree);
    else
        MdsMsg(sts,"Failed to open tree %s",filnam);

    str_free1_dx(&dsc_filnam);
    str_free1_dx(&dsc_ascShot);
    return sts;
   }
