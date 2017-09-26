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
#include <mdsdcl_messages.h>

/**********************************************************************
* TCL_WFEVENT.C --
*
* Wait for an event
*
* History:
*  06-Sep-2000  TWF  Create.  Ported from original mdsPlus code.
*
***********************************************************************/

	/***************************************************************
	 * TclWfevent
	 * Wait for MDSplus event
	 ***************************************************************/

EXPORT int TclWfevent(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  char *event = 0;
  char *timeout = 0;
  int seconds = 0;
  int status;
  cli_get_value(ctx, "EVENT", &event);
  cli_get_value(ctx, "TIMEOUT", &timeout);
  seconds = atoi(timeout);
  free(timeout);
  if (event) {
    if (seconds > 0) {
      status = MDSWfeventTimed(event, 0, 0, 0,seconds);
      if (!(status & 1))
	*error = strdup("Timeout\n");
    } else {
      status = MDSWfevent(event, 0, 0, 0);
    }
    free(event);
  }
  return 1;
}
