#pragma once
#include <mdsplus/mdsconfig.h>
#include <X11/Intrinsic.h>
#include <Mrm/MrmPublic.h>
#include <Xmds/XmdsWaveform.h>
#include <Xmds/XmdsWaveformP.h>
#include <Xmds/ListTreeP.h>

extern EXPORT Boolean XmdsApplyAllXds(Widget w);
extern EXPORT int XmdsBells(int *number_of_bells);
extern EXPORT void XmdsComplain(Widget parent, char *string, ...);
extern EXPORT int XmdsDeviceSetup(Widget parent, int *nid, String uids[], Cardinal num_uids, String ident,
			   MrmRegisterArglist reglist, MrmCount regnum, Widget * widget_return);
extern EXPORT int XmdsGetDeviceNid();
extern EXPORT int XmdsGetNidBooleanValue(int nid, int defaultVal);
extern EXPORT int XmdsGetNidFloatOption(int nid, float *options, int numOptions);
extern EXPORT int XmdsGetNidIntOption(int nid, int *options, int numOptions);
extern EXPORT int XmdsGetNidLongValue(int nid, int minVal, int maxVal, int defaultVal);
extern EXPORT char *XmdsGetNidText(int nid);
extern EXPORT int XmdsGetOptionIdx(Widget w);
extern EXPORT void XmdsInitialize();
extern EXPORT int XmdsIsOn(int nid);
extern EXPORT void XmdsManageWindow(Widget w);
extern EXPORT void XmdsPopupMenuPosition(Widget w, XButtonEvent * event);
extern EXPORT int XmdsPutNidScale(Widget w, int nid, int originalValue);
extern EXPORT int XmdsPutNidSText(Widget w, int nid, char *originalValue);
extern EXPORT int XmdsPutNidToggleButton(Widget w, int nid, int originalValue);
extern EXPORT int XmdsPutNidValue(int nid, unsigned short length, unsigned char dtype, char *pointer,
			   char *originalValue);
extern EXPORT void XmdsRaiseWindow(Widget w);
extern EXPORT void XmdsResetAllXds(Widget w);
extern EXPORT void XmdsSetDeviceNid(int nid);
extern EXPORT void XmdsSetOptionIdx(Widget w, int idx);
extern EXPORT int XmdsSetState(int nid, Widget w);
extern EXPORT int XmdsSetSubvalues(void *record, XtResourceList resources, Cardinal num_resources,
			    ArgList args, Cardinal argcount);
extern EXPORT Pixmap *XmdsUsageGrayIcons();
extern EXPORT Pixmap *XmdsUsageIcons();
extern EXPORT void XmdsUsageIconsInitialize(Widget w);
extern EXPORT int XmdsWidgetToNumber(Widget w, String prefix);
extern EXPORT Boolean XmdsXdsAreValid(Widget w);
extern EXPORT void XmdsWaveformPrint(Widget w, FILE * fid, int width, int height, int rotate, char *title, char *window_title, int inp_resolution);
extern EXPORT void XmdsWaveformReverse(Widget w, int reverse);
extern EXPORT void XmdsWaveformSetCrosshairs(Widget w, float *x, float *y, Boolean attach);
extern EXPORT void XmdsWaveformUpdate(Widget w, XmdsWaveformValStruct * x, XmdsWaveformValStruct * y, char *title, float *xmin, float *xmax, float *ymin, float *ymax, Boolean defer);
extern EXPORT void XmdsWaveformSetPointerMode(Widget w, int mode);
extern EXPORT Widget XmdsCreateWaveform(Widget parent, char *name, ArgList args, Cardinal argcount);
extern EXPORT void XmdsManageChildCallback(Widget w1, Widget * w2);
extern EXPORT void XmdsUnmanageChildCallback(Widget w1, Widget * w2);
extern EXPORT void XmdsRegisterWidgetCallback(Widget w1,Widget *w2);
extern EXPORT void XmdsDestroyWidgetCallback(Widget w1, Widget * w2);
extern EXPORT Widget XmdsCreateXdBoxDialog(Widget parent, char *name, ArgList args, Cardinal argcount);
extern EXPORT int XmdsDeviceSetup(Widget parent, int *nid, String uids[], Cardinal num_uids, String ident,
				  MrmRegisterArglist reglist, MrmCount regnum, Widget * widget_return);
extern EXPORT void XmdsUsageIconsInitialize(Widget w);
extern EXPORT Pixmap *XmdsUsageIcons();
extern EXPORT Pixmap *XmdsUsageGrayIcons();

EXPORT WidgetClass xmdsWaveformWidgetClass;
EXPORT XmdsWaveformClassRec xmdsWaveformClassRec;
