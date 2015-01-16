#include <config.h>
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#include        "tclsysdef.h"

/**********************************************************************
* TCL_WRITE.C --
*
* Write tree file(s).
*
* History:
*  12-Mar-1998  TRG  Create.  Ported from original mdsPlus code.
*
***********************************************************************/

	/***************************************************************
	 * TclWrite:
	 ***************************************************************/
int TclWrite(void *ctx)
{
  int sts;
  char *exp = 0;
  if (cli_get_value(ctx, "FILE", &exp) & 1) {
    char *shotidStr = 0;
    int shotid;
    cli_get_value(ctx, "SHOTID", &shotidStr);
    shotid = atoi(shotidStr);
    sts = TreeWriteTree(exp, shotid);
    if (exp)
      free(exp);
    if (shotidStr)
      free(shotidStr);
  } else {
    sts = TreeWriteTree(0, 0);
  }

  if (~sts & 1) {
#ifdef HAVE_WINDOWS_H
    char errormsg[1024];
    DWORD error = GetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, LANG_USER_DEFAULT, errormsg, 1024,
		  (va_list *) NULL);
    TclTextOut(errormsg);
#endif
    MdsMsg(sts, "TclWrite: *ERR* from TreeWriteTree");
  }
  return sts;
}
