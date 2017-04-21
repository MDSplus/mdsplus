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
#include <config.h>
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
#ifdef _WIN32
 typedef int socklen_t;
 #include <windows.h>
 #define random rand
 #define close closesocket
#else
 #define INVALID_SOCKET -1
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <netdb.h>
 #include <arpa/inet.h>
#endif
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>

#define HAS_CONDITION 987654
#define NOT_DONE 1212121

#if (defined(_DECTHREADS_) && (_DECTHREADS_ != 1)) || !defined(_DECTHREADS_)
  #define pthread_attr_default NULL
  #define pthread_mutexattr_default NULL
  #define pthread_condattr_default NULL
#else
  #undef select
#endif

extern short ArgLen();

extern int GetAnswerInfoTS();

typedef struct _Job {
  int has_condition;
  pthread_cond_t condition;
  pthread_mutex_t mutex;
  int done;
  int marked_for_delete;
  int *retstatus;
  void (*ast) ();
  void *astparam;
  void (*before_ast) ();
  int jobid;
  int conid;
  struct _Job *next;
} Job;

typedef struct _client {
  SOCKET reply_sock;
  int conid;
  unsigned int addr;
  short port;
  struct _client *next;
} Client;

static Job *Jobs = 0;
static int MonJob = -1;
static int JobId = 0;
static Client *ClientList = 0;

int ServerBadSocket(SOCKET socket);

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

static int StartReceiver(short *port);
int ServerConnect(char *server);
static int RegisterJob(int *msgid, int *retstatus, void (*ast) (), void *astparam,
		       void (*before_ast) (), int sock);
static void CleanupJob(int status, int jobid);
static void *Worker(void *sockptr);
static void DoMessage(Client * c, fd_set * fdactive);
static void RemoveClient(Client * c, fd_set * fdactive);
static unsigned int GetHostAddr(char *host);
static void AddClient(unsigned int addr, short port, int send_sock);
static void AcceptClient(SOCKET reply_sock, struct sockaddr_in *sin, fd_set * fdactive);
static void lock_client_list();
static void unlock_client_list();
static void lock_job_list();
static void unlock_job_list();
extern int pthread_cond_timedwait();

static void InitializeSockets()
{
#ifdef _WIN32
  static int initialized = 0;
  if (!initialized) {
    WSADATA wsaData;
    WORD wVersionRequested;
    wVersionRequested = MAKEWORD(1, 1);
    WSAStartup(wVersionRequested, &wsaData);
    initialized = 1;
  }
#endif
}



extern void *GetConnectionInfo();
static SOCKET getSocket(int conid)
{
  size_t len;
  char *info_name;
  SOCKET readfd;
  GetConnectionInfo(conid, &info_name, &readfd, &len);
  return (info_name && strcmp(info_name, "tcp") == 0) ? readfd : INVALID_SOCKET;
}

int ServerSendMessage(int *msgid, char *server, int op, int *retstatus, int *conid_out,
		      void (*ast) (), void *astparam, void (*before_ast) (), int numargs_in, ...)
{
  short port = 0;
  int conid;
  if (!StartReceiver(&port) || ((conid = ServerConnect(server)) < 0)) {
    if (ast)
      ast(astparam);
    return ServerPATH_DOWN;
  }
  INIT_STATUS;
  int flags = 0;
  int jobid;
  int i;
  unsigned int addr = 0;
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
    addr = *(int *)&addr_struct.sin_addr;
  if (!addr) {
    perror("Error getting the address the socket is bound to.\n");
    if (ast)
      ast(astparam);
    return ServerSOCKET_ADDR_ERROR;
  }
  jobid = RegisterJob(msgid, retstatus, ast, astparam, before_ast, conid);
  if (before_ast)
    flags |= SrvJobBEFORE_NOTIFY;
  sprintf(cmd, "MdsServerShr->ServerQAction(%d,%dwu,%d,%d,%d", addr, port, op, flags, jobid);
  va_start(vlist, numargs_in);
  for (i = 0; i < numargs; i++) {
    strcat(cmd, ",");
    arg = va_arg(vlist, struct descrip *);
    if (op == SrvMonitor && numargs == 8 && i == 5 && arg->dtype == DTYPE_LONG
        && *(int *)arg->ptr == MonitorCheckin)
      MonJob = jobid;
    switch (arg->dtype) {
      case DTYPE_CSTRING: {
        int j;
        int k;
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
        printf("shouldn't get here! ServerSendMessage dtype = %d\n", arg->dtype);
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
  if STATUS_NOT_OK
      perror("Error: no response from server");
  if (mem)
    free(mem);
  return status;
}

static void RemoveJob(Job * job)
{
  Job *j, *prev;
  lock_job_list();
  for (j = Jobs, prev = 0; j && j != job; prev = j, j = j->next) ;
  if (j) {
    if (prev)
      prev->next = j->next;
    else
      Jobs = j->next;
    if (j->has_condition != HAS_CONDITION)
      free(j);
  }
  unlock_job_list();
}

static void DoCompletionAst(int jobid, int status, char *msg, int removeJob)
{
  Job *j;
  lock_job_list();
  for (j = Jobs; j && (j->jobid != jobid); j = j->next) ;
  unlock_job_list();
  if (!j) {
    lock_job_list();
    for (j = Jobs; j && (j->jobid != MonJob); j = j->next) ;
    unlock_job_list();
  }
  if (j) {
    int has_condition = j->has_condition == HAS_CONDITION;
    if (j->retstatus)
      *j->retstatus = status;
    if (j->ast)
      (*j->ast) (j->astparam, msg);
    if (removeJob && j->jobid != MonJob)
      RemoveJob(j);
    /**** If job has a condition, RemoveJob will not remove it. ***/
    if (has_condition) {
      pthread_mutex_lock(&j->mutex);
      j->done = 1;
      pthread_cond_signal(&j->condition);
      pthread_mutex_unlock(&j->mutex);
    }
  }
}

void ServerWait(int jobid)
{
  Job *j;
  lock_job_list();
  for (j = Jobs; j && (j->jobid != jobid); j = j->next) ;
  unlock_job_list();
  if (j) {
    if (j->has_condition == HAS_CONDITION) {
      while ((pthread_mutex_lock(&j->mutex) == 0)) {
	if (j->done == NOT_DONE)
	  pthread_cond_wait(&j->condition, &j->mutex);
	pthread_mutex_unlock(&j->mutex);
	if (j->done != NOT_DONE)
	  break;
      }
      pthread_mutex_lock(&j->mutex);
      pthread_cond_destroy(&j->condition);
      pthread_mutex_unlock(&j->mutex);
      pthread_mutex_destroy(&j->mutex);
      free(j);
    }
  }
}

static void DoBeforeAst(int jobid)
{
  Job *j;
  lock_job_list();
  for (j = Jobs; j && (j->jobid != jobid); j = j->next) ;
  unlock_job_list();
  if (j) {
    if (j->before_ast)
      (*j->before_ast) (j->astparam);
  }
}

static int RegisterJob(int *msgid, int *retstatus, void (*ast) (), void *astparam,
		       void (*before_ast) (), int conid)
{
  Job *j = (Job *) malloc(sizeof(Job));
  j->retstatus = retstatus;
  j->ast = ast;
  j->astparam = astparam;
  j->before_ast = before_ast;
  j->marked_for_delete = 0;
  j->conid = conid;
  lock_job_list();
  j->jobid = ++JobId;
  if (msgid) {
    pthread_mutex_init(&j->mutex, pthread_mutexattr_default);
    pthread_cond_init(&j->condition, pthread_condattr_default);
    j->has_condition = HAS_CONDITION;
    j->done = NOT_DONE;
    *msgid = j->jobid;
  } else {
    j->has_condition = 0;
    j->done = 1;
  }
  j->next = Jobs;
  Jobs = j;
  unlock_job_list();
  return j->jobid;
}

static void CleanupJob(int status, int jobid)
{
  Job *j;
  int conid;
  lock_job_list();
  for (j = Jobs; j && (j->jobid != jobid); j = j->next) ;
  unlock_job_list();
  if (j) {
    int done = 0;
    conid = j->conid;
    DisconnectFromMds(conid);
    while (!done) {
      done = 1;
      lock_job_list();
      for (j = Jobs; j; j = j->next)
	if (j->conid == conid)
	  break;
      unlock_job_list();
      if (j != 0) {
	done = 0;
	DoCompletionAst(j->jobid, status, 0, 1);
      }
    }
  }
}

static SOCKET CreatePort(short starting_port, short *port_out)
{
  short port;
  static struct sockaddr_in sin;
  long sendbuf = 6000, recvbuf = 6000;
  SOCKET s;
  int status;
  int tries = 0;
  int one = 1;
  InitializeSockets();
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
  for (tries = 0, status = -1; (status < 0) && (tries < 500); tries++) {
    port = starting_port + (random() & 0xff);
    sin.sin_port = htons(port);
    status = bind(s, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
  }
  if (status < 0) {
    perror("Error binding to service\n");
    return INVALID_SOCKET;
  }
  status = listen(s, 5);
  if (status < 0) {
    perror("Error from listen\n");
    return INVALID_SOCKET;
  }
  *port_out = port;
  return s;
}

static int ThreadRunning = 0;
static pthread_mutex_t worker_mutex;
static pthread_cond_t worker_condition;
static int worker_cond_init = 1;

static int StartReceiver(short *port_out)
{
  static short port = 0;
  static SOCKET sock;
  static pthread_t thread;
  int status = 1;
  if (port == 0) {
    sock = CreatePort((short)8800, &port);
    if (sock == INVALID_SOCKET)
      return -2;
  }
  if (!ThreadRunning) {
#ifndef _WIN32
    size_t ssize;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    status = pthread_attr_getstacksize(&attr, &ssize);
    status = pthread_attr_setstacksize(&attr, ssize * 16);
    status = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
#endif
    if (worker_cond_init) {
      pthread_mutex_init(&worker_mutex, pthread_mutexattr_default);
      pthread_cond_init(&worker_condition, pthread_condattr_default);
      worker_cond_init = 0;
    }
#ifndef _WIN32
    status = pthread_create(&thread, &attr, Worker, (void *)&sock);
    pthread_attr_destroy(&attr);
#else
    status = pthread_create(&thread, pthread_attr_default, Worker, (void *)&sock);
#endif
    if (status != 0) {
      perror("error creating dispatch receiver thread\n");
      status = -2;
    } else {
      while ((ThreadRunning == 0) && (pthread_mutex_lock(&worker_mutex) == 0)) {
	if (!ThreadRunning)
	{
	  struct timespec abstime;
	  struct timeval tmval;
	  gettimeofday(&tmval, 0);
	  abstime.tv_sec = tmval.tv_sec + 1;
	  abstime.tv_nsec = tmval.tv_usec * 1000;
	  pthread_cond_timedwait(&worker_condition, &worker_mutex, &abstime);
	}
	pthread_mutex_unlock(&worker_mutex);
      }
      status = 1;
    }
  }
  *port_out = port;
  return status;
}

static void ThreadExit(void *arg __attribute__ ((unused)))
{
  printf("ServerSendMessage thread exitted\n");
  ThreadRunning = 0;
}

/*
static jmp_buf Env;

static void signal_handler(int dummy)
{
  longjmp(Env, 1);
}
*/

static void ResetFdactive(int rep, SOCKET sock, fd_set * active)
{
  Client *c;
  if (rep > 0) {
    int done = 0;
    while (!done) {
      lock_client_list();
      for (c = ClientList; c; c = c->next) {
	if (ServerBadSocket(c->reply_sock)) {
	  unlock_client_list();
	  printf("removed client in ResetFdactive\n");
	  RemoveClient(c, 0);
	  break;
	}
      }
      unlock_client_list();
      done = (c == 0);
    }
  }
  FD_ZERO(active);
  FD_SET(sock, active);
  lock_client_list();
  for (c = ClientList; c; c = c->next) {
    if (c->reply_sock != INVALID_SOCKET)
      FD_SET(c->reply_sock, active);
  }
  unlock_client_list();
  printf("reset fdactive in ResetFdactive\n");
  return;
}

static void *Worker(void *sockptr)
{
  struct sockaddr_in sin;
  int sock = *(int *)sockptr;
  int tablesize = FD_SETSIZE;
  int num = 0;
  int last_client_addr;
  int rep;
  fd_set readfds, fdactive;
  pthread_cleanup_push(ThreadExit, 0);
  last_client_addr = 0;
  /*
     signal(SIGSEGV, signal_handler);
     signal(SIGBUS, signal_handler);
     if (setjmp(Env) != 0) {
     printf("Signal handler called in Worker\n");
     return;
     }
   */
  pthread_mutex_lock(&worker_mutex);
  ThreadRunning = 1;
  pthread_cond_signal(&worker_condition);
  pthread_mutex_unlock(&worker_mutex);
  FD_ZERO(&fdactive);
  FD_SET(sock, &fdactive);
  for (rep = 0; rep < 10; rep++) {
    readfds = fdactive;
    while ((num = select(tablesize, &readfds, 0, 0, 0)) != -1) {
      rep = 0;
      if (FD_ISSET(sock, &readfds)) {
	socklen_t len = sizeof(struct sockaddr_in);
	AcceptClient(accept(sock, (struct sockaddr *)&sin, &len), &sin, &fdactive);
      } else {
	Client *c, *next;
	int done = 0;
	while (!done) {
	  lock_client_list();
	  for (c = ClientList, next = c ? c->next : 0;
	       c && (c->reply_sock == INVALID_SOCKET || !FD_ISSET(c->reply_sock, &readfds));
	       c = next, next = c ? c->next : 0) ;
	  unlock_client_list();
	  if (c && c->reply_sock != INVALID_SOCKET && FD_ISSET(c->reply_sock, &readfds)) {
	    SOCKET reply_sock = c->reply_sock;
	    last_client_addr = c->addr;
	    DoMessage(c, &fdactive);
	    FD_CLR(reply_sock, &readfds);
	  } else
	    done = 1;
	}
      }
      readfds = fdactive;
    }
    perror("Dispatcher select loop failed");
    printf("Last client addr = %d\n", last_client_addr);
    ResetFdactive(rep, sock, &fdactive);
  }
  printf("Cannot recover from select errors in ServerSendMessage, exitting\n");
  exit(0);
  pthread_cleanup_pop(1);
  return (0);
}

int ServerBadSocket(SOCKET socket)
{
  int status = 1;
  if (socket != INVALID_SOCKET) {
    int tablesize = FD_SETSIZE;
    fd_set fdactive;
    struct timeval timeout = { 0, 1000 };
    FD_ZERO(&fdactive);
    FD_SET(socket, &fdactive);
    status = select(tablesize, &fdactive, 0, 0, &timeout);
  }
  return !(status == 0);
}

EXPORT int ServerDisconnect(char *server_in)
{
  int status = 0;
  char *srv = TranslateLogical(server_in);
  char *server = srv ? srv : server_in;
  unsigned int addr;
  char hostpart[256] = { 0 };
  char portpart[256] = { 0 };
  short port = 0;
  int num = sscanf(server, "%[^:]:%s", hostpart, portpart);
  if (num != 2) {
    printf("Server /%s/ unknown\n", server_in);
  } else {
    addr = GetHostAddr(hostpart);
    if (addr != 0) {
      if (atoi(portpart) == 0) {
	struct servent *sp = getservbyname(portpart, "tcp");
	if (sp != NULL)
	  port = sp->s_port;
	else {
	  char *portnam = getenv(portpart);
	  portnam = (portnam == NULL) ? ((hostpart[0] == '_') ? "8200" : "8000") : portnam;
	  port = htons((short)atoi(portnam));
	}
      } else
	port = htons((short)atoi(portpart));
      if (port) {
	Client *c;
	lock_client_list();
	for (c = ClientList; c && (c->addr != addr || c->port != port); c = c->next) ;
	unlock_client_list();
	if (c) {
	  RemoveClient(c, 0);
	  status = 1;
	}
      }
    }
  }
  if (srv)
    TranslateLogicalFree(srv);
  return (status);
}

EXPORT int ServerConnect(char *server_in)
{
  int conid = -1;
  char *srv = TranslateLogical(server_in);
  char *server = srv ? srv : server_in;
  int found = 0;
  unsigned int addr;
  char hostpart[256] = { 0 };
  char portpart[256] = { 0 };
  short port = 0;
  int num = sscanf(server, "%[^:]:%s", hostpart, portpart);
  if (num != 2) {
    printf("Server /%s/ unknown\n", server_in);
  } else {
    addr = GetHostAddr(hostpart);
    if (addr != 0) {
      if (atoi(portpart) == 0) {
	struct servent *sp = getservbyname(portpart, "tcp");
	if (sp != NULL)
	  port = sp->s_port;
	else {
	  char *portnam = getenv(portpart);
	  portnam = (portnam == NULL) ? ((hostpart[0] == '_') ? "8200" : "8000") : portnam;
	  port = htons((short)atoi(portnam));
	}
      } else
	port = htons((short)atoi(portpart));
      if (port) {
	Client *c;
	lock_client_list();
	for (c = ClientList; c && (c->addr != addr || c->port != port); c = c->next) ;
	unlock_client_list();
	if (c) {
	  if (ServerBadSocket(getSocket(c->conid)))
	    RemoveClient(c, 0);
	  else {
	    conid = c->conid;
	    found = 1;
	  }
	}
      }
    }
    if (conid == -1)
      conid = ConnectToMds(server);
  }
  if (srv)
    TranslateLogicalFree(srv);
  if (!found && conid >= 0)
    AddClient(addr, port, conid);
  return (conid);
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
    DoCompletionAst(jobid, status, msg, 1);
    break;
  case SrvJobSTARTING:
    DoBeforeAst(jobid);
    break;
  case SrvJobCHECKEDIN:
    break;
  default:
    RemoveClient(c, fdactive);
  }
  if (msglen != 0)
    free(msg);
}

static void RemoveClient(Client * c, fd_set * fdactive)
{
  Job *j;
  int client_found = 0;
  int found = 1;
  int conid = -1;
  lock_client_list();
  if (ClientList == c) {
    client_found = 1;
    ClientList = c->next;
  } else {
    Client *cp;
    for (cp = ClientList; cp && cp->next != c; cp = cp->next) ;
    if (cp && cp->next == c) {
      client_found = 1;
      cp->next = c->next;
    }
  }
  unlock_client_list();
  if (client_found) {
    conid = c->conid;
    if (c->reply_sock != INVALID_SOCKET) {
      shutdown(c->reply_sock, 2);
      close(c->reply_sock);
      if (fdactive)
	FD_CLR(c->reply_sock, fdactive);
    }
    if (c->conid >= 0) {
      DisconnectFromMds(c->conid);
    }
    free(c);
  }
  lock_job_list();
  for (j = Jobs; j; j = j->next)
    if (j->conid == conid)
      j->marked_for_delete = 1;
  unlock_job_list();
  while (found) {
    found = 0;
    lock_job_list();
    for (j = Jobs; j && !j->marked_for_delete; j = j->next) ;
    unlock_job_list();
    if (j) {
      found = 1;
      DoCompletionAst(j->jobid, ServerPATH_DOWN, 0, 1);
      RemoveJob(j);
    }
  }
}

static unsigned int GetHostAddr(char *host)
{
  unsigned int addr = 0;
  struct hostent *hp = NULL;
  hp = gethostbyname(host);
#ifdef _WIN32
  if ((hp == NULL) && (WSAGetLastError() == WSANOTINITIALISED)) {
    WSADATA wsaData;
    WORD wVersionRequested;
    wVersionRequested = MAKEWORD(1, 1);
    WSAStartup(wVersionRequested, &wsaData);
    hp = gethostbyname(host);
  }
#endif
  if (hp == NULL) {
    addr = inet_addr(host);
    if (addr != 0xffffffff)
      hp = gethostbyaddr((void *)&addr, (int)sizeof(addr), AF_INET);
  }
  addr = (hp == NULL) ? 0 : *(unsigned int *)hp->h_addr_list[0];
  return addr == 0xffffffff ? 0 : addr;
}

static void AddClient(unsigned int addr, short port, int conid)
{
  Client *c;
  Client *new = (Client *) malloc(sizeof(Client));
  new->reply_sock = INVALID_SOCKET;
  new->conid = conid;
  new->addr = addr;
  new->port = port;
  new->next = 0;
  lock_client_list();
  for (c = ClientList; c && c->next != 0; c = c->next) ;
  if (c)
    c->next = new;
  else
    ClientList = new;
  unlock_client_list();
}

static void AcceptClient(SOCKET reply_sock, struct sockaddr_in *sin, fd_set * fdactive)
{
  unsigned int addr = *(unsigned int *)&sin->sin_addr;
  Client *c;
  lock_client_list();
  for (c = ClientList; c && (c->addr != addr || c->reply_sock != INVALID_SOCKET); c = c->next) ;
  unlock_client_list();
  if (c) {
    c->reply_sock = reply_sock;
    if (reply_sock != INVALID_SOCKET)
      FD_SET(reply_sock, fdactive);
  } else {
    shutdown(reply_sock, 2);
    close(reply_sock);
  }
}

static int client_mutex_initialized = 0;
static pthread_mutex_t client_mutex;

static void lock_client_list()
{

  if (!client_mutex_initialized) {
    client_mutex_initialized = 1;
    pthread_mutex_init(&client_mutex, pthread_mutexattr_default);
  }

  pthread_mutex_lock(&client_mutex);
}

static void unlock_client_list()
{

  if (!client_mutex_initialized) {
    client_mutex_initialized = 1;
    pthread_mutex_init(&client_mutex, pthread_mutexattr_default);
  }

  pthread_mutex_unlock(&client_mutex);
}

static int job_mutex_initialized = 0;
static pthread_mutex_t job_mutex;

static void lock_job_list()
{

  if (!job_mutex_initialized) {
    job_mutex_initialized = 1;
    pthread_mutex_init(&job_mutex, pthread_mutexattr_default);
  }

  pthread_mutex_lock(&job_mutex);
}

static void unlock_job_list()
{

  if (!job_mutex_initialized) {
    job_mutex_initialized = 1;
    pthread_mutex_init(&job_mutex, pthread_mutexattr_default);
  }

  pthread_mutex_unlock(&job_mutex);
}
