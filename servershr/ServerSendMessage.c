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
/*------------------------------------------------------------------------------

                Name:   ServerSendMessage

                Type:   C function

                Author:	TOM FREDIAN

                Date:   17-APR-1992

                Purpose: Send message to server.

------------------------------------------------------------------------------

        Call sequence:

int ServerSendMessage();

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

        Description:

------------------------------------------------------------------------------*/
#include <mdsplus/mdsconfig.h>

#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <socket_port.h>
#include <condition.h>
#include <ipdesc.h>
#include <servershr.h>
#include <mds_stdarg.h>
#include <mdsshr.h>
#include <_mdsshr.h>
#include <libroutines.h>
#define _NO_SERVER_SEND_MESSAGE_PROTO
#include "servershrp.h"

//#define DEBUG
#include "Client.h"

extern short ArgLen();

extern int GetAnswerInfoTS();

int is_broken_socket(SOCKET socket);

EXPORT int ServerConnect(char *server);
static int start_receiver(uint16_t *port);
static void receiver_thread(void *sockptr);
static void accept_client(SOCKET reply_sock, struct sockaddr_in *sin, fd_set *fdactive);

extern void *GetConnectionInfo();

static SOCKET get_socket_by_conid(int conid)
{
  size_t len;
  char *info_name = NULL;
  SOCKET readfd = INVALID_SOCKET;
  GetConnectionInfo(conid, &info_name, &readfd, &len);
  if (info_name && (strcmp(info_name, "tcp") == 0))
    return readfd;
  return INVALID_SOCKET;
}

int ServerSendMessage(int *msgid, char *server, int op, int *retstatus,
                      pthread_rwlock_t *lock, int *conid_out, void (*callback_done)(),
                      void *callback_param, void (*callback_before)(), int numargs_in,
                      ...)
{
  uint16_t port = 0;
  int conid = INVALID_CONNECTION_ID;
  MDSDBG("%s", server);
  const int receiver_err = start_receiver(&port);
  if (receiver_err || ((conid = ServerConnect(server)) == INVALID_CONNECTION_ID))
  {
    if (receiver_err)
    {
      MDSWRN("failed to start receiver");
    }
    else
    {
      MDSWRN("failed to connect");
    }
    if (callback_done)
      callback_done(callback_param);
    return ServerPATH_DOWN;
  }
  INIT_STATUS;
  int flags = 0;
  int jobid;
  int i;
  uint32_t addr = 0;
  char cmd[4096];
  char *ccmd = cmd;
  uint8_t numargs = numargs_in < 0 ? 0 : (numargs_in > 8 ? 8 : numargs_in); // minmax [0, 8]
  char dtype;
  char ndims;
  int dims[8];
  int numbytes;
  int *dptr;
  va_list vlist;
  void *mem = NULL;
  struct descrip *arg;
  if (conid_out)
    *conid_out = conid;
  SOCKET sock = get_socket_by_conid(conid);
  struct sockaddr_in addr_struct = {0};
  socklen_t len = sizeof(addr_struct);
  if (getsockname(sock, (struct sockaddr *)&addr_struct, &len) == 0)
    addr = *(uint32_t *)&addr_struct.sin_addr;
  if (!addr)
  {
    MDSWRN("could not resolve address the socket is bound to");
    if (callback_done)
      callback_done(callback_param);
    return ServerSOCKET_ADDR_ERROR;
  }
  jobid = Job_register(msgid, conid, retstatus, lock, callback_done, callback_param, callback_before);
  if (callback_before)
    flags |= SrvJobBEFORE_NOTIFY;
  ccmd += sprintf(ccmd, "MdsServerShr->ServerQAction(%ulu,%uwu,%d,%d,%d", addr, port, op, flags, jobid);
  va_start(vlist, numargs_in);
  for (i = 0; i < numargs; i++)
  {
    *ccmd++ = ',';
    arg = va_arg(vlist, struct descrip *);
    if (op == SrvMonitor && numargs == 8 && i == 5 &&
        arg->dtype == DTYPE_LONG && *(int *)arg->ptr == MonitorCheckin)
      MonJob = jobid;
    switch (arg->dtype)
    {
    case DTYPE_CSTRING:
    {
      int j;
      char *c = (char *)arg->ptr;
      int len = strlen(c);
      ccmd += sprintf(ccmd, "\"");
      for (j = 0; j < len; j++)
      {
        if (c[j] == '"' || c[j] == '\\')
          *ccmd++ = '\\';
        *ccmd++ = c[j];
      }
      ccmd += sprintf(ccmd, "\"");
      break;
    }
    case DTYPE_LONG:
      ccmd += sprintf(ccmd, "%d", *(int *)arg->ptr);
      break;
    case DTYPE_CHAR:
      ccmd += sprintf(ccmd, "%d", (int)*(char *)arg->ptr);
      break;
    default:
      MDSWRN("unexpected dtype = %d", arg->dtype);
    }
  }
  *ccmd++ = ')';
  status = SendArg(conid, 0, DTYPE_CSTRING, 1, (short)(ccmd - cmd), 0, 0, cmd);
  if (STATUS_NOT_OK)
  {
    MDSWRN("could not sending message to server");
    Job_cleanup(status, jobid);
    return status;
  }
  status = GetAnswerInfoTS(conid, &dtype, &len, &ndims, dims, &numbytes,
                           (void **)&dptr, &mem);
  if (op == SrvStop)
  {
    if (STATUS_NOT_OK)
    {
      status = MDSplusSUCCESS;
      Job_cleanup(status, jobid);
    }
    else
    {
      status = MDSplusERROR;
      Job_cleanup(status, jobid);
    }
  }
  else
  {
    if (STATUS_NOT_OK)
    {
      MDSWRN("no response from server");
      Job_cleanup(status, jobid);
      return status;
    }
  }
  free(mem);
  MDSDBG("status=%d", status);
  return status;
}

EXPORT void ServerWait(int jobid)
{
  if (Job_wait_and_pop_by_jobid(jobid))
    MDSDBG("Job(%d, ?) sync lost!", jobid);
}

DEFINE_INITIALIZESOCKETS;
static SOCKET new_reply_socket(uint16_t *port_out)
{
  static uint16_t start_port = 0, range_port;
  if (!start_port)
  {
    char *range = TranslateLogical("MDSIP_PORT_RANGE");
    if (range)
    {
      char *dash;
      for (dash = range; *dash && *dash != '-'; dash++)
        ;
      if (dash)
        *(dash++) = 0;
      start_port = (uint16_t)(strtol(range, NULL, 0) & 0xffff);
      int end = strtol(dash, NULL, 0);
      if (end > 0 && end < 65536)
        range_port = (uint16_t)end - start_port + 1;
      else
        range_port = 100;
    }
    TranslateLogicalFree(range);
    if (!start_port)
    {
      start_port = 8800;
      range_port = 200;
    }
    MDSDBG("Receiver will be using 'MDSIP_PORT_RANGE=%u-%u'.", start_port,
           start_port + range_port - 1);
  }
  int one = 1;
  INITIALIZESOCKETS;
  SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == INVALID_SOCKET)
  {
    print_socket_error("Error getting Connection Socket");
    return s;
  }
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one));
  int c_status = C_ERROR;
  static struct sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  uint16_t port = start_port;
  const uint16_t end_port = start_port + range_port;
  do
  {
    sin.sin_port = htons(port);
    c_status = bind(s, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
  } while (c_status && (++port != end_port));
  if (c_status)
  {
    print_socket_error("Error binding to service\n");
    return INVALID_SOCKET;
  }
  c_status = listen(s, 5);
  if (c_status)
  {
    print_socket_error("Error from listen\n");
    return INVALID_SOCKET;
  }
  MDSDBG("Listener opened on port %u.", port);
  *port_out = port;
  return s;
}

static Condition ReceiverRunning = CONDITION_INITIALIZER;

static int start_receiver(uint16_t *port_out)
{
  INIT_STATUS;
  static uint16_t port = 0;
  static SOCKET sock;
  static pthread_t thread;
  _CONDITION_LOCK(&ReceiverRunning);
  if (port == 0)
  {
    sock = new_reply_socket(&port);
    if (sock == INVALID_SOCKET)
    {
      MDSWRN("INVALID_SOCKET");
      _CONDITION_UNLOCK(&ReceiverRunning);
      return C_ERROR;
    }
  }
  if (!ReceiverRunning.value)
  {
    CREATE_DETACHED_THREAD(thread, *16, receiver_thread, &sock);
    if (c_status)
    {
      perror("Error creating pthread");
      status = MDSplusERROR;
    }
    else
    {
      _CONDITION_WAIT_SET(&ReceiverRunning);
      status = MDSplusSUCCESS;
    }
  }
  _CONDITION_UNLOCK(&ReceiverRunning);
  *port_out = port;
  return STATUS_NOT_OK;
}

static void receiver_atexit(void *arg)
{
  (void)arg;
  MDSDBG("ServerSendMessage thread exitted");
  CONDITION_RESET(&ReceiverRunning);
}

static void reset_fdactive(int rep, SOCKET server, fd_set *fdactive)
{
  LOCK_CLIENTS;
  Client *c;
  if (rep > 0)
  {
    for (c = Clients; c;)
    {
      if (is_broken_socket(c->reply_sock))
      {
        MDSWRN(CLIENT_PRI " removed", CLIENT_VAR(c));
        Client *o = c;
        c = c->next;
        Client_cleanup_jobs(o, fdactive);
      }
      else
        c = c->next;
    }
  }
  FD_ZERO(fdactive);
  FD_SET(server, fdactive);
  for (c = Clients; c; c = c->next)
  {
    if (c->reply_sock != INVALID_SOCKET)
      FD_SET(c->reply_sock, fdactive);
  }
  UNLOCK_CLIENTS;
  MDSWRN("reset fdactive in reset_fdactive");
}

static void receiver_thread(void *sockptr)
{
  atexit((void *)receiver_atexit);
  // CONDITION_SET(&ReceiverRunning);
  _CONDITION_LOCK(&ReceiverRunning);
  SOCKET sock = *(SOCKET *)sockptr;
  ReceiverRunning.value = B_TRUE;
  _CONDITION_SIGNAL(&ReceiverRunning);
  _CONDITION_UNLOCK(&ReceiverRunning);
  // \CONDITION_SET(&ReceiverRunning);
  struct sockaddr_in sin;
  uint32_t last_client_addr = 0;
  uint16_t last_client_port = 0;
  fd_set readfds, fdactive;
  FD_ZERO(&fdactive);
  FD_SET(sock, &fdactive);
  int rep;
  struct timeval readto, timeout = {10, 0};
  for (rep = 0; rep < 10; rep++)
  {
    for (readfds = fdactive, readto = timeout;;
         readfds = fdactive, readto = timeout, rep = 0)
    {
      int num = select(FD_SETSIZE, &readfds, NULL, NULL, &readto);
      if (num < 0)
        break;
      if (num == 0)
        continue;
      if (FD_ISSET(sock, &readfds))
      {
        socklen_t len = sizeof(struct sockaddr_in);
        accept_client(accept(sock, (struct sockaddr *)&sin, &len), &sin, &fdactive);
        num--;
      }
      {
        Client *c;
        for (;;)
        {
          LOCK_CLIENTS;
          for (c = Clients; c; c = c->next)
          {
            if ((c->reply_sock != INVALID_SOCKET) && FD_ISSET(c->reply_sock, &readfds))
            {
              last_client_addr = c->addr;
              last_client_port = c->port;
              break;
            }
          }
          UNLOCK_CLIENTS;
          if (c)
          {
            FD_CLR(c->reply_sock, &readfds);
            Client_do_message(c, &fdactive);
            num--;
          }
          else
          {
            if (num)
              MDSDBG("num not 0 but %d", num);
            break;
          }
        }
      }
    }
    print_socket_error("Dispatcher select loop failed\n");
    fprintf(stderr, "Last client: " IPADDRPRI ":%u\n", IPADDRVAR(&last_client_addr), last_client_port);
    reset_fdactive(rep, sock, &fdactive);
  }
  fprintf(stderr,
          "Cannot recover from select errors in ServerSendMessage, exitting\n");
  pthread_exit(0);
}

int is_broken_socket(SOCKET socket)
{
  if (socket != INVALID_SOCKET)
  {
    fd_set fdactive;
    FD_ZERO(&fdactive);
    FD_SET(socket, &fdactive);
    struct timeval timeout = {0, 0}; // non-blocking
    return select(socket + 1, &fdactive, 0, 0, &timeout) < 0;
  }
  return B_TRUE;
}

int get_addr_port(char *hostin, uint32_t *addrp, uint16_t *portp)
{
  int err;
  char *port = strchr(hostin, ':');
  struct sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  if (port)
  {
    int hostlen = port - hostin;
    char *host = memcpy(malloc(hostlen + 1), hostin, hostlen);
    FREE_ON_EXIT(host);
    host[hostlen] = 0;
    err = _LibGetHostAddr(host, port + 1, (struct sockaddr *)&sin);
    FREE_NOW(host);
    if (!err)
    {
      *portp = ntohs(sin.sin_port);
      *addrp = sin.sin_addr.s_addr;
    }
  }
  else
  {
    err = _LibGetHostAddr(hostin, NULL, (struct sockaddr *)&sin);
    if (!err)
    {
      *portp = 8000;
      *addrp = sin.sin_addr.s_addr;
    }
  }
  return err;
}

EXPORT int ServerDisconnect(char *server_in)
{
  char *srv = TranslateLogical(server_in);
  char *server = srv ? srv : server_in;
  uint32_t addr;
  uint16_t port;
  const int err = get_addr_port(server, &addr, &port);
  free(srv);
  if (err)
    return MDSplusERROR;
  int status;
  LOCK_CLIENTS;
  Client *c = Client_get_by_addr_and_port_locked(addr, port);
  if (c)
  {
    MDSDBG(CLIENT_PRI, CLIENT_VAR(c));
    Client_remove_locked(c, NULL);
    status = MDSplusSUCCESS;
  }
  else
  {
    status = MDSplusERROR;
  }
  UNLOCK_CLIENTS;
  return status;
}

static inline int server_connect(char *server, uint32_t addr, uint16_t port)
{
  int conid;
  LOCK_CLIENTS;
  conid = ConnectToMds(server);
  if (conid != INVALID_CONNECTION_ID)
  {
    Client *c = newClient(addr, port, conid);
    MDSDBG(CLIENT_PRI " connected to %s", CLIENT_VAR(c), server);
    Client_push_locked(c);
  }
  else
  {
    MDSWRN("Could not connect to %s (" IPADDRPRI ":%d)", server, IPADDRVAR(&addr), port);
  }
  UNLOCK_CLIENTS;
  return conid;
}

EXPORT int ServerConnect(char *server_in)
{

  char *srv = TranslateLogical(server_in);
  char *server = srv ? srv : server_in;
  uint32_t addr;
  uint16_t port = 0;
  if (get_addr_port(server, &addr, &port))
  {
    MDSWRN("Could not resolve %s", server);
    free(srv);
    return INVALID_CONNECTION_ID;
  }
  int conid = server_connect(server, addr, port);
  free(srv);
  return conid;
}

static void accept_client(SOCKET reply_sock, struct sockaddr_in *sin, fd_set *fdactive)
{
  if (reply_sock == INVALID_SOCKET)
    return;
  uint32_t addr = *(uint32_t *)&sin->sin_addr;
  uint16_t port = ntohs(sin->sin_port);
  Client *c;
  LOCK_CLIENTS;
  for (c = Clients; c; c = c->next)
  {
    if (c->addr == addr && c->reply_sock == INVALID_SOCKET)
    {
      c->reply_sock = reply_sock;
      break;
    }
  }
  UNLOCK_CLIENTS;
  if (c)
  {
    FD_SET(reply_sock, fdactive);
    MDSDBG(CLIENT_PRI " accepted", CLIENT_VAR(c));
  }
  else
  {
    MDSWRN("Dropped connection from " IPADDRPRI ":%d", IPADDRVAR(&addr), port);
    shutdown(reply_sock, 2);
    close(reply_sock);
  }
}
