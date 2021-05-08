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
#ifdef _WIN32
static SOCKET int_sock = INVALID_SOCKET;
static void *old_handler;
static void int_select(int signo)
{
  signal(signo, old_handler);
  raise(signo);
  if (int_sock != INVALID_SOCKET)
    closesocket(int_sock);
}
#endif

static int io_connect(Connection *c, char *protocol __attribute__((unused)),
                      char *host)
{
  struct SOCKADDR_IN sin;
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
  if ((err == -1) && (WSAGetLastError() == WSAEWOULDBLOCK) && (connectTimer.tv_sec > 0))
  {
    fd_set rdfds, wrfds;
    FD_ZERO(&wrfds);
    FD_ZERO(&rdfds);
    FD_SET(sock, &wrfds);
    FD_SET(int_sock, &rdfds);
    old_handler = signal(SIGINT, int_select);
    err = select(maxsock, &rdfds, &wrfds, NULL, timeout);
    signal(SIGINT, old_handler);
    if (err < 1)
    {
      if (err == 0)
        fprintf(stderr, "Error in connect: timeout\n");
      else
        perror("Error in connect");
      closesocket(int_sock);
      goto error;
    }
    if (FD_ISSET(int_sock, &rdfds))
    {
      errno = EINTR;
      perror("Error in connect: Interrupted");
      goto error;
    }
    closesocket(int_sock);
    socklen_t len = sizeof(err);
    getsockopt(sock, SOL_SOCKET, SO_ERROR, (char *)&err, &len);
  }
  ul = FALSE;
  ioctlsocket(sock, FIONBIO, &ul);
#else  // _WIN32
  if (connectTimer.tv_sec)
  {
    err = fcntl(sock, F_SETFL, O_NONBLOCK);
    if (err == 0)
    {
      err = connect(sock, (struct sockaddr *)&sin, sizeof(sin));
      if (err == -1)
      {
        if (errno == EINPROGRESS)
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
            goto error;
          }
          socklen_t len = sizeof(err);
          getsockopt(sock, SOL_SOCKET, SO_ERROR, (char *)&err, &len);
        }
      }
      fcntl(sock, F_SETFL, 0);
    }
  }
  else
    err = connect(sock, (struct sockaddr *)&sin, sizeof(sin));
#endif // !_WIN32
  if (err == -1)
  {
    print_socket_error("Error in connect to service");
  error:;
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
#if !defined(__sparc__)
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

static short get_port(char *name)
{
  short port;
  struct servent *sp;
  port = htons((short)strtol(name, NULL, 0));
  if (port == 0)
  {
    sp = getservbyname(name, "tcp");
    if (!sp)
    {
      if (errno)
      {
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
  if (!GetMulti())
    return run_server_mode(&options[1]);
  /// MULTIPLE CONNECTION MODE              ///
  /// multiple connections with own context ///
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
  // Create the socket and set it up to accept connections.
  SOCKET ssock = socket(AF_T, SOCK_STREAM, 0);
  if (ssock == INVALID_SOCKET)
  {
    print_socket_error("Error getting Connection Socket");
    exit(EXIT_FAILURE);
  }
  // OPTIONS //
  set_socket_options(ssock, 1);
  // BIND //
  unsigned short port = get_port(GetPortname());
  struct SOCKADDR_IN sin;
  memset(&sin, 0, sizeof(sin));
  sin.SIN_PORT = port;
  sin.SIN_FAMILY = AF_T;
  sin.SIN_ADDR = _INADDR_ANY;
  if (bind(ssock, (struct sockaddr *)&sin, sizeof(sin)) < 0)
  {
    print_socket_error("Error binding to service (tcp_listen)");
    exit(EXIT_FAILURE);
  }
  // LISTEN //
  if (listen(ssock, 128) < 0)
  {
    print_socket_error("Error from listen");
    exit(EXIT_FAILURE);
  }
  atexit(destroyClientList);
  // LISTEN LOOP //
  struct timeval readto, timeout = {1, 0};
  fd_set readfds;
  FD_ZERO(&readfds);
  for (;;)
  {
    readto = timeout;
    FD_SET(ssock, &readfds);
    int num = select(FD_SETSIZE, &readfds, NULL, NULL, &readto);
    MDSDBG("io_listen: select = %d.", num);
    if (num == 0)
      continue; // timeout
    else if (num > 0)
    { // read ready from socket list
      if (FD_ISSET(ssock, &readfds))
      {
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
          client->addr = ((struct sockaddr_in *)&sin)->sin_addr.s_addr;
          client->sock = sock;
          client->username = username;
          client->iphost = getHostInfo(sock, &client->host);
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
      exit(EXIT_FAILURE);
    }
  } // end LISTEN LOOP //
  return C_ERROR;
}
