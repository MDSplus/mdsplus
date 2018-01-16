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

//#define DEBUG

static Connection *ConnectionList = NULL;
static pthread_mutex_t connection_mutex = PTHREAD_MUTEX_INITIALIZER;

#define CONNECTIONLIST_LOCK   pthread_mutex_lock(&connection_mutex);pthread_cleanup_push((void*)pthread_mutex_unlock, (void*)&connection_mutex);
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
  CONNECTIONLIST_UNLOCK;
  return c;
}

Connection *FindConnectionWithLock(int id, char state){
  Connection *c;
  CONNECTIONLIST_LOCK;
  c = _FindConnection(id, NULL);
  if (c) {
    while (c->state>0) {
#ifdef DEBUG
      fprintf(stderr,"Connection %02d -- %02x waiting\n",c->id,(unsigned char)state);
#endif
      pthread_cond_wait(&c->cond,&connection_mutex);
    }
    if (c->state<0) {
      pthread_cond_signal(&c->cond); // pass on signal
      c = NULL;
    } else {
#ifdef DEBUG
      fprintf(stderr,"Connection %02d -> %02x   locked\n",c->id,(unsigned char)state);
#endif
      c->state = state;
    }
  }
  CONNECTIONLIST_UNLOCK
  return c;
}

void UnlockConnection(Connection* c) {
  if (c) {
    CONNECTIONLIST_LOCK;
    c->state &= CON_DISCONNECT; // preserve CON_DISCONNECT
#ifdef DEBUG
      fprintf(stderr,"Connection %02d -> %02x unlocked\n",c->id,(unsigned char)c->state);
#endif
    pthread_cond_signal(&c->cond);
    CONNECTIONLIST_UNLOCK;
  }
}

#define CONNECTION_UNLOCK_PUSH(c) pthread_cleanup_push((void*)UnlockConnection,(void*)c)
#define CONNECTION_LOCK(c,state)  LockConnection(c,state);CONNECTION_UNLOCK_PUSH(c)
#define CONNECTION_UNLOCK(c)      pthread_cleanup_pop(1)

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
    ans = INVALID_CONNECTION_ID;
  }
  CONNECTIONLIST_UNLOCK;
  return ans;
}

int SendToConnection(int id, const void *buffer, size_t buflen, int nowait){
  int res;
  Connection* c = FindConnectionWithLock(id,CON_SEND);
  CONNECTION_UNLOCK_PUSH(c);
  if (c && c->io && c->io->send)
    res = c->io->send(c, buffer, buflen, nowait);
  else res = -1;
  CONNECTION_UNLOCK(c);
  return res;
}

int FlushConnection(int id){
  int res;
  Connection* c = FindConnectionWithLock(id,CON_FLUSH);
  CONNECTION_UNLOCK_PUSH(c);
  if (c && c->io)
    res = c->io->flush ? c->io->flush(c) : 0;
  else res = -1;
  CONNECTION_UNLOCK(c);
  return res;
}

int ReceiveFromConnection(int id, void *buffer, size_t buflen){
  int res;
  Connection* c = FindConnectionWithLock(id,CON_RECV);
  CONNECTION_UNLOCK_PUSH(c);
  if (c && c->io && c->io->recv)
    res = c->io->recv(c, buffer, buflen);
  else res = -1;
  CONNECTION_UNLOCK(c);
  return res;
}

static void exitHandler(void){
  int id;
  void *ctx = (void *)-1;
  while ((id = NextConnection(&ctx, 0, 0, 0)) != INVALID_CONNECTION_ID) {
    DisconnectConnection(id);
    ctx = 0;
  }
}
static void registerHandler(){
  atexit(exitHandler);
}

////////////////////////////////////////////////////////////////////////////////
//  DisconnectConnection  //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void DisconnectConnectionC(Connection *c){
  c->io->disconnect(c);
  if (c->info)
    free(c->info);
  FreeDescriptors(c);
  free(c->protocol);
  free(c->info_name);
  pthread_cond_destroy(&c->cond);
  free(c);
}

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
    c->state |= CON_DISCONNECT;
    while (c->state&!CON_DISCONNECT) {
      pthread_cond_broadcast(&c->cond);
      pthread_cond_wait(&c->cond,&connection_mutex);
    } // allow current action to finish
  }
  CONNECTIONLIST_UNLOCK;
  if (c) {
    DisconnectConnectionC(c);
    status = MDSplusSUCCESS;
  }
  return status;
}

////////////////////////////////////////////////////////////////////////////////
//  NewConnection  /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Connection* NewConnectionC(char *protocol){
  Connection *connection;
  IoRoutines *io = LoadIo(protocol);
  static pthread_once_t registerExitHandler = PTHREAD_ONCE_INIT;
  if (io) {
    (void) pthread_once(&registerExitHandler,registerHandler);
    connection = memset(malloc(sizeof(Connection)), 0, sizeof(Connection));
    connection->io = io;
    connection->readfd = -1;
    connection->message_id = -1;
    connection->protocol = strcpy(malloc(strlen(protocol) + 1), protocol);
    connection->id = INVALID_CONNECTION_ID;
    connection->state = CON_IDLE;
    pthread_cond_init(&connection->cond,NULL);
    return connection;
  } else
    return NULL;
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
//  GetConnectionIo  ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

IoRoutines *GetConnectionIo(int conid){
  IoRoutines *io;
  CONNECTIONLIST_LOCK;
  Connection *c = _FindConnection(conid, 0);
  io = c ? c->io : NULL;
  CONNECTIONLIST_UNLOCK;
  return io;
}



////////////////////////////////////////////////////////////////////////////////
//  GetConnectionInfo  /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void *GetConnectionInfoC(Connection* c, char **info_name, int *readfd, size_t * len){
  if (c) {
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

void *GetConnectionInfo(int conid, char **info_name, int *readfd, size_t * len){
  void *ans;
  CONNECTIONLIST_LOCK;
  Connection *c = _FindConnection(conid, 0);
  ans = GetConnectionInfoC(c, info_name, readfd, len);
  CONNECTIONLIST_UNLOCK;
  return ans;
}

////////////////////////////////////////////////////////////////////////////////
//  SetConnectionInfo  /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void SetConnectionInfoC(Connection* c, char *info_name, int readfd, void *info, size_t len){
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
}

void SetConnectionInfo(int conid, char *info_name, int readfd, void *info, size_t len){
  CONNECTIONLIST_LOCK;
  Connection *c = _FindConnection(conid, 0);
  if (c) SetConnectionInfoC(c, info_name, readfd, info, len);
  CONNECTIONLIST_UNLOCK;
}


////////////////////////////////////////////////////////////////////////////////
//  ConnectionCompression  /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void SetConnectionCompression(int conid, int compression){
  CONNECTIONLIST_LOCK;
  Connection *c = _FindConnection(conid, NULL);
  if (c) c->compression_level = compression;
  CONNECTIONLIST_UNLOCK;
}

static inline int GetConnectionCompressionC(Connection* c){
  return c ? c->compression_level : 0;
}
int GetConnectionCompression(int conid){
  int complv;
  CONNECTIONLIST_LOCK;
  Connection *c = _FindConnection(conid, NULL);
  complv = GetConnectionCompressionC(c);
  CONNECTIONLIST_UNLOCK;
  return complv;
}

////////////////////////////////////////////////////////////////////////////////
//  IncrementConnectionMessageId  //////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

unsigned char IncrementConnectionMessageIdC(Connection* c){
  if (c) {
    c->message_id++;
    if (c->message_id==0) c->message_id = 1;
    return c->message_id;
  }
  return 0;
}

unsigned char IncrementConnectionMessageId(int conid){
  unsigned char id;
  CONNECTIONLIST_LOCK;
  Connection *c = _FindConnection(conid, NULL);
  id = IncrementConnectionMessageIdC(c);
  CONNECTIONLIST_UNLOCK;
  return id;
}


////////////////////////////////////////////////////////////////////////////////
//  GetConnectionMessageId  ////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

inline static unsigned char GetConnectionMessageIdC(Connection* c){
  return c ? c->message_id : 0;
}

unsigned char GetConnectionMessageId(int conid){
  unsigned char id;
  CONNECTIONLIST_LOCK;
  Connection *c = _FindConnection(conid, 0);
  id = GetConnectionMessageIdC(c);
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
  int type;
  CONNECTIONLIST_LOCK;
  Connection *c = _FindConnection(conid, 0);
  type = c ? c->client_type : 0;
  CONNECTIONLIST_UNLOCK;
  return type;
}

////////////////////////////////////////////////////////////////////////////////
//  AuthorizeClient  ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/// Authorize client by username calling protocol IoRoutine.
/// \param id of the connection to use
/// \param username of the user to be authorized for access
/// \return true if authorized user found, false otherwise

static inline int authorizeClient(Connection* c, char *username){
  if (c && c->io)
    return c->io->authorize ? c->io->authorize(c, username) : MDSplusSUCCESS;
  return MDSplusERROR;
}

////////////////////////////////////////////////////////////////////////////////
//  AcceptConnection  //////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int AddConnection(Connection* c) {
  static int id = 1;
  CONNECTIONLIST_LOCK;
  if (id==INVALID_CONNECTION_ID)
    while (_FindConnection(++id, 0)); //find next free id
  c->id   = id++;
  c->next = ConnectionList;
  ConnectionList = c;
  CONNECTIONLIST_UNLOCK;
  return c->id;
}


int AcceptConnection(char *protocol, char *info_name, int readfd, void *info, size_t info_len, int *id, char **usr){
  Connection* c = NewConnectionC(protocol);
  INIT_STATUS_ERROR;
  if (c) {
    static Message m;
    Message *m_user;
    char *user = NULL, *user_p = NULL;
    // SET INFO //
    SetConnectionInfoC(c, info_name, readfd, info, info_len);
    m_user = GetMdsMsgTOC(c, &status, 10000);
    if (!m_user || STATUS_NOT_OK) {
      if (m_user)
	free(m_user);
      if (usr) *usr = NULL;
      DisconnectConnectionC(c);
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
    status = authorizeClient(c, user_p);
    // SET COMPRESSION //
    if STATUS_OK {
      c->compression_level = m_user->h.status & 0xf;
      *usr = strcpy(malloc(strlen(user_p) + 1), user_p);
    } else
      *usr = NULL;
    if STATUS_NOT_OK
      fprintf(stderr, "Access denied: %s\n",user_p);
    else
      fprintf(stderr, "Connected: %s\n",user_p);
    if (user) free(user);
    m.h.status = STATUS_OK ? (1 | (c->compression_level << 1)) : 0;
    m.h.client_type = m_user ? m_user->h.client_type : 0;
    if (m_user)
      MdsIpFree(m_user);
    // reply to client //
    SendMdsMsgC(c, &m, 0);
    if STATUS_OK {
      // all good add connection
      *id = AddConnection(c);
    } else
      DisconnectConnectionC(c);
    fflush(stderr);
  }
  return status;
}
