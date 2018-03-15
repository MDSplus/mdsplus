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
#include <mdsplus/mdsconfig.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <status.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef _WIN32
 #include <winsock2.h>
#else
 #include <pwd.h>
 #define INVALID_SOCKET -1
#endif

#define LOAD_GETUSERNAME
#include <pthread_port.h>
#include "mdsip_connections.h"

////////////////////////////////////////////////////////////////////////////////
//  Parse Host  ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static void parseHost(char *hostin, char **protocol, char **host)
{
  size_t i;
  *protocol = strcpy((char *)malloc(strlen(hostin) + 10), "");
  *host = strcpy((char *)malloc(strlen(hostin) + 10), "");
  sscanf(hostin, "%[^:]://%s", *protocol, *host);
  if (strlen(*host) == 0) {
    if (hostin[0] == '_') {
      strcpy(*protocol, "gsi");
      strcpy(*host, &hostin[1]);
    } else {
      strcpy(*protocol, "tcp");
      strcpy(*host, hostin);
    }
  }
  for (i = strlen(*host) - 1; (*host)[i] == 32; i--) {
    (*host)[i] = 0;
    if (i == 0)
      break;
  }
}

////////////////////////////////////////////////////////////////////////////////
//  Do Login  //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// Execute login inside server using given connection
///
/// \param id of connection (on client) to be used
/// \return status o login into server 1 if success, MDSplusERROR if not authorized or error
/// occurred
///
static int doLogin(Connection* c)
{
  INIT_STATUS;
  Message *m;
  static char *user_p;
  GETUSERNAME(user_p);
  unsigned int length = strlen(user_p);
  m = calloc(1, sizeof(MsgHdr) + length);
  m->h.client_type = SENDCAPABILITIES;
  m->h.length = (short)length;
  m->h.msglen = sizeof(MsgHdr) + length;
  m->h.dtype = DTYPE_CSTRING;
  m->h.status = c->compression_level;
  m->h.ndims = 0;
  memcpy(m->bytes, user_p, length);
  status = SendMdsMsgC(c, m, 0);
  free(m);
  if STATUS_OK {
    m = GetMdsMsgTOC(c, &status, 10000);
    if (m == 0 || STATUS_NOT_OK) {
      printf("Error in connect\n");
      return MDSplusERROR;
    } else {
      if IS_NOT_OK(m->h.status) {
	printf("Error in connect: Access denied\n");
	free(m);
	return MDSplusERROR;
      }
      // SET CLIENT COMPRESSION FROM SERVER //
      c->compression_level= (m->h.status & 0x1e) >> 1;
    }
    if (m)
      free(m);
  } else {
    fprintf(stderr,"Error connecting to server (DoLogin)\n");
    fflush(stderr);
    return MDSplusERROR;
  }
  return status;
}



////////////////////////////////////////////////////////////////////////////////
//  Reuse Check  ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// Trigger reuse check funcion for IoRoutines on host.
///
int ReuseCheck(char *hostin, char *unique, size_t buflen)
{
  int ok = -1;
  char *host = 0;
  char *protocol = 0;
  parseHost(hostin, &protocol, &host);
  IoRoutines* io = LoadIo(protocol);
  if (io) {
    if (io->reuseCheck)
      ok = io->reuseCheck(host, unique, buflen);
    else {
      strncpy(unique, hostin, buflen);
      ok = 0;
    }
  } else
    memset(unique, 0, buflen);
  if (protocol)
    free(protocol);
  if (host)
    free(host);
  return ok;
}


////////////////////////////////////////////////////////////////////////////////
//  ConnectToMds  //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


int ConnectToMds(char *hostin)
{
  int id = -1;
  char *host = 0;
  char *protocol = 0;
  if (hostin == 0)
    return id;
  parseHost(hostin, &protocol, &host);
  Connection* c = NewConnectionC(protocol);
  if (c) {
    if (c->io && c->io->connect) {
      c->compression_level = GetCompressionLevel();
      if (c->io->connect(c, protocol, host)<0 || IS_NOT_OK(doLogin(c))) {
        DisconnectConnectionC(c);
      } else {
        id = AddConnection(c);
      }
    }
  }
  if (host)
    free(host);
  if (protocol)
    free(protocol);
  return id;
}
