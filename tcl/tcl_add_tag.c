#include        "tclsysdef.h"
#include        <ncidef.h>

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
int TclAddTag(void *ctx)
{				/* Return: status                 */
  int nid;
  int sts;
  char *nodnam=0;
  char *tagnam=0;

  cli_get_value(ctx, "NODE", &nodnam);
  cli_get_value(ctx, "TAGNAME", &tagnam);
  sts = TreeFindNode(nodnam, &nid);
  if (sts & 1)
    sts = TreeAddTag(nid, tagnam);
  if (!(sts & 1)) {
    MdsMsg(sts, "Error adding tag %s", tagnam);
  }
  if (nodnam)
    free(nodnam);
  if (tagnam)
    free(tagnam);
  return sts;
}
