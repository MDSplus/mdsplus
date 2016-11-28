/*------------------------------------------------------------------------------

		Name:   ACTMON

		Type:   C main program

		Author:	Tom Fredian

		Date:   21-APR-1992

		Purpose: Action Monitor

------------------------------------------------------------------------------

	Call sequence:

$ MCR ACTMON -monitor monitor-name

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

#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <mdsserver.h>
#include <ncidef.h>
#include <usagedef.h>
#include <treeshr.h>
#include <mdsshr.h>
#include <mdslib.h>
#include <stdlib.h>
#include <Xm/Xm.h>
#include <Mrm/MrmPublic.h>

#include <Xm/ToggleB.h>
#include <Xm/List.h>
#include <Xm/MessageB.h>
#include <Xm/Text.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <libroutines.h>
#include <strroutines.h>
#include <unistd.h>

extern int ServerMonitorCheckin();
extern int str_element();

#if (defined(_DECTHREADS_) && (_DECTHREADS_ != 1)) || !defined(_DECTHREADS_)
#define pthread_condattr_default NULL
#define pthread_mutexattr_default NULL
#endif
#define def_lock(name) \
\
static int name##_mutex_initialized = 0;\
static pthread_mutex_t name##_mutex;\
\
static void lock_##name()\
{\
\
  if(! name##_mutex_initialized)\
  {\
    name##_mutex_initialized = 1;\
    pthread_mutex_init(&name##_mutex, pthread_mutexattr_default);\
  }\
  pthread_mutex_lock(&name##_mutex);\
}\
\
static void unlock_##name()\
{\
\
  if(! name##_mutex_initialized)\
  {\
    name##_mutex_initialized = 1;\
    pthread_mutex_init(&name##_mutex, pthread_mutexattr_default);\
  }\
\
  pthread_mutex_unlock(&name##_mutex);\
}

def_lock(event_queue)

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

static void Exit(Widget w, int *tag, XtPointer callback_data);
static void MessageAst();
static void EventUpdate();
static void Phase(LinkedEvent * event);
static void Dispatched(LinkedEvent * event);
static void Doing(LinkedEvent * event);
static void Done(LinkedEvent * event);
static void CheckIn(String monitor);
static void DoOpenTree(LinkedEvent * event);
//static void ActivateImage(struct descriptor *image);
//static void ActivateImages(String images);
static void Disable(Widget w, int *tag, XmToggleButtonCallbackStruct * cb);
static void SetKillTarget(Widget w, int *tag, XmListCallbackStruct * cb);
static void ConfirmAbort(Widget w, int *tag, XmListCallbackStruct * cb);
static void ConfirmServerAbort(Widget w, void *tag, void *cb);
static void SetKillSensitive(Widget top);

#define min(a,b) ( ((a)<(b)) ? (a) : (b) )
#define max(a,b) ( ((a)>(b)) ? (a) : (b) )

typedef struct _doingListItem {
  int nid;
  int pos;
  struct _doingListItem *next;
} DoingListItem;

static DoingListItem *DoingList = 0;
static Widget CurrentWidget;
static Widget ErrorWidget;
static Widget LogWidget;
static Widget kill_target_w;
static Boolean ErrorWidgetOff = FALSE;
static Boolean CurrentWidgetOff = FALSE;
static Boolean LogWidgetOff = TRUE;
static char *current_tree = NULL;
static int current_shot = -9999;
static int current_phase = -9999;
static int current_node_entry;
static int current_on;
static const char *asterisks = "****************************************************************";
#define MaxLogLines 4000
#define EventEfn 1
#define DOING 1
#define DONE 2
static LinkedEvent *EventQueueHead = 0;
static LinkedEvent *EventQueueTail = 0;
int unique_tag_seed = 0;
static XmString done_label;
static XmString doing_label;
static XmString dispatched_label;
static XmString error_label;
static XtAppContext app_ctx;
static void DoTimer();

#define TimeString(tm) ctime(&tm)

int main(int argc, String * argv)
{
  static String hierarchy_name[] = { "actmon.uid" };
  static MrmRegisterArg callbacks[] = { {"Exit", (char *)Exit},
  {"Disable", (char *)Disable},
  {"ConfirmAbort", (char *)ConfirmAbort},
  {"SetKillTarget", (char *)SetKillTarget},
  };

  MrmType class;
  static XrmOptionDescRec options[] = { {"-monitor", "*monitor", XrmoptionSepArg, NULL} };
  static XtResource resources[] = {
    {"monitor", "Monitor", XtRString, sizeof(String), 0, XtRString, "ACTION_MONITOR"},
    {"images",  "Images",  XtRString, sizeof(String), sizeof(String), XtRString, ""}
  };
  MrmHierarchy drm_hierarchy;
  struct {
    String monitor;
    String images;
  } resource_list;
  Widget top;
  Widget mainWidget;
  XInitThreads();
  MrmInitialize();
  MrmRegisterNames(callbacks, XtNumber(callbacks));
  top = XtVaAppInitialize(&app_ctx, "ActMon", options, XtNumber(options), &argc, argv, NULL,
			  XmNallowShellResize, 1, NULL);
  XtGetApplicationResources(top, &resource_list, resources, XtNumber(resources), (Arg *) NULL, 0);
  MrmOpenHierarchy(XtNumber(hierarchy_name), hierarchy_name, 0, &drm_hierarchy);
  MrmFetchWidget(drm_hierarchy, "main", top, &mainWidget, &class);
  MrmCloseHierarchy(drm_hierarchy);
  XtManageChild(mainWidget);
  XtRealizeWidget(top);
  CurrentWidget = XtNameToWidget(mainWidget, "*current_actions");
  ErrorWidget = XtNameToWidget(mainWidget, "*errors");
  LogWidget = XtNameToWidget(mainWidget, "*log");
  dispatched_label = XmStringCreateSimple("Dispatched");
  doing_label = XmStringCreateSimple("Doing");
  done_label = XmStringCreateSimple("Done");
  error_label = XmStringCreateSimple("Error");
  if (!kill_target_w) {
    kill_target_w = XtNameToWidget(top, "*server_name");
  }
  SetKillSensitive(top);
  CheckIn(resource_list.monitor);
  XtAppAddTimeOut(app_ctx, 1000, DoTimer, 0);
  XtAppMainLoop(app_ctx);
  return 0;
}

static void Exit(Widget w __attribute__ ((unused)), int *tag __attribute__ ((unused)), XtPointer callback_data __attribute__ ((unused)))
{
  exit(0);
}

typedef struct serverList {
  char *server;
  char path[512];
  struct serverList *next;
} ServerList;

static ServerList *Servers = NULL;

static Widget FindTop(Widget w)
{
  for (; w && XtParent(w); w = XtParent(w)) ;
  return w;
}

static void SetKillTarget(Widget w __attribute__ ((unused)), int *tag __attribute__ ((unused)),
			  XmListCallbackStruct * cb __attribute__ ((unused)))
{
  static ServerList *server;
  int idx;

  if (cb->selected_item_count == 1) {
    idx = cb->item_position;
    for (server = Servers; idx > 1; idx--, server = server->next) ;
    XmTextSetString(kill_target_w, server->server);
  } else {
    XmTextSetString(kill_target_w, "");
  }
}

static void ConfirmAbort(Widget w, int *tag, XmListCallbackStruct * cb __attribute__ ((unused)))
{
  static int operation;
  static Widget dialog = NULL;
  XmString text_cs;
  char message[1024];
  char *server;

  if (!dialog) {
    dialog = XmCreateQuestionDialog(FindTop(w), "ConfirmServerAbort", NULL, 0);
    XtVaSetValues(dialog, XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
		  XmNdefaultButtonType, XmDIALOG_CANCEL_BUTTON, XmNautoUnmanage, True, NULL);
    XtSetSensitive(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON), 0);
    XtAddCallback(dialog, XmNokCallback, ConfirmServerAbort, (void *)&server);
  }
  operation = *(int *)tag;
  server = XmTextGetString(kill_target_w);
  if (strlen(server) > 0) {
    switch (operation) {
    case 0:
      sprintf(message, "Are you sure you want to abort the action\nRunning on server %s ?", server);
      break;
    case 1:
      sprintf(message, "Are you sure you want to kill the action server:\n %s ?", server);
      break;
    case 2:
      sprintf(message, "Are you sure you want to \nKILL THE DISPATCHER ?");
      break;
    default:
      strcpy(message, "");
    }
    if (strlen(message) > 0) {
      text_cs = XmStringCreateLtoR(message, XmSTRING_DEFAULT_CHARSET);
      XtVaSetValues(dialog, XmNmessageString, text_cs, XmNuserData, (void *)&operation, NULL);
      XmStringFree(text_cs);
      XtManageChild(dialog);
    }
  }
}

static int executable(const char *script)
{
  int status;
  static const char *cmd_front = "/bin/sh -c '/usr/bin/which ";
  static const char *cmd_back = " > /dev/null 2>/dev/null'";
  static char cmd[132];
  static struct descriptor cmd_d = { 0, DTYPE_T, CLASS_S, cmd };
  strcpy(cmd, cmd_front);
  strcat(cmd, script);
  strcat(cmd, cmd_back);
  cmd_d.length = strlen(cmd);
  status = LibSpawn(&cmd_d, 1, 0);
/*  printf("evaluating / %s / returned %d\n", cmd, status); */
  return !status;
}

static void SetKillSensitive(Widget top)
{
  int i;
  static const char *widgets[] = { "*abort_server_b", "*kill_server_b", "*kill_dispatcher_b" };
  static const char *scripts[] =
      { "mdsserver_abort", "mdsserver_kill", "mdsserver_kill_dispatcher" };
  for (i = 0; i < 3; i++) {
    Widget w = XtNameToWidget(top, widgets[i]);
    if (executable(scripts[i])) {
      XtVaSetValues(w, XmNsensitive, 1, NULL);
    }
  }
}

static void ConfirmServerAbort(Widget w, void *tag __attribute__ ((unused)), void *cb __attribute__ ((unused)))
{
  int *op_ptr;
  int operation;
  char *server;
  char cmd[132];
  static struct descriptor cmd_dsc = { 0, DTYPE_T, CLASS_S, 0 };

  cmd[0] = '\0';
  XtVaGetValues(w, XmNuserData, (void *)&op_ptr, NULL);
  operation = *op_ptr;
  server = XmTextGetString(kill_target_w);
  if (strlen(server) > 0) {
    switch (operation) {
    case 0:
      strcpy(cmd, "mdsserver_abort ");
      strcat(cmd, server);
      break;
    case 1:
      strcpy(cmd, "mdsserver_kill ");
      strcat(cmd, server);
      break;
    case 2:
      strcpy(cmd, "mdsserver_kill_dispatcher");
      break;
    default:;
    }
    printf("About to execute / %s /\n", cmd);
    cmd_dsc.length = strlen(cmd);
    cmd_dsc.pointer = cmd;
    LibSpawn(&cmd_dsc, 0, 0);
  }
}

static void Disable(Widget w __attribute__ ((unused)), int *tag, XmToggleButtonCallbackStruct * cb)
{
  Widget dw = 0;
  switch (*tag) {
  case 4:
    LogWidgetOff = cb->set;
  case 1:
    dw = XtParent(LogWidget);
    break;
  case 5:
    ErrorWidgetOff = cb->set;
  case 2:
    dw = XtParent(ErrorWidget);
    break;
  case 6:
    CurrentWidgetOff = cb->set;
  case 7:
    dw = XtParent(CurrentWidget);
    break;
  }
  if (dw) {
    if (cb->set)
      XtUnmanageChild(dw);
    else
      XtManageChild(dw);
  }
}

static void ParseTime(LinkedEvent * event)
{
  event->time = strtok(event->time, " ");
  while (event->time && strchr(event->time, ':') == 0)
    event->time = strtok(0, " ");
}

static int ParseMsg(char *msg, LinkedEvent * event)
{
  char *tmp;
  event->msg = strcpy(malloc(strlen(msg) + 1), msg);
  event->tree = strtok(event->msg, " ");
  if (!event->tree)
    return 0;
  tmp = strtok(0, " ");
  if (!tmp)
    return 0;
  event->shot = atoi(tmp);
  if (event->shot <= 0)
    return 0;
  tmp = strtok(0, " ");
  if (!tmp)
    return 0;
  event->phase = atoi(tmp);
  tmp = strtok(0, " ");
  if (!tmp)
    return 0;
  event->nid = atoi(tmp);
  tmp = strtok(0, " ");
  if (!tmp)
    return 0;
  event->on = atoi(tmp);
  if (event->on != 0 && event->on != 1)
    return 0;
  tmp = strtok(0, " ");
  if (!tmp)
    return 0;
  event->mode = atoi(tmp);
  event->server = strtok(0, " ");
  if (!event->server)
    return 0;
  tmp = strtok(0, " ");
  if (!tmp)
    return 0;
  event->status = atoi(tmp);
  event->fullpath = strtok(0, " ");
  if (!event->fullpath)
    return 0;
  event->time = strtok(0, ";");
  if (!event->time)
    return 0;
  event->status_text = strtok(0, ";");
  if (!event->status_text)
    return 0;
  ParseTime(event);
  return 1;
}

static LinkedEvent *GetQEvent()
{
  LinkedEvent *ans = 0;
  lock_event_queue();
  ans = EventQueueHead;
  if (EventQueueHead)
    EventQueueHead = EventQueueHead->next;
  if (!EventQueueHead)
    EventQueueTail = 0;
  unlock_event_queue();
  return ans;
}

static void DoTimer()
{
  LinkedEvent *ev;
  while ((ev = GetQEvent()) != 0) {
    EventUpdate(ev);
    if (ev->msg)
      free(ev->msg);
    free(ev);
  }
  XtAppAddTimeOut(app_ctx, 100, DoTimer, 0);
}

static void QEvent(LinkedEvent * ev)
{
  ev->next = 0;
  lock_event_queue();
  if (EventQueueTail)
    EventQueueTail->next = ev;
  else {
    EventQueueHead = ev;
  }
  EventQueueTail = ev;
  unlock_event_queue();
}

static void MessageAst(int dummy __attribute__ ((unused)), char *reply)
{
  LinkedEvent *event = malloc(sizeof(LinkedEvent));
  event->msg = 0;
  if (reply && ParseMsg(reply, event)) {
    QEvent(event);
  } else {
    if (event->msg)
      free(event->msg);
    free(event);
    CheckIn(0);
  }
}

static void EventUpdate(LinkedEvent * event)
{
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
  XmTextSetString(kill_target_w, "");
}

static int FindServer(char *name, ServerList ** srv)
{
  ServerList *prev, *ptr, *newPtr;
  int idx;
  int match = 1;
  for (prev = NULL, idx = 1, ptr = Servers;
       ptr && (match = strcasecmp(ptr->server, name)) < 0; prev = ptr, ptr = ptr->next, idx++) ;
  if (match != 0) {
    XmString item;
    newPtr = (ServerList *) XtMalloc(sizeof(ServerList));
    newPtr->server = (char *)XtMalloc(strlen(name) + 1);
    strcpy(newPtr->path, "");
    newPtr->next = ptr;
    strcpy(newPtr->server, name);
    if (prev)
      prev->next = newPtr;
    else
      Servers = newPtr;
    item = XmStringCreateSimple(newPtr->server);
    XmListAddItemUnselected(CurrentWidget, item, idx);
    XmStringFree(item);
    ptr = newPtr;
  }
  *srv = ptr;
  return idx;
}

static void PutLog(char *time, char *mode, char *status, char *server, char *path)
{
  ServerList *srv;
  char text[2048];
  XmString item;
  int items;
  if ((LogWidgetOff && CurrentWidgetOff) || (LogWidget == 0))
    return;
  sprintf(text, "%s %12d %-10.10s %-32.32s %-20.20s %s", time, current_shot, mode, status, server,
	  path);
  item = XmStringCreateSimple(text);
  if (!LogWidgetOff) {
    XmListAddItemUnselected(LogWidget, item, 0);
    XmStringFree(item);
    XtVaGetValues(LogWidget, XmNitemCount, &items, NULL);
    if (items > MaxLogLines) {
      DoingListItem *doing;
      for (doing = DoingList; doing; doing = doing->next)
	doing->pos--;
      XmListDeletePos(LogWidget, 1);
    }
    XmListSetBottomPos(LogWidget, 0);
  }
  if (!CurrentWidgetOff) {
    if (strcmp(mode, "DOING") == 0) {
      int idx = FindServer(server, &srv);
      strcpy(srv->path, path);
      sprintf(text, "%-20.20s %s %12d %s", server, time, current_shot, path);
      item = XmStringCreateSimple(text);
      XmListReplaceItemsPos(CurrentWidget, &item, 1, idx);
      XmStringFree(item);
    } else if (strcmp(mode, "DONE") == 0) {
      int idx = FindServer(server, &srv);
      if (strcmp(srv->path, path) == 0) {
	strcpy(srv->path, "");
	item = XmStringCreateSimple(server);
	XmListReplaceItemsPos(CurrentWidget, &item, 1, idx);
	XmStringFree(item);
      }
    }
  }
}

static void PutError(char *time, String mode, char *status, char *server, char *path)
{

  char text[2048];
  XmString item;
  int items;

  if (ErrorWidgetOff)
    return;
  sprintf(text, "%s %12d %s %-36.36s %-20.20s %s", time, current_shot, mode, status, server, path);
  item = XmStringCreateSimple(text);
  XmListAddItemUnselected(ErrorWidget, item, 0);
  XmStringFree(item);
  XtVaGetValues(ErrorWidget, XmNitemCount, &items, NULL);
  if (items > MaxLogLines) {
    DoingListItem *doing;
    for (doing = DoingList; doing; doing = doing->next)
      doing->pos--;
    XmListDeletePos(ErrorWidget, 1);
  }
  XmListSetBottomPos(ErrorWidget, 0);
  /*
     XFlush(XtDisplay(ErrorWidget));
   */

}

static void DoOpenTree(LinkedEvent * event)
{
  DoingListItem *doing;
  DoingListItem *next;
  XmListDeleteAllItems(ErrorWidget);
  current_node_entry = 0;
  current_on = -1;
  current_phase = 9999;
  XmListDeselectAllItems(LogWidget);
  for (doing = DoingList; doing; doing = next) {
    next = doing->next;
    free(doing);
  }
  DoingList = 0;
  if ((event->shot != current_shot) || strcmp(event->tree, current_tree)) {
    current_shot = event->shot;
    current_tree = realloc(current_tree, strlen(event->tree) + 1);
    strcpy(current_tree, event->tree);
    PutLog(event->time, "NEW SHOT", (char *)asterisks, (char *)asterisks, current_tree);
  }
  unique_tag_seed = 0;
}

static void Phase(LinkedEvent * event)
{
  static DESCRIPTOR(const unknown, "UNKNOWN");
  static struct descriptor phase = { 0, DTYPE_T, CLASS_D, 0 };
  static DESCRIPTOR(const phase_lookup, "PHASE_NAME_LOOKUP($)");
  static struct descriptor phase_d = { sizeof(int), DTYPE_L, CLASS_S, 0 };
  phase_d.length = sizeof(int);
  phase_d.pointer = (char *)&event->phase;
  if (current_phase != event->phase) {
    if (!(TdiExecute(&phase_lookup, &phase_d, &phase MDS_END_ARG) & 1))
      StrCopyDx((struct descriptor *)&phase, (struct descriptor *)&unknown);
    PutLog(event->time, "PHASE", (char *)asterisks, (char *)asterisks, phase.pointer);
    current_phase = event->phase;
  }
}

static void Dispatched(LinkedEvent * event)
{
  Phase(event);
  PutLog(event->time, "DISPATCHED", " ", event->server, event->fullpath);
}

static void Doing(LinkedEvent * event)
{
  DoingListItem *doing = malloc(sizeof(DoingListItem));
  DoingListItem *prev;
  DoingListItem *d;
  int items;
  Phase(event);
  PutLog(event->time, "DOING", " ", event->server, event->fullpath);
  XtVaGetValues(LogWidget, XmNitemCount, &items, NULL);
  XmListSelectPos(LogWidget, 0, 0);
  for (prev = 0, d = DoingList; d; prev = d, d = d->next) ;
  if (prev)
    prev->next = doing;
  else
    DoingList = doing;
  doing->nid = event->nid;
  doing->pos = items;
  doing->next = 0;
}

static void Done(LinkedEvent * event)
{
  DoingListItem *doing;
  DoingListItem *prev;
  int *items;
  int num;
  XmListGetSelectedPos(LogWidget, &items, &num);
  for (prev = 0, doing = DoingList; doing && (doing->nid != event->nid);
       prev = doing, doing = doing->next) ;
  if (doing) {
    XmListDeselectPos(LogWidget, doing->pos);
    if (prev)
      prev->next = doing->next;
    else
      DoingList = doing->next;
    free(doing);
  }
  PutLog(event->time, "DONE", (event->status & 1) ? " " : event->status_text, event->server,
	 event->fullpath);
  if (!(event->status & 1))
    PutError(event->time, "DONE", event->status_text, event->server, event->fullpath);
}

static void CheckIn(String monitor_in)
{
  static String monitor;
  int status = 0;
  if (monitor_in)
    monitor = monitor_in;
  while (!(status & 1)) {
    status = ServerMonitorCheckin(monitor, MessageAst, 0);
    if (!(status & 1)) {
      printf("Error connecting to monitor: %s, will try again shortly\n", monitor);
      sleep(2);
    }
  }
}
/*
static void ActivateImages(String images)
{
  struct descriptor list = { 0, DTYPE_T, CLASS_S, 0 };
  struct descriptor image = { 0, DTYPE_T, CLASS_D, 0 };
  static DESCRIPTOR(const comma, ",");
  int i;
  list.length = strlen(images);
  list.pointer = images;
  for (i = 0; (str_element(&image, &i, &comma, &list) & 1); i++)
    ActivateImage(&image);
  return;
}

static void ActivateImage(struct descriptor *image)
{
   void (*sym)();
   lib$establish(lib$sig_to_ret);
   lib$find_image_symbol(image,image,&sym);
}
*/
