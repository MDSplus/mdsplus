#ifndef __SERVERSHR
#define __SERVERSHR

#include <pthread.h>

#define ServerNOT_DISPATCHED  0xfe18008
#define ServerINVALID_DEPENDENCY  0xfe18012
#define ServerCANT_HAPPEN  0xfe1801a
#define ServerINVSHOT  0xfe18022
#define ServerNOREPLY  0xfe1802b
#define ServerABORT    0xfe18032
#define ServerPATH_DOWN 0xfe18042
#define ServerBEFORE    0xfe18001

#ifdef CREATE_STS_TEXT
#include        "facility_list.h"

static struct stsText  servershr_stsText[] = {
    STS_TEXT(ServerNOT_DISPATCHED,"action not dispatched, depended on failed action")
   ,STS_TEXT(ServerINVALID_DEPENDENCY,"action dependency cannot be evaluated")
   ,STS_TEXT(ServerCANT_HAPPEN,"action contains circular dependency or depends on action which was not dispatched")
   ,STS_TEXT(ServerINVSHOT,"invalid shot number, cannot dispatch actions in model")
   ,STS_TEXT(ServerNOREPLY,"operation in progress, do not reply to request yet")
   ,STS_TEXT(ServerABORT,"operation was aborted")
   ,STS_TEXT(ServerPATH_DOWN,"connection to server broken or remote server is down")
   ,STS_TEXT(ServerBEFORE,"server is about to perform action")
   };

#endif

extern int ServerAbortServer( char *server, int *flush );
extern int ServerBuildDispatchTable( char *wildcard, char *monitor_name, void **table);
extern int ServerCloseTrees( char *server );
extern int ServerCreatePulse(pthread_cond_t *efn, char *server, char *tree, int shot,
                        void (*ast)(), void *astprm, int *retstatus, void (*before_ast)());
extern int ServerDispatchAction(pthread_cond_t *efn, char *server, char *tree, int shot, int nid,
                        void (*ast)(), void *astprm, int *retstatus, 
                        void (*before_ast)());
extern int ServerDispatchClose(void *vtable);
extern int ServerDispatchCommand(pthread_cond_t *efn, char *server, char *cli, char *command,
                        void (*ast)(), void *astprm, int *retstatus, void (*before_ast)());
extern int ServerSetLinkDownHandler(void (*handler)());
extern void ServerSetDetailProc(char *(*detail_proc)(int));
extern char *(*ServerGetDetailProc())();
extern int ServerDispatchPhase(pthread_cond_t *efn, void *vtable, char *phasenam, char noact_in,
                          int sync, void (*output_rtn)(), char *monitor);
extern int ServerFailedEssential(void *vtable,int reset);
extern char *ServerFindServers(void **ctx, char *wild_match);
extern int ServerMonitorCheckin(char *server, void (*ast)(), void *astparam, void (*link_down)());
extern int ServerSetLogging( char *server, int logging_mode );
extern int ServerStartServer( char *server );
extern int ServerStopServer( char *server );

#endif

