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
#include <stdlib.h>
#include <stdio.h>
#ifdef HAVE_UNISTD_H
  #include <unistd.h>
#endif
#include <string.h>
#include <servershr.h>
#include <mds_stdarg.h>
#include <mdsshr.h>
#define _NO_SERVER_SEND_MESSAGE_PROTO
#include "servershrp.h"
#include <stdio.h>
#include <errno.h>
#ifdef _WIN32
 typedef int socklen_t;
 #define random rand
 #define close closesocket
 #define SOCKERROR(...) do{errno = WSAGetLastError();fprintf (stderr, __VA_ARGS__);}while (0)
#else
 #define SOCKERROR(...) fprintf (stderr, __VA_ARGS__)
 #include <netinet/in.h>
 #include <netdb.h>
 #include <arpa/inet.h>
#endif
#include <signal.h>

#define DEBUG(...) //printf(__VA_ARGS__)
#define IP(addr)   ((uint8_t*)&addr)
#define ADDR2IP(a) IP(a)[0],IP(a)[1],IP(a)[2],IP(a)[3]


extern short ArgLen();

extern int GetAnswerInfoTS();

/* Job must be a subclass of Condition
 *typedef struct _Condition {
 *  pthread_cond_t  cond;
 *  pthread_mutex_t mutex;
 *  int             value;
 *} Condition;
 */
typedef struct _Job {
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
  struct _Job *next;
} Job;
static pthread_mutex_t jobs_mutex = PTHREAD_MUTEX_INITIALIZER;
#define LOCK_JOBS   pthread_mutex_lock(&jobs_mutex);pthread_cleanup_push((void*)pthread_mutex_unlock, &jobs_mutex)
#define UNLOCK_JOBS pthread_cleanup_pop(1)
static Job *Jobs = NULL;

typedef struct _client {
  SOCKET reply_sock;
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


#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

int is_broken_socket(SOCKET socket);

static int start_receiver(uint16_t *port);
int ServerConnect(char *server);
static int RegisterJob(int *msgid, int *retstatus, pthread_rwlock_t *lock, void (*ast) (), void *astparam,
		       void (*before_ast) (), int sock);
static void CleanupJob(int status, int jobid);
static void ReceiverThread(void *sockptr);
static void DoMessage(Client * c, fd_set * fdactive);
static void RemoveClient(Client * c, fd_set * fdactive);
static uint32_t GetHostAddr(char *host);
static void AddClient(uint32_t addr, uint16_t port, int send_sock);
static void AcceptClient(SOCKET reply_sock, struct sockaddr_in *sin, fd_set * fdactive);

extern void *GetConnectionInfo();
static SOCKET getSocket(int conid)
{
  size_t len;
  char *info_name;
  SOCKET readfd;
  GetConnectionInfo(conid, &info_name, &readfd, &len);
  return (info_name && strcmp(info_name, "tcp") == 0) ? readfd : INVALID_SOCKET;
}

int ServerSendMessage(int *msgid, char *server, int op, int *retstatus, pthread_rwlock_t *lock, int *conid_out,
		      void (*ast) (), void *astparam, void (*before_ast) (), int numargs_in, ...)
{
  uint16_t port = 0;
  int conid;
  if (start_receiver(&port) || ((conid = ServerConnect(server)) < 0)) {
    if (ast && astparam)
      ast(astparam);
    return ServerPATH_DOWN;
  }
  INIT_STATUS;
  int flags = 0;
  int jobid;
  int i;
  uint32_t addr = 0;
  char cmd[4096];
  unsigned char numargs = max(0, min(numargs_in, 8));
  unsigned char idx = 0;
  char dtype;
  char ndims;
  int dims[8];
  int numbytes;
  int *dptr;
  va_list vlist;
  void *mem = NULL;
  struct descrip *arg;
  if (conid_out)
    *conid_out = conid;
  int sock = getSocket(conid);
  struct sockaddr_in addr_struct = {0};
  socklen_t len = sizeof(addr_struct);
  if (getsockname(sock, (struct sockaddr *)&addr_struct, &len) == 0)
    addr = *(uint32_t *)&addr_struct.sin_addr;
  if (!addr) {
    perror("Error getting the address the socket is bound to.\n");
    if (ast && astparam)
      ast(astparam);
    return ServerSOCKET_ADDR_ERROR;
  }
  jobid = RegisterJob(msgid, retstatus, lock, ast, astparam, before_ast, conid);
  if (before_ast)
    flags |= SrvJobBEFORE_NOTIFY;
  sprintf(cmd, "MdsServerShr->ServerQAction(%ulu,%uwu,%d,%d,%d", addr, port, op, flags, jobid);
  va_start(vlist, numargs_in);
  for (i = 0; i < numargs; i++) {
    strcat(cmd, ",");
    arg = va_arg(vlist, struct descrip *);
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
    }
  }
  strcat(cmd, ")");
  status = SendArg(conid, idx++, DTYPE_CSTRING, 1, (short)strlen(cmd), 0, 0, cmd);
  if STATUS_NOT_OK {
      perror("Error sending message to server");
      CleanupJob(status, jobid);
      return status;
  }
  status = GetAnswerInfoTS(conid, &dtype, &len, &ndims, dims, &numbytes, (void **)&dptr, &mem);
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

static void RemoveJob(Job *j){
  LOCK_JOBS;
  Job *jj, *prev;
  for (jj = Jobs, prev = NULL; jj && jj != j; prev=jj, jj=jj->next);
  if (jj) {
    if (prev) prev->next = j->next;
    else      Jobs = j->next;
    if (!jj->has_condition)
      free(jj);
  }
  UNLOCK_JOBS;
}


static pthread_mutex_t job_conds = PTHREAD_MUTEX_INITIALIZER;
static void DoCompletionAst(Job *j, int status, char *msg, int removeJob){
  int has_condition = j->has_condition;
  if (j->lock) pthread_rwlock_wrlock(j->lock);
  if (j->retstatus)
    *j->retstatus = status;
  if (j->lock) pthread_rwlock_unlock(j->lock);
  if (j->ast)
    (*j->ast) (j->astparam, msg);
  if (removeJob && j->jobid != MonJob)
    RemoveJob(j);
  /**** If job has a condition, RemoveJob will not remove it. ***/
  if (has_condition){
    pthread_mutex_lock(&job_conds);pthread_cleanup_push((void*)pthread_mutex_unlock, &job_conds);
    CONDITION_SET(j);
    pthread_cleanup_pop(1);
  }
}

static void DoCompletionAstId(int jobid, int status, char *msg, int removeJob){
  Job *j;
  LOCK_JOBS;
  for (j=Jobs ; j && (j->jobid != jobid); j=j->next);
  if (!j) for (j=Jobs ; j && (j->jobid != MonJob); j=j->next);
  UNLOCK_JOBS;
  if (j) DoCompletionAst(j, status, msg, removeJob);
}

void ServerWait(int jobid)
{
  Job *j;
  LOCK_JOBS;
  for (j = Jobs; j && (j->jobid != jobid); j = j->next) ;
  UNLOCK_JOBS;
  if (j && j->has_condition) {
    CONDITION_WAIT_SET(j);
    CONDITION_DESTROY(j,&job_conds);
    free(j);
  }
}

static void DoBeforeAst(int jobid)
{
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
  if (before_ast)
    before_ast(astparam);
}

static int RegisterJob(int *msgid, int *retstatus, pthread_rwlock_t *lock, void (*ast) (), void *astparam,
		       void (*before_ast) (), int conid)
{
  Job *j = (Job *) malloc(sizeof(Job));
  j->retstatus = retstatus;
  j->lock = lock;
  j->ast = ast;
  j->astparam = astparam;
  j->before_ast = before_ast;
  j->conid = conid;
  LOCK_JOBS;
  j->jobid = ++JobId;
  if (msgid) {
    CONDITION_INIT(j);
    j->has_condition = B_TRUE;
    *msgid = j->jobid;
  } else {
    j->has_condition = B_FALSE;
    j->value = B_TRUE;
  }
  j->next = Jobs;
  Jobs = j;
  UNLOCK_JOBS;
  return j->jobid;
}

static void CleanupJob(int status, int jobid)
{
  Job *j;
  int conid;
  LOCK_JOBS;
  for (j=Jobs; j && (j->jobid != jobid) ; j=j->next);
  UNLOCK_JOBS;
  if (j) {
    conid = j->conid;
    DisconnectFromMds(conid);
    DoCompletionAst(j, status, 0, 1);
    for (;;) {
      LOCK_JOBS;
      for (j=Jobs ; j && (j->conid != conid) ; j=j->next);
      UNLOCK_JOBS;
      if (j)
        DoCompletionAst(j, status, 0, 1);
      else break;
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
    DEBUG("Receiver will be using 'MDSIP_PORT_RANGE=%u-%u'.\n",start_port,start_port+range_port-1);
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
  DEBUG("Listener opened on port %u.\n",port);
  *port_out = port;
  return s;
}

static Condition ReceiverRunning = CONDITION_INITIALIZER;

static int start_receiver(uint16_t *port_out)
{
  INIT_STATUS;
  static uint16_t port = 0;
  static SOCKET sock;
  static pthread_t thread;
// CONDITION_START_THREAD(&ReceiverRunning, thread, *16, ReceiverThread, s);
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
//\CONDITION_START_THREAD(&ReceiverRunning, thread, *16, ReceiverThread, &sock);
  *port_out = port;
  return STATUS_NOT_OK;
}

static void ReceiverExit(void *arg __attribute__ ((unused))){
  DEBUG("ServerSendMessage thread exitted\n");
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
      if (is_broken_socket(c->reply_sock)) {
	DEBUG("removed client in ResetFdactive\n");
        _RemoveClient(c);
        c = Clients;
      } else
        c = c->next;
    }
  }
  FD_ZERO(active);
  FD_SET(sock, active);
  for (c = Clients; c; c = c->next) {
    if (c->reply_sock != INVALID_SOCKET)
      FD_SET(c->reply_sock, active);
  }
  UNLOCK_CLIENTS;
  DEBUG("reset fdactive in ResetFdactive\n");
}

static void ReceiverThread(void *sockptr){
  atexit((void*)ReceiverExit);
  CONDITION_SET(&ReceiverRunning);
// CONDITION_SET(&ReceiverRunning);
  _CONDITION_LOCK(&ReceiverRunning);
  SOCKET sock = *(SOCKET*)sockptr;
  ReceiverRunning.value = B_TRUE;
  _CONDITION_SIGNAL(&ReceiverRunning);
  _CONDITION_UNLOCK(&ReceiverRunning);
  CONDITION_SET(&ReceiverRunning);
// \CONDITION_SET(&ReceiverRunning);
  struct sockaddr_in sin;
  uint32_t last_client_addr = 0;
  uint16_t last_client_port = 0;
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
      if (FD_ISSET(sock, &readfds)) {
          socklen_t len = sizeof(struct sockaddr_in);
          AcceptClient(accept(sock, (struct sockaddr *)&sin, &len), &sin, &fdactive);
      } else {
        Client *c, *next;
        for (;;) {
          LOCK_CLIENTS;
          for (c = Clients, next = c ? c->next : 0;
               c && (c->reply_sock == INVALID_SOCKET || !FD_ISSET(c->reply_sock, &readfds));
               c = next, next = c ? c->next : 0);
          UNLOCK_CLIENTS;
          if (c) {
            SOCKET reply_sock = c->reply_sock;
            last_client_addr = c->addr;
            last_client_port = c->port;
            DoMessage(c, &fdactive);
            FD_CLR(reply_sock, &readfds);
          } else
            break;
        }
      }
    }
    SOCKERROR("Dispatcher select loop failed\nLast client addr = %u.%u.%u.%u:%u\n",ADDR2IP(last_client_addr),last_client_port);
    ResetFdactive(rep, sock, &fdactive);
  }
  fprintf(stderr,"Cannot recover from select errors in ServerSendMessage, exitting\n");
  pthread_exit(0);
}

int is_broken_socket(SOCKET socket)
{
  if (socket != INVALID_SOCKET) {
    fd_set fdactive;
    FD_ZERO(&fdactive);
    FD_SET(socket, &fdactive);
    struct timeval timeout = {0,0}; // non-blocking
    return select(socket+1, &fdactive, 0, 0, &timeout) < 0;
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
    DEBUG("Server '%s' unknown\n", server);
    return NULL;
  }
  addr = GetHostAddr(hostpart);
  if (!addr) return NULL;
  if (atoi(portpart) == 0) {
    struct servent *sp = getservbyname(portpart, "tcp");
    if (sp)
      port = sp->s_port;
    else {
      char *portnam = getenv(portpart);
      portnam = (!portnam) ? ((hostpart[0] == '_') ? "8200" : "8000") : portnam;
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

static void DoMessage(Client * c, fd_set * fdactive)
{
  char reply[60];
  char *msg = 0;
  int jobid;
  int replyType;
  int status;
  int msglen;
  int num;
  int nbytes;
  nbytes = recv(c->reply_sock, reply, 60, 0);
  if (nbytes != 60) {
    RemoveClient(c, fdactive);
    return;
  }
  num = sscanf(reply, "%d %d %d %d", &jobid, &replyType, &status, &msglen);
  if (num != 4) {
    RemoveClient(c, fdactive);
    return;
  }
  FREE_ON_EXIT(msg);
  if (msglen != 0) {
    msg = (char *)malloc(msglen + 1);
    msg[msglen] = 0;
    nbytes = recv(c->reply_sock, msg, msglen, 0);
    if (nbytes != msglen) {
      free(msg);
      RemoveClient(c, fdactive);
      return;
    }
  }
  switch (replyType) {
  case SrvJobFINISHED:
    DoCompletionAstId(jobid, status, msg, 1);
    break;
  case SrvJobSTARTING:
    DoBeforeAst(jobid);
    break;
  case SrvJobCHECKEDIN:
    break;
  default:
    RemoveClient(c, fdactive);
  }
  FREE_NOW(msg);
}


static int get_client_conid(Client * c, fd_set * fdactive) {
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
    if (c->reply_sock != INVALID_SOCKET) {
      shutdown(c->reply_sock, 2);
      close(c->reply_sock);
      if (fdactive)
	FD_CLR(c->reply_sock, fdactive);
    }
    if (c->conid >= 0)
      DisconnectFromMds(c->conid);
    free(c);
    return conid;
  }
  return -1;
}

static void RemoveClient(Client * c, fd_set * fdactive) {
  int conid = get_client_conid(c,fdactive);
  Job *j;
  for (;;) {
    LOCK_JOBS;
    for (j = Jobs; j && (j->conid != conid) ; j = j->next);
    UNLOCK_JOBS;
    if (j) {
      DoCompletionAst(j, ServerPATH_DOWN, NULL, 1);
      RemoveJob(j);
    } else break;
  }
}

static uint32_t GetHostAddr(char *name)
{
  INITIALIZESOCKETS;
  uint32_t addr = 0;
#if defined(__MACH__) || defined(_WIN32)
  struct hostent* hp = gethostbyname(name);
  addr = hp ? *(int *)hp->h_addr_list[0] : (int)inet_addr(name);
#else
  size_t memlen;
  struct hostent hostbuf, *hp = NULL;
  int herr;
  char *hp_mem = NULL;
  FREE_ON_EXIT(hp_mem);
  for ( memlen=1024, hp_mem=malloc(memlen);
	hp_mem && (gethostbyname_r(name,&hostbuf,hp_mem,memlen,&hp,&herr) == ERANGE);
	memlen *= 2, free(hp_mem), hp_mem = malloc(memlen));
  addr = hp ? *(uint32_t *)hp->h_addr_list[0] : (uint32_t)inet_addr(name);
  FREE_NOW(hp_mem);
#endif
  return addr == 0xffffffff ? 0 : addr;
}

static void AddClient(unsigned int addr, uint16_t port, int conid)
{
  Client *c;
  Client *new = (Client *) malloc(sizeof(Client));
  new->reply_sock = INVALID_SOCKET;
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
  DEBUG("added connection from %u.%u.%u.%u\n",ADDR2IP(addr));
}

static void AcceptClient(SOCKET reply_sock, struct sockaddr_in *sin, fd_set * fdactive)
{
  if (reply_sock == INVALID_SOCKET) return;
  uint32_t addr = *(uint32_t *)&sin->sin_addr;
  Client *c;
  LOCK_CLIENTS;
  for (c = Clients; c && (c->addr != addr || c->reply_sock != INVALID_SOCKET);c = c->next);
  UNLOCK_CLIENTS;
  if (c) {
    c->reply_sock = reply_sock;
    FD_SET(reply_sock, fdactive);
    DEBUG("accepted connection from %u.%u.%u.%u\n",ADDR2IP(addr));
  } else {
    shutdown(reply_sock, 2);
    close(reply_sock);
    DEBUG("dropped connection from %u.%u.%u.%u\n",ADDR2IP(addr));
  }
}
