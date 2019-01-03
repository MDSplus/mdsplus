static ssize_t io_send(Connection* c, const void *buffer, size_t buflen, int nowait);
static int io_disconnect(Connection* c);
static int io_listen(int argc, char **argv);
static int io_authorize(Connection* c, char *username);
static int io_connect(Connection* c, char *protocol, char *host);
static ssize_t io_recv_to(Connection* c, void *buffer, size_t len, int to_msec);
#ifdef _TCP
static int io_check(Connection* c);
#else
# define io_check NULL
#endif
static ssize_t io_recv(Connection* c, void *buffer, size_t len){
  return io_recv_to(c, buffer,len, -1);
}
static IoRoutines io_routines = {
  io_connect, io_send, io_recv, io_flush, io_listen, io_authorize, io_reuseCheck, io_disconnect, io_recv_to, io_check
};
#include <mdsshr.h>
#include <inttypes.h>
#ifdef _TCP
 #ifdef _WIN32
  #define close closesocket
 #endif
#else
 #include <poll.h>
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

static SOCKET getSocket(Connection* c){
  size_t len;
  char *info_name;
  SOCKET readfd;
  GetConnectionInfoC(c, &info_name, &readfd, &len);
  return (info_name && strcmp(info_name, PROT) == 0) ? readfd : INVALID_SOCKET;
}

static pthread_mutex_t socket_list_mutex = PTHREAD_MUTEX_INITIALIZER;
static void unlock_socket_list() {  pthread_mutex_unlock(&socket_list_mutex); }
#define LOCK_SOCKET_LIST   pthread_mutex_lock(&socket_list_mutex);pthread_cleanup_push(unlock_socket_list, NULL);
#define UNLOCK_SOCKET_LIST pthread_cleanup_pop(1);

static void PushSocket(SOCKET socket){
  LOCK_SOCKET_LIST;
  Socket *new = malloc(sizeof(Socket));
  new->socket = socket;
  new->next = SocketList;
  SocketList = new;
  UNLOCK_SOCKET_LIST;
}

static void PopSocket(SOCKET socket){
  LOCK_SOCKET_LIST;
  Socket *p, *s;
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

static int GetHostAndPort(char *hostin, struct SOCKADDR_IN *sin){
  int status;
  INITIALIZESOCKETS;
  char *host = strcpy((char *)malloc(strlen(hostin) + 1), hostin);
  FREE_ON_EXIT(host);
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
  struct addrinfo *info = NULL;
  static const struct addrinfo hints = { 0, AF_T, SOCK_STREAM, 0, 0, 0, 0, 0 };
  int err = getaddrinfo(host, service, &hints, &info);
  if (err) {
    status = MDSplusERROR;
    fprintf(stderr,"Error connecting to host: %s, port %s error=%s\n", host, service, gai_strerror(err));
  } else {
    memcpy(sin, info->ai_addr, sizeof(*sin) < info->ai_addrlen ? sizeof(*sin) : info->ai_addrlen);
    status = MDSplusSUCCESS;
  }
  if (info) freeaddrinfo(info);
  FREE_NOW(host);
  return status;
}

#if defined(__MACH__) || defined(_WIN32)
# define GETHOSTBYADDR
#endif
static char *getHostInfo(SOCKET sock, char **iphostptr, char **hostnameptr){
  char *ans = NULL;
  struct SOCKADDR_IN sin;
  SOCKLEN_T len = sizeof(sin);
  if (!GETPEERNAME(sock, (struct sockaddr *)&sin, &len)) {
    GET_IPHOST(sin);
#ifdef GETHOSTBYADDR
    struct hostent* hp = gethostbyaddr((void*)&sin.SIN_ADDR,sizeof(sin.SIN_ADDR),sin.SIN_FAMILY);
#else
    struct hostent hostbuf, *hp = NULL;
    int herr;
    INIT_AND_FREE_ON_EXIT(char*,hp_mem);
    size_t memlen = 1024;
    hp_mem = malloc(memlen);
    while (hp_mem && (gethostbyaddr_r((void*)&sin.SIN_ADDR,sizeof(sin.SIN_ADDR),sin.SIN_FAMILY,&hostbuf,hp_mem,memlen,&hp,&herr) == ERANGE)) {
      memlen *=2;
# ifdef DEBUG
      fprintf(stderr,"hp_mem too small-> %d\n",(int)memlen);
# endif
      hp_mem = realloc(hp_mem,memlen);
    }
#endif
    if (hp && hp->h_name) {
      ans = (char *)malloc(strlen(iphost) + strlen(hp->h_name) + 4);
      sprintf(ans, "%s [%s]", hp->h_name, iphost);
      if (hostnameptr) {
        *hostnameptr = (char *)malloc(strlen(hp->h_name) + 1);
        strcpy(*hostnameptr, hp->h_name);
      }
    } else {
      if (hostnameptr)
        *hostnameptr = NULL;
      ans = (char *)malloc(strlen(iphost) + 1);
      strcpy(ans, iphost);
    }
    if (iphostptr) {
      *iphostptr = (char *)malloc(strlen(iphost) + 1);
      strcpy(*iphostptr, iphost);
    }
#ifndef GETHOSTBYADDR
    FREE_NOW(hp_mem);
#endif
  }
  return ans;
}

#ifdef _WIN32
VOID CALLBACK ShutdownEvent(PVOID arg __attribute__ ((unused)), BOOLEAN fired __attribute__ ((unused))){
  fprintf(stderr, "Service shut down\n");
  exit(0);
}

static int getSocketHandle(char *name){
  HANDLE shutdownEvent, waitHandle;
  HANDLE h;
  int ppid;
  SOCKET psock;
  char shutdownEventName[120];
  char *logdir = GetLogDir();
  FREE_ON_EXIT(logdir);
  char *portnam = GetPortname();
  char *logfile = malloc(strlen(logdir)+strlen(portnam)+50);
  FREE_ON_EXIT(logfile);
  if (name == 0 || sscanf(name, "%d:%d", &ppid, (int*)&psock) != 2) {
    fprintf(stderr, "Mdsip single connection server can only be started from windows service\n");
    free(logfile);
    free(logdir);
    exit(EXIT_FAILURE);
  }
  sprintf(logfile, "%s\\MDSIP_%s_%d.log", logdir, portnam, _getpid());
  freopen(logfile, "a", stdout);
  freopen(logfile, "a", stderr);
  FREE_NOW(logfile);
  FREE_NOW(logdir);
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

static int io_authorize(Connection* c, char *username){
  int ans;
  char *iphost = NULL,*hoststr = NULL ,*info = NULL;
  FREE_ON_EXIT(iphost);
  FREE_ON_EXIT(hoststr);
  FREE_ON_EXIT(info);
  ans = C_OK;
  SOCKET sock = getSocket(c);
  char now[32];Now32(now);
  info = getHostInfo(sock, &iphost, &hoststr);
  if (info) {
    printf("%s (%d) (pid %d) Connection received from %s@%s\r\n",
           now, (int)sock, getpid(), username, info);
    char *matchString[2] = { NULL, NULL };
    FREE_ON_EXIT(matchString[0]);
    FREE_ON_EXIT(matchString[1]);
    int num = 1;
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
    FREE_NOW(matchString[1]);
    FREE_NOW(matchString[0]);
  } else
    PERROR("error getting hostinfo");
  FREE_NOW(info);
  FREE_NOW(iphost);
  FREE_NOW(hoststr);
  fflush(stdout);
  fflush(stderr);
  return ans;
}

////////////////////////////////////////////////////////////////////////////////
//  SEND  //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static ssize_t io_send(Connection* c, const void *bptr, size_t num, int nowait){
  SOCKET sock = getSocket(c);
  int options = nowait ? MSG_DONTWAIT : 0;
  if (sock != INVALID_SOCKET)
    return SEND(sock, bptr, num, options | MSG_NOSIGNAL);
  return -1; //sent
}


////////////////////////////////////////////////////////////////////////////////
//  RECEIVE  ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static ssize_t io_recv_to(Connection* c, void *bptr, size_t num, int to_msec){
  SOCKET sock = getSocket(c);
  ssize_t recved = -1;
  if (sock != INVALID_SOCKET) {
    PushSocket(sock);
    signal(SIGABRT, ABORT);
    if (to_msec<0)
      recved = RECV(sock, bptr, num, MSG_NOSIGNAL);
    else {
#ifdef _TCP
      struct timeval timeout;
      timeout.tv_sec  = to_msec/1000;
      timeout.tv_usec = (to_msec%1000)*1000;
      fd_set readfds;
      FD_ZERO(&readfds);
      FD_SET(sock, &readfds);
      recved = select(sock+1, &readfds, NULL, NULL, &timeout);
#else
      struct pollfd fd;
      fd.fd = sock;
      fd.events = POLLIN;
      recved = poll(&fd, 1, to_msec);
#endif
      switch (recved) {
      case -1: break; // Error
      case  0: break; // Timeout
      default: // for select this will be 1
        recved = RECV(sock, bptr, num, MSG_NOSIGNAL);
        break;
      }
    }
    PopSocket(sock);
  }
  return recved;
}

#ifdef _TCP
static int io_check(Connection* c){
  SOCKET sock = getSocket(c);
  ssize_t err = -1;
  if (sock != INVALID_SOCKET) {
    PushSocket(sock);
    signal(SIGABRT, ABORT);
    struct timeval timeout = {0,0};
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);
    err = select(sock+1, &readfds, NULL, NULL, &timeout);
    switch (err) {
    case -1: break; // Error
    case  0: break; // Timeout
    default: {// for select this will be 1
        char bptr[1];
        err = RECV(sock, bptr, 1, MSG_NOSIGNAL||MSG_PEEK);
        err = (err==1) ? 0 : -1;
        break;
      }
    }
    PopSocket(sock);
  }
  return (int)err;
}
#endif

////////////////////////////////////////////////////////////////////////////////
//  DISCONNECT  ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static int io_disconnect(Connection* con){
  SOCKET sock = getSocket(con);
  int err = C_OK;
  char now[32];Now32(now);
  if (sock != INVALID_SOCKET) {
    Client *c, **p;
    for (p = &ClientList, c = ClientList; c && c->id != con->id; p = &c->next, c = c->next) ;
    if (c) {
      *p = c->next;
#ifdef _TCP
      if (FD_ISSET(sock, &fdactive)) {
        FD_CLR(sock, &fdactive);
#else
      if (server_epoll != -1) {
	udt_epoll_remove_usock(server_epoll, sock);
#endif
        printf("%s (%d) (pid %d) Connection disconnected from %s@%s [%s]\r\n",
               now, (int)sock, getpid(),
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
int runServerMode(Options *options){
#else
int runServerMode(Options *options  __attribute__((unused))){
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
  if IS_NOT_OK(AcceptConnection(PROT, PROT, sock, 0, 0, &id, &username))
    return C_ERROR;
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
  while (DoMessage(id));
  return C_ERROR;
}
