#define _TCP
DEFINE_INITIALIZESOCKETS;
#ifdef _WIN32
static void socketerror() {
  int err;
  switch (err = WSAGetLastError()) {
  case 0:
    perror("");
    break;
  case WSANOTINITIALISED:
    fprintf(stderr, "WSANOTINITIALISED\n");
    break;
  case WSAENETDOWN:
    fprintf(stderr, "WSAENETDOWN\n");
    break;
  case WSAEADDRINUSE:
    fprintf(stderr, "WSAEADDRINUSE\n");
    break;
  case WSAEINTR:
    fprintf(stderr, "WSAEINTR\n");
    break;
  case WSAENOTCONN:
    fprintf(stderr, "WSAENOTCONN\n");
    break;
  case WSAEINPROGRESS:
    fprintf(stderr, "WSAEINPROGRESS\n");
    break;
  case WSAEALREADY:
    fprintf(stderr, "WSAEALREADY\n");
    break;
  case WSAEADDRNOTAVAIL:
    fprintf(stderr, "WSAEADDRNOTAVAIL\n");
    break;
  case WSAEAFNOSUPPORT:
    fprintf(stderr, "WSAEAFNOSUPPORT\n");
    break;
  case WSAECONNREFUSED:
    fprintf(stderr, "WSAECONNREFUSED\n");
    break;
  case WSAENOPROTOOPT:
    fprintf(stderr, "WSAENOPROTOOPT\n");
    break;
  case WSAEFAULT:
    fprintf(stderr, "WSAEFAULT\n");
    break;
  case WSAENOTSOCK:
    fprintf(stderr, "WSAENOTSOCK\n");
    break;
  case WSAESHUTDOWN:
    fprintf(stderr, "WSAESHUTDOWN\n");
    break;
  case WSAEHOSTUNREACH:
    fprintf(stderr, "WSAEHOSTUNREACH\n");
    break;
  case WSAEACCES:
    fprintf(stderr, "WSAEACCES\n");
    break;
  default:
    fprintf(stderr, "WSA %d\n", err);
  }
}
#define PERROR(...)                                                            \
  do {                                                                         \
    fprintf(stderr, __VA_ARGS__);                                              \
    fprintf(stderr, ": ");                                                     \
    socketerror();                                                             \
  } while (0)
#else
#define PERROR(...)                                                            \
  do {                                                                         \
    fprintf(stderr, __VA_ARGS__);                                              \
    fprintf(stderr, ": ");                                                     \
    perror("");                                                                \
  } while (0)
#include <netinet/tcp.h>
#endif
#define SOCKLEN_T socklen_t
#define GETPEERNAME getpeername
#define SEND send
#define RECV recv
// active select file descriptor
static fd_set fdactive;
static int io_flush(Connection *c);
#include "ioroutinesx.h"
////////////////////////////////////////////////////////////////////////////////
//  CONNECT  ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///
/// set socket options on the socket s for TCP protocol. This sets the receive
/// buffer, the send buffer, the SO_OOBINLINE, the SO_KEEPALIVE and TCP_NODELAY
/// .. at the moment
///
/// \param s socket to set options to
/// \param reuse set SO_REUSEADDR to be able to reuse the same address.
///
static void SetSocketOptions(SOCKET s, int reuse) {
  int sendbuf = SEND_BUF_SIZE, recvbuf = RECV_BUF_SIZE;
  int one = 1;
#ifndef _WIN32
  fcntl(s, F_SETFD, FD_CLOEXEC);
#endif
  const char *tcp_window_size = getenv("TCP_WINDOW_SIZE");
  if (tcp_window_size && strlen(tcp_window_size)) {
    recvbuf = sendbuf = strtol(tcp_window_size, NULL, 0);
    setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char *)&recvbuf, sizeof(int));
    setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *)&sendbuf, sizeof(int));
  }
  setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (void *)&one, sizeof(one));
  setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (void *)&one, sizeof(one));
  if (reuse)
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (void *)&one, sizeof(one));
  setsockopt(s, SOL_SOCKET, SO_OOBINLINE, (void *)&one, sizeof(one));
}

/* io_connect()
 * interruptable connect implementation with optional timeout
 * _WIN32 requires hack to break out of select():
 *   create a self connected DGRAM socket and close it on SIGINT
 */
#ifdef _WIN32
static SOCKET int_sock = INVALID_SOCKET;
static void *old_handler;
static void int_select(int signo) {
  signal(signo, old_handler);
  raise(signo);
  if (int_sock != INVALID_SOCKET)
    close(int_sock);
}
#endif
static int io_connect(Connection *c, char *protocol __attribute__((unused)),
                      char *host) {
  struct SOCKADDR_IN sin;
  SOCKET sock;
  if
    IS_NOT_OK(GetHostAndPort(host, &sin)) {
      fprintf(stderr, "Connect failed to host: %s\n", host);
      return C_ERROR;
    }
  INITIALIZESOCKETS;
  sock = socket(AF_T, SOCK_STREAM, 0);
  if (sock == INVALID_SOCKET) {
    PERROR("Error creating socket");
    return C_ERROR;
  }
  struct timeval connectTimer = {0, 0};
  connectTimer.tv_sec = GetMdsConnectTimeout();
  int err;
#ifdef _WIN32
  struct timeval *timeout = connectTimer.tv_sec > 0 ? &connectTimer : NULL;
  u_long ul = TRUE;
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = 0;
  addr.sin_addr.s_addr = INADDR_LOOPBACK;
  int len = sizeof(addr);
  int_sock = socket(AF_INET, SOCK_DGRAM, 0);
  bind(int_sock, (struct sockaddr *)&addr, len);
  getsockname(int_sock, (struct sockaddr *)&addr, &len);
  connect(int_sock, (struct sockaddr *)&addr, len);
  SOCKET maxsock = sock > int_sock ? sock + 1 : int_sock + 1;
  err = ioctlsocket(sock, FIONBIO, &ul);
  if (err == 0)
    err = connect(sock, (struct sockaddr *)&sin, sizeof(sin));
  if ((err == -1) && (WSAGetLastError() == WSAEWOULDBLOCK)) {
    fd_set rdfds, wrfds;
    FD_ZERO(&wrfds);
    FD_ZERO(&rdfds);
    FD_SET(sock, &wrfds);
    FD_SET(int_sock, &rdfds);
    old_handler = signal(SIGINT, int_select);
    err = select(maxsock, &rdfds, &wrfds, NULL, timeout);
    signal(SIGINT, old_handler);
    if (err < 1) {
      if (err < 0)
        PERROR("Error in connect");
      else
        fprintf(stderr, "Error in connect: timeout ?!\n");
      close(int_sock);
      goto error;
    }
    if (FD_ISSET(int_sock, &rdfds)) {
      errno = EINTR;
      perror("Error in connect");
      goto error;
    }
    close(int_sock);
    socklen_t len = sizeof(err);
    getsockopt(sock, SOL_SOCKET, SO_ERROR, (char *)&err, &len);
  }
  ul = FALSE;
  ioctlsocket(sock, FIONBIO, &ul);
#else  // _WIN32
  if (connectTimer.tv_sec) {
    err = fcntl(sock, F_SETFL, O_NONBLOCK);
    if (err == 0)
      err = connect(sock, (struct sockaddr *)&sin, sizeof(sin));
    if ((err == -1) && (errno == EINPROGRESS)) {
      fd_set writefds;
      FD_ZERO(&writefds);
      FD_SET(sock, &writefds);
      sigset_t sigmask, origmask;
      sigemptyset(&sigmask);
      pthread_sigmask(SIG_SETMASK, &sigmask, &origmask);
      err = select(sock + 1, NULL, &writefds, NULL, &connectTimer);
      pthread_sigmask(SIG_SETMASK, &origmask, NULL);
      if (err < 1) {
        if (err == 0)
          fprintf(stderr, "Error in connect: timeout\n");
        else
          perror("Error in connect");
        goto error;
      }
      socklen_t len = sizeof(err);
      getsockopt(sock, SOL_SOCKET, SO_ERROR, (char *)&err, &len);
    }
    if (err != -1)
      fcntl(sock, F_SETFL, 0);
  } else
    err = connect(sock, (struct sockaddr *)&sin, sizeof(sin));
#endif // !_WIN32
  if (err == -1) {
    PERROR("Error in connect to service");
  error:;
    shutdown(sock, SHUT_RDWR);
    close(sock);
    return C_ERROR;
  }
  SetSocketOptions(sock, 0);
  SetConnectionInfoC(c, PROT, sock, NULL, 0);
  return C_OK;
}

////////////////////////////////////////////////////////////////////////////////
//  FLUSH  /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static int io_flush(Connection *c) {
#if !defined(__sparc__)
  SOCKET sock = getSocket(c);
  if (sock != INVALID_SOCKET) {
    struct timeval timout = {0, 1};
    int err;
    FIONREAD_TYPE nbytes;
    int tries = 0;
    char buffer[1000];
    fd_set readfds, writefds;
    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);
    FD_ZERO(&writefds);
    FD_SET(sock, &writefds);
    while (((((err = select(sock + 1, &readfds, &writefds, 0, &timout)) > 0) &&
             FD_ISSET(sock, &readfds)) ||
            (err == -1 && errno == EINTR)) &&
           tries < 10) {
      tries++;
      if (FD_ISSET(sock, &readfds)) {
        err = ioctl(sock, FIONREAD, &nbytes);
        if (nbytes > 0 && err != -1) {
          nbytes = recv(sock, buffer,
                        sizeof(buffer) > (size_t)nbytes
                            ? nbytes
                            : (FIONREAD_TYPE)sizeof(buffer),
                        MSG_NOSIGNAL);
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
  return C_OK;
}

////////////////////////////////////////////////////////////////////////////////
//  LISTEN  ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static short getPort(char *name) {
  short port;
  struct servent *sp;
  port = htons((short)strtol(name, NULL, 0));
  if (port == 0) {
    sp = getservbyname(name, "tcp");
    if (!sp) {
      if (errno) {
        fprintf(stderr, "Error: unknown service port %s/%s; %s\n", name, PROT,
                strerror(errno));
        exit(0);
      }
      fprintf(stderr, "Error: unknown service port %s/%s; default to 8000\n",
              name, PROT);
      return 8000;
    }
    port = sp->s_port;
  }
  return port;
}

static int io_listen(int argc, char **argv) {
  Options options[] = {{"p", "port", 1, 0, 0},
#ifdef _WIN32
                       {"S", "sockethandle", 1, 0, 0},
#endif
                       {0, 0, 0, 0, 0}};
#ifndef _WIN32
  struct sigaction act;
  act.sa_handler = ChildSignalHandler;
  sigemptyset(&act.sa_mask);
#ifdef SA_RESTART // attempt to resume interrupted io
  act.sa_flags = SA_RESTART;
#else
  act.sa_flags = 0;
#endif
  sigaction(SIGCHLD, &act, NULL);
#endif
  ParseCommand(argc, argv, options, 0, 0, 0);
  if (options[0].present && options[0].value)
    SetPortname(options[0].value);
  else if (GetPortname() == 0)
    SetPortname("mdsip");
  INITIALIZESOCKETS;
  if (!GetMulti())
    return runServerMode(&options[1]);
  //////////////////////////////////////////////////////////////////////////
  // MULTIPLE CONNECTION MODE              /////////////////////////////////
  // multiple connections with own context /////////////////////////////////
  char *matchString[] = {"multi"};
  if (CheckClient(0, 1, matchString) == ACCESS_DENIED)
#ifdef _WIN32
    // cannot change user on Windows
    fprintf(stderr, "WARNING: 'multi' user found hostfile but Windows cannot "
                    "change user.\n");
#else
    exit(EX_NOPERM);
#endif
  // SOCKET //
  /* Create the socket and set it up to accept connections. */
  SOCKET ssock = socket(AF_T, SOCK_STREAM, 0);
  if (ssock == INVALID_SOCKET) {
    PERROR("Error getting Connection Socket");
    exit(EXIT_FAILURE);
  }
  FD_ZERO(&fdactive);
  FD_SET(ssock, &fdactive);
  // OPTIONS //
  SetSocketOptions(ssock, 1);
  // BIND //
  unsigned short port = getPort(GetPortname());
  struct SOCKADDR_IN sin;
  memset(&sin, 0, sizeof(sin));
  sin.SIN_PORT = port;
  sin.SIN_FAMILY = AF_T;
  sin.SIN_ADDR = _INADDR_ANY;
  if (bind(ssock, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
    PERROR("Error binding to service (tcp_listen)");
    exit(EXIT_FAILURE);
  }
  // LISTEN //
  if (listen(ssock, 128) < 0) {
    PERROR("Error from listen");
    exit(EXIT_FAILURE);
  }
  // LISTEN LOOP ///////////////////////////////////////////////////////////
  struct timeval readto, timeout = {1, 0};
  int error_count = 0;
  fd_set readfds;
  for (;;) {
    readfds = fdactive;
    readto = timeout;
    int num = select(FD_SETSIZE, &readfds, NULL, NULL, &readto);
    if (num == 0)
      continue; // timeout
    if (num > 0) {
      // read ready from socket list //
      error_count = 0;
      if (FD_ISSET(ssock, &readfds)) {
        socklen_t len = sizeof(sin);
        int id = -1;
        char *username;
        // ACCEPT new connection and register new socket //
        SOCKET sock = accept(ssock, (struct sockaddr *)&sin, &len);
        if (sock == INVALID_SOCKET)
          PERROR("Error accepting socket");
        else
          SetSocketOptions(sock, 0);
        if
          IS_OK(AcceptConnection(PROT, PROT, sock, 0, 0, &id, &username)) {
            // add client to client list //
            Client *client = memset(malloc(sizeof(Client)), 0, sizeof(Client));
            client->id = id;
            client->sock = sock;
            client->next = ClientList;
            client->username = username;
            client->addr = ((struct sockaddr_in *)&sin)->sin_addr.s_addr;
            client->iphost = getHostInfo(sock, &client->host);
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
            for (c_chk = ClientList; c_chk && c_chk != c; c_chk = c_chk->next)
              ;
            if (c_chk)
              FD_CLR(c->sock, &readfds);
            c = ClientList;
          } else
            c = c->next;
        }
      }
    } else if (errno == EINTR) {
      continue; // exit(EINTR);// signal interrupt; can be triggered by python
                // os.system()
    } else {    // Select returned -1 error code
      error_count++;
      PERROR("error in main select");
      fprintf(stderr, "Error count=%d\n", error_count);
      fflush(stderr);
      if (error_count > 100) {
        fprintf(stderr, "Error count exceeded, shutting down\n");
        exit(EXIT_FAILURE);
      } else {
        Client *c;
        FD_ZERO(&fdactive);
        if (ssock != INVALID_SOCKET)
          FD_SET(ssock, &fdactive);
        for (c = ClientList; c; c = c->next) {
          struct SOCKADDR_IN sin;
          socklen_t n = sizeof(sin);
          LockAsts();
          if (getpeername(c->sock, (struct sockaddr *)&sin, &n)) {
            fprintf(stderr, "Removed disconnected client\n");
            fflush(stderr);
            CloseConnection(c->id);
          } else
            FD_SET(c->sock, &fdactive);
          UnlockAsts();
        }
      }
    }
  } // end LISTEN LOOP //
  return C_ERROR;
}
