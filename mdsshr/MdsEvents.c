#include <config.h>
#include <STATICdef.h>
#include <mdsshr.h>
#include <libroutines.h>
#include <mds_stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include "../mdstcpip/mdsip_connections.h"
#include "mdsshrthreadsafe.h"
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif
#include "mdsshrp.h"

#define _GNU_SOURCE		/* glibc2 needs this */
#include <sys/types.h>
#include <ctype.h>

#define MAX_ACTIVE_EVENTS 2000	/* Maximum number events concurrently dealt with by processes */

STATIC_THREADSAFE int receive_ids[256];	/* Connection to receive external events  */
STATIC_THREADSAFE int send_ids[256];	/* Connection to send external events  */
STATIC_THREADSAFE int receive_sockets[256];	/* Socket to receive external events  */
STATIC_THREADSAFE int send_sockets[256];	/* Socket to send external events  */
STATIC_THREADSAFE char *receive_servers[256];	/* Receive server names */
STATIC_THREADSAFE char *send_servers[256];	/* Send server names */
STATIC_THREADSAFE int external_thread_created = 0;	/* Thread for remot event handling flag */
STATIC_THREADSAFE int external_shutdown = 0;	/* flag to request remote events thread termination */
STATIC_THREADSAFE int fds[2];	/* file descriptors used by the pipe */
STATIC_THREADSAFE int external_count = 0;	/* remote event pendings count */
STATIC_THREADSAFE int num_receive_servers = 0;	/* numer of external event sources */
STATIC_THREADSAFE int num_send_servers = 0;	/* numer of external event destination */

STATIC_ROUTINE int eventAstRemote(char const *eventnam, void (*astadr) (), void *astprm, int *eventid);
STATIC_ROUTINE void initializeRemote(int receive_events);
STATIC_CONSTANT int TIMEOUT = 0;

EXPORT int MDSSetEventTimeout(int seconds)
{
  int old_timeout = TIMEOUT;
  TIMEOUT = seconds;
  return old_timeout;
}

static int ConnectToMds_(char const *host)
{
  STATIC_THREADSAFE int (*rtn) () = 0;
  int status = (rtn == 0) ? LibFindImageSymbol_C("MdsIpShr", "ConnectToMds", (void **)&rtn) : 1;
  if (status & 1) {
    return (*rtn) (host);
  }
  return -1;
}

#ifndef _WIN32
static int DisconnectFromMds_(int id)
{
  STATIC_THREADSAFE int (*rtn) () = 0;
  int status = (rtn == 0) ? LibFindImageSymbol_C("MdsIpShr", "DisconnectFromMds", (void **)&rtn) : 1;
  if (status & 1) {
    return (*rtn) (id);
  }
  return -1;
}
#endif

static void *GetConnectionInfo_(int id, char **name, int *readfd, size_t * len)
{
  STATIC_THREADSAFE void *(*rtn) () = 0;
  int status = (rtn == 0) ? LibFindImageSymbol_C("MdsIpShr", "GetConnectionInfo", (void **)&rtn) : 1;
  if (status & 1) {
    return (*rtn) (id, name, readfd, len);
  }
  return 0;
}

static int MdsEventAst_(int sock, char const *eventnam, void (*astadr) (), void *astprm, int *eventid)
{
  STATIC_THREADSAFE int (*rtn) () = 0;
  int status = (rtn == 0) ? LibFindImageSymbol_C("MdsIpShr", "MdsEventAst", (void **)&rtn) : 1;
  if (status & 1) {
    return (*rtn) (sock, eventnam, astadr, astprm, eventid);
  }
  return 0;
}

#ifndef _WIN32
static Message *GetMdsMsg_(int id, int *stat)
{
  STATIC_THREADSAFE Message *(*rtn) () = 0;
  int status = (rtn == 0) ? LibFindImageSymbol_C("MdsIpShr", "GetMdsMsg", (void **)&rtn) : 1;
  if (status & 1) {
    return (*rtn) (id, stat);
  }
  return 0;
}
#endif

#ifdef _WIN32
static Message *GetMdsMsgOOB_(int id, int *stat)
{
  STATIC_THREADSAFE Message *(*rtn) () = 0;
  int status = (rtn == 0) ? LibFindImageSymbol_C("MdsIpShr", "GetMdsMsgOOB", (void **)&rtn) : 1;
  if (status & 1) {
    return (*rtn) (id, stat);
  }
  return 0;
}
#endif

static int MdsEventCan_(int id, int eid)
{
  STATIC_THREADSAFE int (*rtn) () = 0;
  int status = (rtn == 0) ? LibFindImageSymbol_C("MdsIpShr", "MdsEventCan", (void **)&rtn) : 1;
  if (status & 1) {
    return (*rtn) (id, eid);
  }
  return 0;
}

static int MdsValue_(int id, char const *exp, struct descrip *d1, struct descrip *d2, struct descrip *d3)
{
  STATIC_THREADSAFE int (*rtn) () = 0;
  int status = (rtn == 0) ? LibFindImageSymbol_C("MdsIpShr", "MdsValue", (void **)&rtn) : 1;
  if (status & 1) {
    return (*rtn) (id, exp, d1, d2, d3);
  }
  return 0;
}

#ifdef GLOBUS
static int RegisterRead_(int sock)
{
  int status = 1;
  STATIC_THREADSAFE int (*rtn) (int) = 0;
  if (rtn == 0)
    status = LibFindImageSymbol_C("MdsIpShr", "RegisterRead", (void **)&rtn);
  if (!(status & 1)) {
    printf("%s\n", MdsGetMsg(status));
    return status;
  }
  return ((*rtn) (sock));
}
#endif

static char *eventName(char const *eventnam_in) {
  int i,j;
  char *eventnam=0;
  if (eventnam_in) {
    eventnam = strdup(eventnam_in);
    for (i = 0, j = 0; i < strlen(eventnam); i++) {
      if (eventnam[i] != 32)
	eventnam[j++] = toupper(eventnam[i]);
    }
    eventnam[j] = 0;
    if (strlen(eventnam)==0) {
      free(eventnam);
      eventnam=0;
    }
  }
  return eventnam;
}
  
#ifdef HAVE_VXWORKS_H
int MDSEventAst(char const *eventnam, void (*astadr) (), void *astprm, int *eventid)
{
}

int MDSEventCan(void *eventid)
{
}

int MDSEvent(char const *evname)
{
}
#elif (defined(_WIN32))
#define NO_WINDOWS_H
#include <process.h>
STATIC_ROUTINE void newRemoteId(int *id);
STATIC_ROUTINE void setRemoteId(int id, int ofs, int evid);
STATIC_ROUTINE int sendRemoteEvent(char const *evname, int data_len, char *data);
STATIC_ROUTINE int getRemoteId(int id, int ofs);
STATIC_THREADSAFE HANDLE external_thread = 0;
STATIC_THREADSAFE HANDLE external_event = 0;
STATIC_THREADSAFE HANDLE thread_alive_event = 0;

STATIC_THREADSAFE unsigned int threadID;
STATIC_CONSTANT unsigned long zero = 0;

STATIC_ROUTINE void ReconnectToServer(int idx, int recv)
{
  int status = 1;
  char **servers;
  int *sockets;
  int *ids;
  int num_servers;
  if (recv) {
    servers = receive_servers;
    sockets = receive_sockets;
    ids = receive_ids;
    num_servers = num_receive_servers;
  } else {
    servers = send_servers;
    sockets = send_sockets;
    ids = send_ids;
    num_servers = num_send_servers;
  }
  if (idx >= num_servers || servers[idx] == 0)
    return;
  if (status & 1) {
    ids[idx] = ConnectToMds_(servers[idx]);
    if (ids[idx] <= 0) {
      printf("\nError connecting to %s", servers[idx]);
      perror("ConnectToMds_");
      ids[idx] = 0;
    } else {
      GetConnectionInfo_(ids[idx], 0, &sockets[idx], 0);
    }
  }
}

STATIC_ROUTINE int RemoteMDSEventAst(char const *eventnam_in,
			     void (*astadr) (void *, int, char *), void *astprm,
			     int *eventid)
{
  char *eventnam = eventName(eventnam_in);
  int status = 0;
  if (eventnam) {
    initializeRemote(1);
    status = eventAstRemote(eventnam, astadr, astprm, eventid);
    free(eventnam);
  }
  return status;
}

STATIC_ROUTINE int canEventRemote(int eventid)
{
  int status = 1, i;
  /* kill external thread before sending messages over the socket */
  if (status & 1) {
    //     KillHandler();
    for (i = 0; i < num_receive_servers; i++) {
      if (receive_ids[i] > 0)
	status = MdsEventCan_(receive_ids[i], getRemoteId(eventid, i));
    }
    //    startRemoteAstHandler();
  }
  return status;
}

STATIC_ROUTINE int RemoteMDSEventCan(int eventid)
{
  if (eventid < 0)
    return 0;

  initializeRemote(1);
  return canEventRemote(eventid);
}

struct MDSWfevent_struct {
  HANDLE event;
  int buflen;
  char *buffer;
  int *retlen;
};

STATIC_ROUTINE void MDSWfevent_ast(void *astparam, int data_len, char *data)
{
  struct MDSWfevent_struct *event = (struct MDSWfevent_struct *)astparam;
  if (event->buffer) {
    if (event->buflen > 0) {
      memset(event->buffer, 0, event->buflen);
      if (data_len > 0) {
	memcpy(event->buffer, data, min(data_len, event->buflen));
      }
    }
  }
  if (event->retlen)
    *event->retlen = data_len;
  SetEvent(event->event);
}

EXPORT int MDSWfevent(char const *evname, int buflen, char *data, int *datlen)
{
  return MDSWfeventTimed(evname, buflen, data, datlen, TIMEOUT);
}

EXPORT int MDSWfeventTimed(char const *evname, int buflen, char *data, int *datlen, int timeout)
{
  int eventid;
  int status;
  struct MDSWfevent_struct event;
  event.event = CreateEvent(NULL, TRUE, FALSE, NULL);
  event.buflen = buflen;
  event.buffer = data;
  event.retlen = datlen;
  MDSEventAst(evname, MDSWfevent_ast, (void *)&event, &eventid);
  status = WaitForSingleObject(event.event, (timeout > 0) ? timeout * 1000 : INFINITE);
  MDSEventCan(eventid);
  CloseHandle(event.event);
  return (status == WAIT_TIMEOUT) ? 0 : 1;
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
} *QueuedEvents = 0;

STATIC_THREADSAFE pthread_mutex_t eqMutex;
STATIC_THREADSAFE int eqMutex_initialized = 0;

static void CancelEventQueue(int eventid)
{
  struct eventQueueHeader *qh, *qh_p;
  struct eventQueue *q;
  LockMdsShrMutex(&eqMutex, &eqMutex_initialized);
  for (qh = QueuedEvents, qh_p = 0; qh && qh->eventid != eventid; qh_p = qh, qh = qh->next) ;
  if (qh) {
    if (qh_p) {
      qh_p->next = qh->next;
    } else {
      QueuedEvents = qh->next;
    }
    for (q = qh->event; q;) {
      struct eventQueue *this = q;
      q = q->next;
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

static void MDSEventQueue_ast(void *qh_in, int data_len, char *data)
{
  struct eventQueueHeader *qh = (struct eventQueueHeader *)qh_in;
  struct eventQueue *q;
  struct eventQueue *thisEvent = malloc(sizeof(struct eventQueue));
  thisEvent->data_len = data_len;
  thisEvent->next = 0;
  thisEvent->data = (data_len > 0) ? memcpy(malloc(data_len), data, data_len) : 0;
  LockMdsShrMutex(&eqMutex, &eqMutex_initialized);
  for (q = qh->event; q && q->next; q = q->next) ;
  if (q)
    q->next = thisEvent;
  else
    qh->event = thisEvent;
  SetEvent(qh->wakeup);
  UnlockMdsShrMutex(&eqMutex);
}

EXPORT int MDSQueueEvent(char const *evname, int *eventid)
{
  int status;
  struct eventQueueHeader *thisEventH = malloc(sizeof(struct eventQueueHeader));
  LockMdsShrMutex(&eqMutex, &eqMutex_initialized);
  status = MDSEventAst(evname, MDSEventQueue_ast, (void *)thisEventH, eventid);
  if (status & 1) {
    struct eventQueueHeader *qh;
    thisEventH->eventid = *eventid;
    thisEventH->event = 0;
    thisEventH->next = 0;
    thisEventH->wakeup = CreateEvent(NULL, TRUE, FALSE, NULL);
    for (qh = QueuedEvents; qh && qh->next; qh = qh->next) ;
    if (qh) {
      qh->next = thisEventH;
    } else {
      QueuedEvents = thisEventH;
    }
  } else {
    free(thisEventH);
  }
  UnlockMdsShrMutex(&eqMutex);
  return status;
}

EXPORT int MDSGetEventQueue(int eventid, int timeout, int *data_len, char **data)
{
  struct eventQueueHeader *qh;
  int waited = 0;
  int status;
 retry:
  status = 1;
  LockMdsShrMutex(&eqMutex, &eqMutex_initialized);
  for (qh = QueuedEvents; qh && qh->eventid != eventid; qh = qh->next) ;
  if (qh) {
    if (qh->event) {
      struct eventQueue *this = qh->event;
      *data = this->data;
      *data_len = this->data_len;
      qh->event = this->next;
      free(this);
      UnlockMdsShrMutex(&eqMutex);
    } else {
      UnlockMdsShrMutex(&eqMutex);
      if (waited == 0 && timeout >= 0) {
	status = WaitForSingleObject(qh->wakeup, (timeout > 0) ? timeout * 1000 : INFINITE);
	if (status == WAIT_TIMEOUT) {
	  *data_len = 0;
	  *data = 0;
	  status = 0;
	} else {
	  if (status == 0)
	    ResetEvent(qh->wakeup);
	  waited = 1;
	  goto retry;
	}
      } else {
	*data_len = 0;
	*data = 0;
	status = 0;
      }
    }
  } else {
    UnlockMdsShrMutex(&eqMutex);
    status = 2;
  }
  return status;
}

STATIC_ROUTINE int RemoteMDSEvent(char const *evname_in, int data_len, char *data)
{
  int status=0;
  char *evname=eventName(evname_in);
  if (evname) {
    initializeRemote(0);
    status = sendRemoteEvent(evname, data_len, data);
    free(evname);
  }
  return status;

}

STATIC_ROUTINE char *getEnvironmentVar(char const *name)
{
  char *trans = TranslateLogical(name);
  if (!trans || !*trans)
    return NULL;
  return trans;
}

#ifndef _WIN32
STATIC_ROUTINE int searchOpenServer(char *server)
/* Avoid doing MdsConnect on a server already connected before */
/* for now, allow socket duplications */
{
  return 0;
}
#endif

STATIC_ROUTINE void getServerDefinition(char const *env_var, char **servers, int *num_servers)
{
  int i, j;
  char *envname = getEnvironmentVar(env_var);
  char curr_name[256];
  if (!envname || !*envname) {
    *num_servers = 0;
    return;
  }
  i = *num_servers = 0;
  while (i < (int)strlen(envname)) {
    for (j = 0; i < (int)strlen(envname) && envname[i] != ';'; i++, j++)
      curr_name[j] = envname[i];
    curr_name[j] = 0;
    i++;
    servers[*num_servers] = malloc(strlen(curr_name) + 1);
    strcpy(servers[*num_servers], curr_name);
    (*num_servers)++;
  }
}

STATIC_ROUTINE unsigned __stdcall handleRemoteAst(void *p)
{
  int status = 1, i;
  Message *m;
  int selectstat;
  fd_set readfds;
  if (!(status & 1)) {
    printf("%s\n", MdsGetMsg(status));
    return status;
  }
  SetEvent(thread_alive_event);
  while (1) {

    WaitForSingleObject(external_event, INFINITE);
    ResetEvent(external_event);
    if (external_shutdown) {
      external_thread = 0;
      _endthreadex(0);
    }
    FD_ZERO(&readfds);
    for (i = 0; i < num_receive_servers; i++)
      if (receive_ids[i] > 0) {
	FD_SET(receive_sockets[i], &readfds);
      }
    selectstat = select(0, &readfds, 0, 0, 0);

    if (selectstat == -1) {
      perror("select error");
      return 0;
    }
    for (i = 0; i < num_receive_servers; i++) {
      if (receive_ids[i] > 0 && FD_ISSET(receive_sockets[i], &readfds)) {
	if (WSAEventSelect(receive_sockets[i], external_event, 0) != 0)
	  printf("Error in WSAEventSelect: %d\n", WSAGetLastError());
	if (ioctlsocket(receive_sockets[i], FIONBIO, &zero) != 0)
	  printf("Error in ioctlsocket: %d\n", WSAGetLastError());

	m = GetMdsMsgOOB_(receive_ids[i], &status);
	if (status == 1 && m->h.msglen == (sizeof(MsgHdr) + sizeof(MdsEventInfo))) {
	  MdsEventInfo *event = (MdsEventInfo *) m->bytes;
	  ((void (*)())(*event->astadr)) (event->astprm, 12, event->data);
	}
	if (m)
	  free(m);
	ResetEvent(external_event);
	if (WSAEventSelect(receive_sockets[i], external_event, FD_READ) != 0)
	  printf("Error in WSAEventSelect: %d\n", WSAGetLastError());

      }
    }
  }
  return 0;
}

STATIC_THREADSAFE pthread_mutex_t initMutex;
STATIC_THREADSAFE int initMutex_initialized = 0;

STATIC_ROUTINE void initializeRemote(int receive_events)
{
  STATIC_THREADSAFE int receive_initialized=0;
  STATIC_THREADSAFE int send_initialized=0;

  char *servers[256];
  int num_servers;
  int status = 1, i;
  LockMdsShrMutex(&initMutex, &initMutex_initialized);

  if (receive_initialized && receive_events) {
    UnlockMdsShrMutex(&initMutex);
    return;
  }
  if (send_initialized && !receive_events) {
    UnlockMdsShrMutex(&initMutex);
    return;
  }

  if (receive_events) {
    receive_initialized = 1;
    getServerDefinition("mds_event_server", servers, &num_servers);
    num_receive_servers = num_servers;
  } else {
    send_initialized = 1;
    getServerDefinition("mds_event_target", servers, &num_servers);
    num_send_servers = num_servers;
  }
  if (!external_event)
    if ((external_event = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
      printf("Error creating Event\n");

  if (num_servers > 0) {
    if (status & 1) {
      for (i = 0; i < num_servers; i++) {
	if (receive_events) {
	  if (!receive_ids[i])
	    receive_ids[i] = ConnectToMds_(servers[i]);
	  if (receive_ids[i] <= 0) {
	    printf("\nError connecting to %s", servers[i]);
	    perror("ConnectToMds_");
	    receive_ids[i] = 0;
	  } else {
	    GetConnectionInfo_(receive_ids[i], 0, &receive_sockets[i], 0);
	    receive_servers[i] = servers[i];
	  }
	} else {
	  if (!send_ids[i])
	    send_ids[i] = ConnectToMds_(servers[i]);
	  if (send_ids[i] <= 0) {
	    printf("\nError connecting to %s", servers[i]);
	    perror("ConnectToMds_");
	    send_ids[i] = 0;
	  } else {
	    GetConnectionInfo_(send_ids[i], 0, &send_sockets[i], 0);
	    send_servers[i] = servers[i];
	  }
	}
      }
    } else
      printf(MdsGetMsg(status));
  }
  UnlockMdsShrMutex(&initMutex);
}

STATIC_ROUTINE int eventAstRemote(char const *eventnam, void (*astadr) (), void *astprm, int *eventid)
{
  int status = 1, i;
  int curr_eventid;
  if (status & 1) {

    /*First pf all, kill thread listening to sockets */
    if (external_thread) {
      external_shutdown = 1;
      SetEvent(external_event);
      WaitForSingleObject(external_thread, INFINITE);
      external_shutdown = 0;

    }

    newRemoteId(eventid);
    for (i = 0; i < num_receive_servers; i++) {
      if (receive_ids[i] <= 0)
	ReconnectToServer(i, 1);
      if (receive_ids[i] > 0) {
	if (WSAEventSelect(receive_sockets[i], external_event, 0) != 0)
	  printf("Error in WSAEventSelect: %d\n", WSAGetLastError());
	if (ioctlsocket(receive_sockets[i], FIONBIO, &zero) != 0)
	  printf("Error in ioctlsocket: %d\n", WSAGetLastError());
	status = MdsEventAst_(receive_ids[i], eventnam, astadr, astprm, &curr_eventid);
	setRemoteId(*eventid, i, curr_eventid);
      }
    }
    external_count++;
  }
  if (!(status & 1))
    printf(MdsGetMsg(status));
  ResetEvent(external_event);
  for (i = 0; i < num_receive_servers; i++) {
    if (receive_ids[i] > 0) {
      if (WSAEventSelect(receive_sockets[i], external_event, FD_READ) != 0)
	printf("Error in WSAEventSelect: %d\n", WSAGetLastError());
    }
  }

  if (!thread_alive_event)
    thread_alive_event = CreateEvent(NULL, FALSE, FALSE, NULL);
  if ((external_thread =
       (HANDLE) _beginthreadex(NULL, 0, handleRemoteAst, NULL, 0, &threadID)) == (HANDLE) - 1)
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
STATIC_THREADSAFE int event_infoMutex_initialized = 0;

STATIC_ROUTINE void newRemoteId(int *id)
{
  int i;
  LockMdsShrMutex(&event_infoMutex, &event_infoMutex_initialized);
  for (i = 0; i < MAX_ACTIVE_EVENTS - 1 && event_info[i].used; i++) ;
  event_info[i].used = 1;
  event_info[i].external_ids = (int *)malloc(sizeof(int) * 256);
  *id = i;
  UnlockMdsShrMutex(&event_infoMutex);
}

STATIC_ROUTINE void setRemoteId(int id, int ofs, int evid)
{
  LockMdsShrMutex(&event_infoMutex, &event_infoMutex_initialized);
  event_info[id].external_ids[ofs] = evid;
  UnlockMdsShrMutex(&event_infoMutex);
}

STATIC_ROUTINE int getRemoteId(int id, int ofs)
{
  int retId;
  LockMdsShrMutex(&event_infoMutex, &event_infoMutex_initialized);
  retId = event_info[id].external_ids[ofs];
  UnlockMdsShrMutex(&event_infoMutex);
  return retId;
}

STATIC_ROUTINE int sendRemoteEvent(char const *evname, int data_len, char *data)
{
  int status = 1, i, tmp_status;
  char expression[256];
  struct descrip ansarg;
  struct descrip desc;

  desc.dtype = DTYPE_B;
  desc.ptr = data;
  desc.length = 1;
  desc.ndims = 1;
  desc.dims[0] = data_len;
  ansarg.ptr = 0;
  sprintf(expression, "setevent(\"%s\"%s)", evname, data_len > 0 ? ",$" : "");
  if (status & 1) {
    int reconnects = 0;
    for (i = 0; i < num_send_servers; i++) {
      if (send_ids[i] > 0) {
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
	  ReconnectToServer(i, 0);
	  reconnects++;
	  i--;
	}
      }
      if (ansarg.ptr)
	free(ansarg.ptr);
    }
  }
  return status;
}

#else
#ifdef _WIN32
#include "../servershr/servershrp.h"
#else
#include <sys/types.h>
#if USE_SEMAPHORE_H
#include <semaphore.h>
#else
#include <sys/sem.h>
#endif
//#include <sys/msg.h>
#include <sys/time.h>
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
STATIC_THREADSAFE pthread_t external_thread;	/* Thread for remote event handling */


STATIC_ROUTINE void ReconnectToServer(int idx, int recv)
{
  char **servers;
  int *sockets;
  int *ids;
  int num_servers;
  if (recv) {
    servers = receive_servers;
    sockets = receive_sockets;
    ids = receive_ids;
    num_servers = num_receive_servers;
  } else {
    servers = send_servers;
    sockets = send_sockets;
    ids = send_ids;
    num_servers = num_send_servers;
  }
  if (idx >= num_servers || servers[idx] == 0)
    return;
  DisconnectFromMds_(ids[idx]);
  ids[idx] = ConnectToMds_(servers[idx]);
  if (ids[idx] <= 0) {
    printf("\nError connecting to %s", servers[idx]);
    perror("ConnectToMds_");
    sockets[idx] = 0;
  } else
    GetConnectionInfo_(ids[idx], 0, &sockets[idx], 0);
}

/************* OS dependent part ******************/

STATIC_ROUTINE char *getEnvironmentVar(char const *name)
{
  char *trans = getenv(name);
  if (!trans || !*trans)
    return NULL;
  return trans;
}

STATIC_ROUTINE void handleRemoteAst();

STATIC_ROUTINE int createThread(pthread_t * thread, void (*rtn) (), void *par)
{
  int status = 1;
  if (pthread_create(thread, NULL, (void *(*)(void *))rtn, par) != 0) {
    status = 0;
    perror("createThread:pthread_create");
  }
  return status;
}

STATIC_ROUTINE void startRemoteAstHandler()
{
  if (pipe(fds) != 0) {
    fprintf(stderr, "Error creating pipes for AstHandler\n");
    return;
  }
  external_thread_created = createThread(&external_thread, handleRemoteAst, 0);
}



STATIC_THREADSAFE pthread_mutex_t event_infoMutex;
STATIC_THREADSAFE int event_infoMutex_initialized = 0;

/* eventid stuff: when using multiple event servers, the code has to deal with multiple eventids */
STATIC_THREADSAFE struct {
  int used;
  int local_id;
  int *external_ids;
} event_info[MAX_ACTIVE_EVENTS];

STATIC_ROUTINE void newRemoteId(int *id)
{
  int i;
  LockMdsShrMutex(&event_infoMutex, &event_infoMutex_initialized);
  for (i = 0; i < MAX_ACTIVE_EVENTS - 1 && event_info[i].used; i++) ;
  event_info[i].used = 1;
  event_info[i].external_ids = (int *)malloc(sizeof(int) * 256);
  *id = i;
  UnlockMdsShrMutex(&event_infoMutex);
}

STATIC_ROUTINE void setRemoteId(int id, int ofs, int evid)
{
  LockMdsShrMutex(&event_infoMutex, &event_infoMutex_initialized);
  event_info[id].external_ids[ofs] = evid;
  UnlockMdsShrMutex(&event_infoMutex);
}

STATIC_ROUTINE int getRemoteId(int id, int ofs)
{
  int retId;
  LockMdsShrMutex(&event_infoMutex, &event_infoMutex_initialized);
  retId = event_info[id].external_ids[ofs];
  UnlockMdsShrMutex(&event_infoMutex);
  return retId;
}

STATIC_ROUTINE void getServerDefinition(char const *env_var, char **servers, int *num_servers)
{
  unsigned int i, j;
  char *envname = getEnvironmentVar(env_var);
  char curr_name[256];
  if (!envname || !*envname) {
    *num_servers = 0;
    return;
  }
  i = *num_servers = 0;
  while (i < strlen(envname)) {
    for (j = 0; i < strlen(envname) && envname[i] != ';'; i++, j++)
      curr_name[j] = envname[i];
    curr_name[j] = 0;
    i++;
    servers[*num_servers] = malloc(strlen(curr_name) + 1);
    strcpy(servers[*num_servers], curr_name);
    (*num_servers)++;
  }
}

#ifdef GLOBUS
STATIC_ROUTINE void handleRemoteEvent(int sock);

STATIC_ROUTINE void KillHandler()
{
}

STATIC_ROUTINE void handleRemoteAst()
{
  Poll(handleRemoteEvent);
}

STATIC_ROUTINE void handleRemoteEvent(int sock)
{
  char buf[16];
  STATIC_CONSTANT struct descriptor
  int status = 1, i;
  Message *m;
  m = GetMdsMsg_(sock, &status);
  if (status == 1 && m->h.msglen == (sizeof(MsgHdr) + sizeof(MdsEventInfo))) {
    MdsEventInfo *event = (MdsEventInfo *) m->bytes;
    ((void (*)())(*event->astadr)) (event->astprm, 12, event->data);
  }
  if (m)
    free(m);
}

#else

STATIC_CONSTANT void KillHandler()
{
  void *dummy;
  external_shutdown = 1;
  write(fds[1], "x", 1) == 1 ? 0 : -1;
  pthread_join(external_thread, &dummy);
  close(fds[0]);
  close(fds[1]);
  external_shutdown = 0;
  external_thread_created = 0;
}

STATIC_ROUTINE void handleRemoteAst()
{
  char buf[16];
  int status = 1, i;
  Message *m;
  int tablesize = FD_SETSIZE, selectstat;
  fd_set readfds;

  if (!(status & 1)) {
    printf("%s\n", MdsGetMsg(status));
    return;
  }
  while (1) {
    FD_ZERO(&readfds);
    for (i = 0; i < num_receive_servers; i++)
      if (receive_sockets[i])
	FD_SET(receive_sockets[i], &readfds);
    FD_SET(fds[0], &readfds);
    selectstat = select(tablesize, &readfds, 0, 0, 0);
    if (selectstat == -1) {
      perror("select error");
      return;
    }
    if (external_shutdown) {
      status = read(fds[0], buf, 1) == 1 ? 0 : -1;
      pthread_exit(0);
    }
    for (i = 0; i < num_receive_servers; i++) {
      if (receive_ids[i] > 0 && FD_ISSET(receive_sockets[i], &readfds)) {
	m = GetMdsMsg_(receive_ids[i], &status);
	if (status == 1 && m->h.msglen == (sizeof(MsgHdr) + sizeof(MdsEventInfo))) {
	  MdsEventInfo *event = (MdsEventInfo *) m->bytes;
	  ((void (*)())(*event->astadr)) (event->astprm, 12, event->data);
	}
	if (m)
	  free(m);
	else {
	  fprintf(stderr, "Error reading from event server, events may be disabled\n");
	  receive_sockets[i] = 0;
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
STATIC_THREADSAFE int initMutex_initialized = 0;

STATIC_ROUTINE void initializeRemote(int receive_events)
{
  STATIC_THREADSAFE int receive_initialized;
  STATIC_THREADSAFE int send_initialized;

  char *servers[256];
  int num_servers;
  int status = 1, i;

  LockMdsShrMutex(&initMutex, &initMutex_initialized);

  if (receive_initialized && receive_events) {
    UnlockMdsShrMutex(&initMutex);
    return;
  }
  if (send_initialized && !receive_events) {
    UnlockMdsShrMutex(&initMutex);
    return;
  }

  if (receive_events) {
    receive_initialized = 1;
    getServerDefinition("mds_event_server", servers, &num_servers);
    num_receive_servers = num_servers;
  } else {
    send_initialized = 1;
    getServerDefinition("mds_event_target", servers, &num_servers);
    num_send_servers = num_servers;
  }
  if (num_servers > 0) {
    if (status & 1) {
      if (external_thread_created)
	KillHandler();
      for (i = 0; i < num_servers; i++) {
	if (receive_events) {
	  receive_ids[i] = searchOpenServer(servers[i]);
	  if (receive_ids[i] <= 0)
	    receive_ids[i] = ConnectToMds_(servers[i]);
	  if (receive_ids[i] <= 0) {
	    printf("\nError connecting to %s", servers[i]);
	    perror("ConnectToMds_");
	    receive_ids[i] = 0;
	  } else {
#ifdef GLOBUS
	    RegisterRead_(send_sockets[i]);
#endif
	    GetConnectionInfo_(receive_ids[i], 0, &receive_sockets[i], 0);
	    receive_servers[i] = servers[i];
	  }
	} else {
	  send_ids[i] = searchOpenServer(servers[i]);
	  if (send_ids[i] <= 0)
	    send_ids[i] = ConnectToMds_(servers[i]);
	  if (send_ids[i] <= 0) {
	    printf("\nError connecting to %s", servers[i]);
	    perror("ConnectToMds_");
	    send_ids[i] = 0;
	  } else {
	    send_servers[i] = servers[i];
	    GetConnectionInfo_(send_ids[i], 0, &send_sockets[i], 0);
	  }
	}
      }
      startRemoteAstHandler();
    } else
      printf("%s\n", MdsGetMsg(status));
  }
  UnlockMdsShrMutex(&initMutex);
}

STATIC_ROUTINE int eventAstRemote(char const *eventnam, void (*astadr) (), void *astprm, int *eventid)
{
  int status = 1, i;
  int curr_eventid;
  if (status & 1) {
/* if external_thread running, it must be killed before sending messages over socket */
    if (external_thread_created)
      KillHandler();
    newRemoteId(eventid);
    for (i = 0; i < num_receive_servers; i++) {
      if (receive_ids[i] <= 0)
	ReconnectToServer(i, 1);
      if (receive_ids[i] > 0) {
	status = MdsEventAst_(receive_ids[i], eventnam, astadr, astprm, &curr_eventid);
#ifdef GLOBUS
	if (status & 1)
	  RegisterRead_(receive_ids[i]);
#endif
	setRemoteId(*eventid, i, curr_eventid);
      }
    }
/* now external thread must be created in any case */
    if (status & 1) {
      startRemoteAstHandler();
      external_count++;
    }
  }
  if (!(status & 1))
    printf("%s\n", MdsGetMsg(status));
  return status;
}

struct wfevent_thread_cond {
  int active;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  int buflen;
  char *data;
  int *datlen;
};

STATIC_ROUTINE void EventHappened(void *astprm, int len, char *data)
{
  struct wfevent_thread_cond *t = (struct wfevent_thread_cond *)astprm;
  pthread_mutex_lock(&t->mutex);
  if (t->active) {
    if (t->buflen && t->data)
      memcpy(t->data, data, (t->buflen > len) ? len : t->buflen);
    if (t->datlen)
      *t->datlen = len;
  }
  pthread_cond_signal(&t->cond);
  pthread_mutex_unlock(&t->mutex);
}

EXPORT int MDSWfevent(char const *evname, int buflen, char *data, int *datlen)
{
  return MDSWfeventTimed(evname, buflen, data, datlen, TIMEOUT);
}

EXPORT int MDSWfeventTimed(char const *evname, int buflen, char *data, int *datlen, int timeout)
{
  int eventid = -1;
  int status;
  struct wfevent_thread_cond t = { 1 };
  pthread_mutex_init(&t.mutex, NULL);
  pthread_cond_init(&t.cond, NULL);
  t.buflen = buflen;
  t.data = data;
  t.datlen = datlen;
  MDSEventAst(evname, EventHappened, &t, &eventid);
  pthread_mutex_lock(&t.mutex);
  if (timeout > 0) {
    static struct timespec abstime;
#ifdef HAVE_CLOCK_GETTIME
    clock_gettime(CLOCK_REALTIME, &abstime);
#else
    abstime.tv_sec = time(0);
    abstime.tv_nsec = 0;
#endif
    abstime.tv_sec += timeout;
    status = pthread_cond_timedwait(&t.cond, &t.mutex, &abstime);
  } else {
    status = pthread_cond_wait(&t.cond, &t.mutex);
  }
  pthread_mutex_unlock(&t.mutex);
  pthread_mutex_lock(&t.mutex);
  t.active = 0;
  MDSEventCan(eventid);
  pthread_mutex_unlock(&t.mutex);
  pthread_cond_destroy(&t.cond);
  pthread_mutex_destroy(&t.mutex);
  return (status == 0);
}

struct eventQueue {
  int data_len;
  char *data;
  struct eventQueue *next;
};

struct eventQueueHeader {
  int eventid;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  struct eventQueue *event;
  struct eventQueueHeader *next;
} *QueuedEvents = 0;

STATIC_THREADSAFE pthread_mutex_t eqMutex;
STATIC_THREADSAFE int eqMutex_initialized = 0;

static void CancelEventQueue(int eventid)
{
  struct eventQueueHeader *qh, *qh_p;
  struct eventQueue *q;
  LockMdsShrMutex(&eqMutex, &eqMutex_initialized);
  for (qh = QueuedEvents, qh_p = 0; qh && qh->eventid != eventid; qh_p = qh, qh = qh->next) ;
  if (qh) {
    if (qh_p) {
      qh_p->next = qh->next;
    } else {
      QueuedEvents = qh->next;
    }
    for (q = qh->event; q;) {
      struct eventQueue *this = q;
      q = q->next;
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

static void MDSEventQueue_ast(void *qh_in, int data_len, char *data)
{
  struct eventQueueHeader *qh = (struct eventQueueHeader *)qh_in;
  struct eventQueue *q;
  struct eventQueue *thisEvent = malloc(sizeof(struct eventQueue));
  thisEvent->data_len = data_len;
  thisEvent->next = 0;
  thisEvent->data = (data_len > 0) ? memcpy(malloc(data_len), data, data_len) : (void *)0;
  LockMdsShrMutex(&eqMutex, &eqMutex_initialized);
  for (q = qh->event; q && q->next; q = q->next) ;
  if (q)
    q->next = thisEvent;
  else
    qh->event = thisEvent;
  pthread_mutex_lock(&qh->mutex);
  pthread_cond_signal(&qh->cond);
  pthread_mutex_unlock(&qh->mutex);
  UnlockMdsShrMutex(&eqMutex);
}

EXPORT int MDSQueueEvent(char const *evname, int *eventid)
{
  int status;
  struct eventQueueHeader *thisEventH = malloc(sizeof(struct eventQueueHeader));
  LockMdsShrMutex(&eqMutex, &eqMutex_initialized);
  status = MDSEventAst(evname, MDSEventQueue_ast, (void *)thisEventH, eventid);
  if (status & 1) {
    struct eventQueueHeader *qh;
    thisEventH->eventid = *eventid;
    thisEventH->event = 0;
    thisEventH->next = 0;
    pthread_mutex_init(&thisEventH->mutex, NULL);
    pthread_cond_init(&thisEventH->cond, NULL);
    for (qh = QueuedEvents; qh && qh->next; qh = qh->next) ;
    if (qh) {
      qh->next = thisEventH;
    } else {
      QueuedEvents = thisEventH;
    }
  } else {
    free(thisEventH);
  }
  UnlockMdsShrMutex(&eqMutex);
  return status;
}

EXPORT int MDSGetEventQueue(int eventid, int timeout, int *data_len, char **data)
{
  struct eventQueueHeader *qh;
  int waited = 0;
  int status = 1;
 retry:
  LockMdsShrMutex(&eqMutex, &eqMutex_initialized);
  for (qh = QueuedEvents; qh && qh->eventid != eventid; qh = qh->next) ;
  if (qh) {
    if (qh->event) {
      struct eventQueue *this = qh->event;
      *data = this->data;
      *data_len = this->data_len;
      qh->event = this->next;
      free(this);
      UnlockMdsShrMutex(&eqMutex);
    } else {
      UnlockMdsShrMutex(&eqMutex);
      if (waited == 0 && timeout >= 0) {
	pthread_mutex_lock(&qh->mutex);
	if (timeout > 0) {
	  static struct timespec abstime;
#ifdef HAVE_CLOCK_GETTIME
	  clock_gettime(CLOCK_REALTIME, &abstime);
#else
	  abstime.tv_sec = time(0);
	  abstime.tv_nsec = 0;
#endif
	  abstime.tv_sec += timeout;
	  status = pthread_cond_timedwait(&qh->cond, &qh->mutex, &abstime);
	} else {
	  status = pthread_cond_wait(&qh->cond, &qh->mutex);
	}
	pthread_mutex_unlock(&qh->mutex);
	if (status != 0) {
	  *data_len = 0;
	  *data = 0;
	  status = 0;
	} else {
	  status = 1;
	  waited = 1;
	  goto retry;
	}
      } else {
	*data_len = 0;
	*data = 0;
	status = 0;
      }
    }
  } else {
    UnlockMdsShrMutex(&eqMutex);
    status = 2;
  }
  return status;
}

int RemoteMDSEventAst(char const *eventnam_in, void (*astadr) (), void *astprm, int *eventid)
{
  int status = 0;
  char *eventnam = eventName(eventnam_in);
  *eventid = -1;
  if (eventnam) {
    initializeRemote(1);
    status = eventAstRemote(eventnam, astadr, astprm, eventid);
    free(eventnam);
  }
  return status;
}

STATIC_ROUTINE int canEventRemote(int eventid)
{
  int status = 1, i;
  /* kill external thread before sending messages over the socket */
  if (status & 1) {
    KillHandler();
    for (i = 0; i < num_receive_servers; i++) {
      if (receive_ids[i] > 0)
	status = MdsEventCan_(receive_ids[i], getRemoteId(eventid, i));
    }
    startRemoteAstHandler();
  }
  return status;
}

int RemoteMDSEventCan(int eventid)
{
  if (eventid < 0)
    return 0;

  initializeRemote(1);
  return canEventRemote(eventid);
}

STATIC_ROUTINE int sendRemoteEvent(char const *evname, int data_len, char *data)
{
  int status = 1, i, tmp_status;
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
  sprintf(expression, "setevent(\"%s\"%s)", evname, data_len > 0 ? ",$" : "");
  if (status & 1) {
    int reconnects = 0;
    for (i = 0; i < num_send_servers; i++) {
      if (send_ids[i] > 0) {
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
	  ReconnectToServer(i, 0);
	  reconnects++;
	  i--;
	}
      }
      if (ansarg.ptr)
	free(ansarg.ptr);
    }
  }
  return status;
}


int RemoteMDSEvent(char const *evname_in, int data_len, char *data)
{
  int j;
  unsigned int u;
  char *evname;
  int status = 1;
  initializeRemote(0);
  evname = strdup(evname_in);
  for (u = 0, j = 0; u < strlen(evname); u++) {
    if (evname[u] != 32)
      evname[j++] = toupper(evname[u]);
  }
  evname[j] = 0;
  status = sendRemoteEvent(evname, data_len, data);
  free(evname);
  return status;
}


#endif

EXPORT int MDSEventAst(char const *eventNameIn, void (*astadr) (void *, int, char *), void *astprm,
		int *eventid)
{
  char *eventName = malloc(strlen(eventNameIn) + 1);
  unsigned int i, j;
  int status;
  for (i = 0, j = 0; i < strlen(eventNameIn); i++) {
    if (eventNameIn[i] != 32)
      eventName[j++] = toupper(eventNameIn[i]);
  }
  eventName[j] = 0;
  if (getenv("mds_event_server"))
    status = RemoteMDSEventAst(eventName, astadr, astprm, eventid);
  else
    status = MDSUdpEventAst(eventName, astadr, astprm, eventid);
  free(eventName);
  return status;
}

EXPORT int MDSEvent(char const *eventNameIn, int bufLen, char *buf)
{
  char *eventName = alloca(strlen(eventNameIn) + 1);
  unsigned int i, j;
  int status;
  for (i = 0, j = 0; i < strlen(eventNameIn); i++) {
    if (eventNameIn[i] != 32)
      eventName[j++] = toupper(eventNameIn[i]);
  }
  eventName[j] = 0;
  if (getenv("mds_event_target"))
    status = RemoteMDSEvent(eventName, bufLen, buf);
  else
    status = MDSUdpEvent(eventName, bufLen, buf);
  return status;
}

EXPORT int MDSEventCan(int id)
{
  int status;
  if (getenv("mds_event_server"))
    status = RemoteMDSEventCan(id);
  else
    status = MDSUdpEventCan(id);
  CancelEventQueue(id);
  return status;
}
