#include <config.h>

#define _GNU_SOURCE /* glibc2 needs this */
#ifdef __sparc__
#include "/usr/include/sys/types.h"
#elif !defined(HAVE_WINDOWS_H)
#include <sys/types.h>
#endif
#ifdef HAVE_VXWORKS_H
int MDSEventAst(char *eventnam, void (*astadr)(), void *astprm, int *eventid) {}
int MDSEventCan(void *eventid) {}
int MDSEvent(char *evname){}
#elif (defined(HAVE_WINDOWS_H))
#include <process.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <libroutines.h>
#include <mds_stdarg.h>
#include "../mdstcpip/mdsip.h"
extern char *TranslateLogical(char *);
static int eventAstRemote(char *eventnam, void (*astadr)(), void *astprm, int *eventid);
static void initializeLocalRemote(int receive_events, int *use_local);
static void newRemoteId(int *id);
static void setRemoteId(int id, int ofs, int evid);
static int sendRemoteEvent(char *evname, int data_len, char *data);
static int getRemoteId(int id, int ofs);
static int remote_local_initialized = 0;
static SOCKET receive_sockets[256];  	/* Socket to receive external events  */
static SOCKET send_sockets[256];  		/* Socket to send external events  */
static char *receive_servers[256];	/* Receive server names */
static char *send_servers[256];		/* Send server names */
static HANDLE external_thread = 0;
static HANDLE external_event = 0;
static HANDLE thread_alive_event = 0;
#define MAX_ACTIVE_EVENTS 5000   /* Maximum number events concurrently dealt with by processes */

static int external_shutdown = 0;
static int external_count = 0;          /* remote event pendings count */
static int num_receive_servers = 0;	/* numer of external event sources */
static int num_send_servers = 0;	/* numer of external event destination */
static unsigned int threadID;
static int zero = 0;

struct event_struct { char *eventnam;
                      void (*astadr)(void *,int,char *);
                      void *astprm;
					  int len;
					  char *data;
					  unsigned long thread;
					  HANDLE event;
};
static void EventActionProc(struct event_struct *event)
{
		(*event->astadr)(event->astprm,event->len,event->data);
}

static void EventThreadProc(struct event_struct *event)
{
	/*
	UINT msgnum = RegisterWindowMessage("MDSplus Events");
	MSG message;
	int status;
	HWND hWnd = GetTopWindow(NULL);
	status = PeekMessage(&message,hWnd,msgnum,msgnum,PM_NOREMOVE);
	while( (status = GetMessage(&message,hWnd,msgnum,msgnum)) != -1)
	{
    	_beginthread((void (*)(void *))EventActionProc,0,(void *)event);
	}
*/
	/*
	char pipename[256];
	HANDLE pipe = INVALID_HANDLE_VALUE;
	int status = ERROR_PIPE_BUSY;
	char data[256];
	int num;
	for (num=0;(pipe == INVALID_HANDLE_VALUE) && (status == ERROR_PIPE_BUSY);num++)
	{
	  sprintf(pipename,"\\\\.\\pipe\\mdsevents\\%s\\%0x",event->eventnam,num);
	  pipe = CreateNamedPipe(pipename,PIPE_ACCESS_INBOUND,PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,1,512,512,1000,0);
	  if (pipe == INVALID_HANDLE_VALUE)
	  {
	    status = GetLastError();
	    printf("Error creating pipe - %d\n",status);
	  }
	}
	if (pipe != INVALID_HANDLE_VALUE)
	{
		while ((ConnectNamedPipe(pipe,0) != 0) && ((status = ReadFile(pipe,data,sizeof(data),&event->len,0)) != 0))
	  {
		event->data= memcpy(malloc(event->len),data,event->len);
        _beginthread((void (*)(void *))EventActionProc,0,(void *)event);
	  }
	}
	*/


	while ( WaitForSingleObject(event->event,INFINITE) != WAIT_FAILED)
	{
		event->len = 0;
		_beginthread((void (*)(void *))EventActionProc,0,(void *)event);
	}
	
}

int MDSEventAst(char *eventnam, void (*astadr)(void *,int,char *), void *astprm, int *eventid)
{
	struct event_struct *event = (struct event_struct *)malloc(sizeof(struct event_struct));

    int use_local;    

    if(!eventnam || !*eventnam) return 1;
    initializeLocalRemote(1, &use_local);
    if(num_receive_servers > 0)
	    eventAstRemote(eventnam, astadr, astprm, eventid);
    if(!use_local) return 1;

	/* Local stuff */

	*eventid = (int)event;
	event->eventnam = strcpy(malloc(strlen(eventnam)+1),eventnam);
	event->astadr = astadr;
	event->astprm = astprm;
	event->event = CreateEvent(NULL, TRUE, FALSE, eventnam);
	if (event->event == NULL) return(0);
	event->thread = _beginthread( (void(*)(void *))EventThreadProc, 0, (void *)event);
	return(event->thread != -1);
}

static int canEventRemote(int eventid)
{
    struct descriptor 
	library_d = {DTYPE_T, CLASS_S, 8, "MdsIpShr"},
	routine_d = {DTYPE_T, CLASS_S, 11, "MdsEventCan"};
    int status=1, i;
    static int (*rtn)() = 0;
    if (rtn == 0)
      status = LibFindImageSymbol(&library_d, &routine_d, &rtn);
    /* kill external thread before sending messages over the socket */
    if(status & 1)
    {
 //     KillHandler();
      for(i = 0; i < num_receive_servers; i++)
      {
	if(receive_sockets[i]) status = (*rtn) (receive_sockets[i], getRemoteId(eventid, i));
      }
  //    startRemoteAstHandler();
    }
    return status;
}

int MDSEventCan(int eventid) 
{
  int use_local;
  if(eventid < 0) return 0;

  initializeLocalRemote(1, &use_local);
  if(num_receive_servers > 0)
  	canEventRemote(eventid);
  else
  {
    struct event_struct *event = (struct event_struct *)eventid;
    CloseHandle(event->event);
    free(event->eventnam);
    free(event);
  }
	return 1;
}

struct MDSWfevent_struct {
    HANDLE event;
    int buflen;
    char *buffer;
    int *retlen;
};

static void MDSWfevent_ast(void *astparam, int data_len, char *data)
{
  BOOLEAN status;
  struct MDSWfevent_struct *event = (struct MDSWfevent_struct *)astparam;
  if (event->buffer)
  {
    if (event->buflen > 0)
    {
      memset(event->buffer,0,event->buflen);
      if (data_len > 0)
      {
        memcpy(event->buffer,data,min(data_len,event->buflen));
      }
    }
  }
  if (event->retlen)
    *event->retlen = data_len;
  status = SetEvent(event->event);
}

int MDSWfevent(char *evname, int buflen, char *data, int *datlen)
{
  int eventid;
  struct MDSWfevent_struct event;
  event.event = CreateEvent(NULL, TRUE, FALSE, NULL);
  event.buflen = buflen;
  event.buffer = data;
  event.retlen = datlen;
  MDSEventAst(evname,MDSWfevent_ast,(void *)&event,&eventid);
  WaitForSingleObject(event.event,INFINITE);
  MDSEventCan(eventid);
  CloseHandle(event.event);
  return 1;
}

int MDSEvent(char *evname, int data_len, char *data)
{
	HANDLE handle = OpenEvent(EVENT_ALL_ACCESS,0,evname);
	int use_local;
    initializeLocalRemote(0, &use_local);
    if(num_send_servers > 0)
	sendRemoteEvent(evname, data_len, data);
    if(!use_local) return 1;

	if (handle != NULL)
		PulseEvent(handle);
	CloseHandle(handle);
	return 1;

	/*
	UINT msgnum = RegisterWindowMessage("MDSplus Events");
	int status;
	status = BroadcastSystemMessage(BSF_POSTMESSAGE,0, msgnum,32,19876);
	return status;
*/
	/*
	char pipename[256];
	int status;
	int num;
	HANDLE pipe = 0;
	for (num=0;(pipe != INVALID_HANDLE_VALUE);num++)
	{
	  sprintf(pipename,"\\\\.\\pipe\\mdsevents\\%s\\%0x",evname,num);
	  pipe = CreateFile(pipename,GENERIC_WRITE,0,0,OPEN_EXISTING,0,0);
	  if (pipe == INVALID_HANDLE_VALUE)
	  {
	    status = GetLastError();
	    printf("Error creating pipe - %d\n",status);
	  }
	  else
	  {
		  int len_written = 0;
		  status = WriteFile(pipe,data,data_len,&len_written,0);
		  CloseHandle(pipe);
	  }
	}
	return 1;
	*/
}






















static char *getEnvironmentVar(char *name)
{
    char *trans =  TranslateLogical(name);
	if(!trans || !*trans) return NULL;
    return trans;
}

static int searchOpenServer(char *server) 
/* Avoid doing MdsConnect on a server already connected before */
/* for now, allow socket duplications */
{
	return 0;
}
static void getServerDefinition(char *env_var, char **servers, int *num_servers, int *use_local)
{
    int i, j;
    char *envname = getEnvironmentVar(env_var);
    char curr_name[256];
    if(!envname || !*envname)
    {
		*use_local = 1;
		*num_servers = 0;
 		return;
    }
    i = *num_servers = 0;
    *use_local = 0;
    while(i < (int)strlen(envname))
    {
		for(j = 0; i < (int)strlen(envname) && envname[i] != ';'; i++, j++)
			curr_name[j] = envname[i];
		curr_name[j] = 0;
		i++;
		if(!strcmp(curr_name, "0"))
			*use_local = 1; 
		else
		{
			servers[*num_servers] = malloc(strlen(curr_name) + 1);
			strcpy(servers[*num_servers], curr_name);
			(*num_servers)++;
		}
    }
}
static unsigned __stdcall handleRemoteAst(void *p)
{
    struct descriptor 
	library_d = {DTYPE_T, CLASS_S, 8, "MdsIpShr"},
	routine_d = {DTYPE_T, CLASS_S, 12, "GetMdsMsgOOB"};
    int status=1, i;
    static int (*rtn)() = 0;
    Message *m;
    int  selectstat;
    fd_set readfds;
    if (rtn == 0)
      status = LibFindImageSymbol(&library_d, &routine_d, &rtn);
    if(!(status & 1))
    {
		printf("%s\n", MdsGetMsg(status));
		return status;
    }
	  SetEvent(thread_alive_event);
    while(1)
    {

		WaitForSingleObject(external_event, INFINITE);
		ResetEvent(external_event);
		if(external_shutdown)
		{
			external_thread = 0;
			_endthreadex(0);
		}
        FD_ZERO(&readfds);
		for(i = 0; i < num_receive_servers; i++)
			if(receive_sockets[i])
        		FD_SET(receive_sockets[i],&readfds);
		selectstat = select(0, &readfds, 0, 0, 0);

		if (selectstat == -1) 
		{
      		perror("select error"); 
			return 0; 
		}
		for(i = 0; i < num_receive_servers; i++)
		{
			if(receive_sockets[i] && FD_ISSET( receive_sockets[i], &readfds))
			{
				if(WSAEventSelect(receive_sockets[i], external_event, 0)!=0)
					printf("Error in WSAEventSelect: %d\n", WSAGetLastError());
				if(ioctlsocket(receive_sockets[i], FIONBIO, &zero) != 0)
					printf("Error in ioctlsocket: %d\n", WSAGetLastError());

				m = ((Message *(*)())(*rtn))(receive_sockets[i], &status);
        		if (status == 1 && m->h.msglen == (sizeof(MsgHdr) + sizeof(MdsEventInfo)))
        		{
            	    MdsEventInfo *event = (MdsEventInfo *)m->bytes;
            	    ((void (*)())(*event->astadr))(event->astprm,  12, event->data);
				}
                if (m) free(m);
				ResetEvent(external_event);
				if(WSAEventSelect(receive_sockets[i], external_event, FD_READ)!=0)
					printf("Error in WSAEventSelect: %d\n", WSAGetLastError());

			}
 		}
    }
	return 0;
}



static void initializeLocalRemote(int receive_events, int *use_local)
{
    static int receive_initialized;
    static int send_initialized;
    static int use_local_send;
    static int use_local_receive;

    char *servers[256];
    int num_servers;
    struct descriptor 
	  library_d = {DTYPE_T, CLASS_S, 8, "MdsIpShr"},
	  routine_d = {DTYPE_T, CLASS_S, 12, "ConnectToMds"};
    int status=1, i;
    static int (*rtn)() = 0;

    if(receive_initialized && receive_events)
    {
		*use_local = use_local_receive;
		return;
	}
    if(send_initialized && !receive_events)
    {
		*use_local = use_local_send;
		return;
    }

    if(receive_events)
    {
		receive_initialized = 1;
		getServerDefinition("mds_event_server", servers, &num_servers, use_local);   
		num_receive_servers = num_servers;
		use_local_receive = *use_local;
    }
    else
    {
		send_initialized = 1;
   		getServerDefinition("mds_event_target", servers, &num_servers, use_local);
		num_send_servers = num_servers;
		use_local_send = *use_local;
    }
	if(!external_event)
		if((external_event = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
			printf("Error creating Event\n");
  

    if(num_servers > 0)
    {
        if (rtn == 0)
          status = LibFindImageSymbol(&library_d, &routine_d, &rtn);
        if (status & 1)
        {
			for(i = 0; i < num_servers; i++)
			{
				if(receive_events)
				{
					receive_sockets[i] = searchOpenServer(servers[i]);
	 				if(!receive_sockets[i]) receive_sockets[i] = (*rtn) (servers[i]);
	    			if(receive_sockets[i] == -1)
	    			{
						printf("\nError connecting to %s", servers[i]);
						perror("ConnectToMds");
	    				receive_sockets[i] = 0;
					}
					else
					{
						receive_servers[i] = servers[i];
					}
				}
				else
				{
					send_sockets[i] = searchOpenServer(servers[i]);
	 				if(!send_sockets[i]) send_sockets[i] = (*rtn) (servers[i]);
	    			if(send_sockets[i] == -1)
	    			{
						printf("\nError connecting to %s", servers[i]);
						perror("ConnectToMds");
	    				send_sockets[i] = 0;
					}
					else
					{
						send_servers[i] = servers[i];
					}
				}
			}
		}
		else printf(MdsGetMsg(status));
	}
}



static int eventAstRemote(char *eventnam, void (*astadr)(), void *astprm, int *eventid)
{
    struct descriptor 
	library_d = {DTYPE_T, CLASS_S, 8, "MdsIpShr"},
	routine_d = {DTYPE_T, CLASS_S, 11, "MdsEventAst"};
    int status=1, i;
    int curr_eventid;
    static int (*rtn)() = 0;


    if (rtn == 0)
      status = LibFindImageSymbol(&library_d, &routine_d, &rtn);
    if (status & 1)
    {

		/*First pf all, kill thread listening to sockets */
		if(external_thread)
		{
			external_shutdown = 1;
			SetEvent(external_event);
			WaitForSingleObject(external_thread, INFINITE);
			external_shutdown = 0;

		}

		
		newRemoteId(eventid);
		for(i = 0; i < num_receive_servers; i++)
		{
			if(receive_sockets[i])
			{ 
				if(WSAEventSelect(receive_sockets[i], external_event, 0)!=0)
					printf("Error in WSAEventSelect: %d\n", WSAGetLastError());
				if(ioctlsocket(receive_sockets[i], FIONBIO, &zero) != 0)
					printf("Error in ioctlsocket: %d\n", WSAGetLastError());
				status = (*rtn) (receive_sockets[i], eventnam, astadr, astprm, &curr_eventid);
				setRemoteId(*eventid, i, curr_eventid);
			}
		}
		external_count++;
    }
    if(!(status & 1)) printf(MdsGetMsg(status));
	ResetEvent(external_event);
	for(i = 0; i < num_receive_servers; i++)
	{
		if(receive_sockets[i])
		{ 
			if(WSAEventSelect(receive_sockets[i], external_event, FD_READ)!=0)
				printf("Error in WSAEventSelect: %d\n", WSAGetLastError());
		}
	}

	if(!thread_alive_event)
		thread_alive_event = CreateEvent(NULL, FALSE, FALSE, NULL);
	if((external_thread = (HANDLE)_beginthreadex( NULL, 0, handleRemoteAst, NULL,0, &threadID)) == (HANDLE)-1)
		printf("\nError creating thread\n");

	WaitForSingleObject(thread_alive_event, INFINITE);
	ResetEvent(thread_alive_event);
    return status;
}


/* eventid stuff: when using multiple event servers, the code has to deal with multiple eventids */
static struct {
    int used;
    int local_id; 
    int *external_ids; 
} event_info[MAX_ACTIVE_EVENTS];

static void newRemoteId(int *id)
{
    int i;
    for(i = 0; i < MAX_ACTIVE_EVENTS - 1 && event_info[i].used; i++);
    event_info[i].used = 1;
    event_info[i].external_ids = (int *)malloc(sizeof(int) * 256);
    *id = i;
}

static void deleteId(int id)
{
    if(!event_info[id].used) return;
    event_info[id].used = 0;
    free((char *)event_info[id].external_ids);
}

static void setLocalId(int id, int evid)
{
    event_info[id].local_id = evid;
}

static void setRemoteId(int id, int ofs, int evid)
{
    event_info[id].external_ids[ofs] = evid;
}

static int getLocalId(int id)
{
     return event_info[id].local_id;
}

static int getRemoteId(int id, int ofs)
{
    return event_info[id].external_ids[ofs];
}

static int sendRemoteEvent(char *evname, int data_len, char *data)
{
    static int (*rtn)();
    int (*curr_rtn)();
    struct descriptor 
	library_d = {DTYPE_T, CLASS_S, 8, "MdsIpShr"},
	routine_d = {DTYPE_T, CLASS_S, 8, "MdsValue"};
    int status=1, i, tmp_status;
    char expression[256];
    struct descrip ansarg;
    struct descrip desc;

    desc.dtype = DTYPE_B;
    desc.ptr = data;
    desc.length = 1;
    desc.ndims = 1;
    desc.dims[0] = data_len;
    ansarg.ptr = 0;
    sprintf(expression, "setevent(\"%s\"%s)", evname,data_len > 0 ? ",$" : "");
    if (rtn == 0)
      status = LibFindImageSymbol(&library_d, &routine_d, &rtn);
    if (status & 1)
    {
	for(i = 0; i < num_send_servers; i++)
	{
	    curr_rtn = rtn;
	    if(send_sockets[i])
            {
              if (data_len > 0)
                tmp_status = (*curr_rtn) (send_sockets[i], expression, &desc, &ansarg, NULL);
              else
                tmp_status = (*curr_rtn) (send_sockets[i], expression, &ansarg, NULL);
            }
            if (tmp_status & 1)
              tmp_status = (ansarg.ptr != NULL) ? *(int *)ansarg.ptr : 0;
            if (!(tmp_status & 1))
              status = tmp_status;
	    if (ansarg.ptr) free(ansarg.ptr);
	}
    }
    return status;
}



#else
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#include "../servershr/servershrp.h"
#define IPC_CREAT 42
#define IPC_STAT 42
#define IPC_RMID 42
#define SETVAL 42
#define SEM_UNDO 42
#define IPC_EXCL 42
#define SIGINT 42
union semun {int val;};
struct sembuf {int sem_num; int sem_op; int sem_flg;};
struct msqid_ds {int msg_qnum; int msg_stime; int msg_rtime; int msg_ctime;};
#undef close

#else
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#if USE_SEMAPHORE_H
#include <semaphore.h>
#else
#include <sys/sem.h>
#endif
#if USE_PIPED_MESSAGING
#include <sys/stat.h>
#else
#include <sys/msg.h>
#include <sys/time.h>
#endif
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#ifdef __hpux
#undef select
#endif
#include <unistd.h>
#endif
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <mds_stdarg.h>
#include "../mdstcpip/mdsip.h"

/* MDsEvent: UNIX and Win32 implementation of MDS Events */


#define SEM_ID 12
#define SHMEM_ID 77
#define MSG_ID 500

/* Shared memory organization: 

1) array of MAX_EVENTNAMES SharedEventNames structs
2) array of MAX_ACTIVE_EVENTS SharedEventInfo structs
*/


#define MAX_EVENTNAME_LEN 64 	 /* Maximum number of characters in event name */
#define MAX_ACTIVE_EVENTS 5000   /* Maximum number events concurrently dealt with by processes */
#define MAX_EVENTNAMES 	  1000   /* Maximum number of different event names */

#define MAX_DATA_LEN 64		 /* Maximum number of bytes to be broadcasted by events */


#define IGNORE_MSG_TIME 10	/* Messages are not sent to a message queue stalled for more than IGNORE_MSG_TIME seconds*/
#define DEAD_MSG_TIME 60	/* Message queues stalled for more than DEAD_MSG_TIME seconds are removed*/



/* Description of event names */
struct SharedEventName {
    short refcount;  /* number of active event connections making reference to this name */
    char name[MAX_EVENTNAME_LEN];  /* name of the event */
    int first_id;
};

struct SharedEventInfo{
    int nameid; /* Index of the corresponding name descriptor in the SharedEventName array */
		 /* if == -1 this item is not currently used */
    int msgkey;  /* message ID of the message queue connecting to the target process */
    int next_id;
};

/* Description of the message sent over msg queues: name of the event and up to 64 bytes of data */
struct EventMessage {
	long int mtype; 
	char name[MAX_EVENTNAME_LEN];
	char length;
	char data[MAX_DATA_LEN];
};




/* Process private part: Array of MAX_EVENTNAMES PrivateEventInfo structs */
struct PrivateEventInfo {
    char active;  /* indicates wether this descriptor is active, i.e. describing an event declared by the process */
    char name[MAX_EVENTNAME_LEN]; /* name of the event */
    void (*astadr)(void *, int, char *);	  /* ast routine address */
    void *astprm;		  /* ast routine parameter */
};

/* Descriptor for AST arguments */
struct AstDescriptor {
	void (*astadr)(void *, int, char *);
	void *astprm;
	int  data_len;
	char *data;
};


static int is_exiting = 0;

static int shmId = 0, semLocked = 0; 
static int  msgId = 0, msgKey = MSG_ID;
static struct PrivateEventInfo private_info[MAX_ACTIVE_EVENTS];
static struct SharedEventInfo *shared_info = 0;
static struct SharedEventName *shared_name = 0;

static void initializeShared();
static void *handleMessage(void * dummy);
static void removeDeadMsg(int key);


static int remote_local_initialized = 0;
static int receive_sockets[256];  	/* Socket to receive external events  */
static int send_sockets[256];  		/* Socket to send external events  */
static char *receive_servers[256];	/* Receive server names */
static char *send_servers[256];		/* Send server names */
static pthread_t local_thread = 0;      /* Thread for local event handling */
static pthread_t external_thread;	/* Thread for remote event handling */
static int external_thread_created = 0; /* Thread for remot event handling flag */
static int external_shutdown = 0;	/* flag to request remote events thread termination */
static int fds[2];			/* file descriptors used by the pipe */
static int external_count = 0;          /* remote event pendings count */
static int num_receive_servers = 0;	/* numer of external event sources */
static int num_send_servers = 0;	/* numer of external event destination */



/************* OS dependent part ******************/

static char *getEnvironmentVar(char *name)
{
    char *trans =  getenv(name);
    if(!trans || !*trans) return NULL;
    return trans;
}


static void cleanup(int);
static void handleRemoteAst();

static int getSemId()
{
  static int semId = 0;
  if(!semId) /* Acquire semaphore id if not done*/
  {
#ifdef HAVE_WINDOWS_H
	  semId = (int)CreateSemaphore(NULL,1,1,"MDSEvents Semaphore");
	  if (semId == 0)
	  {
		  if (GetLastError() == ERROR_ALREADY_EXISTS)
			  semId = (int)OpenSemaphore(SEMAPHORE_MODIFY_STATE,0,"MDSEvents Semaphore");
	  }
	  if (semId == 0)
	  {
		  printf("Error creating locking semaphore");
		  semId = -1;
	  }
#elif USE_SEMAPHORE_H
	  perror("Internal error with locking semaphore");
#else
    semId = semget(SEM_ID, 1, 0);
    if(semId == -1)
    {
      semId = 0;
      if (errno == ENOENT)
      {
        int status;
        semId = semget(SEM_ID, 1, IPC_CREAT | 0x1ff);
        if (semId == -1)
        {
          perror("Error creating locking semaphore");
          semId = 0;
        }
        else
	{
#ifdef NEED_SEMUN
          union semun { int val; struct semid_ds *buf; unsigned short *array;};
#endif
          union semun arg;
          arg.val = 1;
          status = semctl(semId,0,SETVAL,arg);
          if (status == -1)
            perror("Error accessing locking semaphore");
        }
      }
      else
        perror("Error accessing locking semaphore");
    }
#endif
  }
  return semId;
}

static int semSet(int lock)
{
    int status;
#ifdef HAVE_WINDOWS_H
	if (lock)
	{
		status = WaitForSingleObject((HANDLE)getSemId(),INFINITE);
		if (status != WAIT_FAILED)
			semLocked = 1;
	}
	else
	{
		if (semLocked)
		{
			status = ReleaseSemaphore((HANDLE)getSemId(),1,0);
			semLocked = 0;
		}
	}
	return status;
#elif USE_SEMAPHORE_H
    /* MacOS X prior to 10.2 does'nt have SysV IPC... it does have POSIX semaphores,
     * we won't bother with the silly getSemId call since we have type problems*/
    
    static sem_t *sem_p=NULL;

    if (sem_p == NULL) {
      sem_p = sem_open("MDSEvents Semaphore",O_CREAT,0777,1);
      if (SEM_FAILED == (int)sem_p)
	perror("Error creating locking semaphore");
    }

    status = lock ? sem_wait(sem_p) : sem_post(sem_p);

    if (status == -1)  
      perror("Error locking MdsEvent system\n");
    else 
      semLocked = lock;
    return(status == 0);
#else
    struct sembuf psembuf;
    psembuf.sem_num = 0;
    psembuf.sem_op = lock ? -1 : 1;
    psembuf.sem_flg = SEM_UNDO;
    status = semop(getSemId(), &psembuf, 1);
    if (status == -1)
      perror("Error locking MdsEvent system");
    else
      semLocked = lock;
    return(status == 0);
#endif
}


static int getLock()
{
  return semSet(1);
}

static int releaseLock()
{
  return semLocked ? semSet(0) : 1;
}

static int attachSharedInfo()
{
    int size = sizeof(struct SharedEventInfo) * MAX_ACTIVE_EVENTS + 
	sizeof(struct SharedEventName) * MAX_EVENTNAMES + 2 * sizeof(int);
    shmId = shmget(SHMEM_ID, size, 0777 | IPC_CREAT |IPC_EXCL);
    if(shmId != -1) /* If shared memory memory region not created yet, it must be intialized */
    { 
	shmId = shmget(SHMEM_ID, size, 0777|IPC_CREAT);
	if(shmId == -1) return -1;
	    /* Initialize shared memory structure */
	shared_name = shmat(shmId, 0, 0);
	if(shared_name == (void *)-1) 
	{
	    perror("shmat");
	    return -1;
	}
	shared_info = (struct SharedEventInfo *)&shared_name[MAX_EVENTNAMES];
	initializeShared();
    }
    else
    {
	shmId = shmget(SHMEM_ID, size, 0777 );
	shared_name = shmat(shmId, 0, 0);	    	    		
	if(shared_info == (void *)-1)
	{
	    perror("shmat");
	    return -1;
	}
	shared_info = (struct SharedEventInfo *)&shared_name[MAX_EVENTNAMES];
     }
    return 0;
}




static int readMessage(char *event_name, int *data_len, char *data)
{
    struct EventMessage message;
    int status; 

#ifdef USE_PIPED_MESSAGING
    if((status = read(msgId, &message, sizeof(message))) != sizeof(message)) {
       /* status will be first the size of the message, then 0 for the pipe closing and then -1 */
       /* we can return -1 when it goes to 0 */
        status = status ? status : -1 ;
    } else {
#else /* ! USE_PIPED_MESSAGING */
    if((status = msgrcv(msgId, &message, sizeof(message)-sizeof(message.mtype), 1, 0)) != -1)
    {
#endif /* USE_PIPED_MESSAGING */
	strncpy(event_name, message.name, MAX_EVENTNAME_LEN - 1);
	*data_len = message.length;
	if(message.length > 0)
	    memcpy(data, message.data, message.length);
    }
    return status;
}


#if (defined(_DECTHREADS_) && (_DECTHREADS_ != 1)) || !defined(_DECTHREADS_)
#define pthread_attr_default NULL
#define pthread_mutexattr_default NULL
#define pthread_condattr_default NULL
#else
#undef select
#endif

#ifdef USE_PIPED_MESSAGING
static void setKeyPath(char *path, int key)
{
    sprintf(path,"/tmp/MdsEvent.%d",key);
}
#endif

static void setHandle()
{
   pthread_t thread;
   if(!msgId)
    {
#ifdef USE_PIPED_MESSAGING
      char keypath[PATH_MAX];
      mode_t um;
      
      um = umask(0);
      setKeyPath(keypath,msgKey);
      while (mkfifo(keypath,0777) == -1) {
	if (errno != EEXIST) 
 	   perror("Fatal error with MdsEvent pipes");
        setKeyPath(keypath,++msgKey);
      }
      /* don't block on open */
      msgId = open(keypath, O_RDONLY | O_NONBLOCK);
      
      /* block on next read */
      fcntl(msgId, F_SETFL, 0);
      umask(um);
#else 
	/* get first unused message queue id */
	while((msgId = msgget(msgKey, 0777 | IPC_CREAT | IPC_EXCL)) == -1)
	    msgKey++;
#endif
	if(pthread_create(&thread, pthread_attr_default, handleMessage, 0) !=  0)
	    perror("pthread_create");
    } 
}

static int sendMessage(char *evname, int key, int data_len, char *data)
{
    struct EventMessage message;
    int status, msgid;
#ifdef USE_PIPED_MESSAGING
    char keypath[PATH_MAX];
#else
    struct msqid_ds msginfo;
    time_t curr_time;
#endif

    strncpy(message.name, evname, MAX_EVENTNAME_LEN - 1);
    message.mtype = 1;
    message.length = data_len;
    if(data_len > 0)
    {
	if(data_len > MAX_DATA_LEN) data_len = MAX_DATA_LEN;
	memcpy(message.data, data, data_len);
    }

#ifdef USE_PIPED_MESSAGING
    setKeyPath(keypath,key);
    status = msgid = open(keypath, O_WRONLY | O_NONBLOCK);
    /* opening with O_NONBLOCK will error if its not already open, 
      so this should take care of zombie Msgs, I don't know how to deal with
      stalls. */
    if (status == -1) {
      removeDeadMsg(key);
      printf("Removed dead message pipe %d, error %d\n", key, errno);
      return 0;
    }
    write(msgid,&message,sizeof(message));
    close(msgid);
    return 0;
#else
    status = msgid = msgget(key, 0777);
    if(msgid == -1)
	perror("msgget");
    else
    {
/* Check for dummies: if there are messages on the queue, compare the current time with
   the time in which a messages has been read the last time: if it is greater than IGNORE_MSG_TIME,
   do not send the message; 
*/
        if((status = msgctl(msgid, IPC_STAT, &msginfo)) == -1)
	    perror("msgctl");
	if(msginfo.msg_qnum > 0)
	{
	    curr_time = time(0);
		
	    if(msginfo.msg_stime == 0 || (curr_time - msginfo.msg_stime > IGNORE_MSG_TIME)) /* if no new messages have just been sent */	
	    {
	    	if(((msginfo.msg_rtime > 0)?(curr_time - msginfo.msg_rtime):(curr_time - msginfo.msg_ctime)) > DEAD_MSG_TIME)
	    	{
		    removeDeadMsg(key);
		    printf("\nRemoved dead message queue %d!!!", key);
		    return 0;
	        }  	

	        if(((msginfo.msg_rtime > 0)?(curr_time - msginfo.msg_rtime):(curr_time - msginfo.msg_ctime)) > IGNORE_MSG_TIME)
	        {
		    printf("\nQueue %d stalled from %d seconds: message NOT sent", msgid, 
			((msginfo.msg_rtime > 0)?(curr_time - msginfo.msg_rtime):(curr_time - msginfo.msg_ctime)));
		    return 0;
		}
	    }
	}
    	if((status = msgsnd(msgid, &message, sizeof(message)-sizeof(message.mtype), /*IPC_NOWAIT*/0)) == -1)
	    perror("msgsend");
    }
    return status;
#endif
}

/*static void attachExitHandler(void (*handler)())
{
    struct sigaction action, old_action;

    action.sa_handler = (void (*)(int)) handler;
    action.sa_mask = 0;
    action.sa_mask = 0;
    

    if(atexit(handler) == -1)
	perror("atexit");
    if(sigaction(SIGINT, &action, &old_action) == -1)
	perror("signal");
}*/

static void attachExitHandler(void (*handler)())
{
    if(atexit(handler) == -1)
	perror("atexit");
    signal(SIGINT,handler);
}



static void releaseMessages()
{
    void *dummy;
#ifdef USE_PIPED_MESSAGING
    char keypath[PATH_MAX];
#else
    struct msqid_ds buf;
#endif

    if(!msgId) return;


#ifdef USE_PIPED_MESSAGING    
    /* DTG... I don't understand the purpose of the thread_join,
       but it does crash MacOS X if not initialized */
    if (local_thread) {
        pthread_join(local_thread, &dummy);
        local_thread = 0;
    }
    
    /* I think it is true that when msgId is set so is msgKey */
    close(msgId);
    setKeyPath(keypath,msgKey);
    unlink(keypath);

#else
    pthread_join(local_thread, &dummy);
    if(msgctl(msgId, IPC_RMID, &buf) == -1)
	    perror("msgsend");
#endif

}


static void removeMessage(int key)
{

#ifdef USE_PIPED_MESSAGING
    char keypath[PATH_MAX];
    setKeyPath(keypath, key);
    unlink(keypath);

#else

    struct msqid_ds buf;
    int status, msgid;

    status = msgid = msgget(key, 0777);
    if(msgid == -1)
	perror("msgget");
    status = msgctl(msgid, IPC_RMID, &buf);
    if(status == -1) perror("msgctl");
#endif
}
 
static int createThread(pthread_t *thread, void (*rtn)(), void *par)
{
  int status = 1;
  if(pthread_create(thread, pthread_attr_default, (void *(*)(void *))rtn, par) !=  0)
  {
    status = 0;
    perror("pthread_create");
  }
  return status;
}

static void startRemoteAstHandler()
{
    pipe(fds);
    external_thread_created = createThread(&external_thread, handleRemoteAst,0);
}
	




/***************** End of OS dependent part ****************/


static void removeDeadMsg(int key)
{
    int i, curr, prev;

    for(i = 0; i < MAX_ACTIVE_EVENTS; i++)
    {
	if(shared_info[i].nameid >= 0 && shared_info[i].msgkey == key)
	{
	    if(shared_name[shared_info[i].nameid].first_id == i) /*If it is the first in the queue */
		shared_name[shared_info[i].nameid].first_id = shared_info[i].next_id;
	    else
	    {
		for(curr = prev = shared_name[shared_info[i].nameid].first_id; curr != i; curr = shared_info[curr].next_id);
		shared_info[prev].next_id = shared_info[curr].next_id;
	    }
	    shared_name[shared_info[i].nameid].refcount--;
	    shared_info[i].nameid = -1;
	}
    }
    removeMessage(key);
}







static void initializeShared()
{	
    int i;
    for(i = 0; i < MAX_EVENTNAMES; i++)
    {
	shared_name[i].refcount = 0;
	shared_name[i].first_id = -1;
    }
    for(i = 0; i < MAX_ACTIVE_EVENTS; i++)
    {
	shared_info[i].nameid = -1;
	shared_info[i].next_id = -1;
    }
}


static void executeAst(struct AstDescriptor *ast_d)
{
    (*(ast_d->astadr))(ast_d->astprm, ast_d->data_len, ast_d->data);
    if(ast_d->data_len > 0)
	free((char *)ast_d->data);
    free((char *)ast_d);
}


static void *handleMessage(void * dummy)
{
    int i;
    char event_name[MAX_EVENTNAME_LEN];
    int data_len;
    char data[MAX_DATA_LEN];
    struct AstDescriptor *arg_d;
    while(1)
    {	
	if(readMessage(event_name, &data_len, data) != -1)
	{
    	    for(i = 0; i < MAX_ACTIVE_EVENTS; i++)
    	    {
	        if(private_info[i].active && !strcmp(event_name, private_info[i].name))
		{
/*                    pthread_t thread; */
/*		    (*(private_info[i].astadr))(private_info[i].astprm, data_len, data);*/
/* Do not execute ast routine directly, rather launch a thread for doing it */

		    arg_d = (struct AstDescriptor *)malloc(sizeof(struct AstDescriptor));
		    arg_d->astadr = private_info[i].astadr;
		    arg_d->astprm = private_info[i].astprm;
		    arg_d->data_len = data_len;
		    if(data_len > 0)
		    {
			arg_d->data = malloc(data_len);
			memcpy(arg_d->data, data, data_len);
			} /* will be freed by the ExecuteAst */
		    createThread(&local_thread, executeAst, arg_d);
                    pthread_detach(local_thread);
		}
	    }
	}
	else
	    return NULL;
    }
    return NULL;
}



static void cleanup(int dummy)
{
/* remove all events declared by the process and not removed by MdsEventCan */
    int i, curr_id, prev_id;

    if(is_exiting) return;

    is_exiting = 1;
    if(!shared_info) return;

    getLock();
    for(i = 0; i < MAX_ACTIVE_EVENTS; i++)
    {
	if(shared_info[i].msgkey == msgKey && shared_info[i].nameid >= 0)
	{
	    if(shared_name[shared_info[i].nameid].refcount > 0)
	    	shared_name[shared_info[i].nameid].refcount--;
/* remove shared_infos */
	    if(shared_name[shared_info[i].nameid].refcount == 0)
		shared_name[shared_info[i].nameid].first_id = -1;
	    else
	    {
		for(curr_id = prev_id = shared_name[shared_info[i].nameid].first_id;
			curr_id != i; curr_id = shared_info[curr_id].next_id)
		    prev_id = curr_id;
		if(curr_id == shared_name[shared_info[i].nameid].first_id)
		    shared_name[shared_info[i].nameid].first_id = shared_info[curr_id].next_id;
		else
		    shared_info[prev_id].next_id = shared_info[curr_id].next_id;
	    } 	


	    shared_info[i].nameid = shared_info[i].next_id = -1;
	}
    }
    releaseLock();

    releaseMessages();
    exit(0);

}


/* eventid stuff: when using multiple event servers, the code has to deal with multiple eventids */
static struct {
    int used;
    int local_id; 
    int *external_ids; 
} event_info[MAX_ACTIVE_EVENTS];

static void newRemoteId(int *id)
{
    int i;
    for(i = 0; i < MAX_ACTIVE_EVENTS - 1 && event_info[i].used; i++);
    event_info[i].used = 1;
    event_info[i].external_ids = (int *)malloc(sizeof(int) * 256);
    *id = i;
}

static void deleteId(int id)
{
    if(!event_info[id].used) return;
    event_info[id].used = 0;
    free((char *)event_info[id].external_ids);
}

static void setLocalId(int id, int evid)
{
    event_info[id].local_id = evid;
}

static void setRemoteId(int id, int ofs, int evid)
{
    event_info[id].external_ids[ofs] = evid;
}

static int getLocalId(int id)
{
     return event_info[id].local_id;
}

static int getRemoteId(int id, int ofs)
{
    return event_info[id].external_ids[ofs];
}



static void getServerDefinition(char *env_var, char **servers, int *num_servers, int *use_local)
{
    int i, j;
    char *envname = getEnvironmentVar(env_var);
    char curr_name[256];
    if(!envname || !*envname)
    {
	*use_local = 1;
	*num_servers = 0;
 	return;
    }
    i = *num_servers = 0;
    *use_local = 0;
    while(i < strlen(envname))
    {
	for(j = 0; i < strlen(envname) && envname[i] != ';'; i++, j++)
	    curr_name[j] = envname[i];
	curr_name[j] = 0;
	i++;
	if(!strcmp(curr_name, "0"))
	    *use_local = 1; 
	else
	{
	    servers[*num_servers] = malloc(strlen(curr_name) + 1);
	    strcpy(servers[*num_servers], curr_name);
	    (*num_servers)++;
	}
    }
}

#ifdef GLOBUS
static void handleRemoteEvent(SOCKET sock);

static void KillHandler() {}

static void handleRemoteAst()
{
  static DESCRIPTOR(library_d,"MdsIpShr");
  static DESCRIPTOR(routine_d,"Poll");
  int status=1;
  static void (*rtn)(void (*)(SOCKET)) = 0;
  if (rtn == 0)
    status = LibFindImageSymbol(&library_d, &routine_d, &rtn);
  if(!(status & 1))
  {
    printf("%s\n", MdsGetMsg(status));
    return;
  }
  (*rtn)(handleRemoteEvent);
}

static int RegisterRead(SOCKET sock)
{
  static DESCRIPTOR(library_d,"MdsIpShr");
  static DESCRIPTOR(routine_d,"RegisterRead");
  int status=1;
  static int (*rtn)(SOCKET) = 0;
  if (rtn == 0)
    status = LibFindImageSymbol(&library_d, &routine_d, &rtn);
  if(!(status & 1))
  {
    printf("%s\n", MdsGetMsg(status));
    return;
  }
  return ((*rtn)(sock));
}
  
static void handleRemoteEvent(SOCKET sock)
{
  char buf[16];
  struct descriptor 
  library_d = {DTYPE_T, CLASS_S, 8, "MdsIpShr"},
  routine_d = {DTYPE_T, CLASS_S, 12, "GetMdsMsg"};
  int status=1, i;
  static int (*rtn)() = 0;
  Message *m;
  if (rtn == 0)
    status = LibFindImageSymbol(&library_d, &routine_d, &rtn);
  if(!(status & 1))
  {
    printf("handleRemoteEvent error: %s\n", MdsGetMsg(status));
    return;
  }
  m = ((Message *(*)())(*rtn))(sock, &status);
  if (status == 1 && m->h.msglen == (sizeof(MsgHdr) + sizeof(MdsEventInfo)))
  {
    MdsEventInfo *event = (MdsEventInfo *)m->bytes;
    ((void (*)())(*event->astadr))(event->astprm,  12, event->data);
  }
  if (m) free(m);
}

#else
  
static void KillHandler()
{
  int status;
  void *dummy;
  external_shutdown = 1;
  write(fds[1], "x", 1);
  status = pthread_join(external_thread, &dummy);
  close(fds[0]);
  close(fds[1]);
  external_shutdown = 0;
  external_thread_created = 0;
}

static void handleRemoteAst()
{
    char buf[16];
    struct descriptor 
	library_d = {DTYPE_T, CLASS_S, 8, "MdsIpShr"},
	routine_d = {DTYPE_T, CLASS_S, 12, "GetMdsMsg"};
    int status=1, i;
    static int (*rtn)() = 0;
    Message *m;
    int tablesize = FD_SETSIZE, selectstat;
    fd_set readfds;

    if (rtn == 0)
      status = LibFindImageSymbol(&library_d, &routine_d, &rtn);
    if(!(status & 1))
    {
	printf("%s\n", MdsGetMsg(status));
	return;
    }
    while(1)
    {
        FD_ZERO(&readfds);
	for(i = 0; i < num_receive_servers; i++)
	    if(receive_sockets[i])
        	FD_SET(receive_sockets[i],&readfds);
        FD_SET(fds[0],&readfds);
	selectstat = select(tablesize, &readfds, 0, 0, 0);
  	if (selectstat == -1) {
      	perror("select error"); return; }
	if(external_shutdown)
	{
	    read(fds[0], buf, 1);
	    pthread_exit(0);
	}
	for(i = 0; i < num_receive_servers; i++)
	{
	    if(receive_sockets[i] && FD_ISSET( receive_sockets[i], &readfds))
	    {
		m = ((Message *(*)())(*rtn))(receive_sockets[i], &status);
        	if (status == 1 && m->h.msglen == (sizeof(MsgHdr) + sizeof(MdsEventInfo)))
        	{
            	    MdsEventInfo *event = (MdsEventInfo *)m->bytes;
            	    ((void (*)())(*event->astadr))(event->astprm,  12, event->data);
		}
                if (m) 
                  free(m);
                else
                {
                  fprintf(stderr,"Error reading from event server, events may be disabled\n");
                  receive_sockets[i]=0;
                }
	    }
 	}
    }
}
#endif

static int searchOpenServer(char *server) 
/* Avoid doing MdsConnect on a server already connected before */
/* for now, allow socket duplications */
{
    int i;
	return 0;
}

/*
    for(i = 0; i < num_receive_servers; i++)
	if(receive_servers[i] && !strcmp(server, receive_servers[i])) 
	    return receive_sockets[i];
    for(i = 0; i < num_send_servers; i++)
	if(send_servers[i] && !strcmp(server, send_servers[i])) 
   return 0;
}
*/

static void initializeLocalRemote(int receive_events, int *use_local)
{
    static int receive_initialized;
    static int send_initialized;
    static int use_local_send;
    static int use_local_receive;

    char *servers[256];
    int num_servers;
    struct descriptor 
	library_d = {DTYPE_T, CLASS_S, 8, "MdsIpShr"},
	routine_d = {DTYPE_T, CLASS_S, 12, "ConnectToMds"};
    int status=1, i;
    static int (*rtn)() = 0;
    void *dummy;

    if(receive_initialized && receive_events)
    {
	*use_local = use_local_receive;
	return;
    }
    if(send_initialized && !receive_events)
    {
	*use_local = use_local_send;
	return;
    }

    if(receive_events)
    {
	receive_initialized = 1;
	getServerDefinition("mds_event_server", servers, &num_servers, use_local);   
	num_receive_servers = num_servers;
	use_local_receive = *use_local;
    }
    else
    {
	send_initialized = 1;
   	getServerDefinition("mds_event_target", servers, &num_servers, use_local);
	num_send_servers = num_servers;
	use_local_send = *use_local;
    }
    if(num_servers > 0)
    {
        if (rtn == 0)
          status = LibFindImageSymbol(&library_d, &routine_d, &rtn);
        if (status & 1)
        {
	    if(external_thread_created)
              KillHandler();
	    for(i = 0; i < num_servers; i++)
	    {
		if(receive_events)
		{
		    receive_sockets[i] = searchOpenServer(servers[i]);
	 	    if(!receive_sockets[i]) receive_sockets[i] = (*rtn) (servers[i]);
	    	    if(receive_sockets[i] == -1)
	    	    {
			printf("\nError connecting to %s", servers[i]);
		        perror("ConnectToMds");
	    	        receive_sockets[i] = 0;
		    }
		    else
		    {
#ifdef GLOBUS
		      RegisterRead(send_sockets[i]);
#endif
		      receive_servers[i] = servers[i];
                    }
		}
		else
		{
		    send_sockets[i] = searchOpenServer(servers[i]);
	 	    if(!send_sockets[i]) send_sockets[i] = (*rtn) (servers[i]);
	    	    if(send_sockets[i] == -1)
	    	    {
			printf("\nError connecting to %s", servers[i]);
		        perror("ConnectToMds");
	    	        send_sockets[i] = 0;
		    }
		    else
			send_servers[i] = servers[i];
		}
	    }
	    startRemoteAstHandler();
	}
	else printf(MdsGetMsg(status));
    }
}



	    
static int eventAstRemote(char *eventnam, void (*astadr)(), void *astprm, int *eventid)
{
    struct descriptor 
	library_d = {DTYPE_T, CLASS_S, 8, "MdsIpShr"},
	routine_d = {DTYPE_T, CLASS_S, 11, "MdsEventAst"};
    int status=1, i;
    int curr_eventid;
    void *dummy;
    static int (*rtn)() = 0;
    if (rtn == 0)
      status = LibFindImageSymbol(&library_d, &routine_d, &rtn);
    if (status & 1)
    {
/* if external_thread running, it must be killed before sending messages over socket */
	if(external_thread_created)
          KillHandler();
	newRemoteId(eventid);
	for(i = 0; i < num_receive_servers; i++)
	{
	    if(receive_sockets[i])
	    { 
		status = (*rtn) (receive_sockets[i], eventnam, astadr, astprm, &curr_eventid);
#ifdef GLOBUS
		if (status & 1)
                  RegisterRead(receive_sockets[i]);
#endif
	        setRemoteId(*eventid, i, curr_eventid);
	    }
	}
/* now external thread must be created in any case */
        if (status & 1)
	{
	    startRemoteAstHandler();
	    external_count++;
	}
    }
    if(!(status & 1)) printf(MdsGetMsg(status));
    return status;
}

struct wfevent_thread_cond { pthread_mutex_t  mutex;
                             pthread_cond_t cond;
                             int buflen;
                             char *data;
                             int *datlen;
};


static void EventHappened(void *astprm, int len, char *data)
{
  struct wfevent_thread_cond *t = (struct wfevent_thread_cond *)astprm;
  if (t->buflen && t->data)
    memcpy(t->data,data,(t->buflen > len) ? len : t->buflen); 
  if (t->datlen)
    *t->datlen = len;
  pthread_mutex_lock(&t->mutex);
  pthread_cond_signal(&t->cond);
  pthread_mutex_unlock(&t->mutex);
}

int MDSWfevent(char *evname, int buflen, char *data, int *datlen)
{
    int eventid=-1;
    struct wfevent_thread_cond t;
    pthread_mutex_init(&t.mutex,pthread_mutexattr_default);
    pthread_cond_init(&t.cond,pthread_condattr_default);
    t.buflen = buflen;
    t.data = data;
    t.datlen = datlen;
    MDSEventAst(evname, EventHappened, &t, &eventid);
    pthread_cond_wait(&t.cond,&t.mutex);
    pthread_cond_destroy(&t.cond);
    pthread_mutex_destroy(&t.mutex);
    MDSEventCan(eventid);
    return(1);
}

int MDSEventAst(char *eventnam, void (*astadr)(), void *astprm, int *eventid)
{
    int status;
    int i, j, use_local;    
    int name_already_in_use;

    *eventid = -1;
    if(!eventnam || !*eventnam) return 1;
    initializeLocalRemote(1, &use_local);
    if(num_receive_servers > 0)
	eventAstRemote(eventnam, astadr, astprm, eventid);
    if(!use_local) return 1;


    /* Local stuff */

    /* First check wether the same name is already in use */
    for(i = 0; i < MAX_EVENTNAMES; i++)
	if(private_info[i].active && !strcmp(eventnam, private_info[i].name))
	    break;
    if(i < MAX_EVENTNAMES)
	name_already_in_use = 1;
    else
	name_already_in_use = 0;


    /* define internal event dispatching structure */ 
    for(i = 0; i < MAX_EVENTNAMES; i++)
	if(private_info[i].active && !strcmp(eventnam, private_info[i].name) &&
		private_info[i].astadr == astadr && private_info[i].astprm == astprm)
	     break;
    if(i == MAX_EVENTNAMES) /* if no previous MdsEventAst to that event made by the process */
    {
	for(i = 0; i < MAX_EVENTNAMES && private_info[i].active; i++);
	if(i == MAX_EVENTNAMES) /* if no free private event slot found */
	    return 0;
	strncpy(private_info[i].name, eventnam, MAX_EVENTNAME_LEN - 1);
	private_info[i].active = 1;
    }
    private_info[i].astadr = astadr;
    private_info[i].astprm = (void *)(void *)astprm;


    /* *eventid = i; */
    if(*eventid == -1) /* if no external event receivers */
	*eventid = i;
    else
	setLocalId(*eventid, i);

    if(name_already_in_use)
	return 1;

    setHandle();

    /* lock shared memory region */ 
    getLock();

    if(!shared_info)
    {
	if(attachSharedInfo() == -1)
	{
	    releaseLock();
	    return 0;
	}
    }
/* Find first free SharedEventInfo slot */
    for(i = 0; i < MAX_ACTIVE_EVENTS && shared_info[i].nameid != -1; i++);
    if(i == MAX_ACTIVE_EVENTS) /* If no free SharedEventInfo slot foud */
    {
	releaseLock();
	return 0;
    }
    shared_info[i].msgkey = msgKey;
/* Check wether the same name previously used */
    for(j = 0; j < MAX_EVENTNAMES; j++)
	if(shared_name[j].refcount > 0 && !strcmp(shared_name[j].name, eventnam))
	    break;
    if(j == MAX_EVENTNAMES) /* if the name is not yet active */
    {
	for(j = 0; j < MAX_EVENTNAMES && shared_name[j].refcount > 0; j++);
	if(j == MAX_EVENTNAMES) /* If event names slot full */
	{
	    releaseLock();
	    return 0;
	}
	strncpy(shared_name[j].name, eventnam, MAX_EVENTNAME_LEN - 1);
	shared_name[j].first_id = i;
	shared_info[i].next_id = -1;
    }
    else
    {
	shared_info[i].next_id = shared_name[j].first_id;
	shared_name[j].first_id = i;
    }
	

    shared_name[j].refcount++;
    shared_info[i].nameid = j;

    releaseLock();

    attachExitHandler(cleanup);
    return 1;
}

static int canEventRemote(int eventid)
{
    struct descriptor 
	library_d = {DTYPE_T, CLASS_S, 8, "MdsIpShr"},
	routine_d = {DTYPE_T, CLASS_S, 11, "MdsEventCan"};
    int status=1, i;
    void *dummy;
    static int (*rtn)() = 0;

    if (rtn == 0)
      status = LibFindImageSymbol(&library_d, &routine_d, &rtn);
    /* kill external thread before sending messages over the socket */
    if(status & 1)
    {
      KillHandler();
      for(i = 0; i < num_receive_servers; i++)
      {
	if(receive_sockets[i]) status = (*rtn) (receive_sockets[i], getRemoteId(eventid, i));
      }
      startRemoteAstHandler();
    }
    return status;
}

	
int MDSEventCan(int eventid)
{
    int i, j, k, curr_id, prev_id, use_local, local_eventid, name_in_use;
    struct PrivateEventInfo *evinfo;

    if(eventid < 0) return 0;

    initializeLocalRemote(1, &use_local);
    if(num_receive_servers > 0)
	canEventRemote(eventid);
    if(!use_local) return 1;

    if(num_receive_servers > 0)
    	local_eventid = getLocalId(eventid);
     else
	local_eventid = eventid;
    deleteId(eventid);
/* local stuff */
    evinfo = &private_info[local_eventid];
    if(!shared_info) return 0; /*must follow MdsEventAst */
    name_in_use = 0;
    for(i = 0; i < MAX_EVENTNAMES; i++)
	if((i != local_eventid) && private_info[i].active && !strcmp(evinfo->name, private_info[i].name))
          name_in_use = 1;
    if (!name_in_use)
    {
      getLock();

/* deactivate corresponding SharedEventInfo slot */
      for(i = 0; i < MAX_ACTIVE_EVENTS; i++)
      {
	if(shared_info[i].msgkey == msgKey && shared_info[i].nameid >= 0 
		&& !strcmp(evinfo->name, shared_name[shared_info[i].nameid].name))
	    break;
      }
      if(i < MAX_ACTIVE_EVENTS) /* if corresponding slot found */
      {
	shared_name[shared_info[i].nameid].refcount--;

/* Remove shared info from list */
	if(shared_name[shared_info[i].nameid].refcount == 0)
	    shared_name[shared_info[i].nameid].first_id = -1;
	else
	{
	    for(curr_id = prev_id = shared_name[shared_info[i].nameid].first_id;
		    curr_id != i; curr_id = shared_info[curr_id].next_id)
		prev_id = curr_id;
	    if(curr_id == shared_name[shared_info[i].nameid].first_id)
		shared_name[shared_info[i].nameid].first_id = shared_info[curr_id].next_id;
	    else
		shared_info[prev_id].next_id = shared_info[curr_id].next_id;
	} 	

	shared_info[i].nameid = shared_info[i].next_id = -1;
      }

      releaseLock();
    }
    evinfo->active = 0;
    return 0;
}



static int sendRemoteEvent(char *evname, int data_len, char *data)
{
    static int (*rtn)() = 0;
    int (*curr_rtn)();
    struct descriptor 
	library_d = {DTYPE_T, CLASS_S, 8, "MdsIpShr"},
	routine_d = {DTYPE_T, CLASS_S, 8, "MdsValue"};
    int status=1, i, tmp_status;
    char expression[256];
    struct descrip ansarg;
    struct descrip desc;

/*struct descrip { char dtype;
                 char ndims;
                 int  dims[MAX_DIMS];
                 int  length;
		 void *ptr;
	       };
*/
    desc.dtype = DTYPE_B;
    desc.ptr = data;
    desc.length = 1;
    desc.ndims = 1;
    desc.dims[0] = data_len;
    ansarg.ptr = 0;
    sprintf(expression, "setevent(\"%s\"%s)", evname,data_len > 0 ? ",$" : "");
    if (rtn == 0)
      status = LibFindImageSymbol(&library_d, &routine_d, &rtn);
    if (status & 1)
    {
	for(i = 0; i < num_send_servers; i++)
	{
	    curr_rtn = rtn;
	    if(send_sockets[i])
            {
              if (data_len > 0)
                tmp_status = (*curr_rtn) (send_sockets[i], expression, &desc, &ansarg, NULL);
              else
                tmp_status = (*curr_rtn) (send_sockets[i], expression, &ansarg, NULL);
            }
            if (tmp_status & 1)
              tmp_status = (ansarg.ptr != NULL) ? *(int *)ansarg.ptr : 0;
            if (!(tmp_status & 1))
              status = tmp_status;
	    if (ansarg.ptr) free(ansarg.ptr);
	}
    }
    return status;
}

int MDSEvent(char *evname, int data_len, char *data)
{
    int i, j, name_idx, curr_id, use_local;
    
    initializeLocalRemote(0, &use_local);
    if(num_send_servers > 0)
	sendRemoteEvent(evname, data_len, data);
    if(!use_local) return 1;

/* Local stuff */
    getLock();
    if(!shared_info)
    {
	if(attachSharedInfo() == -1)
	{
	    releaseLock();
	    return 0;
	}
    }

    /* Search event name in the event name list */
    for(name_idx = 0; name_idx < MAX_EVENTNAMES && (shared_name[name_idx].refcount == 0 
	|| strcmp(shared_name[name_idx].name, evname)); name_idx++);
    if(name_idx < MAX_EVENTNAMES)
    {
	for(curr_id = shared_name[name_idx].first_id; curr_id != -1;
		curr_id = shared_info[curr_id].next_id)
	    sendMessage(evname, shared_info[curr_id].msgkey, data_len, data);
    }

    releaseLock();
    printf("%d, MDSEvent returned\n",pthread_self());
    return 0;
}
		    

/** For debugging only **/
void DumpSharedEventInfo()
{
    int i, j;

    getLock();
    if(!shared_info)
    {
	if(attachSharedInfo() == -1)
	{
	    releaseLock();
	}
    }
		    		
    for(i = 0; i < MAX_EVENTNAMES; i++)
    {
	if(shared_name[i].refcount > 0)
	{
	    printf("\n%s\t", shared_name[i].name);
	    for(j = 0; j < MAX_ACTIVE_EVENTS; j++)
	    {
	    	if(shared_info[j].nameid == i)
		    printf("%d  ", shared_info[j].msgkey);
	    }
	}
    }
    releaseLock();
    printf("\n");
}

void InitializeSharedInfo()
{

    getLock();
    if(!shared_info)
    {
	if(attachSharedInfo() == -1)
	{
	    releaseLock();
	}
    }
    initializeShared();
}
		    		
#ifndef USE_PIPED_MESSAGING                                
void RemoveMessages()
{
    int i, msgid, status;
    struct msqid_ds buf;

    for(i = 1; i < 1000; i++)
    {
   	msgid = msgget(i, 0777);
    	if(msgid != -1)
	{
    	    status = msgctl(msgid, IPC_RMID, &buf);
	    printf("\nRemoved queue %d status %d", i, status);
	    if(status == -1) perror("Error in msgctl");
	}
    }
}
#endif
	
#endif



