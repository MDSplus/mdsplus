#ifndef SOCKET_PORT_H
#define SOCKET_PORT_H
#ifdef _WIN32
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT _WIN32_WINNT_WIN8 // Windows 8.0
#include <io.h>
#include <process.h>
#include <winsock2.h>
#include <ws2tcpip.h>
typedef int socklen_t;
#define close closesocket
#define ioctl ioctlsocket
#define FIONREAD_TYPE u_long
#define snprintf _snprintf
#define getpid _getpid
#define SHUT_RDWR 2
#else
typedef int SOCKET;
#define INVALID_SOCKET -1
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
#include <pthread_port.h>

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif
#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT 0
#endif

#define SEND_BUF_SIZE 32768
#define RECV_BUF_SIZE 32768

#ifdef _WIN32
#define DEFINE_INITIALIZESOCKETS                                               \
  static void InitializeSockets() {                                            \
    WSADATA wsaData;                                                           \
    WORD wVersionRequested;                                                    \
    wVersionRequested = MAKEWORD(1, 1);                                        \
    WSAStartup(wVersionRequested, &wsaData);                                   \
  }                                                                            \
  INIT_SHARED_FUNCTION_ONCE(InitializeSockets)
#define INITIALIZESOCKETS RUN_SHARED_FUNCTION_ONCE(InitializeSockets)
#else
#define DEFINE_INITIALIZESOCKETS
#define INITIALIZESOCKETS
#endif

#endif // SOCKET_PORT_H
