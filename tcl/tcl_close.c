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
#include <mdsshr.h>
#include <treeshr.h>

#include "tcl_p.h"

/**********************************************************************
* TCL_CLOSE.C --
*
* Close tree file(s).
*
* History:
*  12-Mar-1998  TRG  Create.  Ported from original mdsPlus code.
*
***********************************************************************/

	/***************************************************************
	 * TclClose:
	 * Close tree file(s).
	 ***************************************************************/
EXPORT int TclClose(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int sts;
  char *exp = 0;
  char *shotidstr = 0;
  int shotid;

  if (cli_get_value(ctx, "FILE", &exp) & 1) {
    cli_get_value(ctx, "SHOTID", &shotidstr);
    sts = tclStringToShot(shotidstr, &shotid, error);
    if (sts & 1)
      sts = TreeClose(exp, shotid);
  } else {
    int doall = cli_present(ctx, "ALL") & 1;
    while ((sts = TreeClose(0, 0)) & 1 && doall) ;
    if (doall && sts == TreeNOT_OPEN)
      sts = TreeNORMAL;
  }
  if (sts == TreeWRITEFIRST) {
    if (cli_present(ctx, "CONFIRM") & 1)
      sts = TreeQuitTree(0, 0);
    else
      *error = strdup("This tree has been modified. Either use the WRITE\n"
		      "command before closing to save modifications or\n"
		      "use CLOSE/CONFIRM to discard changes.\n\n");
  }
  if (sts & 1)
    TclNodeTouched(0, tree);
  if (exp)
    free(exp);
  if (shotidstr)
    free(shotidstr);
  return sts;
}
