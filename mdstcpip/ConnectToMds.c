#include <config.h>
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

static void ParseHost(char *hostin, char **protocol, char **host)
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
static int DoLogin(int id)
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
  m->h.status = GetConnectionCompression(id);
  m->h.ndims = 0;
  memcpy(m->bytes, user_p, length);
  status = SendMdsMsg(id, m, 0);
  free(m);
  if STATUS_OK {
    m = GetMdsMsgTO(id, &status, 10.f);
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
      SetConnectionCompression(id, (m->h.status & 0x1e) >> 1);
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
  IoRoutines *io;
  ParseHost(hostin, &protocol, &host);
  io = LoadIo(protocol);
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
    return -1;
  ParseHost(hostin, &protocol, &host);
  id = NewConnection(protocol);
  if (id != -1) {
    IoRoutines *io;
    io = GetConnectionIo(id);
    if (io && io->connect) {
      SetConnectionCompression(id, GetCompressionLevel());
      if (io->connect(id, protocol, host)<0 || IS_NOT_OK(DoLogin(id))) {
        DisconnectConnection(id);
        id = -1;
      }
    }
  }
  if (host)
    free(host);
  if (protocol)
    free(protocol);
  return id;
}
