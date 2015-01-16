#include        "tclsysdef.h"

/**********************************************************************
* TCL_COMPRESS_DATAFILE.C --
*
* TclCompressDatafile:  Compress a datafile (delete unused space, etc???)
*
* History:
*  14-Apr-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/

	/****************************************************************
	 * TclCompressDatafile:
	 ****************************************************************/
int TclCompressDatafile(void *ctx)
{
  int shot;
  int sts;
  char *filnam=0;
  char *asciiShot=0;
  static char nocompress[] = "Compress of pulse file may destroy\
 previous archive versions:  REFUSED";

  cli_get_value(ctx, "FILE", &filnam);
  cli_get_value(ctx, "SHOTID", &asciiShot);
  sscanf(asciiShot, "%d", &shot);
  if ((shot != -1) && (!(cli_present(ctx, "OVERRIDE") & 1))) {
    TclTextOut(nocompress);
    sts = 1;
    goto done;
  }
  sts = TreeCompressDatafile(filnam, shot);
  if (~sts & 1) {
    MdsMsg(sts, "Problem compressing %s", filnam);
  }
 done:
  if (filnam)
    free(filnam);
  if (asciiShot)
    free(asciiShot);
  return sts;
}
