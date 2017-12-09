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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <treeshr.h>
#include <mdsshr.h>

#include "mdsip_connections.h"

static Connection *ConnectionList = NULL;
static pthread_mutex_t connection_mutex = PTHREAD_MUTEX_INITIALIZER;
inline static void LockConnection() {
#ifdef DEBUG
fprintf(stderr,"connection->lock");
#endif
pthread_mutex_lock(&connection_mutex);
#ifdef DEBUG
fprintf(stderr,"ed\n");
#endif
}
static void UnlockConnection(){
pthread_mutex_unlock(&connection_mutex);
#ifdef DEBUG
fprintf(stderr,"connection->unlocked\n");
#endif
}
#define CONNECTIONLIST_LOCK   LockConnection();pthread_cleanup_push(UnlockConnection, NULL);
#define CONNECTIONLIST_UNLOCK pthread_cleanup_pop(1);

Connection *_FindConnection(int id, Connection ** prev){
  Connection *c, *p;
  for (p = 0, c = ConnectionList; c && c->id != id; p = c, c = c->next);
  if (prev) *prev = p;
  return c;
}

Connection *FindConnection(int id, Connection ** prev){
  Connection *c;
  CONNECTIONLIST_LOCK;
  c = _FindConnection(id, prev);
  CONNECTIONLIST_UNLOCK
  return c;
}

int NextConnection(void **ctx, char **info_name, void **info, size_t * info_len){//check
  int ans;
  CONNECTIONLIST_LOCK;
  Connection *c, *next;
  next = (*ctx != (void *)-1) ? (Connection *) * ctx : ConnectionList;
  for (c = ConnectionList; c && c != next; c = c->next) ;
  if (c) {
    *ctx = c->next;
    if (info_name)
      *info_name = c->info_name;
    if (info)
      *info = c->info;
    if (info_len)
      *info_len = c->info_len;
    ans = c->id;
  } else {
    *ctx = 0;
    ans = -1;
  }
  CONNECTIONLIST_UNLOCK;
  return ans;
}

int FlushConnection(int id){
  IoRoutines *io = GetConnectionIo(id);
  if (io) return io->flush ? io->flush(id) : 0;
  return -1;
}

static void exitHandler(void){
  int id;
  void *ctx = (void *)-1;
  while ((id = NextConnection(&ctx, 0, 0, 0)) != -1) {
    DisconnectConnection(id);
    ctx = 0;
  }
}
static void registerHandler(){
  atexit(exitHandler);
}


////////////////////////////////////////////////////////////////////////////////
//  NewConnection  /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int NewConnection(char *protocol){
  Connection *connection;
  IoRoutines *io = LoadIo(protocol);
  static int id = 1;
  static pthread_once_t registerExitHandler = PTHREAD_ONCE_INIT;
  if (io) {
    (void) pthread_once(&registerExitHandler,registerHandler);
    connection = memset(malloc(sizeof(Connection)), 0, sizeof(Connection));
    connection->io = io;
    connection->readfd = -1;
    connection->message_id = -1;
    connection->protocol = strcpy(malloc(strlen(protocol) + 1), protocol);
    CONNECTIONLIST_LOCK;
    connection->id = id++;
    connection->next = ConnectionList;
    ConnectionList = connection;
    CONNECTIONLIST_UNLOCK;
    return connection->id;
  } else
    return -1;
}


////////////////////////////////////////////////////////////////////////////////
//  AuthorizeClient  ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/// Authorize client by username calling protocol IoRoutine.
/// \param id of the connection to use
/// \param username of the user to be authorized for access
/// \return true if authorized user found, false otherwise
static int AuthorizeClient(int id, char *username){
  Connection *c = FindConnection(id, 0);
  if (c && c->io) return c->io->authorize ? c->io->authorize(id, username) : 1;
  return 0;
}



////////////////////////////////////////////////////////////////////////////////
//  AcceptConnection  //////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int AcceptConnection(char *protocol, char *info_name, int readfd, void *info, size_t info_len, int *id, char **usr){
  *id = NewConnection(protocol);
  int status = MDSplusERROR;
  if (*id > 0) {
    static Message m;
    Message *m_user;
    char *user = NULL, *user_p = NULL;
    // SET INFO //
    SetConnectionInfo(*id, info_name, readfd, info, info_len);
    m_user = GetMdsMsg(*id, &status);
    if (!m_user || STATUS_NOT_OK) {
      if (m_user)
	free(m_user);
      return MDSplusERROR;
    }
    m.h.msglen = sizeof(MsgHdr);
    // AUTHORIZE //
    if (STATUS_OK && (m_user) && (m_user->h.dtype == DTYPE_CSTRING)) {
      user = malloc(m_user->h.length + 1);
      memcpy(user, m_user->bytes, m_user->h.length);
      user[m_user->h.length] = 0;
    }
    user_p = user ? user : "?";
    status = AuthorizeClient(*id, user_p);
    // SET COMPRESSION //
    if STATUS_OK {
      SetConnectionCompression(*id, m_user->h.status & 0xf);
      *usr = strcpy(malloc(strlen(user_p) + 1), user_p);
    } else
      *usr = NULL;
    if STATUS_NOT_OK
      fprintf(stderr, "Access denied: %s\n",user_p);
    else
      fprintf(stderr, "Connected: %s\n",user_p);
    if (user) free(user);
    m.h.status = STATUS_OK ? (1 | (GetConnectionCompression(*id) << 1)) : 0;
    m.h.client_type = m_user ? m_user->h.client_type : 0;
    if (m_user)
      MdsIpFree(m_user);
    // reply to client //
    SendMdsMsg(*id, &m, 0);
    if STATUS_NOT_OK
      DisconnectConnection(*id);
    fflush(stderr);
  }
  return status;
}

////////////////////////////////////////////////////////////////////////////////
//  FreeDescriptors  ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void FreeDescriptors(Connection * c){
  int i;
  if (c) {
    for (i = 0; i < MDSIP_MAX_ARGS; i++) {
      if (c->descrip[i]) {
	if (c->descrip[i] != MdsEND_ARG) {
	  if (c->descrip[i]->pointer)
	    free(c->descrip[i]->pointer);
	  free(c->descrip[i]);
	}
	c->descrip[i] = NULL;
      }
    }
  }
}



////////////////////////////////////////////////////////////////////////////////
//  DisconnectConnection  //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int DisconnectConnection(int conid){
  INIT_STATUS_AS MDSplusERROR;
  Connection *p, *c;
  CONNECTIONLIST_LOCK;
  c = _FindConnection(conid, &p);
  if (c) {
    if (p == 0)
      ConnectionList = c->next;
    else
      p->next = c->next;
  }
  CONNECTIONLIST_UNLOCK;
  if (c) {
    if (c->io && c->io->disconnect)
      c->io->disconnect(c);
    CONNECTIONLIST_LOCK;
    CONNECTIONLIST_UNLOCK;
    if (c->info)
      free(c->info);
    FreeDescriptors(c);
    free(c->protocol);
    free(c->info_name);
    free(c);
    status = MDSplusSUCCESS;
  }
  return status;
}

////////////////////////////////////////////////////////////////////////////////
//  GetConnectionIo  ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

IoRoutines *GetConnectionIo(int conid){
  IoRoutines *io = NULL;
  CONNECTIONLIST_LOCK;
  Connection *c = _FindConnection(conid, 0);
  if (c) io = c->io;
  CONNECTIONLIST_UNLOCK;
  return io;
}



////////////////////////////////////////////////////////////////////////////////
//  GetConnectionInfo  /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void *GetConnectionInfo_(Connection* c, char **info_name, int *readfd, size_t * len){
  void *ans = NULL;
  if (c) {
    if (len)
      *len = c->info_len;
    if (info_name)
      *info_name = c->info_name;
    if (readfd)
      *readfd = c->readfd;
    ans = c->info;
  }
  return ans;
}

void *GetConnectionInfo(int conid, char **info_name, int *readfd, size_t * len){
  void *ans;
  CONNECTIONLIST_LOCK;
  Connection *c = _FindConnection(conid, 0);
  ans = GetConnectionInfo_(c, info_name, readfd, len);
  CONNECTIONLIST_UNLOCK;
  return ans;
}

////////////////////////////////////////////////////////////////////////////////
//  SetConnectionInfo  /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void SetConnectionInfo(int conid, char *info_name, int readfd, void *info, size_t len){
  CONNECTIONLIST_LOCK;
  Connection *c = _FindConnection(conid, 0);
  if (c) {
    c->info_name = strcpy(malloc(strlen(info_name) + 1), info_name);
    if (info) {
      c->info = memcpy(malloc(len), info, len);
      c->info_len = len;
    } else {
      c->info = 0;
      c->info_len = 0;
    }
    c->readfd = readfd;
  }
  CONNECTIONLIST_UNLOCK;
}


////////////////////////////////////////////////////////////////////////////////
//  ConnectionCompression  /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void SetConnectionCompression(int conid, int compression){
  CONNECTIONLIST_LOCK;
  Connection *c = _FindConnection(conid, 0);
  if (c) c->compression_level = compression;
  CONNECTIONLIST_UNLOCK;
}

int GetConnectionCompression(int conid){
  int complv = 0;
  CONNECTIONLIST_LOCK;
  Connection *c = _FindConnection(conid, 0);
  if (c) complv = c->compression_level;
  CONNECTIONLIST_UNLOCK;
  return complv;
}

////////////////////////////////////////////////////////////////////////////////
//  IncrementConnectionMessageId  //////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

unsigned char IncrementConnectionMessageId(int conid){
  unsigned char id = 0;
  CONNECTIONLIST_LOCK;
  Connection *c = _FindConnection(conid, 0);
  if (c) {
    c->message_id++;
    if (!c->message_id) c->message_id = 1;
    id = c->message_id;
  }
  CONNECTIONLIST_UNLOCK;
  return id;
}



////////////////////////////////////////////////////////////////////////////////
//  GetConnectionMessageId  ////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

unsigned char GetConnectionMessageId(int conid){
  unsigned char id = 0;
  CONNECTIONLIST_LOCK;
  Connection *c = _FindConnection(conid, 0);
  if (c) id = c->message_id;
  CONNECTIONLIST_UNLOCK;
  return id;
}

///
/// Finds connection by id and sets the client_type field of the connection structure.
/// \note see ClientType() function.
///
/// \param conid the connection id
/// \param client_type the type of connection to be set
///
void SetConnectionClientType(int conid, int client_type){
  CONNECTIONLIST_LOCK;
  Connection *c = _FindConnection(conid, 0);
  if (c) c->client_type = client_type;
  CONNECTIONLIST_UNLOCK;
}

///
/// Finds connection by id and gets the client_type field of the connection structure
/// \note see ClientType() function.
///
/// \param conid the connection id
/// \return client_type value stored in connection structure
///
int GetConnectionClientType(int conid){
  int type = 0;
  CONNECTIONLIST_LOCK;
  Connection *c = _FindConnection(conid, 0);
  if (c) type = c->client_type;
  CONNECTIONLIST_UNLOCK;
  return type;
}
