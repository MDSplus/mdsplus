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
static pthread_mutex_t jobs_mutex = PTHREAD_MUTEX_INITIALIZER;
#define LOCK_JOBS   pthread_mutex_lock(&jobs_mutex);pthread_cleanup_push((void*)pthread_mutex_unlock, &jobs_mutex)
#define UNLOCK_JOBS pthread_cleanup_pop(1)
static Job *Jobs = NULL;

typedef struct _client {
  SOCKET sock;
  int conid;
  uint32_t addr;
  uint16_t port;
  struct _client *next;
} Client;
static pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
#define LOCK_CLIENTS   pthread_mutex_lock(&clients_mutex);pthread_cleanup_push((void*)pthread_mutex_unlock, &clients_mutex)
#define UNLOCK_CLIENTS pthread_cleanup_pop(1)
#define UNLOCK_CLIENTS_REV pthread_mutex_unlock(&clients_mutex);pthread_cleanup_push((void*)pthread_mutex_lock, &clients_mutex)
#define LOCK_CLIENTS_REV   pthread_cleanup_pop(1)
static Client *Clients = NULL;

static int MonJob = -1;
static int JobId = 0;

int is_broken_socket(SOCKET socket);

static int start_receiver(uint16_t *port);
int ServerConnect(char *server);
static int RegisterJob(int *msgid, int *retstatus, pthread_rwlock_t *lock, void (*ast) (), void *astparam, void (*before_ast) (), int conid, uint32_t addr);
static void CleanupJob(int status, int jobid);
static void ReceiverThread(void *sockptr);
static void DoMessage(Job * j, fd_set * fdactive);
static void RemoveClient(Client * c, fd_set * fdactive);
static void AddClient(uint32_t addr, uint16_t port, int send_sock);
static void AcceptReply(SOCKET sock, struct sockaddr_in *sin, fd_set * fdactive);

extern void *GetConnectionInfo();
static SOCKET getSocket(int conid)
{
  size_t len;
  char *info_name = NULL;
  SOCKET readfd = INVALID_SOCKET;
  GetConnectionInfo(conid, &info_name, &readfd, &len);
  if (info_name && (strcmp(info_name, "tcp") == 0))
    return readfd;
  return INVALID_SOCKET;
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
      CleanupJob(status, jobid);
    } else
      status = MDSplusERROR;
  } else {
    if STATUS_NOT_OK {
      perror("Error: no response from server");
      CleanupJob(status, jobid);
      return status;
    }
  }
  if (mem)
    free(mem);
  return status;
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
  SOCKET sock = getSocket(conid);
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
  jobid = RegisterJob(msgid, retstatus, lock, ast, astparam, before_ast, conid, addr);
  //if (before_ast)
    flags |= SrvJobBEFORE_NOTIFY;
  va_list vlist;
  va_start(vlist, numargs);
  status = send_request(conid, addr, port, op, flags, jobid, minmax(0,numargs,8), &vlist);
  if STATUS_NOT_OK {
      perror("Error sending message to server");
      CleanupJob(status, jobid);
      return status;
  }
  return receive_answer(conid,jobid,op);
}

static void RemoveJob(Job *j){
  LOCK_JOBS;
  Job *jj, *prev;
  for (jj = Jobs, prev = NULL; jj && jj != j; prev=jj, jj=jj->next);
  if (jj) {
    if (!jj->has_condition) {
      if (prev) prev->next = jj->next;
      else      Jobs = jj->next;
      DBG("Job #%d async done.\n",jj->jobid);
      free(jj);
    }
  }
  UNLOCK_JOBS;
}

static pthread_mutex_t job_conds = PTHREAD_MUTEX_INITIALIZER;
static void DoCompletionAst(Job *j, int status, int removeJob){
  int has_condition = j->has_condition;
  if (j->lock) pthread_rwlock_wrlock(j->lock);
  if (j->retstatus)
    *j->retstatus = status;
  if (j->lock) pthread_rwlock_unlock(j->lock);
  if (j->ast && j->astparam) j->ast(j->astparam);
  if (removeJob && j->jobid != MonJob)
    RemoveJob(j);
  /**** If job has a condition, RemoveJob will not remove it. ***/
  if (has_condition){
    pthread_mutex_lock(&job_conds);pthread_cleanup_push((void*)pthread_mutex_unlock, &job_conds);
    CONDITION_SET(j);
    pthread_cleanup_pop(1);
  }
}

static void DoCompletionAstId(int jobid, int status, int removeJob){
  Job *j;
  LOCK_JOBS;
  for (j=Jobs ; j && (j->jobid != jobid); j=j->next);
  if (!j) for (j=Jobs ; j && (j->jobid != MonJob); j=j->next);
  UNLOCK_JOBS;
  if (j) DoCompletionAst(j, status, removeJob);
}

static void abandon(void*in) {
  Job *j = *(Job**)in;
  if (j && j->has_condition) {
    j->has_condition = B_FALSE;
    DBG("Job #%d sync abandoned!\n",j->jobid);
  }
}
static inline Job* get_job_by_conid(int conid) {
  Job *j;
  LOCK_JOBS;
  for (j = Jobs; j && (j->conid != conid) ; j = j->next);
  UNLOCK_JOBS;
  return j;
}
static inline Job* pop_job_by_conid(int conid) {
  Job *j, *prev;
  LOCK_JOBS;
  for (j = Jobs, prev = NULL; j && (j->conid != conid); prev=j, j=j->next);
  if (j) {
    if (prev) prev->next = j->next;
    else      Jobs = j->next;
  }
  UNLOCK_JOBS;
  return j;
}
static inline Job* get_job_by_jobid(int jobid) {
  Job *j;
  LOCK_JOBS;
  for (j = Jobs; j && (j->jobid != jobid) ; j = j->next);
  UNLOCK_JOBS;
  return j;
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
static inline Job* pop_job(Job *j) {
  Job *jj, *prev;
  LOCK_JOBS;
  for (jj = Jobs, prev = NULL; jj && jj != j; prev=jj, jj=jj->next);
  if (prev) prev->next = jj->next;
  else      Jobs = jj->next;
  UNLOCK_JOBS;
  return jj;
}
static inline void remove_job(Job *j) {
  Job *jj = pop_job(j);
  if (jj) free(jj);
}
static inline void wait_and_remove_job(Job *j) {
  CONDITION_WAIT_SET(j);
  CONDITION_DESTROY(j,&job_conds);
  remove_job(j);
}
void ServerWait(int jobid) {
  Job *j = get_job_by_jobid(jobid);
  if (j && j->has_condition) {
    DBG("Job #%d sync pending.\n",jobid);
    pthread_cleanup_push(abandon,(void*)&j);
    wait_and_remove_job(j);
    pthread_cleanup_pop(0);
    DBG("Job #%d sync done.\n",jobid);
  } else DBG("Job #%d sync lost!\n",jobid);
}

static void DoBeforeAst(int jobid) {
  Job *j;
  void *astparam;
  void (*before_ast) ();
  LOCK_JOBS;
  astparam = NULL;
  before_ast = NULL;
  for (j = Jobs; j && (j->jobid != jobid); j = j->next) ;
  if (j) {
    astparam   = j->astparam;
    before_ast = j->before_ast;
  }
  UNLOCK_JOBS;
  if (before_ast) before_ast(astparam);
}

static int RegisterJob(int *msgid, int *retstatus, pthread_rwlock_t *lock, void (*ast) (), void *astparam, void (*before_ast) (), int conid, uint32_t addr){
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
    DBG("Job #%d sync registered.\n",j->jobid);
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

static void CleanupJob(int status, int jobid){
  Job *j = pop_job_by_jobid(jobid);
  if (j) {
    int conid = j->conid;
    DoCompletionAst(j, status, FALSE);
    if (j->sock != INVALID_SOCKET) {
      shutdown(j->sock, 2);
      close(j->sock);
    }
    free(j);
    for (;;) {
      j = pop_job_by_conid(conid);
      if (!j) break;
      DoCompletionAst(j, status, FALSE);
      free(j);
    }
  }
}

static SOCKET CreatePort(uint16_t *port_out) {
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

static Condition ReceiverRunning = CONDITION_INITIALIZER;

static int start_receiver(uint16_t *port_out){
  INIT_STATUS;
  static uint16_t port = 0;
  static SOCKET sock;
  static pthread_t thread;
  _CONDITION_LOCK(&ReceiverRunning);
  if (port == 0) {
    sock = CreatePort(&port);
    if (sock == INVALID_SOCKET) {
      _CONDITION_UNLOCK(&ReceiverRunning);
      return C_ERROR;
    }
  }
  if (!ReceiverRunning.value) {
    CREATE_DETACHED_THREAD(thread, *16, ReceiverThread, &sock);
    if (c_status) {
      perror("Error creating pthread");
      status = MDSplusERROR;
    } else {
      _CONDITION_WAIT_SET(&ReceiverRunning);
      status = MDSplusSUCCESS;
    }
  }
  _CONDITION_UNLOCK(&ReceiverRunning);
  *port_out = port;
  return STATUS_NOT_OK;
}

static void ReceiverExit(void *arg __attribute__ ((unused))){
  DBG("ServerSendMessage thread exitted\n");
  Client * c;
  for (c=Clients ; c ; c=c->next) {
    shutdown(c->sock, 2);
    close(c->sock);
  }
  CONDITION_RESET(&ReceiverRunning);
}

static void _RemoveClient(Client* c){
  UNLOCK_CLIENTS_REV;
  RemoveClient(c, NULL);
  LOCK_CLIENTS_REV;
}
static void ResetFdactive(int rep, SOCKET sock, fd_set* active){
  LOCK_CLIENTS;
  Client* c;
  if (rep > 0) {
    for (c = Clients; c;) {
      if (is_broken_socket(c->sock)) {
	DBG("removed client in ResetFdactive\n");
        _RemoveClient(c);
        c = Clients;
      } else
        c = c->next;
    }
  }
  FD_ZERO(active);
  FD_SET(sock, active);
  for (c = Clients; c; c = c->next) {
    if (c->sock != INVALID_SOCKET)
      FD_SET(c->sock, active);
  }
  UNLOCK_CLIENTS;
  DBG("reset fdactive in ResetFdactive\n");
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclobbered"
// fc21 claims 'last_client_port' is clobbered
static void ReceiverThread(void *sockptr){
  atexit((void*)ReceiverExit);
// CONDITION_SET(&ReceiverRunning);
  _CONDITION_LOCK(&ReceiverRunning);
  SOCKET sock = *(SOCKET*)sockptr;
  ReceiverRunning.value = B_TRUE;
  _CONDITION_SIGNAL(&ReceiverRunning);
  _CONDITION_UNLOCK(&ReceiverRunning);
// \CONDITION_SET(&ReceiverRunning);
  struct sockaddr_in sin;
  uint32_t last_job_addr = 0;
  uint16_t last_job_port = 0;
  fd_set readfds, fdactive;
  FD_ZERO(&fdactive);
  FD_SET(sock, &fdactive);
  int rep;
  struct timeval readto, timeout = {1,0};
  for (rep = 0; rep < 10; rep++) {
    for (readfds=fdactive,readto=timeout
      ;; readfds=fdactive,readto=timeout,rep=0) {
      int num = select(FD_SETSIZE, &readfds, NULL, NULL, &readto);
      if (num <0) break;
      if (num==0) continue;
      Job *j, *next;
      for (;;) {
        LOCK_CLIENTS;
        for (j = Jobs, next = j ? j->next : 0;
          j && (j->sock == INVALID_SOCKET || !FD_ISSET(j->sock, &readfds));
          j = next, next = j ? j->next : 0);
        UNLOCK_CLIENTS;
        if (!j) break;
        DBG("Reply from %u.%u.%u.%u:%u\n",ADDR2IP(j->addr),j->port);
        last_job_addr = j->addr;
        last_job_port = j->port;
        DoMessage(j, &fdactive);
        FD_CLR(j->sock, &readfds);
      }
      if (FD_ISSET(sock, &readfds)) {
        socklen_t len = sizeof(struct sockaddr_in);
        AcceptReply(accept(sock, (struct sockaddr *)&sin, &len), &sin, &fdactive);
      }
    }
    SOCKERROR("Dispatcher select loop failed\nLast client: %u.%u.%u.%u:%u\n",ADDR2IP(last_job_addr),last_job_port);
    ResetFdactive(rep, sock, &fdactive);
  }
  fprintf(stderr,"Cannot recover from select errors in ServerSendMessage, exitting\n");
  pthread_exit(0);
}
#pragma GCC diagnostic pop

int is_broken_socket(SOCKET socket)
{
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

static Client* get_client(uint32_t addr, uint16_t port) {
  Client *c;
  LOCK_CLIENTS;
  for (c = Clients; c && (c->addr != addr || c->port != port); c = c->next) ;
  UNLOCK_CLIENTS;
  return c;
}

static Client* get_addr_port(char* server, uint32_t*addrp, uint16_t*portp) {
  uint32_t addr;
  uint16_t port;
  char hostpart[256] = { 0 };
  char portpart[256] = { 0 };
  int num = sscanf(server, "%[^:]:%s", hostpart, portpart);
  if (num != 2) {
    DBG("Server '%s' unknown\n", server);
    return NULL;
  }
  addr = LibGetHostAddr(hostpart);
  if (!addr) return NULL;
  if (atoi(portpart) == 0) {
    struct servent *sp = getservbyname(portpart, "tcp");
    if (sp)
      port = sp->s_port;
    else {
      char *portnam = getenv(portpart);
      portnam = portnam ? portnam : ((hostpart[0] == '_') ? "8200" : "8000");
      port = htons((uint16_t)atoi(portnam));
    }
  } else
    port = htons((uint16_t)atoi(portpart));
  if (addrp) *addrp=addr;
  if (portp) *portp=port;
  return get_client(addr,port);
}

EXPORT int ServerDisconnect(char *server_in) {
  char *srv = TranslateLogical(server_in);
  Client* c;
  FREE_ON_EXIT(srv);
  char *server = srv ? srv : server_in;
  c = get_addr_port(server,NULL,NULL);
  FREE_NOW(srv);
  if (c) {
    RemoveClient(c, NULL);
    return MDSplusSUCCESS;
  }
  return MDSplusERROR;
}

EXPORT int ServerConnect(char *server_in) {
  int conid;
  char *srv = TranslateLogical(server_in);
  FREE_ON_EXIT(srv);
  conid = -1;
  char *server = srv ? srv : server_in;
  uint32_t addr;
  uint16_t port = 0;
  Client* c = get_addr_port(server,&addr,&port);
  if (c) {
    if (is_broken_socket(getSocket(c->conid)))
      RemoveClient(c, NULL);
    else
      conid = c->conid;
  }
  if (port && conid == -1) {
    conid = ConnectToMds(server);
    if (conid>=0)
      AddClient(addr, port, conid);
  }
  FREE_NOW(srv);
  return conid;
}

static void DoMessage(Job * j, fd_set * fdactive){
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
    FD_CLR(j->sock,fdactive);
    RemoveJob(j);
    return;
  }
  num = sscanf(reply, "%d %d %d %d", &jobid, &replyType, &status, &msglen);
  if (num != 4) {
    DBG("Receiver header format error: '%s'.\n",reply);
    FD_CLR(j->sock,fdactive);
    RemoveJob(j);
    return;
  }
#ifdef DEBUG
  if (jobid != j->jobid)
    DBG("Receiver jobid mismatch %d != %d.\n",jobid,j->jobid);
#endif
  FREE_ON_EXIT(msg);
  if (msglen != 0) {
    msg = (char *)malloc(msglen + 1);
    msg[msglen] = 0;
    nbytes = recv(j->sock, msg, msglen, 0);
    if (nbytes != msglen) {
      DBG("Job #%d msglen mismatch %d != %d.\n",jobid,nbytes,msglen);
      free(msg);
      FD_CLR(j->sock,fdactive);
      RemoveJob(j);
      return;
    }
  }
  switch (replyType) {
  case SrvJobFINISHED:
    DBG("Job #%d finished.\n",jobid);
    FD_CLR(j->sock,fdactive);
    DoCompletionAstId(jobid, status, TRUE);
    break;
  case SrvJobSTARTING:
    DBG("Job #%d starting.\n",jobid);
    DoBeforeAst(jobid);
    break;
  case SrvJobCHECKEDIN:
    DBG("Job #%d checked-in.\n",jobid);
    break;
  default:
    DBG("Job #%d failure %d.\n",jobid,replyType);
    FD_CLR(j->sock,fdactive);
  }
  FREE_NOW(msg);
}


static int pop_client(Client * c) {
  int client_found;
  LOCK_CLIENTS;
  client_found = 0;
  if (Clients == c) {
    client_found = 1;
    Clients = c->next;
  } else {
    Client *cp;
    for (cp = Clients; cp && cp->next != c; cp = cp->next) ;
    if (cp && cp->next == c) {
      client_found = 1;
      cp->next = c->next;
    }
  }
  UNLOCK_CLIENTS;
  if (client_found) {
    int conid = c->conid;
    if (c->sock != INVALID_SOCKET) {
      shutdown(c->sock, 2);
      close(c->sock);
    }
    if (c->conid >= 0)
      DisconnectFromMds(c->conid);
    free(c);
    DBG("Client #%d removed\n",conid);
    return conid;
  }
  return -1;
}

static void RemoveClient(Client * c, fd_set *fdactive) {
  int conid = pop_client(c);
  Job *j;
  for (;;) {
    j = pop_job_by_conid(conid);
    if (!j) break;
    FD_CLR(j->sock,fdactive);
    DoCompletionAst(j, ServerPATH_DOWN, FALSE);
    RemoveJob(j);
  }
}

static void AddClient(unsigned int addr, uint16_t port, int conid){
  Client *c;
  Client *new = (Client *) malloc(sizeof(Client));
  new->sock = INVALID_SOCKET;
  new->conid = conid;
  new->addr = addr;
  new->port = port;
  new->next = 0;
  LOCK_CLIENTS;
  for (c = Clients; c && c->next != 0; c = c->next) ;
  if (c)
    c->next = new;
  else
    Clients = new;
  UNLOCK_CLIENTS;
  DBG("Client #%d added %u.%u.%u.%u:%u\n",conid,ADDR2IP(addr),port);
}

static void AcceptReply(SOCKET sock, struct sockaddr_in *sin, fd_set * fdactive) {
  if (sock == INVALID_SOCKET) return;
  const uint32_t addr = *(uint32_t *)&sin->sin_addr;
  Job *j;
  LOCK_JOBS;
  for (j = Jobs; j && (j->addr != addr || j->port != 0);j = j->next);
  UNLOCK_JOBS;
  if (j) {
    j->sock = sock;
    j->port = sin->sin_port;
    FD_SET(j->sock, fdactive);
    DBG("Job #%d accepted Reply #%d %u.%u.%u.%u:%u\n",j->jobid,j->conid,ADDR2IP(addr),sin->sin_port);
  } else {
    shutdown(sock, 2);
    close(sock);
    DBG("Reply rejected from %u.%u.%u.%u:%u\n",ADDR2IP(addr),sin->sin_port);
  }
}
