#define AF_T AF_INET
#define PF_T PF_INET
#define PORTDELIM ':'
#define SOCKADDR_IN sockaddr_in
#define SIN_FAMILY sin_family
#define SIN_ADDR sin_addr
#define SIN_PORT sin_port
#define GET_IPHOST(sin) char *iphost = inet_ntoa(sin.sin_addr)

#include "mdsip_connections.h"
#include <status.h>
#include <STATICdef.h>
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <config.h>
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
static ssize_t io_send(int conid, const void *buffer, size_t buflen, int nowait);
#ifdef _WIN32
static ssize_t io_recv(int conid, void *buffer, size_t len);
#define io_recv_to NULL
#else
static ssize_t io_recv_to(int conid, void *buffer, size_t len, int to_msec);
inline static ssize_t io_recv(int conid, void *buffer, size_t len){
  return io_recv_to(conid,buffer,len,-1);
}
#endif
static int io_disconnect(int conid);
static int io_flush(int conid);
static int io_listen(int argc, char **argv);
static int io_authorize(int conid, char *username);
static int io_connect(int conid, char *protocol, char *host);
static int io_reuseCheck(char *host, char *unique, size_t buflen);
static IoRoutines io_routines = {
  io_connect, io_send, io_recv, io_flush, io_listen, io_authorize, io_reuseCheck, io_disconnect, io_recv_to
};

static int GetHostAndPort(char *hostin, struct sockaddr_in *sin);

static int io_reuseCheck(char *host, char *unique, size_t buflen){
  struct sockaddr_in sin;
  if IS_OK(GetHostAndPort(host, &sin)) {
    char *addr = (char *)&sin.sin_addr;
    snprintf(unique, buflen,
      "%s://%d.%d.%d.%d:%d",
      PROT, addr[0], addr[1], addr[2], addr[3], ntohs(sin.sin_port));
    return C_OK;
  }
  *unique = 0;
  return C_ERROR;
}
