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
#include <tdimessages.h>
#include <errno.h>

#ifdef HAVE_WINDOWS_H
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
#define pthread_condattr_default NULL
#define pthread_mutexattr_default NULL
#endif

extern int TdiCompletionOf();
extern int TdiExecute();
extern int TdiErrorlogsOf();
extern int TdiGetLong();
extern int ProgLoc;

static void DoDispatch(int idx);
static void Dispatch(int idx);
void SendMonitor(int mode, int idx);
static void SendReply(int nid, char *message);
static void ActionDone(int idx);
static void DoActionDone(int idx);
static void Before(int idx);
static int AbortHandler( unsigned sig_args[], unsigned mech_args[]);
static void SetActionRanges(int phase);
static void AbortRange(int s,int e);
static void SetGroup(int sync);
static int NoOutstandingActions(int s,int e);
static void RecordStatus(int s,int e);
static void Dispatch(int i);
static void WaitForActions(int conditionals);
static void lock_action_done();
static void unlock_action_done();
static void lock_dispatch();
static void unlock_dispatch();

static const int zero = 0;


static ActionInfo *actions;
static char *tree;
static int shot;
static int num_actions;
static int noact;
static int AbortInProgress;
static DispatchTable *table;
static void (*Output)();
static int MonitorOn;
static char *Monitor;
static int first_s;
static int last_s;
static int first_c;
static int last_c;
static int first_g;
static int last_g;
static int WaitForAll;
static pthread_cond_t JobWaitCondition;
static pthread_mutex_t JobWaitMutex;

static char *Server(char *out, char *srv)
{
	int i;
	for (i=0;i<32;i++) out[i] = srv[i] == ' ' ? 0 : srv[i];
	out[i] = 0;
	return out;
}

void SendMonitor(int mode, int idx)
{
  if (MonitorOn)
  {
    struct descrip p1,p2,p3,p4,p5,p6,p7,p8;
    char tree[13];
    char *cptr;
    int i;
    int on = actions[idx].on;
    char server[33];
    for (i=0, cptr = table->tree;i<12;i++) 
    if (cptr[i] == (char)32) 
      break;
    else
      tree[i] = cptr[i];
    tree[i] = 0;
    for (i=0, cptr = actions[idx].server;i<32;i++) 
      if (cptr[i] == (char)32) 
        break;
      else
        server[i] = cptr[i];
    server[i] = 0;
    MonitorOn = ServerSendMessage(0, Monitor, SrvMonitor, 0, 0, 0, 0, 0, 8, 
                      MakeDescrip(&p1,DTYPE_CSTRING,0,0,tree), 
                      MakeDescrip(&p2,DTYPE_LONG,0,0,&table->shot),
                      MakeDescrip(&p3,DTYPE_LONG,0,0,&actions[idx].phase),
                      MakeDescrip(&p4,DTYPE_LONG,0,0,&actions[idx].nid),
                      MakeDescrip(&p5,DTYPE_LONG,0,0,&on),
                      MakeDescrip(&p6,DTYPE_LONG,0,0,&mode),
                      MakeDescrip(&p7,DTYPE_CSTRING,0,0,server),
                      MakeDescrip(&p8,DTYPE_LONG,0,0,&actions[idx].status));
  }
}

static void SendReply(int nid, char *message)
{
/*
  static struct descriptor_d users = {0,DTYPE_T,CLASS_D,0};
  static struct descriptor_d user = {0,DTYPE_T,CLASS_D,0};
  static struct descriptor_d msg_with_bells = {0, DTYPE_T, CLASS_D, 0};
  static DESCRIPTOR(bells,"\007\007");
  static DESCRIPTOR(comma,",");
  static DESCRIPTOR_NID(nid_dsc,0);
  nid_dsc.pointer=(char *)nid;
  if (TdiErrorlogsOf(&nid_dsc,&users) & 1)
  {
    int idx = 0;
    Str$concat(&msg_with_bells,&bells,message);
    while (str$element(&user,&idx,&comma,&users) & 1)
    {
      idx++;
      sys$brkthru(0,&msg_with_bells,&user,BRK$C_USERNAME,0,0,BRK$M_CLUSTER,BRK$C_USER1,5,0,0);
    }
  }
*/
}

static char *now()
{
  static char now[64];
  time_t tim;
  tim = time(0);
  strcpy(now,ctime(&tim));
  now[strlen(now)-1]=0;
  return now;
}

static void ActionDone(int idx)
{
  int i;
  static struct descriptor event_name = {0, DTYPE_T, CLASS_D, 0};
  struct descriptor niddsc = {4, DTYPE_NID, CLASS_S, 0};
  char *path;
  char logmsg[1024];
  static char expression[60];
  static struct descriptor expression_d = {0, DTYPE_T, CLASS_S, expression};
  static EMPTYXD(xd);
  lock_action_done();
  if (idx >= 0)
  {
    niddsc.pointer = (char *)&actions[idx].nid;
    if (TdiCompletionOf(&niddsc,&event_name MDS_END_ARG) & 1 && event_name.length)
    {
      char *event = strncpy((char *)malloc(event_name.length+1),event_name.pointer,event_name.length);
      event[event_name.length] = 0;
      MDSEvent(event,sizeof(int),(char *)&table->shot);
      free(event);
    }
    SendMonitor(MonitorDone, idx);
    path = TreeGetMinimumPath((int *)&zero,actions[idx].nid);
    if (actions[idx].status & 1)
      sprintf(logmsg,"%s, Action %s completed",now(),path);
    else
    {
      char *emsg = MdsGetMsg(actions[idx].status);
      sprintf(logmsg,"%s, Action %s failed, %s",now(),path,emsg);
      SendReply(actions[idx].nid,logmsg);
    }
    TreeFree(path);
    if (Output)
      (*Output)(logmsg);
    if (!AbortInProgress)
    {
      expression_d.length = sprintf(expression,"_ACTION_%08X = %d",actions[idx].nid,actions[idx].status);
      TdiExecute(&expression_d,&xd MDS_END_ARG);
      for (i=0;i<actions[idx].num_references;i++)
      {
        int dstat;
        int doit;
        int cidx = actions[idx].referenced_by[i];
        if (!actions[cidx].done)
        {
          if ((dstat = TdiGetLong(actions[cidx].condition,&doit)) & 1)
          {
            if (doit)
	    {
              Dispatch(cidx);
            }
            else
            {
              actions[cidx].status = ServerNOT_DISPATCHED;
   	      DoActionDone(cidx);
            }
          }
          else if (dstat != TdiUNKNOWN_VAR)
          {
            actions[cidx].status = ServerINVALID_DEPENDENCY;
            DoActionDone(cidx);
          }
        }
      }
    }
    actions[idx].done = 1;
    actions[idx].recorded = 0;
  }
  if ( WaitForAll ? NoOutstandingActions(first_g,last_g) && NoOutstandingActions(first_c,last_c)
                    : (AbortInProgress ? 1 : NoOutstandingActions(first_g,last_g)) )
  {
    pthread_mutex_lock(&JobWaitMutex);
    pthread_cond_signal(&JobWaitCondition);
    pthread_mutex_unlock(&JobWaitMutex);
  }
  unlock_action_done();
  return;
}

static void Before(int idx)
{
  char logmsg[1024];
  actions[idx].doing = 1;
  SendMonitor(MonitorDoing, idx);
  if (Output)
  {
    char server[33];
    static DESCRIPTOR(fao_s, "!%T !AS is beginning action !AS");
    char *path;
    path = TreeGetMinimumPath((int *)&zero,actions[idx].nid);
	sprintf(logmsg,"%s, %s is beginning action %s",now(),Server(server,actions[idx].server),path);
	TreeFree(path);
    (*Output)(logmsg);
  }
  return;
}

static void SetActionRanges(int phase)
{
  int i;
  for (i=0; (i < num_actions) && (actions[i].phase != phase);i++);
  if (i < num_actions)
  {
    if (actions[i].condition)
    {
      first_c = i;
      for (i=first_c+1; (i < num_actions) && (actions[i].phase == phase) && (actions[i].condition);i++);
      last_c = i;
    }
    else
      first_c = last_c = num_actions;
    if (i < num_actions && (actions[i].phase == phase))
    {
      first_s = i;
      for (i=first_s+1; (i < num_actions) && (actions[i].phase == phase);i++);
      last_s = i;
    }
    else
      first_s = last_s = num_actions;
  }
  else
  {
    first_c = num_actions;
    last_c = num_actions;
    first_s = num_actions;
    last_s = num_actions;
  }
}
      
static void AbortRange(int s,int e)
{
  int i;
  for (i=s;i<e;i++)
  {
    if (actions[i].dispatched && !actions[i].done)
    {
      char server[33];
      int one = 1;
      ServerAbortServer(Server(server,actions[i].server),one);
    }
  }
}

static void SetGroup(int sync)
{
  int i;
  int group;
  sync = (sync < 1) ? 1 : sync;
  group = actions[first_g].sequence / sync;
  for (i=first_g+1;i<last_s && ((actions[i].sequence/ sync) == group); i++);
  last_g = i;
}

static int NoOutstandingActions(int s,int e)
{
  int i;
  for (i=s;i<e;i++)
  {
    if (actions[i].dispatched && !actions[i].done)
      return 0;
  }
  return 1;
}

static void RecordStatus(int s,int e)
{
  int i;
  for (i=s;i<e;i++)
  {
    if (actions[i].done && !actions[i].recorded)
    {
      static NCI_ITM setnci[] = {{sizeof(actions[0].status), NciSTATUS, 0, 0},{0,NciEND_OF_LIST,0,0}};
      setnci[0].pointer = (unsigned char *)&actions[i].status;
      TreeSetNci(actions[i].nid,setnci);
      actions[i].recorded = 1;
      if (!(actions[i].status & 1) && (!table->failed_essential))
      {
        static int flags;
        static NCI_ITM getnci[] = {{sizeof(flags), NciGET_FLAGS, (unsigned char *)&flags, 0},{0,NciEND_OF_LIST,0,0}};
        TreeGetNci(actions[i].nid,getnci);
        table->failed_essential =  (flags & NciM_ESSENTIAL) != 0;
      }
    }
  }
}

static void WaitForActions(int all)
{
  int status = ETIMEDOUT;
  WaitForAll = all;
  while ((status == ETIMEDOUT) && !(WaitForAll ? NoOutstandingActions(first_g,last_g) && NoOutstandingActions(first_c,last_c)
                    : (AbortInProgress ? 1 : NoOutstandingActions(first_g,last_g)) ))
  {
    ProgLoc = 600;
    pthread_mutex_lock(&JobWaitMutex);
#ifdef HAVE_WINDOWS_H
	status = pthread_cond_timedwait(&JobWaitCondition, &JobWaitMutex, 1000);
#else
	{
      struct timespec one_sec = {1,0};
      struct timespec abstime;
      struct timeval tmval;
      /*
      pthread_get_expiration_np(&one_sec,&abstime);
      */
      
      gettimeofday(&tmval, 0);
      abstime.tv_sec = tmval.tv_sec + 1;
      abstime.tv_nsec = tmval.tv_usec * 1000;
      

      ProgLoc = 601;
      status = pthread_cond_timedwait( &JobWaitCondition, &JobWaitMutex, &abstime);
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

static char *DetailProc(int full)
{
  int i;
  int first=1;
  int doing;
  char msg1[1024];
  char *msg = 0;
  unsigned int msglen;
  for (doing = 1; doing > (full ? -1 : 0); doing--)
  {
    for (i=first_s;i<last_s;i++)
    {
      if (actions[i].dispatched && !actions[i].done && ((int)actions[i].doing == doing))
      {
        char *path;
        char server[33];
        path = TreeGetMinimumPath((int *)&zero,actions[i].nid);
        if (first)
        {
          msglen = 4096;
          msg = (char *)malloc(4096);
          strcpy(msg,"\nWaiting on:\n");
          first = 0;
        }
        sprintf(msg1,"	%s %s %s for shot %d\n",path,actions[i].doing ? "in progress on" : "dispatched to",
             Server(server,actions[i].server),table->shot);
        TreeFree(path);
        if (msglen < (strlen(msg) + strlen(msg1) + 1))
        {
          char *oldmsg = msg;
          msg = (char *)malloc(msglen + 4096);
          msglen += 4096;
          strcpy(msg,oldmsg);
          free(oldmsg);
        }
        strcat(msg,msg1);
      }
    }
  }
  return msg;
}

int ServerDispatchPhase(int *id, void *vtable, char *phasenam, char noact,
                          int sync, void (*output_rtn)(), char *monitor)
{ 
  int i;
  int status;
  static int phase;
  static DESCRIPTOR_LONG(phase_d,&phase);
  static DESCRIPTOR(phase_lookup,"PHASE_NUMBER_LOOKUP($)");
  struct descriptor phasenam_d = {0, DTYPE_T, CLASS_S, 0};
  static int JobWaitInitialized = 0;
  table = vtable;
  tree = table->tree;
  shot = table->shot;
  Output = output_rtn;
  actions = table->actions;
  num_actions = table->num;
  AbortInProgress = 0;
  WaitForAll = 0;
  table->failed_essential = 0;
  phasenam_d.length = strlen(phasenam);
  phasenam_d.pointer = phasenam;
  ProgLoc = 6001;
  MDSEvent("__DISPATCH__",0,0); /********************* problem with dlopen in threads on linux, force mdsipshr activation ******/
  if (JobWaitInitialized == 0)
  {
    ProgLoc = 6002;
    status = pthread_mutex_init(&JobWaitMutex,pthread_mutexattr_default);
    if (status)
    {
      perror("Error creating pthread mutex");
      exit(status);
    }
    ProgLoc = 6003;
    status = pthread_cond_init(&JobWaitCondition,pthread_condattr_default);
    if (status)
    {
      perror("Error creating pthread condition");
      exit(status);
    }
    ProgLoc = 6004;
    JobWaitInitialized = 1;
  }
  ProgLoc = 6005;
  status = TdiExecute(&phase_lookup,&phasenam_d,&phase_d MDS_END_ARG);
  ProgLoc = 6006;
  if (status & 1 && (phase > 0))
  {
    if (monitor)
    {
      MonitorOn = 1;
      Monitor = monitor;
    }
    else
      MonitorOn = 0;
    ProgLoc = 6007;
    SetActionRanges(phase);
    ProgLoc = 6008;
    ServerSetDetailProc(DetailProc);
    ProgLoc = 6009;
    first_g = first_s;
    while (!AbortInProgress && (first_g < last_s))
    {
      ProgLoc = 6010;
      SetGroup(sync);
      ProgLoc = 6011;
      lock_action_done();
      for (i=first_g;i<last_g;i++)
         Dispatch(i);
      unlock_action_done();
      ProgLoc = 6012;
      WaitForActions(0);
      first_g = last_g;
    }
    ProgLoc = 6013;
    if (AbortInProgress)
    {
      AbortInProgress = 0;
      AbortRange(first_c,last_c);
      AbortRange(first_s,last_s);
      status = ServerABORT;
    }
    ProgLoc = 6014;
    WaitForActions(1);
    ProgLoc = 6015;
    AbortInProgress = 1;
    for (i=first_c;i<last_c;i++)
    {
      if (!actions[i].done)
      {
        actions[i].status = ServerCANT_HAPPEN;
	DoActionDone(i);
      }
    }
    ProgLoc = 6015;
    WaitForActions(1);
    ProgLoc = 6016;
    RecordStatus(first_c,last_c);
    ProgLoc = 6017;
    RecordStatus(first_s,last_s);
    ProgLoc = 6018;
  }
  ProgLoc = 6019;
  return status;
}

int ServerFailedEssential(void *vtable,int reset)
{
  DispatchTable *table = vtable;
  int failed = table ? table->failed_essential : 0;
  if (table && reset)
    table->failed_essential = 0;
  return failed;
}

static void Dispatch(int i)
{
  int status;
  char logmsg[1024];
  char server[33];
  lock_dispatch();
  {
    actions[i].done = 0;
    actions[i].doing = 0;
    actions[i].dispatched = 0;
    if (Output)
    {
      char *path;
      path = TreeGetMinimumPath((int *)&zero,actions[i].nid);
      sprintf(logmsg,"%s, Dispatching node %s to %s",now(),path,Server(server,actions[i].server));
      TreeFree(path);
      (*Output)(logmsg);
    }
    ProgLoc = 7001;
    SendMonitor(MonitorDispatched, i);
    ProgLoc = 7002;
    if (noact)
      {
	actions[i].status = status = 1;
	DoActionDone(i);
      }
    else
      {
	status = ServerDispatchAction(0, Server(server,actions[i].server), tree, shot, actions[i].nid, DoActionDone, (void *)i, &actions[i].status, 
				      &actions[i].netid, Before);
	ProgLoc = 7003;
	if (status & 1)
	  actions[i].dispatched = 1;
      }
    ProgLoc = 7004;
    if (!(status & 1))
      {
	actions[i].status = status;
	DoActionDone(i);
      }
    ProgLoc = 7005;
  }
  unlock_dispatch();
}

static int action_done_mutex_initialized = 0;
static pthread_mutex_t action_done_mutex;

static void lock_action_done()
{

  if(!action_done_mutex_initialized)
  {
    action_done_mutex_initialized = 1;
    pthread_mutex_init(&action_done_mutex, pthread_mutexattr_default);
  }
  pthread_mutex_lock(&action_done_mutex);
}

static void unlock_action_done()
{

  if(!action_done_mutex_initialized)
  {
    action_done_mutex_initialized = 1;
    pthread_mutex_init(&action_done_mutex, pthread_mutexattr_default);
  }

  pthread_mutex_unlock(&action_done_mutex);
}

static void DoActionDone(int i)
{
  pthread_t thread;
#ifndef HAVE_WINDOWS_H
  size_t ssize;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
  pthread_create(&thread, &attr, (void *)(void *)ActionDone, (void *)i);
  pthread_attr_destroy(&attr);
#else
  pthread_create(&thread, pthread_attr_default , (void *)(void *)ActionDone, (void *)i);
#endif
  return;
}

static int dispatch_mutex_initialized = 0;
static pthread_mutex_t dispatch_mutex;

static void lock_dispatch()
{

  if(!dispatch_mutex_initialized)
  {
    dispatch_mutex_initialized = 1;
    pthread_mutex_init(&dispatch_mutex, pthread_mutexattr_default);
  }
  pthread_mutex_lock(&dispatch_mutex);
}

static void unlock_dispatch()
{

  if(!dispatch_mutex_initialized)
  {
    dispatch_mutex_initialized = 1;
    pthread_mutex_init(&dispatch_mutex, pthread_mutexattr_default);
  }

  pthread_mutex_unlock(&dispatch_mutex);
}

static void DoDispatch(int i)
{
  pthread_t thread;
#ifndef HAVE_WINDOWS_H
  size_t ssize;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
  pthread_create(&thread, &attr, (void *)(void *)Dispatch, (void *)i);
  pthread_attr_destroy(&attr);
#else
  pthread_create(&thread, pthread_attr_default , (void *)(void *)Dispatch, (void *)i);
#endif
  return;
}
