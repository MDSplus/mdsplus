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
/*  CMS REPLACEMENT HISTORY, Element DWSCOPESUBS.C */
/*  *54   27-AUG-1997 15:38:13 TWF "Fix it" */
/*  *53   27-AUG-1997 11:55:15 TWF "Make error message multiline" */
/*  *52   27-AUG-1997 11:53:11 TWF "Make error message multiline" */
/*  *51   29-JAN-1997 13:37:19 TWF "Add max/min settings" */
/*  *50   29-JAN-1997 13:29:33 TWF "Add max/min settings" */
/*  *49   29-MAR-1995 08:42:39 TWF "Remove expand changes" */
/*  *48   29-MAR-1995 08:36:31 TWF "Debug expand problem" */
/*  *47   16-FEB-1995 11:24:08 TWF "Put in another Motif bug work aroung for restore" */
/*  *46    9-FEB-1995 15:12:36 TWF "Fix getresource" */
/*  *45    9-FEB-1995 15:03:55 TWF "Fix compiler bug" */
/*  *44   19-JAN-1995 11:55:28 TWF "Portability" */
/*  *43   19-JAN-1995 11:52:11 TWF "Portability" */
/*  *42   19-JAN-1995 11:46:29 TWF "Portability" */
/*  *41    5-JAN-1995 10:28:16 TWF "test for flip bits" */
/*  *40    4-JAN-1995 13:45:54 TWF "" */
/*  *39   22-DEC-1994 13:45:22 TWF "Flip bits" */
/*  *38   22-DEC-1994 12:47:38 TWF "Need to flip bits on some architectures" */
/*  *37   22-DEC-1994 12:42:25 TWF "Need to flip bits on some architectures" */
/*  *36    9-NOV-1994 15:36:57 TWF "port to unix" */
/*  *35    3-OCT-1994 15:38:18 TWF "Unix" */
/*  *34   30-MAR-1994 10:07:15 TWF "Fix for CMODA" */
/*  *33   23-MAR-1994 17:11:38 TWF "Take out no x gbls" */
/*  *32    9-JUL-1993 16:22:32 TWF "Fix compile errors" */
/*  *31    1-MAY-1993 11:09:51 JAS "change the way atoms are delt with" */
/*  *30   22-MAR-1993 16:11:33 TWF "Fix save" */
/*  *29   22-MAR-1993 15:50:02 TWF "Try byte transfer" */
/*  *28   19-MAR-1993 13:35:11 TWF "Fix memory problem" */
/*  *27   19-MAR-1993 13:00:15 TWF "Fix memory problem" */
/*  *26   18-MAR-1993 19:33:25 TWF "free mistake" */
/*  *25   18-MAR-1993 18:45:13 TWF "Add paste data" */
/*  *24   18-MAR-1993 18:30:33 TWF "Add data paste" */
/*  *23   18-MAR-1993 17:36:32 TWF "Add paste" */
/*  *22   18-MAR-1993 17:01:41 TWF "Add paste" */
/*  *21   18-MAR-1993 17:00:31 TWF "Remove dps" */
/*  *20   18-MAR-1993 16:57:17 TWF "Add dps" */
/*  *19   18-MAR-1993 13:44:51 TWF "Fix all_text" */
/*  *18   18-MAR-1993 13:43:18 TWF "Fix declaration" */
/*  *17   18-MAR-1993 13:41:39 TWF "Fix all_text" */
/*  *16   18-MAR-1993 13:38:28 TWF "Fix XDPS" */
/*  *15   18-MAR-1993 13:28:47 TWF "Add data cut" */
/*  *14   24-FEB-1993 17:51:35 TWF "Use DECC" */
/*  *13   18-NOV-1992 15:13:41 TWF "Add expand button to data setup" */
/*  *12    4-AUG-1992 16:07:29 JAS "Convert needs to clear global.y bit" */
/*  *11    4-AUG-1992 15:12:30 JAS "change paste to check available types and accept strings if nesss." */
/*  *10   11-DEC-1991 13:56:50 TWF "fix compile error" */
/*  *9    11-DEC-1991 13:06:20 TWF "Make save current only print values that are not default" */
/*  *8    10-DEC-1991 18:09:14 TWF "Fix pad label change" */
/*  *7     4-DEC-1991 11:29:59 TWF "Update data only when needed" */
/*  *6    25-NOV-1991 12:23:24 TWF "Fix load data setup" */
/*  *5    25-NOV-1991 12:01:29 TWF "Add show mode" */
/*  *4    25-NOV-1991 11:58:09 TWF "Add show mode" */
/*  *3    25-NOV-1991 11:55:57 TWF "Add show mode" */
/*  *2    25-NOV-1991 10:32:11 TWF "add show mode" */
/*  *1    12-NOV-1991 14:33:00 TWF "Routines used by DWPAD and DWSCOPE" */
/*  CMS REPLACEMENT HISTORY, Element DWSCOPESUBS.C */
#include <stdio.h>
#include <stdlib.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/MessageB.h>
#include <Xm/Text.h>
#include <Xm/Scale.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/RowColumnP.h>
#include <Xmds/XmdsWaveform.h>
#include "dwscope.h"
#include <mdsplus/mdsplus.h>

extern int XmdsManageWindow();
Boolean ConvertSelectionToWave(Widget w, Atom result_type, unsigned long length, CutHeader * header,
			       WaveInfo * info);
Boolean ConvertWaveToSelection(Widget w, String prefix, WaveInfo * wave, Atom target, Atom * type,
			       String * header, unsigned long *length, int *format);
Boolean ReplaceString(String * old, String new, Boolean free);
String GetResource(XrmDatabase db, String resource, String default_answer);
String WaveToText(String prefix, WaveInfo * wave, Dimension height, int *ctx);
void LoadDataSetup(Widget w, String title, WaveInfo * info);
void DisableGlobalDefault(Widget w, String tag, XtPointer callback_data);
void GetDataSetup(Widget w, WaveInfo * info, int *change_mask);
void GetWaveFromDb(XrmDatabase db, String prefix, int row, int col, WaveInfo * info);
void InitDefaultsSetupWidget(Widget w, int *tag, XtPointer callback_data);
void ExpandReset(Widget w, int *tag, XtPointer callback_data);
void ExpandCancel(Widget w, int *tag, XtPointer callback_data);
void ExpandOk(Widget w, int *tag, XtPointer callback_data);
void ResetWave(WaveInfo * info);
void SetDirMask(Widget w, String * file, XmAnyCallbackStruct * callback_data);
void DisplayHelp(Widget w_in, String tag, XtPointer callback_data);
XrmDatabase GetFileDatabase(String file_spec);

static void SetOptionIdx(Widget w, int idx);
static int GetOptionIdx(Widget w);
static void GetGlobalDefaults(Widget w, WaveInfo * info);
static String GetPlotResource(XrmDatabase db, String prefix, int row, int column,
			      String plot_resource, String default_answer);
static String ReplaceCountedString(String * old, short length, String new);
static void LoadGlobalDefaults(Widget w, WaveInfo * info);
static String Concat(String prefix, String postfix);
static String FixupLFS(String in);
static String FixupBARS(String in);

#define XA_DWSCOPE_PANEL XInternAtom(XtDisplay(w), "DWSCOPE_PANEL", 0)
#define XA_TARGETS  XInternAtom(XtDisplay(w), "TARGETS", 0)
#define XA_X_AXIS  XInternAtom(XtDisplay(w), "DWSCOPE_X_AXIS", 0)
#define XA_Y_AXIS  XInternAtom(XtDisplay(w), "DWSCOPE_Y_AXIS", 0)

static int FlipBitsIfNecessary(int in)
{
  static union {
    int i;
    unsigned bit:1;
  } test = {
  1};
  if (test.bit)
    return in;
  else {
    int out = 0;
    int i;
    for (i = 0; i < 32; i++)
      out |= ((in & (1 << i)) != 0) << (31 - i);
    return out;
  }
}

Boolean ConvertSelectionToWave(Widget w, Atom type, unsigned long length, CutHeader * header,
			       WaveInfo * wave)
{
  int status = 0;
  if (type == XA_DWSCOPE_PANEL) {
    String text = (String) (header + 1);
    if ((length >= sizeof(CutHeader)) &&
	(length == (sizeof(CutHeader) + header->database_length + header->shot_length +
		    header->default_node_length + header->x_length + header->y_length +
		    header->xmin_length + header->xmax_length + header->ymin_length +
		    header->ymax_length + header->title_length + header->event_length +
		    header->print_title_length + header->pad_label_length))) {
      wave->update = header->update;
      wave->x_grid_labels = header->x_grid_labels;
      wave->y_grid_labels = header->y_grid_labels;
      wave->show_mode = header->show_mode;
      wave->step_plot = header->step_plot;
      wave->x_grid_lines = header->x_grid_lines;
      wave->y_grid_lines = header->y_grid_lines;
      wave->_global.global_defaults = header->global_defaults;
      text = ReplaceCountedString(&wave->database, header->database_length, text);
      text = ReplaceCountedString(&wave->shot, header->shot_length, text);
      text = ReplaceCountedString(&wave->default_node, header->default_node_length, text);
      text = ReplaceCountedString(&wave->x, header->x_length, text);
      text = ReplaceCountedString(&wave->y, header->y_length, text);
      text = ReplaceCountedString(&wave->xmin, header->xmin_length, text);
      text = ReplaceCountedString(&wave->xmax, header->xmax_length, text);
      text = ReplaceCountedString(&wave->ymin, header->ymin_length, text);
      text = ReplaceCountedString(&wave->ymax, header->ymax_length, text);
      text = ReplaceCountedString(&wave->title, header->title_length, text);
      text = ReplaceCountedString(&wave->event, header->event_length, text);
      text = ReplaceCountedString(&wave->print_title, header->print_title_length, text);
      text = ReplaceCountedString(&wave->pad_label, header->pad_label_length, text);
    }
    status = 1;
  } else if (type == XA_STRING) {
    String text = (String) header;
    text = ReplaceCountedString(&wave->y, strlen(text), text);
    wave->_global.global.y = 0;
    if (strlen(wave->pad_label) == 0) {
      int i;
      for (i = strlen(wave->y) - 1; i; i--)
	if (wave->y[i] == '.' || wave->y[i] == ':')
	  break;
      if (i == 0)
	ReplaceCountedString(&wave->pad_label, strlen(wave->y), wave->y);
      else
	ReplaceCountedString(&wave->pad_label, strlen(wave->y) - i - 1, &wave->y[i + 1]);
    }
    status = 1;
  }
  return status;
}

static String ReplaceCountedString(String * old, short length, String new)
{
  if (*old)
    XtFree(*old);
  *old = strncpy(XtMalloc(length + 1), new, length);
  (*old)[length] = 0;
  return new + length;
}

String WaveToText(String prefix, WaveInfo * wave, Dimension height, int *ctx)
{
  String answer = 0;
  int prefix_length = strlen(prefix);
  Boolean done = 0;
  float *fptr;
  while (answer == 0 && !done) {
    switch ((*ctx)++) {
    case -4:
      XtVaGetValues(wave->w, XmdsNxMin, &fptr, NULL);
      if (fptr != NULL) {
	answer = XtMalloc(prefix_length + sizeof(".xmin_zoom: \n") + 12);
	sprintf(answer, "%s.%s: %g\n", prefix, "xmin_zoom", *fptr);
      }
      break;
    case -3:
      XtVaGetValues(wave->w, XmdsNxMax, &fptr, NULL);
      if (fptr != NULL) {
	answer = XtMalloc(prefix_length + sizeof(".xmax_zoom: \n") + 12);
	sprintf(answer, "%s.%s: %g\n", prefix, "xmax_zoom", *fptr);
      }
      break;
    case -2:
      XtVaGetValues(wave->w, XmdsNyMin, &fptr, NULL);
      if (fptr != NULL) {
	answer = XtMalloc(prefix_length + sizeof(".ymin_zoom: \n") + 12);
	sprintf(answer, "%s.%s: %g\n", prefix, "ymin_zoom", *fptr);
      }
      break;
    case -1:
      XtVaGetValues(wave->w, XmdsNyMax, &fptr, NULL);
      if (fptr != NULL) {
	answer = XtMalloc(prefix_length + sizeof(".ymax_zoom: \n") + 12);
	sprintf(answer, "%s.%s: %g\n", prefix, "ymax_zoom", *fptr);
      }
      break;
    case 0:
      if (height) {
	answer = XtMalloc(prefix_length + sizeof(".height: \n") + 16);
	sprintf(answer, "%s.%s: %d\n", prefix, "height", height);
      }
      MDS_ATTR_FALLTHROUGH
    case 1:
      if (!wave->update) {
	answer = XtMalloc(prefix_length + sizeof(".update: 0\n") + 1);
	strcpy(answer, prefix);
	strcat(answer, ".update: 0\n");
      }
      break;
    case 2:
      answer = wave->x_grid_labels ? 0 : Concat(prefix, ".x.grid_labels: 0\n");
      break;
    case 3:
      answer = wave->y_grid_labels ? 0 : Concat(prefix, ".y.grid_labels: 0\n");
      break;
    case 4:
      answer = wave->show_mode ?
	  (wave->show_mode ==
	   1 ? Concat(prefix, ".show_mode: 1\n") : Concat(prefix, ".show_mode: 2\n")) : 0;
      break;
    case 5:
      answer = wave->step_plot ? Concat(prefix, ".step_plot: 1\n") : 0;
      break;
    case 6:
      if (wave->x_grid_lines != 4) {
	answer = XtMalloc(prefix_length + sizeof(".x.grid_lines: \n") + 16);
	sprintf(answer, "%s.%s: %d\n", prefix, "x.grid_lines", wave->x_grid_lines);
      }
      break;
    case 7:
      if (wave->y_grid_lines != 4) {
	answer = XtMalloc(prefix_length + sizeof(".y.grid_lines: \n") + 16);
	sprintf(answer, "%s.%s: %d\n", prefix, "y.grid_lines", wave->y_grid_lines);
      }
      break;
    case 8:
      if (strlen(wave->database)) {
	answer = XtMalloc(prefix_length + sizeof(".experiment: \n") + strlen(wave->database) + 1);
	sprintf(answer, "%s.%s: %s\n", prefix, "experiment", wave->database);
      }
      break;
    case 9:
      if (strlen(wave->shot)) {
	answer = XtMalloc(prefix_length + sizeof(".shot: \n") + strlen(wave->shot) + 1);
	sprintf(answer, "%s.%s: %s\n", prefix, "shot", wave->shot);
      }
      break;
    case 10:
      if (strlen(wave->default_node)) {
	answer =
	    XtMalloc(prefix_length + sizeof(".default_node: \n") + strlen(wave->default_node) + 1);
	sprintf(answer, "%s.%s: %s\n", prefix, "default_node", wave->default_node);
      }
      break;
    case 11:
      if (strlen(wave->x)) {
	String out = FixupLFS(wave->x);
	answer = XtMalloc(prefix_length + sizeof(".x: \n") + strlen(out) + 1);
	sprintf(answer, "%s.%s: %s\n", prefix, "x", out);
	XtFree(out);
      }
      break;
    case 12:
      if (strlen(wave->y)) {
	String out = FixupLFS(wave->y);
	answer = XtMalloc(prefix_length + sizeof(".y: \n") + strlen(out) + 1);
	sprintf(answer, "%s.%s: %s\n", prefix, "y", out);
	XtFree(out);
      }
      break;
    case 13:
      if (strlen(wave->xmin)) {
	answer = XtMalloc(prefix_length + sizeof(".xmin: \n") + strlen(wave->xmin) + 1);
	sprintf(answer, "%s.%s: %s\n", prefix, "xmin", wave->xmin);
      }
      break;
    case 14:
      if (strlen(wave->xmax)) {
	answer = XtMalloc(prefix_length + sizeof(".xmax: \n") + strlen(wave->xmax) + 1);
	sprintf(answer, "%s.%s: %s\n", prefix, "xmax", wave->xmax);
      }
      break;
    case 15:
      if (strlen(wave->ymin)) {
	answer = XtMalloc(prefix_length + sizeof(".ymin: \n") + strlen(wave->ymin) + 1);
	sprintf(answer, "%s.%s: %s\n", prefix, "ymin", wave->ymin);
      }
      break;
    case 16:
      if (strlen(wave->ymax)) {
	answer = XtMalloc(prefix_length + sizeof(".ymax: \n") + strlen(wave->ymax) + 1);
	sprintf(answer, "%s.%s: %s\n", prefix, "ymax", wave->ymax);
      }
      break;
    case 17:
      if (strlen(wave->event)) {
	answer = XtMalloc(prefix_length + sizeof(".event: \n") + strlen(wave->event) + 1);
	sprintf(answer, "%s.%s: %s\n", prefix, "event", wave->event);
      }
      break;
    case 18:
      if (strlen(wave->title)) {
	answer = XtMalloc(prefix_length + sizeof(".title: \n") + strlen(wave->title) + 1);
	sprintf(answer, "%s.%s: %s\n", prefix, "title", wave->title);
      }
      break;
    case 19:
      if (strlen(wave->print_title)) {
	answer =
	    XtMalloc(prefix_length + sizeof(".print_title: \n") + strlen(wave->print_title) + 1);
	sprintf(answer, "%s.%s: %s\n", prefix, "print_title", wave->print_title);
      }
      break;
    case 20:
      if (strlen(wave->pad_label)) {
	answer = XtMalloc(prefix_length + sizeof(".label: \n") + strlen(wave->pad_label) + 1);
	sprintf(answer, "%s.%s: %s\n", prefix, "label", wave->pad_label);
      }
      break;
    case 21:
      if (wave->_global.global_defaults != (strlen(wave->y) ? 0 : -1)) {
	answer = XtMalloc(prefix_length + sizeof(".global_defaults: \n") + 16);
	sprintf(answer, "%s.%s: %d\n", prefix, "global_defaults",
		FlipBitsIfNecessary(wave->_global.global_defaults));
      }
      break;
    default:
      answer = 0;
      *ctx = 0;
      done = 1;
      break;
    }
  }
  return answer;
}

Boolean ConvertWaveToSelection(Widget w, String prefix, WaveInfo * wave, Atom target, Atom * type,
			       String * value, unsigned long *length, int *format)
{
  int status = 0;
  if (target == XA_DWSCOPE_PANEL) {
    CutHeader *header;
    String text;
    *type = target;
    *format = 8;
    *length =
	sizeof(CutHeader) + strlen(wave->database) + strlen(wave->shot) +
	strlen(wave->default_node) + strlen(wave->x) + strlen(wave->y) + strlen(wave->xmin) +
	strlen(wave->xmax) + strlen(wave->ymin) + strlen(wave->ymax) + strlen(wave->title) +
	strlen(wave->event) + strlen(wave->print_title) + strlen(wave->pad_label);
    header = (CutHeader *) (*value = (String) XtMalloc(*length));
    header->update = wave->update;
    header->x_grid_labels = wave->x_grid_labels;
    header->y_grid_labels = wave->y_grid_labels;
    header->show_mode = wave->show_mode;
    header->step_plot = wave->step_plot;
    header->x_grid_lines = wave->x_grid_lines;
    header->y_grid_lines = wave->y_grid_lines;
    header->global_defaults = wave->_global.global_defaults;
    header->database_length = strlen(wave->database);
    header->shot_length = strlen(wave->shot);
    header->default_node_length = strlen(wave->default_node);
    header->x_length = strlen(wave->x);
    header->y_length = strlen(wave->y);
    header->xmin_length = strlen(wave->xmin);
    header->xmax_length = strlen(wave->xmax);
    header->ymin_length = strlen(wave->ymin);
    header->ymax_length = strlen(wave->ymax);
    header->title_length = strlen(wave->title);
    header->event_length = strlen(wave->event);
    header->print_title_length = strlen(wave->print_title);
    header->pad_label_length = strlen(wave->pad_label);
    text = (String) (header + 1);
    memcpy(text, wave->database, strlen(wave->database));
    text += strlen(wave->database);
    memcpy(text, wave->shot, strlen(wave->shot));
    text += strlen(wave->shot);
    memcpy(text, wave->default_node, strlen(wave->default_node));
    text += strlen(wave->default_node);
    memcpy(text, wave->x, strlen(wave->x));
    text += strlen(wave->x);
    memcpy(text, wave->y, strlen(wave->y));
    text += strlen(wave->y);
    memcpy(text, wave->xmin, strlen(wave->xmin));
    text += strlen(wave->xmin);
    memcpy(text, wave->xmax, strlen(wave->xmax));
    text += strlen(wave->xmax);
    memcpy(text, wave->ymin, strlen(wave->ymin));
    text += strlen(wave->ymin);
    memcpy(text, wave->ymax, strlen(wave->ymax));
    text += strlen(wave->ymax);
    memcpy(text, wave->title, strlen(wave->title));
    text += strlen(wave->title);
    memcpy(text, wave->event, strlen(wave->event));
    text += strlen(wave->event);
    memcpy(text, wave->print_title, strlen(wave->print_title));
    text += strlen(wave->print_title);
    memcpy(text, wave->pad_label, strlen(wave->pad_label));
    text += strlen(wave->pad_label);
    status = 1;
  } else if (target == XA_STRING) {
    int ctx = 0;
    String text;
    String all_text = WaveToText(prefix, wave, 0, &ctx);
    while (all_text && (text = WaveToText(prefix, wave, 0, &ctx))) {
      all_text = XtRealloc(all_text, strlen(all_text) + strlen(text) + 1);
      strcat(all_text, text);
      XtFree(text);
    }
    if (all_text && strlen(all_text)) {
      *type = target;
      *value = (String) all_text;
      *length = strlen(all_text);
      *format = 8;
      status = 1;
    }
  } else if (target == XA_X_AXIS) {
    XmdsWaveformValStruct *x;
    int count = 0;
    XtVaGetValues(w, XmdsNxValStruct, &x, XmdsNcount, &count, NULL);
    if (x && count) {
      *format = 8;
      *type = target;
      *value = (String) memcpy(XtMalloc(count * sizeof(float)), x->addr, count * sizeof(float));
      *length = count * 32 / (*format);
      status = 1;
    }
  } else if (target == XA_Y_AXIS) {
    XmdsWaveformValStruct *y;
    int count = 0;
    XtVaGetValues(w, XmdsNyValStruct, &y, XmdsNcount, &count, NULL);
    if (y && count) {
      *format = 8;
      *type = target;
      *value = (String) memcpy(XtMalloc(count * sizeof(float)), y->addr, count * sizeof(float));
      *length = count * 32 / (*format);
      status = 1;
    }
  } else if (target == XA_TARGETS) {
    *type = target;
    *value = (String) XtMalloc(sizeof(Atom) * 4);
    ((Atom *) * value)[0] = XA_STRING;
    ((Atom *) * value)[1] = XA_DWSCOPE_PANEL;
    ((Atom *) * value)[2] = XA_X_AXIS;
    ((Atom *) * value)[3] = XA_Y_AXIS;
    *length = sizeof(Atom) / 4 * 4;
    *format = 32;
    status = 1;
  }
  return status;
}

XrmDatabase GetFileDatabase(String file_spec)
{
  FILE *file = fopen(file_spec, "r");
  XrmDatabase db = 0;
  if (file) {
    char line_text[32768];
    char doubled[32768];
    while (fgets(line_text, 32768, file)) {
      int i;
      int j;
      int put_it = 0;
      int size = strlen(line_text);
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
	for (i = 0; i < size; i++) {
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

void LoadDataSetup(Widget w, String title, WaveInfo * info)
{
  Widget top;
  Widget tw;
  XmString title_s = XmStringCreateSimple(title);
  for (top = tw = w; tw; top = tw, tw = XtParent(tw)) ;
  XtVaSetValues(w, XmNdialogTitle, title_s, NULL);
  XmStringFree(title_s);
  XmTextSetString(XtNameToWidget(w, "exp_text"), info->database);
  XmTextSetString(XtNameToWidget(w, "shot_text"), info->shot);
  XmTextSetString(XtNameToWidget(w, "default_text"), info->default_node);
  XmTextSetString(XtNameToWidget(w, "y_expression"), info->y);
  XmTextSetString(XtNameToWidget(w, "x_expression"), info->x);
  XmTextSetString(XtNameToWidget(w, "y_min_text"), info->ymin);
  XmTextSetString(XtNameToWidget(w, "y_max_text"), info->ymax);
  XmTextSetString(XtNameToWidget(w, "x_min_text"), info->xmin);
  XmTextSetString(XtNameToWidget(w, "x_max_text"), info->xmax);
  XmTextSetString(XtNameToWidget(w, "title_text"), info->title);
  XmToggleButtonSetState(XtNameToWidget(w, "update_limits"), (int)info->update, 1);
  XmTextSetString(XtNameToWidget(w, "event_text"), info->event);
  XmTextSetString(XtNameToWidget(w, "print_text"), info->print_title);
  XmTextSetString(XtNameToWidget(w, "padlabel_expression"), info->pad_label);
  XmToggleButtonSetState(XtNameToWidget(w, "x_grid_labels"), info->x_grid_labels, 1);
  XmToggleButtonSetState(XtNameToWidget(w, "y_grid_labels"), info->y_grid_labels, 1);
  SetOptionIdx(XtNameToWidget(w, "show_mode"), info->show_mode);
  XmToggleButtonSetState(XtNameToWidget(w, "step_plot"), info->step_plot, 1);
  XmScaleSetValue(XtNameToWidget(w, "x_grid_lines"), info->x_grid_lines);
  XmScaleSetValue(XtNameToWidget(w, "y_grid_lines"), info->y_grid_lines);
  LoadGlobalDefaults(XtNameToWidget(top, "*defaults_setup_db"), info);
}

static void LoadGlobalDefaults(Widget w, WaveInfo * info)
{
  XmToggleButtonGadgetSetState(XtNameToWidget(w, "def_exp_text"), info->_global.global.database, 1);
  XmToggleButtonGadgetSetState(XtNameToWidget(w, "def_shot_text"), info->_global.global.shot, 1);
  XmToggleButtonGadgetSetState(XtNameToWidget(w, "def_default_text"),
			       info->_global.global.default_node, 1);
  XmToggleButtonGadgetSetState(XtNameToWidget(w, "def_y_expression"), info->_global.global.y, 1);
  XmToggleButtonGadgetSetState(XtNameToWidget(w, "def_x_expression"), info->_global.global.x, 1);
  XmToggleButtonGadgetSetState(XtNameToWidget(w, "def_y_min_text"), info->_global.global.ymin, 1);
  XmToggleButtonGadgetSetState(XtNameToWidget(w, "def_y_max_text"), info->_global.global.ymax, 1);
  XmToggleButtonGadgetSetState(XtNameToWidget(w, "def_x_min_text"), info->_global.global.xmin, 1);
  XmToggleButtonGadgetSetState(XtNameToWidget(w, "def_x_max_text"), info->_global.global.xmax, 1);
  XmToggleButtonGadgetSetState(XtNameToWidget(w, "def_title_text"), info->_global.global.title, 1);
  XmToggleButtonGadgetSetState(XtNameToWidget(w, "def_update_limits"),
			       (int)info->_global.global.update, 1);
  XmToggleButtonGadgetSetState(XtNameToWidget(w, "def_event_text"), info->_global.global.event, 1);
  XmToggleButtonGadgetSetState(XtNameToWidget(w, "def_print_text"),
			       info->_global.global.print_title, 1);
  XmToggleButtonGadgetSetState(XtNameToWidget(w, "def_padlabel_expression"),
			       info->_global.global.pad_label, 1);
  XmToggleButtonGadgetSetState(XtNameToWidget(w, "def_x_grid_labels"),
			       info->_global.global.x_grid_labels, 1);
  XmToggleButtonGadgetSetState(XtNameToWidget(w, "def_y_grid_labels"),
			       info->_global.global.y_grid_labels, 1);
  XmToggleButtonGadgetSetState(XtNameToWidget(w, "def_show_mode"), info->_global.global.show_mode,
			       1);
  XmToggleButtonGadgetSetState(XtNameToWidget(w, "def_step_plot"), info->_global.global.step_plot,
			       1);
  XmToggleButtonGadgetSetState(XtNameToWidget(w, "def_x_grid_lines"),
			       info->_global.global.x_grid_lines, 1);
  XmToggleButtonGadgetSetState(XtNameToWidget(w, "def_y_grid_lines"),
			       info->_global.global.y_grid_lines, 1);
}

String GetResource(XrmDatabase db, String resource, String default_answer)
{
  XrmValue value = { 0, (String) 0 };
  XrmString type;
  if (XrmGetResource(db, resource, 0, &type, &value) && value.size && (*((String) value.addr)))
    return value.addr;
  else
    return default_answer;
}

static String GetPlotResource(XrmDatabase db, String prefix, int row, int column,
			      String plot_resource, String default_answer)
{
  static char resource[256];
  resource[0] = 0;
  sprintf(resource, "%s_%d_%d.%s", prefix, row + 1, column + 1, plot_resource);
  return GetResource(db, resource, default_answer);
}

void GetWaveFromDb(XrmDatabase db, String prefix, int row, int col, WaveInfo * info)
{
  ReplaceString(&info->database, GetPlotResource(db, prefix, row, col, "experiment", ""), 0);
  ReplaceString(&info->shot, GetPlotResource(db, prefix, row, col, "shot", ""), 0);
  ReplaceString(&info->default_node, GetPlotResource(db, prefix, row, col, "default_node", ""), 0);
  ReplaceString(&info->x, FixupBARS(GetPlotResource(db, prefix, row, col, "x", "")), 1);
  ReplaceString(&info->y, FixupBARS(GetPlotResource(db, prefix, row, col, "y", "")), 1);
  ReplaceString(&info->event, GetPlotResource(db, prefix, row, col, "event", ""), 0);
  info->update = atoi(GetPlotResource(db, prefix, row, col, "update", "1"));
  ReplaceString(&info->title, GetPlotResource(db, prefix, row, col, "title", ""), 0);
  ReplaceString(&info->print_title, GetPlotResource(db, prefix, row, col, "print_title", ""), 0);
  ReplaceString(&info->pad_label, GetPlotResource(db, prefix, row, col, "label", ""), 0);
  info->x_grid_labels = atoi(GetPlotResource(db, prefix, row, col, "x.grid_labels", "1"));
  info->y_grid_labels = atoi(GetPlotResource(db, prefix, row, col, "y.grid_labels", "1"));
  info->show_mode = atoi(GetPlotResource(db, prefix, row, col, "show_mode", "0"));
  info->step_plot = atoi(GetPlotResource(db, prefix, row, col, "step_plot", "0"));
  info->x_grid_lines = atoi(GetPlotResource(db, prefix, row, col, "x.grid_lines", "4"));
  info->y_grid_lines = atoi(GetPlotResource(db, prefix, row, col, "y.grid_lines", "4"));
  ReplaceString(&info->xmin, GetPlotResource(db, prefix, row, col, "xmin", ""), 0);
  ReplaceString(&info->xmax, GetPlotResource(db, prefix, row, col, "xmax", ""), 0);
  ReplaceString(&info->ymin, GetPlotResource(db, prefix, row, col, "ymin", ""), 0);
  ReplaceString(&info->ymax, GetPlotResource(db, prefix, row, col, "ymax", ""), 0);
  info->height = atoi(GetPlotResource(db, prefix, row, col, "height", "0"));
  info->_global.global_defaults =
      FlipBitsIfNecessary(atoi
			  (GetPlotResource
			   (db, prefix, row, col, "global_defaults",
			    strlen(info->y) ? "0" : "-1")));
}

Boolean GetWaveZoomFromDb(XrmDatabase db, String prefix, int row, int col, float *zoom)
{
  String limit;
  limit = GetPlotResource(db, prefix, row, col, "xmin_zoom", "");
  if (limit && strlen(limit) > 0)
    zoom[0] = (float)atof(limit);
  else
    return FALSE;
  limit = GetPlotResource(db, prefix, row, col, "xmax_zoom", "");
  if (limit && strlen(limit) > 0)
    zoom[1] = (float)atof(limit);
  else
    return FALSE;
  limit = GetPlotResource(db, prefix, row, col, "ymin_zoom", "");
  if (limit && strlen(limit) > 0)
    zoom[2] = (float)atof(limit);
  else
    return FALSE;
  limit = GetPlotResource(db, prefix, row, col, "ymax_zoom", "");
  if (limit && strlen(limit) > 0)
    zoom[3] = (float)atof(limit);
  else
    return FALSE;
  return TRUE;
}

void GetDataSetup(Widget w, WaveInfo * info, int *change_mask)
{
  int value;
  int mask = 0;
  int global_defaults = info->_global.global_defaults;
  Widget top = w;
  Widget tw;
  for (tw = w; tw; top = tw, tw = XtParent(tw)) ;
  GetGlobalDefaults(XtNameToWidget(top, "*defaults_setup_db"), info);
#define changed(field,test) \
((test) || ((global_defaults & M_##field) != (info->_global.global_defaults & M_##field))) << B_##field
#define changed_string(field,name) changed(field, ReplaceString(&info->field, XmTextGetString(XtNameToWidget(w, name)), 0))
  mask |= changed_string(database, "exp_text");
  mask |= changed_string(shot, "shot_text");
  mask |= changed_string(default_node, "default_text");
  mask |= changed_string(y, "y_expression");
  mask |= changed_string(x, "x_expression");
  mask |= changed_string(ymin, "y_min_text");
  mask |= changed_string(ymax, "y_max_text");
  mask |= changed_string(xmin, "x_min_text");
  mask |= changed_string(xmax, "x_max_text");
  mask |= changed_string(title, "title_text");
  value = XmToggleButtonGetState(XtNameToWidget(w, "update_limits"));
  mask |= changed(update, info->update != value);
  info->update = value;
  mask |= changed_string(event, "event_text");
  mask |= changed_string(print_title, "print_text");
  mask |= changed_string(pad_label, "padlabel_expression");
  value = XmToggleButtonGetState(XtNameToWidget(w, "x_grid_labels"));
  mask |= changed(x_grid_labels, info->x_grid_labels != value);
  info->x_grid_labels = value;
  value = XmToggleButtonGetState(XtNameToWidget(w, "y_grid_labels"));
  mask |= changed(y_grid_labels, info->y_grid_labels != value);
  info->y_grid_labels = value;
  value = GetOptionIdx(XtNameToWidget(w, "show_mode"));
  mask |= changed(show_mode, info->show_mode != value);
  info->show_mode = value;
  value = XmToggleButtonGetState(XtNameToWidget(w, "step_plot"));
  mask |= changed(step_plot, info->step_plot != value);
  info->step_plot = value;
  XmScaleGetValue(XtNameToWidget(w, "x_grid_lines"), &value);
  mask |= changed(x_grid_lines, info->x_grid_lines != value);
  info->x_grid_lines = value;
  XmScaleGetValue(XtNameToWidget(w, "y_grid_lines"), &value);
  mask |= changed(y_grid_lines, info->y_grid_lines != value);
  info->y_grid_lines = value;
  *change_mask = mask;
#undef changed
#undef changed_string
}

static void GetGlobalDefaults(Widget w, WaveInfo * info)
{
  info->_global.global.database = XmToggleButtonGadgetGetState(XtNameToWidget(w, "def_exp_text"));
  info->_global.global.shot = XmToggleButtonGadgetGetState(XtNameToWidget(w, "def_shot_text"));
  info->_global.global.default_node =
      XmToggleButtonGadgetGetState(XtNameToWidget(w, "def_default_text"));
  info->_global.global.y = XmToggleButtonGadgetGetState(XtNameToWidget(w, "def_y_expression"));
  info->_global.global.x = XmToggleButtonGadgetGetState(XtNameToWidget(w, "def_x_expression"));
  info->_global.global.ymin = XmToggleButtonGadgetGetState(XtNameToWidget(w, "def_y_min_text"));
  info->_global.global.ymax = XmToggleButtonGadgetGetState(XtNameToWidget(w, "def_y_max_text"));
  info->_global.global.xmin = XmToggleButtonGadgetGetState(XtNameToWidget(w, "def_x_min_text"));
  info->_global.global.xmax = XmToggleButtonGadgetGetState(XtNameToWidget(w, "def_x_max_text"));
  info->_global.global.title = XmToggleButtonGadgetGetState(XtNameToWidget(w, "def_title_text"));
  info->_global.global.update =
      XmToggleButtonGadgetGetState(XtNameToWidget(w, "def_update_limits"));
  info->_global.global.event = XmToggleButtonGadgetGetState(XtNameToWidget(w, "def_event_text"));
  info->_global.global.print_title =
      XmToggleButtonGadgetGetState(XtNameToWidget(w, "def_print_text"));
  info->_global.global.pad_label =
      XmToggleButtonGadgetGetState(XtNameToWidget(w, "def_padlabel_expression"));
  info->_global.global.x_grid_labels =
      XmToggleButtonGadgetGetState(XtNameToWidget(w, "def_x_grid_labels"));
  info->_global.global.y_grid_labels =
      XmToggleButtonGadgetGetState(XtNameToWidget(w, "def_y_grid_labels"));
  info->_global.global.show_mode = XmToggleButtonGadgetGetState(XtNameToWidget(w, "def_show_mode"));
  info->_global.global.step_plot = XmToggleButtonGadgetGetState(XtNameToWidget(w, "def_step_plot"));
  info->_global.global.x_grid_lines =
      XmToggleButtonGadgetGetState(XtNameToWidget(w, "def_x_grid_lines"));
  info->_global.global.y_grid_lines =
      XmToggleButtonGadgetGetState(XtNameToWidget(w, "def_y_grid_lines"));
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

void SetDirMask(Widget w, String * file, XmAnyCallbackStruct * callback_data __attribute__ ((unused)))
{
  if (*file) {
    XmString mask;
    char *tmpfile = strcpy(malloc(strlen(*file) + 10), *file);
    char *typpos = strrchr(tmpfile, '.');
    char *dirpos = strrchr(tmpfile, '/');
    if (dirpos) {
      dirpos[1] = 0;
      strcat(tmpfile, "*");
      if (typpos && typpos > dirpos)
	strcat(tmpfile, typpos);
      else
	strcat(tmpfile, ".dat");
    } else
      strcpy(tmpfile, "*.dat");
    mask = XmStringCreateSimple(tmpfile);
    XtVaSetValues(w, XmNdirMask, mask, NULL);
    XmStringFree(mask);
    free(tmpfile);
  }
}

void DisplayHelp(Widget w_in __attribute__ ((unused)),
		 String tag __attribute__ ((unused)),
		 XtPointer callback_data __attribute__ ((unused)))
{
}

void ResetWave(WaveInfo * info)
{
  ReplaceString(&info->database, "", 0);
  ReplaceString(&info->shot, "", 0);
  ReplaceString(&info->default_node, "", 0);
  ReplaceString(&info->x, "", 0);
  ReplaceString(&info->y, "", 0);
  ReplaceString(&info->event, "", 0);
  info->update = 1;
  ReplaceString(&info->title, "", 0);
  ReplaceString(&info->print_title, "", 0);
  ReplaceString(&info->pad_label, "", 0);
  info->x_grid_labels = 1;
  info->y_grid_labels = 1;
  info->show_mode = 0;
  info->step_plot = 0;
  info->x_grid_lines = 4;
  info->y_grid_lines = 4;
  info->_global.global_defaults = -1;
  ReplaceString(&info->xmin, "", 0);
  ReplaceString(&info->xmax, "", 0);
  ReplaceString(&info->ymin, "", 0);
  ReplaceString(&info->ymax, "", 0);
}

void DisableGlobalDefault(Widget w, String tag, XtPointer callback_data __attribute__ ((unused)))
{
  Widget top = w;
  Widget tw;
  Widget defaults_widget;
  char name[128];
  Widget button;
  for (tw = w; tw; top = tw, tw = XtParent(tw)) ;
  defaults_widget = XtNameToWidget(top, "*defaults_setup_db");
  strcpy(name, "def_");
  strcat(name, strlen(tag) ? tag : XtName(w));
  button = XtNameToWidget(defaults_widget, name);
  if (button)
    XmToggleButtonGadgetSetState(button, 0, 1);
}

void InitDefaultsSetupWidget(Widget w, int *tag __attribute__ ((unused)),
			     XtPointer callback_data __attribute__ ((unused)))
{
  static int default_db_inited = 0;
  if (!default_db_inited) {
    Widget top = w;
    Widget tw;
    Widget defaults_widget;
    Widget setup_widget;
    int num;
    Widget *child;
    int i;
    Dimension width;
    Dimension height;
    for (tw = w; tw; top = tw, tw = XtParent(tw)) ;
    defaults_widget = XtNameToWidget(top, "*defaults_setup_db");
    setup_widget = XtNameToWidget(top, "*data_setup_db");
    XtVaGetValues(defaults_widget, XmNnumChildren, &num, XmNchildren, &child, NULL);
    XtVaGetValues(setup_widget, XtNwidth, &width, XtNheight, &height, NULL);
    XtVaSetValues(defaults_widget, XtNwidth, width, XtNheight, height, NULL);
    for (i = 0; i < num; i++) {
      Widget w = XtNameToWidget(setup_widget, &XtName(child[i])[4]);
      if (w) {
	Position x;
	Position y;
	XtVaGetValues(w, XtNx, &x, XtNy, &y, NULL);
	XtVaSetValues(child[i], XtNx, x, XtNy, y, NULL);
      }
    }
    default_db_inited = 1;
  }
}

static String Concat(String prefix, String postfix)
{
  String answer = XtMalloc(strlen(prefix) + strlen(postfix) + 1);
  strcpy(answer, prefix);
  strcat(answer, postfix);
  return answer;
}

void ExpandReset(Widget w, int *tag, XtPointer callback_data __attribute__ ((unused)))
{
  Widget dsw;
  Widget exw;
  String exp;
  for (dsw = w; XtParent(dsw); dsw = XtParent(dsw)) ;
  if (*tag)
    XtVaGetValues(dsw, XmNuserData, &dsw, NULL);
  exw = XtNameToWidget(dsw, "*expand_dialog");
  exp = XmTextGetString(XtNameToWidget(dsw, "*y_expression"));
  XmTextSetString(XtNameToWidget(exw, "*expanded_y"), exp);
  XtFree(exp);
  exp = XmTextGetString(XtNameToWidget(dsw, "*x_expression"));
  XmTextSetString(XtNameToWidget(exw, "*expanded_x"), exp);
  XtFree(exp);
  if (!XtIsManaged(exw)) {
    XtVaSetValues(exw, XmNuserData, dsw, NULL);
    XtManageChild(exw);
  }
}

void ExpandCancel(Widget w, int *tag __attribute__ ((unused)), XtPointer callback_data __attribute__ ((unused)))
{
  Widget dsw;
  for (dsw = w; XtParent(dsw); dsw = XtParent(dsw)) ;
  XtUnmanageChild(XtNameToWidget(dsw, "*expand_dialog"));
}

void ExpandOk(Widget w, int *tag __attribute__ ((unused)), XtPointer callback_data __attribute__ ((unused)))
{
  Widget dsw;
  Widget exw;
  String exp;
  for (dsw = w; XtParent(dsw); dsw = XtParent(dsw)) ;
  if (*tag)
    XtVaGetValues(dsw, XmNuserData, &dsw, NULL);
  exw = XtNameToWidget(dsw, "*expand_dialog");
  exp = XmTextGetString(XtNameToWidget(exw, "*expanded_y"));
  XmTextSetString(XtNameToWidget(dsw, "*y_expression"), exp);
  XtFree(exp);
  exp = XmTextGetString(XtNameToWidget(exw, "*expanded_x"));
  XmTextSetString(XtNameToWidget(dsw, "*x_expression"), exp);
  XtFree(exp);
  ExpandCancel(exw, 0, 0);
}

static String FixupLFS(String in)
{
  int i;
  int j;
  int lfs = 0;
  String out;
  for (i = 0; in[i]; i++)
    if (in[i] == '\n')
      lfs++;
  out = XtMalloc(strlen(in) + lfs * 2 + 1);
  for (i = 0, j = 0; in[i]; i++) {
    if (in[i] == '\n') {
      out[j++] = '|';
      out[j++] = '|';
      out[j++] = '|';
    } else
      out[j++] = in[i];
  }
  out[j] = 0;
  return out;
}

static String FixupBARS(String in)
{
  int i;
  int j;
  int lfs = 0;
  String out;
  for (i = 0; in[i] && in[i + 1] && in[i + 2]; i++)
    if (in[i] == '|' && in[i + 1] == '|' && in[i + 2] == '|') {
      lfs++;
      i += 2;
    }
  out = XtMalloc(strlen(in) - lfs * 2 + 1);
  for (i = 0, j = 0; in[i]; i++) {
    if (lfs && in[i] == '|' && in[i + 1] == '|' && in[i + 2] == '|') {
      out[j++] = '\n';
      lfs--;
      i += 2;
    } else
      out[j++] = in[i];
  }
  out[j] = 0;
  return out;
}

static void SetOptionIdx(Widget w, int idx)
{
  static Widget pulldown;
  XtVaGetValues(w, XmNsubMenuId, &pulldown, NULL);
  if (pulldown) {
    static Widget *options;
    static Cardinal num_options;
    XtVaGetValues(pulldown, XmNchildren, &options, XmNnumChildren, &num_options, NULL);
    if (idx < (int)num_options)
      XtVaSetValues(w, XmNmenuHistory, options[idx], NULL);
  }
}

static int GetOptionIdx(Widget w)
{
  int idx = -1;
  static Widget pulldown;
  static Widget option;
  XtVaGetValues(w, XmNsubMenuId, &pulldown, XmNmenuHistory, &option, NULL);
  if (pulldown && option) {
    static Widget *options;
    static Cardinal num_options;
    int i;
    XtVaGetValues(pulldown, XmNchildren, &options, XmNnumChildren, &num_options, NULL);
    for (i = 0; i < (int)num_options; i++)
      if (options[i] == option) {
	idx = i;
	break;
      }
  }
  return idx;
}

void PositionPopupMenu(XmRowColumnWidget w, XButtonEvent * event)
{
  Widget option = w->row_column.memory_subwidget;
  XtX(w) = event->x_root - (option ? XtX(option) + XtWidth(option) / 2 : 0);
  XtY(w) = event->y_root - (option ? XtY(option) + XtHeight(option) / 2 : 0);
  RC_SetWidgetMoved(w, 1);
  RC_CascadeBtn(w) = XtWindowToWidget(XtDisplay(w), event->window);
}

void PopupComplaint(Widget parent, String string)
{
  Widget new_parent;
  Widget w;
  int i;
  for (new_parent = parent; new_parent && !XtIsWidget(new_parent);
       new_parent = XtParent(new_parent)) ;
  if (new_parent) {
    static XtCallbackRec ok_callback_list[] = { {(XtCallbackProc) XtDestroyWidget, 0}, {0, 0} };
    Arg args[] = { {XmNmessageString, (XtArgVal) 0},
    {XmNdialogTitle, (XtArgVal) 0},
    {XmNokLabelString, (XtArgVal) 0},
    {XmNmessageAlignment, (XtArgVal) XmALIGNMENT_BEGINNING},
    {XmNminimizeButtons, (XtArgVal) TRUE},
    {XmNokCallback, (XtArgVal) ok_callback_list},
    {XmNdefaultPosition, (XtArgVal) 1}
    };
    args[0].value = (XtArgVal) XmStringCreateLtoR(string, XmFONTLIST_DEFAULT_TAG);
    args[1].value = (XtArgVal) XmStringCreateSimple("Error");
    args[2].value = (XtArgVal) XmStringCreateSimple("Dismiss");
    w = XmCreateErrorDialog(new_parent, "Complaint", args, XtNumber(args));
    for (i = 0; i < 3; i++)
      XmStringFree((XmString) args[i].value);
    XtDestroyWidget(XmMessageBoxGetChild(w, XmDIALOG_CANCEL_BUTTON));
    XtDestroyWidget(XmMessageBoxGetChild(w, XmDIALOG_HELP_BUTTON));
    XtManageChild(w);
  } else
    printf
	("Error displaying dialog box\nCould not find widget to 'parent' box\nError message was:\n\t%s\n",
	 string);
}
