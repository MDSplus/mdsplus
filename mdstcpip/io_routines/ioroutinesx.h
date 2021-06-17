#include <sys/types.h>
#include <unistd.h>

#include <socket_port.h>
#include <pthread_port.h>
#include <_mdsshr.h>

//#define DEBUG
#include <mdsmsg.h>

static ssize_t io_send(Connection *c, const void *buffer, size_t buflen,
                       int nowait);
static int io_disconnect(Connection *c);
static int io_listen(int argc, char **argv);
static int io_authorize(Connection *c, char *username);
static int io_connect(Connection *c, char *protocol, char *host);
static ssize_t io_recv_to(Connection *c, void *buffer, size_t len, int to_msec);
#ifdef _TCP
static int io_check(Connection *c);
#else
#define io_check NULL
#endif
static ssize_t io_recv(Connection *c, void *buffer, size_t len)
{
  return io_recv_to(c, buffer, len, -1);
}
static IoRoutines io_routines = {
    io_connect, io_send, io_recv, io_flush, io_listen,
    io_authorize, io_reuseCheck, io_disconnect, io_recv_to, io_check};
#include <mdsshr.h>
#include <signal.h>
#include <inttypes.h>
#include <mdsmsg.h>

#define ACCESS_NOMATCH 0
#define ACCESS_GRANTED 1
#define ACCESS_DENIED 2

// Connected client definition for client list

typedef struct _client
{
  struct _client *next;
  Connection *connection;
  pthread_t *thread;
  char *username;
  char *host;
  char *iphost;
  SOCKET sock;
  uint32_t addr;
} Client;

#define CLIENT_PRI "%s"
#define CLIENT_VAR(c) (c)->iphost

// List of clients connected to server instance.
static pthread_mutex_t ClientListLock = PTHREAD_MUTEX_INITIALIZER;
static Client *ClientList = NULL;

////////////////////////////////////////////////////////////////////////////////
//  SOCKET LIST  ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// List of active sockets
typedef struct _socket
{
  SOCKET socket;
  struct _socket *next;
} Socket;

// List of connected Sockets
static Socket *SocketList = NULL;

EXPORT IoRoutines *Io() { return &io_routines; }

static SOCKET getSocket(Connection *c)
{
  size_t len;
  char *info_name;
  SOCKET readfd;
  ConnectionGetInfo(c, &info_name, &readfd, &len);
  return (info_name && strcmp(info_name, PROT) == 0) ? readfd : INVALID_SOCKET;
}

static pthread_mutex_t socket_list_mutex = PTHREAD_MUTEX_INITIALIZER;
#define LOCK_SOCKET_LIST MUTEX_LOCK_PUSH(&socket_list_mutex)
#define UNLOCK_SOCKET_LIST MUTEX_LOCK_POP(&socket_list_mutex)

#ifdef _TCP
static void socket_list_cleanup()
{
  LOCK_SOCKET_LIST;
  Socket *s;
  for (s = SocketList; s; s = s->next)
    shutdown(s->socket, SHUT_RDWR);
  UNLOCK_SOCKET_LIST;
}

// https://www.gnu.org/software/libc/manual/html_node/Termination-in-Handler.html
static volatile sig_atomic_t fatal_error_in_progress = 0;
#ifndef _WIN32
static struct sigaction act;
#endif
static void signal_handler(int sig)
{
  if (!fatal_error_in_progress)
  {
    fatal_error_in_progress = 1;
    socket_list_cleanup();
  }
  signal(sig, SIG_DFL);
  raise(sig);
}
static void set_signal_handler(int sig)
{
#ifdef _WIN32
  void *old = signal(sig, signal_handler);
  if (old != SIG_DFL)
    signal(sig, old);
#else
  struct sigaction old;
  if (!sigaction(sig, &act, &old) && old.sa_handler != SIG_DFL)
    sigaction(sig, &old, NULL);
#endif
}
#endif

static void PushSocket(SOCKET socket)
{
  LOCK_SOCKET_LIST;
#ifdef _TCP
  static int initialized = FALSE;
  if (!initialized)
  {
#ifndef _WIN32
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = signal_handler;
#endif
    atexit(socket_list_cleanup);
    initialized = TRUE;
  }
  set_signal_handler(SIGABRT);
  set_signal_handler(SIGTERM);
  set_signal_handler(SIGINT);
#endif
  Socket *new = malloc(sizeof(Socket));
  new->socket = socket;
  new->next = SocketList;
  SocketList = new;
  UNLOCK_SOCKET_LIST;
}

static void PopSocket(SOCKET socket)
{
  LOCK_SOCKET_LIST;
  Socket *p, *s;
  for (s = SocketList, p = 0; s && s->socket != socket; p = s, s = s->next)
    ;
  if (s)
  {
    if (p)
      p->next = s->next;
    else
      SocketList = s->next;
    free(s);
  }
  UNLOCK_SOCKET_LIST;
}

static int GetHostAndPort(char *hostin, struct sockaddr *sin)
{
  int status;
  char *port = strchr(hostin, PORTDELIM);
  sin->sa_family = AF_T;
  if (port)
  {
    int hostlen = port - hostin;
    char *host = memcpy(malloc(hostlen + 1), hostin, hostlen);
    FREE_ON_EXIT(host);
    host[hostlen] = 0;
    status = _LibGetHostAddr(host, port + 1, sin)
                 ? MDSplusERROR
                 : MDSplusSUCCESS;
    FREE_NOW(host);
  }
  else
  {
    status = _LibGetHostAddr(hostin, NULL, sin)
                 ? MDSplusERROR
                 : MDSplusSUCCESS;
    ((struct SOCKADDR_IN *)sin)->SIN_PORT = htons(8000);
  }
  return status;
}

static char *getHostInfo(SOCKET sock, char **hostnameptr)
{
  struct SOCKADDR_IN sin;
  int err = 0;
  SOCKLEN_T len = sizeof(sin);
  if (!GETPEERNAME(sock, (struct sockaddr *)&sin, &len))
  {
    GET_IPHOST(sin);
    if (hostnameptr)
    {
      char hbuf[NI_MAXHOST];
      if (getnameinfo((struct sockaddr *)&sin, len, hbuf, sizeof(hbuf), NULL, 0,
                      NI_NAMEREQD) == 0)
      {
        *hostnameptr = strdup(hbuf);
      }
      else
      {
        struct addrinfo *entry, *info = NULL;
        struct addrinfo hints = {AI_CANONNAME, AF_T, SOCK_STREAM, 0, 0,
                                 NULL, NULL, NULL};
        if (!(err = getaddrinfo(iphost, NULL, &hints, &info)))
        {
          for (entry = info; entry && !entry->ai_canonname;
               entry = entry->ai_next)
            ;
          if (entry)
            *hostnameptr = strdup(entry->ai_canonname);
          if (info)
            freeaddrinfo(info);
        }
      }
    }
    if (err)
    {
      fprintf(stderr, "Error resolving ip '%s': error=%s\n", iphost,
              gai_strerror(err));
      return NULL;
    }
    return strdup(iphost);
  }
  else
  {
    fprintf(stderr, "Error resolving ip of socket %d, ", (int)sock);
    perror("error");
  }
  return NULL;
}

#ifdef _WIN32
VOID CALLBACK ShutdownEvent(PVOID arg __attribute__((unused)),
                            BOOLEAN fired __attribute__((unused)))
{
  fprintf(stderr, "Service shut down\n");
  exit(0);
}

static inline SOCKET get_single_server_socket(char *name)
{
  HANDLE shutdownEvent, waitHandle;
  HANDLE h;
  int ppid;
  SOCKET psock;
  char shutdownEventName[120];
  if (name == 0 || sscanf(name, "%d:%d", &ppid, (int *)&psock) != 2)
  {
    fprintf(stderr, "Mdsip single connection server can only be started from "
                    "windows service\n");
    exit(EXIT_FAILURE);
  }
  char *logdir = GetLogDir();
  FREE_ON_EXIT(logdir);
  char *portnam = GetPortname();
  char *logfile = malloc(strlen(logdir) + strlen(portnam) + 50);
  FREE_ON_EXIT(logfile);
  sprintf(logfile, "%s\\MDSIP_%s_%d.log", logdir, portnam, _getpid());
  freopen(logfile, "a", stdout);
  freopen(logfile, "a", stderr);
  FREE_NOW(logfile);
  FREE_NOW(logdir);
  if (!DuplicateHandle(OpenProcess(PROCESS_ALL_ACCESS, TRUE, ppid),
                       (HANDLE)psock, GetCurrentProcess(), (HANDLE *)&h,
                       PROCESS_ALL_ACCESS, TRUE,
                       DUPLICATE_CLOSE_SOURCE | DUPLICATE_SAME_ACCESS))
  {
    fprintf(stderr, "Attempting to duplicate socket from pid %d socket %d\n",
            ppid, (int)psock);
    perror("Error duplicating socket from parent");
    exit(EXIT_FAILURE);
  }
  sprintf(shutdownEventName, "MDSIP_%s_SHUTDOWN", GetPortname());
  shutdownEvent = CreateEvent(NULL, FALSE, FALSE, (LPCTSTR)shutdownEventName);
  if (!RegisterWaitForSingleObject(&waitHandle, shutdownEvent, ShutdownEvent,
                                   NULL, INFINITE, 0))
    perror("Error registering for shutdown event");
  return *(int *)&h;
}
#else
static inline SOCKET get_single_server_socket(char *name)
{
  (void)name;
  return 0;
}
static void ChildSignalHandler(int num __attribute__((unused)))
{
  sigset_t set, oldset;
  pid_t pid;
  int err;
  /* block other incoming SIGCHLD signals */
  sigemptyset(&set);
  sigaddset(&set, SIGCHLD);
  sigprocmask(SIG_BLOCK, &set, &oldset);
  /* wait for child */
  while ((pid = waitpid((pid_t)-1, &err, WNOHANG)) > 0)
  {
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

static int io_authorize(Connection *c, char *username)
{
  int ans;
  char *iphost = NULL, *hoststr = NULL;
  FREE_ON_EXIT(iphost);
  FREE_ON_EXIT(hoststr);
  ans = ACCESS_NOMATCH;
  SOCKET sock = getSocket(c);
  char now[32];
  Now32(now);
  if ((iphost = getHostInfo(sock, &hoststr)))
  {
    fprintf(stdout, "%s (%d) (pid %d) Connection received from %s@%s [%s]\r\n",
            now, (int)sock, getpid(), username, hoststr, iphost);
    char *matchString[2] = {NULL, NULL};
    FREE_ON_EXIT(matchString[0]);
    FREE_ON_EXIT(matchString[1]);
    int num = 1;
    matchString[0] =
        strcpy(malloc(strlen(username) + strlen(iphost) + 3), username);
    strcat(matchString[0], "@");
    strcat(matchString[0], iphost);
    if (hoststr)
    {
      matchString[1] =
          strcpy(malloc(strlen(username) + strlen(hoststr) + 3), username);
      strcat(matchString[1], "@");
      strcat(matchString[1], hoststr);
      num = 2;
    }
    ans = CheckClient(username, num, matchString);
    FREE_NOW(matchString[1]);
    FREE_NOW(matchString[0]);
  }
  else
    print_socket_error("error getting hostinfo");
  FREE_NOW(iphost);
  FREE_NOW(hoststr);
  fflush(stdout);
  fflush(stderr);
  return ans;
}

////////////////////////////////////////////////////////////////////////////////
//  SEND  //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static ssize_t io_send(Connection *c, const void *bptr, size_t num, int nowait)
{
  SOCKET sock = getSocket(c);
  if (sock == INVALID_SOCKET)
    return -1;
  int sent;
  int options = nowait ? MSG_DONTWAIT | MSG_NOSIGNAL : MSG_NOSIGNAL;
  MSG_NOSIGNAL_ALT_PUSH();
  sent = SEND(sock, bptr, num, options);
  MSG_NOSIGNAL_ALT_POP();
  return sent;
}

////////////////////////////////////////////////////////////////////////////////
//  RECEIVE  ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static ssize_t io_recv_to(Connection *c, void *bptr, size_t num, int to_msec)
{
  SOCKET sock = getSocket(c);
  ssize_t recved = -1;
  if (sock != INVALID_SOCKET)
  {
    PushSocket(sock);
    MSG_NOSIGNAL_ALT_PUSH();
#ifdef _TCP
    struct timeval to, timeout;
    if (to_msec < 0)
    {
      timeout.tv_sec = 10;
      timeout.tv_usec = 0;
    }
    else
    {
      timeout.tv_sec = to_msec / 1000;
      timeout.tv_usec = (to_msec % 1000) * 1000;
    }
    fd_set rf, readfds;
    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);
    do
    {
      to = timeout;
      rf = readfds;
      recved = select(sock + 1, &rf, NULL, NULL, &to);
#else
    struct pollfd fd;
    fd.fd = sock;
    fd.events = POLLIN;
    int to_val = to_msec < 0 ? 10 : to_msec;
    do
    {
      recved = poll(&fd, 1, to_val);
#endif
      if (recved > 0)
      { // for select this will be 1
        recved = RECV(sock, bptr, num, MSG_NOSIGNAL);
        break;
      }
      if (recved < 0)
      {
        if (errno == EAGAIN)
          continue;
        break; // Error
      }
    } while (to_msec < 0); // else timeout
    MSG_NOSIGNAL_ALT_POP();
    PopSocket(sock);
  }
  return recved;
}
static inline void Client_cancel(Client *c)
{
#ifdef _WIN32
  shutdown(c->sock, SHUT_RDWR);
  closesocket(c->sock);
#else
  pthread_cancel(*c->thread);
#endif
}

////////////////////////////////////////////////////////////////////////////////
//  DISCONNECT  ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static void destroyClient(Client *c)
{
  MDSDBG("destroyClient");
  if (c->thread)
  {
    if (!pthread_equal(*c->thread, pthread_self()))
    {
      Client_cancel(c);
      pthread_join(*c->thread, NULL);
    }
    else
    {
      pthread_detach(*c->thread);
    }
    free(c->thread);
  }
  else
  {
    Connection *con = c->connection;
    if (con)
    {
      con->io = NULL;
      io_disconnect(con);
    }
    destroyConnection(con);
  }
  free(c->username);
  free(c->iphost);
  free(c->host);
  free(c);
}

static inline void destroyClientList()
{
  Client *cl;
  pthread_mutex_lock(&ClientListLock);
  cl = ClientList;
  for (; ClientList; ClientList = ClientList->next)
  {
    if (ClientList->thread)
    {
      Client_cancel(ClientList);
    }
  }
  pthread_mutex_unlock(&ClientListLock);
  while (cl)
  {
    Client *const c = cl;
    cl = cl->next;
    destroyClient(c);
  }
}

inline static Client *pop_client(Connection *con)
{
  Client *c, *p;
  pthread_mutex_lock(&ClientListLock);
  for (p = NULL, c = ClientList;
       c;
       p = c, c = c->next)
  {
    if (c->connection == con)
    {
      if (p)
        p->next = c->next;
      else
        ClientList = c->next;
      break;
    }
  }
  pthread_mutex_unlock(&ClientListLock);
  return c;
}

static int io_disconnect(Connection *con)
{
  int err = C_OK;
  SOCKET sock = getSocket(con);
  Client *c = pop_client(con);
  if (c)
  {
    char now[32];
    Now32(now);
    fprintf(stdout, "%s (%d) (pid %d) Connection disconnected from %s@%s [%s]\r\n",
            now, (int)sock, getpid(), c->username, c->host, c->iphost);
    c->connection = NULL;
    destroyClient(c);
  }
  if (sock != INVALID_SOCKET)
  {
#ifdef _TCP
    err = shutdown(sock, SHUT_RDWR);
#endif
    err = closesocket(sock);
  }
  fflush(stdout);
  return err;
}

static int run_server_mode(Options *options)
{
  /// SERVER MODE  ///////////////
  /// Handle single connection ///
  SOCKET sock = get_single_server_socket(options->value);
  int id;
  if (IS_NOT_OK(AcceptConnection(PROT, PROT, sock, 0, 0, &id, NULL)))
    return C_ERROR;
  struct SOCKADDR_IN sin;
  SOCKLEN_T len = sizeof(sin);
  if (GETPEERNAME(sock, (struct sockaddr *)&sin, &len) == 0)
    MdsSetClientAddr(((struct sockaddr_in *)&sin)->sin_addr.s_addr);
  Connection *connection = PopConnection(id);
  pthread_cleanup_push((void *)destroyConnection, (void *)connection);
  int status;
  do
    status = ConnectionDoMessage(connection);
  while (STATUS_OK);
  pthread_cleanup_pop(1);
  return C_ERROR;
}

static void *client_thread(void *args)
{
  Client *client = (Client *)args;
  Connection *connection = client->connection;
  MdsSetClientAddr(client->addr);
  pthread_cleanup_push((void *)destroyConnection, (void *)connection);
  int status;
  do
  {
    status = ConnectionDoMessage(connection);
  } while (STATUS_OK);
  pthread_cleanup_pop(1);
  return NULL;
}

static inline int dispatch_client(Client *client)
{
  client->thread = (pthread_t *)malloc(sizeof(pthread_t));
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, 0x40000);
  const int err = pthread_create(client->thread, &attr, client_thread, (void *)client);
  pthread_attr_destroy(&attr);
  if (err)
  {
    errno = err;
    perror("dispatch_client");
    free(client->thread);
    client->thread = NULL;
    client->connection->id = INVALID_CONNECTION_ID;
    destroyClient(client);
  }
  else
  {
    fprintf(stderr, "dispatched client " CLIENT_PRI "\n", CLIENT_VAR(client));
    pthread_mutex_lock(&ClientListLock);
    client->next = ClientList;
    ClientList = client;
    pthread_mutex_unlock(&ClientListLock);
  }
  return err;
}
