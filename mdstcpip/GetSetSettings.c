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
#include "mdsip_connections.h"
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define DEFAULT_HOSTFILE "C:\\MDSIP.HOSTS"
#else
#define DEFAULT_HOSTFILE "/etc/mdsip.hosts"
#endif

static unsigned char multi = 0;
static int ContextSwitching = 0;
static int MaxCompressionLevel = 9;
static int CompressionLevel = 0;
static char *Portname = 0;
static char *protocol = "tcp";
static char *hostfile = 0;
static int flags = 0;
static int socketHandle = 0;

int GetSocketHandle()
{
  return socketHandle;
}

int SetSocketHandle(int handle)
{
  int old = socketHandle;
  socketHandle = handle;
  return old;
}

int GetFlags()
{
  return flags;
}

int SetFlags(int f)
{
  int old = flags;
  flags = f;
  return old;
}

char *GetProtocol()
{
  return protocol;
}

char *SetProtocol(char *p)
{
  char *old = protocol;
  protocol = p;
  return old;
}

char *GetPortname()
{
  return Portname;
}

char *MdsGetServerPortname()
{
  return Portname;
}

char *SetPortname(char *p)
{
  char *old = Portname;
  Portname = p;
  return old;
}

char *GetHostfile()
{
  return hostfile ? hostfile : DEFAULT_HOSTFILE;
}

char *SetHostfile(char *newhostfile)
{
  char *old = hostfile;
  hostfile = newhostfile;
  return old;
}

////////////////////////////////////////////////////////////////////////////////
//  CONTEXT  ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

char *GetLogDir() {
  char *logdir=getenv("MDSIP_SERVICE_LOGDIR");
  if (logdir && (strlen(logdir) > 0)) {
    logdir=strdup(logdir);
    size_t len = strlen(logdir);
    if ((logdir[len-1]=='\\') || (logdir[len-1]=='/')) {
      logdir[len-1]='\000';
    }
  } else {
    logdir=strdup("C:\\");
  }
  return logdir;
}

unsigned char GetMulti()
{
  return multi;
}

///
/// Set multi mode active in this scope. 
/// Mutiple connection mode (accepts multiple connections each with own context)
/// 
unsigned char SetMulti(unsigned char s)
{
  unsigned char old_multi = multi;
  multi = s;
  return old_multi;
}

int GetContextSwitching()
{
  return ContextSwitching;
}

int SetContextSwitching(int s)
{
  int old_ctx_switching = ContextSwitching;
  ContextSwitching = s;
  return old_ctx_switching;
}


////////////////////////////////////////////////////////////////////////////////
//  COMPRESSION  ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


int GetMaxCompressionLevel()
{
  return MaxCompressionLevel;
}

int SetMaxCompressionLevel(int s)
{
  int old_max_compression = MaxCompressionLevel;
  MaxCompressionLevel = s;
  return old_max_compression;
}

int SetCompressionLevel(int level)
{
  int old_level = CompressionLevel;
  CompressionLevel = level;
  return old_level;
}

int GetCompressionLevel()
{
  return CompressionLevel;
}


////////////////////////////////////////////////////////////////////////////////
//  CONNECTION TIMEOUT  ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static int connect_timeout = -1;
int SetMdsConnectTimeout(int sec)
{
  int old = connect_timeout;
  connect_timeout = sec;
  return old;
}


////////////////////////////////////////////////////////////////////////////////
//  GetMdsConnectTimeout  //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int GetMdsConnectTimeout()
{
  if (connect_timeout == -1) {
    char *timeout=getenv("MDSIP_CONNECT_TIMEOUT");
    if (timeout){
      connect_timeout = atoi(timeout); 
    } else {
      connect_timeout = 0;
    }
  }
  return connect_timeout;
}


////////////////////////////////////////////////////////////////////////////////
//  CLIENT ADDRESS  ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


static int ClientAddr = 0;
int MdsGetClientAddr()
{
  return ClientAddr;
}

/// Address of current client structure
void MdsSetClientAddr(int addr)
{
  ClientAddr = addr;
}
