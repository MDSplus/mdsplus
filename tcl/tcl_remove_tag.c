#include        "tclsysdef.h"

/**********************************************************************
* TCL_REMOVE_TAG.C --
*
* Remove a tag name.
*
* History:
*  28-Apr-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/


	/***************************************************************
	 * TclRemoveTag:
	 *  Remove a tag name
	 ***************************************************************/
int TclRemoveTag()
   {
    int   sts;
    static DYNAMIC_DESCRIPTOR(dsc_tagnam);

    cli_get_value("TAG",&dsc_tagnam);
    l2u(dsc_tagnam.dscA_pointer,0);
    sts = TreeRemoveTag(dsc_tagnam.dscA_pointer);
    if (~sts & 1)
        sts = MdsMsg(sts,"Failed to remove tag '%s'",
                dsc_tagnam.dscA_pointer);
    return sts;
   }
