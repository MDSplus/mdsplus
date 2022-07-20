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
/*------------------------------------------------------------------------------

                Name:   SERVER$DISPATCH_PHASE

                Type:   C function

                Author:	TOM FREDIAN

                Date:   21-APR-1992

                Purpose: Dispatch a phase

------------------------------------------------------------------------------

        Call sequence:

int SERVER$DISPATCH_PHASE(int efn, DispatchTable *table, struct descriptor
*phasenam, char *noact, int *sync, void (*output_rtn)(), struct descriptor
*monitor)

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

        Description:

------------------------------------------------------------------------------*/

#include <mdsplus/mdsconfig.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include <condition.h>
#include <mdsdescrip.h>
#include <ipdesc.h>
#include <servershr.h>
#include <ncidef.h>
#include <libroutines.h>
#include <strroutines.h>
#include <treeshr.h>
#include <mdsshr.h>
#include <servershr.h>
#include <mds_stdarg.h>
#include <tdishr_messages.h>
#include "servershrp.h"
//#define DEBUG
#include <mdsmsg.h>

extern int TdiCompletionOf();
extern int TdiExecute();
extern int TdiErrorlogsOf();
extern int TdiGetLong();
extern int ProgLoc;

static void dispatch(int idx);
static void send_monitor(int mode, int idx);
static void action_done(intptr_t idx);
static void action_done_action_locked(int idx);
static void action_done_action_unlocked(int idx);
static void before(int idx);
static inline void set_action_ranges(int phase, int *first_c, int *last_c);
static void abort_range(int s, int e);
static void set_group(int sync, int first_g, int *last_g);
static int check_actions_done(int s, int e);
static void record_status(int s, int e);
static void wait_for_actions(int conditionals, int first_g, int last_g,
                             int first_c, int last_c);

typedef struct _send_monitor
{
  struct _send_monitor *next;
  int idx;
  int mode;
} SendMonitorInfo;

static SendMonitorInfo *SendMonitorQueueHead = NULL;
static SendMonitorInfo *SendMonitorQueueTail = NULL;

static DispatchTable *table;
static int noact;
static void (*Output)();
static int MonitorOn = 0;
static char *Monitor = 0;
static int first_s;
static int last_s;

static int AbortInProgress;
static pthread_mutex_t abortinprogress_mutex = PTHREAD_MUTEX_INITIALIZER;
static inline int setAbortInProgress(int val_in)
{
  int val_out;
  pthread_mutex_lock(&abortinprogress_mutex);
  val_out = AbortInProgress;
  AbortInProgress = val_in;
  pthread_mutex_unlock(&abortinprogress_mutex);
  return val_out;
}
static inline int is_abort_in_progress()
{
  int val;
  pthread_mutex_lock(&abortinprogress_mutex);
  val = AbortInProgress;
  pthread_mutex_unlock(&abortinprogress_mutex);
  return val;
}

static Condition JobWaitC = CONDITION_INITIALIZER;

static pthread_mutex_t send_monitor_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t send_monitor_queue_cond = PTHREAD_COND_INITIALIZER;
#define MONITOR_QUEUE_LOCK MUTEX_LOCK_PUSH(&send_monitor_queue_mutex)
#define MONITOR_QUEUE_UNLOCK MUTEX_LOCK_POP(&send_monitor_queue_mutex)

static pthread_rwlock_t table_lock = PTHREAD_RWLOCK_INITIALIZER;
#define WRLOCK_TABLE pthread_rwlock_wrlock(&table_lock)
#define RDLOCK_TABLE pthread_rwlock_rdlock(&table_lock)
#define UNLOCK_TABLE pthread_rwlock_unlock(&table_lock)

#define ACTION_LOCK(idx) table->actions[idx].lock
#ifdef DEBUG_ACTION
#define ACTION_DBG(idx, info, typ, b)                           \
  fprintf(stderr, "%d: %-10s  %s[%d,%d]%c\n", idx, #info, #typ, \
          ACTION_LOCK(idx).__data.__readers, ACTION_LOCK(idx).__data.__writers, b)
#else
#define ACTION_DBG(idx, info, typ, b) \
  do                                  \
  {                                   \
  } while (0)
#endif
#define XLOCK_ACTION(idx, info, typ)                           \
  (                                                            \
      {                                                        \
        ACTION_DBG(idx, info, typ, '?');                       \
        int r = pthread_rwlock_##typ##lock(&ACTION_LOCK(idx)); \
        ACTION_DBG(idx, info, typ, '!');                       \
        r;                                                     \
      })
#define WRLOCK_ACTION(idx, info) XLOCK_ACTION(idx, info, wr)
#define RDLOCK_ACTION(idx, info) XLOCK_ACTION(idx, info, rd)
#define UNLOCK_ACTION(idx, info) XLOCK_ACTION(idx, info, un)

#define ACTION_PRI "Action(%d, path=%s, phase=%d state=%c%c%c%c"
#define ACTION_VAR(i)                           \
  i,                                            \
      table->actions[i].path,                   \
      table->actions[i].phase,                  \
      table->actions[i].condition ? 'C' : 'c',  \
      table->actions[i].dispatched ? 'D' : 'd', \
      table->actions[i].doing ? 'S' : 's',      \
      table->actions[i].done ? 'F' : 'f'
#define PRINT_ACTIONS                                  \
  {                                                    \
    int i;                                             \
    for (i = 0; i < table->num; i++)                   \
    {                                                  \
      RDLOCK_ACTION(i, debug);                         \
      fprintf(stderr, ACTION_PRI "\n", ACTION_VAR(i)); \
      UNLOCK_ACTION(i, debug);                         \
    }                                                  \
  }
//"

static char *Server(char *out, char *srv)
{
  int i;
  for (i = 0; i < 32; i++)
    out[i] = srv[i] == ' ' ? 0 : srv[i];
  out[i] = 0;
  return out;
}

static void before(int idx)
{
  RDLOCK_TABLE;
  if (table)
  { // if no table prevent seg fault
    WRLOCK_ACTION(idx, b);
    {
      ActionInfo *actions = table->actions;
      char logmsg[1024];
      actions[idx].doing = 1;
      send_monitor(MonitorDoing, idx);
      if (Output)
      {
        char server[33];
        char now[32];
        Now32(now);
        sprintf(logmsg, "%s, %s is beginning action %s", now,
                Server(server, actions[idx].server), actions[idx].path);
        (*Output)(logmsg);
      }
    }
    UNLOCK_ACTION(idx, b);
  }
  UNLOCK_TABLE;
}

/* FIND_NEXT_ACTION(START,END,CONDITION)
 * look-up of actions to find first action between START and END-1 meeting
 * CONDITION if successful, i.e. i<END, action[i] will have RDLOCK acquired
 * FIND_NEXT_ACTION_END(END)
 * UNLOCKs action[i] if FIND_NEXT_ACTION left if locked, i.e. CONDITION was met,
 * i<END.
 */
#define FIND_NEXT_ACTION(START, END, CONDITION, info) \
  i = START;                                          \
  for (; i < END; i++)                                \
  {                                                   \
    RDLOCK_ACTION(i, fna_##info);                     \
    if (CONDITION)                                    \
      break;                                          \
    UNLOCK_ACTION(i, fna_##info);                     \
  } // leaves actions[i] locked if i < table->num
#define FIND_NEXT_ACTION_END(END, info) \
  if (i < END)                          \
    UNLOCK_ACTION(i, fnae_##info);

/// update range of conditional actions
static inline void set_action_ranges(int phase, int *first_c, int *last_c)
{
  int i;
  RDLOCK_TABLE;
  if (table)
  { // if no table prevent seg fault
    ActionInfo *actions = table->actions;
#ifdef DEBUG
    PRINT_ACTIONS;
#endif
    FIND_NEXT_ACTION(0, table->num, actions[i].phase == phase, sar)
    if (i < table->num)
    {
      if (actions[i].condition)
      {
        UNLOCK_ACTION(i, sar1);
        *first_c = i;
        FIND_NEXT_ACTION(*first_c + 1, table->num,
                         (actions[i].phase != phase) || !actions[i].condition,
                         sar1);
        *last_c = i;
      }
      else
        *first_c = *last_c = table->num;
      if (i < table->num && actions[i].phase == phase)
      {
        UNLOCK_ACTION(i, sar2);
        first_s = i;
        FIND_NEXT_ACTION(first_s + 1, table->num, actions[i].phase != phase,
                         sar2);
        last_s = i;
      }
      else
        first_s = last_s = table->num;
      FIND_NEXT_ACTION_END(table->num, sar);
    }
    else
    {
      *first_c = first_s = table->num;
      *last_c = last_s = table->num;
    }
  }
  else
  {
    *first_c = first_s = 0;
    *last_c = last_s = 0;
  }
  UNLOCK_TABLE;
  MDSDBG("range=[%d, %d], cond=[%d, %d]", first_s, last_s, *first_c, *last_c);
}

static void abort_range(int s, int e)
{
  int i;
  RDLOCK_TABLE;
  {
    ActionInfo *actions = table->actions;
    for (i = s; i < e; i++)
    {
      RDLOCK_ACTION(i, ar);
      if (actions[i].dispatched && !actions[i].done)
      {
        char server[33];
        int one = 1;
        ServerAbortServer(Server(server, actions[i].server), one);
      }
      UNLOCK_ACTION(i, ar);
    }
  }
  UNLOCK_TABLE;
}

static void set_group(int sync, int first_g, int *last_g)
{
  if (first_g == last_s)
  {
    *last_g = last_s;
    return;
  }
  int i;
  int group;
  RDLOCK_TABLE;
  if (table)
  { // if no table prevent seg fault
    ActionInfo *actions = table->actions;
    sync = (sync < 1) ? 1 : sync;
    RDLOCK_ACTION(first_g, sg);
    group = actions[first_g].sequence / sync;
    UNLOCK_ACTION(first_g, sg);
    FIND_NEXT_ACTION(first_g + 1, last_s, (actions[i].sequence / sync) != group, sg);
    FIND_NEXT_ACTION_END(last_s, sg);
  }
  else
  {
    i = first_g - 1;
  }
  UNLOCK_TABLE;
  *last_g = i;
}

/// return true if range is complete
static int check_actions_done(const int s, const int e)
{
  int i;
  ActionInfo *actions = table->actions;
  FIND_NEXT_ACTION(s, e, actions[i].dispatched && !actions[i].done, noa);
  FIND_NEXT_ACTION_END(e, noa);
  MDSDBG("range=[%d, %d], current=%d", s, e, i);
  return i >= e;
}

static void record_status(int s, int e)
{
  int i;
  RDLOCK_TABLE;
  if (table)
  { // if no table prevent seg fault
    ActionInfo *actions = table->actions;
    for (i = s; i < e; i++)
    {
      WRLOCK_ACTION(i, rs);
      if (actions[i].done && !actions[i].recorded)
      {
        NCI_ITM setnci[] = {{sizeof(actions[0].status), NciSTATUS, 0, 0},
                            {0, NciEND_OF_LIST, 0, 0}};
        setnci[0].pointer = (unsigned char *)&actions[i].status;
        TreeSetNci(actions[i].nid, setnci);
        actions[i].recorded = 1;
        if (IS_NOT_OK(actions[i].status) && !table->failed_essential)
        {
          int flags;
          NCI_ITM getnci[] = {{sizeof(flags), NciGET_FLAGS, 0, 0},
                              {0, NciEND_OF_LIST, 0, 0}};
          getnci[0].pointer = &flags;
          TreeGetNci(actions[i].nid, getnci);
          table->failed_essential = (flags & NciM_ESSENTIAL) != 0;
        }
      }
      UNLOCK_ACTION(i, rs);
    }
  }
  UNLOCK_TABLE;
}

static void wait_for_actions(int all, int first_g, int last_g,
                             int first_c, int last_c)
{
  int c_status = C_OK;
  _CONDITION_LOCK(&JobWaitC);
  int group_done, cond_done = -1;
  while ((c_status == ETIMEDOUT || c_status == C_OK) && !is_abort_in_progress() &&
         !(group_done = check_actions_done(first_g, last_g) &&
                        (!all || (cond_done = check_actions_done(first_c, last_c)))))
  {
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    if (c_status == C_OK)
    {
      MDSDBG("group_done=%c, cond_done=%c",
             group_done ? 'y' : 'n',
             cond_done < 0 ? '?' : (cond_done ? 'y' : 'n'));
#ifdef DEBUG
      PRINT_ACTIONS;
#endif
    }
    tp.tv_sec++;
    c_status = pthread_cond_timedwait(&JobWaitC.cond, &JobWaitC.mutex, &tp);
#if defined(_DECTHREADS_) && (_DECTHREADS_ == 1)
    if (c_status == -1 && errno == 11)
      c_status = ETIMEDOUT;
#endif
  }
  _CONDITION_UNLOCK(&JobWaitC);
}

static char *detail_proc(int full)
{
  char *msg;
  RDLOCK_TABLE;
  if (table)
  { // if no table prevent seg fault
    msg = NULL;
    int i;
    int doing;
    char msg1[1024];
    unsigned int msglen = 4096;
    ActionInfo *actions = table->actions;
    for (doing = 1; doing > (full ? -1 : 0); doing--)
    {
      for (i = 0; i < table->num; i++)
      {
        RDLOCK_ACTION(i, dp);
        if (actions[i].dispatched && !actions[i].done &&
            ((int)actions[i].doing == doing))
        {
          char server[33];
          sprintf(msg1, "	%s %s %s for shot %d\n", actions[i].path,
                  actions[i].doing ? "in progress on" : "dispatched to",
                  Server(server, actions[i].server), table->shot);
          UNLOCK_ACTION(i, dp_t);
          if (!msg)
            msg = strcpy((char *)malloc(msglen), "\nWaiting on:\n");
          if (msglen < (strlen(msg) + strlen(msg1) + 1))
          {
            char *oldmsg = msg;
            msglen += 4096;
            msg = strcpy((char *)malloc(msglen), oldmsg);
            free(oldmsg);
          }
          strcat(msg, msg1);
        }
        else
          UNLOCK_ACTION(i, dp_e);
      }
    }
  }
  else
  {
    msg = NULL;
  }
  UNLOCK_TABLE;
  return msg;
}

static inline void set_monitor(const char *monitor)
{
  MONITOR_QUEUE_LOCK;
  if (monitor)
  {
    MonitorOn = B_TRUE;
    free(Monitor);
    Monitor = strdup(monitor);
  }
  else
  {
    free(Monitor);
    Monitor = NULL;
    MonitorOn = B_FALSE;
  }
  MONITOR_QUEUE_UNLOCK;
}

EXPORT int ServerDispatchPhase(int *id __attribute__((unused)), void *vtable,
                               char *phasenam, char noact_in, int sync,
                               void (*output_rtn)(), const char *monitor)
{
  WRLOCK_TABLE;
  {
    table = vtable;
    if (table)
      table->failed_essential = B_FALSE;
  }
  UNLOCK_TABLE;
  if (!vtable)
    return TreeSUCCESS;
  int i;
  int phase;
  int first_g, last_g = 0, first_c, last_c;
  DESCRIPTOR_LONG(phase_d, 0);
  static DESCRIPTOR(phase_lookup, "PHASE_NUMBER_LOOKUP($)");
  struct descriptor phasenam_d = {0, DTYPE_T, CLASS_S, 0};
  phase_d.pointer = (char *)&phase;
  Output = output_rtn;
  noact = noact_in;
  setAbortInProgress(0);
  phasenam_d.length = strlen(phasenam);
  phasenam_d.pointer = phasenam;
  ProgLoc = 6005;
  int status = TdiExecute(&phase_lookup, &phasenam_d, &phase_d MDS_END_ARG);
  ProgLoc = 6006;
  if (STATUS_OK && (phase > 0))
  {
    set_monitor(monitor);
    ProgLoc = 6007;
    set_action_ranges(phase, &first_c, &last_c);
    ProgLoc = 6008;
    ServerSetDetailProc(detail_proc);
    ProgLoc = 6009;
    first_g = first_s;
    while (!is_abort_in_progress() && (first_g < last_s))
    {
      ProgLoc = 6010;
      set_group(sync, first_g, &last_g);
      ProgLoc = 6011;
      for (i = first_g; i < last_g; i++)
        dispatch(i);
      ProgLoc = 6012;
      wait_for_actions(0, first_g, last_g, first_c, last_c);
      first_g = last_g;
    }
    ProgLoc = 6013;
    if (setAbortInProgress(0))
    {
      abort_range(first_c, last_c);
      abort_range(first_s, last_s);
      status = ServerABORT;
    }
    ProgLoc = 6014;
    wait_for_actions(1, first_g, last_g, first_c, last_c);
    ProgLoc = 6015;
    setAbortInProgress(1);
    RDLOCK_TABLE;
    if (table)
    {
      ActionInfo *actions = table->actions;
      for (i = first_c; i < last_c; i++)
      {
        WRLOCK_ACTION(i, sdpw);
        if (!actions[i].done)
        {
          actions[i].status = ServerCANT_HAPPEN;
          action_done_action_locked(i);
        }
        UNLOCK_ACTION(i, sdp);
      }
    }
    UNLOCK_TABLE;
    ProgLoc = 6015;
    wait_for_actions(1, first_g, last_g, first_c, last_c);
    ProgLoc = 6016;
    if (!noact)
      record_status(first_c, last_c);
    ProgLoc = 6017;
    if (!noact)
      record_status(first_s, last_s);
    ProgLoc = 6018;
  }
  ProgLoc = 6019;
  return status;
}

EXPORT int ServerFailedEssential(void *vtable, int reset)
{
  // returns B_FALSE if no table
  int failed;
  WRLOCK_TABLE;
  if (vtable)
  {
    DispatchTable *table = (DispatchTable *)vtable;
    failed = table->failed_essential;
    if (reset)
      table->failed_essential = B_FALSE;
  }
  else
  {
    failed = B_FALSE;
  }
  UNLOCK_TABLE;
  return failed;
}

static void dispatch(int i)
{
  RDLOCK_TABLE;
  if (table)
  {
    int status;
    char logmsg[1024];
    char server[33];
    ActionInfo *actions = table->actions;
    WRLOCK_ACTION(i, d);
    MDSDBG(ACTION_PRI, ACTION_VAR(i));
    actions[i].done = 0;
    actions[i].doing = 0;
    actions[i].dispatched = 0;
    if (Output)
    {
      char now[32];
      Now32(now);
      sprintf(logmsg, "%s, Dispatching node %s to %s", now, actions[i].path,
              Server(server, actions[i].server));
      (*Output)(logmsg);
    }
    // ProgLoc = 7001;
    send_monitor(MonitorDispatched, i);
    // ProgLoc = 7002;
    if (noact)
    {
      actions[i].dispatched = 1;
      actions[i].status = status = 1;
      action_done_action_locked(i);
      UNLOCK_ACTION(i, d);
      action_done_action_unlocked(i);
    }
    else
    {
      UNLOCK_ACTION(i, d_w);
      status = ServerDispatchAction(
          0, Server(server, actions[i].server), table->tree, table->shot,
          actions[i].nid, action_done, (void *)(intptr_t)i, &actions[i].status,
          &actions[i].lock, &actions[i].netid, before);
      if (STATUS_OK)
      {
        WRLOCK_ACTION(i, d_w);
        actions[i].dispatched = 1;
        UNLOCK_ACTION(i, d);
      }
      else
      {
        WRLOCK_ACTION(i, d_w);
        actions[i].status = status;
        action_done_action_locked(i);
        UNLOCK_ACTION(i, d);
        action_done_action_unlocked(i);
      }
    }
  }
  UNLOCK_TABLE;
}

static void action_done_action_locked(int idx)
{
  ActionInfo *actions = table->actions;
  if (actions[idx].event)
    MDSEvent(actions[idx].event, sizeof(int), (char *)&table->shot);
  send_monitor(MonitorDone, idx);
  if (Output)
  {
    char logmsg[1024];
    char now[32];
    Now32(now);
    if (IS_OK(actions[idx].status))
      sprintf(logmsg, "%s, Action %s completed", now, actions[idx].path);
    else
    {
      char *emsg = MdsGetMsg(actions[idx].status);
      sprintf(logmsg, "%s, Action %s failed, %s", now, actions[idx].path,
              emsg);
    }
    (*Output)(logmsg);
  }
  actions[idx].done = 1;
  actions[idx].recorded = 0;
  EMPTYXD(xd);
  char expression[60];
  struct descriptor expression_d = {0, DTYPE_T, CLASS_S, 0};
  expression_d.pointer = expression;
  expression_d.length = sprintf(expression, "PUBLIC _ACTION_%08X = %d",
                                actions[idx].nid, actions[idx].status);
  TdiExecute(&expression_d, &xd MDS_END_ARG);
  MdsFree1Dx(&xd, NULL);
}

static void action_done_action_unlocked(int idx)
{
  if (is_abort_in_progress())
    return;
  ActionInfo *actions = table->actions;
  int i;
  for (i = 0; i < actions[idx].num_references; i++)
  {
    int dstat;
    int doit;
    int cidx = actions[idx].referenced_by[i];
    RDLOCK_ACTION(cidx, adl);
    if (!actions[cidx].done && !actions[cidx].dispatched)
    {
      if (IS_OK(dstat = TdiGetLong(actions[cidx].condition, &doit)))
      {
        UNLOCK_ACTION(cidx, ad_ftt);
        if (doit)
        {
          dispatch(cidx);
        }
        else
        {
          WRLOCK_ACTION(cidx, ad_ftte);
          actions[cidx].status = ServerNOT_DISPATCHED;
          action_done_action_locked(cidx);
          UNLOCK_ACTION(cidx, ad_ftte);
          action_done_action_unlocked(cidx);
        }
      }
      else if (dstat != TdiUNKNOWN_VAR)
      {
        UNLOCK_ACTION(cidx, ad_fte);
        WRLOCK_ACTION(cidx, ad_fte);
        actions[cidx].status = ServerINVALID_DEPENDENCY;
        action_done(cidx);
        UNLOCK_ACTION(cidx, ad_fte);
        action_done_action_unlocked(cidx);
      }
    }
    else
      UNLOCK_ACTION(cidx, ad_fe);
  }
}

static inline void action_done_table_locked(int idx)
{
  WRLOCK_ACTION(idx, ad);
  action_done_action_locked(idx);
  UNLOCK_ACTION(idx, ad);
  action_done_action_unlocked(idx);
}

static void action_done_do(intptr_t idx)
{
  MDSDBG("Action(%d)", (int)idx);
  if (idx >= 0)
  {
    RDLOCK_TABLE;
    if (table)
    { // if no table prevent seg fault
      action_done_table_locked(idx);
    }
    UNLOCK_TABLE;
  }
  CONDITION_SET(&JobWaitC);
}

//#define ACTION_DONE_THREAD
#ifdef ACTION_DONE_THREAD
typedef struct _complete
{
  struct _complete *next;
  int idx;
} Complete;

static Condition CompletedC = CONDITION_INITIALIZER;
static Complete *CompletedQueueHead = NULL;
static Complete *CompletedQueueTail = NULL;

static pthread_mutex_t completed_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static inline void action_done_signal() { CONDITION_SET(&CompletedC); }

static inline void action_done_wait()
{
  CONDITION_WAIT_1SEC(&CompletedC);
}

static void action_done_push(intptr_t i)
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
  action_done_signal();
}

static int action_done_pop(intptr_t *i)
{
  int doneAction = -1;
  while (doneAction == -1)
  {
    pthread_mutex_lock(&completed_queue_mutex);
    if (CompletedQueueHead)
    {
      Complete *c = CompletedQueueHead;
      doneAction = CompletedQueueHead->idx;
      CompletedQueueHead = CompletedQueueHead->next;
      if (!CompletedQueueHead)
        CompletedQueueTail = 0;
      free(c);
      pthread_mutex_unlock(&completed_queue_mutex);
    }
    else
    {
      CONDITION_RESET(&CompletedC); // reset list state
      pthread_mutex_unlock(&completed_queue_mutex);
      action_done_wait();
    }
  }
  *i = doneAction;
  return B_TRUE;
}

static Condition ActionDoneRunningC = CONDITION_INITIALIZER;

static void action_done_exit()
{
  MDSDBG("exit");
  CONDITION_RESET(&ActionDoneRunningC);
}

static void action_done_thread()
{
  intptr_t i;
  pthread_cleanup_push(action_done_exit, 0);
  CONDITION_SET(&ActionDoneRunningC);
  while (action_done_pop(&i))
    action_done_do(i);
  pthread_cleanup_pop(1);
}

static void action_done(intptr_t i)
{
  INIT_STATUS;
  static pthread_t thread;
  action_done_push(i); /***** must be done before starting thread ****/
  CONDITION_START_THREAD(&ActionDoneRunningC, thread, , action_done_thread, NULL);
  if (STATUS_NOT_OK)
    perror("action_done: pthread creation failed");
}
#else
static inline void action_done(intptr_t i)
{
  return action_done_do(i);
}
#endif

void send_monitor_do(int mode, int idx)
{
  if (MonitorOn)
  {
    char tree[13];
    char *cptr;
    int i;
    RDLOCK_TABLE;
    if (table)
    { // if no table prevent seg fault
      for (i = 0, cptr = table->tree; i < 12; i++)
        if (cptr[i] == (char)32)
          break;
        else
          tree[i] = cptr[i];
      tree[i] = 0;
      ActionInfo *actions = table->actions;
      RDLOCK_ACTION(idx, sm);
      int on = actions[idx].on;
      char server[33];
      for (i = 0, cptr = actions[idx].server; i < 32 && (cptr[i] != ' '); i++)
        server[i] = cptr[i];
      server[i] = 0;
      MonitorOn = ServerSendMonitor(Monitor, tree, table->shot,
                                    actions[idx].phase, actions[idx].nid, on,
                                    mode, server, actions[idx].status);
      UNLOCK_ACTION(idx, sm);
    }
    UNLOCK_TABLE;
  }
}

static Condition SendMonitorRunningC = CONDITION_INITIALIZER;

static void send_monitor_push(int mode, int i)
{
  SendMonitorInfo *c = malloc(sizeof(SendMonitorInfo));
  c->idx = i;
  c->mode = mode;
  c->next = 0;
  MONITOR_QUEUE_LOCK;
  if (SendMonitorQueueTail)
    SendMonitorQueueTail->next = c;
  else
    SendMonitorQueueHead = c;
  SendMonitorQueueTail = c;
  pthread_cond_signal(&send_monitor_queue_cond);
  MONITOR_QUEUE_UNLOCK;
}

static int send_monitor_pop(int *mode_out, int *i)
{
  int idx;
  int mode;
  MONITOR_QUEUE_LOCK;
  while (!SendMonitorQueueHead)
  {
    pthread_cond_wait(&send_monitor_queue_cond, &send_monitor_queue_mutex);
  }
  SendMonitorInfo *c = SendMonitorQueueHead;
  idx = SendMonitorQueueHead->idx;
  mode = SendMonitorQueueHead->mode;
  SendMonitorQueueHead = SendMonitorQueueHead->next;
  free(c);
  if (!SendMonitorQueueHead)
    SendMonitorQueueTail = NULL;
  MONITOR_QUEUE_UNLOCK;
  *i = idx;
  *mode_out = mode;
  return B_TRUE;
}

static void send_monitor_exit()
{
  CONDITION_RESET(&SendMonitorRunningC);
}

static void send_monitor_thread()
{
  int i;
  int mode;
  pthread_cleanup_push(send_monitor_exit, NULL);
  CONDITION_SET(&SendMonitorRunningC);
  while (send_monitor_pop(&mode, &i))
    send_monitor_do(mode, i);
  pthread_cleanup_pop(1);
}

static void send_monitor(int mode, int idx)
{
  INIT_STATUS;
  static pthread_t thread;
  send_monitor_push(mode, idx); /***** must be done before starting thread ****/
  CONDITION_START_THREAD(&SendMonitorRunningC, thread, , send_monitor_thread,
                         NULL);
  if (STATUS_NOT_OK)
    perror("send_monitor: pthread creation failed");
}

// used in ServerDispatchClose
void server_dispatch_table_disarm(void *vtable)
{
  WRLOCK_TABLE;
  if (table == vtable)
    table = NULL; // disarm table
  UNLOCK_TABLE;
}

static inline void server_dispatch_table_free(void *vtable)
{
  ActionInfo *actions = ((DispatchTable *)vtable)->actions;
  int num_actions = ((DispatchTable *)vtable)->num;
  int i;
  for (i = 0; i < num_actions; i++)
  {
    free(actions[i].referenced_by);
    if (actions[i].path)
      TreeFree(actions[i].path);
    free(actions[i].event);
    if (actions[i].condition)
    {
      MdsFree1Dx((struct descriptor_xd *)actions[i].condition, 0);
      free(actions[i].condition);
    }
  }
  free(vtable);
}

EXPORT int ServerFreeDispatchTable(void *vtable)
{
  // should be called during CloseTopTree
  if (vtable)
  {
    server_dispatch_table_disarm(vtable);
    server_dispatch_table_free(vtable);
  }
  return MDSplusSUCCESS;
}
