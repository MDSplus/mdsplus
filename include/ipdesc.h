#ifdef _WIN32
#include "windows.h"
#else
typedef int SOCKET;
#endif

#define MAX_DIMS 7
#define DTYPE_UCHAR   2
#define DTYPE_USHORT  3
#define DTYPE_ULONG   4
#define DTYPE_ULONGLONG 5
#define DTYPE_CHAR    6
#define DTYPE_SHORT   7
#define DTYPE_LONG    8
#define DTYPE_LONGLONG 9
#ifdef DTYPE_FLOAT
#undef DTYPE_FLOAT
#endif
#define DTYPE_FLOAT   10
#ifdef DTYPE_DOUBLE
#undef DTYPE_DOUBLE
#endif
#define DTYPE_DOUBLE  11
#define DTYPE_COMPLEX 12
#define DTYPE_COMPLEX_DOUBLE 13
#define DTYPE_CSTRING 14
#define DTYPE_EVENT_NOTIFY   99
#ifndef DTYPE_EVENT
#define DTYPE_EVENT DTYPE_EVENT_NOTIFY
#endif

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
extern int   HostToIp();
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
extern int   MdsEventAst(SOCKET sock, char *eventnam, void (*astadr)(), void *astprm, int *eventid);
extern int   MdsEventCan(SOCKET sock, int eventid);
extern int HostToIp(char *host, int *addr, short *port);
#ifndef MdsLib_H
extern int   MdsValue(SOCKET sock, char *,...);
extern int   MdsPut(SOCKET sock, char *node, char *expression,...);
extern int   MdsOpen(SOCKET sock, char *tree, int shot);
extern int   MdsSetDefault(SOCKET sock, char *node);
extern int   MdsClose(SOCKET sock);
#endif
#endif
