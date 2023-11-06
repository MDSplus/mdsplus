/*
Copyright (c) 2023, Massachusetts Institute of Technology All rights reserved.

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
#include <ctype.h>

#include <dbidef.h>
#include <dcl.h>
#include <mds_stdarg.h>
#include <mdsdcl_messages.h>
#include <mdsshr.h>
#include <treeshr.h>

/***************************************************************
 * TclSetAlternateCompression:
 **************************************************************/

EXPORT int TclSetAlternateCompression(void *ctx, char **error,
                          char **output __attribute__((unused)))
{
  int status = 1;

  char * enabled = 0;
  status = cli_get_value(ctx, "ENABLED", &enabled);
  if (STATUS_OK) {
    char * pch = enabled;
    while (pch && (*pch) != '\0') {
      (*pch) = toupper(*pch);
      ++pch;
    }

    if (strcmp(enabled, "ON") == 0) {
      status = TreeSetDbiItm(DbiALTERNATE_COMPRESSION, 1);
    }
    else if (strcmp(enabled, "OFF") == 0) {
      status = TreeSetDbiItm(DbiALTERNATE_COMPRESSION, 0);
    }
    else {
      status = MdsdclIVQUAL;
    }
  }

  if (STATUS_NOT_OK)
  {
    char *msg = MdsGetMsg(status);
    *error = malloc(strlen(msg) + 100);
    sprintf(*error,
            "Error: problem setting alternate compression.\n"
            "Error message was: %s\n",
            msg);
  }
  return status;
}

EXPORT int TclShowAlternateCompression(void *ctx __attribute__((unused)),
                           char **error __attribute__((unused)),
                           char **output)
{
  int enable, status;
  DBI_ITM itmlst[] = {{4, DbiALTERNATE_COMPRESSION, &enable, 0},
                      {0, 0, 0, 0}};
  status = TreeGetDbi(itmlst);
  if (STATUS_OK)
  {
    *output = malloc(500);
    sprintf(*output,
            "  Alternate Compression is %s.\n",
            enable ? "enabled" : "disabled");
  }
  else
  {
    char *msg = MdsGetMsg(status);
    *error = malloc(strlen(msg) + 100);
    sprintf(*error,
            "Error: problem querying alternate compression.\n"
            "Error message was: %s\n",
            msg);
  }
  return status;
}
