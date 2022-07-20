#define _TCP
#define SOCKLEN_T socklen_t
#define GETPEERNAME getpeername
#define SEND send
#define RECV recv
// active select file descriptor
static int io_flush(Connection *c);
#include "ioroutinesx.h"

// CONNECT //

///
/// set socket options on the socket s for TCP protocol. This sets the receive
/// buffer, the send buffer, the SO_OOBINLINE, the SO_KEEPALIVE and TCP_NODELAY
/// .. at the moment
///
/// \param s socket to set options to
/// \param reuse set SO_REUSEADDR to be able to reuse the same address.
///
static void set_socket_options(SOCKET s, int reuse)
{
  int sendbuf = SEND_BUF_SIZE, recvbuf = RECV_BUF_SIZE;
  int one = 1;
#ifndef _WIN32
  fcntl(s, F_SETFD, FD_CLOEXEC);
#endif
  const char *tcp_window_size = getenv("TCP_WINDOW_SIZE");
  if (tcp_window_size && strlen(tcp_window_size))
  {
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

static int io_check(Connection *c)
{
  SOCKET sock = getSocket(c);
  ssize_t err = -1;
  if (sock != INVALID_SOCKET)
  {
    PushSocket(sock);
    MSG_NOSIGNAL_ALT_PUSH();
    struct timeval timeout = {0, 0};
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);
    err = select(sock + 1, &readfds, NULL, NULL, &timeout);
    switch (err)
    {
    case -1:
      break; // Error
    case 0:
      break; // Timeout
    default:
    { // for select this will be 1
      char bptr[1];
      err = RECV(sock, bptr, 1, MSG_NOSIGNAL | MSG_PEEK);
      err = (err == 1) ? 0 : -1;
      break;
    }
    }
    MSG_NOSIGNAL_ALT_POP();
    PopSocket(sock);
  }
  return (int)err;
}

/* io_connect()
 * interruptable connect implementation with optional timeout
 * _WIN32 requires hack to break out of select():
 *   create a self connected DGRAM socket and close it on SIGINT
 */
#ifdef WIN32
pthread_mutex_t int_mutex = PTHREAD_MUTEX_INITIALIZER;
static SOCKET int_socket = INVALID_SOCKET;
static uint32_t int_parallel = 0;
static void *int_handler;
static void int_select(int signo)
{
  signal(signo, int_handler);
  pthread_mutex_lock(&int_mutex);
  if (int_socket != INVALID_SOCKET)
  {
    closesocket(int_socket);
    int_socket = INVALID_SOCKET;
  }
  pthread_mutex_unlock(&int_mutex);
  raise(signo);
}
/// ends with int_socket set up and handler installed
static int int_setup()
{
  int sock;
  pthread_mutex_lock(&int_mutex);
  if (int_socket == INVALID_SOCKET)
  {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_LOOPBACK;
    int len = sizeof(addr);
    int_socket = socket(AF_INET, SOCK_STREAM, 0);
    bind(int_socket, (struct sockaddr *)&addr, len);
    getsockname(int_socket, (struct sockaddr *)&addr, &len);
    connect(int_socket, (struct sockaddr *)&addr, len);
    int_handler = signal(SIGINT, int_select);
  }
  else if (int_parallel == 0)
  {
    int_handler = signal(SIGINT, int_select);
  }
  int_parallel++;
  sock = int_socket;
  pthread_mutex_unlock(&int_mutex);
  return sock;
}
static void int_cleanup(void *null)
{
  (void)null;
  pthread_mutex_lock(&int_mutex);
  int_parallel--;
  if (int_parallel == 0)
  {
    signal(SIGINT, int_handler);
    int_handler = NULL;
  }
  pthread_mutex_unlock(&int_mutex);
}

int interruptable_select(int nfds, fd_set *restrict readfds,
                         fd_set *restrict writefds, fd_set *restrict exceptfds,
                         struct timeval *restrict timeout)
{
  int err, lerrno;
  static fd_set rd;
  int sock = int_setup();
  pthread_cleanup_push(int_cleanup, NULL);
  if (!readfds)
  {
    FD_ZERO(&rd);
    readfds = &rd;
  }
  FD_SET(sock, readfds);
  if (sock >= nfds)
    nfds = sock + 1;
  err = select(nfds, readfds, writefds, exceptfds, timeout);
  lerrno = errno;
  if (FD_ISSET(sock, readfds))
  {
    FD_CLR(sock, readfds);
    if (err > 0)
    {
      lerrno = EINTR;
      err = -1;
    }
  }
  pthread_cleanup_pop(1);
  errno = lerrno;
  return err;
}
#define IS_EINPROGRESS (WSAGetLastError() == WSAEWOULDBLOCK)
#define socket_set_nonblocking(sock) ({u_long ul = TRUE;  ioctlsocket(sock, FIONBIO, &ul); })
#define socket_set_blocking(sock) ({u_long ul = FALSE; ioctlsocket(sock, FIONBIO, &ul); })
#else
#define IS_EINPROGRESS (errno == EINPROGRESS)
#define interruptable_select select
#define socket_set_nonblocking(sock) fcntl(sock, F_SETFL, O_NONBLOCK)
#define socket_set_blocking(sock) fcntl(sock, F_SETFL, 0)
#endif

static inline long get_timeout_sec()
{
  const char *timeout = getenv("MDSIP_CONNECT_TIMEOUT");
  if (timeout)
    return strtol(timeout, NULL, 0);
  return 10;
}

static int io_connect(Connection *c, char *protocol __attribute__((unused)),
                      char *host)
{
  struct sockaddr sin;
  SOCKET sock;
  if (IS_NOT_OK(GetHostAndPort(host, &sin)))
  {
    fprintf(stderr, "Connect failed to host: %s\n", host);
    return C_ERROR;
  }
  INITIALIZESOCKETS;
  sock = socket(AF_T, SOCK_STREAM, 0);
  if (sock == INVALID_SOCKET)
  {
    print_socket_error("Error creating socket");
    return C_ERROR;
  }
  struct timeval connectTimer = {.tv_sec = get_timeout_sec(), .tv_usec = 0};
  socket_set_nonblocking(sock);
  int err = connect(sock, (struct sockaddr *)&sin, sizeof(sin));
  if (err == -1)
  {
    if (IS_EINPROGRESS)
    {
      fd_set writefds;
      FD_ZERO(&writefds);
      FD_SET(sock, &writefds);
      err = select(sock + 1, NULL, &writefds, NULL, &connectTimer);
      if (err < 1)
      {
        if (err == 0)
          fprintf(stderr, "Error in connect: timeout\n");
        else
          perror("Error in connect");
      }
      else
      {
        socklen_t len = sizeof(err);
        getsockopt(sock, SOL_SOCKET, SO_ERROR, (char *)&err, &len);
        if (err)
          print_socket_error("Error in connect to service");
        else
          socket_set_blocking(sock);
      }
    }
  }
  if (err == -1)
  {
    shutdown(sock, SHUT_RDWR);
    closesocket(sock);
    return C_ERROR;
  }
  set_socket_options(sock, 0);
  ConnectionSetInfo(c, PROT, sock, NULL, 0);
  return C_OK;
}

// FLUSH //

static int io_flush(Connection *c)
{
#ifndef __sparc__
  SOCKET sock = getSocket(c);
  if (sock != INVALID_SOCKET)
  {
    struct timeval timout = {0, 1};
    int err;
    FIONREAD_TYPE nbytes;
    int tries = 0;
    char buffer[1000];
    fd_set readfds, writefds;
    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);
    // why write? is this how it detects a disconnection?
    FD_ZERO(&writefds);
    FD_SET(sock, &writefds);
    MSG_NOSIGNAL_ALT_PUSH();
    while (((((err = select(sock + 1, &readfds, NULL, NULL, &timout)) > 0) &&
             FD_ISSET(sock, &readfds)) ||
            (err == -1 && errno == EINTR)) &&
           tries < 10)
    {
      tries++;
      if (FD_ISSET(sock, &readfds))
      {
        err = ioctlsocket(sock, FIONREAD, &nbytes);
        if (nbytes > 0 && err != -1)
        {
          nbytes = recv(sock, buffer,
                        sizeof(buffer) > (size_t)nbytes
                            ? nbytes
                            : (FIONREAD_TYPE)sizeof(buffer),
                        MSG_NOSIGNAL);
          if (nbytes > 0)
            tries = 0;
        }
      }
      else
        FD_SET(sock, &readfds);
      timout.tv_usec = 100000;
      FD_CLR(sock, &writefds);
    }
    MSG_NOSIGNAL_ALT_POP();
  }
#endif
  return C_OK;
}

// LISTEN //

static uint16_t get_nport(char *name)
{
  int port = strtol(name, NULL, 0);
  if (port && port <= 0xFFFF)
    return htons((uint16_t)port);
  struct servent *sp = getservbyname(name, "tcp");
  if (!sp)
  {
    if (errno)
    {
      fprintf(stderr, "Error: unknown service port %s/%s; %s\n", name, PROT,
              strerror(errno));
      return 0;
    }
    fprintf(stderr, "Error: unknown service port %s/%s; default to 8000\n",
            name, PROT);
    return htons(8000);
  }
  return sp->s_port;
}

static inline void listen_loop(SOCKET ssock, int *go)
{
  pthread_cleanup_push(destroyClientList, NULL);
  struct timeval readto, timeout = {1, 0};
  fd_set readfds;
  FD_ZERO(&readfds);
  while (go)
  {
    readto = timeout;
    FD_SET(ssock, &readfds);
    int num = select(FD_SETSIZE, &readfds, NULL, NULL, &readto);
    MDSDBG("io_listen: select = %d.", num);
    if (num == 0)
    {
      continue; // timeout
    }
    else if (num > 0)
    { // read ready from socket list
      if (FD_ISSET(ssock, &readfds))
      {
        struct sockaddr sin;
        socklen_t len = sizeof(sin);
        int id = -1;
        char *username;
        // ACCEPT new connection and register new socket
        SOCKET sock = accept(ssock, (struct sockaddr *)&sin, &len);
        if (sock == INVALID_SOCKET)
          print_socket_error("Error accepting socket");
        else
          set_socket_options(sock, 0);
        if (IS_OK(AcceptConnection(PROT, PROT, sock, 0, 0, &id, &username)))
        {
          // add client to client list //
          Client *client = calloc(1, sizeof(Client));
          client->connection = PopConnection(id);
          client->sock = sock;
          client->username = username;
          client->iphost = getHostInfo(sock, &client->host);
          if (sin.sa_family == AF_INET)
          {
            client->addr = ((struct sockaddr_in *)&sin)->sin_addr.s_addr;
          }
          dispatch_client(client);
        }
      }
    }
    else if (errno == EINTR)
    {
      continue;
    }
    else
    {
      print_socket_error("Error in server select, shutting down");
      break;
    }
  } // end LISTEN LOOP //
  pthread_cleanup_pop(1);
}

static int io_listen(int argc, char **argv)
{
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
  uint16_t nport = get_nport(GetPortname());
  if (nport == 0)
    return C_ERROR;
  if (!GetMulti())
    return run_server_mode(&options[1]);
  /// MULTIPLE CONNECTION MODE              ///
  /// multiple connections with own context ///
  char *matchString[] = {"multi"};
  if (CheckClient(0, 1, matchString) == ACCESS_DENIED)
  {
#ifdef WIN32
    // cannot change user on Windows
    fprintf(stderr, "WARNING: 'multi' user found hostfile but Windows cannot "
                    "change user.\n");
#else
    errno = EX_NOPERM;
    return C_ERROR;
#endif
  }
  // Create the socket and set it up to accept connections.
  SOCKET ssock = socket(AF_T, SOCK_STREAM, 0);
  if (ssock == INVALID_SOCKET)
  {
    print_socket_error("Error getting Connection Socket");
    return C_ERROR;
  }
  set_socket_options(ssock, 1);
  struct SOCKADDR_IN sin;
  memset(&sin, 0, sizeof(sin));
  sin.SIN_FAMILY = AF_T;
  sin.SIN_PORT = nport;
  if (bind(ssock, (struct sockaddr *)&sin, sizeof(sin)) < 0)
  {
    print_socket_error("Error binding to service (tcp_listen)");
    return C_ERROR;
  }
  // LISTEN //
  if (listen(ssock, 128) < 0)
  {
    print_socket_error("Error from listen");
    return C_ERROR;
  }
  int run = 1;
  listen_loop(ssock, &run);
  return C_ERROR;
}
