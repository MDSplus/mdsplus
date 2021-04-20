/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSWAVEFORMP.H */
/*  *38    3-OCT-1994 12:14:22 TWF "Unix port" */
/*  *37   24-JUN-1994 15:33:00 TWF "Motif 1.2" */
/*  *36   23-JUN-1994 08:57:19 TWF "Fix for motif 1.2" */
/*  *35   23-JUN-1994 08:52:40 TWF "Fix for motif 1.2" */
/*  *34   23-JUN-1994 08:51:12 TWF "Fix for motif 1.2" */
/*  *33   23-JUN-1994 08:39:42 TWF "Fix for motif 1.2" */
/*  *32   23-JUN-1994 08:31:31 TWF "Fix for motif 1.2" */
/*  *31   23-JUN-1994 08:16:43 TWF "Fix for motif 1.2" */
/*  *30   22-JUN-1994 16:59:56 TWF "motif 1.2" */
/*  *29   22-JUN-1994 16:40:07 TWF "motif" */
/*  *28   22-JUN-1994 16:36:35 TWF "Add new inherits" */
/*  *27   22-JUN-1994 16:18:58 TWF "Motif 1.2" */
/*  *26   22-JUN-1994 14:36:09 TWF "Add Xmprimitive" */
/*  *25   22-MAR-1994 12:12:39 TWF "Take out prototypes" */
/*  *24   23-FEB-1994 09:37:08 TWF "revert" */
/*  *23   23-FEB-1994 08:33:39 TWF "Test moving memory" */
/*  *22   23-FEB-1994 08:22:09 TWF "End of test" */
/*  *21   23-FEB-1994 08:13:27 TWF "Test shifting memory" */
/*  *20    6-DEC-1993 14:41:41 TWF "Add ticks instead of grid" */
/*  *19   23-JUL-1993 15:03:52 TWF "Add disable_update flag" */
/*  *18   12-JUL-1993 09:25:11 TWF "Revert back to original" */
/*  *17   26-FEB-1993 11:28:27 JAS "port to decc" */
/*  *16   25-FEB-1993 12:28:02 JAS "" */
/*  *15   25-FEB-1993 12:14:55 JAS "port to decc" */
/*  *14   25-FEB-1993 11:42:27 JAS "add all of the inherits" */
/*  *13   25-FEB-1993 11:25:15 JAS "port to decc" */
/*  *12   13-OCT-1992 12:48:24 TWF "Add closed resource" */
/*  *11   13-OCT-1992 12:39:02 TWF "Add closed resource" */
/*  *10   28-MAY-1992 15:55:52 TWF "Add animation resource" */
/*  *9     8-APR-1992 10:08:08 TWF "Fix backing store" */
/*  *8     7-APR-1992 14:01:41 TWF "Alternate method if no backing store" */
/*  *7    11-DEC-1991 09:55:26 TWF "Fix reverse mode plotting" */
/*  *6    10-DEC-1991 17:45:09 TWF "Fix redisplay with backingstore" */
/*  *5     5-DEC-1991 10:10:06 TWF "Fix colors" */
/*  *4    25-NOV-1991 09:53:04 TWF "Add points_only" */
/*  *3     8-NOV-1991 15:54:36 TWF "Release pixvalues ASAP" */
/*  *2     8-NOV-1991 15:00:41 TWF "Make sure deferred updates get drawn" */
/*  *1     8-OCT-1991 13:44:36 TWF "XmdsWaveform private include" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSWAVEFORMP.H */
/*------------------------------------------------------------------------------

                Name:   XmdsWaveformP.H

                Type:   C include

                Author:	Thomas W. Fredian

                Date:   9-OCT-1991

                Purpose: Private declarations for WAVEFORM widget

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
----------------------------------------------------------------------------*/

#ifndef _XmdsWaveformP_h
#define _XmdsWaveformP_h

#include <Xm/XmP.h>
#include <Xmds/XmdsWaveform.h>

#ifndef XmPRIMITIVE
#include <Xm/PrimitiveP.h>
#endif

#include <stdio.h>

/* New fields for the Waveform widget class record */

#ifndef XmInheritBorderHighlight /* Motif 1.1 */
#define XmInheritBorderHighlight ((XtWidgetProc)_XtInherit)
#define XmInheritBorderUnhighlight ((XtWidgetProc)_XtInherit)
#define XmInheritArmAndActivate ((XtWidgetProc)_XtInherit)
#endif

#define XmdsInheritTranslations ((XtTranslations)_XtInherit)
#define XmdsInheritUpdate                                                      \
  ((void (*)(Widget, XmdsWaveformValStruct *, XmdsWaveformValStruct *, char *, \
             float *, float *, float *, float *, Boolean))_XtInherit)
#define XmdsInheritSetCrosshairs \
  ((void (*)(Widget, float *, float *, Boolean))_XtInherit)
#define XmdsInheritSetPointerMode ((void (*)(Widget, int))_XtInherit)
#define XmdsInheritPrint \
  ((void (*)(Widget, FILE *, int, int, int, char *, char *, int))_XtInherit)
#define XmdsInheritReverse ((void (*)(Widget, int))_XtInherit)
#define XmdsInheritSetWave                                                 \
  ((void (*)(Widget, int, float *, float *, Boolean *, Boolean *, Boolean, \
             Boolean))_XtInherit)

typedef struct _WaveformClassPart
{
  XtTranslations default_trans;
  XtTranslations zoom_trans;
  XtTranslations drag_trans;
  XtTranslations point_trans;
  XtTranslations edit_trans;
  void (*update_proc)(Widget, XmdsWaveformValStruct *, XmdsWaveformValStruct *,
                      char *, float *, float *, float *, float *, Boolean);
  /* WaveformUpdate(widget w,WaveformValStruct *x,WaveformValStruct *y,char
   * *title,Boolean autoscale) */
  void (*set_crosshairs_proc)(Widget, float *, float *, Boolean);
  /* WaveformSetCrosshairs(widget w,float *x,float *y, Boolean attach) */
  void (*set_pointer_mode_proc)(Widget, int);
  /* WaveformSetPointerMode(widget w,int mode) */
  void (*print_proc)(Widget, FILE *, int, int, int, char *, char *, int);
  /* WaveformPrint(widget w, FILE *filefid, int inp_total_width, int
     inp_total_height, int inp_rotate, char *title, char *window_title, int
     inp_resolution); */
  void (*reverse_proc)(Widget, int);
  /* WaveformReverse(widget w,int reverse) */
  void (*set_wave_proc)(Widget, int, float *, float *, Boolean *, Boolean *,
                        Boolean, Boolean);
  /* WaveformSetWave(widget w, int count, float *x, float *y, Boolean *select,
   * Boolean *pendown, Boolean autoscale, Boolean defer) */
  caddr_t extension;
} XmdsWaveformClassPart;
/* Full class record declaration */

typedef struct _XmdsWaveformClassRec
{
  CoreClassPart core_class;
  XmPrimitiveClassPart primitive_class;
  XmdsWaveformClassPart waveform_class;
} XmdsWaveformClassRec;

externalref XmdsWaveformClassRec xmdsWaveformClassRec;
/* New fields for the Waveform widget record */

/* Fields describing each axis */

typedef struct _XmdsWaveformAxis
{
  float *minval;
  float *maxval;
  double crosshair;
  int grid_lines;
  Boolean labels;
  int motion;
  float *val_ptr;
  XmdsWaveformValStruct *val_struct;
  float *value;
  int *pix_value;
  int offset;
  double pointer_resolution;
} XmdsWaveformAxis;
/* Fields global to widget */

typedef struct _WaveformPart
{
  char *title;
  int pointer_mode;
  int grid_style;
  XtCallbackList crosshairs_callback;
  XtCallbackList align_callback;
  XtCallbackList limits_callback;
  XtCallbackList button3_callback;
  XtCallbackList underlay_callback;
  XtCallbackList overlay_callback;
  XtCallbackList cut_callback;
  XtCallbackList paste_callback;
  Widget pan_with;
  Boolean attach_crosshairs;
  GC plot_gc;
  GC grid_gc;
  GC flip_gc;
  GC erase_gc;
  int count;
  Boolean *selections;
  Boolean *selections_val_ptr;
  XmdsWaveformValStruct *selections_val_struct;
  Boolean *pen_down;
  Boolean *pen_down_val_ptr;
  XmdsWaveformValStruct *pen_down_val_struct;
  XFontStruct *font_struct;
  union {
    XSegment crosshairs_pix[2];
    XRectangle rectangle[2];
  } cross_box;
  Boolean panning;
  Boolean show_mode;
  Boolean show_selections;
  Boolean step_plot;
  Pixmap pixmap;
  Window drawable;
  XmdsWaveformAxis x;
  XmdsWaveformAxis y;
  Boolean reverse;
  Boolean disabled;
  Boolean redraw;
  Boolean closed;
} XmdsWaveformPart;
/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _XmdsWaveformRec
{
  CorePart core;
  XmPrimitivePart primitive;
  XmdsWaveformPart waveform;
} XmdsWaveformRec;

#endif /* _XmdsWaveformP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
