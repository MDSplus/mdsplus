#include "globus_common.h"
#include "globus_xio.h"
#include "globus_xio_gsi.h"
#include "globus_gss_assist.h"
#include "globus_xio_tcp_driver.h"
#include "mdsip_connections.h"
#include <STATICdef.h>
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
//#include <stdio.h>
#include <config.h>
#include <time.h>
//#include <netdb.h>
//#include <netinet/in.h>
//#include <netinet/tcp.h>
//#include <arpa/inet.h>
//#include <sys/ioctl.h>
#include <stropts.h>
#include <sys/wait.h>

static ssize_t gsi_send(int conid, const void *buffer, size_t buflen, int nowait);
static ssize_t gsi_recv(int conid, void *buffer, size_t len);
static int gsi_disconnect(int conid);
//static int gsi_flush(int conid);
//static int gsi_listen(int argc, char **argv);
static int gsi_authorize(int conid, char *username);
static int gsi_connect(int conid, char *protocol, char *host);
static int gsi_reuseCheck(char *host,char *unique, size_t buflen);
static IoRoutines gsi_routines = {gsi_connect,gsi_send,gsi_recv,0,0,gsi_authorize,gsi_reuseCheck,gsi_disconnect};

static int MDSIP_SNDBUF = 32768;
static int MDSIP_RCVBUF = 32768;

typedef struct _client {
  int sock;
  int id;
  char *username;
  int addr;
  struct _client *next;
} Client;

static Client *ClientList=0;

EXPORT IoRoutines *Io() {
  return &gsi_routines;
}

static void testStatus(globus_xio_handle_t xio_handle,globus_result_t res,char *msg)
{
  if(res != GLOBUS_SUCCESS) {
    fprintf(stderr, "ERROR:%s ---\n       %s\n", 
	    msg,globus_object_printable_to_string(globus_error_get(res)));
    //    if (xio_handle != NULL)
    //  globus_xio_close(xio_handle,NULL);
  }
}

static void  *getIoHandle(int conid) {
  size_t len;
  char *info_name;
  int readfd;
  void *io_handle = GetConnectionInfo(conid,&info_name, &readfd, &len);
  return (info_name && strcmp(info_name,"gsi")==0) && len == sizeof(io_handle) ? io_handle : 0;
}

static int gsi_authorize(int conid, char *username) {
  return 1;
}
/*
  void *io_handle=getIoHandle(conid);
  time_t tim = time(0);
  char *timestr = ctime(&tim);
  struct sockaddr_in sin;
  size_t n=sizeof(sin);
  int ans=0;
  struct hostent *hp=0;
#ifdef HAVE_VXWORKS_H
  char hostent_buf[512];
#endif
  if (getpeername(s, (struct sockaddr *)&sin, &n)==0) {
    char *matchString[2]={0,0};
    int num=1;
    char *iphost=inet_ntoa(sin.sin_addr);
    timestr[strlen(timestr)-1] = 0;
    hp = gethostbyaddr((char *)&sin.sin_addr,sizeof(sin.sin_addr),AF_INET);
    if (hp && hp->h_name)
      printf("%s (%d) (pid %d) Connection received from %s@%s [%s]\r\n", timestr,s, getpid(), username, hp->h_name, iphost);
    else
      printf("%s (%d) (pid %d) Connection received from %s@%s\r\n", timestr, s, getpid(), username, iphost);
    matchString[0]=strcpy(malloc(strlen(username)+strlen(iphost)+3),username);
    strcat(matchString[0],"@");
    strcat(matchString[0],iphost);
    if (hp && hp->h_name) {
      matchString[1]=strcpy(malloc(strlen(username)+strlen(hp->h_name)+3),username);
      strcat(matchString[1],"@");
      strcat(matchString[1],hp->h_name);
      num=2;
    }
    ans = CheckClient(username,num,matchString);
    if (matchString[0])
      free(matchString[0]);
    if (matchString[1])
      free(matchString[1]);
  } else {
	  perror("Error determining connection info from socket\n");
  }
  fflush(stdout);
  fflush(stderr);
  return ans;
}

*/

static ssize_t gsi_send(int conid, const void *bptr, size_t num, int options) {
  globus_size_t nbytes;
  globus_result_t result;
  void *io_handle = getIoHandle(conid);
  ssize_t sent=-1;
  if (io_handle != 0) {
    result = globus_xio_write((globus_xio_handle_t)io_handle, (globus_byte_t *)bptr, num, num, &nbytes, NULL);
    testStatus(0,result,"mdsip_send_message globus_xio_write");
    if (result == GLOBUS_SUCCESS)
      sent=nbytes;
  }
  return sent;
}   

static ssize_t gsi_recv(int conid, void *bptr, size_t num) {
  globus_result_t result;
  void *io_handle = getIoHandle(conid);
  globus_size_t numreceived;
  ssize_t recved = -1;
  if (io_handle != 0) {
    result = globus_xio_read(io_handle, (globus_byte_t *)bptr, num, num, &numreceived, NULL);
    testStatus(io_handle,result,"mdsip_get_message, globus_xio_read");
    if (result == GLOBUS_SUCCESS)
      recved = numreceived;
  }
  return recved;
}


static int gsi_disconnect(int conid) {
  int status=-1;
  void *io_handle = getIoHandle(conid);
  if (io_handle) {
    globus_result_t result = globus_xio_close(io_handle,NULL);
    status = (result == GLOBUS_SUCCESS) ? 0 : -1;
  }
  return status;
}
/*
static short GetPort() {
  short port;
  char *name=GetPortname();
  struct servent *sp;
  port = htons((short)atoi(name));
  if (port == 0) {
    sp = getservbyname(name,"tcp");
    if (sp == NULL) {
      fprintf(stderr,"unknown service: %s/tcp\n\n",name);
      exit(0);
    }
    port = sp->s_port;
  }
  return port;
}

static int getHostAndPort(char *hostin, struct sockaddr_in *sin) {
  struct hostent *hp = NULL;
  int addr;
  size_t i;
  static char *mdsip="mdsip";
  char *host=strcpy((char *)malloc(strlen(hostin)+1),hostin);
  char *service;
  unsigned short portnum;
  InitializeSockets();
  for (i=0;i<strlen(host) && host[i] != ':';i++);
  if (i<strlen(host)) {
    host[i]='\0';
    service = &host[i+1];
  } else {
    service = mdsip;
  } 
#ifndef HAVE_VXWORKS_H
  hp = gethostbyname(host);
#endif
  if (hp == NULL) {
    addr = inet_addr(host);
#ifndef HAVE_VXWORKS_H
    if (addr != 0xffffffff)
    	hp = gethostbyaddr((void *) &addr, (int) sizeof(addr), AF_INET);
#endif
  }
  if (hp == 0) {
    free(host);
    return 0;
  }
#ifdef HAVE_VXWORKS_H
  portnum = htons((atoi(service) == 0) ? 8000 : atoi(service));
#else
  if (atoi(service) == 0) {
    struct servent *sp;
    sp = getservbyname(service,"tcp");
    if (sp != NULL)
      portnum = sp->s_port;
    else {
      char *port = getenv(service);
      port = (port == NULL) ? "8000" : port;
      portnum = htons(atoi(port));
    }
  }
  else
    portnum = htons(atoi(service));
  if (portnum == 0) {
    free(host);
    return 2;
  }
#endif
  sin->sin_port = portnum;
  sin->sin_family = AF_INET;
#if defined( HAVE_VXWORKS_H )
  memcpy(&sin->sin_addr, &addr, sizeof(addr));
#elif defined(ANET)
  memcpy(&sin->sin_addr, hp->h_addr, sizeof(sin->sin_addr));
#else
  memcpy(&sin->sin_addr, hp->h_addr_list[0], hp->h_length);
#endif
  free(host);
  return 1;
}
*/

static int gsi_reuseCheck(char *host, char *unique, size_t buflen) {
  /*
  struct sockaddr_in sin;
  int status = getHostAndPort(host,&sin);
  if (status == 1) {
    unsigned char *addr=(char *)&sin.sin_addr;
    snprintf(unique,buflen,"tcp://%d.%d.%d.%d:%d",addr[0],addr[1],addr[2],addr[3],ntohs(sin.sin_port));
    return 0;
  } else {
    *unique=0;
    return -1;
  }
  */
  snprintf(unique,buflen,"%s",host);
  return 0;
}


static int gsi_connect(int conid, char *protocol, char *host) {
  static int activated=0;
  static globus_xio_stack_t stack_tcp;
  static globus_xio_stack_t stack_gsi;
  static globus_xio_driver_t gsi_driver;
  static globus_xio_driver_t tcp_driver;
  globus_result_t result;
  globus_xio_handle_t xio_handle;
  globus_xio_attr_t attr;
  char *contact_string;

  if (activated == 0) {
    result = globus_module_activate(GLOBUS_XIO_MODULE);
    testStatus(0,result,"mdsip_connect globus_module_activate");
    if (result != GLOBUS_SUCCESS) return -1;
    result = globus_xio_driver_load("tcp",&tcp_driver);
    testStatus(0,result,"mdsip_connect load tcp driver");
    if (result != GLOBUS_SUCCESS) return -1;
    result = globus_xio_driver_load("gsi",&gsi_driver);
    testStatus(0,result,"mdsip_connect load gsi driver");
    if (result != GLOBUS_SUCCESS) return -1;
    result = globus_xio_stack_init(&stack_tcp, NULL);
    testStatus(0,result,"mdsip_connect globus_xio_stack_init");
    if (result != GLOBUS_SUCCESS) return -1;
    result = globus_xio_stack_init(&stack_gsi, NULL);
    testStatus(0,result,"mdsip_connect globus_xio_stack_init");
    if (result != GLOBUS_SUCCESS) return -1;
    result = globus_xio_stack_push_driver(stack_tcp, tcp_driver);
    testStatus(0,result,"mdsip_connect globus_xio_stack_push_driver");
    if (result != GLOBUS_SUCCESS) return -1;
    result = globus_xio_stack_push_driver(stack_gsi, gsi_driver);
    testStatus(0,result,"mdsip_connect globus_xio_stack_push_driver");
    if (result != GLOBUS_SUCCESS) return -1;
    result = globus_xio_stack_push_driver(stack_gsi, gsi_driver);
    testStatus(0,result,"mdsip_connect globus_xio_stack_push_driver");
    if (result != GLOBUS_SUCCESS) return -1;
    activated = 1;
  }
  if (host == NULL || strlen(host) == 0) return -1;
  result = globus_xio_handle_create(&xio_handle, stack_gsi);
  testStatus(0,result,"mdsip_connect globus_xio_handle_create");
  if (result != GLOBUS_SUCCESS) return -1;
  contact_string = strcpy((char *)malloc(strlen(host)+10),host);
  if (strstr(contact_string,":") == NULL)
    strcat(contact_string,":8200");
  result = globus_xio_attr_init(&attr);
  testStatus(0,result,"mdsip_connect globus_xio_attr_init");
  if (result != GLOBUS_SUCCESS) return -1;
  result = globus_xio_attr_cntl(attr,gsi_driver,GLOBUS_XIO_GSI_SET_DELEGATION_MODE,
				GLOBUS_XIO_GSI_DELEGATION_MODE_FULL);
  testStatus(0,result,"mdsip_connect globus_xio_attr_cntl");
  if (result != GLOBUS_SUCCESS) return -1;
  result = globus_xio_attr_cntl(attr,
				gsi_driver,
				GLOBUS_XIO_GSI_SET_AUTHORIZATION_MODE,
				GLOBUS_XIO_GSI_HOST_AUTHORIZATION);
  testStatus(0,result,"mdsip_connect globus_xio_attr_cntl");
  if (result != GLOBUS_SUCCESS) return -1;
  result = globus_xio_attr_cntl(attr,gsi_driver,GLOBUS_XIO_TCP_SET_SNDBUF,MDSIP_SNDBUF);
  testStatus(0,result,"mdsip_connect SET_SNDBUF");
  if (result != GLOBUS_SUCCESS) return -1;
  result = globus_xio_attr_cntl(attr,gsi_driver,GLOBUS_XIO_TCP_SET_RCVBUF,MDSIP_RCVBUF);
  testStatus(0,result,"mdsip_connect SET_RCVBUF");
  if (result != GLOBUS_SUCCESS) return -1;
  result = globus_xio_attr_cntl(attr,gsi_driver,GLOBUS_XIO_TCP_SET_NODELAY,GLOBUS_TRUE);
  testStatus(0,result,"mdsip_connect SET_NODELAY");
  if (result != GLOBUS_SUCCESS) return -1;
  result = globus_xio_attr_cntl(attr,gsi_driver,GLOBUS_XIO_TCP_SET_KEEPALIVE,GLOBUS_TRUE);
  testStatus(0,result,"mdsip_connect SET_KEEPALIVE");
  if (result != GLOBUS_SUCCESS) return -1;
  result = globus_xio_open(xio_handle, contact_string, attr);
  testStatus(0,result,"mdsip_connect globus_xio_open");
  SetConnectionInfo(conid,"tcp",0,&xio_handle,sizeof(xio_handle));
  return 0;
}
/*
static int gsi_listen(int argc, char **argv) {
#ifndef HAVE_WINDOWS_H
  signal(SIGCHLD,ChildSignalHandler);
#endif
  InitializeSockets();
  if (GetMulti()) {
    unsigned short port=GetPort();
    char *matchString[] = {"multi"};
    int s;
    struct sockaddr_in sin;
    int tablesize = FD_SETSIZE;
    int error_count=0;
    fd_set readfds;
    int one=1;
    int status;
    FD_ZERO(&fdactive);
    CheckClient(0,1,matchString);
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) {
      printf("Error getting Connection Socket\n");
      exit(1);
    }
    FD_SET(s,&fdactive);
    SetSocketOptions(s,1);
    memset(&sin,0,sizeof(sin));
    sin.sin_port = port;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    status = bind(s, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
    if (status < 0)  {
      perror("Error binding to service\n");
      exit(1);
    }
    status = listen(s,128);
    if (status < 0) {
      printf("Error from listen\n");
      exit(1);
    }
    while (1) {
      readfds = fdactive;
      if (select(tablesize, &readfds, 0, 0, 0) != -1) {
	error_count=0;
	if (FD_ISSET(s, &readfds)) {
	  socklen_t len = sizeof(sin);
	  int id=-1;
	  int status;
	  char *username;
	  int sock=accept(s, (struct sockaddr *)&sin, &len);
	  SetSocketOptions(sock,0);
	  status=AcceptConnection("tcp", "tcp", sock, 0, 0,&id,&username);
	  if (status & 1) {
	    Client *new=memset(malloc(sizeof(Client)),0,sizeof(Client));
	    new->id=id;
	    new->sock=sock;
	    new->next=ClientList;
	    new->username=username;
		new->addr=*(int *)&sin.sin_addr;
	    ClientList=new;
	    FD_SET(sock,&fdactive);
	  }      
	} else {
	  Client *c;
	  for (c=ClientList;c;) {
		if (FD_ISSET(c->sock, &readfds)) {
		  MdsSetClientAddr(c->addr);
	      DoMessage(c->id);
		  c=ClientList;
		} else
		  c=c->next;
	  }
	}
      }
      else {
	error_count++;
	perror("error in main select");
	fprintf(stderr,"Error count=%d\n",error_count);
	fflush(stderr);
	if (error_count > 100) {
	  fprintf(stderr,"Error count exceeded, shutting down\n");
	  exit(1);
	}
	else {
	  Client *c;
	  FD_ZERO(&fdactive);
	  if (s != -1)
	    FD_SET(s,&fdactive);
	  for (c=ClientList; c; c=c->next) {
	    struct sockaddr sin;
	    socklen_t n = sizeof(sin);
	    LockAsts();
	    if (getpeername(c->sock, (struct sockaddr *)&sin, &n)) {
	      fprintf(stderr,"Removed disconnected client\n");
		  fflush(stderr);
	      CloseConnection(c->id);
	    } else {
	      FD_SET(c->sock, &fdactive);
	    }
	    UnlockAsts();
	  }
	}
      }
    }
  } else {
    int id;
    int sock=GetSocketHandle();
    char *username;
    int status;
	status=AcceptConnection("tcp", "tcp", sock, 0, 0,&id,&username);
    if (status & 1) {
	  struct sockaddr_in sin;
      size_t n=sizeof(sin);
      Client *new=memset(malloc(sizeof(Client)),0,sizeof(Client));
      if (getpeername(sock, (struct sockaddr *)&sin, &n)==0)
	    MdsSetClientAddr(*(int *)&sin.sin_addr);
      new->id=id;
      new->sock=sock;
      new->next=ClientList;
      new->username=username;
      ClientList=new;
      FD_SET(sock,&fdactive);
    }
    while (status & 1)
      status = DoMessage(id);
  }
  return 1;
}
*/
