/*  CMS REPLACEMENT HISTORY, Element IPDESC.H */
/*  *17   16-OCT-1995 13:23:47 TWF "Cleaner windows version" */
/*  *16   23-DEC-1994 09:13:03 TWF "Change how events are handled" */
/*  *15   21-DEC-1994 14:16:20 TWF "Add event can" */
/*  *14   21-DEC-1994 12:03:53 TWF "Add event can" */
/*  *13   21-DEC-1994 12:02:55 TWF "Add event can" */
/*  *12   20-DEC-1994 17:24:56 TWF "support events on unix" */
/*  *11    7-JUL-1994 12:08:10 TWF "Change _NO_PROTO to something else" */
/*  *10   14-JUN-1994 09:28:55 TWF "add CRAY" */
/*  *9    13-JUN-1994 09:52:08 TWF "Add NO_PROTO" */
/*  *8    10-JUN-1994 13:47:00 TWF "use define with int32" */
/*  *7     9-JUN-1994 16:20:20 TWF "Use int32 instead of long" */
/*  *6     9-JUN-1994 16:18:58 TWF "Use int32 instead of long" */
/*  *5     2-JUN-1994 12:20:35 TWF "Change GetAnswerInfo to return pointer to data" */
/*  *4     2-JUN-1994 12:18:36 TWF "Change GetAnswerInfo to return pointer to data" */
/*  *3     1-JUN-1994 12:14:05 TWF "Add RS6000 support" */
/*  *2    17-MAY-1994 09:34:55 TWF "Add CDECL" */
/*  *1    17-MAY-1994 09:33:58 TWF "descriptor structures" */
/*  CMS REPLACEMENT HISTORY, Element IPDESC.H */
#ifdef __MSDOS__
#include "winsock.h"
typedef long int32;
#else
typedef int SOCKET;
typedef int int32;
#define PASCAL
#endif

#define MAX_DIMS 8
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
                 int32  dims[MAX_DIMS];
		 void *ptr;
	       };
#define EndOfArgs ((struct descrip *)0)

#ifdef _NO_MDS_PROTO
extern SOCKET PASCAL ConnectToMds();
extern SOCKET PASCAL ConnectToMdsEvents();
extern int32  PASCAL SendArg();
extern int32  PASCAL GetAnswerInfo();
extern int32   MdsValue();
extern int32   MdsPut();
extern int32   PASCAL DisconnectFromMds();
extern int32   PASCAL MdsOpen();
extern int32   PASCAL MdsSetDefault();
extern int32   PASCAL MdsClose();
extern int32   PASCAL MdsEventAst();
extern int32   PASCAL MdsEventCan();
#else
extern SOCKET PASCAL ConnectToMds(char *host);
extern SOCKET PASCAL ConnectToMdsEvents(char *host);
extern int32  PASCAL SendArg(SOCKET s,unsigned char i,char dtype,unsigned char nargs,short len,char ndims,int32 *dims,
                                 char *ptr);
extern int32   PASCAL GetAnswerInfo(SOCKET s, char *dtype, short *len, char *ndims, int32 *dims, 
			  int32 *nbytes, void * *dptr);
extern int32   MdsValue(SOCKET sock, char *,...);
extern int32   MdsPut(SOCKET sock, char *node, char *expression,...);
extern int32   PASCAL DisconnectFromMds(SOCKET sock);
extern int32   PASCAL MdsOpen(SOCKET sock, char *tree, int32 shot);
extern int32   PASCAL MdsSetDefault(SOCKET sock, char *node);
extern int32   PASCAL MdsClose(SOCKET sock);
extern int32   PASCAL MdsEventAst(SOCKET sock, char *eventnam, void (*astadr)(), void *astprm, int32 *eventid);
extern int32   PASCAL MdsEventCan(SOCKET sock, int32 eventid);
#endif
