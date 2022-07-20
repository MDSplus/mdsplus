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
#include <socket_port.h>
#include <mdsplus/mdsconfig.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <servershr.h>
#include "servershrp.h"
#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <strroutines.h>
#include <treeshr.h>
#include <condition.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/time.h>

//#define DEBUG
#include <mdsmsg.h>

typedef struct _MonitorList
{
  uint32_t addr;
  uint16_t port;
  struct _MonitorList *next;
} MonitorList;

typedef struct _ClientList
{
  uint32_t addr;
  uint16_t port;
  SOCKET sock;
  struct _ClientList *next;
} ClientList;

extern int TdiDoTask();
extern int TdiGetNci();
extern int TdiExecute();
extern int mdsdcl_do_command(char *command);
extern int is_broken_socket(SOCKET sock);

static int send_reply(SrvJob *job, int replyType, int status, int length,
                      char *msg);

static int DoSrvCommand(SrvJob *job_in);
static int DoSrvAction(SrvJob *job_in);
static int DoSrvClose(SrvJob *job_in);
static int DoSrvCreatePulse(SrvJob *job_in);
static void DoSrvMonitor(SrvJob *job_in);

static void RemoveClient(SrvJob *job);
extern uint32_t MdsGetClientAddr();
extern char *GetPortname();
static ClientList *Clients = NULL;

static MonitorList *Monitors = NULL;

static int StartWorker();
static void KillWorker();

static pthread_t Worker;
static Condition WorkerRunning = CONDITION_INITIALIZER;

static int QJob(SrvJob *job);
static void AbortJob(SrvJob *job);
static void FreeJob(SrvJob *job);

static void SetCurrentJob(SrvJob *job);
static int ShowCurrentJob(struct descriptor_xd *ans);
static SrvJob *NextJob(int wait);
static int RemoveLast();

static void LockQueue();
static void UnlockQueue();
#define QUEUE_LOCK \
  LockQueue();     \
  pthread_cleanup_push(UnlockQueue, NULL);
#define QUEUE_UNLOCK pthread_cleanup_pop(1);

static SrvJob *JobQueueNext = NULL;
static SrvJob *CurrentJob = NULL;
static Condition_p JobQueueCond = CONDITION_INITIALIZER;
#define JobQueue (JobQueueCond.value)

static pthread_mutex_t STATIC_lock = PTHREAD_MUTEX_INITIALIZER;
static char *STATIC_current_job_text = NULL;
static int STATIC_Logging = 1;
static int STATIC_Debug
#ifdef DEBUG
    = 1;
#else
    = 0;
#endif
static int STATIC_QueueLocked = 0;
static int STATIC_WorkerDied = 0;
static int STATIC_LeftWorkerLoop = 0;
static int STATIC_CondWStat = 0;
int ProgLoc = 0;

EXPORT struct descriptor *ServerInfo()
{
  static DESCRIPTOR(nothing, "");
  static EMPTYXD(xd);
  int status = ShowCurrentJob(&xd);
  if (STATUS_OK && xd.pointer)
    return xd.pointer;
  else
    return (struct descriptor *)&nothing;
}

EXPORT int ServerDebug(int setting)
{
  int old;
  pthread_mutex_lock(&STATIC_lock);
  old = STATIC_Debug;
  STATIC_Debug = setting;
  pthread_mutex_unlock(&STATIC_lock);
  return old;
}

// main
EXPORT int ServerQAction(uint32_t *addrp, uint16_t *portp, int *op, int *flags,
                         int *jobid, void *p1, void *p2, void *p3, void *p4,
                         void *p5, void *p6, void *p7, void *p8)
{
  uint32_t addr = addrp ? *addrp : MdsGetClientAddr();
  MDSDBG(IPADDRPRI ":%d", IPADDRVAR(&addr), portp ? *portp : -1);
  int status = ServerINVALID_ACTION_OPERATION;
  switch (*op)
  {
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
    if (*(int *)p1)
    {
      SrvJob *next, *job = JobQueue;
      while (job)
      {
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
    job.h.addr = addr;
    job.h.port = *portp;
    job.h.op = *op;
    job.h.length = sizeof(job);
    job.h.flags = *flags;
    job.h.jobid = *jobid;
    job.tree = strdup((char *)p1);
    job.shot = *(int *)p2;
    job.nid = *(int *)p3;
    if (job.h.addr)
    {
      MDSDBG(SVRACTIONJOB_PRI, SVRACTIONJOB_VAR(&job));
      status = QJob((SrvJob *)&job);
    }
    else
    {
      MDSWRN(SVRACTIONJOB_PRI " No Addr", SVRACTIONJOB_VAR(&job));
      status = DoSrvAction((SrvJob *)&job);
    }
    break;
  }
  case SrvClose:
  {
    SrvCloseJob job;
    job.h.addr = addr;
    job.h.port = *portp;
    job.h.op = *op;
    job.h.length = sizeof(job);
    job.h.flags = *flags;
    job.h.jobid = *jobid;
    if (job.h.addr)
    {
      MDSDBG(SVRCLOSEJOB_PRI, SVRCLOSEJOB_VAR(&job));
      status = QJob((SrvJob *)&job);
    }
    else
    {
      MDSWRN(SVRCLOSEJOB_PRI " No Addr", SVRCLOSEJOB_VAR(&job));
      status = DoSrvClose((SrvJob *)&job);
    }
    break;
  }
  case SrvCreatePulse:
  {
    SrvCreatePulseJob job;
    job.h.addr = addr;
    job.h.port = *portp;
    job.h.op = *op;
    job.h.length = sizeof(job);
    job.h.flags = *flags;
    job.h.jobid = *jobid;
    job.tree = strdup((char *)p1);
    job.shot = *(int *)p2;
    if (job.h.addr)
    {
      MDSDBG(SVRCREATEPULSEJOB_PRI, SVRCREATEPULSEJOB_VAR(&job));
      status = QJob((SrvJob *)&job);
    }
    else
    {
      MDSWRN(SVRCREATEPULSEJOB_PRI " No Addr", SVRCREATEPULSEJOB_VAR(&job));
      status = DoSrvCreatePulse((SrvJob *)&job);
    }
    break;
  }
  case SrvSetLogging:
  {
    pthread_mutex_lock(&STATIC_lock);
    STATIC_Logging = *(int *)p1;
    pthread_mutex_unlock(&STATIC_lock);
    status = MDSplusSUCCESS;
    break;
  }
  case SrvCommand:
  {
    SrvCommandJob job;
    job.h.addr = addr;
    job.h.port = *portp;
    job.h.op = *op;
    job.h.length = sizeof(job);
    job.h.flags = *flags;
    job.h.jobid = *jobid;
    job.table = strdup((char *)p1);
    job.command = strdup((char *)p2);
    if (job.h.addr)
    {
      MDSDBG(SVRCOMMANDJOB_PRI, SVRCOMMANDJOB_VAR(&job));
      status = QJob((SrvJob *)&job);
    }
    else
    {
      MDSWRN(SVRCOMMANDJOB_PRI " No Addr", SVRCOMMANDJOB_VAR(&job));
      status = DoSrvCommand((SrvJob *)&job);
    }
    break;
  }
  case SrvMonitor:
  {
    SrvMonitorJob job;
    job.h.addr = addr;
    job.h.port = *portp;
    job.h.op = *op;
    job.h.length = sizeof(job);
    job.h.flags = *flags;
    job.h.jobid = *jobid;
    job.tree = strdup((char *)p1);
    job.shot = *(int *)p2;
    job.phase = *(int *)p3;
    job.nid = *(int *)p4;
    job.on = *(int *)p5;
    job.mode = *(int *)p6;
    job.server = strdup((char *)p7);
    job.status = *(int *)p8;
    if (job.h.addr)
    {
      MDSDBG(SVRMONITORJOB_PRI, SVRMONITORJOB_VAR(&job));
      status = QJob((SrvJob *)&job);
    }
    else
    {
      MDSWRN(SVRMONITORJOB_PRI " No Addr", SVRMONITORJOB_VAR(&job));
      status = MDSplusERROR;
    }
    break;
  }
  case SrvShow:
  {
    status = ShowCurrentJob((struct descriptor_xd *)p1);
    break;
  }
  case SrvStop:
  {
    char now[32];
    Now32(now);
    printf("%s, Server stop requested\n", now);
    exit(0);
    break;
  }
  }
  return status;
}

// main
static void AbortJob(SrvJob *job)
{
  if (job)
  {
    send_reply(job, SrvJobFINISHED, ServerABORT, 0, 0);
    FreeJob(job);
  }
}

// main
static int QJob(SrvJob *job)
{
  MDSDBG("Queued job %d for " IPADDRPRI ":%d", job->h.jobid, IPADDRVAR(&job->h.addr), job->h.port);
  SrvJob *qjob = (SrvJob *)memcpy(malloc(job->h.length), job, job->h.length);
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
static void STATIC_log_prefix_locked(char *ans_c)
{
  if (ans_c)
  {
    char hname[512];
    char *port = GetPortname();
    gethostname(hname, 512);
    char now[32];
    Now32(now);
    sprintf(ans_c, "%s, %s:%s, %s, ", now, hname, port ? port : "?",
            STATIC_Logging ? "logging enabled" : "logging disabled");
    if (STATIC_Debug)
    {
      sprintf(ans_c + strlen(ans_c),
              "\nDebug info: QueueLocked=%d, ProgLoc=%d, WorkerDied=%d, LeftWorkerLoop=%d, CondWStat=%d,\n",
              STATIC_QueueLocked, ProgLoc, STATIC_WorkerDied, STATIC_LeftWorkerLoop, STATIC_CondWStat);
    }
  }
}

// main
static int ShowCurrentJob(struct descriptor_xd *ans)
{
  char *ans_c;
  struct descriptor ans_d = {0, DTYPE_T, CLASS_S, 0};
  pthread_mutex_lock(&STATIC_lock);
  char *job_text = STATIC_current_job_text;
  if (job_text == 0)
  {
    ans_c = malloc(1024);
    STATIC_log_prefix_locked(ans_c);
    strcat(ans_c, "Inactive");
  }
  else
  {
    char *(*detail_proc)(int);
    char *detail;
    if (((detail_proc = ServerGetDetailProc()) != 0) &&
        ((detail = (*detail_proc)(1)) != 0))
    {
      ans_c = malloc(1024 + strlen(job_text) + strlen(detail));
      STATIC_log_prefix_locked(ans_c);
      strcat(ans_c, job_text);
      strcat(ans_c, detail);
      free(detail);
    }
    else
    {
      ans_c = malloc(1024 + strlen(job_text));
      STATIC_log_prefix_locked(ans_c);
      strcat(ans_c, job_text);
    }
  }
  pthread_mutex_unlock(&STATIC_lock);
  ans_d.length = strlen(ans_c);
  ans_d.pointer = ans_c;
  MdsCopyDxXd(&ans_d, ans);
  free(ans_c);
  return MDSplusSUCCESS;
}
// main
static int RemoveLast()
{
  int status;
  SrvJob *job;
  QUEUE_LOCK;
  job = JobQueueNext;
  if (job)
  {
    JobQueueNext = job->h.previous;
    if (JobQueueNext)
      JobQueueNext->h.next = 0;
    else
      JobQueue = 0;
    MDSMSG(SVRJOB_PRI "Removed pending action", SVRJOB_VAR(job));
    FreeJob(job);
    status = MDSplusSUCCESS;
  }
  else
    status = MDSplusERROR;
  QUEUE_UNLOCK;
  return status;
}
// thread
static SrvJob *NextJob(int wait)
{
  SrvJob *job;
  //  int done = 0;
  while (1)
  {
    QUEUE_LOCK;
    job = JobQueue;
    if (job)
    {
      JobQueue = job->h.next;
      if (JobQueue)
        ((SrvJob *)JobQueue)->h.previous = 0;
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
// thread
static void SetCurrentJob(SrvJob *job)
{
  QUEUE_LOCK;
  CurrentJob = job;
  QUEUE_UNLOCK;
}
// thread
static inline void LockQueue()
{
  _CONDITION_LOCK(&JobQueueCond);
  pthread_mutex_lock(&STATIC_lock);
  STATIC_QueueLocked = 1;
  pthread_mutex_unlock(&STATIC_lock);
}
// thread
static inline void UnlockQueue()
{
  _CONDITION_UNLOCK(&JobQueueCond);
  pthread_mutex_lock(&STATIC_lock);
  STATIC_QueueLocked = 0;
  pthread_mutex_unlock(&STATIC_lock);
}
// main
static int doingNid;
static pthread_rwlock_t doing_nid_rwlock = PTHREAD_RWLOCK_INITIALIZER;
EXPORT int GetDoingNid()
{
  pthread_rwlock_rdlock(&doing_nid_rwlock);
  int nid = doingNid;
  pthread_rwlock_unlock(&doing_nid_rwlock);
  return nid;
}
// thread
static int DoSrvAction(SrvJob *job_in)
{
  int status;
  SrvActionJob *job = (SrvActionJob *)job_in;
  char *job_text, *old_job_text;
  sprintf((job_text = (char *)malloc(100)), "Doing nid %d in %s shot %d",
          job->nid, job->tree, job->shot);
  pthread_mutex_lock(&STATIC_lock);
  STATIC_current_job_text = job_text;
  pthread_mutex_unlock(&STATIC_lock);
  void *dbid = NULL;
  status = _TreeNewDbid(&dbid);
  if (STATUS_NOT_OK)
    goto end;
  void *pc = TreeCtxPush(&dbid);
  status = TreeOpen(job->tree, job->shot, 0);
  if (STATUS_OK)
  {
    int retstatus;
    DESCRIPTOR_NID(nid_dsc, 0);
    DESCRIPTOR_LONG(ans_dsc, 0);
    struct descriptor_d fullpath = {0, DTYPE_T, CLASS_D, 0};
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
    sprintf(job_text, "Doing %s in %s shot %d", fullpath.pointer, job->tree,
            job->shot);
    StrFree1Dx(&fullpath);
    nid_dsc.pointer = (char *)&job->nid;
    ans_dsc.pointer = (char *)&retstatus;
    TreeSetDefaultNid(0);
    pthread_mutex_lock(&STATIC_lock);
    old_job_text = STATIC_current_job_text;
    STATIC_current_job_text = job_text;
    free(old_job_text);
    if (STATIC_Logging)
    {
      char now[32];
      Now32(now);
      printf("%s, %s\n", now, STATIC_current_job_text);
      fflush(stdout);
    }
    pthread_mutex_unlock(&STATIC_lock);
    status = TdiDoTask(&nid_dsc, &ans_dsc MDS_END_ARG);
    pthread_mutex_lock(&STATIC_lock);
    memcpy(STATIC_current_job_text, "Done ", 5);
    if (STATIC_Logging)
    {
      char now[32];
      Now32(now);
      printf("%s, %s\n", now, STATIC_current_job_text);
      fflush(stdout);
    }
    pthread_mutex_unlock(&STATIC_lock);
    if (STATUS_OK)
      status = retstatus;
  }
  TreeCtxPop(pc);
end:;
  TreeFreeDbid(dbid);
  if (job_in->h.addr)
    send_reply(job_in, SrvJobFINISHED, status, 0, 0);
  return status;
}
// thread
static int DoSrvClose(SrvJob *job_in)
{
  INIT_STATUS_ERROR;
  char *job_text = strcpy((char *)malloc(32), "Closing trees");
  pthread_mutex_lock(&STATIC_lock);
  STATIC_current_job_text = job_text;
  pthread_mutex_unlock(&STATIC_lock);
  do
  {
    status = TreeClose(0, 0);
  } while (STATUS_OK);
  status = (status == TreeNOT_OPEN) ? TreeSUCCESS : status;
  if (job_in->h.addr)
    send_reply(job_in, SrvJobFINISHED, status, 0, 0);
  return status;
}
// thread
static int DoSrvCreatePulse(SrvJob *job_in)
{
  SrvCreatePulseJob *job = (SrvCreatePulseJob *)job_in;
  char *job_text = malloc(100);
  sprintf(job_text, "Creating pulse for %s shot %d",
          ((SrvCreatePulseJob *)job)->tree, ((SrvCreatePulseJob *)job)->shot);
  pthread_mutex_lock(&STATIC_lock);
  STATIC_current_job_text = job_text;
  pthread_mutex_unlock(&STATIC_lock);
  int status = TreeCreateTreeFiles(job->tree, job->shot, -1);
  if (job_in->h.addr)
    send_reply(job_in, SrvJobFINISHED, status, 0, 0);
  return status;
}
// thread
static int DoSrvCommand(SrvJob *job_in)
{
  INIT_STATUS_ERROR;
  SrvCommandJob *job = (SrvCommandJob *)job_in;
  char *set_table = strcpy(malloc(strlen(job->table) + 24), "set command ");
  char *job_text =
      (char *)malloc(strlen(job->command) + strlen(job->table) + 60);
  sprintf(job_text, "Doing command %s in command table %s", job->command,
          job->table);
  pthread_mutex_lock(&STATIC_lock);
  STATIC_current_job_text = job_text;
  pthread_mutex_unlock(&STATIC_lock);
  strcat(set_table, job->table);
  status = mdsdcl_do_command(set_table);
  free(set_table);
  if (STATUS_OK)
  {
    status = mdsdcl_do_command(job->command);
  }
  if (job_in->h.addr)
    send_reply(job_in, SrvJobFINISHED, status, 0, 0);
  return status;
}
// thread
static int AddMonitorClient(SrvJob *job)
{
  MonitorList *mlist;
  for (mlist = Monitors; mlist; mlist = mlist->next)
    if (job->h.addr == mlist->addr && job->h.port == mlist->port)
      return MDSplusSUCCESS;
  mlist = (MonitorList *)malloc(sizeof(MonitorList));
  mlist->addr = job->h.addr;
  mlist->port = job->h.port;
  mlist->next = Monitors;
  Monitors = mlist;
  return MDSplusSUCCESS;
}
// thread
static void SendToMonitor(MonitorList *m, MonitorList *prev, SrvJob *job_in)
{
  INIT_STATUS_ERROR;
  SrvMonitorJob *job = (SrvMonitorJob *)job_in;
  char *msg = NULL; // char msg[1024];
  // sprintf(msg,"%s %d %d %d %d
  // %s",job->tree,job->shot,job->nid,job->on,job->mode,job->server);
  struct descriptor_d fullpath = {0, DTYPE_T, CLASS_D, 0};
  DESCRIPTOR(fullpath_d, "FULLPATH");
  DESCRIPTOR(nullstr, "\0");
  DESCRIPTOR_NID(niddsc, 0);
  char *status_text = MdsGetMsg(job->status);
  status = TreeOpen(job->tree, job->shot, 0);
  if (STATUS_NOT_OK)
    // try to open model instead
    status = TreeOpen(job->tree, -1, 0);
  char now[32];
  Now32(now);
  if (STATUS_OK)
  {
    niddsc.pointer = (char *)&job->nid;
    status = TdiGetNci(&niddsc, &fullpath_d, &fullpath MDS_END_ARG);
    StrAppend(&fullpath, (struct descriptor *)&nullstr);
    msg = malloc(fullpath.length + 1024 + strlen(status_text));
    if (job->server && *job->server)
      sprintf(msg, "%s %d %d %d %d %d %s %d %s %s; %s", job->tree, job->shot,
              job->phase, job->nid, job->on, job->mode, job->server,
              job->status, fullpath.pointer, now, status_text);
    else
      sprintf(msg, "%s %d %d %d %d %d unknown %d %s %s; %s", job->tree,
              job->shot, job->phase, job->nid, job->on, job->mode,
              job->status, fullpath.pointer, now, status_text);
    StrFree1Dx(&fullpath);
  }
  else
  {
    msg = malloc(1024 + strlen(status_text));
    sprintf(msg, "%s %d %d %d %d %d %s %d unknown %s; %s", job->tree, job->shot,
            job->phase, job->nid, job->on, job->mode, job->server, job->status,
            now, status_text);
  }
  status = send_reply(job_in, SrvJobFINISHED, 1, strlen(msg), msg);
  free(msg);
  if (STATUS_NOT_OK)
  {
    if (prev)
      prev->next = m->next;
    else
      Monitors = m->next;
    free(m);
  }
}
// thread
static int SendToMonitors(SrvJob *job)
{
  MonitorList *m, *prev, *next;
  uint32_t prev_addr = job->h.addr;
  uint16_t prev_port = job->h.port;
  for (prev = NULL, m = Monitors; m; m = next)
  {
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
static void DoSrvMonitor(SrvJob *job_in)
{
  INIT_STATUS_ERROR;
  SrvMonitorJob *job = (SrvMonitorJob *)job_in;
  status = (job->mode == MonitorCheckin) ? AddMonitorClient(job_in)
                                         : SendToMonitors(job_in);
  send_reply(job_in,
             (job->mode == MonitorCheckin) ? SrvJobCHECKEDIN : SrvJobFINISHED,
             status, 0, 0);
}
// thread
static void WorkerExit(void *arg __attribute__((unused)))
{
  pthread_mutex_lock(&STATIC_lock);
  STATIC_WorkerDied++;
  if (STATIC_QueueLocked)
    UnlockQueue();
  pthread_mutex_unlock(&STATIC_lock);
  CONDITION_RESET(&WorkerRunning);
  MDSWRN("Worker thread exitted");
}
// thread
static void WorkerThread(void *arg __attribute__((unused)))
{
  SrvJob *job;
  pthread_cleanup_push(WorkerExit, NULL);
  CONDITION_SET(&WorkerRunning);
  while ((job = NextJob(1)))
  {
    MDSDBG("Starting job %d for " IPADDRPRI ":%d", job->h.jobid, IPADDRVAR(&job->h.addr), job->h.port);
    pthread_mutex_lock(&STATIC_lock);
    if (STATIC_Debug)
      fprintf(stderr, "job started.\n");
    ProgLoc = 1;
    pthread_mutex_unlock(&STATIC_lock);
    ServerSetDetailProc(0);
    SetCurrentJob(job);
    if ((job->h.flags & SrvJobBEFORE_NOTIFY) != 0)
    {
      send_reply(job, SrvJobSTARTING, 1, 0, 0);
    }
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
    MDSDBG("Finished job %d for " IPADDRPRI ":%d", job->h.jobid, IPADDRVAR(&job->h.addr), job->h.port);
    SetCurrentJob(NULL);
    FreeJob(job);
    pthread_mutex_lock(&STATIC_lock);
    ProgLoc = 7;
    char *save_text = STATIC_current_job_text;
    STATIC_current_job_text = NULL;
    free(save_text);
    if (STATIC_Debug)
      fprintf(stderr, "job done.\n");
    pthread_mutex_unlock(&STATIC_lock);
  }
  pthread_mutex_lock(&STATIC_lock);
  STATIC_LeftWorkerLoop++;
  ProgLoc = 9;
  pthread_mutex_unlock(&STATIC_lock);
  pthread_cleanup_pop(1);
  pthread_exit(NULL);
}
// main
static int StartWorker()
{
  INIT_STATUS;
  CONDITION_START_THREAD(&WorkerRunning, Worker, / 4, WorkerThread, NULL);
  if (STATUS_NOT_OK)
    exit(-1);
  return status;
}
// main
static void KillWorker()
{
  MDSDBG("enter");
  _CONDITION_LOCK(&WorkerRunning);
  if (WorkerRunning.value)
  {
#ifndef WIN32
    MDSDBG("cancel");
    if (pthread_cancel(Worker))
#endif
    {
      MDSWRN("kill");
      pthread_kill(Worker, SIGINT);
    }
    _CONDITION_WAIT_RESET(&WorkerRunning);
  }
  _CONDITION_UNLOCK(&WorkerRunning);
}

// both
static SOCKET AttachPort(uint32_t addr, uint16_t port)
{
  SOCKET sock;
  struct sockaddr_in sin;
  ClientList *l, *new;
  for (l = Clients; l; l = l->next)
    if (l->addr == addr && l->port == port)
      return l->sock;
  sin.sin_port = htons(port);
  sin.sin_family = AF_INET;
  *(uint32_t *)(&sin.sin_addr) = addr;
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == INVALID_SOCKET)
  {
    MDSERR("Cannot get socket for " IPADDRPRI ":%u", IPADDRVAR(&addr), port);
  }
  else
  {
    if (connect(sock, (struct sockaddr *)&sin, sizeof(sin)) == -1)
    {
      MDSERR("Cannot connect to " IPADDRPRI ":%u", IPADDRVAR(&addr), port);
      shutdown(sock, 2);
      close(sock);
      return INVALID_SOCKET;
    }
    MDSDBG("Connected to " IPADDRPRI ":%u", IPADDRVAR(&addr), port);
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
// both
static void RemoveClient(SrvJob *job)
{
  ClientList *l, *prev;
  for (prev = 0, l = Clients; l;)
  {
    if (job->h.addr == l->addr && job->h.port == l->port)
    {
      shutdown(l->sock, 2);
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

/// returns the number of bytes sent
static int send_all(SOCKET sock, char *msg, int len)
{
  int sent;
  MSG_NOSIGNAL_ALT_PUSH();
  sent = 0;
  do
  {
    const int bytes = send(sock, msg + sent, len - sent, MSG_NOSIGNAL);
    if (bytes <= 0)
    {
      sent = bytes;
      break;
    }
    sent += bytes;
  } while (sent < len);
  MSG_NOSIGNAL_ALT_POP();
  return sent;
}

static int send_reply(SrvJob *job, int replyType, int status_in, int length, char *msg)
{
  MDSDBG(SVRJOB_PRI " %d", SVRJOB_VAR(job), replyType);
  int status;
  status = MDSplusERROR;
  SOCKET sock;
  long msg_len = msg ? (long)strlen(msg) : 0;
  int try_again = FALSE;
  char reply[60];
  memset(reply, 0, 60);
  sprintf(reply, "%d %d %d %ld", job->h.jobid, replyType, status_in, msg_len);
  do
  {
    errno = 0;
    sock = AttachPort(job->h.addr, (uint16_t)job->h.port);
    if (sock == INVALID_SOCKET)
      break;
    int bytes = send_all(sock, reply, 60);
    if (bytes == 60)
    {
      bytes = send_all(sock, msg, length);
      if (bytes == length)
      {
        status = MDSplusSUCCESS;
        break;
      }
    }
    if (STATUS_NOT_OK)
    {
      try_again = errno == EPIPE;
      int debug;
      pthread_mutex_lock(&STATIC_lock);
      debug = STATIC_Debug;
      pthread_mutex_unlock(&STATIC_lock);
      if (debug)
      {
        MDSMSG(SVRJOB_PRI " drop connection", SVRJOB_VAR(job));
      }
      RemoveClient(job);
    }
  } while (try_again--);
  return status;
}
