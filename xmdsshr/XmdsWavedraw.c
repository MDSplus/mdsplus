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
/*  CMS REPLACEMENT HISTORY, Element XMDSWAVEDRAW.C */
/*  *92   18-MAR-1997 10:22:42 TWF "Fix for DECC 5.3" */
/*  *91   18-MAR-1997 09:31:16 TWF "Fix for DECC 5.3" */
/*  *90    4-NOV-1994 14:43:04 TWF "Fix gridsnap" */
/*  *89   19-OCT-1994 13:35:21 TWF "Nolonger support VAXC" */
/*  *88   22-JUN-1994 16:36:30 TWF "Add new inherits" */
/*  *87   22-JUN-1994 15:25:38 TWF "Compile error with Motif 1.2" */
/*  *86   22-FEB-1994 14:38:42 TWF "Remove NO_X_GBLS" */
/*  *85   27-SEP-1993 11:14:17 TWF "set selection idx to -1 if no knots" */
/*  *84   23-JUL-1993 15:18:10 TWF "Add disable_update flag" */
/*  *83   23-JUL-1993 15:09:08 TWF "Add disable_update flag" */
/*  *82   23-JUL-1993 15:07:16 TWF "Add disable_update flag" */
/*  *81    8-JUL-1993 11:48:25 TWF "Fix slidestretch return" */
/*  *80   17-MAY-1993 15:00:01 TWF "" */
/*  *79   17-MAY-1993 14:33:54 TWF "Fix" */
/*  *78   17-MAY-1993 14:11:37 TWF "Run added points through MovePoint check" */
/*  *77    6-MAY-1993 15:12:15 TWF "Fix xlow,xhigh,ylow,yhigh if not increasing" */
/*  *76    6-MAY-1993 15:04:57 TWF "snap" */
/*  *75    6-MAY-1993 15:03:08 TWF "snap" */
/*  *74    6-MAY-1993 14:53:31 TWF "Fix snap" */
/*  *73    6-MAY-1993 13:58:20 TWF "Add snap to grid" */
/*  *72    6-MAY-1993 13:54:42 TWF "Add snap to grid" */
/*  *71    6-MAY-1993 13:51:32 TWF "Add snap to grid" */
/*  *70   29-APR-1993 09:05:45 TWF "Fix shift lock move" */
/*  *69   15-APR-1993 12:23:26 TWF "Fix WidgetClasses" */
/*  *68   15-APR-1993 12:21:01 TWF "Fix WidgetClasses" */
/*  *67   11-MAR-1993 13:43:24 TWF "Remove fixup" */
/*  *66   11-MAR-1993 13:41:47 TWF "Remove fixup" */
/*  *65    4-MAR-1993 15:04:18 JAS "make it complile with vaxc" */
/*  *64   25-FEB-1993 15:08:20 JAS "port to decc" */
/*  *63   22-DEC-1992 11:37:56 TWF "Add DeleteOnlySelected" */
/*  *62   22-DEC-1992 11:32:57 TWF "Add DeleteOnlySelected" */
/*  *61   21-DEC-1992 12:17:16 TWF "Fix movepoint" */
/*  *60   21-DEC-1992 12:12:07 TWF "Fix movepoint" */
/*  *59   27-OCT-1992 08:35:13 TWF "watch out for roprand" */
/*  *58   22-OCT-1992 09:16:27 TWF "Do not unselect to change movement restrictions" */
/*  *57   22-OCT-1992 09:05:54 TWF "Update curve if callcallbacks on move" */
/*  *56   21-OCT-1992 17:14:48 TWF "Add XmdsWavedrawMovePoint" */
/*  *55   21-OCT-1992 17:07:20 TWF "Add XmdsWavedrawMovePoint" */
/*  *54   21-OCT-1992 16:03:13 TWF "Fix move" */
/*  *53   21-OCT-1992 15:52:34 TWF "Fix move" */
/*  *52   21-OCT-1992 15:42:45 TWF "Fix Move routine" */
/*  *51   21-OCT-1992 10:35:01 TWF "Fix AddPoint and add MovePoint code" */
/*  *50   21-OCT-1992 10:25:59 TWF "Fix AddPoint and add MovePoint code" */
/*  *49   20-OCT-1992 10:06:01 TWF "Fix motion restriction" */
/*  *48   20-OCT-1992 09:59:14 TWF "Restrict movement" */
/*  *47   20-OCT-1992 09:33:16 TWF "Add motion restrictions to selections" */
/*  *46   20-OCT-1992 09:04:46 TWF "Fix move" */
/*  *45   19-OCT-1992 17:34:50 TWF "Fix move" */
/*  *44   19-OCT-1992 17:21:04 TWF "Fix xincreasing on move" */
/*  *43   19-OCT-1992 17:18:34 TWF "Fix xincreasing on move" */
/*  *42   19-OCT-1992 16:40:53 TWF "Change add point" */
/*  *41   19-OCT-1992 15:34:41 TWF "Update after addpoint call" */
/*  *40   13-OCT-1992 12:09:12 TWF "Fix yincreasing" */
/*  *39    6-OCT-1992 14:53:34 TWF "Fix" */
/*  *38    6-OCT-1992 14:43:19 TWF "Add spline fitting" */
/*  *37    5-OCT-1992 17:08:40 TWF "Fix y choices" */
/*  *36    5-OCT-1992 17:01:22 TWF "Fix y choices" */
/*  *35    5-OCT-1992 16:39:25 TWF "Fix y choices" */
/*  *34    5-OCT-1992 16:08:22 TWF "Add slide_stretch pointer mode" */
/*  *33    5-OCT-1992 15:39:44 TWF "Add new resources" */
/*  *32    5-OCT-1992 14:54:44 TWF "Add new resources" */
/*  *31    5-OCT-1992 14:21:54 TWF "Add new resources" */
/*  *30    5-OCT-1992 09:58:41 TWF "Do not move points if animatin" */
/*  *29    5-OCT-1992 09:41:46 TWF "Do not update x and y until after callback" */
/*  *28    5-OCT-1992 09:26:07 TWF "Add SetWave" */
/*  *27    1-OCT-1992 15:31:19 TWF "Add more callback reasons" */
/*  *26   21-SEP-1992 12:21:45 TWF "Fix SelectedSegment algorithm" */
/*  *25   21-SEP-1992 11:41:35 TWF "Fix SelectedSegment algorithm" */
/*  *24   21-SEP-1992 10:27:51 TWF "Fix SelectedSegment algorithm" */
/*  *23   21-SEP-1992 10:00:24 TWF "Fix SelectedSegment algorithm" */
/*  *22   21-SEP-1992 09:49:06 TWF "Fix SelectedSegment algorithm" */
/*  *21   21-SEP-1992 09:25:22 TWF "Fix SelectedSegment algorithm" */
/*  *20   21-SEP-1992 09:23:35 TWF "Fix SelectedSegment algorithm" */
/*  *19   21-SEP-1992 09:03:38 TWF "Fix control" */
/*  *18   21-SEP-1992 09:00:33 TWF "Fix control" */
/*  *17   26-AUG-1992 11:36:41 TWF "Check out redisplay" */
/*  *16   28-MAY-1992 10:20:44 TWF "Only warp pointer if necessary" */
/*  *15   28-MAY-1992 09:32:15 TWF "Only warp pointer if necessary" */
/*  *14   27-MAY-1992 11:09:36 TWF "Allow modifiers on move action" */
/*  *13   27-MAY-1992 11:05:52 TWF "Allow modifiers on move action" */
/*  *12   20-DEC-1991 12:19:27 TWF "Inherit stuff" */
/*  *11   20-DEC-1991 12:16:36 TWF "Inherit redisplay" */
/*  *10   20-DEC-1991 11:47:10 TWF "Fix XPix" */
/*  *9    20-DEC-1991 11:39:02 TWF "Fix redisplay" */
/*  *8    19-DEC-1991 11:37:44 TWF "Fix widget size" */
/*  *7    19-DEC-1991 11:17:17 TWF "Fix widget size" */
/*  *6    19-DEC-1991 09:45:35 TWF "Add routine prototypes" */
/*  *5    18-DEC-1991 16:53:41 TWF "Change callback struct names" */
/*  *4    18-DEC-1991 11:40:45 TWF "remove call to refresh" */
/*  *3    18-DEC-1991 11:35:51 TWF "remove call to refresh" */
/*  *2    18-DEC-1991 10:40:48 TWF "remove unnessary include" */
/*  *1    18-DEC-1991 10:35:37 TWF "Wavedraw widget" */
/*  CMS REPLACEMENT HISTORY, Element XMDSWAVEDRAW.C */
/*------------------------------------------------------------------------------

		Name:   XmdsWavedraw

		Type:   C function

		Author:	Thomas W. Fredian

		Date:   29-MAY-1990

		Purpose: Create an WAVEDRAW widget

------------------------------------------------------------------------------

	Call sequence:

[MIT C]
int XmdsCreateWavedraw( parent, name, args, argcount )

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

 External functions or symbols referenced:                                    */

/*------------------------------------------------------------------------------

 Subroutines referenced:                                                      */

/*------------------------------------------------------------------------------

 Macros:                                                                      */

#define xMin(widget)          ((widget)->waveform.x.minval)
#define xMax(widget)          ((widget)->waveform.x.maxval)
#define xValue(widget)        ((widget)->waveform.x.value)
#define xValPtr(widget)       ((widget)->waveform.x.val_ptr)
#define xPixValue(widget,idx) XPix(widget, idx)
#define xResolution(widget)   ((widget)->waveform.x.pointer_resolution)
#define xLowLimit(widget)     ((widget)->wavedraw.x.low)
#define xHighLimit(widget)    ((widget)->wavedraw.x.high)
#define xMinDistance(widget)  ((widget)->wavedraw.x.min_distance)
#define xIncreasing(widget)   ((widget)->wavedraw.x.increasing)
#define xNumChoices(widget)   ((widget)->wavedraw.x.num_choices)
#define xChoices(widget)      ((widget)->wavedraw.x.choices)
#define yMin(widget)          ((widget)->waveform.y.minval)
#define yMax(widget)          ((widget)->waveform.y.maxval)
#define yValue(widget)        ((widget)->waveform.y.value)
#define yValPtr(widget)       ((widget)->waveform.y.val_ptr)
#define yPixValue(widget,idx) YPix(widget, idx)
#define yResolution(widget)   ((widget)->waveform.y.pointer_resolution)
#define yLowLimit(widget)     ((widget)->wavedraw.y.low)
#define yHighLimit(widget)    ((widget)->wavedraw.y.high)
#define yMinDistance(widget)  ((widget)->wavedraw.y.min_distance)
#define yIncreasing(widget)   ((widget)->wavedraw.y.increasing)
#define yNumChoices(widget)   ((widget)->wavedraw.y.num_choices)
#define yChoices(widget)      ((widget)->wavedraw.y.choices)
#define xGridSnap(widget)     ((widget)->wavedraw.x.grid_snap)
#define yGridSnap(widget)     ((widget)->wavedraw.y.grid_snap)

#define waveformPointerMode(widget)        ((widget)->waveform.pointer_mode)
#define waveformCount(widget)              ((widget)->waveform.count)
#define waveformSelections(widget)         ((widget)->waveform.selections)
#define waveformSelectionsValPtr(widget)   ((widget)->waveform.selections_val_ptr)
#define waveformPenDown(widget)            ((widget)->waveform.pen_down)
#define waveformPenDownValPtr(widget)      ((widget)->waveform.pen_down_val_ptr)
#define waveformPanning(widget)            ((widget)->waveform.panning)
#define waveformRedraw(widget)          ((widget)->waveform.redraw)
#define waveformStepPlot(widget)           ((widget)->waveform.step_plot)
#define waveformDeleteOnlySelected(widget) ((widget)->wavedraw.delete_only_selected)
#define wavedrawMoveCallbacks(widget)         ((widget)->wavedraw.move_callback)
#define wavedrawSelectCallbacks(widget)       ((widget)->wavedraw.select_callback)
#define wavedrawDeselectCallbacks(widget)     ((widget)->wavedraw.deselect_callback)
#define wavedrawAddPointCallbacks(widget)     ((widget)->wavedraw.add_point_callback)
#define wavedrawDeletePointCallbacks(widget)  ((widget)->wavedraw.delete_point_callback)
#define wavedrawFitCallbacks(widget)          ((widget)->wavedraw.fit_callback)
#define wavedrawSelectionIdx(widget)          ((widget)->wavedraw.selection_idx)
#define wavedrawNumSelections(widget)         ((widget)->wavedraw.num_selections)
#define wavedrawMinDistance(widget)           ((widget)->wavedraw.min_distance)

/*------------------------------------------------------------------------------

 Global variables:                                                            */

#include <stdio.h>
#include <Xmds/XmdsWavedrawP.h>
#include <X11/cursorfont.h>
#include <math.h>
#include <libroutines.h>

/*------------------------------------------------------------------------------

 Local variables:                                                             */


static void Refresh(XmdsWavedrawWidget w);
static void SetPointerMode();
static void ClassPartInitialize();
static void Initialize();
static Boolean SetValues();
static void Destroy();
static void DeletePoint(XmdsWavedrawWidget w, int idx, Boolean callcallbacks);
static Boolean AddPoint(XmdsWavedrawWidget w, int idx, float *newx, float *newy,
			Boolean callcallbacks, enum XmdsWaveformMotionRestriction motion);
void XmdsWavedrawDeletePoint(Widget w, int idx, Boolean callcallbacks);
Boolean XmdsWavedrawAddPoint(Widget w, int idx, float *newx, float *newy, Boolean callcallbacks,
			     enum XmdsWaveformMotionRestriction motion);
static int InsertionPoint(XmdsWavedrawWidget w, XButtonEvent * event, int selection);
static unsigned int Distance(int xdistance, int ydistance);
static int SelectedPoint(XmdsWavedrawWidget w, XButtonEvent * event, Boolean anypoint);
static void AddDelete();
static void SetCursor(XmdsWavedrawWidget w);
static void SetTranslations(XmdsWavedrawWidget w);
static int SelectedSegment(XmdsWavedrawWidget w, XButtonEvent * event);
static double SegDistance(double ax, double ay, double bx, double by, double cx, double cy);
static void Move();
static void MoveBegin();
static void MoveEnd();
static int XPix(XmdsWavedrawWidget w, int idx);
static int YPix(XmdsWavedrawWidget w, int idx);
static void SetPen();
static void SelectDeselect();
static double Round(double value, double resolution);
static Boolean MovePoint(XmdsWavedrawWidget w, int idx, float *desired_x, float *desired_y,
			 float *new_x, float *new_y, Boolean callcallbacks, XEvent * event);

static XtActionsRec actionlist[] = { {"WavedrawMove", Move},
{"WavedrawMoveBegin", MoveBegin},
{"WavedrawMoveEnd", MoveEnd},
{"WavedrawSetPen", SetPen},
{"WavedrawSelectDeselect", SelectDeselect},
{"WavedrawAddDelete", AddDelete}
};

static XtResource resources[] = {
  {XmdsNmoveCallback, "MoveCallback", XmRCallback, sizeof(XtCallbackList),
   XtOffset(XmdsWavedrawWidget, wavedraw.move_callback),
   XmRImmediate, 0}
  ,
  {XmdsNselectCallback, "SelectCallback", XmRCallback, sizeof(XtCallbackList),
   XtOffset(XmdsWavedrawWidget, wavedraw.select_callback),
   XmRImmediate, 0}
  ,
  {XmdsNdeselectCallback, "DeselectCallback", XmRCallback, sizeof(XtCallbackList),
   XtOffset(XmdsWavedrawWidget, wavedraw.deselect_callback), XmRImmediate, 0}
  ,
  {XmdsNaddPointCallback, "AddPointCallback", XmRCallback, sizeof(XtCallbackList),
   XtOffset(XmdsWavedrawWidget, wavedraw.add_point_callback), XmRImmediate, 0}
  ,
  {XmdsNdeletePointCallback, "DeletePointCallback", XmRCallback, sizeof(XtCallbackList),
   XtOffset(XmdsWavedrawWidget, wavedraw.delete_point_callback), XmRImmediate, 0}
  ,
  {XmdsNfitCallback, "FitCallback", XmRCallback, sizeof(XtCallbackList),
   XtOffset(XmdsWavedrawWidget, wavedraw.fit_callback), XmRImmediate, 0}
  ,
/*
  {XmdsNminDistance,"MinDistance",XmRPointer, sizeof(XtPointer), XtOffset(XmdsWavedrawWidget,wavedraw.min_distance),
	    XmRImmediate,0},
*/
  {XmdsNdeleteOnlySelected, "DeleteOnlySelected", XmRBoolean, sizeof(Boolean),
   XtOffset(XmdsWavedrawWidget, wavedraw.delete_only_selected), XmRImmediate, 0}
  ,
  {XmdsNlowX, "LowX", XmRPointer, sizeof(XtPointer), XtOffset(XmdsWavedrawWidget, wavedraw.x.low),
   XmRImmediate, 0}
  ,
  {XmdsNhighX, "HighX", XmRPointer, sizeof(XtPointer),
   XtOffset(XmdsWavedrawWidget, wavedraw.x.high), XmRImmediate, 0}
  ,
  {XmdsNlowY, "LowY", XmRPointer, sizeof(XtPointer), XtOffset(XmdsWavedrawWidget, wavedraw.y.low),
   XmRImmediate, 0}
  ,
  {XmdsNhighY, "HighY", XmRPointer, sizeof(XtPointer),
   XtOffset(XmdsWavedrawWidget, wavedraw.y.high), XmRImmediate, 0}
  ,
  {XmdsNxMinDistance, "XMinDistance", XmRPointer, sizeof(XtPointer),
   XtOffset(XmdsWavedrawWidget, wavedraw.x.min_distance),
   XmRImmediate, 0}
  ,
  {XmdsNyMinDistance, "YMinDistance", XmRPointer, sizeof(XtPointer),
   XtOffset(XmdsWavedrawWidget, wavedraw.y.min_distance),
   XmRImmediate, 0}
  ,
  {XmdsNxIncreasing, "XIncreasing", XmRBoolean, sizeof(Boolean),
   XtOffset(XmdsWavedrawWidget, wavedraw.x.increasing),
   XmRImmediate, 0}
  ,
  {XmdsNyIncreasing, "YIncreasing", XmRBoolean, sizeof(Boolean),
   XtOffset(XmdsWavedrawWidget, wavedraw.y.increasing),
   XmRImmediate, 0}
  ,
  {XmdsNxNumChoices, "XNumChoices", XmRInt, sizeof(int),
   XtOffset(XmdsWavedrawWidget, wavedraw.x.num_choices), XmRImmediate, 0},
  {XmdsNyNumChoices, "YNumChoices", XmRInt, sizeof(int),
   XtOffset(XmdsWavedrawWidget, wavedraw.y.num_choices), XmRImmediate, 0},
  {XmdsNxChoices, "XChoices", XmRPointer, sizeof(XtPointer),
   XtOffset(XmdsWavedrawWidget, wavedraw.x.choices), XmRImmediate, 0}
  ,
  {XmdsNyChoices, "YChoices", XmRPointer, sizeof(XtPointer),
   XtOffset(XmdsWavedrawWidget, wavedraw.y.choices), XmRImmediate, 0}
  ,
  {XmdsNxGridSnap, "XGridSnap", XmRPointer, sizeof(XtPointer),
   XtOffset(XmdsWavedrawWidget, wavedraw.x.grid_snap), XmRImmediate, 0}
  ,
  {XmdsNyGridSnap, "YGridSnap", XmRPointer, sizeof(XtPointer),
   XtOffset(XmdsWavedrawWidget, wavedraw.y.grid_snap), XmRImmediate, 0}
  ,
};

XmdsWavedrawClassRec xmdsWavedrawClassRec = {
  {
   /* superclass */ (WidgetClass) & xmdsWaveformClassRec,
   /* class_name */ "XmdsWavedrawWidget",
   /* widget size */ sizeof(XmdsWavedrawRec),
   /* class_initialize */ NULL,
   /* class_part_initialize */ ClassPartInitialize,
   /* class_inited */ 0,
   /* initialize   */ Initialize,
   /* initialize_hook */ 0,
   /* realize */ XtInheritRealize,
   /* actions */ actionlist,
   /* num_actions */ XtNumber(actionlist),
   /* resources */ resources,
   /* num_resources */ XtNumber(resources),
   /* xrm_class     */ 0,
   /* compress_motion */ 1,
   /* compress_exposure */ XtExposeCompressMaximal,
   /* compress_enterleave */ 1,
   /* visible_interest */ 0,
   /* destroy */ Destroy,
   /* Resize */ XtInheritResize,
   /* expose */ XtInheritExpose,
   /* set_values */ SetValues,
   /* set_Values_hook */ NULL,
   /* set_values_almost */ XtInheritSetValuesAlmost,
   /* get_values_hook */ 0,
   /* accept_focus */ XtInheritAcceptFocus,
   /* version type */ XtVersionDontCheck,
   /* callback_offsets */ 0,
   /* translations */ "<Btn3Down>:WaveformButton3()\n @Help <Btn1Down>:WaveformHelp()",
   /* geometry handler */ XtInheritQueryGeometry,
   /* accelerators */ XtInheritDisplayAccelerator,
   /* extension */ 0
   }
  ,
  {				/* Primitive class part */
   /* border_highlight   */ XmInheritBorderHighlight,
   /* border_unhighlight */ XmInheritBorderUnhighlight,
   /* translations */ NULL,
   /* arm_and_activate */ XmInheritArmAndActivate,
   /* syn_resources */ NULL,
   /* num_syn_resources */ 0,
   /* extension */ NULL
   }
  ,
  {
   /* waveform class          */
   /* default translations    */ XmdsInheritTranslations,
   /* zoom translations       */ XmdsInheritTranslations,
   /* drag translations       */ XmdsInheritTranslations,
   /* point translations      */ XmdsInheritTranslations,
   /* edit translations       */ XmdsInheritTranslations,
   /* update proc             */ XmdsInheritUpdate,
   /* set crosshairs proc     */ XmdsInheritSetCrosshairs,
   /* set pointer mode        */ SetPointerMode,
   /* print proc              */ XmdsInheritPrint,
   /* reverse proc            */ XmdsInheritReverse,
   /* set wave proc           */ XmdsInheritSetWave,
   /* extension               */ NULL
   }
  ,
  {
   /* wavedraw class          */
   /* draw1 translations      */ XmdsInheritTranslations,
   /* draw2 translations      */ XmdsInheritTranslations,
   /* setpen translations     */ XmdsInheritTranslations,
   /* add point proc          */ AddPoint,
   /* delete point proc       */ DeletePoint,
   /* move point proc         */ MovePoint,
   /* extension               */ NULL
   }
};

EXPORT WidgetClass xmdsWavedrawWidgetClass;

/*------------------------------------------------------------------------------

 Executable:                                                                  */

EXPORT Widget XmdsCreateWavedraw(Widget parent, char *name, ArgList args, Cardinal argcount)
{
  return XtCreateWidget(name, xmdsWavedrawWidgetClass, parent, args, argcount);
}

static void ClassPartInitialize(XmdsWavedrawWidgetClass class)
{

#define initTrans(field,deftrans)\
  if (field == XmdsInheritTranslations)\
    field = XtParseTranslationTable(deftrans);\
  else\
    field = XtParseTranslationTable((const char *)field)

  initTrans(xmdsWavedrawClassRec.wavedraw_class.draw1_trans, "<Btn1Down> : WavedrawMoveBegin()\n \
<Btn1Motion> : WavedrawMove()\n <Btn1Up> : WavedrawMoveEnd()\n <Btn3Down>:WaveformButton3()\n @Help <Btn1Down>:WaveformHelp()\n \
<Btn2Down> : WavedrawSelectDeselect()");
  initTrans(xmdsWavedrawClassRec.wavedraw_class.draw2_trans, "<Btn1Down> : WavedrawMoveBegin()\n \
<Btn1Motion> : WavedrawMove()\n <Btn1Up> : WavedrawMoveEnd()\n <Btn3Down>:WaveformButton3()\n @Help <Btn1Down>:WaveformHelp()\n \
<Btn2Down> : WavedrawAddDelete()");
  initTrans(xmdsWavedrawClassRec.wavedraw_class.setpen_trans, "<Btn1Down> : WavedrawSetPen()\n \
<Btn3Down>:WaveformButton3()\n @Help <Btn1Down>:WaveformHelp()");
}

static void Initialize(XmdsWavedrawWidget req, XmdsWavedrawWidget w)
{

#define NewInstance(field,size) if (field(req)) field(w) = (void *)memcpy(XtMalloc(size),field(req),size)

  NewInstance(xLowLimit, sizeof(float));
  NewInstance(xHighLimit, sizeof(float));
  NewInstance(xMinDistance, sizeof(float));
  NewInstance(yLowLimit, sizeof(float));
  NewInstance(yHighLimit, sizeof(float));
  NewInstance(yMinDistance, sizeof(float));
  NewInstance(xGridSnap, sizeof(float));
  NewInstance(yGridSnap, sizeof(float));
  /*
     NewInstance(wavedrawMinDistance, sizeof(float));
   */
  NewInstance(xChoices, xNumChoices(req) * sizeof(float));
  NewInstance(yChoices, yNumChoices(req) * sizeof(float));
  SetTranslations(w);
  return;
}

#undef NewInstance

static void Destroy(XmdsWavedrawWidget w)
{

#define FreeInstance(field) if (field(w)) XtFree((char *)field(w))

  FreeInstance(xLowLimit);
  FreeInstance(xHighLimit);
  FreeInstance(xMinDistance);
  FreeInstance(yLowLimit);
  FreeInstance(yHighLimit);
  FreeInstance(yMinDistance);
  FreeInstance(xChoices);
  FreeInstance(yChoices);
  FreeInstance(xGridSnap);
  FreeInstance(yGridSnap);
  /*
     FreeInstance(wavedrawMinDistance);
   */
}

static void MoveBegin(XmdsWavedrawWidget w, XButtonEvent * event)
{
  int selection = SelectedPoint(w, event, 0);
  if (selection >= 0) {
    XmdsWavedrawValueCBStruct cb = { XmdsCRMovePointBegin, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    cb.event = (XEvent *) event;
    cb.idx = selection;
    cb.oldx = xValue(w)[selection];
    cb.oldy = yValue(w)[selection];
    cb.newx = xValue(w)[selection];
    cb.newy = yValue(w)[selection];
    cb.count = waveformCount(w);
    cb.x = xValue(w);
    cb.y = yValue(w);
    cb.selected = waveformSelections(w);
    XtCallCallbacks((Widget) w, XmdsNmoveCallback, &cb);
    wavedrawSelectionIdx(w) = cb.idx;
    XWarpPointer(XtDisplay(w), XtWindow(w), XtWindow(w), event->x, event->y, XtWidth(w),
		 XtHeight(w), xPixValue(w, selection), yPixValue(w, selection));
  } else
    wavedrawSelectionIdx(w) = -1;
  waveformPanning(w) = -1;
}

static void SetPen(XmdsWavedrawWidget w, XButtonEvent * event)
{
  int selection = SelectedSegment(w, event);
  XmdsWavedrawFitCBStruct fitcb = { XmdsCRSetPen, 0, 0, 0, 0, 0, 0 };;
  fitcb.event = (XEvent *) event;
  fitcb.count = waveformCount(w);
  fitcb.x = xValue(w);
  fitcb.y = yValue(w);
  fitcb.selected = waveformSelections(w);
  fitcb.pen_down = waveformPenDown(w);
  if (selection >= 0)
    waveformPenDown(w)[selection] = !waveformPenDown(w)[selection];
  XtCallCallbacks((Widget) w, XmdsNfitCallback, &fitcb);
  Refresh(w);
}

static void Refresh(XmdsWavedrawWidget w)
{
  waveformRedraw(w) = 1;
  (((XmdsWavedrawWidgetClass) w->core.widget_class)->core_class.expose) ((Widget) w, 0, 0);
}

EXPORT Boolean XmdsWavedrawMovePoint(XmdsWavedrawWidget w, int idx, float *desired_x, float *desired_y,
			      float *new_x, float *new_y, Boolean callcallbacks, XEvent * event)
{
  return (((XmdsWavedrawWidgetClass) w->core.widget_class)->wavedraw_class.move_point_proc) (w, idx,
											     desired_x,
											     desired_y,
											     new_x,
											     new_y,
											     callcallbacks,
											     event);
}

static Boolean MovePoint(XmdsWavedrawWidget w, int idx, float *desired_x, float *desired_y,
			 float *new_x, float *new_y, Boolean callcallbacks, XEvent * event)
{
  Boolean stat = 0;
  int range = idx ? (idx >= (waveformCount(w) - 1) ? 3 : 2) : 1;
  if ((idx < 0) || (idx > (waveformCount(w) - 1)))
    return 0;
  if (xGridSnap(w) && (*xGridSnap(w) > 0.0))
    *desired_x = Round(*desired_x, *xGridSnap(w));
  if (yGridSnap(w) && (*yGridSnap(w) > 0.0))
    *desired_y = Round(*desired_y, *yGridSnap(w));
  if (xNumChoices(w)) {
    float min_dif = 1E38;
    int i;
    int closest = -1;
    for (i = 0; i < xNumChoices(w); i++) {
      if ((xLowLimit(w) && (xChoices(w)[i] >= *xLowLimit(w))) || (!xLowLimit(w))) {
	float dif = *desired_x - xChoices(w)[i];
	dif = dif < 0.0 ? -dif : dif;
	if (xIncreasing(w)) {
	  float mindist = xMinDistance(w) ? *xMinDistance(w) : xResolution(w);
	  switch (range) {
	  case 1:
	    if (xChoices(w)[i] > (xValue(w)[idx + 1] - mindist))
	      dif = 1E38;
	    break;
	  case 2:
	    if ((xChoices(w)[i] > (xValue(w)[idx + 1] - mindist)) ||
		(xChoices(w)[i] < (xValue(w)[idx - 1] + mindist)))
	      dif = 1E38;
	    break;
	  case 3:
	    if (xChoices(w)[i] > (xValue(w)[idx - 1] + mindist))
	      dif = 1E38;
	    break;
	  }
	}
	if (dif < min_dif) {
	  min_dif = dif;
	  closest = i;
	}
      }
    }
    if (closest >= 0) {
      *new_x = xChoices(w)[closest];
      stat = 1;
    }
  } else if (xIncreasing(w)) {
    float mindist = xMinDistance(w) ? *xMinDistance(w) : xResolution(w);
    *new_x = *desired_x;
    if (xLowLimit(w) && (*desired_x < *xLowLimit(w)))
      *new_x = *xLowLimit(w);
    if (xHighLimit(w) && (*desired_x > *xHighLimit(w)))
      *new_x = *xHighLimit(w);
    switch (range) {
    case 1:
      if (*desired_x > (xValue(w)[idx + 1] - mindist)) {
	if ((xLowLimit(w) && (*xLowLimit(w) <= (xValue(w)[idx + 1] - mindist))) || !xLowLimit(w)) {
	  *new_x = xValue(w)[idx + 1] - mindist;
	  stat = 1;
	}
      } else {
	*new_x = *desired_x;
	stat = 1;
      }
      break;
    case 2:
      if ((xValue(w)[idx + 1] - xValue(w)[idx - 1]) >= (1.99 * mindist)) {
	stat = 1;
	if (*desired_x < (xValue(w)[idx - 1] + mindist))
	  *new_x = xValue(w)[idx - 1] + mindist;
	else if (*desired_x > (xValue(w)[idx + 1] - mindist))
	  *new_x = xValue(w)[idx + 1] - mindist;
	else
	  *new_x = *desired_x;
      }
      break;
    case 3:
      if (*desired_x < (xValue(w)[idx - 1] + mindist)) {
	if ((xHighLimit(w) && (*xHighLimit(w) >= (xValue(w)[idx - 1] + mindist))) || !xHighLimit(w)) {
	  *new_x = xValue(w)[idx - 1] + mindist;
	  stat = 1;
	}
      } else {
	*new_x = *desired_x;
	stat = 1;
      }
      break;
    }
  } else {
    if (xLowLimit(w) && (*desired_x < *xLowLimit(w)))
      *new_x = *xLowLimit(w);
    else if (xHighLimit(w) && (*desired_x > *xHighLimit(w)))
      *new_x = *xHighLimit(w);
    else
      *new_x = *desired_x;
    stat = 1;
  }
  if (!stat)
    return stat;
  stat = 0;
  /**********************************************/
  if (yNumChoices(w)) {
    float min_dif = 1E38;
    int i;
    int closest = -1;
    for (i = 0; i < yNumChoices(w); i++) {
      if ((yLowLimit(w) && (yChoices(w)[i] >= *yLowLimit(w))) || (!yLowLimit(w))) {
	float dif = *desired_y - yChoices(w)[i];
	dif = dif < 0.0 ? -dif : dif;
	if (yIncreasing(w)) {
	  float mindist = yMinDistance(w) ? *yMinDistance(w) : yResolution(w);
	  switch (range) {
	  case 1:
	    if (yChoices(w)[i] > (yValue(w)[idx + 1] - mindist))
	      dif = 1E38;
	    break;
	  case 2:
	    if ((yChoices(w)[i] > (yValue(w)[idx + 1] - mindist)) ||
		(yChoices(w)[i] < (yValue(w)[idx - 1] + mindist)))
	      dif = 1E38;
	    break;
	  case 3:
	    if (yChoices(w)[i] > (yValue(w)[idx - 1] + mindist))
	      dif = 1E38;
	    break;
	  }
	}
	if (dif < min_dif) {
	  min_dif = dif;
	  closest = i;
	}
      }
    }
    if (closest >= 0) {
      *new_y = yChoices(w)[closest];
      stat = 1;
    }
  } else if (yIncreasing(w)) {
    float mindist = yMinDistance(w) ? *yMinDistance(w) : yResolution(w);
    *new_y = *desired_y;
    if (yLowLimit(w) && (*desired_y < *yLowLimit(w)))
      *new_y = *yLowLimit(w);
    if (yHighLimit(w) && (*desired_y > *yHighLimit(w)))
      *new_y = *yHighLimit(w);
    switch (range) {
    case 1:
      if (*desired_y > (yValue(w)[idx + 1] - mindist)) {
	if ((yLowLimit(w) && (*yLowLimit(w) <= (yValue(w)[idx + 1] - mindist))) || !yLowLimit(w)) {
	  *new_y = yValue(w)[idx + 1] - mindist;
	  stat = 1;
	}
      } else {
	*new_y = *desired_y;
	stat = 1;
      }
      break;
    case 2:
      if ((yValue(w)[idx + 1] - yValue(w)[idx - 1]) >= (2 * mindist)) {
	stat = 1;
	if (*desired_y < (yValue(w)[idx - 1] + mindist))
	  *new_y = yValue(w)[idx - 1] + mindist;
	else if (*desired_y > (yValue(w)[idx + 1] - mindist))
	  *new_y = yValue(w)[idx + 1] - mindist;
	else
	  *new_y = *desired_y;
      }
      break;
    case 3:
      if (*desired_y < (yValue(w)[idx - 1] + mindist)) {
	if ((yHighLimit(w) && (*yHighLimit(w) >= (yValue(w)[idx - 1] + mindist))) || !yHighLimit(w)) {
	  *new_y = yValue(w)[idx - 1] + mindist;
	  stat = 1;
	}
      } else {
	*new_y = *desired_y;
	stat = 1;
      }
      break;
    }
  } else {
    if (yLowLimit(w) && (*desired_y < *yLowLimit(w)))
      *new_y = *yLowLimit(w);
    else if (yHighLimit(w) && (*desired_y > *yHighLimit(w)))
      *new_y = *yHighLimit(w);
    else
      *new_y = *desired_y;
    stat = 1;
  }
  if (!stat)
    return 0;
  if (waveformSelections(w)) {
    switch (waveformSelections(w)[idx]) {
    case XmdsMOTION_XONLY:
      *new_y = yValue(w)[idx];
      break;
    case XmdsMOTION_YONLY:
      *new_x = xValue(w)[idx];
      break;
    case XmdsMOTION_NONE:
      *new_x = xValue(w)[idx];
      *new_y = yValue(w)[idx];
      break;
    }
  }
/*
  if (xGridSnap(w) && (*xGridSnap(w) > 0.0))
    *new_x = Round(*new_x,*xGridSnap(w));
  if (yGridSnap(w) && (*yGridSnap(w) > 0.0))
    *new_y = Round(*new_y,*yGridSnap(w));
*/
  if (callcallbacks) {
    XmdsWavedrawValueCBStruct cb = { XmdsCRMovePoint, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    XmdsWavedrawFitCBStruct fitcb = { XmdsCRMovePoint, 0, 0, 0, 0, 0, 0 };;
    cb.event = (XEvent *) event;
    cb.idx = idx;
    cb.oldx = xValue(w)[idx];
    cb.oldy = yValue(w)[idx];
    cb.newx = xValue(w)[idx] = *new_x;
    cb.newy = yValue(w)[idx] = *new_y;
    cb.count = waveformCount(w);
    cb.x = xValue(w);
    cb.y = yValue(w);
    cb.selected = waveformSelections(w);
    fitcb.event = (XEvent *) event;
    fitcb.count = waveformCount(w);
    fitcb.x = xValue(w);
    fitcb.y = yValue(w);
    fitcb.selected = waveformSelections(w);
    fitcb.pen_down = waveformPenDown(w);
    XtCallCallbacks((Widget) w, XmdsNmoveCallback, &cb);
    wavedrawSelectionIdx(w) = cb.idx;
    XtCallCallbacks((Widget) w, XmdsNfitCallback, &fitcb);
    Refresh(w);
  } else {
    xValue(w)[idx] = *new_x;
    yValue(w)[idx] = *new_y;
  }
  return stat;
}

static void SlideStretch(XmdsWavedrawWidget w, int idx, float *desired_x, float *desired_y,
			 float *new_x, float *new_y, Boolean callcallbacks, XEvent * event,
			 Boolean stretch)
{
  int i;
  float xoff = *desired_x - xValue(w)[idx];
  float yoff = *desired_y - yValue(w)[idx];
  float xratio = xValue(w)[idx] && *desired_x ? *desired_x / xValue(w)[idx] : 1;
  float yratio = yValue(w)[idx] && *desired_y ? *desired_y / yValue(w)[idx] : 1;
  float *oldx =
      (float *)memcpy(XtMalloc(waveformCount(w) * sizeof(float)), xValue(w),
		      waveformCount(w) * sizeof(float));
  float *oldy =
      (float *)memcpy(XtMalloc(waveformCount(w) * sizeof(float)), yValue(w),
		      waveformCount(w) * sizeof(float));
  Boolean ok = 1;
  for (i = 0; i < waveformCount(w); i++) {
    xValue(w)[i] = stretch ? xValue(w)[i] * xratio : xValue(w)[i] + xoff;
    yValue(w)[i] = stretch ? yValue(w)[i] * yratio : yValue(w)[i] + yoff;
  }
  for (i = 0; i < waveformCount(w); i++)
    ok = ok & MovePoint(w, i, &xValue(w)[i], &yValue(w)[i], &xValue(w)[i], &yValue(w)[i], 0, 0);
  if (ok) {
    if (callcallbacks) {
      XmdsWavedrawValueCBStruct cb = { XmdsCRMovePoint, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
      XmdsWavedrawFitCBStruct fitcb = { XmdsCRMovePoint, 0, 0, 0, 0, 0, 0 };
      cb.event = (XEvent *) event;
      cb.idx = idx;
      cb.oldx = xValue(w)[idx];
      cb.oldy = yValue(w)[idx];
      cb.newx = xValue(w)[idx];
      cb.newy = yValue(w)[idx];
      cb.count = waveformCount(w);
      cb.x = xValue(w);
      cb.y = yValue(w);
      cb.selected = waveformSelections(w);
      fitcb.event = (XEvent *) event;
      fitcb.count = waveformCount(w);
      fitcb.x = xValue(w);
      fitcb.y = yValue(w);
      fitcb.selected = waveformSelections(w);
      fitcb.pen_down = waveformPenDown(w);

      XtCallCallbacks((Widget) w, XmdsNmoveCallback, &cb);
      wavedrawSelectionIdx(w) = cb.idx;
      XtCallCallbacks((Widget) w, XmdsNfitCallback, &fitcb);
      Refresh(w);
    }
  } else {
    memcpy(xValue(w), oldx, waveformCount(w) * sizeof(float));
    memcpy(yValue(w), oldy, waveformCount(w) * sizeof(float));
  }
  XtFree((char *)oldx);
  XtFree((char *)oldy);
}

static void Move(XmdsWavedrawWidget w, XButtonEvent * event)
{
  int idx = wavedrawSelectionIdx(w);
  Boolean warp = FALSE;
  float x;
  float y;
  while (XCheckMaskEvent(XtDisplay(w), Button1MotionMask, (XEvent *) event)) ;
  if (idx < 0)
    return;
  if (event->state & LockMask) {
    x = xValue(w)[idx] +
	((event->x ==
	  xPixValue(w,
		    idx)) ? 0 : ((event->state & ShiftMask) ? 0 : (10 *
								   ((event->x >
								     xPixValue(w,
									       idx)) ?
								    xResolution(w) : -1 *
								    xResolution(w)))));
    y = yValue(w)[idx] +
	((event->y ==
	  yPixValue(w,
		    idx)) ? 0 : ((event->state & Mod1Mask) ? 0 : (10 *
								  ((event->y <
								    yPixValue(w,
									      idx)) ? yResolution(w)
								   : -1 * yResolution(w)))));
    warp = TRUE;
  } else {
    x = (event->state & ShiftMask) ? xValue(w)[idx] :
	Round(event->x * (*xMax(w) - *xMin(w)) / XtWidth(w) + *xMin(w), xResolution(w));
    y = (event->state & Mod1Mask) ? yValue(w)[idx] :
	Round(((float)XtHeight(w) - event->y) * (*yMax(w) - *yMin(w)) / XtHeight(w) + *yMin(w),
	      yResolution(w));
  }
  if (waveformPointerMode(w) == XmdsPOINTER_MODE_SLIDE_STRETCH)
    SlideStretch(w, idx, &x, &y, &x, &y, 1, (XEvent *) event, event->state & ControlMask);
  else
    MovePoint(w, idx, &x, &y, &x, &y, 1, (XEvent *) event);
  if (warp)
    XWarpPointer(XtDisplay(w), XtWindow(w), XtWindow(w), event->x, event->y, XtWidth(w),
		 XtHeight(w), xPixValue(w, idx), yPixValue(w, idx));
  Refresh(w);
}

static void MoveEnd(XmdsWavedrawWidget w, XButtonEvent * event)
{
  int selection = wavedrawSelectionIdx(w);
  waveformPanning(w) = 0;
  if (selection >= 0) {
    XmdsWavedrawValueCBStruct cb = { XmdsCRMovePointEnd, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    cb.event = (XEvent *) event;
    cb.idx = selection;
    cb.oldx = xValue(w)[selection];
    cb.oldy = yValue(w)[selection];
    cb.newx = xValue(w)[selection];
    cb.newy = yValue(w)[selection];
    cb.count = waveformCount(w);
    cb.x = xValue(w);
    cb.y = yValue(w);
    cb.selected = waveformSelections(w);
    XtCallCallbacks((Widget) w, XmdsNmoveCallback, &cb);
    wavedrawSelectionIdx(w) = cb.idx;
    Refresh(w);
  }
}

static void SelectDeselect(XmdsWavedrawWidget w, XButtonEvent * event)
{
  if (waveformSelections(w)) {
    int selection = SelectedPoint(w, event, !(event->state & ControlMask));
    XmdsWavedrawValueCBStruct cb = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    XmdsWavedrawFitCBStruct fitcb = { 0, 0, 0, 0, 0, 0, 0 };
    cb.event = (XEvent *) event;
    cb.idx = selection;
    cb.oldx = xValue(w)[selection];
    cb.oldy = yValue(w)[selection];
    cb.newx = xValue(w)[selection];
    cb.newy = yValue(w)[selection];
    cb.count = waveformCount(w);
    cb.x = xValue(w);
    cb.y = yValue(w);
    cb.selected = waveformSelections(w);
    fitcb.event = (XEvent *) event;
    fitcb.count = waveformCount(w);
    fitcb.x = xValue(w);
    fitcb.y = yValue(w);
    fitcb.selected = waveformSelections(w);
    fitcb.pen_down = waveformPenDown(w);

    if (selection >= 0) {
      if (event->state & ControlMask) {
	if (waveformSelections(w)[selection]) {
	  waveformSelections(w)[selection] = 0;
	  wavedrawNumSelections(w)--;
	  fitcb.reason = cb.reason = XmdsCRDeselectPoint;
	  XtCallCallbacks((Widget) w, XmdsNdeselectCallback, &cb);
	  XtCallCallbacks((Widget) w, XmdsNfitCallback, &fitcb);
	  Refresh(w);
	}
      } else {
	if (!waveformSelections(w)[selection])
	  wavedrawNumSelections(w)++;
	waveformSelections(w)[selection] =
	    event->state & ShiftMask ? (event->
					state & Mod1Mask ? XmdsMOTION_NONE : XmdsMOTION_YONLY)
	    : (event->state & Mod1Mask ? XmdsMOTION_XONLY : XmdsMOTION_BOTH);
	fitcb.reason = cb.reason = XmdsCRSelectPoint;
	XtCallCallbacks((Widget) w, XmdsNselectCallback, &cb);
	XtCallCallbacks((Widget) w, XmdsNfitCallback, &fitcb);
	Refresh(w);
      }
    }
  }
}

static void AddDelete(XmdsWavedrawWidget w, XButtonEvent * event)
{
  float newx =
      Round((float)event->x / XtWidth(w) * (*xMax(w) - *xMin(w)) + *xMin(w), xResolution(w));
  float newy =
      Round(((float)(XtHeight(w) - event->y)) / XtHeight(w) * (*yMax(w) - *yMin(w)) + *yMin(w),
	    yResolution(w));
  if (event->state & ShiftMask) {
    if (waveformCount(w) > 2) {
      int selection = SelectedPoint(w, event, !waveformDeleteOnlySelected(w));
      if (selection >= 0)
	XmdsWavedrawDeletePoint((Widget) w, selection, 1);
    }
  } else {
    int selection;
    if (waveformStepPlot(w)) {
      for (selection = 0; selection < waveformCount(w); selection++)
	if (xValue(w)[selection] > newx)
	  break;
    } else
      selection = InsertionPoint(w, event, SelectedPoint(w, event, 1));
    if (!XmdsWavedrawAddPoint((Widget) w, selection, &newx, &newy, 1, XmdsMOTION_BOTH))
      XBell(XtDisplay(w), 0);
  }
}

static int InsertionPoint(XmdsWavedrawWidget w, XButtonEvent * event, int selection)
{
  int idx = selection;
  if (selection) {
    if (selection < (waveformCount(w) - 1)) {
      if ((Distance(event->x - xPixValue(w, selection + 1),
		    waveformStepPlot(w) ? 0 : event->y - yPixValue(w, selection + 1)) +
	   Distance(xPixValue(w, selection - 1) - xPixValue(w, selection),
		    waveformStepPlot(w) ? 0 : yPixValue(w, selection - 1) - yPixValue(w,
										      selection))) <
	  (Distance
	   (event->x - xPixValue(w, selection - 1),
	    waveformStepPlot(w) ? 0 : event->y - yPixValue(w,
							   selection - 1)) + Distance(xPixValue(w,
												selection
												+
												1) -
										      xPixValue(w,
												selection),
										      waveformStepPlot
										      (w) ? 0 :
										      yPixValue(w,
												selection
												+
												1) -
										      yPixValue(w,
												selection))))
	idx++;
    } else {
      if (Distance(event->x - xPixValue(w, selection - 1),
		   waveformStepPlot(w) ? 0 : event->y - yPixValue(w, selection - 1)) >
	  Distance(xPixValue(w, selection) - xPixValue(w, selection - 1),
		   waveformStepPlot(w) ? 0 : yPixValue(w, selection) - yPixValue(w, selection - 1)))
	idx++;
    }
  } else {
    if (Distance(event->x - xPixValue(w, selection + 1),
		 waveformStepPlot(w) ? 0 : event->y - yPixValue(w, selection + 1)) <
	Distance(xPixValue(w, selection) - xPixValue(w, selection + 1),
		 waveformStepPlot(w) ? 0 : yPixValue(w, selection) - yPixValue(w, selection + 1)))
      idx++;
  }
  return idx;
}

static int SelectedSegment(XmdsWavedrawWidget w, XButtonEvent * event)
{
  int i;
  int selection = -1;
  double min_distance = 2000000000;
  int count = waveformCount(w) - 1;
  for (i = 0; i < count; i++) {
    double distance =
	SegDistance(xPixValue(w, i), yPixValue(w, i), xPixValue(w, i + 1), yPixValue(w, i + 1),
		    event->x, event->y);
    if (distance < min_distance) {
      min_distance = distance;
      selection = i;
    }
  }
  return selection;
}

static double SegDistance(double ax, double ay, double bx, double by, double cx, double cy)
{
  double distance;
  double theta = (ax == bx) ? 3.14159 / 2 : atan((by - ay) / (bx - ax));
  double sin_theta = sin(theta);
  double cos_theta = cos(theta);
  double t_cx = (cx - ax) * cos_theta + (cy - ay) * sin_theta;
  double t_bx = (bx - ax) * cos_theta + (by - ay) * sin_theta;
  double t_cy = (cy - ay) * cos_theta - (cx - ax) * sin_theta;
  if ((t_bx < 0) ? t_cx > 0 : t_cx < 0)
    distance = sqrt((ax - cx) * (ax - cx) + (ay - cy) * (ay - cy));
  else {
    if ((t_bx < 0) ? t_cx < t_bx : t_cx > t_bx)
      distance = sqrt((bx - cx) * (bx - cx) + (by - cy) * (by - cy));
    else
      distance = fabs(t_cy);
  }
  return distance;
}

static int SelectedPoint(XmdsWavedrawWidget w, XButtonEvent * event, Boolean anypoint)
{
  int i;
  int selection = -1;
  unsigned int min_distance = 2000000000;
  for (i = 0; i < waveformCount(w); i++) {
    unsigned int distance = Distance(event->x - xPixValue(w, i), event->y - yPixValue(w, i));
    if ((anypoint || !waveformSelections(w) || (waveformSelections(w) && waveformSelections(w)[i]))
	&& (distance < min_distance)) {
      min_distance = distance;
      selection = i;
    }
  }
  return selection;
}

static unsigned int Distance(int xdistance, int ydistance)
{
  return xdistance * xdistance + ydistance * ydistance;
}

static Boolean SetValues(XmdsWavedrawWidget old, XmdsWavedrawWidget req, XmdsWavedrawWidget new)
{
#define NewInstance(field,size) \
  if (field(old) != field(req))\
  {\
    if (field(old))\
      XtFree((char *)field(old));\
    if (field(req))\
      field(new) = (void *)memcpy(XtMalloc(size),field(req),size);\
  }

  NewInstance(xLowLimit, sizeof(float));
  NewInstance(xHighLimit, sizeof(float));
  NewInstance(xMinDistance, sizeof(float));
  NewInstance(yLowLimit, sizeof(float));
  NewInstance(yHighLimit, sizeof(float));
  NewInstance(yMinDistance, sizeof(float));
  NewInstance(xGridSnap, sizeof(float));
  NewInstance(yGridSnap, sizeof(float));
  /*
     NewInstance(wavedrawMinDistance, sizeof(float));
   */
  NewInstance(xChoices, xNumChoices(req) * sizeof(float));
  NewInstance(yChoices, yNumChoices(req) * sizeof(float));
  if (waveformPointerMode(old) != waveformPointerMode(req))
    SetCursor(new);
  if ((waveformCount(old) != waveformCount(new))
      || (waveformPointerMode(old) != waveformPointerMode(new)))
    SetTranslations(new);
  return 0;
}

static void SetCursor(XmdsWavedrawWidget w)
{
  static Boolean first = 1;
  static Cursor draw_cursor;
  if (first) {
    draw_cursor = XCreateFontCursor(XtDisplay(w), XC_dotbox);
    first = 0;
  }

  if (XtWindow(w)) {
    switch (waveformPointerMode(w)) {
    case XmdsPOINTER_MODE_DRAW1:
    case XmdsPOINTER_MODE_DRAW2:
    case XmdsPOINTER_MODE_SET_PEN:
    case XmdsPOINTER_MODE_SLIDE_STRETCH:
      XDefineCursor(XtDisplay(w), XtWindow(w), draw_cursor);
      break;
    }
  }
}

static void SetTranslations(XmdsWavedrawWidget w)
{
  Arg arglist[] = { {XmNtranslations, 0} };
  if (waveformCount(w)) {
    switch (waveformPointerMode(w)) {
    case XmdsPOINTER_MODE_DRAW1:
      arglist[0].value = (long)xmdsWavedrawClassRec.wavedraw_class.draw1_trans;
      XtSetValues((Widget) w, arglist, XtNumber(arglist));
      break;
    case XmdsPOINTER_MODE_SLIDE_STRETCH:
    case XmdsPOINTER_MODE_DRAW2:
      arglist[0].value = (long)xmdsWavedrawClassRec.wavedraw_class.draw2_trans;
      XtSetValues((Widget) w, arglist, XtNumber(arglist));
      break;
    case XmdsPOINTER_MODE_SET_PEN:
      arglist[0].value = (long)xmdsWavedrawClassRec.wavedraw_class.setpen_trans;
      XtSetValues((Widget) w, arglist, XtNumber(arglist));
      break;
    }
  }
}

EXPORT void XmdsWavedrawDeletePoint(Widget w, int idx, Boolean callcallbacks)
{
  XmdsWavedrawWidget wdw = (XmdsWavedrawWidget) w;
  (((XmdsWavedrawWidgetClass) wdw->core.widget_class)->wavedraw_class.delete_point_proc) (wdw, idx,
											  callcallbacks);
}

static void DeletePoint(XmdsWavedrawWidget w, int idx, Boolean callcallbacks)
{
  float oldx = xValue(w)[idx];
  float oldy = yValue(w)[idx];
  memcpy(xValue(w) + idx, xValue(w) + idx + 1, (waveformCount(w) - idx - 1) * sizeof(*xValue(w)));
  memcpy(yValue(w) + idx, yValue(w) + idx + 1, (waveformCount(w) - idx - 1) * sizeof(*yValue(w)));
  if (waveformSelections(w))
    memcpy(waveformSelections(w) + idx, waveformSelections(w) + idx + 1,
	   (waveformCount(w) - idx - 1) * sizeof(*waveformSelections(w)));
  if (waveformPenDown(w))
    memcpy(waveformPenDown(w) + idx, waveformPenDown(w) + idx + 1,
	   (waveformCount(w) - idx - 1) * sizeof(*waveformPenDown(w)));
  waveformCount(w)--;
  if (callcallbacks) {
    XmdsWavedrawValueCBStruct cb = { XmdsCRDeletePoint, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    XmdsWavedrawFitCBStruct fitcb = { XmdsCRDeletePoint, 0, 0, 0, 0, 0, 0 };
    cb.idx = idx;
    cb.oldx = oldx;
    cb.oldy = oldy;
    cb.newx = 0;
    cb.newy = 0;
    cb.count = waveformCount(w);
    cb.x = xValue(w);
    cb.y = yValue(w);
    cb.selected = waveformSelections(w);
    fitcb.count = waveformCount(w);
    fitcb.x = xValue(w);
    fitcb.y = yValue(w);
    fitcb.selected = waveformSelections(w);
    fitcb.pen_down = waveformPenDown(w);

    XtCallCallbacks((Widget) w, XmdsNdeletePointCallback, &cb);
    XtCallCallbacks((Widget) w, XmdsNfitCallback, &fitcb);
  }
  Refresh(w);
}

EXPORT Boolean XmdsWavedrawAddPoint(Widget w, int idx, float *newx, float *newy, Boolean callcallbacks,
			     enum XmdsWaveformMotionRestriction motion)
{
  XmdsWavedrawWidget wdw = (XmdsWavedrawWidget) w;
  return (((XmdsWavedrawWidgetClass) wdw->core.widget_class)->wavedraw_class.add_point_proc)
      (wdw, idx, newx, newy, callcallbacks, motion);
}

static Boolean AddPoint(XmdsWavedrawWidget w, int idx, float *newx, float *newy,
			Boolean callcallbacks, enum XmdsWaveformMotionRestriction motion)
{
  float *oldxvals = xValue(w);
  float *oldyvals = yValue(w);
  Boolean *oldselections = waveformSelections(w);
  Boolean *oldpendown = waveformPenDown(w);
  float x = *newx;
  float y = *newy;
  if ((idx > waveformCount(w)) ||
      (xLowLimit(w) && x < *xLowLimit(w)) ||
      (xHighLimit(w) && x > *xHighLimit(w)) ||
      (yLowLimit(w) && y < *yLowLimit(w)) || (yHighLimit(w) && y > *yHighLimit(w)))
    return 0;
  if (xNumChoices(w)) {
    float min_dif = 1E38;
    int i;
    int closest = 0;
    for (i = 0; i < xNumChoices(w); i++) {
      float dif = x - xChoices(w)[i];
      dif = dif < 0.0 ? -dif : dif;
      if (dif < min_dif) {
	min_dif = dif;
	closest = i;
      }
    }
    x = xChoices(w)[closest];
  }
  if (yNumChoices(w)) {
    float min_dif = 1E38;
    int i;
    int closest = 0;
    for (i = 0; i < yNumChoices(w); i++) {
      float dif = y - yChoices(w)[i];
      dif = dif < 0.0 ? -dif : dif;
      if (dif < min_dif) {
	min_dif = dif;
	closest = i;
      }
    }
    y = yChoices(w)[closest];
  }
  if (xIncreasing(w)) {
    float mindist = xMinDistance(w) ? *xMinDistance(w) : xResolution(w);
    int range = idx ? (idx >= (waveformCount(w) - 1) ? 3 : 2) : 1;
    switch (range) {
    case 1:
      if (x > (xValue(w)[idx] - mindist))
	return 0;
      break;
    case 2:
      if (x < (xValue(w)[idx - 1] + mindist) || x > (xValue(w)[idx] - mindist))
	return 0;
      break;
    case 3:
      if (x < (xValue(w)[idx - 1] + mindist))
	return 0;
      break;
    }
  }
  if (yIncreasing(w)) {
    float mindist = yMinDistance(w) ? *yMinDistance(w) : yResolution(w);
    int range = idx ? (idx >= (waveformCount(w) - 1) ? 3 : 2) : 1;
    switch (range) {
    case 1:
      if (y > (yValue(w)[idx] - mindist))
	return 0;
      break;
    case 2:
      if (y < (yValue(w)[idx - 1] + mindist) || y > (yValue(w)[idx] - mindist))
	return 0;
      break;
    case 3:
      if (y < (yValue(w)[idx - 1] + mindist))
	return 0;
      break;
    }
  }
  xValue(w) = xValPtr(w) = (float *)XtCalloc((waveformCount(w) + 1), sizeof(*xValue(w)));
  yValue(w) = yValPtr(w) = (float *)XtCalloc((waveformCount(w) + 1), sizeof(*yValue(w)));
  if (waveformSelections(w))
    waveformSelections(w) = waveformSelectionsValPtr(w) =
	XtCalloc((waveformCount(w) + 1), sizeof(*waveformSelections(w)));
  if (waveformPenDown(w))
    waveformPenDown(w) = waveformPenDownValPtr(w) =
	XtCalloc((waveformCount(w) + 1), sizeof(*waveformPenDown(w)));
  if (idx) {
    memcpy(xValue(w), oldxvals, idx * sizeof(*xValue(w)));
    memcpy(yValue(w), oldyvals, idx * sizeof(*yValue(w)));
    if (waveformSelections(w))
      memcpy(waveformSelections(w), oldselections, idx * sizeof(*waveformSelections(w)));
    if (waveformPenDown(w))
      memcpy(waveformPenDown(w), oldpendown, idx * sizeof(*waveformPenDown(w)));
  }
  if (idx < waveformCount(w)) {
    memcpy(xValue(w) + idx + 1, oldxvals + idx, (waveformCount(w) - idx) * sizeof(*xValue(w)));
    memcpy(yValue(w) + idx + 1, oldyvals + idx, (waveformCount(w) - idx) * sizeof(*yValue(w)));
    if (waveformSelections(w))
      memcpy(waveformSelections(w) + idx + 1, oldselections + idx,
	     (waveformCount(w) - idx) * sizeof(*waveformSelections(w)));
    if (waveformPenDown(w))
      memcpy(waveformPenDown(w) + idx + 1, oldpendown + idx,
	     (waveformCount(w) - idx) * sizeof(*waveformPenDown(w)));
  }
  MovePoint(w, idx, &x, &y, &x, &y, 0, 0);
  xValue(w)[idx] = x;
  yValue(w)[idx] = y;
  if (waveformSelections(w))
    waveformSelections(w)[idx] = motion;
  if (waveformPenDown(w))
    waveformPenDown(w)[idx] = idx ? waveformPenDown(w)[idx - 1] : waveformPenDown(w)[idx + 1];
  waveformCount(w)++;
  XtFree((char *)oldxvals);
  XtFree((char *)oldyvals);
  if (oldselections)
    XtFree(oldselections);
  if (oldpendown)
    XtFree(oldpendown);
  if (callcallbacks) {
    XmdsWavedrawValueCBStruct cb = { XmdsCRAddPoint, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    XmdsWavedrawFitCBStruct fitcb = { XmdsCRAddPoint, 0, 0, 0, 0, 0, 0 };;
    cb.idx = idx;
    cb.oldx = xValue(w)[idx + 1];
    cb.oldy = yValue(w)[idx + 1];
    cb.newx = *newx;
    cb.newy = *newy;
    cb.count = waveformCount(w);
    cb.x = xValue(w);
    cb.y = yValue(w);
    cb.selected = waveformSelections(w);
    fitcb.count = waveformCount(w);
    fitcb.x = xValue(w);
    fitcb.y = yValue(w);
    fitcb.selected = waveformSelections(w);
    fitcb.pen_down = waveformPenDown(w);

    XtCallCallbacks((Widget) w, XmdsNaddPointCallback, &cb);
    XtCallCallbacks((Widget) w, XmdsNfitCallback, &fitcb);
  }
  Refresh(w);
  return 1;
}

static double Round(double value, double resolution)
{

  return (resolution > 0) ? ((value >= 0) ? (int)(value / resolution + .4999999) * resolution
			     : (int)(value / resolution - .4999999) * resolution)
      : value;
}

static void SetPointerMode(XmdsWavedrawWidget w, int mode)
{
  (*(void (*)(XmdsWavedrawWidget, int))xmdsWaveformClassRec.waveform_class.
   set_pointer_mode_proc) (w, mode);
  SetTranslations(w);
  SetCursor(w);
}

static int XPix(XmdsWavedrawWidget w, int idx)
{
  float value = xValue(w)[idx];
  float minval = *xMin(w);
  float maxval = *xMax(w);
  float span = maxval - minval;
  float pixspan = XtWidth(w);
  float pix_over_span = span ? pixspan / span : 0;
  return (value - minval) * pix_over_span;
}

static int YPix(XmdsWavedrawWidget w, int idx)
{
  float value = yValue(w)[idx];
  float minval = *yMin(w);
  float maxval = *yMax(w);
  float span = maxval - minval;
  float pixspan = XtHeight(w);
  float pix_over_span = span ? pixspan / span : 0;
  return (maxval - value) * pix_over_span;
}
