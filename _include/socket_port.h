#ifndef SOCKET_PORT_H
#define SOCKET_PORT_H
#ifdef _WIN32
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT _WIN32_WINNT_WIN8 // Windows 8.0
#include <stdio.h>
#include <io.h>
#include <process.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "pthread_port.h"
typedef int socklen_t;
#define FIONREAD_TYPE u_long
#define snprintf _snprintf
#define getpid _getpid
#define SHUT_RDWR 2
#else
typedef struct sockaddr SOCKADDR;
#define SOCKADDR struct sockaddr
#define closesocket close
#define ioctlsocket ioctl
typedef int SOCKET;
#define INVALID_SOCKET (SOCKET)(-1)
#define SOCKET_ERROR (-1)
#define FIONREAD_TYPE int
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/wait.h>
#endif

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#ifdef _WIN32
#define MSG_NOSIGNAL_ALT_PUSH() NOP()
#define MSG_NOSIGNAL_ALT_POP() NOP()
#else
#include <signal.h>
#define MSG_NOSIGNAL_ALT_PUSH() signal(SIGPIPE, SIG_IGN)
#define MSG_NOSIGNAL_ALT_POP() signal(SIGPIPE, SIG_DFL)
#endif
#else
#define MSG_NOSIGNAL_ALT_PUSH() NOP()
#define MSG_NOSIGNAL_ALT_POP() NOP()
#endif
#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT 0
#endif

#define SEND_BUF_SIZE 32768
#define RECV_BUF_SIZE 32768

#ifdef _WIN32
#define DEFINE_INITIALIZESOCKETS             \
  static void InitializeSockets()            \
  {                                          \
    WSADATA wsaData;                         \
    WORD wVersionRequested;                  \
    wVersionRequested = MAKEWORD(1, 1);      \
    WSAStartup(wVersionRequested, &wsaData); \
  }                                          \
  INIT_SHARED_FUNCTION_ONCE(InitializeSockets)
#define INITIALIZESOCKETS RUN_SHARED_FUNCTION_ONCE(InitializeSockets)
#define socklen_t int
static void _print_socket_error(char *message, int error)
{
  wchar_t *werrorstr = NULL;
  FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                 NULL, error,
                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                 (LPWSTR)&werrorstr, 0, NULL);
  if (werrorstr)
  {
    fprintf(stderr, "%s - WSA(%d) %S\n", message, error, werrorstr);
    LocalFree(werrorstr);
  }
  else
  {
    char *errorstr;
    switch (error)
    {
#define __SOCKET_CASE(WSA) \
  case WSA:                \
    errorstr = #WSA;       \
    break;
      __SOCKET_CASE(WSAEINTR);
      __SOCKET_CASE(WSAEBADF);
      __SOCKET_CASE(WSAEACCES);
      __SOCKET_CASE(WSAEFAULT);
      __SOCKET_CASE(WSAEINVAL);
      __SOCKET_CASE(WSAEMFILE);
      __SOCKET_CASE(WSAEWOULDBLOCK);
      __SOCKET_CASE(WSAEINPROGRESS);
      __SOCKET_CASE(WSAEALREADY);
      __SOCKET_CASE(WSAENOTSOCK);
      __SOCKET_CASE(WSAEDESTADDRREQ);
      __SOCKET_CASE(WSAEMSGSIZE);
      __SOCKET_CASE(WSAEPROTOTYPE);
      __SOCKET_CASE(WSAENOPROTOOPT);
      __SOCKET_CASE(WSAEPROTONOSUPPORT);
      __SOCKET_CASE(WSAEOPNOTSUPP);
      __SOCKET_CASE(WSAEPFNOSUPPORT);
      __SOCKET_CASE(WSAEADDRINUSE);
      __SOCKET_CASE(WSAEADDRNOTAVAIL);
      __SOCKET_CASE(WSAENETDOWN);
      __SOCKET_CASE(WSAENETUNREACH);
      __SOCKET_CASE(WSAENETRESET);
      __SOCKET_CASE(WSAECONNABORTED);
      __SOCKET_CASE(WSAECONNRESET);
      __SOCKET_CASE(WSAENOBUFS);
      __SOCKET_CASE(WSAEISCONN);
      __SOCKET_CASE(WSAENOTCONN);
      __SOCKET_CASE(WSAESHUTDOWN);
      __SOCKET_CASE(WSAETOOMANYREFS);
      __SOCKET_CASE(WSAETIMEDOUT);
      __SOCKET_CASE(WSAECONNREFUSED);
      __SOCKET_CASE(WSAELOOP);
      __SOCKET_CASE(WSAENAMETOOLONG);
      __SOCKET_CASE(WSAEHOSTDOWN);
      __SOCKET_CASE(WSAEHOSTUNREACH);
      __SOCKET_CASE(WSAENOTEMPTY);
      __SOCKET_CASE(WSAEPROCLIM);
      __SOCKET_CASE(WSASYSNOTREADY);
      __SOCKET_CASE(WSAVERNOTSUPPORTED);
      __SOCKET_CASE(WSANOTINITIALISED);
      __SOCKET_CASE(WSAEDISCON);
#undef __SOCKET_CASE
    default:
      fprintf(stderr, "%s - WSA(%d) WSAError\n", message, error);
      return;
    }
    fprintf(stderr, "%s - WSA(%d) %s\n", message, error, errorstr);
  }
}
inline static void print_socket_error(char *message)
{
  _print_socket_error(message, WSAGetLastError());
}
#else
#define print_socket_error(message) fprintf(stderr, "%s\n", message)
#define DEFINE_INITIALIZESOCKETS
#define INITIALIZESOCKETS
#endif

#endif // SOCKET_PORT_H
