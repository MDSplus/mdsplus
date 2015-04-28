#include <string.h>

#include <mdsshr.h>

#include "tcl_p.h"

/**********************************************************************
* TCL_SHOW_VERSION.C --
*
* TclShowVersion:  Display MDSplus version
*
* History:
*  13-Sep-2002  TWF  Create.  
*
************************************************************************/

/***************************************************************
 * TclShowVersion:
 ***************************************************************/
int TclShowVersion(void *ctx, char **error, char **output)
{
  *output = strdup(MdsRelease());
  tclAppend(output, "\n");
  return 1;
}
