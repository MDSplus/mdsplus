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


/***********************************************************************
* TCL_SET_CURRENT.C --
*
* TclSetCurrent:  set current Experiment and Shot
*
* History:
*  15-Jan-1998  TRG  Created.  Ported from original code.
*
************************************************************************/


extern int TdiExecute();

	/*****************************************************************
	 * TclSetCurrent:
	 *****************************************************************/
EXPORT int TclSetCurrent(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int sts;
  char *experiment = 0;
  char *shotasc = 0;
  int shot;

  cli_get_value(ctx, "EXPERIMENT", &experiment);
  if (cli_present(ctx, "INCREMENT") & 1) {
    shot = TreeGetCurrentShotId(experiment);
    shot++;
    sts = TreeSetCurrentShotId(experiment, shot);
  } else {
    cli_get_value(ctx, "SHOT", &shotasc);
    sts = tclStringToShot(shotasc,&shot, error);
    if (shotasc)
      free(shotasc);
    sts = TreeSetCurrentShotId(experiment, shot);
  }

  if (((sts & 1) != 1) && (*error == NULL)) {
    char *msg = MdsGetMsg(sts);
    *error = malloc(strlen(msg)+100);
    sprintf(*error, "Error: Unable to change current shot\n"
	    "Error message was: %s\n",msg);
  }
  if (experiment)
    free(experiment);
  return sts;
}
