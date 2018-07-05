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
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSINPUTS.C */
/*  *11   22-FEB-1994 15:05:09 TWF "remove NO_X_GBLS" */
/*  *10   10-MAY-1993 14:16:46 TWF "Make it shareable" */
/*  *9     3-MAR-1993 09:24:14 JAS "use prototypes" */
/*  *8    24-FEB-1993 15:53:16 JAS "port to decc" */
/*  *7     9-DEC-1991 12:20:34 TWF "Check out XmdsExprSetNid" */
/*  *6     9-DEC-1991 11:09:19 TWF "Fix input_w reference" */
/*  *5     9-DEC-1991 11:05:20 TWF "Fix input_w reference" */
/*  *4     5-DEC-1991 17:29:36 TWF "Add widgettonumber" */
/*  *3     5-DEC-1991 17:27:12 TWF "Add widgettonumber" */
/*  *2     5-DEC-1991 17:23:12 TWF "Add widgettonumber" */
/*  *1     5-DEC-1991 17:07:29 TWF "XmdsInputs.c" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSINPUTS.C */
/*------------------------------------------------------------------------------

		Name:   XmdsInputs

		Type:   C function

		Author:	JOSH STILLERMAN

		Date:    2-APR-1990

		Purpose: Routines to handle digitizer channels (INPUTS)

------------------------------------------------------------------------------

	Call sequence:
  void XmdsInputCreateCallback(Widget w, XmdsInputCtx ctx)
  void XmdsResetInput(Widget chans_dlog, XmdsInputCtx ctx, int chan)
  void XmdsPutInputSetup(Widget inputs_dlog, XmdsInputCtx ctx, int chan)
  int XmdsWidgetToNumber(Widget w, String prefix);

------------------------------------------------------------------------------
   Copyright (c) 1990
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------

 Macros:                                                                      */

#include <mdsdescrip.h>
#include <stdio.h>
#include <stdlib.h>
#include <strroutines.h>
#include <treeshr.h>
#include <Xm/Xm.h>
#include <Xm/ToggleB.h>
#include <Xmds/XmdsInputs.h>
#include <Xmds/XmdsExpr.h>


/*------------------------------------------------------------------------------

 Executable:                                                                  */

/**************************************************
  XmdsInputCreateCallback - Routine used by the
  startidx and endidx expression widgets as their
  Create callback. Sets the Nid resource of the
  expression widget to the coresponding nid in the
  conglomerate.
***************************************************/
void XmdsInputCreateCallback(Widget w, XmdsInputCtx ctx)
{
  XmdsExprSetNid(w, ctx->inputs_nid + ctx->nodes_per_input * (XmdsWidgetToNumber(w, "input_") - 1),
		 (strcmp(XtName(w), "start_idx") ? ctx->end_offset : ctx->start_offset));
}

/*************************************************************
  XmdsResetInput - Routine to 'RESET' a channel of the digitizer.
  Called by the DW_SETUP Reset routine.
**************************************************************/
void XmdsResetInput(Widget chans_dlog, XmdsInputCtx ctx, int chan)
{
  char input_name[12];
  Widget input_w;
  sprintf(input_name, "*input_%d", chan);
  input_w = XtNameToWidget(chans_dlog, input_name);
  if (input_w) {
    int input_nid = ctx->inputs_nid + (chan - 1) * ctx->nodes_per_input;
    char *path;
    XmString label;
    XmToggleButtonSetState(XtNameToWidget(input_w, "onoff"), TreeIsOn(input_nid) & 1, (Boolean) 0);
    path = TreeGetMinimumPath(0, input_nid + ctx->data_offset);
    label = XmStringCreateSimple(path);
    TreeFree(path);
    XtVaSetValues(XtNameToWidget(input_w, "path"), XmNlabelString, label, NULL);
    XmStringFree(label);
  }
}

/*****************************************************************
  XmdsPutInputSetup - Called by the Apply callback in the DW_SETUP
  routines.  Writes out the information specified by the user for
  a channel.
******************************************************************/
EXPORT void XmdsPutInputSetup(Widget inputs_dlog, XmdsInputCtx ctx, int chan)
{
  char input_name[12];
  Widget input_w;
  sprintf(input_name, "*input_%d", chan);
  input_w = XtNameToWidget(inputs_dlog, input_name);
  if (input_w) {
    int nid = ctx->inputs_nid + (chan - 1) * ctx->nodes_per_input;
    XmdsExprPut(XtNameToWidget(input_w, "start_idx"));
    XmdsExprPut(XtNameToWidget(input_w, "end_idx"));
    if (XmToggleButtonGetState(XtNameToWidget(input_w, "onoff")) & 1)
      TreeTurnOn(nid);
    else
      TreeTurnOff(nid);
  }
}

EXPORT int XmdsWidgetToNumber(Widget w, String prefix)
{
  Widget widg;
  int number = -1;
  int length = strlen(prefix);
  char *endptr;
  for (widg = w; widg; widg = XtParent(widg)) {
    String name = XtName(widg);
    int namelen = strlen(name);
    if (namelen > length) {
      if (!strncmp(prefix, name, length)) {
	number = strtol(name + length, &endptr, 10);
	if (endptr >= (name + namelen))
	  break;
      }
    }
  }
  return number;
}
