#ifndef HAVE_VXWORKS_H
#include <config.h>
#endif
#include <mdstypes.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <strroutines.h>
#include <libroutines.h>
#include "../tdishr/cvtdef.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <mds_stdarg.h>
#include <sys/stat.h>
#if (!defined(HAVE_WINDOWS_H) && !defined(HAVE_VXWORKS_H))
#include <pthread.h>
#include <sys/wait.h>
#endif
#ifdef HAVE_VXWORKS_H
#include <vxWorks.h>
#include <taskLib.h>
#undef max
#undef min
#endif
#include "mdsip.h"
#define MAX_ARGS 256
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#if defined(HAVE_WINDOWS_H)
#include <windows.h>
#include <process.h>
#else
#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif
#ifdef HAVE_VXWORKS_H
#undef BIG_ENDIAN
#include <resolvLib.h>
#else
#include <pwd.h>
#include <unistd.h>  /* for getpid() */
#endif
#define closesocket close
#endif
#ifdef HAVE_WINDOWS_H
#define DEFAULT_HOSTFILE "C:\\MDSIP.HOSTS"
#else
#define DEFAULT_HOSTFILE "/etc/mdsip.hosts"
#endif

#define __tolower(c) (((c) >= 'A' && (c) <= 'Z') ? (c) | 0x20 : (c))

extern char *MdsDescrToCstring();
extern void MdsFree();
extern void SetSocketOptions();
extern int SetCompressionLevel();
extern int GetCompressionLevel();
extern int MdsSetServerPortname(char *);
extern int MdsSetClientAddr(int addr);
extern SOCKET CreateListener();

static int CheckClient(char *host_c, char *user_c);
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
                         int compression_level;
                         struct _client *next;
                       } Client;

static Client *ClientList = 0;
static int multi = 0;
static int IsService = 0;
static int IsWorker = 0;
static int NO_SPAWN = 1;
static int CommandParsed = 0;
static int ContextSwitching = 0;
static char *hostfile;
static char *Portname;
static short port;
static char mode;
static int flags;
static int MaxCompressionLevel = 9;
#define IS_SERVICE 0x200

static void ParseCommand(int argc, char **argv,char **portname, short *port, char **hostfile, char *mode, int *flags, int *compression_level);
extern int TdiSaveContext();
extern int TdiRestoreContext();
extern int TdiExecute();
extern int TdiData();
extern int TdiDebug();
extern int TdiResetPublic();
extern int TdiResetPrivate();
extern char *TranslateLogical();

static int CreateMdsPort(short port, int multi);
static void AddClient(int socket, struct sockaddr_in *sin,char *dn);
static void RemoveClient(Client *c);
static void ProcessMessage(Client *c, Message *message);
static void ExecuteMessage(Client *c);
static void SendResponse(Client *c,int status, struct descriptor *xd);
static int DoMessage(SOCKET s);
static void ConvertFloat(int nbytes, int in_type, char in_length, char *in_ptr, int out_type, char out_length, char *out_ptr);
static void PrintHelp(char *option);
static void SetMode(char modein, char *mode);

extern int ConnectToInet(unsigned short port,void (*AddClient_in)(int,struct sockaddr_in *,char *), int (*DoMessage_in)(SOCKET s));
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
	  strcat(name,Portname);
	}
	return name;
}

static void RedirectOutput(int pid)
{
	char file[120];
	if (pid)
	  sprintf(file,"C:\\MDSIP_%s[%d].log",Portname,pid);
	else
	  sprintf(file,"C:\\MDSIP_%s.log",Portname);
	freopen(file,"a",stdout);
	freopen(file,"a",stderr);
}

static void StartWorker(char **argv)
{
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
  char cmd[1024];
  sprintf(cmd,"mdsip_service -p %s -c %d -W %d %d",Portname,MaxCompressionLevel,GetCurrentProcessId(),sock);
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
    hService = RegisterServiceCtrlHandler(ServiceName(),(LPHANDLER_FUNCTION) serviceHandler);
    SetThisServiceStatus(SERVICE_START_PENDING,1000);
    RedirectOutput(0);
}

int main( int argc, char **argv) 
{
  CommandParsed = 1;
  ParseCommand(argc, argv, &Portname, &port, &hostfile, &mode, &flags, &MaxCompressionLevel);
  if (flags & IS_SERVICE)
  {
    SERVICE_TABLE_ENTRY srvcTable[] = {{ServiceName(),(LPSERVICE_MAIN_FUNCTION)ServiceMain},{NULL,NULL}};
    IsService = 1;
    StartServiceCtrlDispatcher(srvcTable);
  }
  else
    ServiceMain(argc,argv);
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
    char *file = (char *)malloc(strlen(_pgmptr)+strlen(Portname)+strlen(hostfile)+100);
    sprintf(file,"%s -p %s %s -h \"%s\" -F %d",_pgmptr,Portname,
          (typesrv == 0) ? "" : ((typesrv == 1) ? "-m" : "-s"),hostfile, flags | IS_SERVICE);
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
static void InstallService(int arg) {printf("install option only valid with mdsip_service on NT\n"); exit(0);}
static void RemoveService() {printf("install option only valid with mdsip_service on NT\n"); exit(0);}
static void StartWorker(char **argv){}
static void InitWorkerCommunications(){}
static void InitializeService(){}
static void SetThisServiceStatus(int state, int hint) {}
#ifdef HAVE_VXWORKS_H
static int SpawnWorker(int arg){return taskIdSelf();}
#else
static int SpawnWorker(int arg){return getpid();}
#endif
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
#ifdef HAVE_VXWORKS_H
static int BecomeUser(char *remuser, struct descriptor *user)
{
	return 1;
}
#else

static void sigchld_handler(int num);
static int BecomeUser(char *remuser, struct descriptor *local_user)
{
  int ok = 1;
  CompressString(local_user,0);
  if (local_user->length)
  {
    char *luser = MdsDescrToCstring(local_user);
    char *user = (strcmp(luser,"MAP_TO_LOCAL") == 0) ? remuser : 
      ((strcmp(luser,"SELF") == 0) ? (char *)cuserid((char *)0) : luser);
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
    {
       int pid = 0;
       int homelen = strlen(pwd->pw_dir); 
       char *cmd = strcpy(malloc(homelen+10),"HOME=");
       char *mds_path = getenv("MDS_PATH");
       if (mode == 'I')
       {
         signal(SIGCHLD,sigchld_handler);
         pid = fork();
       }
       else
         pid = getpid();
       if (!pid || mode != 'I')
       {
#ifdef GLOBUS
         chown(getenv("X509_USER_PROXY"),pwd->pw_uid,pwd->pw_gid);
#endif
         initgroups(pwd->pw_name,pwd->pw_gid);
         status = setgid(pwd->pw_gid);
         status = setuid(pwd->pw_uid);
         if (status)
           printf("Cannot setuid - run server as root!");
         strcat(cmd,pwd->pw_dir);
         putenv(cmd);
       /* DO NOT FREE CMD --- putenv requires it to stay allocated */
       }
       else
         status = 0;
    }
    else
      printf("Invalid mapping, cannot find user %s\n",user);
    MdsFree(luser);
    ok = (status == 0) ? 1 : 0;
  }
  return ok;
}
#endif
#endif

#ifdef HAVE_VXWORKS_H

int mdsip(int portIn, char modeIn,  int compressionIn)
{
  char port_name[64];
  InitializeSockets();
  /* DebugBreak(); */
  
  sprintf(port_name, "%d", portIn);
  Portname = malloc(strlen(port_name) + 1);
  strcpy(Portname, port_name);
  port = portIn;
  SetMode(modeIn, &mode);
  hostfile =0;
  flags = 0;
  MaxCompressionLevel = compressionIn;

#else
static void sigchld_handler(int num)
                    {
                      sigset_t set, oldset;
                      pid_t pid;
                      int status, exitstatus;

                      /* block other incoming SIGCHLD signals */
                      sigemptyset(&set);
                      sigaddset(&set, SIGCHLD);
                      sigprocmask(SIG_BLOCK, &set, &oldset);

                      /* wait for child */
                      while((pid = waitpid((pid_t)-1, &status, WNOHANG)) > 0)
                        {
			  /*
                          if(WIFEXITED(status))
                            {
                              exitstatus = WEXITSTATUS(status);

                              fprintf(stderr, 
                                      "Parent: child exited, pid = %d, exit status = %d\n", 
                                      (int)pid, exitstatus);
                            }
                          else if(WIFSIGNALED(status))
                            {
                              exitstatus = WTERMSIG(status);

                              fprintf(stderr,
                                      "Parent: child terminated by signal %d, pid = %d\n",
                                      exitstatus, (int)pid);
                            }
                          else if(WIFSTOPPED(status))
                            {
                              exitstatus = WSTOPSIG(status);

                              fprintf(stderr,
                                      "Parent: child stopped by signal %d, pid = %d\n",
                                      exitstatus, (int)pid);
                            }
                          else
                            {
                              fprintf(stderr,
                                      "Parent: child exited magically, pid = %d\n",
                                      (int)pid);
                            }
			  */
                        }

                      /* re-install the signal handler (some systems need this) */
                      signal(SIGCHLD, sigchld_handler);
                      
                      /* and unblock it */
                      sigemptyset(&set);
                      sigaddset(&set, SIGCHLD);
                      sigprocmask(SIG_UNBLOCK, &set, &oldset);
                    }

int main(int argc, char **argv)
{
  signal(SIGCHLD,sigchld_handler);
  InitializeSockets();
  /* DebugBreak(); */
  if (!CommandParsed) 
    ParseCommand(argc, argv, &Portname, &port, &hostfile, &mode, &flags, &MaxCompressionLevel);

#endif
  MdsSetServerPortname(Portname);
  if (IsService)
    InitializeService();
  else
  {
    switch (mode)
    {
    case 0:     ContextSwitching = 0; multi = 0; break;
    case 'I':   ContextSwitching = 0; multi = 0; break;
    case 'm':   ContextSwitching = 1; multi = 1; break;
    case 's':   ContextSwitching = 0; multi = 1; break;
    case 'i':   InstallService(0); exit(0); break;
    case 'M':   InstallService(1); exit(0); break;
    case 'S':   InstallService(2); exit(0); break;
    case 'r':   RemoveService(); exit(0); break;
#ifndef HAVE_VXWORKS_H
    case 'w':   StartWorker(argv); break;
#endif
    }
  }
  if (multi || IsService || mode == 'I')
  {
    InitWorkerCommunications();
    if (multi)
    {
     char multistr[] = "MULTI";
     CheckClient(0,multistr);
    }
    serverSock = CreateMdsPort(port,1);
    shut = 0;
    if (IsService)
    {
      NO_SPAWN = multi;
      SetThisServiceStatus(SERVICE_RUNNING,0);
    }
  }
  else if (!IsWorker)
  {
    serverSock = ConnectToInet(port,AddClient,DoMessage);
    shut = (ClientList == NULL);
#ifdef GLOBUS
    if (shut)
    {
      exit(0);
    }
#endif
  }
#ifdef GLOBUS
  Poll(0);
#else
  {
  static struct sockaddr_in sin;
  int tablesize = FD_SETSIZE;
  extern fd_set FdActive();
  struct timeval timeout = {1,0};
  fd_set readfds;
  readfds = FdActive();
  while (!shut && (select(tablesize, &readfds, 0, 0, (IsWorker || IsService) ? &timeout : 0) != SOCKET_ERROR))
  {
    if ((serverSock != -1) && FD_ISSET(serverSock, &readfds))
    {
      int len = sizeof(struct sockaddr_in);
      (*AddClient)(accept(serverSock, (struct sockaddr *)&sin, &len),&sin,0);
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
            CloseSocket(c->sock);
            exit(0);
          }
        }
        if (FD_ISSET(c->sock, &readfds))
          (*DoMessage)(c->sock);
      }
    }
    shut = (ClientList == NULL) && !multi && !IsService;
    readfds = FdActive();
  }
  CloseSocket(serverSock);
  return 1;
  }
#endif
}

static int DoMessage(SOCKET sock)
{
  int status;
  Message *msgptr = GetMdsMsg(sock,&status);
  Client *c;
  for (c=ClientList;c && c->sock != sock;c = c->next);
  if (c)
  {
    if (status & 1)
      ProcessMessage(c, msgptr);
    else
      RemoveClient(c);
  }
  if (msgptr)
    free(msgptr);
  RegisterRead(sock);
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

#ifdef HAVE_VXWORKS_H
static int CheckClient(char *host_c, char *user_c)
{
  return 1;
}
  
#else

static int CheckClient(char *host_c, char *user_c)
{
  FILE *f = fopen(hostfile,"r");
  int ok = 0;
  if (f)
  {
    static char line_c[1024];
    static char match_c[1024];
    static struct descriptor line_d = {0, DTYPE_T, CLASS_S, line_c};
    static struct descriptor match_d = {0, DTYPE_T, CLASS_S, match_c};
    static struct descriptor match = {0, DTYPE_T, CLASS_D, 0};
    static struct descriptor local_user = {0,DTYPE_T,CLASS_D,0};
    static struct descriptor access_id = {0,DTYPE_T,CLASS_D,0};
    static DESCRIPTOR(delimiter,"|");
    strncpy(match_c,user_c,255);
    if (host_c)
    {
      strncat(match_c,"@",255);
      strncat(match_c,host_c,255);
    }
    match_d.length = strlen(match_c);
    StrUpcase(&match,&match_d);
    while (ok==0 && fgets(line_c,1023,f))
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
              ok = (multi && host_c) ? 1 : BecomeUser(user_c,&local_user);
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

#endif

static void AddClient(SOCKET sock,struct sockaddr_in *sin,char *dn)
{

  if (sock >= 0)
  {
    static Message m;
    Message *m_user;
    char *timestr;
    char *user = 0;
    char *user_p = 0;
    struct hostent *hp=0;
#ifdef HAVE_VXWORKS_H
    char hostent_buf[512];
#endif
    int i;
    int status;
    int pid;
    int ok = 0;
    Client *c;
    time_t tim;
    int m_status;
    int user_compression_level;
#ifndef HAVE_VXWORKS_H
    pid = getpid();
#else
    pid = taskIdSelf();
#endif
    m.h.msglen = sizeof(MsgHdr);
    m_user = GetMdsMsg(sock,&status);
    if ((status & 1) && (m_user) && (m_user->h.dtype == DTYPE_CSTRING))
    {
      user = malloc(m_user->h.length + 1);
      memcpy(user,m_user->bytes,m_user->h.length);
      user[m_user->h.length] = 0;
    }
    user_p = dn ? dn : (user ? user : "?");
    tim = time(0);
    timestr = ctime(&tim);
    timestr[strlen(timestr)-1] = 0;
#ifdef HAVE_VXWORKS_H
    hp = resolvGetHostByAddr((const char *)&sin->sin_addr, hostent_buf, 512);
#else
    hp = gethostbyaddr((char *)&sin->sin_addr,sizeof(sin->sin_addr),AF_INET);
#endif
#ifndef GLOBUS
    if (hp) ok = CheckClient(hp->h_name,user_p);
    if (ok == 0) ok = CheckClient((char *)inet_ntoa(sin->sin_addr),user_p);
#else
    ok = CheckClient(0,dn);
#endif
    if (ok & 1)
    {
      user_compression_level = m_user->h.status & 0xf;
      if (user_compression_level > MaxCompressionLevel)
        user_compression_level = MaxCompressionLevel;
    }
    m_status = m.h.status = (ok & 1) ? (1 | (user_compression_level << 1)) : 0;
    m.h.client_type = m_user ? m_user->h.client_type : 0; 
    if (m_user)
      free(m_user);
    if ((ok & 1) && mode == 'I' && pid == getpid())
      return;
    SetSocketOptions(sock);
    SendMdsMsg(sock,&m,0);
    if (NO_SPAWN)
    {
      if (m_status && NO_SPAWN)
      {
        Client *new = malloc(sizeof(Client));
        SetFD(sock);
        new->sock = sock;
        new->context.tree = 0;
        new->message_id = 0;
        new->event = 0;
        new->compression_level = user_compression_level;
        for (i=0;i<6;i++)
	      new->tdicontext[i] = NULL;
        for (i=0;i<MAX_ARGS;i++)
          new->descrip[i] = NULL;
#ifndef GLOBUS
        new->addr = *(int *)&sin->sin_addr;
#endif
        new->next = NULL;
        for (c=ClientList; c && c->next; c = c->next);
        if (c) 
          c->next = new;
        else
          ClientList = new;
       }

       RegisterRead(sock);

    }
    else
    {
      pid = SpawnWorker(sock);
    }
#ifndef GLOBUS
    if (hp)
      printf("%s (%d) (pid %d) Connection received from %s@%s [%s]\r\n", timestr,sock, pid, user_p, hp->h_name, inet_ntoa(sin->sin_addr));
    else
      printf("%s (%d) (pid %d) Connection received from %s@%s\r\n", timestr, sock, pid, user_p, inet_ntoa(sin->sin_addr));
#else
    if (hp)
      printf("%s (pid %d) Connection received from: %s [%s]\n%s\r\n",timestr,pid,hp->h_name, inet_ntoa(sin->sin_addr), dn);
    else
      printf("%s (pid %d) Connection received from: %s\n%s\r\n",timestr,pid,inet_ntoa(sin->sin_addr), dn);
#endif
    if (!(m_status & 1))
    {
      printf("Access denied\n");
      CloseSocket(sock);
    }
    else
       printf("Connected\n");
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
  ClearFD(c->sock);
  CloseSocket(c->sock);
  FreeDescriptors(c);
  for (e=c->event; e; e=nexte)
  {
    nexte = e->next;
    /**/
#ifndef HAVE_VXWORKS_H
    MDSEventCan(e->eventid);
#endif
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
  if (ClientList == 0 && !multi) exit(0);
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
      static short lengths[] = {0,0,1,2,4,8,1,2,4,8,4,8,8,16,0};
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
      d->length = message->h.dtype == DTYPE_CSTRING ? message->h.length : lengths[min(message->h.dtype,13)];
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
        d->pointer = d->length ? malloc(d->length) : 0;
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
        case CRAY_IEEE_CLIENT:
          switch (d->dtype)
          {
            case DTYPE_USHORT:
            case DTYPE_ULONG: ConvertBinary(num, 0, message->h.length, message->bytes, d->length, d->pointer); break;
            case DTYPE_SHORT:
            case DTYPE_LONG:  ConvertBinary(num, 1, message->h.length, message->bytes, d->length, d->pointer); break;
            default: memcpy(d->pointer,message->bytes,dbytes); break;
          }
          break;
        case CRAY_CLIENT:
          switch (d->dtype)
          {
            case DTYPE_USHORT:
            case DTYPE_ULONG: ConvertBinary(num, 0, message->h.length, message->bytes, d->length, d->pointer); break;
            case DTYPE_SHORT:
            case DTYPE_LONG:  ConvertBinary(num, 1, message->h.length, message->bytes, d->length, d->pointer); break;
            case DTYPE_FLOAT: ConvertFloat(num, CvtCRAY, (char)message->h.length, message->bytes, 
                                                  CvtIEEE_S, (char)d->length, d->pointer); break;
            case DTYPE_COMPLEX: ConvertFloat(num * 2, CvtCRAY, (char)(message->h.length/2), message->bytes, 
                                                  CvtIEEE_S, (char)(d->length/2), d->pointer); break;
            case DTYPE_DOUBLE: ConvertFloat(num, CvtCRAY, (char)message->h.length, message->bytes, 
                                                  CvtIEEE_T, sizeof(double), d->pointer); break;
            default: memcpy(d->pointer,message->bytes,dbytes); break;
          }
          break;
        default:
          switch (d->dtype)
          {
            case DTYPE_FLOAT:   ConvertFloat(num, CvtVAX_F, (char)message->h.length, message->bytes,
                                                  CvtIEEE_S, sizeof(float), d->pointer); break;
            case DTYPE_COMPLEX: ConvertFloat(num * 2, CvtVAX_F, (char)message->h.length, message->bytes,
                                                  CvtIEEE_S, sizeof(float), d->pointer); break;
            case DTYPE_DOUBLE:  if (CType(c->client_type) == VMSG_CLIENT)
                                   ConvertFloat(num, CvtVAX_G, (char)message->h.length, message->bytes,
                                                  CvtIEEE_T, sizeof(double), d->pointer); 
                                else
                                   ConvertFloat(num, CvtVAX_D, (char)message->h.length, message->bytes,
                                                  CvtIEEE_T, sizeof(double), d->pointer);
                                break;

            case DTYPE_COMPLEX_DOUBLE: if (CType(c->client_type) == VMSG_CLIENT)
                                          ConvertFloat(num * 2, CvtVAX_G, (char)(message->h.length/2), message->bytes,
                                                  CvtIEEE_T, sizeof(double), d->pointer); 
                                       else
                                          ConvertFloat(num * 2, CvtVAX_D, (char)(message->h.length/2), message->bytes,
                                                  CvtIEEE_T, sizeof(double), d->pointer);
                          break;
            default: memcpy(d->pointer,message->bytes,dbytes); break;
          }
      }
      switch (d->dtype)
      {
      case DTYPE_FLOAT: d->dtype = DTYPE_FS; break;
      case DTYPE_COMPLEX: d->dtype = DTYPE_FSC; break;
      case DTYPE_DOUBLE: d->dtype = DTYPE_FT; break;
      case DTYPE_COMPLEX_DOUBLE: d->dtype = DTYPE_FTC; break;
      }
      if (message->h.descriptor_idx == (message->h.nargs - 1))
      {
        ExecuteMessage(c);
      }
    }
  }
  else
  {
#define MDS_IO_OPEN_K   1
#define MDS_IO_CLOSE_K  2
#define MDS_IO_LSEEK_K  3
#define MDS_IO_READ_K   4
#define MDS_IO_WRITE_K  5
#define MDS_IO_LOCK_K   6
#define MDS_IO_EXISTS_K 7
#define MDS_IO_REMOVE_K 8
#define MDS_IO_RENAME_K 9

    switch (message->h.descriptor_idx)
    {
    case MDS_IO_OPEN_K:
      { 
	int fd = open(message->bytes,message->h.dims[1],message->h.dims[2]);
	DESCRIPTOR_LONG(fd_d,0);
        fd_d.pointer = (char *)&fd;
        SendResponse(c, 1, (struct descriptor *)&fd_d);
	break;
      }
    case MDS_IO_CLOSE_K:
      {
	int stat = close(message->h.dims[1]);
	DESCRIPTOR_LONG(stat_d,0);
        stat_d.pointer = (char *)&stat;
        SendResponse(c, 1, (struct descriptor *)&stat_d);
	break;
      }
    case MDS_IO_LSEEK_K:
      {
	_int64 ans = (_int64)lseek(message->h.dims[1],(off_t)*(_int64 *)&message->h.dims[2],message->h.dims[4]);
        struct descriptor ans_d = {8, DTYPE_Q, CLASS_S, 0};
        ans_d.pointer = (char *)&ans;
        SendResponse(c, 1, (struct descriptor *)&ans_d);
	break;
      }
    case MDS_IO_READ_K:
      {
        void *buf = malloc(message->h.dims[2]);
	ssize_t nbytes = read(message->h.dims[1],buf,(size_t)message->h.dims[2]);
        if (nbytes > 0)
	{
          DESCRIPTOR_A(ans_d,1,DTYPE_B,0,0);
          ans_d.pointer=buf;
          ans_d.arsize=nbytes;
          SendResponse(c,1,(struct descriptor *)&ans_d);
        }
        else
	{ 
	  DESCRIPTOR(ans_d,"");
          SendResponse(c,1,(struct descriptor *)&ans_d);
        }
	break;
      }
    case MDS_IO_WRITE_K:
      {
	ssize_t nbytes = write(message->h.dims[1],message->bytes,(size_t)message->h.dims[0]);
        DESCRIPTOR_LONG(ans_d,0);
        ans_d.pointer = (char *)&nbytes;
        SendResponse(c,1,(struct descriptor *)&ans_d);
	break;
      }
    case MDS_IO_LOCK_K:
      {
        int fd = message->h.dims[1];
        int status;
        _int64 offset = *(_int64 *)&(message->h.dims[2]);
        int size = message->h.dims[4];
        int mode = message->h.dims[5];
        DESCRIPTOR_LONG(ans_d,0);
#if defined (_WIN32)
        static int LockStart;
        static int LockSize;
        if (mode > 0)
	{
          LockStart = offset >= 0 ? offset : lseek(fd,0,SEEK_END);
  	  LockSize = size;
  	  status = ( (LockFile((HANDLE)_get_osfhandle(fd), LockStart, 0, LockSize, 0) == 0) && 
                     (GetLastError() != ERROR_LOCK_VIOLATION) ) ? TreeFAILURE : TreeSUCCESS;
        }
        else
	  status = UnlockFile((HANDLE)_get_osfhandle(fd),LockStart, 0, LockSize, 0) == 0 ? TreeFAILURE : TreeSUCCESS;
#elif defined (HAVE_VXWORKS_H)
        status = TreeSUCCESS;
#else
        struct flock flock_info;
        flock_info.l_type = (mode == 0) ? F_UNLCK : ((mode == 1) ? F_RDLCK : F_WRLCK);
        flock_info.l_whence = (mode == 0) ? SEEK_SET : ((offset >= 0) ? SEEK_SET : SEEK_END);
        flock_info.l_start = (mode == 0) ? 0 : ((offset >= 0) ? offset : 0);
        flock_info.l_len = (mode == 0) ? 0 : size;
        status = (fcntl(fd,F_SETLKW, &flock_info) != -1) ? TreeSUCCESS : TreeLOCK_FAILURE;
#endif
        ans_d.pointer = (char *)&status;
        SendResponse(c,1,(struct descriptor *)&ans_d);
	break;
      }
    case MDS_IO_EXISTS_K:
      { 
        struct stat statbuf;
        int status = (stat(message->bytes,&statbuf) == 0);
	DESCRIPTOR(status_d,0);
        status_d.pointer = (char *)&status;
        SendResponse(c, 1, (struct descriptor *)&status_d);
	break;
      }
    case MDS_IO_REMOVE_K:
      { 
        int status = remove(message->bytes);
	DESCRIPTOR(status_d,0);
        status_d.pointer = (char *)&status;
        SendResponse(c, 1, (struct descriptor *)&status_d);
	break;
      }
    case MDS_IO_RENAME_K:
      { 
	DESCRIPTOR(status_d,0);
#ifdef HAVE_VXWORKS_H
        int status = copy(message->bytes,message->bytes+strlen(message->bytes)+1);
        if (status == 0)
	{
          status = remove(message->bytes);
          if (status != 0)
            remove(message->bytes+strlen(message->bytes)+1);
        }
#else
        int status = rename(message->bytes,message->bytes+strlen(message->bytes)+1);
#endif
        status_d.pointer = (char *)&status;
        SendResponse(c, 1, (struct descriptor *)&status_d);
	break;
      }
    }
  }
}

static void ClientEventAst(MdsEventList *e, int data_len, char *data)
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
      m->h.dtype = DTYPE_EVENT_NOTIFY;
      info = (JMdsEventInfo *)m->bytes;
      memcpy(info->data, data, (data_len<12)?data_len:12);
      for(i = data_len; i < 12; i++)
        info->data[i] = 0;
      info->eventid = e->jeventid;
      SetCompressionLevel(c->compression_level);
      SendMdsMsg(c->sock, m, 0);
      free(m);
    }
    else
    {
      Message *m = malloc(sizeof(MsgHdr) + e->info_len);
      m->h.client_type = c->client_type;
      m->h.msglen = sizeof(MsgHdr) + e->info_len;
      m->h.dtype = DTYPE_EVENT_NOTIFY;
      memcpy(e->info->data, data, (data_len<12)?data_len:12); 
      for(i = data_len; i < 12; i++)
        e->info->data[i] = 0;
      memcpy(m->bytes,e->info,e->info_len);
      SetCompressionLevel(c->compression_level);
      SendMdsMsg(c->sock, m, MSG_OOB);
      free(m);
    }
  }
  else
  {
#ifndef HAVE_VXWORKS_H
    MDSEventCan(e->eventid);
#endif
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
#ifndef HAVE_VXWORKS_H
    status = MDSEventAst(evname,(void (*)(void *,int,char *))ClientEventAst,newe,&newe->eventid);
#endif
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
#ifndef HAVE_VXWORKS_H
        MDSEventCan(e->eventid);
#endif
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
    MdsSetClientAddr(c->addr);
    ResetErrors();
    SetCompressionLevel(c->compression_level);
    status = LibCallg(&c->nargs, TdiExecute);
    if (status & 1) status = TdiData(xd,&ans MDS_END_ARG);
    if (!(status & 1)) 
      GetErrorText(status,&ans);
    else if (GetCompressionLevel() != c->compression_level)
    {
      c->compression_level = GetCompressionLevel();
      if (c->compression_level > MaxCompressionLevel)
        c->compression_level = MaxCompressionLevel;
      SetCompressionLevel(c->compression_level);
    }
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
  Message *m = 0;
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
  else if (CType(c->client_type) == CRAY_IEEE_CLIENT)
  {
    switch (d->dtype)
    {
        case DTYPE_USHORT: 
        case DTYPE_SHORT: length = 4; break;
        case DTYPE_ULONG:
        case DTYPE_LONG: length = 8; break;
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
                                               CvtIEEE_S, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_FLOAT; break;
        case DTYPE_FC: ConvertFloat(num * 2, CvtVAX_F, (char)(d->length/2), d->pointer, 
                                               CvtIEEE_S, (char)(m->h.length/2), m->bytes); m->h.dtype = DTYPE_COMPLEX; break;
        case DTYPE_FS: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_FLOAT; break;
        case DTYPE_FSC: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_COMPLEX; break;
        case DTYPE_D: ConvertFloat(num, CvtVAX_D, (char)d->length, d->pointer, 
                                              CvtIEEE_T, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_DOUBLE; break;
        case DTYPE_DC: ConvertFloat(num * 2, CvtVAX_D, (char)(d->length/2), d->pointer, 
                                              CvtIEEE_T, (char)(m->h.length/2), m->bytes); m->h.dtype = DTYPE_COMPLEX_DOUBLE; break;
        case DTYPE_G: ConvertFloat(num, CvtVAX_G, (char)d->length, d->pointer, 
                                              CvtIEEE_T, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_DOUBLE; break;
        case DTYPE_GC: ConvertFloat(num * 2, CvtVAX_G, (char)(d->length/2), d->pointer, 
                                              CvtIEEE_T, (char)(m->h.length/2), m->bytes); m->h.dtype = DTYPE_COMPLEX_DOUBLE; break;
        case DTYPE_FT: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_DOUBLE; break;
        case DTYPE_FTC: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_COMPLEX_DOUBLE; break;
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
    case CRAY_IEEE_CLIENT:
      switch (d->dtype) 
      {
        case DTYPE_USHORT:
        case DTYPE_ULONG: ConvertBinary(num, 0, d->length, d->pointer, m->h.length, m->bytes); break;
        case DTYPE_SHORT:
        case DTYPE_LONG:  ConvertBinary(num, 1, (char)d->length, d->pointer, (char)m->h.length, m->bytes); break;
        case DTYPE_F:  ConvertFloat(num, CvtVAX_F, (char)d->length, d->pointer, 
                                               CvtIEEE_S, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_FLOAT; break;
        case DTYPE_FC: ConvertFloat(num * 2, CvtVAX_F, (char)(d->length/2), d->pointer, 
                                               CvtIEEE_S, (char)(m->h.length/2), m->bytes); m->h.dtype = DTYPE_COMPLEX; break;
        case DTYPE_FS: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_FLOAT; break;
        case DTYPE_FSC: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_COMPLEX; break;
        case DTYPE_D: ConvertFloat(num, CvtVAX_D, (char)d->length, d->pointer, 
                                              CvtIEEE_T, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_DOUBLE; break;
        case DTYPE_DC: ConvertFloat(num * 2, CvtVAX_D, (char)(d->length/2), d->pointer, 
                                              CvtIEEE_T, (char)(m->h.length/2), m->bytes); m->h.dtype = DTYPE_COMPLEX_DOUBLE; break;
        case DTYPE_G: ConvertFloat(num, CvtVAX_G, (char)d->length, d->pointer, 
                                              CvtIEEE_T, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_DOUBLE; break;
        case DTYPE_GC: ConvertFloat(num * 2, CvtVAX_G, (char)(d->length/2), d->pointer, 
                                              CvtIEEE_T, (char)(m->h.length/2), m->bytes); m->h.dtype = DTYPE_COMPLEX_DOUBLE; break;
        case DTYPE_FT: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_DOUBLE; break;
        case DTYPE_FTC: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_COMPLEX_DOUBLE; break;
        default: memcpy(m->bytes,d->pointer,nbytes); break;
      }
      break;
    case VMSG_CLIENT:
      switch (d->dtype)
      {
        case DTYPE_F:  memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_FLOAT; break;
        case DTYPE_FC: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_COMPLEX; break;
        case DTYPE_D: ConvertFloat(num, CvtVAX_D, sizeof(double), d->pointer, 
                                              CvtVAX_G, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_DOUBLE; break;
        case DTYPE_DC: ConvertFloat(num * 2, CvtVAX_D, (char)(d->length/2), d->pointer, 
                                              CvtVAX_G, (char)(m->h.length/2), m->bytes); m->h.dtype = DTYPE_COMPLEX_DOUBLE; break;
        case DTYPE_G: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_DOUBLE; break;
        case DTYPE_GC: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_COMPLEX_DOUBLE; break;
        case DTYPE_FS: ConvertFloat(num, CvtIEEE_S, sizeof(float), d->pointer, 
                                              CvtVAX_G, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_FLOAT; break;
        case DTYPE_FSC: ConvertFloat(num * 2, CvtIEEE_S, sizeof(float), d->pointer, 
                                              CvtVAX_G, (char)(m->h.length/2), m->bytes); m->h.dtype = DTYPE_COMPLEX; break;
        case DTYPE_FT: ConvertFloat(num, CvtIEEE_T, sizeof(double), d->pointer, 
                                              CvtVAX_G, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_DOUBLE; break;
        case DTYPE_FTC: ConvertFloat(num * 2, CvtIEEE_T, sizeof(double), d->pointer, 
                                              CvtVAX_G, (char)(m->h.length/2), m->bytes); m->h.dtype = DTYPE_COMPLEX_DOUBLE; break;
        default: memcpy(m->bytes,d->pointer,nbytes); break;
      }
      break;
    default:
      switch (d->dtype)
      {
        case DTYPE_F:  memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_FLOAT; break;
        case DTYPE_FC: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_COMPLEX; break;
        case DTYPE_D: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_DOUBLE; break;
        case DTYPE_DC: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_COMPLEX_DOUBLE; break;
        case DTYPE_G: ConvertFloat(num, CvtVAX_G, sizeof(double), d->pointer, 
                                              CvtVAX_D, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_DOUBLE; break;
        case DTYPE_GC: ConvertFloat(num * 2, CvtVAX_G, sizeof(double), d->pointer, 
                                              CvtVAX_D, (char)(m->h.length/2), m->bytes); m->h.dtype = DTYPE_COMPLEX_DOUBLE; break;
        case DTYPE_FS: ConvertFloat(num, CvtIEEE_S, sizeof(float), d->pointer, 
                                              CvtVAX_F, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_FLOAT; break;
        case DTYPE_FSC: ConvertFloat(num * 2, CvtIEEE_S, sizeof(float), d->pointer, 
                                              CvtVAX_F, (char)(m->h.length/2), m->bytes); m->h.dtype = DTYPE_COMPLEX; break;
        case DTYPE_FT: ConvertFloat(num, CvtIEEE_T, sizeof(double), d->pointer, 
                                              CvtVAX_D, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_DOUBLE; break;
        case DTYPE_FTC: ConvertFloat(num * 2, CvtIEEE_T, sizeof(double), d->pointer, 
                                              CvtVAX_D, (char)(m->h.length/2), m->bytes); m->h.dtype = DTYPE_COMPLEX_DOUBLE; break;
        default: memcpy(m->bytes,d->pointer,nbytes); break;
      }
      break;
  }
  SendMdsMsg(c->sock, m, 0);
  free(m);
}

static int CreateMdsPort(short port, int multi_in)
{
  return CreateListener(port,AddClient,DoMessage);
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

#ifndef HAVE_VXWORKS_H
static short ParsePort(char *name, char **portname)
{
  short port;
  struct servent *sp;
  port = htons((short)atoi(name));
  if (port == 0)
  {
    sp = getservbyname(name,"tcp");
    if (sp == NULL)
    {
      printf("unknown service: %s/tcp\n\n",name);
      PrintHelp(0);
    }
    port = sp->s_port;
  }
  sp = getservbyport(port,"tcp");
  *portname = strcpy((char *)malloc(strlen(sp ? sp->s_name : name)+1),sp ? sp->s_name : name);
  return port;
}
#endif

static void PrintHelp(char *option)
{
  if (option)
    printf("Invalid options specified: %s\n\n",option);
  printf("mdsip - MDSplus data server\n\n");
  printf("  Format:\n\n");
  printf("    mdsip -p port|service [-m|-s] [-i|-r] [-h hostfile] [-c|+c]\n\n");
  printf("      -p port|service    Specifies port number or tcp service name\n");
  printf("      -m                 Use multi mode (accepts multiple connections each with own context)\n");
  printf("      -s                 Use server mode (accepts multiple connections with shared context)\n");
  printf("      -I                 Use inetd mode (accepts multiple connections spawning a process to handle each connection)\n");
  printf("      -i                 Install service (WINNT only)\n");
  printf("      -r                 Remove service (WINNT only)\n");
  printf("      -h hostfile        Specify alternate mdsip.hosts file\n");
  printf("      -c [level]         Specify maximum zlip compression level, 0=nocompression,1-9 fastest/least to slowest/most, 0-default\n");
  printf("      +c                 Allow compression up to maximum level 9\n\n");
  printf("    more verbose switches are also permitted:\n\n");
  printf("      --port port|service\n");
  printf("      --multi\n");
  printf("      --server\n");
  printf("      --install\n");
  printf("      --remove\n");
  printf("      --hostfile hostfile\n");
  printf("      --cert certificatefile (globus only)\n");
  printf("      --key  keyfile (globus only)\n");
  printf("      --nocompression\n");
  printf("      --compression [level,default=9]\n\n");
  printf("  Deprecated Format:\n\n    mdsip port|service [multi|server|install|install_server|install_multi|remove] [hostfile]\n");
  exit(1);
} 

#ifndef HAVE_VXWORKS_H
static int ParseOption(char *option, char **argv, int argc, char **portname, short *port, char **hostfile, char *mode, int *flags, int *compression_level)
{
  int increment = 0;
  if (strcmp(option,"port") == 0)
  {
    if (argc > 0)
    {
      *port = ParsePort(argv[0],portname);
      increment = 1;
    }
    else
    {
      printf("No port or service specified\n\n");
      PrintHelp(0);
    }
  }
  else if (strcmp(option,"multi") == 0)
  {
    SetMode('m',mode);
  }
  else if (strcmp(option,"server") == 0)
  {
    SetMode('s',mode);
  }
  else if (strcmp(option,"install") == 0)
  {
    SetMode('i',mode);
  }
  else if (strcmp(option,"remove") == 0)
  {
    SetMode('r',mode);
  }
  else if (strcmp(option,"hostfile") == 0)
  {
    if (argc > 0)
    {
      *hostfile = strcpy((char *)malloc(strlen(argv[0])+1),argv[0]);
      increment = 1;
    }
    else
    {
      printf("hostfile not specified\n\n");
      PrintHelp(0);
    }
  }
  else if (strcmp(option,"nocompression") == 0)
  {
    *compression_level = 0;
  }
  else if (strcmp(option,"compression") == 0)
  {
    if (argc > 0 && *argv[0] >= '0' && *argv[0] <= '9') *compression_level = atoi(argv[0]); else *compression_level = 9;
  }
  else if (strcmp(option,"cert") == 0)
  {
    if (argc > 0)
    {
      char *cmd = malloc(50+strlen(argv[0]));
      strcpy(cmd,"X509_USER_CERT=");
      strcat(cmd,argv[0]);
      putenv(cmd);
      increment = 1;
    }
    else
    {
      printf("cert file node specified\n\n");
      PrintHelp(0);
    }
  }
  else if (strcmp(option,"key") == 0)
  {
    if (argc > 0)
    {
      char *cmd = malloc(50+strlen(argv[0]));
      strcpy(cmd,"X509_USER_KEY=");
      strcat(cmd,argv[0]);
      putenv(cmd);
      increment = 1;
    }
    else
    {
      printf("cert file node specified\n\n");
      PrintHelp(0);
    }
  }
  else
  {
    printf("Invalid option specified: --%s\n\n",option);
    PrintHelp(0);
  }
  return increment;
}

#endif

static void SetMode(char modein,char *mode)
{
  int multiple = 0;
  switch (modein)
  {
  case 'I': if (*mode == 0)
              *mode = 'I';
            else
              multiple = 1;
            break;
  case 'm': if (*mode == 0)
              *mode = 'm';
            else if (*mode == 'i')
              *mode = 'M';
            else
              multiple = 1;
            break;
  case 's': if (*mode == 0)
              *mode = 's';
            else if (*mode == 'i')
              *mode = 'S';
            else
              multiple = 1;
            break;
  case 'i': if (*mode == 0)
              *mode = 'i';
            else if (*mode == 's')
              *mode = 'S';
            else if (*mode == 'm')
              *mode = 'M';
            else
              multiple = 1;
            break;
  case 'r': if ((*mode == 0) || (*mode == 's') || (*mode == 'm'))
              *mode = 'r';
            else
              multiple = 1;
            break;
  }
  if (multiple)
  {
    printf("Multiple or incompatible modes selected\n\n");
    PrintHelp(0);
  }
}

#ifndef HAVE_VXWORKS_H
static void ParseCommand(int argc, char **argv,char **portname, short *port, char **hostfile, char *mode, int *flags, int *compression_level)
{
  int i;
  if (argc < 2) PrintHelp(0);
  *port = 0;
  *mode = 0;
  *flags = 0;
  *hostfile = 0;
  for (i=1;i<argc;i++)
  {
    char *option = argv[i];
    switch (option[0])
    {
      case '-':
        if (strlen(option) > 2 && option[1] != '-')
	{
          printf("Invalid option specified: %s\n\n",option);
          PrintHelp(0);
        }
        switch ( option[1] )
	{
	  case 'p': if (argc > (i+1)) *port = ParsePort(argv[++i],portname); else PrintHelp(0); break;
          case 'I': SetMode('I',mode); break;
	  case 'm': SetMode('m',mode); break;
	  case 's': SetMode('s',mode); break;
	  case 'i': SetMode('i',mode); break;
          case 'r': SetMode('r',mode); break;
	  case 'h': if (argc > (i+1)) {i++; *hostfile = strcpy((char *)malloc(strlen(argv[i])+1),argv[i]);} else PrintHelp(0); break;
          case 'c': if (argc > (i+1) && *argv[i+1] >= '0' && *argv[i+1] <= '9') {i++; *compression_level = atoi(argv[i]);} else *compression_level=0; break;
	  case '-': i += ParseOption(&option[2],&argv[i+1],argc-i-1,portname,port,hostfile,mode,flags,compression_level); break;
#ifdef HAVE_WINDOWS_H
          case 'F': *flags = atoi(argv[++i]); break;
          case 'W': i += 2; *mode='w'; break;
#endif
	  default: PrintHelp(option); break;
        }
        break;
      case '+':
        if (strlen(option) != 2)
        {
          printf("Invalid option specified: %s\n\n",option);
          PrintHelp(0);
        }
        switch ( option[1] )
	{
	  case 'c': *compression_level=9; break;
          default: PrintHelp(option); break;
        }
        break;
      default:
        if (*port == 0)
          *port = ParsePort(option,portname);
        else if (strcmp(argv[i],"multi") == 0)
          SetMode('m',mode);
        else if (strcmp(argv[i],"install") == 0)
          SetMode('i',mode);
        else if (strcmp(argv[i],"install_server") == 0)
        {
          SetMode('s',mode);
          SetMode('i',mode);
        }
        else if (strcmp(argv[i],"install_multi") == 0)
        {
          SetMode('m',mode);
          SetMode('i',mode);
        }
        else if (strcmp(argv[i],"remove") == 0)
          SetMode('r',mode);
        else if (strcmp(argv[i],"server") == 0)
          SetMode('s',mode);
        else
          *hostfile = strcpy((char *)malloc(strlen(option)+1),option);
        break;
    }
  }
  if (*compression_level < 0)
    *compression_level = 0;
  else if (*compression_level > 9)
    *compression_level = 9;
  if (*hostfile == 0) *hostfile = strcpy((char *)malloc(strlen(DEFAULT_HOSTFILE)+1),DEFAULT_HOSTFILE);
  if (*port == 0)
  {
    printf("port must be specified\n\n");
    PrintHelp(0);
  }
}
#endif
