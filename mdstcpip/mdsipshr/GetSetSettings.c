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
#include "../mdsip_connections.h"
#include "mdsipthreadstatic.h"
#include <stdlib.h>
#include <string.h>

//#define DEBUG
#include <mdsmsg.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
#define GET_THREAD_SAFE(var) ({ \
  typeof(var) ans;              \
  pthread_mutex_lock(&lock);    \
  ans = var;                    \
  pthread_mutex_unlock(&lock);  \
  ans;                          \
})
#define SET_THREAD_SAFE(var, val) ({ \
  typeof(var) ans;                   \
  pthread_mutex_lock(&lock);         \
  ans = var;                         \
  var = val;                         \
  pthread_mutex_unlock(&lock);       \
  ans;                               \
})

static SOCKET socket_handle = 0;
SOCKET GetSocketHandle()
{
  return GET_THREAD_SAFE(socket_handle);
}
SOCKET SetSocketHandle(SOCKET handle)
{
  return SET_THREAD_SAFE(socket_handle, handle);
}

static int flags = 0;
int GetFlags()
{
  return GET_THREAD_SAFE(flags);
}
int SetFlags(int f)
{
  return SET_THREAD_SAFE(flags, f);
}

static char *protocol = "tcp";
char *GetProtocol()
{
  return GET_THREAD_SAFE(protocol);
}
char *SetProtocol(char *p)
{
  return SET_THREAD_SAFE(protocol, p);
}

static char *portname = 0;
char *GetPortname()
{
  return GET_THREAD_SAFE(portname);
}
char *SetPortname(char *p)
{
  return SET_THREAD_SAFE(portname, p);
}

#ifdef _WIN32
#define DEFAULT_HOSTFILE "C:\\mdsip.hosts"
#else
#define DEFAULT_HOSTFILE "/etc/mdsip.hosts"
#endif
static char *hostfile = NULL;
char *GetHostfile()
{
  char *ans = GET_THREAD_SAFE(hostfile);
  return ans ? ans : DEFAULT_HOSTFILE;
}
char *SetHostfile(char *newhostfile)
{
  return SET_THREAD_SAFE(hostfile, newhostfile);
}

/// Set multi mode active in this scope.
/// Mutiple connection mode (accepts multiple connections each with own context)
static unsigned char multi = 0;
unsigned char GetMulti()
{
  return GET_THREAD_SAFE(multi);
}
unsigned char SetMulti(unsigned char s)
{
  return SET_THREAD_SAFE(multi, s);
}

static int ContextSwitching = 0;
int GetContextSwitching()
{
  return GET_THREAD_SAFE(ContextSwitching);
}
int SetContextSwitching(int s)
{
  return SET_THREAD_SAFE(ContextSwitching, s);
}

//  COMPRESSION
static int compression = 0;
/// used in ConnectToMds, mdsip_service
int GetCompressionLevel()
{
  return GET_THREAD_SAFE(compression);
}
int SetCompressionLevel(int level)
{
  return SET_THREAD_SAFE(compression, level);
}

/// CLIENT ADDRESS
/// used in ServerShr
int MdsGetClientAddr()
{
  MDSIPTHREADSTATIC_INIT;
  MDSDBG("GET CLIENT " IPADDRPRI, IPADDRVAR(&MDSIP_CLIENTADDR));
  return MDSIP_CLIENTADDR;
}
void MdsSetClientAddr(int addr)
{
  MDSIPTHREADSTATIC_INIT;
  MDSIP_CLIENTADDR = addr;
  MDSDBG("SET CLIENT " IPADDRPRI, IPADDRVAR(&MDSIP_CLIENTADDR));
}

#ifdef _WIN32
char *GetLogDir()
{
  char *logdir = getenv("MDSIP_SERVICE_LOGDIR");
  if (logdir && (strlen(logdir) > 0))
  {
    logdir = strdup(logdir);
    size_t len = strlen(logdir);
    if ((logdir[len - 1] == '\\') || (logdir[len - 1] == '/'))
    {
      logdir[len - 1] = '\000';
    }
  }
  else
  {
    logdir = strdup("C:\\");
  }
  return logdir;
}
#endif