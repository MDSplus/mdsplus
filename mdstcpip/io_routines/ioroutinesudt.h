#ifdef _WIN32
#define close closesocket
#define PERROR(...)               \
  do                              \
  {                               \
    errno = WSAGetLastError();    \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, ": ");        \
    perror("");                   \
  } while (0)
#undef INVALID_SOCKET
#else
#define PERROR(...)                                     \
  do                                                    \
  {                                                     \
    fprintf(stderr, __VA_ARGS__);                       \
    fprintf(stderr, ": %s\n", udt_getlasterror_desc()); \
  } while (0)
#endif
#undef SOCKET
#define SOCKET UDTSOCKET
#define INVALID_SOCKET -1
#include "udtc.h"
#define SOCKLEN_T int
#define GETPEERNAME udt_getpeername
#define SEND udt_send
#define RECV udt_recv
int server_epoll = -1;
#define io_flush NULL
#include "ioroutinesx.h"
////////////////////////////////////////////////////////////////////////////////
//  CONNECT  ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static int io_connect(Connection *c, char *protocol __attribute__((unused)),
                      char *host)
{
  struct SOCKADDR_IN sin;
  UDTSOCKET sock;
  if (IS_OK(GetHostAndPort(host, &sin)))
  {
    sock = udt_socket(PF_T, SOCK_STREAM, 0);
    if (!sock)
    {
      perror("Error in (udt) connect");
      return C_ERROR;
    }
    if (udt_connect(sock, (struct sockaddr *)&sin, sizeof(sin)))
    {
      PERROR("Error in connect to service");
      return C_ERROR;
    }
    ConnectionSetInfo(c, PROT, sock, NULL, 0);
    return C_OK;
  }
  else
  {
    fprintf(stderr, "Connect failed to host: %s\n", host);
    fflush(stderr);
    return C_ERROR;
  }
}

////////////////////////////////////////////////////////////////////////////////
//  LISTEN  ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static int io_listen(int argc, char **argv)
{
  Options options[] = {{"p", "port", 1, 0, 0},
#ifdef _WIN32
                       {"S", "sockethandle", 1, 0, 0},
#endif
                       {0, 0, 0, 0, 0}};
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
  if (GetMulti())
  {
    //////////////////////////////////////////////////////////////////////////
    // MULTIPLE CONNECTION MODE              /////////////////////////////////
    // multiple connections with own context /////////////////////////////////
    struct addrinfo *result, *rp;
    UDTSOCKET ssock = INVALID_SOCKET;
    struct SOCKADDR_IN sin;
    UDTSOCKET readfds[1024];
    int events = UDT_UDT_EPOLL_IN | UDT_UDT_EPOLL_ERR;
    int gai_stat;
    static const struct addrinfo hints = {AI_PASSIVE, AF_T, SOCK_STREAM, 0,
                                          0, 0, 0, 0};
    gai_stat = getaddrinfo(NULL, GetPortname(), &hints, &result);
    if (gai_stat)
    {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_stat));
      exit(EXIT_FAILURE);
    }
    char *matchString[] = {"multi"};
    if (CheckClient(0, 1, matchString) == ACCESS_DENIED)
#ifdef _WIN32
      // cannot change user on Windows
      fprintf(stderr, "WARNING: 'multi' user found hostfile but Windows cannot "
                      "change user.\n");
#else
      exit(EX_NOPERM);
#endif
    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
      ssock = udt_socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
      if (ssock == INVALID_SOCKET)
        continue;
      if (udt_bind(ssock, rp->ai_addr, rp->ai_addrlen) == 0)
        break;
      close(ssock);
    }
    udt_epoll_add_usock(server_epoll, ssock, &events);
    memset(&sin, 0, sizeof(sin));
    if (udt_listen(ssock, 128) < 0)
    {
      fprintf(stderr, "Error from udt_listen: %s\n", udt_getlasterror_desc());
      exit(EXIT_FAILURE);
    }
    for (;;)
    {
      int readfds_num = 1024;
      if (udt_epoll_wait2(server_epoll, readfds, &readfds_num, NULL, NULL, 5000,
                          NULL, NULL, NULL, NULL))
      {
        Client *c;
        LockAsts();
        int locked = 0;
        pthread_cleanup_push((void*)pthread_mutex_unlock, (void*)&ClientListLock);
        locked = !pthread_mutex_lock(&ClientListLock);
        for (c = ClientList; c;)
        {
          int c_epoll = udt_epoll_create();
          UDTSOCKET readfds[1];
          UDTSOCKET writefds[1];
          int readnum = 1;
          int writenum = 1;
          udt_epoll_add_usock(c_epoll, c->sock, NULL);
          int err = udt_epoll_wait2(c_epoll, readfds, &readnum, writefds,
                                    &writenum, 0, NULL, NULL, NULL, NULL);
          udt_epoll_release(c_epoll);
          if (err)
          {
            locked = pthread_mutex_unlock(&ClientListLock);
            destroyConnection(c->connection);
            locked = !pthread_mutex_lock(&ClientListLock);
            c = ClientList;
          }
          else
            c = c->next;
        }
        pthread_cleanup_pop(locked);
        UnlockAsts();
      }
      else
      {
        int i;
        for (i = 0; readfds_num != 1024 && i < readfds_num; i++)
        {
          if (readfds[i] == ssock)
          {
            // int events = UDT_UDT_EPOLL_IN | UDT_UDT_EPOLL_ERR;
            int len = sizeof(sin);
            int id = -1;
            int status;
            char *username;
            UDTSOCKET sock = udt_accept(ssock, (struct sockaddr *)&sin, &len);
            status =
                AcceptConnection(PROT, PROT, sock, NULL, 0, &id, &username);
            if (STATUS_OK)
            {
              Client *client = calloc(1, sizeof(Client));
              client->connection = PopConnection(id);
              client->sock = sock;
              client->username = username;
              client->addr = ((struct sockaddr_in *)&sin)->sin_addr.s_addr;
              client->iphost = getHostInfo(sock, &client->host);
              pthread_mutex_lock(&ClientListLock);
              client->next = ClientList;
              ClientList = client;
              pthread_mutex_unlock(&ClientListLock);
              udt_epoll_add_usock(server_epoll, sock, &events);
            }
          }
          else
          {
            Client *c;
            pthread_mutex_lock(&ClientListLock);
            for (c = ClientList; c; c = c->next)
            {
              if (c->sock == readfds[i])
                break;
            }
            pthread_mutex_unlock(&ClientListLock);
            if (c)
            {
              int c_epoll = udt_epoll_create();
              UDTSOCKET readfds[1];
              UDTSOCKET writefds[1];
              int readnum = 1;
              int writenum = 1;
              udt_epoll_add_usock(c_epoll, c->sock, NULL);
              int err = udt_epoll_wait2(c_epoll, readfds, &readnum, writefds,
                                        &writenum, 0, NULL, NULL, NULL, NULL);
              udt_epoll_release(c_epoll);
              if (err)
              {
                destroyConnection(c->connection);
                break;
              }
              MdsSetClientAddr(c->addr);
              DoMessageC(c->connection);
            }
          }
        }
      }
    }
  }
  else
    run_server_mode(&options[1]);
  return C_ERROR;
}
