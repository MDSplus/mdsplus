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

 		Name:   DWScope

		Type:   C main program

		Author:	Tom Fredian

		Date:    6-JUN-1990

		Purpose: Scope replacement main program

------------------------------------------------------------------------------

	Call sequence:

$ dwcope [-default setup]

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------*/
#include <mdstypes.h>
#include <mdsplus/mdsconfig.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Mrm/MrmPublic.h>
#include <Xm/XmP.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/Scale.h>
#include <Xm/MessageB.h>
#include <Xm/SashP.h>
#include <Xm/ToggleB.h>
#include <Xm/PushBG.h>
#include <Xm/FileSB.h>
#include <Xm/ToggleBG.h>
#include <pthread.h>
#include <Xmds/XmdsWaveform.h>
#include "dwscope.h"
#include "dwscope_icon.xbm"

#ifndef _NO_DXm
#include <DXm/DXmPrint.h>
#include <DXm/DECspecific.h>
#endif

pthread_mutex_t event_mutex;

extern void XmdsInitialize();
extern void XmdsDestroyWidgetCallback();
extern void XmdsManageChildCallback();
extern void XmdsRegisterWidgetCallback();
extern void XmdsUnmanageChildCallback();

extern void SetupEvent(String event, Boolean * received, void **id);
extern void SetupEventInput(XtAppContext app_context, Widget w);
extern Boolean ConvertSelectionToWave(Widget w, Atom result_type, unsigned long length,
				      XtPointer header, WaveInfo * info);
extern Boolean ConvertWaveToSelection(Widget w, String prefix, WaveInfo * wave, Atom target,
				      Atom * type, XtPointer * header, unsigned long *length,
				      int *format);
extern Boolean ReplaceString(String * old, String new, Boolean free);
extern String GetResource(XrmDatabase db, String resource, String default_answer);
extern String WaveToText(String prefix, WaveInfo * wave, Dimension height, int *ctx);
extern void LoadDataSetup(Widget w, String title, WaveInfo * info);
extern void /*XtCallbackProc */ DisplayHelp(Widget w, XtPointer client_data,
					    XtPointer callback_data);
extern void GetDataSetup(Widget w, WaveInfo * info, int *change_mask);
extern void GetWaveFromDb(XrmDatabase db, String prefix, int row, int col, WaveInfo * info);
extern Boolean GetWaveZoomFromDb(XrmDatabase db, String prefix, int row, int col, float *zoom);
extern void ResetWave(WaveInfo * info);
extern void SetDirMask(Widget w, String * file, XtPointer callback_data);
extern XrmDatabase GetFileDatabase(String file_spec);
extern void /*XtCallbackProc */ DisableGlobalDefault(Widget w, XtPointer client_data,
						     XtPointer callback_data);
extern void /*XtCallbackProc */ InitDefaultsSetupWidget(Widget w, XtPointer client_data,
							XtPointer callback_data);
extern void /*XtCallbackProc */ ExpandReset(Widget w, int *tag, XtPointer callback_data);
extern void /*XtCallbackProc */ ExpandCancel(Widget w, int *tag, XtPointer callback_data);
extern void /*XtCallbackProc */ ExpandOk(Widget w, int *tag, XtPointer callback_data);
extern void PositionPopupMenu(Widget w, XButtonEvent * event);
extern void PopupComplaint(Widget parent, String string);

extern Boolean EvaluateData(Boolean brief, int row, int col, int index, Boolean * event,
			    String database, String shot, String default_node, String x, String y,
			    XmdsWaveformValStruct * x_ret, XmdsWaveformValStruct * y_ret,
			    String * error);
extern Boolean EvaluateText(String text, String error_prefix, String * text_ret, String * error);
extern void CloseDataSources();

static String GlobalShot();
static void RaiseWindows();
static Boolean /*XtConvertSelectionProc */ ConvertSelection(Widget w, Atom * selection,
							    Atom * target, Atom * type,
							    XtPointer * value,
							    unsigned long *length, int *format);
static int UpdateWaveform(Boolean complain, WaveInfo * info, Boolean event, int global_change_mask,
			  int change_mask);
static void /*XtCallbackProc */ GridStyle(Widget w, XtPointer client_data,
					  XmAnyCallbackStruct * cb);
static void Shrink();
static void Expand();
static void MoveVerticalPane(Widget w, XEvent * event, String * params, Cardinal * num_params);
static void Resize(Widget w, XEvent * event, String * params, Cardinal * num_params);
static void /*XtActionProc */ EqualPanes(Widget w, XEvent * event, String * string,
					 Cardinal * num_strings);
static void /*XtActionProc */ Paste(Widget w, XEvent * event, String * params,
				    Cardinal * num_params);
static void /*XtCallbackProc */ ApplyCustomizeWindow(Widget w, XtPointer client_data,
						     XmAnyCallbackStruct * callback_data);
static void /*XtCallbackProc */ ApplyCustomizePrint(Widget w, XtPointer client_data,
						    XmAnyCallbackStruct * callback_data);
static void /*XtCallbackProc */ ApplyFont(Widget w, XtPointer client_data,
					  XmSelectionBoxCallbackStruct * callback_data);
static void /*XtCallbackProc */ CreateCustomizeFont(Widget w, XtPointer client_data,
						    XmAnyCallbackStruct * callback_data);
static void /*XtCallbackProc */ Crosshairs(Widget w, XtPointer client_data,
					   XmdsWaveformCrosshairsCBStruct * cb);
static void /*XtCallbackProc */ Exit(Widget w, XtPointer client_data, XmAnyCallbackStruct * reason);
static void /*XtCallbackProc */ SetPointerMode(Widget w, int *mode, int *reason);
static void /*XtCallbackProc */ SetPointerModeMenu(Widget w, int *mode, int *reason);
static void /*XtCallbackProc */ Align(int w, XtPointer client_data, XmdsWaveformLimitsCBStruct * l);
static void /*XtCallbackProc */ Ok(Widget w, XtPointer client_data,
				   XmAnyCallbackStruct * callback_data);
static void /*XtCallbackProc */ Autoscale(Widget w, String type,
					  XmAnyCallbackStruct * callback_data);
static void /*XtCallbackProc */ Refresh(Widget w, XtPointer client_data,
					XmAnyCallbackStruct * callback_data);
static void /*XtCallbackProc */ Restore(Widget w, int *option,
					XmFileSelectionBoxCallbackStruct * reason);
static void /*XtCallbackProc */ Save(Widget w, int *option,
				     XmFileSelectionBoxCallbackStruct * reason);
static void /*XtCallbackProc */ ResetDataSetup(Widget w, int *global,
					       XmAnyCallbackStruct * callback_data);
static void /*XtCallbackProc */ ResetCustomizeFont(Widget w, XtPointer client_data,
						   XmAnyCallbackStruct * callback_data);
static void /*XtCallbackProc */ ResetCustomizePrint(Widget w, XtPointer client_data,
						    XmAnyCallbackStruct * callback_data);
static void /*XtCallbackProc */ ResetCustomizeWindow(Widget w, XtPointer client_data,
						     XmAnyCallbackStruct * callback_data);
static void /*XtCallbackProc */ ApplyDataSetup(Widget w, int *mode, XtPointer callback_data);
static void /*XtCallbackProc */ ApplyOverride(Widget w, int *mode, XtPointer callback_data);
static void /*XtCallbackProc */ Updates(Widget w, int *mode, XtPointer callback_data);
static void /*XtCallbackProc */ UpdatesMenuButton(Widget w, int *mode, XtPointer callback_data);
static void /*XtCallbackProc */ PrintAll(Widget w, XtPointer client_data,
					 XmAnyCallbackStruct * callback_data);
static void /*XtCallbackProc */ Print(Widget w, XtPointer client_data,
				      XmAnyCallbackStruct * callback_data);
static void /*XtCallbackProc */ Clear(Widget w, XtPointer client_data,
				      XmAnyCallbackStruct * callback_data);
static void /*XtCallbackProc */ Cut(Widget w, XtPointer client_data,
				    XmAnyCallbackStruct * callback_struct);
static void /*XtCallbackProc */ RegisterPane(Widget w, XtPointer client_data,
					     XmAnyCallbackStruct * callback_data);
static void /*XtCallbackProc */ RegisterWave(Widget w, XtPointer client_data,
					     XmAnyCallbackStruct * callback_data);
static void /*XtCallbackProc */ RegisterSash(Widget w, XtPointer client_data,
					     XmAnyCallbackStruct * callback_data);
static void Setup(Widget w, XtPointer client_data, XButtonEvent * event,
		  Boolean * continue_to_dispatch);
void /*XtInputCallbackProc */ EventUpdate(XtPointer client_data, int *source, XtInputId * id);
static void /*XtLoseSelectionProc */ LoseSelection(Widget w, Atom * selection);
static void /*XtSelectionCallbackProc */ PasteComplete(Widget w, WaveInfo * info, Atom * selection,
						       Atom * type, XtPointer value,
						       unsigned long *length, int *format);
static void /*XtSelectionCallbackProc */ PasteTypesComplete(Widget w, XtPointer cdata,
							    Atom * selection, Atom * type,
							    XtPointer value,
							    unsigned long *length, int *format);
static void Complain(WaveInfo * info, char mode, String error);
static void CopyWave(WaveInfo * in, WaveInfo * out);
static void FreeWave(WaveInfo * info);
static void Busy();
static void ManageWaveforms();
static void RemoveZeros(String string, int *length);
static void Unbusy();
static void ClearWaveform(WaveInfo * info);
static void RestoreDatabase(String dbname, Widget w);
static void WriteDatabase(String dbname, Boolean zoom);
static void GetNewLimits(WaveInfo * info, float **xmin, float **xmax, float **ymin, float **ymax);
static void SetWindowTitles();
static WaveInfo *GetPending(Widget w);
static Window CreateBusyWindow(Widget w);
static String SetupTitle();
static XrmDatabase MdsGetFileDatabase(String file_spec);
static char *GetPrinterList();

#define min(a,b) ( ((a)<(b)) ? (a) : (b) )
#define max(a,b) ( ((a)>(b)) ? (a) : (b) )
#define PlotsWidget XtNameToWidget(MainWidget,"plots")

#define MaxCols 4
#define MaxRows 16

#define XA_DWSCOPE_PANEL XInternAtom(XtDisplay(TopWidget), "DWSCOPE_PANEL", 0)
#define XA_TARGETS XInternAtom(XtDisplay(TopWidget), "TARGETS", 0)

static Widget TopWidget;
static Widget MainWidget;
static Widget LockScalesWidget = 0;
static Widget DataSetupWidget;
static Widget DefaultsSetupWidget;
static Widget Pane[MaxCols];
static Widget Sash[MaxCols - 1];
static Widget CustomizeWindowWidget;
static Widget CustomizePrintWidget = 0;
static Widget CustomizeFontWidget = 0;
static Widget Button3Widget;

static String ScopeTitle;
static String ScopeIcon;
static int NumWaves = 0;
static int NumPanes = 0;
static int NumSashs = 0;
static WaveInfo Wave[MaxCols][MaxRows];
static WaveInfo *CurrentWave;
static WaveInfo *PendingWave;
static WaveInfo *SelectedWave = 0;
static WaveInfo GlobalWave;
static Boolean CloseDataSourcesEventReceived = 0;
static Boolean ScopePrintEventReceived = 0;
static Boolean ScopeTitleEventReceived = 0;
static String ScopePrintEvent = 0;
static String ScopeTitleEvent = 0;
static String ScopePrintFile = 0;
static Boolean ScopePrintPortrait = 0;
static String ScopePrinter = 0;
static int ScopeTempFileIdx = 0;
static int ScopePrintWindowTitle = 0;
static int ScopePrintToFile = 0;
static void *ScopePrintEventId = 0;
static void *ScopeTitleEventId = 0;
static int pointermode = XmdsPOINTER_MODE_ZOOM;
static int Rows[MaxCols];
static int Columns;
static Window BusyWindow;
static int MenusHeight;
static int ApplyStatus;
static int BusyLevel = 0;
static Boolean PreventResize = FALSE;
static String defaultfile;
static Boolean UpdatesOn = TRUE;
static Boolean UpdateWhenExpand = FALSE;
static Widget DeltaWaveWidget = 0;
static Widget MarkWidget = 0;
static float DeltaX;
static float DeltaY;
static XtAppContext AppContext;
static XtWorkProcId UpdateWaveformsWorkProcID;

int main(int argc, String * argv)
{
  int i;
  static String hierarchy_names[] = { "dwscope.uid" };
  String fallback_resources[] = { "*default_label.labelString: Default node:",
    "*exp_label.labelString: Experiment:",
    "*brief_errors.set: true", NULL
  };
  static MrmRegisterArg register_list[] = {
    {"Align", (XtPointer) Align},
    {"ApplyCustomizePrint", (XtPointer) ApplyCustomizePrint},
    {"ApplyCustomizeWindow", (XtPointer) ApplyCustomizeWindow},
    {"ApplyDataSetup", (XtPointer) ApplyDataSetup},
    {"ApplyFont", (XtPointer) ApplyFont},
    {"ApplyOverride", (XtPointer) ApplyOverride},
    {"Autoscale", (XtPointer) Autoscale},
    {"Clear", (XtPointer) Clear},
    {"Close", (XtPointer) CloseDataSources},
    {"CreateCustomizeFont", (XtPointer) CreateCustomizeFont},
    {"Crosshairs", (XtPointer) Crosshairs},
    {"Cut", (XtPointer) Cut},
    {"DisableGlobalDefault", (XtPointer) DisableGlobalDefault},
    {"Exit", (XtPointer) Exit},
    {"InitDefaultsSetupWidget", (XtPointer) InitDefaultsSetupWidget},
    {"Ok", (XtPointer) Ok},
    {"Paste", (XtPointer) Paste},
    {"Print", (XtPointer) Print},
    {"PrintAll", (XtPointer) PrintAll},
    {"Refresh", (XtPointer) Refresh},
    {"RegisterPane", (XtPointer) RegisterPane},
    {"RegisterSash", (XtPointer) RegisterSash},
    {"RegisterWave", (XtPointer) RegisterWave},
    {"ResetCustomizePrint", (XtPointer) ResetCustomizePrint},
    {"ResetCustomizeFont", (XtPointer) ResetCustomizeFont},
    {"ResetCustomizeWindow", (XtPointer) ResetCustomizeWindow},
    {"ResetDataSetup", (XtPointer) ResetDataSetup},
    {"Restore", (XtPointer) Restore},
    {"Save", (XtPointer) Save},
    {"DisplayHelp", (XtPointer) DisplayHelp},
    {"SetPointerMode", (XtPointer) SetPointerMode},
    {"SetPointerModeMenu", (XtPointer) SetPointerModeMenu},
    {"Button3Widget", (XtPointer) & Button3Widget},
    {"CustomizeWindowWidget", (XtPointer) & CustomizeWindowWidget},
    {"CustomizeFontWidget", (XtPointer) & CustomizeFontWidget},
    {"CustomizePrintWidget", (XtPointer) & CustomizePrintWidget},
    {"DataSetupWidget", (XtPointer) & DataSetupWidget},
    {"DefaultsSetupWidget", (XtPointer) & DefaultsSetupWidget},
    {"expand_ok", (XtPointer) ExpandOk},
    {"expand_reset", (XtPointer) ExpandReset},
    {"expand_cancel", (XtPointer) ExpandCancel},
    {"Updates", (XtPointer) Updates},
    {"UpdatesMenuButton", (XtPointer) UpdatesMenuButton},
    {"GridStyle", (XtPointer) GridStyle},
    {"XmdsDestroyWidgetCallback", (XtPointer) XmdsDestroyWidgetCallback},
    {"XmdsManageChildCallback", (XtPointer) XmdsManageChildCallback},
    {"XmdsRegisterWidgetCallback", (XtPointer) XmdsRegisterWidgetCallback},
    {"XmdsUnmanageChildCallback", (XtPointer) XmdsUnmanageChildCallback}
  };

  static XtActionsRec actions[] = { {"MoveVerticalPane", MoveVerticalPane},
  {"EqualPanes", EqualPanes},
  {"Resize", Resize},
  {"Shrink", Shrink},
  {"Expand", Expand}
  };
  MrmType class;
  typedef struct {
    String defaultfile;
    Boolean icon_update;
  } AppResourcesRec;
  static AppResourcesRec appRes;
  static XrmOptionDescRec options[] = { {"-defaults", "*defaults", XrmoptionSepArg, NULL},
  {"-icon_update", "*icon_update", XrmoptionNoArg, "True"}
  };
  static XtResource resources[] =
      { {"defaults", "Defaults", XtRString, sizeof(String),
	 XtOffsetOf(AppResourcesRec, defaultfile), XtRString, "*"}
  ,
  {"icon_update", "Icon_update", XtRBoolean, sizeof(Boolean),
   XtOffsetOf(AppResourcesRec, icon_update), XtRImmediate, (XtPointer) False}
  };
  Cursor cursor;
  int r;
  int c;
  void *cds_id = 0;
  MrmHierarchy drm_hierarchy;
  char *printers = GetPrinterList();
  pthread_mutex_init(&event_mutex, NULL);
  MrmInitialize();

#ifndef _NO_DXm
  DXmInitialize();
#endif
  XmdsInitialize();

  MrmRegisterClass(MrmwcUnknown, "XmdsWaveformWidgetClass", "XmdsCreateWaveform",
		   (Widget (*)(void))XmdsCreateWaveform, xmdsWaveformWidgetClass);
  MrmRegisterNames(register_list, XtNumber(register_list));
  TopWidget =
      XtVaAppInitialize(&AppContext, "DwScope", options, XtNumber(options), &argc, argv,
			fallback_resources, XmNallowShellResize, 1, XmNminWidth, 320, XmNminHeight,
			100, NULL);
  XtGetApplicationResources(TopWidget, &appRes, resources, XtNumber(resources), (Arg *) NULL, 0);
  defaultfile = appRes.defaultfile;
  defaultfile = strlen(defaultfile) ? XtNewString(defaultfile) : XtNewString("my.scope");
  XtAppAddActions(AppContext, actions, XtNumber(actions));
  XtAugmentTranslations(TopWidget, XtParseTranslationTable("#augment <ResizeRequest> : Resize() \n\
                                                                     <Unmap> : Shrink()\n\
                                                                     <Map> : Expand()"));
  MrmOpenHierarchy(XtNumber(hierarchy_names), hierarchy_names, 0, &drm_hierarchy);
  MrmFetchWidget(drm_hierarchy, "scope", TopWidget, &MainWidget, &class);
  MrmCloseHierarchy(drm_hierarchy);
  if (!MainWidget) {
    printf("Problem loading UID\n");
    exit(1);
  }
  LockScalesWidget = XtNameToWidget(MainWidget, "*lockScales");
  if (appRes.icon_update) {
    XmToggleButtonGadgetSetState(XtNameToWidget(TopWidget, "*disable_icon_updates"), False, False);
  }
  SetupEventInput(AppContext, TopWidget);
  SetDirMask(XtNameToWidget(TopWidget, "*file_dialog"), &defaultfile, 0);

  XtVaSetValues(Pane[0], XmNleftAttachment, XmATTACH_FORM, NULL);
  for (c = 1; c < NumPanes; c++)
    XtVaSetValues(Pane[c], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET, XmNleftWidget, Sash[c - 1],
		  XmNleftOffset, 2, NULL);
  if (printers) {
    Widget printers_pulldown = XtNameToWidget(MainWidget, "*printer_select_pulldown");
    Arg arglist[] = { {XmNlabelString, 0}
    };
    int num;
    Widget *children;
    char *printer;
    for (printer = strtok(printers, "\n"); printer; printer = strtok(0, "\n")) {
      arglist[0].value = (long)XmStringCreateSimple(printer);
      XmCreatePushButtonGadget(printers_pulldown, printer, arglist, XtNumber(arglist));
    }
    XtVaGetValues(printers_pulldown, XmNnumChildren, &num, XmNchildren, &children, NULL);
    XtManageChildren(children, num);
  }
  XtManageChild(MainWidget);
  RestoreDatabase(defaultfile, MainWidget);
  cursor = XCreateFontCursor(XtDisplay(TopWidget), XC_crosshair);
  for (i = 0; i < NumSashs; i++)
    XDefineCursor(XtDisplay(TopWidget), XtWindow(Sash[i]), cursor);
  for (c = 0; c < MaxCols; c++)
    for (r = 0; r < MaxRows; r++) {
      int p_r = (r + 1) % MaxRows;
      int p_c = p_r ? c : (c + 1) % MaxCols;
      XtVaSetValues(Wave[c][r].w, XmdsNpanWith, Wave[p_c][p_r].w, NULL);
    }
  BusyWindow = CreateBusyWindow(PlotsWidget);
  XtAddEventHandler(PlotsWidget, ButtonPressMask | ButtonReleaseMask, False, (XtEventHandler) Setup,
		    0);
  XtVaSetValues(TopWidget, XtNiconPixmap,
		XCreatePixmapFromBitmapData(XtDisplay(TopWidget), XtWindow(TopWidget),
					    (char *)dwscope_icon_bits, dwscope_icon_width,
					    dwscope_icon_height,
					    BlackPixelOfScreen(XtScreen(TopWidget)),
					    WhitePixelOfScreen(XtScreen(TopWidget)), 1), NULL);
  SetupEvent("DWSCOPE_CLOSE_FILES", &CloseDataSourcesEventReceived, &cds_id);
  XmProcessTraversal(XtNameToWidget(MainWidget, "*override_shot"), XmTRAVERSE_CURRENT);
  XtAppMainLoop(AppContext);
  return 0;
}

#include <errno.h>
#include <mdsdescrip.h>

static void DoPrint(char *filename)
{
  char cmd[512];
  int num = snprintf(cmd, sizeof(cmd), "dwscopePrint %s %s", filename, ScopePrinter);
  if (num > 0 && num <= (int)sizeof(cmd))
    if (system(cmd) != 0)
      printf("Error invoking dwscopePrint\n");
}

static char *GetPrinterList()
{
  char *printers = 0;
  FILE *fp = popen("ScopePrinters", "r");
  if (fp != NULL) {
    char buff[256];
    size_t bytes;
    while ((bytes = fread(buff, 1, sizeof(buff) - 1, fp))) {
      buff[bytes] = 0;
      if (printers) {
	printers = realloc(printers, strlen(printers) + 1 + bytes);
	strcat(printers, buff);
      } else {
	printers = strcpy(malloc(bytes + 1), buff);
      }
    }
  }
  pclose(fp);
  return printers;
}

static void Shrink(Widget w, XEvent * event __attribute__ ((unused)))
{
  if (XmToggleButtonGadgetGetState(XtNameToWidget(w, "*disable_icon_updates"))) {
    UpdateWhenExpand = XmToggleButtonGadgetGetState(XtNameToWidget(w, "*updates"));
    XmToggleButtonGadgetSetState(XtNameToWidget(w, "*updates"), FALSE, TRUE);
  }
}

static void Expand(Widget w, XEvent * event __attribute__ ((unused)))
{
  if (XmToggleButtonGadgetGetState(XtNameToWidget(w, "*disable_icon_updates")) && UpdateWhenExpand)
    XmToggleButtonGadgetSetState(XtNameToWidget(w, "*updates"), TRUE, TRUE);
}

static void /*XtCallbackProc */ UpdatesMenuButton(Widget w, int *mode __attribute__ ((unused)),
						  XtPointer callback_data __attribute__ ((unused)))
{
  XmToggleButtonGadgetSetState(XtNameToWidget(MainWidget, "*updates"),
			       !XmToggleButtonGadgetGetState(w), TRUE);
}

static String XYString(float x, float y)
{
  static char label[42] = { '[' };
  int length;
  String ptr = (String) & label[1];
  sprintf(ptr, "%g%n", x, &length);
  RemoveZeros(ptr + 1, &length);
  ptr[length] = ',';
  ptr = &ptr[length + 1];
  sprintf(ptr, "%g%n", y, &length);
  RemoveZeros(ptr, &length);
  ptr[length] = ']';
  ptr[length + 1] = 0;
  return (String) label;
}

void PositionMark()
{
  if (!MarkWidget)
    MarkWidget = XtNameToWidget(MainWidget, "*crosshairs_mark");
  if (DeltaWaveWidget && XtIsManaged(DeltaWaveWidget)) {
    float *xmin, *xmax, *ymin, *ymax;
    Position plots_rootx, plots_rooty;
    Position wave_rootx, wave_rooty;
    Position x, y;
    Position xpos, ypos;
    XtVaGetValues(DeltaWaveWidget, XmdsNxMin, &xmin, XmdsNxMax, &xmax, XmdsNyMin, &ymin, XmdsNyMax,
		  &ymax, NULL);
    XtTranslateCoords(DeltaWaveWidget, 0, 0, &wave_rootx, &wave_rooty);
    XtTranslateCoords(PlotsWidget, 0, 0, &plots_rootx, &plots_rooty);
    x = wave_rootx - plots_rootx;
    y = wave_rooty - plots_rooty;
    xpos =
	((DeltaX - *xmin) / (*xmax - *xmin) * XtWidth(DeltaWaveWidget) + x -
	 5) / XtWidth(PlotsWidget) * 1000;
    ypos =
	((*ymax - DeltaY) / (*ymax - *ymin) * XtHeight(DeltaWaveWidget) + y -
	 5) / XtHeight(PlotsWidget) * 1000;
    if (xpos < 0 || ypos < 0 || xpos > 1000 || ypos > 1000)
      XtUnmanageChild(MarkWidget);
    else {
      XtVaSetValues(MarkWidget, XmNleftPosition, xpos, XmNtopPosition, ypos, NULL);
      XtManageChild(MarkWidget);
    }
  } else
    XtUnmanageChild(MarkWidget);
}

static void /*XtCallbackProc */ Crosshairs(Widget w, XtPointer client_data __attribute__ ((unused)),
					   XmdsWaveformCrosshairsCBStruct * cb)
{
  static Widget value_w = 0;
  int r;
  int c;
  float display_x;
  float display_y;
  String label;
  if (!value_w)
    value_w = XtNameToWidget(MainWidget, "*crosshairs_value");
  if (cb->event->xbutton.type == ButtonPress) {
    if (cb->event->xbutton.state & ShiftMask) {
      DeltaWaveWidget = DeltaWaveWidget ? 0 : w;
      if (DeltaWaveWidget) {
	DeltaX = cb->x;
	DeltaY = cb->y;
      }
      PositionMark();
    }
  }
  for (c = 0; c < MaxCols; c++)
    for (r = 0; r < MaxRows; r++)
      if (Wave[c][r].w != w)
	XmdsWaveformSetCrosshairs(Wave[c][r].w, &cb->x, &cb->y, 0);
  display_x = cb->x;
  display_y = cb->y;
  if (DeltaWaveWidget) {
    display_x -= DeltaX;
    display_y -= DeltaY;
  }
  label = XYString(display_x, display_y);
  XmTextFieldSetString(value_w, label);
  if (cb->event->xbutton.type == ButtonRelease)
    XmTextFieldSetSelection(value_w, 0, strlen(label), cb->event->xbutton.time);
}

static void /*XtCallbackProc */ Exit(Widget w __attribute__ ((unused)),
				     XtPointer client_data __attribute__ ((unused)),
				     XmAnyCallbackStruct * reason __attribute__ ((unused)))
{
  exit(1);
}

static void /*XtCallbackProc */ SetPointerMode(Widget w, int *mode, int *reason __attribute__ ((unused)))
{
  if (XmToggleButtonGetState(w)) {
    int r;
    int c;
    pointermode = *mode;
    for (c = 0; c < MaxCols; c++)
      for (r = 0; r < MaxRows; r++)
	XmdsWaveformSetPointerMode(Wave[c][r].w, pointermode);
    if (pointermode == XmdsPOINTER_MODE_POINT && DeltaWaveWidget)
      PositionMark();
    else if (MarkWidget)
      XtUnmanageChild(MarkWidget);
  }
}

static void /*XtCallbackProc */ SetPointerModeMenu(Widget w __attribute__ ((unused)),
						   int *mode, int *reason __attribute__ ((unused)))
{
  int r;
  int c;
  switch (*mode) {
  case XmdsPOINTER_MODE_POINT:
    XmToggleButtonSetState(XtNameToWidget(MainWidget, "*Point"), 1, 1);
    break;
  case XmdsPOINTER_MODE_ZOOM:
    XmToggleButtonSetState(XtNameToWidget(MainWidget, "*Zoom"), 1, 1);
    break;
  case XmdsPOINTER_MODE_DRAG:
    XmToggleButtonSetState(XtNameToWidget(MainWidget, "*Drag"), 1, 1);
    break;
  case XmdsPOINTER_MODE_EDIT:
    XmToggleButtonSetState(XtNameToWidget(MainWidget, "*Copy"), 1, 1);
    break;
  default:
    pointermode = *mode;
    for (c = 0; c < MaxCols; c++)
      for (r = 0; r < MaxRows; r++)
	XmdsWaveformSetPointerMode(Wave[c][r].w, pointermode);
  }
}

static void RemoveZeros(String string, int *length)
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

static void /*XtCallbackProc */ Align(int w __attribute__ ((unused)),
				      XtPointer client_data __attribute__ ((unused)), XmdsWaveformLimitsCBStruct * l)
{
  int r;
  int c;
  for (c = 0; c < MaxCols; c++)
    for (r = 0; r < MaxRows; r++)
      XtVaSetValues(Wave[c][r].w, XmdsNxMin, l->xminval, XmdsNxMax, l->xmaxval, NULL);
}

static void /*XtCallbackProc */ GridStyle(Widget w __attribute__ ((unused)),
					  XtPointer client_data __attribute__ ((unused)),
					  XmAnyCallbackStruct * cb __attribute__ ((unused)))
{
  int lines = XmToggleButtonGadgetGetState(w);
  int r;
  int c;
  for (c = 0; c < MaxCols; c++)
    for (r = 0; r < MaxRows; r++)
      XtVaSetValues(Wave[c][r].w, XmdsNgridStyle,
		    lines ? XmdsGRID_STYLE_LINES : XmdsGRID_STYLE_TICKS, NULL);
}

static void /*XtCallbackProc */ Ok(Widget w, XtPointer client_data __attribute__ ((unused)),
				   XmAnyCallbackStruct * callback_data __attribute__ ((unused)))
{
  if (ApplyStatus & 1)
    XtUnmanageChild(XtParent(w));
}

static String SetupTitle()
{
  int c;
  int r;
  static char title[80];
  for (c = 0; c < Columns; c++)
    for (r = 0; r < Rows[c]; r++)
      if (&Wave[c][r] == CurrentWave)
	goto found;
  strcpy(title, "Global Default Settings");
  return title;
 found:
  sprintf(title, "Setup for plot at row %d, column %d", r + 1, c + 1);
  return title;
}

static void /*XtCallbackProc */ Autoscale(Widget w __attribute__ ((unused)), String type,
					  XmAnyCallbackStruct * callback_data __attribute__ ((unused)))
{
  int r;
  int c;
  switch (type[0]) {
  case 'y':
    XtVaSetValues(PendingWave->w, XmdsNyMin, NULL, XmdsNyMax, NULL, NULL);
    break;
  case 'x':
    XtVaSetValues(PendingWave->w, XmdsNxMin, NULL, XmdsNxMax, NULL, NULL);
    break;
  case 'b':
    XtVaSetValues(PendingWave->w, XmdsNxMin, NULL, XmdsNxMax, NULL, XmdsNyMin, NULL, XmdsNyMax,
		  NULL, NULL);
    break;
  case 'Y':
    for (c = 0; c < MaxCols; c++)
      for (r = 0; r < MaxRows; r++)
	XtVaSetValues(Wave[c][r].w, XmdsNyMin, NULL, XmdsNyMax, NULL, NULL);
    break;
  case 'X':
    for (c = 0; c < MaxCols; c++)
      for (r = 0; r < MaxRows; r++)
	XtVaSetValues(Wave[c][r].w, XmdsNxMin, NULL, XmdsNxMax, NULL, NULL);
    break;
  case 'B':
    for (c = 0; c < MaxCols; c++)
      for (r = 0; r < MaxRows; r++)
	XtVaSetValues(Wave[c][r].w, XmdsNxMin, NULL, XmdsNxMax, NULL, XmdsNyMin, NULL, XmdsNyMax,
		      NULL, NULL);
    break;
  case '0':
    {
      float *xMin;
      float *xMax;
      float *yMin;
      float *yMax;
      XtVaGetValues(PendingWave->w, XmdsNxMin, &xMin, XmdsNxMax, &xMax, XmdsNyMin, &yMin, XmdsNyMax,
		    &yMax, NULL);
      for (c = 0; c < MaxCols; c++)
	for (r = 0; r < MaxRows; r++)
	  if (PendingWave->w != Wave[c][r].w)
	    XtVaSetValues(Wave[c][r].w, XmdsNxMin, xMin, XmdsNxMax, xMax, XmdsNyMin, yMin,
			  XmdsNyMax, yMax, NULL);
      break;
    }
  case '1':
    {
      float *xMin;
      float *xMax;
      float *yMin;
      float *yMax;
      XtVaGetValues(PendingWave->w, XmdsNxMin, &xMin, XmdsNxMax, &xMax, XmdsNyMin, &yMin, XmdsNyMax,
		    &yMax, NULL);
      for (c = 0; c < MaxCols; c++)
	for (r = 0; r < MaxRows; r++)
	  if (PendingWave->w != Wave[c][r].w)
	    XtVaSetValues(Wave[c][r].w, XmdsNxMin, xMin, XmdsNxMax, xMax, NULL);
      break;
    }
  case '2':
    {
      float *xMin;
      float *xMax;
      float *yMin;
      float *yMax;
      XtVaGetValues(PendingWave->w, XmdsNxMin, &xMin, XmdsNxMax, &xMax, XmdsNyMin, &yMin, XmdsNyMax,
		    &yMax, NULL);
      for (c = 0; c < MaxCols; c++)
	for (r = 0; r < MaxRows; r++)
	  if (PendingWave->w != Wave[c][r].w)
	    XtVaSetValues(Wave[c][r].w, XmdsNyMin, yMin, XmdsNyMax, yMax, NULL);
      break;
    }
  case '3':
    {
      float *xMin;
      float *xMax;
      float *yMin;
      float *yMax;
      XtVaGetValues(PendingWave->w, XmdsNxMin, &xMin, XmdsNxMax, &xMax, XmdsNyMin, &yMin, XmdsNyMax,
		    &yMax, NULL);
      for (c = 0; c < MaxCols; c++)
	for (r = 0; r < MaxRows; r++)
	  XtVaSetValues(Wave[c][r].w, XmdsNxMin, xMin, XmdsNxMax, xMax, XmdsNyMin, NULL, XmdsNyMax,
			NULL, NULL);
      break;
    }
  case '4':
    {
      float *xMin;
      float *xMax;
      float *yMin;
      float *yMax;
      Boolean update = PendingWave->update;
      PendingWave->update = TRUE;
      GetNewLimits(PendingWave, &xMin, &xMax, &yMin, &yMax);
      PendingWave->update = update;
      XtVaSetValues(PendingWave->w, XmdsNxMin, xMin, XmdsNxMax, xMax, XmdsNyMin, yMin, XmdsNyMax,
		    yMax, NULL);
      break;
    }
  case '5':
    {
      float *xMin;
      float *xMax;
      float *yMin;
      float *yMax;
      for (c = 0; c < MaxCols; c++)
	for (r = 0; r < MaxRows; r++) {
	  Boolean update = Wave[c][r].update;
	  Wave[c][r].update = TRUE;
	  GetNewLimits(&Wave[c][r], &xMin, &xMax, &yMin, &yMax);
	  Wave[c][r].update = update;
	  XtVaSetValues(Wave[c][r].w, XmdsNxMin, xMin, XmdsNxMax, xMax, XmdsNyMin, yMin, XmdsNyMax,
			yMax, NULL);
	}
      break;
    }
  }
  PositionMark();
}

static void /*XtCallbackProc */ Refresh(Widget w __attribute__ ((unused)), XtPointer client_data __attribute__ ((unused)),
					XmAnyCallbackStruct * callback_data __attribute__ ((unused)))
{
  UpdateWaveform(0, PendingWave, 0, -1, -1);
}

static void /*XtCallbackProc */ Restore(Widget w, int *option,
					XmFileSelectionBoxCallbackStruct * reason)
{
  int opt = option ? *option : 0;
  switch (opt) {
  case 0:
    {
      if (reason->length) {
	String filename;
	filename = XmStringUnparse(reason->value, NULL, 0, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
	if (filename) {
	  int length = strlen(filename);
	  if (length) {
	    RestoreDatabase(filename, w);
	    if (defaultfile)
	      XtFree(defaultfile);
	    defaultfile = filename;
	  } else
	    XtFree(filename);
	}
      }
      break;
    }
  case 1:
    RestoreDatabase(defaultfile, TopWidget);
    break;
  case 2:
    {
      Widget w = XtNameToWidget(TopWidget, "*file_dialog");
      XmString title = XmStringCreateSimple("Restore Current Settings From");
      XmString label = XmStringCreateSimple("Select Name of Configuration File: ");
      XmString deffile = XmStringCreateSimple(defaultfile);
      XmString dirmask;
      XtRemoveAllCallbacks(w, XmNokCallback);
      XtAddCallback(w, XmNokCallback, (XtCallbackProc) Restore, 0);
      SetDirMask(XtNameToWidget(TopWidget, "*file_dialog"), &defaultfile, 0);
      XtVaGetValues(w, XmNdirMask, &dirmask, NULL);
      XmFileSelectionDoSearch(w, dirmask);
      XtVaSetValues(w, XmNdialogTitle, title, XmNselectionLabelString, label, XmNtextString,
		    deffile, NULL);
      XtManageChild(w);
      XmStringFree(title);
      XmStringFree(label);
      XmStringFree(deffile);
      break;
    }
  }
}

static void /*XtCallbackProc */ Save(Widget w __attribute__ ((unused)), int *option,
				     XmFileSelectionBoxCallbackStruct * reason)
{
  int opt = option ? *option : 0;
  switch (opt) {
  case 0:
    {
      if (reason->length) {
	String filename;
	filename = XmStringUnparse(reason->value, NULL, 0, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
	if (filename) {
	  int length = strlen(filename);
	  if (length) {
	    WriteDatabase(filename, FALSE);
	    if (defaultfile)
	      XtFree(defaultfile);
	    defaultfile = filename;
	  } else
	    XtFree(filename);
	}
      }
      break;
    }
  case 1:
    WriteDatabase(defaultfile, FALSE);
    break;
  case 2:
    {
      Widget w = XtNameToWidget(TopWidget, "*file_dialog");
      XmString title = XmStringCreateSimple("Save Current Settings As");
      XmString label = XmStringCreateSimple("Select Name for Configuration File: ");
      XmString deffile = XmStringCreateSimple(defaultfile);
      XmString dirmask;
      XtRemoveAllCallbacks(w, XmNokCallback);
      XtAddCallback(w, XmNokCallback, (XtCallbackProc) Save, 0);
      XtVaGetValues(w, XmNdirMask, &dirmask, NULL);
      XmFileSelectionDoSearch(w, dirmask);
      XtVaSetValues(w, XmNdialogTitle, title, XmNselectionLabelString, label, XmNtextString,
		    deffile, NULL);
      XtManageChild(w);
      XmStringFree(title);
      XmStringFree(label);
      XmStringFree(deffile);
      break;
    }
  case 3:
    {
      if (reason->length) {
	String filename;
	filename = XmStringUnparse(reason->value, NULL, 0, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
	if (filename) {
	  int length = strlen(filename);
	  if (length) {
	    WriteDatabase(filename, TRUE);
	    if (defaultfile)
	      XtFree(defaultfile);
	    defaultfile = filename;
	  } else
	    XtFree(filename);
	}
      }
      break;
    }
  case 4:
    WriteDatabase(defaultfile, TRUE);
    break;
  case 5:
    {
      Widget w = XtNameToWidget(TopWidget, "*file_dialog");
      XmString title = XmStringCreateSimple("Save Current Settings As");
      XmString label = XmStringCreateSimple("Select Name for Configuration File: ");
      XmString deffile = XmStringCreateSimple(defaultfile);
      XmString dirmask;
      static int option = 3;
      XtRemoveAllCallbacks(w, XmNokCallback);
      XtAddCallback(w, XmNokCallback, (XtCallbackProc) Save, (void *)&option);
      XtVaGetValues(w, XmNdirMask, &dirmask, NULL);
      XmFileSelectionDoSearch(w, dirmask);
      XtVaSetValues(w, XmNdialogTitle, title, XmNselectionLabelString, label, XmNtextString,
		    deffile, NULL);
      XtManageChild(w);
      XmStringFree(title);
      XmStringFree(label);
      XmStringFree(deffile);
      break;
    }
  }
}

static void ManageWaveforms()
{
  int row;
  int col;
  int max_rows = 0;
  static XtTranslations translations = 0;
  if (!translations)
    translations = XtParseTranslationTable("Shift<Btn1Up>:EqualPanes(H)\n <Btn2Up>:EqualPanes(H)");
  for (col = 0; col < Columns; col++) {
    Widget w[MaxRows];
    if (Rows[col] > max_rows)
      max_rows = Rows[col];
    for (row = Rows[col]; row < MaxRows; row++) {
      if (&Wave[col][row] == SelectedWave)
	XtDisownSelection(SelectedWave->w, XA_PRIMARY,
			  XtLastTimestampProcessed(XtDisplay(SelectedWave->w)));
    }
    for (row = 0; row < MaxRows; row++)
      w[row] = Wave[col][row].w;
    XtManageChildren(w, Rows[col]);
    if (Rows[col] < MaxRows)
      XtUnmanageChildren(&w[Rows[col]], MaxRows - Rows[col]);
  }
  for (col = 0; col < Columns - 1; col++)
    XtVaSetValues(Pane[col], XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET, XmNrightWidget,
		  Sash[col], XmNrightOffset, 2, NULL);
  XtVaSetValues(Pane[Columns - 1], XmNrightAttachment, XmATTACH_FORM, NULL);
  if (Columns > 1)
    XtManageChildren(Sash, Columns - 1);
  XtManageChildren(Pane, Columns);
  for (col = 0; col < Columns; col++) {
    int i;
    int num;
    Widget *child;
    XtVaGetValues(Pane[col], XmNnumChildren, &num, XmNchildren, &child, NULL);
    for (i = 0; i < num; i++)
      if (XmIsSash(child[i]))
	XtOverrideTranslations(child[i], translations);
  }
  XtVaSetValues(TopWidget, XmNminHeight, max_rows * 7 + MenusHeight + 20, NULL);
  if (XtIsRealized(TopWidget))
    RaiseWindows();
}

static void RaiseWindows()
{
  int col;
  for (col = 0; col < Columns - 1; col++)
    XRaiseWindow(XtDisplay(Sash[col]), XtWindow(Sash[col]));
  XRaiseWindow(XtDisplay(MainWidget), XtWindow(XtNameToWidget(TopWidget, "*pane_separator")));
}

static void /*XtCallbackProc */ ResetDataSetup(Widget w __attribute__ ((unused)), int *global,
					       XmAnyCallbackStruct * callback_data __attribute__ ((unused)))
{
  if (*global == 1)
    PendingWave = &GlobalWave;
  CurrentWave = PendingWave;
  if (CurrentWave == &GlobalWave)
    XtUnmanageChild(XtNameToWidget(DataSetupWidget, "setup_defaults"));
  else
    XtManageChild(XtNameToWidget(DataSetupWidget, "setup_defaults"));
  if (*global == 2)
    ClearWaveform(CurrentWave);
  LoadDataSetup(DataSetupWidget, SetupTitle(), CurrentWave);
}

static void /*XtCallbackProc */ ResetCustomizeWindow(Widget w __attribute__ ((unused)),
						     XtPointer client_data __attribute__ ((unused)),
						     XmAnyCallbackStruct * callback_data __attribute__ ((unused)))
{
  int i;
  XmTextSetString(XtNameToWidget(CustomizeWindowWidget, "window_title"), ScopeTitle);
  XmTextSetString(XtNameToWidget(CustomizeWindowWidget, "icon_name"), ScopeIcon);
  XmTextSetString(XtNameToWidget(CustomizeWindowWidget, "title_event"), ScopeTitleEvent);
  for (i = 0; i < MaxCols; i++) {
    char name[8];
    sprintf(name, "rows_%d", i + 1);
    XmScaleSetValue(XtNameToWidget(CustomizeWindowWidget, name), Rows[i]);
  }
}

#include <signal.h>

static void /*XtCallbackProc */ ResetCustomizePrint(Widget w __attribute__ ((unused)),
						    XtPointer client_data __attribute__ ((unused)),
						    XmAnyCallbackStruct * callback_data __attribute__ ((unused)))
{
  Widget *children;
  int numchildren;
  int i;
  Widget portrait_w;
  XmTextSetString(XtNameToWidget(CustomizePrintWidget, "print_file"), ScopePrintFile);
  XmTextSetString(XtNameToWidget(CustomizePrintWidget, "print_event"), ScopePrintEvent);
  XmToggleButtonSetState(XtNameToWidget(CustomizePrintWidget, "print_window_title"),
			 ScopePrintWindowTitle, 1);
  portrait_w = XtNameToWidget(CustomizePrintWidget, "cp_portrait_button");
  if (portrait_w)
    XmToggleButtonSetState(portrait_w, ScopePrintPortrait, 1);
  XtVaGetValues(XtNameToWidget(CustomizePrintWidget, "*printer_select_pulldown"), XmNnumChildren,
		&numchildren, XmNchildren, &children, NULL);
  for (i = 0; i < numchildren; i++) {
    XmString label;
    String label_string;
    XtVaGetValues(children[i], XmNlabelString, &label, NULL);
    label_string = XmStringUnparse(label, NULL, 0, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
    if (strcmp(label_string, ScopePrinter) == 0) {
      XtFree(label_string);
      XtVaSetValues(XtNameToWidget(CustomizePrintWidget, "*printer_select"), XmNmenuHistory,
		    children[i], NULL);
      break;
    } else
      XtFree(label_string);
  }
  if ((i > numchildren) && numchildren > 0)
    XtVaSetValues(XtNameToWidget(CustomizePrintWidget, "*printer_select"), XmNmenuHistory,
		  children[numchildren - 1], NULL);

/*
  XmToggleButtonSetState(XtNameToWidget(CustomizePrintWidget,"print_to_file"),ScopePrintToFile,1);
*/
}

static void /*XtCallbackProc */ CreateCustomizeFont(Widget w, XtPointer client_data __attribute__ ((unused)),
						    XmAnyCallbackStruct * callback_data __attribute__ ((unused)))
{
  int c_count;
  String *courier_fonts =
      XListFonts(XtDisplay(w), "-*-courier-*-*-*--*-*-*-*-*-*-*-*", 1000, &c_count);
  int h_count;
  String *helvetica_fonts =
      XListFonts(XtDisplay(w), "-*-helvetica-*-*-*--*-*-*-*-*-*-*-*", 1000, &h_count);
  int n_count;
  String *newc_fonts =
      XListFonts(XtDisplay(w), "-*-new century schoolbook-*-*-*--*-*-*-*-*-*-*-*", 1000, &n_count);
  int t_count;
  String *times_fonts = XListFonts(XtDisplay(w), "-*-times-*-*-*--*-*-*-*-*-*-*-*", 1000, &t_count);
  int count = c_count + h_count + n_count + t_count;
  XmStringTable table = (XmStringTable) XtMalloc((count + 1) * sizeof(XmString));
  int i;
  int j;
  for (i = 0, j = 0; i < c_count; i++, j++)
    table[j] = XmStringCreateSimple(courier_fonts[i]);
  for (i = 0; i < h_count; i++, j++)
    table[j] = XmStringCreateSimple(helvetica_fonts[i]);
  for (i = 0; i < n_count; i++, j++)
    table[j] = XmStringCreateSimple(newc_fonts[i]);
  for (i = 0; i < t_count; i++, j++)
    table[j] = XmStringCreateSimple(times_fonts[i]);
  table[j] = 0;
  XtVaSetValues(w, XmNlistItemCount, count, XmNlistItems, table, NULL);
  for (i = 0; i < count; i++)
    XmStringFree(table[i]);
  XtFree((String) table);
  XFreeFontNames(courier_fonts);
  XFreeFontNames(helvetica_fonts);
  XFreeFontNames(newc_fonts);
  XFreeFontNames(times_fonts);
}

static void Setup(Widget w __attribute__ ((unused)), XtPointer client_data __attribute__ ((unused)), XButtonEvent * event,
		  Boolean * continue_to_dispatch __attribute__ ((unused)))
{
  if (event->button == Button3) {
    if ((event->type == ButtonPress) && !(event->state & (Button1Mask | Button2Mask))) {
      int col;
      int row;
      for (col = 0; col < Columns - 1 && event->x > XtX(Sash[col]); col++) ;
      for (row = 0;
	   row < Rows[col] - 1 && event->y > XtY(Wave[col][row].w) + XtHeight(Wave[col][row].w);
	   row++) ;
      PendingWave = &Wave[col][row];
      PositionPopupMenu(Button3Widget, event);
      XtManageChild(Button3Widget);
    } else {
      XUngrabPointer(XtDisplay(Button3Widget), event->time);
      XtPopdown(XtParent(Button3Widget));
    }
  }
}

static void /*XtCallbackProc */ ApplyCustomizeWindow(Widget w __attribute__ ((unused)),
						     XtPointer client_data __attribute__ ((unused)),
						     XmAnyCallbackStruct * callback_data __attribute__ ((unused)))
{
  int c;
  int r;
  int old_columns = Columns;
  int old_rows[MaxRows];
  int pane_height = XtHeight(Pane[0]);
  int num;
  Widget *widgets;
  ReplaceString(&ScopeTitle, XmTextGetString(XtNameToWidget(CustomizeWindowWidget, "window_title")),
		1);
  ReplaceString(&ScopeIcon, XmTextGetString(XtNameToWidget(CustomizeWindowWidget, "icon_name")), 1);
  ReplaceString(&ScopeTitleEvent,
		XmTextGetString(XtNameToWidget(CustomizeWindowWidget, "title_event")), 1);
  SetupEvent(ScopeTitleEvent, &ScopeTitleEventReceived, &ScopeTitleEventId);
  SetWindowTitles();
  XtVaGetValues(PlotsWidget, XmNnumChildren, &num, XmNchildren, &widgets, NULL);
  XtUnmanageChildren(widgets, num);
  Columns = 1;
  memcpy(old_rows, Rows, sizeof(old_rows));
  for (c = 0; c < MaxCols; c++) {
    char name[8];
    sprintf(name, "rows_%d", c + 1);
    XmScaleGetValue(XtNameToWidget(CustomizeWindowWidget, name), &Rows[c]);
    if (Rows[c])
      Columns = c + 1;
    else
      Rows[c] = 1;
  }
  for (c = 0; c < MaxCols; c++) {
    for (r = 0; r < MaxRows; r++) {
      if (c >= Columns) {
	ClearWaveform(&Wave[c][r]);
	Rows[c] = 0;
      } else if (r >= Rows[c])
	ClearWaveform(&Wave[c][r]);
      else {
	if ((r < Rows[c]) && (Rows[c] != old_rows[c])) {
	  int height = (pane_height - 2 * Rows[c] + (r % 2) * .4999) / Rows[c];
	  XtVaSetValues(Wave[c][r].w, XtNheight, height, NULL);
	}
	if ((r >= old_rows[c]) && (r < Rows[c])) {
	  WaveInfo *info = &Wave[c][r];
	  UpdateWaveform(0, info, 0, -1, -1);
	  SetupEvent(info->_global.global.event ? GlobalWave.event : info->event, &info->received,
		     &info->eventid);
	}
      }
    }
  }
  if (Columns != old_columns)
    for (c = 0; c < Columns - 1; c++)
      XtVaSetValues(Sash[c], XmNleftPosition, ((c + 1) * 1000) / Columns, NULL);
  ManageWaveforms();
  ApplyStatus = 1;
  return;
}

static void /*XtCallbackProc */ ApplyCustomizePrint(Widget w __attribute__ ((unused)),
						    XtPointer client_data __attribute__ ((unused)),
						    XmAnyCallbackStruct * callback_data __attribute__ ((unused)))
{
  Widget printer_select = XtNameToWidget(MainWidget, "*printer_select");
  Widget option;
  XmString label;
  String label_string;
  Widget portrait_w = XtNameToWidget(CustomizePrintWidget, "cp_portrait_button");
  XtVaGetValues(printer_select, XmNmenuHistory, &option, NULL);
  XtVaGetValues(option, XmNlabelString, &label, NULL);
  label_string = XmStringUnparse(label, NULL, 0, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
  ReplaceString(&ScopePrinter, label_string, 1);
  ReplaceString(&ScopePrintFile,
		XmTextGetString(XtNameToWidget(CustomizePrintWidget, "print_file")), 1);
  ReplaceString(&ScopePrintEvent,
		XmTextGetString(XtNameToWidget(CustomizePrintWidget, "print_event")), 1);
  if (portrait_w)
    ScopePrintPortrait = XmToggleButtonGetState(portrait_w);
  ScopePrintWindowTitle =
      XmToggleButtonGetState(XtNameToWidget(CustomizePrintWidget, "print_window_title"));
  ScopePrintToFile = strcmp(ScopePrinter, "To file") == 0;
  SetupEvent(ScopePrintEvent, &ScopePrintEventReceived, &ScopePrintEventId);
  ApplyStatus = 1;
}

static void SetFont(String font)
{
  int c;
  int r;
  XFontStruct *font_struct = XLoadQueryFont(XtDisplay(TopWidget), font);
  if (font_struct) {
    for (c = 0; c < MaxCols; c++)
      for (r = 0; r < MaxRows; r++)
	if (Wave[c][r].w)
	  XtVaSetValues(Wave[c][r].w, XmdsNlabelFont, font_struct, NULL);
  } else {
    static String prefix = "Unknown font selected: ";
    String error = XtMalloc(strlen(prefix) + strlen(font) + 1);
    sprintf(error, "%s%s", prefix, font);
    PopupComplaint(TopWidget, error);
    XtFree(error);
  }
}

static void ResetCustomizeFont(Widget w __attribute__ ((unused)),
			       XtPointer client_data __attribute__ ((unused)),
			       XmAnyCallbackStruct * callback_data __attribute__ ((unused)))
{
  String font;
  XFontStruct *font_struct;
  unsigned long fontprop;
  XmString font_string;
  XtVaGetValues(Wave[0][0].w, XmdsNlabelFont, &font_struct, NULL);
  XGetFontProperty(font_struct, XA_FONT, &fontprop);
  font = XGetAtomName(XtDisplay(TopWidget), fontprop);
  font_string = XmStringCreateSimple(font);
  XtVaSetValues(CustomizeFontWidget, XmNtextString, font_string, NULL);
  XmStringFree(font_string);
  XFree(font);
}

static void /*XtCallbackProc */ ApplyFont(Widget w __attribute__ ((unused)), XtPointer client_data __attribute__ ((unused)),
					  XmSelectionBoxCallbackStruct * callback_data)
{
  String font;
  font = XmStringUnparse(callback_data->value, NULL, 0, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
  if (font)
    SetFont(font);
}

void /*XtInputCallbackProc */ EventUpdate(XtPointer client_data __attribute__ ((unused)),
					  int *source __attribute__ ((unused)), XtInputId * id __attribute__ ((unused)))
{
  int r;
  int c;
  XAllowEvents(XtDisplay(Button3Widget), AsyncPointer, CurrentTime);
  pthread_mutex_lock(&event_mutex);
  if (ScopeTitleEventReceived) {
    SetWindowTitles();
    ScopeTitleEventReceived = 0;
  }
  if (UpdatesOn) {
    for (c = 0; c < MaxCols; c++)
      for (r = 0; r < MaxRows; r++)
	if (Wave[c][r].received) {
	  UpdateWaveform(0, &Wave[c][r], 1, -1, -1);
	  Wave[c][r].received = 0;
	}
    if (ScopePrintEventReceived) {
      PrintAll(0, 0, 0);
      ScopePrintEventReceived = 0;
    }
  }
  if (CloseDataSourcesEventReceived) {
    CloseDataSourcesEventReceived = 0;
    CloseDataSources();
  }
  pthread_mutex_unlock(&event_mutex);
  return;
}

struct _UpdateWaveformsInfo {
  int r;
  int c;
};

static Boolean UpdateWaveformsWorkproc(XtPointer settings)
{
  struct _UpdateWaveformsInfo *info = (struct _UpdateWaveformsInfo *)settings;
  UpdateWaveform(0, &Wave[info->c][info->r], 0, -1, -1);
  info->r++;
  if (info->r >= Rows[info->c]) {
    info->r = 0;
    info->c++;
    if (info->c >= Columns) {
      XmString label = XmStringCreateSimple("Apply");
      XtVaSetValues(XtNameToWidget(MainWidget, "*override_shot_apply"), XmNlabelString, label,
		    XmNmarginWidth, 5, NULL);
      UpdateWaveformsWorkProcID = 0;
      XmStringFree(label);
      SetWindowTitles();
      info->r = 0;
      info->c = 0;
      return TRUE;
    }
  }
  return FALSE;
}

static void /*XtCallbackProc */ ApplyOverride(Widget w __attribute__ ((unused)),
					      int *mode __attribute__ ((unused)),
					      XtPointer callback_data __attribute__ ((unused)))
{
  static struct _UpdateWaveformsInfo info;
  if (UpdateWaveformsWorkProcID != 0) {
    XmString label = XmStringCreateSimple("Apply");
    XtRemoveWorkProc(UpdateWaveformsWorkProcID);
    XtVaSetValues(XtNameToWidget(MainWidget, "*override_shot_apply"), XmNlabelString, label,
		  XmNmarginWidth, 5, NULL);
    UpdateWaveformsWorkProcID = 0;
    XmStringFree(label);
  } else {
    XmString label = XmStringCreateSimple("Cancel");
    XtVaSetValues(XtNameToWidget(MainWidget, "*override_shot_apply"), XmNlabelString, label,
		  XmNmarginWidth, 2, NULL);
    info.r = 0;
    info.c = 0;
    UpdateWaveformsWorkProcID =
	XtAppAddWorkProc(AppContext, UpdateWaveformsWorkproc, (XtPointer) & info);
    XmStringFree(label);
  }
}

static void /*XtCallbackProc */ Updates(Widget w, int *mode __attribute__ ((unused)),
					XtPointer callback_data __attribute__ ((unused)))
{
  UpdatesOn = XmToggleButtonGadgetGetState(w);
  if (UpdatesOn)
    EventUpdate(0, 0, 0);
  XmToggleButtonGadgetSetState(XtNameToWidget(MainWidget, "*updates_menu"),
			       !XmToggleButtonGadgetGetState(w), FALSE);
}

static void /*XtCallbackProc */ ApplyDataSetup(Widget w, int *mode,
					       XtPointer callback_data __attribute__ ((unused)))
{
  int change_mask;
  if (CurrentWave != &GlobalWave) {
    if (*mode) {
      WaveInfo info;
      CopyWave(CurrentWave, &info);
      GetDataSetup(DataSetupWidget, &info, &change_mask);
      ApplyStatus = UpdateWaveform(*mode, &info, 0, 0, change_mask);
      if (ApplyStatus & 1) {
	String event = info._global.global.event ? GlobalWave.event : info.event;
	FreeWave(CurrentWave);
	*CurrentWave = info;
	SetupEvent(event, &CurrentWave->received, &CurrentWave->eventid);
	if (ApplyStatus & 1 && CurrentWave == SelectedWave)
	  XtDisownSelection(SelectedWave->w, XA_PRIMARY, XtLastTimestampProcessed(XtDisplay(w)));
      } else
	FreeWave(&info);
    } else {
      GetDataSetup(DataSetupWidget, CurrentWave, &change_mask);
      XtUnmanageChild(DataSetupWidget);
    }
  } else {
    int r;
    int c;
    GetDataSetup(DataSetupWidget, &GlobalWave, &change_mask);
    for (c = 0; c < Columns; c++)
      for (r = 0; r < Rows[c]; r++)
	if (Wave[c][r]._global.global_defaults) {
	  UpdateWaveform(0, &Wave[c][r], 0, change_mask, 0);
	  if (Wave[c][r]._global.global.event)
	    SetupEvent(GlobalWave.event, &Wave[c][r].received, &Wave[c][r].eventid);
	}
    ApplyStatus = 1;
    SetWindowTitles();
  }
  PositionMark();
  return;
}

static void CopyWave(WaveInfo * in, WaveInfo * out)
{
  *out = *in;
  out->database = XtNewString(in->database);
  out->shot = XtNewString(in->shot);
  out->default_node = XtNewString(in->default_node);
  out->y = XtNewString(in->y);
  out->x = XtNewString(in->x);
  out->title = XtNewString(in->title);
  out->event = XtNewString(in->event);
  out->print_title = XtNewString(in->print_title);
  out->pad_label = XtNewString(in->pad_label);
}

static void FreeWave(WaveInfo * info)
{
  if (info->database)
    XtFree(info->database);
  if (info->shot)
    XtFree(info->shot);
  if (info->default_node)
    XtFree(info->default_node);
  if (info->y)
    XtFree(info->y);
  if (info->x)
    XtFree(info->x);
  if (info->title)
    XtFree(info->title);
  if (info->event)
    XtFree(info->event);
  if (info->print_title)
    XtFree(info->print_title);
  if (info->pad_label)
    XtFree(info->pad_label);
}

static void /*XtActionProc */ MoveVerticalPane(Widget w, XEvent * event,
					       String * params __attribute__ ((unused)),
					       Cardinal * num_params __attribute__ ((unused)))
{
  Position main_x_root;
  Position main_y_root;
  static Position min_offset;
  static Position max_offset;
  int i;
  static Widget separator = 0;
  XtTranslateCoords(PlotsWidget, 0, 0, &main_x_root, &main_y_root);
  if (!separator)
    separator = XtNameToWidget(TopWidget, "*pane_separator");
  if (event->type == ButtonPress) {
    for (i = 0; i < NumSashs; i++)
      if (Sash[i] == w)
	break;
    if (i) {
      XtVaGetValues(Sash[i - 1], XmNx, &min_offset, NULL);
      min_offset += 10;
    } else
      min_offset = 10;
    if (i < (NumSashs - 1) && XtIsManaged(Sash[i + 1])) {
      XtVaGetValues(Sash[i + 1], XmNx, &max_offset, NULL);
      max_offset -= 10;
    } else
      max_offset = XtWidth(MainWidget) - 10;
    separator->core.widget_class->core_class.compress_motion = 1;
    XtManageChild(separator);
  }
  XtVaSetValues(separator, XmNleftOffset,
		min(max_offset, max(min_offset, event->xbutton.x_root - main_x_root)), NULL);
  if (event->type == ButtonRelease) {
    unsigned short position;
    XtVaGetValues(separator, XmNx, &position, NULL);
    position = position * 1000. / XtWidth(MainWidget) + .49999;
    XtVaSetValues(w, XmNleftPosition, position, NULL);
    XtUnmanageChild(separator);
    separator->core.widget_class->core_class.compress_motion = 0;
  }
}

static XrmDatabase MdsGetFileDatabase(String file_spec)
{
  FILE *file = fopen(file_spec, "r");
  XrmDatabase db = 0;
  if (file) {
    char line_text[8192];
    char doubled[8192];
    while (fgets(line_text, 8192, file)) {
      size_t i;
      size_t j;
      int put_it = 0;
      size_t size = strlen(line_text);
      for (i = 0; i < size; i++)
	if (line_text[i] == ':')
	  break;
      for (j = i + 1; j < size; j++)
	if (line_text[j] != 9 && line_text[j] != 10 && line_text[j] != ' ') {
	  put_it = 1;
	  break;
	}
      if (put_it) {
	j = 0;
	for (i = 0; (i < size) && (j < sizeof(doubled)); i++) {
	  if (line_text[i] == '\\')
	    doubled[j++] = line_text[i];
	  doubled[j++] = line_text[i];
	}
	doubled[j] = 0;
	XrmPutLineResource(&db, doubled);
      }
    }
    fclose(file);
  }
  return db;
}

static Window CreateBusyWindow(Widget toplevel)
{
  unsigned long valuemask;
  XSetWindowAttributes attributes;
/* Ignore device events while the busy cursor is displayed. */
  valuemask = CWDontPropagate | CWCursor;
  attributes.do_not_propagate_mask = (KeyPressMask | KeyReleaseMask |
				      ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
  attributes.cursor = XCreateFontCursor(XtDisplay(toplevel), XC_watch);
/* The window will be as big as the display screen, and clipped by
   its own parent window, so we never have to worry about resizing */
  return XCreateWindow(XtDisplay(toplevel), XtWindow(toplevel), 0, 0,
		       WidthOfScreen(XtScreen(toplevel)), HeightOfScreen(XtScreen(toplevel)),
		       (unsigned int)0, CopyFromParent, InputOnly,
		       CopyFromParent, valuemask, &attributes);
}

static void Busy()
{
  if (BusyLevel++)
    return;
  if (BusyWindow) {
    XEvent event;
    XMapRaised(XtDisplay(MainWidget), BusyWindow);
    while (XCheckMaskEvent(XtDisplay(MainWidget),
			   ButtonPressMask | ButtonReleaseMask | ButtonMotionMask |
			   PointerMotionMask | KeyPressMask, &event)) ;
    XFlush(XtDisplay(MainWidget));
  }
}

static void Unbusy()
{
  if (--BusyLevel)
    return;
  if (BusyWindow)
    XUnmapWindow(XtDisplay(MainWidget), BusyWindow);
}

static void /*XtCallbackProc */ PrintAll(Widget w __attribute__ ((unused)),
					 XtPointer client_data __attribute__ ((unused)),
					 XmAnyCallbackStruct * callback_data __attribute__ ((unused)))
{

#ifndef _NO_DPS
  char filename[256];
  FILE *printfid;
  int width = XtWidth(PlotsWidget);
  int height = XtHeight(PlotsWidget);
  int r;
  int c;
  if (ScopePrintToFile)
    printfid = fopen(ScopePrintFile, "w");	/*,"rop=RAH,WBH","mbc=8","mbf=2","deq=32"); */
  else {
    sprintf(filename, "/tmp/dwscope_tmp_%0x_%0x", getpid(), ScopeTempFileIdx++);
    printfid = fopen(filename, "w");
  }
  if (printfid) {
    XmString filenames[1];
    int orientation = ScopePrintPortrait ? 1 : 2;
    filenames[0] = XmStringCreateSimple(ScopePrintFile);

#ifndef _NO_DXm
    XtVaGetValues(CustomizePrintWidget, DXmNorientation, &orientation, NULL);
#endif

    Busy();
    for (c = 0; c < Columns; c++)
      for (r = 0; r < Rows[c]; r++)
	if (ScopePrintWindowTitle) {
	  String title;
	  String title_error;
	  Boolean title_status = EvaluateText(ScopeTitle, " ", &title, &title_error);
	  if (!title_status) {
	    XtFree(title_error);
	    XmdsWaveformPrint(Wave[c][r].w, printfid, width, height, orientation,
			      Wave[c][r].print_title_evaluated, 0, 0);
	  } else
	    XmdsWaveformPrint(Wave[c][r].w, printfid, width, height, orientation,
			      Wave[c][r].print_title_evaluated, title, 0);
	  XtFree(title);
	} else
	  XmdsWaveformPrint(Wave[c][r].w, printfid, width, height, orientation,
			    Wave[c][r].print_title_evaluated, 0, 0);
    fclose(printfid);
    if (!ScopePrintToFile)
#ifndef _NO_DXm
      DXmPrintWgtPrintJob(CustomizePrintWidget, filenames, 1);
#else
      DoPrint(filename);
#endif				/* DXmNorientation */
    XmStringFree(filenames[0]);
    Unbusy();
  } else {
    static String prefix = "Error creating printfile, ";
    String error = XtMalloc(strlen(prefix) + strlen(ScopePrintFile) + 1);
    sprintf(error, "%s%s", prefix, ScopePrintFile);
    PopupComplaint(TopWidget, error);
    XtFree(error);
  }
#endif				/* _NO_DPS */
  return;
}

static void /*XtCallbackProc */ Print(Widget w __attribute__ ((unused)),
				      XtPointer client_data __attribute__ ((unused)),
				      XmAnyCallbackStruct * callback_data __attribute__ ((unused)))
{
#ifndef _NO_DPS
  int count;
  XtVaGetValues(PendingWave->w, XmdsNcount, &count, NULL);
  if (count) {
    char filename[256];
    FILE *printfid;
    if (ScopePrintToFile)
      printfid = fopen(ScopePrintFile, "w");	/*,"rop=RAH,WBH","mbc=8","mbf=2","deq=32"); */
    else {
      sprintf(filename, "/tmp/dwscope_tmp_%0x_%0x", getpid(), ScopeTempFileIdx++);
      printfid = fopen(filename, "w");
    }
    if (printfid) {
      XmString filenames[1];
      int orientation = ScopePrintPortrait ? 1 : 2;
      filenames[0] = XmStringCreateSimple(ScopePrintFile);
#ifndef _NO_DXm
      XtVaGetValues(CustomizePrintWidget, DXmNorientation, &orientation, NULL);
#endif
      Busy();
      if (ScopePrintWindowTitle) {
	String title;
	String title_error;
	Boolean title_status = EvaluateText(ScopeTitle, " ", &title, &title_error);
	if (!title_status) {
	  XtFree(title_error);
	  XmdsWaveformPrint(PendingWave->w, printfid, 0, 0, orientation,
			    PendingWave->print_title_evaluated, 0, 0);
	} else
	  XmdsWaveformPrint(PendingWave->w, printfid, 0, 0, orientation,
			    PendingWave->print_title_evaluated, title, 0);
	XtFree(title);
      } else
	XmdsWaveformPrint(PendingWave->w, printfid, 0, 0, orientation,
			  PendingWave->print_title_evaluated, 0, 0);
      fclose(printfid);
      if (!ScopePrintToFile)
#ifndef _NO_DXm
	DXmPrintWgtPrintJob(CustomizePrintWidget, filenames, 1);
#else
	DoPrint(filename);
#endif
      XmStringFree(filenames[0]);
      Unbusy();
    } else {
      static String prefix = "Error creating printfile, ";
      String error = XtMalloc(strlen(prefix) + strlen(ScopePrintFile) + 1);
      sprintf(error, "%s%s", prefix, ScopePrintFile);
      PopupComplaint(TopWidget, error);
      XtFree(error);
    }
  } else
    PopupComplaint(TopWidget, "No data to print");
#endif				/* _NO_DPS */
  return;
}

static void /*XtCallbackProc */ Clear(Widget w, XtPointer client_data __attribute__ ((unused)),
				      XmAnyCallbackStruct * callback_data __attribute__ ((unused)))
{
  if (SelectedWave && SelectedWave == GetPending(w))
    XtDisownSelection(SelectedWave->w, XA_PRIMARY, XtLastTimestampProcessed(XtDisplay(w)));
}

static WaveInfo *GetPending(Widget w)
{
  int r;
  int c;
  for (c = 0; c < MaxCols; c++)
    for (r = 0; r < MaxRows; r++)
      if (Wave[c][r].w == w)
	goto found;
  return PendingWave;
 found:
  return &Wave[c][r];
}

static void /*XtCallbackProc */ Cut(Widget w, XtPointer client_data __attribute__ ((unused)),
				    XmAnyCallbackStruct * callback_struct)
{
  WaveInfo *pending = GetPending(w);
  if (pending == SelectedWave)
    XtDisownSelection(SelectedWave->w, XA_PRIMARY, callback_struct->event->xbutton.time);
  else if (XtOwnSelection(pending->w, XA_PRIMARY, callback_struct->event->xbutton.time,
			  (XtConvertSelectionProc) ConvertSelection, LoseSelection, NULL)) {
    SelectedWave = pending;
    XmdsWaveformReverse(SelectedWave->w, 1);
  }
}

static Boolean /*XtConvertSelectionProc */ ConvertSelection(Widget w, Atom * selection __attribute__ ((unused)),
							    Atom * target, Atom * type,
							    XtPointer * value,
							    unsigned long *length, int *format)
{
  int r = 0;
  int c = 0;
  char prefix[36];
  for (c = 0; c < Columns; c++)
    for (r = 0; r < Rows[c]; r++)
      if (Wave[c][r].w == w)
	goto found;
  return 0;
 found:
  sprintf(prefix, "Scope.plot_%d_%d", r + 1, c + 1);
  return ConvertWaveToSelection(w, prefix, &Wave[c][r], *target, type, value, length, format);
}

static void /*XtLoseSelectionProc */ LoseSelection(Widget w, Atom * selection __attribute__ ((unused)))
{
  XmdsWaveformReverse(w, 0);
  if (SelectedWave && (SelectedWave->w == w))
    SelectedWave = 0;
}

static void /*XtActionProc */ Paste(Widget w, XEvent * event __attribute__ ((unused)),
				    String * params __attribute__ ((unused)),
				    Cardinal * num_params __attribute__ ((unused)))
{
  WaveInfo *pending = GetPending(w);
  if (pending)
    XtGetSelectionValue(pending->w, XA_PRIMARY, XA_TARGETS,
			(XtSelectionCallbackProc) PasteTypesComplete, pending,
			XtLastTimestampProcessed(XtDisplay(w)));
}

static void /*XtSelectionCallbackProc */ PasteTypesComplete(Widget w, XtPointer cdata,
							    Atom * selection __attribute__ ((unused)),
							    Atom * type __attribute__ ((unused)),
							    XtPointer value,
							    unsigned long *length,
							    int *format __attribute__ ((unused)))
{
  unsigned long i;
  Atom req_type = XA_STRING;
  Atom *values = (Atom *) value;
  for (i = 0; i < *length; i++)
    if (values[i] == XA_DWSCOPE_PANEL) {
      req_type = XA_DWSCOPE_PANEL;
      break;
    }
  XtGetSelectionValue(w, XA_PRIMARY, req_type, (XtSelectionCallbackProc) PasteComplete,
		      cdata, XtLastTimestampProcessed(XtDisplay(w)));
  if (value)
    XtFree(value);
}

static void /*XtSelectionCallbackProc */ PasteComplete(Widget w, WaveInfo * info,
						       Atom * selection __attribute__ ((unused)),
						       Atom * type, XtPointer value,
						       unsigned long *length,
						       int *format __attribute__ ((unused)))
{
  if (ConvertSelectionToWave(w, *type, *length, value, info)) {
    UpdateWaveform(0, info, 0, -1, -1);
    SetupEvent(info->_global.global.event ? GlobalWave.event : info->event, &info->received,
	       &info->eventid);
  }
  if (value)
    XtFree(value);
}

static void /*XtCallbackProc */ RegisterPane(Widget w, XtPointer client_data __attribute__ ((unused)),
					     XmAnyCallbackStruct * callback_data __attribute__ ((unused)))
{
  Pane[NumPanes++] = w;
  NumWaves = 0;
}

static void /*XtCallbackProc */ RegisterWave(Widget w, XtPointer client_data __attribute__ ((unused)),
					     XmAnyCallbackStruct * callback_data __attribute__ ((unused)))
{
  Wave[NumPanes - 1][NumWaves].w = w;
  ResetWave(&Wave[NumPanes - 1][NumWaves++]);
}

static void /*XtCallbackProc */ RegisterSash(Widget w, XtPointer client_data __attribute__ ((unused)),
					     XmAnyCallbackStruct * callback_data __attribute__ ((unused)))
{
  Sash[NumSashs++] = w;
}

static String GlobalShot()
{
  String override_shot = XmTextFieldGetString(XtNameToWidget(TopWidget, "*override_shot"));
  return strlen(override_shot) ? override_shot : GlobalWave.shot;
}

static int UpdateWaveform(Boolean complain, WaveInfo * info, Boolean event, int global_change_mask,
			  int change_mask)
{

#define changed(field) ((info->_global.global.field ? global_change_mask : change_mask) & M_##field)

  Boolean new_grid = changed(x_grid_lines) || changed(y_grid_lines) || changed(x_grid_labels)
      || changed(y_grid_labels) || changed(show_mode) || changed(step_plot);
  info->received = 0;
  Busy();
  if (changed(shot) || changed(database) || changed(default_node) || changed(x) || changed(y)
      || changed(title) || changed(print_title)) {
    XmdsWaveformValStruct x_wave;
    XmdsWaveformValStruct y_wave;
    String title_evaluated = 0;
    String error = 0;
    String shot = info->_global.global.shot ? GlobalShot() : info->shot;
    String database = info->_global.global.database ? GlobalWave.database : info->database;
    String default_node =
	info->_global.global.default_node ? GlobalWave.default_node : info->default_node;
    String x = info->_global.global.x ? GlobalWave.x : info->x;
    String y = info->_global.global.y ? GlobalWave.y : info->y;
    String title = info->_global.global.title ? GlobalWave.title : info->title;
    String print_title =
	info->_global.global.print_title ? GlobalWave.print_title : info->print_title;
    float *xmin;
    float *xmax;
    float *ymin;
    float *ymax;
    Boolean update = 1;
    int r=0;
    int c;
    int idx = 0;
    Boolean brief = !(complain
		      || !XmToggleButtonGadgetGetState(XtNameToWidget(TopWidget, "*brief_errors")));
    for (c = 0; c < Columns; c++)
      for (r = 0; r < Rows[c]; r++, idx++)
	if (Wave[c][r].w == info->w)
	  goto found;
 found:
    if (!EvaluateData(brief, r, c, idx, event ? &update : (Boolean *) 0,
		      database, shot, default_node, x, y, &x_wave, &y_wave, &error)) {
      String terror = 0;
      if (!complain && EvaluateText(title, "Error evaluating title", &title_evaluated, &terror)
	  && title_evaluated) {
	String save = error;
	int len = strlen(title_evaluated) + 2 + strlen(save);
	error = XtMalloc(len + 1);
	strcpy(error, title_evaluated);
	strcat(error, "\n");
	strcat(error, save);
	error[len] = 0;
      } else if (terror)
	XtFree(terror);
      if (title_evaluated)
	XtFree(title_evaluated);
      Complain(info, complain, error);
      if (error)
	XtFree(error);
      Unbusy();
      return 0;
    }
    if (event & !update) {
      Unbusy();
      return 1;
    }
    if (!EvaluateText(title, "Error evaluating title", &title_evaluated, &error)) {
      if (complain)
	PopupComplaint(DataSetupWidget, error);
      if (error)
	XtFree(error);
    }
    if (info->print_title_evaluated) {
      XtFree(info->print_title_evaluated);
      info->print_title_evaluated = 0;
    }
    if (!EvaluateText
	(print_title, "Error evaluating print title", &info->print_title_evaluated, &error)) {
      if (complain)
	PopupComplaint(DataSetupWidget, error);
      if (error)
	XtFree(error);
    }
    GetNewLimits(info, &xmin, &xmax, &ymin, &ymax);
    if (new_grid)
      XtVaSetValues(info->w, XmdsNdisabled, True, NULL);
    XmdsWaveformUpdate(info->w, &x_wave, &y_wave, title_evaluated, xmin, xmax, ymin, ymax,
		       new_grid);
    XtFree(title_evaluated);
  } else if (changed(xmin) || changed(ymin) || changed(xmax) || changed(ymax)) {
    float *xmin;
    float *xmax;
    float *ymin;
    float *ymax;
    GetNewLimits(info, &xmin, &xmax, &ymin, &ymax);
    XtVaSetValues(info->w, XmdsNxMin, xmin, XmdsNxMax, xmax, XmdsNyMin, ymin, XmdsNyMax, ymax,
		  NULL);
  }
  if (new_grid) {
    int x_grid_lines =
	info->_global.global.x_grid_lines ? GlobalWave.x_grid_lines : info->x_grid_lines;
    int y_grid_lines =
	info->_global.global.y_grid_lines ? GlobalWave.y_grid_lines : info->y_grid_lines;
    Boolean x_grid_labels =
	info->_global.global.x_grid_labels ? GlobalWave.x_grid_labels : info->x_grid_labels;
    Boolean y_grid_labels =
	info->_global.global.y_grid_labels ? GlobalWave.y_grid_labels : info->y_grid_labels;
    char show_mode = info->_global.global.show_mode ? GlobalWave.show_mode : info->show_mode;
    Boolean step_plot = info->_global.global.step_plot ? GlobalWave.step_plot : info->step_plot;
    XtVaSetValues(info->w, XmdsNxGridLines, x_grid_lines, XmdsNyGridLines, y_grid_lines,
		  XmdsNxLabels, x_grid_labels, XmdsNyLabels, y_grid_labels, XmdsNshowMode,
		  show_mode, XmdsNstepPlot, step_plot, XmdsNdisabled, False, NULL);
  }
  Unbusy();
  return 1;
}

static void Complain(WaveInfo * info, char mode, String error)
{
  switch (mode) {
  case 0:
    XtVaSetValues(info->w, XmdsNcount, 0, XmdsNtitle, error, NULL);
    break;
  case 1:
    {
      Widget w = XtNameToWidget(DataSetupWidget, "*data_setup_error");
      XmString error_string = XmStringCreateLtoR(error, XmSTRING_DEFAULT_CHARSET);
      XtVaSetValues(w, XmNmessageString, error_string, NULL);
      XmStringFree(error_string);
      XtUnmanageChild(XmMessageBoxGetChild(w, XmDIALOG_HELP_BUTTON));
      XtManageChild(w);
      XtVaSetValues(info->w, XmdsNcount, 0, NULL);
      break;
    }
  case 2:
    PopupComplaint(DataSetupWidget, error);
  }
}

static void ClearWaveform(WaveInfo * info)
{
  ResetWave(info);
  SetupEvent("", &info->received, &info->eventid);
  if (info->w)
    XtVaSetValues(info->w, XmdsNxGridLines, info->x_grid_lines, XmdsNyGridLines, info->y_grid_lines,
		  XmdsNxLabels, info->x_grid_labels, XmdsNyLabels, info->y_grid_labels,
		  XmdsNshowMode, info->show_mode, XmdsNstepPlot, info->step_plot, XmdsNcount, 0,
		  XmdsNtitle, "", NULL);
}

static void RestoreDatabase(String dbname, Widget w)
{
  int c;
  int r;
  int x;
  int y;
  unsigned int width;
  unsigned int height;
  int num;
  Widget *widgets;
  String override_shot;
  char *default_printer = 0;
  XrmDatabase scopedb;
  scopedb = MdsGetFileDatabase(dbname);
  XtVaGetValues(PlotsWidget, XmNchildren, &widgets, XmNnumChildren, &num, NULL);
  XtUnmanageChildren(widgets, num);
  XParseGeometry(GetResource(scopedb, "Scope.geometry", "600x500+200+200"), &x, &y, &width,
		 &height);
  if (x >= (int)WidthOfScreen(XtScreen(w)))
    x = WidthOfScreen(XtScreen(w)) - 50;
  if ((int)(y + height) > (int)HeightOfScreen(XtScreen(w)))
    y = HeightOfScreen(XtScreen(w)) - height - 50;
  SetFont(GetResource
	  (scopedb, "Scope.font", "-*-NEW CENTURY SCHOOLBOOK-MEDIUM-R-*--*-120-*-*-*-*-ISO8859-1"));
  ReplaceString(&ScopeTitle, GetResource(scopedb, "Scope.title", "\"MDS Scope\""), 0);
  ReplaceString(&ScopeIcon, GetResource(scopedb, "Scope.icon_name", "\"Scope\""), 0);
  ReplaceString(&ScopeTitleEvent, GetResource(scopedb, "Scope.title_event", ""), 0);
  ReplaceString(&ScopePrintEvent, GetResource(scopedb, "Scope.print_event", ""), 0);
  override_shot = GetResource(scopedb, "Scope.override_shot", NULL);
  if (override_shot)
    XmTextFieldSetString(XtNameToWidget(TopWidget, "*override_shot"), override_shot);
  ReplaceString(&ScopePrintFile, GetResource(scopedb, "Scope.print_file", "dwscope.ps"), 0);
  ScopePrintPortrait = atoi(GetResource(scopedb, "Scope.print_portrait", "0"));
  ScopePrintWindowTitle = atoi(GetResource(scopedb, "Scope.print_window_title", "0"));
  default_printer = getenv("PRINTER");
  if (default_printer == 0)
    default_printer = "To file";
  ReplaceString(&ScopePrinter, GetResource(scopedb, "Scope.printer", default_printer), 0);
  ScopePrintToFile = strcmp(ScopePrinter, "To file") == 0;
/*
  ScopePrintToFile = atoi(GetResource(scopedb, "Scope.print_to_file", "0"));
*/
  GetWaveFromDb(scopedb, "Scope.global", 0, 0, &GlobalWave);
  SetWindowTitles();
  SetupEvent(strlen(ScopeTitleEvent) ? ScopeTitleEvent : GlobalWave.event, &ScopeTitleEventReceived,
	     &ScopeTitleEventId);
  SetupEvent(ScopePrintEvent, &ScopePrintEventReceived, &ScopePrintEventId);
  XtVaSetValues(TopWidget, XtNx, x, XtNy, y, NULL);
  if (XtWidth(MainWidget) != width || XtHeight(MainWidget) != height) {
    PreventResize = TRUE;
    /* XtVaSetValues(MainWidget, XtNwidth, width, XtNheight, height, NULL); */
    XtVaSetValues(TopWidget, XtNwidth, width, XtNheight, height, NULL);
    /* PreventResize = TRUE; */
  }
  MenusHeight = 66;
  Columns = min(max(atoi(GetResource(scopedb, "Scope.columns", "1")), 1), MaxCols);
  for (c = 0; c < MaxCols; c++) {
    char resource[50];
    sprintf(resource, "Scope.rows_in_column_%d", c + 1);
    resource[strlen(resource) - 1] = '1' + c;
    Rows[c] = c < Columns ? min(max(atoi(GetResource(scopedb, resource, "1")), 1), MaxRows) : 0;
    if (Rows[c])
      XtVaSetValues(Pane[c], XtNheight, height, NULL);
    for (r = 0; r < MaxRows; r++) {
      WaveInfo *info = &Wave[c][r];
      Dimension pheight;
      if (r < Rows[c]) {
	float zoom[4];
	int old_update;
	GetWaveFromDb(scopedb, "Scope.plot", r, c, info);
	old_update = info->update;
	pheight = info->height;
	pheight = pheight > 5
	    && pheight < height ? pheight : max(1, height - MenusHeight - 2 * Rows[c]) / Rows[c];
	info->update = 1;
	UpdateWaveform(0, info, 0, -1, -1);
	info->update = old_update;
	info->height = pheight;
	XtVaSetValues(info->w, XtNheight, pheight, NULL);
	if (GetWaveZoomFromDb(scopedb, "Scope.plot", r, c, zoom))
	  XtVaSetValues(info->w, XmdsNxMin, &zoom[0], XmdsNxMax, &zoom[1], XmdsNyMin, &zoom[2],
			XmdsNyMax, &zoom[3], NULL);
	SetupEvent(info->_global.global.event ? GlobalWave.event : info->event, &info->received,
		   &info->eventid);
      } else
	ClearWaveform(info);
    }
  }
  for (c = 0; c < Columns - 1; c++) {
    static char resource[] = "Scope.vpane_n";
    int position;
    resource[12] = '1' + c;
    position = atoi(GetResource(scopedb, resource, "-1"));
    if (position <= 0 || position >= 1000)
      position = (c + 1) * 1000 / Columns;
    XtVaSetValues(Sash[c], XmNleftPosition, position, NULL);
  }
  ManageWaveforms();
  if (!XtIsRealized(TopWidget)) {
    XtRealizeWidget(TopWidget);
    RaiseWindows();
/*    PreventResize = FALSE; */
  }
  PreventResize = FALSE;

  XrmDestroyDatabase(scopedb);
}

static void WriteDatabase(String dbname, Boolean zoom)
{
  int r;
  int c;
  Position x;
  Position y;
  Dimension width;
  Dimension height;
  int ctx = 0;
  unsigned long fontprop;
  String text;
  String filename = XtNewString(dbname);
  String font;
  size_t i;
  FILE *file;
  XFontStruct *font_struct;
  for (i = 0; i < strlen(filename); i++)
    if (filename[i] == ';')
      filename[i] = 0;
  file = fopen(filename, "w");
  XtFree(filename);
  if (file) {
    XtVaGetValues(TopWidget, XtNx, &x, XtNy, &y, NULL);
    XtVaGetValues(MainWidget, XtNwidth, &width, XtNheight, &height, NULL);
    fprintf(file, "Scope.geometry: %dx%d+%d+%d\n", width, height, x, y);
    fprintf(file, "Scope.title: %s\n", ScopeTitle);
    fprintf(file, "Scope.icon_name: %s\n", ScopeIcon);
    fprintf(file, "Scope.title_event: %s\n", ScopeTitleEvent);
    fprintf(file, "Scope.print_file: %s\n", ScopePrintFile);
    fprintf(file, "Scope.print_event: %s\n", ScopePrintEvent);
    fprintf(file, "Scope.print_portrait: %d\n", ScopePrintPortrait);
    fprintf(file, "Scope.print_window_title: %d\n", ScopePrintWindowTitle);
    fprintf(file, "Scope.printer: %s\n", ScopePrinter);
/*
    fprintf(file, "Scope.print_to_file: %d\n", ScopePrintToFile);
*/
    if (zoom)
      fprintf(file, "Scope.override_shot: %s\n",
	      XmTextFieldGetString(XtNameToWidget(TopWidget, "*override_shot")));
    XtVaGetValues(Wave[0][0].w, XmdsNlabelFont, &font_struct, NULL);
    XGetFontProperty(font_struct, XA_FONT, &fontprop);
    font = XGetAtomName(XtDisplay(TopWidget), fontprop);
    fprintf(file, "Scope.font: %s\n", font);
    XFree(font);
    fprintf(file, "Scope.columns: %d\n", Columns);
    while ((text = WaveToText("Scope.global_1_1", &GlobalWave, 0, &ctx))) {
      fprintf(file, "%s", text);
      XtFree(text);
    }
    for (c = 0; c < Columns; c++) {
      fprintf(file, "Scope.rows_in_column_%d: %d\n", c + 1, Rows[c]);
      for (r = 0; r < Rows[c]; r++) {
	char prefix[36];
	int ctx = zoom ? -4 : 0;
	int height;
	XtVaGetValues(Wave[c][r].w, XtNheight, &height, NULL);
	fprintf(file, "\n");
	sprintf(prefix, "Scope.plot_%d_%d", r + 1, c + 1);
	while ((text = WaveToText(prefix, &Wave[c][r], height, &ctx))) {
	  fprintf(file, "%s", text);
	  XtFree(text);
	}
      }
    }
    for (c = 0; c < Columns - 1; c++) {
      int position;
      XtVaGetValues(Sash[c], XmNleftPosition, &position, NULL);
      fprintf(file, "Scope.vpane_%d: %d\n", c + 1, position);
    }
    fclose(file);
  } else
    PopupComplaint(MainWidget, "Error writing setup file");
}

static void /*XtActionProc */ Resize(Widget w, XEvent * event, String * params __attribute__ ((unused)),
				     Cardinal * num_params __attribute__ ((unused)))
{
  int c;
  int r;
  double orig_height = XtHeight(Pane[0]);
  double new_height;
  XEvent ev;

  /* flush the event que for resize events */
  while (XCheckTypedWindowEvent(XtDisplay(w), XtWindow(w), event->type, &ev))
    event = &ev;
  if (!PreventResize) {
    for (c = 0; c < Columns; c++)
      for (r = 0; r < Rows[c]; r++)
	Wave[c][r].height = XtHeight(Wave[c][r].w);
  }
  XtVaSetValues(w, XmNoverrideRedirect, 1, XmNwidth, event->xresizerequest.width, XmNheight,
		event->xresizerequest.height, NULL);
  {
    new_height = XtHeight(Pane[0]);
    if (PreventResize) {
      orig_height = new_height;
      PreventResize = FALSE;
    }
    for (c = 0; c < Columns; c++) {
      XtUnmanageChild(Pane[c]);
      for (r = 0; r < Rows[c]; r++)
	XtVaSetValues(Wave[c][r].w, XtNheight,
		      (int)(new_height / orig_height * Wave[c][r].height + (r % 2) * .49999), NULL);
      XtManageChild(Pane[c]);
    }
  }
  XtVaSetValues(w, XmNoverrideRedirect, 0, NULL);
  return;
}

static void /*XtActionProc */ EqualPanes(Widget w, XEvent * event __attribute__ ((unused)), String * string,
					 Cardinal * num_strings)
{
  int c;
  int r;
  if ((*num_strings == 1) && (string[0][0] == 'V')) {
    for (c = 0; c < Columns - 1; c++) {
      int position = (c + 1) * 1000 / Columns;
      XtVaSetValues(Sash[c], XmNleftPosition, position, NULL);
    }
  } else if ((*num_strings == 1) && (string[0][0] == 'H')) {
    Widget p = XtParent(w);
    int height;
    for (c = 0; c < Columns; c++)
      if (p == Pane[c])
	break;
    height = (XtHeight(Pane[c]) - 2 * Rows[c]) / Rows[c];
    XtUnmanageChild(Pane[c]);
    for (r = 0; r < Rows[c]; r++)
      XtVaSetValues(Wave[c][r].w, XtNheight, height, NULL);
    XtManageChild(Pane[c]);
  }
  return;
}

static void GetNewLimits(WaveInfo * info, float **xmin, float **xmax, float **ymin, float **ymax)
{
  int lockScales = LockScalesWidget ? XmToggleButtonGetState(LockScalesWidget) : 0;
  if (info->update && !lockScales) {
    static float xminval;
    static float xmaxval;
    static float yminval;
    static float ymaxval;
    String xmin_s = info->_global.global.xmin ? GlobalWave.xmin : info->xmin;
    String xmax_s = info->_global.global.xmax ? GlobalWave.xmax : info->xmax;
    String ymin_s = info->_global.global.ymin ? GlobalWave.ymin : info->ymin;
    String ymax_s = info->_global.global.ymax ? GlobalWave.ymax : info->ymax;
    *xmin = (float *)0;
    *xmax = (float *)0;
    *ymin = (float *)0;
    *ymax = (float *)0;
    if (strlen(xmin_s))
      if (sscanf(xmin_s, "%f", &xminval))
	*xmin = &xminval;
    if (strlen(xmax_s))
      if (sscanf(xmax_s, "%f", &xmaxval))
	*xmax = &xmaxval;
    if (strlen(ymin_s))
      if (sscanf(ymin_s, "%f", &yminval))
	*ymin = &yminval;
    if (strlen(ymax_s))
      if (sscanf(ymax_s, "%f", &ymaxval))
	*ymax = &ymaxval;
  } else {
    *xmin = (float *)-1;
    *xmax = (float *)-1;
    *ymin = (float *)-1;
    *ymax = (float *)-1;
  }
}

static void SetWindowTitles()
{
  String title;
  String title_error;
  XmdsWaveformValStruct x_wave;
  XmdsWaveformValStruct y_wave;
  String error;
  int evalstat =
      EvaluateData(1, 0, 0, 0, 0, GlobalWave.database, GlobalShot(), "", "", "", &x_wave, &y_wave,
		   &error);
  Boolean title_status = EvaluateText(ScopeTitle, " ", &title, &title_error);
  String icon;
  String icon_error;
  Boolean icon_status = EvaluateText(ScopeIcon, " ", &icon, &icon_error);
  XtVaSetValues(TopWidget, XtNtitle, title_status ? title : ScopeTitle, XtNiconName,
		icon_status ? icon : ScopeIcon, NULL);
  if (!evalstat)
    XtFree(error);
  if (!title_status)
    XtFree(title_error);
  if (!icon_status)
    XtFree(icon_error);
  XtFree(title);
  XtFree(icon);
}
