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
#define PROTOCOL "thread"
#define PROT_THREAD
#include "ioroutines_pipes.h"

#ifdef _WIN32
#define THREAD_PID GetCurrentThread()
#endif
#define PARENT_THREAD ((pthread_t)-1)

static int io_disconnect(Connection *c)
{
  io_pipes_t *p = get_pipes(c);
  if (p && p->pth != PARENT_THREAD)
  {
#ifdef _WIN32
    close_pipe(p->in);
    close_pipe(p->out);
    if (WaitForSingleObject(p->pid, 100) == WAIT_TIMEOUT)
      TerminateThread(p->pid, 0);
    CloseHandle(p->pid);
#else
    pthread_cancel(p->pth);
    pthread_join(p->pth, NULL);
    close_pipe(p->in);
    close_pipe(p->out);
#endif
  }
  return C_OK;
}

static void io_listen(void *pp)
{
  int id;
  int status = AcceptConnection(
      PROTOCOL, PROTOCOL, 0, pp, sizeof(io_pipes_t), &id, NULL);
  free(pp);
  if (STATUS_OK)
  {
    Connection *connection = PopConnection(id);
    pthread_cleanup_push((void *)destroyConnection, (void *)connection);
    do
      status = ConnectionDoMessage(connection);
    while (STATUS_OK);
    pthread_cleanup_pop(1);
  }
}

inline static int io_connect(Connection *c,
                             char *protocol __attribute__((unused)),
                             char *host __attribute__((unused)))
{
  io_pipes_t p, *pp = calloc(1, sizeof(p));
  memset(&p, 0, sizeof(p));
#ifdef _WIN32
  pp->pth = PARENT_THREAD;
  SECURITY_ATTRIBUTES saAttr;
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;
  if (!CreatePipe(&p.in, &pp->out, &saAttr, 0))
    fprintf(stderr, "StdoutRd CreatePipe");
  if (!CreatePipe(&pp->in, &p.out, &saAttr, 0))
    fprintf(stderr, "Stdin CreatePipe");
  if (!(p.pid = CreateThread(NULL, DEFAULT_STACKSIZE, (void *)io_listen, pp, 0,
                             NULL)))
  {
#else
  int pipe_up[2], pipe_dn[2], ok_up, ok_dn;
  ok_up = pipe(pipe_up);
  ok_dn = pipe(pipe_dn);
  if (ok_up || ok_dn)
  {
    perror("Error in mdsip io_connect creating pipes\n");
    if (!ok_up)
    {
      close(pipe_up[0]);
      close(pipe_up[1]);
    }
    if (!ok_dn)
    {
      close(pipe_dn[0]);
      close(pipe_dn[1]);
    }
    return C_ERROR;
  }
  p.in = pipe_dn[0];
  p.out = pipe_up[1];
  pp->in = pipe_up[0];
  pp->out = pipe_dn[1];
  pp->pth = PARENT_THREAD;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, 0x40000);
  const int err = pthread_create(&p.pth, &attr, (void *)io_listen, (void *)pp);
  pthread_attr_destroy(&attr);
  if (err)
  {
#endif
    close_pipe(p.in);
    close_pipe(p.out);
    close_pipe(pp->out);
    close_pipe(pp->in);
    free(pp);
    return C_ERROR;
  }
  ConnectionSetInfo(c, PROTOCOL, 0, &p, sizeof(p));
  return C_OK;
}

const IoRoutines thread_routines = {io_connect, io_send, io_recv, NULL,
                                    NULL, NULL, NULL, io_disconnect,
                                    io_recv_to, NULL};
