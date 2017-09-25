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
#ifndef SERVERSHRP_H
 #define SERVERSHRP_H
 #include <config.h>
 #include <pthread_port.h>
 #ifdef _WIN32
  #include <time.h>
 #else
  #ifndef HAVE_PTHREAD_LOCK_GLOBAL_NP
   extern void pthread_lock_global_np();
   extern void pthread_unlock_global_np();
  #endif
 #endif

 #define SrvNoop        0	   /**** Used to start server ****/
 #define SrvAbort       1	   /**** Abort current action or mdsdcl command ***/
 #define SrvAction      2	   /**** Execute an action nid in a tree ***/
 #define SrvClose       3	   /**** Close open trees ***/
 #define SrvCreatePulse 4	   /**** Create pulse files for single tree (no subtrees) ***/
 #define SrvSetLogging  5	   /**** Turn logging on/off ***/
 #define SrvCommand     6	   /**** Execute MDSDCL command ***/
 #define SrvMonitor     7	   /**** Broadcast messages to action monitors ***/
 #define SrvShow        8	   /**** Request current status of server ***/
 #define SrvStop        9	   /**** Stop server ***/
 #define SrvRemoveLast  10	   /**** Remove last entry in the queue if jobs pending ***/

 #define MonitorBuildBegin 1
 #define MonitorBuild      2
 #define MonitorBuildEnd   3
 #define MonitorCheckin    4
 #define MonitorDispatched  5
 #define MonitorDoing      6
 #define MonitorDone       7

 #define SrvJobABORTED     1
 #define SrvJobSTARTING    2
 #define SrvJobFINISHED    3
 #define SrvJobCHECKEDIN   4

 #define SrvJobBEFORE_NOTIFY 1
 #define SrvJobAFTER_NOTIFY  2

typedef struct {
  struct _SrvJob *next;
  struct _SrvJob *previous;
  int addr;
  int port;
  int sock;
  int length;
  int op;
  int flags;
  int jobid;
} JHeader;

typedef struct _SrvJob {
  JHeader h;
} SrvJob;

typedef struct {
  JHeader h;
  char *tree;
  int shot;
  int nid;
} SrvActionJob;

typedef struct {
  JHeader h;
} SrvCloseJob;

typedef struct {
  JHeader h;
  char *tree;
  int shot;
} SrvCreatePulseJob;

typedef struct {
  JHeader h;
  char *table;
  char *command;
} SrvCommandJob;

typedef struct {
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

typedef struct {
  char server[32];
  int nid;
  int phase;
  int sequence;
  int num_references;
  int *referenced_by;
  struct descriptor *condition;
  int status;
  int netid;
  unsigned on:1;
  unsigned done:1;
  unsigned closed:1;
  unsigned dispatched:1;
  unsigned doing:1;
  unsigned recorded:1;
  char *path;
  char *event;
  pthread_rwlock_t lock;
} ActionInfo;

typedef struct {
  int num;
  char tree[13];
  int shot;
  int failed_essential;
  ActionInfo actions[1];
} DispatchTable;

typedef struct {
  int shot;
  union {
    int nid;
    int phase;
  } u;
  int status;
} DispatchEvent;

/*STATIC_THREADSAFE pthread_rwlock_t actions_rwlock = PTHREAD_RWLOCK_INITIALIZER;*/
#define WRLOCK_ACTION(idx) pthread_rwlock_wrlock(&table->actions[idx].lock)
#define RDLOCK_ACTION(idx) pthread_rwlock_rdlock(&table->actions[idx].lock)
#define UNLOCK_ACTION(idx) pthread_rwlock_unlock(&table->actions[idx].lock)

#ifndef _NO_SERVER_SEND_MESSAGE_PROTO
extern int ServerSendMessage(int *msgid, char *server, int op, int *retstatus, pthread_rwlock_t *lock, int *socket,
			     void (*ast) (), void *astparam, void (*before_ast) (),
			     int numargs_in, ...);
 #endif
extern int ServerConnect(char *);
extern int ServerSendMonitor(char *monitor, char *tree, int shot, int phase, int nid, int on,
			     int mode, char *server, int actstatus);
#endif
