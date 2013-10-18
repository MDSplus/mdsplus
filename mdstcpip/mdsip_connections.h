#ifndef __MDSIP_H__
#define __MDSIP_H__
#define MdsLib_H
#define _GNU_SOURCE /* glibc2 needs this */
#include <config.h>
#include <mdsdescrip.h>
#ifdef HAVE_WINDOWS_H
typedef int ssize_t;
#include <Winsock2.h>
#else
#include <sys/types.h>
#endif
#include <ipdesc.h>
#include <mds_stdarg.h>

#ifndef MDSIP_CONNECTIONS
#define MDSIP_CONNECTIONS
#endif

#define MAX_ARGS 256
#define MAX_COMPRESS 9

#ifndef NULL
#define NULL (void *)0
#endif

#define EVENTASTREQUEST     "---EVENTAST---REQUEST---"
#define EVENTCANREQUEST     "---EVENTCAN---REQUEST---"

#define VMS_CLIENT       1
#define IEEE_CLIENT      2
#define JAVA_CLIENT      3
#define VMSG_CLIENT      4
#define CRAY_IEEE_CLIENT 7
#define CRAY_CLIENT      8

typedef ARRAY_COEFF(char,7) ARRAY_7;

typedef struct _treecontext { void *tree;
                        } TreeContext;


typedef struct _eventinfo { char          data[12];
                            int          eventid;
			    void      (*astadr)(void *, int, char *);
                            void          *astprm;
                          } MdsEventInfo;

typedef struct _jeventinfo { char          data[12];
                             char          eventid;
                          } JMdsEventInfo;


typedef struct _eventlist { int          conid;
                            int          eventid;
			    char           jeventid;
                            MdsEventInfo  *info;
			    int		  info_len;
                            struct _eventlist *next;
                          } MdsEventList;


typedef struct _options {
  char *short_name;
  char *long_name;
  int  expects_value;
  int  present;
  char *value;
} Options;

typedef struct _connection {
  int id; /* unique connection id */
  char *protocol;
  char *info_name;
  void *info;
  size_t info_len;
  TreeContext context;
  unsigned char message_id;
  int client_type;
  int nargs;
  struct descriptor *descrip[MAX_ARGS];
  struct _eventlist *event;
  void         *tdicontext[6];
  int addr;
  int compression_level;
  int readfd;
  struct _io_routines *io;
  char deleted;
  struct _connection *next;
} Connection;


#if defined(__CRAY) || defined(CRAY)
int errno = 0;
#define bits32 :32
#define bits16 :16
#else
#define bits32
#define bits16
#endif

typedef struct _msghdr { int msglen bits32;
			 int status bits32;
                         short length bits16;
                         unsigned char nargs;
                         unsigned char descriptor_idx;
                         unsigned char message_id;
			 unsigned char dtype;
                         signed char client_type;
                         unsigned char ndims;
#if defined(__CRAY) || defined(CRAY)
			 long  dims[(MAX_DIMS+1)/2];
#else
                         int  dims[MAX_DIMS];
                         int  fill;
#endif
                       } MsgHdr;

typedef struct _mds_message { MsgHdr h;
                          char bytes[1];
			} Message, *MsgPtr;

typedef struct _io_routines {
  int (*connect) (int conid, char *protocol, char *connectString);
  ssize_t (*send)(int conid, const void *buffer, size_t buflen, int nowait);
  ssize_t (*recv)(int conid, void *buffer, size_t len);
  int (*flush) (int conid);
  int (*listen) ( int argc, char **argv);
  int (*authorize) (int conid, char *username);
  int (*reuseCheck) (char *connectString, char *uniqueString, size_t buflen);
  int (*disconnect) (int conid);
} IoRoutines;

#define EVENTASTREQUEST     "---EVENTAST---REQUEST---"
#define EVENTCANREQUEST     "---EVENTCAN---REQUEST---"

#define LOGINREQUEST        "---LOGIN------REQUEST___"
#define LOGINUSER           "---LOGIN------USER------"
#define LOGINGETP1          "---LOGIN------GETP1-----"
#define LOGINGETP2          "---LOGIN------GETP2-----"
#define LOGINPWD            "---LOGIN------PWD-------"
#define LOGINVMS            "---LOGIN------VMS-------"

#define SENDCAPABILITIES 0xf

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

#define FlipBytes(num,ptr) \
{\
  int __i;\
  int __n = num;\
  char *__p = ptr;\
  for (__i=0;__i<__n/2;__i++)\
  {\
    char __tmp = __p[__i];\
    __p[__i] = __p[__n - __i -1];\
    __p[__n - __i - 1] = __tmp;\
  }\
}
#if (!defined(HAVE_WINDOWS_H) && !defined(HAVE_VXWORKS_H))
#include <pthread.h>
#else
typedef void *pthread_mutex_t;
#endif

EXPORT int AcceptConnection(char *protocol,char *info_name, int readfd, void *info,size_t infolen, int *conid, char **user);
EXPORT short ArgLen(struct descrip *d);
EXPORT int CheckClient(char *, int, char **);
EXPORT char ClientType(void);
EXPORT int CloseConnection(int conid);
EXPORT int ConnectToMds(char *connection_string);
EXPORT int DisconnectConnection(int id);
EXPORT int DisconnectFromMds(int id);
EXPORT int DoMessage(int id);
EXPORT Connection *FindConnection(int id,Connection **prev);
extern void FlipData(Message *m);
extern void FlipHeader(MsgHdr *header);
extern int FlushConnection(int id);
EXPORT void FreeDescriptors(Connection *);
EXPORT void FreeMessage(void *message);
EXPORT int  GetAnswerInfo(int id, char *dtype, short *length, char *ndims, int *dims, int *numbytes, void * *dptr);
EXPORT int  GetAnswerInfoTS(int id, char *dtype, short *length, char *ndims, int *dims, int *numbytes, void * *dptr, void **m);
EXPORT int GetCompressionLevel();
EXPORT void *GetConnectionInfo(int id, char **info_name, int *readfd, size_t *len);
EXPORT IoRoutines *GetConnectionIo(int id);
EXPORT int GetContextSwitching();
EXPORT int GetFlags();
EXPORT char *GetHostfile();
EXPORT int GetMaxCompressionLevel();
EXPORT unsigned char GetConnectionMessageId(int);
EXPORT int GetMdsConnectTimeout();
EXPORT Message *GetMdsMsg(int id, int *status);
EXPORT Message *GetMdsMsgOOB(int id, int *status);
EXPORT unsigned char GetMode();
EXPORT unsigned char GetMulti();
EXPORT char *GetPortname();
EXPORT char *GetProtocol();
EXPORT int GetService();
EXPORT int GetSocketHandle();
EXPORT int GetWorker();
EXPORT unsigned char IncrementConnectionMessageId(int);
EXPORT IoRoutines *LoadIo(char *protocol);
EXPORT void LockAsts();
EXPORT struct descrip *MakeDescrip(struct descrip *in_descrip, char dtype, char ndims, int *dims, void *ptr);
EXPORT struct descrip *MakeDescripWithLength(struct descrip *in_descrip, char dtype, int length, char ndims, int *dims, void *ptr);
EXPORT int MdsClose(int id);
EXPORT void MdsDispatchEvent(int id);
EXPORT int  MdsEventAst(int id, char *eventnam, void (*astadr)(), void *astprm, int *eventid);
EXPORT int MdsEventCan(int id, int eventid);
EXPORT void MdsIpFree(void *ptr);
EXPORT int MdsLogin(int id, char *username, char *password);
EXPORT int MdsOpen(int id,char *tree, int shot);
EXPORT int MdsPut(int id,char *node, char *exp,...);
EXPORT int MdsSetCompression(int id, int level);
EXPORT int MdsSetDefault(int id, char *node);
EXPORT int MdsValue(int id,char *exp,...);
EXPORT int NextConnection(void **ctx,char **info_name, void **info, size_t *info_len);
EXPORT void ParseCommand(int argc, char **argv, Options options[], int more, int *rem_argc, char ***rem_argv);
EXPORT void ParseStdArgs(int argc, char **argv, int *extra_argc, char ***extra_argv);
EXPORT void PrintHelp(char *);

EXPORT int ReuseCheck(char *hostin, char *unique, size_t buflen);
EXPORT int  SendArg(int id, unsigned char idx, char dtype, unsigned char nargs, short length, char ndims, int *dims, char *bytes);
EXPORT int SendMdsMsg(int id, Message *m, int nowait);
void SetConnectionCompression(int conid, int compression);
EXPORT void SetConnectionInfo(int conid, char *info_name, int readfd, void *info, size_t len);
EXPORT int SetCompressionLevel(int setting);
EXPORT int SetContextSwitching(int setting);
EXPORT int SetFlags(int flags);
EXPORT char *SetHostfile(char *newhostfile);
EXPORT int SetMaxCompressionLevel(int setting);
EXPORT int SetMdsConnectTimeout(int sec);
EXPORT unsigned char SetMode(unsigned char newmode);
EXPORT unsigned char SetMulti(unsigned char setting);
EXPORT char *SetPortname(char *);
EXPORT char *SetProtocol(char *);
EXPORT int SetService(int setting);
EXPORT int SetSocketHandle(int handle);
EXPORT int SetWorker(int setting);
EXPORT void UnlockAsts();
EXPORT int   MdsSetCompression(int conid, int level);
EXPORT int   GetConnectionCompression(int conid);
extern int NewConnection(char *protocol);
//Deprecated ipaddr routines
EXPORT int MdsGetClientAddr();
EXPORT void MdsSetClientAddr(int);
EXPORT char *MdsGetServerPortname();

/* MdsIpSrvShr routines */

EXPORT Message *ProcessMessage(Connection *, Message *message);
EXPORT int RemoveConnection(int id);


#endif
