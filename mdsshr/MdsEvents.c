#include <config.h>
#include <STATICdef.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <libroutines.h>
#include <mds_stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#ifdef MDSIP_CONNECTIONS
#include "../mdstcpip/mdsip_connections.h"
#else
#include "../mdstcpip/mdsip.h"
#endif
#include "mdsshrthreadsafe.h"
//extern int MDSUdpEventAst(char *eventName, void (*astadr)(void *,int,char *), void *astprm, int *eventid);
//extern int MDSUdpEvent(char *name, int bufLen, char *buf);      
//extern int MDSUdpEventCan(int id);

#define _GNU_SOURCE /* glibc2 needs this */
#ifdef __sparc__
#include "/usr/include/sys/types.h"
#elif !defined(HAVE_WINDOWS_H)
#include <sys/types.h>
#define USE_PIPED_MESSAGING 1
#include <limits.h>
#endif

STATIC_CONSTANT int TIMEOUT = 0;

int MDSSetEventTimeout(int seconds) {
  int old_timeout=TIMEOUT;
  TIMEOUT=seconds;
  return old_timeout;
}
static DESCRIPTOR(library_d,"MdsIpShr");

static int ConnectToMds_(char *host) {
  static DESCRIPTOR(routine_d,"ConnectToMds");
  STATIC_THREADSAFE int (*rtn)() = 0;
  int status = (rtn == 0) ? LibFindImageSymbol(&library_d, &routine_d, &rtn) : 1;
  if (status & 1) {
    return (*rtn)(host);
  }
  return -1;
}

static int DisconnectFromMds_(int id) {
  static DESCRIPTOR(routine_d,"DisconnectFromMds");
  STATIC_THREADSAFE int (*rtn)() = 0;
  int status = (rtn == 0) ? LibFindImageSymbol(&library_d, &routine_d, &rtn) : 1;
  if (status & 1) {
    return (*rtn)(id);
  }
  return -1;
}

static void *GetConnectionInfo_(int id, char **name, int *readfd, size_t *len) {
#ifdef MDSIP_CONNECTIONS
  static DESCRIPTOR(routine_d,"GetConnectionInfo");
  STATIC_THREADSAFE void *(*rtn)() = 0;
  int status = (rtn == 0) ? LibFindImageSymbol(&library_d, &routine_d, &rtn) : 1;
  if (status & 1) {
    return (*rtn)(id, name, readfd, len);
  }
  return 0;
#else
  *readfd = id;
#endif
}

static int  MdsEventAst_(SOCKET sock, char *eventnam, void (*astadr)(), void *astprm, int *eventid) {
  static DESCRIPTOR(routine_d,"MdsEventAst");
  STATIC_THREADSAFE int (*rtn)() = 0;
  int status = (rtn == 0) ? LibFindImageSymbol(&library_d, &routine_d, &rtn) : 1;
  if (status & 1) {
    return (*rtn)(sock, eventnam, astadr, astprm, eventid);
  }
  return 0;
}

static Message *GetMdsMsg_(int id, int *stat) {
  static DESCRIPTOR(routine_d,"GetMdsMsg");
  STATIC_THREADSAFE  Message *(*rtn)() = 0;
  int status = (rtn == 0) ? LibFindImageSymbol(&library_d, &routine_d, &rtn) : 1;
  if (status & 1) {
    return (*rtn)(id, stat);
  }
  return 0;
}

static Message *GetMdsMsgOOB_(int id, int *stat) {
  static DESCRIPTOR(routine_d,"GetMdsMsgOOB");
  STATIC_THREADSAFE  Message *(*rtn)() = 0;
  int status = (rtn == 0) ? LibFindImageSymbol(&library_d, &routine_d, &rtn) : 1;
  if (status & 1) {
    return (*rtn)(id, stat);
  }
  return 0;
}

static int MdsEventCan_(int id, int eid) {
  static DESCRIPTOR(routine_d,"MdsEventCan");
  STATIC_THREADSAFE  int (*rtn)() = 0;
  int status = (rtn == 0) ? LibFindImageSymbol(&library_d, &routine_d, &rtn) : 1;
  if (status & 1) {
    return (*rtn)(id, eid);
  }
  return 0;
}

static int MdsValue_(int id, char *exp, struct descrip *d1, struct descrip *d2, struct descrip *d3) {
  static DESCRIPTOR(routine_d,"MdsValue");
  STATIC_THREADSAFE  int (*rtn)() = 0;
  int status = (rtn == 0) ? LibFindImageSymbol(&library_d, &routine_d, &rtn) : 1;
  if (status & 1) {
    return (*rtn)(id, exp, d1,d2,d3);
  }
  return 0;
}

static int RegisterRead_(SOCKET sock) {
  STATIC_CONSTANT DESCRIPTOR(library_d,"MdsIpShr");
  STATIC_CONSTANT DESCRIPTOR(routine_d,"RegisterRead");
  int status=1;
  STATIC_THREADSAFE int (*rtn)(SOCKET) = 0;
  if (rtn == 0)
    status = LibFindImageSymbol(&library_d, &routine_d, &rtn);
  if(!(status & 1))
  {
    printf("%s\n", MdsGetMsg(status));
    return;
  }
  return ((*rtn)(sock));
}

#ifdef HAVE_VXWORKS_H
int MDSEventAst(char *eventnam, void (*astadr)(), void *astprm, int *eventid) {}
int MDSEventCan(void *eventid) {}
int MDSEvent(char *evname){}
#elif (defined(HAVE_WINDOWS_H))
#define NO_WINDOWS_H
#include <process.h>
extern char *TranslateLogical(char *);
STATIC_ROUTINE int eventAstRemote(char *eventnam, void (*astadr)(), void *astprm, int *eventid);
STATIC_ROUTINE void initializeLocalRemote(int receive_events, int *use_local);
STATIC_ROUTINE void newRemoteId(int *id);
STATIC_ROUTINE void setRemoteId(int id, int ofs, int evid);
STATIC_ROUTINE int sendRemoteEvent(char *evname, int data_len, char *data);
STATIC_ROUTINE int getRemoteId(int id, int ofs);
STATIC_THREADSAFE int receive_ids[256];  	/* Connections to receive external events  */
STATIC_THREADSAFE SOCKET receive_sockets[256];  	/* Socket to receive external events  */
STATIC_THREADSAFE int send_ids[256];  		/* Connections to send external events  */
STATIC_THREADSAFE SOCKET send_sockets[256];  		/* Socket to send external events  */
STATIC_THREADSAFE char *receive_servers[256];	/* Receive server names */
STATIC_THREADSAFE char *send_servers[256];		/* Send server names */
STATIC_THREADSAFE HANDLE external_thread = 0;
STATIC_THREADSAFE HANDLE external_event = 0;
STATIC_THREADSAFE HANDLE thread_alive_event = 0;
#define MAX_ACTIVE_EVENTS 2000   /* Maximum number events concurrently dealt with by processes */

STATIC_THREADSAFE int external_shutdown = 0;
STATIC_THREADSAFE int external_count = 0;          /* remote event pendings count */
STATIC_THREADSAFE int num_receive_servers = 0;	/* numer of external event sources */
STATIC_THREADSAFE int num_send_servers = 0;	/* numer of external event destination */
STATIC_THREADSAFE unsigned int threadID;
STATIC_CONSTANT int zero = 0;

STATIC_ROUTINE void ReconnectToServer(int idx,int recv) {
  int status=1;
  char **servers;
  int *sockets;
  int *ids;
  int num_servers;
  if (recv) {
    servers=receive_servers;
    sockets=receive_sockets;
    ids=receive_ids;
    num_servers = num_receive_servers;
  } else {
    servers=send_servers;
    sockets=send_sockets;
    ids=send_ids;
    num_servers = num_send_servers;
  }
  if (idx >= num_servers || servers[idx] == 0) return;
  if (status & 1) {
    ids[idx] = ConnectToMds_(servers[idx]);
    if(ids[idx] <= 0) {
      printf("\nError connecting to %s", servers[idx]);
      perror("ConnectToMds_");
      ids[idx] = 0;
    } else {
      GetConnectionInfo_(ids[idx],0,&sockets[idx],0);
    }
  }
}

struct event_struct { char *eventnam;
                      void (*astadr)(void *,int,char *);
                      void *astprm;
					  int len;
					  char data[256];
					  unsigned long thread;
					  HANDLE event;
					  HANDLE pipe;
};

STATIC_ROUTINE void EventActionProc(struct event_struct *event)
{
		(*event->astadr)(event->astprm,event->len,event->data);
}

STATIC_ROUTINE void EventThreadProc(struct event_struct *event)
{
	while ( WaitForSingleObject(event->event,INFINITE) != WAIT_FAILED)
	{
		if (event->pipe != INVALID_HANDLE_VALUE)
		{
			int status;
			status = ReadFile(event->pipe,event->data,sizeof(event->data),&event->len,0);
			_beginthread((void (*)(void *))EventActionProc,0,(void *)event);
		}
	}
	
}

int old_MDSEventAst(char *eventnam_in, void (*astadr)(void *,int,char *), void *astprm, int *eventid)
{
  struct event_struct *event = (struct event_struct *)malloc(sizeof(struct event_struct));

  int use_local;    
  char *eventnam;
  char pipename[256];
  int status = 1;
  size_t i,j;
  memset(event,0,sizeof(struct event_struct));
  if(!eventnam_in || !*eventnam_in) return 1;
  eventnam = strcpy(malloc(strlen(eventnam_in)+1),eventnam_in);
  for (i=0,j=0;i<strlen(eventnam);i++) 
  {
    if (eventnam[i] != 32)
      eventnam[j++]=toupper(eventnam[i]);
  }
  eventnam[j]=0;
  initializeLocalRemote(1, &use_local);
  if(num_receive_servers > 0)
    eventAstRemote(eventnam, astadr, astprm, eventid);
  if(use_local)
  {
	int status = ERROR_PIPE_BUSY;
	int num;

    /* Local stuff */
    
    *eventid = (int)event;
    event->eventnam = eventnam;
    event->astadr = astadr;
    event->astprm = astprm;
	event->pipe = INVALID_HANDLE_VALUE;
	for (num=0;(event->pipe == INVALID_HANDLE_VALUE) && (status == ERROR_PIPE_BUSY);num++)
	{
	  sprintf(pipename,"\\\\.\\pipe\\mdsevents\\%s\\%0x",event->eventnam,num);
	  event->pipe = CreateNamedPipe(pipename,PIPE_ACCESS_INBOUND,PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,1,512,512,1000,0);
	  if (event->pipe == INVALID_HANDLE_VALUE)
	  {
	    status = GetLastError();
	  }
	}
    event->event = CreateEvent(NULL, TRUE, FALSE, eventnam);
    if (event->event == NULL) return(0);
    event->thread = _beginthread( (void(*)(void *))EventThreadProc, 0, (void *)event);
    status = event->thread != -1;
  }
  else
    free(eventnam);
  return status;
}

STATIC_ROUTINE int canEventRemote(int eventid)
{
    struct descriptor 
	library_d = {DTYPE_T, CLASS_S, 8, "MdsIpShr"},
	routine_d = {DTYPE_T, CLASS_S, 11, "MdsEventCan_"};
    int status=1, i;
    /* kill external thread before sending messages over the socket */
    if(status & 1)
    {
 //     KillHandler();
      for(i = 0; i < num_receive_servers; i++)
      {
	if(receive_ids[i]>0) status = MdsEventCan_(receive_ids[i], getRemoteId(eventid, i));
      }
  //    startRemoteAstHandler();
    }
    return status;
}

int old_MDSEventCan(int eventid) 
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
	CloseHandle(event->pipe);
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

STATIC_ROUTINE void MDSWfevent_ast(void *astparam, int data_len, char *data)
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

int MDSWfevent(char *evname, int buflen, char *data, int *datlen) {
  return MDSWfeventTimed(evname, buflen, data, datlen, TIMEOUT);
}

int MDSWfeventTimed(char *evname, int buflen, char *data, int *datlen,int timeout)
{
  int eventid;
  int status;
  struct MDSWfevent_struct event;
  event.event = CreateEvent(NULL, TRUE, FALSE, NULL);
  event.buflen = buflen;
  event.buffer = data;
  event.retlen = datlen;
  MDSEventAst(evname,MDSWfevent_ast,(void *)&event,&eventid);
  status=WaitForSingleObject(event.event,(timeout > 0) ? timeout*1000 : INFINITE);
  MDSEventCan(eventid);
  CloseHandle(event.event);
  return (status==WAIT_TIMEOUT)?0:1;
}


struct eventQueue {
  int data_len;
  char *data;
  struct eventQueue *next;
};

struct eventQueueHeader {
  int eventid;
  HANDLE wakeup;
  struct eventQueue *event;
  struct eventQueueHeader *next;
} *QueuedEvents=0;

STATIC_THREADSAFE pthread_mutex_t eqMutex;
STATIC_THREADSAFE int            eqMutex_initialized = 0;

static void CancelEventQueue(int eventid) {
  struct eventQueueHeader *qh,*qh_p;
  struct eventQueue *q;
  LockMdsShrMutex(&eqMutex,&eqMutex_initialized);
  for (qh=QueuedEvents,qh_p=0;qh && qh->eventid != eventid; qh_p=qh,qh=qh->next);
  if (qh) {
    if (qh_p) {
      qh_p->next = qh->next;
    } else {
      QueuedEvents=qh->next;
    }
    for (q=qh->event; q;) {
      struct eventQueue *this=q;
      q=q->next;
      if (this->data_len > 0 && this->data) {
	free(this->data);
      }
      free(this);
    }
    SetEvent(qh->wakeup);
    CloseHandle(qh->wakeup);
    free(qh);
  }
  UnlockMdsShrMutex(&eqMutex);
}
    
static void MDSEventQueue_ast(void *qh_in, int data_len, char *data) {
  struct eventQueueHeader *qh=(struct eventQueueHeader *)qh_in;
  struct eventQueue *q;
  struct eventQueue *thisEvent = malloc(sizeof(struct eventQueue));
  thisEvent->data_len=data_len;
  thisEvent->next=0;
  thisEvent->data= (data_len > 0) ? memcpy(malloc(data_len),data,data_len) : 0;
  LockMdsShrMutex(&eqMutex,&eqMutex_initialized);
  for (q=qh->event;q && q->next; q=q->next);
  if (q)
    q->next=thisEvent;
  else
    qh->event=thisEvent;
  SetEvent(qh->wakeup);
  UnlockMdsShrMutex(&eqMutex);
}

int MDSQueueEvent(char *evname,int *eventid) {
  int status;
  struct eventQueueHeader *thisEventH = malloc(sizeof(struct eventQueueHeader));
  LockMdsShrMutex(&eqMutex,&eqMutex_initialized);
  status = MDSEventAst(evname,MDSEventQueue_ast,(void *)thisEventH,eventid);
  if (status & 1) {
    struct eventQueueHeader *qh;
    thisEventH->eventid=*eventid;
    thisEventH->event=0;
    thisEventH->next=0;
    thisEventH->wakeup = CreateEvent(NULL, TRUE, FALSE, NULL);
    for (qh=QueuedEvents;qh && qh->next; qh=qh->next);
    if (qh) {
      qh->next=thisEventH;
    } else {
      QueuedEvents=thisEventH;
    }
  }
  else {
    free(thisEventH);
  }
  UnlockMdsShrMutex(&eqMutex);
  return status;
}

int MDSGetEventQueue(int eventid, int timeout,int *data_len, char **data) {
  struct eventQueueHeader *qh;
  int waited=0;
  int status;
 retry:
  status=1;
  LockMdsShrMutex(&eqMutex,&eqMutex_initialized);
  for (qh=QueuedEvents;qh && qh->eventid != eventid; qh=qh->next);
  if (qh) {
    if (qh->event) {
      struct eventQueue *this=qh->event;
      *data=this->data;
      *data_len=this->data_len;
      qh->event=this->next;
      free(this);
      UnlockMdsShrMutex(&eqMutex);
    } else {
      UnlockMdsShrMutex(&eqMutex);
      if (waited == 0 && timeout >= 0) {
	status=WaitForSingleObject(qh->wakeup,(timeout > 0) ? timeout*1000 : INFINITE);
	if (status == WAIT_TIMEOUT) {
	  *data_len=0;
	  *data=0;
	  status=0;
	} else {
	  if (status==0) ResetEvent(qh->wakeup);
	  waited=1;
	  goto retry;
	}
      } else {
	*data_len=0;
	*data=0;
	status=0;
      }
    }
  } else {
   UnlockMdsShrMutex(&eqMutex);
   status=2;
  }
  return status;
}

int old_MDSEvent(char *evname_in, int data_len, char *data)
{
  int use_local;
  char *evname;
  size_t i,j;
  initializeLocalRemote(0, &use_local);
  evname = strcpy(malloc(strlen(evname_in)+1),evname_in);
  for (i=0,j=0;i<strlen(evname);i++)
  {
    if (evname[i] != 32)
      evname[j++]=toupper(evname[i]);
  }
  evname[j]=0;
  if(num_send_servers > 0)
    sendRemoteEvent(evname, data_len, data);
  if(use_local){
    HANDLE handle = OpenEvent(EVENT_ALL_ACCESS,0,evname);
	if (handle != NULL) {
		char pipename[256];
		int	status;
		int num;
		HANDLE pipe = 0;
		int fail_count=0;
		for (num=0;fail_count<512;num++)
		{
			sprintf(pipename,"\\\\.\\pipe\\mdsevents\\%s\\%0x",evname,num);
			pipe = CreateFile(pipename,GENERIC_WRITE,0,0,OPEN_EXISTING,0,0);
			if (pipe != INVALID_HANDLE_VALUE)
			{
				int len_written = 0;
				status = WriteFile(pipe,data,data_len,&len_written,0);
				CloseHandle(pipe);
				fail_count=0;
			} else {
				fail_count++;
			}
		}
		PulseEvent(handle);
		CloseHandle(handle);
	}
  }
  free(evname);
  return 1;

}

STATIC_ROUTINE char *getEnvironmentVar(char *name)
{
    char *trans =  TranslateLogical(name);
	if(!trans || !*trans) return NULL;
    return trans;
}

STATIC_ROUTINE int searchOpenServer(char *server) 
/* Avoid doing MdsConnect on a server already connected before */
/* for now, allow socket duplications */
{
	return 0;
}

STATIC_ROUTINE void getServerDefinition(char *env_var, char **servers, int *num_servers, int *use_local)
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

STATIC_ROUTINE unsigned __stdcall handleRemoteAst(void *p)
{
    int status=1, i;
    Message *m;
    int  selectstat;
    fd_set readfds;
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
		  if(receive_ids[i]>0) {
		    FD_SET(receive_sockets[i],&readfds);
		  }
		selectstat = select(0, &readfds, 0, 0, 0);

		if (selectstat == -1) 
		{
      		perror("select error"); 
			return 0; 
		}
		for(i = 0; i < num_receive_servers; i++)
		{
			if(receive_ids[i] > 0 && FD_ISSET( receive_sockets[i], &readfds))
			{
				if(WSAEventSelect(receive_sockets[i], external_event, 0)!=0)
					printf("Error in WSAEventSelect: %d\n", WSAGetLastError());
				if(ioctlsocket(receive_sockets[i], FIONBIO, &zero) != 0)
					printf("Error in ioctlsocket: %d\n", WSAGetLastError());

				m = GetMdsMsgOOB_(receive_ids[i], &status);
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


STATIC_THREADSAFE pthread_mutex_t initMutex;
STATIC_THREADSAFE int             initMutex_initialized = 0;

STATIC_ROUTINE void initializeLocalRemote(int receive_events, int *use_local)
{
    STATIC_THREADSAFE int receive_initialized;
    STATIC_THREADSAFE int send_initialized;
    STATIC_THREADSAFE int use_local_send;
    STATIC_THREADSAFE int use_local_receive;

    char *servers[256];
    int num_servers;
    int status=1, i;
    LockMdsShrMutex(&initMutex,&initMutex_initialized);

    if(receive_initialized && receive_events)
    {
		*use_local = use_local_receive;
                UnlockMdsShrMutex(&initMutex);
		return;
	}
    if(send_initialized && !receive_events)
    {
		*use_local = use_local_send;
                UnlockMdsShrMutex(&initMutex);
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
        if (status & 1)
        {
			for(i = 0; i < num_servers; i++)
			{
				if(receive_events)
				{
				  if(!receive_ids[i]) receive_ids[i] = ConnectToMds_(servers[i]);
				  if(receive_ids[i] <= 0)
				    {
				      printf("\nError connecting to %s", servers[i]);
				      perror("ConnectToMds_");
				      receive_ids[i] = 0;
				    }
				  else  {
				    GetConnectionInfo_(receive_ids[i],0,&receive_sockets[i],0);
				    receive_servers[i] = servers[i];
				  }
				}
				else
				{
				  if(!send_ids[i]) send_ids[i] = ConnectToMds_(servers[i]);
				  if(send_ids[i] <= 0)
				    {
				      printf("\nError connecting to %s", servers[i]);
				      perror("ConnectToMds_");
				      send_ids[i] = 0;
				    }
				  else
				    {
				      GetConnectionInfo_(send_ids[i],0,&send_sockets[i],0);
				      send_servers[i] = servers[i];
				    }
				}
			}
		}
		else printf(MdsGetMsg(status));
	}
        UnlockMdsShrMutex(&initMutex);
}



STATIC_ROUTINE int eventAstRemote(char *eventnam, void (*astadr)(), void *astprm, int *eventid)
{
    int status=1, i;
    int curr_eventid;
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
		  if (receive_ids[i] <= 0) ReconnectToServer(i,1);
			if(receive_ids[i] > 0)
			{ 
				if(WSAEventSelect(receive_sockets[i], external_event, 0)!=0)
					printf("Error in WSAEventSelect: %d\n", WSAGetLastError());
				if(ioctlsocket(receive_sockets[i], FIONBIO, &zero) != 0)
					printf("Error in ioctlsocket: %d\n", WSAGetLastError());
				status = MdsEventAst_(receive_ids[i], eventnam, astadr, astprm, &curr_eventid);
				setRemoteId(*eventid, i, curr_eventid);
			}
		}
		external_count++;
    }
    if(!(status & 1)) printf(MdsGetMsg(status));
	ResetEvent(external_event);
	for(i = 0; i < num_receive_servers; i++)
	{
		if(receive_ids[i]>0)
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
STATIC_THREADSAFE struct {
    int used;
    int local_id; 
    int *external_ids; 
} event_info[MAX_ACTIVE_EVENTS];

STATIC_THREADSAFE pthread_mutex_t event_infoMutex;
STATIC_THREADSAFE int             event_infoMutex_initialized = 0;

STATIC_ROUTINE void newRemoteId(int *id)
{
    int i;
    LockMdsShrMutex(&event_infoMutex,&event_infoMutex_initialized);
    for(i = 0; i < MAX_ACTIVE_EVENTS - 1 && event_info[i].used; i++);
    event_info[i].used = 1;
    event_info[i].external_ids = (int *)malloc(sizeof(int) * 256);
    *id = i;
    UnlockMdsShrMutex(&event_infoMutex);
}

STATIC_ROUTINE void deleteId(int id)
{
    LockMdsShrMutex(&event_infoMutex,&event_infoMutex_initialized);
    if(event_info[id].used)
    {
      event_info[id].used = 0;
      free((char *)event_info[id].external_ids);
    }
    UnlockMdsShrMutex(&event_infoMutex);
    return;
}

STATIC_ROUTINE void setLocalId(int id, int evid)
{
    LockMdsShrMutex(&event_infoMutex,&event_infoMutex_initialized);
    event_info[id].local_id = evid;
    UnlockMdsShrMutex(&event_infoMutex);
}

STATIC_ROUTINE void setRemoteId(int id, int ofs, int evid)
{
    LockMdsShrMutex(&event_infoMutex,&event_infoMutex_initialized);
    event_info[id].external_ids[ofs] = evid;
    UnlockMdsShrMutex(&event_infoMutex);
}

STATIC_ROUTINE int getLocalId(int id)
{
    LockMdsShrMutex(&event_infoMutex,&event_infoMutex_initialized);
     return event_info[id].local_id;
    UnlockMdsShrMutex(&event_infoMutex);
}

STATIC_ROUTINE int getRemoteId(int id, int ofs)
{
    LockMdsShrMutex(&event_infoMutex,&event_infoMutex_initialized);
    return event_info[id].external_ids[ofs];
    UnlockMdsShrMutex(&event_infoMutex);
}

STATIC_ROUTINE int sendRemoteEvent(char *evname, int data_len, char *data)
{
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
    if (status & 1)
    {
      int reconnects=0;
	for(i = 0; i < num_send_servers; i++)
	{
	    if(send_ids[i] > 0)
            {
              if (data_len > 0)
                tmp_status = MdsValue_(send_ids[i], expression, &desc, &ansarg, NULL);
              else
                tmp_status = MdsValue_(send_ids[i], expression, &ansarg, NULL, NULL);
            }
            if (tmp_status & 1)
              tmp_status = (ansarg.ptr != NULL) ? *(int *)ansarg.ptr : 0;
            if (!(tmp_status & 1)) {
              status = tmp_status;
              if (reconnects < 3) {
	        ReconnectToServer(i,0);
	        reconnects++;
	        i--;
	      }
	    }
	    if (ansarg.ptr) free(ansarg.ptr);
	}
    }
    return status;
}



#else
#ifdef HAVE_WINDOWS_H
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
#ifndef HAVE_VXWORKS_H
#include <pthread.h>
#endif
#ifdef __hpux
#undef select
#endif
#include <unistd.h>
#endif

/* MDsEvent: UNIX and Win32 implementation of MDS Events */


#define SEM_ID 12
#define SHMEM_ID 77
#define MSG_ID 500

/* Shared memory organization: 

1) array of MAX_EVENTNAMES SharedEventNames structs
2) array of MAX_ACTIVE_EVENTS SharedEventInfo structs
*/


#define MAX_EVENTNAME_LEN 64 	 /* Maximum number of characters in event name */
#define MAX_ACTIVE_EVENTS 2000   /* Maximum number events concurrently dealt with by processes */
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




/* Process private part: Array of MAX_ACTIVE_EVENTS PrivateEventInfo structs */
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


STATIC_THREADSAFE pthread_mutex_t privateMutex;
STATIC_THREADSAFE int             privateMutex_initialized = 0;
STATIC_THREADSAFE pthread_mutex_t sharedMutex;
STATIC_THREADSAFE int             sharedMutex_initialized = 0;
STATIC_THREADSAFE pthread_mutex_t msgIdMutex;
STATIC_THREADSAFE int             msgIdMutex_initialized = 0;

STATIC_THREADSAFE int is_exiting = 0;

STATIC_THREADSAFE int semLocked = 0; 
STATIC_THREADSAFE int  msgId = 0, msgKey = MSG_ID;
STATIC_THREADSAFE struct PrivateEventInfo private_info[MAX_ACTIVE_EVENTS];
STATIC_THREADSAFE struct SharedEventInfo *shared_info = 0;
STATIC_THREADSAFE struct SharedEventName *shared_name = 0;

STATIC_ROUTINE void initializeShared();
STATIC_ROUTINE void *handleMessage(void * dummy);
STATIC_ROUTINE void removeDeadMsg(int key);


STATIC_THREADSAFE int receive_ids[256];  	/* Connection to receive external events  */
STATIC_THREADSAFE int send_ids[256];  		/* Connection to send external events  */
STATIC_THREADSAFE int receive_sockets[256];  	/* Socket to receive external events  */
STATIC_THREADSAFE int send_sockets[256];  		/* Socket to send external events  */
STATIC_THREADSAFE char *receive_servers[256];	/* Receive server names */
STATIC_THREADSAFE char *send_servers[256];		/* Send server names */
STATIC_THREADSAFE pthread_t local_thread = 0;      /* Thread for local event handling */
STATIC_THREADSAFE pthread_t external_thread;	/* Thread for remote event handling */
STATIC_THREADSAFE int external_thread_created = 0; /* Thread for remot event handling flag */
STATIC_THREADSAFE int external_shutdown = 0;	/* flag to request remote events thread termination */
STATIC_THREADSAFE int fds[2];			/* file descriptors used by the pipe */
STATIC_THREADSAFE int external_count = 0;          /* remote event pendings count */
STATIC_THREADSAFE int num_receive_servers = 0;	/* numer of external event sources */
STATIC_THREADSAFE int num_send_servers = 0;	/* numer of external event destination */



STATIC_ROUTINE void ReconnectToServer(int idx,int recv) {
  int status;
  char **servers;
  int *sockets;
  int *ids;
  int num_servers;
  if (recv) {
    servers=receive_servers;
    sockets=receive_sockets;
    ids=receive_ids;
    num_servers = num_receive_servers;
  } else {
    servers=send_servers;
    sockets=send_sockets;
    ids=send_ids;
    num_servers = num_send_servers;
  }
  if (idx >= num_servers || servers[idx] == 0) return;
  DisconnectFromMds_(ids[idx]);
  ids[idx] = ConnectToMds_(servers[idx]);
  if(ids[idx] <= 0) {
    printf("\nError connecting to %s", servers[idx]);
    perror("ConnectToMds_");
    sockets[idx] = 0;
  }
  else
    GetConnectionInfo_(ids[idx],0,&sockets[idx],0);
}

/************* OS dependent part ******************/

STATIC_ROUTINE char *getEnvironmentVar(char *name)
{
    char *trans =  getenv(name);
    if(!trans || !*trans) return NULL;
    return trans;
}

STATIC_ROUTINE void cleanup(int);
STATIC_ROUTINE void handleRemoteAst();

STATIC_ROUTINE int getSemId()
{
  STATIC_THREADSAFE int semId = 0;
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

int semSet(int lock)
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
    
    STATIC_ROUTINE sem_t *sem_p=NULL;

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

STATIC_ROUTINE int getLock()
{
  return semSet(1);
}

STATIC_ROUTINE int releaseLock()
{
  return semLocked ? semSet(0) : 1;
}

STATIC_ROUTINE int attachSharedInfo()
{
    int size = sizeof(struct SharedEventInfo) * MAX_ACTIVE_EVENTS + 
	sizeof(struct SharedEventName) * MAX_EVENTNAMES + 2 * sizeof(int);
    STATIC_THREADSAFE int shmId = 0; 
    LockMdsShrMutex(&sharedMutex,&sharedMutex_initialized);
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
            UnlockMdsShrMutex(&sharedMutex);
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
            UnlockMdsShrMutex(&sharedMutex);
	    return -1;
	}
	shared_info = (struct SharedEventInfo *)&shared_name[MAX_EVENTNAMES];
     }
    UnlockMdsShrMutex(&sharedMutex);
    return 0;
}

#ifdef USE_PIPED_MESSAGING
STATIC_ROUTINE void setKeyPath(char *path, int key)
{
    sprintf(path,"/tmp/MdsEvent.%d",key);
}
#endif



STATIC_ROUTINE int readMessage(char *event_name, int *data_len, char *data)
{
    struct EventMessage message;
    int status; 

#ifdef USE_PIPED_MESSAGING
   if((status = read(msgId, &message, sizeof(message))) != sizeof(message)) {
       /* status will be first the size of the message, then 0 for the pipe closing and then -1 */
       /* we can return -1 when it goes to 0 */
       /* lets not report pipe closing as an error though */
        if (status == -1)
           perror("readMessage error");
        status = status ? status : -1 ;
    } else {
#else /* ! USE_PIPED_MESSAGING */
    status = EINTR;
    while (status == EINTR)
    {
    if((status = msgrcv(msgId, &message, sizeof(message)-sizeof(message.mtype), 1, 0)) != -1)
    {
#endif /* USE_PIPED_MESSAGING */
	strncpy(event_name, message.name, MAX_EVENTNAME_LEN - 1);
	*data_len = message.length;
	if(message.length > 0)
	    memcpy(data, message.data, message.length);
    }
	/* fprintf(stderr,"Got event %s with message %s\n", event_name, data); */
#ifndef USE_PIPED_MESSAGING
    else if (errno == EINTR)
      status = EINTR;
    }
#endif

    return status;
}


#if (defined(_DECTHREADS_) && (_DECTHREADS_ != 1)) || !defined(_DECTHREADS_)
#define pthread_attr_default NULL
#define pthread_mutexattr_default NULL
#define pthread_condattr_default NULL
#else
#undef select
#endif


STATIC_ROUTINE void setHandle()
{
   pthread_t thread;
   LockMdsShrMutex(&msgIdMutex,&msgIdMutex_initialized);
   if(!msgId) {
#ifdef USE_PIPED_MESSAGING
     char keypath[PATH_MAX];
     mode_t um;
      
     um = umask(0);
     msgKey=getpid();
     setKeyPath(keypath,msgKey);
     mkfifo(keypath,0666);
     msgId=-1;
     umask(um);
#else 
     /* get first unused message queue id */
     while((msgId = msgget(msgKey, 0777 | IPC_CREAT | IPC_EXCL)) == -1)
       if (errno == EEXIST)
	 msgKey++;
       else {
	 perror("setHandle:msgget fail - check MSGMNI");
	 break;
       }
#endif
     if(pthread_create(&thread, pthread_attr_default, handleMessage, 0) !=  0)
       perror("setHandle:pthread_create");
   } 
   UnlockMdsShrMutex(&msgIdMutex);
}

STATIC_ROUTINE int sendMessage(char *evname, int key, int data_len, char *data)
{
    struct EventMessage message;
    int status, msgid;
#ifdef USE_PIPED_MESSAGING
    char keypath[PATH_MAX];
    int tries;
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
    for (tries=0;tries<5;tries++) {
      status = msgid = open(keypath, O_WRONLY | O_NONBLOCK);
      if (status != -1) break;
      sleep(1);
    }
    /* opening with O_NONBLOCK will error if its not already open, 
      so this should take care of zombie Msgs, I don't know how to deal with
      stalls. */
    if (status == -1) {
      removeDeadMsg(key);
      printf("Removed dead message pipe %d, error %d\n", key, errno);
      return 0;
    }
    status = write(msgid,&message,sizeof(message)) == sizeof(message) ? 0 : -1;
    close(msgid);
    return status;
#else
    status = msgid = msgget(key, 0777);
    if(msgid == -1)
    {     
	perror("msgget - sendMessage");
        removeDeadMsg(key);
    }
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

/*STATIC_ROUTINE void attachExitHandler(void (*handler)())
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

STATIC_ROUTINE void attachExitHandler(void (*handler)())
{
    if(atexit(handler) == -1)
	perror("atexit");
    signal(SIGINT,handler);
}



STATIC_ROUTINE void releaseMessages()
{
    void *dummy;
#ifdef USE_PIPED_MESSAGING
    char keypath[PATH_MAX];
#else
    struct msqid_ds buf;
#endif
    if(!msgId) return;


#ifdef USE_PIPED_MESSAGING    
    if (local_thread) {
        pthread_join(local_thread, &dummy);
        local_thread = 0;
    }
    
	
    /* I think it is true that when msgId is set so is msgKey */
    close(msgId);
    setKeyPath(keypath,msgKey);
    /* if (kill(msgKey,0) == -1 && errno == ESRCH) */
    unlink(keypath);

#else
    pthread_join(local_thread, &dummy);
    if(msgctl(msgId, IPC_RMID, &buf) == -1)
	    perror("msgsend");
#endif

}


STATIC_ROUTINE void removeMessage(int key)
{

#ifdef USE_PIPED_MESSAGING
    char keypath[PATH_MAX];
    setKeyPath(keypath, key);
    if (kill(key,0) == -1 && errno == ESRCH)
      unlink(keypath);

#else

    struct msqid_ds buf;
    int status, msgid;

    status = msgid = msgget(key, 0777);
    if(msgid == -1)
	perror("msgget removeMessage");
    status = msgctl(msgid, IPC_RMID, &buf);
    if(status == -1) perror("msgctl");
#endif
}
 
STATIC_ROUTINE int createThread(pthread_t *thread, void (*rtn)(), void *par)
{
  int status = 1;
  if(pthread_create(thread, pthread_attr_default, (void *(*)(void *))rtn, par) !=  0)
  {
    status = 0;
    perror("createThread:pthread_create");
  }
  return status;
}

STATIC_ROUTINE void startRemoteAstHandler()
{
    int status=pipe(fds);
    external_thread_created = createThread(&external_thread, handleRemoteAst,0);
}
	




/***************** End of OS dependent part ****************/


STATIC_ROUTINE void removeDeadMsg(int key)
{
    int i, curr, prev;
    LockMdsShrMutex(&sharedMutex,&sharedMutex_initialized);
    for(i = 0; i < MAX_ACTIVE_EVENTS; i++)
    {
	if(shared_info[i].nameid >= 0 && shared_info[i].msgkey == key)
	{
	    if(shared_name[shared_info[i].nameid].first_id == i) /*If it is the first in the queue */
		shared_name[shared_info[i].nameid].first_id = shared_info[i].next_id;
	    else
	    {
		for(curr = prev = shared_name[shared_info[i].nameid].first_id; curr != i && curr != -1; curr = shared_info[curr].next_id);
               if (curr > -1 && prev > -1)
		  shared_info[prev].next_id = shared_info[curr].next_id;
	    }
	    shared_name[shared_info[i].nameid].refcount--;
	    shared_info[i].nameid = -1;
	}
    }
    removeMessage(key);
    UnlockMdsShrMutex(&sharedMutex);
}







STATIC_ROUTINE void initializeShared()
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


STATIC_ROUTINE void executeAst(struct AstDescriptor *ast_d)
{
    (*(ast_d->astadr))(ast_d->astprm, ast_d->data_len, ast_d->data);
    if(ast_d->data_len > 0)
	free((char *)ast_d->data);
    free((char *)ast_d);
}


STATIC_ROUTINE void *handleMessage(void * dummy)
{
    int i;
    char event_name[MAX_EVENTNAME_LEN];
    int data_len;
    char data[MAX_DATA_LEN];
    struct AstDescriptor *arg_d;

	    

#ifdef USE_PIPED_MESSAGING
	/* this will block.. until the first writer! */
    while(1) {
      //	LockMdsShrMutex(&msgIdMutex,&msgIdMutex_initialized);
        { 
		char keypath[PATH_MAX];
		int tries=0;
		setKeyPath(keypath,msgKey);
		/* this will block.. until the first writer! */
		msgId=-1;
		while (msgId == -1) {
		  msgId = open(keypath, O_RDONLY);
		  if (msgId == -1) {
		    printf("readMessage - Error opening %s for read!!!!",keypath);
		    perror("readMessage - Unable to open message pipe");
		    tries=tries+1;
		    sleep(1);
		    if (tries > 5) {
		      exit(-1);
		    }
		  }
		}
	}
    //	UnlockMdsShrMutex(&msgIdMutex);
#endif

      while(1)
      {	
	if(readMessage(event_name, &data_len, data) != -1)
	{
            LockMdsShrMutex(&privateMutex,&privateMutex_initialized);
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
            UnlockMdsShrMutex(&privateMutex);
	}
	else {
	  break;
	}
      }
      close(msgId);
    }
    return NULL;
}



STATIC_ROUTINE void cleanup(int dummy)
{
/* remove all events declared by the process and not removed by MdsEventCan_ */
    int i, curr_id, prev_id;

    if(is_exiting) return;

    is_exiting = 1;
    if(!shared_info) return;
    getLock();
    LockMdsShrMutex(&sharedMutex,&sharedMutex_initialized);
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
    UnlockMdsShrMutex(&sharedMutex);

    releaseMessages();

}

STATIC_THREADSAFE pthread_mutex_t event_infoMutex;
STATIC_THREADSAFE int             event_infoMutex_initialized = 0;


/* eventid stuff: when using multiple event servers, the code has to deal with multiple eventids */
STATIC_THREADSAFE struct {
    int used;
    int local_id; 
    int *external_ids; 
} event_info[MAX_ACTIVE_EVENTS];

STATIC_ROUTINE void newRemoteId(int *id)
{
    int i;
    LockMdsShrMutex(&event_infoMutex,&event_infoMutex_initialized);
    for(i = 0; i < MAX_ACTIVE_EVENTS - 1 && event_info[i].used; i++);
    event_info[i].used = 1;
    event_info[i].external_ids = (int *)malloc(sizeof(int) * 256);
    *id = i;
    UnlockMdsShrMutex(&event_infoMutex);
}

STATIC_ROUTINE void deleteId(int id)
{
    LockMdsShrMutex(&event_infoMutex,&event_infoMutex_initialized);
    if(event_info[id].used)
    {
      event_info[id].used = 0;
      free((char *)event_info[id].external_ids);
    }
    UnlockMdsShrMutex(&event_infoMutex);
    return;
}

STATIC_ROUTINE void setLocalId(int id, int evid)
{
    LockMdsShrMutex(&event_infoMutex,&event_infoMutex_initialized);
    event_info[id].local_id = evid;
    UnlockMdsShrMutex(&event_infoMutex);
}

STATIC_ROUTINE void setRemoteId(int id, int ofs, int evid)
{
    LockMdsShrMutex(&event_infoMutex,&event_infoMutex_initialized);
    event_info[id].external_ids[ofs] = evid;
    UnlockMdsShrMutex(&event_infoMutex);
}

STATIC_ROUTINE int getLocalId(int id)
{
    LockMdsShrMutex(&event_infoMutex,&event_infoMutex_initialized);
     return event_info[id].local_id;
    UnlockMdsShrMutex(&event_infoMutex);
}

STATIC_ROUTINE int getRemoteId(int id, int ofs)
{
    LockMdsShrMutex(&event_infoMutex,&event_infoMutex_initialized);
    return event_info[id].external_ids[ofs];
    UnlockMdsShrMutex(&event_infoMutex);
}



STATIC_ROUTINE void getServerDefinition(char *env_var, char **servers, int *num_servers, int *use_local)
{
    unsigned int i, j;
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
STATIC_ROUTINE void handleRemoteEvent(SOCKET sock);

STATIC_ROUTINE void KillHandler() {}

STATIC_ROUTINE void handleRemoteAst()
{
  Poll(handleRemoteEvent);
}

  
STATIC_ROUTINE void handleRemoteEvent(SOCKET sock)
{
  char buf[16];
  STATIC_CONSTANT struct descriptor 
  int status=1, i;
  Message *m;
  m = GetMdsMsg_(sock, &status);
  if (status == 1 && m->h.msglen == (sizeof(MsgHdr) + sizeof(MdsEventInfo)))
  {
    MdsEventInfo *event = (MdsEventInfo *)m->bytes;
    ((void (*)())(*event->astadr))(event->astprm,  12, event->data);
  }
  if (m) free(m);
}

#else
  
STATIC_CONSTANT void KillHandler()
{
  int status;
  void *dummy;
  external_shutdown = 1;
  status = write(fds[1], "x", 1) == 1 ? 0 : -1;
  status = pthread_join(external_thread, &dummy);
  close(fds[0]);
  close(fds[1]);
  external_shutdown = 0;
  external_thread_created = 0;
}

STATIC_ROUTINE void handleRemoteAst()
{
    char buf[16];
    int status=1, i;
    Message *m;
    int tablesize = FD_SETSIZE, selectstat;
    fd_set readfds;

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
	  status = read(fds[0], buf, 1) == 1 ? 0 : -1;
	  pthread_exit(0);
	}
	for(i = 0; i < num_receive_servers; i++)
	{
	    if(receive_ids[i] > 0 && FD_ISSET( receive_sockets[i], &readfds))
	    {
		m = GetMdsMsg_(receive_ids[i], &status);
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

STATIC_ROUTINE int searchOpenServer(char *server) 
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

STATIC_THREADSAFE pthread_mutex_t initMutex;
STATIC_THREADSAFE int             initMutex_initialized = 0;


STATIC_ROUTINE void initializeLocalRemote(int receive_events, int *use_local)
{
    STATIC_THREADSAFE int receive_initialized;
    STATIC_THREADSAFE int send_initialized;
    STATIC_THREADSAFE int use_local_send;
    STATIC_THREADSAFE int use_local_receive;

    char *servers[256];
    int num_servers;
    int status=1, i;
    void *dummy;

    LockMdsShrMutex(&initMutex,&initMutex_initialized);

    if(receive_initialized && receive_events)
    {
	*use_local = use_local_receive;
        UnlockMdsShrMutex(&initMutex);
	return;
    }
    if(send_initialized && !receive_events)
    {
	*use_local = use_local_send;
        UnlockMdsShrMutex(&initMutex);
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
        if (status & 1)
        {
	    if(external_thread_created)
              KillHandler();
	    for(i = 0; i < num_servers; i++)
	    {
		if(receive_events)
		{
		    receive_ids[i] = searchOpenServer(servers[i]);
	 	    if(receive_ids[i] <= 0) receive_ids[i] = ConnectToMds_(servers[i]);
	    	    if(receive_ids[i] <= 0)
	    	    {
			printf("\nError connecting to %s", servers[i]);
		        perror("ConnectToMds_");
	    	        receive_ids[i] = 0;
		    }
		    else
		    {
#ifdef GLOBUS
		      RegisterRead_(send_sockets[i]);
#endif
		      GetConnectionInfo_(receive_ids[i],0,&receive_sockets[i],0);
		      receive_servers[i] = servers[i];
                    }
		}
		else
		{
		    send_ids[i] = searchOpenServer(servers[i]);
	 	    if(send_ids[i] <= 0) send_ids[i] = ConnectToMds_(servers[i]);
	    	    if(send_ids[i] <= 0)
	    	    {
			printf("\nError connecting to %s", servers[i]);
		        perror("ConnectToMds_");
	    	        send_ids[i] = 0;
		    }
		    else {
			send_servers[i] = servers[i];
			GetConnectionInfo_(send_ids[i],0,&send_sockets[i],0);
		    }
		}
	    }
	    startRemoteAstHandler();
	}
	else printf("%s\n",MdsGetMsg(status));
    }
    UnlockMdsShrMutex(&initMutex);
}



	    
STATIC_ROUTINE int eventAstRemote(char *eventnam, void (*astadr)(), void *astprm, int *eventid)
{
    int status=1, i;
    int curr_eventid;
    void *dummy;
    if (status & 1)
    {
/* if external_thread running, it must be killed before sending messages over socket */
	if(external_thread_created)
          KillHandler();
	newRemoteId(eventid);
	for(i = 0; i < num_receive_servers; i++)
	{
	  if (receive_ids[i] <= 0) ReconnectToServer(i,1);
	    if(receive_ids[i] > 0)
	    { 
		status = MdsEventAst_(receive_ids[i], eventnam, astadr, astprm, &curr_eventid);
#ifdef GLOBUS
		if (status & 1)
                  RegisterRead_(receive_ids[i]);
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
    if(!(status & 1)) printf("%s\n",MdsGetMsg(status));
    return status;
}

struct wfevent_thread_cond {
  int active;
  pthread_mutex_t  mutex;
  pthread_cond_t cond;
  int buflen;
  char *data;
  int *datlen;
};


STATIC_ROUTINE void EventHappened(void *astprm, int len, char *data)
{
  struct wfevent_thread_cond *t = (struct wfevent_thread_cond *)astprm;
  pthread_mutex_lock(&t->mutex);
  if (t->active)
  {
    if (t->buflen && t->data)
      memcpy(t->data,data,(t->buflen > len) ? len : t->buflen); 
    if (t->datlen)
      *t->datlen = len;
  }
  pthread_cond_signal(&t->cond);
  pthread_mutex_unlock(&t->mutex);
}

int MDSWfevent(char *evname, int buflen, char *data, int *datlen) {
  return MDSWfeventTimed(evname,buflen,data,datlen,TIMEOUT);
}

int MDSWfeventTimed(char *evname, int buflen, char *data, int *datlen,int timeout)
{
    int eventid=-1;
    int status;
    struct wfevent_thread_cond t = {1};
    pthread_mutex_init(&t.mutex,pthread_mutexattr_default);
    pthread_cond_init(&t.cond,pthread_condattr_default);
    t.buflen = buflen;
    t.data = data;
    t.datlen = datlen;
    MDSEventAst(evname, EventHappened, &t, &eventid);
    pthread_mutex_lock(&t.mutex);
    if (timeout>0) {
      static struct timespec abstime;
      clock_gettime(CLOCK_REALTIME,&abstime);
      abstime.tv_sec+=timeout;
      status=pthread_cond_timedwait(&t.cond,&t.mutex,&abstime);
    } else {
      status=pthread_cond_wait(&t.cond,&t.mutex);
    }
    pthread_mutex_unlock(&t.mutex);
    pthread_mutex_lock(&t.mutex);
    t.active=0;
    MDSEventCan(eventid);
    pthread_mutex_unlock(&t.mutex);
    pthread_cond_destroy(&t.cond);
    pthread_mutex_destroy(&t.mutex);
    return(status==0);
}

struct eventQueue {
  int data_len;
  char *data;
  struct eventQueue *next;
};

struct eventQueueHeader {
  int eventid;
  pthread_mutex_t  mutex;
  pthread_cond_t cond;
  struct eventQueue *event;
  struct eventQueueHeader *next;
} *QueuedEvents=0;

STATIC_THREADSAFE pthread_mutex_t eqMutex;
STATIC_THREADSAFE int            eqMutex_initialized = 0;

static void CancelEventQueue(int eventid) {
  struct eventQueueHeader *qh,*qh_p;
  struct eventQueue *q;
  LockMdsShrMutex(&eqMutex,&eqMutex_initialized);
  for (qh=QueuedEvents,qh_p=0;qh && qh->eventid != eventid; qh_p=qh,qh=qh->next);
  if (qh) {
    if (qh_p) {
      qh_p->next = qh->next;
    } else {
      QueuedEvents=qh->next;
    }
    for (q=qh->event; q;) {
      struct eventQueue *this=q;
      q=q->next;
      if (this->data_len > 0 && this->data) {
	free(this->data);
      }
      free(this);
    }
    pthread_mutex_lock(&qh->mutex);
    pthread_cond_signal(&qh->cond);
    pthread_mutex_unlock(&qh->mutex);
    pthread_cond_destroy(&qh->cond);
    pthread_mutex_destroy(&qh->mutex);
    free(qh);
  }
  UnlockMdsShrMutex(&eqMutex);
}
    
static void MDSEventQueue_ast(void *qh_in, int data_len, char *data) {
  struct eventQueueHeader *qh=(struct eventQueueHeader *)qh_in;
  struct eventQueue *q;
  struct eventQueue *thisEvent = malloc(sizeof(struct eventQueue));
  thisEvent->data_len=data_len;
  thisEvent->next=0;
  thisEvent->data= (data_len > 0) ? memcpy(malloc(data_len),data,data_len) : 0;
  LockMdsShrMutex(&eqMutex,&eqMutex_initialized);
  for (q=qh->event;q && q->next; q=q->next);
  if (q)
    q->next=thisEvent;
  else
    qh->event=thisEvent;
  pthread_mutex_lock(&qh->mutex);
  pthread_cond_signal(&qh->cond);
  pthread_mutex_unlock(&qh->mutex);
  UnlockMdsShrMutex(&eqMutex);
}

int MDSQueueEvent(char *evname,int *eventid) {
  int status;
  struct eventQueueHeader *thisEventH = malloc(sizeof(struct eventQueueHeader));
  LockMdsShrMutex(&eqMutex,&eqMutex_initialized);
  status = MDSEventAst(evname,MDSEventQueue_ast,(void *)thisEventH,eventid);
  if (status & 1) {
    struct eventQueueHeader *qh;
    thisEventH->eventid=*eventid;
    thisEventH->event=0;
    thisEventH->next=0;
    pthread_mutex_init(&thisEventH->mutex,pthread_mutexattr_default);
    pthread_cond_init(&thisEventH->cond,pthread_condattr_default);
    for (qh=QueuedEvents;qh && qh->next; qh=qh->next);
    if (qh) {
      qh->next=thisEventH;
    } else {
      QueuedEvents=thisEventH;
    }
  }
  else {
    free(thisEventH);
  }
  UnlockMdsShrMutex(&eqMutex);
  return status;
}

int MDSGetEventQueue(int eventid, int timeout,int *data_len, char **data) {
  struct eventQueueHeader *qh;
  int waited=0;
  int status=1;
 retry:
  LockMdsShrMutex(&eqMutex,&eqMutex_initialized);
  for (qh=QueuedEvents;qh && qh->eventid != eventid; qh=qh->next);
  if (qh) {
    if (qh->event) {
      struct eventQueue *this=qh->event;
      *data=this->data;
      *data_len=this->data_len;
      qh->event=this->next;
      free(this);
      UnlockMdsShrMutex(&eqMutex);
    } else {
      UnlockMdsShrMutex(&eqMutex);
      if (waited == 0 && timeout >= 0) {
	pthread_mutex_lock(&qh->mutex);
	if (timeout>0) {
	  static struct timespec abstime;
	  clock_gettime(CLOCK_REALTIME,&abstime);
	  abstime.tv_sec+=timeout;
	  status=pthread_cond_timedwait(&qh->cond,&qh->mutex,&abstime);
	} else {
	  status=pthread_cond_wait(&qh->cond,&qh->mutex);
	}
	pthread_mutex_unlock(&qh->mutex);
	if (status != 0) {
	  *data_len=0;
	  *data=0;
	  status=0;
	} else {
	  status=1;
	  waited=1;
	  goto retry;
	}
      } else {
	*data_len=0;
	*data=0;
	status=0;
      }
    }
  } else {
   UnlockMdsShrMutex(&eqMutex);
   status = 2;
  }
  return status;
}

int old_MDSEventAst(char *eventnam_in, void (*astadr)(), void *astprm, int *eventid)
{
  int status = 1;
  unsigned int i, j, use_local;    
  int name_already_in_use;
  char *eventnam;
  *eventid = -1;
  if(!eventnam_in || !*eventnam_in) return 1;
  eventnam = strcpy(malloc(strlen(eventnam_in)+1),eventnam_in);
  for (i=0,j=0;i<strlen(eventnam);i++)
  {
    if (eventnam[i] != 32)
      eventnam[j++]=toupper(eventnam[i]);
  }
  eventnam[j]=0;
  initializeLocalRemote(1, &use_local);
  if(num_receive_servers > 0)
    eventAstRemote(eventnam, astadr, astprm, eventid);
  if(use_local)
  {
    /* Local stuff */

    /* First check wether the same name is already in use */
    LockMdsShrMutex(&privateMutex,&privateMutex_initialized);
    for(i = 0; i < MAX_ACTIVE_EVENTS; i++)
      if(private_info[i].active && !strcmp(eventnam, private_info[i].name))
	break;
    if(i < MAX_ACTIVE_EVENTS)
      name_already_in_use = 1;
    else
      name_already_in_use = 0;


    /* define internal event dispatching structure */ 
    for(i = 0; i < MAX_ACTIVE_EVENTS; i++)
      if(private_info[i].active && !strcmp(eventnam, private_info[i].name) &&
	 private_info[i].astadr == astadr && private_info[i].astprm == astprm)
	break;
    if(i == MAX_ACTIVE_EVENTS) /* if no previous MdsEventAst_ to that event made by the process */
    {
      for(i = 0; i < MAX_ACTIVE_EVENTS && private_info[i].active; i++);
      if(i == MAX_ACTIVE_EVENTS) /* if no free private event slot found */
      {
        fprintf(stderr,"No more active event slots available. Ignoring event\n");
        status = 0;
        UnlockMdsShrMutex(&privateMutex);
        goto cleanup;
      }
      strncpy(private_info[i].name, eventnam, MAX_EVENTNAME_LEN - 1);
      private_info[i].active = 1;
    }
    private_info[i].astadr = astadr;
    private_info[i].astprm = (void *)(void *)astprm;
    UnlockMdsShrMutex(&privateMutex);


    /* *eventid = i; */
    if(*eventid == -1) /* if no external event receivers */
	*eventid = i;
    else
	setLocalId(*eventid, i);

    if(name_already_in_use)
    {
      status = 1;
      goto cleanup;
    }

    setHandle();

    /* lock shared memory region */
    getLock();

    if(!shared_info)
    {
      if(attachSharedInfo() == -1)
      {
	releaseLock();
        status = 0;
        goto cleanup;
      }
    }
    LockMdsShrMutex(&sharedMutex,&sharedMutex_initialized);
/* Find first free SharedEventInfo slot */
    for(i = 0; i < MAX_ACTIVE_EVENTS && shared_info[i].nameid != -1; i++);
    if(i == MAX_ACTIVE_EVENTS) /* If no free SharedEventInfo slot foud */
    {
      fprintf(stderr,"Global event table full. Event ignored.\n");
      releaseLock();
      UnlockMdsShrMutex(&sharedMutex);
      status = 0;
      goto cleanup;
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
        fprintf(stderr,"Global event name table full. Event ignored.\n");
	releaseLock();
	UnlockMdsShrMutex(&sharedMutex);
        status = 0;
        goto cleanup;
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
    UnlockMdsShrMutex(&sharedMutex);
    
    attachExitHandler(cleanup);
  }
 cleanup:
  free(eventnam);
  return status;
}

STATIC_ROUTINE int canEventRemote(int eventid)
{
    int status=1, i;
    void *dummy;
    /* kill external thread before sending messages over the socket */
    if(status & 1)
    {
      KillHandler();
      for(i = 0; i < num_receive_servers; i++)
      {
	if(receive_ids[i] > 0) status = MdsEventCan_(receive_ids[i], getRemoteId(eventid, i));
      }
      startRemoteAstHandler();
    }
    return status;
}

	
int old_MDSEventCan(int eventid)
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
    if(!shared_info) return 0; /*must follow MdsEventAst_ */
    name_in_use = 0;
    LockMdsShrMutex(&privateMutex,&privateMutex_initialized);
    for(i = 0; i < MAX_ACTIVE_EVENTS; i++)
	if((i != local_eventid) && private_info[i].active && !strcmp(evinfo->name, private_info[i].name))
          name_in_use = 1;
    if (!name_in_use)
    {
      getLock();

/* deactivate corresponding SharedEventInfo slot */
      LockMdsShrMutex(&sharedMutex,&sharedMutex_initialized);
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

     UnlockMdsShrMutex(&sharedMutex);
     releaseLock();
    }
    UnlockMdsShrMutex(&privateMutex);
    evinfo->active = 0;
    return 0;
}



STATIC_ROUTINE int sendRemoteEvent(char *evname, int data_len, char *data)
{
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
    if (status & 1)
    {
      int reconnects=0;
	for(i = 0; i < num_send_servers; i++)
	{
	    if(send_ids[i] > 0)
            {
              if (data_len > 0)
                tmp_status = MdsValue_ (send_ids[i], expression, &desc, &ansarg, NULL);
              else
                tmp_status = MdsValue_ (send_ids[i], expression, &ansarg, NULL, NULL);
            }
            if (tmp_status & 1)
              tmp_status = (ansarg.ptr != NULL) ? *(int *)ansarg.ptr : 0;
            if (!(tmp_status & 1)) {
              status = tmp_status;
	      if (reconnects < 3) {
		ReconnectToServer(i,0);
		reconnects++;
		i--;
	      }
	    }
	    if (ansarg.ptr) free(ansarg.ptr);
	}
    }
    return status;
}

void RemoveAllDeadQueues() {
#ifdef USE_PIPED_MESSAGING                                
  int name_idx,curr_id;
 loop:
  for(name_idx = 0; name_idx < MAX_EVENTNAMES; name_idx++) {
    for(curr_id = shared_name[name_idx].first_id; curr_id != -1;
	curr_id = shared_info[curr_id].next_id) {
      if (kill(shared_info[curr_id].msgkey,0) == -1 && errno == ESRCH) {
	removeDeadMsg(shared_info[curr_id].msgkey);
	goto loop;
      }
    }
  }
#endif
  return;
}

int old_MDSEvent(char *evname_in, int data_len, char *data)
{
  int i, j, name_idx, curr_id, use_local;
  unsigned int u;
  char *evname;
  int status = 1;
  initializeLocalRemote(0, &use_local);
  evname = strcpy(malloc(strlen(evname_in)+1),evname_in);
  for (u=0,j=0;u<strlen(evname);u++)
  {
     if (evname[u] != 32)
       evname[j++]=toupper(evname[u]);
  }
  evname[j]=0;
  if(num_send_servers > 0)
    sendRemoteEvent(evname, data_len, data);
  if(use_local)
  {

/* Local stuff */
    getLock();
    if(!shared_info)
    {
      if(attachSharedInfo() == -1)
      {
	releaseLock();
        status = 0;
        goto cleanup2;
      }
    }

    /* Search event name in the event name list */
    LockMdsShrMutex(&sharedMutex,&sharedMutex_initialized);
    RemoveAllDeadQueues();
    for(name_idx = 0; name_idx < MAX_EVENTNAMES && (shared_name[name_idx].refcount == 0 
      || strcmp(shared_name[name_idx].name, evname)); name_idx++);
    if(name_idx < MAX_EVENTNAMES)
    {
      for(curr_id = shared_name[name_idx].first_id; curr_id != -1;
	  curr_id = shared_info[curr_id].next_id)
	sendMessage(evname, shared_info[curr_id].msgkey, data_len, data);
    }
    releaseLock();
    UnlockMdsShrMutex(&sharedMutex);
  }
 cleanup2:
  free(evname);
  return status;
}
		    

/** For debugging only **/
int NumActiveSharedEvents()
{
    int i, j;
    int num=0;

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
      num+=shared_name[i].refcount;
    }
    releaseLock();
    return num;
}

int NumActivePrivateEvents()
{
  int i,num=0;
  LockMdsShrMutex(&privateMutex,&privateMutex_initialized);
  for(i = 0; i < MAX_ACTIVE_EVENTS; i++)
  {
    if(private_info[i].active) num++;
  }
  UnlockMdsShrMutex(&privateMutex);
  return num;
}

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

/*void InitializeSharedInfo()
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
*/		    		
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

int MDSEventAst(char *eventNameIn, void (*astadr)(void *,int,char *), void *astprm, int *eventid) {
  char *eventName=malloc(strlen(eventNameIn)+1);
  unsigned int i,j;
  int status;
  for (i=0,j=0;i<strlen(eventNameIn);i++) 
  {
    if (eventNameIn[i] != 32)
      eventName[j++]=toupper(eventNameIn[i]);
  }
  eventName[j]=0;
  if ( getenv("mds_event_server") || (getenv("UDP_EVENTS")==0))
    status = old_MDSEventAst(eventName,astadr,astprm,eventid);
  else
    status = MDSUdpEventAst(eventName,astadr,astprm,eventid);
  free(eventName);
  return status;
}

int MDSEvent(char *eventNameIn, int bufLen, char *buf) {
  char *eventName=malloc(strlen(eventNameIn)+1);
  unsigned int i,j;
  int status;
  for (i=0,j=0;i<strlen(eventNameIn);i++) 
  {
    if (eventNameIn[i] != 32)
      eventName[j++]=toupper(eventNameIn[i]);
  }
  eventName[j]=0;
  if (getenv("mds_event_target") || (getenv("UDP_EVENTS")==0))
    status = old_MDSEvent(eventName,bufLen,buf);
  else
    status = MDSUdpEvent(eventName,bufLen,buf);
  return status;
}

int MDSEventCan(int id) {
  int status;
  if (getenv("mds_event_server") || (getenv("UDP_EVENTS")==0))
    status = old_MDSEventCan(id);
  else
    status = MDSUdpEventCan(id);
  CancelEventQueue(id);
  return status;
}


