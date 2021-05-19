#undef SOCKET
#define SOCKET UDTSOCKET
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
  struct sockaddr sin;
  UDTSOCKET sock;
  if (IS_OK(GetHostAndPort(host, &sin)))
  {
    sock = udt_socket(PF_T, SOCK_STREAM, 0);
    if (!sock)
    {
      perror("Error in (udt) connect");
      return C_ERROR;
    }
    if (udt_connect(sock, &sin, sizeof(sin)))
    {
      print_socket_error("Error in connect to service");
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
    UDTSOCKET readfds[1024];
    int events = UDT_UDT_EPOLL_IN | UDT_UDT_EPOLL_ERR;
    int gai_stat;
    static const struct addrinfo hints = {
        AI_PASSIVE, AF_T, SOCK_STREAM, 0, 0, 0, 0, 0};
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
      closesocket(ssock);
    }
    if (ssock == INVALID_SOCKET)
    {
      fprintf(stderr, "Error from udt_socket/bind: %s\n", udt_getlasterror_desc());
      exit(EXIT_FAILURE);
    }
    udt_epoll_add_usock(server_epoll, ssock, &events);
    if (udt_listen(ssock, 128) < 0)
    {
      fprintf(stderr, "Error from udt_listen: %s\n", udt_getlasterror_desc());
      exit(EXIT_FAILURE);
    }
    atexit(destroyClientList);
    for (;;)
    {
      int readfds_num = 1;
      while (udt_epoll_wait2(server_epoll, readfds, &readfds_num, NULL, NULL, 5000,
                             NULL, NULL, NULL, NULL))
        continue;
      if (readfds[0] == ssock)
      {
        struct sockaddr sin;
        int len = sizeof(sin);
        int id = -1;
        char *username = NULL;
        UDTSOCKET sock = udt_accept(ssock, (struct sockaddr *)&sin, &len);
        int status = AcceptConnection(PROT, PROT, sock, NULL, 0, &id, &username);
        if (STATUS_OK)
        {
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
        else
        {
          free(username);
        }
      }
    }
  }
  else
    run_server_mode(&options[1]);
  return C_ERROR;
}
