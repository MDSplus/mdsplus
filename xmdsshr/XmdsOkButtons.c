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
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSOKBUTTONS.C */
/*  *21   23-JUN-1994 11:12:20 TWF "Motif 1.2" */
/*  *20    8-APR-1993 11:34:20 TWF "Use MrmPublic (much smaller)" */
/*  *19    4-MAR-1993 15:15:41 JAS "make it complile with vaxc" */
/*  *18    3-MAR-1993 16:00:07 JAS "use xmdsshr.h" */
/*  *17    2-MAR-1993 17:53:21 JAS "work around bugcheck" */
/*  *16   25-FEB-1993 09:32:06 JAS "port to decc" */
/*  *15   24-FEB-1993 15:13:20 JAS "make an include" */
/*  *14    6-FEB-1992 14:02:57 TWF "" */
/*  *13    6-FEB-1992 13:52:12 TWF "apply all xds" */
/*  *12    6-FEB-1992 12:56:16 TWF "Remove XmdsNapplyProc" */
/*  *11    6-FEB-1992 12:45:29 TWF "Remove XmdsNapplyProc" */
/*  *10    6-FEB-1992 11:39:15 TWF "Remove XmdsNapplyProc" */
/*  *9     6-FEB-1992 11:20:34 TWF "Add XmdsNapplyProc to ApplyButton" */
/*  *8    23-JAN-1992 09:51:49 TWF "make okbuttons widgets (vuit can't deal with gadgets)" */
/*  *7    23-JAN-1992 09:05:44 TWF "Make okbuttons gadgets" */
/*  *6    21-JAN-1992 09:31:49 TWF "Remove box" */
/*  *5    16-JAN-1992 11:27:12 TWF "Automatically add defaults button" */
/*  *4    16-JAN-1992 09:56:48 TWF "Change name of ident" */
/*  *3    16-JAN-1992 09:36:56 TWF "Fix compile error" */
/*  *2    16-JAN-1992 09:26:48 TWF "Change name of routine" */
/*  *1    16-JAN-1992 09:23:26 TWF "OkApplyResetCancel widget" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSOKBUTTONS.C */
/*------------------------------------------------------------------------------

		Name:   XmdsOkButtons

		Type:   C function

		Author:	TOM FREDIAN

		Date:   16-JAN-1992

		Purpose:  Ok, Apply, Reset, Cancel buttons with
			  XmdsOkCallback, XmdsApplyCallback, XmdsResetCallback,
		    and   XmdsCancelCallback activateCallbacks respectively.

------------------------------------------------------------------------------

	Call sequence:

int XmdsCreateOkButton(Widget parent, String name, ArgList args, Cardinal numargs);
int XmdsCreateApplyButton(Widget parent, String name, ArgList args, Cardinal numargs);
int XmdsCreateResetButton(Widget parent, String name, ArgList args, Cardinal numargs);
int XmdsCreateCancelButton(Widget parent, String name, ArgList args, Cardinal numargs);

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------*/

#include <Mrm/MrmPublic.h>
#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xmds/XmdsOkButtons.h>
#include <Xmds/XmdsCallbacks.h>
#include <xmdsshr.h>

static Widget CreateButton(Widget parent, String name, ArgList args, Cardinal numargs, String ident,
			   Boolean set_default);
static int Apply(Widget w, XtCallbackList callbacks);
static Widget FindShellChild(Widget w);
static void Ok(Widget w, XtCallbackList callbacks);
static void Destroy(Widget w, XtCallbackList callbacks);
static int CountCallbacks(XtCallbackList callbacks);

EXPORT Widget XmdsCreateOkButton(Widget parent, String name, ArgList args, Cardinal numargs)
{
  Widget w = CreateButton(parent, name, args, numargs, "Ok", 1);
  if (XtHasCallbacks(w, XmNactivateCallback) == XtCallbackHasSome) {
    int num_callbacks;
    XtCallbackList callbacks;
    XtVaGetValues(w, XmNactivateCallback, &callbacks, NULL);
    num_callbacks = CountCallbacks(callbacks);
    callbacks =
	(XtCallbackList) memcpy(XtMalloc(sizeof(*callbacks) * (num_callbacks + 1)), callbacks,
				sizeof(*callbacks) * (num_callbacks + 1));
    XtRemoveAllCallbacks(w, XmNactivateCallback);
    XtAddCallback(w, XmNactivateCallback, (XtCallbackProc) Ok, callbacks);
    XtAddCallback(w, XmNdestroyCallback, (XtCallbackProc) Destroy, callbacks);
  } else
    XtAddCallback(w, XmNactivateCallback, (XtCallbackProc) XmdsOkCallback, 0);
  return w;
}

static int CountCallbacks(XtCallbackList callbacks)
{
  int i;
  for (i = 0; callbacks->callback; callbacks = callbacks + 1, i++) ;
  return i;
}

static void Ok(Widget w, XtCallbackList callbacks)
{
  if (Apply(w, callbacks))
    XtDestroyWidget(FindShellChild(w));
}

static Widget FindShellChild(Widget w)
{
  Widget sc;
  for (sc = w; sc && !XtIsShell(XtParent(sc)); sc = XtParent(sc)) ;
  return sc;
}

static int Apply(Widget w, XtCallbackList callbacks)
{
  int status = XmdsXdsAreValid(FindShellChild(w));
  for (; callbacks->callback && status; callbacks++)
    status = (*(int (*)())callbacks->callback) (w, callbacks->closure);
  if (status)
    status = XmdsApplyAllXds(FindShellChild(w));
  return status;
}

static void Destroy(Widget w, XtCallbackList callbacks)
{
  XtFree((char *)callbacks);
}

EXPORT Widget XmdsCreateApplyButton(Widget parent, String name, ArgList args, Cardinal numargs)
{
  Widget w = CreateButton(parent, name, args, numargs, "Apply", 0);
  if (XtHasCallbacks(w, XmNactivateCallback) == XtCallbackHasSome) {
    int num_callbacks;
    XtCallbackList callbacks;
    XtVaGetValues(w, XmNactivateCallback, &callbacks, NULL);
    num_callbacks = CountCallbacks(callbacks);
    callbacks =
	(XtCallbackList) memcpy(XtMalloc(sizeof(*callbacks) * (num_callbacks + 1)), callbacks,
				sizeof(*callbacks) * (num_callbacks + 1));
    XtRemoveAllCallbacks(w, XmNactivateCallback);
    XtAddCallback(w, XmNactivateCallback, (XtCallbackProc) Apply, callbacks);
    XtAddCallback(w, XmNdestroyCallback, (XtCallbackProc) Destroy, callbacks);
  } else
    XtAddCallback(w, XmNactivateCallback, (XtCallbackProc) XmdsApplyCallback, 0);
  return w;
}

EXPORT Widget XmdsCreateResetButton(Widget parent, String name, ArgList args, Cardinal numargs)
{
  Widget w = CreateButton(parent, name, args, numargs, "Reset", 0);
  XtAddCallback(w, XmNactivateCallback, (XtCallbackProc) XmdsResetCallback, 0);
  return w;
}

EXPORT Widget XmdsCreateCancelButton(Widget parent, String name, ArgList args, Cardinal numargs)
{
  Widget w = CreateButton(parent, name, args, numargs, "Cancel", 0);
  XtAddCallback(w, XmNactivateCallback, (XtCallbackProc) XmdsCancelCallback, 0);
  return w;
}

static Widget CreateButton(Widget parent, String name, ArgList args, Cardinal numargs, String ident,
			   Boolean set_default)
{
  Arg arglist[] = { {XmNlabelString, 0} };

  Widget w;
  arglist[0].value = (long)XmStringCreateSimple(ident);
  w = XmCreatePushButton(parent, name, arglist, XtNumber(arglist));
  if (numargs)
    XtSetValues(w, args, numargs);
  if (set_default)
    XtVaSetValues(FindShellChild(w), XmNdefaultButton, w, NULL);
  XmStringFree((XmString) arglist[0].value);
  return w;
}
