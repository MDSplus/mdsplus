/*
../lib/libMdsIpShr.so: undefined reference to `sigemptyset'
../lib/libMdsIpShr.so: undefined reference to `perror'
../lib/libMdsIpShr.so: undefined reference to `gethostbyaddr'
../lib/libMdsIpShr.so: undefined reference to `socket'
../lib/libMdsIpShr.so: undefined reference to `select'
../lib/libMdsIpShr.so: undefined reference to `sigaddset'
../lib/libMdsIpShr.so: undefined reference to `cuserid'
../lib/libMdsIpShr.so: undefined reference to `inet_addr'
../lib/libMdsIpShr.so: undefined reference to `setsockopt'
../lib/libMdsIpShr.so: undefined reference to `gethostbyname'
../lib/libMdsIpShr.so: undefined reference to `getservbyname'
../lib/libMdsIpShr.so: undefined reference to `fcntl'
../lib/libMdsIpShr.so: undefined reference to `sigprocmask'
*/

#include "mdsip.h"
#ifdef GLOBUS
#include "globus_io.h"
struct handle_struct { globus_io_handle_t handle;
                       int in_use;
                       MsgHdr *header;
                     };

static struct handle_struct iohandles[512];

static void InitGlobus()
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

static globus_io_handle_t *NewHandle(SOCKET *sock) 
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

static globus_io_handle_t *GetHandle(SOCKET sock)
{
  return  ((sock < 512 && sock > 0 && iohandles[sock-1].in_use) ? &iohandles[sock-1].handle : (globus_io_handle_t *)0);
}

static void ReleaseHandle(SOCKET sock)
{
  if (sock < 512 && sock > 0 && iohandles[sock-1].in_use) iohandles[sock-1].in_use = 0;
}
void ZeroFD() {}
void SetFD(SOCKET sock) {}
void ClearFD(SOCKET sock) {}

#else
static fd_set fdactive;
void ZeroFD() { FD_ZERO(&fdactive);}
void SetFD(SOCKET sock) { FD_SET(sock,&fdactive);}
void ClearFD(SOCKET sock) { FD_CLR(sock,&fdactive);}
void RegisterRead(SOCKET sock){};
#endif


void SetSocketOptions(SOCKET s, int reuse);
static void (*AddClient)(SOCKET,void *,char *) = 0;
static void (*DoMessage)(SOCKET) = 0;

#ifdef GLOBUS

void ReadCallback(void *socket, globus_io_handle_t *handle, globus_result_t result, globus_byte_t *buf, globus_size_t nbytes)
{
  (*DoMessage)((SOCKET)socket);
}

void RegisterRead(SOCKET s)
{
  globus_result_t status;
  globus_io_handle_t *handle = GetHandle(s);
  iohandles[s-1].header = malloc(sizeof(MsgHdr));
  status = globus_io_register_read(handle,(globus_byte_t *)iohandles[s-1].header,sizeof(MsgHdr),sizeof(MsgHdr),ReadCallback,(void *)s);
  if (status != GLOBUS_SUCCESS) 
  {
    CloseSocket(s);
    exit(0);
  }
}

void ConnectReceived(void *callback_arg, globus_io_handle_t *listener_handle, globus_result_t result)
{
  int dofork = (int)callback_arg;
  int doaccept = 1;
  int dolisten = 1;
  if (dofork)
  {
    int pid = 0;
    signal(SIGCHLD,SIG_IGN);
    pid = fork();
    if (!pid)
    {
      CloseSocket(listener_handle);
      dolisten = 0;
    }
    else
      doaccept = 0;
  }
  if (doaccept)
  {
    SOCKET s;
    int host = 0;
    unsigned short port = 0;
    globus_io_handle_t *handle = NewHandle(&s);
    globus_result_t result;
    globus_io_attr_t attr;
    globus_io_tcp_get_attr(listener_handle,&attr);
    if ((result = globus_io_tcp_accept(listener_handle,&attr,handle)) != GLOBUS_SUCCESS)
    {
      globus_object_t *  err = globus_error_get(result);
      if (globus_object_type_match(
  		GLOBUS_IO_ERROR_TYPE_AUTHENTICATION_FAILED,
	        globus_object_get_type(err)))
      {
	    globus_libc_printf("client: authentication failed\n");
      }
      else if (globus_object_type_match(
		   GLOBUS_IO_ERROR_TYPE_AUTHORIZATION_FAILED,
	           globus_object_get_type(err)))
      {
	    globus_libc_printf("client: authorization failed\n");
      }
      else if (globus_object_type_match(
	           GLOBUS_IO_ERROR_TYPE_NO_CREDENTIALS,
	           globus_object_get_type(err)))
      {
	    globus_libc_printf("client: failure: no credentials\n");
      }
      else
      {
	    globus_libc_printf("client: accept() failed, result=%p, port=%d\n",result,port);
      }
      globus_object_free(err);
    }
    else
    {
/*
      globus_io_tcp_get_remote_address(handle,&host,&port);
*/
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
      AddClient(s,0,(char *)gss_buffer.value);
    }
  }
  if (dolisten)
  {
    if (dofork) sleep(1);
    globus_io_tcp_register_listen(listener_handle,ConnectReceived,(void *)dofork);
  }
}

#endif

#ifdef GLOBUS
void Poll(int shut,int IsWorker,int IsService, SOCKET serverSock)
{
   while (1) globus_poll_blocking();
}
#else
fd_set FdActive() { return fdactive; }
#endif

SOCKET CreateListener(unsigned short port,void (*AddClient_in)(SOCKET,void *,char *), void (*DoMessage_in)(SOCKET s),int dofork)
{
  SOCKET s;
#ifndef GLOBUS
  static struct sockaddr_in sin;
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
  FD_SET(s,&fdactive);
  SetSocketOptions(s,1);
  sin.sin_port = htons(port);
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
  static int sendbuf=SEND_BUF_SIZE,recvbuf=RECV_BUF_SIZE;
  globus_io_attr_t attr;
  globus_io_tcpattr_init(&attr);
  globus_io_attr_set_socket_rcvbuf(&attr,recvbuf);
  globus_io_attr_set_socket_sndbuf(&attr,sendbuf);
  globus_io_attr_set_tcp_nodelay(&attr,GLOBUS_TRUE);
  globus_io_attr_set_socket_reuseaddr(&attr,GLOBUS_TRUE);
  globus_io_secure_authorization_data_initialize(&auth_data);
  globus_io_attr_set_secure_authentication_mode(&attr,GLOBUS_IO_SECURE_AUTHENTICATION_MODE_GSSAPI,GSS_C_NO_CREDENTIAL);
  globus_io_attr_set_secure_authorization_mode(&attr,GLOBUS_IO_SECURE_AUTHORIZATION_MODE_SELF,&auth_data);
  globus_io_attr_set_secure_channel_mode(&attr,GLOBUS_IO_SECURE_CHANNEL_MODE_GSI_WRAP);
  AddClient = AddClient_in;
  DoMessage = DoMessage_in;
  if (globus_io_tcp_create_listener(&netport,5,&attr,handle) != GLOBUS_SUCCESS)
  {
    printf("Error in globus_io_tcp_create_listener\n");
    exit(1);
  }
  globus_io_tcpattr_destroy(&attr);
  globus_io_tcp_register_listen(handle,ConnectReceived,(void *)dofork);
#endif
  return s;
}

void FlushSocket(SOCKET sock)
{
#ifndef GLOBUS
#if !((defined(_UCX) || defined(ANET)) && (__CRTL_VER < 70000000))
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
        status = ioctl(sock,I_NREAD,&nbytes);
        if (nbytes > 0 && status != -1)
        {
          nbytes = recv(sock, buffer, sizeof(buffer) > nbytes ? nbytes : sizeof(buffer), 0);
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

int SocketRecv(SOCKET s, char *bptr, int num,int oob)
{
#ifndef GLOBUS
  return recv(s,bptr,num,oob ? MSG_OOB : 0);
#else
  int bytes_to_read = num;
  char *ptr = bptr;
  globus_io_handle_t *handle = GetHandle(s);
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
  if (globus_io_read(handle, (globus_byte_t *)ptr, (globus_size_t)bytes_to_read,(globus_size_t)bytes_to_read,&nbytes_read) == GLOBUS_SUCCESS)
  {
    if (bytes_to_read != nbytes_read) printf("read num=%d nbytes_read=%d\n",num,nbytes_read);
    return((int)num);
  }
  else
    return(-1);
#endif
}   

int SocketSend(SOCKET s, char *bptr, int num, int oob)
{
#ifndef GLOBUS
  return send(s,bptr,num,oob ? MSG_OOB : 0);
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
  static int sendbuf=SEND_BUF_SIZE,recvbuf=RECV_BUF_SIZE;
  int one = 1;
  setsockopt(s, SOL_SOCKET,SO_RCVBUF,(char *)&recvbuf,sizeof(int));
  setsockopt(s, SOL_SOCKET,SO_SNDBUF,(char *)&sendbuf,sizeof(int));
  setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (void *)&one, sizeof(one));
  if (reuse)
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (void *)&one,sizeof(one));
#endif
}

void SetSocketSecure(SOCKET s,int secure)
{
#ifdef GLOBUS
  globus_io_attr_t attr;
  globus_io_handle_t *handle = GetHandle(s);
  globus_io_tcp_get_attr(handle,&attr);
  globus_io_attr_set_secure_channel_mode(&attr,
    secure ? GLOBUS_IO_SECURE_CHANNEL_MODE_GSI_WRAP
           : GLOBUS_IO_SECURE_CHANNEL_MODE_CLEAR);
  globus_io_tcp_set_attr(handle,&attr);
  globus_io_tcpattr_destroy(&attr);
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


SOCKET Connect(char *host, unsigned short port)
{
  SOCKET s;
  int status;
#ifndef GLOBUS
  static struct sockaddr_in sin;
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
#else
  globus_io_handle_t *handle = NewHandle(&s);
  globus_result_t result;
  globus_io_secure_authorization_data_t  auth_data;
  static int sendbuf=SEND_BUF_SIZE,recvbuf=RECV_BUF_SIZE;
  globus_io_attr_t attr;
  globus_io_tcpattr_init(&attr);
  globus_io_attr_set_socket_rcvbuf(&attr,recvbuf);
  globus_io_attr_set_socket_sndbuf(&attr,sendbuf);
  globus_io_attr_set_tcp_nodelay(&attr,GLOBUS_TRUE);
  if (host[0]=='_')
  {
    globus_io_secure_authorization_data_initialize(&auth_data);
    globus_io_attr_set_secure_authentication_mode(&attr,GLOBUS_IO_SECURE_AUTHENTICATION_MODE_GSSAPI,GSS_C_NO_CREDENTIAL);
    globus_io_attr_set_secure_authorization_mode(&attr,GLOBUS_IO_SECURE_AUTHORIZATION_MODE_SELF,&auth_data);
    globus_io_attr_set_secure_channel_mode(&attr,GLOBUS_IO_SECURE_CHANNEL_MODE_GSI_WRAP);
  }
  if ((result = globus_io_tcp_connect((host[0] == '_') ? &host[1] : host,htons(port),&attr,handle)) != GLOBUS_SUCCESS)
  {
    globus_object_t *  err = globus_error_get(result);
    if (globus_object_type_match(
		GLOBUS_IO_ERROR_TYPE_AUTHENTICATION_FAILED,
	        globus_object_get_type(err)))
    {
	    globus_libc_printf("client: authentication failed\n");
    }
    else if (globus_object_type_match(
		   GLOBUS_IO_ERROR_TYPE_AUTHORIZATION_FAILED,
	           globus_object_get_type(err)))
    {
	    globus_libc_printf("client: authorization failed\n");
    }
    else if (globus_object_type_match(
	           GLOBUS_IO_ERROR_TYPE_NO_CREDENTIALS,
	           globus_object_get_type(err)))
    {
	    globus_libc_printf("client: failure: no credentials\n");
    }
    else
    {
	    globus_libc_printf("client: accept() failed, result=%p, port=%d\n",result,port);
    }
    globus_object_free(err);
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

#if defined(_WIN32) && !defined(GLOBUS)
Message *GetMdsMsgOOB(SOCKET sock, int *status)
{
  MsgHdr header;
  Message *msg = 0;
  int msglen = 0;
  static struct timeval timer = {10,0};

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
    *status = GetBytes(sock, msg->bytes, header.msglen - sizeof(MsgHdr), 0);
  }
  if (!(*status & 1) && msg)
    free(msg);
  return (*status & 1) ? msg : 0;
}
#endif

int ConnectToInet(unsigned short port,void (*AddClient_in)(SOCKET,void *,char *), void (*DoMessage_in)(SOCKET s))
{
  SOCKET s=-1;
#ifndef GLOBUS
  static struct sockaddr_in sin;
  int n = sizeof(sin);
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
  (*AddClient)(s,&sin,0);
  return -1;
#else
  unsigned short netport = htons(port);
  globus_io_handle_t *handle = NewHandle(&s);
  globus_io_secure_authorization_data_t  auth_data;
  static int sendbuf=SEND_BUF_SIZE,recvbuf=RECV_BUF_SIZE;
  globus_io_attr_t attr;
  globus_io_tcpattr_init(&attr);
  globus_io_attr_set_socket_rcvbuf(&attr,recvbuf);
  globus_io_attr_set_socket_sndbuf(&attr,sendbuf);
  globus_io_attr_set_tcp_nodelay(&attr,GLOBUS_TRUE);
  globus_io_attr_set_socket_reuseaddr(&attr,GLOBUS_TRUE);
  globus_io_secure_authorization_data_initialize(&auth_data);
  globus_io_attr_set_secure_authentication_mode(&attr,GLOBUS_IO_SECURE_AUTHENTICATION_MODE_GSSAPI,GSS_C_NO_CREDENTIAL);
  globus_io_attr_set_secure_authorization_mode(&attr,GLOBUS_IO_SECURE_AUTHORIZATION_MODE_SELF,&auth_data);
  globus_io_attr_set_secure_channel_mode(&attr,GLOBUS_IO_SECURE_CHANNEL_MODE_GSI_WRAP);
  globus_io_tcp_posix_convert( 0,&attr,handle);
  AddClient = AddClient_in;
  DoMessage = DoMessage_in;
  (*AddClient)(s,0,0);
  return -1;
#endif
}

