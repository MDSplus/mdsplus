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
#include <stdlib.h>
#include <string.h>
#include <mdsplus/mdsconfig.h>
#include <stdio.h>
#include <mdsdescrip.h>
#include <Mrm/MrmPublic.h>
#include <Xm/XmP.h>
#include <Xm/PushB.h>
#include <Xmds/XmdsWavedraw.h>
#include <xmdsshr.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <Xm/Text.h>
#ifdef strlcpy
#undef strlcpy
#endif
#ifdef strlcat
#undef strlcat
#endif
#include "export.h"

enum callback_id { CB_UNKNOWN, CB_AUTOSCALE, CB_CROSSHAIRS, CB_LIMITS, CB_MOVE, CB_STRETCH,
      CB_SET_AT_LIMITS, CB_ADD_POINT,
  CB_DELETE_POINT, CB_PASTE, CB_FIT, CB_USER
};

typedef struct _EventInfo {
  int wave_idx;
  int reason;
  enum callback_id callback_id;
  int moved_idx;
  float values[4];
  int state;
  int button;
  Widget w;
  int eventid;
  struct _EventInfo *next;
} EventInfo;

typedef struct _IdlEventRec {
  char fill[24];
  EventInfo *event;
} IdlEventRec;

extern float csval_();
extern void csakm_();
extern void mskrv1_();
extern void mskrv2_();

static Atom XA_TARGETS;
static Atom XA_X_AXIS;
static Atom XA_Y_AXIS;

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))
#define interp(x1,y1,x2,y2,x) (((y2)-(y1))/((x2)-(x1)) * (x) + (((y1)*(x2))-((y2)*(x1)))/((x2)-(x1)))

static void Align(Widget w, int stub, XmdsWaveformLimitsCBStruct * cb);
static void Cut(Widget w, int stub, XmAnyCallbackStruct * callback_struct);
static void Setup(Widget w, int stub, XButtonEvent * event, Boolean * continue_to_dispatch);
static void /*XtActionProc */ MoveVerticalPane(Widget w, XButtonEvent * event, String * params,
					       Cardinal * num_params);
static void /*XtActionProc */ EqualPanes(Widget w, XEvent * event, String * string,
					 Cardinal * num_strings);
static Widget FindWave(Widget w, XButtonEvent * event);
static Widget TopWidget(Widget w);
static void Autoscale(Widget w, int stub, XmPushButtonCallbackStruct * cb);
static void Crosshairs(Widget w, int stub, XmdsWaveformCrosshairsCBStruct * cb);
static void ApplyCustomizePrint(Widget w, int tag, XmAnyCallbackStruct * cb);
static void ResetCustomizePrint(Widget w, int tag, XmAnyCallbackStruct * cb);
static void Print(Widget w, int tag, XmdsWavedrawValueCBStruct * cb);
static void PrintAll(Widget w, int tag, XmAnyCallbackStruct * cb);
static void Limits(Widget w, int stub, XmdsWaveformLimitsCBStruct * cb);
static EventInfo *NewEvent(Widget w, unsigned long stub, XEvent * event, int reason,
			   enum callback_id callback_id);
static void Fit(Widget w, int stub, XmdsWavedrawFitCBStruct * cb);
static void Paste(Widget w, int stub, XmAnyCallbackStruct * cb);
static void Move(Widget w, int stub, XmdsWavedrawValueCBStruct * v);
static void SetAtLimits(Widget w, int stub, XmPushButtonCallbackStruct * cb);
static void AddPoint(Widget w, int stub, XmdsWavedrawValueCBStruct * cb);
static void DeletePoint(Widget w, int stub, XmdsWavedrawValueCBStruct * cb);
Boolean ReplaceString(String * old, String new, Boolean free);

//static XtAppContext app_context = 0;
//static Display *display = 0;
static Widget setup_wave = 0;
static Widget SelectedWidget = 0;
static Boolean Initialized = 0;
static Widget CustomizePrintWidget = 0;
//static Window BusyWindow;
//static int BusyLevel = 0;
static String PrintFile = "WVEDIT.PS";

static void Init(XtAppContext app_context)
{
  static XtActionsRec actions[] = { {"MoveVerticalPane", (XtActionProc) MoveVerticalPane},
  {"EqualPanes", (XtActionProc) EqualPanes}
  };
  MrmInitialize();
  XmdsInitialize();
  XtToolkitInitialize();
  XtAppAddActions(app_context, actions, XtNumber(actions));
  Initialized = 1;
}

static char *IndexedName(char *name_in, int idx)
{
  static char name[20];
  int i;
  for (i = 0; name_in[i]; i++)
    name[i] = name_in[i];
  name[i] = '0' + idx;
  name[i + 1] = 0;
  return name;
}

static Widget PaneIdxToWidget(Widget w, int idx)
{
  return XtNameToWidget(w, IndexedName("*pane", idx));
}

static Widget SashIdxToWidget(Widget w, int idx)
{
  return XtNameToWidget(w, IndexedName("*sash", idx));
}

static Widget WaveIdxToWidget(Widget w, int idx)
{
  return XtNameToWidget(w, IndexedName("*wave", idx));
}

static void Align(Widget w, int stub __attribute__ ((unused)), XmdsWaveformLimitsCBStruct * cb)
{
  Widget plots = XtNameToWidget(TopWidget(w), "*cw_wvedit");
  int i;
  for (i = 0; 1; i++) {
    Widget pane = PaneIdxToWidget(plots, i);
    if (pane) {
      int i;
      for (i = 0; 1; i++) {
	Widget wave = WaveIdxToWidget(pane, i);
	if (wave) {
	  if (wave != w)
	    XtVaSetValues(wave, XmdsNxMin, cb->xminval, XmdsNxMax, cb->xmaxval, NULL);
	} else
	  break;
      }
    } else
      break;
  }
}

static void UserButton(Widget w, int stub, XmPushButtonCallbackStruct * cb)
{
  Widget wave = setup_wave;
  if (wave) {
    EventInfo *e = NewEvent(wave, stub, cb->event, 19, CB_USER);
    char *string = 0;
    XtVaGetValues(w, XmNuserData, &string, NULL);
    memcpy((char *)e->values, string, min(sizeof(e->values), strlen(string) + 1));
  }
}

static void cw_wvedit_size_func(IDL_ULONG stub, int width, int height)
{
  char *stub_rec;
  unsigned long t_id, b_id;

  IDL_WidgetStubLock(TRUE);

  stub_rec = IDL_WidgetStubLookup(stub);
  if (stub_rec) {
    IDL_WidgetGetStubIds(stub_rec, &t_id, &b_id);
    XtVaSetValues((Widget) b_id, XmNwidth, width, XmNheight, height, NULL);
  }

  IDL_WidgetStubLock(FALSE);
}

EXPORT int CW_WVEDIT(unsigned long *parent_id, unsigned long *stub_id, int *cols, int *rows,
	      Widget * plots, Widget * wavedraw_w, struct descriptor *extra_buttons, int num_extra)
{
  short class;
  static String hierarchy_name[] = { "cw_wvedit.uid" };
  MrmHierarchy hierarchy;
  static MrmRegisterArg arglist[] = { {"WidgetRec", 0},
  {"Autoscale", (char *)Autoscale},
  {"SetAtLimits", (char *)SetAtLimits},
  {"Crosshairs", (char *)Crosshairs},
  {"Align", (char *)Align},
  {"MovePoint", (char *)Move},
  {"AddPoint", (char *)AddPoint},
  {"DeletePoint", (char *)DeletePoint},
  {"Limits", (char *)Limits},
  {"Fit", (char *)Fit},
  {"Paste", (char *)Paste},
  {"Cut", (char *)Cut},
  {"Print", (char *)Print},
  {"PrintAll", (char *)PrintAll},
  {"CustomizePrintWidget", (char *)&CustomizePrintWidget},
  {"ApplyCustomizePrint", (char *)ApplyCustomizePrint},
  {"ResetCustomizePrint", (char *)ResetCustomizePrint}
  };
  int i;
  int j;
  Widget sash = 0;
  Widget pw;
  Widget parent_w;
  //  Widget stub_w;
  Widget t_id;
  int widx = 0;
  //  int id;
  char *parent_rec;
  char *stub_rec;
  int numchildren;
  Widget *child;
  if (*parent_id == 0)
    return 1;
  IDL_WidgetStubLock(TRUE);
  if ((parent_rec = IDL_WidgetStubLookup(*parent_id))
      && (stub_rec = IDL_WidgetStubLookup(*stub_id))) {
    IDL_WidgetGetStubIds(parent_rec, (unsigned long *)&t_id, (unsigned long *)&parent_w);
    if (Initialized == 0)
      Init(XtWidgetToApplicationContext(parent_w));
    if (MrmOpenHierarchy(1, hierarchy_name, 0, &hierarchy) != MrmSUCCESS)
      return 0;
    MrmFetchWidget(hierarchy, "cw_wvedit", parent_w, plots, &class);
    IDL_WidgetSetStubIds(stub_rec, (unsigned long)*plots, (unsigned long)*plots);
    IDL_WidgetStubSetSizeFunc(stub_rec, cw_wvedit_size_func);
    arglist[0].value = (char *)*stub_id;
    MrmRegisterNamesInHierarchy(hierarchy, arglist, XtNumber(arglist));
    MrmFetchWidget(hierarchy, "waveform_popup", *plots, &pw, &class);
    if (num_extra > 0) {
      for (i = 0; i < num_extra; i++) {
	Widget b;
	static XtCallbackRec user_button_callback_list[] =
	    { {(XtCallbackProc) UserButton, (XtPointer) 0}, {0, 0} };
	static Arg args[] = { {XmNlabelString, 0},
	{XmNuserData, 0},
	{XmNactivateCallback, (long)user_button_callback_list}
	};
	char *tmp;
	user_button_callback_list[0].closure = (XtPointer) * stub_id;
	tmp = XtMalloc(extra_buttons->length + 1);
	memcpy(tmp, extra_buttons->pointer + i * extra_buttons->length, extra_buttons->length);
	tmp[extra_buttons->length] = 0;
	args[0].value = (long)XmStringCreateSimple(tmp);
	args[1].value = (long)tmp;
	b = XmCreatePushButton(pw, "", args, XtNumber(args));
	XtManageChild(b);
	XmStringFree((XmString) args[0].value);
      }
    }
    MrmFetchWidget(hierarchy, "customize_print_db", *plots, &pw, &class);
    MrmFetchWidget(hierarchy, "pane_separator", *plots, &pw, &class);
    XtAddEventHandler(*plots, ButtonPressMask, False, (XtEventHandler) Setup, 0);
    for (i = 0; i < (*cols - 1); i++) {
      Arg arglist[] = { {XmNleftPosition, 0}
      };
      arglist[0].value = (int)(((i + 1) * 1000.) / *cols);
      MrmFetchWidgetOverride(hierarchy, "sash", *plots, IndexedName("sash", i), arglist,
			     XtNumber(arglist), &sash, &class);
    }
    for (i = 0; i < *cols; i++) {
      Widget pane;
      Widget lsash = SashIdxToWidget(*plots, i - 1);
      Widget rsash = SashIdxToWidget(*plots, i);
      static Arg args1[] = { {XmNleftAttachment, XmATTACH_FORM},
      {XmNrightAttachment, XmATTACH_FORM}
      };
      Arg args2[] = { {XmNleftAttachment, XmATTACH_FORM},
      {XmNrightAttachment, XmATTACH_WIDGET},
      {XmNrightWidget, 0},
      {XmNrightOffset, -3}
      };
      Arg args3[] = { {XmNleftAttachment, XmATTACH_WIDGET},
      {XmNleftWidget, 0},
      {XmNleftOffset, -3},
      {XmNrightAttachment, XmATTACH_WIDGET},
      {XmNrightWidget, 0},
      {XmNrightOffset, -3}
      };
      Arg args4[] = { {XmNleftAttachment, XmATTACH_WIDGET},
      {XmNleftWidget, 0},
      {XmNleftOffset, -3},
      {XmNrightAttachment, XmATTACH_FORM}
      };
      Arg *arglist = lsash && rsash ? args3 : (lsash ? args4 : (rsash ? args2 : args1));
      int numargs = lsash
	  && rsash ? XtNumber(args3) : (lsash ? XtNumber(args4)
					: (rsash ? XtNumber(args2) : XtNumber(args1)));
      Widget waveform = 0;
      Widget first_waveform = 0;
      args2[2].value = args3[4].value = (long)rsash;
      args3[1].value = args4[1].value = (long)lsash;
      MrmFetchWidgetOverride(hierarchy, "pane", *plots, IndexedName("pane", i), arglist, numargs,
			     &pane, &class);
      for (j = 0; j < rows[i]; j++) {
	Arg arglist[] = { {XmdsNpanWith, 0}
	};
	arglist[0].value = (long)waveform;
	MrmFetchWidgetOverride(hierarchy, "wave", pane, IndexedName("wave", j), arglist,
			       XtNumber(arglist), &waveform, &class);
	wavedraw_w[widx++] = waveform;
	if (!j)
	  first_waveform = waveform;
      }
      XtVaSetValues(first_waveform, XmdsNpanWith, waveform, NULL);
      XtVaGetValues(pane, XmNnumChildren, &numchildren, XmNchildren, &child, NULL);
      for (j = 0; j < numchildren; j++) {
	String name = XtName(child[j]);
	if (name && !strcmp(name, "Sash")) {
	  XtTranslations translations = XtParseTranslationTable("Shift<Btn1Up>:EqualPanes(H)");
	  XtAugmentTranslations(child[j], translations);
	}
      }
      XtManageChildren(child, numchildren);
    }
    MrmCloseHierarchy(hierarchy);
    XtVaGetValues(*plots, XmNnumChildren, &numchildren, XmNchildren, &child, NULL);
    XtManageChildren(child, numchildren);
    XtManageChild(*plots);
    for (i = 0; i < *cols; i++) {
      int r;
      Widget pane = PaneIdxToWidget(*plots, i);
      int height;
      height = (XtHeight(pane) - 2 * rows[i]) / rows[i];
      XtUnmanageChild(pane);
      for (r = 0; r < rows[i]; r++)
	XtVaSetValues(WaveIdxToWidget(pane, r), XmNheight, height, NULL);
      XtManageChild(pane);
    }
  }
  IDL_WidgetStubLock(FALSE);
  return *stub_id;
}

static Widget TopWidget(Widget w)
{
  Widget top;
  for (top = w; XtParent(top); top = XtParent(top)) ;
  return top;
}

static void Setup(Widget w, int stub __attribute__ ((unused)), XButtonEvent * event,
		  Boolean * continue_to_dispatch __attribute__ ((unused)))
{
  if (event->button == Button3) {
    Widget popup = XtNameToWidget(TopWidget(w), "*waveform_popup");
    setup_wave = FindWave(w, event);
    XmdsPopupMenuPosition(popup, event);
    XtManageChild(popup);
  }
}

static void /*XtActionProc */ MoveVerticalPane(Widget w, XButtonEvent * event,
					       String * params __attribute__ ((unused)),
					       Cardinal * num_params __attribute__ ((unused)))
{
  Position main_x_root;
  Position main_y_root;
  int min_offset = 0;
  int max_offset = 10000000;
  //  int i;
  Widget separator = XtNameToWidget(TopWidget(w), "*pane_separator");
  XtTranslateCoords(XtNameToWidget(TopWidget(w), "*cw_wvedit"), 0, 0, &main_x_root, &main_y_root);
  if (event->type == ButtonPress) {
    separator->core.widget_class->core_class.compress_motion = 1;
    XtManageChild(separator);
  }
  XtVaSetValues(separator, XmNleftOffset,
		min(max_offset, max(min_offset, event->x_root - main_x_root)), NULL);
  if (event->type == ButtonRelease) {
    unsigned short position;
    XtVaGetValues(separator, XmNx, &position, NULL);
    position = position * 1000. / XtWidth(XtNameToWidget(TopWidget(w), "*cw_wvedit")) + .49999;
    XtVaSetValues(w, XmNleftPosition, position, NULL);
    XtUnmanageChild(separator);
    separator->core.widget_class->core_class.compress_motion = 0;
  }
}

static void /*XtActionProc */ EqualPanes(Widget w, XEvent * event __attribute__ ((unused)),
					 String * string, Cardinal * num_strings)
{
  Widget plots = XtNameToWidget(TopWidget(w), "*cw_wvedit");
  if ((*num_strings == 1) && (string[0][0] == 'V')) {
    int c;
    int cols;
    for (cols = 1; SashIdxToWidget(plots, cols - 1); cols++) ;
    for (c = 0; c < cols - 1; c++)
      XtVaSetValues(SashIdxToWidget(plots, c), XmNleftPosition, (int)((c + 1) * 1000 / cols), NULL);
  } else if ((*num_strings == 1) && (string[0][0] == 'H')) {
    int r;
    Widget pane = XtParent(w);
    int height;
    int rows;
    for (rows = 0; WaveIdxToWidget(pane, rows); rows++) ;
    height = (XtHeight(pane) - 2 * rows) / rows;
    XtUnmanageChild(pane);
    for (r = 0; r < rows; r++)
      XtVaSetValues(WaveIdxToWidget(pane, r), XmNheight, height, NULL);
    XtManageChild(pane);
  }
  return;
}

static void Autoscale(Widget w __attribute__ ((unused)), int stub, XmPushButtonCallbackStruct * cb)
{
  Widget wave = setup_wave;
  if (wave) {
    EventInfo *e = NewEvent(wave, stub, cb->event, XmdsCRZoomOut, CB_AUTOSCALE);
    float *xminval;
    float *xmaxval;
    float *yminval;
    float *ymaxval;
    XtVaSetValues(wave, XmdsNxMin, NULL, XmdsNxMax, NULL, XmdsNyMin, NULL, XmdsNyMax, NULL, NULL);
    XtVaGetValues(wave, XmdsNxMin, &xminval, XmdsNxMax, &xmaxval, XmdsNyMin, &yminval, XmdsNyMax,
		  &ymaxval, NULL);
    e->values[0] = *xminval;
    e->values[1] = *xmaxval;
    e->values[2] = *yminval;
    e->values[3] = *ymaxval;
  }
}

static Widget FindWave(Widget w, XButtonEvent * event)
{
  Widget plots = XtNameToWidget(TopWidget(w), "*cw_wvedit");
  int i;
  for (i = 0; 1; i++) {
    Widget pane = PaneIdxToWidget(plots, i);
    if (pane) {
      int i;
      for (i = 0; 1; i++) {
	Widget wave = WaveIdxToWidget(pane, i);
	if (wave) {
	  Position x;
	  Position y;
	  XtTranslateCoords(wave, 0, 0, &x, &y);
	  if (event->x_root >= x &&
	      event->x_root <= (x + XtWidth(wave)) &&
	      event->y_root >= y && event->y_root <= (y + XtHeight(wave)))
	    return wave;
	} else
	  break;
      }
    } else
      break;
  }
  return 0;
}

static void Crosshairs(Widget w, int stub, XmdsWaveformCrosshairsCBStruct * cb)
{
  //Widget top = TopWidget(w);
  //  Widget plots = XtNameToWidget(top, "*cw_wvedit");
  EventInfo *e = NewEvent(w, stub, cb->event, cb->reason, CB_CROSSHAIRS);
  e->values[0] = cb->x;
  e->values[1] = cb->y;
}

static void Limits(Widget w, int stub, XmdsWaveformLimitsCBStruct * cb)
{
  EventInfo *e = NewEvent(w, stub, cb->event, cb->reason, CB_LIMITS);
  e->values[0] = *cb->xminval;
  e->values[1] = *cb->xmaxval;
  e->values[2] = *cb->yminval;
  e->values[3] = *cb->ymaxval;
}

static void /*XtSelectionCallbackProc */ PasteComplete(Widget w, int stub, Atom * selection __attribute__ ((unused)),
						       Atom * type,
						       XtPointer value, unsigned long *length,
						       int *format)
{
  static float *x;
  if (*type == XA_X_AXIS)
    x = (float *)value;
  else if (*type == XA_Y_AXIS) {
    int num = (int)(*length * ((float)*format) / 32);
    int i;
    //    float *y = (float *)value;
    if (x) {
      //      EventInfo *e;
      Boolean *knots = (Boolean *) XtMalloc(num * sizeof(Boolean));
      for (i = 0; i < num; i++)
	knots[i] = (Boolean) 1;
      //        memset(knots,1,num * sizeof(Boolean));
      XtVaSetValues(w, XmdsNcount, num, XmdsNxValue, x, XmdsNyValue, value, XmdsNselections, knots,
		    XmdsNpenDown, knots, XmdsNxMax, NULL, XmdsNxMin, NULL, XmdsNyMax, NULL,
		    XmdsNyMin, NULL, NULL);
      XtFree((String) knots);
      XtFree((String) x);
      x = 0;
      XtFree((String) value);
      NewEvent(w, stub, 0, 0, CB_PASTE);
      NewEvent(w, stub, 0, 18, CB_FIT);
    }
  } else if (*type == XA_STRING) {
    XtVaSetValues(w, XmdsNtitle, value, NULL);
    XtFree((String) value);
  } else if (*type == XA_TARGETS) {
    unsigned long i;
    Atom *values = value;
    Boolean supports_data_paste = 0;
    Boolean supports_string_paste = 0;
    for (i = 0; i < *length; i++)
      if (values[i] == XA_X_AXIS)
	supports_data_paste = 1;
      else if (values[i] == XA_STRING)
	supports_string_paste = 1;
    if (supports_data_paste) {
      //      Atom targets[2];
      //targets[0] = XA_X_AXIS;
      //targets[1] = XA_Y_AXIS;
/*
        XtGetSelectionValues(w, XA_PRIMARY, targets, XtNumber(targets), (XtSelectionCallbackProc)PasteComplete, 
                                              (XtPointer)stub, XtLastTimestampProcessed(XtDisplay(w)));
*/
      XtGetSelectionValue(w, XA_PRIMARY, XA_X_AXIS, (XtSelectionCallbackProc) PasteComplete,
			  (XtPointer) ((char *)0+stub), XtLastTimestampProcessed(XtDisplay(w)));
      XtGetSelectionValue(w, XA_PRIMARY, XA_Y_AXIS, (XtSelectionCallbackProc) PasteComplete,
			  (XtPointer) ((char *)0+stub), XtLastTimestampProcessed(XtDisplay(w)));
    } else if (supports_string_paste)
      XtGetSelectionValue(w, XA_PRIMARY, XA_STRING, (XtSelectionCallbackProc) PasteComplete,
			  (XtPointer) ((char *)0+stub), XtLastTimestampProcessed(XtDisplay(w)));
    XtFree((String) values);
  } else if (value)
    XtFree((String) value);
}

static void Paste(Widget w, int stub, XmAnyCallbackStruct * cb __attribute__ ((unused)))
{
  if (XA_X_AXIS == 0) {
    XA_X_AXIS = XInternAtom(XtDisplay(w), "DWSCOPE_X_AXIS", 0);
    XA_Y_AXIS = XInternAtom(XtDisplay(w), "DWSCOPE_Y_AXIS", 0);
    XA_TARGETS = XInternAtom(XtDisplay(w), "TARGETS", 0);
  }
  XtGetSelectionValue(w, XA_PRIMARY, XA_TARGETS, (XtSelectionCallbackProc) PasteComplete,
		      (XtPointer) ((char *)0+stub), XtLastTimestampProcessed(XtDisplay(w)));
}

static void LoseSelection(Widget w, Atom * selection __attribute__ ((unused)))
{
  XmdsWaveformReverse(w, 0);
  SelectedWidget = 0;
}

static Boolean ConvertSelection(Widget w, Atom * selection  __attribute__ ((unused)), Atom * target, Atom * type,
				XtPointer * value, unsigned long *length, int *format)
{
  int status = 0;
  if (XA_X_AXIS == 0) {
    XA_X_AXIS = XInternAtom(XtDisplay(w), "DWSCOPE_X_AXIS", 0);
    XA_Y_AXIS = XInternAtom(XtDisplay(w), "DWSCOPE_Y_AXIS", 0);
    XA_TARGETS = XInternAtom(XtDisplay(w), "TARGETS", 0);
  }
  if (*target == XA_X_AXIS) {
    float *x;
    int count = 0;
    XtVaGetValues(w, XmdsNxValue, &x, XmdsNcount, &count, NULL);
    if (x && count) {
      *type = *target;
      *value = (char *)memcpy(XtMalloc(count * sizeof(float)), x, count * sizeof(float));
      *length = count * sizeof(float);
      *format = 8;
      status = 1;
    }
  } else if (*target == XA_Y_AXIS) {
    float *y;
    int count = 0;
    XtVaGetValues(w, XmdsNyValue, &y, XmdsNcount, &count, NULL);
    if (y && count) {
      *type = *target;
      *value = (char *)memcpy(XtMalloc(count * sizeof(float)), y, count * sizeof(float));
      *length = count * sizeof(float);
      *format = 8;
      status = 1;
    }
  } else if (*target == XA_TARGETS) {
    *type = *target;
    *value = (char *)XtMalloc(sizeof(Atom) * 2);
    ((Atom *) * value)[0] = XA_X_AXIS;
    ((Atom *) * value)[1] = XA_Y_AXIS;
    *length = 2;
    *format = 32;
    status = 1;
  }
  return status;
}

static void Cut(Widget w, int stub __attribute__ ((unused)),
		XmAnyCallbackStruct * callback_struct __attribute__ ((unused)))
{
  if (SelectedWidget == w)
    XtDisownSelection(SelectedWidget, XA_PRIMARY, XtLastTimestampProcessed(XtDisplay(w)));
  else if (XtOwnSelection
	   (w, XA_PRIMARY, XtLastTimestampProcessed(XtDisplay(w)),
	    (XtConvertSelectionProc) ConvertSelection, LoseSelection, NULL)) {
    SelectedWidget = w;
    XmdsWaveformReverse(w, 1);
  }
}

static void Fit(Widget w, int stub, XmdsWavedrawFitCBStruct * cb)
{
  Boolean xincreasing;
  Boolean yincreasing;
  Boolean closed;
  float *lowx, *lowy, *highx, *highy;
  float xmin, ymin, xmax, ymax;
  float *spline_x = (float *)XtMalloc(cb->count * sizeof(float));
  float *spline_y = (float *)XtMalloc(cb->count * sizeof(float));
  int *knotidx = (int *)XtMalloc(cb->count * sizeof(int));
  int end_point = -1;
  int point = 0;
  int spline_points = 0;
  int i;
  size_t thirty_two_k = 32768;
  float zero = 0.0;
  int three = 3;
  NewEvent(w, stub, cb->event, cb->reason, CB_FIT);
  XtVaGetValues(w, XmdsNxIncreasing, &xincreasing, XmdsNyIncreasing, &yincreasing,
		XmdsNlowX, &lowx, XmdsNhighX, &highx, XmdsNlowY, &lowy, XmdsNhighY, &highy,
		XmdsNclosed, &closed, NULL);
  xmin = lowx ? *lowx : -1E+37;
  xmax = highx ? *highx : 1E+37;
  ymin = lowy ? *lowy : -1E+37;
  ymax = highy ? *highy : 1E+37;
  /*
     lib$establish(lib$sig_to_ret);
   */
  while (point < cb->count) {
    spline_points = 0;
    for (point = end_point + 1; point < cb->count && cb->pen_down[point] != -1; point++) {
      if (cb->selected[point]) {
	spline_x[spline_points] = cb->x[point];
	spline_y[spline_points] = cb->y[point];
	knotidx[spline_points++] = point;
      }
    }
    end_point = point;
    if (point < cb->count) {
      memcpy(&cb->x[point], &thirty_two_k, 4);
      memcpy(&cb->y[point], &thirty_two_k, 4);
    }
    if (spline_points < cb->count) {
      if (spline_points > 2) {
	if (xincreasing) {
	  float *fbreak = (float *)XtMalloc(spline_points * sizeof(float));
	  float *cscoef = (float *)XtMalloc(spline_points * 4 * sizeof(float));
	  int breaks = spline_points - 1;
	  csakm_(&spline_points, spline_x, spline_y, fbreak, cscoef);
	  for (i = knotidx[0] + 1; i < knotidx[spline_points - 1]; i++)
	    cb->y[i] = max(ymin, min(ymax, csval_(&cb->x[i], &breaks, fbreak, cscoef)));
	  XtFree((String) fbreak);
	  XtFree((String) cscoef);
	} else if (yincreasing) {
	  float *fbreak = (float *)XtMalloc(spline_points * sizeof(float));
	  float *cscoef = (float *)XtMalloc(spline_points * 4 * sizeof(float));
	  int breaks = spline_points - 1;
	  csakm_(&spline_points, spline_y, spline_x, fbreak, cscoef);
	  for (i = knotidx[0] + 1; i < knotidx[spline_points - 1]; i++)
	    cb->x[i] = max(xmin, min(xmax, csval_(&cb->y[i], &breaks, fbreak, cscoef)));
	  XtFree((String) fbreak);
	  XtFree((String) cscoef);
	} else {
	  int knotid = 1;
	  //	  int first_knot = 0;
	  if (closed) {
	    int offset;
	    spline_x = (float *)XtRealloc((String) spline_x, spline_points * 2 * sizeof(float));
	    spline_y = (float *)XtRealloc((String) spline_y, spline_points * 2 * sizeof(float));
	    knotidx = (int *)XtRealloc((String) knotidx, spline_points * 2 * sizeof(int));
	    knotid = spline_points / 2;
	    memcpy(&spline_x[knotid], &spline_x[0], spline_points * sizeof(float));
	    memcpy(&spline_y[knotid], &spline_y[0], spline_points * sizeof(float));
	    memcpy(&knotidx[knotid], &knotidx[0], spline_points * sizeof(float));
	    offset = &knotidx[knotid] - &knotidx[0];
	    memcpy(&spline_x[0], &spline_x[knotid + spline_points] - offset,
		   offset * sizeof(float));
	    memcpy(&spline_y[0], &spline_y[knotid + spline_points] - offset,
		   offset * sizeof(float));
	    offset = &knotidx[spline_points * 2] - &knotidx[knotid + spline_points];
	    memcpy(&spline_x[knotid + spline_points], &spline_x[knotid], offset * sizeof(float));
	    memcpy(&spline_y[knotid + spline_points], &spline_y[knotid], offset * sizeof(float));
	    knotidx[knotid - 1] = 0;
	    knotidx[knotid + spline_points] = end_point;
	    spline_points = 2 * spline_points;
	  }
	  {
	    float *xp = (float *)XtMalloc(spline_points * sizeof(float));
	    float *yp = (float *)XtMalloc(spline_points * sizeof(float));
	    float *temp = (float *)XtMalloc(spline_points * sizeof(float));
	    float *s = (float *)XtMalloc(spline_points * sizeof(float));
	    int i;
	    int ics = 0;
	    mskrv1_(&spline_points, spline_x, spline_y, &zero, &zero, xp, yp, temp, s, &zero,
		    &three);
	    for (i = knotidx[0]; i < end_point && knotid < spline_points; i++) {
	      if (i < knotidx[knotid]) {
		if (!cb->selected[i]) {
		  float d =
		      (float)(i - knotidx[knotid - 1]) / (float)(knotidx[knotid] -
								 knotidx[knotid - 1]);
		  float s1 = s[knotid - 1] + d * (s[knotid] - s[knotid - 1]);
		  float t = s1 / s[spline_points - 1];
		  mskrv2_(&t, &cb->x[i], &cb->y[i], &spline_points, spline_x, spline_y, xp, yp, s,
			  &zero, &ics, &zero);
		  cb->x[i] = max(xmin, min(xmax, cb->x[i]));
		  cb->y[i] = max(ymin, min(ymax, cb->y[i]));
		}
	      } else
		knotid++;
	    }
	    XtFree((String) xp);
	    XtFree((String) yp);
	    XtFree((String) temp);
	    XtFree((String) s);
	  }
	}
      } else if (spline_points > 1) {
	if (xincreasing || !yincreasing) {
	  for (i = knotidx[0] + 1; i < knotidx[spline_points - 1]; i++) {
	    if (cb->x[i] < spline_x[1])
	      cb->y[i] = interp(spline_x[0], spline_y[0], spline_x[1], spline_y[1], cb->x[i]);
	    else
	      cb->y[i] = interp(spline_x[1], spline_y[1], spline_x[2], spline_y[2], cb->x[i]);
	  }
	} else {
	  for (i = knotidx[0] + 1; i < knotidx[spline_points - 1]; i++) {
	    if (cb->y[i] < spline_y[1])
	      cb->x[i] = interp(spline_y[0], spline_x[0], spline_y[1], spline_x[1], cb->y[i]);
	    else
	      cb->x[i] = interp(spline_y[1], spline_x[1], spline_y[2], spline_x[2], cb->y[i]);
	  }
	}
      }
    }
  }
  XtFree((String) spline_x);
  XtFree((String) spline_y);
  XtFree((String) knotidx);
}

static void Move(Widget w, int stub, XmdsWavedrawValueCBStruct * cb)
{
  if (cb->event) {
    int pointermode;
    EventInfo *e;
    XtVaGetValues(w, XmdsNpointerMode, &pointermode, NULL);
    e = NewEvent(w, stub, cb->event, cb->reason,
		 pointermode > XmdsPOINTER_MODE_SET_PEN ? CB_STRETCH : CB_MOVE);
    e->moved_idx = cb->idx;
    e->values[0] = cb->oldx;;
    e->values[1] = cb->newx;
    e->values[2] = cb->oldy;
    e->values[3] = cb->newy;
  }
  return;
}

static int WaveToIdx(Widget w)
{
  Widget plots = XtNameToWidget(TopWidget(w), "*cw_wvedit");
  int i;
  int idx = 0;
  for (i = 0; 1; i++) {
    Widget pane = PaneIdxToWidget(plots, i);
    if (pane) {
      int i;
      for (i = 0; 1; i++) {
	Widget wave = WaveIdxToWidget(pane, i);
	if (wave) {
	  if (wave == w)
	    return idx;
	  else
	    idx++;
	} else
	  break;
      }
    } else
      break;
  }
  return -1;
}

static void SetAtLimits(Widget w __attribute__ ((unused)), int stub, XmPushButtonCallbackStruct * cb)
{
  Widget wave = setup_wave;
  if (wave) {
    EventInfo *e = NewEvent(wave, stub, cb->event, XmdsCRZoomOut, CB_SET_AT_LIMITS);
    float *xminval;
    float *xmaxval;
    float *yminval;
    float *ymaxval;
    XtVaGetValues(wave, XmdsNlowX, &xminval, XmdsNhighX, &xmaxval, XmdsNlowY, &yminval, XmdsNhighY,
		  &ymaxval, NULL);
    XtVaSetValues(wave, XmdsNxMin, xminval, XmdsNxMax, xmaxval, XmdsNyMin, yminval, XmdsNyMax,
		  ymaxval, NULL);
    XtVaGetValues(wave, XmdsNxMin, &xminval, XmdsNxMax, &xmaxval, XmdsNyMin, &yminval, XmdsNyMax,
		  &ymaxval, NULL);
    e->values[0] = *xminval;
    e->values[1] = *xmaxval;
    e->values[2] = *yminval;
    e->values[3] = *ymaxval;
  }
}

static void AddPoint(Widget w, int stub, XmdsWavedrawValueCBStruct * cb)
{
  EventInfo *e = NewEvent(w, stub, cb->event, cb->reason, CB_ADD_POINT);
  e->moved_idx = cb->idx;
  e->values[0] = cb->oldx;;
  e->values[1] = cb->newx;
  e->values[2] = cb->oldy;
  e->values[3] = cb->newy;
}

static void DeletePoint(Widget w, int stub, XmdsWavedrawValueCBStruct * cb)
{
  EventInfo *e = NewEvent(w, stub, cb->event, cb->reason, CB_DELETE_POINT);
  e->moved_idx = cb->idx;
  e->values[0] = cb->oldx;;
  e->values[1] = cb->newx;
  e->values[2] = cb->oldy;
  e->values[3] = cb->newy;
}

static void Print(Widget w __attribute__ ((unused)), int tag __attribute__ ((unused)),
		  XmdsWavedrawValueCBStruct * cb __attribute__ ((unused)))
{
  return;
}

static void PrintAll(Widget w __attribute__ ((unused)), int tag __attribute__ ((unused)),
		     XmAnyCallbackStruct * cb __attribute__ ((unused)))
{
}

static void ResetCustomizePrint(Widget w __attribute__ ((unused)),
				int tag __attribute__ ((unused)),
				XmAnyCallbackStruct * cb __attribute__ ((unused)))
{
  XmTextSetString(XtNameToWidget(CustomizePrintWidget, "print_file"), PrintFile);
}

static void ApplyCustomizePrint(Widget w __attribute__ ((unused)),
				int tag __attribute__ ((unused)),
				XmAnyCallbackStruct * cb __attribute__ ((unused)))
{
  ReplaceString(&PrintFile, XmTextGetString(XtNameToWidget(CustomizePrintWidget, "print_file")), 1);
}

Boolean ReplaceString(String * old, String new, Boolean free)
{
  Boolean changed = TRUE;
  if (*old) {
    changed = strcmp(*old, new);
    XtFree(*old);
  }
  *old = XtNewString(new);
  if (free)
    XtFree(new);
  return changed;
}

static EventInfo *EVENTLIST = 0;
EXPORT void GetEventInfo(int *eventid, int *info) {
  EventInfo *e,*prev;
  IDL_WidgetStubLock(TRUE);
  memset(info,0,40);
  for (e=EVENTLIST,prev=NULL; e; prev=e,e=e->next) {
    if (e->eventid==*eventid) {
      memcpy(info,e,40);
      if (prev == NULL)
	EVENTLIST=e->next;
      else
	prev->next=e->next;
      free(e);
    }
  }
  IDL_WidgetStubLock(FALSE);
}
  
static EventInfo *NewEvent(Widget w, unsigned long stub, XEvent * event, int reason,
			   enum callback_id callback_id)
{
  static int EVENTID=0;
  static EventInfo *last_e = 0;
  if (!(last_e && last_e->w == w && last_e->reason == reason && last_e->callback_id == callback_id)) {
    char *rec;
    IDL_WidgetStubLock(TRUE);
    rec = IDL_WidgetStubLookup(stub);
    if (rec) {
      EventInfo *e = (EventInfo *) malloc(sizeof(EventInfo));
      e->wave_idx = WaveToIdx(w);
      e->reason = reason;
      e->callback_id = callback_id;
      e->state = event ? ((XButtonEvent *) event)->state : 0;
      e->button = event ? ((XButtonEvent *) event)->button : 0;
      e->w = w;
      e->eventid=++EVENTID;
      if (EVENTLIST==NULL) {
	e->next=0;
	EVENTLIST=e;
      } else {
	e->next=EVENTLIST;
	EVENTLIST=e;
      }
      IDL_WidgetIssueStubEvent(rec, (IDL_LONG) e->eventid);
      last_e = e;
    }
    IDL_WidgetStubLock(FALSE);
  }
  return last_e;
}

EXPORT void *memmoveext(void *dest, const void *src, size_t n)
{
  return memmove(dest, src, n);
}
