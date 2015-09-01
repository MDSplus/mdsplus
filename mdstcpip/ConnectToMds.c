#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifndef _WIN32
#include <pwd.h>
#endif

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
/// \return status o login into server 1 if success, -1 if not authorized or error
/// occurred
///
static int DoLogin(int id)
{
  int status;
  Message *m;
#ifdef _WIN32
  static char user[128];
  int bsize = 128;
#ifdef _NI_RT_
  char *user_p = "Windows User";
#else
  char *user_p = GetUserName(user, &bsize) ? user : "Windows User";
#endif
#elif __MWERKS__
  static char user[128];
  int bsize = 128;
  char *user_p = "Macintosh User";
#elif __APPLE__
  char *user_p;
  struct passwd *pwd;
  pwd = getpwuid(geteuid());
  user_p = pwd->pw_name;
#else
  char *user_p;
  struct passwd *passStruct = getpwuid(geteuid());
  if (!passStruct) {
    /* 
     *  On some RHEL6/64 systems 32 bit
     *  calls to getpwuid return 0
     *  temporary fix to call getlogin()
     *  in that case.
     */
    user_p = getlogin();
    if (!user_p)
      user_p = "Linux";
  } else
    user_p = passStruct->pw_name;
#endif
  m = malloc(sizeof(MsgHdr) + strlen(user_p));
  memset(m, 0, sizeof(MsgHdr) + strlen(user_p));
  m->h.client_type = SENDCAPABILITIES;
  m->h.length = (short)strlen(user_p);
  m->h.msglen = sizeof(MsgHdr) + m->h.length;
  m->h.dtype = DTYPE_CSTRING;
  m->h.status = GetConnectionCompression(id);
  m->h.ndims = 0;
  memcpy(m->bytes, user_p, m->h.length);
  status = SendMdsMsg(id, m, 0);
  free(m);
  if (status & 1) {
    m = GetMdsMsg(id, &status);
    if (m == 0 || !(status & 1)) {
      printf("Error in connect\n");
      return -1;
    } else {
      if (!(m->h.status & 1)) {
	printf("Error in connect: Access denied\n");
	free(m);
	return -1;
      }
      // SET CLIENT COMPRESSION FROM SERVER //
      SetConnectionCompression(id, (m->h.status & 0x1e) >> 1);
    }
    if (m)
      free(m);
  } else {
    perror("Error connecting to server");
    return -1;
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
  int status;
  char *host = 0;
  char *protocol = 0;
  IoRoutines *io;
  ParseHost(hostin, &protocol, &host);
  io = LoadIo(protocol);
  if (io) {
    if (io->reuseCheck)
      status = io->reuseCheck(host, unique, buflen);
    else {
      strncpy(unique, hostin, buflen);
      status = 0;
    }
  } else {
    memset(unique, 0, buflen);
    status = -1;
  }
  if (protocol)
    free(protocol);
  if (host)
    free(host);
  return status;
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
      if (io->connect(id, protocol, host) == -1) {
	DisconnectConnection(id);
	id = -1;
      } else if (DoLogin(id) == -1) {
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
