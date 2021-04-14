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
#include "../mdstcpip/mdsip_connections.h"
#include <errno.h>
#include <fcntl.h>
#include <libroutines.h>
#include <mds_stdarg.h>
#include <mdsplus/mdsconfig.h>
#include <mdsshr.h>
#include <status.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif
#include "mdsshrp.h"

#ifdef _WIN32
#define pipe(fds) _pipe(fds, 4096, _O_BINARY)
#endif

#define _GNU_SOURCE /* glibc2 needs this */
#include <ctype.h>
#include <sys/types.h>
#define MAX_ACTIVE_EVENTS                                                      \
  2000 /* Maximum number events concurrently dealt with by processes */

#ifndef EVENT_THREAD_STACK_SIZE_MIN
#define EVENT_THREAD_STACK_SIZE_MIN 102400
#endif

static int receive_ids[256];       /* Connection to receive external events  */
static int send_ids[256];          /* Connection to send external events  */
static int receive_sockets[256];   /* Socket to receive external events  */
static int send_sockets[256];      /* Socket to send external events  */
static char *receive_servers[256]; /* Receive server names */
static char *send_servers[256];    /* Send server names */
static int external_shutdown =
    0; /* flag to request remote events thread termination */
static int external_count = 0;      /* remote event pendings count */
static int num_receive_servers = 0; /* numer of external event sources */
static int num_send_servers = 0;    /* numer of external event destination */

static int external_thread_created =
    0;             /* Thread for remot event handling flag */
static int fds[2]; /* file descriptors used by the pipe */

static int eventAstRemote(char const *eventnam, void (*astadr)(), void *astprm,
                          int *eventid);
static void initializeRemote(int receive_events);

static int ConnectToMds_(const char *const host) {
  static int (*rtn)(const char *const host) = 0;
  int status = (rtn == 0) ? LibFindImageSymbol_C("MdsIpShr", "ConnectToMds",
                                                 (void **)&rtn)
                          : 1;
  if
    STATUS_OK { return rtn(host); }
  return -1;
}

static int DisconnectFromMds_(const int id) {
  static int (*rtn)() = 0;
  int status =
      (rtn == 0)
          ? LibFindImageSymbol_C("MdsIpShr", "DisconnectFromMds", (void **)&rtn)
          : 1;
  if
    STATUS_OK { return rtn(id); }
  return -1;
}

static void *GetConnectionInfo_(const int id, char **const name,
                                int *const readfd, size_t *const len) {
  static void *(*rtn)() = 0;
  int status =
      (rtn == 0)
          ? LibFindImageSymbol_C("MdsIpShr", "GetConnectionInfo", (void **)&rtn)
          : 1;
  if
    STATUS_OK { return rtn(id, name, readfd, len); }
  return 0;
}

static int MdsEventAst_(const int conid, char const *const eventnam,
                        void (*const astadr)(), void *const astprm,
                        int *const eventid) {
  static int (*rtn)() = 0;
  int status = (rtn == 0) ? LibFindImageSymbol_C("MdsIpShr", "MdsEventAst",
                                                 (void **)&rtn)
                          : 1;
  if
    STATUS_OK { return rtn(conid, eventnam, astadr, astprm, eventid); }
  return 0;
}

static Message *GetMdsMsg_(const int id, const int *const stat) {
  static Message *(*rtn)() = 0;
  int status =
      (rtn == 0) ? LibFindImageSymbol_C("MdsIpShr", "GetMdsMsg", (void **)&rtn)
                 : 1;
  if
    STATUS_OK { return rtn(id, stat); }
  return 0;
}

static int MdsEventCan_(const int id, const int eid) {
  static int (*rtn)() = 0;
  int status = (rtn == 0) ? LibFindImageSymbol_C("MdsIpShr", "MdsEventCan",
                                                 (void **)&rtn)
                          : 1;
  if
    STATUS_OK { return rtn(id, eid); }
  return 0;
}

static int MdsValue_(const int id, const char *const exp,
                     struct descrip *const d1, struct descrip *const d2,
                     struct descrip *const d3) {
  static int (*rtn)() = 0;
  int status = (rtn == 0)
                   ? LibFindImageSymbol_C("MdsIpShr", "MdsValue", (void **)&rtn)
                   : 1;
  if
    STATUS_OK { return rtn(id, exp, d1, d2, d3); }
  return 0;
}

#ifdef GLOBUS
static int RegisterRead_(const int conid) {
  int status = 1;
  static int (*rtn)(int) = 0;
  if (rtn == 0)
    status = LibFindImageSymbol_C("MdsIpShr", "RegisterRead", (void **)&rtn);
  if
    STATUS_NOT_OK {
      printf("%s\n", MdsGetMsg(status));
      return status;
    }
  return rtn(conid);
}
#endif

static char *eventName(const char *const eventnam_in) {
  size_t i, j;
  char *eventnam = 0;
  if (eventnam_in) {
    eventnam = strdup(eventnam_in);
    for (i = 0, j = 0; i < strlen(eventnam); i++) {
      if (eventnam[i] != 32)
        eventnam[j++] = (char)toupper(eventnam[i]);
    }
    eventnam[j] = 0;
    if (strlen(eventnam) == 0) {
      free(eventnam);
      eventnam = 0;
    }
  }
  return eventnam;
}

#ifndef HAVE_VXWORKS_H

#ifdef _WIN32
#include "../servershr/servershrp.h"
#else
#include <signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#ifndef HAVE_VXWORKS_H
#include <pthread.h>
#endif
#ifdef __hpux
#undef select
#endif
#include <unistd.h>
#endif //_WIN32

/* MDsEvent: UNIX and Win32 implementation of MDS Events */
static pthread_t external_thread; /* Thread for remote event handling */

static void ReconnectToServer(int idx, int recv) {
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

static char *getEnvironmentVar(char const *name) {
  char *trans = getenv(name);
  if (!trans || !*trans)
    return NULL;
  return trans;
}

static void *handleRemoteAst(void *);

static int createThread(pthread_t *thread, void *(*rtn)(void *), void *par) {
  int s, status = 1;
  size_t ssize;
  pthread_attr_t attr;
  s = pthread_attr_init(&attr);
  if (s != 0) {
    perror("pthread_attr_init");
    return 0;
  }
  pthread_attr_getstacksize(&attr, &ssize);
  if (ssize < EVENT_THREAD_STACK_SIZE_MIN) {
    s = pthread_attr_setstacksize(&attr, EVENT_THREAD_STACK_SIZE_MIN);
    if (s != 0) {
      perror("pthread_attr_setstacksize");
      return 0;
    }
  }
  if (pthread_create(thread, &attr, (void *(*)(void *))rtn, par) != 0) {
    status = 0;
    perror("createThread:pthread_create");
  }
  return status;
}

static void startRemoteAstHandler() {
  if (pipe(fds) != 0) {
    fprintf(stderr, "Error creating pipes for AstHandler\n");
    return;
  }
  external_thread_created = createThread(&external_thread, handleRemoteAst, 0);
}

/* eventid stuff: when using multiple event servers, the code has to deal with
 * multiple eventids */
static pthread_mutex_t event_info_lock = PTHREAD_MUTEX_INITIALIZER;
#define EVENT_INFO_LOCK pthread_mutex_lock(&event_info_lock)
#define EVENT_INFO_UNLOCK pthread_mutex_unlock(&event_info_lock)
static struct {
  int used;
  int local_id;
  int *external_ids;
} event_info[MAX_ACTIVE_EVENTS];

static void newRemoteId(int *id) {
  int i;
  EVENT_INFO_LOCK;
  for (i = 0; i < MAX_ACTIVE_EVENTS - 1 && event_info[i].used; i++)
    ;
  event_info[i].used = 1;
  event_info[i].external_ids = (int *)malloc(sizeof(int) * 256);
  *id = i;
  EVENT_INFO_UNLOCK;
}

static void setRemoteId(int id, int ofs, int evid) {
  EVENT_INFO_LOCK;
  event_info[id].external_ids[ofs] = evid;
  EVENT_INFO_UNLOCK;
}

static int getRemoteId(int id, int ofs) {
  int retId;
  EVENT_INFO_LOCK;
  retId = event_info[id].external_ids[ofs];
  EVENT_INFO_UNLOCK;
  return retId;
}

static void getServerDefinition(char const *env_var, char **servers,
                                int *num_servers) {
  unsigned int i, j;
  char *envname = getEnvironmentVar(env_var);
  char curr_name[256];
  if (!envname || !*envname) {
    *num_servers = 0;
    return;
  }
  i = 0;
  *num_servers = 0;
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
static void handleRemoteEvent(int conid);

static void KillHandler() {}

static void *handleRemoteAst(void *arg __attribute__((unused))) {
  Poll(handleRemoteEvent);
  return NULL;
}

static void handleRemoteEvent(int conid) {
  char buf[16];
  static struct descriptor int status = 1, i;
  Message *m;
  m = GetMdsMsg_(sock, &status);
  if (status == 1 && m->h.msglen == (sizeof(MsgHdr) + sizeof(MdsEventInfo))) {
    MdsEventInfo *event = (MdsEventInfo *)m->bytes;
    ((void (*)())(*event->astadr))(event->astprm, 12, event->data);
  }
  free(m);
}

#else // GLOBUS
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"

static void KillHandler() {
  void *dummy;
  external_shutdown = 1;
  write(fds[1], "x", 1);
  pthread_join(external_thread, &dummy);
  close(fds[0]);
  close(fds[1]);
  external_shutdown = 0;
  external_thread_created = 0;
}

static void *handleRemoteAst(void *arg __attribute__((unused))) {
  INIT_STATUS;
  char buf[16];
  int i;
  Message *m;
  int selectstat;
  fd_set readfds;
  while (1) {
    FD_ZERO(&readfds);
    for (i = 0; i < num_receive_servers; i++)
      if (receive_sockets[i])
        FD_SET(receive_sockets[i], &readfds);
    FD_SET(fds[0], &readfds);
    selectstat = select(FD_SETSIZE, &readfds, 0, 0, 0);
    if (selectstat == -1) {
      perror("select error");
      return NULL;
    }
    if (external_shutdown) {
      read(fds[0], buf, 1);
      pthread_exit(0);
    }
    for (i = 0; i < num_receive_servers; i++) {
      if (receive_ids[i] > 0 && FD_ISSET(receive_sockets[i], &readfds)) {
        m = GetMdsMsg_(receive_ids[i], &status);
        if (STATUS_OK &&
            m->h.msglen == (sizeof(MsgHdr) + sizeof(MdsEventInfo))) {
          MdsEventInfo *event = (MdsEventInfo *)m->bytes;
          ((void (*)())(*event->astadr))(event->astprm, 12, event->data);
        }
        if (m)
          free(m);
        else {
          fprintf(stderr,
                  "Error reading from event server, events may be disabled\n");
          receive_sockets[i] = 0;
        }
      }
    }
  }
  return NULL;
}
#pragma GCC diagnostic pop

#endif // GLOBUS

static int searchOpenServer(char *server __attribute__((unused)))
/* Avoid doing MdsConnect on a server already connected before
 * for now, allow socket duplications
 */
{
  return 0;
}

static pthread_mutex_t init_lock = PTHREAD_MUTEX_INITIALIZER;
static void initializeRemote(int receive_events) {
  static int receive_initialized;
  static int send_initialized;
  pthread_mutex_lock(&init_lock);
  if (receive_events ? receive_initialized : send_initialized) {
    pthread_mutex_unlock(&init_lock);
    return;
  }
  pthread_cleanup_push((void *)pthread_mutex_unlock, (void *)&init_lock);

  char *servers[256];
  int num_servers;
  int status = 1, i;

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
    if
      STATUS_OK {
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
      }
    else
      printf("%s\n", MdsGetMsg(status));
  }
  pthread_cleanup_pop(1);
}

static int eventAstRemote(char const *eventnam, void (*astadr)(), void *astprm,
                          int *eventid) {
  int status = 1, i;
  int curr_eventid;
  if
    STATUS_OK {
      /* if external_thread running, it must be killed before sending messages
       * over socket */
      if (external_thread_created)
        KillHandler();
      newRemoteId(eventid);
      for (i = 0; i < num_receive_servers; i++) {
        if (receive_ids[i] <= 0)
          ReconnectToServer(i, 1);
        if (receive_ids[i] > 0) {
          status = MdsEventAst_(receive_ids[i], eventnam, astadr, astprm,
                                &curr_eventid);
#ifdef GLOBUS
          if
            STATUS_OK
          RegisterRead_(receive_ids[i]);
#endif
          setRemoteId(*eventid, i, curr_eventid);
        }
      }
      /* now external thread must be created in any case */
      if
        STATUS_OK {
          startRemoteAstHandler();
          external_count++;
        }
    }
  if
    STATUS_NOT_OK
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

static void EventHappened(void *astprm, int len, char *data) {
  struct wfevent_thread_cond *t = (struct wfevent_thread_cond *)astprm;
  pthread_mutex_lock(&t->mutex);
  if (t->active) {
    if (t->buflen && t->data)
      memcpy(t->data, data, (size_t)((t->buflen > len) ? len : t->buflen));
    if (t->datlen)
      *t->datlen = len;
  }
  pthread_cond_signal(&t->cond);
  pthread_mutex_unlock(&t->mutex);
}

static int TIMEOUT = 0;

EXPORT int MDSSetEventTimeout(const int seconds) {
  int old_timeout;
  pthread_mutex_lock(&init_lock);
  old_timeout = TIMEOUT;
  TIMEOUT = seconds;
  pthread_mutex_unlock(&init_lock);
  return old_timeout;
}

EXPORT int MDSWfevent(char const *evname, int buflen, char *data, int *datlen) {
  int timeout;
  pthread_mutex_lock(&init_lock);
  timeout = TIMEOUT;
  pthread_mutex_unlock(&init_lock);
  return MDSWfeventTimed(evname, buflen, data, datlen, timeout);
}

EXPORT int MDSWfeventTimed(char const *evname, int buflen, char *data,
                           int *datlen, int timeout) {
  int eventid = -1;
  int status;
  struct wfevent_thread_cond t = {0};
  t.active = 1;
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
  pthread_cond_destroy(&t.cond);
  pthread_mutex_unlock(&t.mutex);
  pthread_mutex_destroy(&t.mutex);
  return (status == 0);
}

static pthread_mutex_t event_queue_lock = PTHREAD_MUTEX_INITIALIZER;
#define EVENT_QUEUE_LOCK                                                       \
  pthread_mutex_lock(&event_queue_lock);                                       \
  pthread_cleanup_push((void *)pthread_mutex_unlock, (void *)&event_queue_lock)
#define EVENT_QUEUE_UNLOCK pthread_cleanup_pop(1)
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

static void CancelEventQueue(int eventid) {
  struct eventQueueHeader *qh, *qh_p;
  struct eventQueue *q;
  EVENT_QUEUE_LOCK;
  for (qh = QueuedEvents, qh_p = 0; qh && qh->eventid != eventid;
       qh_p = qh, qh = qh->next)
    ;
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
  EVENT_QUEUE_UNLOCK;
}

static void MDSEventQueue_ast(void *const qh_in, const int data_len,
                              char *const data) {
  EVENT_QUEUE_LOCK;
  struct eventQueueHeader *qh = (struct eventQueueHeader *)qh_in;
  struct eventQueue *q;
  struct eventQueue *thisEvent = malloc(sizeof(struct eventQueue));
  thisEvent->data_len = data_len;
  thisEvent->next = 0;
  thisEvent->data =
      (data_len > 0) ? memcpy(malloc((size_t)data_len), data, (size_t)data_len)
                     : NULL;
  for (q = qh->event; q && q->next; q = q->next)
    ;
  if (q)
    q->next = thisEvent;
  else
    qh->event = thisEvent;
  pthread_mutex_lock(&qh->mutex);
  pthread_cond_signal(&qh->cond);
  pthread_mutex_unlock(&qh->mutex);
  EVENT_QUEUE_UNLOCK;
}

EXPORT int MDSQueueEvent(const char *const evname, int *const eventid) {
  int status;
  EVENT_QUEUE_LOCK;
  struct eventQueueHeader *thisEventH = malloc(sizeof(struct eventQueueHeader));
  status = MDSEventAst(evname, MDSEventQueue_ast, (void *)thisEventH, eventid);
  if
    STATUS_OK {
      struct eventQueueHeader *qh;
      thisEventH->eventid = *eventid;
      thisEventH->event = 0;
      thisEventH->next = 0;
      pthread_mutex_init(&thisEventH->mutex, NULL);
      pthread_cond_init(&thisEventH->cond, NULL);
      for (qh = QueuedEvents; qh && qh->next; qh = qh->next)
        ;
      if (qh) {
        qh->next = thisEventH;
      } else {
        QueuedEvents = thisEventH;
      }
    }
  else {
    free(thisEventH);
  }
  EVENT_QUEUE_UNLOCK;
  return status;
}

EXPORT int MDSGetEventQueue(const int eventid, const int timeout,
                            int *const data_len, char **const data) {
  int state, unlock;
  pthread_cleanup_push((void *)pthread_mutex_unlock, (void *)&event_queue_lock);
  unlock = FALSE;
  state = 1;
  struct eventQueueHeader *qh;
  int waited = FALSE;
retry:
  pthread_mutex_lock(&event_queue_lock);
  unlock = TRUE;
  for (qh = QueuedEvents; qh && qh->eventid != eventid; qh = qh->next)
    ;
  if (qh) {
    if (qh->event) {
      struct eventQueue *this = qh->event;
      *data = this->data;
      *data_len = this->data_len;
      qh->event = this->next;
      free(this);
    } else {
      if (!waited && timeout >= 0) {
        pthread_mutex_lock(&qh->mutex);
        pthread_mutex_unlock(&event_queue_lock);
        unlock = FALSE;
        if (timeout > 0) {
          static struct timespec abstime;
#ifdef HAVE_CLOCK_GETTIME
          clock_gettime(CLOCK_REALTIME, &abstime);
#else
          abstime.tv_sec = time(0);
          abstime.tv_nsec = 0;
#endif
          abstime.tv_sec += timeout;
          state = pthread_cond_timedwait(&qh->cond, &qh->mutex, &abstime) == 0;
        } else {
          state = pthread_cond_wait(&qh->cond, &qh->mutex) == 0;
        }
        pthread_mutex_unlock(&qh->mutex);
        if (state) {
          waited = TRUE;
          goto retry;
        } else {
          *data_len = 0;
          *data = NULL;
        }
      } else {
        *data_len = 0;
        *data = NULL;
        state = 0;
      }
    }
  } else
    state = 2;
  pthread_cleanup_pop(unlock);
  return state;
}

int RemoteMDSEventAst(const char *const eventnam_in, void (*const astadr)(),
                      void *const astprm, int *const eventid) {
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

static int canEventRemote(const int eventid) {
  int status = 1, i;
  /* kill external thread before sending messages over the socket */
  if
    STATUS_OK {
      KillHandler();
      for (i = 0; i < num_receive_servers; i++) {
        if (receive_ids[i] > 0)
          status = MdsEventCan_(receive_ids[i], getRemoteId(eventid, i));
      }
      startRemoteAstHandler();
    }
  return status;
}

int RemoteMDSEventCan(const int eventid) {
  if (eventid < 0)
    return 0;

  initializeRemote(1);
  return canEventRemote(eventid);
}

static int sendRemoteEvent(const char *const evname, const int data_len,
                           char *const data) {
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
  if
    STATUS_OK {
      int reconnects = 0;
      tmp_status = 0;
      for (i = 0; i < num_send_servers; i++) {
        if (send_ids[i] > 0) {
          if (data_len > 0)
            tmp_status =
                MdsValue_(send_ids[i], expression, &desc, &ansarg, NULL);
          else
            tmp_status =
                MdsValue_(send_ids[i], expression, &ansarg, NULL, NULL);
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
        free(ansarg.ptr);
      }
    }
  return status;
}

int RemoteMDSEvent(const char *const evname_in, const int data_len,
                   char *const data) {
  int j;
  unsigned int u;
  char *evname;
  int status = 1;
  initializeRemote(0);
  evname = strdup(evname_in);
  for (u = 0, j = 0; u < strlen(evname); u++) {
    if (evname[u] != 32)
      evname[j++] = (char)toupper(evname[u]);
  }
  evname[j] = 0;
  status = sendRemoteEvent(evname, data_len, data);
  free(evname);
  return status;
}

#endif

EXPORT int MDSEventAst(const char *const eventNameIn,
                       void (*const astadr)(void *, int, char *),
                       void *const astprm, int *const eventid) {
  char *eventName = malloc(strlen(eventNameIn) + 1);
  unsigned int i, j;
  int status;
  for (i = 0, j = 0; i < strlen(eventNameIn); i++) {
    if (eventNameIn[i] != 32)
      eventName[j++] = (char)toupper(eventNameIn[i]);
  }
  eventName[j] = 0;
  if (getenv("mds_event_server"))
    status = RemoteMDSEventAst(eventName, astadr, astprm, eventid);
  else
    status = MDSUdpEventAst(eventName, astadr, astprm, eventid);
  free(eventName);
  return status;
}


EXPORT int MDSEventAstMask(const char *const eventNameIn,
                       void (*const astadr)(void *, int, char *),
                       void *const astprm, int *const eventid, unsigned int cpuMask) {
  char *eventName = malloc(strlen(eventNameIn) + 1);
  unsigned int i, j;
  int status;
  for (i = 0, j = 0; i < strlen(eventNameIn); i++) {
    if (eventNameIn[i] != 32)
      eventName[j++] = (char)toupper(eventNameIn[i]);
  }
  eventName[j] = 0;
  if (getenv("mds_event_server"))
    status = RemoteMDSEventAst(eventName, astadr, astprm, eventid);
  else
    status = MDSUdpEventAstMask(eventName, astadr, astprm, eventid, cpuMask);
  free(eventName);
  return status;
}

EXPORT int MDSEvent(const char *const eventNameIn, const int bufLen,
                    char *const buf) {
  char *eventName = alloca(strlen(eventNameIn) + 1);
  unsigned int i, j;
  int status;
  for (i = 0, j = 0; i < strlen(eventNameIn); i++) {
    if (eventNameIn[i] != 32)
      eventName[j++] = (char)toupper(eventNameIn[i]);
  }
  eventName[j] = 0;
  if (getenv("mds_event_target"))
    status = RemoteMDSEvent(eventName, bufLen, buf);
  else
    status = MDSUdpEvent(eventName, bufLen, buf);
  return status;
}

EXPORT int MDSEventCan(const int id) {
  int status;
  if (getenv("mds_event_server"))
    status = RemoteMDSEventCan(id);
  else
    status = MDSUdpEventCan(id);
  CancelEventQueue(id);
  return status;
}
