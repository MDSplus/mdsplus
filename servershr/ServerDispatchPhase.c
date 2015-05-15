/*------------------------------------------------------------------------------

		Name:   SERVER$DISPATCH_PHASE   

		Type:   C function

     		Author:	TOM FREDIAN

		Date:   21-APR-1992

    		Purpose: Dispatch a phase 

------------------------------------------------------------------------------

	Call sequence: 

int SERVER$DISPATCH_PHASE(int efn, DispatchTable *table, struct descriptor *phasenam, char *noact,
                          int *sync, void (*output_rtn)(), struct descriptor *monitor)

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

 	Description:

------------------------------------------------------------------------------*/

#include <STATICdef.h>
#include <mdsdescrip.h>
#include <ipdesc.h>
#include <servershr.h>
#include "servershrp.h"
#include <ncidef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libroutines.h>
#include <strroutines.h>
#include <time.h>
#include <treeshr.h>
#include <mdsshr.h>
#include <servershr.h>
#include <mds_stdarg.h>
#include <tdishr_messages.h>
#include <errno.h>

#ifdef _WIN32
extern int pthread_cond_signal();
extern int pthread_mutex_lock();
extern int pthread_mutex_unlock();
extern int pthread_mutex_init();
extern int pthread_cond_init();
extern int pthread_cond_timedwait();
#define ETIMEDOUT 42
#else
#include <sys/time.h>
#endif

#if (defined(_DECTHREADS_) && (_DECTHREADS_ != 1)) || !defined(_DECTHREADS_)
#define pthread_attr_default NULL
#define pthread_condattr_default NULL
#define pthread_mutexattr_default NULL
#endif

extern int TdiCompletionOf();
extern int TdiExecute();
extern int TdiErrorlogsOf();
extern int TdiGetLong();
extern int ProgLoc;

void SendMonitor(int mode, int idx);

STATIC_ROUTINE void Dispatch(int idx);
STATIC_ROUTINE void DoSendMonitor(int mode, int idx);
STATIC_ROUTINE void ActionDone(int idx);
STATIC_ROUTINE void DoActionDone(int idx);
STATIC_ROUTINE void Before(int idx);
STATIC_ROUTINE void SetActionRanges(int phase, int *first_c, int *last_c);
STATIC_ROUTINE void AbortRange(int s, int e);
STATIC_ROUTINE void SetGroup(int sync, int first_g, int *last_g);
STATIC_ROUTINE int NoOutstandingActions(int s, int e);
STATIC_ROUTINE void RecordStatus(int s, int e);
STATIC_ROUTINE void Dispatch(int i);
STATIC_ROUTINE void WaitForActions(int conditionals, int first_g, int last_g, int first_c,
				   int last_c);
STATIC_CONSTANT const int zero = 0;

typedef struct _complete {
  int idx;
  struct _complete *next;
} Complete;

STATIC_THREADSAFE Complete *CompletedQueueHead = 0;
STATIC_THREADSAFE Complete *CompletedQueueTail = 0;

typedef struct _send_monitor {
  int idx;
  int mode;
  struct _send_monitor *next;
} SendMonitorInfo;

STATIC_THREADSAFE SendMonitorInfo *SendMonitorQueueHead = 0;
STATIC_THREADSAFE SendMonitorInfo *SendMonitorQueueTail = 0;

STATIC_THREADSAFE DispatchTable *table;
STATIC_THREADSAFE int noact;
STATIC_THREADSAFE int AbortInProgress;
STATIC_THREADSAFE void (*Output) ();
STATIC_THREADSAFE int MonitorOn;
STATIC_THREADSAFE char *Monitor;
STATIC_THREADSAFE int first_s;
STATIC_THREADSAFE int last_s;

STATIC_THREADSAFE pthread_cond_t JobWaitCondition;
STATIC_THREADSAFE pthread_mutex_t JobWaitMutex;
STATIC_THREADSAFE pthread_mutex_t wake_send_monitor_mutex;
STATIC_THREADSAFE pthread_cond_t wake_send_monitor_cond;
STATIC_THREADSAFE pthread_mutex_t wake_completed_mutex;
STATIC_THREADSAFE pthread_cond_t wake_completed_cond;
STATIC_THREADSAFE pthread_mutex_t send_monitor_queue_mutex;
STATIC_THREADSAFE pthread_mutex_t completed_queue_mutex;
STATIC_THREADSAFE pthread_mutex_t dispatch_mutex;
STATIC_THREADSAFE pthread_mutex_t send_msg_mutex;

STATIC_ROUTINE char *Server(char *out, char *srv)
{
  int i;
  for (i = 0; i < 32; i++)
    out[i] = srv[i] == ' ' ? 0 : srv[i];
  out[i] = 0;
  return out;
}

void SendMonitor(int mode, int idx)
{
  if (MonitorOn) {
    ActionInfo *actions = table->actions;
    struct descrip p1, p2, p3, p4, p5, p6, p7, p8;
    char tree[13];
    char *cptr;
    int i;
    int on = actions[idx].on;
    char server[33];
    for (i = 0, cptr = table->tree; i < 12; i++)
      if (cptr[i] == (char)32)
	break;
      else
	tree[i] = cptr[i];
    tree[i] = 0;
    for (i = 0, cptr = actions[idx].server; i < 32; i++)
      if (cptr[i] == (char)32)
	break;
      else
	server[i] = cptr[i];
    server[i] = 0;
    pthread_mutex_lock(&send_msg_mutex);
    MonitorOn = ServerSendMessage(0, Monitor, SrvMonitor, 0, 0, 0, 0, 0, 8,
				  MakeDescrip(&p1, DTYPE_CSTRING, 0, 0, tree),
				  MakeDescrip(&p2, DTYPE_LONG, 0, 0, &table->shot),
				  MakeDescrip(&p3, DTYPE_LONG, 0, 0, &actions[idx].phase),
				  MakeDescrip(&p4, DTYPE_LONG, 0, 0, &actions[idx].nid),
				  MakeDescrip(&p5, DTYPE_LONG, 0, 0, &on),
				  MakeDescrip(&p6, DTYPE_LONG, 0, 0, &mode),
				  MakeDescrip(&p7, DTYPE_CSTRING, 0, 0, server),
				  MakeDescrip(&p8, DTYPE_LONG, 0, 0, &actions[idx].status));
    pthread_mutex_unlock(&send_msg_mutex);
  }
}

STATIC_ROUTINE char *now(char *buf)
{
  time_t tim = time(0);
  tim = time(0);
#ifdef _WIN32
  return ctime(&tim);
#else
  ctime_r(&tim, buf);
  buf[strlen(buf) - 1] = 0;
  return buf;
#endif
}

STATIC_ROUTINE void ActionDone(int idx)
{
  int i;
  char logmsg[1024];
  if (idx >= 0) {
    ActionInfo *actions = table->actions;
    if (actions[idx].event) {
      pthread_mutex_lock(&send_msg_mutex);
      MDSEvent(actions[idx].event, sizeof(int), (char *)&table->shot);
      pthread_mutex_unlock(&send_msg_mutex);
    }
    DoSendMonitor(MonitorDone, idx);
    if (Output) {
      char buf[30];
      if (actions[idx].status & 1)
	sprintf(logmsg, "%s, Action %s completed", now(buf), actions[idx].path);
      else {
	char *emsg = MdsGetMsg(actions[idx].status);
	sprintf(logmsg, "%s, Action %s failed, %s", now(buf), actions[idx].path, emsg);
      }
      (*Output) (logmsg);
    }
    if (!AbortInProgress) {
      EMPTYXD(xd);
      char expression[60];
      struct descriptor expression_d = { 0, DTYPE_T, CLASS_S, 0 };
      expression_d.pointer = expression;
      expression_d.length =
	  sprintf(expression, "PUBLIC _ACTION_%08X = %d", actions[idx].nid, actions[idx].status);
      TdiExecute(&expression_d, &xd MDS_END_ARG);
      MdsFree1Dx(&xd, NULL);
      for (i = 0; i < actions[idx].num_references; i++) {
	int dstat;
	int doit;
	int cidx = actions[idx].referenced_by[i];
	if (!actions[cidx].done) {
	  if ((dstat = TdiGetLong(actions[cidx].condition, &doit)) & 1) {
	    if (doit) {
	      Dispatch(cidx);
	    } else {
	      actions[cidx].status = ServerNOT_DISPATCHED;
	      DoActionDone(cidx);
	    }
	  } else if (dstat != TdiUNKNOWN_VAR) {
	    actions[cidx].status = ServerINVALID_DEPENDENCY;
	    DoActionDone(cidx);
	  }
	}
      }
    }
    actions[idx].done = 1;
    actions[idx].recorded = 0;
  }
  pthread_mutex_lock(&JobWaitMutex);
  pthread_cond_signal(&JobWaitCondition);
  pthread_mutex_unlock(&JobWaitMutex);
  return;
}

STATIC_ROUTINE void Before(int idx)
{
  ActionInfo *actions = table->actions;
  char logmsg[1024];
  actions[idx].doing = 1;
  DoSendMonitor(MonitorDoing, idx);
  if (Output) {
    char server[33];
    char buf[30];
    sprintf(logmsg, "%s, %s is beginning action %s", now(buf), Server(server, actions[idx].server),
	    actions[idx].path);
    (*Output) (logmsg);
  }
  return;
}

STATIC_ROUTINE void SetActionRanges(int phase, int *first_c, int *last_c)
{
  int i;
  ActionInfo *actions = table->actions;
  for (i = 0; (i < table->num) && (actions[i].phase != phase); i++) ;
  if (i < table->num) {
    if (actions[i].condition) {
      *first_c = i;
      for (i = *first_c + 1;
	   (i < table->num) && (actions[i].phase == phase) && (actions[i].condition); i++) ;
      *last_c = i;
    } else
      *first_c = *last_c = table->num;
    if (i < table->num && (actions[i].phase == phase)) {
      first_s = i;
      for (i = first_s + 1; (i < table->num) && (actions[i].phase == phase); i++) ;
      last_s = i;
    } else
      first_s = last_s = table->num;
  } else {
    *first_c = table->num;
    *last_c = table->num;
    first_s = table->num;
    last_s = table->num;
  }
}

STATIC_ROUTINE void AbortRange(int s, int e)
{
  int i;
  ActionInfo *actions = table->actions;
  for (i = s; i < e; i++) {
    if (actions[i].dispatched && !actions[i].done) {
      char server[33];
      int one = 1;
      ServerAbortServer(Server(server, actions[i].server), one);
    }
  }
}

STATIC_ROUTINE void SetGroup(int sync, int first_g, int *last_g)
{
  int i;
  int group;
  ActionInfo *actions = table->actions;
  sync = (sync < 1) ? 1 : sync;
  group = actions[first_g].sequence / sync;
  for (i = first_g + 1; i < last_s && ((actions[i].sequence / sync) == group); i++) ;
  *last_g = i;
}

STATIC_ROUTINE int NoOutstandingActions(int s, int e)
{
  int i;
  ActionInfo *actions = table->actions;
  for (i = s; i < e; i++) {
    if (actions[i].dispatched && !actions[i].done)
      return 0;
  }
  return 1;
}

STATIC_ROUTINE void RecordStatus(int s, int e)
{
  int i;
  ActionInfo *actions = table->actions;
  for (i = s; i < e; i++) {
    if (actions[i].done && !actions[i].recorded) {
      NCI_ITM setnci[] = { {sizeof(actions[0].status), NciSTATUS, 0, 0}
      , {0, NciEND_OF_LIST, 0, 0}
      };
      setnci[0].pointer = (unsigned char *)&actions[i].status;
      pthread_mutex_lock(&send_msg_mutex);
      TreeSetNci(actions[i].nid, setnci);
      actions[i].recorded = 1;
      if (!(actions[i].status & 1) && (!table->failed_essential)) {
	int flags;
	NCI_ITM getnci[] = { {sizeof(flags), NciGET_FLAGS, 0, 0}
	, {0, NciEND_OF_LIST, 0, 0}
	};
	getnci[0].pointer = &flags;
	TreeGetNci(actions[i].nid, getnci);
	table->failed_essential = (flags & NciM_ESSENTIAL) != 0;
      }
      pthread_mutex_unlock(&send_msg_mutex);
    }
  }
}

STATIC_ROUTINE void WaitForActions(int all, int first_g, int last_g, int first_c, int last_c)
{
  int status = 0;
  while ((status == ETIMEDOUT || status == 0)
	 && !(all ? NoOutstandingActions(first_g, last_g) && NoOutstandingActions(first_c, last_c)
	      : (AbortInProgress ? 1 : NoOutstandingActions(first_g, last_g)))) {
    ProgLoc = 600;
    pthread_mutex_lock(&JobWaitMutex);
#ifdef _WIN32
    status = pthread_cond_timedwait(&JobWaitCondition, &JobWaitMutex, 1000);
#else
    {
      struct timespec abstime;
      struct timeval tmval;

      gettimeofday(&tmval, 0);
      abstime.tv_sec = tmval.tv_sec + 1;
      abstime.tv_nsec = tmval.tv_usec * 1000;

      ProgLoc = 601;
      status = pthread_cond_timedwait(&JobWaitCondition, &JobWaitMutex, &abstime);
      ProgLoc = 602;
    }
#if defined(_DECTHREADS_) && (_DECTHREADS_ == 1)
    if (status == -1 && errno == 11)
      status = ETIMEDOUT;
#endif
#endif
    pthread_mutex_unlock(&JobWaitMutex);
    ProgLoc = 603;
  }
}

STATIC_ROUTINE char *DetailProc(int full)
{
  int i;
  int first = 1;
  int doing;
  char msg1[1024];
  char *msg = 0;
  unsigned int msglen;
  ActionInfo *actions = table->actions;
  int num = table->num;
  for (doing = 1; doing > (full ? -1 : 0); doing--) {
    for (i = 0; i < num; i++) {
      if (actions[i].dispatched && !actions[i].done && ((int)actions[i].doing == doing)) {
	char server[33];
	if (first) {
	  msglen = 4096;
	  msg = (char *)malloc(4096);
	  strcpy(msg, "\nWaiting on:\n");
	  first = 0;
	}
	sprintf(msg1, "	%s %s %s for shot %d\n", actions[i].path,
		actions[i].doing ? "in progress on" : "dispatched to", Server(server,
									      actions[i].server),
		table->shot);
	if (msglen < (strlen(msg) + strlen(msg1) + 1)) {
	  char *oldmsg = msg;
	  msg = (char *)malloc(msglen + 4096);
	  msglen += 4096;
	  strcpy(msg, oldmsg);
	  free(oldmsg);
	}
	strcat(msg, msg1);
      }
    }
  }
  return msg;
}

int ServerDispatchPhase(int *id, void *vtable, char *phasenam, char noact_in,
			int sync, void (*output_rtn) (), char *monitor)
{
  int i;
  int status;
  int phase;
  int first_g, last_g = 0, first_c, last_c;
  DESCRIPTOR_LONG(phase_d, 0);
  STATIC_CONSTANT DESCRIPTOR(phase_lookup, "PHASE_NUMBER_LOOKUP($)");
  struct descriptor phasenam_d = { 0, DTYPE_T, CLASS_S, 0 };
  STATIC_THREADSAFE int initialized = 0;
  ActionInfo *actions;
  phase_d.pointer = (char *)&phase;
  table = vtable;
  Output = output_rtn;
  actions = table->actions;
  noact = noact_in;
  AbortInProgress = 0;
  table->failed_essential = 0;
  phasenam_d.length = strlen(phasenam);
  phasenam_d.pointer = phasenam;
  ProgLoc = 6001;
  if (initialized == 0) {
    pthread_mutex_init(&wake_send_monitor_mutex, pthread_mutexattr_default);
    pthread_cond_init(&wake_send_monitor_cond, pthread_condattr_default);
    pthread_mutex_init(&wake_completed_mutex, pthread_mutexattr_default);
    pthread_cond_init(&wake_completed_cond, pthread_condattr_default);
    pthread_mutex_init(&send_monitor_queue_mutex, pthread_mutexattr_default);
    pthread_mutex_init(&completed_queue_mutex, pthread_mutexattr_default);
    pthread_mutex_init(&dispatch_mutex, pthread_mutexattr_default);
    pthread_mutex_init(&send_msg_mutex, pthread_mutexattr_default);
    ProgLoc = 6002;
    status = pthread_mutex_init(&JobWaitMutex, pthread_mutexattr_default);
    if (status) {
      perror("Error creating pthread mutex");
      exit(status);
    }
    ProgLoc = 6003;
    status = pthread_cond_init(&JobWaitCondition, pthread_condattr_default);
    if (status) {
      perror("Error creating pthread condition");
      exit(status);
    }
    ProgLoc = 6004;
    initialized = 1;
  }
  ProgLoc = 6005;
  status = TdiExecute(&phase_lookup, &phasenam_d, &phase_d MDS_END_ARG);
  ProgLoc = 6006;
  if (status & 1 && (phase > 0)) {
    if (monitor) {
      MonitorOn = 1;
      Monitor = monitor;
    } else
      MonitorOn = 0;
    ProgLoc = 6007;
    SetActionRanges(phase, &first_c, &last_c);
    ProgLoc = 6008;
    ServerSetDetailProc(DetailProc);
    ProgLoc = 6009;
    first_g = first_s;
    while (!AbortInProgress && (first_g < last_s)) {
      ProgLoc = 6010;
      SetGroup(sync, first_g, &last_g);
      ProgLoc = 6011;
      for (i = first_g; i < last_g; i++)
	Dispatch(i);
      ProgLoc = 6012;
      WaitForActions(0, first_g, last_g, first_c, last_c);
      first_g = last_g;
    }
    ProgLoc = 6013;
    if (AbortInProgress) {
      AbortInProgress = 0;
      AbortRange(first_c, last_c);
      AbortRange(first_s, last_s);
      status = ServerABORT;
    }
    ProgLoc = 6014;
    WaitForActions(1, first_g, last_g, first_c, last_c);
    ProgLoc = 6015;
    AbortInProgress = 1;
    for (i = first_c; i < last_c; i++) {
      if (!actions[i].done) {
	actions[i].status = ServerCANT_HAPPEN;
	DoActionDone(i);
      }
    }
    ProgLoc = 6015;
    WaitForActions(1, first_g, last_g, first_c, last_c);
    ProgLoc = 6016;
    if (!noact)
      RecordStatus(first_c, last_c);
    ProgLoc = 6017;
    if (!noact)
      RecordStatus(first_s, last_s);
    ProgLoc = 6018;
  }
  ProgLoc = 6019;
  return status;
}

int ServerFailedEssential(void *vtable, int reset)
{
  DispatchTable *table = vtable;
  int failed = table ? table->failed_essential : 0;
  if (table && reset)
    table->failed_essential = 0;
  return failed;
}

STATIC_ROUTINE void Dispatch(int i)
{
  int status;
  char logmsg[1024];
  char server[33];
  ActionInfo *actions = table->actions;
  pthread_mutex_lock(&dispatch_mutex);
  {
    actions[i].done = 0;
    actions[i].doing = 0;
    actions[i].dispatched = 0;
    if (Output) {
      char buf[30];
      sprintf(logmsg, "%s, Dispatching node %s to %s", now(buf), actions[i].path,
	      Server(server, actions[i].server));
      (*Output) (logmsg);
    }
    ProgLoc = 7001;
    DoSendMonitor(MonitorDispatched, i);
    ProgLoc = 7002;
    if (noact) {
      actions[i].dispatched = 1;
      actions[i].status = status = 1;
      DoActionDone(i);
    } else {
      pthread_mutex_lock(&send_msg_mutex);
      status = ServerDispatchAction(0, Server(server, actions[i].server), table->tree, table->shot,
				    actions[i].nid, DoActionDone, i + (char *)0, &actions[i].status,
				    &actions[i].netid, Before);
      pthread_mutex_unlock(&send_msg_mutex);
      ProgLoc = 7003;
      if (status & 1)
	actions[i].dispatched = 1;
    }
    ProgLoc = 7004;
    if (!(status & 1)) {
      actions[i].status = status;
      DoActionDone(i);
    }
    ProgLoc = 7005;
  }
  pthread_mutex_unlock(&dispatch_mutex);
}

STATIC_THREADSAFE int ProcessActionDoneRunning = 0;

STATIC_ROUTINE void WakeCompletedActionQueue()
{
  pthread_mutex_lock(&wake_completed_mutex);
  pthread_cond_signal(&wake_completed_cond);
  pthread_mutex_unlock(&wake_completed_mutex);
}

STATIC_ROUTINE void WaitForActionDoneQueue()
{
  pthread_mutex_lock(&wake_completed_mutex);
#ifdef _WIN32
  pthread_cond_timedwait(&wake_completed_cond, &wake_completed_mutex, 1000);
#else
  {
    struct timespec abstime;
    struct timeval tmval;
    gettimeofday(&tmval, 0);
    abstime.tv_sec = tmval.tv_sec + 1;
    abstime.tv_nsec = tmval.tv_usec * 1000;
    pthread_cond_timedwait(&wake_completed_cond, &wake_completed_mutex, &abstime);
  }
#endif
  pthread_mutex_unlock(&wake_completed_mutex);
}

STATIC_ROUTINE void QueueCompletedAction(int i)
{
  Complete *c = malloc(sizeof(Complete));
  c->idx = i;
  c->next = 0;
  pthread_mutex_lock(&completed_queue_mutex);
  if (CompletedQueueTail)
    CompletedQueueTail->next = c;
  else
    CompletedQueueHead = c;
  CompletedQueueTail = c;
  pthread_mutex_unlock(&completed_queue_mutex);
  WakeCompletedActionQueue();
}

STATIC_ROUTINE int DequeueCompletedAction(int *i)
{
  int doneAction = -1;
  while (doneAction == -1) {
    pthread_mutex_lock(&completed_queue_mutex);
    if (CompletedQueueHead) {
      Complete *c = CompletedQueueHead;
      doneAction = CompletedQueueHead->idx;
      CompletedQueueHead = CompletedQueueHead->next;
      if (!CompletedQueueHead)
	CompletedQueueTail = 0;
      free(c);
      pthread_mutex_unlock(&completed_queue_mutex);
    } else {
      pthread_mutex_unlock(&completed_queue_mutex);
      WaitForActionDoneQueue();
    }
  }
  *i = doneAction;
  return 1;
}

STATIC_ROUTINE void ActionDoneThreadExit()
{
  ProcessActionDoneRunning = 0;
}

STATIC_ROUTINE void ProcessActionDone()
{
  int i;
  pthread_cleanup_push(ActionDoneThreadExit, 0);
  ProcessActionDoneRunning = 1;
  while (DequeueCompletedAction(&i))
    ActionDone(i);
  pthread_cleanup_pop(1);
}

STATIC_ROUTINE void StartActionDoneThread()
{
  pthread_t thread;
#ifndef _WIN32
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_create(&thread, &attr, (void *)(void *)ProcessActionDone, (void *)0);
  pthread_attr_destroy(&attr);
#else
  pthread_create(&thread, pthread_attr_default, (void *)(void *)ProcessActionDone, (void *)0);
#endif
}

STATIC_ROUTINE void DoActionDone(int i)
{
  QueueCompletedAction(i); /***** must be done before starting thread ****/
  if (!ProcessActionDoneRunning)
    StartActionDoneThread();
  return;
}

STATIC_THREADSAFE int SendMonitorRunning = 0;

STATIC_ROUTINE void WakeSendMonitorQueue()
{
  pthread_mutex_lock(&wake_send_monitor_mutex);
  pthread_cond_signal(&wake_send_monitor_cond);
  pthread_mutex_unlock(&wake_send_monitor_mutex);
}

STATIC_ROUTINE void WaitForSendMonitorQueue()
{
  pthread_mutex_lock(&wake_send_monitor_mutex);
#ifdef _WIN32
  pthread_cond_timedwait(&wake_send_monitor_cond, &wake_send_monitor_mutex, 1000);
#else
  {
    struct timespec abstime;
    struct timeval tmval;

    gettimeofday(&tmval, 0);
    abstime.tv_sec = tmval.tv_sec + 1;
    abstime.tv_nsec = tmval.tv_usec * 1000;
    pthread_cond_timedwait(&wake_send_monitor_cond, &wake_send_monitor_mutex, &abstime);
  }
#endif
  pthread_mutex_unlock(&wake_send_monitor_mutex);
}

STATIC_ROUTINE void QueueSendMonitor(int mode, int i)
{
  SendMonitorInfo *c = malloc(sizeof(SendMonitorInfo));
  c->idx = i;
  c->mode = mode;
  c->next = 0;
  pthread_mutex_lock(&send_monitor_queue_mutex);
  if (SendMonitorQueueTail)
    SendMonitorQueueTail->next = c;
  else
    SendMonitorQueueHead = c;
  SendMonitorQueueTail = c;
  pthread_mutex_unlock(&send_monitor_queue_mutex);
  WakeSendMonitorQueue();
}

STATIC_ROUTINE int DequeueSendMonitor(int *mode_out, int *i)
{
  int idx = -1;
  int mode;
  while (idx == -1) {
    pthread_mutex_lock(&send_monitor_queue_mutex);
    if (SendMonitorQueueHead) {
      SendMonitorInfo *c = SendMonitorQueueHead;
      idx = SendMonitorQueueHead->idx;
      mode = SendMonitorQueueHead->mode;
      SendMonitorQueueHead = SendMonitorQueueHead->next;
      if (!SendMonitorQueueHead)
	SendMonitorQueueTail = 0;
      free(c);
      pthread_mutex_unlock(&send_monitor_queue_mutex);
    } else {
      pthread_mutex_unlock(&send_monitor_queue_mutex);
      WaitForSendMonitorQueue();
    }
  }
  *i = idx;
  *mode_out = mode;
  return 1;
}

STATIC_ROUTINE void SendMonitorThreadExit()
{
  SendMonitorRunning = 0;
}

STATIC_ROUTINE void SendMonitorThread()
{
  int i;
  int mode;
  pthread_cleanup_push(SendMonitorThreadExit, 0);
  SendMonitorRunning = 1;
  while (DequeueSendMonitor(&mode, &i))
    SendMonitor(mode, i);
  pthread_cleanup_pop(1);
}

STATIC_ROUTINE void StartSendMonitorThread()
{
  pthread_t thread;
#ifndef _WIN32
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_create(&thread, &attr, (void *)(void *)SendMonitorThread, (void *)0);
  pthread_attr_destroy(&attr);
#else
  pthread_create(&thread, pthread_attr_default, (void *)(void *)SendMonitorThread, (void *)0);
#endif
}

STATIC_ROUTINE void DoSendMonitor(int mode, int idx)
{
  QueueSendMonitor(mode, idx);/***** must be done before starting thread ****/
  if (!SendMonitorRunning)
    StartSendMonitorThread();
  return;
}
