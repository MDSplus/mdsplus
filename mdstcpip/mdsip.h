#if defined(__sgi) || defined(sun)
#define memcpy(a,b,c) bcopy(b,a,c)
#include <errno.h>
#elif defined(_WIN32)
#include <errno.h>
#include <time.h>
#else
#ifndef vxWorks
#include <sys/errno.h>
#endif
#endif
#ifdef vxWorks
#include <types/vxTypesOld.h>
#include <errno.h>
#include <time.h>
#endif
#if defined(_WIN32)
#include <windows.h>
#include <io.h>
#include <winsock.h>
#else
#define INVALID_SOCKET -1
#ifndef vxWorks
#include <fcntl.h>
#include <sys/time.h>
#endif
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#ifndef vxWorks
#include <netdb.h>
#endif
#include "signal.h"
#include <netinet/tcp.h>
#endif
#ifdef _AIX /* IBM AIX */
#include <sys/select.h>
#endif

#include <stdio.h>
#ifdef _USE_VARARGS
#include <varargs.h>
#define _NO_MDS_PROTO
#else            
#include <stdarg.h>
#endif
#include <ipdesc.h>
#include <string.h>
#include <stdlib.h>

#define VMS_CLIENT     1
#define IEEE_CLIENT    2
#define JAVA_CLIENT    3
#define VMSG_CLIENT    4
#define CRAY_CLIENT    8
#define BigEndian      0x80
#define SwapEndianOnServer 0x40
#define LittleEndian   0
#define Endian(c)  (c & BigEndian)
#define CType(c)   (c & 0x0f)

#define EVENTASTREQUEST     "---EVENTAST---REQUEST---"
#define EVENTCANREQUEST     "---EVENTCAN---REQUEST---"

#define LOGINREQUEST        "---LOGIN------REQUEST___"
#define LOGINUSER           "---LOGIN------USER------"
#define LOGINGETP1          "---LOGIN------GETP1-----"
#define LOGINGETP2          "---LOGIN------GETP2-----"
#define LOGINPWD            "---LOGIN------PWD-------"
#define LOGINVMS            "---LOGIN------VMS-------"

#ifdef __VMS
#include <starlet.h>
#include <iodef.h>
#include <lib$routines.h>
#endif

#ifdef __VMS
#define SEND_BUF_SIZE 5000
#define RECV_BUF_SIZE 5000
#else
#define SEND_BUF_SIZE 32768
#define RECV_BUF_SIZE 32768
#endif

#if defined(__VMS) || defined(WIN32) || defined(__linux__) || defined(_NO_SIGHOLD)
#define sighold(arg)
#define sigrelse(arg)
#endif

#ifdef  MULTINET
#define close socket_close
#define perror socket_perror
#endif

#ifdef __CRAY
int errno = 0;
#define bits32 :32
#define bits16 :16
#else
#define bits32
#define bits16
#endif

typedef struct _eventinfo { char          data[12];
                            int          eventid;
			    void      (*astadr)(void *, int, char *);
                            void          *astprm;
                          } MdsEventInfo;

typedef struct _jeventinfo { char          data[12];
                             char          eventid;
                          } JMdsEventInfo;

typedef struct _eventlist { SOCKET        sock;
                            int          eventid;
			    char           jeventid;
                            MdsEventInfo  *info;
			    int		  info_len;
                            struct _eventlist *next;
                          } MdsEventList;

typedef struct _msghdr { int msglen bits32;
			 int status bits32;
                         short length bits16;
                         unsigned char nargs;
                         unsigned char descriptor_idx;
                         unsigned char message_id;
			 unsigned char dtype;
                         signed char client_type;
                         unsigned char ndims;
#ifdef __CRAY
			 long  dims[(MAX_DIMS+1)/2];
#else
                         int  dims[MAX_DIMS];
                         int  fill;
#endif
                       } MsgHdr;

typedef struct _message { MsgHdr h;
                          char bytes[1];
			} Message, *MsgPtr;
