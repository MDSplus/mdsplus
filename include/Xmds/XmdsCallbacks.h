/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSCALLBACKS.H */
/*  *5     3-MAR-1993 13:33:09 JAS "Add prottypes" */
/*  *4    24-FEB-1993 15:07:50 JAS "adding declarations for the callback
 * routines" */
/*  *3    26-JUN-1992 15:18:49 JAS "Add menu callbacks" */
/*  *2    21-JAN-1992 11:35:10 JAS "add the on/off state" */
/*  *1    16-JAN-1992 10:59:12 JAS "Include file for xdbox callbacks" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSCALLBACKS.H */
/*------------------------------------------------------------------------------

                Name:   XMdsCallbacks

                Type:   C include

                Author:	Josh Stillerman

                Date:   16-JAN-1992

                Purpose: Callback structure definitions;

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
----------------------------------------------------------------------------*/

#ifndef XmdsCallbacks_H
#define XmdsCallbacks_H

typedef struct _XmdsButtonCallbackStruct
{
  int reason;
  XEvent *event;
  struct descriptor_xd *xd;
  Boolean on_off;
} XmdsButtonCallbackStruct;

typedef struct _XmdsMenuCallbackStruct
{
  int reason;
  XEvent *event;
  int button;
} XmdsMenuCallbackStruct;

/* reasons */
#define XmdsCR_RESET 0
#define XmdsCR_VALUE_CHANGED 1

/* callback routines */
#ifdef _NO_PROTO
extern int XmdsApplyCallback();
extern void XmdsCancelCallback();
extern void XmdsResetCallback();
extern void XmdsDestroyWidgetCallback();
extern void XmdsManageChildCallback();
extern void XmdsRegisterWidgetCallback();
extern void XmdsUnmanageChildCallback();
extern void XmdsOkCallback();

#else

extern int XmdsApplyCallback(Widget w);
extern void XmdsCancelCallback(Widget w);
extern void XmdsResetCallback(Widget w);
extern void XmdsDestroyWidgetCallback(Widget w1, Widget *w2);
extern void XmdsManageChildCallback(Widget w1, Widget *w2);
extern void XmdsRegisterWidgetCallback(Widget w1, Widget *w2);
extern void XmdsUnmanageChildCallback(Widget w1, Widget *w2);
extern void XmdsOkCallback(Widget w);
#endif

#endif
