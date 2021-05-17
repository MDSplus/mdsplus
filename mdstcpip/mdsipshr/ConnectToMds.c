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
#include <socket_port.h>
#include <getusername.h>
#include "../mdsip_connections.h"
#include "../mdsIo.h"

static void parse_host(char *hostin, char **protocol, char **host)
{
  size_t i;
  *protocol = strcpy((char *)malloc(strlen(hostin) + 10), "");
  *host = strcpy((char *)malloc(strlen(hostin) + 10), "");
  sscanf(hostin, "%[^:]://%s", *protocol, *host);
  if (strlen(*host) == 0)
  {
    if (hostin[0] == '_')
    {
      strcpy(*protocol, "gsi");
      strcpy(*host, &hostin[1]);
    }
    else
    {
      strcpy(*protocol, "tcp");
      strcpy(*host, hostin);
    }
  }
  for (i = strlen(*host) - 1; (*host)[i] == 32; i--)
  {
    (*host)[i] = 0;
    if (i == 0)
      break;
  }
}

/// Execute login inside server using given connection
///
/// \param id of connection (on client) to be used
/// \return status o login into server 1 if success, MDSplusERROR if not authorized or error
/// occurred
static int do_login(Connection *c)
{
  static char *user_p = NULL;
  GETUSERNAME(user_p);
  uint32_t length = strlen(user_p);
  Message *msend = calloc(1, sizeof(MsgHdr) + length);
  msend->h.client_type = SENDCAPABILITIES;
  msend->h.length = (short)length;
  msend->h.msglen = sizeof(MsgHdr) + length;
  msend->h.dtype = DTYPE_CSTRING;
  msend->h.status = c->compression_level;
  msend->h.ndims = 1;
  msend->h.dims[0] = MDSIP_VERSION;
  memcpy(msend->bytes, user_p, length);
  int status = SendMdsMsgC(c, msend, 0);
  int err;
  free(msend);
  if (STATUS_NOT_OK)
  {
    perror("Error during login: send");
    err = C_ERROR;
  }
  else
  {
    Message *mrecv = GetMdsMsgTOC(c, &status, 10000);
    if (STATUS_NOT_OK)
    {
      perror("Error during login: recv");
      err = C_ERROR;
    }
    else if (!mrecv)
    {
      fputs("Error during login: recv NULL\n", stderr);
      err = C_ERROR;
    }
    else if (IS_NOT_OK(mrecv->h.status))
    {
      fputs("Error during login: Access denied\n", stderr);
      err = C_ERROR;
    }
    else
    {
      // SET CLIENT COMPRESSION FROM SERVER
      c->compression_level = (mrecv->h.status & 0x1e) >> 1;
      c->client_type = mrecv->h.client_type;
      if (mrecv->h.ndims > 0)
        c->version = mrecv->h.dims[0];
      err = C_OK;
    }
    free(mrecv);
  }
  return err;
}

/// Trigger reuse check funcion for IoRoutines on host.
EXPORT int ReuseCheck(char *hostin, char *unique, size_t buflen)
{
  int ok = -1;
  char *host = 0;
  char *protocol = 0;
  parse_host(hostin, &protocol, &host);
  IoRoutines *io = LoadIo(protocol);
  if (io)
  {
    if (io->reuseCheck)
      ok = io->reuseCheck(host, unique, buflen);
    else
    {
      strncpy(unique, hostin, buflen);
      ok = 0;
    }
  }
  else
    memset(unique, 0, buflen);
  free(protocol);
  free(host);
  return ok;
}

EXPORT int ConnectToMds(char *hostin)
{
  int id = INVALID_CONNECTION_ID;
  char *host = 0;
  char *protocol = 0;
  if (hostin == 0)
    return id;
  parse_host(hostin, &protocol, &host);
  Connection *c = newConnection(protocol);
  if (c)
  {
    if (c->io && c->io->connect)
    {
      c->compression_level = GetCompressionLevel();
      if (c->io->connect(c, protocol, host) < 0 || do_login(c))
      {
        destroyConnection(c);
      }
      else
      {
        id = AddConnection(c);
      }
    }
  }
  free(host);
  free(protocol);
  return id;
}

EXPORT int DisconnectFromMds(int id)
{
  return CloseConnection(id);
}

EXPORT void FreeMessage(void *m)
{
  free(m);
}