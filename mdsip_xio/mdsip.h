#include "globus_xio.h"
#include <ipdesc.h>

#define MAX_DIMS       7
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

#define LOGINREQUEST        "---LOGIN------REQUEST___"
#define LOGINUSER           "---LOGIN------USER------"
#define LOGINGETP1          "---LOGIN------GETP1-----"
#define LOGINGETP2          "---LOGIN------GETP2-----"
#define LOGINPWD            "---LOGIN------PWD-------"
#define LOGINVMS            "---LOGIN------VMS-------"

#if defined(__VMS) || defined(WIN32) || defined(__linux__) || defined(_NO_SIGHOLD)
#define sighold(arg)
#define sigrelse(arg)
#endif

#if defined(__CRAY) || defined(CRAY)
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

typedef struct _eventlist {
  void         *io_handle;
  int          eventid;
  char           jeventid;
  MdsEventInfo  *info;
  int		  info_len;
  struct _eventlist *next;
  struct _mdsip_client_t    *client;
} MdsEventList;

typedef struct _msghdr_t { int msglen bits32;
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
                       } mdsip_message_header_t;

typedef struct _mdsip_message_t { mdsip_message_header_t h;
                          char bytes[1];
			} mdsip_message_t;


#define MAX_ARGS 256

typedef struct _mdsip_options_t
{
  char *port_name;
  int  port;
  char *hostfile;
  int  security_level;
  int  shared_ctx;
  int  rcvbuf;
  int  sndbuf;
  int  delegation;
  void *gsi_driver;
  void *tcp_driver;
  char server_type;
} mdsip_options_t;

typedef struct _mdsip_client_t { 
  mdsip_message_t *message;
  mdsip_message_header_t  header;
  char    *send_buffer;
  char    *local_user;
  char    *remote_user;
  char    *host;
  char    *ipaddr;
  unsigned char message_id;
  int     client_type;
  int     nargs;
  struct descriptor *descrip[MAX_ARGS];
  void    *tree;
  void    *tdicontext[6];
  int     addr;
  mdsip_options_t *options;
  int     uid_set;
  void (*mdsip_read_cb)();
  MdsEventList *event;
} mdsip_client_t;


typedef struct _mdsip_event_info_t { 
  char data[12];
  int eventid;
  void (*astadr)(void *, int, char *);
  void *astprm;
} mdsip_event_info_t;

/********** Function Prototypes ***************/

extern int  mdsip_parse_command(int argc, char **argv, mdsip_options_t *options);
extern char mdsip_client_type();
extern int  mdsip_find_user_mapping(char *hostfile, char *ipaddr, char *host, char *name, char **local_account);
extern char *mdsip_get_remote_host(mdsip_client_t *ctx);
extern char *mdsip_get_remote_ipaddr(mdsip_client_t *ctx);
extern char *mdsip_get_remote_name(mdsip_client_t *ctx);
extern char *mdsip_current_time();
extern void mdsip_initialize_io(mdsip_options_t *options);
extern int  mdsip_listen(void *io_handle, mdsip_options_t *options);
extern mdsip_client_t *mdsip_new_client(mdsip_options_t *options);
extern int  mdsip_authenticate_client(mdsip_client_t *ctx);
extern int  mdsip_become_user(char *local_user, char *remote_user);
extern void mdsip_do_message_cb();
extern void mdsip_io_wait();
extern void mdsip_free_client(mdsip_client_t *ctx);
extern void mdsip_process_message(void *io_handle, mdsip_client_t *c, mdsip_message_t *message);
extern void mdsip_accept_cb();
extern void mdsip_close_cb();
extern void mdsip_open_cb();
extern void mdsip_read_header_cb();
extern void mdsip_authenticate_cb();
void mdsip_test_result(globus_xio_handle_t xio_handle,globus_result_t res,mdsip_client_t *ctx, char *msg);
extern int mdsip_write(void *io_handle, mdsip_client_t *ctx, mdsip_message_t *m, int oob);

extern void *mdsip_connect(char *host);
extern void mdsip_test_status(globus_xio_handle_t xio_handle,globus_result_t res,char *msg);
extern void mdsip_flip_header(mdsip_message_header_t *header);
extern void mdsip_flip_data(mdsip_message_t *msg);
extern mdsip_message_t *mdsip_get_message(void *io_handle, int *status);
extern int mdsip_send_message(void *io_handle, mdsip_message_t *m, int oob);
extern int  mdsip_send_arg(void *io_handle, unsigned char idx, char dtype, unsigned char nargs, short length, char ndims,
			   int *dims, char *bytes);
extern int mdsip_host_to_ipaddr(char *host, int *addr, short *port);
extern int mdsip_get_result(void *io_handle, char *dtype, short *length, char *ndims, 
			    int *dims, int *numbytes, void * *dptr, void **m);
extern struct descrip *mdsip_make_descrip(struct descrip *in_descrip, char dtype, int length, char ndims, int *dims, void *ptr);
extern int mdsip_value(void *io_handle, char *expression, ...);  /**** NOTE: NULL terminated argument list expected ****/
extern int mdsip_put(void *io_handle, char *node, char *expression, ...);  /**** NOTE: NULL terminated argument list expected ****/
extern short mdsip_arglen(struct descrip *d);
extern int  mdsip_open(void *io_channel, char *tree, int shot);
extern int  mdsip_close(void *io_handle);
extern int  mdsip_set_default(void *io_handle, char *node);
extern int  mdsip_event_ast(void *io_channel, char *eventnam, void (*astadr)(), void *astprm, int *eventid);
extern int  mdsip_event_can(void *io_channel, int eventid);
extern void mdsip_dispatch_event(void *io_handle);
extern void MdsSetServerPortname(char *portname);
extern void MdsSetClientAddr(int addr);
extern char *MdsGetServerPortname();
extern int MdsGetClientAddr();






