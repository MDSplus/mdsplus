#ifdef _WIN32
 #define close closesocket
 #define PERROR(string) do{errno = WSAGetLastError();perror(string);}while (0)
#else
 #define PERROR(string) perror(string)
 #include "udtc.h"
#endif

// Connected client definition for client list
typedef struct _client {
  UDTSOCKET sock;
  int id;
  char *username;
  unsigned long addr;
  char *host;
  char *iphost;
  struct _client *next;
} Client;
#define FREE_CLIENT(c)

// List of clients connected to server instance.
static Client *ClientList = NULL;


static int server_epoll = -1;

////////////////////////////////////////////////////////////////////////////////
//  SOCKET LIST  ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// List of active sockets
typedef struct _socket {
  UDTSOCKET socket;
  struct _socket *next;
} Socket;

// List of connected Sockets
static Socket *Sockets = NULL;

EXPORT IoRoutines *Io(){
  return &io_routines;
}

static UDTSOCKET getSocket(int conid){
  size_t len;
  char *info_name;
  int readfd;
  GetConnectionInfo(conid, &info_name, &readfd, &len);
  return (info_name && strcmp(info_name, PROT) == 0) ? (UDTSOCKET) readfd : (UDTSOCKET) - 1;
}

static pthread_mutex_t socket_mutex = PTHREAD_MUTEX_INITIALIZER;
#define SOCKET_LIST_LOCK   pthread_mutex_lock(&socket_mutex)
#define SOCKET_LIST_UNLOCK pthread_mutex_unlock(&socket_mutex)

static void PushSocket(UDTSOCKET socket){
  Socket *oldhead;
  SOCKET_LIST_LOCK;
  oldhead = Sockets;
  Sockets = malloc(sizeof(Socket));
  Sockets->socket = socket;
  Sockets->next = oldhead;
  SOCKET_LIST_UNLOCK;
}

static void PopSocket(UDTSOCKET socket){
  Socket *p, *s;
  SOCKET_LIST_LOCK;
  for (s = Sockets, p = 0; s && s->socket != socket; p = s, s = s->next) ;
  if (s != NULL) {
    if (p)
      p->next = s->next;
    else
      Sockets = s->next;
    free(s);
  }
  SOCKET_LIST_UNLOCK;
}

static void ABORT(int sigval __attribute__ ((unused))){
  Socket *s;
  SOCKET_LIST_LOCK;
  for (s = Sockets; s; s = s->next)
    udt_close(s->socket);
  SOCKET_LIST_UNLOCK;
}

////////////////////////////////////////////////////////////////////////////////
//  CONNECT  ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

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
  UDTSOCKET sock;
  if IS_OK(getHostAndPort(host, &sin)) {
    sock = udt_socket(PF_T, SOCK_STREAM, 0);
    if (!sock) {
      perror("Error in (udt) connect");
      return -1;
    }
    if (udt_connect(sock, (struct sockaddr *)&sin, sizeof(sin))) {
      fprintf(stderr, "Error in connect to service: %s\n", udt_getlasterror_desc());
      fflush(stderr);
      return -1;
    }
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

static char *getHostInfo(UDTSOCKET sock, char **iphostptr, char **hostnameptr){
  char *ans = NULL;
  struct SOCKADDR_IN sin;
  int len = sizeof(sin);
  if (!udt_getpeername(sock, (struct sockaddr *)&sin, &len)) {
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
  UDTSOCKET sock = getSocket(conid);
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
           timestr, sock, getpid(), username, info);
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
  UDTSOCKET sock = getSocket(conid);
  int options = nowait ? MSG_DONTWAIT : 0;
  if (sock != INVALID_SOCKET)
    return udt_send(sock, bptr, num, options | MSG_NOSIGNAL);
  return -1; //sent
}

////////////////////////////////////////////////////////////////////////////////
//  RECEIVE  ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static ssize_t io_recv(int conid, void *bptr, size_t num){
  UDTSOCKET sock = getSocket(conid);
  ssize_t recved = -1;
  if (sock != INVALID_SOCKET) {
    PushSocket(sock);
    struct SOCKADDR_IN sin;
    int len = sizeof(sin);
    signal(SIGABRT, ABORT);
    if (udt_getpeername(sock, (struct sockaddr *)&sin, &len))
      fprintf(stderr, "Error getting peer name from socket: %s\n", udt_getlasterror_desc());
    else
      recved = udt_recv(sock, bptr, num, MSG_NOSIGNAL);
    PopSocket(sock);
  }
  return recved;
}

////////////////////////////////////////////////////////////////////////////////
//  DISCONNECT  ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static int io_disconnect(int conid){
  UDTSOCKET sock = getSocket(conid);
  int err = 0;
  time_t tim = time(0);
  char *timestr = ctime(&tim);
  if (sock != INVALID_SOCKET) {
    Client *c, **p;
    for (p = &ClientList, c = ClientList; c && c->id != conid; p = &c->next, c = c->next) ;
    if (c) {
      *p = c->next;
      if (server_epoll != -1) {
	udt_epoll_remove_usock(server_epoll, sock);
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
    err = udt_close(sock);
  }
  fflush(stdout);
  fflush(stderr);
  return err;
}

////////////////////////////////////////////////////////////////////////////////
//  FLUSH  /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static int io_flush(int conid __attribute__ ((unused))){
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
//  LISTEN  ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
VOID CALLBACK ShutdownEvent(PVOID arg, BOOLEAN fired){
  fprintf(stderr, "Service shut down\n");
  exit(0);
}

static int getSocketHandle(char *name){
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
    fprintf(stderr, "Attempting to duplicate socket from pid %d socket %d\n", ppid, psock);
    perror("Error duplicating socket from parent");
    exit(EXIT_FAILURE);
  }
  sprintf(shutdownEventName, "MDSIP_%s_SHUTDOWN", GetPortname());
  shutdownEvent = CreateEvent(NULL, FALSE, FALSE, (LPCWSTR) shutdownEventName);
  if (!RegisterWaitForSingleObject(&waitHandle, shutdownEvent, ShutdownEvent, NULL, INFINITE, 0))
    perror("Error registering for shutdown event");
  return *(int *)&h;
}
#else
static void ChildSignalHandler(int num __attribute__ ((unused))){
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
  server_epoll = udt_epoll_create();
  if (GetMulti()) {
    //////////////////////////////////////////////////////////////////////////
    // MULTIPLE CONNECTION MODE              /////////////////////////////////
    // multiple connections with own context /////////////////////////////////
    struct addrinfo *result, *rp;
    UDTSOCKET ssock = INVALID_SOCKET;
    struct SOCKADDR_IN sin;
    UDTSOCKET readfds[1024];
    int events = UDT_UDT_EPOLL_IN | UDT_UDT_EPOLL_ERR;
    int gai_stat;
    static const struct addrinfo hints = { AI_PASSIVE, AF_T, SOCK_STREAM, 0, 0, 0, 0, 0 };
    gai_stat = getaddrinfo(NULL, GetPortname(), &hints, &result);
    if (gai_stat) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_stat));
      exit(EXIT_FAILURE);
    }
    char *matchString[] = { "multi" };
    CheckClient(0, 1, matchString);
    for (rp = result; rp != NULL; rp = rp->ai_next) {
      ssock = udt_socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
      if (ssock == INVALID_SOCKET)
        continue;
      if (udt_bind(ssock, rp->ai_addr, rp->ai_addrlen) == 0)
        break;
      close(ssock);
    }
    udt_epoll_add_usock(server_epoll, ssock, &events);
    memset(&sin, 0, sizeof(sin));
    if (udt_listen(ssock, 128) < 0) {
      fprintf(stderr, "Error from udt_listen: %s\n", udt_getlasterror_desc());
      exit(EXIT_FAILURE);
    }
    for (;;) {
      int i;
      int readfds_num = 1024;
      if (udt_epoll_wait2(server_epoll, readfds, &readfds_num, NULL, NULL, 5000, NULL, NULL, NULL, NULL)) {
        Client *c;
        LockAsts();
        for (;;) {
          for (c = ClientList; c; c = c->next) {
            int c_epoll = udt_epoll_create();
            UDTSOCKET readfds[1];
            UDTSOCKET writefds[1];
            int readnum = 1;
            int writenum = 1;
            udt_epoll_add_usock(c_epoll, c->sock, NULL);
            int err = udt_epoll_wait2(c_epoll, readfds, &readnum, writefds, &writenum, 0, NULL, NULL, NULL, NULL);
            udt_epoll_release(c_epoll);
            if (err) {
              CloseConnection(c->id);
              goto next;
              break;
            }
          }
          break;
 next:
          continue;
        }
        UnlockAsts();
      } else {
        for (i = 0; readfds_num != 1024 && i < readfds_num; i++) {
          if (readfds[i] == ssock) {
            //int events = UDT_UDT_EPOLL_IN | UDT_UDT_EPOLL_ERR;
            int len = sizeof(sin);
            int id = -1;
            int status;
            char *username;
            UDTSOCKET sock = udt_accept(ssock, (struct sockaddr *)&sin, &len);
            status = AcceptConnection(PROT, PROT, sock, NULL, 0, &id, &username);
            if STATUS_OK {
              Client *client = memset(malloc(sizeof(Client)), 0, sizeof(Client));
              client->id = id;
              client->sock = sock;
              client->next = ClientList;
              client->username = username;
              client->addr = ((struct sockaddr_in*)&sin)->sin_addr.s_addr;
              client->host = getHostInfo(sock, &client->iphost, NULL);
              ClientList = client;
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
                int err = udt_epoll_wait2(c_epoll, readfds, &readnum, writefds, &writenum, 0, NULL, NULL, NULL, NULL);
                udt_epoll_release(c_epoll);
                if (err) {
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
    if IS_OK(AcceptConnection(PROT,PROT, sock, NULL, 0, &id, &username)) {
      struct SOCKADDR_IN sin;
      int n = sizeof(sin);
      Client *client = memset(malloc(sizeof(Client)), 0, sizeof(Client));
      if (udt_getpeername(sock, (struct sockaddr *)&sin, &n) == 0)
        MdsSetClientAddr(((struct sockaddr_in*)&sin)->sin_addr.s_addr);
      client->id = id;
      client->sock = sock;
      client->next = ClientList;
      client->username = username;
      client->host = getHostInfo(sock, &client->iphost, NULL);
      ClientList = client;
      while IS_OK(DoMessage(id));
    }
  }
  return 1;
}
