#include <string.h>
#include <stdlib.h>

#include <dcl.h>
#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <treeshr.h>

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
EXPORT int TclDoNode(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int sts;
  char *nodnam = 0;
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
  if (~sts & 1) {
    char *msg = MdsGetMsg(sts);
    *error = malloc(strlen(msg) + strlen(nodnam) + 100);
    sprintf(*error, "Error: problem doing node %s\n" "Error message was: %s\n", nodnam, msg);
  }
  free(nodnam);
  return sts;
}
