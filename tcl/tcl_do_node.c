#include        "tclsysdef.h"
#include		<mds_stdarg.h>

/**********************************************************************
* TCL_DO_NODE.C --
*
* TclDoNode:  Execute a node action.
*
* History:
*  27-Apr-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/

extern int TdiDoTask();

	/***************************************************************
	 * TclDoNode:
	 ***************************************************************/
int TclDoNode(void *ctx)
{
  int sts;
  char *nodnam=0;
  static int retstatus;
  static int nid;
  static DESCRIPTOR_NID(niddsc, &nid);
  static DESCRIPTOR_LONG(retstatus_d, &retstatus);

  cli_get_value(ctx, "NODE", &nodnam);
  if ((sts = TreeFindNode(nodnam, &nid)) & 1) {
    sts = TdiDoTask(&niddsc, &retstatus_d MDS_END_ARG);
    if (sts & 1)
      sts = retstatus;
  }
  if (~sts & 1)
    sts = MdsMsg(sts, "TclDoNode: error doing %s", nodnam);
  free(nodnam);
  return sts;
}
