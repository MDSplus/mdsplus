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
#include <stdio.h>
#include <stdlib.h>

#include <ncidef.h>
#include <dcl.h>
#include <mdsshr.h>
#include <treeshr.h>

#include "tcl_p.h"


/**********************************************************************
* TCL_DIRECTORY_TAG.C --
*
* TclDirectoryTag:  Perform directory of all of the tags
*
* History:
*  16-Jan-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/

	/****************************************************************
	 * TclDirectoryTag:
	 * Perform directory of all of the tags
	 ****************************************************************/
EXPORT int TclDirectoryTag(void *ctx, char **error __attribute__ ((unused)), char **output)
{
  int sub_total;
  int grand_total;
  char *nodename;
  char text[400];
  void *ctx1;
  char *tagnam = 0;
  int path = cli_present(ctx, "PATH") & 1;

  *output = strdup("");
  sub_total = grand_total = 0;
  while (cli_get_value(ctx, "TAG", &tagnam) & 1 && (strlen(tagnam) < 24)) {
    ctx1 = 0;
    sub_total = 0;
    sprintf(text, "\nTag listing for %s\n\n", tagnam);
    tclAppend(output, text);
    while ((nodename = TreeFindTagWild(tagnam, 0, &ctx1))) {
      if (path) {
	NCI_ITM itmlist[] = { {0, NciFULLPATH, 0, 0}
	, {0, 0, 0, 0}
	};
	int nid;
	TreeFindNode(nodename, &nid);
	TreeGetNci(nid, itmlist);
	if (itmlist[0].pointer) {
	  char *info = malloc(strlen(nodename) + strlen(itmlist[0].pointer) + 100);
sprintf(info, "%s%.*s = %s\n", nodename, (int)(40 - strlen(nodename)),
	  "                                               ", (char *)itmlist[0].pointer);
	  tclAppend(output, info);
	  free(info);
	  free(itmlist[0].pointer);
	}
      } else {
	sprintf(text, "%s\n", nodename);
	tclAppend(output, text);
      }
      sub_total++;
      mdsdclFlushOutput(*output);
    }
    TreeFindTagEnd(&ctx1);
    grand_total += sub_total;
    sprintf(text, "Total of %d tags\n", sub_total);
    tclAppend(output, text);
    if (tagnam)
      free(tagnam);
  }
  if (grand_total != sub_total) {
    sprintf(text, "Grand Total of %d tags\n", sub_total);
    tclAppend(output, text);
  }
  return 1;
}
