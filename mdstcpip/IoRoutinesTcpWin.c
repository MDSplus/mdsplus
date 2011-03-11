#include <config.h>
#ifndef SERVICE_START_PENDING
#define SERVICE_START_PENDING 0
#define SERVICE_RUNNING 1
#endif

#define IS_SERVICE 0x200

static char *ServiceName(char *portname) {
	static char name[512] = {0};
	if (name[0] == 0)
	{
	  strcpy(name,"MDSIP_");
	  strcat(name,portname);
	}
	return name;
}

static void RedirectOutput(int pid, char *portname) {
	char file[120];
	if (pid)
	  sprintf(file,"C:\\MDSIP_%s[%d].log",portname,pid);
	else
	  sprintf(file,"C:\\MDSIP_%s.log",portname);
	freopen(file,"a",stdout);
	freopen(file,"a",stderr);
}

static void StartWorker(char **argv) {
  HANDLE sharedMemHandle;
  SOCKET sock;
  BOOL dup = DuplicateHandle(OpenProcess(PROCESS_ALL_ACCESS,TRUE,atoi(argv[6])), 
	     (HANDLE)atoi(argv[7]),GetCurrentProcess(),(HANDLE *)&sock,
	     PROCESS_ALL_ACCESS, TRUE,DUPLICATE_CLOSE_SOURCE|DUPLICATE_SAME_ACCESS);
  ClientList = malloc(sizeof(Client));
  SetFD(sock);
  memset(ClientList,0,sizeof(*ClientList));
  ClientList->sock = sock;
  IsWorker = 1;
  sharedMemHandle = OpenFileMapping(FILE_MAP_READ, FALSE, ServiceName());
  workerShutdown = (int *)MapViewOfFile(sharedMemHandle, FILE_MAP_READ, 0, 0, sizeof(int));
  RedirectOutput(GetCurrentProcessId());
}

static void InitWorkerCommunications() {
  HANDLE sharedMemHandle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(int), ServiceName());
  workerShutdown = (int *)MapViewOfFile(sharedMemHandle, FILE_MAP_WRITE, 0, 0, sizeof(int));
  *workerShutdown = 0;
}

static int SpawnWorker(SOCKET sock) {
  BOOL status;
  static STARTUPINFO startupinfo;
  PROCESS_INFORMATION pinfo;
  char cmd[1024];
  sprintf(cmd,"mdsip_service -p %s -c %d -W %d %d",Portname,MaxCompressionLevel,GetCurrentProcessId(),sock);
  startupinfo.cb = sizeof(startupinfo);
  status = CreateProcess(_pgmptr,cmd,NULL,NULL,FALSE,0,NULL,NULL,&startupinfo, &pinfo);
  CloseHandle(pinfo.hProcess);
  CloseHandle(pinfo.hThread);
  return pinfo.dwProcessId;
}

static SERVICE_STATUS_HANDLE hService;
int ServiceMain(int,char**);
static SERVICE_STATUS serviceStatus;

static void SetThisServiceStatus(int state,int hint) {
  serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
  serviceStatus.dwCurrentState = state;
  serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
  serviceStatus.dwWin32ExitCode = NO_ERROR;
  serviceStatus.dwServiceSpecificExitCode = 0;
  serviceStatus.dwCheckPoint = 0;
  serviceStatus.dwWaitHint = hint;
  SetServiceStatus(hService,&serviceStatus);
}

VOID WINAPI serviceHandler(DWORD fdwControl) {
  switch (fdwControl){
  case SERVICE_CONTROL_STOP: {
    SetThisServiceStatus(SERVICE_STOP_PENDING,1000);
    *workerShutdown = 1;
    SetThisServiceStatus(SERVICE_STOPPED,0);
    shut = 1;
    break;
  }
  }
}


static void InitializeService() {
  hService = RegisterServiceCtrlHandler(ServiceName(),(LPHANDLER_FUNCTION) serviceHandler);
  SetThisServiceStatus(SERVICE_START_PENDING,1000);
  RedirectOutput(0);
}

int mdsipStartService()  {
  SERVICE_TABLE_ENTRY srvcTable[] = {{ServiceName(portname),(LPSERVICE_MAIN_FUNCTION)ServiceMain},{NULL,NULL}};
  StartServiceCtrlDispatcher(srvcTable);
  exit(0);
}

static void RemoveService(char *portname) {
  SC_HANDLE hSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE); 
  if (hSCManager) {
    SC_HANDLE hService = OpenService(hSCManager, ServiceName(portname), DELETE);
    if (hService) {
      BOOL status;
      status = DeleteService(hService);
      status = CloseServiceHandle(hService);
    }
    CloseServiceHandle(hSCManager);
  }
}

static void InstallService(char *serverMode, char *portname, char *hostfile) {
  SC_HANDLE hSCManager;
  int status;
  RemoveService();
  hSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE);
  if (hSCManager) {
    SC_HANDLE hService;
    char *file = (char *)malloc(strlen(_pgmptr)+strlen(portname)+strlen(hostfile)+100);
    sprintf(file,"%s -p %s %s -h \"%s\" -F %d",_pgmptr,portname,
          serverMode,hostfile, IS_SERVICE);
    hService = CreateService(hSCManager, ServiceName(portname), ServiceName(portname), 0, SERVICE_WIN32_OWN_PROCESS,
            SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, file, NULL, NULL, NULL, NULL, NULL);
    if (hService == NULL)
      status = GetLastError();
    free(file);
    if (hService)
      CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
  }
}

static int BecomeUser(char *remuser, struct descriptor *user) {
	return 1;
}

static void InitializeSockets() {
  WSADATA wsaData;
  WORD wVersionRequested;
  wVersionRequested = MAKEWORD(1,1);
  WSAStartup(wVersionRequested,&wsaData);
}

int CheckModeForServiceOps(char mode, char *portname, char *hostfile) {
  if (mode=='i') {
    InstallService("", portname, hostfile);
    exit(0);
  } else if (mode='M') {
    InstallService("-m", portname, hostfile);
    exit(0);
  } else if (mode='S') {
    InstallService("-s", portname, hostfile);
    exit(0);
  } else if (mode='r') {
    RemoveService(portname);
    exit(0);
  } else if (mode='w') {
    StartWorker();
  }
}
