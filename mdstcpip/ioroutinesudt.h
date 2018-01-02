#ifdef _WIN32
 #define close closesocket
 #define PERROR(string) do{errno = WSAGetLastError();perror(string);}while (0)
 #undef INVALID_SOCKET
#else
 #define PERROR(string) do{fprintf(stderr,"%s: %s\n",string,udt_getlasterror_desc());fflush(stderr);}while (0)
#endif
#undef SOCKET
#define SOCKET UDTSOCKET
#define INVALID_SOCKET -1
#include "udtc.h"
#define SOCKLEN_T   int
#define GETPEERNAME udt_getpeername
#define SEND udt_send
#define RECV udt_recv
int server_epoll = -1;
#define io_flush NULL
#include "ioroutinesx.h"
////////////////////////////////////////////////////////////////////////////////
//  CONNECT  ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static int io_connect(Connection* c, char *protocol __attribute__ ((unused)), char *host){
  struct SOCKADDR_IN sin;
  UDTSOCKET sock;
  if IS_OK(GetHostAndPort(host, &sin)) {
    sock = udt_socket(PF_T, SOCK_STREAM, 0);
    if (!sock) {
      perror("Error in (udt) connect");
      return C_ERROR;
    }
    if (udt_connect(sock, (struct sockaddr *)&sin, sizeof(sin))) {
      PERROR("Error in connect to service");
      return C_ERROR;
    }
    SetConnectionInfoC(c, PROT, sock, NULL, 0);
    return C_OK;
  } else {
    fprintf(stderr,"Connect failed to host: %s\n",host);
    fflush(stderr);
    return C_ERROR;
  }
}

////////////////////////////////////////////////////////////////////////////////
//  LISTEN  ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

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
  } else
    runServerMode(&options[1]);
  return C_ERROR;
}
