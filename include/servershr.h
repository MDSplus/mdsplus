#ifndef __SERVERSHR
#define __SERVERSHR

#include <config.h>
#ifdef HAVE_WINDOWS_H
typedef void *pthread_cond_t;
typedef void *pthread_t;
typedef void *pthread_mutex_t;
#else
#include <pthread.h>
#endif

#define ServerNOT_DISPATCHED  0xfe18008
#define ServerINVALID_DEPENDENCY  0xfe18012
#define ServerCANT_HAPPEN  0xfe1801a
#define ServerINVSHOT  0xfe18022
#define ServerABORT    0xfe18032
#define ServerPATH_DOWN 0xfe18042

extern int ServerAbortServer( char *server, int flush );
extern int ServerBuildDispatchTable( char *wildcard, char *monitor_name, void **table);
extern int ServerCloseTrees( char *server );
extern int ServerCreatePulse(int *id, char *server, char *tree, int shot,
                        void (*ast)(), void *astprm, int *retstatus, void (*before_ast)());
extern int ServerDispatchAction(int *id, char *server, char *tree, int shot, int nid,
                        void (*ast)(), void *astprm, int *retstatus, 
                        void (*before_ast)());
extern int ServerDispatchClose(void *vtable);
extern int ServerDispatchCommand(int *id, char *server, char *cli, char *command,
                        void (*ast)(), void *astprm, int *retstatus, void (*before_ast)());
extern int ServerSetLinkDownHandler(void (*handler)());
extern void ServerSetDetailProc(char *(*detail_proc)(int));
extern char *(*ServerGetDetailProc())(int);
extern int ServerDispatchPhase(int *id, void *vtable, char *phasenam, char noact_in,
                          int sync, void (*output_rtn)(), char *monitor);
extern int ServerFailedEssential(void *vtable,int reset);
extern char *ServerFindServers(void **ctx, char *wild_match);
extern int ServerMonitorCheckin(char *server, void (*ast)(), void *astparam);
extern int ServerSetLogging( char *server, char logging_mode );
extern int ServerStartServer( char *server );
extern int ServerStopServer( char *server );
extern void ServerWait(int id);

#ifdef CREATE_STS_TEXT
#include        "facility_list.h"

static struct stsText  servershr_stsText[] = {
    STS_TEXT(ServerNOT_DISPATCHED,"action not dispatched, depended on failed action")
   ,STS_TEXT(ServerINVALID_DEPENDENCY,"action dependency cannot be evaluated")
   ,STS_TEXT(ServerCANT_HAPPEN,"action contains circular dependency or depends on action which was not dispatched")
   ,STS_TEXT(ServerINVSHOT,"invalid shot number, cannot dispatch actions in model")
   ,STS_TEXT(ServerABORT,"Server action was aborted")
   ,STS_TEXT(ServerPATH_DOWN,"Path to server lost")
   };
#endif

#endif
