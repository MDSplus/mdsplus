#include        "tclsysdef.h"

/**********************************************************************
* TCL_WRITE.C --
*
* Write tree file(s).
*
* History:
*  12-Mar-1998  TRG  Create.  Ported from original mdsPlus code.
*
***********************************************************************/



	/***************************************************************
	 * TclWrite:
	 ***************************************************************/
int TclWrite()
   {
    int   shotid;
    int   sts;
    static DYNAMIC_DESCRIPTOR(dsc_exp);
    static DYNAMIC_DESCRIPTOR(dsc_shotid);

    if (cli_get_value("FILE",&dsc_exp) & 1)
       {
        cli_get_value("SHOTID",&dsc_shotid);
        sscanf(dsc_shotid.dscA_pointer,"%d",&shotid);
        sts = TreeWriteTree(dsc_exp.dscA_pointer,shotid);
       }
    else
       {
        sts = TreeWriteTree(0,0);
       }

    if (~sts & 1)
        MdsMsg(sts,"TclWrite: *ERR* from TreeWriteTree");

    return sts;
   }
