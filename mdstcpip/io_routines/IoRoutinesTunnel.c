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

#define PROTOCOL "tunnel"
#define PROT_TUNNEL
#include "ioroutines_pipes.h"

// #define DEBUG
#include <mdsmsg.h>

static int io_disconnect(Connection *c)
{
  io_pipes_t *p = get_pipes(c);
  if (p)
  {
#ifdef WIN32
    if (p->pid)
    {
      DWORD exitcode;
      if (GetExitCodeProcess(p->pid, &exitcode) && exitcode == 9009)
        fprintf(stderr, "Protocol is not supported.\n");
      else
        TerminateProcess(p->pid, 0);
    }
    CloseHandle(p->in);
    CloseHandle(p->out);
    CloseHandle(p->pid);
#else
    close(p->in);
    close(p->out);
    int p_pid = p->pid;
    int pid = fork();
    if (pid == 0)
    {
      int i;
      for (i = 30; !waitpid(p_pid, NULL, WNOHANG) && i-- > 0;)
        usleep(100000);
      if (!i)
        kill(p_pid, SIGTERM);
      /* Do not call any functions registered with atexit or onexit */
      _Exit(0);
    }
    if (pid < 0)
      kill(p->pid, SIGTERM);
#endif
  }
  return C_OK;
}

#ifndef WIN32
static void ChildSignalHandler(int num __attribute__((unused)))
{
  // Ensure that the handler does not spoil errno.
  int saved_errno = errno;
  pid_t pid;
  int status;
  while ((pid = waitpid((pid_t)-1, &status, WNOHANG)) > 0)
  {
#ifdef DEBUG
    printf("pid: %d, exited: %d, stop signal: %d, term signal: %d, exit "
           "status: %d\n",
           pid, WIFEXITED(status), WSTOPSIG(status), WTERMSIG(status),
           WEXITSTATUS(status));
#endif
    void *ctx = (void *)-1;
    int id;
    char *info_name;
    void *info;
    size_t info_len = 0;
    while ((id = NextConnection(&ctx, &info_name, &info, &info_len)) !=
           INVALID_CONNECTION_ID)
    {
      if (info_name && strcmp(info_name, PROTOCOL) == 0 &&
          ((io_pipes_t *)info)->pid == pid)
      {
        CloseConnection(id);
        break;
      }
    }
  }
  errno = saved_errno;
}
#endif

static int io_connect(Connection *c, char *protocol, char *host)
{
#ifdef WIN32
  size_t len = strlen(protocol) * 2 + strlen(host) + 512;
  char *cmd = (char *)malloc(len);
  _snprintf_s(cmd, len, len - 1,
              "cmd.exe /Q /C mdsip-client-%s %s mdsip-server-%s", protocol,
              host, protocol);
  SECURITY_ATTRIBUTES saAttr;
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;
  struct
  {
    HANDLE rd;
    HANDLE wr;
  } pipe_p2c = {NULL, NULL}, pipe_c2p = {NULL, NULL};
  HANDLE tmp;
  if (!CreatePipe(&pipe_c2p.rd, &pipe_c2p.wr, &saAttr, 0))
  {
    fprintf(stderr, "pipe_c2p CreatePipe");
    goto err;
  }
  if (!CreatePipe(&pipe_p2c.rd, &pipe_p2c.wr, &saAttr, 0))
  {
    fprintf(stderr, "pipe_p2c CreatePipe");
    goto err;
  }
  if (DuplicateHandle(GetCurrentProcess(), tmp = pipe_p2c.wr,
                      GetCurrentProcess(), &pipe_p2c.wr, 0, FALSE,
                      DUPLICATE_SAME_ACCESS))
    CloseHandle(tmp);
  if (!SetHandleInformation(pipe_p2c.wr, HANDLE_FLAG_INHERIT, 0))
  {
    fprintf(stderr, "pipe_p2c SetHandleInformation");
    goto err;
  };
  if (DuplicateHandle(GetCurrentProcess(), tmp = pipe_c2p.rd,
                      GetCurrentProcess(), &pipe_c2p.rd, 0, FALSE,
                      DUPLICATE_SAME_ACCESS))
    CloseHandle(tmp);
  STARTUPINFO siStartInfo;
  memset(&siStartInfo, 0, sizeof(siStartInfo));
  siStartInfo.cb = sizeof(siStartInfo);
  siStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
  siStartInfo.hStdOutput = pipe_c2p.wr;
  siStartInfo.hStdInput = pipe_p2c.rd;
  siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
  PROCESS_INFORMATION piProcInfo;
  BOOL bSuccess = CreateProcess(NULL, cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW,
                                NULL, NULL, &siStartInfo, &piProcInfo);
  free(cmd);
  if (bSuccess)
  { // parent
    io_pipes_t p;
    p.in = pipe_c2p.rd;
    p.out = pipe_p2c.wr;
    p.pid = piProcInfo.hProcess;
    CloseHandle(pipe_c2p.wr);
    CloseHandle(pipe_p2c.rd);
    CloseHandle(piProcInfo.hThread);
    ConnectionSetInfo(c, PROTOCOL, INVALID_SOCKET, &p, sizeof(p));
    return C_OK;
  }
  fprintf(stderr, "CreateProcess");
err:;
  DWORD errstatus = GetLastError();
  fprintf(stderr, " failed, error: %lu\n", errstatus);
  if (pipe_p2c.rd)
    CloseHandle(pipe_p2c.rd);
  if (pipe_p2c.wr)
    CloseHandle(pipe_p2c.wr);
  if (pipe_c2p.rd)
    CloseHandle(pipe_c2p.rd);
  if (pipe_c2p.wr)
    CloseHandle(pipe_c2p.wr);
  return C_ERROR;
#else
  struct
  {
    int rd;
    int wr;
  } pipe_p2c, pipe_c2p;
  int err_p2c = pipe((int *)&pipe_p2c);
  int err_c2p = pipe((int *)&pipe_c2p);
  if (err_p2c || err_c2p)
  {
    perror("Error in mdsip io_connect creating pipes");
    if (!err_p2c)
    {
      close(pipe_p2c.rd);
      close(pipe_p2c.wr);
    }
    if (!err_c2p)
    {
      close(pipe_c2p.rd);
      close(pipe_c2p.wr);
    }
    return C_ERROR;
  }
  pid_t pid = fork();
  if (pid < 0)
  { // error
    perror("Error from fork()");
    close(pipe_c2p.rd);
    close(pipe_c2p.wr);
    close(pipe_p2c.rd);
    close(pipe_p2c.wr);
    return C_ERROR;
  }
  if (pid > 0)
  { // parent
    io_pipes_t p;
    p.in = pipe_c2p.rd;
    p.out = pipe_p2c.wr;
    p.pid = pid;
    close(pipe_c2p.wr);
    fcntl(pipe_c2p.rd, F_SETFD, FD_CLOEXEC);
    close(pipe_p2c.rd);
    fcntl(pipe_p2c.wr, F_SETFD, FD_CLOEXEC);
    struct sigaction handler;
    handler.sa_handler = ChildSignalHandler;
    handler.sa_flags = SA_RESTART;
    sigemptyset(&handler.sa_mask);
    sigaddset(&handler.sa_mask, SIGCHLD);
    sigaddset(&handler.sa_mask, SIGPIPE);
    sigaction(SIGCHLD, &handler, NULL);
    sigaction(SIGPIPE, &handler, NULL);
    ConnectionSetInfo(c, PROTOCOL, INVALID_SOCKET, &p, sizeof(p));
    return C_OK;
  }
  /*if (pid==0)*/ { // child
    char *localcmd =
        strcpy((char *)malloc(strlen(protocol) + 16), "mdsip-client-");
    strcat(localcmd, protocol);
    char *remotecmd =
        strcpy((char *)malloc(strlen(protocol) + 16), "mdsip-server-");
    strcat(remotecmd, protocol);
    char *arglist[] = {localcmd, host, remotecmd, 0};
    signal(SIGCHLD, SIG_IGN);
    close(0);
    close(1);
    dup2(pipe_p2c.rd, 0);
    dup2(pipe_c2p.wr, 1);
    close(pipe_p2c.wr);
    fcntl(pipe_p2c.rd, F_SETFD, FD_CLOEXEC);
    close(pipe_c2p.rd);
    fcntl(pipe_c2p.wr, F_SETFD, FD_CLOEXEC);
    // sleep(1); // uncomment to simulate slow clients
    MDSDBG("Starting client process for protocol '%s'", protocol);
    int err = execvp(localcmd, arglist) ? errno : 0;
    if (err == 2)
    {
      char *c = protocol;
      for (; *c; c++)
        *c = toupper(*c);
      fprintf(stderr, "Protocol %s is not supported.\n", protocol);
    }
    else if ((errno = err))
      perror("Client process terminated");
    else
    {
      MDSDBG("Client process terminated");
    }
    exit(err);
  }
#endif
}

static int io_listen(int argc __attribute__((unused)),
                     char **argv __attribute__((unused)))
{
  io_pipes_t pipes;
  memset(&pipes, 0, sizeof(pipes));
#ifdef WIN32
  pipes.in = (HANDLE)_get_osfhandle(0);
  pipes.out = (HANDLE)_get_osfhandle(_dup(1));
  close(1);
  _dup2(2, 1);
#else
  pipes.in = 0;       // use stdin directly
  pipes.out = dup(1); // use copy of stdout so we can redirect to stderr
  close(1);
  dup2(2, 1);
  fcntl(pipes.in, F_SETFD, FD_CLOEXEC);
  fcntl(pipes.out, F_SETFD, FD_CLOEXEC);
#endif
  int id;
  int status = AcceptConnection(
      GetProtocol(), PROTOCOL, 0, &pipes, sizeof(io_pipes_t), &id, NULL);
  if (STATUS_OK)
  {
    Connection *connection = PopConnection(id);
    pthread_cleanup_push((void *)destroyConnection, (void *)connection);
    do
      status = ConnectionDoMessage(connection);
    while (STATUS_OK);
    pthread_cleanup_pop(1);
    return C_OK;
  }
  return C_ERROR;
}

const IoRoutines tunnel_routines = {io_connect, io_send, io_recv, NULL,
                                    io_listen, NULL, NULL, io_disconnect,
                                    io_recv_to, NULL};
