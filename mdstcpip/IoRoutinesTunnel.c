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
 #include <unistd.h>
#endif
#ifdef _WIN32
 #include <process.h>
#else
 #include <sys/wait.h>
 #define INVALID_SOCKET -1
#endif
static ssize_t tunnel_send(Connection* c, const void *buffer, size_t buflen, int nowait);
static ssize_t tunnel_recv(Connection* c, void *buffer, size_t len);
static ssize_t tunnel_recv_to(Connection* c, void *buffer, size_t len, int to_msec);
static int tunnel_disconnect(Connection* c);
static int tunnel_connect(Connection* c, char *protocol, char *host);
static int tunnel_listen(int argc, char **argv);
IoRoutines tunnel_routines = {
 tunnel_connect, tunnel_send, tunnel_recv, NULL, tunnel_listen, NULL, NULL, tunnel_disconnect, tunnel_recv_to, NULL
};

typedef struct {
#ifdef _WIN32
  HANDLE out;
  HANDLE in;
  HANDLE hProcess;
#else
  int out;
  int in;
  int pid;
#endif
} tunnel_pipes_t;

static tunnel_pipes_t *getTunnelPipes(Connection* c){
  size_t len;
  char *info_name;
  tunnel_pipes_t *p = (tunnel_pipes_t *)GetConnectionInfoC(c, &info_name, 0, &len);
  return (info_name && !strcmp("tunnel", info_name) && len == sizeof(tunnel_pipes_t)) ? p : 0;
}

static int tunnel_disconnect(Connection* c){
  tunnel_pipes_t *p = getTunnelPipes(c);
  if (p) {
#ifdef _WIN32
    if (p->hProcess){
      DWORD exitcode;
      if (GetExitCodeProcess(p->hProcess, &exitcode) && exitcode == 9009)
        fprintf(stderr,"Protocol is not supported.\n");
      else
        TerminateProcess(p->hProcess,0);
    }
    CloseHandle(p->in);
    CloseHandle(p->out);
    CloseHandle(p->hProcess);
#else
    kill(p->pid, SIGTERM);
    waitpid(p->pid, NULL, WNOHANG);
    close(p->in);
    close(p->out);
#endif
  }
  return C_OK;
}

static ssize_t tunnel_send(Connection* c, const void *buffer, size_t buflen, int nowait __attribute__ ((unused))){
  tunnel_pipes_t *p = getTunnelPipes(c);
  if (!p) return -1;
#ifdef _WIN32
  ssize_t num = 0;
  return WriteFile(p->out, buffer, buflen, (DWORD *)&num, NULL) ? num : -1;
#else
  return write(p->out, buffer, buflen);
#endif
}

static ssize_t tunnel_recv(Connection* c, void *buffer, size_t buflen){
  return tunnel_recv_to(c,buffer,buflen,-1);
}

static ssize_t tunnel_recv_to(Connection* c, void *buffer, size_t buflen, int to_msec){
  tunnel_pipes_t *p = getTunnelPipes(c);
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
  if (to_msec>=0) { // don't time out if to_msec < 0
    struct timeval timeout;
    timeout.tv_sec  = to_msec / 1000;
    timeout.tv_usec =(to_msec % 1000) * 1000;
    fd_set set;
    FD_ZERO(&set); /* clear the set */
    FD_SET(p->in, &set); /* add our file descriptor to the set */
    int rv = select(p->in + 1, &set, NULL, NULL, &timeout);
    if (rv<=0) return rv;
  }
  return read(p->in, buffer, buflen);
#endif
}

#ifndef _WIN32
static void ChildSignalHandler(int num __attribute__ ((unused))){
  // Ensure that the handler does not spoil errno.
  int saved_errno = errno;
  pid_t pid;
  int status;
  while ((pid = waitpid( (pid_t)-1, &status, WNOHANG)) > 0) {
#ifdef DEBUG
    printf("pid: %d, exited: %d, stop signal: %d, term signal: %d, exit status: %d\n",
      pid, WIFEXITED(status), WSTOPSIG(status), WTERMSIG(status), WEXITSTATUS(status));
#endif
    void *ctx = (void *)-1;
    int id;
    char *info_name;
    void *info;
    size_t info_len = 0;
    while ((id = NextConnection(&ctx, &info_name, &info, &info_len)) != INVALID_CONNECTION_ID) {
      if (info_name && strcmp(info_name, "tunnel") == 0
	  && ((tunnel_pipes_t *)info)->pid == pid) {
	DisconnectConnection(id);
	break;
      }
    }
  }
  errno = saved_errno;
}
#endif

static int tunnel_connect(Connection* c, char *protocol, char *host) {
#ifdef _WIN32
  size_t len = strlen(protocol) * 2 + strlen(host) + 512;
  char *cmd = (char *)malloc(len);
  _snprintf_s(cmd, len, len - 1, "cmd.exe /Q /C mdsip-client-%s %s mdsip-server-%s", protocol, host, protocol);
  SECURITY_ATTRIBUTES saAttr;
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;
  struct {HANDLE rd; HANDLE wr;} pipe_p2c = {NULL,NULL}, pipe_c2p = {NULL,NULL};
  HANDLE tmp;
  if (!CreatePipe(&pipe_c2p.rd, &pipe_c2p.wr, &saAttr, 0)) {fprintf(stderr,"pipe_c2p CreatePipe");goto err;}
  if (!CreatePipe(&pipe_p2c.rd, &pipe_p2c.wr, &saAttr, 0)) {fprintf(stderr,"pipe_p2c CreatePipe");goto err;}
  if (DuplicateHandle(GetCurrentProcess(), tmp = pipe_p2c.wr, GetCurrentProcess(), &pipe_p2c.wr, 0, FALSE, DUPLICATE_SAME_ACCESS)) CloseHandle(tmp);
  if (!SetHandleInformation(pipe_p2c.wr, HANDLE_FLAG_INHERIT, 0)) {fprintf(stderr, "pipe_p2c SetHandleInformation");goto err;};
  if (DuplicateHandle(GetCurrentProcess(), tmp = pipe_c2p.rd, GetCurrentProcess(), &pipe_c2p.rd, 0, FALSE, DUPLICATE_SAME_ACCESS)) CloseHandle(tmp);
  STARTUPINFO siStartInfo;
  memset(&siStartInfo, 0, sizeof(siStartInfo));
  siStartInfo.cb = sizeof(siStartInfo);
  siStartInfo.hStdError  = GetStdHandle(STD_ERROR_HANDLE);
  siStartInfo.hStdOutput = pipe_c2p.wr;
  siStartInfo.hStdInput  = pipe_p2c.rd;
  siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
  PROCESS_INFORMATION piProcInfo;
  BOOL bSuccess = CreateProcess(NULL, cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &siStartInfo, &piProcInfo);
  free(cmd);
  if (bSuccess) {//parent
    tunnel_pipes_t p;
    p.out = pipe_p2c.wr;
    p.in  = pipe_c2p.rd;
    p.hProcess = piProcInfo.hProcess;
    CloseHandle(pipe_c2p.wr);
    CloseHandle(pipe_p2c.rd);
    CloseHandle(piProcInfo.hThread);
    SetConnectionInfoC(c, "tunnel", INVALID_SOCKET, &p, sizeof(p));
    return C_OK;
  }
  fprintf(stderr,"CreateProcess");
err: ;
  DWORD errstatus = GetLastError();
  fprintf(stderr," failed, error: %lu\n",errstatus);
  if (pipe_p2c.rd) CloseHandle(pipe_p2c.rd);
  if (pipe_p2c.wr) CloseHandle(pipe_p2c.wr);
  if (pipe_c2p.rd) CloseHandle(pipe_c2p.rd);
  if (pipe_c2p.wr) CloseHandle(pipe_c2p.wr);
  return C_ERROR;
#else
  struct {int rd; int wr;} pipe_p2c, pipe_c2p;
  int err_p2c = pipe((int*)&pipe_p2c);
  int err_c2p = pipe((int*)&pipe_c2p);
  if (err_p2c || err_c2p) {
    perror("Error in mdsip tunnel_connect creating pipes\n");
    if (!err_p2c) {close(pipe_p2c.rd);close(pipe_p2c.wr);}
    if (!err_c2p) {close(pipe_c2p.rd);close(pipe_c2p.wr);}
    return C_ERROR;
  }
  pid_t pid = fork();
  if (pid<0) {// error
    fprintf(stderr, "Error %d from fork()\n", errno);
    close(pipe_c2p.rd);close(pipe_c2p.wr);
    close(pipe_p2c.rd);close(pipe_p2c.wr);
    return C_ERROR;
  }
  if (pid>0) {// parent
    tunnel_pipes_t p;
    p.out = pipe_p2c.wr;
    p.in  = pipe_c2p.rd;
    p.pid = pid;
    close(pipe_c2p.wr);
    close(pipe_p2c.rd);
    struct sigaction handler;
    handler.sa_handler = ChildSignalHandler;
    handler.sa_flags = SA_RESTART;
    sigemptyset(&handler.sa_mask);
    sigaddset(&handler.sa_mask, SIGCHLD);
    sigaddset(&handler.sa_mask, SIGPIPE);
    sigaction(SIGCHLD, &handler,NULL);
    sigaction(SIGPIPE, &handler,NULL);
    SetConnectionInfoC(c, "tunnel", INVALID_SOCKET, &p, sizeof(p));
    return C_OK;
  }
  /*if (pid==0)*/ {//child
    char *localcmd  = strcpy((char *)malloc(strlen(protocol) + 16), "mdsip-client-");strcat(localcmd,  protocol);
    char *remotecmd = strcpy((char *)malloc(strlen(protocol) + 16), "mdsip-server-");strcat(remotecmd, protocol);
    char *arglist[] = { localcmd, host, remotecmd, 0 };
    signal(SIGCHLD, SIG_IGN);
    dup2(pipe_p2c.rd, 0);
    close(pipe_p2c.rd);
    dup2(pipe_c2p.wr, 1);
    close(pipe_c2p.wr);
    int err = execvp(localcmd, arglist) ? errno : 0;
    close(pipe_p2c.wr);
    close(pipe_c2p.rd);
    if (err==2) {
      char* c = protocol;
      for (;*c;c++) *c = toupper(*c);
      fprintf(stderr,"Protocol %s is not supported.\n", protocol);
    } else if ((errno=err)) perror("Client process terminated");
    exit(err);
  }
#endif
}

static int tunnel_listen(int argc __attribute__ ((unused)), char **argv __attribute__ ((unused))){
  int id, status;
  INIT_AND_FREE_ON_EXIT(char*,username);
#ifdef _WIN32
  tunnel_pipes_t p;
  p.in  = GetStdHandle(STD_INPUT_HANDLE);
  p.out = GetStdHandle(STD_OUTPUT_HANDLE);
  p.hProcess = NULL;
#else
  tunnel_pipes_t p = { 0, 1, 0 };
  p.in  = dup(0);
  p.out = dup(1);
  close(1);
  close(0);
  dup2(2, 1);
#endif
  status = AcceptConnection(GetProtocol(), "tunnel", 0, &p, sizeof(p), &id, &username);
  FREE_NOW(username);
  if STATUS_OK while (DoMessage(id));
  return C_OK;
}
