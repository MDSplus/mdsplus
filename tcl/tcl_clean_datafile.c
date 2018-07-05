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
#include <stdio.h>
#include <dcl.h>
#include <mdsshr.h>
#include <treeshr.h>
#include "tcl_p.h"

/**********************************************************************
* TCL_CLEAN_DATAFILE.C --
*
* TclCleanDatafile:  Clean a datafile (delete unused space, etc???)
*
* History:
*  14-Apr-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/

extern int TdiExecute();

	/****************************************************************
	 * TclCleanDatafile:
	 ****************************************************************/
EXPORT int TclCleanDatafile(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int sts;
  int shot;
  char *filnam = 0;
  char *asciiShot = 0;
  cli_get_value(ctx, "FILE", &filnam);
  cli_get_value(ctx, "SHOTID", &asciiShot);
  sts = tclStringToShot(asciiShot, &shot, error);
  if (sts & 1) {
    sts = TreeCleanDatafile(filnam, shot);
    if (!(sts & 1)) {
      char *msg = MdsGetMsg(sts);
      *error = malloc(strlen(msg) + strlen(filnam) + 100);
      sprintf(*error, "Error: Problem cleaning tree '%s' shot '%d'\nError message was: %s\n",
	      filnam, shot, msg);
    }
  }
  if (filnam)
    free(filnam);
  if (asciiShot)
    free(asciiShot);
  return sts;
}
