#include        <mds_stdarg.h>
#include        "tclsysdef.h"
#include        <stdlib.h>

/**********************************************************************
* TCL_DIRECTORY.C --
*
* TclDirectory:  Perform directory function.
*
* History:
*  11-May-1998  TRG  Use TdiExecute to decode ascii shotnumber.
*  xx-Dec-1997  TRG  Create.  Ported from original mds code.
*
************************************************************************/

#define READONLY    1
#ifdef vms
#define TdiExecute    TDI$EXECUTE
#endif

extern int TdiExecute();

	/***************************************************************
	 * TclSetTree:
	 **************************************************************/
int TclSetTree(void *ctx)
{
  int sts;
  static int shot;
  static DESCRIPTOR_LONG(dsc_shot, &shot);
  char *filnam=0;
  char *asciiShot=0;

		/*--------------------------------------------------------
		 * Executable ...
		 *-------------------------------------------------------*/
  cli_get_value(ctx, "FILE", &filnam);
  cli_get_value(ctx, "SHOTID", &asciiShot);
  shot=atoi(asciiShot);
  if (cli_present(ctx, "READONLY") & 1)
    sts = TreeOpen(filnam, shot, READONLY);
  else
    sts = TreeOpen(filnam, shot, 0);
  if (sts & 1)
    TclNodeTouched(0, tree);
  else
    MdsMsg(sts, "Failed to open tree '%s', shot %s", filnam, asciiShot);
  if (filnam)
    free(filnam);
  if (asciiShot)
    free(asciiShot);
  return sts;
}
