#include        "tclsysdef.h"

/**********************************************************************
* TCL_DO_NODE.C --
*
* TclDoNode:  Execute a node action.
*
* History:
*  27-Apr-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/


#ifdef vms
#define TdiDoTask  TDI$DO_TASK
#endif
extern int  TdiDoTask();


	/***************************************************************
	 * TclDoNode:
	 ***************************************************************/
int TclDoNode()
   {
    int   sts;
    static DYNAMIC_DESCRIPTOR(nodnam_dsc);
    static int retstatus;
    static int nid;
    static DESCRIPTOR_NID(niddsc,&nid);
    static DESCRIPTOR_LONG(retstatus_d,&retstatus);

    cli_get_value("NODE",&nodnam_dsc);
    l2u(nodnam_dsc.dscA_pointer,0);
    if ((sts = TreeFindNode(nodnam_dsc.dscA_pointer,&nid)) & 1)
       {
        sts = TdiDoTask(&niddsc,&retstatus_d);
        if (sts & 1)
            sts = retstatus;
       }
    if (~sts & 1)
        sts = MdsMsg(sts,"TclDoNode: error doing %s",
                        nodnam_dsc.dscA_pointer);
    return sts;
   }
