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

#include <pthread.h>
#include <ipdesc.h>
#include <string.h>
#include "servershrp.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>


#if (defined(_DECTHREADS_) && (_DECTHREADS_ != 1)) || !defined(_DECTHREADS_)
#define pthread_attr_default NULL
#else
#undef select
#endif

extern char *TranslateLogical(char *);
extern void TranslateLogicalFree(char *);

typedef struct _Job { pthread_cond_t *condition;
                      int *retstatus;
                      void (*ast)();
                      void *astparam;
                      void (*before_ast)();
                      int jobid;
                      int sock;
                      struct _Job *next;
                    } Job;

typedef struct _client { SOCKET reply_sock;
                         SOCKET send_sock;
                         unsigned int addr;
                         short port;
                         struct _client *next;
                       } Client;


static Job *Jobs = 0;
static Client *ClientList = 0;

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define SndArgChk(a1,a2,a3,a4,a5,a6,a7,a8) status = SendArg(a1,a2,a3,a4,a5,a6,a7,a8); if (!(status & 1)) goto send_error;


static int StartReceiver(unsigned int *addr, short *port);
int ServerConnect(char *server);
static int RegisterJob(pthread_cond_t *condition, int *retstatus,void (*ast)(), void *astparam, void (*before_ast)(), int sock);
static void  CleanupJob(int status,int jobid);
static void *Worker(void *sockptr);
static void DoMessage(Client *c, fd_set *fdactive);
static void RemoveClient(Client *c, fd_set *fdactive);
static unsigned int GetHostAddr(char *host);
static void AddClient(unsigned int addr, short port, int send_sock);
static void AcceptClient(int reply_sock, struct sockaddr_in *sin, fd_set *fdactive);

int ServerSendMessage( pthread_cond_t *condition, char *server, int op, int *retstatus, 
                         void (*ast)(), void *astparam, void (*before_ast)(),
  int numargs_in, struct descrip *p1, struct descrip *p2, struct descrip *p3, struct descrip *p4,
                  struct descrip *p5, struct descrip *p6, struct descrip *p7, struct descrip *p8) 
{
  unsigned int addr;
  short port;
  int sock;
  int flags = 0;
  int status = 0;
  int jobid;
  if (StartReceiver(&addr,&port) && ((sock = ServerConnect(server)) >= 0))
  {
    char cmd[] = "ServerQAction($,$,$,$,$,$,$,$,$,$,$,$,$)";
    int numargs = max(0,min(numargs_in,8));
    int offset = strlen("ServerQAction($,$,$,$,$") + numargs * 2;
    int idx = 0;
    char dtype;
    short len;
    char ndims;
    int  dims[8];
    int numbytes;
    int *dptr;
    jobid = RegisterJob(condition,retstatus,ast,astparam,before_ast,sock);
    cmd[offset] = ')';
    cmd[offset+1] = '\0';
    SndArgChk(sock, idx++, DTYPE_CSTRING, numargs+6, strlen(cmd), 0, 0, cmd);
    SndArgChk(sock, idx++, DTYPE_LONG,    numargs+6, 4, 0, 0, (char *)&addr);
    SndArgChk(sock, idx++, DTYPE_SHORT,    numargs+6, 2, 0, 0, (char *)&port);
    SndArgChk(sock, idx++, DTYPE_LONG,    numargs+6, 4, 0, 0, (char *)&op);
    if (before_ast) flags |= SrvJobBEFORE_NOTIFY;
    SndArgChk(sock, idx++, DTYPE_LONG,    numargs+6, 4, 0, 0, (char *)&flags);
    SndArgChk(sock, idx++, DTYPE_LONG,    numargs+6, 4, 0, 0, (char *)&jobid);
    if (numargs > 0)
    {
      SndArgChk(sock, idx++, p1->dtype, numargs+6, ArgLen(p1), p1->ndims, p1->dims, p1->ptr);
      if (numargs > 1) 
      {
        SndArgChk(sock, idx++, p2->dtype, numargs+6, ArgLen(p2), p2->ndims, p2->dims, p2->ptr);
        if (numargs > 2) 
        {
          SndArgChk(sock, idx++, p3->dtype, numargs+6, ArgLen(p3), p3->ndims, p3->dims, p3->ptr);
          if (numargs > 3) 
          {
            SndArgChk(sock, idx++, p4->dtype, numargs+6, ArgLen(p4), p4->ndims, p4->dims, p4->ptr);
            if (numargs > 4) 
            {
              SndArgChk(sock, idx++, p5->dtype, numargs+6, ArgLen(p5), p5->ndims, p5->dims, p5->ptr);
              if (numargs > 5) 
              {
                SndArgChk(sock, idx++, p6->dtype, numargs+6, ArgLen(p6), p6->ndims, p6->dims, p6->ptr);
                if (numargs > 6) 
                {
                  SndArgChk(sock, idx++, p7->dtype, numargs+6, ArgLen(p7), p7->ndims, p7->dims, p7->ptr);
                  if (numargs > 7) 
                  {
                    SndArgChk(sock, idx++, p8->dtype, numargs+6, ArgLen(p8), p8->ndims, p8->dims, p8->ptr);
		  }
                }
              }
            }
          }
        }
      }
    }
    status = GetAnswerInfo(sock, &dtype, &len, &ndims, dims, &numbytes, (void **)&dptr);
  }
  return status;

 send_error:
  perror("Error sending message to server");
  CleanupJob(status,jobid);
  return status;
}

static void RemoveJob(Job *job)
{
  Job *j,*prev;
  pthread_lock_global_np();
  for (j=Jobs,prev=0;j;prev=j,j=j->next)
  {
    if (j==job)
    {
      if (prev)
        prev->next = j->next;
      else
        Jobs = j->next;
      free(j);
      break;
    }
  }
  pthread_unlock_global_np();
}

static void DoCompletionAst(int jobid,int status,char *msg, int removeJob)
{
  Job *j;
  for (j=Jobs; j && (j->jobid != jobid); j=j->next);
  if (j)
  {
    if (j->retstatus)
      *j->retstatus = status;
    if (j->condition)
      pthread_cond_signal(j->condition);
    if (j->ast)
      (*j->ast)(j->astparam,msg);
    if (removeJob)
      RemoveJob(j);
  }
}
      
static void DoBeforeAst(int jobid)
{
  Job *j;
  for (j=Jobs; j && (j->jobid != jobid); j=j->next);
  if (j)
  {
    if (j->before_ast)
      (*j->before_ast)(j->astparam);
  }
}
      
static int RegisterJob(pthread_cond_t *condition, int *retstatus,void (*ast)(), void *astparam, void (*before_ast)(), int sock)
{
  static int jobid = 0;
  Job *j = (Job *)malloc(sizeof(Job));
  j->condition = condition;
  j->retstatus = retstatus;
  j->ast = ast;
  j->astparam = astparam;
  j->before_ast = before_ast;
  j->jobid = ++jobid;
  j->sock = sock;
  j->next = Jobs;
  pthread_lock_global_np();
  Jobs = j;
  pthread_unlock_global_np();
  return jobid;
}

static void  CleanupJob(int status, int jobid)
{
  Job *j,*prev;
  int sock;
  for (j=Jobs; j && (j->jobid != jobid); j++);
  if (j)
  {
    sock = j->sock;
    shutdown(sock,2);
    close(sock);
    for (j=Jobs,prev=0;j;)
      if (j->sock == sock)
        DoCompletionAst(j->jobid,status,0,0);
    for (j=Jobs;j;)
      if (j->sock == sock)
      {
        Job *next = j->next;
        RemoveJob(j);
        j=next;
      }
  }
}

static int CreatePort(short starting_port, short *port_out)
{
  short port;
  static struct sockaddr_in sin;
  struct servent *sp;
  long sendbuf=32768,recvbuf=32768;
  int s;
  int status;
  int one = 1;
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == -1)
  {
    perror("Error getting Connection Socket\n");
    return(-1);
  }
  setsockopt(s, SOL_SOCKET,SO_RCVBUF,(char *)&recvbuf,sizeof(long));
  setsockopt(s, SOL_SOCKET,SO_SNDBUF,(char *)&sendbuf,sizeof(long));  
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&one,sizeof(int));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  for (port = starting_port, status = -1; (status < 0) && (port < starting_port + 500);port++)
  {
    sin.sin_port = htons(port);
    status = bind(s, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
  }
  if (status < 0)
  {
    perror("Error binding to service\n");
    return(-1);
  }
  status = listen(s,5);
  if (status < 0)
  {
    perror("Error from listen\n");
    return(-1);
  }
  *port_out = port - 1;
  return s;
}

static int ThreadRunning = 0;

static int StartReceiver(unsigned int *addr_out, short *port_out)
{
  static short port = 0;
  static unsigned int addr = 0;
  static int sock;
  static pthread_t thread;
  int status = 1;
  if (port == 0)
  {
    sock = CreatePort((short)8800,&port);
    if (sock < 0)
      return(0);
  }
  if (addr == 0)
    addr = GetHostAddr(0);
  if (!ThreadRunning)
  {
    status = pthread_create(&thread, pthread_attr_default, Worker, (void *)&sock);
    if (status != 0)
    {
      perror("error creating dispatch receiver thread\n");
      status = 0;
    }
    else
      status = 1;
  }
  *port_out = port;
  *addr_out = addr;
  return status;  
}

static void ThreadExit(void *arg)
{
  ThreadRunning = 0;
}
  
static void *Worker(void *sockptr)
{
  struct sockaddr_in sin;
  int sock = *(int *)sockptr;
  int tablesize = FD_SETSIZE;
  fd_set readfds,fdactive;
  pthread_cleanup_push(ThreadExit, 0);
  ThreadRunning = 1;
  FD_ZERO(&fdactive);
  FD_SET(sock,&fdactive);
  readfds = fdactive;
  while ((select(tablesize, &readfds, 0, 0, 0) != -1))
  {
    if (FD_ISSET(sock, &readfds))
    {
      int len = sizeof(struct sockaddr_in);
      AcceptClient(accept(sock, (struct sockaddr *)&sin, &len),&sin,&fdactive);
    }
    else
    {
      Client *c,*next;
      for (c=ClientList,next=c ? c->next : 0; c; c=next,next=c ? c->next : 0)
      {
        if (c->reply_sock >= 0 && FD_ISSET(c->reply_sock, &readfds))
          DoMessage(c,&fdactive);
      }
    }
    readfds = fdactive;
  }
  pthread_cleanup_pop(1);
  return(0);
}

int ServerBadSocket(int socket)
{
  int tablesize = FD_SETSIZE;
  fd_set fdactive;
  int status;
  struct timeval timeout = {0,0};
  FD_ZERO(&fdactive);
  FD_SET(socket,&fdactive);
  status = select(tablesize,&fdactive,0,0,&timeout);
  return status == 1;
}

int ServerConnect(char *server_in)
{
  int sock;
  char *srv = TranslateLogical(server_in);
  char *server = srv ? srv : server_in;
  unsigned int addr;
  char hostpart[256] = {0};
  char portpart[256] = {0};
  short port;
  sscanf(server,"%[^:]:%s",hostpart,portpart);
  addr = GetHostAddr(hostpart);
  if (addr != 0)
  {
    port = (short)atoi(portpart);
    if (port > 0)
    {
      Client *c;
      for (c=ClientList; c && (c->addr != addr || c->port != port); c=c->next);
      if (c)
      {
        int tablesize = FD_SETSIZE;
        fd_set fdactive;
        int status;
        struct timeval timeout = {0,0};
        FD_ZERO(&fdactive);
        FD_SET(c->send_sock,&fdactive);
        status = select(tablesize,&fdactive,0,0,&timeout);
        if (ServerBadSocket(c->send_sock))
          RemoveClient(c,0);
        else
          return(c->send_sock);
      }
    }
  }
  sock = ConnectToMds(server);
  if (srv)
    TranslateLogicalFree(srv);
  if (sock >= 0)
    AddClient(addr,port,sock);
  return(sock);
}
  
static void DoMessage(Client *c, fd_set *fdactive)
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
  if (nbytes != 60)
  {
    RemoveClient(c,fdactive);
    return;
  }
  num = sscanf(reply,"%d %d %d %d",&jobid,&replyType,&status,&msglen);
  if (num != 4)
  {
    RemoveClient(c,fdactive);
    return;
  }
  if (msglen != 0)
  {
    msg = (char *)malloc(msglen);
    nbytes = recv(c->reply_sock, msg, msglen, 0);
    if (nbytes != msglen)
    {
      free(msg);
      RemoveClient(c,fdactive);
      return;
    }
  }
  switch (replyType)
  {
  case SrvJobFINISHED: DoCompletionAst(jobid,status,msg,1); break;
  case SrvJobSTARTING: DoBeforeAst(jobid); break;
  default: RemoveClient(c,fdactive);
  }
  if (msglen != 0)
    free(msg);
}

static void RemoveClient(Client *c, fd_set *fdactive)
{
  Job *j;
  int found = 1;
  while (found)
  {
    found = 0;
    for (j=Jobs;j && (j->sock != c->send_sock);j=j->next);
    if (j)
    {
      found = 1;
      DoCompletionAst(j->jobid, ServerABORT, 0, 1);
    }
  }
  if (fdactive)
    FD_CLR(c->reply_sock,fdactive);
  if (c->reply_sock >= 0)
  {
    shutdown(c->reply_sock,2);
    close(c->reply_sock);
  }
  if (c->send_sock >= 0)
  {
    shutdown(c->send_sock,2);
    close(c->send_sock);
  }
  pthread_lock_global_np();
  if (ClientList == c)
    ClientList = c->next;
  else
  {
    Client *cp;
    for (cp = ClientList; cp && cp->next != c; cp=cp->next);
    if (cp && cp->next == c)
      cp->next = c->next;
  }
  pthread_unlock_global_np();
  free(c);
}
  
static unsigned int GetHostAddr(char *host)
{
  unsigned int addr = 0;
  struct hostent *hp = NULL;
  if (host == NULL)
    return (unsigned int)gethostid();
#ifndef vxWorks
  hp = gethostbyname(host);
#endif
#ifdef _WIN32
  if ((hp == NULL) && (WSAGetLastError() == WSANOTINITIALISED))
  {
	  WSADATA wsaData;
	  WORD wVersionRequested;
	  wVersionRequested = MAKEWORD(1,1);
	  WSAStartup(wVersionRequested,&wsaData);
	  hp = gethostbyname(host);
  }
#endif
  if (hp == NULL)
  {
    addr = inet_addr(host);
#ifndef vxWorks
    if (addr != 0xffffffff)
    	hp = gethostbyaddr((void *) &addr, (int) sizeof(addr), AF_INET);
#endif
  }
#ifndef vxWorks
  addr = (hp == NULL) ? 0 : *(unsigned int *)hp->h_addr_list[0];
#endif
  return addr == 0xffffffff ? 0 : addr;
}

static void AddClient(unsigned int addr, short port, int send_sock)
{
  Client *c;
  Client *new = (Client *)malloc(sizeof(Client));
  new->reply_sock = -1;
  new->send_sock = send_sock;
  new->addr = addr;
  new->port = port;
  new->next = 0;
  pthread_lock_global_np();
  for (c=ClientList; c && c->next != 0; c=c->next);
  if (c)
    c->next = new;
  else
    ClientList = new;
  pthread_unlock_global_np();
}

static void AcceptClient(int reply_sock, struct sockaddr_in *sin, fd_set *fdactive)
{
  unsigned int addr = *(unsigned int *)&sin->sin_addr;
  Client *c;
  for (c=ClientList; c && (c->addr != addr || c->reply_sock != -1); c=c->next);
  if (c)
  {
    c->reply_sock = reply_sock;
    FD_SET(reply_sock,fdactive);
  }
  else
  {
    shutdown(reply_sock,2);
    close(reply_sock);
  }
}
