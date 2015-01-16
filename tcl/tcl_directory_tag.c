#include        "tclsysdef.h"
#include        <ncidef.h>
#include        <usagedef.h>

/**********************************************************************
* TCL_DIRECTORY_TAG.C --
*
* TclDirectoryTag:  Perform directory of all of the tags
*
* History:
*  16-Jan-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/

#define DSC(X)     ((struct descriptor *)X)

	/****************************************************************
	 * TclDirectoryTag:
	 * Perform directory of all of the tags
	 ****************************************************************/
int TclDirectoryTag(void *ctx)
{
  int sub_total;
  int grand_total;
  char *nodename;
  char text[40];
  void *ctx1;
  char *tagnam=0;

  sub_total = grand_total = 0;
  while (cli_get_value(ctx, "TAG", &tagnam) & 1) {
    ctx = 0;
    sub_total = 0;
    TclTextOut(" ");
    sprintf(text, "Tag listing for %s", tagnam);
    TclTextOut(text);
    TclTextOut(" ");
    while ((nodename = TreeFindTagWild(tagnam, 0, &ctx1))) {
      TclTextOut(nodename);
      sub_total++;
    }
    TreeFindTagEnd(&ctx1);
    grand_total += sub_total;
    sprintf(text, "Total of %d tags\n", sub_total);
    TclTextOut(text);
  }
  if (grand_total != sub_total) {
    sprintf(text, "Grand Total of %d tags\n", sub_total);
    TclTextOut(text);
  }
  return 1;
}
