#include        <mds_stdarg.h>
#include        "tclsysdef.h"

/**********************************************************************
* TCL_DIRECTORY.C --
*
* TclDirectory:  Perform directory function.
*
* History:
*  11-May-1998  TRG  Use TdiExecute to decode ascii shotnumber.
*  xx-Dec-1997  TRG  Create.  Ported from original mds code.
*
************************************************************************/


#define READONLY    1
#ifdef vms
#define TdiExecute    TDI$EXECUTE
#endif


extern int   TdiExecute();


	/***************************************************************
	 * TclSetTree:
	 **************************************************************/
int TclSetTree()
   {
    int   sts;
    static int   shot;
    static DESCRIPTOR_LONG(dsc_shot,&shot);
    char  *filnam;
    static DYNAMIC_DESCRIPTOR(dsc_filnam);
    static DYNAMIC_DESCRIPTOR(dsc_asciiShot);

		/*--------------------------------------------------------
		 * Executable ...
		 *-------------------------------------------------------*/
    cli_get_value("FILE",&dsc_filnam);
    cli_get_value("SHOTID",&dsc_asciiShot);
    dsc_asciiShot.dscB_class = CLASS_S;		/* vms: malloc vs str$	*/
    sts = TdiExecute(&dsc_asciiShot,&dsc_shot MDS_END_ARG);
    dsc_asciiShot.dscB_class = CLASS_D;
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
        MdsMsg(sts,"Failed to open tree '%s', shot %s",(char *)dsc_filnam.dscA_pointer,(char *)dsc_asciiShot.dscA_pointer);

    str_free1_dx(&dsc_filnam);
    str_free1_dx(&dsc_asciiShot);
    return sts;
   }
