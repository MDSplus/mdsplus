#include <string.h>
#include <stdlib.h>

#include <dcl.h>
#include <mdsshr.h>
#include <treeshr.h>


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
EXPORT int TclRemoveTag(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int sts;
  char *tagnam = 0;

  cli_get_value(ctx, "TAGNAME", &tagnam);
  sts = TreeRemoveTag(tagnam);
  if (~sts & 1) {
    char *msg = MdsGetMsg(sts);
    *error = malloc(strlen(msg)+strlen(tagnam)+100);
    sprintf(*error,"Error: Failed to remove tag '%s'\n"
	    "Error message was: %s\n",tagnam,msg);
  }
  if (tagnam)
    free(tagnam);
  return sts;
}
