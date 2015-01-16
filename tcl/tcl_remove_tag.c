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
int TclRemoveTag(void *ctx)
{
  int sts;
  char *tagnam=0;

  cli_get_value(ctx, "TAGNAME", &tagnam);
  sts = TreeRemoveTag(tagnam);
  if (~sts & 1)
    sts = MdsMsg(sts, "Failed to remove tag '%s'", tagnam);
  if (tagnam)
    free(tagnam);
  return sts;
}
