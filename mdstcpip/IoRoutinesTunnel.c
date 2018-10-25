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
 #include <unistd.h>
#endif
#ifdef _WIN32
 #include <process.h>
#else
 #include <sys/wait.h>
#endif

static ssize_t tunnel_send(Connection* c, const void *buffer, size_t buflen, int nowait);
static ssize_t tunnel_recv(Connection* c, void *buffer, size_t len);
static ssize_t tunnel_recv_to(Connection* c, void *buffer, size_t len, int to_msec);
static int tunnel_disconnect(Connection* c);
static int tunnel_connect(Connection* c, char *protocol, char *host);
static int tunnel_listen(int argc, char **argv);
IoRoutines tunnel_routines =
    { tunnel_connect, tunnel_send, tunnel_recv, NULL, tunnel_listen, NULL, NULL, tunnel_disconnect, tunnel_recv_to};

struct TUNNEL_PIPES {
#ifdef _WIN32
  HANDLE stdin_pipe;
  HANDLE stdout_pipe;
  HANDLE hProcess;
#else
  int stdin_pipe;
  int stdout_pipe;
  int pid;
#endif
};

static struct TUNNEL_PIPES *getTunnelPipes(Connection* c){
  size_t len;
  char *info_name;
  struct TUNNEL_PIPES *p = (struct TUNNEL_PIPES *)GetConnectionInfoC(c, &info_name, 0, &len);
  return (info_name && !strcmp("tunnel", info_name) && len == sizeof(struct TUNNEL_PIPES)) ? p : 0;
}

static int tunnel_disconnect(Connection* c){
  struct TUNNEL_PIPES *p = getTunnelPipes(c);
  if (p) {
#ifdef _WIN32
    if (p->hProcess){
      DWORD exitcode;
      if (GetExitCodeProcess(p->hProcess, &exitcode) && exitcode == 9009)
        fprintf(stderr,"Protocol is not supported.\n");
      else
        TerminateProcess(p->hProcess,0);
    }
    CloseHandle(p->stdin_pipe);
    CloseHandle(p->stdout_pipe);
    CloseHandle(p->hProcess);
#else
    kill(p->pid, SIGTERM);
    waitpid(p->pid, NULL, WNOHANG);
    close(p->stdin_pipe);
    close(p->stdout_pipe);
#endif
  }
  return C_OK;
}

static ssize_t tunnel_send(Connection* c, const void *buffer, size_t buflen, int nowait __attribute__ ((unused))){
  struct TUNNEL_PIPES *p = getTunnelPipes(c);
  if (!p) return -1;
#ifdef _WIN32
  ssize_t num = 0;
  return WriteFile(p->stdin_pipe, buffer, buflen, (DWORD *)&num, NULL) ? num : -1;
#else
  return write(p->stdin_pipe, buffer, buflen);
#endif
}

static ssize_t tunnel_recv(Connection* c, void *buffer, size_t buflen){
  return tunnel_recv_to(c,buffer,buflen,-1);
}

static ssize_t tunnel_recv_to(Connection* c, void *buffer, size_t buflen, int to_msec){
  struct TUNNEL_PIPES *p = getTunnelPipes(c);
  if (!p) return -1;
#ifdef _WIN32
  DWORD toval;
  if      (to_msec< 0) toval = 0;
  else if (to_msec==0) toval = MAXDWORD;
  else                 toval = to_msec;
  COMMTIMEOUTS timeouts = { 0, 0, toval, 0, 0};
  SetCommTimeouts(p->stdout_pipe, &timeouts);
  ssize_t num = 0;
  return ReadFile(p->stdout_pipe, buffer, buflen, (DWORD *)&num, NULL) ? num : -1;
#else
  if (to_msec>=0) { // don't time out if to_msec < 0
    struct timeval timeout;
    timeout.tv_sec  = to_msec / 1000;
    timeout.tv_usec = to_msec % 1000;
    fd_set set;
    FD_ZERO(&set); /* clear the set */
    FD_SET(p->stdout_pipe, &set); /* add our file descriptor to the set */
    int rv = select(p->stdout_pipe + 1, &set, NULL, NULL, &timeout);
    if (rv<=0) return rv;
  }
  return read(p->stdout_pipe, buffer, buflen);
#endif
}

#ifndef _WIN32
static void ChildSignalHandler(int num __attribute__ ((unused))){
  // Ensure that the handler does not spoil errno.
  int saved_errno = errno;
  pid_t pid;
  int status;
  while ((pid = waitpid( (pid_t)-1, &status, WNOHANG)) > 0) {
    printf("pid: %d, exited: %d, stop signal: %d, term signal: %d, exit status: %d\n",
      pid, WIFEXITED(status), WSTOPSIG(status), WTERMSIG(status), WEXITSTATUS(status));
    void *ctx = (void *)-1;
    int id;
    char *info_name;
    void *info;
    size_t info_len = 0;
    while ((id = NextConnection(&ctx, &info_name, &info, &info_len)) != INVALID_CONNECTION_ID) {
      if (info_name && strcmp(info_name, "tunnel") == 0
	  && ((struct TUNNEL_PIPES *)info)->pid == pid) {
	DisconnectConnection(id);
	break;
      }
    }
  }
  errno = saved_errno;
}
#endif

static int tunnel_connect(Connection* c, char *protocol, char *host){
#ifdef _WIN32
  SECURITY_ATTRIBUTES saAttr;
  size_t len = strlen(protocol) * 2 + strlen(host) + 512;
  char *cmd = (char *)malloc(len);
  BOOL bSuccess = FALSE;
  PROCESS_INFORMATION piProcInfo;
  STARTUPINFO siStartInfo;
  struct TUNNEL_PIPES p;
  HANDLE g_hChildStd_IN_Rd = NULL;
  HANDLE g_hChildStd_IN_Wr = NULL;
  HANDLE g_hChildStd_IN_Wr_tmp = NULL;
  HANDLE g_hChildStd_OUT_Rd = NULL;
  HANDLE g_hChildStd_OUT_Rd_tmp = NULL;
  HANDLE g_hChildStd_OUT_Wr = NULL;
  _snprintf_s(cmd, len, len - 1, "cmd.exe /Q /C mdsip-client-%s %s mdsip-server-%s", protocol, host, protocol);
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;
  if (!CreatePipe(&g_hChildStd_OUT_Rd_tmp, &g_hChildStd_OUT_Wr, &saAttr, 0))
    fprintf(stderr, "StdoutRd CreatePipe");
  DuplicateHandle(GetCurrentProcess(), g_hChildStd_OUT_Rd_tmp,
		  GetCurrentProcess(), &g_hChildStd_OUT_Rd, 0, FALSE, DUPLICATE_SAME_ACCESS);
  CloseHandle(g_hChildStd_OUT_Rd_tmp);
  if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr_tmp, &saAttr, 0))
    fprintf(stderr, "Stdin CreatePipe");
  DuplicateHandle(GetCurrentProcess(), g_hChildStd_IN_Wr_tmp,
		  GetCurrentProcess(), &g_hChildStd_IN_Wr, 0, FALSE, DUPLICATE_SAME_ACCESS);
  CloseHandle(g_hChildStd_IN_Wr_tmp);
  if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
    fprintf(stderr, "Stdin SetHandleInformation");
  memset(&siStartInfo, 0, sizeof(siStartInfo));
  siStartInfo.cb = sizeof(siStartInfo);
  siStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
  siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
  siStartInfo.hStdInput = g_hChildStd_IN_Rd;
  siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
  bSuccess = CreateProcess(NULL, cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &siStartInfo, &piProcInfo);
  free(cmd);
  if (bSuccess) {
    p.stdin_pipe  = g_hChildStd_IN_Wr;
    p.stdout_pipe = g_hChildStd_OUT_Rd;
    p.hProcess    = piProcInfo.hProcess;
    CloseHandle(g_hChildStd_IN_Rd);
    CloseHandle(g_hChildStd_OUT_Wr);
    CloseHandle(piProcInfo.hThread);
    SetConnectionInfoC(c, "tunnel", 0, &p, sizeof(p));
    return C_OK;
  } else {
    DWORD errstatus = GetLastError();
    fprintf(stderr,"Error in CreateProcees, error: %lu\n",errstatus);
    return C_ERROR;
  }
#else
  pid_t pid;
  int pipe_fd1[2], pipe_fd2[2];
  if (pipe(pipe_fd1) == -1)
    perror("Error in mdsip tunnel_connect creating pipes\n");
  if (pipe(pipe_fd2) == -1)
    perror("Error in mdsip tunnel_connect creating pipes\n");
  pid = fork();
  if (!pid) {
    char *localcmd =
	strcpy((char *)malloc(strlen(protocol) + strlen("mdsip-client-") + 1), "mdsip-client-");
    char *remotecmd =
	strcpy((char *)malloc(strlen(protocol) + strlen("mdsip-server-") + 1), "mdsip-server-");
    char *arglist[] = { localcmd, host, remotecmd, 0 };
    strcat(localcmd, protocol);
    strcat(remotecmd, protocol);
    signal(SIGCHLD, SIG_IGN);
    dup2(pipe_fd2[0], 0);
    close(pipe_fd2[0]);
    dup2(pipe_fd1[1], 1);
    close(pipe_fd1[1]);
    int err = execvp(arglist[0], arglist) ? errno : 0;
    close(pipe_fd2[1]);
    close(pipe_fd1[0]);
    if (err==2) {
      char* c = protocol;
      for (;*c;c++) *c = toupper(*c);
      fprintf(stderr,"Protocol %s is not supported.\n", protocol);
    }
    exit(err);
  } else if (pid == -1) {
    fprintf(stderr, "Error %d from fork()\n", errno);
    close(pipe_fd1[0]);
    close(pipe_fd1[1]);
    close(pipe_fd2[0]);
    close(pipe_fd2[1]);
    return C_ERROR;
  } else {
    struct TUNNEL_PIPES p;
    p.stdin_pipe = pipe_fd2[1];
    p.stdout_pipe = pipe_fd1[0];
    p.pid = pid;
    close(pipe_fd1[1]);
    close(pipe_fd2[0]);
    struct sigaction handler;
    handler.sa_handler = ChildSignalHandler;
    handler.sa_flags = SA_RESTART;
    sigemptyset(&handler.sa_mask);
    sigaddset(&handler.sa_mask, SIGCHLD);
    sigaddset(&handler.sa_mask, SIGPIPE);
    sigaction(SIGCHLD, &handler,NULL);
    sigaction(SIGPIPE, &handler,NULL);
    SetConnectionInfoC(c, "tunnel", p.stdout_pipe, &p, sizeof(p));
  }
  return C_OK;
#endif
}

static int tunnel_listen(int argc __attribute__ ((unused)), char **argv __attribute__ ((unused))){
  int id;
  char *username;
#ifdef _WIN32
  struct TUNNEL_PIPES p;
  p.stdin_pipe  = GetStdHandle(STD_OUTPUT_HANDLE);
  p.stdout_pipe = GetStdHandle(STD_INPUT_HANDLE);
  p.hProcess    = NULL;
#else
  struct TUNNEL_PIPES p = { 1, 0, 0 };
  p.stdin_pipe = dup2(1, 10);
  p.stdout_pipe = dup2(0, 11);
  close(0);
  close(1);
  dup2(2, 1);
#endif
  int status = AcceptConnection(GetProtocol(), "tunnel", 0, &p, sizeof(p), &id, &username);
  if (username)
    free(username);
  if STATUS_OK while (DoMessage(id));
  return C_OK;
}
