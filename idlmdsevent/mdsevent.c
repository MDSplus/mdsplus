/*------------------------------------------------------------------------------

                Name:   MDSEVENTV5

                Type:   C function

                Author: TOM FREDIAN

                Date:   14-DEC-1993

                Purpose:  Implement MDS events in IDL Widget applications

------------------------------------------------------------------------------

        Call sequence:

EventStruct *MDSEVENT(int *base_id, int *stub_id, struct dsc$descriptor *name)

------------------------------------------------------------------------------
   Copyright (c) 1993
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

        Description:

Invoked from MDSEVENT.PRO

------------------------------------------------------------------------------*/
#include <ipdesc.h>

typedef struct _event_struct { int stub_id;
                               int base_id;
                               int *event_id;
                               int  pid;
                               char name[28];
                               char value[12];
                             } EventStruct;

#ifdef WIN32
#include <windows.h>
extern void IDL_WidgetStubLock(int lock);
extern char *IDL_WidgetStubLookup(int id);
extern void IDL_WidgetIssueStubEvent(char *stub_rec, EventStruct *e);
extern void IDL_WidgetGetStubIds(char *stub_rec, HWND *wid1, HWND *wid2);
#else
#include <X11/Intrinsic.h>
#include <stdio.h>
extern XtInputCallbackProc MdsDispatchEvent();
static XtInputId XTINPUTID=0;
#endif

static void EventAst(EventStruct *e);
#include <export.h>

int IDLMdsEventCan(int argc, void * *argv)
{
	SOCKET sock = (SOCKET)argv[0];
	unsigned long eventid = (unsigned long)argv[1];
	return MdsEventCan(sock, (void *)eventid);
}

EventStruct *IDLMdsEvent(int argc, void * *argv)
{
  SOCKET sock = (SOCKET)argv[0];
  int *base_id = (int *)argv[1];
  int *stub_id = (int *)argv[2];
  char *name = (char *)argv[3];
  EventStruct *e = (EventStruct *)malloc(sizeof(EventStruct));
  if (MdsEventAst(sock, name,(void (*)(int))EventAst,e,(void *)&e->event_id) & 1)
  {
    char *parent_rec;
    char *stub_rec;
    IDL_WidgetStubLock(TRUE);
    if ((parent_rec = IDL_WidgetStubLookup(*base_id))
        && (stub_rec = IDL_WidgetStubLookup(*stub_id)))
    {
//	  IDL_WidgetSetStubIds(stub_rec, parent_rec, parent_rec);  
      if (!XTINPUTID) {
        Widget w1, w2;
        IDL_WidgetGetStubIds(parent_rec, (unsigned long *)&w1, (unsigned long *)&w2);
        XTINPUTID = XtAppAddInput(XtWidgetToApplicationContext(w1), sock,  (XtPointer)XtInputExceptMask, MdsDispatchEvent, sock);
      }
      e->stub_id = *stub_id;
      e->base_id = *base_id;
      strncpy(e->name, name, sizeof(e->name));

      IDL_WidgetStubLock(FALSE);
      return e;
    }
  }
//  free((char *)e);
  return 0;
}

static void EventAst(EventStruct *e)
{
  char *stub_rec;
  char *base_rec;
  IDL_WidgetStubLock(TRUE);
  if ((stub_rec = IDL_WidgetStubLookup(e->stub_id)) && (base_rec = IDL_WidgetStubLookup(e->base_id)))
  {
#ifdef WIN32
	HWND wid1, wid2;
	IDL_WidgetGetStubIds(stub_rec, &wid1, &wid2);
#endif
        IDL_WidgetIssueStubEvent(stub_rec, (IDL_LONG)e);
#ifdef WIN32
	PostMessage(wid1, WM_MOUSEMOVE, (WPARAM)NULL, (LPARAM)NULL);
#else
  {
    Widget top;
    Widget w;
    IDL_WidgetGetStubIds(base_rec, (unsigned long *)&top, (unsigned long *)&w);
    if (w)
    {
      XClientMessageEvent event;
      event.type = ClientMessage;
      event.display = XtDisplay(top);
      event.window = XtWindow(top);
      event.format = 8;
      XSendEvent(XtDisplay(top),XtWindow(top),TRUE,0,(XEvent *)&event);
      XFlush(XtDisplay(top));
    }
  }
#endif
  }

  IDL_WidgetStubLock(FALSE);
}
