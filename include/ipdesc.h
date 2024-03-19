#ifndef IPDESC_H
#define IPDESC_H

#define MAX_DIMS 8
#define DTYPE_UCHAR 2
#define DTYPE_USHORT 3
#define DTYPE_ULONG 4
#define DTYPE_ULONGLONG 5
#define DTYPE_CHAR 6
#define DTYPE_SHORT 7
#define DTYPE_LONG 8
#define DTYPE_LONGLONG 9
#ifdef DTYPE_FLOAT
#undef DTYPE_FLOAT
#endif
#define DTYPE_FLOAT 10
#ifdef DTYPE_DOUBLE
#undef DTYPE_DOUBLE
#endif
#define DTYPE_DOUBLE 11
#define DTYPE_COMPLEX 12
#define DTYPE_COMPLEX_DOUBLE 13
#define DTYPE_CSTRING 14
#define DTYPE_EVENT_NOTIFY 99
#define DTYPE_SERIAL 24

struct descrip
{
  char dtype;
  char ndims;
  int dims[MAX_DIMS];
  int length;
  void *ptr;
};
#define EndOfArgs ((struct descrip *)0)

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef __MDSIP_H__
#define INVALID_CONNECTION_ID -1
  extern int ConnectToMds(char *host);
  extern int SendArg(int s, unsigned char i, char dtype, unsigned char nargs,
                     unsigned short len, char ndims, int *dims, char *ptr);
  extern int GetAnswerInfo(int s, char *dtype, short *len, char *ndims, int *dims,
                           int *nbytes, void **dptr);
  extern int DisconnectFromMds(int conid);
  extern struct descrip *MakeDescrip(struct descrip *in_descrip, char dtype,
                                     char ndims, int *dims, void *ptr);
  extern struct descrip *MakeDescripWithLength(struct descrip *in_descrip,
                                               char dtype, int length, char ndims,
                                               int *dims, void *ptr);
  extern int MdsEventAst(int conid, char *eventnam, void (*astadr)(),
                         void *astprm, int *eventid);
  extern int MdsEventCan(int conid, int eventid);
  extern int HostToIp(char *host, int *addr, short *port);
#ifndef MdsLib_H
  extern int MdsValue(int conid, char *, ...);
  extern int MdsPut(int conid, char *node, char *expression, ...);
  extern int MdsOpen(int conid, char *tree, int shot);
  extern int MdsSetDefault(int conid, char *node);
  extern int MdsClose(int conid);
  extern void FreeMessage(void *m);
#endif
#endif

#ifdef __cplusplus
} // extern "C"
#endif
#endif
