/*  CMS REPLACEMENT HISTORY, Element IPDESC.H */
/*  *22   12-JAN-1999 16:13:39 JMS "Removed function prototypes for MdsOpen, MdsValue etc. */
/*  *21   21-APR-1998 11:16:34 TWF "Add MakeDescripWidthLen" */
/*  *20   17-APR-1998 10:57:54 TWF "Add MakeDescr prototype" */
/*  *19   17-APR-1998 10:56:53 TWF "Add MakeDescr prototype" */
/*  *18   14-APR-1998 11:58:54 TWF "Reduce maxdims and return length field" */
/*  *17   16-OCT-1995 13:23:47 TWF "Cleaner windows version" */
/*  *16   23-DEC-1994 09:13:03 TWF "Change how events are handled" */
/*  *15   21-DEC-1994 14:16:20 TWF "Add event can" */
/*  *14   21-DEC-1994 12:03:53 TWF "Add event can" */
/*  *13   21-DEC-1994 12:02:55 TWF "Add event can" */
/*  *12   20-DEC-1994 17:24:56 TWF "support events on unix" */
/*  *11    7-JUL-1994 12:08:10 TWF "Change _NO_PROTO to something else" */
/*  *10   14-JUN-1994 09:28:55 TWF "add CRAY" */
/*  *9    13-JUN-1994 09:52:08 TWF "Add NO_PROTO" */
/*  *8    10-JUN-1994 13:47:00 TWF "use define with int" */
/*  *7     9-JUN-1994 16:20:20 TWF "Use int instead of long" */
/*  *6     9-JUN-1994 16:18:58 TWF "Use int instead of long" */
/*  *5     2-JUN-1994 12:20:35 TWF "Change GetAnswerInfo to return pointer to data" */
/*  *4     2-JUN-1994 12:18:36 TWF "Change GetAnswerInfo to return pointer to data" */
/*  *3     1-JUN-1994 12:14:05 TWF "Add RS6000 support" */
/*  *2    17-MAY-1994 09:34:55 TWF "Add CDECL" */
/*  *1    17-MAY-1994 09:33:58 TWF "descriptor structures" */
/*  CMS REPLACEMENT HISTORY, Element IPDESC.H */
#ifdef _WIN32
#include "windows.h"
//#include "winsock.h"
#else
typedef int SOCKET;
#endif

#define MAX_DIMS 7
#define DTYPE_UCHAR   2
#define DTYPE_USHORT  3
#define DTYPE_ULONG   4
#define DTYPE_CHAR    6
#define DTYPE_SHORT   7
#define DTYPE_LONG    8
#define DTYPE_FLOAT   10
#define DTYPE_DOUBLE  11
#define DTYPE_COMPLEX 12
#define DTYPE_CSTRING 14
#define DTYPE_EVENT   99

struct descrip { char dtype;
                 char ndims;
                 int  dims[MAX_DIMS];
                 int  length;
		 void *ptr;
	       };
#define EndOfArgs ((struct descrip *)0)

#ifdef _NO_MDS_PROTO
extern SOCKET ConnectToMds();
extern SOCKET ConnectToMdsEvents();
extern int  SendArg();
extern int  GetAnswerInfo();
extern int   DisconnectFromMds();
extern struct descrip *MakeDescrip();
extern struct descrip *MakeDescripWithLength();
extern int   MdsEventAst();
extern int   MdsEventCan();
#ifndef MdsLib_H
extern int   MdsValue();
extern int   MdsPut();
extern int   MdsOpen();
extern int   MdsSetDefault();
extern int   MdsClose();
#endif
#else
extern SOCKET ConnectToMds(char *host);
extern SOCKET ConnectToMdsEvents(char *host);
extern int  SendArg(SOCKET s,unsigned char i,char dtype,unsigned char nargs,short len,char ndims,int *dims,
                                 char *ptr);
extern int   GetAnswerInfo(SOCKET s, char *dtype, short *len, char *ndims, int *dims, 
			  int *nbytes, void * *dptr);
extern int   DisconnectFromMds(SOCKET sock);
extern struct descrip *MakeDescrip(struct descrip *in_descrip, char dtype, char ndims, int *dims, void *ptr);
extern struct descrip *MakeDescripWithLength(struct descrip *in_descrip, char dtype, int length, char ndims, int *dims, void *ptr);
extern int   MdsEventAst(SOCKET sock, char *eventnam, void (*astadr)(), void *astprm, void **eventid);
extern int   MdsEventCan(SOCKET sock, void *eventid);
#ifndef MdsLib_H
extern int   MdsValue(SOCKET sock, char *,...);
extern int   MdsPut(SOCKET sock, char *node, char *expression,...);
extern int   MdsOpen(SOCKET sock, char *tree, int shot);
extern int   MdsSetDefault(SOCKET sock, char *node);
extern int   MdsClose(SOCKET sock);
#endif
#endif
