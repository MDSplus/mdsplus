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
#include <pthread_port.h>
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <status.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef _WIN32
# include <process.h>
# define close_pipe(p) CloseHandle(p)
#else
# include <sys/wait.h>
# define INVALID_SOCKET -1
# define close_pipe(p) close(p)
#endif

typedef struct {
#ifdef _WIN32
  HANDLE out;
  HANDLE in;
  HANDLE pid;
#else
  int out;
  int in;
# ifdef PROT_TUNNEL
  int pid;
# endif
#endif
#ifdef PROT_THREAD
  pthread_t pth;
#endif
} io_pipes_t;

static io_pipes_t *get_pipes(Connection* c){
  size_t len;
  char *info_name;
  io_pipes_t *p = (io_pipes_t *)GetConnectionInfoC(c, &info_name, 0, &len);
  return (info_name && !strcmp(PROTOCOL, info_name) && len == sizeof(io_pipes_t)) ? p : 0;
}

static ssize_t io_send(Connection* c, const void *buffer, size_t buflen, int nowait __attribute__ ((unused))){
  io_pipes_t *p = get_pipes(c);
  if (!p) return -1;
#ifdef _WIN32
  ssize_t num = 0;
  return WriteFile(p->out, buffer, buflen, (DWORD *)&num, NULL) ? num : -1;
#else
  return write(p->out, buffer, buflen);
#endif
}

static ssize_t io_recv_to(Connection* c, void *buffer, size_t buflen, int to_msec){
  io_pipes_t *p = get_pipes(c);
  if (!p) return -1;
#ifdef _WIN32
  DWORD toval;
  if      (to_msec< 0) toval = 0;
  else if (to_msec==0) toval = MAXDWORD;
  else                 toval = to_msec;
  COMMTIMEOUTS timeouts = { 0, 0, toval, 0, 0};
  SetCommTimeouts(p->in, &timeouts);
  ssize_t num = 0;
  return ReadFile(p->in, buffer, buflen, (DWORD *)&num, NULL) ? num : -1;
#else
  struct timeval to,timeout;
  if (to_msec<0) {
    timeout.tv_sec  = 1;
    timeout.tv_usec = 0;
  } else {
    timeout.tv_sec  = to_msec / 1000;
    timeout.tv_usec =(to_msec % 1000) * 1000;
  }
  int sel,fd = p->in;
  fd_set rf,readfds;
  FD_ZERO(&readfds);
  FD_SET(fd, &readfds);
  do {// loop even for nowait for responsiveness
    to = timeout; rf = readfds;
    sel = select(fd+1, &rf, NULL, NULL, &to);
    if (sel > 0) // good to go
      return read(fd, buffer, buflen);
    if (errno == EAGAIN) continue;
    if (sel < 0) // Error
      return sel;
  } while (to_msec<0);
  return 0; // timeout
#endif
}

static ssize_t io_recv(Connection* c, void *buffer, size_t buflen){
  return io_recv_to(c,buffer,buflen,-1);
}
