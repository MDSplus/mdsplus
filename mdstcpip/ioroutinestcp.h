#ifdef _WIN32
 #define close closesocket
 #define PERROR(string) do{errno = WSAGetLastError();perror(string);}while (0)
#else
 #define PERROR(string) perror(string)
 #include <netinet/tcp.h>
#endif

// Connected client definition for client list
typedef struct _client {
  SOCKET sock;
  int id;
  char *username;
  unsigned long addr;
  char *host;
  char *iphost;
  struct _client *next;
} Client;

// List of clients connected to server instance.
static Client *ClientList = NULL;

// active select file descriptor
static fd_set fdactive;

////////////////////////////////////////////////////////////////////////////////
//  SOCKET LIST  ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// List of active sockets
typedef struct _socket {
  SOCKET socket;
  struct _socket *next;
} Socket;

// List of connected Sockets
static Socket *SocketList = NULL;

EXPORT IoRoutines *Io(){
  return &io_routines;
}

static SOCKET getSocket(int conid){
  size_t len;
  char *info_name;
  int readfd;
  GetConnectionInfo(conid, &info_name, &readfd, &len);
  return (info_name && strcmp(info_name, PROT) == 0) ? readfd : -1;
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

static int getHostAndPort(char *hostin, struct SOCKADDR_IN *sin){
  INIT_STATUS_ERROR;
  INITIALIZESOCKETS;
  char *host = strcpy((char *)malloc(strlen(hostin) + 1), hostin);
  char *service = "mdsip";
  size_t i;
  for (i = 0 ; i < strlen(host) && host[i] != PORTDELIM ; i++);
  if (i < strlen(host)) {
    host[i] = '\0';
    service = &host[i+1];
  }
  struct addrinfo *info;
  static const struct addrinfo hints = { 0, AF_T, SOCK_STREAM, 0, 0, 0, 0, 0 };
  int n = getaddrinfo(host, service, &hints, &info);
  if (n < 0)
    fprintf(stderr,"Error connecting to host: %s, port %s error=%s\n", host, service, gai_strerror(n));
  else {
    memcpy(sin, info->ai_addr, info->ai_addrlen);
    freeaddrinfo(info);
    status = MDSplusSUCCESS;
  }
  free(host);
  return status;
}

static int io_connect(int conid, char *protocol __attribute__ ((unused)), char *host){
  struct SOCKADDR_IN sin;
  SOCKET sock;
  if IS_OK(getHostAndPort(host, &sin)) {
    INITIALIZESOCKETS;
    sock = socket(AF_T, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
      PERROR("Error creating socket");
      return -1;
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
          return -1;
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
      return -1;
    }
    if (sock == INVALID_SOCKET) {
      fprintf(stderr,"Error in connect to service\n");
      fflush(stderr);
      return -1;
    }
    SetSocketOptions(sock, 0);
    SetConnectionInfo(conid, PROT, sock, NULL, 0);
    return 0;
  } else {
    fprintf(stderr, "Connect failed to host: %s\n",host);
    fflush(stderr);
    return -1;
  }
}

////////////////////////////////////////////////////////////////////////////////
//  AUTHORIZE  /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static char *getHostInfo(SOCKET sock, char **iphostptr, char **hostnameptr){
  char *ans = NULL;
  struct SOCKADDR_IN sin;
  socklen_t len = sizeof(sin);
  if (!getpeername(sock, (struct sockaddr *)&sin, &len)) {
    GET_IPHOST(sin);
    GETHOSTBYADDR(sin.SIN_ADDR,sin.SIN_FAMILY);
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
      } else
        *hostnameptr = 0;
    }
    FREE_HP;
  }
  return ans;
}

static int io_authorize(int conid, char *username){
  SOCKET sock = getSocket(conid);
  time_t tim = time(0);
  char *timestr = ctime(&tim);
  int ans = 0;
  char *hoststr = 0;
  char *iphost = 0;
  char *info = getHostInfo(sock, &iphost, &hoststr);
  if (info) {
    char *matchString[2] = { 0, 0 };
    int num = 1;
    timestr[strlen(timestr) - 1] = 0;
    printf("%s (%d) (pid %d) Connection received from %s@%s\r\n",
           timestr, (int)sock, getpid(), username, info);
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

////////////////////////////////////////////////////////////////////////////////
//  SEND  //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static ssize_t io_send(int conid, const void *bptr, size_t num, int nowait){
  SOCKET sock = getSocket(conid);
  int options = nowait ? MSG_DONTWAIT : 0;
  if (sock != INVALID_SOCKET)
    return send(sock, bptr, num, options | MSG_NOSIGNAL);
  return -1; //sent
}

////////////////////////////////////////////////////////////////////////////////
//  RECEIVE  ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static ssize_t io_recv(int conid, void *bptr, size_t num){
  SOCKET sock = getSocket(conid);
  ssize_t recved = -1;
  if (sock != INVALID_SOCKET) {
    PushSocket(sock);
    signal(SIGABRT, ABORT);
    struct SOCKADDR_IN sin;
    socklen_t len = sizeof(sin);
    if (getpeername(sock, (struct sockaddr *)&sin, &len))
      PERROR("Error getting peer name from socket");
    else
      recved = recv(sock, bptr, num, MSG_NOSIGNAL);
    PopSocket(sock);
  }
  return recved;
}

////////////////////////////////////////////////////////////////////////////////
//  DISCONNECT  ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static int io_disconnect(int conid){
  SOCKET sock = getSocket(conid);
  int err = 0;
  time_t tim = time(0);
  char *timestr = ctime(&tim);
  if (sock != INVALID_SOCKET) {
    Client *c, **p;
    for (p = &ClientList, c = ClientList; c && c->id != conid; p = &c->next, c = c->next) ;
    if (c) {
      *p = c->next;
      if (FD_ISSET(sock, &fdactive)) {
        FD_CLR(sock, &fdactive);
        timestr[strlen(timestr) - 1] = 0;
        printf("%s (%d) (pid %d) Connection disconnected from %s@%s [%s]\r\n",
               timestr, (int)sock, getpid(),
               c->username ? c->username : "?",
               c->host     ? c->host     : "?",
               c->iphost   ? c->iphost   : "?");
      }
      if (c->username) free(c->username);
      if (c->iphost) free(c->iphost);
      if (c->host) free(c->host);
      free(c);
    }
    err = shutdown(sock, 2);
    err = close(sock);
  }
  fflush(stdout);
  fflush(stderr);
  return err;
}

////////////////////////////////////////////////////////////////////////////////
//  FLUSH  /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static int io_flush(int conid){
#if !defined(__sparc__)
  SOCKET sock = getSocket(conid);
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
  return 0;
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

#ifdef _WIN32
VOID CALLBACK ShutdownEvent(PVOID arg __attribute__ ((unused)), BOOLEAN fired __attribute__ ((unused))){
  fprintf(stderr, "Service shut down\n");
  exit(0);
}

static int getSocketHandle(char *name){
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
    exit(EXIT_FAILURE);
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
    exit(EXIT_FAILURE);
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
  int err;
  /* block other incoming SIGCHLD signals */
  sigemptyset(&set);
  sigaddset(&set, SIGCHLD);
  sigprocmask(SIG_BLOCK, &set, &oldset);
  /* wait for child */
  while ((pid = waitpid((pid_t) - 1, &err, WNOHANG)) > 0) {
    /* re-install the signal handler (some systems need this) */
    signal(SIGCHLD, ChildSignalHandler);
    /* and unblock it */
    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);
    sigprocmask(SIG_UNBLOCK, &set, &oldset);
  }
}
#endif

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
  } else {
  //////////////////////////////////////////////////////////////////////////
  // SERVER MODE                                ////////////////////////////
  // multiple connections with the same context ////////////////////////////
#ifdef _WIN32
    SOCKET sock = getSocketHandle(options[1].value);
#else
    SOCKET sock = 0;
#endif
    int id;
    char *username;
    if IS_OK(AcceptConnection(PROT, PROT, sock, 0, 0, &id, &username)) {
      struct SOCKADDR_IN sin;
      socklen_t n = sizeof(sin);
      if (getpeername(sock, (struct sockaddr *)&sin, &n) == 0)
        MdsSetClientAddr(((struct sockaddr_in*)&sin)->sin_addr.s_addr);
      Client *client = memset(malloc(sizeof(Client)), 0, sizeof(Client));
      client->id = id;
      client->sock = sock;
      client->next = ClientList;
      client->username = username;
      client->host = getHostInfo(sock, &client->iphost, NULL);
      ClientList = client;
      FD_SET(sock, &fdactive);
      while IS_OK(DoMessage(id));
    }
  }
  return 1;
}
