#include        "tclsysdef.h"
#include        <ncidef.h>
#include        <usagedef.h>

/**********************************************************************
* TCL_ADD_TAG.C --
*
* TclAddTag:  Add a tag name to a node
*
* History:
*  30-Jan-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/



	/*****************************************************************
	 * TclAddTag:
	 * Add a tag name to a node
	 *****************************************************************/
int   TclAddTag()		/* Return: status			*/
   {
    int nid;
    int sts;
    static DYNAMIC_DESCRIPTOR(dsc_nodnam);
    static DYNAMIC_DESCRIPTOR(dsc_tagnam);

    cli_get_value("NODE",&dsc_nodnam);
    cli_get_value("TAG",&dsc_tagnam);
    sts = TreeFindNode(dsc_nodnam.dscA_pointer,&nid);
    if (sts & 1)
        sts = TreeAddTag(nid,dsc_tagnam.dscA_pointer);
    if (!(sts & 1))
       {
        mdsMsg(sts,"Error adding tag %s",dsc_tagnam.dscA_pointer);
#ifdef vms
        lib$signal(sts,0);
#endif
       }
    return sts;
   }
