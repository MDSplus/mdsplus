#include        "tclsysdef.h"

/**********************************************************************
* TCL_CLOSE.C --
*
* Close tree file(s).
*
* History:
*  12-Mar-1998  TRG  Create.  Ported from original mdsPlus code.
*
***********************************************************************/



	/***************************************************************
	 * TclClose:
	 * Close tree file(s).
	 ***************************************************************/
int TclClose()
   {
    int sts;
    static char  promptWritefirst[] =
            "This tree has been modified, write it before closing? [Y]: ";
    static DYNAMIC_DESCRIPTOR(exp);
    static DYNAMIC_DESCRIPTOR(dsc_shotid);
    int shotid;

    if (cli_get_value("FILE",&exp) & 1)
       {
        cli_get_value("SHOTID",&dsc_shotid);
        sscanf(dsc_shotid.dscA_pointer,"%d",&shotid);
        sts = TreeClose(exp.dscA_pointer,shotid);
       }
    else
       {
        int doall =  cli_present("ALL") & 1;
        while ((sts = TreeClose(0,0)) & 1 && doall);
        if (doall && sts == TreeNOT_OPEN)
            sts = TreeNORMAL;
       }
    if (sts == TreeWRITEFIRST)
       {
        printf(promptWritefirst);
        if (yesno(1))
           {
            sts = TreeWriteTree(0,0);
            if (sts & 1)
               {
                TreeClose(0,0);
               }
           }
        else
          sts = TreeQuitTree(0,0);
       }
    if (sts & 1)
        TclNodeTouched(0,tree);
    else
        mdsMsg(sts,"TclClose: *WARN* unexpected status");
    return sts;
   }
