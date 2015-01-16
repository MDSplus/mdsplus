#include        "tclsysdef.h"
#include        <mds_stdarg.h>
#include        <mdsshr.h>
#include        <alloca.h>
#include        <string.h>

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
int TclDecompile(void *ctx)
{
  int nid;
  int sts;
  char *path_string=0;
  struct descriptor dsc_string = {0, DTYPE_T, CLASS_D, 0};
  static struct descriptor_xd xd = { 0, 0, CLASS_XD, 0, 0 };

  str_free1_dx(&path_string);
  cli_get_value(ctx, "PATH", &path_string);
  sts = TreeFindNode(path_string, &nid);
  free(path_string);
  if (sts & 1) {
    sts = TreeGetRecord(nid, &xd);
    if (sts & 1) {
      sts = TdiDecompile(&xd, &dsc_string MDS_END_ARG);
      if (sts & 1) {
	char *p=alloca(dsc_string.length+1);
	strncpy(p, dsc_string.pointer, dsc_string.length);
	p[dsc_string.length] = '\0';
	TclTextOut(p);
      }
      StrFree1Dx(&dsc_string);	/* need to use mdsplus free */
    }
  }
  if (~sts & 1)
    MdsMsg(sts, 0);
  return sts;
}
