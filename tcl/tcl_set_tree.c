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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <dcl.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <treeshr_messages.h>
#include <mdsdcl_messages.h>

#include "tcl_p.h"

extern int TdiExecute();

int tclStringToShot(char *str, int *shot_out, char **error)
{
  int shot = -2;
  int status = TreeINVSHOT;
  char *endptr;
  if (str && strlen(str) > 0) {
    status = 1;
    /* First try just treating arg as an integer string */
    shot = strtol(str, &endptr, 0);
    if (*endptr != 0) {
      /* Next see if the string "model" was used. */
      if (strcasecmp(str, "model") == 0)
	shot = -1;
      else {
	/* Next see if TDI can make sense of the string */
	DESCRIPTOR_LONG(dsc_shot, &shot);
	struct descriptor str_d = { strlen(str), DTYPE_T, CLASS_S, str };
	status = TdiExecute(&str_d, &dsc_shot MDS_END_ARG);
	if (!(status & 1)) {
	  *error = malloc(strlen(str) + 100);
	  sprintf(*error, "Error: Could not convert shot specified '%s' to a valid shot number.\n",
		  str);
	}
      }
    }
    if ((status & 1) && (shot < -1)) {
      *error = malloc(100);
      sprintf(*error, "Error: Invalid shot number specified - %d\n", shot);
      status = TreeINVSHOT;
    }
  } else
    *error = strdup("Error: Zero length shot string specified\n");
  if (status & 1)
    *shot_out = shot;
  return status;
}

	/***************************************************************
	 * TclSetTree:
	 **************************************************************/
EXPORT int TclSetTree(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int sts = MdsdclIVVERB;
  int shot;
  char *filnam = 0;
  char *asciiShot = 0;

		/*--------------------------------------------------------
		 * Executable ...
		 *-------------------------------------------------------*/
  cli_get_value(ctx, "FILE", &filnam);
  cli_get_value(ctx, "SHOTID", &asciiShot);
  sts = tclStringToShot(asciiShot, &shot, error);
  if (sts & 1) {
    if (cli_present(ctx, "READONLY") & 1)
      sts = TreeOpen(filnam, shot, 1);
    else
      sts = TreeOpen(filnam, shot, 0);
    if (sts & 1)
      TclNodeTouched(0, tree);
    else {
      char *msg = MdsGetMsg(sts);
      *error = malloc(strlen(filnam) + strlen(msg) + 100);
      sprintf(*error, "Error: Failed to open tree '%s' shot %d\n"
	      "Error message was: %s\n", filnam, shot, msg);
    }
  }
  if (filnam)
    free(filnam);
  if (asciiShot)
    free(asciiShot);
  return sts;
}
