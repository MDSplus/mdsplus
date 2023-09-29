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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <mdsshr.h>
#include "../mdsip_connections.h"
#include "../mdsIo.h"
#include "mdsipthreadstatic.h"

#include <pthread_port.h>
#include <treeshr.h>

// #define DEBUG
#include <mdsmsg.h>

Connection *_FindConnection(int id, Connection **prev, MDSIPTHREADSTATIC_ARG)
{
  if (id == INVALID_CONNECTION_ID)
    return NULL;
  Connection *c = MDSIP_CONNECTIONS, *p = NULL;
  for (; c; p = c, c = c->next)
  {
    if (c->id == id)
      break;
  }
  if (prev)
    *prev = p;
  return c;
}

Connection *PopConnection(int id)
{
  MDSIPTHREADSTATIC_INIT;
  Connection *p, *c;
  c = _FindConnection(id, &p, MDSIPTHREADSTATIC_VAR);
  if (c && !(c->state & CON_INLIST))
    c = NULL;
  else if (c)
  {
    c->state &= CON_ACTIVITY; // clears  INLIST
    if (c->state & CON_ACTIVITY)
    { // if any oustanding task
      struct timespec tp;
      clock_gettime(CLOCK_REALTIME, &tp);
      tp.tv_sec += 10;
      // wait upto 10 seconds to allow current task to finish
      // while exits if no other task but disconnect or on timeout
      if (c->state & CON_ACTIVITY)
      {
        MDSDBG(CON_PRI " is waiting for lock", CON_VAR(c));
      }
      c = _FindConnection(id, &p, MDSIPTHREADSTATIC_VAR); // we were waiting, so we need to update p
    }
    if (c)
    {
      // remove after task is complete
      if (p)
      {
        p->next = c->next;
      }
      else
      {
        MDSIP_CONNECTIONS = c->next;
      }
      c->next = NULL;
      MDSDBG(CON_PRI " popped", CON_VAR(c));
    }
  }
  return c;
}

/// Find Connection that is ready for sending
/// must be CON_IDLE or CON_REQUEST
Connection *FindConnectionSending(int id)
{
  MDSIPTHREADSTATIC_INIT;
  Connection *c;
  c = _FindConnection(id, NULL, MDSIPTHREADSTATIC_VAR);
  if (c)
  {
    if ((c->state & CON_ACTIVITY & ~CON_REQUEST) && (c->state & CON_INLIST))
    {
      if (c->state & CON_REQUEST)
      {
        c->state &= ~CON_REQUEST; // clear request
        MDSDBG(CON_PRI " unlocked 0x%02x", CON_VAR(c), CON_REQUEST);
      }
      c = NULL;
    }
  }
  return c;
}

EXPORT int MdsIpGetConnectionVersion(int id)
{
  MDSIPTHREADSTATIC_INIT;
  Connection *c = _FindConnection(id, NULL, MDSIPTHREADSTATIC_VAR);
  return (c) ? (int)c->version : -1;
}

Connection *FindConnectionWithLock(int id, con_t state)
{
  MDSIPTHREADSTATIC_INIT;
  Connection *c = _FindConnection(id, NULL, MDSIPTHREADSTATIC_VAR);
  while (c && (c->state & CON_ACTIVITY) && (c->state & CON_INLIST))
  {
    MDSDBG(CON_PRI " is waiting for lock 0x%02x", CON_VAR(c), state);
    c = _FindConnection(id, NULL, MDSIPTHREADSTATIC_VAR);
    if (c && !(c->state & CON_INLIST))
    {
      c = NULL;
    }
  }
  if (c)
  {
    c->state |= state & CON_ACTIVITY;
    MDSDBG(CON_PRI " locked 0x%02x", CON_VAR(c), state);
  }
  return c;
}

void UnlockConnection(Connection *c_in)
{
  MDSIPTHREADSTATIC_INIT;
  Connection *c; // check if not yet freed
  for (c = MDSIP_CONNECTIONS; c; c = c->next)
  {
    if (c == c_in)
    {
      c->state &= ~CON_ACTIVITY; // clear activity
      MDSDBG(CON_PRI " unlocked 0x%02x", CON_VAR(c), CON_ACTIVITY);
      break;
    }
  }
}

int NextConnection(void **ctx, char **info_name, void **info,
                   size_t *info_len)
{ // check
  int ans;
  MDSIPTHREADSTATIC_INIT;
  Connection *c, *next;
  next = (*ctx != (void *)-1) ? (Connection *)*ctx : MDSIP_CONNECTIONS;
  for (c = MDSIP_CONNECTIONS; c && c != next; c = c->next)
    ;
  if (c)
  {
    *ctx = c->next;
    if (info_name)
      *info_name = c->info_name;
    if (info)
      *info = c->info;
    if (info_len)
      *info_len = c->info_len;
    ans = c->id;
  }
  else
  {
    *ctx = 0;
    ans = INVALID_CONNECTION_ID;
  }
  return ans;
}

int SendToConnection(int id, const void *buffer, size_t buflen, int nowait)
{
  int res;
  Connection *c = FindConnectionWithLock(id, CON_SEND);
  if (c && c->io && c->io->send)
    res = c->io->send(c, buffer, buflen, nowait);
  else
    res = -1;
  UnlockConnection(c);
  return res;
}

int FlushConnection(int id)
{
  int res;
  Connection *c = FindConnectionWithLock(id, CON_FLUSH);
  if (c && c->io)
    res = c->io->flush ? c->io->flush(c) : 0;
  else
    res = -1;
  UnlockConnection(c);
  return res;
}

int ReceiveFromConnection(int id, void *buffer, size_t buflen)
{
  int res;
  Connection *c = FindConnectionWithLock(id, CON_RECV);
  if (c && c->io && c->io->recv)
    res = c->io->recv(c, buffer, buflen);
  else
    res = -1;
  UnlockConnection(c);
  return res;
}

////////////////////////////////////////////////////////////////////////////////
//  NewConnection  /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Connection *newConnection(char *protocol)
{
  Connection *connection;
  IoRoutines *io = LoadIo(protocol);
  if (io)
  {
    connection = calloc(1, sizeof(Connection));
    connection->io = io;
    connection->readfd = INVALID_SOCKET;
    connection->message_id = INVALID_MESSAGE_ID;
    connection->protocol = strdup(protocol);
    connection->id = INVALID_CONNECTION_ID;
    connection->state = CON_IDLE;
    connection->compression_level = 0;
    return connection;
  }
  else
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////
//  FreeDescriptors  ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void FreeDescriptors(Connection *c)
{
  int i;
  if (c)
  {
    for (i = 0; i < MDSIP_MAX_ARGS; i++)
    {
      MdsFreeDescriptor(c->descrip[i]);
      c->descrip[i] = NULL;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
//  CloseConnection  //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
extern int TdiDeleteContext();
extern int MDSEventCan();
int destroyConnection(Connection *connection)
{
  if (!connection)
    return MDSplusERROR;
  if (connection->id != INVALID_CONNECTION_ID)
  {
    if (connection->state & CON_INLIST)
    {
      MDSIPTHREADSTATIC_INIT;
      if (connection == _FindConnection(connection->id, NULL, MDSIPTHREADSTATIC_VAR))
      {
        PopConnection(connection->id);
      }
    }
    MdsEventList *e, *nexte;
    for (e = connection->event; e; e = nexte)
    {
      nexte = e->next;
      MDSEventCan(e->eventid);
      if (e->info_len > 0)
        free(e->info);
      free(e);
    }
    TdiDeleteContext(connection->tdicontext);
    FreeDescriptors(connection);
  }
  if (connection->io)
  {
    connection->io->disconnect(connection);
  }
  MDSDBG(CON_PRI " disconnected", CON_VAR(connection));
  free(connection->info);
  free(connection->protocol);
  free(connection->info_name);
  free(connection->rm_user);
  free(connection);
  return MDSplusSUCCESS;
}

int CloseConnection(int id)
{
  Connection *const c = PopConnection(id);
  return destroyConnection(c);
}

////////////////////////////////////////////////////////////////////////////////
//  GetConnectionInfo  /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void *ConnectionGetInfo(Connection *c, char **info_name, SOCKET *readfd,
                        size_t *len)
{
  if (c)
  {
    if (len)
      *len = c->info_len;
    if (info_name)
      *info_name = c->info_name;
    if (readfd)
      *readfd = c->readfd;
    return c->info;
  }
  return NULL;
}

void *GetConnectionInfo(int conid, char **info_name, SOCKET *readfd,
                        size_t *len)
{
  MDSIPTHREADSTATIC_INIT;
  void *ans;
  Connection *c = _FindConnection(conid, NULL, MDSIPTHREADSTATIC_VAR);
  ans = ConnectionGetInfo(c, info_name, readfd, len);
  return ans;
}

////////////////////////////////////////////////////////////////////////////////
//  ConnectionSetInfo  /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void ConnectionSetInfo(Connection *c, char *info_name, SOCKET readfd,
                       void *info, size_t len)
{
  if (c)
  {
    c->info_name = strdup(info_name);
    if (info)
    {
      c->info = memcpy(malloc(len), info, len);
      c->info_len = len;
    }
    else
    {
      c->info = 0;
      c->info_len = 0;
    }
    c->readfd = readfd;
  }
}

void SetConnectionInfo(int conid, char *info_name, SOCKET readfd, void *info,
                       size_t len)
{
  MDSIPTHREADSTATIC_INIT;
  Connection *c = _FindConnection(conid, NULL, MDSIPTHREADSTATIC_VAR);
  ConnectionSetInfo(c, info_name, readfd, info, len);
}

////////////////////////////////////////////////////////////////////////////////
//  ConnectionCompression  /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void SetConnectionCompression(int conid, int compression)
{
  MDSIPTHREADSTATIC_INIT;
  Connection *c = _FindConnection(conid, NULL, MDSIPTHREADSTATIC_VAR);
  if (c)
    c->compression_level = compression;
}

static inline int ConnectionGetCompression(Connection *c)
{
  return c ? c->compression_level : 0;
}
int GetConnectionCompression(int conid)
{
  MDSIPTHREADSTATIC_INIT;
  int complv;
  Connection *c = _FindConnection(conid, NULL, MDSIPTHREADSTATIC_VAR);
  complv = ConnectionGetCompression(c);
  return complv;
}

unsigned char ConnectionIncMessageId(Connection *c)
{
  if (c)
  {
    c->message_id++;
    if (c->message_id == INVALID_MESSAGE_ID)
      c->message_id = 1;
    return c->message_id;
  }
  return 0;
}

///
/// Finds connection by id and sets the client_type field of the connection
/// structure. \note see ClientType() function.
///
/// \param conid the connection id
/// \param client_type the type of connection to be set
///
void SetConnectionClientType(int conid, int client_type)
{
  MDSIPTHREADSTATIC_INIT;
  Connection *c = _FindConnection(conid, NULL, MDSIPTHREADSTATIC_VAR);
  if (c)
    c->client_type = client_type;
}

///
/// Finds connection by id and gets the client_type field of the connection
/// structure \note see ClientType() function.
///
/// \param conid the connection id
/// \return client_type value stored in connection structure
///
client_t GetConnectionClientType(int conid)
{
  MDSIPTHREADSTATIC_INIT;
  client_t type;
  Connection *c = _FindConnection(conid, NULL, MDSIPTHREADSTATIC_VAR);
  type = c ? c->client_type : INVALID_CLIENT;
  return type;
}

////////////////////////////////////////////////////////////////////////////////
//  authorize_client  ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/// Authorize client by username calling protocol IoRoutine.
/// \param id of the connection to use
/// \param username of the user to be authorized for access
/// \return true if authorized user found, false otherwise

static inline int authorize_client(Connection *c, char *username)
{
  return c->io->authorize ? c->io->authorize(c, username) : MDSplusSUCCESS;
  return MDSplusERROR;
}

////////////////////////////////////////////////////////////////////////////////
//  AcceptConnection  //////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int AddConnection(Connection *c)
{
  MDSIPTHREADSTATIC_INIT;
  static int id = INVALID_CONNECTION_ID;
  static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock(&lock);
  do
  {
    id++; // find next free id
  } while ((id == INVALID_CONNECTION_ID) || _FindConnection(id, NULL, MDSIPTHREADSTATIC_VAR));
  c->id = id;
  pthread_mutex_unlock(&lock);
  c->state |= CON_INLIST;
  c->next = MDSIP_CONNECTIONS;
  MDSIP_CONNECTIONS = c;
  MDSDBG("Connection %02d connected", c->id);
  return c->id;
}

static inline int get_max_version()
{
#define MDSIP_MAX_VERSION "MDSIP_MAX_VERSION"
  static int max_version = -1;
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock(&mutex);
  if (max_version < 0)
  {
    // if env not specified, use max
    max_version = 0xFFFF;
    char *tmp = getenv(MDSIP_MAX_VERSION);
    if (tmp)
    {
      long max_long = strtol(tmp, &tmp, 0);
      if (!*tmp && max_long >= 0 && max_long < max_version)
      {
        max_version = (int)max_long;
      }
      MDSMSG(MDSIP_MAX_VERSION " = %d", max_version);
    }
  }
  pthread_mutex_unlock(&mutex);
  return max_version;
}

int AcceptConnection(char *protocol, char *info_name, SOCKET readfd, void *info,
                     size_t info_len, int *id, char **usr)
{
  if (usr)
    *usr = NULL;
  Connection *c = newConnection(protocol);
  INIT_STATUS_ERROR;
  if (c)
  {
    Message *msg;
    char *user = NULL, *user_p = NULL;
    // SET INFO //
    ConnectionSetInfo(c, info_name, readfd, info, info_len);
    msg = GetMdsMsgTOC(c, &status, 10000);
    if (!msg || STATUS_NOT_OK)
    {
      free(msg);
      destroyConnection(c);
      return MDSplusERROR;
    }
    // AUTHORIZE //
    if (STATUS_OK && (msg) && (msg->h.dtype == DTYPE_CSTRING))
    {
      user = malloc(msg->h.length + 1);
      memcpy(user, msg->bytes, msg->h.length);
      user[msg->h.length] = 0;
    }
    c->rm_user = user;
    user_p = user ? user : "?";
    status = authorize_client(c, user_p);
    // SET COMPRESSION //
    if (STATUS_OK)
    {
      const int max_version = get_max_version();
      c->compression_level = msg->h.status & 0xf;
      c->client_type = msg->h.client_type;
      c->version = msg->h.ndims > 0 ? msg->h.dims[0] : 0;
      if (c->version > max_version)
        c->version = max_version;
      fprintf(stderr, "Connected: %s\n", user_p);
    }
    else
    {
      fprintf(stderr, "Access denied: %s\n", user_p);
    }
    msg->h.msglen = sizeof(MsgHdr);
    msg->h.status = STATUS_OK ? (1 | (c->compression_level << 1)) : 0;
    msg->h.ndims = 1;
    msg->h.dims[0] = MDSIP_VERSION;
    // reply to client //
    status = SendMdsMsgC(c, msg, 0);
    free(msg);
    if (STATUS_OK)
    {
      if (usr)
        *usr = strdup(user_p);
      // all good add connection
      *id = AddConnection(c);
    }
    else
    {
      destroyConnection(c);
    }
  }
  return status;
}
