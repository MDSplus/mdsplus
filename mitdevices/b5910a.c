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
#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mds_stdarg.h>
#include <mitdevices_msg.h>
#include <ncidef.h>
#include <Xm/XmP.h>
#include <stdio.h>
#include <Xmds/XmdsWavedraw.h>
#include "b5910a_gen.h"
#include <Mrm/MrmPublic.h>
#include <Xm/RowColumn.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <libroutines.h>
#include <treeshr.h>
#include <mdsshr.h>
#include <xmdsshr.h>
#include <Xmds/XmdsExpr.h>
#include <Xmds/XmdsNidOptionMenu.h>
#include <Xmds/XmdsXdBox.h>
#include <Xmds/XmdsXdBoxDialogButton.h>
#define DEVRANGE_MISMATCH       0x277c807a
#define DEVBAD_ACTIVE_CHAN      0x277c8152
static float fval = 0;
static DESCRIPTOR_FLOAT(float_d, &fval);





static float (*csval) () = 0;
static void (*csakm) () = 0;
extern int DevCamChk();



static int one = 1;
static int zero = 0;

int b5910a_LINFIT(int *num_knots, float *knots_x, float *knots_y, int *num_v, float *x, float *y);
int b5910a_SPLFIT(int *num_knots, float *knots_x, float *knots_y, int *num_v, float *x, float *y);

static void RemoveZeros();
static char *d_name(int channel);
static char *xd_name(int channel);
static char *fit_name(int channel);
static void ResetWave(Widget w);
static void Draw(Widget w);
static void AutoY();
static void SameX();
static void SetPoint();
static void SetPointerMode();
static int NumKnots(int count, Boolean * selected);
static void Align();
static void AutoscaleAll();
static void FullscaleAll();
static void Crosshairs();
static void Spline(int count, float *x, float *y, Boolean * selected, Boolean * pen_down);
static void Linear(int count, float *x, float *y, Boolean * selected, Boolean * pen_down);
static void Reset(Widget w);
static void OutputValue(Widget w, float x, float y);
static void Move();
static void AddPoint();
static void DeletePoint();
static void Limits();
static void Fit();
static void FitChange();
static void Dismiss();
static void InitWave();
static Widget Top(Widget w);
static int Count(Widget w);
static String CoordinatesString(float *x, float *y);
static void UpdateXdBox(Widget w, int count, float *x, float *y, Boolean * selected);
/*------------------------------------------------------------------------------

 Macros:                                                                      */

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) ( ((a)>(b)) ? (a) : (b) )

/*------------------------------------------------------------------------------

 Global variables:                                                            */

/*------------------------------------------------------------------------------

 Executable:                                                                  */

EXPORT int b5910a__dw_setup(struct descriptor *niddsc __attribute__ ((unused)), struct descriptor *methoddsc __attribute__ ((unused)), Widget parent)
{
  static String uids[] = { "B5910A.uid" };
  static int nid;
  static NCI_ITM nci[] =
      { {4, NciCONGLOMERATE_NIDS, (unsigned char *)&nid, 0}, {0, NciEND_OF_LIST, 0, 0} };
  Widget w;
  int status;
  static MrmRegisterArg uilnames[] = { {"external_clock_nid", 0},
  {"AddPoint", (char *)AddPoint},
  {"Align", (char *)Align},
  {"AutoscaleAll", (char *)AutoscaleAll},
  {"AutoY", (char *)AutoY},
  {"Dismiss", (char *)Dismiss},
  {"Crosshairs", (char *)Crosshairs},
  {"DeletePoint", (char *)DeletePoint},
  {"Draw", (char *)Draw},
  {"Fit", (char *)Fit},
  {"FitChange", (char *)FitChange},
  {"FullscaleAll", (char *)FullscaleAll},
  {"InitWave", (char *)InitWave},
  {"Limits", (char *)Limits},
  {"Move", (char *)Move},
  {"Reset", (char *)Reset},
  {"ResetWave", (char *)ResetWave},
  {"SameX", (char *)SameX},
  {"SetPoint", (char *)SetPoint},
  {"SetPointerMode", (char *)SetPointerMode}
  };
  char *title;
  TreeGetNci(*(int *)niddsc->pointer, nci);
  uilnames[0].value = (char *)0 + (nid + B5910A_N_EXT_CLOCK);
  status =
      XmdsDeviceSetup(parent, (int *)niddsc->pointer, uids, XtNumber(uids), "B5910A", uilnames,
		      XtNumber(uilnames), &w);
  XtVaGetValues(XtParent(w), XtNtitle, &title, NULL);
  XtVaSetValues(XtParent(XtNameToWidget(w, "*draw_popup")), XtNtitle, title, NULL);
  return status;
}

static void Reset(Widget w)
{
  Widget top = Top(w);
  XmdsResetAllXds(top);
  if (XtIsManaged(XtNameToWidget(top, "*draw_popup")))
    ResetWave(w);
}

static void Draw(Widget w)
{
  XtManageChild(XtNameToWidget(Top(w), "*draw_popup"));
}

static void ResetTime(Widget w, int count, Boolean * selected, float *x)
{
  float *saved_times;
  int num_knots = NumKnots(count, selected);
  Widget first_dw = XtNameToWidget(Top(w), d_name(0));
  XtVaGetValues(first_dw, XmNuserData, &saved_times, NULL);
  if (saved_times)
    memcpy(&x[num_knots], saved_times, (count - num_knots) * sizeof(float));
}

static void ResetWave(Widget w)
{
  static EMPTYXD(knot_y_xd);
  static EMPTYXD(knot_x_xd);
  static EMPTYXD(times_xd);
  static int zero = 0;
  static int end_idx;
  float *time;
  static DESCRIPTOR_LONG(start_d, &zero);
  static DESCRIPTOR_LONG(end_d, &end_idx);
  static DESCRIPTOR_WINDOW(window, &start_d, &end_d, 0);
  static DESCRIPTOR_DIMENSION(dimension, &window, 0);
  int count = Count(w);
  int i;
  int NeedToFreeAxis = 1;
  end_idx = count - 1;
  dimension.axis = (struct descriptor *)XmdsNidOptionMenuGetXd(XtNameToWidget(Top(w), "*clock"));
  if (!dimension.axis) {
    XmdsComplain(w, "Clock cannot be resolved so drawing cannot be performed");
    return;
  }
  window.value_at_idx0 = XmdsExprGetXd(XtNameToWidget(Top(w), "*trigger"));
  if (!window.value_at_idx0) {
    XmdsComplain(w, "Trigger cannot be resolved so drawing cannot be performed");
    return;
  }
  if (dimension.axis->dtype == DTYPE_DSC && dimension.axis->pointer == 0) {
    XtPointer user_data;
    static int ext_clock_nid;
    static DESCRIPTOR_NID(ext_clock, &ext_clock_nid);

    /* free the one allocated by XmdsNidOptionMenuGetXd */
    MdsFree1Dx((struct descriptor_xd *)dimension.axis, 0);
    XtFree((char *)dimension.axis);
    NeedToFreeAxis = 0;

    XtVaGetValues(XtNameToWidget(Top(w), "*ext_clock"), XmNuserData, &user_data, NULL);
    ext_clock_nid = (intptr_t) user_data;
    dimension.axis = &ext_clock;
  }
  if ((TdiData((struct descriptor *)&dimension, &times_xd MDS_END_ARG) & 1) &&
      (TdiCvt((struct descriptor *)&times_xd, &float_d, &times_xd MDS_END_ARG) & 1)) {
    if (times_xd.pointer->class == CLASS_A) {
      struct descriptor_a *array = (struct descriptor_a *)times_xd.pointer;
      count = min(count, ((int)(array->arsize / sizeof(float))));
      time = (float *)array->pointer;
    } else
      count = 1;
    if (count >= 2) {
      float *saved_times;
      Widget first_dw = XtNameToWidget(Top(w), d_name(0));
      XtVaGetValues(first_dw, XmNuserData, &saved_times, NULL);
      if (saved_times)
	XtFree((XtPointer) saved_times);
      XtVaSetValues(first_dw, XmNuserData,
		    memcpy(XtMalloc(sizeof(float) * count), time, sizeof(float) * count), NULL);
      for (i = 0; i < 4; i++) {
	struct descriptor_xd *valid_xd;
	float *y;
	float *x;
	Boolean *selected;
	Boolean *pendown;
	float *knot_x;
	float *knot_y;
	int num_knots = 0;
	int j;
	//int knot = 0;
	//	int on_state;
	Widget xdbw = XmdsXdBoxDialogButtonGetXdBox(XtNameToWidget(Top(w), xd_name(i)));
	Widget dw = XtNameToWidget(Top(w), d_name(i));
	Widget fw = XtNameToWidget(Top(w), fit_name(i));
	if ((valid_xd = (struct descriptor_xd *)XmdsXdBoxGetXd(xdbw))
	    && (TdiData((struct descriptor *)valid_xd->pointer, &knot_y_xd MDS_END_ARG) & 1)
	    && (TdiCvt((struct descriptor *)&knot_y_xd, &float_d, &knot_y_xd MDS_END_ARG) & 1)) {
	  if (knot_y_xd.pointer->class == CLASS_A) {
	    struct descriptor_a *array = (struct descriptor_a *)knot_y_xd.pointer;
	    num_knots = array->arsize / sizeof(float);
	    knot_y = (float *)array->pointer;
	  } else {
	    num_knots = 1;
	    knot_y = (float *)knot_y_xd.pointer->pointer;
	  }
	}
	if (num_knots && valid_xd && (TdiDimOf(valid_xd->pointer, &knot_x_xd MDS_END_ARG) & 1) &&
	    (TdiData((struct descriptor *)&knot_x_xd, &knot_x_xd MDS_END_ARG) & 1)
	    && (TdiCvt((struct descriptor *)&knot_x_xd, &float_d, &knot_x_xd MDS_END_ARG) & 1)) {
	  if (knot_x_xd.pointer->class == CLASS_A) {
	    struct descriptor_a *array = (struct descriptor_a *)knot_x_xd.pointer;
	    num_knots = min(num_knots, ((int)(array->arsize / sizeof(float))));
	    knot_x = (float *)array->pointer;
	  } else {
	    num_knots = 1;
	    knot_x = (float *)knot_x_xd.pointer->pointer;
	  }
	} else
	  num_knots = 0;
	if (valid_xd) {
	  MdsFree1Dx(valid_xd, 0);
	  XtFree((char *)valid_xd);
	}
	x = (float *)XtCalloc(num_knots + count, sizeof(float));
	y = (float *)XtCalloc(num_knots + count, sizeof(float));
	selected = XtCalloc(num_knots + count, sizeof(Boolean));
	pendown = XtCalloc(num_knots + count, sizeof(Boolean));
	for (j = 0; j < num_knots; j++) {
	  x[j] = knot_x[j];
	  y[j] = knot_y[j];
	  selected[j] = 1;
	  pendown[j] = 0;
	}
	for (j = 0; j < count; j++) {
	  selected[j + num_knots] = 0;
	  pendown[j + num_knots] = 1;
	}
	ResetTime(dw, count + num_knots, selected, x);
	XmdsSetOptionIdx(fw, XmdsXdBoxGetState(xdbw));
	if (XmdsGetOptionIdx(fw))
	  Linear(count + num_knots, x, y, selected, pendown);
	else
	  Spline(count + num_knots, x, y, selected, pendown);
	XtVaSetValues(dw, XmdsNcount, count + num_knots, XmdsNxValue, x, XmdsNyValue, y,
		      XmdsNselections, selected, XmdsNpenDown, pendown, XmdsNxMin, (void *)0,
		      XmdsNxMax, (void *)0, XmdsNyMin, (void *)0, XmdsNyMax, (void *)0,
		      XmdsNdeleteOnlySelected, (void *)1, NULL);
	XtFree((char *)x);
	XtFree((char *)y);
	XtFree(selected);
	XtFree(pendown);
      }
    } else
      XmdsComplain(w,
		   "Number of samples and/or Trigger and clock information have specified less than 2 samples");
  } else
    XmdsComplain(w, "Trigger and clock must be resolvable before drawing can be enabled");
  if (dimension.axis && NeedToFreeAxis) {
    MdsFree1Dx((struct descriptor_xd *)dimension.axis, 0);
    XtFree((char *)dimension.axis);
  }
  if (window.value_at_idx0) {
    MdsFree1Dx((struct descriptor_xd *)window.value_at_idx0, 0);
    XtFree((char *)window.value_at_idx0);
  }
  MdsFree1Dx(&knot_x_xd, 0);
  MdsFree1Dx(&knot_y_xd, 0);
  MdsFree1Dx(&times_xd, 0);
}

static void Crosshairs(Widget w, int tag __attribute__ ((unused)), XmdsWaveformCrosshairsCBStruct * c)
{
  int i;
  Widget *children;
  int num;
  XtVaGetValues(XtParent(w), XtNnumChildren, &num, XtNchildren, &children, NULL);
  for (i = 0; i < num; i++)
    if (XmdsIsWavedraw(children[i]) && w != children[i])
      XmdsWaveformSetCrosshairs(children[i], &c->x, &c->y, 0);
  OutputValue(w, c->x, c->y);
}

static Widget Top(Widget w)
{
  for ( ; w && ((XtName(w) == 0) || strcmp(XtName(w), "B5910A")); w = XtParent(w)) ;
  return w;
}

static void OutputValue(Widget w, float x, float y)
{
  String label = CoordinatesString(&x, &y);
  XmTextSetString(XtNameToWidget(Top(w), "*crosshairs_st"), label);
  XtFree(label);
}

static void AutoY(Widget w, int *idx, XmPushButtonCallbackStruct * cb __attribute__ ((unused)))
{
  //int i;
  //float *xmin;
  //float *xmax;

  Widget wave = XtNameToWidget(Top(w), d_name(*idx - 1));
  XtVaSetValues(wave, XmdsNyMin, 0, XmdsNyMax, 0, NULL);
}

static void SameX(Widget w, int *idx, XmPushButtonCallbackStruct * cb __attribute__ ((unused)))
{
  int i;
  float *xmin;
  float *xmax;

  Widget wave = XtNameToWidget(Top(w), d_name(*idx - 1));
  XtVaGetValues(wave, XmdsNxMin, &xmin, XmdsNxMax, &xmax, NULL);
  for (i = 0; i < 4; i++)
    if (i != (*idx - 1)) {
      Widget wave = XtNameToWidget(Top(w), d_name(i));
      XtVaSetValues(wave, XmdsNxMin, xmin, XmdsNxMax, xmax, NULL);
    }
}

static void SetPointerMode(Widget w, int *mode, XmToggleButtonCallbackStruct * cb __attribute__ ((unused)))
{
  int i;
  XmString label;
  String help = "";
  if (!cb || cb->set)
    for (i = 0; i < 4; i++)
      XmdsWaveformSetPointerMode(XtNameToWidget(Top(w), d_name(i)), *mode);
  switch (*mode) {
  case XmdsPOINTER_MODE_POINT:
    help = "Button 1: Position crosshairs";
    break;
  case XmdsPOINTER_MODE_ZOOM:
    help = "Button 1: Zoom in 2x, Button 1 drag: Zoom in box, Button 2: Zoom out";
    break;
  case XmdsPOINTER_MODE_DRAG:
    help = "Button 1: Drag curves (change axis, not curve values)";
    break;
  case XmdsPOINTER_MODE_ADD_DELETE:
    help =
	"Button 1: Move knot (modifiers: shift=lock X, alt=lock Y, shift-lock=high res), Button 2: Add point (shift=Remove point)";
    break;
  case XmdsPOINTER_MODE_SLIDE_STRETCH:
    help =
	"Button 1: Drag curve (change values)  (modifiers: shift=lock X, alt=lock Y, shift-lock=high res, ctrl=Stretch/Shrink curve)";
    break;
  }
  label = XmStringCreateSimple(help);
  XtVaSetValues(XtNameToWidget(Top(w), "*helpText"), XmNlabelString, label, NULL);
  XmStringFree(label);
}

static void Align(int w __attribute__ ((unused)), int tag __attribute__ ((unused)), XmdsWaveformLimitsCBStruct * l __attribute__ ((unused)))
{
}

static int NumKnots(int count, Boolean * selected)
{
  int i;
  int num_knots;
  for (i = 0, num_knots = 0; i < count && selected[i]; i++, num_knots++) ;
  return num_knots;
}

static void AddPoint(Widget w, int tag __attribute__ ((unused)), XmdsWavedrawValueCBStruct * v)
{
  Boolean *pendown;
  Boolean *selected;
  int num_knots = NumKnots(v->count, v->selected);
  int i;
  int j;
  if (v->idx >= num_knots) {
    memcpy(&v->x[num_knots + 1], &v->x[num_knots], (v->idx - num_knots) * sizeof(float));
    memcpy(&v->y[num_knots + 1], &v->y[num_knots], (v->idx - num_knots) * sizeof(float));
    v->x[num_knots] = v->newx;
    v->y[num_knots] = v->newy;
  }
  XtVaGetValues(w, XmdsNpenDown, &pendown, XmdsNselections, &selected, NULL);
  num_knots++;
  memset(selected, 1, num_knots);
  memset(&selected[num_knots], 0, v->count - num_knots);
  memset(pendown, 0, num_knots);
  memset(&pendown[num_knots], 1, v->count - num_knots);
  for (i = 0; i < num_knots - 1; i++)
    for (j = i + 1; j < num_knots; j++)
      if (v->x[i] > v->x[j]) {
	float t_x = v->x[i];
	float t_y = v->y[i];
	v->x[i] = v->x[j];
	v->y[i] = v->y[j];
	v->x[j] = t_x;
	v->y[j] = t_y;
      }
  for (i = 0; i < num_knots - 1; i++)
    if (v->x[i] >= v->x[i + 1])
      v->x[i + 1] = v->x[i] + (v->x[num_knots - 1] - v->x[0]) / 1E6;
  UpdateXdBox(w, v->count, v->x, v->y, v->selected);
  return;
}

static void DeletePoint(Widget w, int tag __attribute__ ((unused)), XmdsWavedrawValueCBStruct * v)
{
  UpdateXdBox(w, v->count, v->x, v->y, v->selected);
  return;
}

static void Move(Widget w, int tag __attribute__ ((unused)), XmdsWavedrawValueCBStruct * v)
{
  v->y[v->idx] = v->newy;
  if (((v->idx < v->count - 1) && (v->newx >= v->x[v->idx + 1]) && (v->selected[v->idx + 1])) ||
      ((v->idx > 0) && (v->newx <= v->x[v->idx - 1])))
    v->x[v->idx] = v->oldx;
  else
    v->x[v->idx] = v->newx;
  OutputValue(w, v->x[v->idx], v->y[v->idx]);
  if (v->reason == XmdsCRMovePointEnd) {
    int i;
    for (i = 0; i < 4; i++)
      if (XtNameToWidget(Top(w), d_name(i)) == w)
	break;
    XtVaSetValues(XtNameToWidget(Top(w), "*set_point"), XmNuserData, i * 65536 + v->idx, NULL);
    UpdateXdBox(w, v->count, v->x, v->y, v->selected);
  }
  return;
}

static void SetPoint(Widget w)
{
  String text = XmTextFieldGetString(XtNameToWidget(Top(w), "*crosshairs_st"));
  float x;
  float y;
  int num = sscanf(text, " ( %g , %g ) ", &x, &y);
  XtPointer temp;
  int userData;
  int channel;
  int idx;
  Widget dw;
  XtVaGetValues(w, XmNuserData, &temp, NULL);
  userData = (intptr_t) temp;
  channel = userData / 65536;
  idx = userData % 65536;
  if (channel >= 0 && channel < 4 && num == 2) {
    float *xptr;
    float *yptr;
    int count;
    Boolean *selected;
    float new_x;
    float new_y;
    dw = XtNameToWidget(Top(w), d_name(channel));
    XmdsWavedrawMovePoint((XmdsWavedrawWidget) dw, idx, &x, &y, &new_x, &new_y, 1, 0);
    XtVaGetValues(dw, XmdsNcount, &count, XmdsNxValue, &xptr, XmdsNyValue, &yptr, XmdsNselections,
		  &selected, NULL);
    UpdateXdBox(dw, count, xptr, yptr, selected);
  }
  XtFree(text);
}

static void UpdateXdBox(Widget w, int count, float *x, float *y, Boolean * selected)
{
  int i;
  for (i = 0; i < 4; i++)
    if (XtNameToWidget(Top(w), d_name(i)) == w)
      break;
  {
    int num_knots = NumKnots(count, selected);
    Widget xdbw = XmdsXdBoxDialogButtonGetXdBox(XtNameToWidget(Top(w), xd_name(i)));
    if (num_knots) {
      static DESCRIPTOR_A(x_array, sizeof(float), DTYPE_NATIVE_FLOAT, 0, 0);
      static DESCRIPTOR_A(y_array, sizeof(float), DTYPE_NATIVE_FLOAT, 0, 0);
      static DESCRIPTOR_SIGNAL_1(signal, &y_array, 0, &x_array);
      x_array.pointer = (char *)x;
      x_array.arsize = num_knots * sizeof(float);
      y_array.pointer = (char *)y;
      y_array.arsize = num_knots * sizeof(float);
      XmdsXdBoxSetXd(xdbw, (struct descriptor *)&signal);
    } else {
      static EMPTYXD(xd);
      XmdsXdBoxSetXd(xdbw, (struct descriptor *)&xd);
    }
  }
}

static void Limits(Widget w __attribute__ ((unused)), int tag __attribute__ ((unused)), XmdsWaveformLimitsCBStruct * l __attribute__ ((unused)))
{
}

static void Fit(Widget w, int tag __attribute__ ((unused)), XmdsWavedrawFitCBStruct * c)
{
  //int fit;
  char name[10] = "*";
  strcat(name, XtName(w));
  strcat(name, "_fit");
  ResetTime(w, c->count, c->selected, c->x);
  if (XmdsGetOptionIdx(XtNameToWidget(Top(w), name)))
    Linear(c->count, c->x, c->y, c->selected, c->pen_down);
  else
    Spline(c->count, c->x, c->y, c->selected, c->pen_down);
}

static void FitChange(Widget w, int tag __attribute__ ((unused)), XmRowColumnCallbackStruct * cb)
{
  int i;
  for (i = 0; i < 4; i++) {
    Widget submenu;
    Widget xdbw = XmdsXdBoxDialogButtonGetXdBox(XtNameToWidget(Top(w), xd_name(i)));
    XtVaGetValues(XtNameToWidget(Top(w), fit_name(i)), XmNsubMenuId, &submenu, NULL);
    if (submenu == w) {
      int count;
      float *x;
      float *y;
      Boolean *selections;
      Boolean *pendown;
      Widget wave = XtNameToWidget(Top(w), d_name(i));
      XtVaGetValues(wave, XmdsNxValue, &x, XmdsNyValue, &y, XmdsNselections, &selections,
		    XmdsNpenDown, &pendown, XmdsNcount, &count, NULL);
      ResetTime(wave, count, selections, x);
      if (*cb->data)
	Linear(count, x, y, selections, pendown);
      else
	Spline(count, x, y, selections, pendown);
      (wave->core.widget_class->core_class.expose) (wave, 0, 0);
      XmdsXdBoxSetState(xdbw, *cb->data);
      break;
    }
  }
}

static int InitMath()
{
  static DESCRIPTOR(image, "MdsMath");
  static DESCRIPTOR(csakm_d, "csakm_");
  static DESCRIPTOR(csval_d, "csval_");
  int status;
  status = LibFindImageSymbol(&image, &csakm_d, &csakm);
  if (status & 1)
    status = LibFindImageSymbol(&image, &csval_d, &csval);
  return status;
}

EXPORT int b5910a_SPLFIT(int *num_knots, float *knots_x, float *knots_y, int *num_v, float *x, float *y)
{
  int i;
  if (*num_knots > 2) {
    int status = 1;
    float *fbreak = (float *)XtCalloc(*num_knots, sizeof(float));
    float *cscoef = (float *)XtCalloc(*num_knots * 4, sizeof(float));
    if (csakm == 0 || csval == 0)
      status = InitMath();
    if (status & 1) {
      (*csakm) (num_knots, knots_x, knots_y, fbreak, cscoef);
      for (i = 0; i < *num_v; i++)
	y[i] = (*csval) (&x[i], num_knots, fbreak, cscoef);
      XtFree((char *)fbreak);
      XtFree((char *)cscoef);
      return 1;
    } else
      return status;
  } else
    return b5910a_LINFIT(num_knots, knots_x, knots_y, num_v, x, y);
}

static void Spline(int count, float *x, float *y, Boolean * selected, Boolean * pen_down)
{
  int num_knots = NumKnots(count, selected);
  int num_v = count - num_knots;
  int i;
  b5910a_SPLFIT(&num_knots, x, y, &num_v, &x[num_knots], &y[num_knots]);
  for (i = 0; i < num_v; i++)
    pen_down[num_knots + i] = 1;
}

EXPORT int b5910a_LINFIT(int *num_knots, float *knots_x, float *knots_y, int *num_v, float *x, float *y)
{
  float slope;
  float intercept;
  int i;
  int knot_idx = 1;
  float high_x = 0.0;
  if (*num_knots > 1) {
    slope = (knots_x[1] == knots_x[0]) ? 0 : (knots_y[1] - knots_y[0]) / (knots_x[1] - knots_x[0]);
    intercept = knots_y[1] - slope * knots_x[1];
    high_x = knots_x[1];
  } else if (*num_knots == 1) {
    slope = 0.0;
    intercept = knots_y[0];
  } else {
    slope = 0.0;
    intercept = 0.0;
  }
  if (*num_v > 0) {
    for (i = 0; i < *num_v; i++) {
      while ((x[i] > high_x) && (knot_idx < (*num_knots - 1))) {
	knot_idx++;
	slope = (knots_x[knot_idx] == knots_x[knot_idx - 1]) ? 0 :
	    (knots_y[knot_idx] - knots_y[knot_idx - 1]) / (knots_x[knot_idx] -
							   knots_x[knot_idx - 1]);
	intercept = knots_y[knot_idx] - slope * knots_x[knot_idx];
	high_x = knots_x[knot_idx];
      }
      y[i] = slope * x[i] + intercept;
    }
  } else {
    float min_x = x[0];
    float max_x = x[-(*num_v) - 1];
    int j = 0;
    while ((min_x > high_x) && (knot_idx < (*num_knots - 1))) {
      knot_idx++;
      slope = (knots_x[knot_idx] == knots_x[knot_idx - 1]) ? 0 :
	  (knots_y[knot_idx] - knots_y[knot_idx - 1]) / (knots_x[knot_idx] - knots_x[knot_idx - 1]);
      intercept = knots_y[knot_idx] - slope * knots_x[knot_idx];
      high_x = knots_x[knot_idx];
    }
    y[j++] = slope * min_x + intercept;
    while ((max_x > high_x) && (knot_idx < (*num_knots - 1))) {
      y[j] = knots_y[knot_idx];
      x[j++] = knots_x[knot_idx];
      knot_idx++;
      slope = (knots_x[knot_idx] == knots_x[knot_idx - 1]) ? 0 :
	  (knots_y[knot_idx] - knots_y[knot_idx - 1]) / (knots_x[knot_idx] - knots_x[knot_idx - 1]);
      intercept = knots_y[knot_idx] - slope * knots_x[knot_idx];
      high_x = knots_x[knot_idx];
    }
    y[j] = slope * max_x + intercept;
    x[j++] = max_x;
    *num_v = j;
  }
  return 1;
}

static void Linear(int count, float *knots_x, float *knots_y, Boolean * selected,
		   Boolean * pen_down)
{
  int num_knots = NumKnots(count, selected);
  int num_v = num_knots - count;
  int i;
  float *x = &knots_x[num_knots];
  float *y = &knots_y[num_knots];
  b5910a_LINFIT(&num_knots, knots_x, knots_y, &num_v, x, y);
  for (i = 0; i < num_v; i++)
    pen_down[num_knots + i] = 1;
  for (i = num_v + num_knots - 1; i < count; i++)
    pen_down[i] = 0;
}

static void AutoscaleAll(Widget w)
{
  int i;
  for (i = 0; i < 4; i++)
    XtVaSetValues(XtNameToWidget(Top(w), d_name(i)), XmdsNxMin, 0, XmdsNxMax, 0, XmdsNyMin, 0,
		  XmdsNyMax, 0, NULL);
}

static void FullscaleAll(Widget w, int *range)
{
  static float const maxs[] = { 10.24, 5.12, 10.24, 5.12 };
  static float mins[] = { -10.24, -5.12, 0, 0 };
  float ymin;
  float ymax;
  int i;
  ymax = maxs[*range] + .1 * (maxs[*range] - mins[*range]);
  ymin = mins[*range] - .1 * (maxs[*range] - mins[*range]);
  for (i = 0; i < 4; i++)
    XtVaSetValues(XtNameToWidget(Top(w), d_name(i)), XmdsNyMin, &ymin, XmdsNyMax, &ymax, NULL);
}

static void InitWave(Widget w, int *idx)
{
  int count;
  int num_knots;
  float *x;
  float *y;
  Boolean *selected;
  Boolean *pendown;
  EMPTYXD(xd);
  Widget xdbw = XmdsXdBoxDialogButtonGetXdBox(XtNameToWidget(Top(w), xd_name(*idx - 1)));
  Widget wave = XtNameToWidget(Top(w), d_name(*idx - 1));
  XtVaGetValues(wave, XmdsNcount, &count, XmdsNxValue, &x, XmdsNyValue, &y, XmdsNselections,
		&selected, XmdsNpenDown, &pendown, NULL);
  num_knots = NumKnots(count, selected);
  memset(y, 0, count * sizeof(*y));
  XtVaSetValues(wave, XmdsNcount, count - num_knots, XmdsNxValue, &x[num_knots], XmdsNyValue,
		&y[num_knots], XmdsNselections, &selected[num_knots], XmdsNpenDown,
		&pendown[num_knots], XmdsNxMin, (XtPointer) NULL, XmdsNxMax, (XtPointer) NULL,
		XmdsNyMin, (XtPointer) NULL, XmdsNyMax, (XtPointer) NULL, NULL);
  XmdsXdBoxSetXd(xdbw, (struct descriptor *)&xd);
}

static void Dismiss(Widget w)
{
  XtUnmanageChild(XtNameToWidget(Top(w), "*draw_popup"));
}

static String CoordinatesString(float *x, float *y)
{
  char label[42] = "(";
  int length;
  char *ptr = &label[1];
  sprintf(ptr, "%g%n", *x, &length);
  RemoveZeros(ptr, &length);
  ptr[length] = ',';
  ptr = &ptr[length + 1];
  sprintf(ptr, "%g%n", *y, &length);
  RemoveZeros(ptr, &length);
  ptr[length] = ')';
  ptr[length + 1] = 0;
  return XtNewString(label);
}

static void RemoveZeros(char *string, int *length)
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

static int Count(Widget w)
{
  int count = 0;
  DESCRIPTOR_LONG(count_d, 0);
  struct descriptor_xd *valid_xd;
  count_d.pointer = (char *)&count;
  if ((valid_xd = (struct descriptor_xd *)XmdsExprGetXd(XtNameToWidget(Top(w), "*samples"))))
    TdiData((struct descriptor *)valid_xd, &count_d MDS_END_ARG);
  return count;
}

static char *d_name(int channel)
{
  static char dname[] = "*sn";
  dname[2] = '1' + channel;
  return dname;
}

static char *xd_name(int channel)
{
  static char xdname[] = "*xdn";
  xdname[3] = '1' + channel;
  return xdname;
}

static char *fit_name(int channel)
{
  static char fname[] = "*sn_fit";
  fname[2] = '1' + channel;
  return fname;
}

#define pio(f,a,d,q) {\
 if (!((status = DevCamChk(CamPiow(setup->name,a,f,d,16,0),&one,q)) & 1)) return status;}
#define qrep(f,a,c,d) {\
 if (!((status = DevCamChk(CamQrepw(setup->name,a,f,c,d,16,0),&one,&one)) & 1)) return status;}
#define return_on_error(call,retstatus)\
 if (!((status = (call)) & 1)) {return retstatus;}

EXPORT int b5910a___init(struct descriptor *niddsc_ptr __attribute__ ((unused)), InInitStruct * setup)
{

  int status;
  int chan;
  struct b5910_setup {
    unsigned noc:3;
    unsigned range:2;
    unsigned status:3;
    unsigned freq:3;
    unsigned clock:1;
    unsigned iterations:4;
  } reg = {
  0, 0, 0, 0, 0, 0}, state;
  short mem_data[32768];
  //int clock_range;
  static DESCRIPTOR(dt_expr, "SLOPE_OF($)");
  static float dt;
  static DESCRIPTOR_FLOAT(dt_dsc, &dt);
  return_on_error((setup->noc * setup->samples) <= 32768, B5910A$_BAD_SAMPS);
  reg.noc = setup->noc;
  if (setup->int_clock) {
    int i;
    static float dts[] =
	{ 1. / 2000, 1. / 5000, 1. / 10000, 1. / 20000, 1. / 50000, 1. / 100000, 1. / 200000,
1. / 500000 };
    return_on_error(TdiExecute((struct descriptor *)&dt_expr, setup->int_clock, &dt_dsc MDS_END_ARG), B5910A$_BAD_CLOCK);
    for (i = 0; i < 7 && dt < dts[i]; i++) ;
    reg.freq = i;
    reg.clock = 0;
  } else
    reg.clock = 1;
  reg.iterations = setup->iterations;

  pio(26, 0, 0, 0);
  pio(24, 0, 0, &one);

  for (chan = 0; chan < reg.noc; chan++) {
    static DESCRIPTOR(chan_exp, "DATA($)");
    int mem_start = chan * 32768 / reg.noc;
    int range;
    struct descriptor *data;
    static EMPTYXD(y);
    pio(1, chan, (short *)&state, &one)
	range = state.range;
    switch (chan) {
    case 0:
      data = setup->channel_1;
      break;
    case 1:
      data = setup->channel_2;
      break;
    case 2:
      data = setup->channel_3;
      break;
    case 3:
      data = setup->channel_4;
      break;
    default:
      return DEVBAD_ACTIVE_CHAN;
    }
    status = TdiExecute((struct descriptor *)&chan_exp, data, &y MDS_END_ARG);
    if (status & 1) {
      float *yval = (float *)y.pointer->pointer;
      float volts_per_bit;
      short max_counts;
      short min_counts;
      int i;
      switch (range) {
      case 0:
	volts_per_bit = 5.00E-3;
	max_counts = 2047;
	min_counts = -2048;
	break;
      case 1:
	volts_per_bit = 2.50E-3;
	max_counts = 2047;
	min_counts = -2048;
	break;
      case 2:
	volts_per_bit = 2.50E-3;
	max_counts = 4095;
	min_counts = 0;
	break;
      case 3:
	volts_per_bit = 1.25E-3;
	max_counts = 4095;
	min_counts = 0;
	break;
      default:
	return DEVRANGE_MISMATCH;
      }
      for (i = 0; i < setup->samples; i++) {
	float counts_f = yval[i] / volts_per_bit;
	short counts = counts_f + (counts_f > 0 ? .5 : -.5);
	if (counts > max_counts)
	  counts = max_counts;
	else if (counts < min_counts)
	  counts = min_counts;
	mem_data[i + mem_start] = counts;
      }
      MdsFree1Dx(&y, 0);
    } else if (status == TreeNODATA) {
      memset(&mem_data[mem_start], 0, setup->samples * 2);
    } else {
      status = B5910A$_BAD_CHAN;
    }
  }
  pio(17, 0, &one, &one);	/* as per David Ray Bira  3/11/99 */
  pio(17, 0, (short *)&reg, &one);
  setup->samples--;
  pio(16, 2, &setup->samples, &one);
  pio(16, 1, &zero, &one);
  qrep(16, 0, 16384, &mem_data[0]);
  qrep(16, 0, 16384, &mem_data[16384]);
  pio(26, 0, 0, 0);
  if (reg.iterations == 0) {
    if (setup->start_trig) {
      float one_second = 1.0;
      LibWait(&one_second);
      pio(25, 0, 0, 0);
    }
  }
  return status;
}
