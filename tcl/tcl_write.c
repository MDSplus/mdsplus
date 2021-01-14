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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dcl.h>
#include <mdsshr.h>
#include <treeshr.h>

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
EXPORT int TclWrite(void *ctx, char **error __attribute__((unused)),
                    char **output) {
  int sts;
  char *exp = 0;
  if (cli_get_value(ctx, "FILE", &exp) & 1) {
    char *shotidStr = 0;
    int shotid;
    cli_get_value(ctx, "SHOTID", &shotidStr);
    shotid = strtol(shotidStr, NULL, 0);
    sts = TreeWriteTree(exp, shotid);
    free(exp);
    free(shotidStr);
  } else {
    sts = TreeWriteTree(0, 0);
  }

  if (~sts & 1) {
    char *msg = MdsGetMsg(sts);
    *output = malloc(strlen(msg) + 100);
    sprintf(*output,
            "Error: Unable to write tree\n"
            "Error message was: %s\n",
            msg);
  }
  return sts;
}
