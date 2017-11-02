/*------------------------------------------------------------------------------

		Name:   ACTLOG

		Type:   C main program

		Author:	Tom Fredian, Timo Schroeder

		Date:   21-APR-1992, 2017

		Purpose: Action Monitor

------------------------------------------------------------------------------

   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

 External functions or symbols referenced:                                    */

#include <errno.h>
#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <mdsserver.h>
#include <ncidef.h>
#include <usagedef.h>
#include <treeshr.h>
#include <mdsshr.h>
#include <mdslib.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread_port.h>
#include <libroutines.h>
#include <strroutines.h>
#include <unistd.h>

extern int ServerMonitorCheckin();
extern int str_element();

extern int TdiExecute();

typedef struct _LinkedEvent {
  char *msg;
  char *tree;
  int shot;
  int phase;
  int nid;
  int on;
  int mode;
  char *server;
  int status;
  char *fullpath;
  char *time;
  char *status_text;
  struct _LinkedEvent *next;
} LinkedEvent;

static pthread_mutex_t event_mutex = PTHREAD_MUTEX_INITIALIZER;
static LinkedEvent *EventQueueHead = NULL;
static LinkedEvent *EventQueueTail = NULL;

static void MessageAst();
static void EventUpdate();
static void Phase(LinkedEvent * event);
static void Dispatched(LinkedEvent * event);
static void Doing(LinkedEvent * event);
static void Done(LinkedEvent * event);
static void CheckIn(char* monitor);
static void DoOpenTree(LinkedEvent * event);

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))
//#define min(a,b) ( ((a)<(b)) ? (a) : (b) )
//#define max(a,b) ( ((a)>(b)) ? (a) : (b) )

static char *current_tree = NULL;
static int current_shot = -9999;
static int current_phase = -9999;
static int current_node_entry;
static int current_on;
static const char *asterisks = "********************************************";
static void DoTimer();
static void PutLog(char *time, char *mode, char *status, char *server, char *path);
static void PutError(char *time, char *mode, char *status, char *server, char *path);

static void ParseTime(LinkedEvent * event){
  event->time = strtok(event->time, " ");
  while (event->time && strchr(event->time, ':') == 0)
    event->time = strtok(0, " ");
}

static int parseMsg(char *msg, LinkedEvent * event){
  char *tmp;
  if (!msg) return C_ERROR;
  event->msg = strcpy(malloc(strlen(msg) + 1), msg);
  event->tree = strtok(event->msg, " ");
  if (!event->tree) return C_ERROR;
  tmp = strtok(0, " ");
  if (!tmp) return C_ERROR;
  event->shot = atoi(tmp);
  if (event->shot <= 0) return C_ERROR;
  tmp = strtok(0, " ");
  if (!tmp) return C_ERROR;
  event->phase = atoi(tmp);
  tmp = strtok(0, " ");
  if (!tmp) return C_ERROR;
  event->nid = atoi(tmp);
  tmp = strtok(0, " ");
  if (!tmp) return C_ERROR;
  event->on = atoi(tmp);
  if (event->on != 0 && event->on != 1) return C_ERROR;
  tmp = strtok(0, " ");
  if (!tmp) return C_ERROR;
  event->mode = atoi(tmp);
  event->server = strtok(0, " ");
  if (!event->server) return C_ERROR;
  tmp = strtok(0, " ");
  if (!tmp) return C_ERROR;
  event->status = atoi(tmp);
  event->fullpath = strtok(0, " ");
  if (!event->fullpath) return C_ERROR;
  event->time = strtok(0, ";");
  if (!event->time) return C_ERROR;
  event->status_text = strtok(0, ";");
  if (!event->status_text) return C_ERROR;
  ParseTime(event);
 return C_OK;
}

static LinkedEvent *GetQEvent(){
  pthread_mutex_lock(&event_mutex);
  LinkedEvent *ans = EventQueueHead;
  if (ans)
     EventQueueHead = ans->next;
  if (!EventQueueHead)
    EventQueueTail = NULL;
  pthread_mutex_unlock(&event_mutex);
  return ans;
}

inline static void _DoTimer(){
  LinkedEvent *ev;
  //while (sleep(100)==0)
  while ((ev = GetQEvent())) {
    EventUpdate(ev);
    if (ev->msg)
      free(ev->msg);
    free(ev);
  }
}

static void QEvent(LinkedEvent * ev){
  ev->next = NULL;
  pthread_mutex_lock(&event_mutex);
  if (EventQueueTail)
    EventQueueTail->next = ev;
  else
    EventQueueHead = ev;
  EventQueueTail = ev;
  pthread_mutex_unlock(&event_mutex);
}

static void MessageAst(void* dummy __attribute__ ((unused)), char *reply){
  LinkedEvent *event = malloc(sizeof(LinkedEvent));
  event->msg = NULL;
  if (!parseMsg(reply, event)) {
    QEvent(event);
    return;
  }
  if (event->msg)
    free(event->msg);
  free(event);
  CheckIn(0);
}

inline static void _EventUpdate(LinkedEvent * event){
  switch (event->mode) {
  case build_table_begin:
    DoOpenTree(event);
  case build_table:
    break;
  case build_table_end:
    break;
  case dispatched:
    Dispatched(event);
    break;
  case doing:
    Doing(event);
    break;
  case done:
    Done(event);
    break;
  }
}

inline static void _DoOpenTree(LinkedEvent * event)
{
  current_node_entry = 0;
  current_on = -1;
  current_phase = 9999;
  if ((event->shot != current_shot) || strcmp(event->tree, current_tree)) {
    current_shot = event->shot;
    current_tree = realloc(current_tree, strlen(event->tree) + 1);
    strcpy(current_tree, event->tree);
    PutLog(event->time, "NEW SHOT", (char *)asterisks, (char *)asterisks, current_tree);
  }
}

static void Phase(LinkedEvent * event){
  static DESCRIPTOR(const unknown, "UNKNOWN");
  static struct descriptor phase = { 0, DTYPE_T, CLASS_D, 0 };
  static DESCRIPTOR(const phase_lookup, "PHASE_NAME_LOOKUP($)");
  static struct descriptor phase_d = { sizeof(int), DTYPE_L, CLASS_S, 0 };
  phase_d.pointer = (char *)&event->phase;
  if (current_phase != event->phase) {
    if (!(TdiExecute(&phase_lookup, &phase_d, &phase MDS_END_ARG) & 1))
      StrCopyDx((struct descriptor *)&phase, (struct descriptor *)&unknown);
    char *str = MdsDescrToCstring(&phase);
    PutLog(event->time, "PHASE", (char *)asterisks, (char *)asterisks, str);
    if (str) free(str);
    current_phase = event->phase;
  }
}
inline static void _Doing(LinkedEvent * event){
  Phase(event);
  PutLog(event->time, "DOING", " ", event->server, event->fullpath);
}

inline static void _Done(LinkedEvent * event){
  PutLog(event->time, "DONE", IS_OK(event->status) ? " " : event->status_text, event->server, event->fullpath);
  if IS_NOT_OK(event->status)
    PutError(event->time, "DONE", event->status_text, event->server, event->fullpath);
}


static void Dispatched(LinkedEvent * event){
  Phase(event);
  PutLog(event->time, "DISPATCHED", " ", event->server, event->fullpath);
}

static void CheckIn(char* monitor_in){
  static char* monitor;
  INIT_STATUS_ERROR;
  if (monitor_in)
    monitor = monitor_in;
  for (;;) {
    status = ServerMonitorCheckin(monitor, MessageAst, 0);
    if STATUS_OK return;
    printf("Error %d connecting to monitor: %s, will try again shortly\n", status, monitor);
    sleep(2);
  }
}
