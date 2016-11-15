#include "mdsip_connections.h"
#include <STATICdef.h>
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef _WIN32
#include <process.h>
#include <winuser.h>
#else
#include <sys/wait.h>
#endif

static ssize_t tunnel_send(int id, const void *buffer, size_t buflen, int nowait);
static ssize_t tunnel_recv(int id, void *buffer, size_t len);
static int tunnel_disconnect(int id);
static int tunnel_connect(int id, char *protocol, char *host);
static int tunnel_listen(int argc, char **argv);
static IoRoutines tunnel_routines =
    { tunnel_connect, tunnel_send, tunnel_recv, 0, tunnel_listen, 0, 0, tunnel_disconnect };

EXPORT IoRoutines *Io()
{
  return &tunnel_routines;
}

#ifdef _WIN32
struct TUNNEL_PIPES {
  HANDLE stdin_pipe;
  HANDLE stdout_pipe;
};
#else
struct TUNNEL_PIPES {
  int stdin_pipe;
  int stdout_pipe;
  int pid;
};
#endif

static struct TUNNEL_PIPES *getTunnelPipes(int id)
{
  size_t len;
  char *info_name;
  struct TUNNEL_PIPES *p = (struct TUNNEL_PIPES *)GetConnectionInfo(id, &info_name, 0, &len);

  return (info_name && strcmp("tunnel", info_name) == 0
	  && len == sizeof(struct TUNNEL_PIPES)) ? p : 0;
}

#ifdef _WIN32
static int tunnel_disconnect(int id)
{
  struct TUNNEL_PIPES *p = getTunnelPipes(id);
  if (p) {
    CloseHandle(p->stdin_pipe);
    CloseHandle(p->stdout_pipe);
  }
  return 0;
}
#else
static int tunnel_disconnect(int id)
{
  struct TUNNEL_PIPES *p = getTunnelPipes(id);
  if (p) {
    kill(p->pid, SIGTERM);
    waitpid(p->pid, NULL, WNOHANG);
    close(p->stdin_pipe);
    close(p->stdout_pipe);
  }
  return 0;
}
#endif

static ssize_t tunnel_send(int id, const void *buffer, size_t buflen, int nowait __attribute__ ((unused))){
  struct TUNNEL_PIPES *p = getTunnelPipes(id);
#ifdef _WIN32
  ssize_t num = 0;
  return (p && WriteFile(p->stdin_pipe, buffer, buflen, (DWORD *)&num, NULL)) ? num : -1;
#else
  return p ? write(p->stdin_pipe, buffer, buflen) : -1;
#endif
}

static ssize_t tunnel_recv(int id, void *buffer, size_t buflen){
  struct TUNNEL_PIPES *p = getTunnelPipes(id);
#ifdef _WIN32
  ssize_t num = 0;
  return (p && ReadFile(p->stdout_pipe, buffer, buflen, (DWORD *)&num, NULL)) ? num : -1;
#else
  return p ? read(p->stdout_pipe, buffer, buflen) : -1;
#endif
}

#ifndef _WIN32
static void ChildSignalHandler(int num __attribute__ ((unused)))
{
  sigset_t set, oldset;
  pid_t pid;
  int status;
  /* block other incoming SIGCHLD signals */
  sigemptyset(&set);
  sigaddset(&set, SIGCHLD);
  sigprocmask(SIG_BLOCK, &set, &oldset);
  /* wait for child */
  while ((pid = waitpid((pid_t) - 1, &status, WNOHANG)) > 0) {
    void *ctx = (void *)-1;
    int id;
    char *info_name;
    void *info;
    size_t info_len;
    while ((id = NextConnection(&ctx, &info_name, &info, &info_len)) != -1) {
      if (info_name && strcmp(info_name, "tunnel") == 0
	  && ((struct TUNNEL_PIPES *)info)->pid == pid) {
	DisconnectConnection(id);
	break;
      }
    }
    /* re-install the signal handler (some systems need this) */
    signal(SIGCHLD, ChildSignalHandler);
    /* and unblock it */
    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);
    sigprocmask(SIG_UNBLOCK, &set, &oldset);
  }
}
#endif

static int tunnel_connect(int id, char *protocol, char *host){
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
  int status;
  _snprintf_s(cmd, len, len - 1, "cmd.exe /Q /C mdsip-client-%s %s mdsip-server-%s", protocol, host,
	      protocol);
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
  bSuccess =
      CreateProcess(NULL, cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &siStartInfo,
		    &piProcInfo);
  free(cmd);
  if (bSuccess) {
    WaitForInputIdle(piProcInfo.hProcess, 10000);
    p.stdin_pipe = g_hChildStd_IN_Wr;
    p.stdout_pipe = g_hChildStd_OUT_Rd;
    CloseHandle(g_hChildStd_IN_Rd);
    CloseHandle(g_hChildStd_OUT_Wr);
    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);
    SetConnectionInfo(id, "tunnel", 0, &p, sizeof(p));
    status = 1;
  } else {
    DWORD errstatus = GetLastError();
    fprintf(stderr,"Error in CreateProcees, error: %lu\n",errstatus);
    status = 0;
  }
  return status;
#else
  pid_t pid;
  int pipe_fd1[2], pipe_fd2[2];
  pipe(pipe_fd1);
  pipe(pipe_fd2);
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
    execvp(arglist[0], arglist);
    exit(1);
  } else if (pid == -1) {
    fprintf(stderr, "Error %d from fork()\n", errno);
    close(pipe_fd1[0]);
    close(pipe_fd1[1]);
    close(pipe_fd2[0]);
    close(pipe_fd2[1]);
    return (-1);
  } else {
    struct TUNNEL_PIPES p;
    p.stdin_pipe = pipe_fd2[1];
    p.stdout_pipe = pipe_fd1[0];
    p.pid = pid;
    close(pipe_fd1[1]);
    close(pipe_fd2[0]);
    signal(SIGCHLD, ChildSignalHandler);
    SetConnectionInfo(id, "tunnel", p.stdout_pipe, &p, sizeof(p));
  }
  return 0;
#endif
}

static int tunnel_listen(int argc __attribute__ ((unused)), char **argv __attribute__ ((unused))){
#ifdef _WIN32
  return 0;
#else
  struct TUNNEL_PIPES p = { 1, 0, 0 };
  int id;
  char *username;
  p.stdin_pipe = dup2(1, 10);
  p.stdout_pipe = dup2(0, 11);
  close(0);
  close(1);
  dup2(2, 1);
  AcceptConnection(GetProtocol(), "tunnel", 0, &p, sizeof(p), &id, &username);
  if (username)
    free(username);
  while (DoMessage(id) != 0) ;
  return 1;
#endif
}
