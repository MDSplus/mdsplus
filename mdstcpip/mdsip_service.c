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
#define LOAD_INITIALIZESOCKETS
#include <pthread_port.h>
#include <stdlib.h>
#include <mdsplus/mdsconfig.h>
#include <process.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include "mdsip_connections.h"

static int shut = 0;
static HANDLE shutdownEventHandle;
static int extra_argc;
static char **extra_argv;
// #ifdef _WIN64
// static char *dirname = "bin_x86_64";
// #else
// static char *dirname = "bin_x86";
// #endif

static char *ServiceName(int generic)
{
  char *name = strcpy((char *)malloc(512), "MDSplus ");
  if (!generic)
    strcat(name, GetMulti()? "Action Server - Port " : "Data Server - Port ");
  strcat(name, GetPortname());
  return name;
}

static int SpawnWorker(SOCKET sock)
{
  int status;
  STARTUPINFO startupinfo;
  PROCESS_INFORMATION pinfo;
  char cmd[1024];
  SECURITY_ATTRIBUTES sc_atts;
  sc_atts.nLength = sizeof(sc_atts);
  sc_atts.bInheritHandle = TRUE;
  sc_atts.lpSecurityDescriptor = NULL;
  // sprintf(cmd,
  //	  "%s\\%s\\mdsip.exe --port=%s --hostfile=\"%s\" --compression=%d --sockethandle=%d:%d",
  //	  getenv("MDSPLUS_DIR"), dirname, GetPortname(), GetHostfile(), GetMaxCompressionLevel(),
  //	  _getpid(), sock);
  sprintf(cmd,
	  "mdsip.exe --port=%s --hostfile=\"%s\" --compression=%d --sockethandle=%d:%u",
	  GetPortname(),
	  GetHostfile(),
	  GetMaxCompressionLevel(),
	  _getpid(),
	  (unsigned int)sock);
  memset(&startupinfo, 0, sizeof(startupinfo));
  startupinfo.cb = sizeof(startupinfo);
  status = CreateProcess(NULL, TEXT(cmd), NULL, NULL, FALSE, 0, NULL, NULL, &startupinfo, &pinfo);
  printf("CreateProcess returned %d with cmd=%s\n", status, cmd);
  fflush(stdout);
  CloseHandle(pinfo.hProcess);
  CloseHandle(pinfo.hThread);
  return pinfo.dwProcessId;
}

static SERVICE_STATUS_HANDLE hService;
static int ServiceMain(int, char **);
static SERVICE_STATUS serviceStatus;

static void SetThisServiceStatus(int state, int hint)
{
  serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
  serviceStatus.dwCurrentState = state;
  serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
  serviceStatus.dwWin32ExitCode = NO_ERROR;
  serviceStatus.dwServiceSpecificExitCode = 0;
  serviceStatus.dwCheckPoint = 0;
  serviceStatus.dwWaitHint = hint;
  SetServiceStatus(hService, &serviceStatus);
}

VOID WINAPI serviceHandler(DWORD fdwControl)
{
  switch (fdwControl) {
  case SERVICE_CONTROL_STOP:
    {
      SetThisServiceStatus(SERVICE_STOP_PENDING, 1000);
      SetThisServiceStatus(SERVICE_STOPPED, 0);
      shut = 1;
      PulseEvent(shutdownEventHandle);
      break;
    }
  }
}

static void InitializeService()
{
  char name[120];
  hService = RegisterServiceCtrlHandler(TEXT(ServiceName(1)), (LPHANDLER_FUNCTION) serviceHandler);
  sprintf(name, "MDSIP_%s_SHUTDOWN", GetPortname());
  shutdownEventHandle = CreateEvent(NULL, FALSE, FALSE, TEXT(name));
  SetThisServiceStatus(SERVICE_START_PENDING, 1000);
}

static BOOL RemoveService()
{
  BOOL status = 1;
  SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
  if (hSCManager) {
    SC_HANDLE hService = OpenService(hSCManager, TEXT(ServiceName(1)), DELETE);
    if (hService) {
      status = DeleteService(hService);
      status = CloseServiceHandle(hService);
    }
    CloseServiceHandle(hSCManager);
  }
  return status;
}

static int InstallService()
{
  int status = 1;
  SC_HANDLE hSCManager;
  RemoveService();
  hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
  if (hSCManager) {
    SC_HANDLE hService;
    char *cmd;
    static const char *multi_opt = "--multi";
    static const char *server_opt = "--server";
    static const char *data_opt = "";
    const char *opts = GetMulti()? (GetContextSwitching()? "--multi" : "--server") : "";
    SERVICE_DESCRIPTION sd;
    LPTSTR description = (LPTSTR) malloc(4096);
    if (GetMulti()) {
      if (GetContextSwitching()) {
	opts = multi_opt;
	wsprintf(description,
		 TEXT
		 ("MDSplus data service listening on port %s.\nPermits multiple connections each with own tdi and tree context\n"),
		 GetPortname());
      } else {
	opts = server_opt;
	wsprintf(description,
		 TEXT
		 ("MDSplus data service listening on port %s.\nPermits multiple connections with shared tdi and tree context\n"),
		 GetPortname());
      }
    } else {
      opts = data_opt;
      wsprintf(description,
	       TEXT
	       ("MDSplus data service listening on port %s.\nEach connections will spawn a private server.\n"),
	       GetPortname());
    }
    sd.lpDescription = description;
    cmd = (char *)malloc(strlen(GetPortname()) + strlen(GetHostfile()) + 500);
//    sprintf(cmd, "%%MDSPLUS_DIR%%\\%s\\mdsip_service.exe --port=%s --hostfile=\"%s\" %s", dirname,
//	    GetPortname(), GetHostfile(), opts);
    sprintf(cmd, "mdsip_service.exe --port=%s --hostfile=\"%s\" %s",
          GetPortname(), GetHostfile(), opts);
    hService =
	CreateService(hSCManager, ServiceName(1), ServiceName(0), SERVICE_ALL_ACCESS,
		      SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, cmd,
		      NULL, NULL, NULL, NULL, NULL);
    if (hService == NULL)
      status = GetLastError();
    else {
      ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &sd);
      if (GetMulti()) {
	SERVICE_FAILURE_ACTIONS sfa;
	SC_ACTION actions[] = { {SC_ACTION_RESTART, 5000}
	};
	sfa.dwResetPeriod = INFINITE;
	sfa.lpRebootMsg = NULL;
	sfa.lpCommand = NULL;
	sfa.cActions = 1;
	sfa.lpsaActions = actions;
	status = ChangeServiceConfig2(hService, SERVICE_CONFIG_FAILURE_ACTIONS, &sfa);
	status = GetLastError();
      }
    }
    free(description);
    free(cmd);
    if (hService)
      CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
  }
  return status;
}

static short GetPort()
{
  short port;
  char *name = GetPortname();
  struct servent *sp;
  port = htons((short)atoi(name));
  if (port == 0) {
    sp = getservbyname(name, "tcp");
    if (sp == NULL) {
      fprintf(stderr, "unknown service: %s/tcp\n\n", name);
      exit(0);
    }
    port = sp->s_port;
  }
  return port;
}

static void RedirectOutput()
{
  char *logdir = GetLogDir();
  char *portname = GetPortname();
  char *file = malloc(strlen(logdir)+strlen(portname)+20);
  sprintf(file, "%s\\MDSIP_%s.log", logdir,GetPortname());
  freopen(file, "w", stdout);
  freopen(file, "a", stderr);
  free(logdir);
  free(file);
}

static int ServiceMain(int argc, char **argv)
{
  SOCKET s;
  int status;
  static struct sockaddr_in sin;
  int tablesize = FD_SETSIZE;
  extern fd_set FdActive();
  struct timeval timeout = { 1, 0 };
  int error_count = 0;
  fd_set readfds;
  fd_set fdactive;
  RedirectOutput();
  InitializeService();
  SetThisServiceStatus(SERVICE_RUNNING, 0);
  if (GetMulti()) {
    IoRoutines *io;
    io = LoadIo(GetProtocol());
    if (io && io->listen)
      io->listen(argc, argv);
    else {
      fprintf(stderr, "Protocol %s does not support servers\n", GetProtocol());
      return 1;
    }
    return 0;
  } else {
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
      printf("Error getting Connection Socket\n");
      exit(1);
    }
    memset(&sin, 0, sizeof(sin));
    sin.sin_port = GetPort();
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    status = bind(s, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
    if (status < 0) {
      perror("Error binding to service\n");
      exit(1);
    }
    status = listen(s, 128);
    if (status < 0) {
      perror("Error listen on port\n");
      exit(1);
    }
    FD_ZERO(&fdactive);
    FD_SET(s, &fdactive);
    for (readfds = fdactive; !shut; readfds = fdactive) {
      if (select(tablesize, &readfds, 0, 0, &timeout) != SOCKET_ERROR) {
	error_count = 0;
	if (FD_ISSET(s, &readfds)) {
	  int len = sizeof(struct sockaddr_in);
	  SOCKET sock = accept(s, (struct sockaddr *)&sin, &len);
	  SpawnWorker(sock);
	}
      } else {
	error_count++;
	perror("error in main select");
	fprintf(stderr, "Error count=%d\n", error_count);
	fflush(stderr);
	if (error_count > 100) {
	  fprintf(stderr, "Error count exceeded, shutting down\n");
	  shut = 1;
	}
      }
    }
    shutdown(s, 2);
    return 1;
  }
}

int main(int argc, char **argv)
{
  int x_argc;
  char **x_argv;
  Options options[] = { {"i", "install", 0, 0, 0},
  {"r", "remove", 0, 0, 0},
  {"p", "port", 1, 0, 0},
  {0, 0, 0, 0, 0}
  };
  ParseStdArgs(argc, argv, &x_argc, &x_argv);
  ParseCommand(x_argc, x_argv, options, 1, &extra_argc, &extra_argv);
  if (options[2].present && options[2].value)
    SetPortname(options[2].value);
  else if (GetPortname() == 0)
    SetPortname("mdsip");
  if (options[0].present) {
    InstallService();
    exit(0);
  } else if (options[1].present) {
    RemoveService();
    exit(0);
  } else {
    SERVICE_TABLE_ENTRY srvcTable[] = { {ServiceName(1), (LPSERVICE_MAIN_FUNCTION) ServiceMain}
    , {NULL, NULL}
    };
    INITIALIZESOCKETS;
    StartServiceCtrlDispatcher(srvcTable);
  }
  return 1;
}
