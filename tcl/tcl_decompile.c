#include        "tclsysdef.h"
#include        <mds_stdarg.h>
#include        <mdsshr.h>

/***********************************************************************
* TCL_DECOMPILE.C --
*
* Interpret a tdi expression ...
* History:
*  18-Mar-1998  TRG  Create.  Port from original mdsPlus code.
*
************************************************************************/

extern int TdiDecompile();

#ifdef vms
#define TdiDecompile  tdi$decompile
extern int  tdi$decompile();
#endif

	/****************************************************************
	 * TclDecompile:
	 ****************************************************************/
int TclDecompile()
   {
    int   i,k;
    int   nid;
    int   sts;
    char  *p;
    static DYNAMIC_DESCRIPTOR(dsc_string);
    static struct descriptor_xd xd = {0, 0, CLASS_XD, 0, 0};

    str_free1_dx(&dsc_string);
    cli_get_value("PATH",&dsc_string);
    sts = TreeFindNode(dsc_string.dscA_pointer,&nid);
    str_free1_dx(&dsc_string);
    if (sts & 1)
       {
        sts = TreeGetRecord(nid,&xd);
        if (sts & 1)
           {
            sts = TdiDecompile(&xd,&dsc_string MDS_END_ARG);
            if (sts & 1)
               {
                k = dsc_string.dscW_length;
                p = malloc(k+1);
                strncpy(p,dsc_string.dscA_pointer,k);
                p[k] = '\0';
                TclTextOut(p);
                free(p);
               }
           }
       }
    if (~sts & 1)
        MdsMsg(sts,0);
    return sts;
   }
