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
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSXDBOXONOFFBUTTON.C */
/*  *15   23-JUN-1994 11:00:59 TWF "Motif 1.2" */
/*  *14   23-JUN-1994 10:51:20 TWF "Motif 1.2" */
/*  *13   22-FEB-1994 15:17:40 TWF "remove NO_X_GBLS" */
/*  *12    4-MAR-1993 15:47:24 JAS "make it complile with vaxc" */
/*  *11    4-MAR-1993 10:09:55 JAS "use xmdsshr.h" */
/*  *10    2-MAR-1993 12:49:50 JAS "port to decc" */
/*  *9    22-JAN-1992 11:03:45 TWF "Fix callbacks" */
/*  *8    22-JAN-1992 10:10:41 TWF "Fix callbacks" */
/*  *7    22-JAN-1992 09:42:40 TWF "Make xdbox and onoff button states the same" */
/*  *6    16-JAN-1992 16:43:00 TWF "No need to do reset" */
/*  *5    15-JAN-1992 16:11:30 TWF "Change adjust margin" */
/*  *4    15-JAN-1992 15:23:59 TWF "Fix xdboxonoff" */
/*  *3    15-JAN-1992 15:21:06 TWF "Fix xdboxonoff" */
/*  *2    15-JAN-1992 14:43:08 TWF "Fix compile error" */
/*  *1    15-JAN-1992 14:26:41 TWF "XmdsXdBoxOnOffButton widget" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSXDBOXONOFFBUTTON.C */
/*------------------------------------------------------------------------------

		Name:   XmdsXdBoxOnOffButton

		Type:   C function

		Author:	TOM FREDIAN

		Date:   15-JAN-1992

		Purpose:  Popup XdBoxOnOff button Pseudo-widget

------------------------------------------------------------------------------

	Call sequence:

Widget XmdsCreateXdBoxOnOffButton(Widget parent, String name, ArgList args, Cardinal argcount);
Boolean XmdsIsXdBoxOnOffButton(Widget w);
void XmdsXdBoxOnOffButtonReset(Widget w);
int XmdsXdBoxOnOffButtonPut(Widget w);
int XmdsXdBoxOnOffButtonApply(Widget w);

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------*/
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xmds/XmdsOnOffToggleButton.h>
#include <Xmds/XmdsXdBoxOnOffButton.h>
#include <Xmds/XmdsXdBoxDialogButton.h>
#include <Xmds/XmdsXdBox.h>
#include <Mrm/MrmPublic.h>
#include <xmdsshr.h>


typedef struct _Resources {
  int nid;
  int nid_offset;
  Boolean put_on_apply;
  XmString label;
} Resources;

static XtResource resources[] = {
  {XmdsNnid, "Nid", XmRInt, sizeof(int), XtOffsetOf(Resources, nid), XmRImmediate, 0},
  {XmdsNnidOffset, "Nid", XmRInt, sizeof(int), XtOffsetOf(Resources, nid_offset), XmRImmediate, 0},
  {XmdsNputOnApply, "PutOnApply", XmRBoolean, sizeof(Boolean), XtOffsetOf(Resources, put_on_apply),
   XmRImmediate, (void *)1},
  {XmNlabelString, "LabelString", XmRCompoundText, sizeof(XmString), XtOffsetOf(Resources, label),
   XmRImmediate, 0}
};

static void SetXdState(Widget w, Widget xd_w, XmToggleButtonCallbackStruct * cb);
static void SetTbState(Widget w, Widget oo_w, XmdsButtonCallbackStruct * cb);

EXPORT Widget XmdsCreateXdBoxOnOffButton(Widget parent, String name, ArgList args, Cardinal argcount)
{
  Widget w;
  Resources info = { 0, 0, 1, 0 };
  XmdsSetSubvalues(&info, resources, XtNumber(resources), args, argcount);
  w = XmCreateRowColumn(parent, name, args, argcount);
  XtVaSetValues(w, XmNpacking, XmPACK_TIGHT, XmNorientation, XmHORIZONTAL, XmNspacing, 0,
		XmNadjustMargin, 0, NULL);
  {
    Widget oo_w;
    Widget xd_w;
    XmString blank = XmStringCreateSimple("");
    Arg arglist[] = { {XmdsNnid, 0},
    {XmdsNnidOffset, 0},
    {XmdsNputOnApply, 0},
    {XmNlabelString, 0}
    };
    arglist[0].value = info.nid;
    arglist[1].value = info.nid_offset;
    arglist[2].value = info.put_on_apply;
    arglist[3].value = (long)blank;
    XtManageChild(oo_w =
		  XmdsCreateOnOffToggleButton(w, "xmds_xdbox_on_off", arglist, XtNumber(arglist)));
    arglist[3].value = (long)info.label;
    XtManageChild(xd_w =
		  XmdsCreateXdBoxDialogButton(w, "xmds_xdbox_dialog_button", arglist,
					      XtNumber(arglist)));
    xd_w = XmdsXdBoxDialogButtonGetXdBox(xd_w);
    XtAddCallback(oo_w, XmNvalueChangedCallback, (XtCallbackProc) SetXdState, xd_w);
    XtAddCallback(xd_w, XmdsNapplyCallback, (XtCallbackProc) SetTbState, oo_w);
    XtAddCallback(xd_w, XmdsNokCallback, (XtCallbackProc) SetTbState, oo_w);
  }
  return w;
}

static void SetXdState(Widget w, Widget xd_w, XmToggleButtonCallbackStruct * cb)
{
  XmdsXdBoxSetState(xd_w, cb->set);
}

static void SetTbState(Widget w, Widget oo_w, XmdsButtonCallbackStruct * cb)
{
  XmToggleButtonSetState(oo_w, cb->on_off, 0);
}

EXPORT Boolean XmdsIsXdBoxOnOffButton(Widget w)
{
  return XtNameToWidget(w, "xmds_xdbox_on_off") && XtNameToWidget(w, "xmds_xdbox_dialog_button");
}

EXPORT void XmdsXdBoxOnOffButtonReset(Widget w)
{
  if (XmdsIsXdBoxOnOffButton(w)) {
    XmdsOnOffToggleButtonReset(XtNameToWidget(w, "xmds_xdbox_on_off"));
    XmdsXdBoxDialogButtonReset(XtNameToWidget(w, "xmds_xdbox_dialog_button"));
  }
}

EXPORT int XmdsXdBoxOnOffButtonPut(Widget w)
{
  int status = 0;
  if (XmdsIsXdBoxOnOffButton(w))
    if ((status = XmdsOnOffToggleButtonPut(XtNameToWidget(w, "xmds_xdbox_on_off"))) & 1)
      status = XmdsXdBoxDialogButtonPut(XtNameToWidget(w, "xmds_xdbox_dialog_button"));
  return status;
}

EXPORT int XmdsXdBoxOnOffButtonApply(Widget w)
{
  int status = 0;
  if (XmdsIsXdBoxOnOffButton(w))
    if ((status = XmdsOnOffToggleButtonApply(XtNameToWidget(w, "xmds_xdbox_on_off"))) & 1)
      status = XmdsXdBoxDialogButtonApply(XtNameToWidget(w, "xmds_xdbox_dialog_button"));
  return status;
}
