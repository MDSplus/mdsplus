/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSWIDGETCALLBACKS.C */
/*  *12    3-OCT-1994 16:05:36 TWF "Unix" */
/*  *11    3-OCT-1994 15:59:31 TWF "Unix" */
/*  *10   22-FEB-1994 10:48:21 TWF "Take out NO_X_GLBS" */
/*  *9     3-MAR-1993 16:13:05 JAS "use xmdsshr.h" */
/*  *8    26-FEB-1993 11:37:15 JAS "port to decc" */
/*  *7     8-JAN-1993 08:38:37 TWF "Remove call to acceptfocus" */
/*  *6    17-SEP-1992 10:44:22 TWF "Remove setinputfocus call" */
/*  *5     6-FEB-1992 11:40:34 TWF "Remove XmdsNapplyProc" */
/*  *4     6-FEB-1992 11:20:49 TWF "Add XmdsNapplyProc to ApplyButton" */
/*  *3    14-JAN-1992 16:41:38 TWF "Add Apply,Ok,Reset,Cancel callbacks" */
/*  *2    14-JAN-1992 16:29:45 TWF "Add Apply,Ok,Reset,Cancel callbacks" */
/*  *1     8-OCT-1991 13:33:44 TWF "Callback convenience routines" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSWIDGETCALLBACKS.C */
/*------------------------------------------------------------------------------

		Name:   XmdsWIDGETCALLBACKS

		Type:   C function

		Author:	TOM FREDIAN

		Date:    9-MAR-1990

		Purpose: Generic callbacks which take a widget as the tag
			 Use XmdsRegisterWidgetCallback to enable widgets to be
			 tag argument.

------------------------------------------------------------------------------

	Call sequence:

void XmdsManageChildCallback(Widget w1,Widget *w2);
void XmdsUnmanageChildCallback(Widget w1,Widget *w2);
void XmdsDestroyWidgetCallback(Widget w1,Widget *w2);
void XmdsRegisterWidgetCallback(Widget w1,Widget *w2);
void XmdsRaiseWindow(Widget w);
void XmdsManageWindow(Widget w);

------------------------------------------------------------------------------
   Copyright (c) 1990
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:


------------------------------------------------------------------------------*/

#include <Xm/Xm.h>

static Widget FindShellChild(Widget w)
{
  Widget sc;
  for (sc = w; sc && !XtIsShell(XtParent(sc)); sc = XtParent(sc));
  return sc;
}

void XmdsRaiseWindow(Widget w)
{
  Widget shell;
  for (shell = w; shell && !XtIsShell(shell); shell = XtParent(shell));
  if (shell)
    XtPopup(shell,XtGrabNone);
}

void XmdsManageWindow(Widget w)
{
  if (!XtIsManaged(w))
    XtManageChild(w);
  else
    XmdsRaiseWindow(w);
}

void XmdsManageChildCallback(Widget w1,Widget *w2)
{
  XmdsManageWindow(*w2);
}

void XmdsUnmanageChildCallback(Widget w1,Widget *w2)
{
  XtUnmanageChild(*w2);
}

void XmdsDestroyWidgetCallback(Widget w1,Widget *w2)
{
  XtDestroyWidget(*w2);
}

void XmdsRegisterWidgetCallback(Widget w1,Widget *w2)
{
  *w2 = w1;
}

#ifndef _NO_XDS
extern void XmdsResetAllXds(Widget w);
extern Boolean XmdsXdsAreValid(Widget w);
extern Boolean XmdsApplyAllXds(Widget w);

void XmdsResetCallback(Widget w)
{
  XmdsResetAllXds(FindShellChild(w));
  return;
}

void XmdsOkCallback(Widget w)
{
  int XmdsApplyCallback(Widget w);
  if (XmdsApplyCallback(w))
    XtDestroyWidget(FindShellChild(w));
}

int XmdsApplyCallback(Widget w)
{
  int status;
  Widget db = FindShellChild(w);
  if (status = XmdsXdsAreValid(db))
    status = XmdsApplyAllXds(db);
  return status;
}
#endif /* _NO_XDS */

void XmdsCancelCallback(Widget w)
{
  XtDestroyWidget(FindShellChild(w));
}
