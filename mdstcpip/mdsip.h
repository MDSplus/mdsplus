/*  CMS REPLACEMENT HISTORY, Element MDSIP.H */
/*  *84   21-JUN-2000 13:35:57 TWF "Error getting peer name" */
/*  *83   21-JUN-2000 11:09:48 TWF "Make same as unix" */
/*  *82   21-JUN-2000 10:33:18 TWF "Add compression" */
/*  *81   21-JUN-2000 09:32:45 TWF "Add compression" */
/*  *80   21-JUN-2000 09:25:20 TWF "Add compression" */
/*  *79   21-JUN-2000 09:22:40 TWF "Add compression" */
/*  *78   20-JUN-2000 16:42:39 TWF "Add compression" */
/*  *77   20-JUN-2000 16:39:33 TWF "Add compression" */
/*  *76    8-MAY-2000 11:16:50 TWF "Change buffer sizes" */
/*  *75   10-MAR-2000 15:16:05 TWF "" */
/*  *74   10-MAR-2000 14:40:38 TWF "Add JAPAN ANET support" */
/*  *73   10-MAR-2000 10:26:32 TWF "Add Japan ANET Support" */
/*  *72   10-MAR-2000 10:04:18 TWF "Add Japan ANET support" */
/*  *71   22-OCT-1999 16:36:11 TWF "Fix multinet" */
/*  *70   22-OCT-1999 15:56:57 TWF "don't include fcntl on vms" */
/*  *69   22-OCT-1999 12:05:21 TWF "Make same as unix" */
/*  *68   21-OCT-1999 10:39:50 TWF "Add login" */
/*  *67   20-OCT-1999 16:11:45 TWF "" */
/*  *66   16-JUL-1999 15:21:31 TWF "Fix compile error" */
/*  *65    5-JAN-1999 10:21:52 TWF "Fix swap on server" */
/*  *64    5-JAN-1999 10:16:35 TWF "Fix swap on server" */
/*  *63    5-JAN-1999 10:16:12 TWF "Fix swap on server" */
/*  *62    4-JAN-1999 14:13:39 TWF "add swap on server capability" */
/*  *61    4-JAN-1999 12:25:35 TWF "add swap on server capability" */
/*  *60   16-JUN-1998 16:21:26 TWF "use 5000 on OpenVMS" */
/*  *59   15-JUN-1998 15:32:21 TWF "Select buffer size" */
/*  *58   14-APR-1998 15:58:00 TWF "Add sighold defines" */
/*  *57   14-APR-1998 15:23:06 TWF "Fix header length to be compatible" */
/*  *56    7-APR-1998 10:10:45 TWF "Support VAXG client (from epfl)" */
/*  *55   23-SEP-1997 10:22:14 TWF "Avoid interrupts from IDL" */
/*  *54   23-SEP-1997 10:21:52 TWF "Avoid interrupts from IDL" */
/*  *53   21-MAY-1997 14:20:56 TWF "add save/restore context" */
/*  *52   21-MAY-1997 11:21:12 TWF "Add ability to get unique connection name" */
/*  *51   10-DEC-1996 12:17:52 TWF "Add client event id capability" */
/*  *50   20-SEP-1996 16:07:32 TWF "Fix for linux" */
/*  *49    1-MAR-1996 15:25:17 TWF "" */
/*  *48    1-MAR-1996 15:16:40 TWF "Add Java Client" */
/*  *47    1-MAR-1996 14:33:16 TWF "Add Java Client" */
/*  *46    2-JAN-1996 14:10:16 TWF "Fix event msg" */
/*  *45    2-JAN-1996 13:55:39 TWF "New multinet" */
/*  *44   16-OCT-1995 13:25:14 TWF "Cleaner windows version" */
/*  *43   20-JUL-1995 08:09:23 TWF "multinet change" */
/*  *42    6-JAN-1995 15:03:19 TWF "Fix aix" */
/*  *41    6-JAN-1995 11:48:30 TWF "Compute client type" */
/*  *40    6-JAN-1995 11:43:06 TWF "Compute client type" */
/*  *39    5-JAN-1995 16:48:34 TWF "Add SUN support" */
/*  *38    5-JAN-1995 16:27:34 TWF "SUN have reversed bytes" */
/*  *37    5-JAN-1995 15:50:22 TWF "include errno on sun" */
/*  *36    5-JAN-1995 09:45:04 TWF "New definitions" */
/*  *35   23-DEC-1994 09:43:43 TWF "Change eventinfo" */
/*  *34   23-DEC-1994 09:13:06 TWF "Change how events are handled" */
/*  *33   21-DEC-1994 16:46:17 TWF "Support aix" */
/*  *32   21-DEC-1994 14:16:25 TWF "Add event can" */
/*  *31   21-DEC-1994 12:02:52 TWF "Add event can" */
/*  *30   12-DEC-1994 15:03:24 TWF "Use compiler builtins" */
/*  *29   22-NOV-1994 17:28:46 TWF "Add sgi" */
/*  *28   22-NOV-1994 17:27:42 TWF "Add sgi" */
/*  *27   22-NOV-1994 17:27:22 TWF "Add sgi" */
/*  *26   22-NOV-1994 17:25:05 TWF "Add sgi" */
/*  *25    6-JUL-1994 16:18:31 TWF "Convert OSF_CLIENT floating point" */
/*  *24   14-JUN-1994 13:22:39 TWF "handle dims differently on Cray" */
/*  *23   14-JUN-1994 11:56:02 TWF "Change client types" */
/*  *22   14-JUN-1994 10:13:38 TWF "put client_type in message" */
/*  *21   14-JUN-1994 09:34:08 TWF "add CRAY" */
/*  *20   14-JUN-1994 09:22:16 TWF "add CRAY" */
/*  *19   13-JUN-1994 16:00:48 TWF "add CRAY" */
/*  *18   13-JUN-1994 15:09:58 TWF "add HP" */
/*  *17   13-JUN-1994 15:06:31 TWF "add HP" */
/*  *16   10-JUN-1994 14:42:44 TWF "use <stdarg.h> for linux" */
/*  *15   10-JUN-1994 12:08:23 TWF "Fix location of stdarg" */
/*  *14   10-JUN-1994 11:50:30 TWF "Move externs" */
/*  *13   10-JUN-1994 11:48:12 TWF "Fix problem with dec includes" */
/*  *12   10-JUN-1994 11:35:21 TWF "Add another definition" */
/*  *11   10-JUN-1994 11:32:13 TWF "Add another definition" */
/*  *10   10-JUN-1994 11:23:12 TWF "Add errno" */
/*  *9    10-JUN-1994 09:48:59 TWF "Add alpha osf1" */
/*  *8     9-JUN-1994 16:19:02 TWF "Use int32 instead of long" */
/*  *7     9-JUN-1994 08:20:13 TWF "Add linux" */
/*  *6     7-JUN-1994 15:24:05 TWF "Add Ultrix system" */
/*  *5     1-JUN-1994 14:44:04 TWF "Flip bytes" */
/*  *4     1-JUN-1994 12:35:11 TWF "Temporary fix for RS6000" */
/*  *3     1-JUN-1994 12:24:32 TWF "Support RS6000" */
/*  *2    17-MAY-1994 15:09:57 TWF "Put IPDESC.H in MDS$ROOT:[SYSLIB]" */
/*  *1    17-MAY-1994 09:31:48 TWF "Include for MDSIPSHR" */
/*  CMS REPLACEMENT HISTORY, Element MDSIP.H */
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
#if defined(_WIN32) || defined(__VMS)
#define I_NREAD FIONREAD
#endif

#if defined(_WIN32)
#define ioctl ioctlsocket
#else
#include <sys/ioctl.h>
#endif

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
#include <sys/types.h>
#ifndef vxWorks
#ifndef __VMS
#include <fcntl.h>
#endif
#include <sys/time.h>
#endif
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
#endif

#include <stdio.h>
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
#include <string.h>
#include <stdlib.h>

#define VMS_CLIENT     1
#define IEEE_CLIENT    2
#define JAVA_CLIENT    3
#define VMSG_CLIENT    4
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

#define SEND_BUF_SIZE 32768
#define RECV_BUF_SIZE 32768

#if defined(__VMS) || defined(WIN32) || defined(__linux__) || defined(_NO_SIGHOLD)
#define sighold(arg)
#define sigrelse(arg)
#endif

#ifdef  MULTINET
#define close socket_close
#define perror socket_perror
#define ioctl socket_ioctl
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

typedef struct _mds_message { MsgHdr h;
                          char bytes[1];
			} Message, *MsgPtr;
