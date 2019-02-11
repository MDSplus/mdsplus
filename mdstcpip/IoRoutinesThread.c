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
#include <STATICdef.h>
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
# define THREAD_PID    GetCurrentThread()
#else
# include <sys/wait.h>
# define close_pipe(p) close(p)
#endif

static ssize_t thread_send(Connection* c, const void *buffer, size_t buflen, int nowait);
static ssize_t thread_recv(Connection* c, void *buffer, size_t len);
static ssize_t thread_recv_to(Connection* c, void *buffer, size_t len, int to_msec);
static int thread_disconnect(Connection* c);
static int thread_connect(Connection* c, char *protocol, char *host);
const IoRoutines thread_routines = {
 thread_connect, thread_send, thread_recv, NULL, NULL, NULL, NULL, thread_disconnect, thread_recv_to, NULL
};

#define PARENT_THREAD ((pthread_t)-1)

typedef struct {
#ifdef _WIN32
  HANDLE out;
  HANDLE in;
  HANDLE pid;
#else
  int out;
  int in;
#endif
  pthread_t pth;
} thread_pipes_t;

static thread_pipes_t *getThreadPipes(Connection* c){
  size_t len;
  char *info_name;
  thread_pipes_t *p = (thread_pipes_t *)GetConnectionInfoC(c, &info_name, 0, &len);
  return (info_name && !strcmp("thread", info_name) && len == sizeof(thread_pipes_t)) ? p : 0;
}

ssize_t thread_send(Connection* c, const void *buffer, size_t buflen, int nowait __attribute__ ((unused))){
  thread_pipes_t *p = getThreadPipes(c);
  if (!p) return -1;
#ifdef _WIN32
  ssize_t num = 0;
  return WriteFile(p->in, buffer, buflen, (DWORD *)&num, NULL) ? num : -1;
#else
  return write(p->in, buffer, buflen);
#endif
}

ssize_t thread_recv(Connection* c, void *buffer, size_t buflen){
  return thread_recv_to(c,buffer,buflen,-1);
}

ssize_t thread_recv_to(Connection* c, void *buffer, size_t buflen, int to_msec){
  thread_pipes_t *p = getThreadPipes(c);
  if (!p) return -1;
#ifdef _WIN32
  DWORD toval;
  if      (to_msec< 0) toval = 0;
  else if (to_msec==0) toval = MAXDWORD;
  else                 toval = to_msec;
  COMMTIMEOUTS timeouts = { 0, 0, toval, 0, 0};
  SetCommTimeouts(p->out, &timeouts);
  ssize_t num = 0;
  return ReadFile(p->out, buffer, buflen, (DWORD *)&num, NULL) ? num : -1;
#else
  if (to_msec>=0) { // don't time out if to_msec < 0
    struct timeval timeout;
    timeout.tv_sec  = to_msec / 1000;
    timeout.tv_usec =(to_msec % 1000) * 1000;
    fd_set set;
    FD_ZERO(&set); /* clear the set */
    FD_SET(p->out, &set); /* add our file descriptor to the set */
    int rv = select(p->out + 1, &set, NULL, NULL, &timeout);
    if (rv<=0) return rv;
  }
  return read(p->out, buffer, buflen);
#endif
}

static int thread_disconnect(Connection* c){
  thread_pipes_t *p = getThreadPipes(c);
  if (p && p->pth != PARENT_THREAD) {
#ifdef _WIN32
    if (WaitForSingleObject(p->pid, 0) == WAIT_TIMEOUT)
      TerminateThread(p->pid,0);
    CloseHandle(p->pid);
#else
    pthread_cancel(p->pth);
    pthread_join(p->pth,NULL);
#endif
    close_pipe(p->in);
    close_pipe(p->out);
  }
  return C_OK;
}


static void thread_cleanup(void*pp){
  void *ctx = (void *)-1;
  int id;
  char *info_name;
  thread_pipes_t *info;
  size_t info_len = 0;
  pthread_t me = pthread_self();
  while ((id = NextConnection(&ctx, &info_name, (void*)&info, &info_len)) != INVALID_CONNECTION_ID) {
    if (info_name && strcmp(info_name, "thread") == 0
     && pthread_equal(info->pth,me)) {
      DisconnectConnection(id);
      break;
    }
  }
  free(pp);
}

static void thread_listen(void*pp) {
  int id, status;
  pthread_cleanup_push(thread_cleanup,pp);
  INIT_AND_FREE_ON_EXIT(char*,username);
  status = AcceptConnection("thread", "thread", 0, pp, sizeof(thread_pipes_t), &id, &username);
  FREE_NOW(username);
  if STATUS_OK while (DoMessage(id));
  close_pipe(((thread_pipes_t*)pp)->in);
  close_pipe(((thread_pipes_t*)pp)->out);
  pthread_cleanup_pop(1);
}

inline static int thread_connect(Connection* c, char* protocol __attribute__((unused)), char *host __attribute__((unused))) {
#ifdef _WIN32
  thread_pipes_t p = {NULL,NULL,NULL,0}, *pp = calloc(sizeof(p),1);
  pp->pth = PARENT_THREAD;
  SECURITY_ATTRIBUTES saAttr;
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;
  if (!CreatePipe(&p.out, &pp->in, &saAttr, 0)) fprintf(stderr, "StdoutRd CreatePipe");
  if (!CreatePipe(&pp->out, &p.in, &saAttr, 0)) fprintf(stderr, "Stdin CreatePipe");
  if (!(p.pid = CreateThread(NULL, DEFAULT_STACKSIZE, (void*)thread_listen, pp, 0, NULL))) {
#else
  thread_pipes_t p, *pp = malloc(sizeof(p));
  int pipe_up[2], pipe_dn[2], ok_up, ok_dn;
  ok_up = pipe(pipe_up);
  ok_dn = pipe(pipe_dn);
  if (ok_up || ok_dn) {
    perror("Error in mdsip thread_connect creating pipes\n");
    if (!ok_up) {close(pipe_up[0]);close(pipe_up[1]);}
    if (!ok_dn) {close(pipe_dn[0]);close(pipe_dn[1]);}
    return C_ERROR;
  }
  p.out  = pipe_dn[0];
  p.in   = pipe_up[1];
  pp->out= pipe_up[0];
  pp->in = pipe_dn[1];
  pp->pth = PARENT_THREAD;
  if (pthread_create(&p.pth,NULL,(void*)thread_listen,pp)) {
#endif
    close_pipe(p.in);  close_pipe(p.out);
    close_pipe(pp->in);close_pipe(pp->out);
    return C_ERROR;
  }
  SetConnectionInfoC(c, "thread", 0, &p, sizeof(p));
  return C_OK;
}
