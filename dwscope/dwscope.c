/*  CMS REPLACEMENT HISTORY, Element DWSCOPE.C */
/*  *217  18-SEP-1997 14:21:11 TWF "Add brief/full messages" */
/*  *216  18-SEP-1997 14:19:31 TWF "Add brief/full messages" */
/*  *215  18-SEP-1997 13:39:50 TWF "Add brief/full messages" */
/*  *214  27-AUG-1997 15:38:07 TWF "Fix it" */
/*  *213  27-AUG-1997 15:07:06 TWF "Fix inline error messages" */
/*  *212  27-AUG-1997 14:52:53 TWF "Add multiline error messages" */
/*  *211  27-AUG-1997 14:50:36 TWF "Add multiline error messages" */
/*  *210  22-AUG-1997 08:35:12 TWF "Add print to file capability" */
/*  *209  18-MAR-1997 10:25:02 TWF "Fix for DECC 5.3" */
/*  *208  30-JAN-1997 09:14:57 TWF "Add cancel button" */
/*  *207  30-JAN-1997 09:03:26 TWF "Add cancel button" */
/*  *206  30-JAN-1997 09:01:48 TWF "Add cancel button" */
/*  *205  30-JAN-1997 09:00:32 TWF "Add cancel button" */
/*  *204  30-JAN-1997 08:41:29 TWF "Add cancel button" */
/*  *203  30-JAN-1997 08:11:35 TWF "Add cancel button" */
/*  *202  30-JAN-1997 08:10:16 TWF "Add cancel button" */
/*  *201  29-JAN-1997 16:19:16 TWF "Add cancel button" */
/*  *200  29-JAN-1997 14:33:38 TWF "Add snap save" */
/*  *199  29-JAN-1997 13:53:34 TWF "Add zoom save" */
/*  *198  29-JAN-1997 13:33:25 TWF "Add max/min settings" */
/*  *197  29-JAN-1997 13:29:57 TWF "Add max/min settings" */
/*  *196  27-NOV-1996 13:48:31 TWF "Fix for linux" */
/*  *195  23-AUG-1996 10:08:31 JAS "Destroy the resource database when done with it" */
/*  *194  23-APR-1996 15:53:37 TWF "Fix positionmark" */
/*  *193  23-APR-1996 15:51:35 TWF "Fix positionmark" */
/*  *192  23-APR-1996 15:39:59 TWF "Fix mark" */
/*  *191  23-APR-1996 15:32:48 TWF "Fix mark" */
/*  *190  23-APR-1996 15:11:11 TWF "Fix" */
/*  *189  23-APR-1996 15:09:58 TWF "dwscope" */
/*  *188  23-APR-1996 14:40:18 TWF "Fix mark" */
/*  *187  23-APR-1996 14:25:44 TWF "Fix positionmark" */
/*  *186  23-APR-1996 14:20:41 TWF "mark" */
/*  *185  23-APR-1996 11:52:44 TWF "Add measure" */
/*  *184  22-APR-1996 16:57:00 TWF "Add offset measurement" */
/*  *183  22-APR-1996 16:54:31 TWF "Add offset measurement" */
/*  *182  22-APR-1996 15:55:39 TWF "Add offset measurement" */
/*  *181  22-APR-1996 15:37:15 TWF "Add offset measurement" */
/*  *180  22-APR-1996 15:23:58 TWF "Add offset measurement" */
/*  *179  22-APR-1996 15:11:58 TWF "Add offset measurement" */
/*  *178  22-APR-1996 15:10:41 TWF "Add offset measurement" */
/*  *177  22-APR-1996 15:04:10 TWF "Add offset measurement" */
/*  *176  22-APR-1996 14:41:43 TWF "Add offset measurement" */
/*  *175  22-APR-1996 14:40:03 TWF "Add offset measurement" */
/*  *174  22-APR-1996 14:36:58 TWF "Add offset measurement" */
/*  *173  22-APR-1996 14:25:50 TWF "Add offset measurement" */
/*  *172   6-FEB-1996 16:47:07 TWF "Close files whether updating or not" */
/*  *171   5-JUL-1995 09:26:35 TWF "Fix EqualPanes" */
/*  *170   5-JUL-1995 08:45:40 TWF "Add button2 for equal panes" */
/*  *169  30-MAY-1995 16:51:59 TWF "Add all same x auto y and reset scales" */
/*  *168  30-MAY-1995 16:50:46 TWF "Add all same x auto y and reset scales" */
/*  *167  19-JAN-1995 11:44:56 TWF "Portability" */
/*  *166  14-NOV-1994 12:33:34 TWF "put default print file in sys$login" */
/*  *165   9-NOV-1994 15:36:52 TWF "port to unix" */
/*  *164   4-NOV-1994 16:13:36 TWF "Fix resize" */
/*  *163   4-NOV-1994 15:33:03 TWF "fix resize" */
/*  *162   4-NOV-1994 15:28:10 TWF "Fix resize" */
/*  *161   4-NOV-1994 15:22:38 TWF "Try fixing resize" */
/*  *160   4-OCT-1994 08:03:17 TWF "Unix" */
/*  *159   4-OCT-1994 08:00:32 TWF "Unix" */
/*  *158  29-SEP-1994 08:53:48 TWF "Unix porting" */
/*  *157  29-SEP-1994 08:36:56 TWF "Unix porting" */
/*  *156  23-JUN-1994 15:29:53 TWF "For Motif 1.2" */
/*  *155  23-JUN-1994 15:26:35 TWF "For Motif 1.2" */
/*  *154  22-APR-1994 08:51:12 TWF "Change crosshairs string to []" */
/*  *153  22-APR-1994 08:43:47 TWF "Add x,y text selection on Crosshairs" */
/*  *152   4-APR-1994 10:58:28 TWF "Do DWSCOPE_IDL in separate make" */
/*  *151  31-MAR-1994 13:19:13 MRL "Fix window title print." */
/*  *150  31-MAR-1994 10:53:35 MRL "Fix print." */
/*  *149  30-MAR-1994 14:22:17 MRL "Fix window title print." */
/*  *148  29-MAR-1994 16:18:06 MRL "Fix print." */
/*  *147  29-MAR-1994 13:25:22 MRL "Add print window title." */
/*  *146  24-MAR-1994 14:55:28 TWF "Fix waveformprint" */
/*  *145  24-MAR-1994 09:23:49 TWF "Revert" */
/*  *144  24-MAR-1994 09:20:16 TWF "Fix call to xmdswaveformprint" */
/*  *143  23-MAR-1994 17:11:26 TWF "Take out no x gbls" */
/*  *142  23-MAR-1994 16:50:27 TWF "New DECW includes" */
/*  *141  15-MAR-1994 13:24:16 TWF "Update only if" */
/*  *140  15-MAR-1994 13:11:21 TWF "Add update only if not event or not current" */
/*  *139  15-MAR-1994 13:08:56 TWF "Add update only if not event or not current" */
/*  *138  15-MAR-1994 13:06:43 TWF "Update only if " */
/*  *137  15-MAR-1994 11:57:00 TWF "Add row col index" */
/*  *136  15-MAR-1994 11:38:46 TWF "Add row col event to evaluate calls" */
/*  *135  15-MAR-1994 11:37:18 TWF "Add row col event to evaluate calls" */
/*  *134  15-MAR-1994 11:07:49 TWF "Add all same y" */
/*  *133  14-MAR-1994 16:43:11 TWF "Fix processtraversal" */
/*  *132  14-MAR-1994 16:37:28 TWF "Add accelerators" */
/*  *131  14-MAR-1994 16:35:07 TWF "Traverse to the shot override widget" */
/*  *130   2-MAR-1994 15:56:40 TWF "Fix accvio" */
/*  *129   2-MAR-1994 12:34:33 TWF "Add autoscale all" */
/*  *128   6-DEC-1993 15:13:10 TWF "change tick marks" */
/*  *127   6-DEC-1993 15:11:51 TWF "change tick marks" */
/*  *126   6-DEC-1993 15:10:41 TWF "change tick marks" */
/*  *125   3-NOV-1993 13:01:19 TWF "Fix uid ref" */
/*  *124   3-NOV-1993 12:21:59 TWF "Add font selection" */
/*  *123   7-OCT-1993 16:08:51 TWF "Reset scales on restore" */
/*  *122   7-OCT-1993 15:37:12 TWF "Reset scales on restore" */
/*  *121  21-SEP-1993 12:00:46 TWF "AXP" */
/*  *120  20-SEP-1993 14:14:21 TWF "Add autoscale y" */
/*  *119  25-AUG-1993 09:14:39 TWF "Auto update on expand from icon" */
/*  *118  20-AUG-1993 12:09:41 TWF "Add disable icon updates" */
/*  *117  20-AUG-1993 11:59:09 TWF "Add disable icon updates" */
/*  *116  20-AUG-1993 11:55:54 TWF "Add disable icon updates" */
/*  *115  20-AUG-1993 11:47:39 TWF "Add disable icon updates" */
/*  *114  20-AUG-1993 11:46:40 TWF "Add disable icon updates" */
/*  *113  20-AUG-1993 11:43:39 TWF "Add noupdate on shrink" */
/*  *112  20-AUG-1993 10:04:28 TWF "Add noupdate on shrink" */
/*  *111  20-AUG-1993 09:59:34 TWF "Add noupdate on shrink" */
/*  *110  20-AUG-1993 09:55:56 TWF "Add noupdate on shrink" */
/*  *109  20-AUG-1993 09:53:58 TWF "Add noupdate on shrink" */
/*  *108  20-AUG-1993 09:50:26 TWF "Add noupdate on shrink" */
/*  *107  20-AUG-1993 09:48:22 TWF "Fix grab again" */
/*  *106  20-AUG-1993 09:24:26 TWF "Fix grab again" */
/*  *105  20-AUG-1993 09:17:40 TWF "Fix grab again" */
/*  *104  20-AUG-1993 09:04:10 TWF "Add auto disable updates" */
/*  *103  20-AUG-1993 08:20:54 TWF "Add live updates" */
/*  *102  20-AUG-1993 08:20:21 TWF "Add live updates" */
/*  *101  20-AUG-1993 08:13:12 TWF "Add live updates" */
/*  *100  23-JUL-1993 15:37:39 TWF "New disabled resource on waveform widget" */
/*  *99   19-JUL-1993 10:09:41 TWF "Ungrab pointer on button3 combos" */
/*  *98   15-JUL-1993 14:49:28 TWF "Do not popup Setup if any other buttons are pressed" */
/*  *97   15-JUL-1993 14:47:54 TWF "Do not popup Setup if any other buttons are pressed" */
/*  *96    9-JUL-1993 16:34:25 TWF "Fix ACCVIO" */
/*  *95    9-JUL-1993 16:15:37 TWF "Fix compile errors" */
/*  *94   16-JUN-1993 08:06:14 TWF "Fix window title when using shot override" */
/*  *93   11-JUN-1993 17:03:58 TWF "Fix erase global" */
/*  *92   11-JUN-1993 16:46:50 TWF "add new features" */
/*  *91   11-JUN-1993 16:45:15 TWF "Add new attachment" */
/*  *90    9-JUN-1993 08:11:59 TWF "Avoid zoom during equalpanes" */
/*  *89   27-MAY-1993 12:28:32 TWF "Fix hang" */
/*  *88   27-MAY-1993 12:15:53 TWF "Fix hang" */
/*  *87   27-MAY-1993 12:14:36 TWF "Fix hang" */
/*  *86   27-MAY-1993 11:58:15 TWF "Fix hang" */
/*  *85   27-MAY-1993 10:38:22 TWF "Fix hang" */
/*  *84   27-MAY-1993 10:20:26 TWF "Can't use get_ef for ef" */
/*  *83   27-MAY-1993 10:13:01 TWF "Fix menu hang" */
/*  *82   27-MAY-1993 10:11:57 TWF "Fix menu hang" */
/*  *81   19-MAY-1993 15:24:44 TWF "Remove close data source calls" */
/*  *80   19-MAY-1993 14:37:38 TWF "Add TREE$SET_STACK_SIZE" */
/*  *79   17-MAY-1993 17:53:15 TWF "Use preopen event" */
/*  *78   12-MAY-1993 15:47:54 TWF "Setup event after paste" */
/*  *77    1-MAY-1993 11:08:56 JAS "change the way atoms are delt with" */
/*  *76   22-APR-1993 09:04:11 TWF "Fix % in complain" */
/*  *75   19-MAR-1993 12:43:09 TWF "Free paste data" */
/*  *74   12-MAR-1993 15:28:53 TWF "Fix writing in constant string" */
/*  *73   12-MAR-1993 13:59:49 TWF "Make few lines" */
/*  *72   24-FEB-1993 17:51:26 TWF "Use DECC" */
/*  *71   18-NOV-1992 15:13:16 TWF "Use same widgets for save and restore" */
/*  *70    4-AUG-1992 16:06:23 JAS "Change paste to ask about avail types" */
/*  *69   13-APR-1992 08:10:50 TWF "Fix resize" */
/*  *68    9-APR-1992 17:04:33 TWF "Fix resize" */
/*  *67    9-APR-1992 16:46:11 TWF "Fix resize" */
/*  *66    9-APR-1992 16:44:13 TWF "Fix resize" */
/*  *65    9-APR-1992 16:01:43 TWF "Fix resize" */
/*  *64    9-APR-1992 15:48:39 TWF "Fix resize" */
/*  *63    9-APR-1992 15:29:34 TWF "Fix resize" */
/*  *62    9-APR-1992 14:31:32 TWF "Fix resize" */
/*  *61    9-APR-1992 14:26:08 TWF "Fix resize" */
/*  *60    9-APR-1992 14:11:37 TWF "Fix resize" */
/*  *59    9-APR-1992 13:57:37 TWF "Fix resize" */
/*  *58    9-APR-1992 13:40:05 TWF "Fix resize" */
/*  *57    9-APR-1992 12:57:51 TWF "Fix resize" */
/*  *56    8-APR-1992 14:40:57 TWF "Fix sizing" */
/*  *55   19-MAR-1992 11:25:09 JAS "trim the events" */
/*  *54   19-MAR-1992 11:23:11 JAS "trim the events" */
/*  *53   19-MAR-1992 11:20:50 JAS "trim the events" */
/*  *52    9-JAN-1992 09:31:19 TWF "Fix accvio" */
/*  *51    2-JAN-1992 10:09:32 MRL "Fix print orientation." */
/*  *50   17-DEC-1991 16:14:57 TWF "remove show timer" */
/*  *49   12-DEC-1991 12:56:15 TWF "Speedup print" */
/*  *48   12-DEC-1991 08:40:51 TWF "Reduce print time" */
/*  *47   12-DEC-1991 08:09:52 TWF "Reduce print time" */
/*  *46   12-DEC-1991 07:59:16 TWF "Reduce print time" */
/*  *45   11-DEC-1991 13:47:24 TWF "put blank lines between panels in saved setting" */
/*  *44   11-DEC-1991 13:45:45 TWF "put blank lines between panels in saved setting" */
/*  *43   11-DEC-1991 11:25:35 TWF "Busy getting stuck?" */
/*  *42   11-DEC-1991 09:05:31 TWF "Set topwidget size" */
/*  *41   10-DEC-1991 13:14:46 TWF "Fix calls to evaluatedata" */
/*  *40    6-DEC-1991 15:05:48 TWF "Make ApplyDataSetup update window title if global" */
/*  *39    6-DEC-1991 14:54:58 TWF "Make sure global wave is filled in" */
/*  *38    6-DEC-1991 14:43:16 TWF "Open global shot, if specified, when setting window title" */
/*  *37    6-DEC-1991 14:41:09 TWF "Open global shot, if specified, when setting window title" */
/*  *36    4-DEC-1991 11:34:46 TWF "Update data only when needed" */
/*  *35    4-DEC-1991 10:46:58 TWF "Update data only when needed" */
/*  *34    4-DEC-1991 10:40:29 TWF "Update data only when needed" */
/*  *33    2-DEC-1991 14:19:01 TWF "Fix icon on multiplane systems" */
/*  *32    2-DEC-1991 12:39:03 TWF "Fix icon on multiplane systems" */
/*  *31    2-DEC-1991 12:38:18 TWF "" */
/*  *30    2-DEC-1991 09:26:32 TWF "Close database at appropriate times" */
/*  *29    2-DEC-1991 09:07:25 TWF "Fix resizing" */
/*  *28   27-NOV-1991 17:16:46 TWF "fix icon on color screens" */
/*  *27   27-NOV-1991 16:14:15 TWF "Fix icon on color screens" */
/*  *26   27-NOV-1991 15:49:44 TWF "Fix icon on color screens" */
/*  *25   27-NOV-1991 15:33:24 TWF "fix vertical pane" */
/*  *24   27-NOV-1991 13:17:10 TWF "Use globals during restore" */
/*  *23   27-NOV-1991 12:57:08 TWF "Fix sizing during resize" */
/*  *22   27-NOV-1991 12:38:40 TWF "Fix sizing during resize" */
/*  *21   27-NOV-1991 11:52:05 TWF "Fix restoreinprog" */
/*  *20   27-NOV-1991 10:26:36 TWF "Fix restoreinprog" */
/*  *19   26-NOV-1991 08:25:30 TWF "Disable resize when restoring database" */
/*  *18   25-NOV-1991 16:52:34 TWF "Disable resize when restoring database" */
/*  *17   25-NOV-1991 16:28:23 TWF "Fix window sizing" */
/*  *16   25-NOV-1991 16:10:31 TWF "Fix window sizing" */
/*  *15   25-NOV-1991 15:39:52 TWF "Fix window sizing" */
/*  *14   25-NOV-1991 15:25:41 TWF "Fix window sizing" */
/*  *13   25-NOV-1991 14:54:35 TWF "Fix window sizing" */
/*  *12   25-NOV-1991 14:51:34 TWF "Fix window sizing" */
/*  *11   25-NOV-1991 14:18:55 TWF "Fix window sizing" */
/*  *10   25-NOV-1991 13:15:36 TWF "Fix default" */
/*  *9    25-NOV-1991 13:06:58 TWF "Fix restore data setup" */
/*  *8    25-NOV-1991 12:10:11 TWF "remove excess setvalues on waveforms" */
/*  *7    25-NOV-1991 10:36:08 TWF "add show mode" */
/*  *6    25-NOV-1991 10:32:01 TWF "add show mode" */
/*  *5    15-NOV-1991 11:44:17 TWF "prepare for MDSV1" */
/*  *4    15-NOV-1991 10:46:08 TWF "Prepare to make MDSV1 version" */
/*  *3    13-NOV-1991 14:07:01 TWF "Fix printall code" */
/*  *2    12-NOV-1991 15:01:42 TWF "Make title update happen first" */
/*  *1    12-NOV-1991 14:32:17 TWF "Scope program" */
/*  CMS REPLACEMENT HISTORY, Element DWSCOPE.C */
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include <Xm/FileSB.h>
#include <Xm/ToggleBG.h>
#include <Xmds/XmdsWaveform.h>
#include "dwscope.h"
#include "dwscope_icon.xbm"

#ifndef _NO_DXm
#include <DXm/DXmPrint.h>
#include <DXm/DECspecific.h>
#endif

extern void XmdsInitialize();
extern void XmdsDestroyWidgetCallback();
extern void XmdsManageChildCallback();
extern void XmdsRegisterWidgetCallback();
extern void XmdsUnmanageChildCallback();

extern void  SetupEvent(String event, Boolean *received, void **id);
extern void SetupEventInput(XtAppContext app_context);
extern Boolean ConvertSelectionToWave(Widget w, Atom result_type, unsigned int length, XtPointer header, WaveInfo *info);
extern Boolean ConvertWaveToSelection(Widget w, String prefix, WaveInfo *wave, Atom target, Atom *type, XtPointer *header,
				                unsigned int *length, int *format);
extern Boolean ReplaceString(String *old, String new, Boolean free);
extern String GetResource(XrmDatabase db, String resource, String default_answer);
extern String WaveToText(String prefix, WaveInfo *wave, Dimension height, int *ctx);
extern void LoadDataSetup(Widget w, String title, WaveInfo *info);
extern void /*XtCallbackProc*/DisplayHelp(Widget w, XtPointer client_data, XtPointer callback_data);
extern void GetDataSetup(Widget w, WaveInfo *info, int *change_mask);
extern void GetWaveFromDb(XrmDatabase db, String prefix, int row, int col, WaveInfo *info);
extern Boolean GetWaveZoomFromDb(XrmDatabase db, String prefix, int row, int col, float *zoom);
extern void ResetWave(WaveInfo *info);
extern void SetDirMask(Widget w, String *file, XtPointer callback_data);
extern XrmDatabase GetFileDatabase(String file_spec);
extern void /*XtCallbackProc*/DisableGlobalDefault(Widget w, XtPointer client_data, XtPointer callback_data);
extern void /*XtCallbackProc*/InitDefaultsSetupWidget(Widget w, XtPointer client_data, XtPointer callback_data);
extern void /*XtCallbackProc*/ExpandReset(Widget w, int *tag, XtPointer callback_data);
extern void /*XtCallbackProc*/ExpandCancel(Widget w, int *tag, XtPointer callback_data);
extern void /*XtCallbackProc*/ExpandOk(Widget w, int *tag, XtPointer callback_data);
extern void PositionPopupMenu(Widget w,XButtonEvent *event);
extern void PopupComplaint(Widget parent,String string);

extern Boolean   EvaluateData(Boolean brief, int row, int col, int index, Boolean *event, 
                                 String database, String shot, String default_node, String x, String y,
		                 XmdsWaveformValStruct *x_ret, XmdsWaveformValStruct *y_ret, String *error);
extern Boolean   EvaluateText(String text, String error_prefix, String *text_ret, String *error);
extern void CloseDataSources();

static String GlobalShot();
static void RaiseWindows();
static    Boolean /*XtConvertSelectionProc*/ConvertSelection(Widget w, Atom *selection, Atom *target, Atom *type, XtPointer *value,
							              unsigned int *length, int *format);
static int UpdateWaveform(Boolean complain, WaveInfo *info, Boolean event, int global_change_mask, int change_mask);
static void /*XtCallbackProc*/GridStyle(Widget w, XtPointer client_data, XmAnyCallbackStruct *cb);
static void Shrink();
static void Expand();
static void MoveVerticalPane(Widget w, XEvent *event, String *params, Cardinal *num_params);
static void Resize(Widget w, XEvent *event, String *params, Cardinal *num_params);
static void /*XtActionProc*/EqualPanes(Widget w, XEvent *event, String *string, Cardinal *num_strings);
static void /*XtActionProc*/Paste(Widget w, XEvent *event, String *params, Cardinal *num_params);
static void /*XtCallbackProc*/ApplyCustomizeWindow(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data);
static void /*XtCallbackProc*/ApplyCustomizePrint(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data);
static void /*XtCallbackProc*/ApplyFont(Widget w, XtPointer client_data, XmSelectionBoxCallbackStruct *callback_data);
static void /*XtCallbackProc*/ CreateCustomizeFont(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data);
static void /*XtCallbackProc*/Crosshairs(Widget w, XtPointer client_data, XmdsWaveformCrosshairsCBStruct *cb);
static void /*XtCallbackProc*/Exit(Widget w, XtPointer client_data, XmAnyCallbackStruct *reason);
static void /*XtCallbackProc*/SetPointerMode(Widget w, int *mode, int *reason);
static void /*XtCallbackProc*/SetPointerModeMenu(Widget w, int *mode, int *reason);
static void /*XtCallbackProc*/Align(int w, XtPointer client_data, XmdsWaveformLimitsCBStruct *l);
static void /*XtCallbackProc*/Ok(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data);
static void /*XtCallbackProc*/Autoscale(Widget w, String type, XmAnyCallbackStruct *callback_data);
static void /*XtCallbackProc*/Refresh(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data);
static void /*XtCallbackProc*/Restore(Widget w, int *option, XmFileSelectionBoxCallbackStruct *reason);
static void /*XtCallbackProc*/Save(Widget w, int *option, XmFileSelectionBoxCallbackStruct *reason);
static void /*XtCallbackProc*/ResetDataSetup(Widget w, int *global, XmAnyCallbackStruct *callback_data);
static void /*XtCallbackProc*/ResetCustomizeFont(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data);
static void /*XtCallbackProc*/ResetCustomizePrint(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data);
static void /*XtCallbackProc*/ResetCustomizeWindow(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data);
static void /*XtCallbackProc*/ApplyDataSetup(Widget w, int *mode, XtPointer callback_data);
static void /*XtCallbackProc*/ApplyOverride(Widget w, int *mode, XtPointer callback_data);
static void /*XtCallbackProc*/Updates(Widget w, int *mode, XtPointer callback_data);
static void /*XtCallbackProc*/UpdatesMenuButton(Widget w, int *mode, XtPointer callback_data);
static void /*XtCallbackProc*/PrintAll(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data);
static void /*XtCallbackProc*/Print(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data);
static void /*XtCallbackProc*/Clear(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data);
static void /*XtCallbackProc*/Cut(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_struct);
static void /*XtCallbackProc*/RegisterPane(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data);
static void /*XtCallbackProc*/RegisterWave(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data);
static void /*XtCallbackProc*/RegisterSash(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data);
static void Setup(Widget w, XtPointer client_data, XButtonEvent *event, Boolean *continue_to_dispatch);
void /*XtInputCallbackProc*/EventUpdate(XtPointer client_data, int *source, XtInputId *id);
static void /*XtLoseSelectionProc*/LoseSelection(Widget w, Atom *selection);
static void /*XtSelectionCallbackProc*/PasteComplete(Widget w, WaveInfo *info, Atom *selection, Atom *type, XtPointer value,
						              unsigned int *length, int *format);
static void /*XtSelectionCallbackProc*/PasteTypesComplete(Widget w, XtPointer cdata, Atom *selection, Atom *type, XtPointer value,
						              unsigned int *length, int *format);
static void  Complain(WaveInfo *info, char mode, String error);
static void  CopyWave(WaveInfo *in, WaveInfo *out);
static void  FreeWave(WaveInfo *info);
static void  Busy();
static void  ManageWaveforms();
static void  RemoveZeros(String string, int *length);
static void  Unbusy();
static void  ClearWaveform(WaveInfo *info);
static void  RestoreDatabase(String dbname);
static void  WriteDatabase(String dbname,Boolean zoom);
static void  GetNewLimits(WaveInfo *info, float **xmin, float **xmax, float **ymin, float **ymax);
static void  SetWindowTitles();
static WaveInfo *GetPending(Widget w);
static Window CreateBusyWindow(Widget w);
static String SetupTitle();
static XrmDatabase MdsGetFileDatabase(String file_spec);

#define min(a,b) ( ((a)<(b)) ? (a) : (b) )
#define max(a,b) ( ((a)>(b)) ? (a) : (b) )
#define PlotsWidget XtNameToWidget(MainWidget,"plots")

#define MaxCols 4
#define MaxRows 16

#define XA_DWSCOPE_PANEL XInternAtom(XtDisplay(TopWidget), "DWSCOPE_PANEL", 0)
#define XA_TARGETS XInternAtom(XtDisplay(TopWidget), "TARGETS", 0)

static Widget TopWidget;
static Widget MainWidget;
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
static WaveInfo *PasteWave = 0;
static WaveInfo GlobalWave;
static Boolean CloseDataSourcesEventReceived = 0;
static Boolean ScopePrintEventReceived = 0;
static Boolean ScopeTitleEventReceived = 0;
static String ScopePrintEvent = 0;
static String ScopeTitleEvent = 0;
static String ScopePrintFile = 0;
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

int       main(int argc, String *argv)
{
  int       i;
  static String hierarchy_names[] = {{"dwscope.uid"}};
  String fallback_resources[] = {"*default_label.labelString: Default node:",
                                 "*exp_label.labelString: Experiment:",
				 "*brief_errors.set: true",NULL};
  static MrmRegisterArg register_list[] = {
			    {"Align", (XtPointer)Align},
			    {"ApplyCustomizePrint", (XtPointer)ApplyCustomizePrint},
			    {"ApplyCustomizeWindow", (XtPointer)ApplyCustomizeWindow},
			    {"ApplyDataSetup", (XtPointer)ApplyDataSetup},
			    {"ApplyFont", (XtPointer)ApplyFont},
			    {"ApplyOverride", (XtPointer)ApplyOverride},
			    {"Autoscale", (XtPointer)Autoscale},
			    {"Clear", (XtPointer)Clear},
                            {"CreateCustomizeFont", (XtPointer)CreateCustomizeFont},
			    {"Crosshairs", (XtPointer)Crosshairs},
			    {"Cut", (XtPointer)Cut},
			    {"DisableGlobalDefault", (XtPointer)DisableGlobalDefault},
			    {"Exit", (XtPointer)Exit},
			    {"InitDefaultsSetupWidget", (XtPointer)InitDefaultsSetupWidget},
			    {"Ok", (XtPointer)Ok},
			    {"Paste", (XtPointer)Paste},
			    {"Print", (XtPointer)Print},
			    {"PrintAll", (XtPointer)PrintAll},
			    {"Refresh", (XtPointer)Refresh},
			    {"RegisterPane", (XtPointer)RegisterPane},
			    {"RegisterSash", (XtPointer)RegisterSash},
			    {"RegisterWave", (XtPointer)RegisterWave},
			    {"ResetCustomizePrint", (XtPointer)ResetCustomizePrint},
			    {"ResetCustomizeFont", (XtPointer)ResetCustomizeFont},
			    {"ResetCustomizeWindow", (XtPointer)ResetCustomizeWindow},
			    {"ResetDataSetup", (XtPointer)ResetDataSetup},
			    {"Restore", (XtPointer)Restore},
			    {"Save", (XtPointer)Save},
			    {"DisplayHelp", (XtPointer)DisplayHelp},
			    {"SetPointerMode", (XtPointer)SetPointerMode},
			    {"SetPointerModeMenu", (XtPointer)SetPointerModeMenu},
			    {"Button3Widget", (XtPointer)&Button3Widget},
			    {"CustomizeWindowWidget", (XtPointer)&CustomizeWindowWidget},
			    {"CustomizeFontWidget", (XtPointer)&CustomizeFontWidget},
			    {"CustomizePrintWidget", (XtPointer)&CustomizePrintWidget},
			    {"DataSetupWidget", (XtPointer)&DataSetupWidget},
			    {"DefaultsSetupWidget", (XtPointer)&DefaultsSetupWidget},
                            {"expand_ok",(XtPointer)ExpandOk},
                            {"expand_reset",(XtPointer)ExpandReset},
                            {"expand_cancel",(XtPointer)ExpandCancel},
                            {"Updates",(XtPointer)Updates},
                            {"UpdatesMenuButton",(XtPointer)UpdatesMenuButton},
                            {"GridStyle",(XtPointer)GridStyle},
			    {"XmdsDestroyWidgetCallback", (XtPointer) XmdsDestroyWidgetCallback},
			    {"XmdsManageChildCallback", (XtPointer) XmdsManageChildCallback},
			    {"XmdsRegisterWidgetCallback", (XtPointer) XmdsRegisterWidgetCallback},
			    {"XmdsUnmanageChildCallback", (XtPointer) XmdsUnmanageChildCallback} };

  static XtActionsRec actions[] = {{"MoveVerticalPane", MoveVerticalPane},
				  {"EqualPanes", EqualPanes},
				  {"Resize", Resize},
                                  {"Shrink", Shrink},
                                  {"Expand", Expand}};
  MrmType   class;
  static XrmOptionDescRec options[] = {{"-defaults", "*defaults", XrmoptionSepArg, NULL}};
  static XtResource resources[] = {{"defaults", "Defaults", XtRString, sizeof(String), 0, XtRString,
				    "SCOPE_DEFAULTS.DAT"}};
  Cursor    cursor;
  int       r;
  int       c;
  void *cds_id = 0;
  XmString  deffile;
  MrmHierarchy drm_hierarchy;
  MrmInitialize();

#ifndef _NO_DXm
  DXmInitialize();
#endif
  XmdsInitialize();

  MrmRegisterClass(MrmwcUnknown,"XmdsWaveformWidgetClass","XmdsCreateWaveform",XmdsCreateWaveform,xmdsWaveformWidgetClass);
  MrmRegisterNames(register_list, XtNumber(register_list));
  TopWidget = XtVaAppInitialize(&AppContext, "DwScope", options, XtNumber(options), &argc, argv, fallback_resources,
				XmNallowShellResize, 1, XmNminWidth, 320, XmNminHeight, 100, NULL);
  XtGetApplicationResources(TopWidget, &defaultfile, resources, XtNumber(resources), (Arg *) NULL, 0);
  defaultfile = strlen(defaultfile) ? XtNewString(defaultfile) : XtNewString("SCOPE_DEFAULTS.DAT");
  XtAppAddActions(AppContext, actions, XtNumber(actions));
  XtAugmentTranslations(TopWidget, XtParseTranslationTable("#augment <ResizeRequest> : Resize() \n\
                                                                     <Unmap> : Shrink()\n\
                                                                     <Map> : Expand()"));
  MrmOpenHierarchy(XtNumber(hierarchy_names), hierarchy_names, 0, &drm_hierarchy);
  MrmFetchWidget(drm_hierarchy, "scope", TopWidget, &MainWidget, &class);
  MrmCloseHierarchy(drm_hierarchy);
  if (!MainWidget)
    { printf("Problem loading UID\n");
      exit(1);
    }

#ifdef __VMS
  SetDirMask(XtNameToWidget(TopWidget,"*file_dialog"),&defaultfile,0);
#endif

  XtVaSetValues(Pane[0], XmNleftAttachment, XmATTACH_FORM, NULL);
  for (c = 1; c < NumPanes; c++)
    XtVaSetValues(Pane[c], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET, XmNleftWidget, Sash[c - 1], XmNleftOffset, 2, NULL);
  XtManageChild(MainWidget);
  RestoreDatabase(defaultfile);
  cursor = XCreateFontCursor(XtDisplay(TopWidget), XC_crosshair);
  for (i = 0; i < NumSashs; i++)
    XDefineCursor(XtDisplay(TopWidget), XtWindow(Sash[i]), cursor);
  for (c = 0; c < MaxCols; c++)
    for (r = 0; r < MaxRows; r++)
    {
      int       p_r = (r + 1) % MaxRows;
      int       p_c = p_r ? c : (c + 1) % MaxCols;
      XtVaSetValues(Wave[c][r].w, XmdsNpanWith, Wave[p_c][p_r].w, NULL);
    }
  BusyWindow = CreateBusyWindow(PlotsWidget);
  XtAddEventHandler(PlotsWidget, ButtonPressMask | ButtonReleaseMask, False, (XtEventHandler)Setup, 0);
  SetupEventInput(AppContext);
  XtVaSetValues(TopWidget, XtNiconPixmap, 
    XCreatePixmapFromBitmapData(XtDisplay(TopWidget), XtWindow(TopWidget), dwscope_icon_bits, dwscope_icon_width, 
      dwscope_icon_height, BlackPixelOfScreen(XtScreen(TopWidget)), WhitePixelOfScreen(XtScreen(TopWidget)), 1), NULL);
  SetupEvent("DWSCOPE_CLOSE_FILES", &CloseDataSourcesEventReceived,&cds_id);
  XmProcessTraversal(XtNameToWidget(MainWidget,"*override_shot"),XmTRAVERSE_CURRENT);
  XtAppMainLoop(AppContext);
}

static void Shrink(Widget w, XEvent *event)
{
  if (XmToggleButtonGadgetGetState(XtNameToWidget(w,"*disable_icon_updates")))
  {
    UpdateWhenExpand = XmToggleButtonGadgetGetState(XtNameToWidget(w,"*updates"));
    XmToggleButtonGadgetSetState(XtNameToWidget(w,"*updates"),FALSE,TRUE);
  }
}

static void Expand(Widget w, XEvent *event)
{
  if (XmToggleButtonGadgetGetState(XtNameToWidget(w,"*disable_icon_updates")) && UpdateWhenExpand)
    XmToggleButtonGadgetSetState(XtNameToWidget(w,"*updates"),TRUE,TRUE);
}

static void /*XtCallbackProc*/UpdatesMenuButton(Widget w, int *mode, XtPointer callback_data)
{
  XmToggleButtonGadgetSetState(XtNameToWidget(MainWidget,"*updates"),!XmToggleButtonGadgetGetState(w),TRUE);
}

static String XYString(float x, float y)
{
  static char label[42] = {'['};
  int length;
  String ptr = (String)&label[1];
  sprintf(ptr, "%g%n", x, &length);
  RemoveZeros(ptr+1, &length);
  ptr[length] = ',';
  ptr = &ptr[length + 1];
  sprintf(ptr, "%g%n", y, &length);
  RemoveZeros(ptr, &length);
  ptr[length] = ']';
  ptr[length + 1] = 0;
  return (String)label;
}

void PositionMark()
{
  if (!MarkWidget)
    MarkWidget = XtNameToWidget(MainWidget,"*crosshairs_mark");
  if (DeltaWaveWidget && XtIsManaged(DeltaWaveWidget))
  {
    float *xmin,*xmax,*ymin,*ymax;
    Position plots_rootx,plots_rooty;
    Position wave_rootx,wave_rooty;
    Position x,y;
    Position xpos,ypos;
    XtVaGetValues(DeltaWaveWidget,XmdsNxMin,&xmin,XmdsNxMax,&xmax,XmdsNyMin,&ymin,XmdsNyMax,&ymax,NULL);
    XtTranslateCoords(DeltaWaveWidget,0,0,&wave_rootx,&wave_rooty);
    XtTranslateCoords(PlotsWidget,0,0,&plots_rootx,&plots_rooty);
    x = wave_rootx - plots_rootx;
    y = wave_rooty - plots_rooty;
    xpos = ((DeltaX - *xmin)/(*xmax - *xmin)*XtWidth(DeltaWaveWidget)+x-5)/XtWidth(PlotsWidget) * 1000;
    ypos = ((*ymax - DeltaY)/(*ymax - *ymin)*XtHeight(DeltaWaveWidget)+y-5)/XtHeight(PlotsWidget) * 1000;
    if (xpos < 0 || ypos < 0 || xpos > 1000 || ypos > 1000)
      XtUnmanageChild(MarkWidget);
    else
    {
      XtVaSetValues(MarkWidget,XmNleftPosition,xpos,XmNtopPosition,ypos,NULL);
      XtManageChild(MarkWidget);
    }
  }
  else
    XtUnmanageChild(MarkWidget);
}

static void /*XtCallbackProc*/Crosshairs(Widget w, XtPointer client_data, XmdsWaveformCrosshairsCBStruct *cb)
{
  static Widget value_w = 0;
  int       r;
  int       c;
  float     display_x;
  float     display_y;
  String label;
  if (!value_w)
    value_w = XtNameToWidget(MainWidget, "*crosshairs_value");
  if (cb->event->xbutton.type == ButtonPress)
  { 
    if (cb->event->xbutton.state & ShiftMask)
    {  
      DeltaWaveWidget = DeltaWaveWidget ? 0 : w;
      if (DeltaWaveWidget)
      {
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
  if (DeltaWaveWidget)
  {
    display_x -= DeltaX;
    display_y -= DeltaY;
  }
  label = XYString(display_x,display_y);
  XmTextFieldSetString(value_w, label);
  if (cb->event->xbutton.type == ButtonRelease)
    XmTextFieldSetSelection(value_w,0,strlen(label),cb->event->xbutton.time);
}

static void /*XtCallbackProc*/Exit(Widget w, XtPointer client_data, XmAnyCallbackStruct *reason)
{
  exit(1);
}

static void /*XtCallbackProc*/SetPointerMode(Widget w, int *mode, int *reason)
{
  if (XmToggleButtonGetState(w))
  {
    int       r;
    int       c;
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

static void /*XtCallbackProc*/SetPointerModeMenu(Widget w, int *mode, int *reason)
{
  int       r;
  int       c;
  switch (*mode)
  {
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

static void  RemoveZeros(String string, int *length)
{
  int       i;
  int       done = 0;
  while (!done)
  {
    done = 1;
    for (i = *length; i > 0; i--)
    {
      if (((string[i] == 'e') && (string[i + 1] == '0')) ||
	  ((string[i] == '-') && (string[i + 1] == '0')) ||
	  ((string[i] == '+') && (string[i + 1] == '0')))
      {
	int       j;
	for (j = i + 1; j < *length; j++)
	  string[j] = string[j + 1];
	(*length)--;
	done = 0;
      }
      else if ((string[i] == '0') && (string[i + 1] == 'e'))
      {
	int       j;
	for (j = i; j < *length; j++)
	  string[j] = string[j + 1];
	(*length)--;
	done = 0;
      }
      else if ((string[i] == '.') && (string[i + 1] == 'e'))
      {
	int       j;
	for (j = i; j < *length; j++)
	  string[j] = string[j + 1];
	(*length)--;
      }
    }
  }
  return;
}

static void /*XtCallbackProc*/Align(int w, XtPointer client_data, XmdsWaveformLimitsCBStruct *l)
{
  int       r;
  int       c;
  for (c = 0; c < MaxCols; c++)
    for (r = 0; r < MaxRows; r++)
      XtVaSetValues(Wave[c][r].w, XmdsNxMin, l->xminval, XmdsNxMax, l->xmaxval, NULL);
}

static void /*XtCallbackProc*/GridStyle(Widget w, XtPointer client_data, XmAnyCallbackStruct *cb)
{
  int lines = XmToggleButtonGadgetGetState(w);
  int       r;
  int       c;
  for (c = 0; c < MaxCols; c++)
    for (r = 0; r < MaxRows; r++)
      XtVaSetValues(Wave[c][r].w, XmdsNgridStyle, lines ? XmdsGRID_STYLE_LINES : XmdsGRID_STYLE_TICKS, NULL);
}

static int scope_height,
					            scope_width,
					            scope_x,
					            scope_y;

static void /*XtCallbackProc*/Ok(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data)
{
  if (ApplyStatus & 1)
    XtUnmanageChild(XtParent(w));
}

static String SetupTitle()
{
  int       c;
  int       r;
  static char title[80];
  for (c = 0; c < Columns; c++)
    for (r = 0; r < Rows[c]; r++)
      if (&Wave[c][r] == CurrentWave)
	goto found;
  strcpy(title,"Global Default Settings");
  return title;
found:
  sprintf(title, "Setup for plot at row %d, column %d", r + 1, c + 1);
  return title;
}

static void /*XtCallbackProc*/Autoscale(Widget w, String type, XmAnyCallbackStruct *callback_data)
{
  int       r;
  int       c;
  switch (type[0])
  {
    case 'y':
      XtVaSetValues(PendingWave->w, XmdsNyMin, 0, XmdsNyMax, 0, NULL); break;
    case 'x':
      XtVaSetValues(PendingWave->w, XmdsNxMin, 0, XmdsNxMax, 0, NULL); break;
    case 'b':
      XtVaSetValues(PendingWave->w, XmdsNxMin, 0, XmdsNxMax, 0, XmdsNyMin, 0, XmdsNyMax, 0, NULL); break;
    case 'Y':
      for (c = 0; c < MaxCols; c++)
        for (r = 0; r < MaxRows; r++)
          XtVaSetValues(Wave[c][r].w, XmdsNyMin, 0, XmdsNyMax, 0, NULL); break;
    case 'X':
      for (c = 0; c < MaxCols; c++)
        for (r = 0; r < MaxRows; r++)
          XtVaSetValues(Wave[c][r].w, XmdsNxMin, 0, XmdsNxMax, 0, NULL); break;
    case 'B':
      for (c = 0; c < MaxCols; c++)
        for (r = 0; r < MaxRows; r++)
          XtVaSetValues(Wave[c][r].w, XmdsNxMin, 0, XmdsNxMax, 0, XmdsNyMin, 0, XmdsNyMax, 0, NULL); break;
    case '0':
      {
        float *xMin;
        float *xMax;
        float *yMin;
        float *yMax;
        XtVaGetValues(PendingWave->w, XmdsNxMin, &xMin, XmdsNxMax, &xMax, XmdsNyMin, &yMin, XmdsNyMax, &yMax, NULL);
        for (c = 0; c < MaxCols; c++)
          for (r = 0; r < MaxRows; r++)
            if (PendingWave->w != Wave[c][r].w)
              XtVaSetValues(Wave[c][r].w, XmdsNxMin, xMin, XmdsNxMax, xMax, XmdsNyMin, yMin, XmdsNyMax, yMax, NULL);
        break;
      }
    case '1':
      {
        float *xMin;
        float *xMax;
        float *yMin;
        float *yMax;
        XtVaGetValues(PendingWave->w, XmdsNxMin, &xMin, XmdsNxMax, &xMax, XmdsNyMin, &yMin, XmdsNyMax, &yMax, NULL);
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
        XtVaGetValues(PendingWave->w, XmdsNxMin, &xMin, XmdsNxMax, &xMax, XmdsNyMin, &yMin, XmdsNyMax, &yMax, NULL);
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
        XtVaGetValues(PendingWave->w, XmdsNxMin, &xMin, XmdsNxMax, &xMax, XmdsNyMin, &yMin, XmdsNyMax, &yMax, NULL);
        for (c = 0; c < MaxCols; c++)
          for (r = 0; r < MaxRows; r++)
            XtVaSetValues(Wave[c][r].w, XmdsNxMin, xMin, XmdsNxMax, xMax, XmdsNyMin, 0, XmdsNyMax, 0, NULL);
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
        XtVaSetValues(PendingWave->w, XmdsNxMin, xMin, XmdsNxMax, xMax, XmdsNyMin, yMin, XmdsNyMax, yMax, NULL);
        break;
      }
    case '5':
      {
        float *xMin;
        float *xMax;
        float *yMin;
        float *yMax;
        for (c = 0; c < MaxCols; c++)
          for (r = 0; r < MaxRows; r++)
          {
            Boolean update = Wave[c][r].update;
            Wave[c][r].update = TRUE;
            GetNewLimits(&Wave[c][r], &xMin, &xMax, &yMin, &yMax);
            Wave[c][r].update = update;
            XtVaSetValues(Wave[c][r].w, XmdsNxMin, xMin, XmdsNxMax, xMax, XmdsNyMin, yMin, XmdsNyMax, yMax, NULL);
          }
        break;
      }
  }
  PositionMark();
}

static void /*XtCallbackProc*/Refresh(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data)
{
  UpdateWaveform(0, PendingWave, 0, -1, -1);
}

static void /*XtCallbackProc*/Restore(Widget w, int *option, XmFileSelectionBoxCallbackStruct *reason)
{
  int opt = option ? *option : 0;
  switch (opt)
  {
    case 0:
            {
              if (reason->length)
              {
                String filename;
                if (XmStringGetLtoR(reason->value, (XmStringCharSet) XmSTRING_DEFAULT_CHARSET, &filename))
                {
                  int       length = strlen(filename);
                  if (length)
                  {
                    RestoreDatabase(filename);
                    if (defaultfile) 
                      XtFree(defaultfile);
                    defaultfile = filename;
                  }
                  else
                    XtFree(filename);
               }
             }
             break;
           }
    case 1:
            RestoreDatabase(defaultfile);
            break;
    case 2: 
            { Widget w = XtNameToWidget(TopWidget,"*file_dialog");
              XmString title = XmStringCreateSimple("Restore Current Settings From");
              XmString label = XmStringCreateSimple("Select Name of Configuration File: ");
              XmString deffile = XmStringCreateSimple(defaultfile);
              XmString dirmask;
              XtRemoveAllCallbacks(w,XmNokCallback);
              XtAddCallback(w,XmNokCallback,(XtCallbackProc)Restore,0);
              XtVaGetValues(w,XmNdirMask,&dirmask,0);
              XmFileSelectionDoSearch(w, dirmask);
              XtVaSetValues(w, XmNdialogTitle, title, XmNselectionLabelString, label, XmNtextString, deffile, NULL);
              XtManageChild(w);
              XmStringFree(title);
              XmStringFree(label);
              XmStringFree(deffile);
              break;
            }
  }
}

static void /*XtCallbackProc*/Save(Widget w, int *option, XmFileSelectionBoxCallbackStruct *reason)
{
  int opt = option ? *option : 0;
  switch (opt)
  {
    case 0:
            {
              if (reason->length)
              {
                String filename;
                if (XmStringGetLtoR(reason->value, (XmStringCharSet) XmSTRING_DEFAULT_CHARSET, &filename))
                {
                  int       length = strlen(filename);
                  if (length)
                  {
                    WriteDatabase(filename,FALSE);
                    if (defaultfile) 
                      XtFree(defaultfile);
                    defaultfile = filename;
                  }
                  else
                    XtFree(filename);
               }
             }
             break;
           }
    case 1:
            WriteDatabase(defaultfile,FALSE);
            break;
    case 2: 
            { Widget w = XtNameToWidget(TopWidget,"*file_dialog");
              XmString title = XmStringCreateSimple("Save Current Settings As");
              XmString label = XmStringCreateSimple("Select Name for Configuration File: ");
              XmString deffile = XmStringCreateSimple(defaultfile);
              XmString dirmask;
              XtRemoveAllCallbacks(w,XmNokCallback);
              XtAddCallback(w,XmNokCallback,(XtCallbackProc)Save,0);
              XtVaGetValues(w,XmNdirMask,&dirmask,0);
              XmFileSelectionDoSearch(w, dirmask);
              XtVaSetValues(w, XmNdialogTitle, title, XmNselectionLabelString, label, XmNtextString, deffile, NULL);
              XtManageChild(w);
              XmStringFree(title);
              XmStringFree(label);
              XmStringFree(deffile);
              break;
            }
    case 3:
            {
              if (reason->length)
              {
                String filename;
                if (XmStringGetLtoR(reason->value, (XmStringCharSet) XmSTRING_DEFAULT_CHARSET, &filename))
                {
                  int       length = strlen(filename);
                  if (length)
                  {
                    WriteDatabase(filename,TRUE);
                    if (defaultfile) 
                      XtFree(defaultfile);
                    defaultfile = filename;
                  }
                  else
                    XtFree(filename);
               }
             }
             break;
           }
    case 4:
            WriteDatabase(defaultfile,TRUE);
            break;
    case 5: 
            { Widget w = XtNameToWidget(TopWidget,"*file_dialog");
              XmString title = XmStringCreateSimple("Save Current Settings As");
              XmString label = XmStringCreateSimple("Select Name for Configuration File: ");
              XmString deffile = XmStringCreateSimple(defaultfile);
              XmString dirmask;
              static int option = 3;
              XtRemoveAllCallbacks(w,XmNokCallback);
              XtAddCallback(w,XmNokCallback,(XtCallbackProc)Save,(void *)&option);
              XtVaGetValues(w,XmNdirMask,&dirmask,0);
              XmFileSelectionDoSearch(w, dirmask);
              XtVaSetValues(w, XmNdialogTitle, title, XmNselectionLabelString, label, XmNtextString, deffile, NULL);
              XtManageChild(w);
              XmStringFree(title);
              XmStringFree(label);
              XmStringFree(deffile);
              break;
            }
  }
}

static void  ManageWaveforms()
{
  int       row;
  int       col;
  int       max_rows = 0;
  static XtTranslations translations = 0;
  if (!translations)
    translations = XtParseTranslationTable("Shift<Btn1Up>:EqualPanes(H)\n <Btn2Up>:EqualPanes(H)");
  for (col = 0; col < Columns; col++)
  {
    Widget    w[MaxRows];
    if (Rows[col] > max_rows)
      max_rows = Rows[col];
    for (row = Rows[col]; row < MaxRows; row++)
    {
      if (&Wave[col][row] == SelectedWave)
	XtDisownSelection(SelectedWave->w, XA_PRIMARY, XtLastTimestampProcessed(XtDisplay(SelectedWave->w)));
    }
    for (row = 0; row < MaxRows; row++)
      w[row] = Wave[col][row].w;
    XtManageChildren(w, Rows[col]);
    if (Rows[col] < MaxRows)
      XtUnmanageChildren(&w[Rows[col]],MaxRows - Rows[col]);
  }
  for (col = 0; col < Columns - 1; col++)
    XtVaSetValues(Pane[col], XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET, XmNrightWidget, Sash[col], XmNrightOffset, 2, NULL);
  XtVaSetValues(Pane[Columns - 1], XmNrightAttachment, XmATTACH_FORM, NULL);
  if (Columns > 1)
    XtManageChildren(Sash, Columns - 1);
  XtManageChildren(Pane, Columns);
  for (col = 0; col < Columns; col++)
  {
    int       i;
    int       num;
    Widget   *child;
    XtVaGetValues(Pane[col],XmNnumChildren,&num,XmNchildren,&child,NULL);
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

static void /*XtCallbackProc*/ResetDataSetup(Widget w, int *global, XmAnyCallbackStruct *callback_data)
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

static void /*XtCallbackProc*/ResetCustomizeWindow(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data)
{
  int       i;
  XmTextSetString(XtNameToWidget(CustomizeWindowWidget, "window_title"), ScopeTitle);
  XmTextSetString(XtNameToWidget(CustomizeWindowWidget, "icon_name"), ScopeIcon);
  XmTextSetString(XtNameToWidget(CustomizeWindowWidget, "title_event"), ScopeTitleEvent);
  for (i = 0; i < MaxCols; i++)
  {
    char      name[8];
    sprintf(name, "rows_%d", i + 1);
    XmScaleSetValue(XtNameToWidget(CustomizeWindowWidget, name), Rows[i]);
  }
}

static void /*XtCallbackProc*/ResetCustomizePrint(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data)
{
  XmTextSetString(XtNameToWidget(CustomizePrintWidget, "print_file"), ScopePrintFile);
  XmTextSetString(XtNameToWidget(CustomizePrintWidget, "print_event"), ScopePrintEvent);
  XmToggleButtonSetState(XtNameToWidget(CustomizePrintWidget,"print_window_title"),ScopePrintWindowTitle,1);
  XmToggleButtonSetState(XtNameToWidget(CustomizePrintWidget,"print_to_file"),ScopePrintToFile,1);
}

static void /*XtCallbackProc*/CreateCustomizeFont(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data)
{
  int c_count;
  String *courier_fonts = XListFonts(XtDisplay(w),"-*-courier-*-*-*--*-*-*-*-*-*-*-*",1000,&c_count);
  int h_count;
  String *helvetica_fonts = XListFonts(XtDisplay(w),"-*-helvetica-*-*-*--*-*-*-*-*-*-*-*",1000,&h_count);
  int n_count;
  String *newc_fonts = XListFonts(XtDisplay(w),"-*-new century schoolbook-*-*-*--*-*-*-*-*-*-*-*",1000,&n_count);
  int t_count;
  String *times_fonts = XListFonts(XtDisplay(w),"-*-times-*-*-*--*-*-*-*-*-*-*-*",1000,&t_count);
  int count = c_count + h_count + n_count + t_count;
  XmStringTable table = (XmStringTable)XtMalloc((count+1) * sizeof(XmString));
  int i;
  int j;
  for (i=0,j=0;i<c_count;i++,j++)
    table[j] = XmStringCreateSimple(courier_fonts[i]);
  for (i=0;i<h_count;i++,j++)
    table[j] = XmStringCreateSimple(helvetica_fonts[i]);
  for (i=0;i<n_count;i++,j++)
    table[j] = XmStringCreateSimple(newc_fonts[i]);
  for (i=0;i<t_count;i++,j++)
    table[j] = XmStringCreateSimple(times_fonts[i]);
  table[j] = 0;
  XtVaSetValues(w,XmNlistItemCount,count,XmNlistItems,table,NULL);
  for (i=0;i<count;i++)
    XmStringFree(table[i]);
  XtFree((String)table);
  XFreeFontNames(courier_fonts);
  XFreeFontNames(helvetica_fonts);
  XFreeFontNames(newc_fonts);
  XFreeFontNames(times_fonts);
}

static void  Setup(Widget w, XtPointer client_data, XButtonEvent *event, Boolean *continue_to_dispatch)
{
  if (event->button == Button3)
  {
    if ((event->type == ButtonPress) && !(event->state & (Button1Mask | Button2Mask)))
    {
      int       col;
      int       row;
      for (col = 0; col < Columns - 1 && event->x > XtX(Sash[col]); col++);
      for (row = 0; row < Rows[col] - 1 && event->y > XtY(Wave[col][row].w) + XtHeight(Wave[col][row].w); row++);
      PendingWave = &Wave[col][row];
      PositionPopupMenu(Button3Widget, event);
      XtManageChild(Button3Widget);
    }
    else
    {
      XUngrabPointer(XtDisplay(Button3Widget), CurrentTime);
      XtPopdown(XtParent(Button3Widget));
    }
  }
}

static void /*XtCallbackProc*/ApplyCustomizeWindow(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data)
{
  int       c;
  int       r;
  int       old_columns = Columns;
  int      old_rows[MaxRows];
  int      pane_height = XtHeight(Pane[0]);
  int      num;
  Widget   *widgets;
  ReplaceString(&ScopeTitle, XmTextGetString(XtNameToWidget(CustomizeWindowWidget, "window_title")), 1);
  ReplaceString(&ScopeIcon, XmTextGetString(XtNameToWidget(CustomizeWindowWidget, "icon_name")), 1);
  ReplaceString(&ScopeTitleEvent, XmTextGetString(XtNameToWidget(CustomizeWindowWidget, "title_event")), 1);
  SetupEvent(ScopeTitleEvent, &ScopeTitleEventReceived, &ScopeTitleEventId);
  SetWindowTitles();
  XtVaGetValues(PlotsWidget,XmNnumChildren,&num,XmNchildren,&widgets,NULL);
  XtUnmanageChildren(widgets,num);
  Columns = 1;
  memcpy(old_rows,Rows,sizeof(old_rows));
  for (c = 0; c < MaxCols; c++)
  {
    char      name[8];
    sprintf(name, "rows_%d", c + 1);
    XmScaleGetValue(XtNameToWidget(CustomizeWindowWidget, name), &Rows[c]);
    if (Rows[c])
      Columns = c + 1;
    else
      Rows[c] = 1;
  }
  for (c = 0; c < MaxCols; c++)
  {
    for (r = 0; r < MaxRows; r++)
    {
      if (c >= Columns)
      {
        ClearWaveform(&Wave[c][r]);
        Rows[c] = 0;
      }
      else if (r >= Rows[c])
        ClearWaveform(&Wave[c][r]);
      else
      {
        if ((r < Rows[c]) && (Rows[c] != old_rows[c]))
        {
          int height = (pane_height - 2 * Rows[c] + (r % 2) * .4999) / Rows[c];
  	  XtVaSetValues(Wave[c][r].w, XtNheight, height, NULL);
        }
        if ((r >= old_rows[c]) && (r < Rows[c]))
          UpdateWaveform(0, &Wave[c][r], 0, -1, -1);
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

static void /*XtCallbackProc*/ApplyCustomizePrint(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data)
{
  ReplaceString(&ScopePrintFile, XmTextGetString(XtNameToWidget(CustomizePrintWidget, "print_file")), 1);
  ReplaceString(&ScopePrintEvent, XmTextGetString(XtNameToWidget(CustomizePrintWidget, "print_event")), 1);
  ScopePrintWindowTitle = XmToggleButtonGetState(XtNameToWidget(CustomizePrintWidget,"print_window_title"));
  ScopePrintToFile = XmToggleButtonGetState(XtNameToWidget(CustomizePrintWidget,"print_to_file"));
  SetupEvent(ScopePrintEvent, &ScopePrintEventReceived, &ScopePrintEventId);
}

static void SetFont(String font)
{
  int c;
  int r;
  XFontStruct *font_struct = XLoadQueryFont(XtDisplay(TopWidget),font);
  if (font_struct)
  {
    for (c = 0; c < MaxCols; c++)
      for (r = 0; r < MaxRows; r++)
        if (Wave[c][r].w)
          XtVaSetValues(Wave[c][r].w, XmdsNlabelFont, font_struct, NULL);
  }
  else {
    static String prefix="Unknown font selected: ";
    String error = XtMalloc(strlen(prefix)+strlen(font)+1);
    sprintf(error,"%s%s",prefix,font);
    PopupComplaint(TopWidget, error);
    XtFree(error);
  }
}

static void ResetCustomizeFont(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data)
{
  String font;
  XFontStruct *font_struct;
  unsigned long fontprop;
  XmString font_string;
  XtVaGetValues(Wave[0][0].w, XmdsNlabelFont, &font_struct, NULL);
  XGetFontProperty(font_struct,XA_FONT,&fontprop);
  font = XGetAtomName(XtDisplay(TopWidget), fontprop);
  font_string = XmStringCreateSimple(font);
  XtVaSetValues(CustomizeFontWidget, XmNtextString, font_string, NULL);
  XmStringFree(font_string);
  XtFree(font);
}

static void /*XtCallbackProc*/ApplyFont(Widget w, XtPointer client_data, XmSelectionBoxCallbackStruct *callback_data)
{
  String font;
  if (XmStringGetLtoR(callback_data->value,XmSTRING_DEFAULT_CHARSET,&font) && strlen(font))
    SetFont(font);
}

void /*XtInputCallbackProc*/EventUpdate(XtPointer client_data, int *source, XtInputId *id)
{
  int       r;
  int       c;
  XAllowEvents(XtDisplay(Button3Widget), AsyncPointer, CurrentTime);
  if (ScopeTitleEventReceived)
  {
    SetWindowTitles();
    ScopeTitleEventReceived = 0;
  }
  if (UpdatesOn)
  {
    for (c = 0; c < MaxCols; c++)
      for (r = 0; r < MaxRows; r++)
      if (Wave[c][r].received)
      {
	UpdateWaveform(0, &Wave[c][r], 1, -1, -1);
	Wave[c][r].received = 0;
      }
    if (ScopePrintEventReceived)
    {
      PrintAll(0, 0, 0);
      ScopePrintEventReceived = 0;
    }
  }
  if (CloseDataSourcesEventReceived)
  {
    CloseDataSourcesEventReceived = 0;
    CloseDataSources();
  }
  return;
}

struct _UpdateWaveformsInfo { int r;
                              int c;
                            };

static Boolean UpdateWaveformsWorkproc(XtPointer settings)
{
  struct _UpdateWaveformsInfo *info = (struct _UpdateWaveformsInfo *)settings;  
  UpdateWaveform(0, &Wave[info->c][info->r], 0, -1, -1);
  info->r++;
  if (info->r >= Rows[info->c])
  {
    info->r = 0;
    info->c++;
    if (info->c >= Columns)
    {
      XmString label = XmStringCreateSimple("Apply");
      XtVaSetValues(XtNameToWidget(MainWidget,"*override_shot_apply"),XmNlabelString,label,XmNmarginWidth,5,NULL);
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

static void /*XtCallbackProc*/ApplyOverride(Widget w, int *mode, XtPointer callback_data)
{
  static struct _UpdateWaveformsInfo info;
  if (UpdateWaveformsWorkProcID != 0)
  {
    XmString label = XmStringCreateSimple("Apply");
    XtRemoveWorkProc(UpdateWaveformsWorkProcID);
    XtVaSetValues(XtNameToWidget(MainWidget,"*override_shot_apply"),XmNlabelString,label,XmNmarginWidth,5,NULL);
    UpdateWaveformsWorkProcID = 0;
    XmStringFree(label);
  }
  else
  {
    XmString label = XmStringCreateSimple("Cancel");
    XtVaSetValues(XtNameToWidget(MainWidget,"*override_shot_apply"),XmNlabelString,label,XmNmarginWidth,2,NULL);
    info.r = 0;
    info.c = 0;
    UpdateWaveformsWorkProcID = XtAppAddWorkProc(AppContext,UpdateWaveformsWorkproc,(XtPointer)&info);
    XmStringFree(label);
  }
}

static void /*XtCallbackProc*/Updates(Widget w, int *mode, XtPointer callback_data)
{
  UpdatesOn = XmToggleButtonGadgetGetState(w);
  if (UpdatesOn)
    EventUpdate(0,0,0);
  XmToggleButtonGadgetSetState(XtNameToWidget(MainWidget,"*updates_menu"),!XmToggleButtonGadgetGetState(w),FALSE);
}

static void /*XtCallbackProc*/ApplyDataSetup(Widget w, int *mode, XtPointer callback_data)
{
  int       change_mask;
  if (CurrentWave != &GlobalWave)
  {
    if (*mode)
    {
      String event;
      WaveInfo  info;
      CopyWave(CurrentWave, &info);
      GetDataSetup(DataSetupWidget, &info, &change_mask);
      ApplyStatus = UpdateWaveform(*mode, &info, 0, 0, change_mask);
      if (ApplyStatus & 1)
      {
	String    event = info._global.global.event ? GlobalWave.event : info.event;
	FreeWave(CurrentWave);
	*CurrentWave = info;
	SetupEvent(event, &CurrentWave->received, &CurrentWave->eventid);
	if (ApplyStatus & 1 && CurrentWave == SelectedWave)
	  XtDisownSelection(SelectedWave->w, XA_PRIMARY, XtLastTimestampProcessed(XtDisplay(w)));
      }
      else
	FreeWave(&info);
    }
    else
    {
      GetDataSetup(DataSetupWidget, CurrentWave, &change_mask);
      XtUnmanageChild(DataSetupWidget);
    }
  }
  else
  {
    int       r;
    int       c;
    GetDataSetup(DataSetupWidget, &GlobalWave, &change_mask);
    for (c = 0; c < Columns; c++)
      for (r = 0; r < Rows[c]; r++)
	if (Wave[c][r]._global.global_defaults)
	{
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

static void  CopyWave(WaveInfo *in, WaveInfo *out)
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

static void  FreeWave(WaveInfo *info)
{
  XtFree(info->database);
  XtFree(info->shot);
  XtFree(info->default_node);
  XtFree(info->y);
  XtFree(info->x);
  XtFree(info->title);
  XtFree(info->event);
  XtFree(info->print_title);
  XtFree(info->pad_label);
}

static void /*XtActionProc*/MoveVerticalPane(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
  Position       main_x_root;
  Position       main_y_root;
  static Position min_offset;
  static Position max_offset;
  int       i;
  static Widget separator = 0;
  XtTranslateCoords(PlotsWidget, 0, 0, &main_x_root, &main_y_root);
  if (!separator)
    separator = XtNameToWidget(TopWidget, "*pane_separator");
  if (event->type == ButtonPress)
  {
    for (i = 0; i < NumSashs; i++)
      if (Sash[i] == w)
	break;
    if (i)
    {
      XtVaGetValues(Sash[i - 1], XmNx, &min_offset, NULL);
      min_offset += 10;
    }
    else
      min_offset = 10;
    if (i < (NumSashs - 1) && XtIsManaged(Sash[i + 1]))
    {
      XtVaGetValues(Sash[i + 1], XmNx, &max_offset, NULL);
      max_offset -= 10;
    }
    else
      max_offset = XtWidth(MainWidget) - 10;
    separator->core.widget_class->core_class.compress_motion = 1;
    XtManageChild(separator);
  }
  XtVaSetValues(separator, XmNleftOffset, min(max_offset, max(min_offset, event->xbutton.x_root - main_x_root)), NULL);
  if (event->type == ButtonRelease)
  {
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
  FILE      *file = fopen(file_spec, "r");
  XrmDatabase db = 0;
  if (file)
  {
    char      line_text[1024];
    char      doubled[1024];
    while (fgets(line_text, 1024, file))
    {
      int       i;
      int       j;
      int       put_it = 0;
      int       size = strlen(line_text);
      for (i = 0; i < size; i++)
	if (line_text[i] == ':')
	  break;
      for (j = i + 1; j < size; j++)
	if (line_text[j] != 9 && line_text[j] != 10 && line_text[j] != ' ')
	{
	  put_it = 1;
	  break;
	}
      if (put_it)
      {
	j = 0;
	for (i = 0; i < size; i++)
	{
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
		       (unsigned int) 0, CopyFromParent, InputOnly,
		       CopyFromParent, valuemask, &attributes);
}

static void  Busy()
{
  if (BusyLevel++)
    return;
  if (BusyWindow)
  {
    XEvent    event;
    XMapRaised(XtDisplay(MainWidget), BusyWindow);
    while (XCheckMaskEvent(XtDisplay(MainWidget),
			   ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | PointerMotionMask | KeyPressMask, &event));
    XFlush(XtDisplay(MainWidget));
  }
}

static void  Unbusy()
{
  if (--BusyLevel)
    return;
  if (BusyWindow)
    XUnmapWindow(XtDisplay(MainWidget), BusyWindow);
}

static void /*XtCallbackProc*/PrintAll(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data)
{

#ifndef _NO_DPS
  FILE     *printfid = fopen(ScopePrintFile, "w"); /*,"rop=RAH,WBH","mbc=8","mbf=2","deq=32"); */
  int       width = XtWidth(PlotsWidget);
  int       height = XtHeight(PlotsWidget);
  int       r;
  int       c;
  if (printfid)
  {
    XmString  filenames[1];
    int       orientation = 0;
    filenames[0] = XmStringCreateSimple(ScopePrintFile);

#ifndef _NO_DXm
    XtVaGetValues(CustomizePrintWidget, DXmNorientation, &orientation, NULL);
#endif

    Busy();
    for (c = 0; c < Columns; c++)
      for (r = 0; r < Rows[c]; r++)
      if (ScopePrintWindowTitle)
      {
        String    title;
        String    title_error;
        Boolean   title_status = EvaluateText(ScopeTitle, " ", &title, &title_error);
        if (!title_status)
        {
          XtFree(title_error);
          XmdsWaveformPrint(Wave[c][r].w, printfid, width, height, orientation, 
			Wave[c][r].print_title_evaluated, 0, 0);
        }
        else
          XmdsWaveformPrint(Wave[c][r].w, printfid, width, height, orientation, 
			Wave[c][r].print_title_evaluated, title, 0);
        XtFree(title);
      }
      else
	XmdsWaveformPrint(Wave[c][r].w, printfid, width, height, orientation, 
			Wave[c][r].print_title_evaluated, 0, 0);
    fclose(printfid);
#ifndef _NO_DXm
    if (!ScopePrintToFile)
      DXmPrintWgtPrintJob(CustomizePrintWidget, filenames, 1);
#endif /* DXmNorientation */
    XmStringFree(filenames[0]);
    Unbusy();
  }
  else {
    static String prefix="Error creating printfile, ";
    String error = XtMalloc(strlen(prefix)+strlen(ScopePrintFile)+1);
    sprintf(error,"%s%s",prefix,ScopePrintFile);
    PopupComplaint(TopWidget, error);
    XtFree(error);
  }
#endif /* _NO_DPS */
  return;
}

static void /*XtCallbackProc*/Print(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data)
{
#ifndef _NO_DPS
  int       count;
  XtVaGetValues(PendingWave->w, XmdsNcount, &count, NULL);
  if (count)
  {
    FILE     *printfid = fopen(ScopePrintFile, "w");
    if (printfid)
    {
      XmString  filenames[1];
      int       orientation = 0;
      filenames[0] = XmStringCreateSimple(ScopePrintFile);
#ifndef _NO_DXm
      XtVaGetValues(CustomizePrintWidget, DXmNorientation, &orientation, NULL);
#endif
      Busy();
      if (ScopePrintWindowTitle)
      {
        String    title;
        String    title_error;
        Boolean   title_status = EvaluateText(ScopeTitle, " ", &title, &title_error);
        if (!title_status)
        {
          XtFree(title_error);
          XmdsWaveformPrint(PendingWave->w, printfid, 0, 0, orientation, PendingWave->print_title_evaluated, 0, 0);
        }
        else
          XmdsWaveformPrint(PendingWave->w, printfid, 0, 0, orientation, PendingWave->print_title_evaluated, title, 0);
        XtFree(title);
      }
      else
        XmdsWaveformPrint(PendingWave->w, printfid, 0, 0, orientation, PendingWave->print_title_evaluated, 0, 0);
      fclose(printfid);
#ifndef _NO_DXm
      if (!ScopePrintToFile)
        DXmPrintWgtPrintJob(CustomizePrintWidget, filenames, 1);
#endif
      XmStringFree(filenames[0]);
      Unbusy();
    }
    else {
      static String prefix="Error creating printfile, ";
      String error = XtMalloc(strlen(prefix)+strlen(ScopePrintFile)+1);
      sprintf(error,"%s%s",prefix,ScopePrintFile);
      PopupComplaint(TopWidget, error);
      XtFree(error);
    }
  }
  else
    PopupComplaint(TopWidget, "No data to print");
#endif /* _NO_DPS */
  return;
}

static void /*XtCallbackProc*/Clear(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data)
{
  if (SelectedWave && SelectedWave == GetPending(w))
    XtDisownSelection(SelectedWave->w, XA_PRIMARY, XtLastTimestampProcessed(XtDisplay(w)));
}

static WaveInfo *GetPending(Widget w)
{
  int       r;
  int       c;
  for (c = 0; c < MaxCols; c++)
    for (r = 0; r < MaxRows; r++)
      if (Wave[c][r].w == w)
	goto found;
  return PendingWave;
found:
  return &Wave[c][r];
}

static void /*XtCallbackProc*/Cut(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_struct)
{
  WaveInfo *pending = GetPending(w);
  if (pending == SelectedWave)
    XtDisownSelection(SelectedWave->w, XA_PRIMARY, XtLastTimestampProcessed(XtDisplay(w)));
  else if (XtOwnSelection(pending->w, XA_PRIMARY, XtLastTimestampProcessed(XtDisplay(w)), 
         (XtConvertSelectionProc)ConvertSelection, LoseSelection, NULL))
  {
    SelectedWave = pending;
    XmdsWaveformReverse(SelectedWave->w, 1);
  }
}

static    Boolean /*XtConvertSelectionProc*/ConvertSelection(Widget w, Atom *selection, Atom *target, Atom *type, XtPointer *value,
							              unsigned int *length, int *format)
{
  int       r = 0;
  int       c = 0;
  char      prefix[36];
  for (c = 0; c < Columns; c++)
    for (r = 0; r < Rows[c]; r++)
      if (Wave[c][r].w == w)
	goto found;
  return 0;
found:
  sprintf(prefix, "Scope.plot_%d_%d", r + 1, c + 1);
  return ConvertWaveToSelection(w, prefix, &Wave[c][r], *target, type, value, length, format);
}

static void /*XtLoseSelectionProc*/LoseSelection(Widget w, Atom *selection)
{
  XmdsWaveformReverse(w, 0);
  if (SelectedWave && (SelectedWave->w == w))
    SelectedWave = 0;
}

static void /*XtActionProc*/Paste(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
  WaveInfo *pending = GetPending(w);
  if (pending)
    XtGetSelectionValue(pending->w, XA_PRIMARY, XA_TARGETS, (XtSelectionCallbackProc)PasteTypesComplete, 
    pending, XtLastTimestampProcessed(XtDisplay(w)));
}


static void /*XtSelectionCallbackProc*/PasteTypesComplete(Widget w, XtPointer cdata, Atom *selection, Atom *type, XtPointer value,
						              unsigned int *length, int *format)
{
  int i;
  Atom req_type = XA_STRING;
  Atom *values = (Atom *)value;
  for (i=0; i<*length; i++)
    if (values[i] == XA_DWSCOPE_PANEL) {
      req_type = XA_DWSCOPE_PANEL;
      break;
    }
  XtGetSelectionValue(w, XA_PRIMARY, req_type, (XtSelectionCallbackProc) PasteComplete, 
    cdata, XtLastTimestampProcessed(XtDisplay(w)));
  if (value)
    XtFree(value);
}

static void /*XtSelectionCallbackProc*/PasteComplete(Widget w, WaveInfo *info, Atom *selection, Atom *type, XtPointer value,
						              unsigned int *length, int *format)
{
  if (ConvertSelectionToWave(w, *type, *length, value, info))
  {
    UpdateWaveform(0, info, 0, -1, -1);
    SetupEvent(info->_global.global.event ? GlobalWave.event : info->event, &info->received, &info->eventid);
  }
  if (value)
    XtFree(value);
}

static void /*XtCallbackProc*/RegisterPane(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data)
{
  Pane[NumPanes++] = w;
  NumWaves = 0;
}

static void /*XtCallbackProc*/RegisterWave(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data)
{
  Wave[NumPanes - 1][NumWaves].w = w;
  ResetWave(&Wave[NumPanes - 1][NumWaves++]);
}

static void /*XtCallbackProc*/RegisterSash(Widget w, XtPointer client_data, XmAnyCallbackStruct *callback_data)
{
  Sash[NumSashs++] = w;
}

static String GlobalShot()
{
  String override_shot = XmTextFieldGetString(XtNameToWidget(TopWidget,"*override_shot"));
  return strlen(override_shot) ? override_shot : GlobalWave.shot;
}

static int UpdateWaveform(Boolean complain, WaveInfo *info, Boolean event, int global_change_mask, int change_mask)
{

#define changed(field) ((info->_global.global.field ? global_change_mask : change_mask) & M_##field)

  Boolean   new_grid = changed(x_grid_lines) || changed(y_grid_lines) || changed(x_grid_labels) || changed(y_grid_labels) ||
                       changed(show_mode) || changed(step_plot);
  info->received = 0;
  Busy();
  if (changed(shot) || changed(database) || changed(default_node) || changed(x) || changed(y) || changed(title) || 
      changed(print_title))
  {
    XmdsWaveformValStruct x_wave;
    XmdsWaveformValStruct y_wave;
    String    title_evaluated = 0;
    String    error = 0;
    String    shot = info->_global.global.shot ? GlobalShot() : info->shot;
    String    database = info->_global.global.database ? GlobalWave.database : info->database;
    String    default_node = info->_global.global.default_node ? GlobalWave.default_node : info->default_node;
    String    x = info->_global.global.x ? GlobalWave.x : info->x;
    String    y = info->_global.global.y ? GlobalWave.y : info->y;
    String    title = info->_global.global.title ? GlobalWave.title : info->title;
    String    print_title = info->_global.global.print_title ? GlobalWave.print_title : info->print_title;
    float    *xmin;
    float    *xmax;
    float    *ymin;
    float    *ymax;
    Boolean  update = 1;
    int r;
    int c;
    int idx = 0;
    Boolean brief = !(complain || !XmToggleButtonGadgetGetState(XtNameToWidget(TopWidget,"*brief_errors")));
    for (c = 0; c < Columns; c++)
      for (r = 0; r < Rows[c]; r++,idx++)
        if (Wave[c][r].w == info->w) goto found;
found:
    if (!EvaluateData(brief, r, c, idx, event ? &update : (Boolean *)0, 
          database, shot, default_node, x, y, &x_wave, &y_wave, &error))
    {
      String terror = 0;
      if (!complain && EvaluateText(title, "Error evaluating title", &title_evaluated, &terror) && title_evaluated)
      {
        String save = error;
        int len = strlen(title_evaluated)+2+strlen(save);
        error = XtMalloc(len+1);
        strcpy(error,title_evaluated);
        strcat(error,"\n");
        strcat(error,save);
        error[len]=0;
      }
      else if (terror)
        XtFree(terror);
      if (title_evaluated)
        XtFree(title_evaluated);
      Complain(info, complain, error);
      if (error)
        XtFree(error);
      Unbusy();
      return 0;
    }
    if (event & !update)
    {
      Unbusy();
      return 1;
    }
    if (!EvaluateText(title, "Error evaluating title", &title_evaluated, &error))
    {
      if (complain)
	PopupComplaint(DataSetupWidget, error);
      if (error)
        XtFree(error);
    }
    if (info->print_title_evaluated)
    {
      XtFree(info->print_title_evaluated);
      info->print_title_evaluated = 0;
    }
    if (!EvaluateText(print_title, "Error evaluating print title", &info->print_title_evaluated, &error))
    {
      if (complain)
	PopupComplaint(DataSetupWidget, error);
      if (error)
        XtFree(error);
    }
    GetNewLimits(info, &xmin, &xmax, &ymin, &ymax);
    if (new_grid)
      XtVaSetValues(info->w, XmdsNdisabled, True, NULL);
    XmdsWaveformUpdate(info->w, &x_wave, &y_wave, title_evaluated, xmin, xmax, ymin, ymax, new_grid);
    XtFree(title_evaluated);
  }
  else if (changed(xmin) || changed(ymin) || changed(xmax) || changed(ymax))
  {
    float    *xmin;
    float    *xmax;
    float    *ymin;
    float    *ymax;
    GetNewLimits(info, &xmin, &xmax, &ymin, &ymax);
    XtVaSetValues(info->w, XmdsNxMin, xmin, XmdsNxMax, xmax, XmdsNyMin, ymin, XmdsNyMax, ymax, NULL);
  }
  if (new_grid)
  {
    int       x_grid_lines = info->_global.global.x_grid_lines ? GlobalWave.x_grid_lines : info->x_grid_lines;
    int       y_grid_lines = info->_global.global.y_grid_lines ? GlobalWave.y_grid_lines : info->y_grid_lines;
    Boolean   x_grid_labels = info->_global.global.x_grid_labels ? GlobalWave.x_grid_labels : info->x_grid_labels;
    Boolean   y_grid_labels = info->_global.global.y_grid_labels ? GlobalWave.y_grid_labels : info->y_grid_labels;
    char      show_mode = info->_global.global.show_mode ? GlobalWave.show_mode : info->show_mode;
    Boolean   step_plot = info->_global.global.step_plot ? GlobalWave.step_plot : info->step_plot;
    XtVaSetValues(info->w, XmdsNxGridLines, x_grid_lines, XmdsNyGridLines, y_grid_lines, XmdsNxLabels, x_grid_labels,
		    XmdsNyLabels, y_grid_labels, XmdsNshowMode, show_mode, XmdsNstepPlot, step_plot, 
                    XmdsNdisabled, False, NULL);
  }
  Unbusy();
  return 1;
}

static void  Complain(WaveInfo *info, char mode, String error)
{
  switch (mode)
  {
    case 0:
    XtVaSetValues(info->w, XmdsNcount, 0, XmdsNtitle, error, NULL);
    break;
    case 1:
    {
      Widget    w = XtNameToWidget(DataSetupWidget, "*data_setup_error");
      XmString  error_string = XmStringCreateLtoR(error, XmSTRING_DEFAULT_CHARSET);
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
  XtFree(error);
}

static void  ClearWaveform(WaveInfo *info)
{
  ResetWave(info);
  SetupEvent("", &info->received, &info->eventid);
  if (info->w)
    XtVaSetValues(info->w, XmdsNxGridLines, info->x_grid_lines, XmdsNyGridLines, info->y_grid_lines, XmdsNxLabels,
		info->x_grid_labels, XmdsNyLabels, info->y_grid_labels, XmdsNshowMode, info->show_mode, XmdsNstepPlot,
		info->step_plot, XmdsNcount, 0, XmdsNtitle, "", NULL);
}

static void  RestoreDatabase(String dbname)
{
  int       c;
  int       r;
  int       x;
  int       y;
  unsigned int width;
  unsigned int height;
  int      num;
  Widget   *widgets;
  String   override_shot;
  XrmDatabase scopedb;
  scopedb = MdsGetFileDatabase(dbname);
  XtVaGetValues(PlotsWidget,XmNchildren,&widgets,XmNnumChildren,&num,NULL);
  XtUnmanageChildren(widgets, num);
  XParseGeometry(GetResource(scopedb, "Scope.geometry", "600x500+200+200"), &x, &y, &width, &height);
  SetFont(GetResource(scopedb,"Scope.font","-*-NEW CENTURY SCHOOLBOOK-MEDIUM-R-*--*-120-*-*-*-*-ISO8859-1"));
  ReplaceString(&ScopeTitle, GetResource(scopedb, "Scope.title", "\"MDS Scope\""), 0);
  ReplaceString(&ScopeIcon, GetResource(scopedb, "Scope.icon_name", "\"Scope\""), 0);
  ReplaceString(&ScopeTitleEvent, GetResource(scopedb, "Scope.title_event", ""), 0);
  ReplaceString(&ScopePrintEvent, GetResource(scopedb, "Scope.print_event", ""), 0);
  override_shot = GetResource(scopedb,"Scope.override_shot",NULL);
  if (override_shot)
    XmTextFieldSetString(XtNameToWidget(TopWidget,"*override_shot"),override_shot);
#ifdef __VMS
  ReplaceString(&ScopePrintFile, GetResource(scopedb, "Scope.print_file", "sys$login:dwscope.ps"), 0);
#else
  ReplaceString(&ScopePrintFile, GetResource(scopedb, "Scope.print_file", "dwscope.ps"), 0);
#endif
  ScopePrintWindowTitle = atoi(GetResource(scopedb, "Scope.print_window_title", "0"));
  ScopePrintToFile = atoi(GetResource(scopedb, "Scope.print_to_file", "0"));
  GetWaveFromDb(scopedb, "Scope.global", 0, 0, &GlobalWave);
  SetWindowTitles();
  SetupEvent(strlen(ScopeTitleEvent) ? ScopeTitleEvent : GlobalWave.event, &ScopeTitleEventReceived, &ScopeTitleEventId);
  SetupEvent(ScopePrintEvent, &ScopePrintEventReceived, &ScopePrintEventId);
  XtVaSetValues(TopWidget, XtNx, x, XtNy, y, NULL);
  if (XtWidth(MainWidget) != width || XtHeight(MainWidget) != height)
  {
    XtVaSetValues(MainWidget, XtNwidth, width, XtNheight, height, NULL);
    XtVaSetValues(TopWidget, XtNwidth, width, XtNheight, height, NULL);
    PreventResize = TRUE;
  }
  MenusHeight = 66;
  Columns = min(max(atoi(GetResource(scopedb, "Scope.columns", "1")), 1), MaxCols);
  for (c = 0; c < MaxCols; c++)
  {
    char resource[50];
    sprintf(resource,"Scope.rows_in_column_%d",c+1);
    resource[strlen(resource) - 1] = '1' + c;
    Rows[c] = c < Columns ? min(max(atoi(GetResource(scopedb, resource, "1")), 1), MaxRows) : 0;
    if (Rows[c])
      XtVaSetValues(Pane[c], XtNheight, height, NULL);
    for (r = 0; r < MaxRows; r++)
    {
      WaveInfo *info = &Wave[c][r];
      Dimension pheight;
      if (r < Rows[c])
      {
        float zoom[4];
        int old_update;
	GetWaveFromDb(scopedb, "Scope.plot", r, c, info);
        old_update = info->update;
	pheight = info->height;
	pheight = pheight > 5 && pheight < height ? pheight : max(1, height - MenusHeight - 2 * Rows[c]) / Rows[c];
        info->update = 1;
	UpdateWaveform(0, info, 0, -1, -1);
        info->update = old_update;
        info->height = pheight;
	XtVaSetValues(info->w, XtNheight, pheight, NULL);
        if (GetWaveZoomFromDb(scopedb,"Scope.plot",r,c,zoom))
          XtVaSetValues(info->w, XmdsNxMin, &zoom[0], XmdsNxMax, &zoom[1], XmdsNyMin, &zoom[2], XmdsNyMax, &zoom[3], NULL);
	SetupEvent(info->_global.global.event ? GlobalWave.event : info->event, &info->received, &info->eventid);
      }
      else
	ClearWaveform(info);
    }
  }
  for (c = 0; c < Columns - 1; c++)
  {
    static char      resource[] = "Scope.vpane_n";
    int       position;
    resource[12] = '1' + c;
    position = atoi(GetResource(scopedb, resource, "-1"));
    if (position <= 0 || position >= 1000)
      position = (c + 1) * 1000 / Columns;
    XtVaSetValues(Sash[c], XmNleftPosition, position, NULL);
  }
  ManageWaveforms();
  if (!XtIsRealized(TopWidget))
  {
    XtRealizeWidget(TopWidget);
    RaiseWindows();
    PreventResize = FALSE;
  }
  XrmDestroyDatabase(scopedb);
}

static void  WriteDatabase(String dbname, Boolean zoom)
{
  int       r;
  int       c;
  Position  x;
  Position  y;
  Dimension width;
  Dimension height;
  int       ctx = 0;
  unsigned long fontprop;
  String    text;
  String    filename = XtNewString(dbname);
  String    font;
  int       i;
  FILE      *file;
  XFontStruct *font_struct;
  for (i = 0; i < strlen(filename); i++)
    if (filename[i] == ';')
      filename[i] = 0;
  file = fopen(filename, "w");
  XtFree(filename);
  if (file)
  {
    XtVaGetValues(TopWidget, XtNx, &x, XtNy, &y, NULL);
    XtVaGetValues(MainWidget, XtNwidth, &width, XtNheight, &height, NULL);
    fprintf(file, "Scope.geometry: %dx%d+%d+%d\n", width, height, x, y);
    fprintf(file, "Scope.title: %s\n", ScopeTitle);
    fprintf(file, "Scope.icon_name: %s\n", ScopeIcon);
    fprintf(file, "Scope.title_event: %s\n", ScopeTitleEvent);
    fprintf(file, "Scope.print_file: %s\n", ScopePrintFile);
    fprintf(file, "Scope.print_event: %s\n", ScopePrintEvent);
    fprintf(file, "Scope.print_window_title: %d\n", ScopePrintWindowTitle);
    fprintf(file, "Scope.print_to_file: %d\n", ScopePrintToFile);
    if (zoom) fprintf(file,"Scope.override_shot: %s\n",XmTextFieldGetString(XtNameToWidget(TopWidget,"*override_shot")));
    XtVaGetValues(Wave[0][0].w, XmdsNlabelFont, &font_struct, NULL);
    XGetFontProperty(font_struct,XA_FONT,&fontprop);
    font = XGetAtomName(XtDisplay(TopWidget), fontprop);
    fprintf(file, "Scope.font: %s\n",font);
    XtFree(font);
    fprintf(file, "Scope.columns: %d\n", Columns);
    while (text = WaveToText("Scope.global_1_1", &GlobalWave, 0, &ctx))
    {
      fprintf(file, "%s", text);
      XtFree(text);
    }
    for (c = 0; c < Columns; c++)
    {
      fprintf(file, "Scope.rows_in_column_%d: %d\n", c + 1, Rows[c]);
      for (r = 0; r < Rows[c]; r++)
      {
	char      prefix[36];
	int       ctx = zoom ? -4 : 0;
	int       height;
	XtVaGetValues(Wave[c][r].w, XtNheight, &height, NULL);
        fprintf(file, "\n");
	sprintf(prefix, "Scope.plot_%d_%d", r + 1, c + 1);
	while (text = WaveToText(prefix, &Wave[c][r], height, &ctx))
	{
	  fprintf(file, "%s", text);
	  XtFree(text);
	}
      }
    }
    for (c = 0; c < Columns - 1; c++)
    {
      int       position;
      XtVaGetValues(Sash[c], XmNleftPosition, &position, NULL);
      fprintf(file, "Scope.vpane_%d: %d\n", c + 1, position);
    }
    fclose(file);
  }
  else
    PopupComplaint(MainWidget, "Error writing setup file");
}

static void /*XtActionProc*/Resize(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
  int       c;
  int       r;
  double orig_height = XtHeight(Pane[0]);
  double new_height;
  if (!PreventResize)
  {
    for (c = 0; c < Columns; c++)
      for (r = 0; r < Rows[c]; r++)
        Wave[c][r].height = XtHeight(Wave[c][r].w);
  }
  XtVaSetValues(w, XmNoverrideRedirect, 1, XmNwidth, event->xresizerequest.width, XmNheight, 
         event->xresizerequest.height, NULL);
  {
    new_height = XtHeight(Pane[0]);
    if (PreventResize)
    {
      orig_height = new_height;
      PreventResize = FALSE;
    }
    for (c = 0; c < Columns; c++)
    {
      XtUnmanageChild(Pane[c]);
      for (r = 0; r < Rows[c]; r++)
        XtVaSetValues(Wave[c][r].w, XtNheight, (int) (new_height/orig_height * Wave[c][r].height + (r % 2) * .49999), NULL);
      XtManageChild(Pane[c]);
    }
  }
  XtVaSetValues(w, XmNoverrideRedirect, 0, NULL);
  return;
}

static void /*XtActionProc*/EqualPanes(Widget w, XEvent *event, String *string, Cardinal *num_strings)
{
  int       c;
  int       r;
  if ((*num_strings == 1) && (string[0][0] == 'V'))
  {
    for (c = 0; c < Columns - 1; c++)
    {
      int       position = (c + 1) * 1000 / Columns;
      XtVaSetValues(Sash[c], XmNleftPosition, position, NULL);
    }
  }
  else if ((*num_strings == 1) && (string[0][0] == 'H'))
  {
    Widget    p = XtParent(w);
    int       height;
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

static void  GetNewLimits(WaveInfo *info, float **xmin, float **xmax, float **ymin, float **ymax)
{
  if (info->update)
  {
    static float xminval;
    static float xmaxval;
    static float yminval;
    static float ymaxval;
    String    xmin_s = info->_global.global.xmin ? GlobalWave.xmin : info->xmin;
    String    xmax_s = info->_global.global.xmax ? GlobalWave.xmax : info->xmax;
    String    ymin_s = info->_global.global.ymin ? GlobalWave.ymin : info->ymin;
    String    ymax_s = info->_global.global.ymax ? GlobalWave.ymax : info->ymax;
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
  }
  else
  {
    *xmin = (float *)-1;
    *xmax = (float *)-1;
    *ymin = (float *)-1;
    *ymax = (float *)-1;
  }
}

static void  SetWindowTitles()
{
  String    title;
  String    title_error;
  XmdsWaveformValStruct x_wave;
  XmdsWaveformValStruct y_wave;
  String    error;
  int       evalstat = EvaluateData(1, 0, 0, 0, 0, GlobalWave.database, GlobalShot(), "", "", "", &x_wave, &y_wave, &error);
  Boolean   title_status = EvaluateText(ScopeTitle, " ", &title, &title_error);
  String    icon;
  String    icon_error;
  Boolean   icon_status = EvaluateText(ScopeIcon, " ", &icon, &icon_error);
  XtVaSetValues(TopWidget, XtNtitle, title_status ? title : ScopeTitle, XtNiconName, icon_status ? icon : ScopeIcon, NULL);
  if (!evalstat)
    XtFree(error);
  if (!title_status)
    XtFree(title_error);
  if (!icon_status)
    XtFree(icon_error);
  XtFree(title);
  XtFree(icon);
}
