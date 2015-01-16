#include        "tclsysdef.h"

/***********************************************************************
* TCL_EDIT.C --
*
* Open a tree for edit.
*
* History:
*  14-Apr-1998  TRG  Create.  Ported from original MDSplus code.
*
************************************************************************/

	/***************************************************************
	 * TclEdit:
	 * Open tree for edit
	 ***************************************************************/
int TclEdit(void *ctx)
{
  int shot;
  int sts;
  char *filnam=0;
  char *asciiShot=0;

  cli_get_value(ctx, "FILE", &filnam);
  cli_get_value(ctx, "SHOTID", &asciiShot);
  sscanf(asciiShot, "%d", &shot);
  if (cli_present(ctx, "NEW") & 1)
    sts = TreeOpenNew(filnam, shot);
  else
    sts = TreeOpenEdit(filnam, shot);
  if (sts & 1)
    TclNodeTouched(0, tree);
  else {
    sts = MdsMsg(sts, "Error opening tree-file %s for EDIT", filnam);
  }
  if (filnam)
    free(filnam);
  if (asciiShot)
    free(asciiShot);
  return sts;
}
