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
#include <mdsshr.h>
#include <ipdesc.h>
#include <servershr.h>
#include "servershrp.h"
#include <string.h>
#include <stdlib.h>
#include <treeshr.h>

int ServerSendMonitor(char *monitor, char *tree, int shot, int phase,
		      int nid, int on, int mode, char *server, int actstatus) {
  static int initialized=B_FALSE;
  static char *event = NULL;
  char now[64];
  time_t tim;
  tim = time(0);
  strcpy(now, ctime(&tim));
  now[strlen(now) - 1] = 0;
  const char*        event_str = "event:";
  const unsigned int event_len = strlen(event_str);
  if (!initialized) {
    initialized = B_TRUE;
    char *mon_env = getenv(monitor);
    if (!mon_env)
      mon_env = monitor;
    if ((strlen(mon_env) > event_len)
     && (strncasecmp(mon_env,event_str,event_len) == 0))
      event=strdup(mon_env+event_len);
  }
  if (event) {
    char *path=TreeGetPath(nid);
    char *status_text = MdsGetMsg(actstatus);
    char *eventmsg=alloca(1024);
    int msglen = snprintf(eventmsg,1023,"%s %d %d %d %d %d %s %d %s %s; %s", tree,
			  shot, phase, nid, on, mode,
			  (server && *server) ? server : "unknown",
			  actstatus, path ? path : "unknown", now, status_text);
    eventmsg[msglen]=0;
    int status = MDSEvent(event,msglen+1,eventmsg);
    if (path) free(path);
    return status;
  }//else
  struct descrip p1, p2, p3, p4, p5, p6, p7, p8;
  return ServerSendMessage(0, monitor, SrvMonitor, NULL, NULL, NULL, NULL, NULL, NULL, 8,
			  MakeDescrip(&p1, DTYPE_CSTRING, 0, 0, tree),
			  MakeDescrip(&p2, DTYPE_LONG, 0, 0, &shot),
			  MakeDescrip(&p3, DTYPE_LONG, 0, 0, &phase),
			  MakeDescrip(&p4, DTYPE_LONG, 0, 0, &nid),
			  MakeDescrip(&p5, DTYPE_LONG, 0, 0, &on),
			  MakeDescrip(&p6, DTYPE_LONG, 0, 0, &mode),
			  MakeDescrip(&p7, DTYPE_CSTRING, 0, 0, server),
			  MakeDescrip(&p8, DTYPE_LONG, 0, 0, &actstatus));
}
