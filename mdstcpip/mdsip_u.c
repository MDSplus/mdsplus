#include <config.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <strroutines.h>
#include <libroutines.h>
#include "../tdishr/cvtdef.h"
#undef DTYPE_FLOAT
#undef DTYPE_DOUBLE
#undef DTYPE_EVENT
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <mds_stdarg.h>
#ifdef _THREADS
#include <pthread.h>
#endif
#include "mdsip.h"
extern char *ctime();
#define MAX_ARGS 256
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#if defined(_WIN32)
#include <process.h>
#else
#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif
#include <pwd.h>
#include <unistd.h>  /* for getpid() */
#define closesocket close
#endif
#define __tolower(c) (((c) >= 'A' && (c) <= 'Z') ? (c) | 0x20 : (c))
extern char *MdsDescrToCstring();
extern void MdsFree();

typedef ARRAY_COEFF(char,7) ARRAY_7;

typedef struct _context { void *tree;
                        } Context;

typedef struct _client { SOCKET sock;
                         Context context;
                         unsigned char message_id;
                         int client_type;
                         int nargs;
                         struct descriptor *descrip[MAX_ARGS];
                         MdsEventList *event;
			 void         *tdicontext[6];
                         int addr;
                         struct _client *next;
                       } Client;

static Client *ClientList = 0;
static fd_set fdactive;
static int multi = 0;
static int IsService = 0;
static int IsWorker = 0;
static int NO_SPAWN = 1;
static int ContextSwitching = 0;
static char *PortName;
#if defined(_WIN32)
static char *hostfile = "C:\\mdsip.hosts";
#else
static char *hostfile = "/etc/mdsip.hosts";
#endif
static char *portname = "8000";

static void DefineTdi(char *execute_string, char dtype, short length, void *value);
extern int TdiSaveContext();
extern int TdiRestoreContext();
extern int TdiExecute();
extern int TdiData();
extern int TdiDebug();
extern int TdiResetPublic();
extern int TdiResetPrivate();
extern char *TranslateLogical();

static int ConnectToInet();
static int CreateMdsPort(char *service, int multi);
static void AddClient(int socket, struct sockaddr_in *sin);
static void RemoveClient(Client *c);
static void ProcessMessage(Client *c, Message *message);
static void ExecuteMessage(Client *c);
static void SendResponse(Client *c,int status, struct descriptor *xd);
static int DoMessage(Client *c);
static void ConvertFloat(int nbytes, int in_type, char in_length, char *in_ptr, int out_type, char out_length, char *out_ptr);
extern Message *GetMdsMsg(int sock, int *status);
extern int SendMdsMsg(int sock, Message *m, int oob);
void GetErrorText(int status, struct descriptor_xd *xd);
void ResetErrors();
static void CompressString(struct descriptor *in, int upcase);

static int zero = 0;
static int one = 1;
static unsigned short two = 2;
static SOCKET serverSock = INVALID_SOCKET;
static int shut = 0;
static int *workerShutdown;
#define MakeDesc(name) memcpy(malloc(sizeof(name)),&name,sizeof(name))

#ifdef _NT_SERVICE

static char *ServiceName()
{
	static char name[512] = {0};
	if (name[0] == 0)
	{
	  strcpy(name,"MDSIP_");
	  strcat(name,portname);
	}
	return name;
}

static void StartWorker(char **argv)
{
  HANDLE sharedMemHandle;
  SOCKET sock;
  BOOL dup = DuplicateHandle(OpenProcess(PROCESS_ALL_ACCESS,TRUE,atoi(argv[3])), 
	     (HANDLE)atoi(argv[4]),GetCurrentProcess(),(HANDLE *)&sock,
	     PROCESS_ALL_ACCESS, TRUE,DUPLICATE_CLOSE_SOURCE|DUPLICATE_SAME_ACCESS);
  ClientList = malloc(sizeof(Client));
  FD_SET(sock,&fdactive);
  memset(ClientList,0,sizeof(*ClientList));
  ClientList->sock = sock;
  IsWorker = 1;
  PortName = portname;
  sharedMemHandle = OpenFileMapping(FILE_MAP_READ, FALSE, ServiceName());
  workerShutdown = (int *)MapViewOfFile(sharedMemHandle, FILE_MAP_READ, 0, 0, sizeof(int));
}

static void InitWorkerCommunications()
{
	HANDLE sharedMemHandle = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, sizeof(int), ServiceName());
	workerShutdown = (int *)MapViewOfFile(sharedMemHandle, FILE_MAP_WRITE, 0, 0, sizeof(int));
	*workerShutdown = 0;
}

static int SpawnWorker(SOCKET sock)
{
	BOOL status;
	static STARTUPINFO startupinfo;
	PROCESS_INFORMATION pinfo;
	char cmd[512];
	sprintf(cmd,"%s %s worker %d %d","mdsip_service",portname,GetCurrentProcessId(),sock);
	startupinfo.cb = sizeof(startupinfo);
    status = CreateProcess(_pgmptr,cmd,NULL,NULL,FALSE,0,NULL,NULL,&startupinfo, &pinfo);
	CloseHandle(pinfo.hProcess);
	CloseHandle(pinfo.hThread);
	return pinfo.dwProcessId;
}

static unsigned long hService;
int ServiceMain(int,char**);
static SERVICE_STATUS serviceStatus;

static void SetThisServiceStatus(int state,int hint)
{
  serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
  serviceStatus.dwCurrentState = state;
  serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
  serviceStatus.dwWin32ExitCode = NO_ERROR;
  serviceStatus.dwServiceSpecificExitCode = 0;
  serviceStatus.dwCheckPoint = 0;
  serviceStatus.dwWaitHint = hint;
  SetServiceStatus(hService,&serviceStatus);
}

VOID WINAPI serviceHandler(DWORD fdwControl)
{
	switch (fdwControl)
	{
	case SERVICE_CONTROL_STOP:
		{
		SetThisServiceStatus(SERVICE_STOP_PENDING,1000);
		*workerShutdown = 1;
		SetThisServiceStatus(SERVICE_STOPPED,0);
		shut = 1;
        break;
		}
	}

}

static void InitializeService()
{
	char file[120];
        hService = RegisterServiceCtrlHandler(ServiceName(),(LPHANDLER_FUNCTION) serviceHandler);
        SetThisServiceStatus(SERVICE_START_PENDING,1000);
	strcpy(file,"C:\\");
	strcat(file,ServiceName());
	strcat(file,".log");
	freopen(file, "a", stdout );
}

int main( int argc, char **argv) 
{
  int service_per_user = strcmp(argv[2],"service") == 0;
  int service_multi = strcmp(argv[2],"service_multi") == 0;
  int service_server = strcmp(argv[2],"service_server") == 0;
  if (argc >= 3 && (service_per_user || service_multi || service_server))
  {
	portname = argv[1];
	if (argc > 3)
		hostfile = argv[3];
	multi = NO_SPAWN = !service_per_user;
	IsService = 1;
	{
      SERVICE_TABLE_ENTRY srvcTable[] = {{ServiceName(),(LPSERVICE_MAIN_FUNCTION)ServiceMain},{NULL,NULL}};
      StartServiceCtrlDispatcher(srvcTable);
	}
  }
  else
  {
	  ServiceMain(argc,argv);
  }
  return 1;
}
#define main ServiceMain

static void RemoveService()
{
  SC_HANDLE hSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE); 
  if (hSCManager)
  {
	  SC_HANDLE hService = OpenService(hSCManager, ServiceName(), DELETE);
      if (hService)
	  {
	    BOOL status;
	    status = DeleteService(hService);
	    status = CloseServiceHandle(hService);
	  }
	  CloseServiceHandle(hSCManager);
  }
}

static void InstallService(int typesrv)
{
  SC_HANDLE hSCManager;
  int status;
  RemoveService();
  hSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE);
  if (hSCManager)
  {
	  SC_HANDLE hService;
	  char *file = (char *)malloc(strlen(_pgmptr)+strlen(portname)+strlen(hostfile)+20);
	  strcpy(file,_pgmptr);
	  strcat(file," ");
	  strcat(file,portname);
	  switch (typesrv)
	  {
	  case 0: strcat(file," service "); break;
	  case 1: strcat(file," service_multi "); break;
	  case 2: strcat(file," service_server "); break;
	  }
	  if (strchr(hostfile,' '))
	  {
		  strcat(file,"\"");
		  strcat(file,hostfile);
		  strcat(file,"\"");
	  }
	  else
	    strcat(file,hostfile);
	  hService = CreateService(hSCManager, ServiceName(), ServiceName(), 0, SERVICE_WIN32_OWN_PROCESS,
	  SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, file, NULL, NULL, NULL, NULL, NULL);
	  if (hService == NULL)
		  status = GetLastError();
	  free(file);
	  if (hService)
		  CloseServiceHandle(hService);
	  CloseServiceHandle(hSCManager);
  }
}

#else
static void InstallService() {printf("install option only valid with mdsip_service on NT\n"); exit(0);}
static void RemoveService() {printf("install option only valid with mdsip_service on NT\n"); exit(0);}
static void StartWorker(char **argv){}
static void InitWorkerCommunications(){}
static void InitializeService(){}
static void SetThisServiceStatus(int state, int hint) {}
static int SpawnWorker(){return getpid();}
#ifndef SERVICE_START_PENDING
#define SERVICE_START_PENDING 0
#define SERVICE_RUNNING 1
#endif
#endif

#ifdef HAVE_WINDOWS_H
static int BecomeUser(char *remuser, struct descriptor *user)
{
	return 1;
}
static void InitializeSockets()
{
  WSADATA wsaData;
  WORD wVersionRequested;
  wVersionRequested = MAKEWORD(1,1);
  WSAStartup(wVersionRequested,&wsaData);
}

#else
static void InitializeSockets(){}
static int BecomeUser(char *remuser, struct descriptor *local_user)
{
  int ok = 1;
  CompressString(local_user,0);
  if (local_user->length)
  {
    char *luser = MdsDescrToCstring(local_user);
    char *user = strcmp(luser,"MAP_TO_LOCAL") == 0 ? remuser : luser;
    int status = -1;
    struct passwd *pwd = getpwnam(user);
    if (!pwd && remuser == user)
    {
      int i;
      for (i=0;i<strlen(user);i++)
        user[i] = __tolower(user[i]);
      pwd = getpwnam(user);
    }
    if (pwd)
       status = setuid(pwd->pw_uid);
    MdsFree(luser);
    ok = (status == 0) ? 1 : 2;
  }
  return ok;
}
#endif

int main(int argc, char **argv)
{
  static struct sockaddr_in sin;
  int tablesize = FD_SETSIZE;
  fd_set readfds;
  struct timeval timeout = {1,0};
  InitializeSockets();
  /* DebugBreak(); */
  FD_ZERO(&fdactive);
  if (argc <= 1 && !IsService)
  {
    printf("Usage: mdsip portname|portnumber [multi|install|install_server|install_multi|remove|server] [hostfile]\n");
    exit(0);
  }
  ContextSwitching = 0;
  if (IsService)
	  InitializeService();
  else
  {
	multi = 0;
    portname = argv[1];
    if (argc > 2)
	{
      if (argc > 3)
        hostfile = argv[3];
      if (strcmp("multi",argv[2]) == 0)
	  {
                multi = 1;
                ContextSwitching = 1;
	  }
	  else if (strcmp("install",argv[2]) == 0)
	  {
		InstallService(0);
		exit(0);
	  }
	  else if (strcmp("install_multi",argv[2]) == 0)
	  {
		InstallService(1);
		exit(0);
	  }
	  else if (strcmp("install_server",argv[2]) == 0)
	  {
		InstallService(2);
		exit(0);
	  }
	  else if (strcmp("remove",argv[2]) == 0)
	  {
		RemoveService();
		exit(0);
	  }
	  else if (strcmp("worker",argv[2]) == 0)
	  {
		StartWorker(argv);
          }
          else if (strcmp("server",argv[2]) == 0)
	  {
                multi = 1;
          }
      else
        hostfile = argv[2];
	}
  }
  if (multi || IsService)
  {
    InitWorkerCommunications();
    serverSock = CreateMdsPort(portname,1);
    shut = 0;
	if (IsService)
      SetThisServiceStatus(SERVICE_RUNNING,0);
  }
  else if (!IsWorker)
  {
    serverSock = ConnectToInet(portname);
    shut = (ClientList == NULL);
  }
  readfds = fdactive;
  while (!shut && (select(tablesize, &readfds, 0, 0, (IsWorker || IsService) ? &timeout : 0) != SOCKET_ERROR))
  {
    if (FD_ISSET(serverSock, &readfds))
    {
      int len = sizeof(struct sockaddr_in);
      AddClient(accept(serverSock, (struct sockaddr *)&sin, &len),&sin);
    }
    else
    {
      Client *c,*next;
      for (c=ClientList,next=c ? c->next : 0; c; c=next,next=c ? c->next : 0)
	  {
		if (IsWorker)
		{
			if (*workerShutdown)
			{
				shutdown(c->sock,2);
				closesocket(c->sock);
				_exit(0);
			}
		}
        if (FD_ISSET(c->sock, &readfds))
          DoMessage(c);
	  }
    }
    shut = (ClientList == NULL) && !multi && !IsService;
    readfds = fdactive;
  }
  shutdown(serverSock,2);
  closesocket(serverSock);
  return 1;
}

static int DoMessage(Client *c)
{
  int status;
  Message *msgptr;
  msgptr = GetMdsMsg(c->sock,&status);
  if (status & 1)
  {
    send(c->sock, 0, 0, 0);
    ProcessMessage(c, msgptr);
  }
  else
    RemoveClient(c);
  return status;
}

static void CompressString(struct descriptor *in, int upcase)
{
  unsigned short len;
  StrTrim(in,in,&len);
  if (upcase)
    StrUpcase(in,in);
  while(in->length && (in->pointer[0] == ' ' || in->pointer[0] == '	'))
    StrRight(in,in,&two);
}

static int CheckClient(char *host_c, char *user_c)
{
  FILE *f = fopen(hostfile,"r");
  int ok = 0;
  if (f)
  {
    static char line_c[256];
    static char match_c[256];
    static struct descriptor line_d = {0, DTYPE_T, CLASS_S, line_c};
    static struct descriptor match_d = {0, DTYPE_T, CLASS_S, match_c};
    static struct descriptor match = {0, DTYPE_T, CLASS_D, 0};
    static struct descriptor local_user = {0,DTYPE_T,CLASS_D,0};
    static struct descriptor access_id = {0,DTYPE_T,CLASS_D,0};
    static DESCRIPTOR(delimiter,"|");
    if (multi)
    {
      strcpy(match_c,"MULTI");
    }
    else
    {
      strncpy(match_c,user_c,255);
      strncat(match_c,"@",255);
      strncat(match_c,host_c,255);
    }
    match_d.length = strlen(match_c);
    StrUpcase(&match,&match_d);
    while (ok==0 && fgets(line_c,255,f))
    {
      if (line_c[0] != '#')
      {
        line_d.length = strlen(line_c) - 2;
	    StrElement(&access_id,&zero,&delimiter,&line_d);
        StrElement(&local_user,&one,&delimiter,&line_d);
        CompressString(&access_id,1);
        if (access_id.length)
        {
          if (access_id.pointer[0] != '!')
          {
            if (StrMatchWild(&match,&access_id) & 1)
				ok = BecomeUser(user_c,&local_user);
          }
          else
          {
            StrRight(&access_id,&access_id,&two);
            if (StrMatchWild(&match,&access_id) & 1)
              ok = 2;
          }
        }
      }
    }
    fclose(f);
  }
  else
  {
    printf("Unable to open hostfile: %s\n",hostfile);
    exit(1);
  }
  return ok;
}

static void AddClient(int sock,struct sockaddr_in *sin)
{

  if (sock >= 0)
  {
    static Message m;
    Message *m_user;
    char *timestr;
    char *user = 0;
    char *user_p = 0;
    struct hostent *hp;
    int i;
    int status;
    int pid;
    int ok = 0;
    Client *c;
    time_t tim;
    m.h.msglen = sizeof(MsgHdr);
    hp = gethostbyaddr((char *)&sin->sin_addr,sizeof(sin->sin_addr),AF_INET);
    m_user = GetMdsMsg(sock,&status);
    if ((status & 1) && (m_user) && (m_user->h.dtype == DTYPE_CSTRING))
    {
      user = malloc(m_user->h.length + 1);
      memcpy(user,m_user->bytes,m_user->h.length);
      user[m_user->h.length] = 0;
    }
    user_p = user ? user : "?";
    tim = time(0);
    timestr = ctime(&tim);
    timestr[strlen(timestr)-1] = 0;
    if (!multi)
    {
      if (hp) ok = CheckClient(hp->h_name,user_p);
      if (ok == 0) ok = CheckClient((char *)inet_ntoa(sin->sin_addr),user_p);
    }
    else
      ok = 1;
    m.h.status = ok & 1;
    m.h.client_type = m_user->h.client_type;
    SendMdsMsg(sock,&m,0);
	if (NO_SPAWN)
	{
      if (m.h.status && NO_SPAWN)
	  {
        Client *new = malloc(sizeof(Client));
        FD_SET(sock,&fdactive);
        new->sock = sock;
        new->context.tree = 0;
        new->message_id = 0;
        new->event = 0;
        for (i=0;i<6;i++)
	      new->tdicontext[i] = NULL;
        for (i=0;i<MAX_ARGS;i++)
          new->descrip[i] = NULL;
        new->addr = *(int *)&sin->sin_addr;
        new->next = NULL;
        for (c=ClientList; c && c->next; c = c->next);
        if (c) 
          c->next = new;
        else
          ClientList = new;
	  }
	  pid = getpid();
	}
	else
      pid = SpawnWorker(sock);
    if (hp)
      printf("%s (%d) (pid %d) Connection received from %s@%s [%s]\r\n", timestr,sock, pid, user_p, hp->h_name, inet_ntoa(sin->sin_addr));
    else
      printf("%s (%d) (pid %d) Connection received from %s@%s\r\n", timestr, sock, pid, user_p, inet_ntoa(sin->sin_addr));
    if (!m.h.status)
    {
      printf("Access denied\n");
      shutdown(sock,2);
      closesocket(sock);
    }
    if (user) free(user);
	fflush(stdout);
  }
}

static void FreeDescriptors(Client *c)
{
  int i;
  for (i=0;i<MAX_ARGS;i++)
  {
    if (c->descrip[i])
    {
      if (c->descrip[i] != MdsEND_ARG)
      {
        if (c->descrip[i]->pointer)
          free(c->descrip[i]->pointer);
        free(c->descrip[i]);
      }
      c->descrip[i] = NULL;
    }
  }
}

static void RemoveClient(Client *c)
{
  Client *p,*nc;
  MdsEventList *e,*nexte;
  FD_CLR(c->sock,&fdactive);
  shutdown(c->sock,2);
  closesocket(c->sock);
  FreeDescriptors(c);
  for (e=c->event; e; e=nexte)
  {
    nexte = e->next;
    /**/
    MDSEventCan(e->eventid);
    /**/
    if (e->info_len > 0) free(e->info);
    free(e);
  }
  if (c->context.tree)
  {
    void  *old_context;
    void *tdi_context[6];
    old_context = TreeSwitchDbid(c->context.tree);
    TreeClose(0,0);
    c->context.tree = TreeSwitchDbid(old_context);
    TdiSaveContext(tdi_context);
    TdiRestoreContext(c->tdicontext);
    TdiResetPublic(MdsEND_ARG);
    TdiResetPrivate(MdsEND_ARG);
    TdiRestoreContext(tdi_context);
  }
  for (p=0,nc=ClientList; nc && nc!=c; p=nc,nc=nc->next);
  if (nc == c)
  {
    if (p)
      p->next = c->next;
    else
      ClientList = c->next;
  }
  free(c);
}

static void ConvertFloat(int num, int in_type, char in_length, char *in_ptr, int out_type, char out_length, char *out_ptr)
{
  int i;
  char *in_p;
  char *out_p;
  for (i=0,in_p=in_ptr,out_p=out_ptr;i<num;i++,in_p += in_length, out_p += out_length)
  {
    char *ptr = in_p;
    char cray_f[8];
    if (in_type == CvtCRAY)
    {
      int j,k;
      for (j=0;j<2;j++)
        for (k=0;k<4;k++)
#ifdef _big_endian
          cray_f[j * 4 + k] = ptr[j * 4 + 3 - k];
#else
          cray_f[(1-j)*4 + k] = ptr[j * 4 + 3 - k];
#endif
      ptr = cray_f;
    }
    CvtConvertFloat(ptr, in_type, out_p, out_type, 0);
#ifdef _big_endian
    if (out_type == CvtCRAY)
    {
      int j,k;
      ptr = out_p;
      for (j=0;j<2;j++)
        for (k=0;k<4;k++)
          cray_f[j * 4 + k] = ptr[j * 4 + 3 - k];
      for (j=0;j<8;j++) ptr[j] = cray_f[j];
    }
#endif
  }
}

static void ConvertBinary(int num, int sign_extend, short in_length, char *in_ptr, short out_length, char *out_ptr)
{
  int i;
  int j;
  signed char *in_p = (signed char *)in_ptr;
  signed char *out_p = (signed char *)out_ptr;
  short min_len = min(out_length,in_length);
  for (i=0; i<num; i++,in_p += in_length, out_p += out_length)
  {
    for (j=0;j < min_len; j++) 
      out_p[j] = in_p[j];
    for (;j < out_length; j++)
      out_p[j] = sign_extend ? (in_p[min_len-1] < 0 ? -1 : 0) : 0;
  }
}

static void ProcessMessage(Client *c, Message *message)
{
  if (c->message_id != message->h.message_id)
  {
    FreeDescriptors(c);
    if (message->h.nargs < MAX_ARGS-1)
    {
      c->message_id = message->h.message_id;
      c->nargs = message->h.nargs;
    }
    else
      return;
  }
  if (message->h.descriptor_idx < c->nargs)
  {
    struct descriptor *d = c->descrip[message->h.descriptor_idx];
    int idx = message->h.descriptor_idx;
    c->client_type = message->h.client_type;
    if (!d)
    {
      static short lengths[] = {0,0,1,2,4,0,1,2,4,0,4,8,8,0,0};
      switch (message->h.ndims)
      {
        static struct descriptor scalar = {0, 0, CLASS_S, 0};
        static DESCRIPTOR_A_COEFF(array_1,0,0,0,1,0);
        static DESCRIPTOR_A_COEFF(array_2,0,0,0,2,0);
        static DESCRIPTOR_A_COEFF(array_3,0,0,0,3,0);
        static DESCRIPTOR_A_COEFF(array_4,0,0,0,4,0);
        static DESCRIPTOR_A_COEFF(array_5,0,0,0,5,0);
        static DESCRIPTOR_A_COEFF(array_6,0,0,0,6,0);
        static DESCRIPTOR_A_COEFF(array_7,0,0,0,7,0);
        case 0:  d = (struct descriptor *)MakeDesc(scalar);  break;
        case 1:  d = (struct descriptor *)MakeDesc(array_1); break;
        case 2:  d = (struct descriptor *)MakeDesc(array_2); break;
        case 3:  d = (struct descriptor *)MakeDesc(array_3); break;
        case 4:  d = (struct descriptor *)MakeDesc(array_4); break;
        case 5:  d = (struct descriptor *)MakeDesc(array_5); break;
        case 6:  d = (struct descriptor *)MakeDesc(array_6); break;
        case 7:  d = (struct descriptor *)MakeDesc(array_7); break;
      }
      d->length = message->h.dtype == DTYPE_CSTRING ? message->h.length : lengths[min(message->h.dtype,12)];
      d->dtype = message->h.dtype;
      if (d->class == CLASS_A)
      {
        ARRAY_7 *a = (ARRAY_7 *)d;
        int num = 1;
        int i;
        for (i=0;i<message->h.ndims;i++)
        {
          a->m[i] = message->h.dims[i];
          num *= a->m[i];
        }
        a->arsize = a->length * num;
        a->pointer = a->a0 = malloc(a->arsize);
      }
      else
        d->pointer = malloc(d->length);
      c->descrip[message->h.descriptor_idx] = d;
    }
    if (d)
    {
      int dbytes = d->class == CLASS_S ? (int)d->length : (int)((ARRAY_7 *)d)->arsize;
      int num = dbytes/max(1,d->length);
      switch (CType(c->client_type))
      {
        case IEEE_CLIENT:
        case JAVA_CLIENT:
          memcpy(d->pointer,message->bytes,dbytes); break;
          break;
        case CRAY_CLIENT:
          switch (d->dtype)
          {
            case DTYPE_USHORT:
            case DTYPE_ULONG: ConvertBinary(num, 0, message->h.length, message->bytes, d->length, d->pointer); break;
            case DTYPE_SHORT:
            case DTYPE_LONG:  ConvertBinary(num, 1, message->h.length, message->bytes, d->length, d->pointer); break;
            case DTYPE_F: ConvertFloat(num, CvtCRAY, (char)message->h.length, message->bytes, 
                                                  CvtIEEE_S, (char)d->length, d->pointer); break;
            case DTYPE_FC: ConvertFloat(num * 2, CvtCRAY, (char)(message->h.length/2), message->bytes, 
                                                  CvtIEEE_S, (char)(d->length/2), d->pointer); break;
            case DTYPE_D: ConvertFloat(num, CvtCRAY, (char)message->h.length, message->bytes, 
                                                  CvtIEEE_T, sizeof(double), d->pointer); break;
            default: memcpy(d->pointer,message->bytes,dbytes); break;
          }
          break;
        default:
          switch (d->dtype)
          {
            case DTYPE_F:
            case DTYPE_FC: ConvertFloat(num, CvtVAX_F, (char)message->h.length, message->bytes,
                                                  CvtIEEE_S, sizeof(float), d->pointer); break;
            case DTYPE_D: if (CType(c->client_type) == VMSG_CLIENT)
                            ConvertFloat(num, CvtVAX_G, (char)message->h.length, message->bytes,
                                                  CvtIEEE_T, sizeof(double), d->pointer); 
                          else
                            ConvertFloat(num, CvtVAX_D, (char)message->h.length, message->bytes,
                                                  CvtIEEE_T, sizeof(double), d->pointer);
                          break;
            default: memcpy(d->pointer,message->bytes,dbytes); break;
          }
      }
      switch (d->dtype)
      {
      case DTYPE_F: d->dtype = DTYPE_FS; break;
      case DTYPE_FC: d->dtype = DTYPE_FSC; break;
      case DTYPE_D: d->dtype = DTYPE_FT; break;
      }
      if (message->h.descriptor_idx == (message->h.nargs - 1))
      {
#if defined(_THREADS)
        pthread_t thread;
        pthread_create(&thread,pthread_attr_default,(pthread_startroutine_t)ExecuteMessage,(pthread_addr_t)c);
#endif
        ExecuteMessage(c);
      }
    }
  }
}

static void ClientEventAst(MdsEventList *e, int len, char *data)
{
  Client *c;
  int i;
  for (c=ClientList; c && (c->sock != e->sock); c = c->next);
  if (c)
  {
    if (CType(c->client_type) == JAVA_CLIENT)
    {
      JMdsEventInfo *info;
      int len = sizeof(MsgHdr) + sizeof(JMdsEventInfo);
      Message *m = malloc(len);
      m->h.client_type = c->client_type;
      m->h.msglen = len;
      m->h.dtype = DTYPE_EVENT;
      info = (JMdsEventInfo *)m->bytes;
      memcpy(info->data, data, (len<12)?len:12);
      for(i = len; i < 12; i++)
        info->data[i] = 0;
      info->eventid = e->jeventid;
      SendMdsMsg(c->sock, m, 0);
      free(m);
    }
    else
    {
      Message *m = malloc(sizeof(MsgHdr) + e->info_len);
      m->h.client_type = c->client_type;
      m->h.msglen = sizeof(MsgHdr) + e->info_len;
      m->h.dtype = DTYPE_EVENT;
      memcpy(e->info->data, data, (len<12)?len:12); 
      for(i = len; i < 12; i++)
        e->info->data[i] = 0;
      memcpy(m->bytes,e->info,e->info_len);
      SendMdsMsg(c->sock, m, MSG_OOB);
      free(m);
    }
  }
  else
  {
    MDSEventCan(e->eventid);
    if (e->info_len > 0) free(e->info);
    free(e);
  }
}

static void ExecuteMessage(Client *c)
{
  int status=1;
  static EMPTYXD(emptyxd);
  struct descriptor_xd *xd;
  char *evname;
  static DESCRIPTOR(eventastreq,EVENTASTREQUEST);
  static DESCRIPTOR(eventcanreq,EVENTCANREQUEST);
  int java = CType(c->client_type) == JAVA_CLIENT;
  if (StrCompare(c->descrip[0],&eventastreq) == 0)
  {
    static int eventid;
    static DESCRIPTOR_LONG(eventiddsc,&eventid);
    MdsEventList *newe = (MdsEventList *)malloc(sizeof(MdsEventList));
    struct descriptor_a *info = (struct descriptor_a *)c->descrip[2];
    newe->sock = c->sock;
    /**/
    evname = malloc(c->descrip[1]->length + 1);
    memcpy(evname, c->descrip[1]->pointer, c->descrip[1]->length);
    evname[c->descrip[1]->length] = 0;
    status = MDSEventAst(evname,(void (*)())ClientEventAst,newe,&newe->eventid);
    free(evname); 
 /**/
    if (java)
    {
      newe->info = 0;
      newe->info_len = 0;
      newe->jeventid = *c->descrip[2]->pointer;
    }
    else
    {
      newe->info = (struct _eventinfo *)memcpy(malloc(info->arsize),info->pointer,info->arsize);
      newe->info_len = info->arsize;
      newe->info->eventid = newe->eventid;
    }
    newe->next = 0;
    if (!(status & 1))
    {
      free(newe->info);
      free(newe);
    }
    else
    {
      MdsEventList *e;
      if (c->event)
      {
        for(e=c->event;e->next;e=e->next);
        e->next = newe;
      }
      else
        c->event = newe;
    }
    if (!java)
      SendResponse(c,status,&eventiddsc);
  }
  else if (StrCompare(c->descrip[0],&eventcanreq) == 0)
  {
    static int eventid;
    static DESCRIPTOR_LONG(eventiddsc,&eventid);
    MdsEventList *e;
    MdsEventList **p;
    if (!java)
      eventid = *(int *)c->descrip[1]->pointer;
    if (c->event)
    {
      if (java)
        for(p=&c->event,e=c->event;e && (*c->descrip[1]->pointer != e->jeventid) ;p=&e->next,e=e->next);
      else
        for(p=&c->event,e=c->event;e && (e->eventid != eventid) ;p=&e->next,e=e->next);
      if (e)
      {
	/**/
        MDSEventCan(e->eventid);
	/**/
        free(e);
        *p = e->next;
      }
    }
    if (!java)
      SendResponse(c,status,&eventiddsc);
  }
  else
  {
    void  *old_context;
    void *tdi_context[6];
    EMPTYXD(ans);
    if (ContextSwitching)
    {
      old_context = TreeSwitchDbid(c->context.tree);
      TdiSaveContext(tdi_context);
      TdiRestoreContext(c->tdicontext);
    }
    c->descrip[c->nargs++] = (struct descriptor *)(xd = (struct descriptor_xd *)memcpy(malloc(sizeof(emptyxd)),&emptyxd,sizeof(emptyxd)));
    c->descrip[c->nargs++] = MdsEND_ARG;
    DefineTdi("public $REMADDR=$",DTYPE_LONG,4,&c->addr);
    DefineTdi("public $PORTNAME=$",DTYPE_T,(short)strlen(PortName),PortName);
    ResetErrors();
    status = LibCallg(&c->nargs, TdiExecute);
    if (status & 1) status = TdiData(xd,&ans MDS_END_ARG);
    if (!(status & 1)) GetErrorText(status,&ans);
    SendResponse(c,status,ans.pointer);
    MdsFree1Dx(xd,NULL);
	MdsFree1Dx(&ans,NULL);
    if (ContextSwitching)
    {
      TdiSaveContext(c->tdicontext);
      TdiRestoreContext(tdi_context);
      c->context.tree = TreeSwitchDbid(old_context);
    }
  }
  FreeDescriptors(c);
}

static void SendResponse(Client *c, int status, struct descriptor *d)
{
  static Message *m = 0;
  int flag = 0;
  int nbytes = (d->class == CLASS_S) ? d->length : ((ARRAY_7 *)d)->arsize;
  int num = nbytes/max(1,d->length);
  short length = d->length;
  if (CType(c->client_type) == CRAY_CLIENT)
  {
    switch (d->dtype)
    {
        case DTYPE_USHORT: 
        case DTYPE_ULONG:
        case DTYPE_SHORT:
        case DTYPE_LONG:
        case DTYPE_F:
        case DTYPE_FS: length = 8; break;
        case DTYPE_FC: 
        case DTYPE_FSC:
        case DTYPE_D: 
        case DTYPE_G: 
        case DTYPE_FT: length = 16; break;
        default: length = d->length; break;
    }
    nbytes = num * length;
  }
  m = malloc(sizeof(MsgHdr) + nbytes);
  m->h.msglen = sizeof(MsgHdr) + nbytes;
  m->h.client_type = c->client_type;
  m->h.message_id = c->message_id;
  m->h.status = status;
  m->h.dtype = d->dtype;
  m->h.length = length;
  if (d->class == CLASS_S)
    m->h.ndims = 0;
  else
  {
    int i;
    array_coeff *a = (array_coeff *)d;
    m->h.ndims = a->dimct;
    if (a->aflags.coeff)
      for (i=0;i<m->h.ndims && i<7;i++)
        m->h.dims[i] = a->m[i];
    else
      m->h.dims[0] = a->length ? a->arsize/a->length : 0;
    for (i=m->h.ndims; i<7; i++)
      m->h.dims[i] = 0;
  }
  switch (CType(c->client_type))
  {
    case IEEE_CLIENT:
    case JAVA_CLIENT:
      switch (d->dtype)
      {
        case DTYPE_F:  ConvertFloat(num, CvtVAX_F, (char)d->length, d->pointer, 
                                               CvtIEEE_S, (char)m->h.length, m->bytes); break;
        case DTYPE_FC: ConvertFloat(num * 2, CvtVAX_F, (char)(d->length/2), d->pointer, 
                                               CvtIEEE_S, (char)(m->h.length/2), m->bytes); break;
        case DTYPE_D: ConvertFloat(num, CvtVAX_D, (char)d->length, d->pointer, 
                                              CvtIEEE_T, (char)m->h.length, m->bytes); break;
        case DTYPE_G: ConvertFloat(num, CvtVAX_G, (char)d->length, d->pointer, 
                                              CvtIEEE_T, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_D; break;
        default: memcpy(m->bytes,d->pointer,nbytes); break;
      }
      break;
    case CRAY_CLIENT:
      switch (d->dtype)
      {
      case DTYPE_USHORT:
      case DTYPE_ULONG: 
        ConvertBinary(num, 0, d->length, d->pointer, m->h.length, m->bytes);
        break;
      case DTYPE_SHORT:
      case DTYPE_LONG:  
        ConvertBinary(num, 1, (char)d->length, d->pointer, (char)m->h.length, m->bytes); 
        break;
      case DTYPE_F:
        ConvertFloat(num, CvtVAX_F, (char)d->length, d->pointer,CvtCRAY, (char)m->h.length, m->bytes); 
        break;
      case DTYPE_FS: 
        ConvertFloat(num, CvtIEEE_S, (char)d->length, d->pointer, CvtCRAY, (char)m->h.length, m->bytes); 
        break;
      case DTYPE_FC: 
        ConvertFloat(num*2,CvtVAX_F, (char)(d->length/2), d->pointer,CvtCRAY,(char)(m->h.length/2), m->bytes); 
        break;
      case DTYPE_FSC: 
        ConvertFloat(num*2,CvtIEEE_S,(char)(d->length/2),d->pointer,CvtCRAY,(char)(m->h.length/2), m->bytes);
        break;
      case DTYPE_D:
        ConvertFloat(num, CvtVAX_D, sizeof(double), d->pointer, CvtCRAY, (char)m->h.length, m->bytes); 
        break;
      case DTYPE_G:
        ConvertFloat(num, CvtVAX_G, sizeof(double), d->pointer, CvtCRAY, (char)m->h.length, m->bytes); 
        break;
      case DTYPE_FT: 
        ConvertFloat(num, CvtIEEE_T, sizeof(double), d->pointer, CvtCRAY, (char)m->h.length, m->bytes); 
        break;
      default: 
        memcpy(m->bytes,d->pointer,nbytes); 
        break;
      }
      break;
    case VMSG_CLIENT:
      switch (d->dtype)
      {
        case DTYPE_FS: ConvertFloat(num, CvtIEEE_S, sizeof(float), d->pointer, 
                                              CvtVAX_F, (char)m->h.length, m->bytes); break;
        case DTYPE_D: ConvertFloat(num, CvtVAX_D, sizeof(double), d->pointer, 
                                              CvtVAX_G, (char)m->h.length, m->bytes); break;
        case DTYPE_FT: ConvertFloat(num, CvtIEEE_T, sizeof(double), d->pointer, 
                                              CvtVAX_G, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_D; break;
        default: memcpy(m->bytes,d->pointer,nbytes); break;
      }
      break;
    default:
      switch (d->dtype)
      {
        case DTYPE_FS: ConvertFloat(num, CvtIEEE_S, sizeof(float), d->pointer, 
                                              CvtVAX_F, (char)m->h.length, m->bytes); break;
        case DTYPE_G: ConvertFloat(num, CvtVAX_G, sizeof(double), d->pointer, 
                                              CvtVAX_D, (char)m->h.length, m->bytes); break;
        case DTYPE_FT: ConvertFloat(num, CvtIEEE_T, sizeof(double), d->pointer, 
                                              CvtVAX_D, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_D; break;
        default: memcpy(m->bytes,d->pointer,nbytes); break;
      }
      break;
  }
  if (m->h.dtype == DTYPE_G || m->h.dtype == DTYPE_FT)
    m->h.dtype = DTYPE_D;
  else if (m->h.dtype == DTYPE_FS)
    m->h.dtype = DTYPE_F;
  else if (m->h.dtype == DTYPE_FSC)
    m->h.dtype = DTYPE_FC;
  SendMdsMsg(c->sock, m, 0);
  free(m);
}

static int CreateMdsPort(char *service, int multi_in)
{
  static struct sockaddr_in sin;
  struct servent *sp;
  long sendbuf=32768,recvbuf=32768;
  int s;
  int status;
  if (multi)
    CheckClient(0,0);
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == -1)
  {
    printf("Error getting Connection Socket\n");
    return 0;
  }
  FD_SET(s,&fdactive);
  sin.sin_port = htons((short)atoi(service));
  if (sin.sin_port == 0)
  {
    sp = getservbyname(service,"tcp");
    if (sp == NULL)
    {
      printf("unknown service: %s/tcp\n",service);
      exit(1);
    }
    sin.sin_port = sp->s_port;
  }
  sp = getservbyport(sin.sin_port,"tcp");
  PortName = strcpy((char *)malloc(strlen(sp ? sp->s_name :service)+1),sp ? sp->s_name : service);
  setsockopt(s, SOL_SOCKET,SO_RCVBUF,(char *)&recvbuf,sizeof(long));
  setsockopt(s, SOL_SOCKET,SO_SNDBUF,(char *)&sendbuf,sizeof(long));  
  status = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&multi_in,sizeof(1));
  if (status < 0)
  {
    printf("Error setting socket options\n");
    exit(1);
  }
  if (multi_in)
  {
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    status = bind(s, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
    if (status < 0)
    {
      perror("Error binding to service\n");
      exit(1);
    }
  }
  status = listen(s,5);
  if (status < 0)
  {
    printf("Error from listen\n");
    exit(1);
  }
  return s;
}

static int ConnectToInet(char *service)
{
  static struct sockaddr_in sin;
  int s=-1;
  int n = sizeof(sin);
  int status = 1;
#ifdef _VMS
  status = sys$assign(&INET, &s, 0, 0);
#else
  s=0;
#endif
  if (!(status & 1)) { exit(status);}
  FD_SET(s,&fdactive);
  if ((status=getpeername(s, (struct sockaddr *)&sin, &n)) < 0)
  {
    perror("Error getting peer name");
    exit(0);
  }
  AddClient(s,&sin);
  return CreateMdsPort(service, 0);
}

void ResetErrors()
{
  static int four = 4;
  static DESCRIPTOR_LONG(clear_messages,&four);
  static DESCRIPTOR(messages,"");
  TdiDebug(&clear_messages,&messages MDS_END_ARG);
}

void GetErrorText(int status, struct descriptor_xd *xd)
{
  static int one = 1;
  static DESCRIPTOR_LONG(get_messages,&one);
  TdiDebug(&get_messages,xd MDS_END_ARG);
  if (!xd->length)
  {
    static DESCRIPTOR(unknown,"unknown error occured");
    struct descriptor message = {0, DTYPE_T, CLASS_S, 0};
    if ((message.pointer = MdsGetMsg(status)) != NULL)
    {
	message.length = strlen(message.pointer);
        MdsCopyDxXd(&message,xd);
    }
    else
      MdsCopyDxXd((struct descriptor *)&unknown,xd);
  }
}

static void DefineTdi(char *execute_string, char dtype, short length, void *value)
{
  struct descriptor cmd = {0, DTYPE_T, CLASS_S, 0};
  struct descriptor val = {0, 0, CLASS_S, 0};
  EMPTYXD(emptyxd);
  cmd.length = strlen(execute_string);
  cmd.pointer = execute_string;
  val.length = length;
  val.dtype = dtype;
  val.pointer = value;
  TdiExecute(&cmd,&val,&emptyxd MDS_END_ARG);
  MdsFree1Dx(&emptyxd,NULL);
}
