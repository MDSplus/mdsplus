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

/**********************************************************************
* TCL_SHOW_CURRENT.C --
*
* TclShowCurrent:  Display current shotid.
*
* History:
*  27-Apr-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/

	/***************************************************************
	 * TclShowCurrent:
	 ***************************************************************/
EXPORT int TclShowCurrent(void *ctx, char **error, char **output)
{
  int shot;
  char *experiment = 0;

  cli_get_value(ctx, "EXPERIMENT", &experiment);
  shot = TreeGetCurrentShotId(experiment);
  if (shot) {
    *output = malloc(100);
    sprintf(*output, "Current shot is %d\n", shot);
  } else {
    *error = strdup("Failed to get shotid.\n");
  }
  if (experiment)
    free(experiment);
  return 1;
}
