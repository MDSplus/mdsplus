#ifndef SERVERSHRP_H
#define SERVERSHRP_H
#include <mdsplus/mdsconfig.h>
#include <mdsdescrip.h>
#ifdef _WIN32
#include <time.h>
#else
#ifndef HAVE_PTHREAD_LOCK_GLOBAL_NP
extern void pthread_lock_global_np();
extern void pthread_unlock_global_np();
#endif
#endif

#ifdef MDSOBJECTSCPPSHRVS_EXPORTS
// visual studio uses int types for typedef
#define TYPEDEF(bytes) enum
#define ENDDEF(type, name) \
  ;                        \
  typedef type name
#else
#define TYPEDEF(bytes) typedef enum __attribute__((__packed__))
#define ENDDEF(type, name) name
#endif

TYPEDEF(4){
    SrvNoop = 0,   /**** Used to start server ****/
    SrvAbort = 1,  /**** Abort current action or mdsdcl command ***/
    SrvAction = 2, /**** Execute an action nid in a tree ***/
    SrvClose = 3,  /**** Close open trees ***/
    SrvCreatePulse =
        4,             /**** Create pulse files for single tree (no subtrees) ***/
    SrvSetLogging = 5, /**** Turn logging on/off ***/
    SrvCommand = 6,    /**** Execute MDSDCL command ***/
    SrvMonitor = 7,    /**** Broadcast messages to action monitors ***/
    SrvShow = 8,       /**** Request current status of server ***/
    SrvStop = 9,       /**** Stop server ***/
    SrvRemoveLast =
        10, /**** Remove last entry in the queue if jobs pending ***/
} ENDDEF(int, srv_t);

TYPEDEF(4){
    MonitorBuildBegin = 1,
    MonitorBuild = 2,
    MonitorBuildEnd = 3,
    MonitorCheckin = 4,
    MonitorDispatched = 5,
    MonitorDoing = 6,
    MonitorDone = 7,
} ENDDEF(int, monitor_t);

TYPEDEF(4){
    SrvJobABORTED = 1,
    SrvJobSTARTING = 2,
    SrvJobFINISHED = 3,
    SrvJobCHECKEDIN = 4,
} ENDDEF(int, srvjob_t);

TYPEDEF(4){
    SrvJobBEFORE_NOTIFY = 1,
    SrvJobAFTER_NOTIFY = 2,
} ENDDEF(int, srvjobnotify_t);
#undef TYPEDEF
#undef ENDDEF

typedef struct
{
  struct _SrvJob *next;
  struct _SrvJob *previous;
  unsigned int addr;
  unsigned int port;
  int sock;
  int length;
  int op;
  int flags;
  int jobid;
} JHeader;
#define JHEADER_PRI "(op=%d, jobid=%d, addr=" IPADDRPRI ", port=%d)"
#define JHEADER_VAR(h) (h)->op, (h)->jobid, IPADDRVAR(&(h)->addr), (h)->port

typedef struct _SrvJob
{
  JHeader h;
} SrvJob;
#define SVRJOB_PRI "SvrJob" JHEADER_PRI
#define SVRJOB_VAR(j) JHEADER_VAR(&(j)->h)

typedef struct
{
  JHeader h;
  char *tree;
  int shot;
  int nid;
} SrvActionJob;
#define SVRACTIONJOB_PRI "SrvActionJob(" JHEADER_PRI ", tree='%s', shot=%d, nid=%d)"
#define SVRACTIONJOB_VAR(j) JHEADER_VAR(&(j)->h), (j)->tree, (j)->shot, (j)->nid

typedef struct
{
  JHeader h;
} SrvCloseJob;
#define SVRCLOSEJOB_PRI "SrvCloseJob(" JHEADER_PRI ")"
#define SVRCLOSEJOB_VAR(j) JHEADER_VAR(&(j)->h)

typedef struct
{
  JHeader h;
  char *tree;
  int shot;
} SrvCreatePulseJob;
#define SVRCREATEPULSEJOB_PRI "SrvCreatePulseJob(" JHEADER_PRI ", tree='%s', shot=%d)"
#define SVRCREATEPULSEJOB_VAR(j) JHEADER_VAR(&(j)->h), (j)->tree, (j)->shot

typedef struct
{
  JHeader h;
  char *table;
  char *command;
} SrvCommandJob;
#define SVRCOMMANDJOB_PRI "SrvCommandJob(" JHEADER_PRI ", table='%s', command='%s')"
#define SVRCOMMANDJOB_VAR(j) JHEADER_VAR(&(j)->h), (j)->table, (j)->command

typedef struct
{
  JHeader h;
  char *tree;
  int shot;
  int phase;
  int nid;
  int on;
  int mode;
  char *server;
  int status;
} SrvMonitorJob;
#define SVRMONITORJOB_PRI "SrvMonitorJob(" JHEADER_PRI ", tree='%s', shot=%d, phase=%d, nid=%d, on=%d, mode=%d, server='%s', status=%d)"
#define SVRMONITORJOB_VAR(j) JHEADER_VAR(&(j)->h), (j)->tree, (j)->shot, (j)->phase, (j)->nid, (j)->on, (j)->mode, (j)->server, (j)->status

typedef struct
{
  char server[32];
  int nid;
  int phase;
  int sequence;
  int num_references;
  int *referenced_by;
  struct descriptor *condition;
  int status;
  int netid;
  unsigned on : 1;
  unsigned done : 1;
  unsigned closed : 1;
  unsigned dispatched : 1;
  unsigned doing : 1;
  unsigned recorded : 1;
  char *path;
  char *event;
  pthread_rwlock_t lock;
} ActionInfo;

typedef struct
{
  int num;
  char tree[13];
  int shot;
  int failed_essential;
  ActionInfo actions[1];
} DispatchTable;

typedef struct
{
  int shot;
  union {
    int nid;
    int phase;
  } u;
  int status;
} DispatchEvent;

#ifndef _NO_SERVER_SEND_MESSAGE_PROTO
extern int ServerSendMessage(int *msgid, char *server, int op, int *retstatus,
                             pthread_rwlock_t *lock, int *socket, void (*ast)(),
                             void *astparam, void (*before_ast)(),
                             int numargs_in, ...);
#endif
extern int ServerConnect(char *);
extern int ServerSendMonitor(char *monitor, char *tree, int shot, int phase,
                             int nid, int on, int mode, char *server,
                             int actstatus);
#endif
