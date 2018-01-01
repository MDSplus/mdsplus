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
#include <mdsplus/mdsconfig.h>
#include <servershr.h>
#include "servershrp.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <strroutines.h>
#include <treeshr.h>
#include <pthread_port.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef _WIN32
#include <windows.h>
#else
 typedef int SOCKET;
 #define INVALID_SOCKET -1
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <signal.h>
#endif
#include <sys/time.h>

typedef struct _MonitorList {
  int addr;
  short port;
  struct _MonitorList *next;
} MonitorList;

typedef struct _ClientList {
  int addr;
  short port;
  int sock;
  struct _ClientList *next;
} ClientList;

extern int TdiDoTask();
extern int TdiGetNci();
extern int TdiExecute();
extern int mdsdcl_do_command(char *command);
extern int ServerBadSocket(int sock);

static int SendReply(SrvJob * job, int replyType, int status, int length, char *msg);

static int DoSrvCommand(SrvJob * job_in);
static int DoSrvAction(SrvJob * job_in);
static int DoSrvClose(SrvJob * job_in);
static int DoSrvCreatePulse(SrvJob * job_in);
static void DoSrvMonitor(SrvJob * job_in);
static char *Now();

static void RemoveClient(SrvJob * job);
extern int MdsGetClientAddr();
extern char *MdsGetServerPortname();
static ClientList *Clients = NULL;

static MonitorList *Monitors = NULL;

static int StartWorker();
static void KillWorker();

static pthread_t Worker;
static Condition WorkerRunning = CONDITION_INITIALIZER;

static int QJob(SrvJob * job);
static void AbortJob(SrvJob * job);
static void FreeJob(SrvJob * job);

static void SetCurrentJob(SrvJob * job);
static int ShowCurrentJob(struct descriptor_xd *ans);
static SrvJob *NextJob(int wait);
static int RemoveLast();

static void LockQueue();
static void UnlockQueue();
#define QUEUE_LOCK   LockQueue();pthread_cleanup_push(UnlockQueue, NULL);
#define QUEUE_UNLOCK pthread_cleanup_pop(1);

static SrvJob *JobQueueNext = NULL;
static SrvJob *CurrentJob = NULL;
static Condition_p JobQueueCond = CONDITION_INITIALIZER;
#define JobQueue (JobQueueCond.value)

static char *current_job_text = NULL;

static int Logging = 1;
static int Debug = 0;
static int QueueLocked = 0;
static int WorkerDied = 0;
static int LeftWorkerLoop = 0;
static int CondWStat = 0;
int ProgLoc = 0;

EXPORT struct descriptor *ServerInfo(){
  static DESCRIPTOR(nothing, "");
  static EMPTYXD(xd);
  int status = ShowCurrentJob(&xd);
  if (STATUS_OK && xd.pointer)
    return xd.pointer;
  else
    return (struct descriptor *)&nothing;
}

EXPORT int ServerDebug(int setting){
  int old = Debug;
  Debug = setting;
  return old;
}
// main
EXPORT int ServerQAction(int *addr, short *port, int *op, int *flags, int *jobid,
		  void *p1, void *p2, void *p3, void *p4, void *p5, void *p6, void *p7, void *p8)
{
  int status = ServerINVALID_ACTION_OPERATION;
  switch (*op) {
  case SrvRemoveLast:
    {
      status = RemoveLast();
      break;
    }
  case SrvAbort:
    {
      KillWorker();
      ServerSetDetailProc(0);
      QUEUE_LOCK;
      AbortJob(CurrentJob);
      CurrentJob = 0;
      if (*(int *)p1) {
	SrvJob *next,*job = JobQueue;
	while (job){
          next = job->h.next;
	  AbortJob(job);
          job = next;
        }
        JobQueueNext = 0;
        JobQueueCond.value = 0;
      }
      QUEUE_UNLOCK;
      break;
    }
  case SrvAction:
    {
      SrvActionJob job;
      job.h.addr = addr ? *addr : MdsGetClientAddr();
      job.h.port = *port;
      job.h.op = *op;
      job.h.length = sizeof(job);
      job.h.flags = *flags;
      job.h.jobid = *jobid;
      job.tree = strcpy(malloc(strlen((char *)p1) + 1), (char *)p1);
      job.shot = *(int *)p2;
      job.nid = *(int *)p3;
      if (job.h.addr)
	status = QJob((SrvJob *) & job);
      else
	status = DoSrvAction((SrvJob *) & job);
      break;
    }
  case SrvClose:
    {
      SrvCloseJob job;
      job.h.addr = MdsGetClientAddr();
      job.h.port = *port;
      job.h.op = *op;
      job.h.length = sizeof(job);
      job.h.flags = *flags;
      job.h.jobid = *jobid;
      if (job.h.addr)
	status = QJob((SrvJob *) & job);
      else
	status = DoSrvClose((SrvJob *) & job);
      break;
    }
  case SrvCreatePulse:
    {
      SrvCreatePulseJob job;
      job.h.addr = MdsGetClientAddr();
      job.h.port = *port;
      job.h.op = *op;
      job.h.length = sizeof(job);
      job.h.flags = *flags;
      job.h.jobid = *jobid;
      job.tree = strcpy(malloc(strlen((char *)p1) + 1), (char *)p1);
      job.shot = *(int *)p2;
      if (job.h.addr)
	status = QJob((SrvJob *) & job);
      else
	status = DoSrvCreatePulse((SrvJob *) & job);
      break;
    }
  case SrvSetLogging:
    {
      Logging = *(int *)p1;
      status = MDSplusSUCCESS;
      break;
    }
  case SrvCommand:
    {
      SrvCommandJob job;
      job.h.addr = MdsGetClientAddr();
      job.h.port = *port;
      job.h.op = *op;
      job.h.length = sizeof(job);
      job.h.flags = *flags;
      job.h.jobid = *jobid;
      job.table = strcpy(malloc(strlen((char *)p1) + 1), (char *)p1);
      job.command = strcpy(malloc(strlen((char *)p2) + 1), (char *)p2);
      if (job.h.addr)
	status = QJob((SrvJob *) & job);
      else
	status = DoSrvCommand((SrvJob *) & job);
      break;
    }
  case SrvMonitor:
    {
      SrvMonitorJob job;
      job.h.addr = MdsGetClientAddr();
      job.h.port = *port;
      job.h.op = *op;
      job.h.length = sizeof(job);
      job.h.flags = *flags;
      job.h.jobid = *jobid;
      job.tree = strcpy(malloc(strlen((char *)p1) + 1), (char *)p1);
      job.shot = *(int *)p2;
      job.phase = *(int *)p3;
      job.nid = *(int *)p4;
      job.on = *(int *)p5;
      job.mode = *(int *)p6;
      job.server = strcpy(malloc(strlen((char *)p7) + 1), (char *)p7);
      job.status = *(int *)p8;
      if (job.h.addr)
	status = QJob((SrvJob *) & job);
      else
	status = MDSplusERROR;
      break;
    }
  case SrvShow:
    {
      status = ShowCurrentJob((struct descriptor_xd *)p1);
      break;
    }
  case SrvStop:
    {
      printf("%s, Server stop requested\n", Now());
      exit(0);
      break;
    }
  }
  return status;
}
// main
static void AbortJob(SrvJob * job){
  if (job) {
    SendReply(job, SrvJobFINISHED, ServerABORT, 0, 0);
    FreeJob(job);
  }
}
// main
static int QJob(SrvJob * job){
  SrvJob *qjob = (SrvJob *) memcpy(malloc(job->h.length), job, job->h.length);
  QUEUE_LOCK;
  if (JobQueueNext)
    JobQueueNext->h.next = qjob;
  qjob->h.next = 0;
  qjob->h.previous = JobQueueNext;
  JobQueueNext = qjob;
  if (!JobQueue)
    JobQueue = qjob;
  _CONDITION_SIGNAL(&JobQueueCond);
  QUEUE_UNLOCK;
  return StartWorker();
}
// main
static void LogPrefix(char *ans_c){
  if (ans_c) {
    char hname[512];
    char *port = MdsGetServerPortname();
    gethostname(hname, 512);
    sprintf(ans_c, "%s, %s:%s, %s, ", Now(), hname, port ? port : "?",
          Logging == 0 ? "logging disabled" : "logging enabled");
    if (Debug) {
      sprintf(ans_c + strlen(ans_c), "\nDebug info: QueueLocked = %d ProgLoc = %d WorkerDied = %d"
            "\n            LeftWorkerLoop = %d CondWStat = %d\n",
            QueueLocked, ProgLoc, WorkerDied, LeftWorkerLoop, CondWStat);
    }
  }
}
// main
static int ShowCurrentJob(struct descriptor_xd *ans){
  char *ans_c;
  struct descriptor ans_d = { 0, DTYPE_T, CLASS_S, 0 };
  QUEUE_LOCK;
  char *job_text = current_job_text;
  if (job_text == 0) {
    ans_c = malloc(1024);
    LogPrefix(ans_c);
    strcat(ans_c, "Inactive");
  } else {
    char *(*detail_proc) (int);
    char *detail;
    if (((detail_proc = ServerGetDetailProc()) != 0) && ((detail = (*detail_proc) (1)) != 0)) {
      ans_c = malloc(1024 + strlen(job_text) + strlen(detail));
      LogPrefix(ans_c);
      strcat(ans_c, job_text);
      strcat(ans_c, detail);
      free(detail);
    } else {
      ans_c = malloc(1024 + strlen(job_text));
      LogPrefix(ans_c);
      strcat(ans_c, job_text);
    }
  }
  QUEUE_UNLOCK;
  ans_d.length = strlen(ans_c);
  ans_d.pointer = ans_c;
  MdsCopyDxXd(&ans_d, ans);
  free(ans_c);
  return MDSplusSUCCESS;
}
// main
static int RemoveLast(){
  int status;
  SrvJob *job;
  QUEUE_LOCK;
  job = JobQueueNext;
  if (job) {
    JobQueueNext = job->h.previous;
    if (JobQueueNext)
      JobQueueNext->h.next = 0;
    else
      JobQueue = 0;
    FreeJob(job);
    printf("Removed pending action");
    status = MDSplusSUCCESS;
  } else
    status = MDSplusERROR;
  QUEUE_UNLOCK;
  return status;
}
// thread
static SrvJob *NextJob(int wait){
  SrvJob *job;
  //  int done = 0;
  while (1) {
    QUEUE_LOCK;
    job = JobQueue;
    if (job) {
      JobQueue = job->h.next;
      if (JobQueue)
	((SrvJob*)JobQueue)->h.previous = 0;
      else
	JobQueueNext = 0;
    }
    if (job == NULL && wait)
      _CONDITION_WAIT(&JobQueueCond);
    QUEUE_UNLOCK;
    if (job || (!wait))
      break;
  }
  return job;
}
// both
static void FreeJob(SrvJob * job){
  switch (job->h.op) {
  case SrvAction:
    free(((SrvActionJob *) job)->tree);
    break;
  case SrvCreatePulse:
    free(((SrvCreatePulseJob *) job)->tree);
    break;
  case SrvCommand:
    free(((SrvCommandJob *) job)->table);
    free(((SrvCommandJob *) job)->command);
    break;
  case SrvMonitor:
    free(((SrvMonitorJob *) job)->tree);
    free(((SrvMonitorJob *) job)->server);
  }
  free(job);
}
// thread
static void SetCurrentJob(SrvJob * job){
  QUEUE_LOCK;
  CurrentJob = job;
  QUEUE_UNLOCK;
}
// thread
static inline void LockQueue(){
  _CONDITION_LOCK(&JobQueueCond);
  QueueLocked = 1;
}
// thread
static inline void UnlockQueue(){
  QueueLocked = 0;
  _CONDITION_UNLOCK(&JobQueueCond);
}
// both
static char *Now(){
  static char now[64];
  time_t tim;
  tim = time(0);
  strcpy(now, ctime(&tim));
  now[strlen(now) - 1] = 0;
  return now;
}
// main
static int doingNid;
static pthread_rwlock_t doing_nid_rwlock = PTHREAD_RWLOCK_INITIALIZER;
EXPORT int GetDoingNid(){
  pthread_rwlock_rdlock(&doing_nid_rwlock);
  int nid = doingNid;
  pthread_rwlock_unlock(&doing_nid_rwlock);
  return nid;
}
// thread
static int DoSrvAction(SrvJob * job_in){
  INIT_STATUS_ERROR;
  SrvActionJob *job = (SrvActionJob *) job_in;
  char *job_text, *old_job_text;
  sprintf((job_text =
	   (char *)malloc(100)), "Doing nid %d in %s shot %d", job->nid, job->tree, job->shot);
  current_job_text = job_text;
  status = TreeOpen(job->tree, job->shot, 0);
  if STATUS_OK {
    int retstatus;
    DESCRIPTOR_NID(nid_dsc, 0);
    DESCRIPTOR_LONG(ans_dsc, 0);
    struct descriptor_d fullpath = { 0, DTYPE_T, CLASS_D, 0 };
    DESCRIPTOR(fullpath_d, "FULLPATH");
    DESCRIPTOR(nullstr, "\0");
    DESCRIPTOR_NID(niddsc, 0);
    niddsc.pointer = (char *)&job->nid;
    pthread_rwlock_wrlock(&doing_nid_rwlock);
    doingNid = job->nid;
    pthread_rwlock_unlock(&doing_nid_rwlock);
    status = TdiGetNci(&niddsc, &fullpath_d, &fullpath MDS_END_ARG);
    StrAppend(&fullpath, (struct descriptor *)&nullstr);
    job_text = malloc(fullpath.length + 1024);
    sprintf(job_text, "Doing %s in %s shot %d", fullpath.pointer, job->tree, job->shot);
    old_job_text = current_job_text;
    current_job_text = job_text;
    free(old_job_text);
    StrFree1Dx(&fullpath);
    nid_dsc.pointer = (char *)&job->nid;
    ans_dsc.pointer = (char *)&retstatus;
    TreeSetDefaultNid(0);
    if (Logging) {
      printf("%s, %s\n", Now(), current_job_text);
      fflush(stdout);
    }
    status = TdiDoTask(&nid_dsc, &ans_dsc MDS_END_ARG);
    if (Logging) {
      memcpy(current_job_text, "Done ", 5);
      printf("%s, %s\n", Now(), current_job_text);
      fflush(stdout);
    }
    if STATUS_OK
      status = retstatus;
  }
  if (job_in->h.addr)
    SendReply(job_in, SrvJobFINISHED, status, 0, 0);
  return status;
}
// thread
static int DoSrvClose(SrvJob * job_in){
  INIT_STATUS_ERROR;
  char *job_text = strcpy((char *)malloc(32), "Closing trees");
  current_job_text = job_text;
  while IS_OK(status = TreeClose(0, 0));
  status = (status == TreeNOT_OPEN) ? TreeNORMAL : status;
  if (job_in->h.addr)
    SendReply(job_in, SrvJobFINISHED, status, 0, 0);
  return status;
}
// thread
static int DoSrvCreatePulse(SrvJob * job_in){
  INIT_STATUS_ERROR;
  SrvCreatePulseJob *job = (SrvCreatePulseJob *) job_in;
  char *job_text = malloc(100);
  sprintf(job_text, "Creating pulse for %s shot %d", ((SrvCreatePulseJob *) job)->tree,
	  ((SrvCreatePulseJob *) job)->shot);
  current_job_text = job_text;
  status = TreeCreateTreeFiles(job->tree, job->shot, -1);
  if (job_in->h.addr)
    SendReply(job_in, SrvJobFINISHED, status, 0, 0);
  return status;
}
// thread
static int DoSrvCommand(SrvJob * job_in){
  INIT_STATUS_ERROR;
  SrvCommandJob *job = (SrvCommandJob *) job_in;
  char *set_table = strcpy(malloc(strlen(job->table) + 24), "set command ");
  char *job_text = (char *)malloc(strlen(job->command) + strlen(job->table) + 60);
  ProgLoc = 61;
  sprintf(job_text, "Doing command %s in command table %s", job->command, job->table);
  ProgLoc = 62;
  current_job_text = job_text;
  ProgLoc = 63;
  strcat(set_table, job->table);
  ProgLoc = 64;
  status = mdsdcl_do_command(set_table);
  ProgLoc = 65;
  free(set_table);
  ProgLoc = 66;
  if STATUS_OK {
    ProgLoc = 67;
    status = mdsdcl_do_command(job->command);
    //ProgLoc = 68;
  }
  ProgLoc = 69;
  if (job_in->h.addr)
    SendReply(job_in, SrvJobFINISHED, status, 0, 0);
  ProgLoc = 70;
  return status;
}
// thread
static int AddMonitorClient(SrvJob * job){
  MonitorList *mlist;
  for (mlist = Monitors ; mlist ; mlist = mlist->next)
    if (job->h.addr == mlist->addr && job->h.port == mlist->port)
      return MDSplusSUCCESS;
  mlist = (MonitorList *) malloc(sizeof(MonitorList));
  mlist->addr = job->h.addr;
  mlist->port = job->h.port;
  mlist->next = Monitors;
  Monitors = mlist;
  return MDSplusSUCCESS;
}
// thread
static void SendToMonitor(MonitorList *m, MonitorList *prev, SrvJob *job_in){
  INIT_STATUS_ERROR;
  SrvMonitorJob *job = (SrvMonitorJob *) job_in;
  char *msg = NULL;// char msg[1024];
  // sprintf(msg,"%s %d %d %d %d %s",job->tree,job->shot,job->nid,job->on,job->mode,job->server);
  struct descriptor_d fullpath = { 0, DTYPE_T, CLASS_D, 0 };
  DESCRIPTOR(fullpath_d, "FULLPATH");
  DESCRIPTOR(nullstr, "\0");
  DESCRIPTOR_NID(niddsc, 0);
  char *status_text = MdsGetMsg(job->status);
  status = TreeOpen(job->tree, job->shot, 0);
  if STATUS_NOT_OK // try to open model instead
      status = TreeOpen(job->tree, -1, 0);
  if STATUS_OK {
    niddsc.pointer = (char *)&job->nid;
    status = TdiGetNci(&niddsc, &fullpath_d, &fullpath MDS_END_ARG);
    StrAppend(&fullpath, (struct descriptor *)&nullstr);
    msg = malloc(fullpath.length + 1024 + strlen(status_text));
    if (job->server && *job->server)
      sprintf(msg, "%s %d %d %d %d %d %s %d %s %s; %s", job->tree, job->shot, job->phase,
	      job->nid, job->on, job->mode, job->server,
	      job->status, fullpath.pointer, Now(), status_text);
    else
      sprintf(msg, "%s %d %d %d %d %d unknown %d %s %s; %s", job->tree, job->shot, job->phase,
	      job->nid, job->on, job->mode, job->status, fullpath.pointer, Now(), status_text);
    StrFree1Dx(&fullpath);
  } else {
    msg = malloc(1024 + strlen(status_text));
    sprintf(msg, "%s %d %d %d %d %d %s %d unknown %s; %s", job->tree, job->shot, job->phase,
	    job->nid, job->on, job->mode, job->server, job->status, Now(), status_text);
  }
  status = SendReply(job_in, SrvJobFINISHED, 1, strlen(msg), msg);
  if (msg)
    free(msg);
  if STATUS_NOT_OK {
    if (prev)
        prev->next = m->next;
    else
        Monitors = m->next;
    free(m);
  }
}
// thread
static int SendToMonitors(SrvJob * job){
  MonitorList *m, *prev, *next;
  int prev_addr = job->h.addr;
  short prev_port = job->h.port;
  for (prev = NULL , m = Monitors ; m ; m = next) {
    job->h.addr = m->addr;
    job->h.port = m->port;
    next = m->next;
    SendToMonitor(m, prev, job);
  }
  job->h.addr = prev_addr;
  job->h.port = prev_port;
  return MDSplusSUCCESS;
}
// thread
static void DoSrvMonitor(SrvJob * job_in){
  INIT_STATUS_ERROR;
  SrvMonitorJob *job = (SrvMonitorJob *) job_in;
  status = (job->mode == MonitorCheckin) ? AddMonitorClient(job_in) : SendToMonitors(job_in);
  SendReply(job_in, (job->mode == MonitorCheckin) ? SrvJobCHECKEDIN : SrvJobFINISHED, status, 0, 0);
}
// thread
static void WorkerExit(void *arg __attribute__ ((unused))){
  if (QueueLocked) UnlockQueue();
  CONDITION_RESET(&WorkerRunning);
  WorkerDied++;
  fprintf(stderr,"Worker thread exitted\n");
}
// thread
static void WorkerThread(void *arg __attribute__ ((unused)) ){
  SrvJob *job;
  pthread_cleanup_push(WorkerExit, NULL);
  ProgLoc = 1;
  CONDITION_SET(&WorkerRunning);
  while ((job = NextJob(1))) {
    if (Debug) fprintf(stderr,"job started.\n");
    char *save_text;
    ProgLoc = 2;
    ServerSetDetailProc(0);
    ProgLoc = 3;
    SetCurrentJob(job);
    ProgLoc = 4;
    if ((job->h.flags & SrvJobBEFORE_NOTIFY) != 0) {
      ProgLoc = 5;
      SendReply(job, SrvJobSTARTING, 1, 0, 0);
    }
    ProgLoc = 6;
    switch (job->h.op) {
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
    if (Debug) fprintf(stderr,"job done.\n");
  }
  LeftWorkerLoop++;
  pthread_cleanup_pop(1);
  pthread_exit(NULL);
}
// main
static int StartWorker(){
  INIT_STATUS;
  CONDITION_START_THREAD(&WorkerRunning, Worker, /4, WorkerThread,NULL);
  if STATUS_NOT_OK exit(-1);
  return status;
}
// main
static void KillWorker(){
  _CONDITION_LOCK(&WorkerRunning);
  if (WorkerRunning.value){
    pthread_cancel(Worker);
    _CONDITION_WAIT_RESET(&WorkerRunning);
  }
  _CONDITION_UNLOCK(&WorkerRunning);
}

// both
static SOCKET AttachPort(int addr, short port){
  int sock;
  struct sockaddr_in sin;
  ClientList *l, *new;
  for (l = Clients; l; l = l->next)
    if (l->addr == addr && l->port == port) {
      if (ServerBadSocket(l->sock)) {
	SrvJob job;
	job.h.addr = l->addr;
	job.h.port = l->port;
	RemoveClient(&job);
	break;
      } else
	return l->sock;
    }
  sin.sin_port = htons(port);
  sin.sin_family = AF_INET;
  *(int *)(&sin.sin_addr) = addr;
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock >= 0) {
    if (connect(sock, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
      shutdown(sock, 2);
      close(sock);
      perror("Error establishing reply connection to mdstcl client (i.e. dispatcher)");
      return INVALID_SOCKET;
    }
    new = (ClientList *) malloc(sizeof(ClientList));
    l = Clients;
    Clients = new;
    new->addr = addr;
    new->port = port;
    new->sock = sock;
    new->next = l;
  }
  return sock;
}
// both
static void RemoveClient(SrvJob * job){
  ClientList *l, *prev;
  for (prev = 0, l = Clients; l;) {
    if (job->h.addr == l->addr && job->h.port == l->port) {
      shutdown(l->sock, 2);
      close(l->sock);
      if (prev)
	prev->next = l->next;
      else
	Clients = l->next;
      free(l);
      break;
    } else {
      prev = l;
      l = l->next;
    }
  }
}
// both
static int SendReply(SrvJob * job, int replyType, int status_in, int length, char *msg){
  INIT_STATUS_ERROR;
  SOCKET sock;
#ifndef _WIN32
  signal(SIGPIPE, SIG_IGN);
#endif
  sock = AttachPort(job->h.addr, (short)job->h.port);
  if (sock != INVALID_SOCKET) {
    char reply[60];
    int bytes;
    memset(reply, 0, 60);
    sprintf(reply, "%d %d %d %ld", job->h.jobid, replyType, status_in, msg ? (long)strlen(msg) : 0);
    bytes = send(sock, reply, 60, 0);
    if (bytes == 60) {
      if (length) {
	bytes = send(sock, msg, length, 0);
	if (bytes == length)
	  status = MDSplusSUCCESS;
      } else
	status = MDSplusSUCCESS;
    }
    if STATUS_NOT_OK
      RemoveClient(job);
  }
#ifndef _WIN32
  signal(SIGPIPE, SIG_DFL);
#endif
  return status;
}

