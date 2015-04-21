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
#include <winsock2.h>
#include <ws2tcpip.h>
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
static ssize_t UDTV6_send(int conid, const void *buffer, size_t buflen, int nowait);
static ssize_t UDTV6_recv(int conid, void *buffer, size_t len);
static int UDTV6_disconnect(int conid);
static int UDTV6_flush(int conid);
static int UDTV6_listen(int argc, char **argv);
static int UDTV6_authorize(int conid, char *username);
static int UDTV6_connect(int conid, char *protocol, char *host);
static int UDTV6_reuseCheck(char *host, char *unique, size_t buflen);
static IoRoutines UDTV6_routines =
    { UDTV6_connect, UDTV6_send, UDTV6_recv, UDTV6_flush, UDTV6_listen, UDTV6_authorize,
UDTV6_reuseCheck, UDTV6_disconnect };

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
  return &UDTV6_routines;
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
  void *info = GetConnectionInfo(conid, &info_name, &readfd, &len);
  return (info_name && strcmp(info_name, "udtv6") == 0) ? (UDTSOCKET) readfd : (UDTSOCKET) - 1;
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

static void ABORT(int sigval)
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
  struct sockaddr_in6 sin;
  socklen_t n = sizeof(sin);
  if (udt_getpeername(s, (struct sockaddr *)&sin, &n) == 0) {
    struct hostent *hp = 0;
    char *matchString[2] = { 0, 0 };
    char *hoststr = 0;
    int num = 1;
    char straddr[INET6_ADDRSTRLEN];
    const char *iphost = inet_ntop(AF_INET6, &sin.sin6_addr, straddr, sizeof(straddr));

    hp = gethostbyaddr((char *)&sin.sin6_addr, sizeof(sin.sin6_addr), AF_INET6);
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

static int UDTV6_authorize(int conid, char *username)
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

static ssize_t UDTV6_send(int conid, const void *bptr, size_t num, int nowait)
{
  UDTSOCKET s = getSocket(conid);
  int options = nowait ? MSG_DONTWAIT : 0;
  ssize_t sent = -1;
  if (s != -1) {
    sent = udt_send(s, bptr, num, options | MSG_NOSIGNAL);
  }
  return sent;
}

static ssize_t UDTV6_recv(int conid, void *bptr, size_t num)
{
  UDTSOCKET s = getSocket(conid);
  ssize_t recved = -1;
  if (s != -1) {
    struct sockaddr sin;
    socklen_t n = sizeof(sin);
    int status;
    PushSocket(s);
    signal(SIGABRT, ABORT);
    if ((status = udt_getpeername(s, (struct sockaddr *)&sin, &n)) == 0)
      recved = udt_recv(s, bptr, num, MSG_NOSIGNAL);
    else
      fprintf(stderr, "Error getting connection information from udtv6 socket. Status=%d\n%s\n",
	      status, udt_getlasterror_desc());
    PopSocket(s);
  }
  return recved;
}

static int UDTV6_disconnect(int conid)
{
  UDTSOCKET s = getSocket(conid);
  int status = 0;
  time_t tim = time(0);
  char *timestr = ctime(&tim);
  struct sockaddr_in6 sin;
  socklen_t n = sizeof(sin);
  struct hostent *hp = 0;
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

static int UDTV6_flush(int conid)
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
    while ((((status = udt_epoll_wait2(epoll, readfds, &readfds_num,
				       NULL, NULL, timeout, NULL, NULL, NULL, NULL)) == 0) &&
	    sock == readfds[0]) || (status < 0) && tries < 10) {
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

static int getHostAndPort(char *hostin, struct sockaddr_in6 *sin)
{
  size_t i;
  static char const *mdsip = "mdsip";
  char *host = strcpy((char *)malloc(strlen(hostin) + 1), hostin);
  char const *service;
  int status = 0;
  struct addrinfo *res = 0;
  static const struct addrinfo hints = { 0, AF_INET6, SOCK_STREAM, 0, 0, 0, 0, 0 };
  int gai_stat;
  InitializeSockets();
  for (i = 0; i < strlen(host) && host[i] != '#'; i++) ;
  if (i < strlen(host)) {
    host[i] = '\0';
    service = &host[i + 1];
  } else {
    service = mdsip;
  }
  if ((gai_stat = getaddrinfo(host, service, &hints, &res)) == 0 && res &&
      res->ai_family == AF_INET6 &&
      res->ai_socktype == SOCK_STREAM && res->ai_addrlen == sizeof(*sin)) {
    memcpy(sin, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);
    status = 1;
  } else {
    printf("Error connecting to host: %s, port %s error=%s\n", host, service,
	   gai_strerror(gai_stat));
  }
  free(host);
  return status;
}

static int UDTV6_reuseCheck(char *host, char *unique, size_t buflen)
{
  struct sockaddr_in6 sin;
  int status = getHostAndPort(host, &sin);
  if (status == 1) {
    unsigned short *addr = (unsigned short *)&sin.sin6_addr;
    snprintf(unique, buflen, "udtv6://%x:%x:%x:%x:%x:%x:%x:%x#%x", addr[0], addr[1], addr[2],
	     addr[3], addr[4], addr[5], addr[6], addr[7], ntohs(sin.sin6_port));
    return 0;
  } else {
    *unique = 0;
    return -1;
  }
}

static int UDTV6_connect(int conid, char *protocol, char *host)
{
  struct sockaddr_in6 sin;
  UDTSOCKET s;
  int status = getHostAndPort(host, &sin);
  int sinlen = sizeof(sin);
  if (status == 1) {
    struct timeval connectTimer = { 0, 0 };
    InitializeSockets();
    s = udt_socket(AF_INET6, SOCK_STREAM, 0);
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
    SetConnectionInfo(conid, "udtv6", s, NULL, 0);
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
  char logfile[1024];
  HANDLE h;
  int ppid;
  int psock;
  char shutdownEventName[120];
  HANDLE shutdownEvent, waitHandle;
  if (name == 0 || sscanf(name, "%d:%d", &ppid, &psock) != 2) {
    fprintf(stderr, "Mdsip single connection server can only be started from windows service\n");
    exit(1);
  }
  sprintf(logfile, "C:\\MDSIP_%s_%d.log", GetPortname(), _getpid());
  freopen(logfile, "a", stdout);
  freopen(logfile, "a", stderr);
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
static void ChildSignalHandler(int num)
{
  sigset_t set, oldset;
  pid_t pid;
  int status, exitstatus;
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

static int UDTV6_listen(int argc, char **argv)
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
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;
    char *matchString[] = { "multi" };
    UDTSOCKET s;
    struct sockaddr_in6 sin;
    int addrlen = sizeof(sin);
    int error_count = 0;
    UDTSOCKET readfds[1024];
    UDTSOCKET writefds[1024];
    int one = 1;
    int status;
    int events = UDT_UDT_EPOLL_IN | UDT_UDT_EPOLL_ERR;
    int gai_stat;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    gai_stat = getaddrinfo(NULL, GetPortname(), &hints, &result);
    if (gai_stat != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_stat));
      exit(EXIT_FAILURE);
    }
    CheckClient(0, 1, matchString);
    for (rp = result; rp != NULL; rp = rp->ai_next) {
      s = udt_socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
      if (s == -1)
	continue;
      if (udt_bind(s, rp->ai_addr, rp->ai_addrlen) == 0)
	break;
      close(s);
    }
    udt_epoll_add_usock(server_epoll, s, &events);
    memset(&sin, 0, sizeof(sin));
    status = udt_listen(s, 128);
    if (status < 0) {
      fprintf(stderr, "Error return by udt_listen: %s\n", udt_getlasterror_desc());
      exit(EXIT_FAILURE);
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
	    struct sockaddr sin;
	    socklen_t n = sizeof(sin);
	    //      printf("client->sock=%d\n",c->sock);
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
	error_count = 0;
	for (i = 0; readfds_num != 1024 && i < readfds_num; i++) {
	  if (readfds[i] == s) {
	    int events = UDT_UDT_EPOLL_IN | UDT_UDT_EPOLL_ERR;
	    socklen_t len = sizeof(sin);
	    int id = -1;
	    int status;
	    char *username;
	    UDTSOCKET sock = udt_accept(s, (struct sockaddr *)&sin, &len);
	    status = AcceptConnection("udtv6", "udtv6", sock, NULL, 0, &id, &username);
	    if (status & 1) {
	      Client *new = memset(malloc(sizeof(Client)), 0, sizeof(Client));
	      new->id = id;
	      new->sock = sock;
	      new->next = ClientList;
	      new->username = username;
	      new->addr = *(int *)&sin.sin6_addr;
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
#ifdef _WIN32
    int sock = getSocketHandle(options[1].value);
#else
    UDTSOCKET sock = 0;
#endif
    int id;
    char *username;
    int status;
    status = AcceptConnection("udtv6", "udtv6", sock, NULL, 0, &id, &username);
    if (status & 1) {
      struct sockaddr_in6 sin;
      socklen_t n = sizeof(sin);
      Client *new = memset(malloc(sizeof(Client)), 0, sizeof(Client));
      if (udt_getpeername(sock, (struct sockaddr *)&sin, &n) == 0)
	MdsSetClientAddr(*(int *)&sin.sin6_addr);
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
