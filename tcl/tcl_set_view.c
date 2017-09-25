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
#include <mdstypes.h>
#include <libroutines.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <ncidef.h>
#include <usagedef.h>
#include <strroutines.h>
#include <libroutines.h>

/**********************************************************************
* TCL_SET_VIEW.C --
*
* TclSetView:  Set view date context.
*
* History:
*  10-AUG-2006  TWF  Create
*
************************************************************************/

	/****************************************************************
	 * TclSetView:
	 ****************************************************************/
EXPORT int TclSetView(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int status;
  int64_t viewDate = -1;
  char *viewDateStr = 0;
  cli_get_value(ctx, "DATE", &viewDateStr);
  if ((strcasecmp(viewDateStr, "NOW") == 0) ||
      ((status = LibConvertDateString(viewDateStr, &viewDate)) & 1)) {
    status = TreeSetViewDate(&viewDate);
  }
  if (!(status & 1)) {
    char *msg = MdsGetMsg(status);
    *error = malloc(strlen(msg) + 200);
    sprintf(*error, "Error: Bad time specified, use dd-mon-yyy hh:mm:ss format.\n"
	    "All fields required!\n" "Error message was: %s\n", msg);
  }
  if (viewDateStr)
    free(viewDateStr);
  return status;
}
