#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <treeshr.h>
#include <mdsshr.h>

#include "mdsip_connections.h"

static Connection *ConnectionList = 0;
static int connection_mutex_initialized = 0;
static pthread_mutex_t connection_mutex;

///
/// Locks the connection list for safe multithreading access to ConnectionList
/// static variable.
///
static void lock_connection_list()
{
  if (!connection_mutex_initialized) {
    connection_mutex_initialized = 1;
    pthread_mutex_init(&connection_mutex, 0);
  }
  pthread_mutex_lock(&connection_mutex);
}

///
/// Unlocks the connection list for safe multithreading access to ConnectionList
/// static variable.
static void unlock_connection_list()
{

  if (!connection_mutex_initialized) {
    connection_mutex_initialized = 1;
    pthread_mutex_init(&connection_mutex, 0);
  }

  pthread_mutex_unlock(&connection_mutex);
}

///
/// Finds a Connection structure inside the static defined list ConnectionList.
/// \note The connection list is locked during operation.
///
/// \param id the Connection id to be searched for
/// \param prev if prev is set the Connection found is copied inside prev instance
/// \return the Connection intance identified by id or NULL pointer of not found.
///
Connection *FindConnection(int id, Connection ** prev)
{
  Connection *c = 0, *p;
  lock_connection_list();
  for (p = 0, c = ConnectionList; c && c->id != id; p = c, c = c->next) ;
  if (prev)
    *prev = p;
  unlock_connection_list();
  return c;
}


int NextConnection(void **ctx, char **info_name, void **info, size_t * info_len)
{
  Connection *c, *next;
  int ans;
  lock_connection_list();
  next = (*ctx != 0) ? (Connection *) * ctx : ConnectionList;
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
  unlock_connection_list();
  return ans;
}

int FlushConnection(int id)
{
  IoRoutines *io = GetConnectionIo(id);
  if (io)
    return io->flush ? io->flush(id) : 0;
  else
    return -1;
}

#ifdef HAVE_WINDOWS_H
static void exitHandler(void)
{
}
#else
static void exitHandler(void)
{
  int id;
  void *ctx = 0;
  while ((id = NextConnection(&ctx, 0, 0, 0)) != -1) {
    DisconnectConnection(id);
    ctx = 0;
  }
}
#endif

/// Creates new Connection instance, the proper ioRoutines is loaded selecting the protocol
/// from input argument. The created Connection is added to the static ConnectionList using
/// a thread safe lock.
/// The new Connection is initialized with the next connection id and the assigned protocol.
///
/// \param protocol the protocol to be used for this Connection
/// \return The new instanced connection id or -1 if error occurred.
///
int NewConnection(char *protocol)
{
  Connection *oldhead, *connection;
  IoRoutines *io = LoadIo(protocol);
  static int id = 1;
  static int registerExitHandler = 1;
  if (io) {
    registerExitHandler = registerExitHandler ? atexit(exitHandler) : 0;
    connection = memset(malloc(sizeof(Connection)), 0, sizeof(Connection));
    connection->io = io;
    connection->readfd = -1;
    lock_connection_list();
    oldhead = ConnectionList;
    ConnectionList = connection;
    connection->id = id++;
    connection->message_id = -1;
    connection->next = oldhead;
    connection->protocol = strcpy(malloc(strlen(protocol) + 1), protocol);
    unlock_connection_list();
    return connection->id;
  } else
    return -1;
}

///
/// Authorize client by username calling protocol IoRoutine.
/// 
/// \param id of the connection to use
/// \param username of the user to be authorized for access
/// \return true if authorized user found, false otherwise
///
static int AuthorizeClient(int id, char *username)
{
  Connection *c = FindConnection(id, 0);
  return c && c->io ? (c->io->authorize ? c->io->authorize(id, username) : 1) : 0;
}

///
/// Creates a new connection instance using given protocol, if a valid 
/// connection is instanced. 
/// 
/// Executes:
/// 1. SetConnectionInfo()
/// 2. AuthorizeClient()
/// 3. SetConnectionCompression()
///
///
/// \param protocol the protocol name identifier
/// \param info_name info name passed to SetConnectionInfo()
/// \param readfd input file descriptor i.e. the socket id
/// \param info 
/// \param info_len
/// \param id
/// \param usr
/// \return
///
int AcceptConnection(char *protocol, char *info_name, int readfd, void *info, size_t info_len,
		     int *id, char **usr)
{
  int ok = 0;
  *id = NewConnection(protocol);
  if (*id > 0) {
    static Message m;
    Message *m_user;
    char *user = 0;
    char *user_p = 0;
    int status;
    int m_status;
    
    // SET INFO //
    SetConnectionInfo(*id, info_name, readfd, info, info_len);
    m_user = GetMdsMsg(*id, &status);
    if (m_user == 0 || !(status & 1)) {
      if (m_user)
	free(m_user);
      return 0;
    }
    m.h.msglen = sizeof(MsgHdr);
    
    // AUTHORIZE //
    if ((status & 1) && (m_user) && (m_user->h.dtype == DTYPE_CSTRING)) {
      user = malloc(m_user->h.length + 1);
      memcpy(user, m_user->bytes, m_user->h.length);
      user[m_user->h.length] = 0;
    }
    user_p = user ? user : "?";
    ok = AuthorizeClient(*id, user_p);
    
    // SET COMPRESSION //
    if (ok & 1) {
      SetConnectionCompression(*id, m_user->h.status & 0xf);
      *usr = strcpy(malloc(strlen(user_p) + 1), user_p);
    } else
      *usr = 0;
    m_status = m.h.status = (ok & 1) ? (1 | (GetConnectionCompression(*id) << 1)) : 0;    
    
    // client type //
    m.h.client_type = m_user ? m_user->h.client_type : 0;
    
    if (m_user)
      MdsIpFree(m_user);
    
    // reply to client //
    SendMdsMsg(*id, &m, 0);
    
    if (!(ok & 1)) {
      fprintf(stderr, "Access denied\n");
      DisconnectConnection(*id);
    } else
      fprintf(stderr, "Connected\n");
    fflush(stderr);
  }
  return ok;
}

///
/// Free all desctriptors held by the Connection structure in argument.
/// \param c the connection to be freed
///
void FreeDescriptors(Connection * c)
{
  int i;
  if (c) {
    for (i = 0; i < MAX_ARGS; i++) {
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

///
/// Disconnect the connection identified by id.
///
/// \param conid the id of connection to be disconnected
/// \return true if the connection was correctly freed or false otherwise.
///
int DisconnectConnection(int conid)
{
  int status = 0;
  Connection *p, *c;
  c = FindConnection(conid, &p);
  if (c && c->deleted == 0) {
    c->deleted = 1;
    if (c->io && c->io->disconnect)
      c->io->disconnect(conid);
    lock_connection_list();
    if (p == 0)
      ConnectionList = c->next;
    else
      p->next = c->next;
    if (c->info)
      free(c->info);
    FreeDescriptors(c);
    free(c->protocol);
    free(c->info_name);
    free(c);
    status = 1;
    unlock_connection_list();
  }
  return status;
}

///
/// \brief GetConnectionIo
/// finds the Connection structure in ConnectionList and
/// returns the ioRoutines structure associated to a given Connection identified by id.
///
/// \param conid id of the connection that holds the ioRoutines
/// \return ioRoutines structure in the io field of Connection element found.
///
IoRoutines *GetConnectionIo(int conid)
{
  Connection *c = FindConnection(conid, 0);
  return (c != 0) ? c->io : 0;
}

void *GetConnectionInfo(int conid, char **info_name, int *readfd, size_t * len)
{
  Connection *c = FindConnection(conid, 0);
  void *ans = 0;
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

///
/// Sets connection info to a \ref Connection structure identified by id.
/// For example the TCP routine "tcp_connect" of ioRoutinesTcp.c uses this
/// function as the following: SetConnectionInfo(conid, "tcp", soket_id, 0, 0);
///
/// \param conid the connection id to set the info to.
/// \param info_name the name associated to the info structure inside \ref Connection
/// \param readfd the file descriptor associated to the Connection
/// \param info accessory info descriptor
/// \param len length of accessory info descriptor
///
void SetConnectionInfo(int conid, char *info_name, int readfd, void *info, size_t len)
{
  Connection *c = FindConnection(conid, 0);
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


///
/// Finds the Connection intance held in the list of connections by id and
/// sets the compression level indicated in arguments.
/// Compression level spans from 0 (no compression) to \ref GetMaxCompressionLevel (maximum compression level usually the integer value 9)
///
/// \param conid id of the Connection to set the compression
/// \param compression the compression level to set
///
void SetConnectionCompression(int conid, int compression)
{
  Connection *c = FindConnection(conid, 0);
  if (c) {
    c->compression_level = compression;
  }
}

///
/// Finds the Connection intance held in the list of connections by id and
/// get the value of compression level stored inside.
/// Compression level spans from 0 (no compression) to \ref GetMaxCompressionLevel (maximum compression level usually the integer value 9)
///
/// \param conid the Connection structure id to query.
/// \return the compression level value found or 0 (no compression) if Connection was not found
///
int GetConnectionCompression(int conid)
{
  Connection *c = FindConnection(conid, 0);
  if (c) {
    return c->compression_level;
  } else
    return 0;
}


///
/// Finds the Connection intance held in the list of connections by id and
/// increments the connection message id.
///
/// \param conid id of the connection
/// \return the incremented connection message id or 0 if connection was not found.
///
unsigned char IncrementConnectionMessageId(int conid)
{
  unsigned char ans = 0;
  Connection *c = FindConnection(conid, 0);
  if (c) {
    c->message_id++;
    if (c->message_id == 0)
      c->message_id = 1;
    ans = c->message_id;
  }
  return ans;
}

///
/// Finds connection by id and returns its the message id
///
/// \param conid the connection id
/// \return message_id field of selected connection or 0 as no connection found
///
unsigned char GetConnectionMessageId(int conid)
{
  Connection *c = FindConnection(conid, 0);
  if (c) {
    return c->message_id;
  } else
    return 0;
}

///
/// Finds connection by id and sets the client_type field of the connection structure.
/// \note see ClientType() function.
///
/// \param conid the connection id
/// \param client_type the type of connection to be set
///
void SetConnectionClientType(int conid, int client_type)
{
  Connection *c = FindConnection(conid, 0);
  if (c) {
    c->client_type = client_type;
  }
}

///
/// Finds connection by id and gets the client_type field of the connection structure
/// \note see ClientType() function.
///
/// \param conid the connection id
/// \return client_type value stored in connection structure
///
int GetConnectionClientType(int conid)
{
  Connection *c = FindConnection(conid, 0);
  if (c) {
    return c->client_type;
  } else
    return 0;
}
