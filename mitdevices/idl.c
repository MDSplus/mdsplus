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
#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <libroutines.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "idl_gen.h"

static char *GetNextCmd(char **cmds);

static int idl_initialized = 0;

static int (*execute) (char *cmd);

static void InitIdl()
{
  //int zero;
  if (!idl_initialized) {
    DESCRIPTOR(image, "IDL");
    DESCRIPTOR(execute_d, "execute");
    int status;
    status = LibFindImageSymbol(&image, &execute_d, &execute);
    if (status & 1)
      status = (*execute) ("print,'IDL Activated'") == 0;
    if (status & 1)
      idl_initialized = 1;
    else
      printf("Error activating IDL");
  }
}

EXPORT int idl___execute(struct descriptor *niddsc __attribute__ ((unused)), InExecuteStruct * setup)
{
  static char *initialize[] = { "close,/all", "!ERROR = 0", "retall" };
  int status = IDL$_ERROR;
  char *cmd;
  char *cmds;
  int i;

  if (idl_initialized == 0) {
    InitIdl();
  }

  if (idl_initialized == 0) {
    return status;
  }
  for (i = 0; i < ((int)(sizeof(initialize) / sizeof(initialize[0]))); i++)
    (*execute) (initialize[i]);

  for (cmds = setup->commands; status && (cmd = GetNextCmd(&cmds));)
    status = (*execute) (cmd) == 0;

  if (status == 0)
    status = IDL$_ERROR;
  return status;
}

static char *GetNextCmd(char **cmds)
{
  char *ptr = *cmds;
  char *ans;
  int chars = 0;
  for (; *ptr && *ptr == '\n'; ptr++, chars++) ;
  if (*ptr) {
    ans = ptr;
    for (; *ptr && *ptr != '\n'; ptr++, chars++) ;
    if (*ptr == '\n')
      chars++;
    *ptr = 0;
  } else
    ans = 0;
  *cmds += chars;
  return ans;
}
