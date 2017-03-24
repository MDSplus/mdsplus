#include "mdsip_connections.h"
#include <status.h>
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
#include <pthread_port.h>
#ifdef _WIN32
 #define ioctl ioctlsocket
 #define FIONREAD_TYPE u_long
 typedef int socklen_t;
 #define snprintf _snprintf
 #define MSG_DONTWAIT 0
 #include <io.h>
 #define close closesocket
 #include <process.h>
 #define getpid _getpid
#else
 #define INVALID_SOCKET -1
 #define FIONREAD_TYPE int
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
static IoRoutines tcp_routines = {
  tcp_connect, tcp_send, tcp_recv, tcp_flush, tcp_listen, tcp_authorize, tcp_reuseCheck, tcp_disconnect
};

#ifndef TCPV6
typedef struct _IoRoutineTcpInfo {
    int rcvbuf; /// < SO_RCVBUF
    int sndbuf; /// < SO_SNDBUF
    int reuse;  /// < reuse address
} IoRoutineTcpInfo;
#endif
/// Connected client definition for client list
typedef struct _client {
  int sock;
  int id;
  char *username;
#ifndef TCPV6
  int addr;
#endif
  struct _client *next;
} Client;

/// List of clients connected to server instance.
static Client *ClientList = NULL;

/// active select file descriptor
static fd_set fdactive;


////////////////////////////////////////////////////////////////////////////////
//  SOCKET LIST  ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/// List of active sockets
typedef struct _socket_list {
  SOCKET socket;
  struct _socket_list *next;
} Socket;

/// List of connected Sockets
static Socket *SocketList = NULL;

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
  return (info_name && strcmp(info_name, TCPVER) == 0) ? readfd : -1;
}

static pthread_mutex_t socket_mutex = PTHREAD_MUTEX_INITIALIZER;
#define LOCK_SOCKET_LIST   pthread_mutex_lock(&socket_mutex)
#define UNLOCK_SOCKET_LIST pthread_mutex_unlock(&socket_mutex)

static void PushSocket(SOCKET socket){
  Socket *oldhead;
  LOCK_SOCKET_LIST;
  oldhead = SocketList;
  SocketList = malloc(sizeof(Socket));
  SocketList->socket = socket;
  SocketList->next = oldhead;
  UNLOCK_SOCKET_LIST;
}

static void PopSocket(SOCKET socket){
  Socket *p, *s;
  LOCK_SOCKET_LIST;
  for (s = SocketList, p = 0; s && s->socket != socket; p = s, s = s->next) ;
  if (s) {
      if (p)
        p->next = s->next;
      else
        SocketList = s->next;
      free(s);
    }
  UNLOCK_SOCKET_LIST;
}

static void ABORT(int sigval __attribute__ ((unused))){
  Socket *s;
  LOCK_SOCKET_LIST;
  for (s = SocketList; s; s = s->next)
      shutdown(s->socket, 2);
  UNLOCK_SOCKET_LIST;
}

////////////////////////////////////////////////////////////////////////////////
//  AUTHORIZE  /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static int tcp_authorize(int conid, char *username)
{
  SOCKET s = getSocket(conid);
  time_t tim = time(0);
  char *timestr = ctime(&tim);
  STRUCT_SOCKADDR sin;
  socklen_t n = sizeof(sin);
  int ans = 0;
  struct hostent *hp = 0;
  if (getpeername(s, (struct sockaddr *)&sin, &n) == 0) {
    int num = 1;
    char *matchString[2] = { 0, 0 };
    timestr[strlen(timestr) - 1] = 0;
#ifdef TCPV6
    char iphost[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, &sin.sin6_addr, iphost, INET6_ADDRSTRLEN);
    hp = gethostbyaddr((char *)&sin.sin6_addr, sizeof(sin.sin6_addr), AF_INET6);
#else
    char *iphost = inet_ntoa(sin.sin_addr);
    hp = gethostbyaddr((char *)&sin.sin_addr, sizeof(sin.sin_addr), AF_INET);
#endif
    if (hp && hp->h_name)
      printf("%s (%d) (pid %d) Connection received from %s@%s [%s]\r\n", timestr, (int)s, getpid(),
	     username, hp->h_name, iphost);
    else
      printf("%s (%d) (pid %d) Connection received from %s@%s\r\n", timestr, (int)s, getpid(),
             username, iphost);
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

////////////////////////////////////////////////////////////////////////////////
//  SEND  //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static ssize_t tcp_send(int conid, const void *bptr, size_t num, int nowait){
  SOCKET s = getSocket(conid);
  int options = nowait ? MSG_DONTWAIT : 0;
  ssize_t sent = -1;
  if (s != INVALID_SOCKET)
    sent = send(s, bptr, num, options | MSG_NOSIGNAL);
  return sent;
}

////////////////////////////////////////////////////////////////////////////////
//  RECEIVE  ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static ssize_t tcp_recv(int conid, void *bptr, size_t num){
  SOCKET s = getSocket(conid);
  ssize_t recved = -1;
  if (s != INVALID_SOCKET) {
    STRUCT_SOCKADDR sin;
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

////////////////////////////////////////////////////////////////////////////////
//  DISCONNECT  ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static int tcp_disconnect(int conid){
  SOCKET s = getSocket(conid);
  int c_status = 0;
  time_t tim = time(0);
  char *timestr = ctime(&tim);
  STRUCT_SOCKADDR sin;
  socklen_t n = sizeof(sin);
  struct hostent *hp = 0;
  if (s != INVALID_SOCKET) {
    Client *c, **p;
    for (p = &ClientList, c = ClientList; c && c->id != conid; p = &c->next, c = c->next) ;
    if (c) {
      *p = c->next;
      if (FD_ISSET(s, &fdactive)) {
	FD_CLR(s, &fdactive);
	if (getpeername(s, (struct sockaddr *)&sin, &n) == 0) {
	  timestr[strlen(timestr) - 1] = 0;
#ifdef TCPV6
          char iphost[INET6_ADDRSTRLEN];
	  inet_ntop(AF_INET6, &sin.sin6_addr, iphost, INET6_ADDRSTRLEN);
	  hp = gethostbyaddr((char *)&sin.sin6_addr, sizeof(sin.sin6_addr), AF_INET6);
#else
	  char *iphost = inet_ntoa(sin.sin_addr);
	  hp = gethostbyaddr((char *)&sin.sin_addr, sizeof(sin.sin_addr), AF_INET);
#endif
	  if (hp)
	    printf("%s (%d) (pid %d) Connection disconnected from %s@%s [%s]\r\n", timestr, (int)s,
		   getpid(), c->username, hp->h_name, iphost);
	  else
	    printf("%s (%d) (pid %d) Connection disconnected from %s@%s\r\n", timestr, (int)s, getpid(),
		   c->username, iphost);
	}
      }
      free(c->username);
      free(c);
    }
    c_status = shutdown(s, 2);
    c_status = close(s);
  }
  fflush(stdout);
  fflush(stderr);
  return c_status;
}

////////////////////////////////////////////////////////////////////////////////
//  FLUSH  /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static int tcp_flush(int conid){
#if !defined(__sparc__)
  SOCKET s = getSocket(conid);
  if (s != INVALID_SOCKET) {
    struct timeval timout = { 0, 1 };
    int c_status;
    FIONREAD_TYPE nbytes;
    int tries = 0;
    char buffer[1000];
    fd_set readfds, writefds;
    FD_ZERO(&readfds);
    FD_SET(s, &readfds);
    FD_ZERO(&writefds);
    FD_SET(s, &writefds);
    while (((((c_status = select(FD_SETSIZE, &readfds, &writefds, 0, &timout)) > 0)
             && FD_ISSET(s, &readfds)) || (c_status == -1 && errno == EINTR)) && tries < 10) {
      tries++;
      if (FD_ISSET(s, &readfds)) {
        c_status = ioctl(s, FIONREAD, &nbytes);
        if (nbytes > 0 && c_status != -1) {
          nbytes = recv(s, buffer,
                        sizeof(buffer) > (size_t)nbytes ? nbytes : (FIONREAD_TYPE)sizeof(buffer),
                        MSG_NOSIGNAL);
          if (nbytes > 0)
            tries = 0;
        }
      } else
        FD_SET(s, &readfds);
      timout.tv_usec = 100000;
      FD_CLR(s, &writefds);
    }
  }
#endif
  return 0;
}

///
/// set socket options on the socket s for TCP protocol. This sets the receive
/// buffer, the send buffer, the SO_OOBINLINE, the SO_KEEPALIVE and TCP_NODELAY
/// .. at the moment
///
/// \param s socket to set options to
/// \param reuse set SO_REUSEADDR to be able to reuse the same address.
///
static void SetSocketOptions(SOCKET s, int reuse){
  int sendbuf = SEND_BUF_SIZE, recvbuf = RECV_BUF_SIZE;
  int one = 1;
#ifndef _WIN32
  fcntl(s, F_SETFD, FD_CLOEXEC);
#endif
  const char * tcp_window_size = getenv("TCP_WINDOW_SIZE");
  if(tcp_window_size && strlen(tcp_window_size)) {
      recvbuf = sendbuf = atoi(tcp_window_size);
      setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char *)&recvbuf, sizeof(int));
      setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *)&sendbuf, sizeof(int));
  }
  setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (void *)&one, sizeof(one));
  setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (void *)&one, sizeof(one));
  if (reuse)
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (void *)&one, sizeof(one));
  setsockopt(s, SOL_SOCKET, SO_OOBINLINE, (void *)&one, sizeof(one));
}

static short GetPort(char *name){
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

#ifdef TCPV6
static int getHostAndPort(char *hostin, STRUCT_SOCKADDR *sin){
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
#else
static int getHostAndPort(char *hostin, STRUCT_SOCKADDR *sin){
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
#endif

////////////////////////////////////////////////////////////////////////////////
//  REUSE CHECK  ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static int tcp_reuseCheck(char *host, char *unique, size_t buflen){
  STRUCT_SOCKADDR sin;
  if (getHostAndPort(host, &sin) == 1) {
#ifdef TCPV6
    snprintf(unique, buflen,
	     "tcpv6://%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%d",
	     sin.sin6_addr.s6_addr[0], sin.sin6_addr.s6_addr[1], sin.sin6_addr.s6_addr[2],
	     sin.sin6_addr.s6_addr[3], sin.sin6_addr.s6_addr[4], sin.sin6_addr.s6_addr[5],
	     sin.sin6_addr.s6_addr[6], sin.sin6_addr.s6_addr[7], sin.sin6_addr.s6_addr[8],
	     sin.sin6_addr.s6_addr[9], sin.sin6_addr.s6_addr[10], sin.sin6_addr.s6_addr[11],
	     sin.sin6_addr.s6_addr[12], sin.sin6_addr.s6_addr[13], sin.sin6_addr.s6_addr[14],
	     sin.sin6_addr.s6_addr[15], ntohs(sin.sin6_port));
#else
    char *addr = (char *)&sin.sin_addr;
    snprintf(unique, buflen, "tcp://%d.%d.%d.%d:%d", addr[0], addr[1], addr[2], addr[3], ntohs(sin.sin_port));
#endif
    return 0;
  } else {
    *unique = 0;
    return -1;
  }
}

////////////////////////////////////////////////////////////////////////////////
//  CONNECT  ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static int tcp_connect(int conid, char *protocol __attribute__ ((unused)), char *host){
  STRUCT_SOCKADDR sin;
  SOCKET s;
  int c_status = getHostAndPort(host, &sin);
  if (c_status == 1) {
    InitializeSockets();
    s = NEW_SOCKET;
    if (s == INVALID_SOCKET) {
      perror("Error creating socket");
      return -1;
    }
#ifndef _WIN32
    struct timeval connectTimer = { 0, 0 };
    connectTimer.tv_sec = GetMdsConnectTimeout();
    if (connectTimer.tv_sec) {
      c_status = fcntl(s, F_SETFL, O_NONBLOCK);
      c_status = connect(s, (struct sockaddr *)&sin, sizeof(sin));
      if ((c_status == -1) && (errno == EINPROGRESS)) {
        fd_set readfds;
        fd_set exceptfds;
        fd_set writefds;
        FD_ZERO(&readfds);
        FD_SET(s, &readfds);
        FD_ZERO(&exceptfds);
        FD_SET(s, &exceptfds);
        FD_ZERO(&writefds);
        FD_SET(s, &writefds);
        c_status = select(FD_SETSIZE, &readfds, &writefds, &exceptfds, &connectTimer);
        if (c_status == 0) {
          fprintf(stderr, "Error in connect: Timeout on connection\n");
          shutdown(s, 2);
          close(s);
          fflush(stderr);
          return -1;
        }
      }
      if (c_status != -1)
        fcntl(s, F_SETFL, 0);
    } else
#endif
#ifdef TCPV6
#ifdef SO_BINDTODEVICE
      c_status = setsockopt(s, SOL_SOCKET, SO_BINDTODEVICE, "eth0", strlen("eth0"));
#else
      // BSD and derivatives do not have SO_BINDTODEVICE
#endif
#endif //TCPV6
      c_status = connect(s, (struct sockaddr *)&sin, sizeof(sin));
    if (c_status == -1) {
      shutdown(s, 2);
      s = INVALID_SOCKET;
    }
    if (s == INVALID_SOCKET) {
      perror("Error in connect to service\n");
      fflush(stderr);
      return -1;
    }
    SetSocketOptions(s, 0);
    SetConnectionInfo(conid, TCPVER, s, 0, 0);
    return 0;
  } else if (c_status == 0) {
    fprintf(stderr, "Connect failed, unknown host\n");
    fflush(stderr);
    return -1;
  } else {
    fprintf(stderr, "Connect failed, unknown service\n");
    fflush(stderr);
    return -1;
  }
}

////////////////////////////////////////////////////////////////////////////////
//  LISTEN  ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
VOID CALLBACK ShutdownEvent(PVOID arg __attribute__ ((unused)), BOOLEAN fired __attribute__ ((unused))){
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
  SOCKET psock;
  char shutdownEventName[120];
  HANDLE shutdownEvent, waitHandle;
  if (name == 0 || sscanf(name, "%d:%d", &ppid, (int*)&psock) != 2) {
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
    fprintf(stderr, "Attempting to duplicate socket from pid %d socket %d\n", ppid, (int)psock);
    perror("Error duplicating socket from parent");
    exit(1);
  }
  sprintf(shutdownEventName, "MDSIP_%s_SHUTDOWN", GetPortname());
  shutdownEvent = CreateEvent(NULL, FALSE, FALSE, (LPCTSTR) shutdownEventName);
  if (!RegisterWaitForSingleObject(&waitHandle, shutdownEvent, ShutdownEvent, NULL, INFINITE, 0))
    perror("Error registering for shutdown event");
  return *(int *)&h;
}
#else
static void ChildSignalHandler(int num __attribute__ ((unused))){
  sigset_t set, oldset;
  pid_t pid;
  int c_status;
  /* block other incoming SIGCHLD signals */
  sigemptyset(&set);
  sigaddset(&set, SIGCHLD);
  sigprocmask(SIG_BLOCK, &set, &oldset);
  /* wait for child */
  while ((pid = waitpid((pid_t) - 1, &c_status, WNOHANG)) > 0) {
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
    //////////////////////////////////////////////////////////////////////////
    // MULTIPLE CONNECTION MODE              /////////////////////////////////
    // multiple connections with own context /////////////////////////////////
    unsigned short port = GetPort(GetPortname());
    char *matchString[] = { "multi" };
    int s;
    STRUCT_SOCKADDR sin;
    int tablesize = FD_SETSIZE;
    int error_count = 0;
    fd_set readfds;
    int c_status;
    FD_ZERO(&fdactive);
    CheckClient(0, 1, matchString);
    // SOCKET //
    /* Create the socket and set it up to accept connections. */
    s = NEW_SOCKET;
    if (s == -1) {
      printf("Error getting Connection Socket\n");
      exit(1);
    }
    FD_SET(s, &fdactive);
    // OPTIONS //
    SetSocketOptions(s, 1);
    memset(&sin, 0, sizeof(sin));
#ifdef TCPV6
    sin.sin6_port = port;
    sin.sin6_family = AF_INET6;
#else
    sin.sin_port = port;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
#endif
    // BIND //
    c_status = bind(s, (struct sockaddr *)&sin, sizeof(STRUCT_SOCKADDR));
    if (c_status < 0) {
      perror("Error binding to service\n");
      exit(1);
    }
    // LISTEN //
    c_status = listen(s, 128);
    if (c_status < 0) {
      printf("Error from listen\n");
      exit(1);
    }
    // LISTEN LOOP ///////////////////////////////////////////////////////////
    for(;;) {
      readfds = fdactive;
      // SELECT select read ready from socket list //
      if (select(tablesize, &readfds, 0, 0, 0) != -1) {
        error_count = 0;
        if (FD_ISSET(s, &readfds)) {
          socklen_t len = sizeof(sin);
          int id = -1;
          char *username;
          // ACCEPT new connection and register new socket //
          int sock = accept(s, (struct sockaddr *)&sin, &len);
          SetSocketOptions(sock, 0);
	  int status = AcceptConnection(TCPVER, TCPVER, sock, 0, 0, &id, &username);
          if STATUS_OK {
            // add client to client list //
            Client *client = memset(malloc(sizeof(Client)), 0, sizeof(Client));
            client->id = id;
            client->sock = sock;
            client->next = ClientList;
            client->username = username;
#ifndef TCPV6
            client->addr = *(int *)&sin.sin_addr;
#endif
            ClientList = client;
            // add socket to active sockets //
            FD_SET(sock, &fdactive);
          }
        } else {
          // Process Clients in list searching for active sockets //
          Client *c = ClientList;
          while (c) {
            if (FD_ISSET(c->sock, &readfds)) {
              // process active socket client //
#ifndef TCPV6
              MdsSetClientAddr(c->addr);
#endif
              // DO MESSAGE ---> ProcessMessage() on client c //
              DoMessage(c->id);
              Client *c_chk;
              for (c_chk = ClientList; c_chk && c_chk != c; c_chk = c_chk->next) ;
              if (c_chk) FD_CLR(c->sock, &readfds);
              c = ClientList;
            } else
              c = c->next;
          }
        }
      } else if (errno==EINTR){
        exit(EINTR);// signal interrupt
      } else {// Select returned -1 error code
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
            STRUCT_SOCKADDR sin;
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
    }// end LISTEN LOOP //
  } else {
  //////////////////////////////////////////////////////////////////////////
  // SERVER MODE                                ////////////////////////////
  // multiple connections with the same context ////////////////////////////
#ifdef _WIN32
    int sock = getSocketHandle(options[1].value);
#else
    int sock = 0;
#endif
    int id;			// set by AcceptConnection
    char *username;		// set by AcceptConnection
    int status = AcceptConnection(TCPVER, TCPVER, sock, 0, 0, &id, &username);
    if STATUS_OK {
#ifndef TCPV6
      STRUCT_SOCKADDR sin;
      socklen_t n = sizeof(sin);
      if (getpeername(sock, (struct sockaddr *)&sin, &n) == 0)
        MdsSetClientAddr(*(int *)&sin.sin_addr);
#endif
      Client *client = memset(malloc(sizeof(Client)), 0, sizeof(Client));
      client->id = id;
      client->sock = sock;
      client->next = ClientList;
      client->username = username;
      ClientList = client;
      FD_SET(sock, &fdactive);
    }
    while STATUS_OK
      // DO MESSAGE ---> ProcessMessage() in MdsIpSrvShr //
      status = DoMessage(id);
  }
  return 1;
}
