#include        "tclsysdef.h"

/**********************************************************************
* TCL_SET_DEFAULT.C --
*
* TclSetDefault:  Set default
* TclShowDefault:  Show default
*
* History:
*  15-Jan-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/



	/****************************************************************
	 * TclSetDefault:
	 ****************************************************************/
int   TclSetDefault()
   {
    int   nid;
    int   sts;
    static DYNAMIC_DESCRIPTOR(dsc_nodename);

    cli_get_value("NODE",&dsc_nodename);
    l2u(dsc_nodename.dscA_pointer,0);		/* to upper case	*/

    sts = TreeSetDefault(dsc_nodename.dscA_pointer,&nid);
    if (sts & 1)
        TclNodeTouched(nid,set_def);
    else
        mdsMsg(sts,"Error trying to set %s",dsc_nodename.dscA_pointer);
    return sts;
   }



	/***************************************************************
	 * TclShowDefault:
	 ***************************************************************/
int   TclShowDefault()		/* Returns: status			*/
   {
    char  *p;
    int nid;
    TreeGetDefaultNid(&nid);
    if (p = TreeGetPath(nid))
       {
        TclTextOut(p);
        TreeFree(p);
       }
    return(p ? 1 : 0);
   }
