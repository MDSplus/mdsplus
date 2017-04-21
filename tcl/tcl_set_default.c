#include <string.h>
#include <stdlib.h>

#include <dcl.h>
#include <mdsshr.h>
#include <treeshr.h>

#include "tcl_p.h"

/**********************************************************************
* TCL_SET_DEFAULT.C --
*
* TclSetDefault:  Set default
* TclShowDefault:  Show default
*
* History:
*  15-Jan-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/

	/****************************************************************
	 * TclSetDefault:
	 ****************************************************************/
EXPORT int TclSetDefault(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int nid;
  int sts;
  char *nodename;

  cli_get_value(ctx, "NODE", &nodename);
  sts = TreeSetDefault(nodename, &nid);
  if (sts & 1)
    TclNodeTouched(nid, set_def);
  else {
    char *msg = MdsGetMsg(sts);
    *error = malloc(strlen(msg) + strlen(nodename) + 100);
    sprintf(*error,"Error: Problem setting default to node '%s'\n"
	    "Error message was: %s\n", nodename, msg);
  }
  if (nodename)
    free(nodename);
  return sts;
}

	/***************************************************************
	 * TclShowDefault:
	 ***************************************************************/
EXPORT int TclShowDefault(void *ctx __attribute__ ((unused)),
			  char **error __attribute__ ((unused)), char **output)
{				/* Returns: status                        */
  char *p;
  int nid;
  TreeGetDefaultNid(&nid);
  if ((p = TreeGetPath(nid))) {
    *output = malloc(strlen(p) + 10);
    sprintf(*output, "%s\n", p);
    TreeFree(p);
  }
  return (p ? 1 : 0);
}
