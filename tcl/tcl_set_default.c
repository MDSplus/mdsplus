#include        "tclsysdef.h"

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
int TclSetDefault(void *ctx)
{
  int nid;
  int sts;
  char *nodename;

  cli_get_value(ctx, "NODE", &nodename);
  sts = TreeSetDefault(nodename, &nid);
  if (sts & 1)
    TclNodeTouched(nid, set_def);
  else
    MdsMsg(sts, "Error trying to set default to %s", nodename);
  if (nodename)
    free(nodename);
  return sts;
}

	/***************************************************************
	 * TclShowDefault:
	 ***************************************************************/
int TclShowDefault(void *ctx)
{				/* Returns: status                        */
  char *p;
  int nid;
  TreeGetDefaultNid(&nid);
  if ((p = TreeGetPath(nid))) {
    TclTextOut(p);
    TreeFree(p);
  }
  return (p ? 1 : 0);
}
