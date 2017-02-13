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
 #define ioctl ioctlsocket
 #define FIONREAD_TYPE u_long
 typedef int socklen_t;
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
static IoRoutines tcp_routines =
{ tcp_connect, tcp_send, tcp_recv, tcp_flush, tcp_listen, tcp_authorize, tcp_reuseCheck,
  tcp_disconnect
};

/// Connected client definition for client list
typedef struct _client {
  int sock;
  int id;
  char *username;
  int addr;
  struct _client *next;
} Client;

/// List of clients connected to server instance.
static Client *ClientList = 0;

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
static Socket *SocketList = 0;

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
  return (info_name && strcmp(info_name, "tcp") == 0) ? readfd : -1;
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

static void PushSocket(SOCKET socket)
{
  Socket *oldhead;
  lock_socket_list();
  oldhead = SocketList;
  SocketList = malloc(sizeof(Socket));
  SocketList->socket = socket;
  SocketList->next = oldhead;
  unlock_socket_list();
}

static void PopSocket(SOCKET socket)
{
  Socket *p, *s;
  lock_socket_list();
  for (s = SocketList, p = 0; s && s->socket != socket; p = s, s = s->next) ;
  if (s) {
      if (p)
        p->next = s->next;
      else
        SocketList = s->next;
      free(s);
    }
  unlock_socket_list();
}



////////////////////////////////////////////////////////////////////////////////
//  AUTHORIZE  /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


static int tcp_authorize(int conid, char *username)
{
  SOCKET s = getSocket(conid);
  time_t tim = time(0);
  char *timestr = ctime(&tim);
  struct sockaddr_in sin;
  socklen_t n = sizeof(sin);
  int ans = 0;
  struct hostent *hp = 0;
  int num = 1;
  if (getpeername(s, (struct sockaddr *)&sin, &n) == 0) {
    char *matchString[2] = { 0, 0 };
    char *iphost = inet_ntoa(sin.sin_addr);
    timestr[strlen(timestr) - 1] = 0;
    hp = gethostbyaddr((char *)&sin.sin_addr, sizeof(sin.sin_addr), AF_INET);
    if (hp && hp->h_name)
      printf("%s (%d) (pid %d) Connection received from %s@%s [%s]\r\n", timestr, (int)s, getpid(),
	     username, hp->h_name, iphost);
    else
      printf("%s (%d) (pid %d) Connection received from %s@%s\r\n", timestr, (int)s, getpid(), username,
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

////////////////////////////////////////////////////////////////////////////////
//  SEND  //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


static ssize_t tcp_send(int conid, const void *bptr, size_t num, int nowait){
  SOCKET s = getSocket(conid);
  int options = nowait ? MSG_DONTWAIT : 0;
  ssize_t sent = -1;
  if (s != INVALID_SOCKET) {
      sent = send(s, bptr, num, options | MSG_NOSIGNAL);
    }
  return sent;
}

////////////////////////////////////////////////////////////////////////////////
//  RECEIVE  ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


static void ABORT(int sigval __attribute__ ((unused))){
  Socket *s;
  lock_socket_list();
  for (s = SocketList; s; s = s->next) {
      shutdown(s->socket, 2);
    }
  unlock_socket_list();
}

static ssize_t tcp_recv(int conid, void *bptr, size_t num){
  SOCKET s = getSocket(conid);
  ssize_t recved = -1;
  if (s != INVALID_SOCKET) {
      struct sockaddr sin;
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
  int status = 0;
  time_t tim = time(0);
  char *timestr = ctime(&tim);
  struct sockaddr_in sin;
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
	  char *iphost = inet_ntoa(sin.sin_addr);
	  timestr[strlen(timestr) - 1] = 0;
	  hp = gethostbyaddr((char *)&sin.sin_addr, sizeof(sin.sin_addr), AF_INET);
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
    status = shutdown(s, 2);
    status = close(s);
  }
  fflush(stdout);
  fflush(stderr);
  return status;
}


////////////////////////////////////////////////////////////////////////////////
//  FLUSH  /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// Flush pending TCP packets
///
static int tcp_flush(int conid){
  SOCKET sock = getSocket(conid);
  if (sock != INVALID_SOCKET) {
#if !defined(__sparc__)
      struct timeval timout = { 0, 1 };
      int status;
      FIONREAD_TYPE nbytes;
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
		  recv(sock, buffer, sizeof(buffer) > (size_t)nbytes ? nbytes : (FIONREAD_TYPE)sizeof(buffer), MSG_NOSIGNAL);
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



///
/// set socket options on the socket s for TCP protocol. This sets the receive
/// buffer, the send buffer, the SO_OOBINLINE, the SO_KEEPALIVE and TCP_NODELAY
/// .. at the moment
///
/// \param s socket to set options to
/// \param reuse set SO_REUSEADDR to be able to reuse the same address.
///
static void SetSocketOptions(SOCKET s, int reuse){
//  fprintf(stderr,"Set socket OPTIONS on socket: %i //// \n",s);

  STATIC_CONSTANT int sendbuf = SEND_BUF_SIZE, recvbuf = RECV_BUF_SIZE;
  int one = 1;
  socklen_t len;
  static int debug_winsize = 0;
  static int init = 1;
  static int set_window_size = 0;
  if (init) {
      char *winsize = getenv("TCP_WINDOW_SIZE");
      if (winsize) {
          sendbuf = atoi(winsize);
          recvbuf = atoi(winsize);
          set_window_size = 1;
        }
      debug_winsize = (getenv("DEBUG_WINDOW_SIZE") != 0);
      init = 0;
    }
#ifndef _WIN32
  fcntl(s, F_SETFD, FD_CLOEXEC);
#endif
  if(set_window_size) {
      setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char *)&recvbuf, sizeof(int));
      setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *)&sendbuf, sizeof(int));
  }
  if (debug_winsize) {
      getsockopt(s, SOL_SOCKET, SO_RCVBUF, (void *)&recvbuf, &len);
      fprintf(stderr, "Got a recvbuf of %d\n", recvbuf);
      getsockopt(s, SOL_SOCKET, SO_SNDBUF, (void *)&sendbuf, &len);
      fprintf(stderr, "Got a sendbuf of %d\n", sendbuf);
      fprintf(stderr, "Compression level: %d\n", GetCompressionLevel() );
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

static int getHostAndPort(char *hostin, struct sockaddr_in *sin){
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


////////////////////////////////////////////////////////////////////////////////
//  REUSE CHECK  ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


static int tcp_reuseCheck(char *host, char *unique, size_t buflen){
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


////////////////////////////////////////////////////////////////////////////////
//  CONNECT  ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


static int tcp_connect(int conid, char *protocol __attribute__ ((unused)), char *host){
  struct sockaddr_in sin;
  SOCKET s;
  int status = getHostAndPort(host, &sin);
  if (status == 1) {
      InitializeSockets();
      s = socket(AF_INET, SOCK_STREAM, 0);
      if (s == INVALID_SOCKET) {
          perror("Error creating socket");
          return -1;
        }
#ifndef _WIN32
      struct timeval connectTimer = { 0, 0 };
      connectTimer.tv_sec = GetMdsConnectTimeout();
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
        status = connect(s, (struct sockaddr *)&sin, sizeof(sin));
      if (status == -1) {
          shutdown(s, 2);
          s = INVALID_SOCKET;
        }
      if (s == INVALID_SOCKET) {
          perror("Error in connect to service\n");
          fflush(stderr);
          return -1;
        }
      SetSocketOptions(s, 0);
      SetConnectionInfo(conid, "tcp", s, 0, 0);
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
      //////////////////////////////////////////////////////////////////////////
      // MULTIPLE CONNECTION MODE              /////////////////////////////////
      // multiple connections with own context /////////////////////////////////
      unsigned short port = GetPort(GetPortname());
      char *matchString[] = { "multi" };
      int s;
      struct sockaddr_in sin;
      int tablesize = FD_SETSIZE;
      int error_count = 0;
      fd_set readfds;
      int status;
      FD_ZERO(&fdactive);
      CheckClient(0, 1, matchString);
      // SOCKET //
      /* Create the socket and set it up to accept connections. */
      s = socket(AF_INET, SOCK_STREAM, 0);
      if (s == -1) {
          printf("Error getting Connection Socket\n");
          exit(1);
        }
      FD_SET(s, &fdactive);
      // OPTIONS //
      SetSocketOptions(s, 1);
      memset(&sin, 0, sizeof(sin));
      sin.sin_port = port;
      sin.sin_family = AF_INET;
      sin.sin_addr.s_addr = INADDR_ANY;
      // BIND //
      status = bind(s, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
      if (status < 0) {
          perror("Error binding to service\n");
          exit(1);
        }
      // LISTEN //
      status = listen(s, 128);
      if (status < 0) {
          printf("Error from listen\n");
          exit(1);
        }

      // LISTEN LOOP ///////////////////////////////////////////////////////////
      while (1) {
          readfds = fdactive;
          // SELECT select read ready from socket list //
          if (select(tablesize, &readfds, 0, 0, 0) != -1) {
              error_count = 0;
              if (FD_ISSET(s, &readfds)) {
                  socklen_t len = sizeof(sin);
                  int status;
                  int id = -1;
                  char *username;
                  // ACCEPT new connection and register new socket //
                  int sock = accept(s, (struct sockaddr *)&sin, &len);
                  SetSocketOptions(sock, 0);
                  status = AcceptConnection("tcp", "tcp", sock, 0, 0, &id, &username);
                  if (status & 1) {
                      // add client to client list //
                      Client *client = memset(malloc(sizeof(Client)), 0, sizeof(Client));
                      client->id = id;
                      client->sock = sock;
                      client->next = ClientList;
                      client->username = username;
                      client->addr = *(int *)&sin.sin_addr;
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
                          MdsSetClientAddr(c->addr);
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
            } else {
              // Select returned -1 error code //
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
                      struct sockaddr sin;
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
        }				// end LISTEN LOOP //
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
      int status = AcceptConnection("tcp", "tcp", sock, 0, 0, &id, &username);
      if (status & 1) {
          struct sockaddr_in sin;
          socklen_t n = sizeof(sin);
          Client *client = memset(malloc(sizeof(Client)), 0, sizeof(Client));
          if (getpeername(sock, (struct sockaddr *)&sin, &n) == 0)
            MdsSetClientAddr(*(int *)&sin.sin_addr);
          client->id = id;
          client->sock = sock;
          client->next = ClientList;
          client->username = username;
          ClientList = client;
          FD_SET(sock, &fdactive);
        }
      while (status & 1)
        // DO MESSAGE ---> ProcessMessage() in MdsIpSrvShr //
        status = DoMessage(id);
    }
  return 1;
}
