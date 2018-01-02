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

		Name:   XmdsWaveform

		Type:   C function

		Author:	Thomas W. Fredian

		Date:   29-MAY-1990

		Purpose: Create an WAVEFORM widget

------------------------------------------------------------------------------

	Call sequence:

Widget XmdsCreateWaveform( parent, name, args, argcount )

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

 Macros:                                                                      */

#define min(a,b) ( ((a)<(b)) ? (a) : (b) )
#define max(a,b) ( ((a)>(b)) ? (a) : (b) )
#define compare_sign(a,b) ( ((a)>0 && (b)>0) ? 1 : (((a)<0 && (b)<0) ? 1 : 0) )
#define interp(x1,y1,x2,y2,x) ( ((y2)-(y1)) * ( (x) / ((x2)-(x1)) ) + ( ( (y1) * ( (x2) / ((x2)-(x1)) ) ) - \
         ( (y2) * ( (x1) / ((x2)-(x1)) ) ) ) )
#define pointsPerIo 1024
#define recsPerIo 256
#define roprand 32768
#define missing (int)0x80000000
#define huge 1E37

#define xMin(widget)          ((widget)->waveform.x.minval)
#define xMax(widget)          ((widget)->waveform.x.maxval)
#define xCrosshair(widget)    ((widget)->waveform.x.crosshair)
#define xGridLines(widget)    ((widget)->waveform.x.grid_lines)
#define xLabels(widget)       ((widget)->waveform.x.labels)
#define xMotion(widget)       ((widget)->waveform.x.motion)
#define xValue(widget)        ((widget)->waveform.x.value)
#define xValPtr(widget)    ((widget)->waveform.x.val_ptr)
#define xValStruct(widget)    ((widget)->waveform.x.val_struct)
#define xPixValue(widget)     ((widget)->waveform.x.pix_value)
#define xOffset(widget)       ((widget)->waveform.x.offset)
#define xResolution(widget)   ((widget)->waveform.x.pointer_resolution)
#define yMin(widget)          ((widget)->waveform.y.minval)
#define yMax(widget)          ((widget)->waveform.y.maxval)
#define yCrosshair(widget)    ((widget)->waveform.y.crosshair)
#define yGridLines(widget)    ((widget)->waveform.y.grid_lines)
#define yLabels(widget)       ((widget)->waveform.y.labels)
#define yMotion(widget)       ((widget)->waveform.y.motion)
#define yValue(widget)        ((widget)->waveform.y.value)
#define yValPtr(widget)    ((widget)->waveform.y.val_ptr)
#define yValStruct(widget)    ((widget)->waveform.y.val_struct)
#define yPixValue(widget)     ((widget)->waveform.y.pix_value)
#define yOffset(widget)       ((widget)->waveform.y.offset)
#define yResolution(widget)   ((widget)->waveform.y.pointer_resolution)

#define waveformRedraw(widget)          ((widget)->waveform.redraw)
#define waveformTitle(widget)           ((widget)->waveform.title)
#define waveformPointerMode(widget)     ((widget)->waveform.pointer_mode)
#define waveformGridStyle(widget)       ((widget)->waveform.grid_style)
#define waveformAttachCrosshairs(widget) ((widget)->waveform.attach_crosshairs)
#define waveformCrosshairsCallbacks(widget) ((widget)->waveform.crosshairs_callback)
#define waveformAlignCallbacks(widget)  ((widget)->waveform.align_callback)
#define waveformButton3Callbacks(widget)  ((widget)->waveform.button3_callback)
#define waveformUnderlayCallbacks(widget)   ((widget)->waveform.underlay_callback)
#define waveformOverlayCallbacks(widget)   ((widget)->waveform.overlay_callback)
#define waveformCutCallbacks(widget)   ((widget)->waveform.cut_callback)
#define waveformPasteCallbacks(widget)   ((widget)->waveform.paste_callback)
#define waveformReverse(widget)           ((widget)->waveform.reverse)
#define waveformPlotGC(widget)          ((widget)->waveform.plot_gc)
#define waveformGridGC(widget)          ((widget)->waveform.grid_gc)
#define waveformFlipGC(widget)          ((widget)->waveform.flip_gc)
#define waveformEraseGC(widget)          ((widget)->waveform.erase_gc)
#define waveformCount(widget)           ((widget)->waveform.count)
#define waveformSelectionsValue(widget) ((widget)->waveform.selections)
#define waveformSelectionsValStruct(widget)      ((widget)->waveform.selections_val_struct)
#define waveformSelectionsValPtr(widget)      ((widget)->waveform.selections_val_ptr)
#define waveformPenDownValue(widget)         ((widget)->waveform.pen_down)
#define waveformPenDownValStruct(widget)      ((widget)->waveform.pen_down_val_struct)
#define waveformPenDownValPtr(widget)      ((widget)->waveform.pen_down_val_ptr)
#define waveformPanning(widget)         ((widget)->waveform.panning)
#define waveformPixmap(widget)         ((widget)->waveform.pixmap)
#define waveformDrawable(widget)         ((widget)->waveform.drawable)
#define waveformFontStruct(widget)      ((widget)->waveform.font_struct)
#define waveformCrosshairsPix(widget)   ((widget)->waveform.cross_box.crosshairs_pix)
#define waveformBoxPix(widget)   ((widget)->waveform.cross_box.rectangle)
#define waveformPanWith(widget)         ((widget)->waveform.pan_with)
#define waveformShowMode(widget)      ((widget)->waveform.show_mode)
#define waveformShowSelections(widget)  ((widget)->waveform.show_selections)
#define waveformStepPlot(widget)        ((widget)->waveform.step_plot)
#define waveformDisabled(widget)         ((widget)->waveform.disabled)
#define waveformClosed(widget)	          ((widget)->waveform.closed)

/*------------------------------------------------------------------------------

 Global variables:                                                            */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>
#include <xmdsshr.h>
#include <Xmds/XmdsWaveformP.h>

/*------------------------------------------------------------------------------

 Local variables:                                                             */

static float height_page;
enum crosshairsmode {
  move_xh, first_xh, last_xh
};

static void ClassPartInitialize();
static void Initialize();
static void Flush();
static void ForceUpdate();
static void Realize();
static void Destroy();
static void Resize();
static void Redisplay();
static Boolean SetValues();
static void Update(Widget w_in, XmdsWaveformValStruct * x, XmdsWaveformValStruct * y, char *title,
		   float *xmin, float *xmax, float *ymin, float *ymax, Boolean defer_update);
static void SetWave(Widget w_in, int count, float *x, float *y, Boolean * waveformSelections,
		    Boolean * waveformPenDown, Boolean autoscale, Boolean defer_update);
static void SetCrosshairs(Widget w, float *x, float *y, Boolean attach);
static void SetPointerMode();
static Boolean SetupXandY(XmdsWaveformWidget old, XmdsWaveformWidget req, XmdsWaveformWidget new);
static void WaveformButton3();
static void WaveformHelp();
static void SetCursor(XmdsWaveformWidget w);
static void SetResolutions(XmdsWaveformWidget w);
static double Round(double value, double resolution);
static void SetTranslations(XmdsWaveformWidget w);
static void AutoScale(int num, float *value, float *minval, float *maxval, float *xval,
		      float *minxval, float *maxxval);
static Boolean UpdateLimit(float *old, float *req, float **new);
static Boolean UpdateLimits(XmdsWaveformWidget old, float *xmin, float *xmax, float *ymin,
			    float *ymax, XmdsWaveformWidget new);
int FixupMissing(unsigned sig_args[], unsigned mech_args[]);
static void Pan();
static void PanEnd();
static void DragZoom();
static void ZoomIn();
static void ZoomOut();
static void DrawGrid();
static void DrawCrosshairs();
static void RemoveZeroes();
static void GridScaling(float min_in, float max_in, int divisions_in, int width, int *divisions_out,
			float *first_pix, float *pix_inc, float *first_val, float *val_inc);
static void Point();
static void Align();
static void FreePixVals(XmdsWaveformWidget w);
static void GetPixVals(XmdsWaveformWidget w, Dimension width, Dimension height);
static void Plot(XmdsWaveformWidget w, Boolean free_mem);
static void ConvertToPix(int num, float *minval, float *maxval, float *value,
			 unsigned short pixspan, int *pixval, unsigned int reverse);
static void Print();
static void DrawLines(Display * display, Window win, GC gc, XPoint * point, int kp, Dimension width,
		      Dimension height);
static void DrawRectangles(Display * display, Window win, GC gc, XRectangle * rectangle, int num,
			   Dimension width, Dimension height);
static void DrawSegments(Display * display, Window win, GC gc, float *crosshairs, int num);
static void SetDashes(Display * display, GC gc, int offset, char *dashes, int num);
static void DrawString(XmdsWaveformWidget w, Display * display, Window win, GC gc, float x, float y,
		       char *label, int length);
static void Reverse();
static void Cut();
static void Paste();

static inline int intAbs(int val) {
  return val < 0 ? -val : val;
}

static XtActionsRec actionlist[] = { {"WaveformPan", Pan},
{"WaveformPanEnd", PanEnd},
{"WaveformPanZoom", DragZoom},
{"WaveformZoomIn", ZoomIn},
{"WaveformZoomOut", ZoomOut},
{"WaveformPoint", Point},
{"WaveformAlign", Align},
{"WaveformCut", Cut},
{"WaveformPaste", Paste},
{"WaveformHelp", WaveformHelp},
{"WaveformButton3", WaveformButton3}
};

static XtResource resources[] = {
  {XmdsNshowMode, "ShowMode", XmRBoolean, sizeof(Boolean),
   XtOffset(XmdsWaveformWidget, waveform.show_mode), XmRImmediate, (XtPointer) NULL}
  ,
  {XmdsNshowSelections, "ShowSelections", XmRBoolean, sizeof(Boolean),
   XtOffset(XmdsWaveformWidget, waveform.show_selections),
   XmRImmediate, (XtPointer) NULL}
  ,
  {XmdsNstepPlot, "StepPlot", XmRBoolean, sizeof(Boolean),
   XtOffset(XmdsWaveformWidget, waveform.step_plot), XmRImmediate, (XtPointer) NULL}
  ,
  {XmdsNdisabled, "Disabled", XmRBoolean, sizeof(Boolean),
   XtOffset(XmdsWaveformWidget, waveform.disabled), XmRImmediate, (XtPointer) NULL}
  ,
  {XmdsNclosed, "Closed", XmRBoolean, sizeof(Boolean),
   XtOffset(XmdsWaveformWidget, waveform.closed), XmRImmediate, (XtPointer) NULL}
  ,
  {XmdsNpointerMode, "PointerMode", XmRInt, sizeof(int),
   XtOffset(XmdsWaveformWidget, waveform.pointer_mode), XmRImmediate,
   (void *)1},
  {XmdsNgridStyle, "GridStyle", XmRInt, sizeof(int),
   XtOffset(XmdsWaveformWidget, waveform.grid_style), XmRImmediate,
   (void *)XmdsGRID_STYLE_LINES},
  {XmdsNlabelFont, "Font", XmRFontStruct, sizeof(XFontStruct *),
   XtOffset(XmdsWaveformWidget, waveform.font_struct),
   XmRString, "-*-NEW CENTURY SCHOOLBOOK-MEDIUM-R-*--*-120-*-*-*-*-ISO8859-1"}
  ,
  {XmdsNcrosshairsCallback, "CrosshairsCallback", XmRCallback, sizeof(XtCallbackList),
   XtOffset(XmdsWaveformWidget, waveform.crosshairs_callback), XmRImmediate, (XtPointer) NULL}
  ,
  {XmdsNattachCrosshairs, "AttachCrosshairs", XmRBoolean, sizeof(Boolean),
   XtOffset(XmdsWaveformWidget, waveform.attach_crosshairs),
   XmRImmediate, (XtPointer) NULL}
  ,
  {XmdsNalignCallback, "AlignCallback", XmRCallback, sizeof(XtCallbackList),
   XtOffset(XmdsWaveformWidget, waveform.align_callback),
   XmRImmediate, (XtPointer) NULL}
  ,
  {XmdsNlimitsCallback, "LimitsCallback", XmRCallback, sizeof(XtCallbackList),
   XtOffset(XmdsWaveformWidget, waveform.limits_callback), XmRImmediate, (XtPointer) NULL}
  ,
  {XmdsNbutton3Callback, "Button3Callback", XmRCallback, sizeof(XtCallbackList),
   XtOffset(XmdsWaveformWidget, waveform.button3_callback), XmRImmediate, (XtPointer) NULL}
  ,
  {XmdsNunderlayCallback, "UnderlayCallback", XmRCallback, sizeof(XtCallbackList),
   XtOffset(XmdsWaveformWidget, waveform.underlay_callback), XmRImmediate, (XtPointer) NULL}
  ,
  {XmdsNoverlayCallback, "OverlayCallback", XmRCallback, sizeof(XtCallbackList),
   XtOffset(XmdsWaveformWidget, waveform.overlay_callback), XmRImmediate, (XtPointer) NULL}
  ,
  {XmdsNtitle, "Title", XmRString, sizeof(String), XtOffset(XmdsWaveformWidget, waveform.title),
   XmRImmediate, (XtPointer) NULL}
  ,
  {XmdsNpanWith, "PanWith", XmRPointer, sizeof(Widget),
   XtOffset(XmdsWaveformWidget, waveform.pan_with), XmRImmediate, (XtPointer) NULL}
  ,
  {XmdsNcount, "Count", XmRInt, sizeof(int), XtOffset(XmdsWaveformWidget, waveform.count),
   XmRImmediate, (XtPointer) NULL},
  {XmdsNselections, "Selections", XmRPointer, sizeof(Boolean *),
   XtOffset(XmdsWaveformWidget, waveform.selections_val_ptr),
   XmRImmediate, (XtPointer) NULL}
  ,
  {XmdsNselectionsValStruct, "val.struct", XmRPointer, sizeof(XmdsWaveformValStructPtr),
   XtOffset(XmdsWaveformWidget, waveform.selections_val_struct), XmRImmediate, (XtPointer) NULL}
  ,
  {XmdsNpenDown, "PenDown", XmRPointer, sizeof(Boolean *),
   XtOffset(XmdsWaveformWidget, waveform.pen_down_val_ptr), XmRImmediate, (XtPointer) NULL}
  ,
  {XmdsNpenDownValStruct, "val.struct", XmRPointer, sizeof(XmdsWaveformValStructPtr),
   XtOffset(XmdsWaveformWidget, waveform.pen_down_val_struct), XmRImmediate, (XtPointer) NULL}
  ,
  {XmdsNxValue, "XValue", XmRPointer, sizeof(float *),
   XtOffset(XmdsWaveformWidget, waveform.x.val_ptr), XmRImmediate, (XtPointer) NULL},
  {XmdsNxValStruct, "val.struct", XmRPointer, sizeof(XmdsWaveformValStructPtr),
   XtOffset(XmdsWaveformWidget, waveform.x.val_struct), XmRImmediate, (XtPointer) NULL}
  ,
  {XmdsNxMin, "Limits", XmRPointer, sizeof(float *),
   XtOffset(XmdsWaveformWidget, waveform.x.minval), XmRImmediate, (XtPointer) NULL},
  {XmdsNxMax, "Limits", XmRPointer, sizeof(float *),
   XtOffset(XmdsWaveformWidget, waveform.x.maxval), XmRImmediate, (XtPointer) NULL},
  {XmdsNxCrosshair, "Crosshairs", "Double", sizeof(double *),
   XtOffset(XmdsWaveformWidget, waveform.x.crosshair), XmRImmediate, (XtPointer) NULL},
  {XmdsNxGridLines, "GridLines", XmRInt, sizeof(int),
   XtOffset(XmdsWaveformWidget, waveform.x.grid_lines),
   XmRImmediate, (void *)4},
  {XmdsNxLabels, "GridLabels", XmRBoolean, sizeof(Boolean),
   XtOffset(XmdsWaveformWidget, waveform.x.labels),
   XmRImmediate, (void *)1},

  {XmdsNyValue, "YValue", XmRPointer, sizeof(float *),
   XtOffset(XmdsWaveformWidget, waveform.y.val_ptr), XmRImmediate, (XtPointer) NULL},
  {XmdsNyValStruct, "val.struct", XmRPointer, sizeof(XmdsWaveformValStructPtr),
   XtOffset(XmdsWaveformWidget, waveform.y.val_struct), XmRImmediate, (XtPointer) NULL}
  ,
  {XmdsNyMin, "Limits", XmRPointer, sizeof(float *),
   XtOffset(XmdsWaveformWidget, waveform.y.minval), XmRImmediate, (XtPointer) NULL},
  {XmdsNyMax, "Limits", XmRPointer, sizeof(float *),
   XtOffset(XmdsWaveformWidget, waveform.y.maxval), XmRImmediate, (XtPointer) NULL},
  {XmdsNyCrosshair, "Crosshairs", "Double", sizeof(double *),
   XtOffset(XmdsWaveformWidget, waveform.y.crosshair), XmRImmediate, (XtPointer) NULL},
  {XmdsNyGridLines, "GridLines", XmRInt, sizeof(int),
   XtOffset(XmdsWaveformWidget, waveform.y.grid_lines),
   XmRImmediate, (void *)4},
  {XmdsNyLabels, "GridLabels", XmRBoolean, sizeof(Boolean),
   XtOffset(XmdsWaveformWidget, waveform.y.labels),
   XmRImmediate, (void *)1},
  {XmdsNcutCallback, "CutCallback", XmRCallback, sizeof(XtCallbackList),
   XtOffset(XmdsWaveformWidget, waveform.cut_callback), XmRImmediate, (XtPointer) NULL}
  ,
  {XmdsNpasteCallback, "PasteCallback", XmRCallback, sizeof(XtCallbackList),
   XtOffset(XmdsWaveformWidget, waveform.paste_callback), XmRImmediate, (XtPointer) NULL}
  ,
};

XmdsWaveformClassRec xmdsWaveformClassRec = {
  {
   /* superclass */ (WidgetClass) & xmPrimitiveClassRec,
   /* class_name */ "XmdsWaveformWidget",
   /* widget size */ sizeof(XmdsWaveformRec),
   /* class_initialize */ NULL,
   /* class_part_initialize */ ClassPartInitialize,
   /* class_inited */ 0,
   /* initialize   */ Initialize,
   /* initialize_hook */ 0,
   /* realize */ Realize,
   /* actions */ actionlist,
   /* num_actions */ XtNumber(actionlist),
   /* resources */ resources,
   /* num_resources */ XtNumber(resources),
   /* xrm_class     */ 0,
   /* compress_motion */ 1,
   /* compress_exposure */ XtExposeCompressMultiple,
   /* compress_enterleave */ 1,
   /* visible_interest */ 1,
   /* destroy */ Destroy,
   /* Resize */ Resize,
   /* expose */ Redisplay,
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
   /* border_highlight */ XmInheritBorderHighlight,
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
   /* update proc             */ Update,
   /* set crosshairs proc     */ SetCrosshairs,
   /* set pointer mode proc   */ SetPointerMode,
   /* print proc              */ Print,
   /* reverse proc            */ Reverse,
   /* set wave proc           */ SetWave,
   /* extension               */ NULL
   }
};

WidgetClass xmdsWaveformWidgetClass;

/* Print related variables */

static FILE *printfid;
static int waveformPrint = 0;
static int resolution = 75;
static float scale, width, height;
static float fontsize;
static float resinc;
/*------------------------------------------------------------------------------

 Executable:                                                                  */

EXPORT Widget XmdsCreateWaveform(Widget parent, char *name, ArgList args, Cardinal argcount)
{
  return XtCreateWidget(name, xmdsWaveformWidgetClass, parent, args, argcount);
}

static void ClassPartInitialize(XmdsWaveformWidgetClass class)
{
#define initTrans(field,deftrans)\
  if (field == XmdsInheritTranslations)\
    field = XtParseTranslationTable(deftrans);\
  else\
    field = XtParseTranslationTable((const char *)field)

  initTrans(class->waveform_class.default_trans,
	    "<Btn3Down>:WaveformButton3()\n @Help <Btn1Down>:WaveformHelp()");
  initTrans(class->waveform_class.zoom_trans,
	    "<Btn1Motion>:WaveformPanZoom()\n <Btn1Up>: WaveformZoomIn()\n \
<Btn3Down>:WaveformButton3()\n @Help <Btn1Down>:WaveformHelp()\n <Btn2Down>: WaveformZoomOut()");
  initTrans(class->waveform_class.drag_trans,
	    "<Btn1Motion> : WaveformPan()\n <Btn1Up> : WaveformPanEnd()\n \
<Btn3Down>:WaveformButton3()\n @Help <Btn1Down>:WaveformHelp()\n <Btn2Motion> : WaveformPan()\n \
<Btn2Up> : WaveformPanEnd()");
  initTrans(class->waveform_class.point_trans,
	    "<Btn1Down> : WaveformPoint()\n <Btn1Motion> : WaveformPoint()\n \
<Btn1Up> : WaveformPoint()\n <Btn3Down>:WaveformButton3()\n @Help <Btn1Down>:WaveformHelp()\n \
<Btn2Down> : WaveformAlign()");
  initTrans(class->waveform_class.edit_trans, "<Btn1Down> : WaveformCut()\n \
<Btn3Down>:WaveformButton3()\n @Help <Btn1Down>:WaveformHelp()\n <Btn2Down> : WaveformPaste()");
  if (class->waveform_class.update_proc == XmdsInheritUpdate)
    class->waveform_class.update_proc = Update;
  if (class->waveform_class.set_crosshairs_proc == XmdsInheritSetCrosshairs)
    class->waveform_class.set_crosshairs_proc = SetCrosshairs;
  if (class->waveform_class.set_pointer_mode_proc == XmdsInheritSetPointerMode)
    class->waveform_class.set_pointer_mode_proc = SetPointerMode;
  if (class->waveform_class.print_proc == XmdsInheritPrint)
    class->waveform_class.print_proc = Print;
  if (class->waveform_class.reverse_proc == XmdsInheritReverse)
    class->waveform_class.reverse_proc = Reverse;
  if (class->waveform_class.set_wave_proc == XmdsInheritSetWave)
    class->waveform_class.set_wave_proc = SetWave;
}

static void Initialize(XmdsWaveformWidget req, XmdsWaveformWidget w, ArgList args,
		       Cardinal * num_args)
{
  XGCValues values;
  static char dashes[] = { 1, 4 };
  xMin(w) = xMax(w) = yMin(w) = yMax(w) = (XtPointer) NULL;
  xValue(w) = 0;
  yValue(w) = 0;
  waveformSelectionsValue(w) = 0;
  waveformPenDownValue(w) = 0;
  waveformPixmap(w) = 0;
  xMotion(w) = 0;
  xOffset(w) = 0;
  yMotion(w) = 0;
  yOffset(w) = 0;
  xPixValue(w) = 0;
  yPixValue(w) = 0;
  xCrosshair(w) = 0.0;
  yCrosshair(w) = 0.0;
  waveformReverse(w) = 0;
  waveformDisabled(w) = 0;
  SetupXandY(0, req, w);
  UpdateLimits(w, xMin(req), xMax(req), yMin(req), yMax(req), w);
  if (waveformTitle(req))
    waveformTitle(w) = XtNewString(waveformTitle(req));
  SetTranslations(w);
  values.foreground = XBlackPixelOfScreen(XtScreen(w));
  values.background = XWhitePixelOfScreen(XtScreen(w));
  values.line_style = LineOnOffDash;
  values.font = waveformFontStruct(w)->fid;
  values.function = GXinvert;
  values.plane_mask = XBlackPixelOfScreen(XtScreen(w)) | XWhitePixelOfScreen(XtScreen(w));
  waveformPlotGC(w) = XtGetGC((Widget) w, GCForeground | GCBackground | GCFont, &values);
  waveformGridGC(w) =
      XtGetGC((Widget) w, GCForeground | GCBackground | GCLineStyle | GCFont, &values);
  SetDashes(XtDisplay(w), waveformGridGC(w), 0, dashes, 2);
  waveformFlipGC(w) = XtGetGC((Widget) w, GCFunction | GCPlaneMask | GCFont, &values);
  values.foreground = XWhitePixelOfScreen(XtScreen(w));
  waveformEraseGC(w) = XtGetGC((Widget) w, GCForeground, &values);
  waveformPanning(w) = 0;
  return;
}

static void VisibilityChange(XmdsWaveformWidget w, XtPointer dummy, XVisibilityEvent * event)
{
  if (event->state == VisibilityUnobscured || event->state == VisibilityPartiallyObscured) {
    XtRemoveEventHandler((Widget) w, VisibilityChangeMask, False, (void (*)())VisibilityChange,
			 dummy);
    ForceUpdate(w);
  }
}

static void Realize(XmdsWaveformWidget w, XtValueMask * mask, XSetWindowAttributes * attr)
{
  *mask |= CWBackingStore;
  attr->backing_store = XDoesBackingStore(XtScreen(w->core.parent)) == Always ? Always : 0;
  (*xmPrimitiveClassRec.core_class.realize) ((Widget) w, mask, attr);
  if (attr->backing_store != Always) {
    waveformPixmap(w) = waveformDrawable(w) =
	XCreatePixmap(XtDisplay(w), XtWindow(w), XtWidth(w), XtHeight(w),
		      XDefaultDepthOfScreen(XtScreen(w)));
    waveformRedraw(w) = True;
    ForceUpdate(w);
  } else
    waveformDrawable(w) = XtWindow(w);
}

static void Redisplay(XmdsWaveformWidget w, XExposeEvent * event)
{
  XEvent ev;
  if (event) {
/*
    XEvent visibility_event;
    if (XCheckWindowEvent(XtDisplay(w),XtWindow(w),VisibilityChangeMask,&visibility_event))
    {
      XPutBackEvent(XtDisplay(w),(XEvent *) event);
      XPutBackEvent(XtDisplay(w),&visibility_event);
    }
    else
    {
*/
    while (XCheckWindowEvent(XtDisplay(w), XtWindow(w), ExposureMask, &ev))
      event = (XExposeEvent *) & ev;
    if (XtIsRealized((Widget) w) && XtIsManaged((Widget) w)) {
      if (waveformPixmap(w) && !waveformRedraw(w))
	XCopyArea(XtDisplay(w), waveformPixmap(w), XtWindow(w), waveformPlotGC(w), 0, 0, XtWidth(w),
		  XtHeight(w), 0, 0);
      else if (!w->core.visible)
	XtAddEventHandler((Widget) w, VisibilityChangeMask, False, (void (*)())VisibilityChange, 0);
      else
	Plot(w, 1);
      SetCursor(w);
    }
/*
    }
*/
  } else {
    Plot(w, 1);
    SetCursor(w);
  }
}

static void Resize(XmdsWaveformWidget w)
{
  if (waveformPixmap(w)) {
    XFreePixmap(XtDisplay(w), waveformPixmap(w));
    waveformPixmap(w) = waveformDrawable(w) =
	XCreatePixmap(XtDisplay(w), XtWindow(w), XtWidth(w), XtHeight(w),
		      XDefaultDepthOfScreen(XtScreen(w)));
    waveformRedraw(w) = True;
    ForceUpdate(w);
  }
/*
  if (!w->core.visible)
    XtAddEventHandler((Widget)w,VisibilityChangeMask,False,(void (*)())VisibilityChange,0);
*/
}

static void Destroy(XmdsWaveformWidget w)
{
  if (waveformTitle(w))
    XtFree(waveformTitle(w));
  if (waveformPixmap(w))
    XFreePixmap(XtDisplay(w), waveformPixmap(w));
  FreePixVals(w);

#define destroyVal(field)\
    if (field##ValStruct(w)){\
      if (field##ValStruct(w)->destroy){\
        (*field##ValStruct(w)->destroy)(w,field##ValStruct(w)->destroy_arg);\
      }\
      XtFree((char *)field##ValStruct(w));\
    }\
    if (field##ValPtr(w)) XtFree((char *)field##ValPtr(w));

  destroyVal(y);
  destroyVal(x);
  destroyVal(waveformSelections);
  destroyVal(waveformPenDown);
  XtReleaseGC((Widget) w, waveformPlotGC(w));
  XtReleaseGC((Widget) w, waveformGridGC(w));
  XtReleaseGC((Widget) w, waveformFlipGC(w));
  XtReleaseGC((Widget) w, waveformEraseGC(w));
}

static void Pan(XmdsWaveformWidget w, XButtonEvent * event)
{
  XEvent ev;
  XmdsWaveformWidget pan_w;
  while (XCheckMaskEvent(XtDisplay(w), Button2MotionMask, &ev))
    event = (XButtonEvent *) & ev;
  for (pan_w = w; pan_w; pan_w = (XmdsWaveformWidget) waveformPanWith(pan_w)) {
    if (waveformCount(pan_w)) {
      if (event->state & Button2Mask)
	yMotion(pan_w) = event->y;
      if (waveformPanning(pan_w)) {
	XmdsWaveformLimitsCBStruct limits = { XmdsCRDrag, 0, 0, 0, 0, 0 };
	float delta_x =
	    ((*xMin(pan_w) - *xMax(pan_w)) / (float)XtWidth(pan_w)) * (float)(event->x -
									      xMotion(pan_w));
	float delta_y =
	    ((*yMin(pan_w) - *yMax(pan_w)) / (float)XtHeight(pan_w)) * (float)(event->y -
									       yMotion(pan_w));
	limits.event = (XEvent *) event;
	limits.xminval = xMin(pan_w);
	limits.xmaxval = xMax(pan_w);
	limits.yminval = yMin(pan_w);
	limits.ymaxval = yMax(pan_w);
	xOffset(pan_w) += event->x - xMotion(pan_w);
	yOffset(pan_w) += event->y - yMotion(pan_w);
	xMotion(pan_w) = event->x;
	yMotion(pan_w) = event->y;
	*xMin(pan_w) += delta_x;
	*xMax(pan_w) += delta_x;
	*yMin(pan_w) -= delta_y;
	*yMax(pan_w) -= delta_y;
	*xMin(pan_w) = min(huge, max(-huge, *xMin(pan_w)));
	*xMax(pan_w) = min(huge, max(-huge, *xMax(pan_w)));
	*yMin(pan_w) = min(huge, max(-huge, *yMin(pan_w)));
	*yMax(pan_w) = min(huge, max(-huge, *yMax(pan_w)));
	XtCallCallbacks((Widget) pan_w, XmdsNlimitsCallback, &limits);
	*xMin(pan_w) = min(huge, max(-huge, *xMin(pan_w)));
	*xMax(pan_w) = min(huge, max(-huge, *xMax(pan_w)));
	*yMin(pan_w) = min(huge, max(-huge, *yMin(pan_w)));
	*yMax(pan_w) = min(huge, max(-huge, *yMax(pan_w)));
	Plot(pan_w, 0);
      } else {
	waveformPanning(pan_w) = 1;
	xMotion(pan_w) = event->x;
	yMotion(pan_w) = event->y;
	Plot(pan_w, 0);
      }
    }
    if ((!(event->state & Button2Mask)) || ((XmdsWaveformWidget) waveformPanWith(pan_w) == w))
      break;
  }
}

static void PanEnd(XmdsWaveformWidget w, XButtonEvent * event)
{
  XmdsWaveformWidget pan_w;
  for (pan_w = w; pan_w; pan_w = (XmdsWaveformWidget) waveformPanWith(pan_w)) {
    if (waveformCount(pan_w)) {
      if (event->state & ShiftMask) {
	XmdsWaveformLimitsCBStruct limits = { XmdsCRDragEnd, 0, 0, 0, 0, 0 };
	limits.event = (XEvent *) event;
	limits.xminval = xMin(pan_w);
	limits.xmaxval = xMax(pan_w);
	limits.yminval = yMin(pan_w);
	limits.ymaxval = yMax(pan_w);
	*xMin(pan_w) = *xMin(w);
	*xMax(pan_w) = *xMax(w);
	XtCallCallbacks((Widget) pan_w, XmdsNlimitsCallback, &limits);
	*xMin(pan_w) = min(huge, max(-huge, *xMin(pan_w)));
	*xMax(pan_w) = min(huge, max(-huge, *xMax(pan_w)));
	*yMin(pan_w) = min(huge, max(-huge, *yMin(pan_w)));
	*yMax(pan_w) = min(huge, max(-huge, *yMax(pan_w)));
	waveformPanning(pan_w) = 0;
	SetResolutions(pan_w);
	Plot(pan_w, 1);
      } else {
	if (event->state & Button2Mask)
	  yMotion(pan_w) = event->y;
	if (waveformPanning(pan_w)) {
	  XmdsWaveformLimitsCBStruct limits = { XmdsCRDragEnd, 0, 0, 0, 0, 0 };
	  float delta_x =
	      ((*xMin(pan_w) - *xMax(pan_w)) / (float)XtWidth(pan_w)) * (float)(event->x -
										xMotion(pan_w));
	  float delta_y =
	      ((*yMin(pan_w) - *yMax(pan_w)) / (float)XtHeight(pan_w)) * (float)(event->y -
										 yMotion(pan_w));
	  limits.event = (XEvent *) event;
	  limits.xminval = xMin(pan_w);
	  limits.xmaxval = xMax(pan_w);
	  limits.yminval = yMin(pan_w);
	  limits.ymaxval = yMax(pan_w);
	  *xMin(pan_w) += delta_x;
	  *xMax(pan_w) += delta_x;
	  *yMin(pan_w) -= delta_y;
	  *yMax(pan_w) -= delta_y;
	  *xMin(pan_w) = min(huge, max(-huge, *xMin(pan_w)));
	  *xMax(pan_w) = min(huge, max(-huge, *xMax(pan_w)));
	  *yMin(pan_w) = min(huge, max(-huge, *yMin(pan_w)));
	  *yMax(pan_w) = min(huge, max(-huge, *yMax(pan_w)));
	  XtCallCallbacks((Widget) pan_w, XmdsNlimitsCallback, &limits);
	  *xMin(pan_w) = min(huge, max(-huge, *xMin(pan_w)));
	  *xMax(pan_w) = min(huge, max(-huge, *xMax(pan_w)));
	  *yMin(pan_w) = min(huge, max(-huge, *yMin(pan_w)));
	  *yMax(pan_w) = min(huge, max(-huge, *yMax(pan_w)));
	  waveformPanning(pan_w) = 0;
	  SetResolutions(pan_w);
	  Plot(pan_w, 1);
	}
      }
    }
    if ((XmdsWaveformWidget) waveformPanWith(pan_w) == w)
      break;
  }
}

static void DragZoom(XmdsWaveformWidget w, XButtonEvent * event)
{
  if (waveformCount(w)) {
    if (waveformPanning(w)) {
      DrawRectangles(XtDisplay(w), XtWindow(w), waveformFlipGC(w), waveformBoxPix(w), 1, XtWidth(w),
		     XtHeight(w));
      waveformBoxPix(w)[0].x = min(xMotion(w), event->x);
      waveformBoxPix(w)[0].y = min(yMotion(w), event->y);
      waveformBoxPix(w)[0].width = abs(xMotion(w) - event->x);
      waveformBoxPix(w)[0].height = abs(yMotion(w) - event->y);
      DrawRectangles(XtDisplay(w), XtWindow(w), waveformFlipGC(w), waveformBoxPix(w), 1, XtWidth(w),
		     XtHeight(w));
      if (((max(xMotion(w), event->x) - min(xMotion(w), event->x)) > 2)
	  || ((max(yMotion(w), event->y) - min(yMotion(w), event->y)) > 2))
	waveformPanning(w) = 2;
    } else {
      xMotion(w) = event->x;
      yMotion(w) = event->y;
      waveformBoxPix(w)[0].x = 0;
      waveformBoxPix(w)[0].y = 0;
      waveformBoxPix(w)[0].width = 0;
      waveformBoxPix(w)[0].height = 0;
      waveformPanning(w) = 1;
    }
  }
}

static void ZoomIn(XmdsWaveformWidget w, XButtonEvent * event)
{
  if (!waveformCount(w))
    return;
  if (waveformPanning(w) == 2 &&
      ((max(xMotion(w), event->x) - min(xMotion(w), event->x)) < 5) &&
      ((max(yMotion(w), event->y) - min(yMotion(w), event->y)) < 5)) {
    waveformPanning(w) = 0;
    Plot(w, 1);
  } else {
    XmdsWaveformLimitsCBStruct limits = { 0, 0, 0, 0, 0, 0 };
    double xspan = *xMax(w) - *xMin(w);
    double xbase = *xMin(w);
    double yspan = *yMax(w) - *yMin(w);
    double ybase = *yMin(w);
    double xmin_save = *xMin(w);
    double xmax_save = *xMax(w);
    double ymin_save = *yMin(w);
    double ymax_save = *yMax(w);
    limits.event = (XEvent *) event;
    limits.xminval = xMin(w);
    limits.xmaxval = xMax(w);
    limits.yminval = yMin(w);
    limits.ymaxval = yMax(w);
    if (waveformPanning(w) == 2) {
      limits.reason = XmdsCRBoxZoom;
      *xMin(w) = xbase + min(xMotion(w), event->x) * (xspan / XtWidth(w));
      *xMax(w) = xbase + max(xMotion(w), event->x) * (xspan / XtWidth(w));
      *yMin(w) =
	  ybase + (double)(XtHeight(w) - (double)max(yMotion(w), event->y)) * (yspan / XtHeight(w));
      *yMax(w) =
	  ybase + (double)(XtHeight(w) - (double)min(yMotion(w), event->y)) * (yspan / XtHeight(w));
      waveformPanning(w) = 0;
    } else {
      limits.reason = XmdsCRZoomIn;
      *xMin(w) = xbase + (event->x - .25 * XtWidth(w)) * (xspan / XtWidth(w));
      *xMax(w) = xbase + (event->x + .25 * XtWidth(w)) * (xspan / XtWidth(w));
      *yMin(w) = ybase + (.75 * XtHeight(w) - event->y) * (yspan / XtHeight(w));
      *yMax(w) = ybase + (1.25 * XtHeight(w) - event->y) * (yspan / XtHeight(w));
      waveformPanning(w) = 0;
    }
    if ((*xMin(w) >= *xMax(w)) || (*xMin(w) < -huge) || (*xMax(w) > huge)) {
      *xMin(w) = xmin_save;
      *xMax(w) = xmax_save;
    }
    if ((*yMin(w) >= *yMax(w)) || (*yMin(w) < -huge) || (*yMax(w) > huge)) {
      *yMin(w) = ymin_save;
      *yMax(w) = ymax_save;
    }
    XtCallCallbacks((Widget) w, XmdsNlimitsCallback, &limits);
    *xMin(w) = min(huge, max(-huge, *xMin(w)));
    *xMax(w) = min(huge, max(-huge, *xMax(w)));
    *yMin(w) = min(huge, max(-huge, *yMin(w)));
    *yMax(w) = min(huge, max(-huge, *yMax(w)));
    SetResolutions(w);
    Plot(w, 1);
    XWarpPointer(XtDisplay(w), None, XtWindow(w), event->x, event->y, XtWidth(w), XtHeight(w),
		 XtWidth(w) / 2, XtHeight(w) / 2);
    if (event->state & ShiftMask) {
      XmdsWaveformWidget pan_w;
      for (pan_w = (XmdsWaveformWidget) waveformPanWith(w); pan_w;
	   pan_w = (XmdsWaveformWidget) waveformPanWith(pan_w)) {
	if (waveformCount(pan_w)) {
	  XmdsWaveformLimitsCBStruct limits = { XmdsCRZoomIn, 0, 0, 0, 0, 0 };
	  limits.event = (XEvent *) event;
	  limits.xminval = xMin(pan_w);
	  limits.xmaxval = xMax(pan_w);
	  limits.yminval = yMin(pan_w);
	  limits.ymaxval = yMax(pan_w);
	  *xMin(pan_w) = *xMin(w);
	  *xMax(pan_w) = *xMax(w);
	  *xMin(pan_w) = min(huge, max(-huge, *xMin(pan_w)));
	  *xMax(pan_w) = min(huge, max(-huge, *xMax(pan_w)));
	  XtCallCallbacks((Widget) pan_w, XmdsNlimitsCallback, &limits);
	  *xMin(pan_w) = min(huge, max(-huge, *xMin(pan_w)));
	  *xMax(pan_w) = min(huge, max(-huge, *xMax(pan_w)));
	  *yMin(pan_w) = min(huge, max(-huge, *yMin(pan_w)));
	  *yMax(pan_w) = min(huge, max(-huge, *yMax(pan_w)));
	  SetResolutions(pan_w);
	  Plot(pan_w, 1);
	}
	if ((XmdsWaveformWidget) waveformPanWith(pan_w) == w)
	  break;
      }
    }
  }
}

static void ZoomOut(XmdsWaveformWidget w, XButtonEvent * event)
{
  XmdsWaveformLimitsCBStruct limits = { XmdsCRZoomOut, 0, 0, 0, 0, 0 };
  double xspan = *xMax(w) - *xMin(w);
  double xbase = *xMin(w);
  double yspan = *yMax(w) - *yMin(w);
  double ybase = *yMin(w);
  double xhere = event->x;
  double yhere = event->y;
  limits.event = (XEvent *) event;
  limits.xminval = xMin(w);
  limits.xmaxval = xMax(w);
  limits.yminval = yMin(w);
  limits.ymaxval = yMax(w);
  if (!waveformCount(w))
    return;
  *xMin(w) = xbase + (xhere - XtWidth(w)) * (xspan / XtWidth(w));
  *xMax(w) = xbase + (xhere + XtWidth(w)) * (xspan / XtWidth(w));
  *yMin(w) = ybase + (-yhere) * (yspan / XtHeight(w));
  *yMax(w) = ybase + (2.0 * XtHeight(w) - yhere) * (yspan / XtHeight(w));
  *xMin(w) = min(huge, max(-huge, *xMin(w)));
  *xMax(w) = min(huge, max(-huge, *xMax(w)));
  *yMin(w) = min(huge, max(-huge, *yMin(w)));
  *yMax(w) = min(huge, max(-huge, *yMax(w)));
  XtCallCallbacks((Widget) w, XmdsNlimitsCallback, &limits);
  *xMin(w) = min(huge, max(-huge, *xMin(w)));
  *xMax(w) = min(huge, max(-huge, *xMax(w)));
  *yMin(w) = min(huge, max(-huge, *yMin(w)));
  *yMax(w) = min(huge, max(-huge, *yMax(w)));
  SetResolutions(w);
  Plot(w, 1);
  XWarpPointer(XtDisplay(w), None, XtWindow(w), event->x, event->y, XtWidth(w), XtHeight(w),
	       XtWidth(w) / 2, XtHeight(w) / 2);
  if (event->state & ShiftMask) {
    XmdsWaveformWidget pan_w;
    for (pan_w = (XmdsWaveformWidget) waveformPanWith(w); pan_w;
	 pan_w = (XmdsWaveformWidget) waveformPanWith(pan_w)) {
      if (waveformCount(pan_w)) {
	XmdsWaveformLimitsCBStruct limits = { XmdsCRZoomOut, 0, 0, 0, 0, 0 };
	limits.event = (XEvent *) event;
	limits.xminval = xMin(pan_w);
	limits.xmaxval = xMax(pan_w);
	limits.yminval = yMin(pan_w);
	limits.ymaxval = yMax(pan_w);
	*xMin(pan_w) = *xMin(w);
	*xMax(pan_w) = *xMax(w);
	*xMin(pan_w) = min(huge, max(-huge, *xMin(pan_w)));
	*xMax(pan_w) = min(huge, max(-huge, *xMax(pan_w)));
	XtCallCallbacks((Widget) pan_w, XmdsNlimitsCallback, &limits);
	*xMin(pan_w) = min(huge, max(-huge, *xMin(pan_w)));
	*xMax(pan_w) = min(huge, max(-huge, *xMax(pan_w)));
	*yMin(pan_w) = min(huge, max(-huge, *yMin(pan_w)));
	*yMax(pan_w) = min(huge, max(-huge, *yMax(pan_w)));
	SetResolutions(pan_w);
	Plot(pan_w, 1);
      }
      if ((XmdsWaveformWidget) waveformPanWith(pan_w) == w)
	break;
    }
  }
}

static void Point(XmdsWaveformWidget w, XButtonEvent * event)
{
  XEvent ev;
  XmdsWaveformCrosshairsCBStruct crosshair = { XmdsCRCrosshairs, 0, 0, 0};
  float x, y;
  /* flush the event que for crosshair events */
  while (XCheckTypedWindowEvent(XtDisplay(w), XtWindow(w), event->type, &ev))
    event = (XButtonEvent *) & ev;
  x = Round(((float)event->x / XtWidth(w)) * (*xMax(w) - *xMin(w)) + *xMin(w), xResolution(w));
  y = Round((((float)((int)XtHeight(w) - event->y)) / XtHeight(w)) * (*yMax(w) - *yMin(w)) +
	    *yMin(w), yResolution(w));
  crosshair.event = (XEvent *) event;
  if (!waveformCount(w))
    return;
  SetCrosshairs((Widget) w, &x, &y, waveformAttachCrosshairs(w));
  crosshair.x = xCrosshair(w);
  crosshair.y = yCrosshair(w);
  XtCallCallbacks((Widget) w, XmdsNcrosshairsCallback, &crosshair);
}

static void Align(XmdsWaveformWidget w, XButtonEvent * event)
{
  XmdsWaveformLimitsCBStruct limits = { XmdsCRAlign, 0, 0, 0, 0, 0 };
  limits.event = (XEvent *) event;
  limits.xminval = xMin(w);
  limits.xmaxval = xMax(w);
  limits.yminval = yMin(w);
  limits.ymaxval = yMax(w);
  if (!waveformCount(w))
    return;
  XtCallCallbacks((Widget) w, XmdsNalignCallback, &limits);
}

static Boolean SetValues(XmdsWaveformWidget old, XmdsWaveformWidget req, XmdsWaveformWidget new,
			 ArgList args, Cardinal * num_args)
{
  Boolean values_changed = SetupXandY(old, req, new);
  Boolean limits_changed = UpdateLimits(old, xMin(req), xMax(req), yMin(req), yMax(req), new);
  Boolean plot_changed = values_changed ||
      limits_changed ||
      xGridLines(old) != xGridLines(req) ||
      xLabels(old) != xLabels(req) ||
      yGridLines(old) != yGridLines(req) ||
      yLabels(old) != yLabels(req) ||
      waveformShowMode(old) != waveformShowMode(req) ||
      waveformShowSelections(old) != waveformShowSelections(req) ||
      waveformStepPlot(old) != waveformStepPlot(req) ||
      waveformTitle(old) != waveformTitle(req) ||
      waveformFontStruct(old) != waveformFontStruct(req);

  waveformPanning(new) = 0;
  FreePixVals(new);
  if (waveformFontStruct(old) != waveformFontStruct(req)) {
    XGCValues values;
    static char dashes[] = { 1, 4 };
    values.foreground = XBlackPixelOfScreen(XtScreen(req));
    values.background = XWhitePixelOfScreen(XtScreen(req));
    values.line_style = LineOnOffDash;
    values.font = waveformFontStruct(req)->fid;
    values.function = GXinvert;
    values.plane_mask = XBlackPixelOfScreen(XtScreen(req)) | XWhitePixelOfScreen(XtScreen(req));
    if (waveformPlotGC(req))
      XtReleaseGC((Widget) req, waveformPlotGC(req));
    if (waveformGridGC(req))
      XtReleaseGC((Widget) req, waveformGridGC(req));
    if (waveformFlipGC(req))
      XtReleaseGC((Widget) req, waveformFlipGC(req));
    waveformPlotGC(new) = XtGetGC((Widget) new, GCForeground | GCBackground | GCFont, &values);
    waveformGridGC(new) =
	XtGetGC((Widget) new, GCForeground | GCBackground | GCLineStyle | GCFont, &values);
    SetDashes(XtDisplay(new), waveformGridGC(new), 0, dashes, 2);
    waveformFlipGC(new) = XtGetGC((Widget) new, GCFunction | GCPlaneMask | GCFont, &values);
  }
  if (waveformPointerMode(old) != waveformPointerMode(req)) {
    if (waveformPointerMode(old) == XmdsPOINTER_MODE_POINT)
      DrawCrosshairs(new, last_xh);
    else if (waveformPointerMode(new) == XmdsPOINTER_MODE_POINT)
      DrawCrosshairs(new, first_xh);
    SetCursor(new);
  }
  if (((waveformCount(old) != waveformCount(new)) && (!(waveformCount(old) && waveformCount(new))))
      || (waveformPointerMode(old) != waveformPointerMode(new)))
    SetTranslations(new);
  if (waveformTitle(req) != waveformTitle(old)) {
    if (waveformTitle(old) != 0)
      XtFree(waveformTitle(old));
    waveformTitle(new) = waveformTitle(req) ? XtNewString(waveformTitle(req)) : 0;
  }
  if (waveformDisabled(req) ? 0 : (waveformDisabled(old) ? 1 : plot_changed))
    Plot(new, 1);
  else if (!(waveformDisabled(req))
	   && ((xCrosshair(old) != xCrosshair(req)) || (yCrosshair(old) != yCrosshair(req)))) {
    float float_x = (float)xCrosshair(new);
    float float_y = (float)yCrosshair(new);
    SetCrosshairs((Widget) new, &float_x, &float_y, waveformAttachCrosshairs(new));
  }
  return 0;
}

static void FreePixVals(XmdsWaveformWidget w)
{
  if (xPixValue(w))
    XtFree((char *)xPixValue(w));
  if (yPixValue(w))
    XtFree((char *)yPixValue(w));
  xPixValue(w) = yPixValue(w) = 0;
}

static void GetPixVals(XmdsWaveformWidget w, Dimension width, Dimension height)
{
  FreePixVals(w);
  xPixValue(w) = (int *)XtCalloc(waveformCount(w), sizeof(*xPixValue(w)));
  ConvertToPix(waveformCount(w), xMin(w), xMax(w), xValue(w), width, xPixValue(w), 0);
  xOffset(w) = 0;
  yPixValue(w) = (int *)XtCalloc(waveformCount(w), sizeof(*yPixValue(w)));
  ConvertToPix(waveformCount(w), yMin(w), yMax(w), yValue(w), height, yPixValue(w), 1);
  yOffset(w) = 0;
}

#define LoadPoint(idx,penDown)\
{\
  XPoint    this_point;\
  static Boolean last_pen_down = False;\
  int x = xp[idx];\
  int y = yp[idx];\
  Boolean is_missing = (x == missing || y == missing);\
  Boolean   pen_down;\
  if (is_missing)\
    pen_down = 0;\
  else\
  {\
    int       xval = x + xoff;\
    int       yval = y + yoff;\
    this_point.x = xval;\
    if (this_point.x != xval)\
    {\
      if (xval < -32768)\
        this_point.x = -32768;\
      else if (xval > 32767)\
        this_point.x = 32767;\
    }\
    this_point.y = yval;\
    if (this_point.y != yval)\
    {\
      if (yval < -32768)\
        this_point.y = -32768;\
      else if (yval > 32767)\
        this_point.y = 32767;\
    }\
    is_missing = ((this_point.x == 32767) || (this_point.x == -32768)) || ((this_point.y == 32767) || (this_point.y == -32768));\
    pen_down = penDown && !is_missing;\
  }\
  if (pen_down || (last_pen_down && idx))\
  {\
    if (step && pidx)\
    {\
      points[pidx].x = this_point.x;\
      points[pidx].y = points[pidx - 1].y;\
      pidx++;\
    }\
    points[pidx++] = this_point;\
  }\
  if (((pidx + 1 >= maxP) || (!pen_down && pidx >= 2)))\
  {\
    DrawLines(XtDisplay(w), waveformDrawable(w), waveformPlotGC(w), points, pidx,XtWidth(w),XtHeight(w));\
    if (pen_down)\
    {\
      points[0] = points[pidx - 1];\
      pidx = 1;\
    }\
    else\
      pidx = 0;\
  }\
  last_pen_down = pen_down;\
  if (!is_missing && (ss || sm))\
 {\
    if (ss && selections[idx])\
      recs[ridx].height = sss;\
    else if (sm)\
      recs[ridx].height = sps;\
    else\
      recs[ridx].height = 0;\
    if (waveformPrint)\
      recs[ridx].height *= resinc;\
    if (recs[ridx].height)\
    {\
      recs[ridx].width = recs[ridx].height;\
      recs[ridx].x = this_point.x - recs[ridx].width / 2;\
      recs[ridx].y = this_point.y - recs[ridx].height / 2;\
      (ridx)++;\
      if (ridx >= maxR)\
      {\
        DrawRectangles(XtDisplay(w), waveformDrawable(w), waveformPlotGC(w), recs, ridx,XtWidth(w),XtHeight(w));\
        recs[0] = recs[ridx - 1];\
        ridx = 1;\
      }\
    }\
  }\
}

static void Plot(XmdsWaveformWidget w, Boolean free_mem)
{
  waveformRedraw(w) = False;
  if (!XtIsRealized((Widget) w))
    return;

  if (!waveformPrint) {
    if (w->core.background_pixmap == XtUnspecifiedPixmap)
      XFillRectangle(XtDisplay(w), waveformDrawable(w), waveformEraseGC(w), 0, 0, XtWidth(w),
		     XtHeight(w));
    else {
      int x, y;
      unsigned long root;
      unsigned int width = 0, height = 0, bw, depth;
      XGetGeometry(XtDisplay(w), w->core.background_pixmap, &root, &x, &y, &width, &height, &bw,
		   &depth);
      for (x = 0; x < XtWidth(w); x += width)
	for (y = 0; y < XtHeight(w); y += height)
	  XCopyArea(XtDisplay(w), w->core.background_pixmap, XtWindow(w), waveformPlotGC(w), 0, 0,
		    XtWidth(w), XtHeight(w), x, y);
    }
    XtCallCallbacks((Widget) w, XmdsNunderlayCallback, (void *)waveformDrawable(w));
  }
  if (waveformCount(w)) {
    if (xPixValue(w) == 0 || yPixValue(w) == 0)
      GetPixVals(w, XtWidth(w), XtHeight(w));
    {
      int i;
      int count = waveformCount(w);
      Boolean *pd = waveformPenDownValue(w);
      int *xp = xPixValue(w);
      int xoff = xOffset(w);
      int *yp = yPixValue(w);
      int yoff = yOffset(w);
      Boolean *selections = waveformSelectionsValue(w);
      Boolean step = waveformStepPlot(w);
      char sm = waveformShowMode(w);
      Boolean ss = waveformShowSelections(w) && waveformSelectionsValue(w);
      char sps = 2;		/* waveformShowPointSize(w); */
      char sss = 4;		/* waveformShowSelectionSize(w); */
      XPoint points[pointsPerIo];
      XRectangle recs[recsPerIo];
      /********************************
       Draw lines. Block I/O's into
       acceptible size. Load X and Y
       values into point array and
       call DRAW LINES routine.
      ********************************/
      int pidx = 0;
      int ridx = 0;
      int max_req = XMaxRequestSize(XtDisplay(w));
      int maxP = max_req / sizeof(XPoint) > pointsPerIo ? pointsPerIo : max_req / sizeof(XPoint);
      int maxR =
	  max_req / sizeof(XRectangle) > recsPerIo ? recsPerIo : max_req / sizeof(XRectangle);
      if (waveformPanning(w) > 0) {
	int j;
	int pix;
	int xmin = -xoff;
	int xmax = XtWidth(w) - xoff;
	for (i = 0; (xp[i] < xmin) && (i < count); i++) ;
	if (i != count) {
	  j = i ? i - 1 : 0;
	  if (pd
	      && ((waveformShowMode(w) == XmdsSHOW_MODE_LINE)
		  || (waveformShowMode(w) == XmdsSHOW_MODE_BOTH))) {
	    LoadPoint(j, pd[j]);
	    LoadPoint(i, pd[i]);
	    for (pix = xmin; pix < xmax; pix = max(pix + 1, xp[i])) {
	      for (i++; xp[i] < pix && i < count; i++) ;
	      if (i == count)
		break;
	      LoadPoint(i, pd[i]);
	    }
	    i = i < count ? i : count - 1;
	    LoadPoint(i, pd[i]);
	  } else {
	    Boolean pd = ((waveformShowMode(w) == XmdsSHOW_MODE_LINE)
			  || (waveformShowMode(w) == XmdsSHOW_MODE_BOTH));
	    LoadPoint(j, pd);
	    LoadPoint(i, pd);
	    for (pix = xmin; pix < xmax; pix = max(pix + 1, xp[i])) {
	      for (i++; xp[i] < pix && i < count; i++) ;
	      if (i == count)
		break;
	      LoadPoint(i, pd);
	    }
	    i = i < count ? i : count - 1;
	    LoadPoint(i, pd);
	  }
	}
      } else {
	if (pd
	    && ((waveformShowMode(w) == XmdsSHOW_MODE_LINE)
		|| (waveformShowMode(w) == XmdsSHOW_MODE_BOTH))) {
	  for (i = 0; i < count; i++)
	    LoadPoint(i, pd[i]);
	  if (waveformClosed(w))
	    LoadPoint(0, pd[0]);
	} else {
	  Boolean pd = ((waveformShowMode(w) == XmdsSHOW_MODE_LINE)
			|| (waveformShowMode(w) == XmdsSHOW_MODE_BOTH));
	  for (i = 0; i < count; i++)
	    LoadPoint(i, pd);
	  if (waveformClosed(w))
	    LoadPoint(0, pd);
	}
      }
      Flush(w, points, pidx, recs, ridx);
    }
    if (free_mem)
      FreePixVals(w);
  }
  if (!waveformPanning(w)) {
    DrawGrid(w);
    if (waveformPointerMode(w) == XmdsPOINTER_MODE_POINT && !waveformPrint)
      DrawCrosshairs(w, first_xh);
  }
  if (!waveformPrint) {
    XtCallCallbacks((Widget) w, XmdsNoverlayCallback, (void *)waveformDrawable(w));
    if (waveformReverse(w))
      XCopyArea(XtDisplay(w), waveformDrawable(w), waveformDrawable(w), waveformFlipGC(w), 0, 0,
		XtWidth(w), XtHeight(w), 0, 0);
    if (waveformPixmap(w))
      XCopyArea(XtDisplay(w), waveformDrawable(w), XtWindow(w), waveformPlotGC(w), 0, 0, XtWidth(w),
		XtHeight(w), 0, 0);
  }
}

static void Flush(XmdsWaveformWidget w, XPoint * points, int numpoints, XRectangle * recs,
		  int numrecs)
{
  if (numpoints >= 2)
    DrawLines(XtDisplay(w), waveformDrawable(w), waveformPlotGC(w), points, numpoints, XtWidth(w),
	      XtHeight(w));
  if (numrecs)
    DrawRectangles(XtDisplay(w), waveformDrawable(w), waveformPlotGC(w), recs, numrecs, XtWidth(w),
		   XtHeight(w));
}

static void DrawGrid(XmdsWaveformWidget w)
{
  int xlines = 0;
  float xfirst;
  float xinc;
  float xfirstv;
  float xvinc;
  int ylines = 0;
  float yfirst;
  float yinc;
  float yfirstv;
  float yvinc;
  int i;
  static char label[16];
  int width = XtWidth(w);
  int height = XtHeight(w);
  Boolean toosmall = width < 100 || height < 40;
  if (!XtIsRealized((Widget) w) || toosmall)
    return;
  if (waveformCount(w)) {
    if (xGridLines(w)) {
      GridScaling(*xMin(w), *xMax(w), xGridLines(w), (int)XtWidth(w), &xlines, &xfirst, &xinc,
		  &xfirstv, &xvinc);
      {
	float *line = (float *)XtMalloc(xlines * sizeof(float) * 4);
	for (i = 0; i < xlines * 4; i = i + 4) {
	  line[i] = line[i + 1] = xfirst + i / 4 * xinc;
	  line[i + 2] = 0;
	  line[i + 3] = waveformGridStyle(w) == XmdsGRID_STYLE_TICKS ? XtHeight(w) * .02 : height;
	}
	DrawSegments(XtDisplay(w), waveformDrawable(w),
		     waveformGridStyle(w) ==
		     XmdsGRID_STYLE_TICKS ? waveformPlotGC(w) : waveformGridGC(w), line, xlines);
	if (waveformGridStyle(w) == XmdsGRID_STYLE_TICKS) {
	  for (i = 0; i < xlines * 4; i = i + 4) {
	    line[i] = line[i + 1] = xfirst + i / 4 * xinc;
	    line[i + 2] = height - XtHeight(w) * .02;
	    line[i + 3] = height;
	  }
	  DrawSegments(XtDisplay(w), waveformDrawable(w), waveformPlotGC(w), line, xlines);
	}
	XtFree((char *)line);
      }
    }
    if (yGridLines(w)) {
      GridScaling(*yMin(w), *yMax(w), yGridLines(w), (int)XtHeight(w), &ylines, &yfirst, &yinc,
		  &yfirstv, &yvinc);
      {
	float *line = (float *)XtMalloc(ylines * sizeof(float) * 4);
	for (i = 0; i < ylines * 4; i = i + 4) {
	  line[i] = 0;
	  line[i + 1] = waveformGridStyle(w) == XmdsGRID_STYLE_TICKS ? XtWidth(w) * .02 : width;
	  line[i + 2] = line[i + 3] = height - (yfirst + i / 4 * yinc);
	}
	DrawSegments(XtDisplay(w), waveformDrawable(w),
		     waveformGridStyle(w) ==
		     XmdsGRID_STYLE_TICKS ? waveformPlotGC(w) : waveformGridGC(w), line, ylines);
	if (waveformGridStyle(w) == XmdsGRID_STYLE_TICKS) {
	  for (i = 0; i < ylines * 4; i = i + 4) {
	    line[i] = width - XtWidth(w) * .02;
	    line[i + 1] = width;
	    line[i + 2] = line[i + 3] = height - (yfirst + i / 4 * yinc);
	  }
	  DrawSegments(XtDisplay(w), waveformDrawable(w), waveformPlotGC(w), line, ylines);
	}
	XtFree((char *)line);
      }
    }
    if (xLabels(w)) {
      for (i = 0; i < xlines; i++) {
	float x = xfirst + i * xinc, twidth;
	double value = Round(xfirstv + i * xvinc, xResolution(w));
	int length = 0;
	sprintf(label, "%g%n", value, &length);
	RemoveZeroes(label, &length);
	twidth = XTextWidth(waveformFontStruct(w), label, length);
	if (waveformPrint)
	  twidth = 0;		/* Let print code figure this. */
	x = max(0, min(x - twidth / 2, (int)XtWidth(w) - twidth));
	if (waveformPrint)
	  x = -x;		/* Tells print code to center. */
	DrawString(w, XtDisplay(w), waveformDrawable(w), waveformPlotGC(w), x,
		   (float)(XtHeight(w) - 2.), label, length);
      }
    }
    if (yLabels(w)) {
      for (i = 0; i < ylines; i++) {
	float y = XtHeight(w) - (yfirst + i * yinc);
	double value = Round(yfirstv + i * yvinc, yResolution(w));
	int length = 0;
	sprintf(label, "%g%n", value, &length);
	RemoveZeroes(label, &length);
	y = (waveformPrint) ? max(fontsize / scale,
				  min(y + fontsize / (scale * 2.),
				      (int)XtHeight(w) -
				      2)) : max(waveformFontStruct(w)->max_bounds.ascent,
						min(y +
						    (waveformFontStruct(w)->max_bounds.ascent +
						     waveformFontStruct(w)->max_bounds.descent) / 2,
						    (int)XtHeight(w) -
						    waveformFontStruct(w)->max_bounds.descent));
	if (waveformPrint)
	  y = max(fontsize / scale, min((int)(scale * (y -
						       fontsize / (scale * 2.))) / scale +
					fontsize / (scale * 2.), (int)XtHeight(w) - 2));
	DrawString(w, XtDisplay(w), waveformDrawable(w), waveformPlotGC(w), 2, y, label, length);
      }
    }
  }
  if (waveformTitle(w)) {
    char *tptr;
    char *eptr;
    int line;
    size_t length;
    int lineheight =
	waveformFontStruct(w)->max_bounds.ascent + waveformFontStruct(w)->max_bounds.descent + 2;
    float lasty = -1E20;
    for (line = 0,
	 tptr = waveformTitle(w), eptr = tptr + strlen(tptr);
	 tptr < eptr; tptr += (length + 1), line++) {
      char *lf = strchr(tptr, '\n');
      int twidth;
      float x;
      float y;
      char *tmpstr;
      length = lf ? (size_t)(lf - tptr) : strlen(tptr);
      tmpstr = strncpy(malloc(length + 1), tptr, length);
      tmpstr[length] = 0;
      twidth = (waveformPrint) ? 0 : XTextWidth(waveformFontStruct(w), tptr, length);
      x = max(0, ((int)XtWidth(w) - twidth) / 2) * ((waveformPrint) ? -1. : 1.);
      y = line * lineheight;
      y = (waveformPrint) ? max(fontsize / scale,
				min(y + fontsize / (scale * 2.),
				    (int)XtHeight(w) -
				    2)) : max(waveformFontStruct(w)->max_bounds.ascent,
					      min(y +
						  (waveformFontStruct(w)->max_bounds.ascent +
						   waveformFontStruct(w)->max_bounds.descent) / 2,
						  (int)XtHeight(w) -
						  waveformFontStruct(w)->max_bounds.descent));
      if (y > lasty)
	DrawString(w, XtDisplay(w), waveformDrawable(w), waveformPlotGC(w), x, y, tmpstr, length);
      free(tmpstr);
      lasty = y;
    }
  }
  return;
}

static void GridScaling(float min_in,
			float max_in,
			int divisions_in,
			int width,
			int *divisions_out,
			float *first_pix, float *pix_inc, float *first_val, float *val_inc)
{
  float divlog;
  float grid;
  double power10;
  float intinc;
  divlog = log10(fabs(max_in - min_in) / divisions_in);
  intinc = pow(10.0, fabs(modf(divlog, &power10)));
  if (intinc < 1.33333333)
    intinc = 1;
  else if (intinc < 2.857)
    intinc = 2;
  else if (intinc < 6.6666)
    intinc = 5;
  else
    intinc = 10;
  *val_inc = pow(10.0, ((divlog < 0) ? -log10(intinc) : log10(intinc)) + power10);
  grid = ceil(min_in / (*val_inc)) * (*val_inc);
  *divisions_out = floor((max_in - grid + (*val_inc)) / (*val_inc));
  if (max_in > min_in) {
    *first_pix = width * ((grid - min_in) / (max_in - min_in));
    *pix_inc = width * ((*val_inc) / (max_in - min_in));
    *first_val = (*first_pix * ((max_in - min_in) / width)) + min_in;
  } else {
    *divisions_out = 2;
    *first_pix = 0;
    *pix_inc = width;
    *first_val = min_in;
    *val_inc = 0;
  }
  return;
}

static void RemoveZeroes(char *string, int *length)
{
  int i;
  int done = 0;
  while (!done) {
    done = 1;
    for (i = *length; i > 0; i--) {
      if (((string[i] == 'e') && (string[i + 1] == '0')) ||
	  ((string[i] == '-') && (string[i + 1] == '0')) ||
	  ((string[i] == '+') && (string[i + 1] == '0'))) {
	int j;
	for (j = i + 1; j < *length; j++)
	  string[j] = string[j + 1];
	(*length)--;
	done = 0;
      } else if ((string[i] == '0') && (string[i + 1] == 'e')) {
	int j;
	for (j = i; j < *length; j++)
	  string[j] = string[j + 1];
	(*length)--;
	done = 0;
      } else if ((string[i] == '.') && (string[i + 1] == 'e')) {
	int j;
	for (j = i; j < *length; j++)
	  string[j] = string[j + 1];
	(*length)--;
      }
    }
  }
  return;
}

static void DrawCrosshairs(XmdsWaveformWidget w, enum crosshairsmode mode)
{
  if (xMax(w) && xMin(w) && yMax(w) && yMin(w)) {
    float xspan = *xMax(w) - *xMin(w);
    float yspan = *yMax(w) - *yMin(w);
    if (xspan > 0 && yspan > 0) {
      unsigned short xpix =
	  max(0, min(XtWidth(w), (xCrosshair(w) - *xMin(w)) * (XtWidth(w) / xspan)) + .499999);
      unsigned short ypix =
	  max(0,
	      min(XtHeight(w),
		  XtHeight(w) - (yCrosshair(w) - *yMin(w)) * (XtHeight(w) / yspan)) + .49999);
      XSegment crosshairs[4];
      crosshairs[0].x1 = waveformCrosshairsPix(w)[0].x1;
      crosshairs[0].y1 = waveformCrosshairsPix(w)[0].y1;
      crosshairs[0].x2 = waveformCrosshairsPix(w)[0].x2;
      crosshairs[0].y2 = waveformCrosshairsPix(w)[0].y2;
      crosshairs[1].x1 = waveformCrosshairsPix(w)[1].x1;
      crosshairs[1].y1 = waveformCrosshairsPix(w)[1].y1;
      crosshairs[1].x2 = waveformCrosshairsPix(w)[1].x2;
      crosshairs[1].y2 = waveformCrosshairsPix(w)[1].y2;
      crosshairs[2].x1 = xpix;
      crosshairs[2].y1 = 0;
      crosshairs[2].x2 = xpix;
      crosshairs[2].y2 = XtHeight(w);
      crosshairs[3].x1 = 0;
      crosshairs[3].y1 = ypix;
      crosshairs[3].x2 = XtWidth(w);
      crosshairs[3].y2 = ypix;

      if (XtIsRealized((Widget) w)) {
	switch (mode) {
	case move_xh:
	  XDrawSegments(XtDisplay(w), XtWindow(w), waveformFlipGC(w), crosshairs, 4);
	  if (waveformPixmap(w))
	    XDrawSegments(XtDisplay(w), waveformPixmap(w), waveformFlipGC(w), crosshairs, 4);
	  waveformCrosshairsPix(w)[0] = crosshairs[2];
	  waveformCrosshairsPix(w)[1] = crosshairs[3];
	  break;

	case first_xh:
	  XDrawSegments(XtDisplay(w), XtWindow(w), waveformFlipGC(w), &crosshairs[2], 2);
	  if (waveformPixmap(w))
	    XDrawSegments(XtDisplay(w), waveformPixmap(w), waveformFlipGC(w), &crosshairs[2], 2);
	  waveformCrosshairsPix(w)[0] = crosshairs[2];
	  waveformCrosshairsPix(w)[1] = crosshairs[3];
	  break;

	case last_xh:
	  XDrawSegments(XtDisplay(w), XtWindow(w), waveformFlipGC(w), waveformCrosshairsPix(w), 2);
	  if (waveformPixmap(w))
	    XDrawSegments(XtDisplay(w), waveformPixmap(w), waveformFlipGC(w),
			  waveformCrosshairsPix(w), 2);
	  break;
	}
      }
    }
  }
}

static void WaveformButton3(XmdsWaveformWidget w, XButtonEvent * event)
{
  XmAnyCallbackStruct cb = { XmdsCRButton3, 0 };
  cb.event = (XEvent *) event;
  XtCallCallbacks((Widget) w, XmdsNbutton3Callback, &cb);
}

static void WaveformHelp(XmdsWaveformWidget w, XButtonEvent * event)
{
  XmAnyCallbackStruct cb = { 0, 0 };
  cb.event = (XEvent *) event;
  XtCallCallbacks((Widget) w, XmNhelpCallback, &cb);
}

static void ConvertToPix(int num, float *minvalin, float *maxvalin, float *value,
			 unsigned short pixspan, int *pixval, unsigned int reverse)
{
  int i;
  float minval = *minvalin;
  float maxval = *maxvalin;
  float span = maxval - minval;
  int *ival = (int *)value;
  if (span) {
    float pix_over_span = pixspan / span;
    if (value) {
      for (i = 0; i < num; i++) {
	if (ival[i] == roprand)
	  pixval[i] = missing;
	else {
	  float fpixval = (reverse) ? (maxval - value[i]) : (value[i] - minval);
	  if (pix_over_span > 1E9) {
	    fpixval = (fpixval > 1) ? 1.1E9 : ((fpixval < -1) ? -1.1E9 : fpixval * pix_over_span);
	  } else {
	    fpixval *= pix_over_span;
	  }
	  pixval[i] = fpixval < -1E9 ? -1E9 : (fpixval > 1E9 ? 1E9 : fpixval);
	}
      }
    } else {
      if (reverse)
	for (i = 0; i < num; i++)
	  pixval[i] = ival[i] == roprand ? missing : (maxval - i) * pix_over_span;
      else
	for (i = 0; i < num; i++)
	  pixval[i] = ival[i] == roprand ? missing : (i - minval) * pix_over_span;
    }
  }
}

static void AutoScale(int num, float *value, float *minval, float *maxval, float *xval,
		      float *minxval, float *maxxval)
{
  int i;
  float span;
  float vmin;
  float vmax;
  int *ival = (int *)value;
  int *ixval = (int *)xval;
  if (value) {
    vmin = huge;
    vmax = -huge;
    for (i = 0; i < num; i++) {
      if ((ival[i] != roprand) && (ixval[i] != roprand)) {
	if (vmin > value[i]) {
	  if (minxval && maxxval) {
	    if (xval[i] >= *minxval && xval[i] <= *maxxval)
	      vmin = value[i];
	  } else if (minxval) {
	    if (xval[i] >= *minxval)
	      vmin = value[i];
	  } else if (maxxval) {
	    if (xval[i] <= *maxxval)
	      vmin = value[i];
	  } else
	    vmin = value[i];
	}
	if (vmax < value[i]) {
	  if (minxval && maxxval) {
	    if (xval[i] >= *minxval && xval[i] <= *maxxval)
	      vmax = value[i];
	  } else if (minxval) {
	    if (xval[i] >= *minxval)
	      vmax = value[i];
	  } else if (maxxval) {
	    if (xval[i] <= *maxxval)
	      vmax = value[i];
	  } else
	    vmax = value[i];
	}
      }
    }
    if ((minxval || maxxval) && (vmin > vmax)) {
      AutoScale(num, value, minval, maxval, xval, 0, 0);
      return;
    }
  } else {
    vmin = 0.;
    vmax = num - 1;
  }
  vmin = max(vmin, -huge);
  vmax = max(vmin, min(vmax, huge));
  if (vmax == vmin) {
    if (vmin == huge)
      vmin = vmax = 0.0;
    span = fabs(vmax);
    if (span <= 0.0)
      span = 10.0;
  } else
    span = vmax - vmin;
  *maxval = vmax + span * .1;
  *minval = vmin - span * .1;
}

static void SetCursor(XmdsWaveformWidget w)
{
  static Boolean first = 1;
  static Cursor zoom_cursor;
  static Cursor drag_cursor;
  static Cursor point_cursor;
  static Cursor edit_cursor;
  if (first) {
    drag_cursor = XCreateFontCursor(XtDisplay(w), XC_fleur);
    point_cursor = XCreateFontCursor(XtDisplay(w), XC_crosshair);
    zoom_cursor = XCreateFontCursor(XtDisplay(w), XC_sizing);
    edit_cursor = XCreateFontCursor(XtDisplay(w), XC_exchange);
    first = 0;
  }
  if (XtWindow(w)) {
    switch (waveformPointerMode(w)) {
    case XmdsPOINTER_MODE_NONE:
      XUndefineCursor(XtDisplay(w), XtWindow(w));
      break;
    case XmdsPOINTER_MODE_ZOOM:
      XDefineCursor(XtDisplay(w), XtWindow(w), zoom_cursor);
      break;
    case XmdsPOINTER_MODE_POINT:
      XDefineCursor(XtDisplay(w), XtWindow(w), point_cursor);
      break;
    case XmdsPOINTER_MODE_DRAG:
      XDefineCursor(XtDisplay(w), XtWindow(w), drag_cursor);
      break;
    case XmdsPOINTER_MODE_EDIT:
      XDefineCursor(XtDisplay(w), XtWindow(w), edit_cursor);
      break;
    }
  }
}

static void SetTranslations(XmdsWaveformWidget w)
{
  Arg arglist[] = { {XmNtranslations, 0} };
  int mode = (waveformPointerMode(w) == XmdsPOINTER_MODE_USER) ? XmdsPOINTER_MODE_USER
      : ((waveformPointerMode(w) == XmdsPOINTER_MODE_EDIT) ? XmdsPOINTER_MODE_EDIT
	 : (waveformCount(w) ? waveformPointerMode(w) : XmdsPOINTER_MODE_NONE));
  switch (mode) {
  case XmdsPOINTER_MODE_USER:
    break;
  case XmdsPOINTER_MODE_NONE:
    arglist[0].value =
	(XtArgVal) ((XmdsWaveformWidgetClass) w->core.widget_class)->waveform_class.default_trans;
    XtSetValues((Widget) w, arglist, XtNumber(arglist));
    break;
  case XmdsPOINTER_MODE_ZOOM:
    arglist[0].value =
	(XtArgVal) ((XmdsWaveformWidgetClass) w->core.widget_class)->waveform_class.zoom_trans;
    XtSetValues((Widget) w, arglist, XtNumber(arglist));
    break;
  case XmdsPOINTER_MODE_POINT:
    arglist[0].value =
	(XtArgVal) ((XmdsWaveformWidgetClass) w->core.widget_class)->waveform_class.point_trans;
    XtSetValues((Widget) w, arglist, XtNumber(arglist));
    break;
  case XmdsPOINTER_MODE_DRAG:
    arglist[0].value =
	(XtArgVal) ((XmdsWaveformWidgetClass) w->core.widget_class)->waveform_class.drag_trans;
    XtSetValues((Widget) w, arglist, XtNumber(arglist));
    break;
  case XmdsPOINTER_MODE_EDIT:
    arglist[0].value =
	(XtArgVal) ((XmdsWaveformWidgetClass) w->core.widget_class)->waveform_class.edit_trans;
    XtSetValues((Widget) w, arglist, XtNumber(arglist));
    break;
  }
}

static void SetResolutions(XmdsWaveformWidget w)
{
  float divlog;
  if (xMin(w) && xMax(w) && yMin(w) && yMax(w) && (*xMin(w) < *xMax(w)) && (*yMin(w) < *yMax(w))) {
    divlog = log10(*xMax(w) - *xMin(w));
    xResolution(w) = pow(10.0, (divlog < 0 ? ceil(divlog) : floor(divlog)) - 4);
    divlog = log10(*yMax(w) - *yMin(w));
    yResolution(w) = pow(10.0, (divlog < 0 ? ceil(divlog) : floor(divlog)) - 4);
  }
}

static double Round(double value, double resolution)
{
  return (resolution > 0) ? floor(value / resolution + .4999999) * resolution : value;
}

static Boolean SetupXandY(XmdsWaveformWidget old, XmdsWaveformWidget req, XmdsWaveformWidget new)
{
  /*    Macros used in this routine only */

#define destroyValStructIfChanged(field)\
    if (field##ValStruct(req) != field##ValStruct(old)){\
      if (field##ValStruct(old)){\
        if (field##ValStruct(old)->destroy) (*field##ValStruct(old)->destroy)(old,field##ValStruct(old)->destroy_arg);\
        XtFree((char *)field##ValStruct(old));\
      }\
      changed = TRUE;\
    }

#define updateValStruct(field,type,minsize)\
  if (((!old) || (field##ValStruct(req) != field##ValStruct(old))) && field##ValStruct(req)) {\
    field##ValStruct(new) = (XmdsWaveformValStruct *)memcpy(XtNew(XmdsWaveformValStruct),field##ValStruct(req),\
                                     sizeof(XmdsWaveformValStruct));\
    waveformCount(new) = min((int)(field##ValStruct(req)->size/sizeof(type)),(minsize)); \
    field##Value(new) = (type *)field##ValStruct(req)->addr;\
    field##ValPtr(new) = (type *)NULL;\
    changed = TRUE;\
  }

#define updateValPtr(field, type)\
    if (field##ValPtr(old) && (field##ValPtr(old) == field##Value(old)))\
    {\
      if (old_count != waveformCount(new))\
      {\
        if ((field##ValPtr(new) == field##Value(new)) && waveformCount(new))\
        {\
          field##Value(new) = (type *)XtRealloc((char *)field##Value(new),waveformCount(new)*sizeof(*field##Value(new)));\
          field##ValPtr(new) = field##Value(new);\
        }\
        else\
        {\
          if (field##ValPtr(new))\
          {\
            field##Value(new) = (type *)XtMalloc(waveformCount(new) * sizeof(*field##Value(new)));\
            memcpy(field##Value(new),field##ValPtr(req),waveformCount(new)*sizeof(*field##Value(new)));\
            field##ValPtr(new) = field##Value(new);\
          }\
          XtFree((char *)field##ValPtr(old));\
        }\
        changed = TRUE;\
      }\
      else if (field##ValPtr(new) && (field##ValPtr(new) != field##Value(new)))\
      {\
        memcpy(field##Value(new),field##ValPtr(req),waveformCount(new)*sizeof(*field##Value(new)));\
        field##ValPtr(new) = field##Value(new);\
        changed = TRUE;\
      }\
    }\
    else if (field##ValPtr(new))\
    {\
      field##Value(new) = (type *)XtMalloc(waveformCount(new) * sizeof(*field##Value(new)));\
      memcpy(field##Value(new),field##ValPtr(req),waveformCount(new)*sizeof(*field##Value(new)));\
      field##ValPtr(new) = field##Value(new);\
      changed = TRUE;\
    }

#define initializeValPtr(field, type)\
      if (field##ValPtr(new))\
      {\
        field##Value(new) = (type *)XtMalloc(waveformCount(new) * sizeof(*field##Value(new)));\
        memcpy(field##Value(new),field##ValPtr(req),waveformCount(new)*sizeof(*field##Value(new)));\
        field##ValPtr(new) = field##Value(new);\
        changed = TRUE;\
      }

  int old_count = 0;
  Boolean changed = FALSE;
  if (old) {
    old_count = waveformCount(old);
    destroyValStructIfChanged(y);
    destroyValStructIfChanged(x);
    destroyValStructIfChanged(waveformSelections);
    destroyValStructIfChanged(waveformPenDown);
  }
  updateValStruct(y, float, 1000000000);
  updateValStruct(x, float, waveformCount(new));
  updateValStruct(waveformSelections, Boolean, waveformCount(new));
  updateValStruct(waveformPenDown, Boolean, waveformCount(new));
/*
  waveformCount(new) = (waveformCount(new) > 1) ? waveformCount(new) : 0;
*/
  if (old) {
    if (old_count != waveformCount(new))
      changed = TRUE;
    updateValPtr(y, float);
    updateValPtr(x, float);
    updateValPtr(waveformSelections, Boolean);
    updateValPtr(waveformPenDown, Boolean);
  } else {
    if (waveformCount(new)) {
      changed = TRUE;
      initializeValPtr(y, float);
      initializeValPtr(x, float);
      initializeValPtr(waveformSelections, Boolean);
      initializeValPtr(waveformPenDown, Boolean);
    } else {
      yValue(new) = yValPtr(new) = 0;
      xValue(new) = xValPtr(new) = 0;
      waveformSelectionsValue(new) = waveformSelectionsValPtr(new) = 0;
      waveformPenDownValue(new) = waveformPenDownValPtr(new) = 0;
    }
  }
  return changed;
}

EXPORT void XmdsWaveformUpdate(Widget w, XmdsWaveformValStruct * x, XmdsWaveformValStruct * y, char *title,
			float *xmin, float *xmax, float *ymin, float *ymax, Boolean defer)
{
  (((XmdsWaveformWidgetClass) w->core.widget_class)->waveform_class.update_proc) (w, x, y, title,
										  xmin, xmax, ymin,
										  ymax, defer);
}

EXPORT void XmdsWaveformSetCrosshairs(Widget w, float *x, float *y, Boolean attach)
{
  (((XmdsWaveformWidgetClass) w->core.widget_class)->waveform_class.set_crosshairs_proc) (w, x, y,
											  attach);
}

EXPORT void XmdsWaveformSetPointerMode(Widget w, int mode)
{
  (((XmdsWaveformWidgetClass) w->core.widget_class)->waveform_class.set_pointer_mode_proc) (w,
											    mode);
}

EXPORT void XmdsWaveformSetWave(Widget w, int count, float *x, float *y, Boolean * select,
			 Boolean * pendown, Boolean autoscale, Boolean defer_update)
{
  (((XmdsWaveformWidgetClass) w->core.widget_class)->waveform_class.set_wave_proc) (w, count, x, y,
										    select, pendown,
										    autoscale,
										    defer_update);
}

static void Update(Widget w_in, XmdsWaveformValStruct * x, XmdsWaveformValStruct * y, char *title,
		   float *xmin, float *xmax, float *ymin, float *ymax, Boolean defer_update)
{
  XmdsWaveformWidget w = (XmdsWaveformWidget) w_in;
  int old_count = waveformCount(w);
  waveformPanning(w) = 0;
  FreePixVals(w);
  if (xValStruct(w)) {
    if (xValStruct(w)->destroy)
      (*xValStruct(w)->destroy) (w, xValStruct(w)->destroy_arg);
    XtFree((char *)xValStruct(w));
  }
  if (yValStruct(w)) {
    if (yValStruct(w)->destroy)
      (*yValStruct(w)->destroy) (w, yValStruct(w)->destroy_arg);
    XtFree((char *)yValStruct(w));
  }
  yValStruct(w) =
      (XmdsWaveformValStruct *) memcpy(XtNew(XmdsWaveformValStruct), y,
				       sizeof(XmdsWaveformValStruct));
  waveformCount(w) = y->size / sizeof(float);
  yValue(w) = (float *)y->addr;
  yValPtr(w) = 0;
  xValStruct(w) =
      (XmdsWaveformValStruct *) memcpy(XtNew(XmdsWaveformValStruct), x,
				       sizeof(XmdsWaveformValStruct));
  waveformCount(w) = min((int)(x->size / sizeof(float)), waveformCount(w));
  xValue(w) = (float *)x->addr;
  xValPtr(w) = 0;
/*
  waveformCount(w) = (waveformCount(w) > 1) ? waveformCount(w) : 0;
*/
  XtFree(waveformTitle(w));
  if (title)
    waveformTitle(w) = XtNewString(title);
  else
    waveformTitle(w) = 0;
  UpdateLimits(w, xmin, xmax, ymin, ymax, w);
  if ((old_count != waveformCount(w)) && (!(old_count && waveformCount(w))))
    SetTranslations(w);
  if (!waveformDisabled(w))
    Plot(w, 1);
}

static void SetWave(Widget w_in, int count, float *x, float *y, Boolean * waveformSelections,
		    Boolean * waveformPenDown, Boolean autoscale, Boolean defer_update)
{
  XmdsWaveformWidget w = (XmdsWaveformWidget) w_in;
  int old_count = waveformCount(w);
  waveformPanning(w) = 0;
  FreePixVals(w);
  waveformCount(w) = count;
#define UpdateField(field, type) \
  if (old_count != count)\
  {\
    if (count)\
    {\
      field##Value(w) = (type *)(old_count ? XtRealloc((char *)field##Value(w),count * sizeof(*field##Value(w))) :\
                                      XtMalloc(count * sizeof(*field##Value(w))));\
      field##ValPtr(w) = field##Value(w);\
    }\
    else\
    {\
      XtFree((char *)field##ValPtr(w));\
      field##Value(w) = 0;\
      field##ValPtr(w) = 0;\
    }\
  }\
  if (count)\
    memcpy(field##ValPtr(w),field,count * sizeof(*field##Value(w)));\

  if (x) {
    UpdateField(x, float);
  }
  if (y) {
    UpdateField(y, float);
  }
  if (waveformSelections) {
    UpdateField(waveformSelections, Boolean);
  }
  if (waveformPenDown) {
    UpdateField(waveformPenDown, Boolean);
  }
  if (count && (autoscale || !(xMin(w) && yMin(w) && xMax(w) && yMax(w)))) {
    if (!xMin(w))
      xMin(w) = (float *)XtMalloc(sizeof(float *));
    if (!xMax(w))
      xMax(w) = (float *)XtMalloc(sizeof(float *));
    if (!yMin(w))
      yMin(w) = (float *)XtMalloc(sizeof(float *));
    if (!yMax(w))
      yMax(w) = (float *)XtMalloc(sizeof(float *));
    AutoScale(count, x, xMin(w), xMax(w), y, 0, 0);
    AutoScale(count, y, yMin(w), yMax(w), x, 0, 0);
  }
  SetResolutions(w);
  if ((old_count != waveformCount(w)) && (!(old_count && waveformCount(w))))
    SetTranslations(w);
  if (!waveformDisabled(w))
    Plot(w, 1);
}

static void SetCrosshairs(Widget w_in, float *x, float *y, Boolean attach)
{
  XmdsWaveformWidget w = (XmdsWaveformWidget) w_in;
  if (waveformCount(w)) {
    xCrosshair(w) = *x;
    yCrosshair(w) = *y;
    if (attach) {
      int *xint = (int *)xValue(w);
      int *yint = (int *)yValue(w);
      float *xval = xValue(w);
      int num = waveformCount(w);
      int i;
      for (i = 0; i < num; i++)
	if ((xint[i] != roprand) && (yint[i] != roprand) && (xCrosshair(w) <= xval[i]))
	  break;
      if ((i == 0) || (i == num)) {
	i = min(i, waveformCount(w) - 1);
	xCrosshair(w) = xint[i] != roprand ? xValue(w)[i] : 0.0;
	yCrosshair(w) = yint[i] != roprand ? yValue(w)[i] : 0.0;
      } else {
	if (waveformStepPlot(w))
	  yCrosshair(w) =
	      yValue(w)[i - ((xCrosshair(w) < xValue(w)[i] && yint[i - 1] != roprand) ? 1 : 0)];
	else {
	  if (xint[i - 1] == roprand || yint[i - 1] == roprand)
	    yCrosshair(w) = yValue(w)[i];
	  else
	    yCrosshair(w) =
		interp((double)xValue(w)[i - 1], (double)yValue(w)[i - 1], (double)xValue(w)[i],
		       (double)yValue(w)[i], (double)xCrosshair(w));
	}
      }
    }
    if (waveformPointerMode(w) == XmdsPOINTER_MODE_POINT)
      DrawCrosshairs(w, move_xh);
  }
}

static void SetPointerMode(XmdsWaveformWidget w, int mode)
{
  int old_mode = waveformPointerMode(w);
  waveformPointerMode(w) = mode;
  if (old_mode != waveformPointerMode(w)) {
    if (waveformCount(w)) {
      if (old_mode == XmdsPOINTER_MODE_POINT)
	DrawCrosshairs(w, last_xh);
      else if (waveformPointerMode(w) == XmdsPOINTER_MODE_POINT)
	DrawCrosshairs(w, first_xh);
    }
    SetCursor(w);
    SetTranslations(w);
  }
}

EXPORT void XmdsWaveformPrint(Widget w, FILE * fid, int width, int height, int rotate,
		       char *title, char *window_title, int inp_resolution)
{
  (((XmdsWaveformWidgetClass) w->core.widget_class)->waveform_class.print_proc)
      (w, fid, width, height, rotate, title, window_title, inp_resolution);
}

static void SetScale(int scaleit)
{
  static int scaled = 0;
  if (scaleit == scaled)
    return;

  scaled = scaleit;

  if (scaleit) {
    fprintf(printfid, "gsave\n");
    fprintf(printfid, "%g %g translate\n", 0., 825. * (1. - scale));
    /* Grid y values are set to ylimit-y (decwindows y are reversed from
       postscript), but the ylimit, normally 825, changes due to scale */
    fprintf(printfid, "%g %g scale\n", scale, scale);
  } else
    fprintf(printfid, "grestore\n");
}

static void Print(XmdsWaveformWidget w, FILE * filefid, int inp_total_width, int inp_total_height,
		  int inp_rotate, char *title, char *window_title, int inp_resolution)

/* if inp_rotate = 0 then print in direction that allows greatest magnification
   if inp_rotate = 1 then print in portrait position
   if inp_rotate = 2 then print in landscape position */
/* If multiple plot on a single page, the first plot printed must be the one located at top left hand corner. */
{

  char *libname = getenv("MDS_LIB_PS");
  static char buf[500];
  static FILE *libfid;
  static Position xorigin, yorigin;
  static float xoffset, yoffset;
  static float x, y;
  static float rotate, width_limit, height_limit, width_page;
  static short swidth, sheight;
  static float margin = .375;
  static unsigned long fatom;
  float xstart, ystart, xend, yend;
  XFontStruct *fs = waveformFontStruct(w);
  int i;
  char *fontinfo = 0;

  printfid = filefid;
  if (printfid == NULL)
    return;

  if (ftell(printfid) == 0) {
    char fontname[80];
    if (inp_rotate == 1) {	/* Portrait */
      rotate = 0;
    } else if (inp_rotate == 2) {	/* Landscape */
      rotate = 90;
    } else {
      if (((inp_total_height) ? inp_total_height : XtHeight(w)) >=
	  ((inp_total_width) ? inp_total_width : XtWidth(w)))
	rotate = 0;
      else
	rotate = 90;
    }

    if (inp_resolution > 0)
      resolution = inp_resolution;

    resinc = resolution / 75.;

    if (getenv("A4PAPER") == NULL) {
      width_page = 8.5;
      height_page = 11.;
    } else {
      width_page = 8.27;
      height_page = 11.69;
    }
    if (rotate == 0) {
      width_limit = (width_page - 2. * margin) * resolution;
      height_limit = (height_page - 2. * margin) * resolution;
    } else {
      width_limit = (height_page - 2. * margin) * resolution;
      height_limit = (width_page - 2. * margin) * resolution;
    }

    XtTranslateCoords((Widget) w, 0, 0, &xorigin, &yorigin);

    width = ((inp_total_width) ? inp_total_width : XtWidth(w)) * resinc;
    height = ((inp_total_height) ? inp_total_height : XtHeight(w)) * resinc;
    scale = min(width_limit / width, height_limit / height);	/* Scale to full size of page */
    width = width * scale;
    height = height * scale;

    libfid = fopen(libname, "r");
    if (libfid == NULL)
      return;
    fprintf(printfid, "%%!PS-Adobe-3.0 EPSF-3.0\n");
    {
      float llx = ((width_limit - width) * 72. / resolution) / 2 + margin * 72;
      float urx = llx + width * 72. / resolution;
      float lly = ((height_limit - height) * 72. / resolution) / 2 + margin * 72;
      float ury = lly + height * 72. / resolution;

      if (rotate == 0)
	fprintf(printfid, "%%%%BoundingBox: %g %g %g %g\n", llx, lly, urx, ury);
      else
	fprintf(printfid, "%%%%BoundingBox: %g %g %g %g\n", lly, llx, ury, urx);

      while (fgets(buf, 500, libfid))
	fwrite(buf, 1, strlen(buf), printfid);
      fclose(libfid);

      fontsize = 13. * 2. / 3.;	/* 2/3 gives the actual height of characters. */
      fatom = XInternAtom(XtDisplay(w), "FAMILY_NAME", False);
      if (!fatom)
	strcpy(fontname, "Courier");
      else {
	for (i = 0; i < fs->n_properties; i++)
	  if (fs->properties[i].name == fatom) {
	    fontinfo = XGetAtomName(XtDisplay(w), fs->properties[i].card32);
	    break;
	  }
	strcpy(fontname, fontinfo);
	XFree(fontinfo);
	fontinfo = NULL;
	if (strstr(fontname, "New Century Schoolbook"))
	  strcpy(fontname, "NewCenturySchlbk");
	while (strstr(fontname, " "))
	  strncpy(strstr(fontname, " "), "-", 1);
	fatom = XInternAtom(XtDisplay(w), "WEIGHT_NAME", False);
	for (i = 0; i < fs->n_properties; i++)
	  if (fs->properties[i].name == fatom) {
	    fontinfo = XGetAtomName(XtDisplay(w), fs->properties[i].card32);
	    break;
	  }
	if (fontinfo != NULL && strstr(fontinfo, "Bold"))
	  strcat(fontname, "-Bold");
	if (fontinfo != NULL) {
	  XFree(fontinfo);
	  fontinfo = NULL;
	}
	fatom = XInternAtom(XtDisplay(w), "FULL_NAME", False);
	for (i = 0; i < fs->n_properties; i++)
	  if (fs->properties[i].name == fatom) {
	    fontinfo = XGetAtomName(XtDisplay(w), fs->properties[i].card32);
	    break;
	  }
	if (fontinfo != NULL && strstr(fontinfo, "Italic")) {
	  if (!strstr(fontname, "-Bold"))
	    strcat(fontname, "-");
	  strcat(fontname, "Italic");
	}
	if (fontinfo != NULL && strstr(fontinfo, "Oblique")) {
	  if (!strstr(fontname, "-Bold"))
	    strcat(fontname, "-");
	  strcat(fontname, "Oblique");
	}
	if (!strcmp(fontname, "Times") || !strcmp(fontname, "NewCenturySchlbk"))
	  strcat(fontname, "-Roman");
	if (fontinfo != NULL) {
	  XFree(fontinfo);
	  fontinfo = NULL;
	}
      }
      fatom = XInternAtom(XtDisplay(w), "POINT_SIZE", False);
      for (i = 0; i < fs->n_properties; i++)
	if (fs->properties[i].name == fatom)
	  break;
      if (fatom)
	fontsize = fontsize * fs->properties[i].card32 / 120.;

      if (rotate != 0)
	fprintf(printfid, "%g %g translate\n",
		height_page * 72. - (height_page - width_page) * 72. / 2.,
		(height_page - width_page) * 72. / 2.);
      /* Rotation is around origin, so must move origin for proper plot position */
      fprintf(printfid, "%g rotate\n", rotate);

      if (window_title && strlen(window_title))
	/* If window title, allow space at top of plot for printing window title. */
      {
	float yscale = 1. - fontsize * 2.0 * 1.125 / (height - 1.);

	fprintf(printfid, "/%s findfont\n", fontname);
	fprintf(printfid, "%g scalefont\n", fontsize * 1.5);	/* Specify 13 as fontsize (see note on fontsize) */
	fprintf(printfid, "setfont\n");

	if (rotate != 0) {
	  float y = XtBorderWidth(w) * scale;
	  if (!inp_total_height)
	    y = -(72. * (height_page - 2. * margin) - height) / 2. - 72. * margin;
	  else
	    y = -(72. * (height_page - 2. * margin) - height) / 2. - y * resinc - 72. * margin;

	  fprintf(printfid, "(%s) %g %g centershow\n", window_title,
		  height_limit * 72. / resolution + 72. * 2. * margin,
		  (height_page * 72.) / 2. + (ury - lly) / 2. - 1.125 * fontsize);

	  /* Scale so bottom of plot still at same place, */
	  fprintf(printfid, "%g %g scale\n", 1., yscale);
	  fprintf(printfid, "%g %g translate\n", 0., y * (yscale - 1));
	} else {
	  fprintf(printfid, "(%s) %g %g centershow\n", window_title,
		  width_limit * 72. / resolution + 72. * 2. * margin, ury - 1.125 * fontsize);

	  /* Scale so bottom of plot still at same place, */
	  fprintf(printfid, "0 %g translate\n", lly * (1. - yscale));
	  fprintf(printfid, "%g %g scale\n", 1., yscale);
	}
      }
    }

    fprintf(printfid, "%g %g scale\n", .72 / .75, .72 / .75);

    fprintf(printfid, "%g setlinewidth\n", .75);	/* Set line width so it generates a true 1/75 */
    /* .72/.75 should work, but it doesn't. */
    xoffset = xorigin * scale;
    yoffset = yorigin * scale;

    fprintf(printfid, "/fontsize %g def\n", fontsize * 1.5 / scale);	/* Fontsize definition needed for overlayshow */
    fprintf(printfid, "/%s findfont\n", fontname);
    fprintf(printfid, "%g scalefont\n", fontsize * 1.5 / scale);	/* Specify 13 as fontsize (see note on fontsize) */
    fprintf(printfid, "setfont\n");
  } else {
    fseek(printfid, -10, 1);	/* rewind past showpage command */
  }

  XtTranslateCoords((Widget) w, 0, 0, &xorigin, &yorigin);
  x = (xorigin + XtBorderWidth(w)) * scale - xoffset;
  y = (yorigin + XtBorderWidth(w)) * scale - yoffset;

  if (!inp_total_width)
    x = (resolution * (width_page - 2. * margin) - width) / 2. + resolution * margin;
  else
    x = (resolution * (width_page - 2. * margin) - width) / 2. + x * resinc + resolution * margin;

  if (!inp_total_height)
    y = -(resolution * (height_page - 2. * margin) - height) / 2. - resolution * margin;
  else
    y = -(resolution * (height_page - 2. * margin) - height) / 2. - y * resinc -
	resolution * margin;

  fprintf(printfid, "gsave\n");
  fprintf(printfid, "%g %g translate\n", x / resinc, y / resinc);	/* Center plot */

  /* fudge factor, not sure why this is needed. */

  if (rotate != 0)
    fprintf(printfid, "0 -1 translate\n");	/* Center landscape plot */
  else
    fprintf(printfid, "1 0 translate\n");	/* Center portrait plot */

  swidth = XtWidth(w) * resinc * scale;
  sheight = XtHeight(w) * resinc * scale;

  fprintf(printfid, "gsave\n");
  fprintf(printfid, "%g %g scale\n", 1. / resinc, 1. / resinc);

  xstart = ((!inp_total_width) ? -1 : -(int)(XtBorderWidth(w)) * scale);
  xend = ((!inp_total_width) ? (int)(XtWidth(w) * scale) : XtWidth(w) * scale);
  ystart =
      height_page * 75. -
      ((!inp_total_height) ? (int)(XtHeight(w) * scale) : (XtBorderWidth(w) + XtHeight(w)) * scale);
  yend = height_page * 75. + ((!inp_total_height) ? 1. : (int)(XtBorderWidth(w)) * scale);

  if (inp_total_width && (inp_total_width > XtWidth(w) + 4 || inp_total_height > XtHeight(w) + 4)) {
    float xtest = xorigin * scale;
    float ytest = yorigin * scale;

    fprintf(printfid, "gsave\n");
    fprintf(printfid, "%g %g scale\n", resinc, resinc);
    fprintf(printfid, "%g setlinewidth\n", .75 * 3);	/* Thick border around multi-frame */

    /* The following mess draws finds borders that intersect at the */
    /* corners and draws them in succession to obtain rounded corners. */

    if (xoffset == xtest && intAbs((yoffset / scale + inp_total_height) - (yorigin + XtHeight(w))) > 5) {
      fprintf(printfid, "%g %g moveto\n", xstart, ystart);
      fprintf(printfid, "%g %g lineto\n", xstart, yend);
      if (yoffset == ytest) {
	fprintf(printfid, "%g %g lineto\n", xend, yend);
	if (intAbs((xoffset / scale + inp_total_width) - (xorigin + XtWidth(w))) < 5)
	  fprintf(printfid, "%g %g lineto\n", xend, ystart);
      } else if (intAbs((xoffset / scale + inp_total_width) - (xorigin + XtWidth(w))) < 5) {
	fprintf(printfid, "stroke\n");
	fprintf(printfid, "%g %g moveto\n", xend, yend);
	fprintf(printfid, "%g %g lineto\n", xend, ystart);
      }
      fprintf(printfid, "stroke\n");
    } else if (yoffset == ytest && (xoffset != xtest ||
				    intAbs((yoffset / scale + inp_total_height) -
					(yorigin + XtHeight(w))) > 5)) {
      fprintf(printfid, "%g %g moveto\n", xstart, yend);
      fprintf(printfid, "%g %g lineto\n", xend, yend);
      if (intAbs((xoffset / scale + inp_total_width) - (xorigin + XtWidth(w))) < 5) {
	fprintf(printfid, "%g %g lineto\n", xend, ystart);
	if (intAbs((yoffset / scale + inp_total_height) - (yorigin + XtHeight(w))) < 5)
	  fprintf(printfid, "%g %g lineto\n", xstart, ystart);
      } else if (intAbs((yoffset / scale + inp_total_height) - (yorigin + XtHeight(w))) < 5) {
	fprintf(printfid, "stroke\n");
	fprintf(printfid, "%g %g moveto\n", xstart, ystart);
	fprintf(printfid, "%g %g lineto\n", xend, ystart);
      }
      fprintf(printfid, "stroke\n");
    } else if (intAbs((xoffset / scale + inp_total_width) - (xorigin + XtWidth(w))) < 5) {
      fprintf(printfid, "%g %g moveto\n", xend, yend);
      fprintf(printfid, "%g %g lineto\n", xend, ystart);
      if (intAbs((yoffset / scale + inp_total_height) - (yorigin + XtHeight(w))) < 5)
	fprintf(printfid, "%g %g lineto\n", xstart, ystart);
      if (xoffset == xtest)
	fprintf(printfid, "%g %g lineto\n", xstart, yend);
      fprintf(printfid, "stroke\n");
    } else if (intAbs((yoffset / scale + inp_total_height) - (yorigin + XtHeight(w))) < 5) {
      fprintf(printfid, "%g %g moveto\n", xend, ystart);
      fprintf(printfid, "%g %g lineto\n", xstart, ystart);
      if (xoffset == xtest)
	fprintf(printfid, "%g %g lineto\n", xstart, yend);
      if (yoffset == ytest)
	fprintf(printfid, "%g %g lineto\n", xend, yend);
      fprintf(printfid, "stroke\n");
    }
    fprintf(printfid, "grestore\n");
  }

  if (title && strlen(title))
    /* If title, allow space at bottom of plot for printing title. */
  {
    float yscale = 1. - fontsize * 1.5 * 1.25 * resinc / (sheight - 1.);
    /* Scale so top of plot still at same place, */
    float yoffset = height_page * resolution * (1. - yscale);
    /* but bottom appears raised from original spot. */
    fprintf(printfid, "%g %g translate\n", 0., yoffset);
    fprintf(printfid, "%g %g scale\n", 1., yscale);
  }

  fprintf(printfid, "%g %g scale\n", resinc, resinc);
  fprintf(printfid, "%g %g %g %g dorectangleclip\n", xstart, ystart, xend - xstart, yend - ystart);

  waveformPrint = 1;
  if (waveformCount(w))
    GetPixVals(w, swidth, sheight);
  Plot(w, 1);
  waveformPrint = 0;

  SetScale(0);
  fprintf(printfid, "grestore\n");	/* To clear clipping region */

  if (title && strlen(title)) {
    int length = strlen(title);
    int twidth = XTextWidth(waveformFontStruct(w), title, length);
    float x = max(0, ((int)XtWidth(w) - twidth) / 2);
    float y = (height_page * 75. - (XtHeight(w) - 1) + (.25 * fontsize) / 2.);
    float middle = x + twidth / 2 - XTextWidth(waveformFontStruct(w), title, 1) / 2.;
    int i;
    SetScale(1);
    for (i = 0; i < length; i++) {
      char c[2];
      fprintf(printfid, "%g %g moveto\n", middle + (x - middle) / scale, y);
      c[0] = title[i];
      c[1] = 0;
      x += XTextWidth(waveformFontStruct(w), c, 1);
      fprintf(printfid, "(%s) show\n", c);
    }
    SetScale(0);
  }

  fprintf(printfid, "grestore\n");
  fprintf(printfid, "\nshowpage\n");
}

static void DrawLines(Display * display, Window win, GC gc, XPoint * point, int kp,
		      Dimension pwidth, Dimension pheight)
{
  static int i, imax, imin, ymax, ymin;
  if (!waveformPrint) {
    int kpnew = 0;
    int num_to_left = 0;
    int num_to_right = 0;
    int num_in_middle = 0;
    short lastx;
    short enter_y;
    short max_y;
    short min_y;
    short leave_y;
    for (i = 0; i < kp; i++) {
      if (point[i].x < 0) {
	num_to_right = 0;
	num_in_middle = 0;
	if (num_to_left < 2) {
	  point[kpnew].x = point[i].x;
	  point[kpnew++].y = point[i].y;
	} else {
	  point[kpnew - 1].x = point[i].x;
	  point[kpnew - 1].y = point[i].y;
	}
	num_to_left++;
      } else if (point[i].x > pwidth && num_to_right > 0) {
	num_to_left = 0;
	num_in_middle = 0;
	if (num_to_right < 2) {
	  point[kpnew].x = point[i].x;
	  point[kpnew++].y = point[i].y;
	} else {
	  point[kpnew - 1].x = point[i].x;
	  point[kpnew - 1].y = point[i].y;
	}
	num_to_right++;
      } else {
	num_to_left = 0;
	num_to_right = (point[i].x > pwidth) ? 1 : 0;
	if (num_in_middle < 1) {
	  point[kpnew].x = lastx = point[i].x;
	  point[kpnew++].y = min_y = max_y = enter_y = leave_y = point[i].y;
	} else if (point[i].x == lastx) {
	  leave_y = point[i].y;
	  if (point[i].y < min_y)
	    min_y = point[i].y;
	  else if (point[i].y > max_y)
	    max_y = point[i].y;
	}
	if ((point[i].x != lastx) || (i == kp - 1)) {
	  if (min_y != max_y) {
	    switch (((enter_y == min_y) * 1) |
		    ((enter_y == max_y) * 2) |
		    ((min_y == leave_y) * 4) | ((max_y == leave_y) * 8)) {
	    case 0:
	      point[kpnew].x = lastx;
	      point[kpnew++].y = min_y;
	      point[kpnew].x = lastx;
	      point[kpnew++].y = max_y;
	      point[kpnew].x = lastx;
	      point[kpnew++].y = leave_y;
	      break;
	    case 1:
	    case 4:
	    case 5:
	      point[kpnew].x = lastx;
	      point[kpnew++].y = max_y;
	      point[kpnew].x = lastx;
	      point[kpnew++].y = leave_y;
	      break;
	    case 2:
	    case 8:
	    case 10:
	      point[kpnew].x = lastx;
	      point[kpnew++].y = min_y;
	      point[kpnew].x = lastx;
	      point[kpnew++].y = leave_y;
	      break;
	    case 3:
	    case 7:
	    case 11:
	    case 12:
	    case 13:
	    case 14:
	    case 15:
	      break;
	    case 6:
	    case 9:
	      point[kpnew].x = lastx;
	      point[kpnew++].y = leave_y;
	      break;
	    }
	  }
	  point[kpnew].x = lastx = point[i].x;
	  point[kpnew++].y = min_y = max_y = enter_y = leave_y = point[i].y;
	}
	num_in_middle++;
      }
    }
    XDrawLines(display, win, gc, point, kpnew, CoordModeOrigin);
    return;
  } else {

    int pen_down = 1;
    int plot;
    int lastpoint = 0;
    int np = 0;
    SetScale(0);
    fprintf(printfid, "gsave\n");
    fprintf(printfid, "%g %g scale\n", 1. / resinc, 1. / resinc);
    for (i = 1; i < kp; i++) {
      if (pen_down) {
	plot = (point[lastpoint].x != point[i].x || point[lastpoint].y != point[i].y);
	if (plot) {
	  while (i < kp - 2 && point[i + 1].x == point[i].x && point[i + 1].y == point[i].y)
	    i++;
	  if (point[i + 1].x == point[i].x && point[lastpoint].x == point[i].x) {
	    ymin = ymax = point[lastpoint].y;
	    imin = imax = lastpoint;
	    while (point[i].x == point[i + 1].x && i < kp - 2) {
	      if (ymax < point[i].y) {
		ymax = point[i].y;
		imax = i;
	      }
	      if (ymin > point[i].y) {
		ymin = point[i].y;
		imin = i;
	      }
	      i++;
	    }
	    if (ymin < point[i].y && imin != lastpoint) {
	      fprintf(printfid, "%d %d\n", point[lastpoint].x - point[imin].x,
		      point[imin].y - point[lastpoint].y);
	      np++;
	      lastpoint = imin;
	      if (np == 100) {
		fprintf(printfid, "%d %d %d v\n", np, point[lastpoint].x,
			(int)(height_page * resolution - point[lastpoint].y));
		np = 0;
	      }
	    }
	    if (ymax > point[i].y && imax != lastpoint) {
	      fprintf(printfid, "%d %d \n", point[lastpoint].x - point[imax].x,
		      point[imax].y - point[lastpoint].y);
	      np++;
	      lastpoint = imax;
	      if (np == 100) {
		fprintf(printfid, "%d %d %d v\n", np, point[lastpoint].x,
			(int)(height_page * resolution - point[lastpoint].y));
		np = 0;
	      }
	    }
	  } else if (point[i + 1].y == point[i].y && point[lastpoint].y == point[i].y)
	    plot = !compare_sign(point[i + 1].x - point[i].x, point[i].x - point[lastpoint].x);
	  else
	    plot = (point[i + 1].y == point[i].y || point[lastpoint].y == point[i].y ||
		    (float)(point[i + 1].x - point[i].x) / (float)(point[i + 1].y - point[i].y) !=
		    (float)(point[i].x - point[lastpoint].x) / (float)(point[i].y -
								       point[lastpoint].y));
	}
	if (plot) {
	  fprintf(printfid, "%d %d\n", point[lastpoint].x - point[i].x,
		  point[i].y - point[lastpoint].y);
	  np++;
	  lastpoint = i;
	  if (np == 100) {
	    fprintf(printfid, "%d %d %d v\n", np, point[lastpoint].x,
		(int)(height_page * resolution - point[lastpoint].y));
	    np = 0;
	  }
	}
      }
      if ((point[i].x < 0 && point[i + 1].x < 0) || (point[i].x > width && point[i + 1].x > width)
	  || (point[i].y < 0 && point[i + 1].y < 0) || (point[i].y > height
							&& point[i + 1].y > height))
	/* Skip lines that occur outside of grid.  Algorithm eliminates most, but not all. */
      {
	if (np) {
	  if (lastpoint != i) {
	    fprintf(printfid, "%d %d\n", point[lastpoint].x - point[i].x,
		    point[i].y - point[lastpoint].y);
	    np++;
	  }
	  fprintf(printfid, "%d %d %d v\n", np, point[i].x, (int)(height_page * resolution - point[i].y));
	  np = 0;
	}
	pen_down = 0;
      } else if (!pen_down) {
	fprintf(printfid, "%d %d\n", point[i - 1].x - point[i].x, point[i].y - point[i - 1].y);
	np = 1;
	pen_down = 1;
	lastpoint = i;
      }
    }
    if (np) {
      fprintf(printfid, "%d %d %d v\n", np, point[lastpoint].x,
	(int)(height_page * resolution - point[lastpoint].y));
    }
    fprintf(printfid, "grestore\n");
  }
}

static void DrawRectangles(Display * display, Window win, GC gc, XRectangle * rectangle, int num,
			   Dimension pwidth, Dimension pheight)
{
  static int i;
  if (!waveformPrint) {
    int n = 0;
    for (i = 0; i < num; i++) {
      if (((rectangle[i].x + rectangle[i].width) >= 0) &&
	  (rectangle[i].x <= pwidth) &&
	  ((rectangle[i].y + rectangle[i].height) >= 0) && (rectangle[i].y <= pheight))
	rectangle[n++] = rectangle[i];
    }
    XDrawRectangles(display, win, gc, rectangle, n);
    return;
  } else {
    SetScale(0);
    fprintf(printfid, "gsave\n");
    fprintf(printfid, "%g %g scale\n", 1. / resinc, 1. / resinc);
    for (i = 0; i < num; i++) {
      if (((rectangle[i].x >= 0 && rectangle[i].x < width) ||
	   (rectangle[i].x + rectangle[i].width >= 0
	    && rectangle[i].x + rectangle[i].width < width)) && ((rectangle[i].y < height
								  && rectangle[i].y > 0)
								 || (rectangle[i].y +
								     rectangle[i].height < height
								     && rectangle[i].y +
								     rectangle[i].height > 0)))

	/* Test to see if rectangle points are in the grid.  If not, do not draw the rectangle.
	   Note that theoretically the points could be outside the grid, yet some of it's sides
	   could be in the grid.  However, for our application, this test is good enough. */

	fprintf(printfid, "%d %d %d %d dorectangle\n",
		rectangle[i].x,(int)(height_page * resolution - rectangle[i].y),
                rectangle[i].width,rectangle[i].height);
    }
    fprintf(printfid, "grestore\n");
  }
}

static float *dashes_ps = 0;
static int num_dashes_ps;
static float offset_dashes_ps;
static GC dash_gc;

static void SetDashes(Display * display, GC gc, int offset, char *dashes, int num)
{
  int i;
  if (dashes_ps)
    XtFree((char *)dashes_ps);
  dashes_ps = (float *)XtMalloc(sizeof(float) * num);
  for (i = 0; i < num; i++)
    dashes_ps[i] = dashes[i];
  num_dashes_ps = num;
  offset_dashes_ps = offset;
  XSetDashes(display, gc, offset, dashes, num);
  dash_gc = gc;
}

static void DrawSegments(Display * display, Window win, GC gc, float *crosshairs, int num)
{
  static int i;
  if (!waveformPrint) {
    XSegment *line = (XSegment *) XtMalloc(num * sizeof(XSegment));
    for (i = 0; i < num * 4; i = i + 4) {
      line[i / 4].x1 = crosshairs[i];
      line[i / 4].x2 = crosshairs[i + 1];
      line[i / 4].y1 = crosshairs[i + 2];
      line[i / 4].y2 = crosshairs[i + 3];
    }
    XDrawSegments(display, win, gc, line, num);
    XtFree((char *)line);
    return;
  } else {
    float *dashes = (float *)XtMalloc(num_dashes_ps * sizeof(float));
    float offset_dashes = offset_dashes_ps * resinc;
    fprintf(printfid, "gsave\n");
    fprintf(printfid, "newpath\n");
    for (i = 0; i < num_dashes_ps; i++)
      dashes[i] = dashes_ps[i] * resinc;
    if (gc == dash_gc)
      fprintf(printfid, "[%d %d] %d setdash\n", (int)*(dashes), num_dashes_ps, (int)offset_dashes);
    XtFree((char *)dashes);
    fprintf(printfid, "%g %g scale\n", 1. / resinc, 1. / resinc);
    fprintf(printfid, "%g setlinewidth\n", .25);	/* Width for grid lines */
    for (i = 0; i < num * 4; i = i + 4) {
      fprintf(printfid, "%d %d mv\n", (int)(crosshairs[i] * scale * resinc),
	      (int)(height_page * resolution) - (int)(crosshairs[i + 2] * scale * resinc));
      fprintf(printfid, "%d %d ln\n", (int)(crosshairs[i + 1] * scale * resinc),
	      (int)(height_page * resolution) - (int)(crosshairs[i + 3] * scale * resinc));
      fprintf(printfid, "stroke\n");
    }
    fprintf(printfid, "grestore\n");
    fprintf(printfid, "[] 0 setdash\n");
  }
}

static void DrawString(XmdsWaveformWidget w, Display * display, Window win, GC gc, float x, float y,
		       char *label, int length)
{
  if (!waveformPrint) {
    int sx = x;
    int sy = y;
    XDrawString(display, win, gc, sx, sy, label, length);
    return;
  } else {
    SetScale(1);
    if (x < 0)
      fprintf(printfid, "(%s) %g %g centershow\n", label, -x * 2, 825. - y);
    else {
      fprintf(printfid, "%g %g moveto\n", x, 825. - y);
      fprintf(printfid, "(%s) show\n", label);
    }
  }
}

EXPORT void XmdsWaveformReverse(Widget w, int reverse)
{
  (((XmdsWaveformWidgetClass) w->core.widget_class)->waveform_class.reverse_proc) (w, reverse);
}

static void Reverse(Widget w_in, int reverse)
{
  XmdsWaveformWidget w = (XmdsWaveformWidget) w_in;
  if (reverse != waveformReverse(w) && XtIsRealized((Widget) w)) {
    XCopyArea(XtDisplay(w), waveformDrawable(w), waveformDrawable(w), waveformFlipGC(w), 0, 0,
	      XtWidth(w), XtHeight(w), 0, 0);
    if (waveformPixmap(w))
      XCopyArea(XtDisplay(w), waveformDrawable(w), XtWindow(w), waveformPlotGC(w), 0, 0, XtWidth(w),
		XtHeight(w), 0, 0);
  }
  waveformReverse(w) = reverse;
}

static void Cut(XmdsWaveformWidget w, XButtonEvent * event)
{
  XmAnyCallbackStruct cb = { 0, 0 };
  cb.event = (XEvent *) event;
  XtCallCallbacks((Widget) w, XmdsNcutCallback, &cb);
}

static void Paste(XmdsWaveformWidget w, XButtonEvent * event)
{
  XmAnyCallbackStruct cb = { 0, 0 };
  cb.event = (XEvent *) event;
  XtCallCallbacks((Widget) w, XmdsNpasteCallback, &cb);
}

#ifdef __VAX

#include <starlet.h>
#include <ssdef.h>
#include <descrip.h>

int FixupMissing(unsigned sig_args[], unsigned mech_args[])
{
  unsigned code;
  int FixupFault();
  static struct dsc$descriptor_p fixup = { 4, DSC$K_DTYPE_L, DSC$K_CLASS_P, FixupFault };
  code = sig_args[1];
  if (code == SS$_UNWIND)
    return;
  if (code == SS$_ROPRAND || code == SS$_FLTOVF_F || code == SS$_FLTDIV_F || code == SS$_FLTUND_F)
    return lib$decode_fault(sig_args, mech_args, &fixup, 0, 0);
  sig_args[0] -= 2;
  sys$putmsg(sig_args, 0, 0, 0);
  mech_args[3] = code;
  sys$unwind(0, 0);
}

#include psldef
union real {
  long l[4];
};
struct dcf {
  unsigned dcfacc:3;
  unsigned dcftyp:5;
  unsigned:24;
};
#include <libdcfdef.h>

int FixupFault(unsigned *opcode,
	       unsigned *ipc,
	       unsigned *psl,
	       unsigned regs[16],
	       unsigned *opcount,
	       struct dcf optypes[],
	       unsigned short *readops[],
	       union real *writeops[],
	       unsigned sig_args[],
	       int sig_routine(), int context, int userarg, unsigned original[16])
{
  int n;
  unsigned found = 0;
  for (n = *opcount; --n >= 0;) {
    if (writeops[n] != 0) {
      *psl |= (PSL$M_N | PSL$M_Z | PSL$M_V | PSL$M_C);
      switch (optypes[n].dcftyp) {
      case LIB$K_DCFTYP_L:
	(*writeops[n]).l[0] = missing;
	found = 1;
	break;
      case LIB$K_DCFTYP_H:
	(*writeops[n]).l[3] = 0;
	(*writeops[n]).l[2] = 0;
      case LIB$K_DCFTYP_G:
      case LIB$K_DCFTYP_D:
	(*writeops[n]).l[1] = 0;
      case LIB$K_DCFTYP_F:
	(*writeops[n]).l[0] = found = 0x8000;
	break;
      }
    }
  }
  if (found)
    return SS$_CONTINUE;
  /****************************
    Look for a bad input operand.
  ****************************/
  for (n = *opcount; --n >= 0;) {
    if (readops[n]
	&& (optypes[n].dcftyp >= LIB$K_DCFTYP_F)
	&& (*readops[n] == 0x8000))
      break;
  }

  /*****************************************
    Set all bits for compare/test of reserved.
  *****************************************/
  if (n >= 0) {
    *psl |= (PSL$M_N | PSL$M_Z | PSL$M_V | PSL$M_C);
    return SS$_CONTINUE;
  }

  return SS$_RESIGNAL;
}
#endif

static Boolean UpdateLimits(XmdsWaveformWidget old, float *xmin, float *xmax, float *ymin,
			    float *ymax, XmdsWaveformWidget new)
{
  Boolean changed = UpdateLimit(xMin(old), xmin, &xMin(new)) |
      UpdateLimit(xMax(old), xmax, &xMax(new)) |
      UpdateLimit(yMin(old), ymin, &yMin(new)) | UpdateLimit(yMax(old), ymax, &yMax(new));
  if (waveformCount(new)) {
    if (!(xMin(new) && xMax(new))) {
      float _xmin;
      float _xmax;
      AutoScale(waveformCount(new), xValue(new), &_xmin, &_xmax, yValue(new), yMin(new), yMax(new));
      if (!xMin(new)) {
	xMin(new) = XtNew(float);
	*xMin(new) = _xmin;
      }
      if (!xMax(new)) {
	xMax(new) = XtNew(float);
	*xMax(new) = _xmax;
      }
      changed = TRUE;
    }
    if (!(yMin(new) && yMax(new))) {
      float _ymin;
      float _ymax;
      AutoScale(waveformCount(new), yValue(new), &_ymin, &_ymax, xValue(new), xMin(new), xMax(new));
      if (!yMin(new)) {
	yMin(new) = XtNew(float);
	*yMin(new) = _ymin;
      }
      if (!yMax(new)) {
	yMax(new) = XtNew(float);
	*yMax(new) = _ymax;
      }
      changed = TRUE;
    }
    SetResolutions(new);
  }
  return changed;
}

static Boolean UpdateLimit(float *old, float *req, float **new)
{
  if (old == req)
    return FALSE;
  else if ((((char *)req - (char *)0) & 0xffffffff) == 0xffffffff) {
    *new = old;
    return FALSE;
  } else {
    if (old)
      XtFree((char *)old);
    if (req) {
      *new = XtNew(float);
      **new = min(huge, max(-huge, *req));
    } else
      *new = req;
  }
  return TRUE;
}

static void ForceUpdate(XmdsWaveformWidget w)
{
  XEvent event;
  while (XCheckWindowEvent(XtDisplay(w), XtWindow(w), ExposureMask, &event)) ;
  if (XtIsRealized((Widget) w) && XtIsManaged((Widget) w))
    XClearArea(XtDisplay(w), XtWindow(w), 0, 0, 0, 0, TRUE);
}
