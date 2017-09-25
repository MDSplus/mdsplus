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
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSXDBOXDIALOGBUTTON.C */
/*  *13   23-JUN-1994 10:48:47 TWF "Motif 1.2" */
/*  *12   22-FEB-1994 15:18:29 TWF "remove NO_X_GBLS" */
/*  *11    4-MAR-1993 10:05:54 JAS "use xmdsshr.h" */
/*  *10    2-MAR-1993 12:38:41 JAS "port to decc" */
/*  *9    22-JAN-1992 10:10:58 TWF "Add XmdsXdBoxDButtonGetXdBoxD" */
/*  *8    21-JAN-1992 11:17:44 JAS "compilation problem" */
/*  *7    21-JAN-1992 10:39:00 TWF "Support -1 device_nid" */
/*  *6    16-JAN-1992 16:42:53 TWF "No need to do reset" */
/*  *5    16-JAN-1992 12:49:26 JAS "typos" */
/*  *4    16-JAN-1992 12:48:27 JAS "typos" */
/*  *3    16-JAN-1992 11:44:19 TWF "Call XmdsCreateXdBoxDialog" */
/*  *2    15-JAN-1992 12:40:49 TWF "Fix resource" */
/*  *1    15-JAN-1992 12:33:56 TWF "Add XmdsXdBoxDialogButton" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSXDBOXDIALOGBUTTON.C */
/*------------------------------------------------------------------------------

		Name:   XmdsXdBoxDialogButton

		Type:   C function

		Author:	TOM FREDIAN

		Date:   15-JAN-1992

		Purpose:  Popup XdBoxDialog button Pseudo-widget

------------------------------------------------------------------------------

	Call sequence:

Widget XmdsCreateXdBoxDialogButton(Widget parent, String name, ArgList args, Cardinal argcount);
Boolean XmdsIsXdBoxDialogButton(Widget w);
void XmdsXdBoxDialogButtonReset(Widget w);
int XmdsXdBoxDialogButtonPut(Widget w);
int XmdsXdBoxDialogButtonApply(Widget w);
Widget XmdsXdBoxDialogButtonGetXdBox(Widget w);

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------*/
#include <Xm/PushB.h>
#include <Xmds/XmdsXdBoxDialogButton.h>
#include <Xmds/XmdsXdBoxDialog.h>
#include <Xmds/XmdsXdBox.h>
#include <Mrm/MrmPublic.h>
#include <xmdsshr.h>

typedef struct _Resources {
  int nid;
  int nid_offset;
  Boolean put_on_apply;
  Widget popup_w;
} Resources;

static Resources *GetResources(Widget w);
static void Destroy(Widget w, Resources * info, XtPointer cb);
static void Popup(Widget w, Resources * info, XtPointer cb);

static XtResource resources[] = {
  {XmdsNnid, "Nid", XmRInt, sizeof(int), XtOffsetOf(Resources, nid), XmRImmediate, 0},
  {XmdsNnidOffset, "Nid", XmRInt, sizeof(int), XtOffsetOf(Resources, nid_offset), XmRImmediate, 0},
  {XmdsNputOnApply, "PutOnApply", XmRBoolean, sizeof(Boolean), XtOffsetOf(Resources, put_on_apply),
   XmRImmediate, (void *)1}
};

EXPORT Widget XmdsCreateXdBoxDialogButton(Widget parent, String name, ArgList args, Cardinal argcount)
{
  Widget w;
  Resources *info = (Resources *) XtMalloc(sizeof(Resources));
  Resources default_info = { 0, 0, 1, 0 };
  *info = default_info;
  XmdsSetSubvalues(info, resources, XtNumber(resources), args, argcount);
  if (info->nid == -1)
    info->nid = XmdsGetDeviceNid();
  w = XmCreatePushButton(parent, name, args, argcount);
  {
    Arg arglist[] = { {XmdsNnid, 0}
    ,
    {XmdsNnidOffset, 0}
    ,
    {XmdsNputOnApply, 0}
    };
    arglist[0].value = info->nid;
    arglist[1].value = info->nid_offset;
    arglist[2].value = info->put_on_apply;
    info->popup_w = XmdsCreateXdBoxDialog(parent, name, arglist, XtNumber(arglist));
  }
  XtAddCallback(w, XmNdestroyCallback, (XtCallbackProc) Destroy, info);
  XtAddCallback(w, XmNactivateCallback, (XtCallbackProc) Popup, info);
  return w;
}

static void Destroy(Widget w, Resources * info, XtPointer cb)
{
  XtFree((char *)info);
}

static void Popup(Widget w, Resources * info, XtPointer cb)
{
  if (info->popup_w)
    XmdsManageWindow(info->popup_w);
}

EXPORT Boolean XmdsIsXdBoxDialogButton(Widget w)
{
  return GetResources(w) != 0;
}

static Resources *GetResources(Widget w)
{
  Resources *answer = 0;
  if (XmIsPushButton(w) && (XtHasCallbacks(w, XmNdestroyCallback) == XtCallbackHasSome)) {
    XtCallbackList callbacks;
    XtVaGetValues(w, XmNdestroyCallback, &callbacks, NULL);
    for (;
	 callbacks->callback
	 && !(answer =
	      (Resources *) ((callbacks->callback == (XtCallbackProc) Destroy) ? callbacks->
			     closure : 0)); callbacks = callbacks + 1) ;
  }
  return answer;
}

EXPORT void XmdsXdBoxDialogButtonReset(Widget w)
{
  Resources *info = GetResources(w);
  if (info)
    XmdsXdBoxReset(info->popup_w);
  return;
}

EXPORT int XmdsXdBoxDialogButtonPut(Widget w)
{
  Resources *info = GetResources(w);
  return info ? XmdsXdBoxPut(info->popup_w) : 0;
}

EXPORT int XmdsXdBoxDialogButtonApply(Widget w)
{
  Resources *info = GetResources(w);
  return info ? XmdsXdBoxApply(info->popup_w) : 0;
}

EXPORT Widget XmdsXdBoxDialogButtonGetXdBox(Widget w)
{
  Resources *info = GetResources(w);
  return info ? info->popup_w : 0;
}
