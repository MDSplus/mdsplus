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
#include "../mdstcpip/mdsIo.h"

//#define DEBUG
#ifdef DEBUG
 #define DBG(...) fprintf(stderr,__VA_ARGS__)
#else
 #define DBG(...) do{/**/}while(0)
#endif

extern short ArgLen();

extern int GetConnectionVersion(int);
extern int GetAnswerInfoTS();

/* Job must be a subclass of Condition
 *typedef struct _Condition {
 *  pthread_cond_t  cond;
 *  pthread_mutex_t mutex;
 *  int             value;
 *} Condition;
 */
typedef struct client {
  int conid;
  int version;
  uint32_t addr;
  uint16_t port;
  SOCKET sock;
} Client;
typedef struct clientlist {
  int conid;
  int version;
  uint32_t addr;
  uint16_t port;
  SOCKET sock;
  struct clientlist* next;
} ClientList;
static ClientList *Clients = NULL;

typedef struct job {
  pthread_cond_t cond;
  pthread_mutex_t mutex;
  int value; //aka done
  pthread_rwlock_t *lock;
  int has_condition;
  int was_abandoned;
  int *retstatus;
  void (*ast) ();
  void *astparam;
  void (*before_ast) ();
  int jobid;
  Client * c;
  struct job* next;
} Job;
static fd_set fdactive;
static pthread_mutex_t jobs_mutex = PTHREAD_MUTEX_INITIALIZER;
#define LOCK_JOBS   pthread_mutex_lock(&jobs_mutex);pthread_cleanup_push((void*)pthread_mutex_unlock, &jobs_mutex)
#define UNLOCK_JOBS pthread_cleanup_pop(1)
static Job *Jobs = NULL;

static inline Client *new_client(int conid, uint32_t addr, uint16_t port) {
  ClientList *c;
  LOCK_JOBS;
  for (c = Clients; c && c->conid != conid; c = c->next) ;
  UNLOCK_JOBS;
  if (c) {
    DBG("reuse connection to %u.%u.%u.%u:%u\n",ADDR2IP(addr),port);
    return (Client*)c;
  } else {
    Client *new = (Client*) malloc(sizeof(Client));
    new->sock  = INVALID_SOCKET;
    new->conid = conid;
    new->addr  = addr;
    new->port  = port;
    new->version= GetConnectionVersion(conid);
    DBG("added connection to %u.%u.%u.%u:%u\n",ADDR2IP(addr),port);
    return new;
  }
}

static inline Client *add_client(Client *n) {
  ClientList *c;
  LOCK_JOBS;
  for (c = Clients; c && (Client*)c!=n ; c = c->next) ;
  if (!c) {
    ClientList *new = realloc(n,sizeof(ClientList));
    new->next  = NULL;
    for (c = Clients; c && c->next != 0; c = c->next) ;
    if (c)
      c->next = new;
    else
      Clients = new;
    n = (Client*)new;
    DBG("stored old client %d %u.%u.%u.%u:%u\n",n->conid,ADDR2IP(n->addr),n->port);
  }
  UNLOCK_JOBS;
  return n;
}

static int MonJob = 0;
static int JobId = 0;

extern void *GetConnectionInfo();
static inline SOCKET get_socket(Client * c) {
  size_t len;
  char *info_name = NULL;
  SOCKET readfd = INVALID_SOCKET;
  GetConnectionInfo(c->conid, &info_name, &readfd, &len);
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

static inline void free_job(Job *j) {
  if (j) {
    if (j->c->version>=MDSIP_VERSION_SERVER_JOBS)
      free(j->c);
    free(j);
  }
}
static inline void remove_job(Job *j) {
  free_job(pop_job(j));
}
static pthread_mutex_t job_conds = PTHREAD_MUTEX_INITIALIZER;
static void abandon(void*in) {
  Job *j = (Job*)in;
  if (j && j->has_condition) {
    j->was_abandoned = TRUE;
    DBG("Job #%d sync abandoned!\n",j->jobid);
  }
}
static void do_completion_ast_now(Job *j, int status, char* msg) {
  if (j->retstatus) {
    if (j->lock) pthread_rwlock_wrlock(j->lock);
    *j->retstatus = status;
    if (j->lock) pthread_rwlock_unlock(j->lock);
  }
  if (j->ast && j->astparam) j->ast(j->astparam, msg);
}
static void do_completion_ast(Job *j, int status, char* msg, int removeJob){
  if (j->c->version>=MDSIP_VERSION_SERVER_JOBS) {
    LOCK_JOBS;
    FD_CLR(j->c->sock, &fdactive);
    UNLOCK_JOBS;
  }
  /**** If job has a condition, ServerWait will not remove it. ***/
  if (j->has_condition){
    int abandoned = FALSE; // ensure we notice if client abandoned job
    pthread_mutex_lock(&job_conds);pthread_cleanup_push((void*)pthread_mutex_unlock, &job_conds);
    _CONDITION_LOCK(j);
    if (!(abandoned = j->was_abandoned))
      do_completion_ast_now(j,status,msg);
    j->value = TRUE;
    _CONDITION_SIGNAL(j);
    _CONDITION_UNLOCK(j);
    pthread_cleanup_pop(1);
    if (!abandoned) return;
  } else
    do_completion_ast_now(j,status,msg);
  DBG("Job #%d async done.\n",j->jobid);
  if (removeJob && j->jobid != MonJob)
    remove_job(j);
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
static void cleanup_job(int status, int jobid, int freejob){
  Job *j = pop_job_by_jobid(jobid);
  if (j) {
    DisconnectFromMds(j->c->conid);
    do_completion_ast(j, status, NULL, FALSE);
    if (j->c->sock != INVALID_SOCKET && j->c->version>=MDSIP_VERSION_SERVER_JOBS) {
      LOCK_JOBS;
      FD_SET(j->c->sock, &fdactive);
      UNLOCK_JOBS;
      shutdown(j->c->sock, 2);
      close(j->c->sock);
    }
    if (!j->has_condition || freejob)
      free_job(j);
  }
}

static inline void cleanup_sync_job(Job*j){
  _CONDITION_LOCK(j);
  pthread_cleanup_push(abandon,(void*)j);
  _CONDITION_WAIT_SET(j);
  pthread_cleanup_pop(0);
  _CONDITION_UNLOCK(j);
  CONDITION_DESTROY(j,&job_conds);
}

void ServerWait(int jobid) {
  Job *j;
  LOCK_JOBS;
  for (j = Jobs; j && (j->jobid != jobid); j=j->next);
  UNLOCK_JOBS;
  if (j) {
    if (!j->has_condition) {
      DBG("Job #%d async not waiting.\n",jobid);
      return;
    }
    DBG("Job #%d sync pending.\n",jobid);
    cleanup_sync_job(j);
    remove_job(j);
    DBG("Job #%d sync done.\n",jobid);
  } else
    DBG("Job #%d sync lost!\n",jobid);
}

static inline void do_before_ast(Job *j) {
  if (j->before_ast) j->before_ast(j->astparam);
}

#define minmax(l,x,u) ((l>x) ? l : ( (u<x) ? u : x ) )
static inline int send_request(Client *client, uint32_t addr, uint16_t port, int op, int flags, int jobid, int numargs, va_list *vlist) {
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
  DBG("sending to %d: %s\n",client->conid,cmd);
  return SendArg(client->conid, 0, DTYPE_CSTRING, 1, (short)strlen(cmd), 0, 0, cmd);
}
static inline int receive_answer(Client*client,int jobid,int op){
  char dtype;
  length_t len;
  char ndims;
  int dims[8];
  int numbytes;
  int *dptr;
  void *mem = NULL;
  int status = GetAnswerInfoTS(client->conid, &dtype, &len, &ndims, dims, &numbytes, (void **)&dptr, &mem);
  if (op==SrvStop) {
    if STATUS_NOT_OK {
      status = MDSplusSUCCESS;
      cleanup_job(status, jobid, FALSE);
    } else
      status = MDSplusERROR;
  } else {
    if STATUS_NOT_OK {
      perror("Error: no response from server");
      cleanup_job(status, jobid, TRUE);
      return status;
    }
  }
  if (mem)
    free(mem);
  return status;
}

static inline int register_job(int *msgid, int *retstatus, pthread_rwlock_t *lock, void (*ast) (), void *astparam, void (*before_ast) (), Client *c){
  int jobid;
  Job *j;
  LOCK_JOBS;
  j = (Job *) malloc(sizeof(Job));
  j->ast = ast;
  j->astparam = astparam;
  j->before_ast = before_ast;
  j->lock = lock;
  j->retstatus = retstatus;
  j->was_abandoned = FALSE;
  j->c = c;
  // jobid should not be 0 or MonJob
  if (++JobId == 0     ) ++JobId;
  if (  JobId == MonJob) ++JobId;
  jobid = JobId;
  j->jobid = c->version<MDSIP_VERSION_SERVER_JOBS ? jobid : 0;
  if (msgid) {
    CONDITION_INIT(j);
    j->has_condition = TRUE;
    *msgid = jobid;
    DBG("Job #%d sync  ",jobid);
  } else {
    j->has_condition = FALSE;
    j->value = TRUE;
    DBG("Job #%d async ",jobid);
  }
  DBG("registered on con %d with 0x%"PRIxPTR" %u.%u.%u.%u:%u\n",j->c->conid,(uintptr_t)j,ADDR2IP(j->c->addr),j->c->port);
  j->next = Jobs;
  Jobs = j;
  UNLOCK_JOBS;
  return jobid;
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
  int c_status = C_ERROR;
  int tries = 0;
  int one = 1;
  INITIALIZESOCKETS;
  SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == INVALID_SOCKET) {
    perror("Error getting Connection Socket\n");
    return INVALID_SOCKET;
  }
  setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&recvbuf, sizeof(long));
  setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&sendbuf, sizeof(long));
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(int));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  for (tries = 0 ; (c_status < 0) && (tries < 500); tries++) {
    port = start_port + (random()%range_port);
    sin.sin_port = htons(port);
    c_status = bind(sock, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
  }
  if (c_status < 0) {
    perror("Error binding to service\n");
    return INVALID_SOCKET;
  }
  c_status = listen(sock, 5);
  if (c_status < 0) {
    perror("Error from listen\n");
    return INVALID_SOCKET;
  }
  DBG("Listener socket %d opened on port %u.\n",sock,port);
  *port_out = port;
  return sock;
}

static int is_broken_socket(SOCKET socket){
  if (socket != INVALID_SOCKET) {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(socket, &fds);
    struct timeval timeout = {0,0}; // non-blocking
    if (select(socket+1, &fds, 0, 0, &timeout) >= 0)
      return send(socket, NULL, 0, MSG_DONTWAIT) != 0;
  }
  return TRUE;
}
static inline void reset_fdactive(int rep, SOCKET sock){
  Job *j, *p;
  LOCK_JOBS;
  if (rep > 0) {
    for (j = Jobs, p = NULL; j ; ) {
      if (is_broken_socket(j->c->sock)) {
	DBG("Job #%d removed by reset_fdactive\n",j->jobid);
        if (p) p->next = j->next;
        else  Jobs = j = j->next;
      } else {
        p = j;
        j = j->next;
      }
    }
  }
  FD_ZERO(&fdactive);
  FD_SET(sock, &fdactive);
  for (j = Jobs; j ; j = j->next) {
    if (j->c->sock != INVALID_SOCKET)
      FD_SET(j->c->sock, &fdactive);
  }
  UNLOCK_JOBS;
  DBG("reset fdactive\n");
}

////////////RECEIVER///////////////////////////////////////////////////////////
static void do_message(Job*j,int jobid,int replyType,int status,char*msg){
  switch (replyType) {
  case SrvJobFINISHED:
    DBG("Job #%d finished.\n",jobid);
    do_completion_ast(j, status, msg, TRUE);
    break;
  case SrvJobSTARTING:
    DBG("Job #%d starting.\n",jobid);
    do_before_ast(j);
    break;
  case SrvJobCHECKEDIN:
    DBG("Job #%d checked-in as 0x%"PRIxPTR".\n",jobid,(uintptr_t)j);
    break;
  default:
    DBG("Job #%d failure %d.\n",jobid,replyType);
    cleanup_job(MDSplusFATAL,jobid, FALSE);
  }
}
static void do_message_j(Job * j){
  char reply[60];
  int nbytes = recv(j->c->sock, reply, 60, 0);
  if (nbytes != 60) {
    DBG("Receiver header error %d != 60.\n",nbytes);
    cleanup_job(MDSplusFATAL,j->jobid, FALSE);
    return;
  }
  int jobid;
  int replyType;
  int status;
  int msglen;
  int num = sscanf(reply, "%d %d %d %d", &jobid, &replyType, &status, &msglen);
  if (num != 4) {
    DBG("Receiver header format error: '%s'.\n",reply);
    cleanup_job(MDSplusFATAL,j->jobid, FALSE);
    return;
  }
  if (j->jobid && j->jobid != jobid)
    fprintf(stderr,"######### Receiver jobid mismatch %d != %d.\n",jobid,j->jobid);
  INIT_AND_FREE_ON_EXIT(char*,msg);
  if (msglen != 0) {
    msg = (char *)malloc(msglen + 1);
    msg[msglen] = 0;
    nbytes = recv(j->c->sock, msg, msglen, 0);
    if (nbytes != msglen) {
      DBG("Job #%d msglen mismatch %d != %d.\n",jobid,nbytes,msglen);
      cleanup_job(MDSplusFATAL,jobid, FALSE);
      goto end;
    }
  }
  if (j->jobid != jobid) j->jobid = jobid;
  do_message(j,jobid,replyType,status,msg);
end: ;
  FREE_NOW(msg);
}
static void do_message_c(Client * c){
  char reply[60];
  int nbytes = recv(c->sock, reply, 60, 0);
  if (nbytes != 60) {
    DBG("Receiver header error %d != 60.\n",nbytes);
    return;
  }
  int jobid;
  int replyType;
  int status;
  int msglen;
  int num = sscanf(reply, "%d %d %d %d", &jobid, &replyType, &status, &msglen);
  if (num != 4) {
    DBG("Receiver header format error: '%s'.\n",reply);
    return;
  }
  INIT_AND_FREE_ON_EXIT(char*,msg);
  if (msglen != 0) {
    msg = (char *)malloc(msglen + 1);
    msg[msglen] = 0;
    nbytes = recv(c->sock, msg, msglen, 0);
    if (nbytes != msglen) {
      DBG("Job #%d msglen mismatch %d != %d.\n",jobid,nbytes,msglen);
      cleanup_job(MDSplusFATAL,jobid, FALSE);
      goto end;
    }
  }
  Job *j;
  LOCK_JOBS;
  for (j = Jobs; j && j->jobid != jobid; j=j->next);
  UNLOCK_JOBS;
  if (j) {
    j->c->sock  = c->sock;
    do_message(j,jobid,replyType,status,msg);
  }
end: ;
  FREE_NOW(msg);
}

static inline Job *accept_reply_j(SOCKET sock, struct sockaddr_in *sin) {
  Job *j;
  LOCK_JOBS;
  const uint32_t addr = *(uint32_t *)&sin->sin_addr;
  for (j = Jobs; j && (j->c->addr != addr || j->c->sock != INVALID_SOCKET);j = j->next);
  if (j) {
    j->c->sock = sock;
    if (j->c->version>=MDSIP_VERSION_SERVER_JOBS)
      do_message_j(j);
    FD_SET(sock, &fdactive);
    DBG("Job #%d accepted Reply #%d %u.%u.%u.%u:%u\n",j->jobid,j->c->conid,ADDR2IP(sin->sin_addr),sin->sin_port);
  }
  UNLOCK_JOBS;
  return j;
}
static inline Client *accept_reply_c(SOCKET sock, struct sockaddr_in *sin) {
  ClientList *c;
  LOCK_JOBS;
  const uint32_t addr = *(uint32_t *)&sin->sin_addr;
  for (c = Clients; c && (c->addr != addr || c->sock != INVALID_SOCKET);c = c->next);
  if (c) {
    c->sock = sock;
    FD_SET(sock, &fdactive);
    DBG("Client #%d accepted %u.%u.%u.%u:%u\n",c->conid,ADDR2IP(sin->sin_addr),sin->sin_port);
  }
  UNLOCK_JOBS;
  return  (Client*)c;
}
static inline void accept_reply(SOCKET sock, struct sockaddr_in *sin) {
  if (sock == INVALID_SOCKET) {
    SOCKERROR("accept returned invalid socket with errno %d",errno);
    if (errno==ENOTSOCK) abort();
    sleep(60);
    return;
  }
  if (accept_reply_j(sock,sin)||accept_reply_c(sock,sin)) return;
  shutdown(sock, 2);
  close(sock);
  DBG("Reply rejected from %u.%u.%u.%u:%u\n",ADDR2IP(sin->sin_addr),sin->sin_port);
}
static inline Job *lookup_job(fd_set *fds) {
  Job *j;
  LOCK_JOBS;
  for (j = Jobs;
       j && (j->c->sock == INVALID_SOCKET || !FD_ISSET(j->c->sock, fds));
       j = j->next);
  UNLOCK_JOBS;
  return j;
}
static inline Client *lookup_client(fd_set *fds) {
  ClientList *c;
  LOCK_JOBS;
  for (c = Clients;
       c && (c->sock == INVALID_SOCKET || !FD_ISSET(c->sock, fds));
       c = c->next);
  UNLOCK_JOBS;
  return (Client*)c;
}

static Condition receiver_running = CONDITION_INITIALIZER;
static void receiver_exit(void *arg __attribute__ ((unused))){
  DBG("ServerSendMessage thread exitted\n");
  CONDITION_RESET(&receiver_running);
}
static inline void fd_copy(fd_set*dst, fd_set*src) {
  LOCK_JOBS;
  *dst = *src;
  UNLOCK_JOBS;
}
static void receiver_thread(void *sockptr){
  SOCKET sock;
  LOCK_JOBS;
  sock = *(SOCKET*)sockptr;
  FD_ZERO(&fdactive);
  FD_SET(sock, &fdactive);
  UNLOCK_JOBS;
  atexit((void*)receiver_exit);
  fd_set readfds;
  struct sockaddr_in sin;
  struct {
    int jobid;
    uint32_t addr;
    uint16_t port;
  } last;memset(&last, 0, sizeof(Job));
  int rep;
  CONDITION_SET_TO(&receiver_running,2);
  DBG("Receiver Thread using socket %d\n",sock);
  struct timeval readto, timeout = {1,0};
  for (rep = 0; rep < 10; rep++) {
    for (;;) {
      fd_copy(&readfds,&fdactive);
      readto = timeout;
      int num = select(FD_SETSIZE, &readfds, NULL, NULL, &readto);
      if (num <0) break;
      if (num==0) continue;
      rep = 0;
      if (FD_ISSET(sock, &readfds)) {
        socklen_t len = sizeof(struct sockaddr_in);
        accept_reply(accept(sock, (struct sockaddr *)&sin, &len), &sin);
      }
      for (;;) { // check clients first
        Client *c = lookup_client(&readfds);
        if (!c) break;
        DBG("Reply from %u.%u.%u.%u:%u\n",ADDR2IP(c->addr),c->port);
        last.jobid = 0; last.addr=c->addr; last.port=c->port;
        FD_CLR(c->sock, &readfds);
        do_message_c(c);
      }
      for (;;) { // we are left with new gen clients
        Job *j = lookup_job(&readfds);
        if (!j) break;
        DBG("Reply from %u.%u.%u.%u:%u\n",ADDR2IP(j->c->addr),j->c->port);
        last.jobid=j->jobid; last.addr=j->c->addr; last.port=j->c->port;
        FD_CLR(j->c->sock, &readfds);
        do_message_j(j);
      }
    }
    SOCKERROR("Dispatcher select loop failed\nLast Job #%d: %u.%u.%u.%u:%u\n",last.jobid,ADDR2IP(last.addr),last.port);
    reset_fdactive(rep, sock);
  }
  fprintf(stderr,"Cannot recover from select errors in ServerSendMessage, exitting\n");
  pthread_exit(0);
}
static int start_receiver(uint16_t *port_out){
  static uint16_t port = 0;
  static SOCKET sock = INVALID_SOCKET;
  static pthread_t thread;
  _CONDITION_LOCK(&receiver_running);
  if (port == 0 || sock == INVALID_SOCKET) {
    sock = create_port(&port);
    if (sock == INVALID_SOCKET) {
      perror("Error creating listener socket");
      _CONDITION_UNLOCK(&receiver_running);
      return C_ERROR;
    }
  }
  do {
    if (!receiver_running.value) {
      CREATE_DETACHED_THREAD(thread, *16, receiver_thread, &sock);
      if (c_status) {
        perror("Error creating pthread");
        _CONDITION_UNLOCK(&receiver_running);
        return C_ERROR;
      } else
        receiver_running.value = 1;
    }
    while (receiver_running.value==1)
      _CONDITION_WAIT(&receiver_running);
  } while(receiver_running.value<2);
  _CONDITION_UNLOCK(&receiver_running);
  *port_out = port;
  return C_OK;
}
////////////RECEIVER///////////////////////////////////////////////////////////

static inline Client* get_addr_port(char* server, uint32_t *addr_o, uint16_t *port_o) {
  char hostpart[256] = { 0 };
  char portpart[256] = { 0 };
  int num = sscanf(server, "%[^:]:%s", hostpart, portpart);
  if (num != 2) {
    DBG("Server '%s' unknown\n", server);
    return NULL;
  }
  uint32_t addr = LibGetHostAddr(hostpart);
  if (!addr) {
    DBG("Error resolving host '%s' to ip address\n",hostpart);
    return NULL;
  }
  uint16_t port;
  if ((port = atoi(portpart)) == 0) {
    struct servent *sp = getservbyname(portpart, "tcp");
    if (sp)
      port = sp->s_port;
    else {
      char *portnam = getenv(portpart);
      portnam = (!portnam) ? ((hostpart[0] == '_') ? "8200" : "8000") : portnam;
      port = (uint16_t)atoi(portnam);
    }
  }
  DBG("Resolved '%s' to %u.%u.%u.%u:%u\n",server,ADDR2IP(addr),port);
  ClientList *c;
  LOCK_JOBS;
  for (c = Clients; c && (c->addr != addr || c->port != port); c = c->next) ;
  UNLOCK_JOBS;
  if (addr_o) *addr_o = addr;
  if (port_o) *port_o = port;
  return (Client*)c;
}
static Client* server_connect(char* server_in) {
  Client *c;
  INIT_AND_FREE_ON_EXIT(char*,srv);
  srv = TranslateLogical(server_in);
  char *server = srv ? srv : server_in;
  uint32_t addr = 0;
  uint16_t port = 0;
  c = get_addr_port(server,&addr,&port);
  if (!c) {
    int conid = ConnectToMds(server);
    if (conid >=0) {
      c = new_client(conid,addr,port);
      if (GetConnectionVersion(conid) < MDSIP_VERSION_SERVER_JOBS)
        c = add_client(c);
    }
  }
  FREE_NOW(srv);
  return c;
}

EXPORT int ServerConnect(char *server_in) {
  Client *c = server_connect(server_in);
  if (c) return c->conid;
  return -1;
}
EXPORT int ServerDisconnect(char *server_in) {
  Client *c;
  INIT_AND_FREE_ON_EXIT(char*,srv);
  srv = TranslateLogical(server_in);
  char *server = srv ? srv : server_in;
  c = get_addr_port(server,NULL,NULL);
  FREE_NOW(srv);
  if (c)
    return DisconnectFromMds(c->conid);
  return MDSplusSUCCESS;
}

int ServerSendMessage(int *msgid, char *server, int op, int *retstatus, pthread_rwlock_t *lock, int *conid_out,
		      void (*ast) (), void *astparam, void (*before_ast) (), int numargs, ...){
  uint16_t port = 0;
  if (start_receiver(&port)) {
    if (ast && astparam)
      ast(astparam);
    return MDSplusFATAL;
  }
  Client * client = server_connect(server);
  if (!client) {
    if (ast && astparam)
      ast(astparam);
    return ServerPATH_DOWN;
  }
  INIT_STATUS;
  int flags = 0;
  int jobid;
  uint32_t addr = 0;
  if (conid_out)
    *conid_out = client->conid;
  SOCKET sock = get_socket(client);
  struct sockaddr_in sin = {0};
  socklen_t slen = sizeof(sin);
  if (getsockname(sock, (struct sockaddr *)&sin, &slen) == 0)
    addr = *(uint32_t *)&sin.sin_addr;
  if (!addr) {
    perror("Error getting the address the socket is bound to.\n");
    DisconnectFromMds(client->conid);
    if (ast && astparam)
      ast(astparam);
    return ServerSOCKET_ADDR_ERROR;
  }
  jobid = register_job(msgid, retstatus, lock, ast, astparam, before_ast, client);
  //if (before_ast)
    flags |= SrvJobBEFORE_NOTIFY;
  va_list vlist;
  va_start(vlist, numargs);
  status = send_request(client, addr, port, op, flags, jobid, minmax(0,numargs,8), &vlist);
  if STATUS_NOT_OK {
    perror("Error sending message to server");
    cleanup_job(status, jobid, TRUE);
    if (client->version>=MDSIP_VERSION_SERVER_JOBS)
      DisconnectFromMds(client->conid);
    return status;
  }
  status = receive_answer(client,jobid,op);
  if STATUS_NOT_OK {
    perror("Error receiving answer from server");
    cleanup_job(status, jobid, TRUE);
    if (client->version>=MDSIP_VERSION_SERVER_JOBS)
      DisconnectFromMds(client->conid);
    return status;
  }
#ifdef DEBUG
  Job * j;
  LOCK_JOBS;
  for (j = Jobs; j && j->jobid != jobid; j=j->next);
  UNLOCK_JOBS;
  DBG("Job #%d linked with 0x%"PRIxPTR"\n",jobid,(uintptr_t)j);
#endif
  if (client->version>=MDSIP_VERSION_SERVER_JOBS)
     DisconnectFromMds(client->conid);
  return status;
}
