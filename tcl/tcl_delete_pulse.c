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
#include <mdsshr.h>
#include <treeshr.h>
#include <dcl.h>

/**********************************************************************
* TCL_DELETE_PULSE.C --
*
* TclDeletePulse:  Delete a whole pulse
*
* History:
*  27-Apr-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/

EXPORT int TclDeletePulse(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  char *asciiShot = 0;
  int shot;
  int sts;
  int delete_all = cli_present(ctx, "ALL") & 1;

  cli_get_value(ctx, "SHOT", &asciiShot);
  sscanf(asciiShot, "%d", &shot);
  free(asciiShot);
  sts = TreeDeletePulseFile(shot, delete_all);
  if (~sts & 1) {
    char *msg = MdsGetMsg(sts);
    *error = malloc(strlen(msg) + 100);
    sprintf(*error, "Error: Could not delete pulse file %d\n" "Error message was: %s\n", shot, msg);
  }
  return sts;
}
