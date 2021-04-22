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
#ifdef DEBUG
#define DBG(...) fprintf(stderr, __VA_ARGS__)
#define PID ((intptr_t)pthread_self())
#else
#define DBG(...) \
  { /**/         \
  }
#endif



Connection *_FindConnection(int id, Connection **prev)
{
  MDSIPTHREADSTATIC_INIT;
  Connection *p = NULL, *c = MDSIP_CONNECTIONS;
  while (c)
  {
    if (c->id == id)
    {
      break;
    }
    p = c;
    c = c->next;
  }
  if (prev)
    *prev = p;
  return c;
}

Connection *PopConnection(int id)
{
  Connection *p, *c;
  c = _FindConnection(id, &p);
  if (c && c->state & CON_DETACHED)
    c = NULL;
  else if (c)
  {
    c->state |= CON_DETACHED; // sets disconnect
    if (c->state & CON_ACTIVITY)
    { // if any task but disconnect
      struct timespec tp;
      clock_gettime(CLOCK_REALTIME, &tp);
      tp.tv_sec += 10;
      // wait upto 10 seconds to allow current task to finish
      // while exits if no other task but disconnect or on timeout
      if (c->state & CON_ACTIVITY)
      {
        DBG("Connection %02d -- 0x%02x : 0x%" PRIxPTR " would wait to pop\n",
            c->id, c->state, PID);
      }
      c = _FindConnection(id, &p); // we were waiting, so we need to update p
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
        MDSIPTHREADSTATIC_INIT;
        MDSIP_CONNECTIONS = c->next;
      }
      c->next = NULL;
      DBG("Connections: %02d -> 0x%02x : 0x%" PRIxPTR " popped\n",
          c->id, c->state, PID);
    }
  }
  return c;
}

/// Find Connection that is ready for sending
/// must be CON_IDLE or CON_REQUEST
Connection *FindConnectionSending(int id)
{
  Connection *c;
  c = _FindConnection(id, NULL);
  if (c)
  {
    if ((c->state & CON_ACTIVITY & ~CON_REQUEST) && !(c->state & CON_DISCONNECT))
    {
      if (c->state & CON_REQUEST)
      {
        c->state &= ~CON_REQUEST; // clear sendarg
        DBG("Connections: %02d -> 0x%02x : 0x%" PRIxPTR " unlocked sendarg\n",
            c->id, c->state, PID);
      }
      c = NULL;
    }
  }
  return c;
}

EXPORT int GetConnectionVersion(int id)
{
  int version;
  Connection *c = _FindConnection(id, NULL);
  version = c ? (int)c->version : -1;
  return version;
}

Connection *FindConnectionWithLock(int id, con_t state)
{
  Connection *c;
  c = _FindConnection(id, NULL);
  while (c && (c->state & CON_ACTIVITY) && !(c->state & CON_DISCONNECT))
  {
    DBG("Connections: %02d -- 0x%02x : 0x%" PRIxPTR " is would wait to lock 0x%02x\n",
        c->id, c->state, PID, state);
    c = _FindConnection(id, NULL);
    if (c && (c->state & CON_DISCONNECT))
    {
      c = NULL;
    }
  }
  if (c)
  {
    c->state |= state;
    DBG("Connections: %02d -> 0x%02x : 0x%" PRIxPTR " locked 0x%02x\n",
        c->id, c->state, PID, state);
  }
  return c;
}

void UnlockConnection(Connection *c_in)
{
  MDSIPTHREADSTATIC_INIT;
  Connection *c; // check if not yet freed
  for (c = MDSIP_CONNECTIONS; c && c != c_in; c = c->next)
    ;
  if (c)
  {
    c->state &= ~CON_ACTIVITY; // clear activity
    DBG("Connections: %02d -> 0x%02x : 0x%" PRIxPTR " unlocked 0x%02x\n",
        c->id, c->state, PID, CON_ACTIVITY);
  }
}

#define CONNECTION_UNLOCK_PUSH(c) \
  pthread_cleanup_push((void *)UnlockConnection, (void *)c)
#define CONNECTION_UNLOCK(c) pthread_cleanup_pop(1)

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
  CONNECTION_UNLOCK_PUSH(c);
  if (c && c->io && c->io->send)
    res = c->io->send(c, buffer, buflen, nowait);
  else
    res = -1;
  CONNECTION_UNLOCK(c);
  return res;
}

int FlushConnection(int id)
{
  int res;
  Connection *c = FindConnectionWithLock(id, CON_FLUSH);
  CONNECTION_UNLOCK_PUSH(c);
  if (c && c->io)
    res = c->io->flush ? c->io->flush(c) : 0;
  else
    res = -1;
  CONNECTION_UNLOCK(c);
  return res;
}

int ReceiveFromConnection(int id, void *buffer, size_t buflen)
{
  int res;
  Connection *c = FindConnectionWithLock(id, CON_RECV);
  CONNECTION_UNLOCK_PUSH(c);
  if (c && c->io && c->io->recv)
    res = c->io->recv(c, buffer, buflen);
  else
    res = -1;
  CONNECTION_UNLOCK(c);
  return res;
}

static void exitHandler(void)
{
  int id;
  void *ctx = (void *)-1;
  while ((id = NextConnection(&ctx, 0, 0, 0)) != INVALID_CONNECTION_ID)
  {
    DisconnectConnection(id);
    ctx = 0;
  }
}
static void registerHandler() { atexit(exitHandler); }

////////////////////////////////////////////////////////////////////////////////
//  DisconnectConnection  //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int DisconnectConnectionC(Connection *c)
{
  if (c && c->id != INVALID_CONNECTION_ID //
      && c->state != CON_DETACHED)
  { // connection should not have been in list at this point
    c = PopConnection(c->id);
  }
  if (!c)
    return MDSplusERROR;
  c->io->disconnect(c);
  DBG("Connections: %02d disconnected\n", c->id);
  free(c->info);
  FreeDescriptors(c);
  free(c->protocol);
  free(c->info_name);
  free(c->rm_user);
  free(c);
  return MDSplusSUCCESS;
}

int DisconnectConnection(int conid)
{
   Connection *const c = PopConnection(conid);
  return DisconnectConnectionC(c);
}

////////////////////////////////////////////////////////////////////////////////
//  NewConnection  /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Connection *NewConnectionC(char *protocol)
{
  Connection *connection;
  IoRoutines *io = LoadIo(protocol);
  if (io)
  {
    RUN_FUNCTION_ONCE(registerHandler);
    connection = calloc(1, sizeof(Connection));
    connection->io = io;
    connection->readfd = -1;
    connection->message_id = -1;
    connection->protocol = strdup(protocol);
    connection->id = INVALID_CONNECTION_ID;
    connection->state = CON_IDLE;
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
      if (c->descrip[i])
      {
        if (c->descrip[i] != MdsEND_ARG)
        {
          free(c->descrip[i]->pointer);
          free(c->descrip[i]);
        }
        c->descrip[i] = NULL;
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
//  GetConnectionIo  ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

IoRoutines *GetConnectionIo(int conid)
{
  IoRoutines *io;
  Connection *c = _FindConnection(conid, 0);
  io = c ? c->io : NULL;
  return io;
}

////////////////////////////////////////////////////////////////////////////////
//  GetConnectionInfo  /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void *GetConnectionInfoC(Connection *c, char **info_name, SOCKET *readfd,
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
  void *ans;
  Connection *c = _FindConnection(conid, 0);
  ans = GetConnectionInfoC(c, info_name, readfd, len);
  return ans;
}

////////////////////////////////////////////////////////////////////////////////
//  SetConnectionInfo  /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void SetConnectionInfoC(Connection *c, char *info_name, SOCKET readfd,
                        void *info, size_t len)
{
  if (c)
  {
    c->info_name = strcpy(malloc(strlen(info_name) + 1), info_name);
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
  Connection *c = _FindConnection(conid, 0);
  if (c)
    SetConnectionInfoC(c, info_name, readfd, info, len);
}

////////////////////////////////////////////////////////////////////////////////
//  ConnectionCompression  /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void SetConnectionCompression(int conid, int compression)
{
  Connection *c = _FindConnection(conid, NULL);
  if (c)
    c->compression_level = compression;
}

static inline int GetConnectionCompressionC(Connection *c)
{
  return c ? c->compression_level : 0;
}
int GetConnectionCompression(int conid)
{
  int complv;
  Connection *c = _FindConnection(conid, NULL);
  complv = GetConnectionCompressionC(c);
  return complv;
}

////////////////////////////////////////////////////////////////////////////////
//  IncrementConnectionMessageId  //////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

unsigned char IncrementConnectionMessageIdC(Connection *c)
{
  if (c)
  {
    c->message_id++;
    if (c->message_id == 0)
      c->message_id = 1;
    return c->message_id;
  }
  return 0;
}

unsigned char IncrementConnectionMessageId(int conid)
{
  unsigned char id;
  Connection *c = _FindConnection(conid, NULL);
  id = IncrementConnectionMessageIdC(c);
  return id;
}

////////////////////////////////////////////////////////////////////////////////
//  GetConnectionMessageId  ////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

inline static unsigned char GetConnectionMessageIdC(Connection *c)
{
  return c ? c->message_id : 0;
}

unsigned char GetConnectionMessageId(int conid)
{
  unsigned char id;
  Connection *c = _FindConnection(conid, 0);
  id = GetConnectionMessageIdC(c);
  return id;
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
  Connection *c = _FindConnection(conid, 0);
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
  client_t type;
  Connection *c = _FindConnection(conid, 0);
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
  do
  {
    id++; // find next free id
  } while (id == INVALID_CONNECTION_ID && _FindConnection(id, NULL));
  c->id = id;
  c->next = MDSIP_CONNECTIONS;
  MDSIP_CONNECTIONS = c;
  DBG("Connections: %02d connected\n", c->id);
  return c->id;
}

int AcceptConnection(char *protocol, char *info_name, SOCKET readfd, void *info,
                     size_t info_len, int *id, char **usr)
{
  Connection *c = NewConnectionC(protocol);
  INIT_STATUS_ERROR;
  if (c)
  {
    static Message m;
    Message *m_user;
    char *user = NULL, *user_p = NULL;
    // SET INFO //
    SetConnectionInfoC(c, info_name, readfd, info, info_len);
    m_user = GetMdsMsgTOC(c, &status, 10000);
    if (!m_user || STATUS_NOT_OK)
    {
      free(m_user);
      *usr = NULL;
      DisconnectConnectionC(c);
      return MDSplusERROR;
    }
    m.h.msglen = sizeof(MsgHdr);
    // AUTHORIZE //
    if (STATUS_OK && (m_user) && (m_user->h.dtype == DTYPE_CSTRING))
    {
      user = malloc(m_user->h.length + 1);
      memcpy(user, m_user->bytes, m_user->h.length);
      user[m_user->h.length] = 0;
    }
    c->rm_user = user;
    user_p = user ? user : "?";
    status = authorize_client(c, user_p);
    // SET COMPRESSION //
    if (STATUS_OK)
    {
      c->compression_level = m_user->h.status & 0xf;
      c->client_type = m_user->h.client_type;
      *usr = strdup(user_p);
      if (m_user->h.ndims > 0)
        c->version = m_user->h.dims[0];
    }
    else
      *usr = NULL;
    if (STATUS_NOT_OK)
      fprintf(stderr, "Access denied: %s\n", user_p);
    else
      fprintf(stderr, "Connected: %s\n", user_p);
    m.h.status = STATUS_OK ? (1 | (c->compression_level << 1)) : 0;
    m.h.client_type = m_user ? m_user->h.client_type : 0;
    m.h.ndims = 1;
    m.h.dims[0] = MDSIP_VERSION;
    MdsIpFree(m_user);
    // reply to client //
    SendMdsMsgC(c, &m, 0);
    if (STATUS_OK)
    {
      // all good add connection
      *id = AddConnection(c);
    }
    else
      DisconnectConnectionC(c);
  }
  return status;
}
