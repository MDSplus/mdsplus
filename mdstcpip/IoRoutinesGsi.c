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
#include <config.h>
#include <time.h>
#include <stropts.h>
#include <sys/wait.h>

static ssize_t gsi_send(int conid, const void *buffer, size_t buflen, int nowait);
static ssize_t gsi_recv(int conid, void *buffer, size_t len);
static int gsi_disconnect(int conid);
static int gsi_listen(int argc, char **argv);
static int gsi_authorize(int conid, char *username);
static int gsi_connect(int conid, char *protocol, char *host);
static int gsi_reuseCheck(char *host,char *unique, size_t buflen);
static IoRoutines gsi_routines = {gsi_connect,gsi_send,gsi_recv,0,gsi_listen,gsi_authorize,gsi_reuseCheck,gsi_disconnect};

static int MDSIP_SNDBUF = 32768;
static int MDSIP_RCVBUF = 32768;

static globus_mutex_t globus_l_mutex;
static globus_cond_t  globus_l_cond;

typedef struct _gsi_info {
  globus_xio_handle_t xio_handle;
  globus_xio_driver_t gsi_driver;
  globus_xio_driver_t tcp_driver;
  char *connection_name;
} GSI_INFO;


EXPORT IoRoutines *Io() {
  return &gsi_routines;
}

static short GetPort(char *pname) {
  short port;
  char *name=pname ? pname : GetPortname();
  struct servent *sp;
  if (name==0 || strcmp(name,"mdsip")==0)
    name="mdsips";
  port = htons((short)atoi(name));
  if (port == 0) {
    sp = getservbyname(name,"tcp");
    if (sp == NULL) {
      fprintf(stderr,"unknown service: %s/tcp\n\n",name);
      exit(0);
    }
    port = sp->s_port;
  }
  return ntohs(port);
}

static void testStatus(globus_xio_handle_t xio_handle,globus_result_t res,char *msg) {
  if(res != GLOBUS_SUCCESS) {
    fprintf(stderr, "ERROR:%s ---\n       %s\n", 
	    msg,globus_object_printable_to_string(globus_error_get(res)));
    //    if (xio_handle != NULL)
    //  globus_xio_close(xio_handle,NULL);
  }
}

static GSI_INFO  *getGsiInfo(int conid) {
  size_t len;
  char *info_name;
  int readfd;
  GSI_INFO *info = (GSI_INFO *)GetConnectionInfo(conid,&info_name, &readfd, &len);
  return (info_name && strcmp(info_name,"gsi")==0) && len == sizeof(GSI_INFO) ? info : 0;
}

static int gsi_authorize(int conid, char *username) {
  GSI_INFO *info = getGsiInfo(conid);
  int ans=0;
  if (info) {
    char *hostname;
    char *hostip;
    globus_result_t res;
    gss_buffer_desc                         peer_name_buffer = GSS_C_EMPTY_BUFFER;
    OM_uint32                               status;
    gss_name_t peer;
    char *match_string[2]={0,0};
    time_t tim=time(0);
    char *timestr = ctime(&tim);
    timestr[strlen(timestr)-1]=0;
    res = globus_xio_handle_cntl(info->xio_handle,
				 info->tcp_driver,
				 GLOBUS_XIO_TCP_GET_REMOTE_CONTACT,
				 &hostname);
    testStatus(info->xio_handle,res,"GET_REMOTE_CONTACT");
    res = globus_xio_handle_cntl(info->xio_handle,
				 info->tcp_driver,
				 GLOBUS_XIO_TCP_GET_REMOTE_NUMERIC_CONTACT,
				 &hostip);
    testStatus(info->xio_handle,res,"GET_REMOTE_NUMERIC_CONTACT");
    res = globus_xio_handle_cntl(info->xio_handle,info->gsi_driver,
				 GLOBUS_XIO_GSI_GET_PEER_NAME,
				 &peer);
    testStatus(info->xio_handle,res,"GET_PEER_NAME");
    res = gss_display_name(&status, peer, &peer_name_buffer, GLOBUS_NULL);
    gss_release_name(&status,&peer);
    match_string[0]=(char *)malloc(strlen(hostname)+strlen((char *)peer_name_buffer.value)+2);
    match_string[1]=(char *)malloc(strlen(hostip)+strlen((char *)peer_name_buffer.value)+2);
    strcpy(match_string[0],(char *)peer_name_buffer.value);
    strcpy(match_string[1],match_string[0]);
    strcat(match_string[0],"@");
    strcat(match_string[1],"@");
    strcat(match_string[0],hostname);
    strcat(match_string[1],hostip);
    match_string[0][strlen((char *)peer_name_buffer.value)+1+strcspn(hostname,":")]=0;
    match_string[1][strlen((char *)peer_name_buffer.value)+1+strcspn(hostip,":")]=0;
    info->connection_name=malloc(strlen(match_string[0])+strlen(match_string[1])+strlen(username));
    sprintf(info->connection_name,"%s - %s@%s [%s]",(char *)peer_name_buffer.value,
	    username, &match_string[0][strlen((char *)peer_name_buffer.value)+2],
	    &match_string[1][strlen((char *)peer_name_buffer.value)+2]);
    printf("%s (pid %d) Connection received from %s\r\n", timestr, getpid(), info->connection_name);
    gss_release_buffer(&status,&peer_name_buffer);
    ans = CheckClient(username,2,match_string);
    if (ans) {
      gss_cred_id_t credential;
      OM_uint32 major_status,minor_status;
      gss_buffer_desc buffer_desc = GSS_C_EMPTY_BUFFER;
      res = globus_xio_handle_cntl(info->xio_handle,info->gsi_driver,
				   GLOBUS_XIO_GSI_GET_DELEGATED_CRED,&credential);
      testStatus(info->xio_handle,res,"openCallback,GET_DELEGATED_CRED");
      major_status = gss_export_cred(&minor_status,credential,GSS_C_NO_OID,0,&buffer_desc);
      if (major_status != GSS_S_COMPLETE) {
	char *error_str;
	globus_gss_assist_display_status_str(&error_str,NULL,major_status,minor_status,0);
	fprintf(stderr,"\nLINE %d ERROR: %s\n\n",__LINE__,error_str);
      } else {
	char cred_file_name[]="/tmp/x509pp_pXXXXXX";
	int fd=mkstemp(cred_file_name);
	if (fd != -1) {
	  fchmod(fd,00600);
	  write(fd,buffer_desc.value,buffer_desc.length);
	  fchmod(fd,00400);
	  close(fd);
	  setenv("X509_USER_PROXY",cred_file_name,1);
	} else {
	  perror("Error creating proxy credential file");
	}
	major_status = gss_release_buffer(&minor_status,&buffer_desc);
      }
    }
  }
  return ans;
}

static ssize_t gsi_send(int conid, const void *bptr, size_t num, int options) {
  globus_size_t nbytes;
  globus_result_t result;
  GSI_INFO *info = getGsiInfo(conid);
  ssize_t sent=-1;
  if (info != 0) {
    result = globus_xio_write(info->xio_handle, (globus_byte_t *)bptr, num, num, &nbytes, NULL);
    testStatus(0,result,"mdsip_send_message globus_xio_write");
    if (result == GLOBUS_SUCCESS)
      sent=nbytes;
  }
  return sent;
}   

static ssize_t gsi_recv(int conid, void *bptr, size_t num) {
  globus_result_t result;
  GSI_INFO *info = getGsiInfo(conid);
  globus_size_t numreceived;
  ssize_t recved = -1;
  if (info != 0) {
    result = globus_xio_read(info->xio_handle, (globus_byte_t *)bptr, num, num, &numreceived, NULL);
    testStatus(info->xio_handle,result,"mdsip_get_message, globus_xio_read");
    if (result == GLOBUS_SUCCESS)
      recved = numreceived;
  }
  return recved;
}


static int gsi_disconnect(int conid) {
  int status=-1;
  GSI_INFO *info = getGsiInfo(conid);
  if (info) {
    if (info->connection_name) {
      time_t tim=time(0);
      char *timestr = ctime(&tim);
      timestr[strlen(timestr)-1]=0;
      printf("%s (pid %d) Connection disconnected from %s\r\n",timestr,getpid(),info->connection_name);
    }
    globus_result_t result = globus_xio_close(info->xio_handle,NULL);
    status = (result == GLOBUS_SUCCESS) ? 0 : -1;
  }
  return status;
}


static int gsi_reuseCheck(char *host, char *unique, size_t buflen) {
  IoRoutines *io=LoadIo("tcp");
  return (io && io->reuseCheck) ? io->reuseCheck(host, unique, buflen) : -1;
}

static int gsi_connect(int conid, char *protocol, char *host_in) {
  static int activated=0;
  static globus_xio_stack_t stack_gsi;
  static globus_xio_driver_t gsi_driver;
  static globus_xio_driver_t tcp_driver;
  globus_result_t result;
  globus_xio_handle_t xio_handle;
  globus_xio_attr_t attr;
  char *contact_string;
  char *portname;
  char *colon;
  char *host = host_in ? strcpy((char *)malloc(strlen(host_in)+1),host_in):0;
  if (!host)
    return -1;
  if ((colon=strchr(host,':'))!=0) {
    *colon=0;
    portname=colon+1;
  } else {
    portname="mdsips";
  }
  contact_string = (char *)malloc(strlen(host)+50);
  sprintf(contact_string,"%s:%d",host,GetPort(portname));
  printf("Contact string=%s\n",contact_string);
  free(host);
  
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
    result = globus_xio_stack_init(&stack_gsi, NULL);
    testStatus(0,result,"mdsip_connect globus_xio_stack_init");
    if (result != GLOBUS_SUCCESS) return -1;
    result = globus_xio_stack_push_driver(stack_gsi, tcp_driver);
    testStatus(0,result,"mdsip_connect globus_xio_stack_push_driver");
    if (result != GLOBUS_SUCCESS) return -1;
    result = globus_xio_stack_push_driver(stack_gsi, gsi_driver);
    testStatus(0,result,"mdsip_connect globus_xio_stack_push_driver");
    if (result != GLOBUS_SUCCESS) return -1;
    activated = 1;
  }
  result = globus_xio_handle_create(&xio_handle, stack_gsi);
  testStatus(0,result,"mdsip_connect globus_xio_handle_create");
  if (result != GLOBUS_SUCCESS) return -1;
  result = globus_xio_attr_init(&attr);
  testStatus(0,result,"mdsip_connect globus_xio_attr_init");
  if (result != GLOBUS_SUCCESS) return -1;
  result = globus_xio_attr_cntl(attr,gsi_driver,GLOBUS_XIO_GSI_SET_DELEGATION_MODE,
				GLOBUS_XIO_GSI_DELEGATION_MODE_FULL);
  testStatus(0,result,"mdsip_connect globus_xio_attr_cntl, set delegation");
  if (result != GLOBUS_SUCCESS) return -1;
  result = globus_xio_attr_cntl(attr,
				gsi_driver,
				GLOBUS_XIO_GSI_SET_AUTHORIZATION_MODE,
				GLOBUS_XIO_GSI_HOST_AUTHORIZATION);
  testStatus(0,result,"mdsip_connect globus_xio_attr_cntl");
  if (result != GLOBUS_SUCCESS) return -1;
  result = globus_xio_attr_cntl(attr,tcp_driver,GLOBUS_XIO_TCP_SET_SNDBUF,MDSIP_SNDBUF);
  testStatus(0,result,"mdsip_connect SET_SNDBUF");
  if (result != GLOBUS_SUCCESS) return -1;
  result = globus_xio_attr_cntl(attr,tcp_driver,GLOBUS_XIO_TCP_SET_RCVBUF,MDSIP_RCVBUF);
  testStatus(0,result,"mdsip_connect SET_RCVBUF");
  if (result != GLOBUS_SUCCESS) return -1;
  result = globus_xio_attr_cntl(attr,tcp_driver,GLOBUS_XIO_TCP_SET_NODELAY,GLOBUS_TRUE);
  testStatus(0,result,"mdsip_connect SET_NODELAY");
  if (result != GLOBUS_SUCCESS) return -1;
  result = globus_xio_attr_cntl(attr,tcp_driver,GLOBUS_XIO_TCP_SET_KEEPALIVE,GLOBUS_TRUE);
  testStatus(0,result,"mdsip_connect SET_KEEPALIVE");
  if (result != GLOBUS_SUCCESS) return -1;
  result = globus_xio_open(xio_handle, contact_string, attr);
  testStatus(0,result,"mdsip_connect globus_xio_open");
  SetConnectionInfo(conid,"tcp",0,&xio_handle,sizeof(xio_handle));
  return 0;
}

static void readCallback(
			 globus_xio_handle_t xio_handle,
			 globus_result_t result,
			 globus_byte_t *buffer,
			 globus_size_t len,
			 globus_size_t nbytes,
			 globus_xio_data_descriptor_t data_desc,
			 void *userarg) {
  int id = userarg ? *(int *)userarg : -1;
  if (id > 0) {
    GSI_INFO *info = getGsiInfo(id);
    if (info) {
      globus_result_t res;
      DoMessage(id);
      res = globus_xio_register_read(info->xio_handle, 0, 0, 0, 0, readCallback, userarg);
    }
  }
}

static void acceptCallback(
		     globus_xio_server_t server,
		     globus_xio_handle_t xio_handle,
		     globus_result_t     result,
		     void *userarg) {
  globus_result_t res;
  char *username;
  int status;
  int id;
  GSI_INFO *info_s=(GSI_INFO *)userarg;
  if (info_s != 0) {
    GSI_INFO info;
    info.tcp_driver=info_s->tcp_driver;
    info.gsi_driver=info_s->gsi_driver;
    info.xio_handle=xio_handle;
    info.connection_name=0;
    res = globus_xio_open(xio_handle,NULL,NULL);
    testStatus(0,res,"mdsip_accept_cp, open");
    if (res == GLOBUS_SUCCESS) {
      status=AcceptConnection("gsi", "gsi", 0, &info, sizeof(info), &id, &username);
      if (status & 1)
	res = globus_xio_register_read(xio_handle, 0, 0, 0, 0, readCallback, memcpy(malloc(sizeof(id)),&id,sizeof(id)));
    }
    res = globus_xio_server_register_accept(server,acceptCallback,userarg);
    testStatus(0,res,"mdsip_accept_cb, error in globus_xio_server_register_accept");
    if (res != GLOBUS_SUCCESS)
      exit(0);
  }
}

static int gsi_listen(int argc, char **argv) {
  Options options[] = {{"p","port",1,0,0},
		       {"S","--security",1,0,0},
		       {0,0,0,0,0}};
  globus_xio_stack_t stack;
  globus_result_t res;
  globus_xio_attr_t server_attr;
  char *security_level="integrity";
  GSI_INFO info;
  int sl=-1;
  ParseCommand(argc,argv,options,0,0,0);
  if (options[0].present && options[0].value)
    SetPortname(options[0].value);
  else if (GetPortname()==0)
    SetPortname("mdsip");
  if (options[1].present && options[1].value)
    security_level=options[1].value;
  if (strcmp(security_level,"none")==0)
    sl=GLOBUS_XIO_GSI_PROTECTION_LEVEL_NONE;
  else if (strcmp(security_level,"integrity")==0)
    sl=GLOBUS_XIO_GSI_PROTECTION_LEVEL_INTEGRITY;
  else if (strcmp(security_level,"privacy")==0)
    sl=GLOBUS_XIO_GSI_PROTECTION_LEVEL_PRIVACY;
  else {
    fprintf(stderr,"Invalid security level specified, must be one of none,integrity or private\n");
    exit(1);
  }
  info.connection_name=0;
  globus_mutex_init(&globus_l_mutex,NULL);
  globus_cond_init(&globus_l_cond,NULL);
  globus_module_activate(GLOBUS_XIO_MODULE);
  globus_xio_stack_init(&stack, NULL);
  globus_xio_driver_load("tcp",&info.tcp_driver);
  globus_xio_stack_push_driver(stack,info.tcp_driver);
  globus_xio_driver_load("gsi",&info.gsi_driver);
  globus_xio_stack_push_driver(stack,info.gsi_driver);
  globus_xio_attr_init(&server_attr);
  if (GetMulti()) {
    res = globus_xio_attr_cntl(server_attr, info.tcp_driver,
			       GLOBUS_XIO_TCP_SET_PORT,
			       GetPort(0));
    res = globus_xio_attr_cntl(server_attr, info.tcp_driver,
			       GLOBUS_XIO_TCP_SET_SNDBUF,
			       MDSIP_SNDBUF);
    res = globus_xio_attr_cntl(server_attr, info.tcp_driver,
			       GLOBUS_XIO_TCP_SET_RCVBUF,
			       MDSIP_RCVBUF);
    res = globus_xio_attr_cntl(server_attr, info.tcp_driver,
			       GLOBUS_XIO_TCP_SET_NODELAY,
			       GLOBUS_TRUE);
    res = globus_xio_attr_cntl(server_attr, info.tcp_driver,
			       GLOBUS_XIO_TCP_SET_KEEPALIVE,
			       GLOBUS_TRUE);
  } else {
    globus_xio_attr_cntl(server_attr, info.tcp_driver,
			 GLOBUS_XIO_TCP_SET_HANDLE,
			 STDIN_FILENO);
  }
        
  globus_xio_attr_cntl(server_attr, info.gsi_driver,
		       GLOBUS_XIO_GSI_SET_PROTECTION_LEVEL,
		       sl);
  globus_xio_attr_cntl(server_attr, info.gsi_driver,
		       GLOBUS_XIO_GSI_SET_AUTHORIZATION_MODE,
		       GLOBUS_XIO_GSI_NO_AUTHORIZATION);
  globus_xio_attr_cntl(server_attr, info.gsi_driver,
		       GLOBUS_XIO_GSI_SET_DELEGATION_MODE,
		       GLOBUS_XIO_GSI_DELEGATION_MODE_FULL);
  if (GetMulti()) {
    res = globus_xio_server_create((globus_xio_server_t *)&info.xio_handle, server_attr, stack);
    testStatus(0,res,"gsi_listen,server_create");
    if (res == GLOBUS_SUCCESS) {
      //      int id = NewConnection("gsi");
      // SetConnectionInfo(id, "gsi", 0, &info, sizeof(info));
      res = globus_xio_server_register_accept((globus_xio_server_t)info.xio_handle,acceptCallback,&info);
      if (res != GLOBUS_SUCCESS)
	exit(1);
      else {
	while (1) {
	  res = globus_mutex_lock(&globus_l_mutex);
	  res = globus_cond_wait(&globus_l_cond,&globus_l_mutex);
	  res = globus_mutex_unlock(&globus_l_mutex);
	}
      }
    }
  } else {
    char *username;
    int status;
    int id;
    globus_xio_attr_cntl(server_attr, info.gsi_driver, GLOBUS_XIO_GSI_FORCE_SERVER_MODE, GLOBUS_TRUE);
    globus_xio_handle_create(&info.xio_handle,stack);
    res = globus_xio_open(info.xio_handle, NULL, server_attr);
    testStatus(0,res,"get handle to connection");
    status=AcceptConnection("gsi", "gsi", 0, &info, sizeof(info), &id, &username);
    while (status & 1) {
      status = DoMessage(id);
    }
  }
  return 0;
}
