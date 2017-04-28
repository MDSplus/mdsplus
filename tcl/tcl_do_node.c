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
  INIT_STATUS, retstatus;
  char *nodnam = NULL;
  int nid;
  DESCRIPTOR_NID(niddsc, &nid);
  DESCRIPTOR_LONG(retstatus_d, &retstatus);
  cli_get_value(ctx, "NODE", &nodnam);
  status = TreeFindNode(nodnam, &nid);
  if STATUS_OK {
    status = TdiDoTask(&niddsc, &retstatus_d MDS_END_ARG);
    if STATUS_OK
      status = retstatus;
  }
  if STATUS_NOT_OK {
    char *msg = MdsGetMsg(status);
    *error = malloc(strlen(msg) + strlen(nodnam) + 100);
    sprintf(*error, "Error: problem doing node %s\n" "Error message was: %s\n", nodnam, msg);
  }
  free(nodnam);
  return status;
}
