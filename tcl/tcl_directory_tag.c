#include        "tclsysdef.h"
#include        <ncidef.h>
#include        <usagedef.h>

/**********************************************************************
* TCL_DIRECTORY_TAG.C --
*
* TclDirectoryTag:  Perform directory of all of the tags
*
* History:
*  16-Jan-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/


#define DSC(X)     ((struct descriptor *)X)



	/****************************************************************
	 * TclDirectoryTag:
	 * Perform directory of all of the tags
	 ****************************************************************/
int TclDirectoryTag()
   {
    int   sub_total;
    int   grand_total;
    char  *nodename;
    char  *tagnam;
    char  text[40];
    void  *ctx;
    static DYNAMIC_DESCRIPTOR(dsc_tagnam);
    static DYNAMIC_DESCRIPTOR(dsc_outLine);

    sub_total = grand_total = 0;
    while (cli_get_value("TAG",&dsc_tagnam) & 1)
       {
        tagnam = dsc_tagnam.dscA_pointer;
        l2u(tagnam,0);
        ctx = 0;
        sub_total = 0;
        TclTextOut(" ");
        sprintf(text,"Tag listing for %s",tagnam);
        TclTextOut(text);
        TclTextOut(" ");
        while (nodename = TreeFindTagWild(tagnam,0,&ctx))
           {
            TclTextOut(nodename);
            sub_total++;
           }
        TreeFindTagEnd(&ctx);
        grand_total += sub_total;
        sprintf(text,"Total of %d tags\n",sub_total);
        TclTextOut(text);
       }
    if (grand_total != sub_total)
       {
        sprintf(text,"Grand Total of %d tags\n",sub_total);
        TclTextOut(text);
       }
    return 1;
   }
