#include "mdsip.h"
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifdef WIN32
#include <io.h>
#else
#ifndef HAVE_VXWORKS_H
#include <pwd.h>
#endif
#endif

static unsigned char message_id = 1;
#ifdef NOCOMPRESSION
static int CompressionLevel = 0;
#define compress2(a,b,c,d,e) -1
#define uncompress(a,b,c,d) -1
#else
static int CompressionLevel = 0;
extern int compress2();
extern int uncompress();
#endif
Message *GetMdsMsg(SOCKET sock, int *status);
#ifdef _WIN32
Message *GetMdsMsgOOB(SOCKET sock, int *status);
#endif
int SendMdsMsg(SOCKET sock, Message *m, int oob);
int  GetAnswerInfoTS(SOCKET sock, char *dtype, short *length, char *ndims, int *dims, int *numbytes, void * *dptr, Message **m);

extern void SetSocketOptions(SOCKET s, int reuse);
static int initialized = 0;
void FlipHeader(MsgHdr *header);
static void FlipData(Message *m);
extern int SocketSend(SOCKET s, char *bptr, int num, int oob);
extern int SocketRecv(SOCKET s, char *bptr, int num,int oob);
extern int CloseSocket(SOCKET s);
extern SOCKET MConnect(char *host, unsigned short port);
extern void FlushSocket(SOCKET sock);

#if defined( _UCX) || defined(_WIN32)
#else
extern int inet_addr();
#endif

#ifdef __VMS
extern int MdsDispatchEvent();
#endif
static int SendBytes(SOCKET sock, char *bptr, int bytes_to_send, int oob)
{
  int tries = 0;
  while ((bytes_to_send > 0) && (tries < 10))
  {
	int bytes_sent;
    int bytes_this_time = min(bytes_to_send,BUFSIZ);
    bytes_sent = SocketSend(sock, bptr, bytes_to_send, oob ? MSG_OOB : 0);
    if (bytes_sent <= 0)
    {
      if (errno != EINTR)
        return 0;
	  tries++;
    }
    else
    {
      bytes_to_send -= bytes_sent;
      bptr += bytes_sent;
	  tries = 0;
    }
  }
  if (tries >= 10)
  {
    CloseSocket(sock);
    fprintf(stderr,"\rSendBytes shutdown socket %d",sock);
    return 0;
  }
  return 1;
}

int GetBytes(SOCKET sock, char *bptr, int bytes_to_recv, int oob)
{
  int tries = 0;
  while (bytes_to_recv > 0 && (tries < 10))
  {
    int bytes_recv;
    int bytes_this_time = min(bytes_to_recv,BUFSIZ);
    bytes_recv = SocketRecv(sock, bptr, bytes_to_recv, oob ? MSG_OOB : 0);
    if (bytes_recv <= 0)
    {
      if (errno != EINTR)
        return 0;
      tries++;
    }
    else
    {
      tries = 0;
      bytes_to_recv -= bytes_recv;
      bptr += bytes_recv;
    }
  }
  if (tries >= 10)
  {
    CloseSocket(sock);
    fprintf(stderr,"\rGetBytes shutdown socket %d: too many EINTR's",sock);
    return 0;
  }
  return 1;
}

int SetCompressionLevel(int level)
{
  int old_level = CompressionLevel;
  CompressionLevel = level;
  return old_level;
}

int GetCompressionLevel()
{
  return CompressionLevel;
}

char ClientType(void)
{
  static char ctype = 0;
  if (!ctype)
  {
    union { int i bits32;
            char  c[sizeof(double)];
            float x;
            double d;
          } client_test;
    client_test.x = 1.;
    if (client_test.i == 0x4080) {
      client_test.d=12345678;
      if(client_test.c[5])
       ctype = VMSG_CLIENT;
      else
      ctype = VMS_CLIENT;
    }
    else if (client_test.i == 0x3F800000)
    {
      if (sizeof(int) == 8)
        ctype = CRAY_IEEE_CLIENT;
      else
        ctype = IEEE_CLIENT;
    }
    else
      ctype = CRAY_CLIENT;
    client_test.i = 1;
    if (!client_test.c[0]) ctype |= BigEndian;
  }
  return ctype;
}

struct descrip *MakeDescripWithLength(struct descrip *in_descrip, char dtype, int length, char ndims, int *dims, void *ptr)
{
  int i;
  in_descrip->dtype = dtype;
  in_descrip->ndims = ndims;
  in_descrip->length = length;
  for (i=0;i<ndims;i++) in_descrip->dims[i] = dims[i];
  for (i=ndims; i<MAX_DIMS; i++) in_descrip->dims[i] = 0;
  in_descrip->ptr = ptr;
  return in_descrip;
}

struct descrip *MakeDescrip(struct descrip *in_descrip, char dtype, char ndims, int *dims, void *ptr)
{
  int i;
  in_descrip->dtype = dtype;
  in_descrip->ndims = ndims;
  in_descrip->length = 0;
  for (i=0;i<ndims;i++) in_descrip->dims[i] = dims[i];
  for (i=ndims; i<MAX_DIMS; i++) in_descrip->dims[i] = 0;
  in_descrip->ptr = ptr;
  return in_descrip;
}

short ArgLen(struct descrip *d)
{
  short len;
  switch (d->dtype)
  {
    case DTYPE_CSTRING :  len = d->length ? d->length : (d->ptr ? strlen(d->ptr) : 0); break;
    case DTYPE_UCHAR   :
    case DTYPE_CHAR    :  len = sizeof(char); break;
    case DTYPE_USHORT  :
    case DTYPE_SHORT   :  len = sizeof(short); break;
    case DTYPE_ULONG   :  
    case DTYPE_LONG    :  len = sizeof(int); break;
    case DTYPE_FLOAT   :  len = sizeof(float); break;
    case DTYPE_DOUBLE  :  len = sizeof(double); break;
    case DTYPE_COMPLEX :  len = sizeof(float) * 2; break;
    case DTYPE_COMPLEX_DOUBLE :  len = sizeof(double) * 2; break;
    case DTYPE_ULONGLONG :
    case DTYPE_LONGLONG  :  len = 8; break;
  }
  return len;
}

void MdsIpFree(void *ptr)
{
	free(ptr);
}

static SOCKET ConnectToPort(char *host, char *service)
{
  SOCKET s;
  unsigned short portnum = 0;
  static struct sockaddr_in sin;
  struct servent *sp;
  static int one=1;

#ifdef HAVE_VXWORKS_H
  portnum = htons((atoi(service) == 0) ? 8000 : (unsigned short)atoi(service));
#else
  if (atoi(service) == 0)
  {
    sp = getservbyname(service,"tcp");
    if (sp != NULL)
      portnum = sp->s_port;
    else
    {
      char *port = getenv(service);
      port = (port == NULL) ? ((host[0]=='_') ? "8200" : "8000") : port;
      portnum = htons((short)atoi(port));
    }
  }
  else
    portnum = htons((short)atoi(service));
#endif
  if (portnum == 0)
  {
    printf("Error in MDSplus ConnectToPort: Unknown service: %s\nSet environment variable %s if port is known\n",service,service);
    return INVALID_SOCKET;
  }
  s = MConnect(host, portnum);
#ifdef GLOBUS
  sleep(1);
#endif
  if (s == INVALID_SOCKET)
  {
    perror("Error in connect to service\n");
    return INVALID_SOCKET;
  }
  else
  {
    int status;
    Message *m;
#ifdef _WIN32
	static char user[128];
	int bsize=128;
	char *user_p = GetUserName(user,&bsize) ? user : "Windows User";
#elif __MWERKS__
        static char user[128];
        int bsize=128;
        char *user_p = "Macintosh User";
#elif __APPLE__
    char *user_p;
    struct passwd *pwd;
    pwd = getpwuid(geteuid());
    user_p = pwd->pw_name;
#else
    char *user_p;
#ifdef HAVE_VXWORKS_H
    user_p = "vxWorks";
#else
    user_p = (getpwuid(geteuid()))->pw_name;
    /*
    user_p = (cuserid(user) && strlen(user)) ? user : "?";
    */
#endif
#endif
    m = malloc(sizeof(MsgHdr) + strlen(user_p));
    m->h.client_type = SENDCAPABILITIES;
    m->h.length = strlen(user_p);
    m->h.msglen = sizeof(MsgHdr) + m->h.length;
    m->h.dtype = DTYPE_CSTRING;
    m->h.status = CompressionLevel;
    m->h.ndims = 0;
    memcpy(m->bytes,user_p,m->h.length);
    status = SendMdsMsg(s,m,0);
    free(m);
    if (status & 1)
    {
      m = GetMdsMsg(s,&status);
      if (!(status & 1))
      {
        printf("Error in MDSplus ConnectToPort connect to service\n");
        return INVALID_SOCKET;
      }
      else
      {
        if (!(m->h.status & 1))
        {
          printf("MDSplus ConnectToPort: Access denied\n");
          DisconnectFromMds(s);
          return INVALID_SOCKET;
        }
        CompressionLevel = (m->h.status & 0x1e) >> 1;
      }
      if (m) free(m);
   }
    else
    {
      printf("MDSplus ConnectToPort: Error in user verification\n");
      return INVALID_SOCKET;
    }
  }
  SetSocketOptions(s,0);
#ifndef GLOBUS
#ifndef _WIN32
  setsockopt(s, SOL_SOCKET,SO_OOBINLINE,(void *)&one,sizeof(one));
#endif
#ifdef MULTINET
  sys$qiow(0,s,IO$_SETMODE | IO$M_ATTNAST,0,0,0,MdsDispatchEvent,s,0,0,0,0);
#endif
#endif
  return s;
}

int HostToIp(char *host, int *addr, short *port)
{
  struct hostent *hp = NULL;
  struct servent *sp;

  char hostpart[256] = {0};
  char portpart[256] = {0};

  sscanf(host,"%[^:]:%s",hostpart,portpart);
  if (strlen(portpart) == 0)
  {
    if (host[0] == '_')
      strcpy(portpart,"mdsips");
    else
      strcpy(portpart,"mdsip");
  }
  if (strcmp(hostpart, "local") == 0) {
    *addr = 0;
    *port = 0;
    return 1;
  }
#ifndef HAVE_VXWORKS_H
  hp = gethostbyname(hostpart);
#endif
#ifdef _WIN32
  if ((hp == NULL) && (WSAGetLastError() == WSANOTINITIALISED))
  {
    WSADATA wsaData;
    WORD wVersionRequested;
    wVersionRequested = MAKEWORD(1,1);
    WSAStartup(wVersionRequested,&wsaData);
    hp = gethostbyname(hostpart);
  }
#endif
  if (hp == NULL)
  {
    *addr = inet_addr(hostpart);
#ifndef HAVE_VXWORKS_H
    if (*addr != 0xffffffff)
      hp = gethostbyaddr((void *) addr, (int) sizeof(* addr), AF_INET);
#endif
  }
#ifdef HAVE_VXWORKS_H
  if (*addr == 0xffffffff)
#else
  if (hp == NULL)
#endif
  {
    printf("Error in MDSplus ConnectToPort: %s unknown\n",host);
    return INVALID_SOCKET;
  }
#ifdef HAVE_VXWORKS_H
  if (atoi(portpart) == 0)
  {
    *port=8000;
  }
  else
    *port = (short)atoi(portpart);
#else
  if (atoi(portpart) == 0)
  {
    sp = getservbyname(portpart,"tcp");
    if (sp != NULL)
      *port = ntohs(sp->s_port);
    else
    {
      char *portc = getenv(portpart);
      *port = (portc == NULL) ? 8000 : (short)atoi(portc);
    }
  }
  else
    *port = (short)atoi(portpart);
#endif
  if (*port == 0)
  {
    printf("Error in MDSplus ConnectToPort: Unknown service: %s\nSet environment variable %s if port is known\n",portpart,portpart);
    return INVALID_SOCKET;
  }
/* for vxWorks addr is already filled in */
#ifndef HAVE_VXWORKS_H
#ifdef ANET
  memcpy(addr, hp->h_addr, sizeof(*addr));
#else
  memcpy(addr, hp->h_addr_list[0], sizeof(*addr));
#endif
#endif
  return 1;
}

SOCKET  ConnectToMds(char *host)
{
  char hostpart[256] = {0};
  char portpart[256] = {0};
  int i;
  sscanf(host,"%[^:]:%s",hostpart,portpart);
  if (strlen(portpart) == 0)
  {
    if (host[0] == '_')
      strcpy(portpart,"mdsips");
    else
      strcpy(portpart,"mdsip");
  }
  for (i=strlen(hostpart)-1;i>=0 && hostpart[i]==32;hostpart[i]=0,i--);
  return ConnectToPort(hostpart,portpart);
}

SOCKET  ConnectToMdsEvents(char *host)
{
  char hostpart[256] = {0};
  char portpart[256] = {0};
  sscanf(host,"%[^:]:%s",hostpart,portpart);
  if (strlen(portpart) == 0)
    strcpy(portpart,"mdsipe");
  return ConnectToPort(hostpart, portpart);
}

int  GetAnswerInfo(SOCKET sock, char *dtype, short *length, char *ndims, int *dims, int *numbytes, void * *dptr)
{
  static Message *m = 0;
  if (m)
  {    
    free(m);
    m = 0;
  }
  return GetAnswerInfoTS(sock,dtype,length,ndims,dims,numbytes,dptr,&m);
}

int  GetAnswerInfoTS(SOCKET sock, char *dtype, short *length, char *ndims, int *dims, int *numbytes, void * *dptr, Message **m)
{
  int status;
  int i;
  *m = 0;
  *numbytes = 0;
  *m = GetMdsMsg(sock, &status);
  if (status != 1)
  {
    *dtype = 0;
    *length = 0;
    *ndims = 0;
    *numbytes = 0;
    *dptr = 0;
    if (*m) 
    {
      free(*m);
      *m=0;
    }
    return 0;
  }
  if ((*m)->h.ndims)
  {
    *numbytes = (*m)->h.length;
    for (i=0;i<(*m)->h.ndims;i++)
    {
#ifdef __CRAY
      dims[i] = i % 2 ? (*m)->h.dims[i/2] & 0xffffffff : (*m)->h.dims[i/2] >> 32;
#else
      dims[i] = (*m)->h.dims[i];
#endif
      *numbytes *= dims[i];
#ifdef DEBUG
      printf("dim[%d] = %d\n",i,dims[i]);
#endif
    }
    for (i=(*m)->h.ndims;i < MAX_DIMS; i++)
      dims[i] = 0;
  }
  else
  {
    *numbytes = (*m)->h.length;
    for (i=0;i<MAX_DIMS;i++)
      dims[i] = 0;
  }
  if ((int)(sizeof(MsgHdr) + *numbytes) != (*m)->h.msglen)
  {
    *numbytes = 0;
    if (*m) free(*m);
    return 0;
  }
  *dtype = (*m)->h.dtype;
  *length = (*m)->h.length;
  *ndims = (*m)->h.ndims;
  *dptr = (*m)->bytes;
  return (*m)->h.status;
}

int  DisconnectFromMds(SOCKET sock)
{
  int status = CloseSocket(sock);
#ifdef __MSDOS__
  WSACleanup();
#endif
  return status;
}

int  SendArg(SOCKET sock, unsigned char idx, char dtype, unsigned char nargs, short length, char ndims,
int *dims, char *bytes)
{
  int status;
  int msglen;
  int i;
  int nbytes = length;
  Message *m;
  if (idx > nargs)
  {
    /**** Special I/O message ****/ 
    nbytes = dims[0];
  }
  else
  {
    for (i=0;i<ndims;i++)
      nbytes *= dims[i];
  }
  msglen = sizeof(MsgHdr) + nbytes;
  m = malloc(msglen);
  m->h.client_type = 0;
  m->h.msglen = msglen;
  m->h.message_id = message_id;
  m->h.descriptor_idx = idx;
  m->h.dtype = dtype;
  m->h.nargs = nargs;
  m->h.length = length;
  m->h.ndims = ndims;
#ifdef __CRAY
  for (i=0;i<4;i++)
    m->h.dims[i] = ((ndims > i * 2) ? (dims[i * 2] << 32) : 0) | ((ndims > (i * 2 + 1)) ? (dims[i * 2 + 1]) : 0); 
#else
  for (i=0;i<MAX_DIMS;i++) m->h.dims[i] = i < ndims ? dims[i] : 0;
#endif
  memcpy(m->bytes,bytes,nbytes);
  status = SendMdsMsg(sock, m, 0);
  if (idx > nargs || idx == (nargs -1)) message_id++;
  if (message_id == 0) message_id = 1;
  free(m);
  return status;
}

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


int SendMdsMsg(SOCKET sock, Message *m, int oob)
{
  unsigned long len = m->h.msglen - sizeof(m->h);
  unsigned long clength = 0; 
  Message *cm = 0;
  int status;
  int do_swap = 0; /*Added to handle byte swapping with compression*/
 
  if (len > 0 && CompressionLevel > 0 && m->h.client_type != SENDCAPABILITIES)
  {
	  clength = len;
	  cm = (Message *)malloc(m->h.msglen + 4);
  }
  if (!oob) FlushSocket(sock);
  if (m->h.client_type == SENDCAPABILITIES)
    m->h.status = CompressionLevel;
  if ((m->h.client_type & SwapEndianOnServer) != 0)
  {
    if (Endian(m->h.client_type) != Endian(ClientType()))
    {
      FlipData(m);
      FlipHeader(&m->h);
      do_swap = 1; /* Recall that the header field msglen needs to be swapped */
    }
  }
  else
    m->h.client_type = ClientType();
  if (clength && compress2(cm->bytes+4,&clength,m->bytes,len,CompressionLevel) == 0 && clength < len)
  {
    cm->h = m->h;
    cm->h.client_type |= COMPRESSED;
    memcpy(cm->bytes,&cm->h.msglen,4);
    cm->h.msglen = clength + 4 + sizeof(MsgHdr);
/*If byte swapping required, swap msglen */
    if(do_swap)
 	FlipBytes(4,(char *)&cm->h.msglen);	
 /* status = SendBytes(sock, (char *)cm, cm->h.msglen, oob);*/
/* now msglen is swapped, and cannot be used as byte counter */
    status = SendBytes(sock, (char *)cm, clength + 4 + sizeof(MsgHdr), oob);
  }
  else
    status = SendBytes(sock, (char *)m, len + sizeof(MsgHdr), oob);
  if (clength) free(cm);
  return status;
}

  
void FlipHeader(MsgHdr *header)
{
  int i;
#ifdef __CRAY
#define Flip32(n) n = ((n >> 24) & 0xff) | ((n >> 8) & 0xff00) | ((n << 8) & 0xff0000) | ((n << 24) & 0xff000000)
#define Flip16(n) n = ((n >> 8) & 0xff) | ((n << 8) & 0xff00)
  Flip32(header->msglen);
  Flip32(header->status);
  Flip16(header->length);
  for (i=0;i<MAX_DIMS;i++) FlipBytes(4,((char *)header->dims)+4*i);
#else
  FlipBytes(4,(char *)&header->msglen);
  FlipBytes(4,(char *)&header->status);
  FlipBytes(2,(char *)&header->length);
  for (i=0;i<MAX_DIMS;i++) FlipBytes(sizeof(header->dims[i]),(char *)&header->dims[i]);
#endif
}

static void FlipData(Message *m)
{
  int num = 1;
  int i;
  char *ptr;
  int dims[MAX_DIMS];
  for (i=0;i<MAX_DIMS;i++)
  {
#ifdef __CRAY
    dims[i] = i % 2 ? m->h.dims[i/2] & 0xffffffff : m->h.dims[i/2] >> 32;
#else
    dims[i] = m->h.dims[i];
#endif
  }
  if (m->h.ndims) for (i=0;i<m->h.ndims;i++) num *= dims[i];
#ifdef DEBUG
  printf("num to flip = %d\n",num);
#endif
  switch (m->h.dtype)
  {
#ifndef __CRAY
    case DTYPE_COMPLEX:
    case DTYPE_COMPLEX_DOUBLE: for (i=0,ptr=m->bytes;i<(num * 2);i++,ptr += m->h.length/2) FlipBytes(m->h.length/2,ptr); break;
    case DTYPE_FLOAT:   
    case DTYPE_DOUBLE:
#endif
    case DTYPE_LONGLONG:
    case DTYPE_ULONGLONG:
    case DTYPE_USHORT:
    case DTYPE_SHORT:  
    case DTYPE_ULONG:
    case DTYPE_LONG:       for (i=0,ptr=m->bytes;i<num;i++,ptr += m->h.length) FlipBytes(m->h.length,ptr); break;
  }
}

Message *GetMdsMsg(SOCKET sock, int *status)
{
  MsgHdr header;
  Message *msg = 0;
  int msglen = 0;
  *status = 0;
  *status = GetBytes(sock, (char *)&header, sizeof(MsgHdr), 0);
  if (*status &1)
  {
    if ( Endian(header.client_type) != Endian(ClientType()) ) FlipHeader(&header);
#ifdef DEBUG
    printf("msglen = %d\nstatus = %d\nlength = %d\nnargs = %d\ndescriptor_idx = %d\nmessage_id = %d\ndtype = %d\n",
               header.msglen,header.status,header.length,header.nargs,header.descriptor_idx,header.message_id,header.dtype);
    printf("client_type = %d\nndims = %d\n",header.client_type,header.ndims);
#endif
    if (CType(header.client_type) > CRAY_CLIENT || header.ndims > MAX_DIMS)
    {
      CloseSocket(sock);
      fprintf(stderr,"\rGetMdsMsg shutdown socket %d: bad msg header, header.ndims=%d, client_type=%d\n",sock,header.ndims,CType(header.client_type));
      *status = 0;
      return 0;
    }  
    msglen = header.msglen;
    msg = malloc(header.msglen);
    msg->h = header;
    *status = GetBytes(sock, msg->bytes, msglen - sizeof(MsgHdr), 0);
    if (*status & 1 && IsCompressed(header.client_type))
    {
      Message *m;
      unsigned long dlen;
      memcpy(&msglen, msg->bytes, 4);
      if (Endian(header.client_type) != Endian(ClientType()))
        FlipBytes(4,(char *)&msglen);
      m = malloc(msglen);
      m->h = header;
      dlen = msglen - sizeof(MsgHdr);
      *status = uncompress(m->bytes, &dlen, msg->bytes + 4, header.msglen - sizeof(MsgHdr) - 4) == 0;
      if (*status & 1)
      {
	m->h.msglen = msglen;
        free(msg);
	msg = m;
      }
      else
	free(m);
    }
    if (*status & 1 && (Endian(header.client_type) != Endian(ClientType())))
      FlipData(msg);
  }
  return msg;
}

