/*  CMS REPLACEMENT HISTORY, Element XMDSWAVEDRAW.H */
/*  *14   18-MAR-1997 09:52:11 TWF "Fix for DECC 5.3" */
/*  *13   18-MAR-1997 09:44:47 TWF "Fix for DECC 5.3" */
/*  *12   19-SEP-1994 16:53:12 TWF "Fix compile error" */
/*  *11    6-MAY-1993 13:51:37 TWF "Add snap to grid" */
/*  *10    3-MAR-1993 14:45:01 JAS "Add prottypes" */
/*  *9    22-DEC-1992 11:33:03 TWF "Add DeleteOnlySelected" */
/*  *8    21-OCT-1992 10:32:55 TWF "Fix AddPoint and add MovePoint code" */
/*  *7    21-OCT-1992 10:25:53 TWF "Fix AddPoint and add MovePoint code" */
/*  *6     5-OCT-1992 14:54:38 TWF "Add new resources" */
/*  *5     5-OCT-1992 14:22:06 TWF "Add new resources" */
/*  *4     1-OCT-1992 15:31:28 TWF "Add more callback reasons" */
/*  *3    19-DEC-1991 09:53:30 TWF "Add routine prototypes" */
/*  *2    18-DEC-1991 16:53:52 TWF "Change callback struct names" */
/*  *1    18-DEC-1991 10:36:18 TWF "Wavedraw widget" */
/*  CMS REPLACEMENT HISTORY, Element XMDSWAVEDRAW.H */
/*------------------------------------------------------------------------------

                Name:   WAVEDRAW.H

                Type:   C include

                Author:	Thomas W. Fredian

                Date:   29-MAY-1990

                Purpose: Public declarations for WAVEDRAW widget

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
----------------------------------------------------------------------------*/

#ifndef _XmdsWavedraw_h
#define _XmdsWavedraw_h

#ifndef _Xm_h
#include <Xm/Xm.h>
#endif

#ifndef _XmdsWaveform_h
#include <Xmds/XmdsWaveform.h>
#endif

/* Resources */

#define XmdsNmoveCallback "moveCallback"
#define XmdsNselectCallback "selectCallback"
#define XmdsNdeselectCallback "deselectCallback"
#define XmdsNaddPointCallback "addPointCallback"
#define XmdsNdeletePointCallback "deletePointCallback"
#define XmdsNfitCallback "fitCallback"
#define XmdsNdeleteOnlySelected "deleteOnlySelected"
#define XmdsNlowX "lowX"
#define XmdsNlowY "lowY"
#define XmdsNhighX "highX"
#define XmdsNhighY "highY"
#define XmdsNxMinDistance "xMinDistance"
#define XmdsNyMinDistance "yMinDistance"
#define XmdsNxIncreasing "xIncreasing"
#define XmdsNyIncreasing "yIncreasing"
#define XmdsNxNumChoices "xNumChoices"
#define XmdsNxChoices "xChoices"
#define XmdsNyNumChoices "yNumChoices"
#define XmdsNyChoices "yChoices"
#define XmdsNxGridSnap "xGridSnap"
#define XmdsNyGridSnap "yGridSnap"

/* Callback structures */

typedef struct
{
  int reason;
  XEvent *event;
  int idx;
  float oldx;
  float oldy;
  float newx;
  float newy;
  int count;
  float *x;
  float *y;
  Boolean *selected;
} XmdsWavedrawValueCBStruct;

typedef struct
{
  int reason;
  XEvent *event;
  int count;
  float *x;
  float *y;
  Boolean *selected;
  Boolean *pen_down;
} XmdsWavedrawFitCBStruct;

/* Callback reasons */

#define XmdsCRMovePointBegin 10
#define XmdsCRMovePoint 11
#define XmdsCRMovePointEnd 12
#define XmdsCRSelectPoint 13
#define XmdsCRDeselectPoint 14
#define XmdsCRAddPoint 15
#define XmdsCRDeletePoint 16
#define XmdsCRSetPen 17

#define XmdsPOINTER_MODE_DRAW1 6
#define XmdsPOINTER_MODE_SELECT 6
#define XmdsPOINTER_MODE_DRAW2 7
#define XmdsPOINTER_MODE_ADD_DELETE 7
#define XmdsPOINTER_MODE_SET_PEN 8
#define XmdsPOINTER_MODE_SLIDE_STRETCH 9

/* Class record pointer */
externalref WidgetClass xmdsWavedrawWidgetClass;
externalref struct _XmdsWavedrawClassRec xmdsWavedrawClassRec;

typedef struct _XmdsWavedrawClassRec *XmdsWavedrawWidgetClass;
/* C Widget type definition */
typedef struct _XmdsWavedrawRec *XmdsWavedrawWidget;

/* fast XtIsSubclass define */
#ifndef XmdsIsWavedraw
#define XmdsIsWavedraw(w) XtIsSubclass(w, xmdsWavedrawWidgetClass)
#endif

/* External creation routines:
 */
#ifdef _NO_PROTO_

extern Widget XmdsCreateWavedraw();
extern Boolean XmdsWavedrawAddPoint();
extern void XmdsWavedrawDeletePoint();
extern Boolean XmdsWavedrawMovePoint();

#else /* _NO_PROTO_ */

extern Widget XmdsCreateWavedraw(Widget parent, char *name, ArgList args,
                                 Cardinal argcount);
extern Boolean XmdsWavedrawAddPoint(Widget w, int idx, float *newx, float *newy,
                                    Boolean callcallbacks,
                                    enum XmdsWaveformMotionRestriction motion);
extern void XmdsWavedrawDeletePoint(Widget w, int idx, Boolean callcallbacks);
extern Boolean XmdsWavedrawMovePoint(XmdsWavedrawWidget w, int idx,
                                     float *desired_x, float *desired_y,
                                     float *new_x, float *new_y,
                                     Boolean callcallbacks, XEvent *event);

#endif /* _NO_PROTO_ */

#endif /* _XmdsWavedraw */
