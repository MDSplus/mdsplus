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
typedef int socklen_t;
#define ioctl ioctlsocket
#define FIONREAD_TYPE u_long
#include <winsock2.h>
#include <ws2tcpip.h>
#define snprintf _snprintf
#define MSG_DONTWAIT 0
#include <io.h>
#define close closesocket
#include <process.h>
#define getpid _getpid
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#else
extern int pthread_mutex_init();
extern int pthread_mutex_lock();
extern int pthread_mutex_unlock();
#endif
#else
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#endif
#define SEND_BUF_SIZE 32768
#define RECV_BUF_SIZE 32768
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif
static ssize_t tcp_send(int conid, const void *buffer, size_t buflen, int nowait);
static ssize_t tcp_recv(int conid, void *buffer, size_t len);
static int tcp_disconnect(int conid);
static int tcp_flush(int conid);
static int tcp_listen(int argc, char **argv);
static int tcp_authorize(int conid, char *username);
static int tcp_connect(int conid, char *protocol, char *host);
static int tcp_reuseCheck(char *host, char *unique, size_t buflen);
static IoRoutines tcp_routines =
    { tcp_connect, tcp_send, tcp_recv, tcp_flush, tcp_listen, tcp_authorize, tcp_reuseCheck,
tcp_disconnect };

typedef struct _client {
  int sock;
  int id;
  char *username;
  struct _client *next;
} Client;

static Client *ClientList = 0;

static fd_set fdactive;

typedef struct _socket_list {
  int socket;
  struct _socket_list *next;
} SocketList;

static SocketList *Sockets = 0;

EXPORT IoRoutines *Io()
{
  return &tcp_routines;
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

static int getSocket(int conid)
{
  size_t len;
  char *info_name;
  int readfd;
  GetConnectionInfo(conid, &info_name, &readfd, &len);
  return (info_name && strcmp(info_name, "tcpv6") == 0) ? readfd : -1;
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

static void PushSocket(int socket)
{
  SocketList *oldhead;
  lock_socket_list();
  oldhead = Sockets;
  Sockets = malloc(sizeof(SocketList));
  Sockets->socket = socket;
  Sockets->next = oldhead;
  unlock_socket_list();
}

static void PopSocket(int socket)
{
  SocketList *p, *s;
  lock_socket_list();
  for (s = Sockets, p = 0; s && s->socket != socket; p = s, s = s->next) ;
  if (s) {
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
    shutdown(s->socket, 2);
  }
  unlock_socket_list();
}

static int tcp_authorize(int conid, char *username)
{
  int s = getSocket(conid);
  time_t tim = time(0);
  char *timestr = ctime(&tim);
  struct sockaddr_in6 sin;
  socklen_t n = sizeof(sin);
  int ans = 0;
  struct hostent *hp = 0;
  if (getpeername(s, (struct sockaddr *)&sin, &n) == 0) {
    char *matchString[2] = { 0, 0 };
    int num = 1;
    char iphost[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, &sin.sin6_addr, iphost, INET6_ADDRSTRLEN);
    timestr[strlen(timestr) - 1] = 0;
    hp = gethostbyaddr((char *)&sin.sin6_addr, sizeof(sin.sin6_addr), AF_INET6);
    if (hp && hp->h_name)
      printf("%s (%d) (pid %d) Connection received from %s@%s [%s]\r\n", timestr, s, getpid(),
	     username, hp->h_name, iphost);
    else
      printf("%s (%d) (pid %d) Connection received from %s@%s\r\n", timestr, s, getpid(), username,
	     iphost);
    matchString[0] = strcpy(malloc(strlen(username) + strlen(iphost) + 3), username);
    strcat(matchString[0], "@");
    strcat(matchString[0], iphost);
    if (hp && hp->h_name) {
      matchString[1] = strcpy(malloc(strlen(username) + strlen(hp->h_name) + 3), username);
      strcat(matchString[1], "@");
      strcat(matchString[1], hp->h_name);
      num = 2;
    }
    ans = CheckClient(username, num, matchString);
    if (matchString[0])
      free(matchString[0]);
    if (matchString[1])
      free(matchString[1]);
  } else {
    perror("Error determining connection info from socket\n");
  }
  fflush(stdout);
  fflush(stderr);
  return ans;
}

static ssize_t tcp_send(int conid, const void *bptr, size_t num, int nowait)
{
  int s = getSocket(conid);
  int options = nowait ? MSG_DONTWAIT : 0;
  ssize_t sent = -1;
  if (s != -1) {
    sent = send(s, bptr, num, options | MSG_NOSIGNAL);
  }
  return sent;
}

static ssize_t tcp_recv(int conid, void *bptr, size_t num)
{
  int s = getSocket(conid);
  ssize_t recved = -1;
  if (s != -1) {
    struct sockaddr_in6 sin;
    socklen_t n = sizeof(sin);
    PushSocket(s);
    signal(SIGABRT, ABORT);
    if (getpeername(s, (struct sockaddr *)&sin, &n) == 0)
      recved = recv(s, bptr, num, MSG_NOSIGNAL);
    else
      perror("Error getting connection information from socket\n");
    PopSocket(s);
  }
  return recved;
}

static int tcp_disconnect(int conid)
{
  int s = getSocket(conid);
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
      if (FD_ISSET(s, &fdactive)) {
	FD_CLR(s, &fdactive);
	if (getpeername(s, (struct sockaddr *)&sin, &n) == 0) {
	  char iphost[INET6_ADDRSTRLEN];
	  inet_ntop(AF_INET6, &sin.sin6_addr, iphost, INET6_ADDRSTRLEN);
	  timestr[strlen(timestr) - 1] = 0;
	  hp = gethostbyaddr((char *)&sin.sin6_addr, sizeof(sin.sin6_addr), AF_INET6);
	  if (hp)
	    printf("%s (%d) (pid %d) Connection disconnected from %s@%s [%s]\r\n", timestr, s,
		   getpid(), c->username, hp->h_name, iphost);
	  else
	    printf("%s (%d) (pid %d) Connection disconnected from %s@%s\r\n", timestr, s, getpid(),
		   c->username, iphost);
	}
      }
      free(c->username);
      free(c);
    }
    status = close(s);
    status = shutdown(s, 2);
  }
  fflush(stdout);
  fflush(stderr);
  return status;
}

static int tcp_flush(int conid)
{
  int sock = getSocket(conid);
  if (sock != -1) {
#if !defined(__sparc__)
    struct timeval timout = { 0, 1 };
    int status;
    ssize_t nbytes;
    int tries = 0;
    char buffer[1000];
    fd_set readfds, writefds;
    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);
    FD_ZERO(&writefds);
    FD_SET(sock, &writefds);
    while (((((status = select(FD_SETSIZE, &readfds, &writefds, 0, &timout)) > 0)
	     && FD_ISSET(sock, &readfds)) || (status == -1 && errno == EINTR)) && tries < 10) {
      tries++;
      if (FD_ISSET(sock, &readfds)) {
	status = ioctl(sock, FIONREAD, &nbytes);
	if (nbytes > 0 && status != -1) {
	  nbytes =
	    recv(sock, buffer, (sizeof(buffer) > (size_t)nbytes) ? (size_t)nbytes : sizeof(buffer), MSG_NOSIGNAL);
	  if (nbytes > 0)
	    tries = 0;
	}
      } else
	FD_SET(sock, &readfds);
      timout.tv_usec = 100000;
      FD_CLR(sock, &writefds);
    }
  }
#endif
  return 0;
}

static void SetSocketOptions(SOCKET s, int reuse)
{
  STATIC_CONSTANT int sendbuf = SEND_BUF_SIZE, recvbuf = RECV_BUF_SIZE;
  int one = 1;
  socklen_t len;
  static int debug_winsize = 0;
  static int init = 1;
  if (init) {
    char *winsize = getenv("TCP_WINDOW_SIZE");
    if (winsize) {
      sendbuf = atoi(winsize);
      recvbuf = atoi(winsize);
    }
    debug_winsize = (getenv("DEBUG_WINDOW_SIZE") != 0);
    init = 0;
  }
#ifndef _WIN32
  fcntl(s, F_SETFD, FD_CLOEXEC);
#endif
  setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char *)&recvbuf, sizeof(int));
  setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *)&sendbuf, sizeof(int));
  if (debug_winsize) {
    getsockopt(s, SOL_SOCKET, SO_RCVBUF, (void *)&recvbuf, &len);
    fprintf(stderr, "Got a recvbuf of %d\n", recvbuf);
    getsockopt(s, SOL_SOCKET, SO_SNDBUF, (void *)&sendbuf, &len);
    fprintf(stderr, "Got a sendbuf of %d\n", sendbuf);
  }
  setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (void *)&one, sizeof(one));
  setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (void *)&one, sizeof(one));
  if (reuse)
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (void *)&one, sizeof(one));
  setsockopt(s, SOL_SOCKET, SO_OOBINLINE, (void *)&one, sizeof(one));
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
  struct addrinfo hints, *info;
  int n;
  size_t i;
  static char *mdsip = "mdsip";
  char *host = strcpy((char *)malloc(strlen(hostin) + 1), hostin);
  char *service = mdsip;
  int num_colons;
  int ans = 1;
  InitializeSockets();
  for (i = 0, num_colons = 0; i < strlen(host); i++) {
    if (host[i] == ':')
      num_colons++;
  }
  if (num_colons > 0) {
    for (i = strlen(host); i > 0; i--) {
      if (host[i - 1] == ':') {
	service = &host[i];
	host[i - 1] = '\0';
	break;
      }
    }
  }
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET6;
  hints.ai_socktype = SOCK_STREAM;
  n = getaddrinfo(host, service, &hints, &info);
  if (n < 0) {
    fprintf(stderr, "getaddrinfo error:: [%s]\n", gai_strerror(n));
    ans = 0;
  } else {
    memcpy(sin, info->ai_addr, info->ai_addrlen);
    freeaddrinfo(info);
  }

  free(host);
  return ans;
}

static int tcp_reuseCheck(char *host, char *unique, size_t buflen)
{
  struct sockaddr_in6 sin;
  int status = getHostAndPort(host, &sin);
  if (status == 1) {
    snprintf(unique, buflen,
	     "tcpv6://%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%d",
	     sin.sin6_addr.s6_addr[0], sin.sin6_addr.s6_addr[1], sin.sin6_addr.s6_addr[2],
	     sin.sin6_addr.s6_addr[3], sin.sin6_addr.s6_addr[4], sin.sin6_addr.s6_addr[5],
	     sin.sin6_addr.s6_addr[6], sin.sin6_addr.s6_addr[7], sin.sin6_addr.s6_addr[8],
	     sin.sin6_addr.s6_addr[9], sin.sin6_addr.s6_addr[10], sin.sin6_addr.s6_addr[11],
	     sin.sin6_addr.s6_addr[12], sin.sin6_addr.s6_addr[13], sin.sin6_addr.s6_addr[14],
	     sin.sin6_addr.s6_addr[15], ntohs(sin.sin6_port));
    return 0;
  } else {
    *unique = 0;
    return -1;
  }
}

static int tcp_connect(int conid, char *protocol __attribute__ ((unused)), char *host)
{
  struct sockaddr_in6 sin;
  int s;
  int status = getHostAndPort(host, &sin);
  if (status == 1) {
    struct timeval connectTimer = { 0, 0 };
    InitializeSockets();
    s = socket(PF_INET6, SOCK_STREAM, 0);
    if (s == -1) {
      perror("Error in connect");
      return -1;
    }
    connectTimer.tv_sec = GetMdsConnectTimeout();
#ifndef _WIN32
    if (connectTimer.tv_sec) {
      status = fcntl(s, F_SETFL, O_NONBLOCK);
      status = connect(s, (struct sockaddr *)&sin, sizeof(sin));
      if ((status == -1) && (errno == EINPROGRESS)) {
	fd_set readfds;
	fd_set exceptfds;
	fd_set writefds;
	FD_ZERO(&readfds);
	FD_SET(s, &readfds);
	FD_ZERO(&exceptfds);
	FD_SET(s, &exceptfds);
	FD_ZERO(&writefds);
	FD_SET(s, &writefds);
	status = select(FD_SETSIZE, &readfds, &writefds, &exceptfds, &connectTimer);
	if (status == 0) {
	  fprintf(stderr, "Error in connect: Timeout on connection\n");
	  shutdown(s, 2);
	  close(s);
	  fflush(stderr);
	  return -1;
	}
      }
      if (status != -1)
	fcntl(s, F_SETFL, 0);
    } else
#endif
#ifdef SO_BINDTODEVICE
      status = setsockopt(s, SOL_SOCKET, SO_BINDTODEVICE, "eth0", strlen("eth0"));
#else
      // BSD and derivatives do not have SO_BINDTODEVICE
#endif
      status = connect(s, (struct sockaddr *)&sin, sizeof(sin));
    if (status == -1) {
      shutdown(s, 2);
      s = -1;
    }
    if (s == -1) {
      perror("Error in connect to service\n");
      fflush(stderr);
      return -1;
    }
    SetSocketOptions(s, 0);
    SetConnectionInfo(conid, "tcpv6", s, 0, 0);
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

static int tcp_listen(int argc, char **argv)
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
  if (GetMulti()) {
    unsigned short port = GetPort(GetPortname());
    char *matchString[] = { "multi" };
    int s;
    struct sockaddr_in6 sin;
    int tablesize = FD_SETSIZE;
    int error_count = 0;
    fd_set readfds;
    int status;
    FD_ZERO(&fdactive);
    CheckClient(0, 1, matchString);
    s = socket(PF_INET6, SOCK_STREAM, 0);
    if (s == -1) {
      printf("Error getting Connection Socket\n");
      exit(1);
    }
    FD_SET(s, &fdactive);
    SetSocketOptions(s, 1);
    memset(&sin, 0, sizeof(sin));
    sin.sin6_port = port;
    sin.sin6_family = AF_INET6;
    //    sin.sin_addr.s_addr = INADDR_ANY;
    status = bind(s, (struct sockaddr *)&sin, sizeof(struct sockaddr_in6));
    if (status < 0) {
      perror("Error binding to service\n");
      exit(1);
    }
    status = listen(s, 128);
    if (status < 0) {
      printf("Error from listen\n");
      exit(1);
    }
    while (1) {
      readfds = fdactive;
      if (select(tablesize, &readfds, 0, 0, 0) != -1) {
	error_count = 0;
	if (FD_ISSET(s, &readfds)) {
	  socklen_t len = sizeof(sin);
	  int id = -1;
	  int status;
	  char *username;
	  int sock = accept(s, (struct sockaddr *)&sin, &len);
	  SetSocketOptions(sock, 0);
	  status = AcceptConnection("tcpv6", "tcpv6", sock, 0, 0, &id, &username);
	  if (status & 1) {
	    Client *new = memset(malloc(sizeof(Client)), 0, sizeof(Client));
	    new->id = id;
	    new->sock = sock;
	    new->next = ClientList;
	    new->username = username;
	    ClientList = new;
	    FD_SET(sock, &fdactive);
	  }
	} else {
	  Client *c;
	  for (c = ClientList; c;) {
	    if (FD_ISSET(c->sock, &readfds)) {
	      Client *c_chk;
	      DoMessage(c->id);
	      for (c_chk = ClientList; c_chk && c_chk != c; c_chk = c_chk->next) ;
	      if (c_chk)
		FD_CLR(c->sock, &readfds);
	      c = ClientList;
	    } else
	      c = c->next;
	  }
	}
      } else {
	error_count++;
	perror("error in main select");
	fprintf(stderr, "Error count=%d\n", error_count);
	fflush(stderr);
	if (error_count > 100) {
	  fprintf(stderr, "Error count exceeded, shutting down\n");
	  exit(1);
	} else {
	  Client *c;
	  FD_ZERO(&fdactive);
	  if (s != -1)
	    FD_SET(s, &fdactive);
	  for (c = ClientList; c; c = c->next) {
	    struct sockaddr_in6 sin;
	    socklen_t n = sizeof(sin);
	    LockAsts();
	    if (getpeername(c->sock, (struct sockaddr *)&sin, &n)) {
	      fprintf(stderr, "Removed disconnected client\n");
	      fflush(stderr);
	      CloseConnection(c->id);
	    } else {
	      FD_SET(c->sock, &fdactive);
	    }
	    UnlockAsts();
	  }
	}
      }
    }
  } else {
#ifdef _WIN32
    int sock = getSocketHandle(options[1].value);
#else
    int sock = 0;
#endif
    int id;
    char *username;
    int status;
    status = AcceptConnection("tcpv6", "tcpv6", sock, 0, 0, &id, &username);
    if (status & 1) {
      Client *new = memset(malloc(sizeof(Client)), 0, sizeof(Client));
      new->id = id;
      new->sock = sock;
      new->next = ClientList;
      new->username = username;
      ClientList = new;
      FD_SET(sock, &fdactive);
    }
    while (status & 1)
      status = DoMessage(id);
  }
  return 1;
}
