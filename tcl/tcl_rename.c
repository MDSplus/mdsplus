#include        "tclsysdef.h"

/***********************************************************************
* TCL_RENAME.C --
*
* Rename a node or subtree.
* History:
*  20-Mar-1998  TRG  Create.  Port from original mdsPlus code.
*
************************************************************************/



	/***************************************************************
	 * TclRename:
	 * Rename a node or subtree.
	 ***************************************************************/
int   TclRename()
   {
    int nid;
    int sts;
    static DYNAMIC_DESCRIPTOR(srcnam);
    static DYNAMIC_DESCRIPTOR(destnam);

    cli_get_value("SOURCE",&srcnam);
    cli_get_value("DEST",&destnam);
    sts = TreeFindNode(srcnam.dscA_pointer,&nid);
    if (sts & 1)
        sts = TreeRenameNode(nid,destnam.dscA_pointer);

    if (sts & 1)
        TclNodeTouched(nid,rename_node);
    else
        MdsMsg(sts,0);

    return sts;
   }
