#define _TCP
#ifdef _WIN32
 #define PERROR(string) do{errno = WSAGetLastError();perror(string);}while (0)
#else
 #define PERROR(string) perror(string)
 #include <netinet/tcp.h>
#endif
#define SOCKLEN_T   socklen_t
#define GETPEERNAME getpeername
#define SEND        send
#define RECV        recv
// active select file descriptor
static fd_set fdactive;
static int io_flush(Connection* c);
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

static int io_connect(Connection* c, char *protocol __attribute__ ((unused)), char *host){
  struct SOCKADDR_IN sin;
  SOCKET sock;
  if IS_OK(GetHostAndPort(host, &sin)) {
    INITIALIZESOCKETS;
    sock = socket(AF_T, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
      PERROR("Error creating socket");
      return C_ERROR;
    }
    struct timeval connectTimer = { 0, 0 };
    connectTimer.tv_sec = GetMdsConnectTimeout();
    int err;
    if (connectTimer.tv_sec) {
#ifndef _WIN32
      err = fcntl(sock, F_SETFL, O_NONBLOCK);
#endif
      err = connect(sock, (struct sockaddr *)&sin, sizeof(sin));
      if ((err == -1) && (errno == EINPROGRESS)) {
        fd_set readfds;
        fd_set exceptfds;
        fd_set writefds;
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);
        FD_ZERO(&exceptfds);
        FD_SET(sock, &exceptfds);
        FD_ZERO(&writefds);
        FD_SET(sock, &writefds);
        err = select(FD_SETSIZE, &readfds, &writefds, &exceptfds, &connectTimer);
        if (err == 0) {
          PERROR("Error in connect");
          shutdown(sock, 2);
          close(sock);
          fflush(stderr);
          return C_ERROR;
        }
      }
#ifndef _WIN32
      if (err != -1)
        fcntl(sock, F_SETFL, 0);
#endif
    } else {
      err = connect(sock, (struct sockaddr *)&sin, sizeof(sin));
    }
    if (err == -1) {
      shutdown(sock, 2);
      sock = INVALID_SOCKET;
      PERROR("Error in connect to service");
      return C_ERROR;
    }
    if (sock == INVALID_SOCKET) {
      fprintf(stderr,"Error in connect to service\n");
      fflush(stderr);
      return C_ERROR;
    }
    SetSocketOptions(sock, 0);
    SetConnectionInfoC(c, PROT, sock, NULL, 0);
    return C_OK;
  } else {
    fprintf(stderr, "Connect failed to host: %s\n",host);
    fflush(stderr);
    return C_ERROR;
  }
}

////////////////////////////////////////////////////////////////////////////////
//  FLUSH  /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static int io_flush(Connection* c){
#if !defined(__sparc__)
  SOCKET sock = getSocket(c);
  if (sock != INVALID_SOCKET) {
    struct timeval timout = { 0, 1 };
    int err;
    FIONREAD_TYPE nbytes;
    int tries = 0;
    char buffer[1000];
    fd_set readfds, writefds;
    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);
    FD_ZERO(&writefds);
    FD_SET(sock, &writefds);
    while (((((err = select(FD_SETSIZE, &readfds, &writefds, 0, &timout)) > 0)
             && FD_ISSET(sock, &readfds)) || (err == -1 && errno == EINTR)) && tries < 10) {
      tries++;
      if (FD_ISSET(sock, &readfds)) {
        err = ioctl(sock, FIONREAD, &nbytes);
        if (nbytes > 0 && err != -1) {
          nbytes = recv(sock, buffer,
                        sizeof(buffer) > (size_t)nbytes ? nbytes : (FIONREAD_TYPE)sizeof(buffer),
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

static short getPort(char *name){
  short port;
  struct servent *sp;
  port = htons((short)atoi(name));
  if (port == 0) {
    sp = getservbyname(name, "tcp");
    if (!sp) {
      fprintf(stderr, "Error unknown service: %s/%s: %s/n", name, PROT, strerror(errno));
      exit(0);
    }
    port = sp->s_port;
  }
  return port;
}

static int io_listen(int argc, char **argv){
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
  INITIALIZESOCKETS;
  if (GetMulti()) {
    //////////////////////////////////////////////////////////////////////////
    // MULTIPLE CONNECTION MODE              /////////////////////////////////
    // multiple connections with own context /////////////////////////////////
    char *matchString[] = { "multi" };
    CheckClient(0, 1, matchString);
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
#if AF_T == AF_INET
    sin.sin_addr.s_addr = INADDR_ANY;
#endif
    if (bind(ssock, (struct sockaddr *)&sin, sizeof(sin))<0) {
      PERROR("Error binding to service (tcp_listen)");
      exit(EXIT_FAILURE);
    }
    // LISTEN //
    if (listen(ssock, 128)<0) {
      PERROR("Error from listen");
      exit(EXIT_FAILURE);
    }
    // LISTEN LOOP ///////////////////////////////////////////////////////////
    int tablesize = FD_SETSIZE;
    int error_count = 0;
    fd_set readfds;
    for(;;) {
      readfds = fdactive;
      // SELECT select read ready from socket list //
      if (select(tablesize, &readfds, 0, 0, 0) != -1) {
        error_count = 0;
        if (FD_ISSET(ssock, &readfds)) {
          socklen_t len = sizeof(sin);
          int id = -1;
          char *username;
          // ACCEPT new connection and register new socket //
          SOCKET sock = accept(ssock, (struct sockaddr *)&sin, &len);
          if (sock == INVALID_SOCKET) PERROR("Error accepting socket");
          else SetSocketOptions(sock, 0);
          if IS_OK(AcceptConnection(PROT, PROT, sock, 0, 0, &id, &username)) {
            // add client to client list //
            Client *client = memset(malloc(sizeof(Client)), 0, sizeof(Client));
            client->id = id;
            client->sock = sock;
            client->next = ClientList;
            client->username = username;
            client->addr = ((struct sockaddr_in*)&sin)->sin_addr.s_addr;
            client->host = getHostInfo(sock, &client->iphost, NULL);
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
      } else if (errno==EINTR){
        exit(EINTR);// signal interrupt
      } else {// Select returned -1 error code
        error_count++;
        perror("error in main select");
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
            } else {
              FD_SET(c->sock, &fdactive);
            }
            UnlockAsts();
          }
        }
      }
    }// end LISTEN LOOP //
  } else
    runServerMode(&options[1]);
  return C_ERROR;
}
