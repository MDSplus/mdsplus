#include        "tclsysdef.h"
#include        <ncidef.h>
#include        <usagedef.h>

/**********************************************************************
* TCL_ADD_NODE.C --
*
* TclAddNode:  Add a node
*
* History:
*  02-Feb-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/

#define DSC(A)        (struct descriptor *)(A)

	/*****************************************************************
	 * TclAddNode:
	 * Add a node
	 *****************************************************************/
int TclAddNode(void *ctx)
{				/* Return: status                 */
  int k;
  int nid;
  int sts;
  int stsQual;
  char usage=0;
  char *nodnam=0;
  char *modelType=0;
  char *qualifiers=0;
  char *usageStr=0;
  sts = cli_get_value(ctx, "NODENAME", &nodnam);

  if (cli_get_value(ctx, "MODEL", &modelType) & 1) {
    stsQual = cli_get_value(ctx, "QUALIFIERS", &qualifiers);
    if (stsQual & 1)
      fprintf(stderr, "--> QUALIFIERS option is obsolete (ignored)\n");
    sts = TreeAddConglom(nodnam, modelType, &nid);
  } else {
    cli_get_value(ctx, "USAGE", &usageStr);
    printf("Need to handle usage\n");
    sts = TreeAddNode(nodnam, &nid, usage);
  }
  if (sts & 1)
    TclNodeTouched(nid, new);
  else {
    MdsMsg(sts, "Error adding node %s", nodnam);
  }
  if (nodnam)
    free(nodnam);
  if (modelType)
    free(modelType);
  if (qualifiers)
    free(qualifiers);
  if (usageStr)
    free(usageStr);
  return sts;
}
