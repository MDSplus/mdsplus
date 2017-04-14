#ifdef _WIN32
 #define close closesocket
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
  return (info_name && strcmp(info_name, PROT) == 0) ? (SOCKET)readfd : (SOCKET)-1;
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

static int getHostAndPort(char *hostin, struct SOCKADDR_IN *sin){
  INIT_STATUS_ERROR;
  INITIALIZESOCKETS;
  char *host = strcpy((char *)malloc(strlen(hostin) + 1), hostin);
  char *service = NULL;
  size_t i;
  for (i = 0 ; i < strlen(host) && host[i] != PORTDELIM ; i++);
  if (i < strlen(host)) {
    host[i] = '\0';
    service = &host[i+1];
  } else {
    service = "mdsip";
  }
  if (atoi(service) == 0) {
    if (!getservbyname(service, "tcp")) {
      char *env_service = getenv(service);
      if ( (env_service == NULL) ) {
	if (strcmp(service,"mdsip")==0) {
	  service = "8000";
	}
      } else {
	service = env_service;
      }
    }
  }
  struct addrinfo *info;
  static const struct addrinfo hints = { 0, AF_T, SOCK_STREAM, 0, 0, 0, 0, 0 };
  int n = getaddrinfo(host, service, &hints, &info);
  if (n)
    fprintf(stderr,"Error connecting to host: %s, port %s error=%s\n", host, service, gai_strerror(n));
  else {
    memcpy(sin, info->ai_addr, sizeof(*sin) < info->ai_addrlen ? sizeof(*sin) : info->ai_addrlen);
    freeaddrinfo(info);
    status = MDSplusSUCCESS;
  }
  free(host);
  return status;
}

static char *getHostInfo(SOCKET sock, char **iphostptr, char **hostnameptr){
  char *ans = NULL;
  struct SOCKADDR_IN sin;
  SOCKLEN_T len = sizeof(sin);
  if (!GETPEERNAME(sock, (struct sockaddr *)&sin, &len)) {
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

////////////////////////////////////////////////////////////////////////////////
//  AUTHORIZE  /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

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
    return SEND(sock, bptr, num, options | MSG_NOSIGNAL);
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
    SOCKLEN_T len = sizeof(sin);
    if (GETPEERNAME(sock, (struct sockaddr *)&sin, &len))
      PERROR("Error getting peer name from socket");
    else
      recved = RECV(sock, bptr, num, MSG_NOSIGNAL);
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
#ifdef _TCP
      if (FD_ISSET(sock, &fdactive)) {
        FD_CLR(sock, &fdactive);
#else
      if (server_epoll != -1) {
	udt_epoll_remove_usock(server_epoll, sock);
#endif
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
#ifdef _TCP
    err = shutdown(sock, 2);
#endif
    err = close(sock);
  }
  fflush(stdout);
  fflush(stderr);
  return err;
}


#ifdef _WIN32
void runServerMode(Options *options){
#else
void runServerMode(Options *options  __attribute__((unused))){
#endif
  //////////////////////////////////////////////////////////////////////////
  // SERVER MODE                                ////////////////////////////
  // multiple connections with the same context ////////////////////////////
#ifdef _WIN32
    SOCKET sock = getSocketHandle(options->value);
#else
    SOCKET sock = 0;
#endif
    int id;
    char *username;
    if IS_OK(AcceptConnection(PROT, PROT, sock, 0, 0, &id, &username)) {
      struct SOCKADDR_IN sin;
      SOCKLEN_T len = sizeof(sin);
      if (GETPEERNAME(sock, (struct sockaddr *)&sin, &len) == 0)
        MdsSetClientAddr(((struct sockaddr_in*)&sin)->sin_addr.s_addr);
      Client *client = memset(malloc(sizeof(Client)), 0, sizeof(Client));
      client->id = id;
      client->sock = sock;
      client->next = ClientList;
      client->username = username;
      client->host = getHostInfo(sock, &client->iphost, NULL);
      ClientList = client;
#ifdef _TCP
      FD_SET(sock, &fdactive);
#endif
      while IS_OK(DoMessage(id));
    }
}
