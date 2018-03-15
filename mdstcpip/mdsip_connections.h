#ifndef __MDSIP_H__
#define __MDSIP_H__
#define MdsLib_H
#define _GNU_SOURCE /* glibc2 needs this */
#include <mdsplus/mdsconfig.h>
#include <mdsdescrip.h>
#ifdef _WIN32
 #ifndef __SIZE_TYPE__
 typedef int ssize_t;
 #endif
 #include <winsock2.h>
#else
 #include <sys/types.h>
#endif
#include <ipdesc.h>
#include <mds_stdarg.h>
#include <pthread.h>
#define MDSIP_MAX_ARGS 256
#define MDSIP_MAX_COMPRESS 9

#ifndef NULL
#define NULL (void *)0
#endif

enum _mdsip_client_types {
  VMS_CLIENT = 1,
  IEEE_CLIENT = 2,
  JAVA_CLIENT = 3,
  VMSG_CLIENT = 4,
  CRAY_IEEE_CLIENT = 7,
  CRAY_CLIENT = 8
};


typedef struct _eventinfo {
  char data[12];
  int eventid;
  void (*astadr)(void *, int, char *);
  void *astprm;
} MdsEventInfo;

typedef struct _jeventinfo {
  char data[12];
  char eventid;
} JMdsEventInfo;

typedef struct _eventlist {
  int conid;
  int eventid;
  char jeventid;
  MdsEventInfo *info;
  int info_len;
  struct _eventlist *next;
} MdsEventList;

typedef struct _options {
  char *short_name;
  char *long_name;
  int expects_value;
  int present;
  char *value;
} Options;

typedef struct _connection {
  int id; // unique connection id
  struct _connection *next;
  pthread_cond_t cond;
  char state;
  char *protocol;
  char *info_name;
  void *info;
  size_t info_len;
  void* DBID;
  unsigned char message_id;
  int client_type;
  int nargs;
  struct descriptor *descrip[MDSIP_MAX_ARGS]; // list of descriptors for the message arguments
  struct _eventlist *event;
  void *tdicontext[6];
  int addr;
  int compression_level;
  int readfd;
  struct _io_routines *io;
} Connection;

#define INVALID_CONNECTION_ID 0

#define CON_IDLE	0x00
#define CON_CONNECT	0x01
#define CON_AUTHORIZE	0x02
#define CON_SEND	0x04
#define CON_FLUSH	0x08
#define CON_RECV	0x10
#define CON_SENDARG	0x20
#define CON_USER	0x40
#define CON_DISCONNECT	0x80

#if defined(__CRAY) || defined(CRAY)
int errno = 0;
#define bits32 :32
#define bits16 :16
#else
#define bits32
#define bits16
#endif

///
/// \brief Header of Message structure.
///
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
  long dims[(MAX_DIMS_R + 1) / 2];
#else
  int dims[MAX_DIMS_R];
  int fill;
#endif
} MsgHdr;

///
/// \brief Message structure for passing data through connections
///
typedef struct _mds_message {
  MsgHdr h;
  char bytes[1];
} Message, *MsgPtr;

///
/// \brief Structure for Protocol plugin anchor function
///
/// | function ptr | description                     |
/// |:--------|--------------------------------------|
/// | connect | connects client using connectString  |
/// | send    | send buffer through connection       |
/// | recv    | receive buffer from cocnection       |
/// | flush   | flush pending connection messages    |
/// | listen  | listen for new incoming  connections |
/// | authorize  | authorize client with username    |
/// | reuseCheck |                                   |
/// | disconnect | clear connection instance         |
/// | recv_to    | receive buffer from cocnection with time out |
///
typedef struct _io_routines {
  int (*connect)(Connection* c, char *protocol, char *connectString);
  ssize_t (*send)(Connection* c, const void *buffer, size_t buflen, int nowait);
  ssize_t (*recv)(Connection* c, void *buffer, size_t buflen);
  int (*flush)(Connection* c);
  int (*listen)(int argc, char **argv);
  int (*authorize)(Connection* c, char *username);
  int (*reuseCheck)(char *connectString, char *uniqueString, size_t buflen);
  int (*disconnect)(Connection* c);
  ssize_t (*recv_to)(Connection* c, void *buffer, size_t len, int to_msec);
} IoRoutines;

#define EVENTASTREQUEST "---EVENTAST---REQUEST---"
#define EVENTCANREQUEST "---EVENTCAN---REQUEST---"

#define SENDCAPABILITIES 0xf

#define BigEndian 0x80
#define SwapEndianOnServer 0x40
#define COMPRESSED 0x20
#define SENDCAPABILITIES 0xf
#define LittleEndian 0
#define Endian(c) (c & BigEndian)
#define CType(c) (c & 0x0f)
#define IsCompressed(c) (c & COMPRESSED)
#ifdef NOCOMPRESSION
#define SUPPORTS_COMPRESSION 0
#else
#define SUPPORTS_COMPRESSION 0x8000
#endif
#define SupportsCompression(c) (c & SUPPORTS_COMPRESSION)

#define FlipBytes(num, ptr)                                                    \
  {                                                                            \
    int __i;                                                                   \
    int __n = num;                                                             \
    char *__p = ptr;                                                           \
    for (__i = 0; __i < __n / 2; __i++) {                                      \
      char __tmp = __p[__i];                                                   \
      __p[__i] = __p[__n - __i - 1];                                           \
      __p[__n - __i - 1] = __tmp;                                              \
    }                                                                          \
  }
#if HAVE_PTHREAD_H
#include <pthread.h>
#else
typedef void *pthread_mutex_t;
#endif

////////////////////////////////////////////////////////////////////////////////
///
/// Creates  a  new  connection  instance  using  given  protocol,  if  a  valid
/// connection is instanced.
///
/// Executes:
/// 1. SetConnectionInfo()
/// 2. AuthorizeClient()
/// 3. SetConnectionCompression()
///
///
/// \param protocol the protocol name identifier
/// \param info_name info name passed to SetConnectionInfo()
/// \param readfd input file descriptor i.e. the socket id
/// \param info
/// \param infolen
/// \param conid
/// \param user
/// \return
///
EXPORT int AcceptConnection(char *protocol, char *info_name, int readfd,
                            void *info, size_t infolen, int *conid,
                            char **user);

////////////////////////////////////////////////////////////////////////////////
///
/// Calculate the length of a descriptor data from the instance dtype value.
///
EXPORT short ArgLen(struct descrip *d);

////////////////////////////////////////////////////////////////////////////////
///
EXPORT int CheckClient(char *, int, char **);

////////////////////////////////////////////////////////////////////////////////
///
/// Gets the id of this client machine using internal test function.
/// The client types discovered by this function are the followings:
///
EXPORT char ClientType(void);

////////////////////////////////////////////////////////////////////////////////
///
/// Closes active connection identified by id calling removeConnection() from
/// the server "MdsIpSrvShr" library.
///
EXPORT int CloseConnection(int conid);

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Remote mdsip server connection.
///
/// The \em hostin input argument are parsed to find host name protocol and
/// port to connect to. Then a connection structure with the above properties
/// is instaced and added to the connection list calling \ref NewConnection().
/// Once the connection is established the \em connect function from protocol
/// IoRoutines is called. To log into the remote sever the system login user
/// and passwd is used. The compression level of the new connection is set to
/// the client value ( not the server one ) as the client may have to make
/// tuning.
///
/// The connection address follows this syntax: \em host:8000::/dir/subdir/file
/// see ParseHost() for further details
///
/// \return The id of the new connection instanced if success or -1 otherwise.
///
EXPORT int ConnectToMds(char *connection_string);

////////////////////////////////////////////////////////////////////////////////
///
/// Disconnect the connection identified by id.
///
/// \param id the id of connection to be disconnected
/// \return true if the connection was correctly freed or false otherwise.
///
EXPORT int DisconnectConnection(int id);

////////////////////////////////////////////////////////////////////////////////
///
/// calls DisconnectConnection for the connection id.
///
EXPORT int DisconnectFromMds(int id);

////////////////////////////////////////////////////////////////////////////////
///
/// Process one pending message for the connection id at server side calling
/// ProcessMessage function symbol in MdsIpSrvShr library. This at first waits
/// for incoming message with GetMdsMsg() then calls ProcessMessage() that
/// execute the TDI expression, and finally returns the answer back to the
/// client with a SendMdsMsg() call. If any error occurrs within transaction
/// the connection is closed.
///
/// \param id of the connection to use
/// \return the status of the Process message.
///
EXPORT int DoMessage(int id);

////////////////////////////////////////////////////////////////////////////////
///
/// Finds a Connection structure inside the static defined list ConnectionList.
/// \note The connection list is locked during operation.
///
/// \param id the Connection id to be searched for
/// \param prev if prev is set the Connection found is copied inside prev
///        instance
/// \return the Connection intance identified by id or NULL pointer of not found
///
EXPORT Connection *FindConnection(int id, Connection **prev);

extern void FlipData(Message *m);
extern void FlipHeader(MsgHdr *header);

////////////////////////////////////////////////////////////////////////////////
///
/// Free all desctriptors held by the Connection structure in argument.
/// \param c the connection to be freed
///
EXPORT void FreeDescriptors(Connection *c);

////////////////////////////////////////////////////////////////////////////////
///
/// Release message memory
///
EXPORT void FreeMessage(void *message);

////////////////////////////////////////////////////////////////////////////////
///
/// Waits for the server reply as \ref Message holding a descriptor. Then it
/// fills the descriptor info such as dtype, length, ndims and numbytes passed
/// as a reference to the arguments. Usually this function is called by client
/// that waits for the answer of an expression evaluation passed using
/// SendArg()
///
/// \note The output message is kept statically inside function so this code is
/// **NOT thread safe**.
///
/// \param id the id of the connection to use
/// \param dtype pointer to store the descriptor dtype info
/// \param length pointer to store the descriptor length
/// \param ndims pointer to store the descriptor dimensions number
/// \param dims pointer to store the descriptor dimensions
/// \param numbytes pointer to store the descriptor total number of bytes
/// \param dptr pointer to store the descriptor data
/// \return the function returns the status held by the answered descriptor.
///
EXPORT int GetAnswerInfo(int id, char *dtype, short *length, char *ndims,
                         int *dims, int *numbytes, void **dptr);


////////////////////////////////////////////////////////////////////////////////
///
/// Thread Safe version of GetAnswerInfo(), waits for the server reply as \ref
/// Message holding a descriptor. Then it fills the descriptor info such as
/// dtype, length, ndims and numbytes passed as a reference to the arguments.
/// Usually this function is called by client that waits for the answer of an
/// expression evaluation passed using SendArg()
///
/// \param id the id of the connection to use
/// \param dtype pointer to store the descriptor dtype info
/// \param length pointer to store the descriptor length
/// \param ndims pointer to store the descriptor dimensions number
/// \param dims pointer to store the descriptor dimensions
/// \param numbytes pointer to store the descriptor total number of bytes
/// \param dptr pointer to store the descriptor data
/// \param m a pointer to the Message structure to store the incoming message
/// \return the function returns the status held by the answered descriptor
///
EXPORT int GetAnswerInfoTS(int id, char *dtype, short *length, char *ndims,
                           int *dims, int *numbytes, void **dptr, void **m);
EXPORT int GetAnswerInfoTO(int id, char *dtype, short *length, char *ndims,
                           int *dims, int *numbytes, void **dptr, void **m, int timeout);

////////////////////////////////////////////////////////////////////////////////
///
/// Get current compression level
///
EXPORT int GetCompressionLevel();

////////////////////////////////////////////////////////////////////////////////
///
EXPORT void *GetConnectionInfoC(Connection* c, char **info_name, int *readfd,
                               size_t *len);
EXPORT void *GetConnectionInfo(int id, char **info_name, int *readfd,
                               size_t *len);

////////////////////////////////////////////////////////////////////////////////
///
/// \brief GetConnectionIo finds the Connection structure in ConnectionList and
/// returns the ioRoutines structure associated to a given Connection
/// identified by id.
///
/// \param id id of the connection that holds the ioRoutines
/// \return ioRoutines structure in the io field of Connection element found.
///
EXPORT IoRoutines *GetConnectionIo(int id);

EXPORT int GetContextSwitching();

EXPORT int GetFlags();

EXPORT char *GetHostfile();
EXPORT char *GetLogDir();
EXPORT int GetMaxCompressionLevel();

////////////////////////////////////////////////////////////////////////////////
///
/// Finds connection by id and returns its the message id
///
/// \param id the connection id
/// \return message_id field of selected connection or 0 as no connection found
///
EXPORT unsigned char GetConnectionMessageId(int id);

EXPORT int GetMdsConnectTimeout();

////////////////////////////////////////////////////////////////////////////////
///
/// Wait for an incoming message from the server using the connection id. This
/// recursively calls the proper protocol recv function reserving a message
/// fuffer for the result. A message structure is configured and returned to
/// the user while a status of the receive operation is written inside status
/// instance.
///
/// \param id id of the connection to be used
/// \param status writes out the exit status to the pointed instace
/// \return returns a \ref Message structure filled by content of the response
///
EXPORT Message *GetMdsMsg(int id, int *status);

////////////////////////////////////////////////////////////////////////////////
///
/// Wait for an incoming message from the server using the connection id. This
/// recursively calls the proper protocol recv function reserving a message
/// fuffer for the result. A message structure is configured and returned to
/// the user while a status of the receive operation is written inside status
/// instance.
///
/// \param id id of the connection to be used
/// \param status writes out the exit status to the pointed instace
/// \return returns a \ref Message structure filled by content of the response
///
EXPORT Message *GetMdsMsgTO(int id, int *status, int timeout);
EXPORT Message *GetMdsMsgOOB(int id, int *status);
Message *GetMdsMsgTOC(Connection* c, int *status, int to_msec);

EXPORT unsigned char GetMode();

////////////////////////////////////////////////////////////////////////////////
///
/// Get multi mode active in this scope. Mutiple connection mode (accepts
/// multiple connections each with own context)
///
EXPORT unsigned char GetMulti();

EXPORT char *GetPortname();

EXPORT char *GetProtocol();

EXPORT int GetService();

EXPORT int GetSocketHandle();

EXPORT int GetWorker();

////////////////////////////////////////////////////////////////////////////////
///
/// Finds the Connection intance held in the list of connections by id and
/// increments the connection message id.
///
/// \param id id of the connection
/// \return the incremented connection message id or 0 if connection was not
///         found.
///
EXPORT unsigned char IncrementConnectionMessageId(int id);

////////////////////////////////////////////////////////////////////////////////
///
/// This is the dynamic protocol loader. The mdsshr lib routines are used to
/// seek Io symbol inside a library that must be reachable and named as
/// "{protocol}MdsIp" See \ref IoRoutines to get the list of declared funcion
/// pointers.
///
///\return pointer to the IoRoutines static instance compiled inside the
///        protocol library.
///
EXPORT IoRoutines *LoadIo(char *protocol);

////////////////////////////////////////////////////////////////////////////////
///
/// Locks AST mutex
///
EXPORT void LockAsts();

EXPORT struct descrip *MakeDescrip(struct descrip *in_descrip, char dtype,
                                   char ndims, int *dims, void *ptr);
EXPORT struct descrip *MakeDescripWithLength(struct descrip *in_descrip,
                                             char dtype, int length, char ndims,
                                             int *dims, void *ptr);

////////////////////////////////////////////////////////////////////////////////
///
/// Executes the TDI expression "TreeClose()" on the server throug the
/// connection identified by id.
///
/// \param id the id of the connection to be used
/// \return the status of the TDI expression evaluation
///
EXPORT int MdsClose(int id);

////////////////////////////////////////////////////////////////////////////////
///
EXPORT void MdsDispatchEvent(int id);

////////////////////////////////////////////////////////////////////////////////
///
/// Send an AST (Asyncronous System Trap) request to the server through a
/// special TDI call defined as \ref EVENTASTREQUEST macro.
///
/// \param id the connection id to be used
/// \param eventnam name of the event request
/// \param astadr address of the AST handler
/// \param astprm
/// \param eventid
/// \return
///
EXPORT int MdsEventAst(int id, char *eventnam, void (*astadr)(), void *astprm,
                       int *eventid);

////////////////////////////////////////////////////////////////////////////////
///
EXPORT int MdsEventCan(int id, int eventid);


////////////////////////////////////////////////////////////////////////////////
///
EXPORT void MdsIpFree(void *ptr);

////////////////////////////////////////////////////////////////////////////////
///
EXPORT int MdsLogin(int id, char *username, char *password);

////////////////////////////////////////////////////////////////////////////////
///
/// Opens a MDSplus tree parsefile identified by tree name and shot number.
/// The actual implementation relies on the MdsValue() of the TDI expression
/// "TreeOpen($1,$2) with name and shot arguments.
///
/// \param id the id of the connection to use
/// \param tree the tree name to be opened
/// \param shot the shot id
/// \return status if evaluation was succesfull this is the exit status of the
/// TreeOpen command converted to int, the status of MdsValue() is returned
/// otherwise.
///
EXPORT int MdsOpen(int id, char *tree, int shot);

////////////////////////////////////////////////////////////////////////////////
///
/// Variadic argument function to \em put a TDI expression result into a remote
/// tree node. This calls the TreePut($) command on server side with the sent
/// expression string and arguments passed as descriptors through the
/// connection
///
/// \param id the id of the used connection in connection list.
/// \param node the node path of the remote target tree.
/// \param exp the TDI expression to execute on server.
/// \return the exit status of the remote command converted to int or the status
///         of SendArg() function if it fails.
///
EXPORT int MdsPut(int id, char *node, char *exp, ...);

////////////////////////////////////////////////////////////////////////////////
///
/// Sets the compression level on the client side and the server through the
/// connection id calling the remote execution of MdsSetCompression TDI
/// internal function
///
/// \return the old set value of this connection.
///
EXPORT int MdsSetCompression(int id, int level);

////////////////////////////////////////////////////////////////////////////////
///
/// Sets the defauld node from which the relative path computation is
/// performed. The TreeSetDefault($) expression is evaluated via MdsValue() and
/// the relative exit status is returned.
///
/// \param id of the connection to use
/// \param node the node name of the remote tree to be set as the default node
/// \return id if the expression evaluation was succesfull returns the exit
/// status, the MdsValue() status is returned othewise.
///
EXPORT int MdsSetDefault(int id, char *node);

////////////////////////////////////////////////////////////////////////////////
///
/// Executes a TDI expression inside the server through the selected
/// connection. Variadic arguments must be descriptors and are passed as
/// argument to the expression.
///
/// \param id the id of the connection to use
/// \param exp the TDI expression c string to be avaluated.
/// \return the evaluation exit status of the expression.
///
EXPORT int MdsValue(int id, char *exp, ...);

EXPORT int NextConnection(void **ctx, char **info_name, void **info,
                          size_t *info_len);

EXPORT void ParseCommand(int argc, char **argv, Options options[], int more,
                         int *rem_argc, char ***rem_argv);

EXPORT void ParseStdArgs(int argc, char **argv, int *extra_argc,
                         char ***extra_argv);

EXPORT void PrintHelp(char *);

EXPORT int ReuseCheck(char *hostin, char *unique, size_t buflen);

////////////////////////////////////////////////////////////////////////////////
///
/// This function can be used to send data form a mds descriptor. It was
/// designed to pass descriptors arguments of a TDI command. It accepts the
/// data descriptor info values as input arguments, then a Message is compiled
/// with these informations and the proper connection message id. The
/// connection message_id is read from idx field and written inside message
/// header It has to be incremented only if this is the first command argument
/// passed.. this aims to tag with the same message_id all the arguments that
/// belongs to the same command. The idx field can also tag for special io
/// commands (MDS_IO_xxx) identified by checking (idx > nargs) in \ref
/// ProcessMessage() function. This is used to add distribute client access to
/// parse files. The function SendMdsMsg() is used to format message memory and
/// actually send it through the connection.
///
///
/// \param id the id of the instanced connection in the connections list
/// \param idx the id of the descriptor argument in a evaluated expression
/// \param dtype the descriptor type code
/// \param nargs number of descriptor arguments
/// \param length data type length of a single element in descriptor.
/// \param ndims number of dimensions of the descriptor data
/// \param dims dimensions of descriptor data
/// \param bytes pointer to descriprot memory
/// \return the exit status of SendMdsMsg() that is true if the message was
/// succesfully sent or false otherwise.
///
EXPORT int SendArg(int id, unsigned char idx, char dtype, unsigned char nargs,
                   unsigned short length, char ndims, int *dims, char *bytes);

////////////////////////////////////////////////////////////////////////////////
///
/// This is the main function that is called by user or any higher level
/// functions to send a message through an instanced network connection. The
/// message buffer is set as the proper memory endianess of the client to make
/// the transaction architectural independent. The compression is also applied
/// to the buffer reading the value of compression level from
/// GetCompressionLevel() output. The compression is done using a version of
/// zlib library embedded in the code.
///
/// The message buffer is then sent using the send function of the ioRoutine
/// set in the connection instance. Socket options can be set from socket flags
/// in socket.h such as MSG_OOB and MSG_NOWAIT, if no flags are present
/// (msg_options == 0) the connection is pre-flushed. In case of a transmission
/// failure caused by system interrupt (EINTR) the send is repeated up to 10
/// times before to attempt a Disconnection of the channel and prompting an
/// error message.
///
///
/// \param id used to find the connection and attached connection routine
/// \param m the message buffer to be sent
/// \param msg_options socket messags FLAGS
/// \return true if the message was succesfully sent or false otherwise.
///
EXPORT int SendMdsMsg(int id, Message *m, int msg_options);
int SendMdsMsgC(Connection* c, Message *m, int msg_options);

////////////////////////////////////////////////////////////////////////////////
///
/// Sets connection info to a \ref Connection structure identified by id.
/// For example the TCP routine "tcp_connect" of ioRoutinesTcp.c uses this
/// function as the following: SetConnectionInfo(conid, "tcp", soket_id, 0, 0);
///
/// \param conid the connection id to set the info to.
/// \param info_name the name associated to the info field of \ref Connection
/// \param readfd the file descriptor associated to the Connection
/// \param info accessory info descriptor
/// \param len length of accessory info descriptor
///
EXPORT void SetConnectionInfo(int conid, char *info_name, int readfd, void *info, size_t len);
EXPORT void SetConnectionInfoC(Connection* c, char *info_name, int readfd, void *info, size_t len);

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

////////////////////////////////////////////////////////////////////////////////
///
/// Finds the Connection intance held in the list of connections by id and sets
/// the compression level indicated in arguments. Compression level spans from
/// 0 (no compression) to \ref GetMaxCompressionLevel (maximum compression
/// level usually the integer value 9)
///
/// \param conid id of the Connection to set the compression
/// \param compression the compression level to set
///
EXPORT void SetConnectionCompression(int conid, int compression);

EXPORT int MdsSetCompression(int conid, int level);

////////////////////////////////////////////////////////////////////////////////
///
/// Finds the Connection intance held in the list of connections by id and get
/// the value of compression level stored inside. Compression level spans from
/// 0 (no compression) to \ref GetMaxCompressionLevel (maximum compression
/// level usually the integer value 9)
///
/// \param conid the Connection structure id to query.
/// \return the compression level value found or 0 (no compression) if
/// Connection was not found
///
EXPORT int GetConnectionCompression(int conid);

////////////////////////////////////////////////////////////////////////////////
/// 
/// Creates new Connection instance, the proper ioRoutines is loaded selecting
/// the protocol from input argument. The created Connection is added to the
/// static ConnectionList using a thread safe lock. The new Connection is
/// initialized with the next connection id and the assigned protocol.
///
/// \param protocol the protocol to be used for this Connection
/// \return The new instanced connection id or -1 if error occurred.
///

Connection* NewConnectionC(char *protocol);
void DisconnectConnectionC(Connection* c);
unsigned char IncrementConnectionMessageIdC(Connection* c);
int AddConnection(Connection* c);

Connection *FindConnectionWithLock(int id, char state);
void UnlockConnection(Connection* c);

EXPORT int SendToConnection(int id, const void *buffer, size_t buflen, int nowait);
EXPORT int FlushConnection(int id);
EXPORT int ReceiveFromConnection(int id, void *buffer, size_t buflen);

// Deprecated ipaddr routines
EXPORT int MdsGetClientAddr();
EXPORT void MdsSetClientAddr(int);
EXPORT char *MdsGetServerPortname();

/* MdsIpSrvShr routines */

EXPORT Message *ProcessMessage(Connection *connection, Message *message);
EXPORT int RemoveConnection(int id);

#endif
