#include        "tclsysdef.h"
#include <string.h>

/**********************************************************************
* TCL_CLEAN_DATAFILE.C --
*
* TclCleanDatafile:  Clean a datafile (delete unused space, etc???)
*
* History:
*  14-Apr-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/

extern int TdiExecute();

	/****************************************************************
	 * TclCleanDatafile:
	 ****************************************************************/
int TclCleanDatafile(void *ctx)
{
  int sts;
  static int shot;
  char *filnam=0;
  char *asciiShot=0;
  static DESCRIPTOR_LONG(dsc_shot, &shot);
  struct descriptor dsc_asciiShot = {0, DTYPE_T, CLASS_S, 0};
  static char noclean[] = "Clean of pulse file may destroy\
 previous archive versions:  REFUSED";

  cli_get_value(ctx, "FILE", &filnam);
  cli_get_value(ctx, "SHOTID", &asciiShot);
  dsc_asciiShot.length=strlen(asciiShot);
  dsc_asciiShot.pointer=asciiShot;
  sts = TdiExecute(&dsc_asciiShot, &dsc_shot MDS_END_ARG);
  if ((shot != -1) && (!(cli_present(ctx, "OVERRIDE") & 1))) {
    TclTextOut(noclean);
    sts=1;
    goto done;
  }
  sts = TreeCleanDatafile(filnam, shot);
  if (~sts & 1) {
    MdsMsg(sts, "Problem cleaning %s", filnam);
  }
 done:
  if (filnam)
    free(filnam);
  if (asciiShot)
    free(asciiShot);
  return sts;
}
