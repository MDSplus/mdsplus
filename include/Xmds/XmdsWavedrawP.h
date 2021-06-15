/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSWAVEDRAWP.H */
/*  *7     6-MAY-1993 13:51:40 TWF "Add snap to grid" */
/*  *6    25-FEB-1993 15:09:34 JAS "port to decc" */
/*  *5    22-DEC-1992 11:33:06 TWF "Add DeleteOnlySelected" */
/*  *4    21-OCT-1992 17:07:32 TWF "Add XmdsWavedrawMovePoint" */
/*  *3    21-OCT-1992 10:30:52 TWF "Fix AddPoint and add MovePoint code" */
/*  *2     5-OCT-1992 14:22:11 TWF "Add new resources" */
/*  *1    18-DEC-1991 10:36:05 TWF "Wavedraw widget" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSWAVEDRAWP.H */
/*------------------------------------------------------------------------------

                Name:   WAVEDRAWP.H

                Type:   C include

                Author:	Thomas W. Fredian

                Date:   29-MAY-1990

                Purpose: Private declarations for WAVEDRAW widget

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
----------------------------------------------------------------------------*/

#ifndef _XmdsWavedrawP_h
#define _XmdsWavedrawP_h

#include <Xmds/XmdsWavedraw.h>
#include <Xmds/XmdsWaveformP.h>

typedef struct _XmdsWavedrawClassPart
{
  XtTranslations draw1_trans;
  XtTranslations draw2_trans;
  XtTranslations setpen_trans;
  Boolean (*add_point_proc)(XmdsWavedrawWidget w, int idx, float *newx,
                            float *newy, Boolean callcallbacks,
                            enum XmdsWaveformMotionRestriction motion);
  void (*delete_point_proc)(XmdsWavedrawWidget w, int idx,
                            Boolean callcallbacks);
  Boolean (*move_point_proc)(XmdsWavedrawWidget w, int idx, float *desired_x,
                             float *desired_y, float *new_x, float *new_y,
                             Boolean callcallbacks, XEvent *event);
  caddr_t extension;
} XmdsWavedrawClassPart;

typedef struct _XmdsWavedrawClassRec
{
  CoreClassPart core_class;
  XmPrimitiveClassPart primitive_class;
  XmdsWaveformClassPart waveform_class;
  XmdsWavedrawClassPart wavedraw_class;
} XmdsWavedrawClassRec;

externalref XmdsWavedrawClassRec xmdsWavedrawClassRec;

typedef struct _XmdsWavedrawAxis
{
  float *low;
  float *high;
  float *min_distance;
  Boolean increasing;
  int num_choices;
  float *choices;
  float *grid_snap;
} XmdsWavedrawAxis;

typedef struct _XmdsWavedrawPart
{
  XtCallbackList move_callback;
  XtCallbackList select_callback;
  XtCallbackList deselect_callback;
  XtCallbackList add_point_callback;
  XtCallbackList delete_point_callback;
  XtCallbackList fit_callback;
  int num_selections;
  int selection_idx;
  Boolean delete_only_selected;
  XmdsWavedrawAxis x;
  XmdsWavedrawAxis y;
  float *min_distance;
} XmdsWavedrawPart;

/* Full instance record declaration */
typedef struct _XmdsWavedrawRec
{
  CorePart core;
  XmPrimitivePart common;
  XmdsWaveformPart waveform;
  XmdsWavedrawPart wavedraw;
} XmdsWavedrawRec;

#endif /* _XmdsWaveformP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
