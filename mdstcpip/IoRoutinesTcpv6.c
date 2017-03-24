#define TCPVER "tcpv6"
#define STRUCT_SOCKADDR struct sockaddr_in6
#define NEW_SOCKET socket(PF_INET6, SOCK_STREAM, 0)
#ifdef _WIN32
 #include <winsock2.h>
 #include <ws2tcpip.h>
#endif
#define TCPV6
#include "ioroutinestcp.h"
