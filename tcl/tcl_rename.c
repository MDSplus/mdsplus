#include <string.h>
#include <stdlib.h>

#include <dcl.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <mdsdcl_messages.h>

#include "tcl_p.h"

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
EXPORT int TclRename(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int nid;
  int sts = MdsdclIVVERB;
  char *srcnam = 0;
  char *destnam = 0;

  cli_get_value(ctx, "SOURCE", &srcnam);
  if (srcnam && (strlen(srcnam) > 0)) {
    cli_get_value(ctx, "DEST", &destnam);
    if (destnam && (strlen(destnam) > 0)) {
      sts = TreeFindNode(srcnam, &nid);
      if (sts & 1) {
	sts = TreeRenameNode(nid, destnam);
	if (sts & 1)
	  TclNodeTouched(nid, rename_node);
	else {
	  char *msg = MdsGetMsg(sts);
	  *error = malloc(strlen(srcnam) + strlen(destnam) + strlen(msg) + 100);
	  sprintf(*error, "Error: Problem renaming node %s to %s\n"
		  "Error message was: %s\n", srcnam, destnam, msg);
	}
      } else {
	*error = malloc(strlen(srcnam) + 100);
	sprintf(*error, "Error: Cannot find node %s\n", srcnam);
      }
    } else {
      *error = strdup("Error: destination node parameter missing\n");
    }
  } else {
    *error = strdup("Error: source node parameter missing\n");
  }
  if (srcnam)
    free(srcnam);
  if (destnam)
    free(destnam);

  return sts;
}
