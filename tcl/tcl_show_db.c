/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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
