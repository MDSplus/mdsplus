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
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element DWPAD.C */
/*  *26    9-MAR-1995 14:30:43 TWF "Fix call to ConvertSelectionToWave" */
/*  *25   16-FEB-1995 09:46:19 TWF "Create copy of defaultfile" */
/*  *24    7-FEB-1995 14:08:17 TWF "Fix restore" */
/*  *23    7-FEB-1995 13:50:58 TWF "Fix restore" */
/*  *22   27-DEC-1994 11:40:05 TWF "Fix Atom" */
/*  *21    9-NOV-1994 16:03:04 TWF "port to unix" */
/*  *20    9-NOV-1994 15:49:42 TWF "port to unix" */
/*  *19    9-NOV-1994 15:36:48 TWF "port to unix" */
/*  *18   13-OCT-1994 15:28:53 TWF "Font problem on DVX" */
/*  *17   20-SEP-1994 08:24:06 TWF "Use DECC" */
/*  *16   24-JUN-1994 09:16:32 TWF "Motif 1.2" */
/*  *15    1-MAY-1993 11:07:23 JAS "change the way atoms are delt with" */
/*  *14    8-APR-1993 11:07:44 TWF "Use MrmPublic (much smaller)" */
/*  *13   19-MAR-1993 12:43:57 TWF "Free paste data" */
/*  *12   24-FEB-1993 17:51:22 TWF "Use DECC" */
/*  *11   18-NOV-1992 15:12:47 TWF "Add expand axis capability" */
/*  *10    4-AUG-1992 15:12:03 JAS "change paste to check available types and accept strings if nesss." */
/*  *9    18-MAR-1992 11:25:38 JAS "boy I'm stupid" */
/*  *8    18-MAR-1992 11:05:28 JAS "boy I'm stupid" */
/*  *7    18-MAR-1992 10:53:53 JAS "try signed arithmetic" */
/*  *6    17-MAR-1992 17:19:34 JAS "does not work if it is scrolled" */
/*  *5    11-DEC-1991 13:47:34 TWF "put blank lines between panels in saved setting" */
/*  *4    15-NOV-1991 11:43:27 TWF "prepare for MDSV1" */
/*  *3    15-NOV-1991 10:46:00 TWF "Prepare to make MDSV1 version" */
/*  *2    12-NOV-1991 15:55:28 TWF "Use DisplayHelp not PadHelp" */
/*  *1    12-NOV-1991 14:31:45 TWF "Scope Pad program" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element DWPAD.C */
/*------------------------------------------------------------------------------

		Name:   DWPAD

		Type:   C main program

		Author:	Mark London

		Date:   1-MAY-1991

		Purpose: Pad replacement main program

------------------------------------------------------------------------------

	Call sequence:

$ Run SYS$SYSTEM:DWPad

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

 External functions or symbols referenced:                                    */

#include <X11/cursorfont.h>
#include <X11/Xatom.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <Xm/ToggleB.h>
#include <Xm/PushB.h>
#include <Xm/Scale.h>
#include <Xm/FileSB.h>
#include "dwscope.h"
#include <stdio.h>
#include <stdlib.h>
#include <Mrm/MrmPublic.h>
#include <Xm/XmP.h>
#ifndef _NO_DXm
#include <DXm/DECspecific.h>
#endif

extern void XmdsDestroyWidgetCallback();
extern void XmdsManageChildCallback();
extern void XmdsRegisterWidgetCallback();
extern void XmdsUnmanageChildCallback();

extern void PositionPopupMenu(Widget w, XButtonEvent * event);
extern void PopupComplaint(Widget parent, char *string);
extern Boolean ConvertSelectionToWave(Widget w, Atom result_type, unsigned int length,
				      XtPointer header, WaveInfo * info);
extern Boolean ConvertWaveToSelection(Widget w, String prefix, WaveInfo * wave, Atom target,
				      Atom * type, XtPointer * header, unsigned int *length,
				      int *format);
extern Boolean ReplaceString(String * old, String new, Boolean free);
extern String GetResource(XrmDatabase db, String resource, String default_answer);
extern String WaveToText(String prefix, WaveInfo * wave, Dimension height, int *ctx);
extern void LoadDataSetup(Widget w, String title, WaveInfo * info);
extern void DisplayHelp(Widget w, String tag, XtPointer callback_data);
extern void GetDataSetup(Widget w, WaveInfo * info, int *change_mask);
extern void GetWaveFromDb(XrmDatabase db, String prefix, int row, int col, WaveInfo * info);
extern void ResetWave(WaveInfo * info);
extern void SetDirMask(Widget w, String file, XtPointer callback_data);
extern XrmDatabase GetFileDatabase(String file_spec);
extern void DisableGlobalDefault(Widget w, int *tag, XtPointer callback_data);
extern void InitDefaultsSetupWidget(Widget w, int *tag, XtPointer callback_data);
extern void ExpandReset(Widget w, int *tag, XtPointer callback_data);
extern void ExpandCancel(Widget w, int *tag, XtPointer callback_data);
extern void ExpandOk(Widget w, int *tag, XtPointer callback_data);

static int FindWave(Widget w, int *c, int *r);

static void ComputeOffsets(Widget child, Widget ancestor, int *x_offset, int *y_offset);
static void LabelSetString(Widget w, String string);
static void MakeWaves(int cols, int rows);
static void ResetWindow(String geometry, String title, String icon, int cols, int rows,
			Boolean bigfont, int btnwidth);
static void RestoreDatabase(String dbname);
static void SetPadVideo(Widget w, Boolean reverse);
static void UpdatePopups();
static void WriteDatabase(String dbname);

static String SetupTitle();

static void Cut(Widget w, XEvent * event, String * params, Cardinal * num_params);
static void Paste(Widget w, XEvent * event, String * params, Cardinal * num_params);

static void ApplyCustomizeWindow(Widget w, int *tag, XtPointer callback_data);
static void ApplyDataSetup(Widget w, int *tag, XtPointer callback_data);
static void ApplyLabel(Widget w, int *tag, XtPointer callback_data);
static void EraseDataSetup(Widget w, int *tag, XtPointer callback_data);
static void ExitWithCheck(Widget w, int *tag, XtPointer callback_data);
static void Exit(Widget w, int *tag, XtPointer callback_data);
static void Ok(Widget w, int *tag, XtPointer callback_data);
static void ResetDataSetup(Widget w, int *tag, XtPointer callback_data);
static void ResetCustomizeWindow(Widget w, int *tag, XtPointer callback_data);
static void ResetLabel(Widget w, int *tag, XtPointer callback_data);
static void Restore(Widget w, int *option, XmFileSelectionBoxCallbackStruct * reason);
static void /*XtCallbackProc */ Save(Widget w, int *option,
				     XmFileSelectionBoxCallbackStruct * reason);

static Boolean ConvertSelection(Widget w, Atom * selection, Atom * target, Atom * type,
				XtPointer * value, unsigned int *length, int *format);

static void Setup(Widget w, XtPointer client_data, XButtonEvent * event,
		  Boolean * continue_to_dispatch);

static void LoseSelection(Widget w, Atom * selection);

static void PasteComplete(Widget w, XtPointer cdata, Atom * selection, Atom * type, XtPointer value,
			  unsigned int *length, int *format);
static void PasteTypesComplete(Widget w, XtPointer cdata, Atom * selection, Atom * type,
			       XtPointer value, unsigned int *length, int *format);
/*------------------------------------------------------------------------------

 Subroutines referenced:                                                      */

/*------------------------------------------------------------------------------

 Macros:                                                                      */

#define min(a,b) ( ((a)<(b)) ? (a) : (b) )
#define max(a,b) ( ((a)>(b)) ? (a) : (b) )
#define N_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define XA_DWSCOPE_PANEL  XInternAtom(XtDisplay(TopWidget), "DWSCOPE_PANEL", 0)
#define XA_TARGETS  XInternAtom(XtDisplay(TopWidget), "TARGETS", 0)

/*------------------------------------------------------------------------------

 Global variables:                                                            */

static Widget TopWidget;
static Widget MainWidget;
static Widget DataSetupWidget;
static Widget DefaultsSetupWidget;
static Widget CustomizeWindowWidget;
static Widget SaveAsWidget;
static Widget RestoreFromWidget;
static Widget PadWidget;
static Widget PadLabelWidget;
static Widget SelectedWidget;
static Widget WriteBeforeExitWidget;
static Boolean PadBigFont;
static int PadLabelWidth;
static Widget Button3Widget;
static int Rows;
static int Columns;
static Widget CurrentWidget = 0;
static Widget PendingWidget = 0;
static String PadTitle = 0;
static String PadIcon = 0;
static XFontStruct *font[2] = { 0, 0 };

static XmFontList fontlist[2];
static int ApplyStatus = 0;
static int Modified = 0;
static String defaultfile;

int main(int argc, char **argv)
{
/*------------------------------------------------------------------------------

 Local variables:                                                             */

  size_t i;
  static String hierarchy_name[] = { "dwpad.uid", "dwscope.uid" };
  String fallback_resources[] = { "*default_label.labelString: Default node:",
    "*exp_label.labelString: Experiment:", NULL
  };
  static MrmRegisterArg callbacks[] = { {"Exit", (char *)Exit},
  {"Restore", (char *)Restore},
  {"Save", (char *)Save},
  {"ApplyDataSetup", (char *)ApplyDataSetup},
  {"ApplyCustomizeWindow", (char *)ApplyCustomizeWindow},
  {"ApplyLabel", (char *)ApplyLabel},
  {"DisableGlobalDefault", (char *)DisableGlobalDefault},
  {"InitDefaultsSetupWidget", (char *)InitDefaultsSetupWidget},
  {"ResetCustomizeWindow", (char *)ResetCustomizeWindow},
  {"ResetDataSetup", (char *)ResetDataSetup},
  {"ResetLabel", (char *)ResetLabel},
  {"EraseDataSetup", (char *)EraseDataSetup},
  {"Ok", (char *)Ok},
  {"DisplayHelp", (char *)DisplayHelp},
  {"SetDirMask", (char *)SetDirMask},
  {"ExitWithCheck", (char *)ExitWithCheck},
  {"DataSetupWidget", (char *)&DataSetupWidget},
  {"DefaultsSetupWidget", (char *)&DefaultsSetupWidget},
  {"RestoreFromWidget", (char *)&RestoreFromWidget},
  {"SaveAsWidget", (char *)&SaveAsWidget},
  {"PadLabelWidget", (char *)&PadLabelWidget},
  {"CustomizeWindowWidget", (char *)&CustomizeWindowWidget},
  {"PadWidget", (char *)&PadWidget},
  {"Button3Widget", (char *)&Button3Widget},
  {"DefaultFile", (char *)&defaultfile},
  {"WriteBeforeExitWidget", (char *)&WriteBeforeExitWidget},
  {"expand_ok", (char *)ExpandOk},
  {"expand_reset", (char *)ExpandReset},
  {"expand_cancel", (char *)ExpandCancel},
  {"XmdsDestroyWidgetCallback", (char *)XmdsDestroyWidgetCallback},
  {"XmdsManageChildCallback", (char *)XmdsManageChildCallback},
  {"XmdsRegisterWidgetCallback", (char *)XmdsRegisterWidgetCallback},
  {"XmdsUnmanageChildCallback", (char *)XmdsUnmanageChildCallback}
  };
  MrmType class;
  static XtActionsRec actions[] = { {"Cut", Cut},
  {"Paste", Paste}
  };
  static XrmOptionDescRec options[] = { {"-defaults", "*defaults", XrmoptionSepArg, NULL} };
  static XtResource resources[] =
      { {"defaults", "Defaults", XtRString, sizeof(String), 0, XtRString,
	 "PAD_DEFAULTS.DAT"}
  };
  XtAppContext app_ctx;
  MrmHierarchy drm_hierarchy;
  static const char *fontfamilies[] =
      { "adobe", "b&h", "bitstream", "bold", "bolditalic", "isas", "italic", "misc", "schumacher",
"sony", "sun", "xfree86" };
  static const char *fontfmt = "-%s-%s-medium-r-normal-*-*-*-%3.3d-%3.3d-*-*-*-*";
#define MAXFONTNAMELEN 255
  char fontname[MAXFONTNAMELEN];

/*------------------------------------------------------------------------------

 Executable:                                                                  */

  MrmInitialize();
#ifndef _NO_DXm
  DXmInitialize();
#endif
  MrmRegisterNames(callbacks, XtNumber(callbacks));
  TopWidget =
      XtVaAppInitialize(&app_ctx, "DwPad", options, XtNumber(options), &argc, argv,
			fallback_resources, XmNallowShellResize, 1, NULL);
  XtGetApplicationResources(TopWidget, &defaultfile, resources, XtNumber(resources), (Arg *) NULL,
			    0);
  if (defaultfile)
    defaultfile = XtNewString(defaultfile);
  XtAppAddActions(app_ctx, actions, XtNumber(actions));
  MrmOpenHierarchy(XtNumber(hierarchy_name), hierarchy_name, 0, &drm_hierarchy);
  MrmFetchWidget(drm_hierarchy, "pad", TopWidget, &MainWidget, &class);
  MrmCloseHierarchy(drm_hierarchy);
  XtManageChild(MainWidget);
  sprintf(fontname, fontfmt, "*", "menu", 100, 100);
  font[0] = XLoadQueryFont(XtDisplay(TopWidget), fontname);
  for (i = 0; ((!font[0]) && (i < N_ELEMENTS(fontfamilies))); i++) {
    sprintf(fontname, fontfmt, fontfamilies[i], "*", 100, 100);
    font[0] = XLoadQueryFont(XtDisplay(TopWidget), fontname);
  }
  sprintf(fontname, fontfmt, "*", "menu", 120, 120);
  font[1] = XLoadQueryFont(XtDisplay(TopWidget), fontname);
  for (i = 0; ((!font[1]) && (i < N_ELEMENTS(fontfamilies))); i++) {
    sprintf(fontname, fontfmt, fontfamilies[i], "*", 120, 120);
    font[1] = XLoadQueryFont(XtDisplay(TopWidget), fontname);
  }
  if ((!font[0]) && (!font[1])) {
    printf("No useable Fonts found-Please check your X-Windows configuration\n");
    exit(0);
  }
  fontlist[0] = XmFontListCreate(font[0], XmSTRING_DEFAULT_CHARSET);
  fontlist[1] = XmFontListCreate(font[1], XmSTRING_DEFAULT_CHARSET);
  XtRealizeWidget(TopWidget);
  RestoreDatabase(defaultfile);
  XtAddEventHandler(MainWidget, ButtonPressMask, False, (XtEventHandler) Setup, 0);
  XtAppMainLoop(app_ctx);
  return 0;
}

static void LabelSetString(Widget w, String string)
{
  XmString label = XmStringCreateSimple(strlen(string) ? string : " ");
  XtVaSetValues(w, XmNlabelString, label, NULL);
  XmStringFree(label);
}

static void ExitWithCheck(Widget w __attribute__ ((unused)),
			  int *tag __attribute__ ((unused)),
			  XtPointer callback_data __attribute__ ((unused)))
{
  if (Modified)
    XtManageChild(WriteBeforeExitWidget);
  else
    exit(1);
}

static void Exit(Widget w __attribute__ ((unused)), int *tag __attribute__ ((unused)),
		 XtPointer callback_data __attribute__ ((unused)))
{
  exit(1);
}

static void ApplyCustomizeWindow(Widget w __attribute__ ((unused)),
				 int *tag __attribute__ ((unused)),
				 XtPointer callback_data __attribute__ ((unused)))
{
  String title = XmTextGetString(XtNameToWidget(CustomizeWindowWidget, "window_title"));
  String icon = XmTextGetString(XtNameToWidget(CustomizeWindowWidget, "icon_name"));
  String cols_string = XmTextGetString(XtNameToWidget(CustomizeWindowWidget, "columns"));
  String rows_string = XmTextGetString(XtNameToWidget(CustomizeWindowWidget, "rows_in_columns"));
  Boolean bigfont =
      XmToggleButtonGetState(XtNameToWidget(CustomizeWindowWidget, "font_box.big_font"));
  int btnwidth;
  int rows;
  int cols;
  ApplyStatus = 0;
  XmScaleGetValue(XtNameToWidget(CustomizeWindowWidget, "padlabelwidth"), &btnwidth);
  btnwidth = max(btnwidth, 1);
  cols = atoi(cols_string);
  XtFree(cols_string);
  rows = atoi(rows_string);
  XtFree(rows_string);
  if (cols <= 0 || cols > 1000)
    PopupComplaint(TopWidget, "Columns value is invalid.");
  else if (rows <= 0 || rows > 1000)
    PopupComplaint(TopWidget, "Rows value is invalid.");
  else if (rows * cols > 1000)
    PopupComplaint(TopWidget, "Cannot have more than 1000 buttons");
  else {
    ResetWindow(0, title, icon, cols, rows, bigfont, btnwidth);
    ApplyStatus = 1;
    Modified = 1;
  }
  XtFree(title);
  XtFree(icon);
  return;
}

static void ResetCustomizeWindow(Widget w __attribute__ ((unused)),
				 int *tag __attribute__ ((unused)),
				 XtPointer callback_data __attribute__ ((unused)))
{
  char number[12];
  XmTextSetString(XtNameToWidget(CustomizeWindowWidget, "window_title"), PadTitle);
  XmTextSetString(XtNameToWidget(CustomizeWindowWidget, "icon_name"), PadIcon);
  sprintf(number, "%d", Columns);
  XmTextSetString(XtNameToWidget(CustomizeWindowWidget, "columns"), number);
  sprintf(number, "%d", Rows);
  XmTextSetString(XtNameToWidget(CustomizeWindowWidget, "rows_in_columns"), number);
  XmScaleSetValue(XtNameToWidget(CustomizeWindowWidget, "padlabelwidth"), PadLabelWidth);
  XmToggleButtonSetState(XtNameToWidget(CustomizeWindowWidget, "font_box.big_font"), PadBigFont, 1);
}

static void ApplyLabel(Widget w __attribute__ ((unused)),
		       int *tag __attribute__ ((unused)),
		       XtPointer callback_data __attribute__ ((unused)))
{
  WaveInfo *info;
  XtVaGetValues(CurrentWidget, XmNuserData, &info, NULL);
  if (ReplaceString
      (&info->pad_label, XmTextGetString(XtNameToWidget(PadLabelWidget, "pad_label_expression")),
       1)) {
    LabelSetString(CurrentWidget, info->pad_label);
    Modified = 1;
  }
}

static void ApplyDataSetup(Widget w __attribute__ ((unused)),
			   int *tag __attribute__ ((unused)),
			   XtPointer callback_data __attribute__ ((unused)))
{
  WaveInfo *info;
  int change_mask;
  XtVaGetValues(CurrentWidget, XmNuserData, &info, NULL);
  GetDataSetup(DataSetupWidget, info, &change_mask);
  LabelSetString(info->w, info->pad_label);
  ApplyStatus = 1;
  Modified = 1;
  return;
}

static void Ok(Widget w, int *tag __attribute__ ((unused)),
	       XtPointer callback_data __attribute__ ((unused)))
{
  if (ApplyStatus & 1)
    XtUnmanageChild(XtParent(w));
}

static void UpdatePopups()
{
  if (XtIsManaged(DataSetupWidget))
    ResetDataSetup(NULL, NULL, NULL);
  else if (XtIsManaged(PadLabelWidget))
    ResetLabel(NULL, NULL, NULL);
}

static void EraseDataSetup(Widget w __attribute__ ((unused)),
			   int *tag __attribute__ ((unused)),
			   XtPointer callback_data __attribute__ ((unused)))
{
  WaveInfo *info;
  XtVaGetValues(PendingWidget, XmNuserData, &info, NULL);
  ResetWave(info);
  LabelSetString(info->w, "");
  XtUnmanageChild(Button3Widget);
  if (PendingWidget == CurrentWidget)
    UpdatePopups();
}

static void /*XtCallbackProc */ Restore(Widget w __attribute__ ((unused)), int *option,
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
	    RestoreDatabase(filename);
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
    RestoreDatabase(defaultfile);
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
	    WriteDatabase(filename);
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
    WriteDatabase(defaultfile);
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
  }
}

static void ResetLabel(Widget w __attribute__ ((unused)), int *tag __attribute__ ((unused)),
		       XtPointer callback_data __attribute__ ((unused)))
{
  WaveInfo *info;
  XmString title = XmStringCreateSimple(SetupTitle());
  CurrentWidget = PendingWidget;
  XtVaGetValues(CurrentWidget, XmNuserData, &info, NULL);
  XtVaSetValues(PadLabelWidget, XmNdialogTitle, title, NULL);
  XmStringFree(title);
  XmTextSetString(XtNameToWidget(PadLabelWidget, "pad_label_expression"), info->pad_label);
  XtUnmanageChild(Button3Widget);
}

static void ResetDataSetup(Widget w __attribute__ ((unused)),
			   int *tag __attribute__ ((unused)),
			   XtPointer callback_data __attribute__ ((unused)))
{
  WaveInfo *info;
  CurrentWidget = PendingWidget;
  XtVaGetValues(CurrentWidget, XmNuserData, &info, NULL);
  LoadDataSetup(DataSetupWidget, SetupTitle(), info);
}

static void Setup(Widget w, XtPointer client_data __attribute__ ((unused)), XButtonEvent * event,
		  Boolean * continue_to_dispatch __attribute__ ((unused)))
{
  int i;
  int num_waves;
  Widget *widgets;
  Widget widg = 0;
  int x_offset;
  int y_offset;
  int event_x = event->x;
  int event_y = event->y;
  XtVaGetValues(PadWidget, XmNnumChildren, &num_waves, XmNchildren, &widgets, NULL);
  ComputeOffsets(widgets[0], w, &x_offset, &y_offset);
  event_x -= x_offset;
  event_y -= y_offset;
  widg = 0;
  for (i = 0; i < num_waves; i++) {
    int w_x = XtX(widgets[i]);
    int w_y = XtY(widgets[i]);
    if (event_x > w_x && event_x < w_x + (int)XtWidth(widgets[i]) &&
	event_y > w_y && event_y < w_y + (int)XtHeight(widgets[i])) {
      widg = widgets[i];
      break;
    }
  }
  if (widg && event->button == Button3) {
    PendingWidget = widg;
    PositionPopupMenu(Button3Widget, event);
    XtManageChild(Button3Widget);
  }
}

static void ComputeOffsets(Widget child, Widget ancestor, int *x_offset, int *y_offset)
{
  Widget parent;
  *x_offset = 0;
  *y_offset = 0;
  for (parent = XtParent(child); parent != ancestor; parent = XtParent(parent)) {
    (*x_offset) += XtX(parent);
    (*y_offset) += XtY(parent);
  }
  return;
}

static void SetPadVideo(Widget w, Boolean reverse)
{
  if (w) {
    int fore, back;
    XtVaGetValues(MainWidget, XmNbackground, &back, XmNforeground, &fore, NULL);
    XtVaSetValues(w, XmNbackground, reverse ? fore : back, XmNforeground, reverse ? back : fore,
		  NULL);
  }
}

static void Cut(Widget w, XEvent * event __attribute__ ((unused)),
		String * params __attribute__ ((unused)),
		Cardinal * num_params __attribute__ ((unused)))
{
  if (w == SelectedWidget)
    XtDisownSelection(w, XA_PRIMARY, XtLastTimestampProcessed(XtDisplay(w)));
  else if (XtOwnSelection
	   (w, XA_PRIMARY, XtLastTimestampProcessed(XtDisplay(w)),
	    (XtConvertSelectionProc) ConvertSelection, (XtLoseSelectionProc) LoseSelection, NULL)) {
    SetPadVideo(w, 1);
    SelectedWidget = w;
  }
}

static Boolean ConvertSelection(Widget w, Atom * selection __attribute__ ((unused)), Atom * target, Atom * type,
				XtPointer * value, unsigned int *length, int *format)
{
  int r = 0;
  int c = 0;
  char prefix[36];
  WaveInfo *info;
  FindWave(w, &c, &r);
  sprintf(prefix, "Pad.pad_%d_%d", r + 1, c + 1);
  XtVaGetValues(w, XmNuserData, &info, NULL);
  return ConvertWaveToSelection(w, prefix, info, *target, type, value, length, format);
}

static void LoseSelection(Widget w, Atom * selection __attribute__ ((unused)))
{
  SetPadVideo(w, 0);
  if (SelectedWidget == w)
    SelectedWidget = 0;
}

static void MakeWaves(int cols, int rows)
{
  int old_numwaves;
  int num_waves = cols * rows;
  int i;
  static XtTranslations translations = 0;
  XtVaGetValues(PadWidget, XmNnumChildren, &old_numwaves, NULL);
  if (!translations)
    translations = XtParseTranslationTable("<Btn1Down>:Cut()\n <Btn2Down>:Paste()");
  if (old_numwaves < num_waves) {
    for (i = old_numwaves; i < num_waves; i++) {
      WaveInfo *info = (WaveInfo *) XtMalloc(sizeof(WaveInfo));
      memset(info, 0, sizeof(WaveInfo));
      ResetWave(info);
      info->w = XmCreatePushButton(PadWidget, "pad_button", NULL, 0);
      XtVaSetValues(info->w, XmNrecomputeSize, 0, XmNhighlightThickness, 0, XtNtranslations,
		    translations, XmNuserData, info, NULL);
    }
  }
  Columns = cols;
  Rows = rows;
}

static void ResetWindow(String geometry, String title, String icon, int cols, int rows,
			Boolean bigfont, int btnwidth)
{
  int x;
  int y;
  unsigned int width;
  unsigned int height;
  int num;
  int i;
  int reset_dimensions = 0;
  Widget *widgets;
  XtUnmanageChild(PadWidget);
  XtVaGetValues(PadWidget, XmNnumChildren, &num, XmNchildren, &widgets, NULL);
  XtUnmanageChildren(widgets, num);
  if (geometry) {
    XParseGeometry(geometry, &x, &y, &width, &height);
    if (x >= WidthOfScreen(XtScreen(PadWidget)))
      x = WidthOfScreen(XtScreen(PadWidget)) - 50;
    if ((int)(y + height) > HeightOfScreen(XtScreen(PadWidget)))
      y = HeightOfScreen(XtScreen(PadWidget)) - height - 50;
    if (width > 200 && height > 100)
      XtVaSetValues(MainWidget, XtNwidth, width, XtNheight, height, NULL);
    else
      reset_dimensions = 1;
    XtVaSetValues(TopWidget, XtNx, x, XtNy, y, NULL);
  }
  XtVaSetValues(TopWidget, XtNtitle, title, XtNiconName, icon, NULL);
  ReplaceString(&PadTitle, title, 0);
  ReplaceString(&PadIcon, icon, 0);
  PadBigFont = bigfont;
  PadLabelWidth = max(btnwidth, 1);
  width = XTextWidth(font[bigfont & 1], "A", 1) * PadLabelWidth + 8;
  height = font[bigfont & 1]->max_bounds.ascent + font[bigfont & 1]->max_bounds.descent + 8;
  MakeWaves(cols, rows);
  num = Rows * Columns;
  XtVaGetValues(PadWidget, XmNchildren, &widgets, NULL);
  for (i = 0; i < num; i++) {
    WaveInfo *info;
    XtVaGetValues(widgets[i], XmNuserData, &info, NULL);
    XtVaSetValues(widgets[i], XmNwidth, width, XmNheight, height, XmNfontList,
		  fontlist[bigfont & 1], NULL);
    LabelSetString(widgets[i], info->pad_label);
  }
  XtVaSetValues(PadWidget, XmNnumColumns, Columns, NULL);
  XtManageChildren(widgets, Rows * Columns);
  XtManageChild(PadWidget);
  if (!geometry || reset_dimensions)
    XtVaSetValues(MainWidget, XtNwidth, min(1024, XtWidth(PadWidget) + 6),
		  XtNheight, min(1024,
				 XtHeight(PadWidget) +
				 XtHeight(XtNameToWidget(MainWidget, "menu_bar")) + 6), NULL);

}

static void RestoreDatabase(String dbname)
{
  int c;
  int r;
  Widget *widgets;
  XrmDatabase paddb = GetFileDatabase(dbname);
  int cols = max(atoi(GetResource(paddb, "Pad.columns", "1")), 1);
  int rows = max(atoi(GetResource(paddb, "Pad.rows", "10")), 1);
  MakeWaves(cols, rows);
  XtVaGetValues(PadWidget, XmNchildren, &widgets, NULL);
  for (c = 0; c < cols; c++)
    for (r = 0; r < rows; r++) {
      WaveInfo *info;
      XtVaGetValues(widgets[c * Rows + r], XmNuserData, &info, NULL);
      GetWaveFromDb(paddb, "Pad.pad", r, c, info);
    }
  ResetWindow(GetResource(paddb, "Pad.geometry", "50x50+100+100"),
	      GetResource(paddb, "Pad.title", "MDS Pad"), GetResource(paddb, "Pad.icon_name",
								      "Pad"), cols, rows,
	      atoi(GetResource(paddb, "Pad.font", "0")),
	      max(atoi(GetResource(paddb, "Pad.padlabelwidth", "25")), 1));
  XrmDestroyDatabase(paddb);
}

static void Paste(Widget w, XEvent * event __attribute__ ((unused)),
		  String * params __attribute__ ((unused)),
		  Cardinal * num_params __attribute__ ((unused)))
{
  XtGetSelectionValue(w, XA_PRIMARY, XA_TARGETS, (XtSelectionCallbackProc) PasteTypesComplete, 0,
		      XtLastTimestampProcessed(XtDisplay(w)));
}

static void PasteTypesComplete(Widget w, XtPointer cdata __attribute__ ((unused)),
			       Atom * selection __attribute__ ((unused)),
			       Atom * type __attribute__ ((unused)),
			       XtPointer value, unsigned int *length,
			       int *format __attribute__ ((unused)))
{
  unsigned int i;
  Atom req_type = XA_STRING;
  Atom *values = (Atom *) value;
  for (i = 0; i < *length; i++)
    if (values[i] == XA_DWSCOPE_PANEL) {
      req_type = XA_DWSCOPE_PANEL;
      break;
    }
  XtGetSelectionValue(w, XA_PRIMARY, req_type, (XtSelectionCallbackProc) PasteComplete, 0,
		      XtLastTimestampProcessed(XtDisplay(w)));
  if (value)
    XtFree(value);
}

static void PasteComplete(Widget w, XtPointer cdata __attribute__ ((unused)),
			  Atom * selection __attribute__ ((unused)), Atom * type, XtPointer value,
			  unsigned int *length,
			  int *format __attribute__ ((unused)))
{
  WaveInfo *info;
  XtVaGetValues(w, XmNuserData, &info, NULL);
  if (ConvertSelectionToWave(w, *type, *length, value, info)) {
    if (w == CurrentWidget)
      UpdatePopups();
    if (strlen(info->pad_label))
      LabelSetString(w, info->pad_label);
    else {
      PendingWidget = w;
      ResetLabel(w, 0, 0);
      XtManageChild(PadLabelWidget);
    }
    Modified = 1;
  }
  if (value)
    XtFree(value);
}

static String SetupTitle()
{
  static char title[80];
  int r = 0;
  int c = 0;
  FindWave(PendingWidget, &c, &r);
  sprintf(title, "Setup for button at row %d, column %d", r + 1, c + 1);
  return title;
}

static int FindWave(Widget w, int *c, int *r)
{
  int num;
  Widget *widgets;
  XtVaGetValues(PadWidget, XmNnumChildren, &num, XmNchildren, &widgets, NULL);
  for ((*c) = 0; (*c) < Columns; (*c)++)
    for ((*r) = 0; (*r) < Rows; (*r)++)
      if (widgets[(*c) * Rows + (*r)] == w)
	return 1;
  return 0;
}

static void WriteDatabase(String dbname)
{
  int r;
  int c;
  Position x;
  Position y;
  Dimension width;
  Dimension height;
  Widget *widgets;
  FILE *file = fopen(dbname, "w");
  if (file) {
    XtVaGetValues(PadWidget, XmNchildren, &widgets, NULL);
    XtVaGetValues(TopWidget, XtNx, &x, XtNy, &y, NULL);
    XtVaGetValues(MainWidget, XtNwidth, &width, XtNheight, &height, NULL);
    fprintf(file, "Pad.geometry: %dx%d+%d+%d\n", width, height, x, y);
    fprintf(file, "Pad.title: %s\n", PadTitle);
    fprintf(file, "Pad.icon_name: %s\n", PadIcon);
    fprintf(file, "Pad.columns: %d\n", Columns);
    fprintf(file, "Pad.rows: %d\n", Rows);
    fprintf(file, "Pad.padlabelwidth: %d\n", PadLabelWidth);
    fprintf(file, "Pad.font: %d\n", PadBigFont);
    for (c = 0; c < Columns; c++) {
      for (r = 0; r < Rows; r++) {
	WaveInfo *info;
	char prefix[36];
	int ctx = 0;
	String text;
	fprintf(file, "\n");
	sprintf(prefix, "Pad.pad_%d_%d", r + 1, c + 1);
	XtVaGetValues(widgets[c * Rows + r], XmNuserData, &info, NULL);
	while ((text = WaveToText(prefix, info, 0, &ctx))) {
	  fprintf(file, "%s", text);
	  XtFree(text);
	}
      }
    }
    fclose(file);
  } else
    PopupComplaint(MainWidget, "Error writing setup file");
}
