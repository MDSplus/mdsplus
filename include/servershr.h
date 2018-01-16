#ifndef __SERVERSHR
#define __SERVERSHR

#include <mdsplus/mdsconfig.h>
#ifdef _WIN32
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#else
typedef void *pthread_cond_t;
typedef void *pthread_t;
typedef void *pthread_mutex_t;
typedef void *pthread_rwlock_t;
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

EXPORT extern int ServerAbortServer(char *server, int flush);
EXPORT extern int ServerBuildDispatchTable(char *wildcard, char *monitor_name, void **table);
EXPORT extern int ServerCloseTrees(char *server);
EXPORT extern int ServerCreatePulse(int *id, char *server, char *tree, int shot,
			     void (*ast) (), void *astprm, int *retstatus, pthread_rwlock_t *lock, void (*before_ast) ());
EXPORT extern int ServerDispatchAction(int *id, char *server, char *tree, int shot, int nid,
				void (*ast) (), void *astprm, int *retstatus, pthread_rwlock_t *lock, int *socket,
				void (*before_ast) ());
EXPORT extern int ServerDispatchClose(void *vtable);
EXPORT extern int ServerDispatchCommand(int *id, char *server, char *cli, char *command,
				 void (*ast) (), void *astprm, int *retstatus, pthread_rwlock_t *lock,
				 void (*before_ast) ());
EXPORT extern int ServerSetLinkDownHandler(void (*handler) ());
EXPORT extern void ServerSetDetailProc(char *(*detail_proc) (int));
EXPORT extern char *(*ServerGetDetailProc()) (int);
EXPORT extern int ServerDispatchPhase(int *id, void *vtable, char *phasenam, char noact_in,
			       int sync, void (*output_rtn) (), char *monitor);
EXPORT extern int ServerFailedEssential(void *vtable, int reset);
EXPORT extern char *ServerFindServers(void **ctx, char *wild_match);
EXPORT extern int ServerMonitorCheckin(char *server, void (*ast) (), void *astparam);
EXPORT extern int ServerSetLogging(char *server, char logging_mode);
EXPORT extern int ServerStartServer(char *server);
EXPORT extern int ServerStopServer(char *server);
EXPORT extern void ServerWait(int id);
EXPORT extern int ServerFreeDispatchTable(void *vtable);
EXPORT extern char *ServerGetInfo(int full, char *server);
#endif
