#include "mdsip.h"
extern int  GetAnswerInfoTS(SOCKET sock, char *dtype, short *length, char *ndims, int *dims, int *numbytes, void * *dptr, void **m);

#if defined(__VMS) || defined(WIN32)
#define BlockSig(arg)
#define UnBlockSig(arg) 
#else
static int BlockSig(int sig_number)
{
  sigset_t newsigset;
#if defined(sun)
  if (sig_number == SIGALRM)
    {				/* Solaris: simple block doesn't work?	*/
     struct sigaction  act;
     sigaction(sig_number,NULL,&act);	/* get current state ...	*/
     if (~act.sa_flags & SA_RESTART)
        {				/*...set SA_RESTART bit		*/
         act.sa_flags |= SA_RESTART;
         if (sigaction(sig_number,&act,NULL))
             perror("BlockSig *err* sigaction");
        }
    }
#endif
  sigemptyset(&newsigset);
  sigaddset(&newsigset,sig_number);
  return sigprocmask(SIG_BLOCK, &newsigset, NULL);
}

static int UnBlockSig(int sig_number)
{
  sigset_t newsigset;
  sigemptyset(&newsigset);
  sigaddset(&newsigset,sig_number);
  return sigprocmask(SIG_UNBLOCK, &newsigset, NULL);
}
#endif

#ifdef __MWERKS__
// Start of Mac Changes
static short bGUSIInit = 0;

static void BlockSig ( int ) {
	if ( !bGUSIInit ) {
	//	GUSISetup ( GUSIwithInternetSockets );
		GUSISetupConfig ();
		bGUSIInit = 1;
		}
	}
static void UnBlockSig ( int ) {}

void main () {}

// End of Mac Changes
#endif

extern short ArgLen();
extern int Lgihpwd();
extern Message *GetMdsMsg();
extern Message *GetMdsMsgOOB();
extern int SetCompressionLevel(int level);

#define __toupper(c) (((c) >= 'a' && (c) <= 'z') ? (c) & 0xDF : (c))

#ifdef _USE_VARARGS
int MdsValue(va_alist) va_dcl
{
  SOCKET sock;
  char *expression;
#else
int MdsValue(SOCKET sock, char *expression, ...)  /**** NOTE: NULL terminated argument list expected ****/
{
#endif
  va_list incrmtr;
  int a_count;
  int i;
  unsigned char nargs;
  unsigned char idx;
  int status = 1;
  struct descrip exparg;
  struct descrip *arg = &exparg;
#ifdef _USE_VARARGS
  va_start(incrmtr);
  sock = va_arg(incrmtr, SOCKET);
  expression = va_arg(incrmtr, char *);
#else
  va_start(incrmtr, expression);
#endif
  for (a_count = 1; arg != NULL; a_count++)
  {
    arg = va_arg(incrmtr, struct descrip *);
  }
#ifdef _USE_VARARGS
  va_start(incrmtr);
  sock = va_arg(incrmtr, SOCKET);
  expression = va_arg(incrmtr, char *);
#else
  va_start(incrmtr, expression);
#endif
  nargs = a_count - 2;
  arg = MakeDescrip((struct descrip *)&exparg,DTYPE_CSTRING,0,0,expression);
  for (i=1;i<a_count-1 && (status & 1);i++)
  {
    idx = i - 1;
    status = SendArg(sock, idx, arg->dtype, nargs, ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
    arg = va_arg(incrmtr, struct descrip *);
  }
  va_end(incrmtr);
  if (status & 1)
  {
    short len;
    int numbytes;
    void *dptr;
    void *mem = 0;
    status = GetAnswerInfoTS(sock, &arg->dtype, &len, &arg->ndims, arg->dims, &numbytes, &dptr, &mem);
    arg->length = len;
    if (numbytes)
    {
      if (arg->dtype == DTYPE_CSTRING)
      {
        arg->ptr = malloc(numbytes+1);
        ((char *)arg->ptr)[numbytes] = 0;
      }
      else if (numbytes > 0)
        arg->ptr = malloc(numbytes);
      if (numbytes > 0)
        memcpy(arg->ptr,dptr,numbytes);
    }
    else
      arg->ptr = NULL;
    if (mem) free(mem);
  }
  else
    arg->ptr = NULL;
  return status;
}

#ifdef _USE_VARARGS
int MdsPut(va_alist) va_dcl
{
  SOCKET sock;
  char *node;
  char *expression;
#else
int MdsPut(SOCKET sock, char *node, char *expression, ...)  /**** NOTE: NULL terminated argument list expected ****/
{
#endif
  va_list incrmtr;
  int a_count;
  int i;
  unsigned char nargs;
  unsigned char idx = 0;
  int status = 1;
  static char *putexpprefix = "TreePut(";
  static char *argplace = "$,";
  char *putexp;
  struct descrip putexparg;
  struct descrip exparg;
  struct descrip *arg;
#ifdef _USE_VARARGS
  va_start(incrmtr);
  sock = va_arg(incrmtr, SOCKET);
  node = va_arg(incrmtr, char *);
  expression = va_arg(incrmtr, char *);
#else
  va_start(incrmtr, expression);
#endif
  for (a_count = 3; va_arg(incrmtr, struct descrip *); a_count++);
  putexp = malloc(strlen(putexpprefix) + (a_count - 1) * strlen(argplace) + 1);
  strcpy(putexp,putexpprefix);
  for (i=0;i<a_count - 1;i++) strcat(putexp,argplace);
  putexp[strlen(putexp)-1] = ')';
#ifdef _USE_VARARGS
  va_start(incrmtr);
  sock = va_arg(incrmtr, SOCKET);
  node = va_arg(incrmtr, char *);
  expression = va_arg(incrmtr, char *);
#else
  va_start(incrmtr, expression);
#endif
  nargs = a_count;
  arg = MakeDescrip(&putexparg,DTYPE_CSTRING,0,0,putexp);
  status = SendArg(sock, idx++, arg->dtype, nargs, ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
  free(putexp);
  arg = MakeDescrip(&exparg,DTYPE_CSTRING,0,0,node);
  status = SendArg(sock, idx++, arg->dtype, nargs, ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
  arg = MakeDescrip(&exparg,DTYPE_CSTRING,0,0,expression);
  for (i=idx;i<a_count && (status & 1);i++)
  {
    status = SendArg(sock, (char)i, arg->dtype, nargs, ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
    arg = va_arg(incrmtr, struct descrip *);
  }
  va_end(incrmtr);
  if (status & 1)
  {
    char dtype;
    int dims[MAX_DIMS];
    char ndims;
    short len;
    int numbytes;
    void *dptr;
    void *mem = 0;
    status = GetAnswerInfoTS(sock, &dtype, &len, &ndims, dims, &numbytes, &dptr, &mem);
    if (status & 1 && dtype == DTYPE_LONG && ndims == 0 && numbytes == sizeof(int))
      memcpy(&status,dptr,numbytes);
    if (mem) free(mem);
  }
  return status;
}

int  MdsOpen(SOCKET sock, char *tree, int shot)
{
  struct descrip treearg;
  struct descrip shotarg;
  struct descrip ansarg;
  static char *expression = "TreeOpen($,$)";
  int status = MdsValue(sock, expression, MakeDescrip((struct descrip *)&treearg,DTYPE_CSTRING,0,0,tree), 
			      MakeDescrip((struct descrip *)&shotarg,DTYPE_LONG,0,0,&shot),
			      (struct descrip *)&ansarg, (struct descrip *)NULL);
  if ((status & 1) && (ansarg.dtype == DTYPE_LONG)) status = *(int *)ansarg.ptr;
  if (ansarg.ptr) free(ansarg.ptr);
  return status;
}

int  MdsClose(SOCKET sock)
{
  struct descrip ansarg;
  static char *expression = "TreeClose()";
  int status = MdsValue(sock, expression, &ansarg, NULL);
  if ((status & 1) && (ansarg.dtype == DTYPE_LONG)) status = *(int *)ansarg.ptr;
  if (ansarg.ptr) free(ansarg.ptr);
  return status;
}

int  MdsSetDefault(SOCKET sock, char *node)
{
  struct descrip nodearg;
  struct descrip ansarg;
  static char *expression = "TreeSetDefault($)";
  int status = MdsValue(sock, expression, MakeDescrip(&nodearg,DTYPE_CSTRING,0,0,node), &ansarg, NULL);
  if ((status & 1) && (ansarg.dtype == DTYPE_LONG)) status = *(int *)ansarg.ptr;
  if (ansarg.ptr) free(ansarg.ptr);
  return status;
}

static int MdsLoginVMS(SOCKET sock, char *username, char *password)
{
  struct descrip loginget_arg;
  struct descrip loginpwd_arg;
  struct descrip pwd_arg;
  struct descrip ansarg;
  int status;
  ansarg.ptr = 0;
  status = MdsValue(sock, LOGINREQUEST, MakeDescrip(&loginget_arg,DTYPE_CSTRING,0,0,LOGINGETP1), &ansarg, NULL);
  if (status & 1)
  {
    if (ansarg.ptr && ansarg.dtype == DTYPE_CHAR)
    {
      char alg = *(char *)ansarg.ptr;
      free(ansarg.ptr);
      status = MdsValue(sock, LOGINREQUEST, MakeDescrip(&loginget_arg,DTYPE_CSTRING,0,0,LOGINGETP2), &ansarg, NULL);
      if (status & 1)
      {
        if (ansarg.ptr && ansarg.dtype == DTYPE_SHORT)
        {
          struct dscr { short length; char dtype; char class; void *pointer;};
          int hash[2];
          int i;
          struct dscr hashd = {8, 9, 1, 0};
          struct dscr pwdd = {0,14,1,0};
          struct dscr userd = {0,14,1,0};
          short salt = *(short *)ansarg.ptr;
          free(ansarg.ptr);
          ansarg.ptr = 0;
          hashd.pointer = hash;
          pwdd.length = strlen(password);
          pwdd.pointer = strcpy((char *)malloc(pwdd.length+1),password);
          userd.length = strlen(username);
          userd.pointer = strcpy((char *)malloc(userd.length+1),username);
          for (i=0;i<pwdd.length;i++)
            ((char *)pwdd.pointer)[i] = __toupper(((char *)pwdd.pointer)[i]);
          for (i=0;i<userd.length;i++)
            ((char *)userd.pointer)[i] = __toupper(((char *)userd.pointer)[i]);
          status = Lgihpwd(&hashd,&pwdd,alg,salt,&userd);
          if (status & 1)
          {
            int two = 2;
            status = MdsValue(sock, LOGINREQUEST, MakeDescrip(&loginpwd_arg,DTYPE_CSTRING,0,0,LOGINPWD),
                                                  MakeDescrip(&pwd_arg,DTYPE_LONG,1,&two,hash), &ansarg, NULL);
          }
          else
            printf("Login error: Error checking password\n");
        }
        else
        {
          if (ansarg.ptr)
          {
            free(ansarg.ptr);
            ansarg.ptr = 0;
          }
          printf("Login error: Unrecognized response from server\n");
          status = 0;
        }
      }
    }
    else
    {
      if (ansarg.ptr)
      {
        free(ansarg.ptr);
        ansarg.ptr = 0;
      }
      printf("Login error: Unrecognized response from server\n");
      status = 0;
    }
  }
  if (!(status & 1) && ansarg.ptr) printf("Login error: %s\n",(char *)ansarg.ptr);
  if (ansarg.ptr) free(ansarg.ptr);
  return status;
}

int MdsLogin(SOCKET sock, char *username, char *password)
{
  struct descrip loginuser_arg;
  struct descrip userarg;
  struct descrip ansarg;
  int status;
  ansarg.ptr = 0;
  status = MdsValue(sock, LOGINREQUEST, MakeDescrip(&loginuser_arg,DTYPE_CSTRING,0,0,LOGINUSER),
                                        MakeDescrip(&userarg,DTYPE_CSTRING,0,0,username), &ansarg, NULL);
  if (status & 1)
  {
    int typelen = strlen(LOGINVMS);
    if (ansarg.dtype == DTYPE_CSTRING && ansarg.length == typelen && ansarg.ptr)
    {
      if (strncmp(ansarg.ptr,LOGINVMS,typelen)==0)
      {
        free(ansarg.ptr);
        ansarg.ptr = 0;
        status = MdsLoginVMS(sock, username, password);
      }
      else
      {
        free(ansarg.ptr);
        ansarg.ptr = 0;
        printf("Login error: Unrecognized response from server\n");
        status = 0;
      }
    }
    else
    {
      if (ansarg.ptr)
      {
        free(ansarg.ptr);
        ansarg.ptr = 0;
      }
      printf("Login error: Unrecognized response from server\n");
      status = 0;
    }
  }
  if (!(status & 1) && ansarg.ptr) printf("Login error: %s\n",(char *)ansarg.ptr);
  if (ansarg.ptr) free(ansarg.ptr);
  return status;
}

#ifdef _WIN32
typedef struct _EventThread {
	SOCKET sock;
	HANDLE thread_handle;
	DWORD  thread_id;
	int    eventid;
	int    event_count;
	struct _EventThread *next;
} EventThread;

static EventThread *eThreads = NULL;

unsigned long WINAPI MdsDispatchEvent(SOCKET sock);
#endif

int  MdsEventAst(SOCKET sock, char *eventnam, void (*astadr)(), void *astprm, int *eventid)
{
#ifdef _WIN32
  EventThread *et;
#endif

  struct descrip eventnamarg;
  struct descrip infoarg;
  struct descrip ansarg;
  MdsEventInfo info;
  int size = sizeof(info);
  int status;
#ifdef _WIN32
  for (et=eThreads; et && et->sock != sock; et = et->next);
  if (et == NULL) {
	  et =  (EventThread *)malloc(sizeof(EventThread));
      et->sock = sock;
	  et->event_count = 0;
	  et->next = eThreads;
	  et->thread_handle = NULL;
	  eThreads = et;
  }
#endif
  info.astadr = (void (*)(void *,int ,char *))astadr;
  info.astprm = astprm;
  status = MdsValue(sock, EVENTASTREQUEST, MakeDescrip((struct descrip *)&eventnamarg,DTYPE_CSTRING,0,0,eventnam), 
			      MakeDescrip((struct descrip *)&infoarg,DTYPE_UCHAR,1,&size,&info),
			      (struct descrip *)&ansarg, (struct descrip *)NULL);
  if ((status & 1) && (ansarg.dtype == DTYPE_LONG)) {
    *eventid = *(int *)ansarg.ptr;
/*#ifdef _WIN32
    et->eventid = *(int *)ansarg.ptr;
    if (et->thread_handle == NULL)
    et->thread_handle = CreateThread((LPSECURITY_ATTRIBUTES)NULL, 0, (LPTHREAD_START_ROUTINE)MdsDispatchEvent, (LPVOID)sock, (DWORD)NULL, &et->thread_id);
    et->event_count++;
#endif
*/  }
  if (ansarg.ptr) free(ansarg.ptr);
  return status;
}

int  MdsEventCan(SOCKET sock, int eventid)
{
#ifdef _WIN32
  EventThread *et, *prev;
#endif
  struct descrip eventarg;
  struct descrip ansarg = {0,0,0,0};
  int status = MdsValue(sock, EVENTCANREQUEST, MakeDescrip((struct descrip *)&eventarg,DTYPE_LONG,0,0,&eventid), 
			      (struct descrip *)&ansarg, (struct descrip *)NULL);
  if (ansarg.ptr) free(ansarg.ptr);
#ifdef _WIN32
  for (prev=NULL,et=eThreads; et && (et->sock != sock); prev = et,et = et->next);
  if (et) {
	et->event_count--;
	if (et->event_count == 0)
		TerminateThread(et->thread_handle, 0);
      if (prev)
		  prev->next = et->next;
	  else
		  eThreads = et->next;
  }
#endif
  return status;
}

#ifndef _WIN32
void MdsDispatchEvent(SOCKET sock)
#else
unsigned long WINAPI MdsDispatchEvent(SOCKET sock)
#endif
{
  int status;
  Message  *m;
#ifndef _WIN32
  if ((m = GetMdsMsg(sock,&status)) != 0)
#else
  while ((m = GetMdsMsgOOB(sock,&status)) != 0)
#endif
  {
    if (status == 1 && m->h.msglen == (sizeof(MsgHdr) + sizeof(MdsEventInfo)))
    {
      MdsEventInfo *event = (MdsEventInfo *)m->bytes;
      (*event->astadr)(event->astprm, event->eventid, event->data);
    }
    free(m);
#ifdef MULTINET
    sys$qiow(0,sock,IO$_SETMODE | IO$M_ATTNAST,0,0,0,MdsDispatchEvent,sock,0,0,0,0);
#endif
  }
#ifdef _WIN32
  return 0;
#endif
}

int MdsSetCompression(SOCKET sock, int level)
{
  int old_level;
  int status;
  if (level < 0)
    level = 0;
  else if (level > 9)
    level = 9;
  old_level = SetCompressionLevel(level);
  if (sock != INVALID_SOCKET) 
  {
    char expression[128];
    struct descrip ans;
    sprintf(expression,"MdsSetCompression(%d)",level);
    status = MdsValue(sock,expression,&ans,0);
    if (ans.ptr != 0) free(ans.ptr);
  }
  return old_level;
}

static char *ServerPortname = 0;
static int ClientAddr = 0;
void MdsSetServerPortname(char *portname){  ServerPortname = portname; }
void MdsSetClientAddr(int addr) { ClientAddr = addr; }
char *MdsGetServerPortname() {return ServerPortname;}
int MdsGetClientAddr() {return ClientAddr;}

#ifndef HAVE_VXWORKS_H

int  IdlMdsClose(int lArgc, void * * lpvArgv)
{
/*  status = call_external('mdsipshr','IdlMdsClose', socket, value=[1b])
*/
  int status;
  BlockSig(SIGALRM);
  status = MdsClose((SOCKET)lpvArgv[0]);
  UnBlockSig(SIGALRM);
  return status;
}

int  IdlConnectToMds(int lArgc, void * * lpvArgv)
{
/*  status = call_external('mdsipshr','IdlConnectToMds', 'host-name')
*/
  int status;
  BlockSig(SIGALRM);
  status = ConnectToMds((char *)lpvArgv[0]);
  UnBlockSig(SIGALRM);
  return status;
}

int  IdlDisconnectFromMds(int lArgc, void * * lpvArgv)
{
/*  status = call_external('mdsipshr','IdlDisconnectFromMds', socket, value=[1b])
*/
  int status;
  BlockSig(SIGALRM);
  status = DisconnectFromMds((SOCKET)lpvArgv[0]);
  UnBlockSig(SIGALRM);
  return status;
}

int  IdlMdsOpen(int lArgc, void * * lpvArgv)
{
/*  status = call_external('mdsipshr','IdlMdsOpen', sock, 'tree-name', shot, value = [1b,0b,1b]) 
*/
  int status;
  BlockSig(SIGALRM);
  status = MdsOpen((int)lpvArgv[0],(char *)lpvArgv[1],(int)lpvArgv[2]);
  UnBlockSig(SIGALRM);
  return status;
}

int  IdlMdsSetDefault(int lArgc, void * * lpvArgv)
{
/*  status = call_external('mdsipshr','IdlMdsSetDefault', sock, 'node', value = [1b,0b]) 
*/
  int status;
  BlockSig(SIGALRM);
  status = MdsSetDefault((int)lpvArgv[0],(char *)lpvArgv[1]);
  UnBlockSig(SIGALRM);
  return status;
}

int  IdlMdsLogin(int lArgc, void * * lpvArgv)
{
/*  status = call_external('mdsipshr','IdlMdsLogin', sock, 'user', 'passwd', value = [1b,0b,0b]) 
*/
  int status;
  BlockSig(SIGALRM);
  status = MdsLogin((int)lpvArgv[0],(char *)lpvArgv[1],(char *)lpvArgv[2]);
  UnBlockSig(SIGALRM);
  return status;
}

int  IdlGetAnsInfo(int lArgc, void * * lpvArgv)
{
/*  status = call_external('mdsipshr','IdlGetAnsInfo', socket_l, dtype_b, length_w, ndims_b, dims_l[7], numbytes_l, 
                               value=[1b,0b,0b,0b,0b,0b,0b])
*/
  int status;
  BlockSig(SIGALRM);
  status = GetAnswerInfo((SOCKET)lpvArgv[0], (char *)lpvArgv[1], (short *)lpvArgv[2], (char *)lpvArgv[3],
                       (int *)lpvArgv[4], (int *)lpvArgv[5], (void **)lpvArgv[6]);
  UnBlockSig(SIGALRM);
  return status;
}

int  Idlmemcpy(int lArgc, void * * lpvArgv)
{
/*  status = call_external('mdsipshr','Idlmemcpy', answer, answer_ptr, nbytes, value=[0b,1b,1b])
*/
#ifdef __alpha
  memcpy((void *)lpvArgv[0],*(void **)lpvArgv[1], (int)lpvArgv[2]);
#else
  memcpy((void *)lpvArgv[0],(void *)lpvArgv[1], (int)lpvArgv[2]);
#endif
  return 1;
}

int  IdlSendArg(int lArgc, void * * lpvArgv)
{
/*  status = call_external('mdsipshr','IdlSendArg', sock_l, idx_l, dtype_b, nargs_w, length_w, ndims_b, dims_l[7], 
			    bytes, value=[1b,1b,1b,1b,1b,1b,1b,0b,0b])
*/
  unsigned char idx    = (unsigned char)(int)lpvArgv[1];
  unsigned char dtype  = (unsigned char)(int)lpvArgv[2];
  unsigned char nargs  = (unsigned char)(int)lpvArgv[3];
  short         length = (short)(int)lpvArgv[4];
  char          ndims  = (char)(int)lpvArgv[5];
  int status;
  BlockSig(SIGALRM);
  status = SendArg((SOCKET)lpvArgv[0], idx, dtype, nargs, length, ndims, (int *)lpvArgv[6], (char *)lpvArgv[7]);
  UnBlockSig(SIGALRM);
  return status;
}

int IdlSetCompressionLevel(int lArgc, void * * lpvArgv)
{
/*  status = call_external('mdsipshr','IdlSetCompressionLevel', sock_l, level_l, value=[1b,1b])
*/
  return MdsSetCompression((SOCKET)lpvArgv[0],(int)lpvArgv[1]);
}

#endif //HAVE_VXWORKS_H
