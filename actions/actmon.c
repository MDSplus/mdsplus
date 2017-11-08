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

#include "actlogp.h"
#include <ctype.h>
#include <Xm/Xm.h>
#include <Mrm/MrmPublic.h>
#include <Xm/ToggleB.h>
#include <Xm/List.h>
#include <Xm/MessageB.h>
#include <Xm/Text.h>
#include <mdsplus/mdsplus.h>
static void Exit(Widget w, int *tag, XtPointer callback_data);
static void MessageAst();
static void EventUpdate();
static void Phase(LinkedEvent * event);
static void Dispatched(LinkedEvent * event);
static void Doing(LinkedEvent * event);
static void Done(LinkedEvent * event);
static void CheckIn(char* monitor);
static void DoOpenTree(LinkedEvent * event);
static void Disable(Widget w, int *tag, XmToggleButtonCallbackStruct * cb);
static void SetKillTarget(Widget w, int *tag, XmListCallbackStruct * cb);
static void ConfirmAbort(Widget w, int *tag, XmListCallbackStruct * cb);
static void ConfirmServerAbort(Widget w, void *tag, void *cb);
static void SetKillSensitive(Widget top);

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
static Boolean LogWidgetOff = FALSE;

#define MaxLogLines 4000
#define EventEfn 1
#define DOING 1
#define DONE 2

int unique_tag_seed = 0;
static XmString done_label;
static XmString doing_label;
static XmString dispatched_label;
static XmString error_label;
static XtAppContext app_ctx;
static char* expt  = NULL;

#define TimeString(tm) ctime(&tm)
#define offset(strc,field) (int)((void*)&(strc).field-(void*)&(strc))


int main(int argc, char** argv)
{
  static char* hierarchy_name[] = { "actmon.uid" };
  static MrmRegisterArg callbacks[] = { {"Exit", (char *)Exit},
  {"Disable", (char *)Disable},
  {"ConfirmAbort", (char *)ConfirmAbort},
  {"SetKillTarget", (char *)SetKillTarget},
  };

  MrmType class;
  static XrmOptionDescRec options[] = {
     {"-monitor", "*monitor", XrmoptionSepArg, NULL},
     {"-expt",    "*expt",    XrmoptionSepArg, NULL},
  };
  struct {
    char* monitor;
    char* images;
    char* expt;
  } resource_list;
  static XtResource resources[] = {
    {"monitor", "Monitor", XtRString, sizeof(char*), offset(resource_list,monitor), XtRString, "ACTION_MONITOR"},
    {"images",  "Images",  XtRString, sizeof(char*), offset(resource_list,images ), XtRString, ""},
    {"expt",    "Expt",    XtRString, sizeof(char*), offset(resource_list,expt   ), XtRString, NULL},
  };
  MrmHierarchy drm_hierarchy;
  Widget top;
  Widget mainWidget;
  XInitThreads();
  MrmInitialize();
  MrmRegisterNames(callbacks, XtNumber(callbacks));
  top = XtVaAppInitialize(&app_ctx, "ActMon", options, XtNumber(options), &argc, argv, NULL,
			  XmNallowShellResize, 1, NULL);
  XtGetApplicationResources(top, &resource_list, resources, XtNumber(resources), (Arg *) NULL, 0);
  fprintf(stderr,"MONITOR: '%s'\n",resource_list.monitor);
  if (resource_list.expt) {
    int len = strlen(resource_list.expt);
    expt = malloc(sizeof(char)*(len+1));
    expt[len] = 0;
    while (len-->0)
      expt[len] = toupper(resource_list.expt[len]);
    fprintf(stderr,"EXPT:    '%s'\n",expt);
  }
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

static void Exit(Widget w __attribute__ ((unused)), int *tag __attribute__ ((unused)), XtPointer callback_data __attribute__ ((unused))){
  exit(0);
}

typedef struct serverList {
  char *server;
  char path[512];
  struct serverList *next;
} ServerList;

static ServerList *Servers = NULL;

static Widget FindTop(Widget w){
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

static void ConfirmAbort(Widget w, int *tag, XmListCallbackStruct * cb __attribute__ ((unused))){
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

static int executable(const char *script){
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

static void SetKillSensitive(Widget top){
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
    MDS_ATTR_FALLTHROUGH
  case 1:
    dw = XtParent(LogWidget);
    break;
  case 5:
    ErrorWidgetOff = cb->set;
    MDS_ATTR_FALLTHROUGH
  case 2:
    dw = XtParent(ErrorWidget);
    break;
  case 6:
    CurrentWidgetOff = cb->set;
    MDS_ATTR_FALLTHROUGH
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

static void DoTimer(){
  _DoTimer();
  XtAppAddTimeOut(app_ctx, 100, DoTimer, 0);
}

static void EventUpdate(LinkedEvent * event){
  _EventUpdate(event);
  XmTextSetString(kill_target_w, "");
}

static int FindServer(char *name, ServerList ** srv){
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

static void PutLog(char *time, char *mode, char *status, char *server, char *path){
  ServerList *srv;
  char text[2048];
  XmString item;
  int items;
  if ((LogWidgetOff && CurrentWidgetOff) || (LogWidget == 0))
    return;
  sprintf(text, "%s %12d %-10.10s %-44.44s %-20.20s %s", time, current_shot, mode, status, server, path);
  if (!LogWidgetOff) {
    item = XmStringCreateSimple(text);
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

static void PutError(char *time, char* mode, char *status, char *server, char *path)
{

  char text[2048];
  XmString item;
  int items;

  if (ErrorWidgetOff)
    return;
  sprintf(text, "%s %12d %-10.10s %-44.44s %-20.20s %s", time, current_shot, mode, status, server, path);
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

static void DoOpenTree(LinkedEvent * event){
  if (expt && strcmp(event->tree,expt)) return;
  DoingListItem *doing;
  DoingListItem *next;
  XmListDeleteAllItems(ErrorWidget);
  XmListDeselectAllItems(LogWidget);
  for (doing = DoingList; doing; doing = next) {
    next = doing->next;
    free(doing);
  }
  DoingList = 0;
  _DoOpenTree(event);
  unique_tag_seed = 0;
}

static void Doing(LinkedEvent * event)
{
  DoingListItem *doing = malloc(sizeof(DoingListItem));
  DoingListItem *prev;
  DoingListItem *d;
  _Doing(event);
  int items;
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

static void Done(LinkedEvent * event){
  DoingListItem *doing;
  DoingListItem *prev;
  int *items;
  int num;
  XmListGetSelectedPos(LogWidget, &items, &num);
  if (items) free(items);
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
  _Done(event);
}

/*
static void ActivateImages(char* images)
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
