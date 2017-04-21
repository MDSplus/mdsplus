#include <mdsshr.h>
#include <ipdesc.h>
#include <servershr.h>
#include "servershrp.h"
#include <string.h>
#include <stdlib.h>
#include <treeshr.h>

int ServerSendMonitor(char *monitor, char *tree, int shot, int phase,
		      int nid, int on, int mode, char *server, int actstatus) {
  static int initialize=1;
  static char *event=0;
  int status;
  char now[64];
  time_t tim;
  tim = time(0);
  strcpy(now, ctime(&tim));
  now[strlen(now) - 1] = 0;

  if (initialize) {
    initialize=0;
    char *mon_env=getenv(monitor);
    if (mon_env == 0) {
      mon_env=monitor;
    }
    if ((strncasecmp(mon_env,"event:",strlen("event:")) == 0) &&
	(strlen(mon_env) > strlen("event:"))) {
      event=strdup(mon_env+strlen("event:"));
    }
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
    status = MDSEvent(event,msglen+1,eventmsg);
    if (path) free(path);
  }
  else {
    struct descrip p1, p2, p3, p4, p5, p6, p7, p8;
    status = ServerSendMessage(0, monitor, SrvMonitor, 0, 0, 0, 0, 0, 8,
				  MakeDescrip(&p1, DTYPE_CSTRING, 0, 0, tree),
				  MakeDescrip(&p2, DTYPE_LONG, 0, 0, &shot),
				  MakeDescrip(&p3, DTYPE_LONG, 0, 0, &phase),
				  MakeDescrip(&p4, DTYPE_LONG, 0, 0, &nid),
				  MakeDescrip(&p5, DTYPE_LONG, 0, 0, &on),
				  MakeDescrip(&p6, DTYPE_LONG, 0, 0, &mode),
				  MakeDescrip(&p7, DTYPE_CSTRING, 0, 0, server),
				  MakeDescrip(&p8, DTYPE_LONG, 0, 0, &actstatus));
  }
  return status;
}
