/*  CMS REPLACEMENT HISTORY, Element XMDSWAVEFORM.H */
/*  *20   18-MAR-1997 09:52:37 TWF "Fix for DECC 5.3" */
/*  *19   20-SEP-1994 08:02:42 TWF "Change _NO_PROTO" */
/*  *18   24-MAR-1994 10:25:46 MRL "Fix print call." */
/*  *17   24-MAR-1994 08:52:57 TWF "Fix xmdswaveformprint" */
/*  *16   22-MAR-1994 14:12:12 MRL "Change print command." */
/*  *15   22-MAR-1994 12:02:19 TWF "Fix waveformprint" */
/*  *14    6-DEC-1993 14:41:37 TWF "Add ticks instead of grid" */
/*  *13   23-JUL-1993 15:03:49 TWF "Add disable_update flag" */
/*  *12   26-FEB-1993 11:27:42 JAS "port to decc" */
/*  *11   25-FEB-1993 12:21:49 JAS "" */
/*  *10   24-FEB-1993 14:58:43 TWF "Change print prototype" */
/*  *9    24-FEB-1993 12:15:36 JAS "port to decc" */
/*  *8    22-OCT-1992 12:29:52 TWF "Fix setwave" */
/*  *7    21-OCT-1992 10:25:20 TWF "Fix AddPoint and add MovePoint code" */
/*  *6    13-OCT-1992 12:38:57 TWF "Add closed resource" */
/*  *5    28-MAY-1992 15:55:48 TWF "Add animation resource" */
/*  *4    25-NOV-1991 09:52:58 TWF "Add points_only" */
/*  *3     1-NOV-1991 08:07:10 TWF "Change Update to take min/max's" */
/*  *2     8-OCT-1991 14:42:44 TWF "Include prototypes for convenience routines"
 */
/*  *1     8-OCT-1991 13:43:05 TWF "XmdsWaveform public include" */
/*  CMS REPLACEMENT HISTORY, Element XMDSWAVEFORM.H */
/*------------------------------------------------------------------------------

                Name:   WAVEFORM.H

                Type:   C include

                Author:	Thomas W. Fredian

                Date:   29-MAY-1990

                Purpose: Public declarations for WAVEFORM widget

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
----------------------------------------------------------------------------*/

#ifndef _XMDSWAVEFORM_H
#define _XMDSWAVEFORM_H
#include <stdio.h>
#ifndef _Xm_h
#include <Xm/Xm.h>
#endif

/* Global Resources */

#define XmdsNtitle "title" /* Title string displayed at top of waveform */
#define XmdsNpointerMode \
  "pointerMode"                    /* Selects pointer mode (i.e. zoom, crosshairs, drag) */
#define XmdsNlabelFont "labelFont" /* Font used for title and axis labels */
#define XmdsNshowMode \
  "showMode"                                 /* Show lines, points, or both (box at data points) */
#define XmdsNshowSelections "showSelections" /* Show selection points */
#define XmdsNstepPlot "stepPlot"             /* Step plot mode */
#define XmdsNcount "count"                   /* Number of X,Y datapoints */
#define XmdsNselections "selections"         /* Selections (char [count]) */
#define XmdsNselectionsValStruct \
  "selectionsValStruct"        /* Selections (using value struct mechanism) */
#define XmdsNpenDown "penDown" /* Pen down (char [count]) */
#define XmdsNpenDownValStruct \
  "penDownValStruct"           /* Pen down (using value struct mechanism) */
#define XmdsNpanWith "panWith" /* Pan with widget */
#define XmdsNattachCrosshairs \
  "attachCrosshairs" /* Attach crosshairs to curve - boolean */
#define XmdsNdisabled \
  "disabled" /* True = disable drawing until disabled cleared */
#define XmdsNclosed \
  "closed"                         /* True = connect last point to first point - boolean */
#define XmdsNgridStyle "gridStyle" /* Select type of grid */

/* X-axis Resources */

#define XmdsNxValue "xValue" /* float X-values[count] */
#define XmdsNxValStruct \
  "xValStruct"                       /* X-values (using value struct mechanism) */
#define XmdsNxMin "xMin"             /* float *min_x */
#define XmdsNxMax "xMax"             /* float *max_x */
#define XmdsNxCrosshair "xCrosshair" /* float *x_crosshair */
#define XmdsNxGridLines "xGridLines" /* int number of grid lines */
#define XmdsNxLabels "xLabels"       /* boolean display labels */

/* Y-axis Resources */

#define XmdsNyValue "yValue" /* SAME AS X-axis Resources */
#define XmdsNyValStruct "yValStruct"
#define XmdsNyMin "yMin"
#define XmdsNyMax "yMax"
#define XmdsNyCrosshair "yCrosshair"
#define XmdsNyGridLines "yGridLines"
#define XmdsNyLabels "yLabels"

/* Callbacks */

#define XmdsNbutton3Callback \
  "button3Callback" /* Callback when button 3 is pressed */
#define XmdsNcrosshairsCallback                                                \
  "crosshairsCallback" /* Callback when crosshairs change position in waveform \
                          window */
#define XmdsNalignCallback                                                 \
  "alignCallback" /* Callback when right button is depressed in crosshairs \
                     pointer mode */
#define XmdsNlimitsCallback                                                    \
  "limitsCallback" /* Callback when change in max/min axis values (after zoom, \
                      pan, etc.) */
#define XmdsNunderlayCallback \
  "underlayCallback" /* Callback to write to window before waveform */
#define XmdsNoverlayCallback \
  "overlayCallback"                        /* Callback to write to window after waveform */
#define XmdsNcutCallback "cutCallback"     /* Callback when cut occurs */
#define XmdsNpasteCallback "pasteCallback" /* Callback when paste occurs */

/* Value Struct resource mechanism */

typedef struct _XmdsWaveformValStruct
{
  unsigned int size;
  caddr_t addr;
  void (*destroy)(); /* Destroy called with widget and destroy arg */
  caddr_t destroy_arg;
} XmdsWaveformValStruct, *XmdsWaveformValStructPtr;
/* Callback structures */

typedef struct
{
  int reason;
  XEvent *event;
  float x;
  float y;
} XmdsWaveformCrosshairsCBStruct;

typedef struct
{
  int reason;
  XEvent *event;
  float *xminval;
  float *xmaxval;
  float *yminval;
  float *ymaxval;
} XmdsWaveformLimitsCBStruct;
/* Callback reasons */

#define XmdsCRCrosshairs 1
#define XmdsCRAlign 2
#define XmdsCRZoomIn 3
#define XmdsCRZoomOut 4
#define XmdsCRBoxZoom 5
#define XmdsCRDrag 6
#define XmdsCRButton3 7
#define XmdsCRNewData 8
#define XmdsCRDragEnd 9

#define XmdsPOINTER_MODE_USER 0
#define XmdsPOINTER_MODE_NONE 1
#define XmdsPOINTER_MODE_POINT 2
#define XmdsPOINTER_MODE_ZOOM 3
#define XmdsPOINTER_MODE_DRAG 4
#define XmdsPOINTER_MODE_EDIT 5

#define XmdsSHOW_MODE_LINE 0
#define XmdsSHOW_MODE_POINTS 1
#define XmdsSHOW_MODE_BOTH 2

enum XmdsWaveformMotionRestriction
{
  XmdsMOTION_UNSELECTED,
  XmdsMOTION_BOTH,
  XmdsMOTION_XONLY,
  XmdsMOTION_YONLY,
  XmdsMOTION_NONE
};

enum XmdsWaveformGridStyle
{
  XmdsGRID_STYLE_LINES,
  XmdsGRID_STYLE_TICKS
};
/* Class record constants */

externalref WidgetClass xmdsWaveformWidgetClass;
externalref struct _XmdsWaveformClassRec xmdsWaveformClassRec;

typedef struct _XmdsWaveformClassRec *XmdsWaveformWidgetClass;
typedef struct _XmdsWaveformRec *XmdsWaveformWidget;
/* fast XtIsSubclass define */
#ifndef XmdsIsWaveform
#define XmdsIsWaveform(w) XtIsSubclass(w, xmdsWaveformWidgetClass)
#endif

/* External creation routines:
 */

extern Widget XmdsCreateWaveform(Widget parent, char *name, ArgList al,
                                 Cardinal ac);
extern void XmdsWaveformPrint(Widget w, FILE *fid, int width, int height,
                              int rotate, char *title, char *window_title,
                              int resolution);
extern void XmdsWaveformReverse(Widget w, int reverse);
extern void XmdsWaveformSetCrosshairs(Widget w, float *x, float *y,
                                      Boolean attach);
extern void XmdsWaveformSetPointerMode(Widget w, int mode);
extern void XmdsWaveformUpdate(Widget w, XmdsWaveformValStruct *x,
                               XmdsWaveformValStruct *y, char *title,
                               float *xmin, float *xmax, float *ymin,
                               float *ymax, Boolean defer_update);
extern void XmdsWaveformSetWave(Widget w, int count, float *x, float *y,
                                Boolean *select, Boolean *pendown,
                                Boolean autoscale, Boolean defer_update);
#endif //ifndef _XMDSWAVEFORM_H
