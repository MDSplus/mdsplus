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

#include <mds_stdarg.h>
#include <dbidef.h>
#include <dcl.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <mdsdcl_messages.h>

/***************************************************************
 * TclSetVersions:
 **************************************************************/

EXPORT int TclSetVersions(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int status = 1;

  /*--------------------------------------------------------
   * Executable ...
   *-------------------------------------------------------*/
  switch (cli_present(ctx, "MODEL")) {
  case MdsdclPRESENT:
    status = TreeSetDbiItm(DbiVERSIONS_IN_MODEL, 1);
    break;
  case MdsdclNEGATED:
    status = TreeSetDbiItm(DbiVERSIONS_IN_MODEL, 0);
    break;
  }
  if (!(status & 1))
    goto error;
  switch (cli_present(ctx, "SHOT")) {
  case MdsdclPRESENT:
    status = TreeSetDbiItm(DbiVERSIONS_IN_PULSE, 1);
    break;
  case MdsdclNEGATED:
    status = TreeSetDbiItm(DbiVERSIONS_IN_PULSE, 0);
    break;
  }
 error:
  if (!(status & 1)) {
    char *msg = MdsGetMsg(status);
    *error = malloc(strlen(msg)+100);
    sprintf(*error,"Error: problem setting versions\n"
	    "Error message was: %s\n",msg);
  }
  return status;
}

EXPORT int TclShowVersions(void *ctx __attribute__ ((unused)), char **error __attribute__ ((unused)), char **output)
{
  int in_model, in_pulse, status;
  DBI_ITM itmlst[] =
      { {4, DbiVERSIONS_IN_MODEL, &in_model, 0}, {4, DbiVERSIONS_IN_PULSE, &in_pulse, 0}, {0, 0, 0,
											   0}
  };
  status = TreeGetDbi(itmlst);
  if (status & 1) {
    *output = malloc(500);
    sprintf(*output, "  Versions are %s in the model file and %s in the shot file.\n",
	    in_model ? "enabled" : "disabled", in_pulse ? "enabled" : "disabled");
  } else {
    char *msg = MdsGetMsg(status);
    *error = malloc(strlen(msg) + 100);
    sprintf(*error, "Error: problem obtaining versions.\n" "Error message was: %s\n", msg);
  }
  return status;
}
