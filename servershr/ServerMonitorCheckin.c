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
/*------------------------------------------------------------------------------

                Name:   SERVER$MONITOR_CHECKIN

                Type:   C function

                Author:	TOM FREDIAN

                Date:   23-APR-1992

                Purpose: Checkin routine for action monitor

------------------------------------------------------------------------------

        Call sequence:

int SERVER$MONITOR_CHECKIN(struct dsc$descriptor *server, void (*ast)(), int
astparam, void (*link_down)())

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

        Description:

------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread_port.h>
#include <mdsshr.h>
#include <ipdesc.h>
#include <servershr.h>
#include "servershrp.h"

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static void (*appAst)() = 0;
static int usingEvents = -1;

static void event_ast(void *astprm, int msglen __attribute__((unused)), char *msg)
{
  void (*ast)();
  pthread_mutex_lock(&lock);
  ast = appAst;
  pthread_mutex_unlock(&lock);
  (*ast)(astprm, msg);
}

#define EVENT_PREFIX "event:"
#define EVENT_PREFIX_LEN (sizeof(EVENT_PREFIX) - 1)
static inline int using_events(char *server, void (*ast)(), void *astprm)
{
  int yesno;
  pthread_mutex_lock(&lock);
  if (usingEvents == -1)
  {
    char *svr_env = getenv(server);
    if (!svr_env)
      svr_env = server;
    if ((strlen(svr_env) > EVENT_PREFIX_LEN) &&
        (strncasecmp(svr_env, EVENT_PREFIX, EVENT_PREFIX_LEN) == 0))
    {
      int evid;
      appAst = ast;
      usingEvents = IS_OK(MDSEventAst(svr_env + EVENT_PREFIX_LEN, event_ast, astprm, &evid));
    }
    else
      usingEvents = B_FALSE;
  }
  yesno = usingEvents;
  pthread_mutex_unlock(&lock);
  return yesno;
}

EXPORT int ServerMonitorCheckin(char *server, void (*ast)(), void *astprm)
{
  if (using_events(server, ast, astprm))
    return MDSplusSUCCESS;
  struct descrip p1, p2, p3, p4, p5, p6, p7, p8;
  char *cstring = "";
  int zero = 0;
  int mode = MonitorCheckin;
  return ServerSendMessage(0, server, SrvMonitor, NULL, NULL, NULL, ast,
                           astprm, NULL, 8,
                           MakeDescrip(&p1, DTYPE_CSTRING, 0, 0, cstring),
                           MakeDescrip(&p2, DTYPE_LONG, 0, 0, &zero),
                           MakeDescrip(&p3, DTYPE_LONG, 0, 0, &zero),
                           MakeDescrip(&p4, DTYPE_LONG, 0, 0, &zero),
                           MakeDescrip(&p5, DTYPE_LONG, 0, 0, &zero),
                           MakeDescrip(&p6, DTYPE_LONG, 0, 0, &mode),
                           MakeDescrip(&p7, DTYPE_CSTRING, 0, 0, cstring),
                           MakeDescrip(&p8, DTYPE_LONG, 0, 0, &zero));
}
