#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ipdesc.h>
#define MAX_STREAMS 32
static int streamFds[MAX_STREAMS];
static int numStreams=1;

#define MDS_IO_PARALLEL_K 11

struct client_streams {
  int numStreams;
  int streamFds[MAX_STREAMS];
  struct client_streams *next;
};

struct client_streams *clientStreams=0;

struct client_streams *GetClientStreams(int socket,int add) {
  struct client_streams *s,*p;
  for (p=0,s=clientStreams;s!=0;p=s,s=s->next) {
    if (s->streamFds[0]==socket)
      return s;
  }
  if (add) {
    s=malloc(sizeof(*s));
    s->streamFds[0]=socket;
    s->numStreams=1;
    s->next=0;
    if (p) {
      p->next=s;
    } else {
      clientStreams=s;
    }
    return s;
  }
  return 0;
}

static char *sockName(int pid) {
  static char filename[1000];
  sprintf(filename,"/tmp/mdsip_streams_%d",pid);
  return filename;
}

static int sock_dgram(int pid)
{
  struct sockaddr_un unix_socket_name = {0};
  int fd;
  
  strcpy(unix_socket_name.sun_path,sockName(pid));
  if (unlink(sockName(pid)) < 0) {
    if (errno != ENOENT) {
      fprintf(stderr, "%s: ",sockName(pid));
      perror("unlink");
      return -1;
    }
  }
  
  unix_socket_name.sun_family = AF_UNIX;

  fd = socket(PF_UNIX, SOCK_DGRAM, 0);
  if (fd == -1) return -1;
  if (bind(fd, (struct sockaddr *)&unix_socket_name, sizeof(unix_socket_name))) {
    close(fd);
    return -1;
  }
  return fd;
}

/* receive a file descriptor over file descriptor fd */
static int receive_fd(int fd)
{
  struct msghdr msg;
  struct iovec iov;
  char buf[1];
  int rv;
  int connfd = -1;
  char ccmsg[CMSG_SPACE(sizeof(connfd))];
  struct cmsghdr *cmsg;

  iov.iov_base = buf;
  iov.iov_len = 1;

  msg.msg_name = 0;
  msg.msg_namelen = 0;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  /* old BSD implementations should use msg_accrights instead of 
   * msg_control; the interface is different. */
  msg.msg_control = ccmsg;
  msg.msg_controllen = sizeof(ccmsg); /* ? seems to work... */
    
  rv = recvmsg(fd, &msg, 0);
  if (rv == -1) {
    perror("recvmsg");
    return -1;
  }

  cmsg = CMSG_FIRSTHDR(&msg);
  if (!cmsg->cmsg_type == SCM_RIGHTS) {
    fprintf(stderr, "got control message of unknown type %d\n", 
	    cmsg->cmsg_type);
    return -1;
  }
  return *(int*)CMSG_DATA(cmsg);
}

int connectFd = -1;

int mdsipConnectStreamsStart()
{
  int pid=getpid();
  mdsipConnectStreamsEnd();
  connectFd = sock_dgram(getpid());
  if (numStreams > 1) {
    int i;
    for (i=1;i<numStreams;i++) {
      close(streamFds[i]);
    }
    numStreams = 1;
  }
  return connectFd != -1;
}

int mdsipConnectStreamsEnd() {
  if (connectFd != -1) {
    close(connectFd);
    connectFd=-1;
  }
  unlink(sockName(getpid()));
  return 1;
}

int mdsipConnectStream() {
  int i;
  if (numStreams >= MAX_STREAMS)
    return 0;
  streamFds[numStreams]=receive_fd(connectFd);
  if (streamFds[numStreams]==-1) {
    for (i=1;i<numStreams;i++) {
      close(streamFds[i]);
    }
    mdsipConnectStreamsEnd();
    numStreams=1;
    return 0;
  }
  numStreams++;
  return 1;
}

int mdsipParallelInfo(int socket,int **fds) {
  int num=1;
  if (socket == 0) {
    num=numStreams;
    *fds=streamFds;
  }
  else {
    struct client_streams *s = GetClientStreams(socket,0);
    if (s) {
      num=s->numStreams;
      *fds=s->streamFds;
    }
  }
  return num;
}

int mdsipSendStream(int pid) {
  struct msghdr msg;
  int fd=0;
  char ccmsg[CMSG_SPACE(sizeof(fd))];
  struct cmsghdr *cmsg;
  struct iovec vec;  /* stupidity: must send/receive at least one byte */
  char *str = "x";
  int rv;
  struct sockaddr_un unix_socket_name = {0};
  int unix_socket_fd;
  unix_socket_name.sun_family = AF_UNIX;
  strcpy(unix_socket_name.sun_path, sockName(pid));
  unix_socket_fd = socket(PF_UNIX, SOCK_DGRAM, 0);
  if (unix_socket_fd == -1) return -1;

  msg.msg_name = (struct sockaddr*)&unix_socket_name;
  msg.msg_namelen = sizeof(unix_socket_name);

  vec.iov_base = str;
  vec.iov_len = 1;
  msg.msg_iov = &vec;
  msg.msg_iovlen = 1;

  /* old BSD implementations should use msg_accrights instead of 
   * msg_control; the interface is different. */
  msg.msg_control = ccmsg;
  msg.msg_controllen = sizeof(ccmsg);
  cmsg = CMSG_FIRSTHDR(&msg);
  cmsg->cmsg_level = SOL_SOCKET;
  cmsg->cmsg_type = SCM_RIGHTS;
  cmsg->cmsg_len = CMSG_LEN(sizeof(fd));
  *(int*)CMSG_DATA(cmsg) = fd;
  msg.msg_controllen = cmsg->cmsg_len;

  msg.msg_flags = 0;

  rv = (sendmsg(unix_socket_fd, &msg, 0) != -1);
  if (rv) close(fd);
  return rv;
}

static int connectStreamsStart(int socket, int *pid) {
  int status;
  int info[] = {0,0};
  status=SendArg(socket,MDS_IO_PARALLEL_K,0,0,0,sizeof(info)/sizeof(int),info,0);
  if (status & 1) {
    char dtype;
    unsigned short length;
    char ndims;
    int dims[7];
    int numbytes;
    void *dptr;
    void *msg=0;
    status = GetAnswerInfoTS(socket,&dtype,&length,&ndims,dims,&numbytes,&dptr,&msg);
    if (status & 1) {
      if (numbytes==4 && dtype==DTYPE_LONG && dptr != 0) {
	*pid = *(int *)dptr;
      }
      else
	status=0;
    }
    if (msg != 0)
      free(msg);
  }
  return status;
}

static int connectStreamsEnd(int socket) {
  int status;
  int info[] = {0,2};
  status=SendArg(socket,MDS_IO_PARALLEL_K,0,0,0,sizeof(info)/sizeof(int),info,0);
  if (status & 1) {
    char dtype;
    unsigned short length;
    char ndims;
    int dims[7];
    int numbytes;
    void *dptr;
    void *msg=0;
    status = GetAnswerInfoTS(socket,&dtype,&length,&ndims,dims,&numbytes,&dptr,&msg);
    if (msg != 0)
      free(msg);
  }
  return status;
}

static int sendStream(int socket, int pid) {
  int status;
  int info[] = {0,3,pid};
  status=SendArg(socket,MDS_IO_PARALLEL_K,0,0,0,sizeof(info)/sizeof(int),info,0);
  return status;
}

static int connectStreamStart(int socket) {
  int status;
  int info[] = {0,1};
  status=SendArg(socket,MDS_IO_PARALLEL_K,0,0,0,sizeof(info)/sizeof(int),info,0);
  return status;
}

static int connectStreamEnd(int socket) {
  int status;
  char dtype;
  unsigned short length;
  char ndims;
  int dims[7];
  int numbytes;
  void *dptr;
  void *msg=0;
  status = GetAnswerInfoTS(socket,&dtype,&length,&ndims,dims,&numbytes,&dptr,&msg);
  if (status & 1) {
    if (numbytes==4 && dtype==DTYPE_LONG && dptr != 0) {
      status = *(int *)dptr;
    }
    else
      status=0;
  }
  if (msg != 0)
    free(msg);
  return status;
}

int MdsSetStreams(int socket,int num) {
  struct sockaddr_in socket_name;
  int len=sizeof(socket_name);
  int status;
  int i;
  int pid;
  struct client_streams *s;
  if (num > MAX_STREAMS)
    return 0;
  s=GetClientStreams(socket,1);
  for (i=1;i<s->numStreams;i++) {
    close(s->streamFds[i]);
  }
  s->numStreams=1;
  if (getpeername(socket,(struct sockaddr *)&socket_name,&len) == 0) {
    int port;
    char hostname[100];
    unsigned char *ipaddr=(char *)&socket_name.sin_addr;
    port=ntohs(socket_name.sin_port);
    sprintf(hostname,"%d.%d.%d.%d:%d",ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3],port);
    status=connectStreamsStart(socket,&pid);
    if (status & 1) {
      for (i=1;i<num;i++) {
	s->streamFds[i]=ConnectToMds(hostname);
	if (s->streamFds[i] == -1) goto cleanup;
	s->numStreams++;
	status=connectStreamStart(socket);
	if (!(status & 1)) goto cleanup;
	status=sendStream(s->streamFds[i],pid);
	if (!(status & 1)) goto cleanup;
	status=connectStreamEnd(socket);
	if (!(status & 1)) goto cleanup;
      }
      status = connectStreamsEnd(socket);
    }
    return status;
  }
  else
    return 0;
 cleanup:
  for (i=1;i<s->numStreams;i++)
    DisconnectFromMds(s->streamFds[i]);
  s->numStreams=1;
  return 0;
}
