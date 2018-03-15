#ifdef ANET
#include "ANETP_SOCK_ROUTINES.H"
#include "ANETP_TYPES.H"
#include "ANETP_SOCKET.H"
#include "ANETP_IN.H"
#include "ANETP_NETDB.H"
#include "ANETP_TIME.H"
#define INVALID_SOCKET -1
#define FD_ZERO(set) memset(set,0,sizeof(fd_set))
#define FD_SET(s,set) lib$insv(&1,&s,&1,set)
#define FD_CLR(s,set) lib$insv(&0,&s,&1,set)
#define FD_ISSET(s,set) lib$extv(&s,&1,set)
#define FD_SETSIZE 16
#define TCP_NODELAY 1
#include <errno.h>
#else
#if defined(_WIN32) || defined(__APPLE__)
#define I_NREAD FIONREAD
#endif

#if defined(_WIN32)
#define ioctl ioctlsocket
#else
#include <sys/ioctl.h>
#ifndef I_NREAD
#ifdef FIONREAD
#define I_NREAD FIONREAD
#else
#if !defined(__sparc__) && !defined(__QNX__)
#include <stropts.h>
#endif
#endif
#endif
#endif

#if defined(__sgi) || defined(sun)
#define memcpy(a,b,c) (bcopy(b,a,c),(void *)b)
#include <errno.h>
#elif defined(_WIN32)
#include <errno.h>
#include <time.h>
#elif defined (__QNX__)
#include <errno.h>
#else
#include <sys/errno.h>
#endif
#ifdef _WIN32
//#include <windows.h>
//#include <io.h>
#include <winsock2.h>
//#include <winsock.h>
#else
#define INVALID_SOCKET -1
#include <sys/types.h>
#ifdef __APPLE__
#include <pwd.h>
#endif
#include <fcntl.h>
#include <sys/time.h>
#ifdef _XOPEN_SOURCE_EXTENDED
#include <arpa/inet.h>
#else
#include <netinet/in.h>
#endif
#include <sys/socket.h>
#include <netdb.h>
#include "signal.h"
#include <netinet/tcp.h>
#endif
#ifdef _AIX			/* IBM AIX */
#include <sys/select.h>
#endif
#endif

#include <stdio.h>
#include <string.h>
#ifdef _USE_VARARGS
#include <varargs.h>
#define _NO_MDS_PROTO
#else
#include <stdarg.h>
#endif
#ifdef __MWERKS__
#include "ipdesc.h"
#else
#include <ipdesc.h>
#endif
#include <stdlib.h>

#define VMS_CLIENT     1
#define IEEE_CLIENT    2
#define JAVA_CLIENT    3
#define VMSG_CLIENT    4
#define CRAY_IEEE_CLIENT 7
#define CRAY_CLIENT    8
#define BigEndian      0x80
#define SwapEndianOnServer 0x40
#define COMPRESSED    0x20
#define SENDCAPABILITIES 0xf
#define LittleEndian   0
#define Endian(c)  (c & BigEndian)
#define CType(c)   (c & 0x0f)
#define IsCompressed(c) (c & COMPRESSED)
#ifdef NOCOMPRESSION
#define SUPPORTS_COMPRESSION 0
#else
#define SUPPORTS_COMPRESSION 0x8000
#endif
#define SupportsCompression(c) (c & SUPPORTS_COMPRESSION)

#define EVENTASTREQUEST     "---EVENTAST---REQUEST---"
#define EVENTCANREQUEST     "---EVENTCAN---REQUEST---"

#define SEND_BUF_SIZE 32768
#define RECV_BUF_SIZE 32768

#if defined(WIN32) || defined(__linux__) || defined(_NO_SIGHOLD)
#define sighold(arg)
#define sigrelse(arg)
#endif

#ifdef  MULTINET
#define close socket_close
#define perror socket_perror
#define ioctl socket_ioctl
#endif

#if defined(__CRAY) || defined(CRAY)
int errno = 0;
#define bits32 :32
#define bits16 :16
#else
#define bits32
#define bits16
#endif

typedef struct _eventinfo {
  char data[12];
  int eventid;
  void (*astadr) (void *, int, char *);
  void *astprm;
} MdsEventInfo;

typedef struct _jeventinfo {
  char data[12];
  char eventid;
} JMdsEventInfo;

typedef struct _eventlist {
  SOCKET sock;
  int eventid;
  char jeventid;
  MdsEventInfo *info;
  int info_len;
  struct _eventlist *next;
} MdsEventList;

typedef struct _msghdr {
  int msglen bits32;
  int status bits32;
  short length bits16;
  unsigned char nargs;
  unsigned char descriptor_idx;
  unsigned char message_id;
  unsigned char dtype;
  signed char client_type;
  unsigned char ndims;
#if defined(__CRAY) || defined(CRAY)
  long dims[(MAX_DIMS + 1) / 2];
#else
  int dims[MAX_DIMS];
  int fill;
#endif
} MsgHdr;

typedef struct _mds_message {
  MsgHdr h;
  char bytes[1];
} Message, *MsgPtr;
