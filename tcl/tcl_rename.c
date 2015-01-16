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
int TclRename(void *ctx)
{
  int nid;
  int sts;
  char *srcnam=0;
  char *destnam=0;

  cli_get_value(ctx, "SOURCE", &srcnam);
  cli_get_value(ctx, "DEST", &destnam);
  sts = TreeFindNode(srcnam, &nid);
  if (sts & 1)
    sts = TreeRenameNode(nid, destnam);

  if (sts & 1)
    TclNodeTouched(nid, rename_node);
  else
    MdsMsg(sts, 0);
  if (srcnam)
    free(srcnam);
  if (destnam)
    free(destnam);

  return sts;
}
