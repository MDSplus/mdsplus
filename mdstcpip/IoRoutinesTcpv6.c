#define TCPVER "tcpv6"
#define STRUCT_SOCKADDR struct sockaddr_in6
#define NEW_SOCKET socket(PF_INET6, SOCK_STREAM, 0)
#ifdef _WIN32
 #include <winsock2.h>
 #include <ws2tcpip.h>
#endif
#define TCPV6

#include "ioroutinestcp.h"
static int getHostAndPort(char *hostin, STRUCT_SOCKADDR *sin){
  struct addrinfo hints, *info;
  int n;
  size_t i;
  static char *mdsip = "mdsip";
  char *host = strcpy((char *)malloc(strlen(hostin) + 1), hostin);
  char *service = mdsip;
  int num_colons;
  int ans = 1;
  INITIALIZESOCKETS;
  for (i = 0, num_colons = 0; i < strlen(host); i++) {
    if (host[i] == ':')
      num_colons++;
  }
  if (num_colons > 0) {
    for (i = strlen(host); i > 0; i--) {
      if (host[i - 1] == ':') {
        service = &host[i];
        host[i - 1] = '\0';
        break;
      }
    }
  }
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET6;
  hints.ai_socktype = SOCK_STREAM;
  n = getaddrinfo(host, service, &hints, &info);
  if (n < 0) {
    fprintf(stderr, "getaddrinfo error:: [%s]\n", gai_strerror(n));
    ans = 0;
  } else {
    memcpy(sin, info->ai_addr, info->ai_addrlen);
    freeaddrinfo(info);
  }

  free(host);
  return ans;
}
