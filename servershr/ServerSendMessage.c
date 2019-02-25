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

		Name:   ServerSendMessage

		Type:   C function

     		Author:	TOM FREDIAN

		Date:   17-APR-1992

    		Purpose: Send message to server.

------------------------------------------------------------------------------

	Call sequence:

int ServerSendMessage();

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

 	Description:

------------------------------------------------------------------------------*/
#define LOAD_INITIALIZESOCKETS
#include <pthread_port.h>
#include <mdsplus/mdsconfig.h>
#include <ipdesc.h>
#include <stdio.h>
#ifdef HAVE_UNISTD_H
  #include <unistd.h>
#endif
#include <string.h>
#include <mds_stdarg.h>
#include <mdsshr.h>
#include <libroutines.h>
#define _NO_SERVER_SEND_MESSAGE_PROTO
#include <errno.h>
#include <signal.h>
#include "servershrp.h"
#include <servershr.h>

#define DEBUG
#ifdef DEBUG
 #define DBG(...) fprintf(stderr,__VA_ARGS__)
#else
 #define DBG(...) {/**/}
#endif

extern short ArgLen();

extern int GetAnswerInfoTS();

/* Job must be a subclass of Condition
 *typedef struct _Condition {
 *  pthread_cond_t  cond;
 *  pthread_mutex_t mutex;
 *  int             value;
 *} Condition;
 */
typedef struct job {
  pthread_cond_t cond;
  pthread_mutex_t mutex;
  int value; //aka done
  int has_condition;
  int *retstatus;
  pthread_rwlock_t *lock;
  void (*ast) ();
  void *astparam;
  void (*before_ast) ();
  int jobid;
  int conid;
  uint32_t addr;
  uint16_t port;
  SOCKET sock;
  struct job* next;
} Job;
static fd_set fdactive;
static pthread_mutex_t jobs_mutex = PTHREAD_MUTEX_INITIALIZER;
#define LOCK_JOBS   pthread_mutex_lock(&jobs_mutex);pthread_cleanup_push((void*)pthread_mutex_unlock, &jobs_mutex)
#define UNLOCK_JOBS pthread_cleanup_pop(1)
static Job *Jobs = NULL;

static int MonJob = -1;
static int JobId = 0;

extern void *GetConnectionInfo();
static SOCKET get_socket(int conid) {
  size_t len;
  char *info_name = NULL;
  SOCKET readfd = INVALID_SOCKET;
  GetConnectionInfo(conid, &info_name, &readfd, &len);
  if (info_name && (strcmp(info_name, "tcp") == 0))
    return readfd;
  return INVALID_SOCKET;
}

static inline Job* pop_job(Job *j) {
  Job *jj, *prev;
  LOCK_JOBS;
  for (jj = Jobs, prev = NULL; jj && jj != j; prev=jj, jj=jj->next);
  if (jj) {
    if (prev) prev->next = jj->next;
    else      Jobs = jj->next;
  }
  UNLOCK_JOBS;
  return jj;
}
static inline void remove_job(Job *j) {
  Job *jj = pop_job(j);
  if (jj) free(jj);
}
static pthread_mutex_t job_conds = PTHREAD_MUTEX_INITIALIZER;
static inline void wait_and_remove_job(Job *j) {
  CONDITION_WAIT_SET(j);
  CONDITION_DESTROY(j,&job_conds);
  remove_job(j);
}
static void do_completion_ast(Job *j, int status, char* msg, int removeJob){
  int has_condition = j->has_condition;
  LOCK_JOBS;
  FD_CLR(j->sock, &fdactive);
  UNLOCK_JOBS;
  if (j->lock) pthread_rwlock_wrlock(j->lock);
  if (j->retstatus)
    *j->retstatus = status;
  if (j->lock) pthread_rwlock_unlock(j->lock);
  if (j->ast && j->astparam) j->ast(j->astparam, msg);
  /**** If job has a condition, ServerWait will not remove it. ***/
  if (has_condition){
    pthread_mutex_lock(&job_conds);pthread_cleanup_push((void*)pthread_mutex_unlock, &job_conds);
    CONDITION_SET(j);
    pthread_cleanup_pop(1);
  } else {
    DBG("Job #%d async done.\n",j->jobid);
    if (removeJob && j->jobid != MonJob)
      remove_job(j);
  }
}

static void abandon(void*in) {
  Job *j = *(Job**)in;
  if (j && j->has_condition) {
    j->has_condition = B_FALSE;
    DBG("Job #%d sync abandoned!\n",j->jobid);
  }
}

static inline Job* pop_job_by_jobid(int jobid) {
  Job *j, *prev;
  LOCK_JOBS;
  for (j = Jobs, prev = NULL; j && (j->jobid != jobid); prev=j, j=j->next);
  if (j) {
    if (prev) prev->next = j->next;
    else      Jobs = j->next;
  }
  UNLOCK_JOBS;
  return j;
}
static void cleanup_job(int status, int jobid){
  Job *j = pop_job_by_jobid(jobid);
  if (j) {
    DisconnectFromMds(j->conid);
    do_completion_ast(j, status, NULL, FALSE);
    if (j->sock != INVALID_SOCKET) {
      shutdown(j->sock, 2);
      close(j->sock);
    }
    free(j);
  }
}

void ServerWait(int jobid) {
  Job *j;
  LOCK_JOBS;
  for (j = Jobs; j && (j->jobid != jobid); j=j->next);
  UNLOCK_JOBS;
  if (j && j->has_condition) {
    DBG("Job #%d sync pending.\n",jobid);
    pthread_cleanup_push(abandon,(void*)&j);
    wait_and_remove_job(j);
    pthread_cleanup_pop(0);
    DBG("Job #%d sync done.\n",jobid);
  } else DBG("Job #%d sync lost!\n",jobid);
}

static inline void do_before_ast(Job *j) {
  if (j->before_ast) j->before_ast(j->astparam);
}

#define minmax(l,x,u) ((l>x) ? l : ( (u<x) ? u : x ) )
static inline int send_request(int conid, int addr, uint16_t port, int op, int flags, int jobid, int numargs, va_list *vlist) {
  char cmd[4096];
  sprintf(cmd, "MdsServerShr->ServerQAction(%ulu,%uwu,%d,%d,%d", addr, port, op, flags, jobid);
  int i;
  for (i = 0; i < numargs; i++) {
    strcat(cmd, ",");
    struct descrip *arg = va_arg(*vlist, struct descrip *);
    if (op == SrvMonitor && numargs == 8 && i == 5 && arg->dtype == DTYPE_LONG
        && *(int *)arg->ptr == MonitorCheckin)
      MonJob = jobid;
    switch (arg->dtype) {
      case DTYPE_CSTRING: {
        int j, k;
        char *c = (char *)arg->ptr;
        int len = strlen(c);
        strcat(cmd, "\"");
        for (j = 0, k = strlen(cmd); j < len; j++, k++) {
          if (c[j] == '"' || c[j] == '\\')
            cmd[k++] = '\\';
          cmd[k] = c[j];
        }
        cmd[k] = 0;
        strcat(cmd, "\"");
        break;
      }
      case DTYPE_LONG:
        sprintf(&cmd[strlen(cmd)], "%d", *(int *)arg->ptr);
        break;
      case DTYPE_CHAR:
        sprintf(&cmd[strlen(cmd)], "%d", (int)*(char *)arg->ptr);
        break;
      default:
        fprintf(stderr,"shouldn't get here! ServerSendMessage dtype = %d\n", arg->dtype);
        sprintf(&cmd[strlen(cmd)], "0");
    }
  }
  strcat(cmd, ")");
  DBG("sending to %d: %s\n",conid,cmd);
  return SendArg(conid, 0, DTYPE_CSTRING, 1, (short)strlen(cmd), 0, 0, cmd);
}
static inline int receive_answer(int conid,int jobid,int op){
  char dtype;
  length_t len;
  char ndims;
  int dims[8];
  int numbytes;
  int *dptr;
  void *mem = NULL;
  int status = GetAnswerInfoTS(conid, &dtype, &len, &ndims, dims, &numbytes, (void **)&dptr, &mem);
  if (op==SrvStop) {
    if STATUS_NOT_OK {
      status = MDSplusSUCCESS;
      cleanup_job(status, jobid);
    } else
      status = MDSplusERROR;
  } else {
    if STATUS_NOT_OK {
      perror("Error: no response from server");
      cleanup_job(status, jobid);
      return status;
    }
  }
  if (mem)
    free(mem);
  return status;
}

static inline int register_job(int *msgid, int *retstatus, pthread_rwlock_t *lock, void (*ast) (), void *astparam, void (*before_ast) (), int conid, uint32_t addr){
  Job *j = (Job *) malloc(sizeof(Job));
  j->retstatus = retstatus;
  j->lock = lock;
  j->ast = ast;
  j->astparam = astparam;
  j->before_ast = before_ast;
  j->conid = conid;
  j->addr = addr;
  j->port = 0;
  j->sock = INVALID_SOCKET;
  LOCK_JOBS;
  j->jobid = ++JobId;
  if (msgid) {
    CONDITION_INIT(j);
    j->has_condition = B_TRUE;
    *msgid = j->jobid;
    DBG("Job #%d sync registered with %"PRIxPTR".\n",j->jobid,(uintptr_t)j);
  } else {
    j->has_condition = B_FALSE;
    j->value = B_TRUE;
    DBG("Job #%d async registered.\n",j->jobid);
  }
  j->next = Jobs;
  Jobs = j;
  UNLOCK_JOBS;
  return j->jobid;
}

static inline SOCKET create_port(uint16_t *port_out) {
  static uint16_t start_port = 0, range_port;
  if (!start_port) {
    char *range = TranslateLogical("MDSIP_PORT_RANGE");
    if (range) {
      char* dash;
      for (dash=range; *dash && *dash!='-' ; dash++);
      if (dash)
        *(dash++)=0;
      start_port = (uint16_t)(atoi(range)&0xffff);
      int end = atoi(dash);
      if (end>0 && end<65536)
        range_port = (uint16_t)end-start_port+1;
      else
        range_port = 100;
    }
    TranslateLogicalFree(range);
    if (!start_port) {
      start_port = 8800;
      range_port =  256;
    }
    DBG("Receiver will be using 'MDSIP_PORT_RANGE=%u-%u'.\n",start_port,start_port+range_port-1);
  }
  uint16_t port;
  static struct sockaddr_in sin;
  long sendbuf = 6000, recvbuf = 6000;
  SOCKET s;
  int c_status = C_ERROR;
  int tries = 0;
  int one = 1;
  INITIALIZESOCKETS;
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == INVALID_SOCKET) {
    perror("Error getting Connection Socket\n");
    return s;
  }
  setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char *)&recvbuf, sizeof(long));
  setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *)&sendbuf, sizeof(long));
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(int));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  for (tries = 0 ; (c_status < 0) && (tries < 500); tries++) {
    port = start_port + (random()%range_port);
    sin.sin_port = htons(port);
    c_status = bind(s, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
  }
  if (c_status < 0) {
    perror("Error binding to service\n");
    return INVALID_SOCKET;
  }
  c_status = listen(s, 5);
  if (c_status < 0) {
    perror("Error from listen\n");
    return INVALID_SOCKET;
  }
  DBG("Listener opened on port %u.\n",port);
  *port_out = port;
  return s;
}

static int is_broken_socket(SOCKET socket){
  if (socket != INVALID_SOCKET) {
    fd_set fdactive;
    FD_ZERO(&fdactive);
    FD_SET(socket, &fdactive);
    struct timeval timeout = {0,0}; // non-blocking
    if (select(socket+1, &fdactive, 0, 0, &timeout) >= 0)
      return send(socket, NULL, 0, MSG_DONTWAIT) != 0;
  }
  return B_TRUE;
}
static inline void reset_fdactive(int rep, SOCKET sock, fd_set* active){
  Job *j, *p;
  LOCK_JOBS;
  if (rep > 0) {
    for (j = Jobs, p = NULL; j ; ) {
      if (is_broken_socket(j->sock)) {
	DBG("Job #%d removed by reset_fdactive\n",j->jobid);
        if (p) p->next = j->next;
        else  Jobs = j = j->next;
      } else {
        p = j;
        j = j->next;
      }
    }
  }
  FD_ZERO(active);
  FD_SET(sock, active);
  for (j = Jobs; j ; j = j->next) {
    if (j->sock != INVALID_SOCKET)
      FD_SET(j->sock, active);
  }
  UNLOCK_JOBS;
  DBG("reset fdactive\n");
}

////////////RECEIVER///////////////////////////////////////////////////////////
static inline void do_message(Job * j){
  char reply[60];
  char *msg = 0;
  int jobid;
  int replyType;
  int status;
  int msglen;
  int num;
  int nbytes;
  nbytes = recv(j->sock, reply, 60, 0);
  if (nbytes != 60) {
    DBG("Receiver header error %d != 60.\n",nbytes);
    cleanup_job(MDSplusFATAL,j->jobid);
    return;
  }
  num = sscanf(reply, "%d %d %d %d", &jobid, &replyType, &status, &msglen);
  if (num != 4) {
    DBG("Receiver header format error: '%s'.\n",reply);
    cleanup_job(MDSplusFATAL,j->jobid);
    return;
  }
  if (jobid != j->jobid)
    fprintf(stderr,"######### Receiver jobid mismatch %d != %d.\n",jobid,j->jobid);
  FREE_ON_EXIT(msg);
  if (msglen != 0) {
    msg = (char *)malloc(msglen + 1);
    msg[msglen] = 0;
    nbytes = recv(j->sock, msg, msglen, 0);
    if (nbytes != msglen) {
      DBG("Job #%d msglen mismatch %d != %d.\n",jobid,nbytes,msglen);
      cleanup_job(MDSplusFATAL,jobid);
      goto end;
    }
  }
  switch (replyType) {
  case SrvJobFINISHED:
    DBG("Job #%d finished.\n",j->jobid);
    do_completion_ast(j, status, msg, TRUE);
    break;
  case SrvJobSTARTING:
    DBG("Job #%d starting.\n",j->jobid);
    do_before_ast(j);
    break;
  case SrvJobCHECKEDIN:
    DBG("Job #%d checked-in.\n",jobid);
    LOCK_JOBS;
    j->jobid = jobid;
    FD_SET(j->sock, &fdactive);
    UNLOCK_JOBS;
    break;
  default:
    DBG("Job #%d failure %d.\n",jobid,replyType);
    cleanup_job(MDSplusFATAL,jobid);
  }
end: ;
  FREE_NOW(msg);
}
static inline void accept_reply(SOCKET sock, struct sockaddr_in *sin) {
  if (sock == INVALID_SOCKET) return;
  const uint32_t addr = *(uint32_t *)&sin->sin_addr;
  Job *j;
  LOCK_JOBS;
  for (j = Jobs; j && (j->addr != addr || j->port != 0);j = j->next);
  UNLOCK_JOBS;
  if (j) {
    j->sock = sock;
    j->port = sin->sin_port;
    do_message(j);
    DBG("Job #%d accepted Reply #%d %u.%u.%u.%u:%u\n",j->jobid,j->conid,ADDR2IP(addr),sin->sin_port);
  } else {
    shutdown(sock, 2);
    close(sock);
    DBG("Reply rejected from %u.%u.%u.%u:%u\n",ADDR2IP(addr),sin->sin_port);
  }
}
static inline Job *lookup_job(fd_set *fds) {
  Job *j;
  LOCK_JOBS;
  for (j = Jobs;
       j && (j->sock == INVALID_SOCKET || !FD_ISSET(j->sock, fds));
       j = j->next);
  UNLOCK_JOBS;
  return j;
}

static Condition receiver_running = CONDITION_INITIALIZER;
static void receiver_exit(void *arg __attribute__ ((unused))){
  DBG("ServerSendMessage thread exitted\n");
  CONDITION_RESET(&receiver_running);
}
static void receiver_thread(void *sockptr){
  atexit((void*)receiver_exit);
// CONDITION_SET(&receiver_running);
  _CONDITION_LOCK(&receiver_running);
  SOCKET sock = *(SOCKET*)sockptr;
  receiver_running.value = B_TRUE;
  _CONDITION_SIGNAL(&receiver_running);
  _CONDITION_UNLOCK(&receiver_running);
// \CONDITION_SET(&receiver_running);
  struct sockaddr_in sin;
  Job last_job;memset(&last_job, 0, sizeof(Job));
  fd_set readfds;
  LOCK_JOBS;
  FD_ZERO(&fdactive);
  FD_SET(sock, &fdactive);
  UNLOCK_JOBS;
  int rep;
  struct timeval readto, timeout = {1,0};
  for (rep = 0; rep < 10; rep++) {
    for (;;) {
      LOCK_JOBS;
      readfds = fdactive;
      UNLOCK_JOBS;
      readto = timeout;
      int num = select(FD_SETSIZE, &readfds, NULL, NULL, &readto);
      if (num <0) break;
      if (num==0) continue;
      rep = 0;
      for (;;) {
        Job *j = lookup_job(&readfds);
        if (!j) break;
        DBG("Reply from %u.%u.%u.%u:%u\n",ADDR2IP(j->addr),j->port);
        last_job = *j;
        FD_CLR(j->sock, &readfds);
        do_message(j);
      }
      if (FD_ISSET(sock, &readfds)) {
        socklen_t len = sizeof(struct sockaddr_in);
        accept_reply(accept(sock, (struct sockaddr *)&sin, &len), &sin);
      }
    }
    SOCKERROR("Dispatcher select loop failed\nLast Job #%d: %u.%u.%u.%u:%u\n",last_job.jobid,ADDR2IP(last_job.addr),last_job.port);
    reset_fdactive(rep, sock, &fdactive);
  }
  fprintf(stderr,"Cannot recover from select errors in ServerSendMessage, exitting\n");
  pthread_exit(0);
}
static int start_receiver(uint16_t *port_out){
  INIT_STATUS;
  static uint16_t port = 0;
  static SOCKET sock;
  static pthread_t thread;
  _CONDITION_LOCK(&receiver_running);
  if (port == 0) {
    sock = create_port(&port);
    if (sock == INVALID_SOCKET) {
      _CONDITION_UNLOCK(&receiver_running);
      return C_ERROR;
    }
  }
  if (!receiver_running.value) {
    CREATE_DETACHED_THREAD(thread, *16, receiver_thread, &sock);
    if (c_status) {
      perror("Error creating pthread");
      status = MDSplusERROR;
    } else {
      _CONDITION_WAIT_SET(&receiver_running);
      status = MDSplusSUCCESS;
    }
  }
  _CONDITION_UNLOCK(&receiver_running);
  *port_out = port;
  return STATUS_NOT_OK;
}
////////////RECEIVER///////////////////////////////////////////////////////////

EXPORT int ServerDisconnect(char *server_in __attribute__((__unused__))) {
  return MDSplusSUCCESS;
}

EXPORT int ServerConnect(char *server_in) {
  int conid;
  INIT_AND_FREE_ON_EXIT(char*,srv);
  srv = TranslateLogical(server_in);
  char *server = srv ? srv : server_in;
  conid = ConnectToMds(server);
  FREE_NOW(srv);
  return conid;
}

int ServerSendMessage(int *msgid, char *server, int op, int *retstatus, pthread_rwlock_t *lock, int *conid_out,
		      void (*ast) (), void *astparam, void (*before_ast) (), int numargs, ...){
  uint16_t port = 0;
  if (start_receiver(&port)) {
    if (ast && astparam)
      ast(astparam);
    return MDSplusFATAL;
  }
  int conid;
  if ((conid = ServerConnect(server)) < 0) {
    if (ast && astparam)
      ast(astparam);
    return ServerPATH_DOWN;
  }
  INIT_STATUS;
  int flags = 0;
  int jobid;
  uint32_t addr = 0;
  if (conid_out)
    *conid_out = conid;
  SOCKET sock = get_socket(conid);
  struct sockaddr_in addr_struct = {0};
  socklen_t slen = sizeof(addr_struct);
  if (getsockname(sock, (struct sockaddr *)&addr_struct, &slen) == 0)
    addr = *(uint32_t *)&addr_struct.sin_addr;
  if (!addr) {
    perror("Error getting the address the socket is bound to.\n");
    DisconnectFromMds(conid);
    if (ast && astparam)
      ast(astparam);
    return ServerSOCKET_ADDR_ERROR;
  }
  jobid = register_job(msgid, retstatus, lock, ast, astparam, before_ast, conid, addr);
  //if (before_ast)
    flags |= SrvJobBEFORE_NOTIFY;
  va_list vlist;
  va_start(vlist, numargs);
  status = send_request(conid, addr, port, op, flags, jobid, minmax(0,numargs,8), &vlist);
  if STATUS_NOT_OK {
      perror("Error sending message to server");
      cleanup_job(status, jobid);
      return status;
  }
  status = receive_answer(conid,jobid,op);
  if STATUS_NOT_OK {
    perror("Error receiving answer from server");
    cleanup_job(status, jobid);
    return status;
  }
  Job * j;
  LOCK_JOBS;
  for (j = Jobs; j && j->jobid != jobid; j=j->next);
  UNLOCK_JOBS;
  DBG("Job #%d linked with %"PRIxPTR"\n",jobid,(uintptr_t)j);
  DisconnectFromMds(conid);
  return status;
}
