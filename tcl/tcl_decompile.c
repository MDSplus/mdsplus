#include 	<config.h>
#include        "tclsysdef.h"
#include        <mds_stdarg.h>
#include        <mdsshr.h>
#include 	<stdlib.h>
#ifdef HAVE_ALLOCA_H
#include        <alloca.h>
#endif
#include        <string.h>
#include        <dcl.h>

/***********************************************************************
* TCL_DECOMPILE.C --
*
* Interpret a tdi expression ...
* History:
*  18-Mar-1998  TRG  Create.  Port from original mdsPlus code.
*
************************************************************************/

extern int TdiDecompile();

#ifdef vms
#define TdiDecompile  tdi$decompile
extern int tdi$decompile();
#endif

extern int StrFree1Dx();

	/****************************************************************
	 * TclDecompile:
	 ****************************************************************/
int TclDecompile(void *ctx, char **error, char **output)
{
  int nid;
  int sts = MdsdclIVVERB;
  char *path = 0;
  struct descriptor dsc_string = { 0, DTYPE_T, CLASS_D, 0 };
  static struct descriptor_xd xd = { 0, 0, CLASS_XD, 0, 0 };

  cli_get_value(ctx, "PATH", &path);
  if (path != NULL) {
    sts = TreeFindNode(path, &nid);
    free(path);
    if (sts & 1) {
      sts = TreeGetRecord(nid, &xd);
      if (sts & 1) {
	sts = TdiDecompile(&xd, &dsc_string MDS_END_ARG);
	if (sts & 1) {
	  *output = malloc(dsc_string.length + 100);
	  sprintf(*output, "%.*s\n", dsc_string.length, dsc_string.pointer);
	}
	StrFree1Dx(&dsc_string);
      }
    }
    if (~sts & 1) {
      char *msg = MdsGetMsg(sts);
      *error = malloc(strlen(msg) + 100);
      sprintf(*error, "Error: Unable to get data for this node.\n" "Error message was: %s\n", msg);
    }
  }
  return sts;
}
