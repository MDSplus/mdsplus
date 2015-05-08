#include        "tclsysdef.h"
#include        <dbidef.h>
#include <string.h>

/**********************************************************************
* TCL_SHOW_DB.C --
*
* Show MDSplus database information.
*
* History:
*  04-May-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/

	/***************************************************************
	 * TclShowDb:
	 ***************************************************************/
int TclShowDB(void *ctx, char **error, char **output)
{
  int sts;
  char outstr[132];
  static int open;
  static int idx;
  static char nameStr[64 + 1];
  static char pathStr[132 + 1];
  static int nameLen;
  static int pathLen;
  static int shotid;
  static char modified;
  static char edit;
  static DBI_ITM itm1[] = {
    {sizeof(open), DbiNUMBER_OPENED, &open, 0}
    , {0}
  };
  static DBI_ITM itm2[] = {
    {sizeof(idx), DbiINDEX, &idx, 0}
    , {sizeof(nameStr) - 1, DbiNAME, nameStr, &nameLen}
    , {sizeof(shotid), DbiSHOTID, &shotid, 0}
    , {sizeof(modified), DbiMODIFIED, &modified, 0}
    , {sizeof(edit), DbiOPEN_FOR_EDIT, &edit, 0}
    , {sizeof(pathStr) - 1, DbiDEFAULT, pathStr, &pathLen}
    , {0}
  };

  sts = TreeGetDbi(itm1);
  if (sts & 1) {
    *output = strdup("");
    for (idx = 0; idx < open; idx++) {
      sts = TreeGetDbi(itm2);
      if (sts & 1) {
	nameStr[nameLen] = '\0';
	pathStr[pathLen] = '\0';
	sprintf(outstr, "%03d  %-32s  shot: %d [%s] %s%s\n",
		idx, nameStr, shotid, pathStr,
		edit ? "open for edit" : " ", modified ? ",modified" : " ");
	tclAppend(output, outstr);
      }
    }
    tclAppend(output, "\n");
  }
  return sts;
}
