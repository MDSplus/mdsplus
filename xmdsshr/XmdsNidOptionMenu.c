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
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSNIDOPTIONMENU.C */
/*  *25   23-JUN-1994 12:04:26 TWF "Motif 1.2" */
/*  *24   22-FEB-1994 15:18:09 TWF "remove NO_X_GBLS" */
/*  *23    4-MAR-1993 15:08:57 JAS "make it complile with vaxc" */
/*  *22    3-MAR-1993 15:58:53 JAS "use xmdsshr.h" */
/*  *21    3-MAR-1993 09:32:21 JAS "use prototypes" */
/*  *20   24-FEB-1993 17:48:11 JAS "port to decc" */
/*  *19    4-DEC-1992 15:22:35 JAS "put the xd into the computed guy when menuchanged" */
/*  *18    3-DEC-1992 15:03:18 JAS "add an entry callback to call the buttons' activate calllbacks" */
/*  *17    3-DEC-1992 10:50:13 JAS "Need num argument to getbuttons" */
/*  *16    2-DEC-1992 16:29:05 JAS "adding idxgetxd" */
/*  *15    2-DEC-1992 16:25:38 JAS "adding idxgetxd" */
/*  *14    2-DEC-1992 16:01:06 JAS "adding xmdsnidoptionmenuidxgetxd" */
/*  *13    2-DEC-1992 14:00:51 JAS "Fix GetXd" */
/*  *12    1-DEC-1992 16:41:59 JAS "Add getxd " */
/*  *11    1-DEC-1992 11:57:47 JAS "fix callbacks" */
/*  *10   30-NOV-1992 17:40:53 JAS "change the way callbacks are called" */
/*  *9    23-NOV-1992 16:13:09 TWF "Add XmdsNidOptionMenuButtons" */
/*  *8    26-JUN-1992 15:26:42 JAS "change include locations" */
/*  *7    30-JAN-1992 09:00:52 TWF "free merged" */
/*  *6    29-JAN-1992 13:00:49 TWF "Free merged arglist" */
/*  *5    29-JAN-1992 12:52:16 TWF "Why funny menu" */
/*  *4    29-JAN-1992 12:19:22 TWF "Need to copy stringtables" */
/*  *3    28-JAN-1992 17:17:34 TWF "Manage the buttons" */
/*  *2    28-JAN-1992 16:20:34 TWF "Add NidOptionMenu widge" */
/*  *1    28-JAN-1992 15:55:06 TWF "NidOptionMenu Pseudo widget" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSNIDOPTIONMENU.C */
/*------------------------------------------------------------------------------

		Name:   XmdsNidOptionMenu

		Type:   C function

		Author:	TOM FREDIAN

		Date:   28-JAN-1992

		Purpose: Display an option menu with MDS expressions for values.

------------------------------------------------------------------------------

	Call sequence:

Widget XmdsCreateNidOptionMenu(Widget parent, String name, ArgList args, Cardinal argcount);
Boolean XmdsIsNidOptionMenu(Widget w);
void XmdsNidOptionMenuReset(Widget w);
int XmdsNidOptionMenuPut(Widget w);
int XmdsNidOptionMenuApply(Widget w);
struct dsc$descriptor_xd *XmdsNidOptionMenuGetXd(Widget w);
struct dsc$descriptor_xd *XmdsNidOptionMenuIdxGetXd(Widget w, int selected);

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------*/

#include <mdsdescrip.h>
#include <treeshr.h>
#include <mdsshr.h>
#include <X11/StringDefs.h>
#include <Xm/RowColumn.h>
#include <Xm/PushBG.h>
#include <Xmds/XmdsCallbacks.h>
#include <Xmds/XmdsXdBox.h>
#include <Xmds/XmdsXdBoxDialogButton.h>
#include <Xmds/XmdsNidOptionMenu.h>
#include <Mrm/MrmPublic.h>
#include <Xm/Xm.h>
#include <xmdsshr.h>
#include <mds_stdarg.h>

typedef struct _Resources {
  int nid;
  int nid_offset;
  Boolean put_on_apply;
  Widget pulldown;
  XmStringTable labels;
  XmStringTable values;
} Resources;

extern int TdiCompile();
extern int MdsCompareXd();

static Resources *GetResources(Widget w);
static void Destroy(Widget w, Resources * info, XtPointer cb);
static void ButtonPushed(Widget w, int index, XmPushButtonCallbackStruct * cb);
static void MenuChanged(Widget w, Resources * info, XmRowColumnCallbackStruct * cb);
static XtResource resources[] = {
  {XmdsNnid, "Nid", XmRInt, sizeof(int), XtOffsetOf(Resources, nid), XmRImmediate, 0},
  {XmdsNnidOffset, "Nid", XmRInt, sizeof(int), XtOffsetOf(Resources, nid_offset), XmRImmediate, 0},
  {XmdsNputOnApply, "PutOnApply", XmRBoolean, sizeof(Boolean), XtOffsetOf(Resources, put_on_apply),
   XmRImmediate, (void *)1},
  {XmdsNlabelStrings, "Strings", XmRStringTable, sizeof(XmStringTable),
   XtOffsetOf(Resources, labels), XmRImmediate, 0}
  ,
  {XmdsNvalueStrings, "Strings", XmRStringTable, sizeof(XmStringTable),
   XtOffsetOf(Resources, values), XmRImmediate, 0}
  ,
};

EXPORT Widget XmdsCreateNidOptionMenu(Widget parent, String name, ArgList args, Cardinal argcount)
{
  Widget w;
  WidgetList children;
  int num;
  int idx;
  XmStringTable s;
  XmStringTable v;
  Resources *info = (Resources *) XtMalloc(sizeof(Resources));
  Resources default_info = { 0, 0, 1, 0, 0, 0 };
  *info = default_info;
  XmdsSetSubvalues(info, resources, XtNumber(resources), args, argcount);
  if (info->nid == -1)
    info->nid = XmdsGetDeviceNid();
  info->pulldown = XmCreatePulldownMenu(parent, "", args, argcount);
  XtAddCallback(info->pulldown, XmNentryCallback, (XtCallbackProc) MenuChanged, info);
  for (idx = 0, s = info->labels, v = info->values; s && *s && v && *v; s++, v++, idx++) {
    String expression = "";
    expression = XmStringUnparse(*v, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
    {
      static EMPTYXD(empty_xd);
      struct descriptor_xd *xd = (struct descriptor_xd *)XtMalloc(sizeof(empty_xd));
      struct descriptor exp_d = { 0, DTYPE_T, CLASS_S, (char *)0 };
      exp_d.length = strlen(expression);
      exp_d.pointer = expression;
      *xd = empty_xd;
      TdiCompile(&exp_d, xd MDS_END_ARG);
      {
	Arg arglist[] = { {XmNlabelString, 0},
	{XmNuserData, 0}
	};
	Widget b;
	arglist[0].value = (long)*s;
	arglist[1].value = (long)xd;
	b = XmCreatePushButtonGadget(info->pulldown, "", arglist, XtNumber(arglist));
	XtAddCallback(b, XmNactivateCallback, (XtCallbackProc) ButtonPushed,
		      (XtPointer) (idx + (char *)0));
      }
    }
  }
  {
    Arg arglist[] = { {XmNlabelString, 0},
    {XmdsNnid, 0},
    {XmdsNnidOffset, 0},
    {XmdsNputOnApply, 0}
    };
    arglist[0].value = (long)XmStringCreateSimple("Computed");
    arglist[1].value = info->nid;
    arglist[2].value = info->nid_offset;
    arglist[3].value = 0;
    XmdsCreateXdBoxDialogButton(info->pulldown, "computed", arglist, XtNumber(arglist));
    XmStringFree((XmString) arglist[0].value);
  }
  XtVaGetValues(info->pulldown, XtNnumChildren, &num, XtNchildren, &children, NULL);
  XtManageChildren(children, num);
  {
    Arg submenu_args[] = { {XmNsubMenuId, 0} };
    ArgList merged_args;
    submenu_args[0].value = (long)info->pulldown;
    merged_args = XtMergeArgLists(submenu_args, XtNumber(submenu_args), args, argcount);
    w = XmCreateOptionMenu(parent, name, merged_args, argcount + XtNumber(submenu_args));
    XtFree((char *)merged_args);
  }
  XtAddCallback(w, XmNdestroyCallback, (XtCallbackProc) Destroy, info);
  XmdsNidOptionMenuReset(w);
  return w;
}

static void Destroy(Widget w, Resources * info, XtPointer cb)
{
  int num;
  Widget *labels;
  int i;
  XtVaGetValues(info->pulldown, XtNnumChildren, &num, XtNchildren, &labels, NULL);
  num--;
  for (i = 0; i < num; i++) {
    struct descriptor_xd *xd;
    XtVaGetValues(labels[i], XmNuserData, &xd, NULL);
    MdsFree1Dx(xd, 0);
    XtFree((char *)xd);
  }
  XtFree((char *)info);
}

EXPORT Boolean XmdsIsNidOptionMenu(Widget w)
{
  return GetResources(w) != 0;
}

EXPORT int *XmdsNidOptionMenuGetButtons(Widget w, int *num)
{
  Widget par;
  int *ans = 0;
  for (par = w; par && !XmdsIsNidOptionMenu(par); par = XtParent(par)) ;
  if (par) {
    Resources *info = GetResources(par);
    XtVaGetValues(info->pulldown, XmNchildren, &ans, XmNnumChildren, num, NULL);
  }
  return ans;
}

static Resources *GetResources(Widget w)
{
  Resources *answer = 0;
  if (XmIsRowColumn(w) && (XtHasCallbacks(w, XmNdestroyCallback) == XtCallbackHasSome)) {
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

EXPORT void XmdsNidOptionMenuReset(Widget w)
{
  Resources *info = GetResources(w);
  if (info) {
    int nid = info->nid + info->nid_offset;
    if (nid) {
      int idx;
      int num;
      Widget *labels;
      EMPTYXD(xd);
      XtVaGetValues(info->pulldown, XtNnumChildren, &num, XtNchildren, &labels, NULL);
      num--;
      TreeGetRecord(nid, &xd);
      for (idx = 0; idx < num; idx++) {
	struct descriptor_xd *button_xd;
	XtVaGetValues(labels[idx], XmNuserData, &button_xd, NULL);
	if ((xd.l_length || button_xd->l_length) ?
	    MdsCompareXd((struct descriptor *)&xd, (struct descriptor *)button_xd) & 1 : 1)
	  break;
      }
      MdsFree1Dx(&xd, 0);
      XmdsSetOptionIdx(w, idx);
    }
  }
  return;
}

EXPORT struct descriptor_xd *XmdsNidOptionMenuIdxGetXd(Widget w, int selected)
{
  struct descriptor_xd *xd = 0;
  Resources *info = GetResources(w);
  if (info) {
    int num;
    Widget *buttons;
    XtVaGetValues(info->pulldown, XtNnumChildren, &num, XtNchildren, &buttons, NULL);
    num--;
    if (selected < num) {
      static EMPTYXD(empty);
      struct descriptor_xd *ans = (struct descriptor_xd *)XtMalloc(sizeof(struct descriptor_xd));
      XtVaGetValues(buttons[selected], XmNuserData, &xd, NULL);
      *ans = empty;
      MdsCopyDxXd((struct descriptor *)xd, ans);
      xd = ans;
    } else
      xd = (struct descriptor_xd *)
	  XmdsXdBoxGetXd(XmdsXdBoxDialogButtonGetXdBox(XtNameToWidget(info->pulldown, "computed")));
  }
  return xd;
}

EXPORT struct descriptor_xd *XmdsNidOptionMenuGetXd(Widget w)
{
  struct descriptor_xd *xd = 0;
  Resources *info = GetResources(w);
  if (info) {
    int selected = XmdsGetOptionIdx(w);
    int num;
    Widget *buttons;
    XtVaGetValues(info->pulldown, XtNnumChildren, &num, XtNchildren, &buttons, NULL);
    num--;
    if (selected < num) {
      static EMPTYXD(empty);
      struct descriptor_xd *ans = (struct descriptor_xd *)XtMalloc(sizeof(struct descriptor_xd));
      XtVaGetValues(buttons[selected], XmNuserData, &xd, NULL);
      *ans = empty;
      MdsCopyDxXd((struct descriptor *)xd, ans);
      xd = ans;
    } else
      xd = (struct descriptor_xd *)
	  XmdsXdBoxGetXd(XmdsXdBoxDialogButtonGetXdBox(XtNameToWidget(info->pulldown, "computed")));
  }
  return xd;
}

EXPORT int XmdsNidOptionMenuPut(Widget w)
{
  Resources *info = GetResources(w);
  int status = 0;
  if (info) {
    int selected = XmdsGetOptionIdx(w);
    int num;
    Widget *buttons;
    int nid = info->nid + info->nid_offset;
    XtVaGetValues(info->pulldown, XtNnumChildren, &num, XtNchildren, &buttons, NULL);
    num--;
    if (nid) {
      if (selected < num) {
	struct descriptor_xd *button_xd;
	EMPTYXD(xd);
	XtVaGetValues(buttons[selected], XmNuserData, &button_xd, NULL);
	TreeGetRecord(nid, &xd);
	if ((xd.l_length || button_xd->l_length) ?
	    !(MdsCompareXd((struct descriptor *)&xd, (struct descriptor *)button_xd) & 1) : 0)
	  status = TreePutRecord(nid, (struct descriptor *)button_xd, 0);
	else
	  status = 1;
      } else
	status = XmdsXdBoxDialogButtonPut(XtNameToWidget(info->pulldown, "computed"));
    }
  }
  return status;
}

EXPORT int XmdsNidOptionMenuApply(Widget w)
{
  Resources *info = GetResources(w);
  int status = 0;
  if (info) {
    if (info->put_on_apply)
      status = XmdsNidOptionMenuPut(w);
    else
      status = 1;
  }
  return status;
}

static void MenuChanged(Widget w, Resources * info, XmRowColumnCallbackStruct * cb)
{
  if (cb->reason == XmCR_ACTIVATE) {
    int num;
    int bnum = (char *)cb->data - (char *)0;
    Widget *buttons;
    struct descriptor_xd *xd = 0;
    XtVaGetValues(info->pulldown, XtNnumChildren, &num, XtNchildren, &buttons, NULL);
    num--;
    XtCallCallbacks(cb->widget, XmNactivateCallback, cb->data);
    if (bnum < num) {
      XtVaGetValues(buttons[bnum], XmNuserData, &xd, NULL);
      XmdsXdBoxSetXd(XmdsXdBoxDialogButtonGetXdBox(buttons[num]), (struct descriptor *)xd);
    }
  }
}

static void ButtonPushed(Widget w, int index, XmPushButtonCallbackStruct * cb)
{
}

EXPORT void XmdsNidOptionMenuSetButton(Widget w, int idx, String text)
{
  Resources *info = GetResources(w);
  int num;
  Widget *buttons;
  XtVaGetValues(info->pulldown, XtNnumChildren, &num, XtNchildren, &buttons, NULL);
  num--;
  if (idx < num) {
    XmString label = XmStringCreateSimple(text);
    XtVaSetValues(buttons[idx], XmNlabelString, label, NULL);
    XmStringFree(label);
  }
}
