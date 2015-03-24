#include        "tclsysdef.h"
#include        <mdsshr.h>
#include <string.h>

/**********************************************************************
* TCL_SHOW_VERSION.C --
*
* TclShowVersion:  Display MDSplus version
*
* History:
*  13-Sep-2002  TWF  Create.  
*
************************************************************************/

extern char *MdsRelease();

/***************************************************************
 * TclShowVersion:
 ***************************************************************/
int TclShowVersion(void *ctx, char **error, char **output)
{
  *output = strdup(MdsRelease());
  tclAppend(output, "\n");
  return 1;
}
