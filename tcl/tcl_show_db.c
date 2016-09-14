#include <string.h>
#include <stdlib.h>

#include <dcl.h>
#include <dbidef.h>
#include <mdsshr.h>
#include <treeshr.h>

#include "tcl_p.h"

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
EXPORT int TclShowDB(void *ctx __attribute__ ((unused)), char **error __attribute__ ((unused)), char **output)
{
  int sts;
  int open;
  DBI_ITM itm1[] = {
    {sizeof(open), DbiNUMBER_OPENED, &open, 0}
    , {0}
  };
  sts = TreeGetDbi(itm1);
  if (sts & 1) {
    int idx;
    *output = strdup("");
    for (idx = 0; idx < open; idx++) {
      int shotid;
      char modified;
      char edit;
      char *outstr=0;
      DBI_ITM itm2[] = {
	{sizeof(idx), DbiINDEX, &idx, 0}
	, {0, DbiNAME, 0, 0}
	, {sizeof(shotid), DbiSHOTID, &shotid, 0}
	, {sizeof(modified), DbiMODIFIED, &modified, 0}
	, {sizeof(edit), DbiOPEN_FOR_EDIT, &edit, 0}
	, {0, DbiDEFAULT, 0, 0}
	, {0}
    };
      sts = TreeGetDbi(itm2);
      outstr = malloc(strlen(itm2[1].pointer)+strlen(itm2[5].pointer)+100);
      sprintf(outstr, "%03d  %-12s  shot: %d [%s] %s%s\n",
	      idx, (char *)itm2[1].pointer, shotid, (char *)itm2[5].pointer,
		edit ? "open for edit" : " ", modified ? ",modified" : " ");
      free(itm2[1].pointer);
      free(itm2[5].pointer);
      tclAppend(output, outstr);
      free(outstr);
    }
    tclAppend(output, "\n");
  }
  return sts;
}
