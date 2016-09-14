#include <mdsshr.h>
#include <string.h>
#include <stdlib.h>
#include <dcl.h>
#include <treeshr.h>

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
EXPORT int TclAddTag(void *ctx, char **error, char **output __attribute__ ((unused)))
{				/* Return: status                 */
  int nid;
  int sts;
  char *nodnam = 0;
  char *tagnam = 0;

  cli_get_value(ctx, "NODE", &nodnam);
  cli_get_value(ctx, "TAGNAME", &tagnam);
  sts = TreeFindNode(nodnam, &nid);
  if (sts & 1)
    sts = TreeAddTag(nid, tagnam);
  if (!(sts & 1)) {
    char *msg = MdsGetMsg(sts);
    *error = malloc(strlen(tagnam) + strlen(msg) + 100);
    sprintf(*error, "Error adding tag %s\nError message was: %s\n", tagnam, msg);
  }
  if (nodnam)
    free(nodnam);
  if (tagnam)
    free(tagnam);
  return sts;
}
