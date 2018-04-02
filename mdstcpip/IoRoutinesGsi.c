/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "globus_common.h"
#include "globus_xio.h"
#include "globus_xio_gsi.h"
#include "globus_gss_assist.h"
#include "globus_xio_tcp_driver.h"

// Undefine symbols defined again in config.h
#ifdef HAVE_STDARG_H
#undef HAVE_STDARG_H
#endif
#ifdef HAVE_GETADDRINFO
#undef HAVE_GETADDRINFO
#endif
#ifdef HAVE_GETPWUID
#undef HAVE_GETPWUID
#endif
#ifdef _GNU_SOURCE
#undef _GNU_SOURCE
#endif
#ifdef SIZEOF_LONG
#undef SIZEOF_LONG
#endif

#include <STATICdef.h>
#include <signal.h>
#include <status.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <mdsplus/mdsconfig.h>
#include <time.h>
#include <sys/wait.h>

#include "mdsip_connections.h"

static ssize_t gsi_send(Connection* c, const void *buffer, size_t buflen, int nowait);
static ssize_t gsi_recv(Connection* c, void *buffer, size_t len);
static int gsi_disconnect(Connection* c);
static int gsi_listen(int argc, char **argv);
static int gsi_authorize(Connection* c, char *username);
static int gsi_connect(Connection* c, char *protocol, char *host);
static int gsi_reuseCheck(char *host, char *unique, size_t buflen);
static IoRoutines gsi_routines = {
  gsi_connect, gsi_send, gsi_recv, NULL, gsi_listen, gsi_authorize, gsi_reuseCheck, gsi_disconnect, NULL
};

static int MDSIP_SNDBUF = 32768;
static int MDSIP_RCVBUF = 32768;

static globus_mutex_t globus_l_mutex;
static globus_cond_t globus_l_cond;

typedef struct _gsi_info {
  globus_xio_handle_t xio_handle;
  globus_xio_driver_t gsi_driver;
  globus_xio_driver_t tcp_driver;
  char *connection_name;
} GSI_INFO;

EXPORT IoRoutines *Io(){
  return &gsi_routines;
}

static short GetPort(char *pname)
{
  short port;
  char *name = pname ? pname : GetPortname();
  struct servent *sp;
  if (name == 0 || strcmp(name, "mdsip") == 0)
    name = "mdsips";
  port = htons((short)atoi(name));
  if (port == 0) {
    sp = getservbyname(name, "tcp");
    if (sp == NULL) {
      fprintf(stderr, "unknown service: %s/tcp\n\n", name);
      exit(0);
    }
    port = sp->s_port;
  }
  return ntohs(port);
}

static void testStatus(globus_result_t res, char *msg)
{
  if (res != GLOBUS_SUCCESS) {
    fprintf(stderr, "ERROR:%s ---\n       %s\n",
	    msg, globus_object_printable_to_string(globus_error_get(res)));
  }
}

static GSI_INFO *getGsiInfoC(Connection* c)
{
  size_t len;
  char *info_name;
  int readfd;
  GSI_INFO *info = (GSI_INFO *) GetConnectionInfoC(c, &info_name, &readfd, &len);
  return (info_name && strcmp(info_name, "gsi") == 0) && len == sizeof(GSI_INFO) ? info : 0;
}
static GSI_INFO *getGsiInfo(int conid)
{
  size_t len;
  char *info_name;
  int readfd;
  GSI_INFO *info = (GSI_INFO *) GetConnectionInfo(conid, &info_name, &readfd, &len);
  return (info_name && strcmp(info_name, "gsi") == 0) && len == sizeof(GSI_INFO) ? info : 0;
}

#define doit(statvar,gfuncall,msg,fail_action) \
  statvar=gfuncall;\
  testStatus(statvar,msg);\
  if (statvar!=GLOBUS_SUCCESS) fail_action

static int gsi_authorize(Connection* c, char *username)
{
  GSI_INFO *info = getGsiInfoC(c);
  int ans = 0;
  if (info) {
    char *hostname;
    char *hostip;
    globus_result_t res;
    gss_buffer_desc peer_name_buffer = GSS_C_EMPTY_BUFFER;
    OM_uint32 status, mstatus;
    gss_name_t peer;
    char *match_string[2] = { 0, 0 };
    time_t tim = time(0);
    char *timestr = ctime(&tim);
    timestr[strlen(timestr) - 1] = 0;
    doit(res,
	 globus_xio_handle_cntl(info->xio_handle, info->tcp_driver,
				GLOBUS_XIO_TCP_GET_REMOTE_CONTACT, &hostname), "Get Remote Contact",
	 return C_ERROR);
    doit(res,
	 globus_xio_handle_cntl(info->xio_handle, info->tcp_driver,
				GLOBUS_XIO_TCP_GET_REMOTE_NUMERIC_CONTACT, &hostip),
	 "Get Remote Numeric Contact", return 0);
    doit(res,
	 globus_xio_handle_cntl(info->xio_handle, info->gsi_driver, GLOBUS_XIO_GSI_GET_PEER_NAME,
				&peer), "Get Peer Name", return 0);
    status = gss_display_name(&mstatus, peer, &peer_name_buffer, GLOBUS_NULL);
    //gss_release_name(&mstatus,&peer);
    if (status != GSS_S_COMPLETE) {
      fprintf(stderr, "Error getting display name\n");
      return C_ERROR;
    }
    match_string[0] = (char *)malloc(strlen(hostname) + strlen((char *)peer_name_buffer.value) + 2);
    match_string[1] = (char *)malloc(strlen(hostip) + strlen((char *)peer_name_buffer.value) + 2);
    strcpy(match_string[0], (char *)peer_name_buffer.value);
    strcpy(match_string[1], match_string[0]);
    strcat(match_string[0], "@");
    strcat(match_string[1], "@");
    strcat(match_string[0], hostname);
    strcat(match_string[1], &hostip[8]);
    match_string[0][strlen((char *)peer_name_buffer.value) + 1 + strcspn(hostname, ":")] = 0;
    match_string[1][strlen((char *)peer_name_buffer.value) + strcspn(&hostip[8], ":")] = 0;
    info->connection_name =
	malloc(strlen(match_string[0]) + strlen(match_string[1]) + strlen(username));
    sprintf(info->connection_name, "%s - %s@%s [%s]", (char *)peer_name_buffer.value, username,
	    &match_string[0][strlen((char *)peer_name_buffer.value) + 1],
	    &match_string[1][strlen((char *)peer_name_buffer.value) + 1]);
    printf("%s (pid %d) Connection received from %s\r\n", timestr, getpid(), info->connection_name);
    //gss_release_buffer(&status,&peer_name_buffer);
    ans = CheckClient(username, 2, match_string);
    if (ans && GetMulti() == 0) {
      gss_cred_id_t credential;
      OM_uint32 major_status, minor_status;
      gss_buffer_desc buffer_desc = GSS_C_EMPTY_BUFFER;
      res = globus_xio_handle_cntl(info->xio_handle, info->gsi_driver,
				   GLOBUS_XIO_GSI_GET_DELEGATED_CRED, &credential);
      testStatus(res, "openCallback,GET_DELEGATED_CRED");
      major_status = gss_export_cred(&minor_status, credential, GSS_C_NO_OID, 0, &buffer_desc);
      if (major_status != GSS_S_COMPLETE) {
	char *error_str;
	globus_gss_assist_display_status_str(&error_str, NULL, major_status, minor_status, 0);
	fprintf(stderr, "\nLINE %d ERROR: %s\n\n", __LINE__, error_str);
      } else {
	char cred_file_name[] = "/tmp/x509pp_pXXXXXX";
	int fd = mkstemp(cred_file_name);
	if (fd != -1) {
	  fchmod(fd, 00600);
	  if (write(fd, buffer_desc.value, buffer_desc.length) == -1)
	    perror("Error in gsi_authorize writing to temporary file\n");
	  fchmod(fd, 00400);
	  close(fd);
	  setenv("X509_USER_PROXY", cred_file_name, 1);
	} else {
	  perror("Error creating proxy credential file");
	}
	//major_status = gss_release_buffer(&minor_status,&buffer_desc);
      }
    }
  }
  return ans;
}

static ssize_t gsi_send(Connection* c, const void *bptr, size_t num, int options __attribute__ ((unused)))
{
  globus_size_t nbytes;
  globus_result_t result;
  GSI_INFO *info = getGsiInfoC(c);
  ssize_t sent = -1;
  if (info != 0) {
    doit(result,
	 globus_xio_write(info->xio_handle, (globus_byte_t *) bptr, num, num, &nbytes, NULL),
	 "GSI write", return -1);
    sent = nbytes;
  }
  return sent;
}

static ssize_t gsi_recv(Connection* c, void *bptr, size_t num)
{
  GSI_INFO *info = getGsiInfoC(c);
  globus_size_t numreceived;
  globus_result_t result;
  ssize_t recved = -1;
  if (info != 0) {
    doit(result,
	 globus_xio_read(info->xio_handle, (globus_byte_t *) bptr, num, num, &numreceived, NULL),
	 "GSI read", return -1);
    recved = numreceived;
  }
  return recved;
}

static int gsi_disconnect(Connection* c){
  GSI_INFO *info = getGsiInfoC(c);
  if (info) {
    if (info->connection_name) {
      time_t tim = time(0);
      char *timestr = ctime(&tim);
      timestr[strlen(timestr) - 1] = 0;
      printf("%s (pid %d) Connection disconnected from %s\r\n", timestr, getpid(),
	     info->connection_name);
      free(info->connection_name);
    }
    globus_result_t result = globus_xio_close(info->xio_handle, NULL);
    return (result == GLOBUS_SUCCESS) ? C_OK : C_ERROR;
  }
  return C_ERROR;
}

static int gsi_reuseCheck(char *host, char *unique, size_t buflen)
{
  int ans;
  IoRoutines *io = LoadIo("tcp");
  char *hoststr = strcpy((char *)malloc(strlen(host) + 10), host);
  if ((strchr(host, ':')) == 0) {
    strcat(hoststr, ":");
    strcat(hoststr, "mdsips");
  }
  ans = (io && io->reuseCheck) ? io->reuseCheck(hoststr, unique, buflen) : -1;
  if (strlen(unique) > 4 && strncmp("tcp:", unique, 4) == 0) {
    unique[0] = 'g';
    unique[1] = 's';
    unique[2] = 'i';
  }
  free(hoststr);
  return ans;
}

static int gsi_connect(Connection* c, char *protocol __attribute__ ((unused)), char *host_in)
{
  static int activated = 0;
  static globus_xio_stack_t stack_gsi;
  globus_result_t result;
  globus_xio_attr_t attr;
  char *contact_string;
  char *portname;
  char *colon;
  char *host = host_in ? strcpy((char *)malloc(strlen(host_in) + 1), host_in) : 0;
  GSI_INFO info;
  if (!host)
    return C_ERROR;
  info.connection_name = 0;
  if ((colon = strchr(host, ':')) != 0) {
    *colon = 0;
    portname = colon + 1;
  } else {
    portname = "mdsips";
  }
  contact_string = (char *)malloc(strlen(host) + 50);
  sprintf(contact_string, "%s:%d", host, GetPort(portname));
  free(host);

  if (activated == 0) {
    doit(result, globus_module_activate(GLOBUS_XIO_MODULE),
       "GSI XIO Activate", return C_ERROR);
    activated = 1;
  }
  doit(result, globus_xio_driver_load("tcp", &info.tcp_driver),
       "GSI Load TCP", return C_ERROR);
  doit(result, globus_xio_driver_load("gsi", &info.gsi_driver),
       "GSI Load GSI", return C_ERROR);
  doit(result, globus_xio_stack_init(&stack_gsi, NULL),
       "GSI Init stack", return C_ERROR);
  doit(result, globus_xio_stack_push_driver(stack_gsi, info.tcp_driver),
       "GSI Push TCP", return C_ERROR);
  doit(result, globus_xio_stack_push_driver(stack_gsi, info.gsi_driver),
       "GSI Push GSI", return C_ERROR);
  doit(result, globus_xio_handle_create(&info.xio_handle, stack_gsi),
       "GSI Create Handle", return C_ERROR);
  doit(result, globus_xio_attr_init(&attr),
       "GSI Init Attr", return C_ERROR);
  doit(result, globus_xio_attr_cntl(attr, info.gsi_driver, GLOBUS_XIO_GSI_SET_DELEGATION_MODE, GLOBUS_XIO_GSI_DELEGATION_MODE_FULL),
       "GSI Set Delegation", return C_ERROR);
  doit(result, globus_xio_attr_cntl(attr, info.gsi_driver, GLOBUS_XIO_GSI_SET_AUTHORIZATION_MODE, GLOBUS_XIO_GSI_HOST_AUTHORIZATION),
       "GSI Set Authorization", return -1);
  doit(result, globus_xio_attr_cntl(attr, info.tcp_driver, GLOBUS_XIO_TCP_SET_SNDBUF, MDSIP_SNDBUF),
       "GSI Set SNDBUF", return C_ERROR);
  doit(result, globus_xio_attr_cntl(attr, info.tcp_driver, GLOBUS_XIO_TCP_SET_RCVBUF, MDSIP_RCVBUF),
       "GSI Set RCVBUF", return C_ERROR);
  doit(result, globus_xio_attr_cntl(attr, info.tcp_driver, GLOBUS_XIO_TCP_SET_NODELAY, GLOBUS_TRUE),
       "GSI Set NODELAY", return C_ERROR);
  doit(result, globus_xio_attr_cntl(attr, info.tcp_driver, GLOBUS_XIO_TCP_SET_KEEPALIVE, GLOBUS_TRUE),
       "GSI Set KEEPALIVE", return C_ERROR);
  doit(result, globus_xio_open(info.xio_handle, contact_string, attr),
       "Error connecting",  return C_ERROR);
  SetConnectionInfoC(c, "gsi", 0, &info, sizeof(info));
  return C_OK;
}

static void readCallback(globus_xio_handle_t xio_handle __attribute__ ((unused)),
			 globus_result_t result __attribute__ ((unused)),
			 globus_byte_t * buffer __attribute__ ((unused)),
			 globus_size_t len __attribute__ ((unused)),
			 globus_size_t nbytes __attribute__ ((unused)),
			 globus_xio_data_descriptor_t data_desc __attribute__ ((unused)), void *userarg)
{
  int id = userarg ? *(int *)userarg : -1;
  if (id > 0) {
    GSI_INFO *info = getGsiInfo(id);
    if (info) {
      globus_result_t res __attribute__ ((unused));
      globus_byte_t buff[1];
      int status = DoMessage(id);
      if STATUS_OK
	res = globus_xio_register_read(info->xio_handle, buff, 0, 0, 0, readCallback, userarg);
    }
  }
}

static void acceptCallback(globus_xio_server_t server,
			   globus_xio_handle_t xio_handle, globus_result_t result __attribute__ ((unused)), void *userarg)
{
  globus_result_t res;
  char *username;
  int status;
  int id;
  GSI_INFO *info_s = (GSI_INFO *) userarg;
  if (info_s != 0) {
    GSI_INFO info;
    info.tcp_driver = info_s->tcp_driver;
    info.gsi_driver = info_s->gsi_driver;
    info.xio_handle = xio_handle;
    info.connection_name = 0;
    res = globus_xio_open(xio_handle, NULL, NULL);
    testStatus(res, "mdsip_accept_cp, open");
    if (res == GLOBUS_SUCCESS) {
      status = AcceptConnection("gsi", "gsi", 0, &info, sizeof(info), &id, &username);
      if STATUS_OK {
	globus_byte_t buff[1];
	doit(res,
	     globus_xio_register_read(xio_handle, buff, 0, 0, 0, readCallback,
				      memcpy(malloc(sizeof(id)), &id, sizeof(id))),
	     "GSI Register Read", DisconnectFromMds(id));
      }
    }
    res = globus_xio_server_register_accept(server, acceptCallback, userarg);
    testStatus(res, "mdsip_accept_cb, error in globus_xio_server_register_accept");
    if (res != GLOBUS_SUCCESS)
      exit(0);
  }
}

static int gsi_listen(int argc, char **argv)
{
  Options options[] = { {"p", "port", 1, 0, 0},
  {"S", "security", 1, 0, 0},
  {0, 0, 0, 0, 0}
  };
  globus_xio_stack_t stack;
  globus_result_t res;
  globus_xio_attr_t server_attr;
  char *security_level = "integrity";
  GSI_INFO info;
  int sl = -1;
  ParseCommand(argc, argv, options, 0, 0, 0);
  if (options[0].present && options[0].value)
    SetPortname(options[0].value);
  else if (GetPortname() == 0)
    SetPortname("mdsip");
  if (options[1].present && options[1].value)
    security_level = options[1].value;
  if (strcmp(security_level, "none") == 0)
    sl = GLOBUS_XIO_GSI_PROTECTION_LEVEL_NONE;
  else if (strcmp(security_level, "integrity") == 0)
    sl = GLOBUS_XIO_GSI_PROTECTION_LEVEL_INTEGRITY;
  else if (strcmp(security_level, "privacy") == 0)
    sl = GLOBUS_XIO_GSI_PROTECTION_LEVEL_PRIVACY;
  else {
    fprintf(stderr, "Invalid security level specified, must be one of none,integrity or private\n");
    exit(C_ERROR);
  }
  info.connection_name = 0;
  globus_mutex_init(&globus_l_mutex, NULL);
  globus_cond_init(&globus_l_cond, NULL);
  globus_module_activate(GLOBUS_XIO_MODULE);
  globus_xio_stack_init(&stack, NULL);
  globus_xio_driver_load("tcp", &info.tcp_driver);
  globus_xio_stack_push_driver(stack, info.tcp_driver);
  globus_xio_driver_load("gsi", &info.gsi_driver);
  globus_xio_stack_push_driver(stack, info.gsi_driver);
  globus_xio_attr_init(&server_attr);
  if (GetMulti()) {
    res = globus_xio_attr_cntl(server_attr, info.tcp_driver, GLOBUS_XIO_TCP_SET_PORT, GetPort(0));
    res = globus_xio_attr_cntl(server_attr, info.tcp_driver,
			       GLOBUS_XIO_TCP_SET_SNDBUF, MDSIP_SNDBUF);
    res = globus_xio_attr_cntl(server_attr, info.tcp_driver,
			       GLOBUS_XIO_TCP_SET_RCVBUF, MDSIP_RCVBUF);
    res = globus_xio_attr_cntl(server_attr, info.tcp_driver,
			       GLOBUS_XIO_TCP_SET_NODELAY, GLOBUS_TRUE);
    res = globus_xio_attr_cntl(server_attr, info.tcp_driver,
			       GLOBUS_XIO_TCP_SET_KEEPALIVE, GLOBUS_TRUE);
  } else {
    globus_xio_attr_cntl(server_attr, info.tcp_driver, GLOBUS_XIO_TCP_SET_HANDLE, STDIN_FILENO);
  }

  globus_xio_attr_cntl(server_attr, info.gsi_driver, GLOBUS_XIO_GSI_SET_PROTECTION_LEVEL, sl);
  globus_xio_attr_cntl(server_attr, info.gsi_driver,
		       GLOBUS_XIO_GSI_SET_AUTHORIZATION_MODE, GLOBUS_XIO_GSI_NO_AUTHORIZATION);
  globus_xio_attr_cntl(server_attr, info.gsi_driver,
		       GLOBUS_XIO_GSI_SET_DELEGATION_MODE, GLOBUS_XIO_GSI_DELEGATION_MODE_FULL);
  if (GetMulti()) {
    res = globus_xio_server_create((globus_xio_server_t *) & info.xio_handle, server_attr, stack);
    testStatus(res, "gsi_listen,server_create");
    if (res == GLOBUS_SUCCESS) {
      res =
	  globus_xio_server_register_accept((globus_xio_server_t) info.xio_handle, acceptCallback,
					    &info);
      if (res != GLOBUS_SUCCESS)
	exit(C_ERROR);
      else {
	while (1) {
	  res = globus_mutex_lock(&globus_l_mutex);
	  res = globus_cond_wait(&globus_l_cond, &globus_l_mutex);
	  res = globus_mutex_unlock(&globus_l_mutex);
	}
      }
    }
  } else {
    char *username;
    int status;
    int id;
    globus_xio_attr_cntl(server_attr, info.gsi_driver, GLOBUS_XIO_GSI_FORCE_SERVER_MODE,
			 GLOBUS_TRUE);
    globus_xio_handle_create(&info.xio_handle, stack);
    res = globus_xio_open(info.xio_handle, NULL, server_attr);
    testStatus(res, "get handle to connection");
    status = AcceptConnection("gsi", "gsi", 0, &info, sizeof(info), &id, &username);
    while STATUS_OK {
      status = DoMessage(id);
    }
  }
  return C_OK;
}
