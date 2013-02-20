#include "mdsip_connections.h"
#include <STATICdef.h>
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#ifdef HAVE_WINDOWS_H
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
static IoRoutines tunnel_routines = {tunnel_connect,tunnel_send,tunnel_recv,0,tunnel_listen,0,0,tunnel_disconnect};

EXPORT IoRoutines *Io() {return &tunnel_routines;}

#ifdef HAVE_WINDOWS_H
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

static struct TUNNEL_PIPES *getTunnelPipes(id) {
  size_t len;
  char *info_name;
  struct TUNNEL_PIPES *p = (struct TUNNEL_PIPES *)GetConnectionInfo(id,&info_name,0,&len);
  return (info_name && strcmp("tunnel",info_name)==0 && len == sizeof(struct TUNNEL_PIPES)) ? p : 0;
}

#ifdef HAVE_WINDOWS_H
static int tunnel_disconnect(int id) {
  struct TUNNEL_PIPES *p = getTunnelPipes(id);
  if (p) {
    CloseHandle(p->stdin_pipe);
    CloseHandle(p->stdout_pipe);
  }
  return 0;
}
#else
static int tunnel_disconnect(int id) {
  struct TUNNEL_PIPES *p = getTunnelPipes(id);
  if (p) {
    int status;
    kill(p->pid,6);
    waitpid(p->pid,&status,0);
    close(p->stdin_pipe);
    close(p->stdout_pipe);
  }
  return 0;
}
#endif

#ifdef HAVE_WINDOWS_H
static ssize_t tunnel_send(int id, const void *buffer, size_t buflen, int nowait) {
  struct TUNNEL_PIPES *p = getTunnelPipes(id);
  ssize_t num;
  return p && WriteFile(p->stdin_pipe,buffer,buflen,&num,NULL) ? num : -1;
}
#else
static ssize_t tunnel_send(int id, const void *buffer, size_t buflen, int nowait) {
  struct TUNNEL_PIPES *p = getTunnelPipes(id);
  return p ? write(p->stdin_pipe,buffer,buflen) : -1;
}
#endif

#ifdef HAVE_WINDOWS_H
static ssize_t tunnel_recv(int id, void *buffer, size_t buflen) {
  struct TUNNEL_PIPES *p = getTunnelPipes(id);
  ssize_t num;
  return p && ReadFile(p->stdout_pipe,buffer,buflen,&num,NULL) ? num : -1;
}
#else
static ssize_t tunnel_recv(int id, void *buffer, size_t buflen) {
  struct TUNNEL_PIPES *p = getTunnelPipes(id);
  return p ?  read(p->stdout_pipe,buffer,buflen) : -1;
}
#endif

#ifndef HAVE_WINDOWS_H
static void ChildSignalHandler(int num) {
  sigset_t set, oldset;
  pid_t pid;
  int status, exitstatus;
  /* block other incoming SIGCHLD signals */
  sigemptyset(&set);
  sigaddset(&set, SIGCHLD);
  sigprocmask(SIG_BLOCK, &set, &oldset);
  /* wait for child */
  while((pid = waitpid((pid_t)-1, &status, WNOHANG)) > 0) {
    void *ctx=0;
    int id;
    char *info_name;
    void *info;
    size_t info_len;
    while ((id=NextConnection(&ctx,&info_name,&info,&info_len)) != -1) {
      if (info_name && strcmp(info_name,"tunnel")==0 && ((struct TUNNEL_PIPES *)info)->pid == pid) {
	DisconnectConnection(id);
	break;
      }
    }
    /* re-install the signal handler (some systems need this) */
    signal(SIGCHLD,ChildSignalHandler);
    /* and unblock it */
    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);
    sigprocmask(SIG_UNBLOCK, &set, &oldset);
  }
}
#endif

#ifdef HAVE_WINDOWS_H
static int tunnel_connect(int id, char *protocol, char *host) {
  SECURITY_ATTRIBUTES saAttr; 
  size_t len=strlen(protocol)*2+strlen(host)+128;
  char *cmd=(char *)malloc(len);
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
  HANDLE g_hInputFile = NULL;
  int nSize;
  int status;
  WCHAR *lcmd;
  _snprintf_s(cmd,len,len-1,"cmd /Q /C mdsip-client-%s %s mdsip-server-%s",protocol,host,protocol); 
  nSize = MultiByteToWideChar(CP_UTF8, 0, cmd, -1, NULL, 0);
  lcmd=malloc(nSize+1);
  nSize = MultiByteToWideChar(CP_UTF8, 0, cmd, -1, lcmd, nSize);
  free(cmd);
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
  saAttr.bInheritHandle = TRUE; 
  saAttr.lpSecurityDescriptor = NULL; 
  if ( ! CreatePipe(&g_hChildStd_OUT_Rd_tmp, &g_hChildStd_OUT_Wr, &saAttr, 0) ) 
    fprintf(stderr,"StdoutRd CreatePipe"); 
  DuplicateHandle(GetCurrentProcess(),g_hChildStd_OUT_Rd_tmp,
  GetCurrentProcess(),&g_hChildStd_OUT_Rd,0,FALSE,DUPLICATE_SAME_ACCESS);
  CloseHandle(g_hChildStd_OUT_Rd_tmp);
  if (! CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr_tmp, &saAttr, 0)) 
	fprintf(stderr,"Stdin CreatePipe"); 
  DuplicateHandle(GetCurrentProcess(),g_hChildStd_IN_Wr_tmp,
	 GetCurrentProcess(),&g_hChildStd_IN_Wr,0,FALSE,DUPLICATE_SAME_ACCESS);
  CloseHandle(g_hChildStd_IN_Wr_tmp);
  if ( ! SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0) )
    fprintf(stderr,"Stdin SetHandleInformation"); 
  memset(&siStartInfo,0,sizeof(siStartInfo));
  siStartInfo.cb=sizeof(siStartInfo);
  siStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
  siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
  siStartInfo.hStdInput = g_hChildStd_IN_Rd;
  siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
  bSuccess = CreateProcess(NULL,lcmd,NULL,NULL,TRUE,CREATE_NEW_CONSOLE,NULL,NULL,&siStartInfo,&piProcInfo); 
  if (bSuccess) {
	WaitForInputIdle(piProcInfo.hProcess,10000);
	p.stdin_pipe=g_hChildStd_IN_Wr;
	p.stdout_pipe=g_hChildStd_OUT_Rd;
	CloseHandle(g_hChildStd_IN_Rd);
	CloseHandle(g_hChildStd_OUT_Wr);
    CloseHandle(piProcInfo.hProcess);
	CloseHandle(piProcInfo.hThread);
	SetConnectionInfo(id,"tunnel",0,&p,sizeof(p));
	status=1;
  } else {
	status=0;
  }
  free(lcmd);
  return status;
}
#else
static int tunnel_connect(int id, char *protocol, char *host) {
  pid_t  pid,xpid;
  int sts=0;
  int pipe_fd1[2],pipe_fd2[2];
  pipe(pipe_fd1);
  pipe(pipe_fd2);
  pid = fork();
  if (!pid) {
    char *localcmd=strcpy((char *)malloc(strlen(protocol)+strlen("mdsip-client-")+1),"mdsip-client-");
    char *remotecmd=strcpy((char *)malloc(strlen(protocol)+strlen("mdsip-server-")+1),"mdsip-server-");
    char *arglist[]={localcmd,host,remotecmd,0};
    char  *p;
    int i=0;
    strcat(localcmd,protocol);
    strcat(remotecmd,protocol);
    signal(SIGCHLD,SIG_IGN);
    dup2(pipe_fd2[0],0);
    close(pipe_fd2[0]);
    dup2(pipe_fd1[1],1);
    close(pipe_fd1[1]);
    sts = execvp(arglist[0],arglist);
    exit(1);
  } else if (pid == -1) {
    fprintf(stderr,"Error %d from fork()\n",errno);
    close(pipe_fd1[0]);
    close(pipe_fd1[2]);
    close(pipe_fd2[0]);
    close(pipe_fd2[1]);
    return(-1);
  } else {
    struct TUNNEL_PIPES p;
    p.stdin_pipe=pipe_fd2[1];
    p.stdout_pipe=pipe_fd1[0];
    p.pid=pid;
    close(pipe_fd1[1]);
    close(pipe_fd2[0]);
    signal(SIGCHLD,ChildSignalHandler);
    SetConnectionInfo(id,"tunnel",p.stdout_pipe,&p,sizeof(p));
  }
  return 0;
}
#endif

#ifdef HAVE_WINDOWS_H
static int tunnel_listen(int argc, char **argv) {
  return 0;
}
#else
static int tunnel_listen(int argc, char **argv) {
  struct TUNNEL_PIPES p = {1,0,0};
  int id;
  char *username;
  int status;
  p.stdin_pipe=dup2(1,10);
  p.stdout_pipe=dup2(0,11);
  close(0);
  close(1);
  dup2(2,1);
  status=AcceptConnection(GetProtocol(),"tunnel",0,&p,sizeof(p),&id,&username);
  if (username) free(username);
  while (DoMessage(id) != 0);
  return 1;
}
#endif

