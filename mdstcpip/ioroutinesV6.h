#define AF_T AF_INET6
#define PF_T AF_INET6
#define PORTDELIM '#'
#define SOCKADDR_IN sockaddr_in6
#define SIN_FAMILY sin6_family
#define SIN_ADDR sin6_addr
#define SIN_PORT sin6_port
#define GET_IPHOST(sin) \
char iphost[INET6_ADDRSTRLEN];\
inet_ntop(AF_INET6, &sin.sin6_addr, iphost, INET6_ADDRSTRLEN)

#include "mdsip_connections.h"
#include <STATICdef.h>
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <mdsplus/mdsconfig.h>
#include <time.h>
#ifdef HAVE_UNISTD_H
 #include <unistd.h>
#endif
#ifdef HAVE_SYS_FILIO_H
 #include <sys/filio.h>
#endif
#ifdef _WIN32
 #ifdef _WIN32_WINNT
  #undef _WIN32_WINNT
 #endif
 #define _WIN32_WINNT _WIN32_WINNT_WIN8 // Windows 8.0
 #include <winsock2.h>
 #include <ws2tcpip.h>
 #define ioctl ioctlsocket
 #define FIONREAD_TYPE u_long
 typedef int socklen_t;
 #define snprintf _snprintf
 #define MSG_DONTWAIT 0
 #include <io.h>
 #include <process.h>
 #define getpid _getpid
#else
 #define INVALID_SOCKET -1
 #define FIONREAD_TYPE int
 #include <sys/socket.h>
 #include <netdb.h>
 #include <netinet/in.h>
 #include <netinet/tcp.h>
 #include <arpa/inet.h>
 #include <sys/ioctl.h>
 #include <sys/wait.h>
#endif
#define SEND_BUF_SIZE 32768
#define RECV_BUF_SIZE 32768
#ifndef MSG_NOSIGNAL
 #define MSG_NOSIGNAL 0
#endif
#define LOAD_INITIALIZESOCKETS
#include <pthread_port.h>

static int GetHostAndPort(char *hostin, struct sockaddr_in6 *sin);

static int io_reuseCheck(char *host, char *unique, size_t buflen){
  struct sockaddr_in6 sin;
  if IS_OK(GetHostAndPort(host, &sin)) {
    unsigned short *addr = (unsigned short *)&sin.sin6_addr;
    snprintf(unique, buflen, "%s://%x:%x:%x:%x:%x:%x:%x:%x#%d", PROT,
             addr[0], addr[1], addr[2], addr[3],
             addr[4], addr[5], addr[6], addr[7], ntohs(sin.sin6_port));
    return C_OK;
  } else {
    *unique = 0;
    return C_ERROR;
  }
}

