#include        "tclsysdef.h"

/**********************************************************************
* TCL_COMPRESS_DATAFILE.C --
*
* TclCompressDatafile:  Compress a datafile (delete unused space, etc???)
*
* History:
*  14-Apr-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/



	/****************************************************************
	 * TclCompressDatafile:
	 ****************************************************************/
int   TclCompressDatafile()
   {
    int   shot;
    int   sts;
    static DYNAMIC_DESCRIPTOR(dsc_filnam);
    static DYNAMIC_DESCRIPTOR(dsc_asciiShot);
    static char  nocompress[] = "Compress of pulse file may destroy\
 previous archive versions:  REFUSED";

    cli_get_value("FILE",&dsc_filnam);
    cli_get_value("SHOTID",&dsc_asciiShot);
    sscanf(dsc_asciiShot.dscA_pointer,"%d",&shot);
    if ((shot != -1) && (!(cli_present("OVERRIDE") & 1)))
       {
        TclTextOut(nocompress);
        return 1;
       }
    sts = TreeCompressDatafile(dsc_filnam.dscA_pointer,shot);
    if (~sts & 1)
       {
        MdsMsg(sts,"Problem compressing %s",dsc_filnam.dscA_pointer);
#ifdef vms
        lib$signal(sts,0);
#endif
       }
    return sts;
   }
