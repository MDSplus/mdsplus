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
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef WIN32
#include <process.h>
#define close_pipe(p) CloseHandle(p)
#else
#include <sys/wait.h>
#define close_pipe(p) close(p)
#endif

#include "../mdsip_connections.h"
#include <pthread_port.h>
#include <status.h>

// #define DEBUG
#include <mdsmsg.h>

typedef struct
{
#ifdef WIN32
  HANDLE out;
  HANDLE in;
  HANDLE pid;
#else
  int out;
  int in;
#ifdef PROT_TUNNEL
  int pid;
#endif
#endif
#ifdef PROT_THREAD
  pthread_t pth;
#endif
} io_pipes_t;

static io_pipes_t *get_pipes(Connection *c)
{
  size_t len;
  char *info_name;
  io_pipes_t *p = (io_pipes_t *)ConnectionGetInfo(c, &info_name, 0, &len);
  return (info_name && !strcmp(PROTOCOL, info_name) &&
          len == sizeof(io_pipes_t))
             ? p
             : NULL;
}

static ssize_t io_send(Connection *c, const void *buffer, size_t buflen,
                       int nowait __attribute__((unused)))
{
  io_pipes_t *p = get_pipes(c);
  if (!p)
    return -1;
  ssize_t ans = 0;
  errno = 0;
#ifdef WIN32
  if (!WriteFile(p->out, buffer, buflen, (DWORD *)&ans, NULL))
    ans = -1;
#else
  ans = write(p->out, buffer, buflen);
#endif
  MDSDBG("conid=%d, ans=%" PRId64 ", errno=%d: %s",
         c->id, (int64_t)ans, errno, strerror(errno));
  return ans;
}

static ssize_t io_recv_to(Connection *c, void *buffer, size_t buflen,
                          const int to_msec)
{
  io_pipes_t *p = get_pipes(c);
  if (!p)
    return -1;
  ssize_t ans = 0;
  errno = 0;
#ifdef WIN32
  DWORD toval;
  if (to_msec < 0)
    toval = 0;
  else if (to_msec == 0)
    toval = MAXDWORD;
  else
    toval = to_msec;
  COMMTIMEOUTS timeouts = {0, 0, toval, 0, 0};
  SetCommTimeouts(p->in, &timeouts);
  if (!ReadFile(p->in, buffer, buflen, (DWORD *)&ans, NULL))
    ans = -1;
#else
  struct timeval to, timeout;
  if (to_msec < 0)
  {
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
  }
  else
  {
    timeout.tv_sec = to_msec / 1000;
    timeout.tv_usec = (to_msec % 1000) * 1000;
  }
  int fd = p->in;
  fd_set readfds;
  FD_ZERO(&readfds);
  to = timeout;
  for (;;)
  { // loop even for nowait for responsiveness
    FD_SET(fd, &readfds);
    ans = select(fd + 1, &readfds, NULL, NULL, &to);
    MDSDBG("select %d, conid=%d, ans=%" PRId64 ", errno=%d: %s",
           fd, c->id, (int64_t)ans, errno, strerror(errno));
    if (ans > 0) // good to go
    {
      ans = read(fd, buffer, buflen);
      MDSDBG("read %d, conid=%d, ans=%" PRId64 ", errno=%d: %s",
             fd, c->id, (int64_t)ans, errno, strerror(errno));
      break;
    }
    else if (ans < 0)
    {
      if (errno == EINTR)
        continue;
      if (errno != EAGAIN)
        break;
    }
    if (to_msec >= 0)
    {
      ans = 0;
      errno = ETIMEDOUT;
      break;
    }
    to = timeout;
  }
#endif
  MDSDBG("conid=%d, ans=%" PRId64 ", errno=%d: %s",
         c->id, (int64_t)ans, errno, strerror(errno));
  return ans;
}

static ssize_t io_recv(Connection *c, void *buffer, size_t buflen)
{
  return io_recv_to(c, buffer, buflen, -1);
}
