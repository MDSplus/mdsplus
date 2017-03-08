#include "mdsip_connections.h"
#include <STATICdef.h>
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <config.h>
#include <time.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_FILIO_H
#include <sys/filio.h>
#endif
#ifdef _WIN32
#include "udtc.h"
typedef int socklen_t;
#define snprintf _snprintf
#define MSG_DONTWAIT 0
#include <io.h>
#include <process.h>
#define getpid _getpid
extern int pthread_mutex_init();
extern int pthread_mutex_lock();
extern int pthread_mutex_unlock();
#else
#include "udtc.h"
//#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
//#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#endif
#define SEND_BUF_SIZE 32768
#define RECV_BUF_SIZE 32768
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif
static ssize_t UDT_send(int conid, const void *buffer, size_t buflen, int nowait);
static ssize_t UDT_recv(int conid, void *buffer, size_t len);
static int UDT_disconnect(int conid);
static int UDT_flush(int conid);
static int UDT_listen(int argc, char **argv);
static int UDT_authorize(int conid, char *username);
static int UDT_connect(int conid, char *protocol, char *host);
static int UDT_reuseCheck(char *host, char *unique, size_t buflen);
static IoRoutines UDT_routines =
{ UDT_connect, UDT_send, UDT_recv, UDT_flush, UDT_listen, UDT_authorize, UDT_reuseCheck,
  UDT_disconnect };

typedef struct _client {
  UDTSOCKET sock;
  int id;
  char *username;
  int addr;
  char *host;
  struct _client *next;
} Client;

static Client *ClientList = 0;

static int server_epoll = -1;

typedef struct _socket_list {
  UDTSOCKET socket;
  struct _socket_list *next;
} SocketList;

static SocketList *Sockets = 0;

EXPORT IoRoutines *Io()
{
  return &UDT_routines;
}

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

static UDTSOCKET getSocket(int conid)
{
  size_t len;
  char *info_name;
  int readfd;
  GetConnectionInfo(conid, &info_name, &readfd, &len);
  return (info_name && strcmp(info_name, "udt") == 0) ? (UDTSOCKET) readfd : (UDTSOCKET) - 1;
}

static int socket_mutex_initialized = 0;
static pthread_mutex_t socket_mutex;

static void lock_socket_list()
{
  if (!socket_mutex_initialized) {
      socket_mutex_initialized = 1;
      pthread_mutex_init(&socket_mutex, 0);
    }
  pthread_mutex_lock(&socket_mutex);
}

static void unlock_socket_list()
{
  if (!socket_mutex_initialized) {
      socket_mutex_initialized = 1;
      pthread_mutex_init(&socket_mutex, 0);
    }
  pthread_mutex_unlock(&socket_mutex);
}

static void PushSocket(UDTSOCKET socket)
{
  SocketList *oldhead;
  lock_socket_list();
  oldhead = Sockets;
  Sockets = malloc(sizeof(SocketList));
  Sockets->socket = socket;
  Sockets->next = oldhead;
  unlock_socket_list();
}

static void PopSocket(UDTSOCKET socket)
{
  SocketList *p, *s;
  lock_socket_list();
  for (s = Sockets, p = 0; s && s->socket != socket; p = s, s = s->next) ;
  if (s != NULL) {
      if (p)
        p->next = s->next;
      else
        Sockets = s->next;
      free(s);
    }
  unlock_socket_list();
}

static void ABORT(int sigval __attribute__ ((unused)))
{
  SocketList *s;
  lock_socket_list();
  for (s = Sockets; s; s = s->next) {
      //    shutdown(s->socket,2);
      udt_close(s->socket);
    }
  unlock_socket_list();
}

static char *getHostInfo(UDTSOCKET s, char **iphostptr, char **hostnameptr)
{
  char *ans = NULL;
  struct sockaddr_in sin;
  int n = sizeof(sin);
  if (udt_getpeername(s, (struct sockaddr *)&sin, &n) == 0) {
    struct hostent *hp = 0;
    char *iphost = inet_ntoa(sin.sin_addr);
    hp = gethostbyaddr((char *)&sin.sin_addr, sizeof(sin.sin_addr), AF_INET);
    if (hp && hp->h_name) {
      ans = (char *)malloc(strlen(iphost) + strlen(hp->h_name) + 10);
      sprintf(ans, "%s [%s]", hp->h_name, iphost);
    } else {
      ans = (char *)malloc(strlen(iphost) + 1);
      strcpy(ans, iphost);
    }
    if (iphostptr) {
      *iphostptr = (char *)malloc(strlen(iphost) + 1);
      strcpy(*iphostptr, iphost);
    }
    if (hostnameptr) {
      if (hp && hp->h_name) {
	*hostnameptr = (char *)malloc(strlen(hp->h_name) + 1);
	strcpy(*hostnameptr, hp->h_name);
      } else {
	*hostnameptr = 0;
      }
    }
  }
  return ans;
}

static int UDT_authorize(int conid, char *username)
{
  UDTSOCKET s = getSocket(conid);
  time_t tim = time(0);
  char *timestr = ctime(&tim);
  int ans = 0;
  char *hoststr = 0;
  char *iphost = 0;
  char *info = getHostInfo(s, &iphost, &hoststr);
  if (info) {
      char *matchString[2] = { 0, 0 };
      int num = 1;
      timestr[strlen(timestr) - 1] = 0;
      printf("%s (%d) (pid %d) Connection received from %s@%s\r\n", timestr, s, getpid(), username,
             info);
      matchString[0] = strcpy(malloc(strlen(username) + strlen(iphost) + 3), username);
      strcat(matchString[0], "@");
      strcat(matchString[0], iphost);
      if (hoststr) {
          matchString[1] = strcpy(malloc(strlen(username) + strlen(hoststr) + 3), username);
          strcat(matchString[1], "@");
          strcat(matchString[1], hoststr);
          num = 2;
        }
      ans = CheckClient(username, num, matchString);
      if (matchString[0])
        free(matchString[0]);
      if (matchString[1])
        free(matchString[1]);
    }
  if (info)
    free(info);
  if (iphost)
    free(iphost);
  if (hoststr)
    free(hoststr);
  fflush(stdout);
  fflush(stderr);
  return ans;
}

static ssize_t UDT_send(int conid, const void *bptr, size_t num, int nowait)
{
  UDTSOCKET s = getSocket(conid);
  int options = nowait ? MSG_DONTWAIT : 0;
  ssize_t sent = -1;
  if (s != -1) {
      sent = udt_send(s, bptr, num, options | MSG_NOSIGNAL);
    }
  return sent;
}

static ssize_t UDT_recv(int conid, void *bptr, size_t num)
{
  UDTSOCKET s = getSocket(conid);
  ssize_t recved = -1;
  if (s != -1) {
    struct sockaddr sin;
    int n = sizeof(sin);
    PushSocket(s);
    signal(SIGABRT, ABORT);
    if (udt_getpeername(s, (struct sockaddr *)&sin, &n) == 0)
      recved = udt_recv(s, bptr, num, MSG_NOSIGNAL);
    else
      perror("Error getting connection information from socket\n");
    PopSocket(s);
  }
  return recved;
}

static int UDT_disconnect(int conid)
{
  UDTSOCKET s = getSocket(conid);
  int status = 0;
  time_t tim = time(0);
  char *timestr = ctime(&tim);
  if (s != -1) {
    Client *c, **p;
    for (p = &ClientList, c = ClientList; c && c->id != conid; p = &c->next, c = c->next) ;
    if (c) {
      *p = c->next;
      if (server_epoll != -1) {
	udt_epoll_remove_usock(server_epoll, s);
	timestr[strlen(timestr) - 1] = 0;
	printf("%s (%d) (pid %d) Connection disconnected from %s@%s\r\n", timestr, s, getpid(),
	       c->username, c->host);
      }
      free(c->username);
      free(c);
    }
    status = udt_close(s);
  }
  fflush(stdout);
  fflush(stderr);
  return status;
}

static int UDT_flush(int conid)
{
  UDTSOCKET sock = getSocket(conid);
  return 0;
  if (sock != -1) {
#if !defined(__sparc__)
    int status;
    int tries = 0;
    UDTSOCKET readfds[1024];
    int readfds_num = 1024;
    int epoll = udt_epoll_create();
    int timeout = 1000;
    int events = UDT_UDT_EPOLL_IN;
    udt_epoll_add_usock(epoll, sock, &events);
    while (((((status = udt_epoll_wait2(epoll, readfds, &readfds_num,
				       NULL, NULL, timeout, NULL, NULL, NULL, NULL)) == 0) &&
	     sock == readfds[0]) || (status < 0)) && (tries < 10)) {
      char buff[32768];
      int nbytes = udt_recv(sock, buff, 32768, 0);
      tries++;
      if (nbytes > 0)
	tries = 0;
      readfds_num = 1024;
    }
    udt_epoll_release(epoll);
  }
#endif
  return 0;
}

static short GetPort(char *name)
{
  short port;
  struct servent *sp;
  port = htons((short)atoi(name));
  if (port == 0) {
      sp = getservbyname(name, "tcp");
      if (sp == NULL) {
          fprintf(stderr, "unknown service: %s/tcp\n\n", name);
          exit(0);
        }
      port = sp->s_port;
    }
  return port;
}

static int getHostAndPort(char *hostin, struct sockaddr_in *sin)
{
  struct hostent *hp = NULL;
  int addr;
  size_t i;
  static char *mdsip = "mdsip";
  char *host = strcpy((char *)malloc(strlen(hostin) + 1), hostin);
  char *service;
  unsigned short portnum;
  InitializeSockets();
  for (i = 0; i < strlen(host) && host[i] != ':'; i++) ;
  if (i < strlen(host)) {
      host[i] = '\0';
      service = &host[i + 1];
    } else {
      service = mdsip;
    }
  hp = gethostbyname(host);
  if (hp == NULL) {
      addr = inet_addr(host);
      if (addr != -1)
        hp = gethostbyaddr((void *)&addr, (int)sizeof(addr), AF_INET);
    }
  if (hp == 0) {
      free(host);
      return 0;
    }
  if (atoi(service) == 0) {
      struct servent *sp;
      sp = getservbyname(service, "tcp");
      if (sp != NULL)
        portnum = sp->s_port;
      else {
          char *port = getenv(service);
          port = (port == NULL) ? "8000" : port;
          portnum = htons(atoi(port));
        }
    } else
    portnum = htons(atoi(service));
  if (portnum == 0) {
      free(host);
      return 2;
    }
  sin->sin_port = portnum;
  sin->sin_family = AF_INET;
#if defined(ANET)
  memcpy(&sin->sin_addr, hp->h_addr, sizeof(sin->sin_addr));
#else
  memcpy(&sin->sin_addr, hp->h_addr_list[0], hp->h_length);
#endif
  free(host);
  return 1;
}

static int UDT_reuseCheck(char *host, char *unique, size_t buflen)
{
  struct sockaddr_in sin;
  int status = getHostAndPort(host, &sin);
  if (status == 1) {
      char *addr = (char *)&sin.sin_addr;
      snprintf(unique, buflen, "tcp://%d.%d.%d.%d:%d", addr[0], addr[1], addr[2], addr[3],
          ntohs(sin.sin_port));
      return 0;
    } else {
      *unique = 0;
      return -1;
    }
}

static int UDT_connect(int conid, char *protocol __attribute__ ((unused)), char *host)
{
  struct sockaddr_in sin;
  UDTSOCKET s;
  int status = getHostAndPort(host, &sin);
  int sinlen = sizeof(sin);
  if (status == 1) {
    InitializeSockets();
    s = udt_socket(AF_INET, SOCK_STREAM, 0);
    if (!s) {
      perror("Error in connect");
      return -1;
    }
    status = udt_connect(s, (struct sockaddr *)&sin, sinlen);
    if (status != 0) {
      fprintf(stderr, "Error in connect to service: %s\n", udt_getlasterror_desc());
      fflush(stderr);
      return -1;
    }
    SetConnectionInfo(conid, "udt", s, NULL, 0);
    return 0;
  } else if (status == 0) {
    fprintf(stderr, "Connect failed, unknown host\n");
    fflush(stderr);
    return -1;
  } else {
    fprintf(stderr, "Connect failed, unknown service\n");
    fflush(stderr);
    return -1;
  }
}

#ifdef _WIN32
VOID CALLBACK ShutdownEvent(PVOID arg, BOOLEAN fired)
{
  fprintf(stderr, "Service shut down\n");
  exit(0);
}

static int getSocketHandle(char *name)
{
  char *logdir = GetLogDir();
  char *portnam = GetPortname();
  char *logfile = malloc(strlen(logdir)+strlen(portnam)+50);
  HANDLE h;
  int ppid;
  int psock;
  char shutdownEventName[120];
  HANDLE shutdownEvent, waitHandle;
  if (name == 0 || sscanf(name, "%d:%d", &ppid, &psock) != 2) {
   fprintf(stderr, "Mdsip single connection server can only be started from windows service\n");
    exit(1);
  }
  sprintf(logfile, "%s\\MDSIP_%s_%d.log", logdir, portnam, _getpid());
  freopen(logfile, "a", stdout);
  freopen(logfile, "a", stderr);
  free(logdir);
  free(logfile);
  if (!DuplicateHandle(OpenProcess(PROCESS_ALL_ACCESS, TRUE, ppid),
                       (HANDLE) psock, GetCurrentProcess(), (HANDLE *) & h,
                       PROCESS_ALL_ACCESS, TRUE, DUPLICATE_CLOSE_SOURCE | DUPLICATE_SAME_ACCESS)) {
      fprintf(stderr, "Attempting to duplicate socket from pid %d socket %d\n", ppid, psock);
      perror("Error duplicating socket from parent");
      exit(1);
    }
  sprintf(shutdownEventName, "MDSIP_%s_SHUTDOWN", GetPortname());
  shutdownEvent = CreateEvent(NULL, FALSE, FALSE, (LPCWSTR) shutdownEventName);
  if (!RegisterWaitForSingleObject(&waitHandle, shutdownEvent, ShutdownEvent, NULL, INFINITE, 0))
    perror("Error registering for shutdown event");
  return *(int *)&h;
}
#else
static void ChildSignalHandler(int num __attribute__ ((unused)))
{
  sigset_t set, oldset;
  pid_t pid;
  int status;
  /* block other incoming SIGCHLD signals */
  sigemptyset(&set);
  sigaddset(&set, SIGCHLD);
  sigprocmask(SIG_BLOCK, &set, &oldset);
  /* wait for child */
  while ((pid = waitpid((pid_t) - 1, &status, WNOHANG)) > 0) {
      /* re-install the signal handler (some systems need this) */
      signal(SIGCHLD, ChildSignalHandler);
      /* and unblock it */
      sigemptyset(&set);
      sigaddset(&set, SIGCHLD);
      sigprocmask(SIG_UNBLOCK, &set, &oldset);
    }
}
#endif

static int UDT_listen(int argc, char **argv)
{
  Options options[] = { {"p", "port", 1, 0, 0},
#ifdef _WIN32
  {"S", "sockethandle", 1, 0, 0},
#endif
  {0, 0, 0, 0, 0}
  };
#ifndef _WIN32
  signal(SIGCHLD, ChildSignalHandler);
#endif
  ParseCommand(argc, argv, options, 0, 0, 0);
  if (options[0].present && options[0].value)
    SetPortname(options[0].value);
  else if (GetPortname() == 0)
    SetPortname("mdsip");
  InitializeSockets();
  server_epoll = udt_epoll_create();
  
  if (GetMulti()) {
    unsigned short port = GetPort(GetPortname());
    char *matchString[] = { "multi" };
    UDTSOCKET s;
    struct sockaddr_in sin;
    int addrlen = sizeof(sin);
    UDTSOCKET readfds[1024];
    int status;
    int events = UDT_UDT_EPOLL_IN | UDT_UDT_EPOLL_ERR;
    CheckClient(0, 1, matchString);
    s = udt_socket(AF_INET, SOCK_STREAM, 0);
    //    printf("listen sock=%d\n",s);
    if (s == -1) {
      printf("Error getting Connection Socket\n");
      exit(1);
    }
    udt_epoll_add_usock(server_epoll, s, &events);
    memset(&sin, 0, sizeof(sin));
    sin.sin_port = port;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    status = udt_bind(s, (struct sockaddr *)&sin, addrlen);
    if (status < 0) {
      fprintf(stderr, "Error binding to service: %s\n", udt_getlasterror_desc());
      exit(1);
    }
    status = udt_listen(s, 128);
    if (status < 0) {
      fprintf(stderr, "Error return by udt_listen: %s\n", udt_getlasterror_desc());
      exit(1);
    }
    while (1) {
      int i;
      int readfds_num = 1024;
      status =
	  udt_epoll_wait2(server_epoll, readfds, &readfds_num, NULL, NULL, 5000, NULL, NULL, NULL,
			  NULL);
      //      printf("status=%d, readfds_num=%d, writefds_num=%d\n",status,readfds_num,writefds_num);
      if (status != 0) {
	Client *c;
	LockAsts();
	while (1) {
	  for (c = ClientList; c; c = c->next) {
	    int c_epoll = udt_epoll_create();
	    UDTSOCKET readfds[1];
	    UDTSOCKET writefds[1];
	    int readnum = 1;
	    int writenum = 1;
	    udt_epoll_add_usock(c_epoll, c->sock, NULL);
	    status =
		udt_epoll_wait2(c_epoll, readfds, &readnum, writefds, &writenum, 0, NULL, NULL,
				NULL, NULL);
	    udt_epoll_release(c_epoll);
	    if (status != 0) {
	      CloseConnection(c->id);
	      goto next;
	      break;
	    }
	  }
	  break;
 next:
	  continue;
	}
	/*         
	   if ((c->sock == writefds[i]) && udt_getpeername(c->sock, (struct sockaddr *)&sin, &n)) {
	   fprintf(stderr,"Removed disconnected client\n");
	   fflush(stderr);
	   CloseConnection(c->id);
	   break;
	   }
	   }
	 */
	UnlockAsts();
      } else {
	for (i = 0; readfds_num != 1024 && i < readfds_num; i++) {
	  if (readfds[i] == s) {
	    int events = UDT_UDT_EPOLL_IN | UDT_UDT_EPOLL_ERR;
	    int len = sizeof(sin);
	    int id = -1;
	    int status;
	    char *username;
	    UDTSOCKET sock = udt_accept(s, (struct sockaddr *)&sin, &len);
	    status = AcceptConnection("udt", "udt", sock, NULL, 0, &id, &username);
	    if (status & 1) {
	      Client *new = memset(malloc(sizeof(Client)), 0, sizeof(Client));
	      new->id = id;
	      new->sock = sock;
	      new->next = ClientList;
	      new->username = username;
	      new->addr = *(int *)&sin.sin_addr;
	      new->host = getHostInfo(sock, NULL, NULL);
	      ClientList = new;
	      udt_epoll_add_usock(server_epoll, sock, &events);
	    }
	  } else {
	    Client *c;
	    for (c = ClientList; c;) {
	      if (c->sock == readfds[i]) {
		Client *c_chk;
		int c_epoll = udt_epoll_create();
		UDTSOCKET readfds[1];
		UDTSOCKET writefds[1];
		int readnum = 1;
		int writenum = 1;
		udt_epoll_add_usock(c_epoll, c->sock, NULL);
		status =
		    udt_epoll_wait2(c_epoll, readfds, &readnum, writefds, &writenum, 0, NULL, NULL,
				    NULL, NULL);
		udt_epoll_release(c_epoll);
		if (status != 0) {
		  CloseConnection(c->id);
		  break;
		}
		MdsSetClientAddr(c->addr);
		DoMessage(c->id);
		for (c_chk = ClientList; c_chk && c_chk != c; c_chk = c_chk->next) ;
		c = c_chk ? c->next : ClientList;
	      } else
		c = c->next;
	    }
	  }
	}
      }
    }
  } else {

      //////////////////////////////////////////////////////////////////////////
      // SERVER MODE                                ////////////////////////////
      // multiple connections with the same context ////////////////////////////

#ifdef _WIN32
    int sock = getSocketHandle(options[1].value);
#else
    UDTSOCKET sock = 0;
#endif
    int id;
    char *username;
    int status;
    status = AcceptConnection("udt", "udt", sock, NULL, 0, &id, &username);
    if (status & 1) {
      struct sockaddr_in sin;
      int n = sizeof(sin);
      Client *new = memset(malloc(sizeof(Client)), 0, sizeof(Client));
      if (udt_getpeername(sock, (struct sockaddr *)&sin, &n) == 0)
	MdsSetClientAddr(*(int *)&sin.sin_addr);
      new->id = id;
      new->sock = sock;
      new->next = ClientList;
      new->username = username;
      ClientList = new;
    }
    while (status & 1)
      status = DoMessage(id);
  }
  return 1;
}
