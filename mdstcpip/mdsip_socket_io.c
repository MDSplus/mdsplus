#include "mdsip.h"
#include <STATICdef.h>

int CloseSocket(SOCKET s);
extern int GetBytes(SOCKET sock, char *bptr, int bytes_to_recv, int oob);
extern char ClientType(void);
extern void FlipHeader(MsgHdr *header);
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif
#ifdef GLOBUS
#if defined(__VMS)
#include <descrip.h>
#include <impdef.h>
#include <netdb.h>
#include <ucx$inetdef.h>
    typedef struct SocketParamStruct
        {
        unsigned short protocol;
        unsigned char  type;
        unsigned char  family;
        }    socketParamType, /* For one occurance */
            *socketParamPtr;  /* For a pointer to an occ.*/
#elif defined(WIN32)
#define globus_libc_printf printf
#else
#include <pthread.h>
#if (defined(_DECTHREADS_) && (_DECTHREADS_ != 1)) || !defined(_DECTHREADS_)
#define pthread_attr_default NULL
#define pthread_mutexattr_default NULL
#define pthread_condattr_default NULL
#endif
#include <syslog.h>
#include <sys/param.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/signal.h>
#include <sys/wait.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>
#include <sys/stat.h>
#include "globus_io.h"
/* #include "globus_l_io.h" */

struct handle_struct { globus_io_handle_t handle;
                       int in_use;
                       MsgHdr *header;
                     };
static struct handle_struct iohandles[512];

STATIC_ROUTINE void InitGlobus()
{
  static int initialized = 0;
  int i;
  if (!initialized)
  {
    int status;
    status = globus_module_activate(GLOBUS_COMMON_MODULE);
    globus_assert(status == GLOBUS_SUCCESS);
    status = globus_module_activate(GLOBUS_IO_MODULE);
    globus_assert(status == GLOBUS_SUCCESS);
    initialized = 1;
  }
}

STATIC_ROUTINE globus_io_handle_t *NewHandle(SOCKET *sock) 
{
  int i;
  InitGlobus();
  for (i=0;i<512;i++)
  {
    if (!iohandles[i].in_use)
    {
      *sock = (SOCKET)i+1;
      iohandles[i].in_use=1;
      iohandles[i].header = 0;
      return &iohandles[i].handle;
    }
  }
  return 0;
}

STATIC_ROUTINE globus_io_handle_t *GetHandle(SOCKET sock)
{
  return  ((sock < 512 && sock > 0 && iohandles[sock-1].in_use) ? &iohandles[sock-1].handle : (globus_io_handle_t *)0);
}

STATIC_ROUTINE void ReleaseHandle(SOCKET sock)
{
  if (sock < 512 && sock > 0 && iohandles[sock-1].in_use) iohandles[sock-1].in_use = 0;
}
void ZeroFD() {}
void SetFD(SOCKET sock) {}
void ClearFD(SOCKET sock) {}

#else
/* #include <signal.h> */
#ifndef WIN32
#include <pthread.h>
#if (defined(_DECTHREADS_) && (_DECTHREADS_ != 1)) || !defined(_DECTHREADS_)
#define pthread_attr_default NULL
#define pthread_mutexattr_default NULL
#define pthread_condattr_default NULL
#endif
#else
typedef void *pthread_mutex_t;
typedef int socklen_t;
#define pthread_attr_default NULL
#define pthread_mutexattr_default NULL
#define pthread_condattr_default NULL
#endif
static fd_set fdactive;
void ZeroFD() { FD_ZERO(&fdactive);}
void SetFD(SOCKET sock) { FD_SET(sock,&fdactive);}
void ClearFD(SOCKET sock) { FD_CLR(sock,&fdactive);}
void RegisterRead(SOCKET sock){}
#endif


void SetSocketOptions(SOCKET s, int reuse);
STATIC_ROUTINE void (*AddClient)(SOCKET,void *,char *) = 0;
STATIC_ROUTINE void (*DoMessage)(SOCKET) = 0;

#ifdef GLOBUS

void ReadCallback(void *sock, globus_io_handle_t *handle, globus_result_t result, globus_byte_t *buf, globus_size_t nbytes)
{
  if (result != GLOBUS_SUCCESS)
  {
    globus_libc_printf("Error in ReadCallback:\n\t");
    globus_libc_printf(globus_object_printable_to_string(globus_error_get(result)));
    free(iohandles[(SOCKET)sock-1].header);
    iohandles[(SOCKET)sock-1].header = 0;
  }
  (*DoMessage)((SOCKET)sock);
}

void RegisterRead(SOCKET s)
{
  globus_result_t status;
  globus_io_handle_t *handle = GetHandle(s);
  iohandles[s-1].header = malloc(sizeof(MsgHdr));
  status = globus_io_register_read(handle,(globus_byte_t *)iohandles[s-1].header,sizeof(MsgHdr),sizeof(MsgHdr),ReadCallback,(void *)s);
  if (status != GLOBUS_SUCCESS) 
    CloseSocket(s);
}

char *MGetName(globus_io_handle_t *handle)
{
    gss_ctx_id_t context = 0;
    OM_uint32 minor_status = 0;
    gss_name_t src_name = 0;
    gss_name_t targ_name = 0;
    int lifetime = 0;
    unsigned int status;
    gss_buffer_desc gss_buffer = {0,0};
    gss_OID gss_oid = 0;
    globus_io_tcp_get_security_context(handle,&context);
    gss_inquire_context(&minor_status, context, &src_name, &targ_name, 0, 0,0,0,0);
    gss_display_name(&minor_status,src_name,&gss_buffer,&gss_oid);
    return (char *)gss_buffer.value;
}


void ConnectReceived(void *callback_arg, globus_io_handle_t *listener_handle, globus_result_t result_in)
{
  int pid = getpid();
  SOCKET s;
  int host = 0;
  unsigned short port = 0;
  globus_io_handle_t *handle = NewHandle(&s);
  globus_result_t result;
  globus_io_attr_t attr;
  globus_io_tcp_get_attr(listener_handle,&attr);
  if ((result = globus_io_tcp_accept(listener_handle,&attr,handle)) != GLOBUS_SUCCESS)
  {
    globus_libc_printf("Error accepting client connection:\n\t");
    globus_libc_printf(globus_object_printable_to_string(globus_error_get(result)));
  }
  else
  {
    int host[4];
    int in_host;
    struct sockaddr_in sin;
    gss_ctx_id_t context = 0;
    OM_uint32 minor_status = 0;
    gss_name_t src_name = 0;
    gss_name_t targ_name = 0;
    int lifetime = 0;
    unsigned int status;
    gss_buffer_desc gss_buffer;
    gss_OID gss_oid = 0;
    char name[4096];
    gss_buffer.length=4096;
    gss_buffer.value=name;
    globus_io_tcp_get_security_context(handle,&context);
    gss_inquire_context(&minor_status, context, &src_name, &targ_name, 0, 0,0,0,0);
    gss_display_name(&minor_status,src_name,&gss_buffer,&gss_oid);
    if ((result = globus_io_tcp_get_remote_address(handle,host,&sin.sin_port)) != GLOBUS_SUCCESS)
    {
      globus_libc_printf("Error accepting client connection:\n\t");
      globus_libc_printf(globus_object_printable_to_string(globus_error_get(result)));
    }
    in_host = host[0] | (host[1] << 8) | (host[2] << 16) | (host[3] << 24);
    memcpy(&sin.sin_addr,&in_host,sizeof(in_host));
    (*AddClient)(s,&sin,(char *)gss_buffer.value);
  }
  if (pid == getpid())
  {
#ifdef WIN32
    _sleep(1000);
#else
    sleep(1);
#endif
    globus_io_tcp_register_listen(listener_handle,ConnectReceived,(void *)0);
  }
}

#endif

#ifdef GLOBUS
void Poll(void (*DoMessage_in)(SOCKET s))
{
  if (DoMessage_in != 0)
    DoMessage = DoMessage_in;
  while (1) globus_poll_blocking();
}

STATIC_ROUTINE globus_bool_t AuthenticationCallback(void *arg, globus_io_handle_t *handle, globus_result_t result, char *identity, gss_ctx_id_t *context_handle)
{
/*
  printf("AuthenticationCallback from identity %s\n",identity);
*/
  return 1;
}

#else
fd_set FdActive() { return fdactive; }
#endif

void SetCloseOnExec(SOCKET sock)
{
#ifndef _WIN32
  fcntl(sock,F_SETFD,FD_CLOEXEC);
#endif
}

SOCKET CreateListener(unsigned short port,void (*AddClient_in)(SOCKET,void *,char *), void (*DoMessage_in)(SOCKET s))
{
  SOCKET s;
#ifndef GLOBUS
  struct sockaddr_in sin;
  int one=1;
  int status;
  AddClient = AddClient_in;
  DoMessage = DoMessage_in;
/****** Do this elsewhere *****
  char multistr[] = "MULTI";
  if (multi)
    CheckClient(0,multistr);
******************************/
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == -1)
  {
    printf("Error getting Connection Socket\n");
    exit(1);
  }
  SetCloseOnExec(s);
  FD_SET(s,&fdactive);
  SetSocketOptions(s,1);
  memset(&sin,0,sizeof(sin));
  sin.sin_port = port;
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  status = bind(s, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
  if (status < 0)
  {
    perror("Error binding to service\n");
    exit(1);
  }
  status = listen(s,5);
  if (status < 0)
  {
    printf("Error from listen\n");
    exit(1);
  }
#else
  unsigned short netport = htons(port);
  globus_io_handle_t *handle = NewHandle(&s);
  globus_io_secure_authorization_data_t  auth_data;
  STATIC_CONSTANT int sendbuf=SEND_BUF_SIZE,recvbuf=RECV_BUF_SIZE;
  globus_result_t result;
  globus_io_attr_t attr;
  globus_io_tcpattr_init(&attr);
  globus_io_attr_set_socket_rcvbuf(&attr,recvbuf);
  globus_io_attr_set_socket_sndbuf(&attr,sendbuf);
  globus_io_attr_set_tcp_nodelay(&attr,GLOBUS_TRUE);
  globus_io_attr_set_socket_reuseaddr(&attr,GLOBUS_TRUE);
  globus_io_attr_set_socket_oobinline(&attr,GLOBUS_TRUE);
  globus_io_secure_authorization_data_initialize(&auth_data);
  globus_io_secure_authorization_data_set_callback(&auth_data,AuthenticationCallback,0);
  globus_io_attr_set_secure_authentication_mode(&attr,GLOBUS_IO_SECURE_AUTHENTICATION_MODE_GSSAPI,GSS_C_NO_CREDENTIAL);
  globus_io_attr_set_secure_authorization_mode(&attr,GLOBUS_IO_SECURE_AUTHORIZATION_MODE_CALLBACK,&auth_data);
  globus_io_attr_set_secure_channel_mode(&attr,GLOBUS_IO_SECURE_CHANNEL_MODE_GSI_WRAP);
  globus_io_attr_set_secure_protection_mode(&attr,GLOBUS_IO_SECURE_PROTECTION_MODE_SAFE);
  AddClient = AddClient_in;
  DoMessage = DoMessage_in;
  if ((result = globus_io_tcp_create_listener(&netport,5,&attr,handle)) != GLOBUS_SUCCESS)
  {
    printf("Error in globus_io_tcp_create_listener:\n\t");
    globus_libc_printf(globus_object_printable_to_string(globus_error_get(result)));
    exit(1);
  }
  globus_io_tcpattr_destroy(&attr);
  globus_io_tcp_register_listen(handle,ConnectReceived,(void *)0);
#endif
  return s;
}

void FlushSocket(SOCKET sock)
{
#ifndef GLOBUS
#if !defined(__sparc__) && !((defined(_UCX) || defined(ANET)) && (__CRTL_VER < 70000000))
  struct timeval timout = {0,1};
  int status;
  int nbytes;
  int tries = 0;
  char buffer[1000];
  fd_set readfds, writefds;
  FD_ZERO(&readfds);
  FD_SET(sock,&readfds);
  FD_ZERO(&writefds);
  FD_SET(sock,&writefds);
  while(((((status = select(FD_SETSIZE, &readfds, &writefds, 0, &timout)) > 0) && FD_ISSET(sock,&readfds)) ||
	       (status == -1 && errno == EINTR)) && tries < 10)
  {
    tries++;
    if (FD_ISSET(sock,&readfds))
    {
#ifdef __QNX__
	status = ioctl(sock, FIONREAD, &nbytes);
#else
        status = ioctl(sock,I_NREAD,&nbytes);
#endif
        if (nbytes > 0 && status != -1)
        {
#ifdef HAVE_WINDOWS_H
			nbytes = recv(sock, buffer, sizeof(buffer) > nbytes ? nbytes : sizeof(buffer), 0);
#else
			nbytes = recv(sock, buffer, sizeof(buffer) > nbytes ? nbytes : sizeof(buffer), MSG_NOSIGNAL);
#endif
			if (nbytes > 0) tries = 0;
	}
    }
    else
      FD_SET(sock,&readfds);
    timout.tv_usec = 100000;
    FD_CLR(sock,&writefds);
  }
#endif
#else
  globus_io_handle_t *handle = GetHandle(sock);
  globus_size_t nbytes_read=0;
  globus_byte_t buf[1024];
  int done = 0;
  while(!done)
  {
    globus_io_try_read(handle,buf,sizeof(buf),&nbytes_read);
    done = nbytes_read == 0;
  }  
#endif
}

typedef struct _socket_list {int socket; struct _socket_list *next;} SocketList;

static SocketList *Sockets = 0;

static int socket_mutex_initialized = 0;
static pthread_mutex_t socket_mutex;

static void lock_socket_list()
{

  if(!socket_mutex_initialized)
  {
    socket_mutex_initialized = 1;
#ifdef HAVE_WINDOWS_H
    pthread_mutex_init(&socket_mutex, pthread_mutexattr_default);
#endif
  }
#ifdef HAVE_WINDOWS_H
  pthread_mutex_lock(&socket_mutex);
#endif
}

static void unlock_socket_list()
{

  if(!socket_mutex_initialized)
  {
    socket_mutex_initialized = 1;
#ifdef HAVE_WINDOWS_H
    pthread_mutex_init(&socket_mutex, pthread_mutexattr_default);
#endif
  }
#ifdef HAVE_WINDOWS_H
  pthread_mutex_unlock(&socket_mutex);
#endif
}

static void PushSocket(SocketList *s)
{
  SocketList *oldhead;
  lock_socket_list();
  oldhead=Sockets;
  Sockets=s;
  s->next=oldhead;
  unlock_socket_list();
}

static void PopSocket(SocketList *s_in)
{
  SocketList *p,*s;
  lock_socket_list();
  for (s=Sockets,p=0;s && s!=s_in; p=s,s=s->next);
  if (s)
  {
    if (p)
      p->next = s->next;
    else
      Sockets = s->next;
  }
  unlock_socket_list();
}

static void ABORT(int sigval)
{
  SocketList *s;
  lock_socket_list();
  for (s=Sockets;s;s=s->next) CloseSocket(s->socket);
  unlock_socket_list();
}

int SocketRecv(SOCKET s, char *bptr, int num,int oob)
{
#ifndef GLOBUS
  int num_got=0;
  struct sockaddr sin;
  socklen_t n = sizeof(sin);
  SocketList this_socket = {s,0};
  PushSocket(&this_socket);
  this_socket.socket=s;
#ifndef WIN32
  signal(SIGABRT,ABORT);
#endif
  if (getpeername(s, (struct sockaddr *)&sin, &n)==0)
#ifdef WIN32
	  num=num > 256000 ? 256000 : num;
#endif
    num_got=recv(s,bptr,num,(oob ? MSG_OOB : 0) | MSG_NOSIGNAL);
  PopSocket(&this_socket);
  return num_got;
#else
  int bytes_to_read = num;
  char *ptr = bptr;
  globus_io_handle_t *handle = GetHandle(s);
  globus_result_t status;
  globus_size_t nbytes_read = 0;
  /***************** oob not implemented yet. need documentation on globus_io_recv ***************/
  if (iohandles[s-1].header && (num >= sizeof(MsgHdr)))
  {
      memcpy(ptr,iohandles[s-1].header,sizeof(MsgHdr));
      free(iohandles[s-1].header);
      iohandles[s-1].header = 0;
      bytes_to_read -= sizeof(MsgHdr);
      ptr += sizeof(MsgHdr);
  }
  if (bytes_to_read <= 0)
    return num;
  if ((status = globus_io_read(handle, (globus_byte_t *)ptr, 
           (globus_size_t)bytes_to_read,(globus_size_t)bytes_to_read,&nbytes_read)) == GLOBUS_SUCCESS)
  {
    if (bytes_to_read != nbytes_read) printf("read num=%d nbytes_read=%d\n",num,nbytes_read);
    return((int)num);
  }
  else
  {
    char *errstr = 0;
    errstr = globus_object_printable_to_string(globus_error_get(status));
    fprintf(stderr,"Error reading:\n\t%s",errstr);
    return(-1);
  }
#endif
}   

int SocketSend(SOCKET s, char *bptr, int num, int options)
{
#ifndef GLOBUS
#ifdef WIN32
	num=(num > 256000 ? 256000 : num);
#endif
  return send(s,bptr,num, options | MSG_NOSIGNAL);
#else
  globus_io_handle_t *handle = GetHandle(s);
  globus_size_t nbytes_written = 0;
  globus_result_t status;
  /***************** oob not implemented yet. need documentation on globus_io_send ***************/
  status = globus_io_write(handle, (globus_byte_t *)bptr, (globus_size_t)num,&nbytes_written);
  if (num != nbytes_written) 
    printf("num = %d nbytes_written =  %d\n",num,nbytes_written);
  if (status == GLOBUS_SUCCESS)
    return((int)num);
  else
    return(-1);
#endif
}   


void SetSocketOptions(SOCKET s, int reuse)
{
#ifndef GLOBUS
  STATIC_CONSTANT int sendbuf=SEND_BUF_SIZE,recvbuf=RECV_BUF_SIZE;
  int one = 1;
  unsigned long len;
  static int debug_winsize=0;
  static int init=1;
  if (init)
  {
    char *winsize=getenv("TCP_WINDOW_SIZE");
    if (winsize)
    {
      sendbuf = atoi(winsize);
      recvbuf = atoi(winsize);
    }
    debug_winsize = (getenv("DEBUG_WINDOW_SIZE") != 0);
    init = 0;
  }
  setsockopt(s, SOL_SOCKET,SO_RCVBUF,(char *)&recvbuf,sizeof(int));
  setsockopt(s, SOL_SOCKET,SO_SNDBUF,(char *)&sendbuf,sizeof(int));
  if (debug_winsize)
  {
    getsockopt(s, SOL_SOCKET,SO_RCVBUF,(char *)&recvbuf,&len);
    printf("Got a recvbuf of %d\n",recvbuf);
    getsockopt(s, SOL_SOCKET,SO_SNDBUF,(char *)&sendbuf,&len);
    printf("Got a sendbuf of %d\n",sendbuf);
  }
  setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (void *)&one, sizeof(one));
  if (reuse)
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (void *)&one,sizeof(one));
#endif
}

#ifndef GLOBUS
#if !defined(__VMS) && !defined(_WIN32) && !defined(HAVE_VXWORKS_H)
static struct timeval connectTimer = {0,0};

int SetMdsConnectTimeout(int sec)
{
  int old = connectTimer.tv_sec;
  connectTimer.tv_sec = sec;
  return old;
}
#endif
#endif


SOCKET MConnect(char *host, unsigned short port)
{
  SOCKET s;
  int status;
#ifndef GLOBUS
  struct sockaddr_in sin;
  struct hostent *hp = NULL;
  int addr;
#ifndef HAVE_VXWORKS_H
  hp = gethostbyname(host);
#endif
#ifdef _WIN32
  if ((hp == NULL) && (WSAGetLastError() == WSANOTINITIALISED))
  {
    WSADATA wsaData;
    WORD wVersionRequested;
    wVersionRequested = MAKEWORD(1,1);
    WSAStartup(wVersionRequested,&wsaData);
    hp = gethostbyname(host);
  }
#endif
  if (hp == NULL)
  {
    addr = inet_addr(host);
#ifndef HAVE_VXWORKS_H
    if (addr != 0xffffffff)
    	hp = gethostbyaddr((void *) &addr, (int) sizeof(addr), AF_INET);
#endif
  }
#ifdef HAVE_VXWORKS_H
  if (addr == 0xffffffff)
#else
  if (hp == NULL)
#endif
  {
    printf("Error in MDSplus ConnectToPort: %s unknown\n",host);
    return INVALID_SOCKET;
  }
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == INVALID_SOCKET) return INVALID_SOCKET;
  sin.sin_port = port;
  sin.sin_family = AF_INET;
#if defined( HAVE_VXWORKS_H )
  memcpy(&sin.sin_addr, &addr, sizeof(addr));
#elif defined(ANET)
  memcpy(&sin.sin_addr, hp->h_addr, sizeof(sin.sin_addr));
#else
  memcpy(&sin.sin_addr, hp->h_addr_list[0], hp->h_length);
#endif
#if !defined(__VMS) && !defined(_WIN32) && !defined(HAVE_VXWORKS_H)
  if (connectTimer.tv_sec)
  {
    status = fcntl(s,F_SETFL,O_NONBLOCK);
    status = connect(s, (struct sockaddr *)&sin, sizeof(sin));
    if ((status == INVALID_SOCKET) && (errno == EINPROGRESS))
    {
      fd_set readfds;
      fd_set exceptfds;
      fd_set writefds;
      FD_ZERO(&readfds);
      FD_SET(s,&readfds);
      FD_ZERO(&exceptfds);
      FD_SET(s,&exceptfds);
      FD_ZERO(&writefds);
      FD_SET(s,&writefds);
      status = select(FD_SETSIZE, &readfds, &writefds, &exceptfds, &connectTimer);
      if (status == 0)
      {
        printf("Error in connect to service\n: Timeout on connection\n");
        CloseSocket(s);
        return INVALID_SOCKET;
      }
    }
    if (status == INVALID_SOCKET)
      CloseSocket(s);
    else
      fcntl(s,F_SETFL,0);
  } else
#endif
  status = connect(s, (struct sockaddr *)&sin, sizeof(sin));
  if (status == -1)
  {
    CloseSocket(s);
    s=INVALID_SOCKET;
  }
  SetCloseOnExec(s);
#else
  globus_io_handle_t *handle = NewHandle(&s);
  globus_result_t result;
  globus_io_secure_authorization_data_t  auth_data;
  STATIC_CONSTANT int sendbuf=SEND_BUF_SIZE,recvbuf=RECV_BUF_SIZE;
  globus_io_attr_t attr;
  globus_io_tcpattr_init(&attr);
  globus_io_attr_set_socket_rcvbuf(&attr,recvbuf);
  globus_io_attr_set_socket_sndbuf(&attr,sendbuf);
  globus_io_attr_set_tcp_nodelay(&attr,GLOBUS_TRUE);
  globus_io_attr_set_socket_oobinline(&attr,GLOBUS_TRUE);
  if (host[0]=='_')
  {
    globus_io_secure_authorization_data_initialize(&auth_data);
    /*
    globus_io_secure_authorization_data_set_identity(&auth_data,
							      "/O=Grid/O=National Fusion Collaboratory/OU=MIT/CN=LBNL-MDSplusDataServer");;
    globus_io_attr_set_secure_authorization_mode(&attr,GLOBUS_IO_SECURE_AUTHORIZATION_MODE_IDENTITY,&auth_data);
    */
    globus_io_attr_set_secure_authentication_mode(&attr,GLOBUS_IO_SECURE_AUTHENTICATION_MODE_GSSAPI,GSS_C_NO_CREDENTIAL);
    globus_io_attr_set_secure_authorization_mode(&attr,GLOBUS_IO_SECURE_AUTHORIZATION_MODE_HOST,&auth_data);
    globus_io_attr_set_secure_delegation_mode(&attr,GLOBUS_IO_SECURE_DELEGATION_MODE_FULL_PROXY);
    globus_io_attr_set_secure_channel_mode(&attr,GLOBUS_IO_SECURE_CHANNEL_MODE_GSI_WRAP);
    globus_io_attr_set_secure_protection_mode(&attr,GLOBUS_IO_SECURE_PROTECTION_MODE_SAFE);
  }
  if ((result = globus_io_tcp_connect((host[0] == '_') ? &host[1] : host,htons(port),&attr,handle)) != GLOBUS_SUCCESS)
  {
    printf("Error connecting to server:\n\t");
    globus_libc_printf(globus_object_printable_to_string(globus_error_get(result)));
    ReleaseHandle(s);
    s = INVALID_SOCKET;
  }
#endif
  return s;
}

int CloseSocket(SOCKET s)
{
#ifndef GLOBUS
  shutdown(s,2);
  return (close(s) == 0);
#else
  globus_io_handle_t *handle = GetHandle(s);
  globus_result_t status;
  status = globus_io_close(handle);
  ReleaseHandle(s);
  return (status == GLOBUS_SUCCESS);
#endif
}

#if defined(_WIN32)
#ifdef GLOBUS
Message *GetMdsMsgOOB(SOCKET sock,int *status){return 0;}
#else
Message *GetMdsMsgOOB(SOCKET sock, int *status)
{
  MsgHdr header;
  Message *msg = 0;
  int msglen = 0;
  STATIC_CONSTANT struct timeval timer = {10,0};

  int tablesize = FD_SETSIZE;

  int selectstat=0;
  char last;
  fd_set readfds;
  fd_set exceptfds;
  DWORD oob_data;
  int stat;
  FD_ZERO(&readfds);
  FD_SET(sock,&readfds);
  FD_ZERO(&exceptfds);
  FD_SET(sock,&exceptfds);
  *status = 0;
  selectstat = select(tablesize, 0, 0, &exceptfds, NULL);
  if (selectstat == SOCKET_ERROR) {
      perror("GETMSGOOB select error");
      printf(" errno = %d\n",errno);
	  *status = 0;
	  return 0;
  }
  *status = GetBytes(sock, (char *)&last, 1, 1);
  if (!(*status & 1)) {
      perror("GETMSGOOB first recv error");
      printf("errno = %d\n",errno);
	  return 0;
  }
  stat = 	ioctlsocket(sock,SIOCATMARK,&oob_data);
  if (stat == SOCKET_ERROR) {
      perror("GETMSGOOB IOCTL error");
      printf("errno = %d\n",errno);
	  *status = 0;
	  return 0;
  }

  *status = GetBytes(sock, (char *)&header, sizeof(MsgHdr), 0);
  if (*status & 1) 
  {
	if (Endian(header.client_type) != Endian(ClientType()) )
	  FlipHeader(&header);
#ifdef DEBUG
    printf("msglen = %d\nstatus = %d\nlength = %d\nnargs = %d\ndescriptor_idx = %d\nmessage_id = %d\ndtype = %d\n",
               header.msglen,header.status,header.length,header.nargs,header.descriptor_idx,header.message_id,header.dtype);
    printf("client_type = %d\nndims = %d\n",header.client_type,header.ndims);
#endif
    if (CType(header.client_type) > CRAY_CLIENT || header.ndims > MAX_DIMS)
    {
      *status = 0;
      return 0;
    }  
    msg = malloc(header.msglen);
    msg->h = header;
    *status = GetBytes(sock, msg->bytes, header.msglen - sizeof(MsgHdr)-1, 0);
    msg->bytes[header.msglen - sizeof(MsgHdr) -1] = last;
  }
  if (!(*status & 1) && msg)
    free(msg);
  return (*status & 1) ? msg : 0;
}
#endif
#endif

#if defined(GLOBUS)
#ifdef WIN32
globus_result_t globus_io_tcp_accept_inetd(globus_io_attr_t *attr,   globus_io_handle_t *handle)
{
  return -1;
}
#else
extern globus_result_t   globus_i_io_initialize_handle(globus_io_handle_t *handle,int GLOBUS_IO_HANDLE_TYPE_TCP_CONNECTED);
extern globus_result_t   globus_i_io_copy_tcpattr_to_handle(globus_io_attr_t *attr,globus_io_handle_t *handle);
extern globus_result_t   globus_i_io_setup_nonblocking(globus_io_handle_t *handle);

globus_result_t globus_io_tcp_accept_inetd(globus_io_attr_t *attr,   globus_io_handle_t *handle)
{
  static FILE *   fdin;
  static FILE *   fdout;
  globus_result_t result;
  OM_uint32 major_status = 0;
  OM_uint32 minor_status = 0;
  OM_uint32 ret_flags = 0;
  int       token_status = 0;
  int s=0;
#ifdef __VMS
  static $DESCRIPTOR(INET,"SYS$NET");
  int status = sys$assign(&INET,&s,0,0);
  socketParamType socketParam;
  socketParam.protocol = UCX$C_TCP;
  socketParam.type     = INET_PROTYP$C_STREAM;
  socketParam.family   = UCX$C_AUXS; /* From AUXServer */
  status = sys$qiow( 0,             /* efn.v | 0 */
                          s,     /* chan.v */
                          IO$_SETMODE,   /* func.v */
                          0,         /* iosb.r | 0 */
		          0, 0,          /* astadr, astprm: UNUSED */
		          &socketParam,  /* p1.r Socket creation parameter */
		          0, 0, 0, 0, 0  /* p2, p3, p4, p5, p6 UNUSED */ );
  s = decc$socket_fd(s);
#endif
  /*
  globus_i_io_mutex_lock();
  */
  result = globus_i_io_initialize_handle(handle,GLOBUS_IO_HANDLE_TYPE_TCP_CONNECTED);
  result = globus_i_io_copy_tcpattr_to_handle(attr,handle);
  handle->fd = s;
  handle->state=GLOBUS_IO_HANDLE_STATE_CONNECTED;
  /*
  globus_i_io_mutex_unlock();
  */
  major_status = globus_gss_assist_acquire_cred_ext(&minor_status,
                              0,
                              GSS_C_INDEFINITE,
                              GSS_C_NO_OID_SET,
                              GSS_C_ACCEPT,
                              &handle->securesocket_attr.credential,
                              NULL,
                              NULL);
  if (major_status != GSS_S_COMPLETE)
  {
      globus_gss_assist_display_status(stderr,
                            "GSS failed getting server credentials: ",
                            major_status,
                            minor_status,
                            0);
      exit(0);
  }
  fdin = fdopen(dup(s),"r");
  fdout = fdopen(dup(0),"w");
  setbuf(fdout,NULL);
  major_status = globus_gss_assist_accept_sec_context(&minor_status,
                       &handle->context,
                       handle->securesocket_attr.credential,
						      0,
						      /*
                       client_name,
						      */
                       &ret_flags,
                       NULL,            /* don't need user_to_user */
                       &token_status,
    		       &handle->delegated_credential,
                       globus_gss_assist_token_get_fd,
                       (void *)fdin,
                       globus_gss_assist_token_send_fd,
                       (void *)fdout);
  if (major_status != GSS_S_COMPLETE)
  {
        globus_gss_assist_display_status(stdout,
                "GSS authentication failure ",
                major_status,
                minor_status,
                token_status);
	exit(0);
  }
  if (handle->delegated_credential)
  {
    char *delcname;
    char *x509_delegate;
    minor_status = 0xdee0;
    major_status = gss_inquire_cred(&minor_status,
				    handle->delegated_credential,
				    (gss_name_t *)&delcname,
				    NULL,
				    NULL,
				    NULL);
    if (major_status == GSS_S_COMPLETE)
    {
      if (minor_status == 0xdee1 && delcname)
      {
        char *cp;
        cp = strchr(delcname,'=');
        cp++;
#ifndef _AIX
        unsetenv("X509_USER_KEY");
        unsetenv("X509_USER_CERT");
        unsetenv("X509_USER_PROXY");
#endif
        setenv("X509_USER_PROXY",cp,1);
      }
    }
  }

  result = globus_i_io_setup_nonblocking(handle);
  if(handle->securesocket_attr.channel_mode !=
       GLOBUS_IO_SECURE_CHANNEL_MODE_CLEAR)
  {
	OM_uint32			max_input_size;
		
	major_status =
            gss_wrap_size_limit(&minor_status,
			        handle->context,
			        handle->securesocket_attr.protection_mode ==
                                    GLOBUS_IO_SECURE_PROTECTION_MODE_PRIVATE,
				GSS_C_QOP_DEFAULT,
				1<<30,
				&max_input_size);
	if(major_status != GLOBUS_SUCCESS)
	{
	  return (void *)-1;
	}
	handle->max_wrap_length = (globus_size_t) max_input_size;
		
	globus_fifo_init(&handle->wrapped_buffers);
	globus_fifo_init(&handle->unwrapped_buffers);
  }
  return GLOBUS_SUCCESS;
}
#endif
#endif

int ConnectToInet(unsigned short port,void (*AddClient_in)(SOCKET,void *,char *), void (*DoMessage_in)(SOCKET s))
{
  SOCKET s=-1;
#ifndef GLOBUS
  struct sockaddr_in sin;
  unsigned long n = sizeof(sin);
  int status = 1;
#ifdef _VMS
  status = sys$assign(&INET, &s, 0, 0);
#else
  s=0;
#endif
  if (!(status & 1)) { exit(status);}
  SetFD(s);
  if ((status=getpeername(s, (struct sockaddr *)&sin, &n)) < 0)
  {
    perror("Error getting peer name");
    exit(0);
  }
  AddClient = AddClient_in;
  DoMessage = DoMessage_in;
  (*AddClient)(s,&sin,0);
  return -1;
#else
  globus_io_handle_t *handle = NewHandle(&s);
  globus_io_secure_authorization_data_t  auth_data;
  STATIC_CONSTANT int sendbuf=SEND_BUF_SIZE,recvbuf=RECV_BUF_SIZE;
  globus_io_attr_t attr;
  globus_result_t result = 0;
  int host[4];
  int in_host;
  struct sockaddr_in sin;
  unsigned int status;
  result = globus_io_tcpattr_init(&attr);
  result = globus_io_attr_set_socket_rcvbuf(&attr,recvbuf);
  result = globus_io_attr_set_socket_sndbuf(&attr,sendbuf);
  result = globus_io_attr_set_tcp_nodelay(&attr,GLOBUS_TRUE);
  result = globus_io_attr_set_socket_reuseaddr(&attr,GLOBUS_TRUE);
  result = globus_io_attr_set_socket_oobinline(&attr,GLOBUS_TRUE);
  result = globus_io_secure_authorization_data_initialize(&auth_data);
  result = globus_io_secure_authorization_data_set_callback(&auth_data,AuthenticationCallback,0);
  result = globus_io_attr_set_secure_authentication_mode(&attr,GLOBUS_IO_SECURE_AUTHENTICATION_MODE_GSSAPI,GSS_C_NO_CREDENTIAL);
  result = globus_io_attr_set_secure_authorization_mode(&attr,GLOBUS_IO_SECURE_AUTHORIZATION_MODE_CALLBACK,&auth_data);
  result = globus_io_attr_set_secure_channel_mode(&attr,GLOBUS_IO_SECURE_CHANNEL_MODE_GSI_WRAP);
  globus_io_attr_set_secure_protection_mode(&attr,GLOBUS_IO_SECURE_PROTECTION_MODE_SAFE);
  result = globus_io_tcp_accept_inetd(&attr,handle);
  result = globus_io_tcpattr_destroy(&attr);
  if ((result = globus_io_tcp_get_remote_address(handle,host,&sin.sin_port)) != GLOBUS_SUCCESS)
  {
    globus_libc_printf("Error accepting client connection:\n\t");
    globus_libc_printf(globus_object_printable_to_string(globus_error_get(result)));
  }
  in_host = host[0] | (host[1] << 8) | (host[2] << 16) | (host[3] << 24);
  memcpy(&sin.sin_addr,&in_host,sizeof(host));
  AddClient = AddClient_in;
  DoMessage = DoMessage_in;
  (*AddClient)(s,&sin,MGetName(handle));
#ifndef WIN32
  chown(getenv("X509_USER_PROXY"),getuid(),getgid());
#endif
  return -1;
#endif
}

