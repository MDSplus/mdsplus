#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <strroutines.h>
#include <treeshr.h>
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#else
typedef int SOCKET;
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#endif
#include <servershr.h>
#include "servershrp.h"

#if (defined(_DECTHREADS_) && (_DECTHREADS_ != 1)) || !defined(_DECTHREADS_)
#define pthread_attr_default NULL
#define pthread_mutexattr_default NULL
#define pthread_condattr_default NULL
#else
#undef select
#endif

typedef struct _MonitorList { int addr;
                              short port;
                              struct _MonitorList *next;
                            } MonitorList;

typedef struct _ClientList { int addr;
                             short port;
                             int sock;
                             struct _ClientList *next;
                            } ClientList;

extern int TdiDoTask();
extern int TdiGetNci();
extern int TdiExecute();
extern int mdsdcl_do_command(char *command);
extern int ServerBadSocket(int sock);

static int SendReply(SrvJob *job, int replyType, int status, int length, char *msg);
static int ShowCurrentJob(struct descriptor_xd *ans);
static void ReleaseCurrentJob();
static int StartThread();
static void WaitForJob();
static void LockQueue();
static void UnlockQueue();
static void KillThread();
static void DoSrvCommand(SrvJob *job_in);
static void DoSrvAction(SrvJob *job_in);
static void DoSrvClose(SrvJob *job_in);
static void DoSrvCreatePulse(SrvJob *job_in);
static void DoSrvMonitor(SrvJob *job_in);
static void AbortJob(SrvJob *job);
static SrvJob *NextJob(int wait);
static int QJob(SrvJob *job);
static void FreeJob(SrvJob *job);
static void SetCurrentJob(SrvJob *job);
static SrvJob *GetCurrentJob();
static char *Now();
static void RemoveClient(SrvJob *job);
extern int MdsGetClientAddr();
extern char *MdsGetServerPortname();

static int     Logging = 1;
static SrvJob *LastQueueEntry;
static SrvJob *FirstQueueEntry;
static SrvJob *CurrentJob;
static MonitorList *Monitors;
static ClientList *Clients = 0;
static int WorkerThreadRunning = 0;
static pthread_t WorkerThread;
static pthread_cond_t JobWaitCondition;
static pthread_mutex_t JobWaitMutex;
static char *current_job_text = 0;
static int Debug = 1;
static int QueueLocked = 0;
int ProgLoc = 0;
static int WorkerDied = 0;
static int LeftWorkerLoop = 0;
static int CondWStat = 0;   


struct descriptor *ServerInfo()
{
  static DESCRIPTOR(nothing,"");
  static EMPTYXD(xd);
  int status = ShowCurrentJob(&xd);
  if (status & 1 && xd.pointer)
    return xd.pointer;
  else
    return (struct descriptor *)&nothing;
}

int ServerDebug(int setting)
{
  int old = Debug;
  Debug = setting;
  return old;
}

int ServerQAction(int *addr, short *port, int *op, int *flags, int *jobid, 
                  void *p1, void *p2, void *p3, void *p4, void *p5, void *p6, void *p7, void *p8)
{
  int status;
  switch (*op)
  {
  case SrvAbort:
    {
      ServerSetDetailProc(0);
      KillThread();
      AbortJob(GetCurrentJob());
      ReleaseCurrentJob();
      SetCurrentJob(0);
      if (*(int *)p1)
      {
        SrvJob *job;
        while ((job = NextJob(0)) != 0)
          AbortJob(job);
      }
      status = StartThread();
      break;
    }
  case SrvAction:
    {
      SrvActionJob job;
      job.h.addr  = MdsGetClientAddr();
      job.h.port  = *port;
      job.h.op    = *op;
      job.h.length = sizeof(job);
      job.h.flags = *flags;
      job.h.jobid = *jobid;
      job.tree = strcpy(malloc(strlen((char *)p1)+1),(char *)p1);
      job.shot = *(int *)p2;
      job.nid = *(int *)p3;
      status = QJob((SrvJob *)&job);
      break;
    }
  case SrvClose:
    {
      SrvCloseJob job;
      job.h.addr  = MdsGetClientAddr();
      job.h.port  = *port;
      job.h.op    = *op;
      job.h.length = sizeof(job);
      job.h.flags = *flags;
      job.h.jobid = *jobid;
      status = QJob((SrvJob *)&job);
      break;
    }
  case SrvCreatePulse:
    {
      SrvCreatePulseJob job;
      job.h.addr  = MdsGetClientAddr();
      job.h.port  = *port;
      job.h.op    = *op;
      job.h.length = sizeof(job);
      job.h.flags = *flags;
      job.h.jobid = *jobid;
      job.tree = strcpy(malloc(strlen((char *)p1)+1),(char *)p1);
      job.shot = *(int *)p2;
      status = QJob((SrvJob *)&job);
      break;
    }
  case SrvSetLogging:
    {
      Logging = *(int *)p1;
      break;
    }
  case SrvCommand:
    {
      SrvCommandJob job;
      job.h.addr  = MdsGetClientAddr();
      job.h.port  = *port;
      job.h.op    = *op;
      job.h.length = sizeof(job);
      job.h.flags = *flags;
      job.h.jobid = *jobid;
      job.table = strcpy(malloc(strlen((char *)p1)+1),(char *)p1);
      job.command = strcpy(malloc(strlen((char *)p2)+1),(char *)p2);
      status = QJob((SrvJob *)&job);
      break;
    }
  case SrvMonitor:
    {
      SrvMonitorJob job;
      job.h.addr  = MdsGetClientAddr();
      job.h.port  = *port;
      job.h.op    = *op;
      job.h.length = sizeof(job);
      job.h.flags = *flags;
      job.h.jobid = *jobid;
      job.tree = strcpy(malloc(strlen((char *)p1)+1),(char *)p1);
      job.shot = *(int *)p2;
      job.phase  = *(int *)p3;
      job.nid  = *(int *)p4;
      job.on   = *(int *)p5;
      job.mode = *(int *)p6;
      job.server = strcpy(malloc(strlen((char *)p1)+1),(char *)p7);
      job.status = *(int *)p8; 
      status = QJob((SrvJob *)&job);
      break;
    }
  case SrvShow:
    {
      status = ShowCurrentJob((struct descriptor_xd *)p1);
      break;
    }
  case SrvStop:
    {
      printf("%s, Server stop requested\n",Now());
      exit(0);
      break;
    }
  }
  return status;
}
   
static int QJob(SrvJob *job)
{
  SrvJob *qjob = (SrvJob *)memcpy(malloc(job->h.length),job,job->h.length);
  LockQueue();
  if (LastQueueEntry != 0)
    LastQueueEntry->h.next = qjob;
  qjob->h.next = 0;
  qjob->h.previous = LastQueueEntry;
  LastQueueEntry = qjob;
  if (FirstQueueEntry == 0)
    FirstQueueEntry = qjob;
  UnlockQueue();
  return StartThread();
}

static SrvJob *NextJob(int wait)
{
  SrvJob *job;
  int done = 0;
  while (!done)
  {
    LockQueue();
    job = FirstQueueEntry;
    if (job)
    {
      FirstQueueEntry = job->h.next;
      if (FirstQueueEntry != 0)
        FirstQueueEntry->h.previous = 0;
      else
        LastQueueEntry = 0;
    }
    UnlockQueue();
    if (job || (!wait))
      done = 1;
    else
      WaitForJob();
  }
  return job;
}
      
static void AbortJob(SrvJob *job)
{
  if (job)
  {
    SendReply(job,SrvJobFINISHED,ServerABORT,0,0);
    FreeJob(job);
  }
}

static void FreeJob(SrvJob *job)
{
  switch (job->h.op)
  {
  case SrvAction:
    free(((SrvActionJob *)job)->tree);
    break;
  case SrvCreatePulse:
    free(((SrvCreatePulseJob *)job)->tree);
    break;
  case SrvCommand:
    free(((SrvCommandJob *)job)->table);
    free(((SrvCommandJob *)job)->command);
    break;
  case SrvMonitor:
    free(((SrvMonitorJob *)job)->tree);
    free(((SrvMonitorJob *)job)->server);
  }
  free(job);
}

static void ThreadCleanup(void *arg)
{
  WorkerThreadRunning = 0;
  WorkerDied++;
  printf("Worker thread exitted\n");
}

static void *Worker(void *arg)
{
  SrvJob *job;
  pthread_cleanup_push(ThreadCleanup,0);
  WorkerThreadRunning = 1;
  ProgLoc = 1;
  while (job = NextJob(1))
  {
    char *save_text;
    ProgLoc = 2;
    ServerSetDetailProc(0);
    ProgLoc = 3;
    SetCurrentJob(job);
    ProgLoc = 4;
    if ((job->h.flags & SrvJobBEFORE_NOTIFY) != 0)
    {
      ProgLoc = 5;
      SendReply(job,SrvJobSTARTING,1,0,0);
    }
    ProgLoc = 6;
    switch (job->h.op)
    {
    case SrvAction: 
                    DoSrvAction(job); 
                    break;
    case SrvClose:  
                    DoSrvClose(job); 
                    break;
    case SrvCreatePulse: 
                    DoSrvCreatePulse(job); 
                    break;
    case SrvCommand: 
                    DoSrvCommand(job); 
                    break;
    case SrvMonitor: 
                    DoSrvMonitor(job); 
                    break;
    }
    ProgLoc = 7;
    SetCurrentJob(0);
    ProgLoc = 8;
    FreeJob(job);
    ProgLoc = 9;
    save_text = current_job_text;
    current_job_text = 0;
    if (save_text)
      free(save_text);
    ProgLoc = 10;
  }
  LeftWorkerLoop++;
  pthread_cleanup_pop(1);
  return 0;
}

static SrvJob *GetCurrentJob()
{
  SrvJob *job;
  LockQueue();
  job = CurrentJob;
  return job;
}

static void ReleaseCurrentJob()
{
  UnlockQueue();
}

static void SetCurrentJob(SrvJob *job)
{
  LockQueue();
  CurrentJob = job;
  UnlockQueue();
}

static char *Now()
{
  static char now[64];
  time_t tim;
  tim = time(0);
  strcpy(now,ctime(&tim));
  now[strlen(now)-1]=0;
  return now;
}

static void DoSrvAction(SrvJob *job_in)
{
  int status;
  SrvActionJob *job = (SrvActionJob *)job_in;
  status = TreeOpen(job->tree,job->shot,0);
  if (status & 1)
  {
    int retstatus;
    char *job_text;
    DESCRIPTOR_NID(nid_dsc,0);
    DESCRIPTOR_LONG(ans_dsc,0);
    struct descriptor fullpath = {0, DTYPE_T, CLASS_D, 0};
    DESCRIPTOR(fullpath_d,"FULLPATH");
    DESCRIPTOR(nullstr,"\0");
    DESCRIPTOR_NID(niddsc,0);
    struct descriptor ans_d = {0, DTYPE_T, CLASS_S, 0};
    niddsc.pointer = (char *)&job->nid;
    status = TdiGetNci(&niddsc,&fullpath_d,&fullpath MDS_END_ARG);
    StrAppend(&fullpath,&nullstr);
    job_text = malloc(fullpath.length + 1024);
    sprintf(job_text,"Doing %s in %s shot %d",fullpath.pointer,job->tree,job->shot);
    current_job_text = job_text;
    StrFree1Dx(&fullpath);
    nid_dsc.pointer = (char *)&job->nid;
    ans_dsc.pointer = (char *)&retstatus; 
    TreeSetDefaultNid(0);
    if (Logging)
      {
	  printf("%s, %s\n",Now(),current_job_text);
	  fflush(stdout);
      }
    status = TdiDoTask(&nid_dsc,&ans_dsc MDS_END_ARG);
    if (status & 1)
      status = retstatus;
  }
  SendReply(job_in,SrvJobFINISHED,status,0,0);
}

static void DoSrvClose(SrvJob *job_in)
{
  int status;
  char *job_text = strcpy((char *)malloc(32),"Closing trees");
  current_job_text = job_text;
  while ((status = TreeClose(0,0)) & 1);
  status = (status == TreeNOT_OPEN) ? TreeNORMAL : status;
  SendReply(job_in,SrvJobFINISHED,status,0,0);
}

static void DoSrvCreatePulse(SrvJob *job_in)
{
  int status;
  SrvCreatePulseJob *job = (SrvCreatePulseJob *)job_in;
  char *job_text = malloc(100);
  sprintf(job_text,"Creating pulse for %s shot %d",((SrvCreatePulseJob *)job)->tree,((SrvCreatePulseJob *)job)->shot);
  current_job_text = job_text;
  status = TreeCreateTreeFiles(job->tree,job->shot,-1);
  SendReply(job_in,SrvJobFINISHED,status,0,0);
}

static void DoSrvCommand(SrvJob *job_in)
{
  int status;
  SrvCommandJob *job = (SrvCommandJob *)job_in;
  char *set_table = strcpy(malloc(strlen(job->table)+24),"set command ");
  char *job_text = (char *)malloc(strlen(job->command) + strlen(job->table) + 60);
  ProgLoc = 61;
  sprintf(job_text,"Doing command %s in command table %s",job->command,job->table);
  ProgLoc = 62;
  current_job_text = job_text;
  ProgLoc = 63;
  strcat(set_table,job->table);
  ProgLoc = 64;
  status = mdsdcl_do_command(set_table);
  ProgLoc = 65;
  free(set_table);
  ProgLoc = 66;
  if (status & 1)
  {
    ProgLoc = 67;
    status = mdsdcl_do_command(job->command);
    ProgLoc = 68;
  }
  ProgLoc = 69;
  SendReply(job_in,SrvJobFINISHED,status,0,0);
  ProgLoc = 70;
}

static int AddMonitorClient(SrvJob *job)
{
  MonitorList *new = (MonitorList *)malloc(sizeof(MonitorList));
  new->addr = job->h.addr;
  new->port = job->h.port;
  new->next = Monitors;
  Monitors = new;
  return(1);
}

static void SendToMonitor(MonitorList *m, MonitorList *prev, SrvJob *job_in)
{
  int status;
  SrvMonitorJob *job = (SrvMonitorJob *)job_in;
  char msg[1024];
  sprintf(msg,"%s %d %d %d %d %s",job->tree,job->shot,job->nid,job->on,job->mode,job->server);
  status = SendReply(job_in, SrvJobFINISHED,1,strlen(msg),msg);
  if (!(status & 1))
  {
    if (prev)
      prev->next = m->next;
    else
      Monitors = m->next;
    free(m);
  }
}

static int SendToMonitors(SrvJob *job)
{
  MonitorList *m,*prev;
  for (prev=0,m=Monitors; m; prev=m,m=m->next)
    SendToMonitor(m, prev, job);
  return 1;
}

static void DoSrvMonitor(SrvJob *job_in)
{
  int status;
  SrvMonitorJob *job = (SrvMonitorJob *)job_in;
  status = (job->mode == MonitorCheckin) ? AddMonitorClient(job_in) : SendToMonitors(job_in);
  SendReply(job_in,SrvJobFINISHED,status,0,0);
}

static void LogPrefix(char *ans_c)
{
  char hname[512];
  char *port = MdsGetServerPortname();
  gethostname(hname,512);
  sprintf(ans_c,"%s, %s:%s, %s, ",Now(), hname, port ? port : "?", Logging == 0 ? "logging disabled" : "logging enabled");
  if (Debug)
    sprintf(ans_c+strlen(ans_c),"\nDebug info: QueueLocked = %d ProgLoc = %d WorkerDied = %d"
                                "\n            LeftWorkerLoop = %d CondWStat = %d\n",
     QueueLocked,ProgLoc,WorkerDied,LeftWorkerLoop,CondWStat);
}

static int ShowCurrentJob(struct descriptor_xd *ans)
{
  char *job_text = current_job_text;
  char *ans_c;
  struct descriptor ans_d = {0, DTYPE_T, CLASS_S, 0};
  if (job_text == 0)
  {
    ans_c = malloc(1024);
    LogPrefix(ans_c);
    strcat(ans_c,"Inactive");
  }
  else
  {
    char * (*detail_proc)(int);
    char * detail;
    if (((detail_proc = ServerGetDetailProc()) != 0) && ((detail = (*detail_proc)(1))!=0))
    {
      ans_c = malloc(1024+strlen(job_text)+strlen(detail));
      LogPrefix(ans_c);
      strcat(ans_c,job_text);
      strcat(ans_c,detail);
      free(detail);
    }
    else
    {
      ans_c = malloc(1024+strlen(job_text));
      LogPrefix(ans_c);
      strcat(ans_c,job_text);
    }
  }
  ans_d.length = strlen(ans_c);
  ans_d.pointer = ans_c;
  MdsCopyDxXd(&ans_d,ans);
  free(ans_c);
  return 1;
}



static void KillThread()
{
  if (WorkerThreadRunning != 0)
  {
    pthread_cancel(WorkerThread);
    WorkerThreadRunning = 0;
  }
}

static void LockQueue()
{
  pthread_lock_global_np();
  QueueLocked = 1;
}

static void UnlockQueue()
{
  QueueLocked = 0;
  pthread_unlock_global_np();
}

static void WaitForJob()
{
  ProgLoc = 11;
  pthread_mutex_lock(&JobWaitMutex);
  ProgLoc = 12;
#ifdef HAVE_WINDOWS_H
  status = pthread_cond_timedwait(&JobWaitCondition, 1000);
#else
  {
    struct timespec one_sec = {1,0};
    struct timespec abstime;
    ProgLoc = 12;
    pthread_get_expiration_np(&one_sec,&abstime);
    CondWStat = pthread_cond_timedwait( &JobWaitCondition, &JobWaitMutex, &abstime);
    ProgLoc = 13;
  }
#endif
  /*
  pthread_cond_wait( &JobWaitCondition, &JobWaitMutex);
  */
  ProgLoc = 14;
  pthread_mutex_unlock(&JobWaitMutex);
  ProgLoc = 15;
}

static int StartThread()
{
  static int JobWaitInitialized = 0;
  int status;
  if (JobWaitInitialized == 0)
  {
    status = pthread_mutex_init(&JobWaitMutex,pthread_mutexattr_default);
    if (status)
    {
      perror("Error creating pthread mutex");
      exit(status);
    }
    status = pthread_cond_init(&JobWaitCondition,pthread_condattr_default);
    if (status)
    {
      perror("Error creating pthread condition");
      exit(status);
    }
    JobWaitInitialized = 1;
  }
  if (WorkerThreadRunning == 0)
  {
#ifdef HAVE_WINDOWS_H
	  status = pthread_create(&WorkerThread,0,Worker,0);
#else
    pthread_attr_t att;
    pthread_attr_init(&att);
    pthread_attr_setstacksize(&att,0xffffff);
#ifdef __hpux
    status = pthread_create(&WorkerThread,att, Worker, 0);
    pthread_detach(&WorkerThread);
#else
    pthread_attr_setdetachstate(&att,PTHREAD_CREATE_DETACHED);
    status = pthread_create(&WorkerThread,&att, Worker, 0);
#endif
#endif
    if (status)
    {
      perror("Error creating pthread");
      exit(status);
    }
  }
  status = pthread_cond_signal(&JobWaitCondition);
  if (status)
  {
    perror("Error signalling condition");
    exit(status);
  }
  return 1;
}

static SOCKET AttachPort(int addr, short port)
{
  int sock;
  struct sockaddr_in sin;
  ClientList *l,*new;
  for (l=Clients;l;l=l->next)
    if (l->addr == addr && l->port == port)
    {
      if (ServerBadSocket(l->sock))
      {
        SrvJob job;
        job.h.addr = l->addr;
        job.h.port = l->port;
        RemoveClient(&job);
		break;
      }
      else
        return l->sock;
    }
  sin.sin_port = htons(port);
  sin.sin_family = AF_INET;
  *(int *)(&sin.sin_addr) = addr;
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock >= 0)
  {
    if (connect(sock, (struct sockaddr *)&sin, sizeof(sin)) == -1)
    {
      shutdown(sock,2);
      close(sock);
      return -1;
    }
    new = (ClientList *)malloc(sizeof(ClientList));
    l = Clients;
    Clients = new;
    new->addr = addr;
    new->port = port;
    new->sock = sock;
    new->next = l;
  }
  return sock;
}

static void RemoveClient(SrvJob *job)
{
  ClientList *l,*prev;
  for (prev=0,l=Clients;l;)
  {
    if (job->h.addr == l->addr && job->h.port == l->port)
    {
      shutdown(l->sock,2);
      close(l->sock);
      if (prev)
        prev->next = l->next;
      else
        Clients = l->next;
      free(l);
      break;
    }
    else
    {
      prev = l;
      l = l->next;
    }
  }
}

static int SendReply(SrvJob *job, int replyType, int status_in, int length, char *msg)
{
  int status = 0;
  SOCKET sock;
#ifndef HAVE_WINDOWS_H
  signal(SIGPIPE,SIG_IGN);
#endif
  sock = AttachPort(job->h.addr, (short)job->h.port);
  if (sock >= 0)
  {
    char reply[60];
    int bytes;
    sprintf(reply,"%d %d %d %d",job->h.jobid,replyType, status_in, msg ? strlen(msg) : 0);
    bytes = send(sock, reply, 60, 0);
    if (bytes == 60)
    {
      if (length)
      {
        bytes = send(sock, msg, length, 0);
        if (bytes == length)
          status = 1;
      }
      else
        status = 1;
    }
    if (!(status & 1))
      RemoveClient(job);
  }
#ifndef HAVE_WINDOWS_H
  signal(SIGPIPE,SIG_DFL);
#endif
  return status;
}
