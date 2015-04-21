#ifndef __SERVERSHR
#define __SERVERSHR

#include <config.h>
#ifdef _WIN32
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#else
typedef void *pthread_cond_t;
typedef void *pthread_t;
typedef void *pthread_mutex_t;
#endif
#else
#define _GNU_SOURCE		/* glibc2 needs this */
#if defined(__sparc__)
#include "/usr/include/sys/types.h"
#else
#include <sys/types.h>
#endif
#include <pthread.h>
#endif

#include <servershr_messages.h>

extern int ServerAbortServer(char *server, int flush);
extern int ServerBuildDispatchTable(char *wildcard, char *monitor_name, void **table);
extern int ServerCloseTrees(char *server);
extern int ServerCreatePulse(int *id, char *server, char *tree, int shot,
			     void (*ast) (), void *astprm, int *retstatus, void (*before_ast) ());
extern int ServerDispatchAction(int *id, char *server, char *tree, int shot, int nid,
				void (*ast) (), void *astprm, int *retstatus, int *socket,
				void (*before_ast) ());
extern int ServerDispatchClose(void *vtable);
extern int ServerDispatchCommand(int *id, char *server, char *cli, char *command,
				 void (*ast) (), void *astprm, int *retstatus,
				 void (*before_ast) ());
extern int ServerSetLinkDownHandler(void (*handler) ());
extern void ServerSetDetailProc(char *(*detail_proc) (int));
extern char *(*ServerGetDetailProc()) (int);
extern int ServerDispatchPhase(int *id, void *vtable, char *phasenam, char noact_in,
			       int sync, void (*output_rtn) (), char *monitor);
extern int ServerFailedEssential(void *vtable, int reset);
extern char *ServerFindServers(void **ctx, char *wild_match);
extern int ServerMonitorCheckin(char *server, void (*ast) (), void *astparam);
extern int ServerSetLogging(char *server, char logging_mode);
extern int ServerStartServer(char *server);
extern int ServerStopServer(char *server);
extern void ServerWait(int id);

#endif
