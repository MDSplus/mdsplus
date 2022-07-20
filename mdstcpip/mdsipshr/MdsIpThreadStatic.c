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
#include <mdsplus/mdsconfig.h>

#include <stdlib.h>
#include <string.h>

#include <libroutines.h>
#include <mdsshr.h>
#include <status.h>
#include <strroutines.h>
#include <socket_port.h>

#include <_mdsversion.h>
#include "mdsipthreadstatic.h"

#define DEBUG
#include <mdsmsg.h>

static void buffer_free(MDSIPTHREADSTATIC_ARG)
{
  Connection *c;
  while ((c = MDSIP_CONNECTIONS))
  {
    MDSIP_CONNECTIONS = c->next;
    MDSDBG(CON_PRI, CON_VAR(c));
    destroyConnection(c);
  }
  free(MDSIPTHREADSTATIC_VAR);
}
static inline MDSIPTHREADSTATIC_TYPE *buffer_alloc()
{
  MDSIPTHREADSTATIC_ARG =
      (MDSIPTHREADSTATIC_TYPE *)calloc(1, sizeof(MDSIPTHREADSTATIC_TYPE));

  return MDSIPTHREADSTATIC_VAR;
}

IMPLEMENT_GETTHREADSTATIC(MDSIPTHREADSTATIC_TYPE, MdsIpGetThreadStatic,
                          THREADSTATIC_MDSIP, buffer_alloc, buffer_free)
