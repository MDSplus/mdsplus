#define TCPVER "tcp"
#define STRUCT_SOCKADDR struct sockaddr_in
#define NEW_SOCKET socket(AF_INET, SOCK_STREAM, 0)
#include "ioroutinestcp.h"

static int getHostAndPort(char *hostin, STRUCT_SOCKADDR *sin){
  size_t i;
  static char *mdsip = "mdsip";
  char *host = strcpy((char *)malloc(strlen(hostin) + 1), hostin);
  char *service;
  unsigned short portnum;
  INITIALIZESOCKETS;
  for (i = 0; i < strlen(host) && host[i] != ':'; i++) ;
  if (i < strlen(host)) {
    host[i] = '\0';
    service = &host[i + 1];
  } else {
    service = mdsip;
  }
  int addr;
  GETHOSTBYNAMEORADDR(host,addr);
  if (!hp) {
    free(host);
    FREE_HP;
    return 0;
  }
  if (atoi(service) == 0) {
    struct servent *sp;
    sp = getservbyname(service, "tcp");
    if (sp != NULL)
      portnum = sp->s_port;
    else {
      char *port = getenv(service);
      port = (port == NULL) ? "8000" : port;
      portnum = htons(atoi(port));
    }
  } else
    portnum = htons(atoi(service));
  if (portnum == 0) {
    free(host);
    FREE_HP;
    return 2;
  }
  sin->sin_port = portnum;
  sin->sin_family = AF_INET;
#if defined(ANET)
  memcpy(&sin->sin_addr, hp->h_addr, sizeof(sin->sin_addr));
#else
  memcpy(&sin->sin_addr, hp->h_addr_list[0], hp->h_length);
#endif
  free(host);
  FREE_HP;
  return 1;
}
