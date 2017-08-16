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
