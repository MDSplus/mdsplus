#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <treeshr.h>
#include <sys/socket.h>
#include <netinet/in.h>
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
static int GetRemAddr();
static char *GetPortName();

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

int ServerQAction(int *addr, short *port, int *op, int *flags, int *jobid, 
                  void *p1, void *p2, void *p3, void *p4, void *p5, void *p6, void *p7, void *p8)
{
  int status;
  switch (*op)
  {
  case SrvAbort:
    {
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
      job.h.addr  = GetRemAddr();
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
      job.h.addr  = GetRemAddr();
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
      job.h.addr  = GetRemAddr();
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
      job.h.addr  = GetRemAddr();
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
      job.h.addr  = GetRemAddr();
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
  int status;
  SrvJob *qjob = memcpy(malloc(job->h.length),job,job->h.length);
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

static void *Worker(void *arg)
{
  SrvJob *job;
  WorkerThreadRunning = 1;
  while (job = NextJob(1))
  {
    if (job)
    {
      char *save_text;
      SetCurrentJob(job);
      if ((job->h.flags & SrvJobBEFORE_NOTIFY) != 0)
        SendReply(job,SrvJobSTARTING,1,0,0);
      switch (job->h.op)
      {
      case SrvAction: DoSrvAction(job); break;
      case SrvClose: DoSrvClose(job); break;
      case SrvCreatePulse: DoSrvCreatePulse(job); break;
      case SrvCommand: DoSrvCommand(job); break;
      case SrvMonitor: DoSrvMonitor(job); break;
      }
      SetCurrentJob(0);
      FreeJob(job);
      save_text = current_job_text;
      current_job_text = 0;
      if (save_text)
        free(save_text);
    }
  }
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
      printf("%s, %s\n",Now(),current_job_text);
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
  char *set_table = strcpy(malloc(strlen(job->table)+12),"set command ");
  char *job_text = (char *)malloc(strlen(job->command) + strlen(job->table) + 60);
  sprintf(job_text,"Doing command %s in command table %s",job->command,job->table);
  current_job_text = job_text;
  strcat(set_table,job->table);
  status = mdsdcl_do_command(set_table);
  free(set_table);
  if (status & 1)
    status = mdsdcl_do_command(job->command);
  SendReply(job_in,SrvJobFINISHED,status,0,0);
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
  char *port = GetPortName();
  gethostname(hname,512);
  sprintf(ans_c,"%s, %s:%s, %s, ",Now(), hname, port ? port : "?", Logging == 0 ? "logging disabled" : "logging enabled");
}

static int ShowCurrentJob(struct descriptor_xd *ans)
{
  char *job_text = current_job_text;
  char *ans_c;
  struct descriptor ans_d = {0, DTYPE_T, CLASS_S, 0};
  if (job_text == 0)
    job_text = "Inactive";
  ans_c = malloc(100+strlen(job_text));
  LogPrefix(ans_c);
  strcat(ans_c,job_text);
  ans_d.length = strlen(ans_c);
  ans_d.pointer = ans_c;
  MdsCopyDxXd(&ans_d,ans);
  return 1;
}

#ifndef _WIN32

#include <pthread.h>

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
}

static void UnlockQueue()
{
  pthread_unlock_global_np();
}

static void WaitForJob()
{
  pthread_mutex_lock(&JobWaitMutex);
  pthread_cond_wait( &JobWaitCondition, &JobWaitMutex);
  pthread_mutex_unlock(&JobWaitMutex);
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
    status = pthread_create(&WorkerThread, pthread_attr_default, Worker, 0);
    if (status)
    {
      perror("Error creating pthread");
      exit(status);
    }
  }
  else
  {
    status = pthread_cond_signal(&JobWaitCondition);
    if (status)
    {
      perror("Error signalling condition");
      exit(status);
    }
  }
  return 1;
}

#endif

int main(int argc, char **argv)
{
  ServerQAction(0,0,0,0,0,0,0,0,0,0,0,0,0);
}

static int AttachPort(int addr, short port)
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
      perror("Error in connect back to client");
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
  int sock = AttachPort(job->h.addr, job->h.port);
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
  return status;
}
      
static int GetRemAddr()
{
  static int addr;
  static DESCRIPTOR_LONG(addr_d,&addr);
  static DESCRIPTOR(cmd,"public $REMADDR");
  TdiExecute(&cmd,&addr_d MDS_END_ARG);
  return addr;
}

static char *GetPortName()
{
  static char *portname = 0;
  if (!portname)
  {
    int status;
    struct descriptor port_d = {0, DTYPE_T, CLASS_D, 0};
    static DESCRIPTOR(endnull,"\0");
    static DESCRIPTOR(cmd,"public $PORTNAME");
    status = TdiExecute(&cmd,&port_d MDS_END_ARG);
    StrAppend(&port_d,&endnull);
    portname = port_d.pointer;
  }
  return portname;
}
