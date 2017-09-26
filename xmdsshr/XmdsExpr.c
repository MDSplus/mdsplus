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
/*  CMS REPLACEMENT HISTORY, Element XMDSEXPR.C */
/*  *79   18-MAR-1997 10:22:38 TWF "Fix for DECC 5.3" */
/*  *78   18-MAR-1997 09:30:09 TWF "Fix for DECC 5.3" */
/*  *77   18-MAR-1997 09:29:16 TWF "Fix for DECC 5.3" */
/*  *76   19-OCT-1994 13:35:17 TWF "Nolonger support VAXC" */
/*  *75   23-JUN-1994 09:51:17 TWF "Motif 1.2" */
/*  *74   22-FEB-1994 14:56:32 TWF "remove NO_X_GBLS" */
/*  *73   16-APR-1993 08:12:26 TWF "Fix WidgetClasses" */
/*  *72   16-APR-1993 08:01:56 TWF "Fix WidgetClasses" */
/*  *71   15-APR-1993 12:35:30 TWF "Fix WidgetClasses" */
/*  *70   15-APR-1993 12:20:55 TWF "Fix WidgetClasses" */
/*  *69   17-MAR-1993 13:54:42 JAS "fix $shot being treated as a string" */
/*  *68    4-MAR-1993 16:18:20 JAS "remove illegal pragma" */
/*  *67    3-MAR-1993 15:52:28 JAS "use xmdsshr.h" */
/*  *66    2-MAR-1993 18:20:14 JAS "do decc" */
/*  *65   22-FEB-1993 17:27:40 JAS "port to decc" */
/*  *64   18-DEC-1992 17:37:06 JAS "Set default before decompiling" */
/*  *63   20-NOV-1992 15:27:59 TWF "Fix init sizing" */
/*  *62   20-NOV-1992 15:09:31 TWF "Fix initialize sizing" */
/*  *61   20-NOV-1992 15:07:26 TWF "Fix initialize sizing" */
/*  *60   20-NOV-1992 14:16:33 TWF "Fix initialize sizing" */
/*  *59   20-NOV-1992 13:03:25 TWF "Combine experfield" */
/*  *58   20-NOV-1992 12:42:36 TWF "Combine experfield" */
/*  *57   20-NOV-1992 12:36:32 TWF "Combine experfield" */
/*  *56   20-NOV-1992 12:33:32 TWF "Combine experfield" */
/*  *55   20-NOV-1992 12:30:22 TWF "Combine experfield" */
/*  *54   20-NOV-1992 12:22:30 TWF "Combine experfield" */
/*  *53   20-NOV-1992 12:14:53 TWF "Combine experfield" */
/*  *52   20-NOV-1992 10:40:54 TWF "Try out XmScrolledText" */
/*  *51   20-NOV-1992 10:32:29 TWF "Try out XmScrolledText" */
/*  *50   20-NOV-1992 10:07:51 TWF "Try out XmScrolledText" */
/*  *49   20-NOV-1992 10:05:14 TWF "Try out XmScrolledText" */
/*  *48   20-NOV-1992 09:55:38 TWF "Try out XmScrolledText" */
/*  *47   20-NOV-1992 09:23:53 TWF "Try out XmScrolledText" */
/*  *46   20-NOV-1992 09:17:11 TWF "Try out XmScrolledText" */
/*  *45   20-NOV-1992 09:04:18 TWF "Try out XmScrolledText" */
/*  *44   20-NOV-1992 09:02:29 TWF "Try out XmScrolledText" */
/*  *43   20-NOV-1992 08:47:51 TWF "Try out XmScrolledText" */
/*  *42   20-NOV-1992 08:46:15 TWF "Try out XmScrolledText" */
/*  *41   19-NOV-1992 17:16:43 TWF "Try out XmScrolledText" */
/*  *40   19-NOV-1992 17:11:01 TWF "Try out XmScrolledText" */
/*  *39   19-NOV-1992 17:10:19 TWF "Try out XmScrolledText" */
/*  *38   19-NOV-1992 16:59:47 TWF "Try out XmScrolledText" */
/*  *37   19-NOV-1992 16:41:20 TWF "Try out XmScrolledText" */
/*  *36   19-NOV-1992 16:38:20 TWF "Try out XmScrolledText" */
/*  *35   19-NOV-1992 16:28:01 TWF "Try out XmScrolledText" */
/*  *34   19-NOV-1992 16:20:00 TWF "Try out XmScrolledText" */
/*  *33   19-NOV-1992 16:01:25 TWF "Try out XmScrolledText" */
/*  *32   19-NOV-1992 15:41:33 TWF "Try out XmScrolledText" */
/*  *31   19-NOV-1992 15:40:08 TWF "Try out XmScrolledText" */
/*  *30   19-NOV-1992 15:31:26 TWF "Try out XmScrolledText" */
/*  *29   19-NOV-1992 15:29:38 TWF "Try out XmScrolledText" */
/*  *28   19-NOV-1992 15:01:22 TWF "Try out XmScrolledText" */
/*  *27   19-NOV-1992 14:43:02 TWF "Try out XmScrolledText" */
/*  *26   19-NOV-1992 14:03:40 TWF "Try out XmScrolledText" */
/*  *25   19-NOV-1992 14:03:17 TWF "Try out XmScrolledText" */
/*  *24   19-NOV-1992 13:51:57 TWF "Move quotes out only if DTYPE_S or DTYPE_D" */
/*  *23   14-APR-1992 12:33:26 JAS "Initialize status for Put" */
/*  *22   30-JAN-1992 17:38:33 JAS "fix the resizing behavior" */
/*  *21   21-JAN-1992 10:42:54 JAS "add XmdsGetDeviceNid if nid == -1" */
/*  *20   21-JAN-1992 10:41:14 JAS "add XmdsGetDeviceNid if nid == -1" */
/*  *19   16-JAN-1992 19:49:58 JAS "don't make it scrolled" */
/*  *18   16-JAN-1992 19:30:31 JAS "make text scrolled text" */
/*  *17   16-JAN-1992 18:32:02 JAS "some fixes" */
/*  *16   16-JAN-1992 11:04:38 JAS "some fixes" */
/*  *15   13-JAN-1992 19:14:58 JAS " " */
/*  *14   13-JAN-1992 17:57:01 JAS "don't decompile if it is text" */
/*  *13   13-JAN-1992 12:39:15 TWF "Add XmdsExprApply" */
/*  *12   10-JAN-1992 13:01:11 JAS "Add put on apply and fix some stuff" */
/*  *11   10-JAN-1992 10:45:30 JAS "More stuff about size ..." */
/*  *10    9-JAN-1992 17:44:13 JAS "Add Geometry Stuff" */
/*  *9    16-DEC-1991 19:06:15 JAS "make the text smaller instead of getting bigger" */
/*  *8    16-DEC-1991 19:00:35 JAS "make the text smaller instead of getting bigger" */
/*  *7     9-DEC-1991 12:50:05 TWF "Fix SetNid" */
/*  *6     9-DEC-1991 12:46:20 TWF "Fix SetNid" */
/*  *5     9-DEC-1991 12:20:28 TWF "Check out XmdsExprSetNid" */
/*  *4     9-DEC-1991 11:05:07 TWF "Fix MDS1$FREE1_DX" */
/*  *3     6-DEC-1991 17:49:27 JAS "fix setvalues" */
/*  *2     5-DEC-1991 15:59:15 TWF "Change reset to XmdsExprReset" */
/*  *1     5-DEC-1991 15:13:02 TWF "Expr Widget" */
/*  CMS REPLACEMENT HISTORY, Element XMDSEXPR.C */
/*------------------------------------------------------------------------------

		Name:   XmdsExpr

		Type:   C function

		Author:	Josh Stillerman

		Date:   23-OCT-1989

		Purpose: This module impliments an  XmdsExpr
			widget.

------------------------------------------------------------------------------

	Call sequence:
  Widget XmdsCreateExpr( Widget parent, char *name, ArgList args, Cardinal argcount );
  struct descriptor *XmdsExprGetXd (Widget w);
  Boolean XmdsExprPut(Widget w);
  Boolean XmdsExprApply(Widget w);
  void XmdsExprRegister();
  void XmdsExprReset(Widget w);
  void XmdsExprSetDefaultNid(Widget w, int nid);
  void XmdsExprSetNid(Widget w, int nid, int offset);
  void XmdsExprSetXd(Widget w, struct descriptor *dsc);

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------*/

#include <strroutines.h>
#include <mdsdescrip.h>
#include <treeshr.h>
#include <ncidef.h>
#include <mds_stdarg.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xmds/XmdsExprP.h>
#include <Xmds/XmdsExprField.h>
#include <Xmds/XmdsSupport.h>
#include <Mrm/MrmPublic.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/PushB.h>
/*------------------------------------------------------------------------------

 External functions or symbols referenced:                                    */

extern int TdiCompile();
extern int TdiDecompile();
extern char *DescToNull(struct descriptor_s *text);
#include <mdsshr.h>
extern int MdsCompareXd();
#include <treeshr.h>
#include <xmdsshr.h>
/*------------------------------------------------------------------------------

 Functions and symbols defined:						      */

/*------------------------------------------------------------------------------

 Internal functions or symbols:                                              */

static void ChangeQuotes(Widget q_w, XmdsExprWidget e_w);
static void InitializeExprField(Widget req, Widget new, ArgList args, Cardinal * num_args);
static void InitializeExpr(Widget req, Widget new, ArgList args, Cardinal * num_args);
static void Initialize(Widget req, Widget new, ArgList args, Cardinal * num_args, Boolean is_field);
static XtGeometryResult GeometryManager(Widget w, XtWidgetGeometry * desired,
					XtWidgetGeometry * allowed);
static void LoadExpr(XmdsExprWidget w, struct descriptor *dsc);
static int GetDefaultNid(XmdsExprWidget ew);
static void SetEnclosures(XmdsExprWidget w, struct descriptor *dsc);
static Boolean SetValues(Widget old, Widget req, Widget new, ArgList args, Cardinal * arg_count);
static void Resize(Widget w);
/*------------------------------------------------------------------------------

 Subroutines referenced:                                                      */

/*------------------------------------------------------------------------------

 Macros:                                                                      */

#define IsText(ptr) ( (ptr->dtype == DTYPE_T) &&\
                      ((ptr->class == CLASS_S) || (ptr->class == CLASS_D)) )

/*------------------------------------------------------------------------------

 Local variables:                                                             */

static struct descriptor_xd const empty_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };

/*------------------------------------------------------------------------------

 Global variables:                                                            */

static void Focus_In(Widget w, XEvent * event, String * params, Cardinal num_params);
static XtActionsRec actions[] = { {"Focus_In", (XtActionProc) Focus_In} };

static XtResource resources[] = {
  {XmdsNxd, "Xd", XtRPointer, sizeof(struct descriptor_xd *), XtOffset(XmdsExprWidget, expr.xd),
   XtRImmediate, (void *)0},
  {XmdsNdefaultNid, "Nid", XtRInt, sizeof(int), XtOffset(XmdsExprWidget, expr.default_nid),
   XtRImmediate, (XtPointer) - 1},
  {XmdsNnid, "Nid", XtRInt, sizeof(int), XtOffset(XmdsExprWidget, expr.nid), XtRImmediate,
   (XtPointer) 0},
  {XmdsNnidOffset, "NidOffset", XtRInt, sizeof(int), XtOffset(XmdsExprWidget, expr.nid_offset),
   XtRImmediate, (XtPointer) 0},
  {XmdsNcompile, "Compile", XtRFunction, sizeof(int (*)()), XtOffset(XmdsExprWidget, expr.compile),
   XtRImmediate, (XtPointer) TdiCompile},
  {XmdsNdecompile, "Decompile", XtRFunction, sizeof(int (*)()),
   XtOffset(XmdsExprWidget, expr.decompile), XtRImmediate, (XtPointer) TdiDecompile},
  {XmdsNautoQuote, "AutoQuote", XtRBoolean, sizeof(Boolean),
   XtOffset(XmdsExprWidget, expr.auto_quote), XtRImmediate, (XtPointer) False}
  ,
  {XmdsNdefaultQuote, "DefaultQuote", XtRBoolean, sizeof(Boolean),
   XtOffset(XmdsExprWidget, expr.default_quote), XtRImmediate,
   (XtPointer) True}
  ,
  {XmdsNputOnApply, "PutOnApply", XtRBoolean, sizeof(Boolean),
   XtOffset(XmdsExprWidget, expr.put_on_apply), XtRImmediate, (XtPointer) True}
};


XmdsExprClassRec xmdsExprClassRec = {
  {				/* core_class */
   /*   superclass:           */ (WidgetClass) & xmManagerClassRec,
   /*   class_name:           */ "XmdsExpr",
   /*   widget_size:          */ sizeof(XmdsExprWidgetRec),
   /*   class_initialize:     */ NULL,
   /*   class_part_initialize: */ NULL,
   /*   class_inited:         */ FALSE,
   /*   initialize:           */ InitializeExpr,
   /*   initialize_hook:      */ NULL,
   /*   realize:              */ XtInheritRealize,
   /*   actions:              */ actions,
   /*   num_actions:          */ XtNumber(actions),
   /*   resources:            */ resources,
   /*   num_resources:        */ XtNumber(resources),
   /*   xrm_class:            */ NULLQUARK,
   /*   compress_motion:      */ TRUE,
   /*   compress_exposure:    */ TRUE,
   /*   compress_enterleave:  */ TRUE,
   /*   visible_interest:     */ FALSE,
   /*   destroy:              */ NULL,
   /*   resize:               */ Resize,
   /*   expose:               */ XtInheritExpose,
   /*   set_values:           */ SetValues,
   /*   set_values_hook:      */ NULL,
   /*   set_values_almost:    */ XtInheritSetValuesAlmost,
   /*   get_values_hook:      */ NULL,
   /*   accept_focus:         */ NULL,
   /*   version:              */ XtVersionDontCheck,
   /*   callback_private:     */ NULL,
   /*   tm_table:             */ "<FocusIn>:Focus_In()\n",
   /*   query_geometry:       */ XtInheritQueryGeometry,
   /*   display_accelerator:  */ XtInheritDisplayAccelerator,
   /*   extension:            */ NULL
   }
  ,
  {				/* composite_class */
   /*   geometry_manager      */ GeometryManager,
   /*   change_managed:       */ XtInheritChangeManaged,
   /*   insert_child:         */ XtInheritInsertChild,
   /*   delete_child:         */ XtInheritDeleteChild,
					/*   extension:            */ NULL
					/* (char *)&composite_extension */
   }
  ,
  {				/* constraint_class */
   /* constraint resource list */ NULL,
   /* num_resources            */ 0,
   /* constraint_size;         */ 0,
   /* initialize               */ NULL,
   /* destroy                  */ NULL,
   /* set_values               */ NULL,
   /* extension                */ NULL
   }
  ,
  {				/* ManagerClassPart */
   /* translations             */ NULL,
   /* syn_resources            */ NULL,
   /* num_syn_resources        */ 0,
   /* syn_constraint_resources */ NULL,
   /* num_syn_constraint_resources */ 0,
   /* parent_process           */ XmInheritParentProcess,
   /* extension                */ 0
   }
  ,
  {				/*   expr_class */
   /*   extension        */ 0
   }
};

WidgetClass xmdsExprWidgetClass;
WidgetClass xmdsExprFieldWidgetClass;

/*------------------------------------------------------------------------------

 Executable:                                                                  */

/****************************************************
  Expr to create a  Expr Widget.  Use
  resources originalXd, and buttonCallback to setup
  the initial widget.
****************************************************/
EXPORT Widget XmdsCreateExpr(Widget parent, char *name, ArgList args, Cardinal argcount)
{
  Widget widg;
  widg = XtCreateWidget(name, (WidgetClass) & xmdsExprClassRec, parent, args, argcount);
  return widg;
}

EXPORT Widget XmdsCreateExprField(Widget parent, char *name, ArgList args, Cardinal argcount)
{
  Widget widg;
  xmdsExprClassRec.core_class.initialize = InitializeExprField;
  widg = XtCreateWidget(name, (WidgetClass) & xmdsExprClassRec, parent, args, argcount);
  xmdsExprClassRec.core_class.initialize = InitializeExpr;
  return widg;
}

#define GetString(w) (XmIsTextField(w) ? XmTextFieldGetString(w) : XmTextGetString(w))
#define SetString(w,string) (XmIsTextField(w) ? XmTextFieldSetString(w,string) : XmTextSetString(w,string))

EXPORT struct descriptor *XmdsExprFieldGetXd(Widget w)
{
  return XmdsExprGetXd(w);
}

EXPORT struct descriptor *XmdsExprGetXd(Widget w)
{
  struct descriptor_xd *ans = (struct descriptor_xd *)XtMalloc(sizeof(struct descriptor_xd));
  XmdsExprWidget ew = (XmdsExprWidget) w;
  char *text = GetString(ew->expr.text_widget);
  struct descriptor_s text_dsc = { 0, DTYPE_T, CLASS_S, (char *)0 };
  text_dsc.length = strlen(text);
  text_dsc.pointer = text;
  ResetErrors();
  *ans = empty_xd;
  if (ew->expr.is_text)
    MdsCopyDxXd((struct descriptor *)&text_dsc, ans);
  else {
    int status;
    int old_def;
    int def_nid = GetDefaultNid(ew);
    if (def_nid != -1) {
      TreeGetDefaultNid(&old_def);
      TreeSetDefaultNid(def_nid);
    }
    status = (*ew->expr.compile) (&text_dsc, ans MDS_END_ARG);
    if ((status & 1) == 0) {
      TdiComplain(w);
      XtFree((char *)ans);
      ans = 0;
    }
    if (def_nid != -1)
      TreeSetDefaultNid(old_def);
  }
  return (struct descriptor *)ans;
}

EXPORT Boolean XmdsExprFieldPut(Widget w)
{
  return XmdsExprPut(w);
}

EXPORT Boolean XmdsExprPut(Widget w)
{
  int status = 1;
  XmdsExprWidget ew = (XmdsExprWidget) w;
  int nid = ew->expr.nid + ew->expr.nid_offset;
  if (nid) {
    struct descriptor_xd *new_xd = (struct descriptor_xd *)XmdsExprGetXd((Widget) ew);
    if (new_xd) {
      struct descriptor_xd *old_xd;
      old_xd = (struct descriptor_xd *)TdiGet(nid);
      if ((!old_xd && new_xd->l_length)
	  || !MdsCompareXd((struct descriptor *)new_xd, (struct descriptor *)old_xd))
	status = TreePutRecord(nid, (struct descriptor *)new_xd, 0);
      if (old_xd) {
	MdsFree1Dx(old_xd, 0);
	XtFree((char *)old_xd);
      }
      MdsFree1Dx(new_xd, 0);
      XtFree((char *)new_xd);
    }
  }
  return status & 1;
}

EXPORT Boolean XmdsExprFieldApply(Widget w)
{
  return XmdsExprApply(w);
}

EXPORT Boolean XmdsExprApply(Widget w)
{
  XmdsExprWidget ew = (XmdsExprWidget) w;
  return ew->expr.put_on_apply ? XmdsExprPut(w) : 1;
}

EXPORT void XmdsExprFieldSetDefaultNid(Widget w, int nid)
{
  XmdsExprSetDefaultNid(w, nid);
}

EXPORT void XmdsExprSetDefaultNid(Widget w, int nid)
{
  XmdsExprWidget ew = (XmdsExprWidget) w;
  if (nid != ew->expr.default_nid) {
    ew->expr.default_nid = nid;
    LoadExpr(ew, (struct descriptor *)ew->expr.xd);
    Resize((Widget) ew);
  }
}

EXPORT void XmdsExprFieldRegister()
{
  XmdsExprRegister();
}

EXPORT void XmdsExprRegister()
{
  MrmRegisterClass(1, "XmdsExprWidget", "XmdsCreateExpr", (Widget (*)(void))XmdsCreateExpr, xmdsExprWidgetClass);
}

EXPORT void XmdsExprFieldReset(Widget w)
{
  XmdsExprReset(w);
}

EXPORT void XmdsExprReset(Widget w)
{
  XmdsExprWidget ew = (XmdsExprWidget) w;
  if (ew->expr.nid)
    XmdsExprSetNid(w, ew->expr.nid, ew->expr.nid_offset);
  else {
    LoadExpr(ew, (struct descriptor *)ew->expr.xd);
    Resize((Widget) ew);
  }
}

EXPORT void XmdsExprFieldSetNid(Widget w, int nid, int offset)
{
  XmdsExprSetNid(w, nid, offset);
}

EXPORT void XmdsExprSetNid(Widget w, int nid, int offset)
{
  XmdsExprWidget ew = (XmdsExprWidget) w;
  int new_nid;
  int status;
  if (ew->expr.xd)
    MdsFree1Dx(ew->expr.xd, 0);
  else {
    ew->expr.xd = (struct descriptor_xd *)XtMalloc(sizeof(struct descriptor_xd));
    *ew->expr.xd = empty_xd;
  }
  if (nid == -1)
    ew->expr.nid = XmdsGetDeviceNid();

  new_nid = ew->expr.nid + offset;
  status = TreeGetRecord(new_nid, ew->expr.xd);
  if (status & 1)
    LoadExpr(ew, (struct descriptor *)ew->expr.xd);
  else
    LoadExpr(ew, 0);
  Resize((Widget) ew);
  ew->expr.nid_offset = offset;
}

EXPORT void XmdsExprFieldSetXd(Widget w, struct descriptor *dsc)
{
  XmdsExprSetXd(w, dsc);
}

EXPORT void XmdsExprSetXd(Widget w, struct descriptor *dsc)
{
  XmdsExprWidget ew = (XmdsExprWidget) w;
  if (ew->expr.xd)
    MdsFree1Dx(ew->expr.xd, 0);
  else {
    ew->expr.xd = (struct descriptor_xd *)XtMalloc(sizeof(struct descriptor_xd));
    *ew->expr.xd = empty_xd;
  }
  MdsCopyDxXd(dsc, ew->expr.xd);
  LoadExpr(ew, (struct descriptor *)ew->expr.xd);
  Resize((Widget) ew);
}

static XtGeometryResult GeometryManager(Widget w, XtWidgetGeometry * desired,
					XtWidgetGeometry * allowed)
{
  return XtGeometryYes;
}

static void InitializeExpr(Widget req, Widget new, ArgList args, Cardinal * num_args)
{
  Initialize(req, new, args, num_args, 0);
}

static void InitializeExprField(Widget req, Widget new, ArgList args, Cardinal * num_args)
{
  Initialize(req, new, args, num_args, 1);
}

static void Initialize(Widget req, Widget new, ArgList args, Cardinal * num_args, Boolean is_field)
{
  XmdsExprWidget w = (XmdsExprWidget) new;
  static XtCallbackRec change_quotes_callback_list[] =
      { {(XtCallbackProc) ChangeQuotes, (XtPointer) 0}, {0, 0} };
  Arg text_args[] = { {XmNnavigationType, XmTAB_GROUP} };
  ArgList new_args = XtMergeArgLists(args, *num_args, text_args, XtNumber(text_args));
  int nid;
  change_quotes_callback_list[0].closure = (XtPointer) w;
  w->manager.navigation_type = XmTAB_GROUP;
  if (w->expr.auto_quote) {
    static Arg quote_args[] = {
      {XmNlabelString, 0},
      {XmNx, 0},
      {XmNheight, 20},
      {XmNactivateCallback, (XtArgVal) change_quotes_callback_list},
      {XmNtraversalOn, FALSE}
    };
    quote_args[0].value = (XtArgVal) XmStringCreateSimple("\"");
    w->expr.open_quote_widget =
	XmCreatePushButton((Widget) w, "open_quote", quote_args, XtNumber(quote_args));
    w->expr.close_quote_widget =
	XmCreatePushButton((Widget) w, "close_quote", quote_args, XtNumber(quote_args));
    XmStringFree((XmString) quote_args[0].value);
  } else {
    w->expr.open_quote_widget = 0;
    w->expr.close_quote_widget = 0;
  }
  w->expr.text_widget =
      is_field ? XmCreateTextField((Widget) w, "expr_text", new_args,
				   *num_args +
				   XtNumber(text_args)) : XmCreateScrolledText((Widget) w,
									       "expr_text",
									       new_args,
									       *num_args +
									       XtNumber(text_args));
  XtFree((char *)new_args);
  XtManageChild(w->expr.text_widget);
  if (w->expr.nid == -1)
    w->expr.nid = XmdsGetDeviceNid();
  nid = w->expr.nid + w->expr.nid_offset;
  if (nid)
    w->expr.xd = (struct descriptor_xd *)TdiGet(nid);
  else if (w->expr.xd) {
    XmdsExprWidget req_e_w = (XmdsExprWidget) req;
    w->expr.xd = (struct descriptor_xd *)XtMalloc(sizeof(struct descriptor_xd));
    *w->expr.xd = empty_xd;
    MdsCopyDxXd((struct descriptor *)req_e_w->expr.xd, w->expr.xd);
  }
  LoadExpr(w, (struct descriptor *)w->expr.xd);
  XtWidth(w) =
      XtWidth(w->expr.text_widget) +
      (w->expr.is_text ? 2 * (1 + XtWidth(w->expr.open_quote_widget)) : 0);
  XtHeight(w) = XtHeight(w->expr.text_widget);
  Resize((Widget) w);
}

static void Resize(Widget w)
{
  XmdsExprWidget ew = (XmdsExprWidget) w;
  Widget tw =
      XmIsTextField(ew->expr.text_widget) ? ew->expr.text_widget : XtParent(ew->expr.text_widget);
  if (ew->expr.is_text) {
    Dimension text_width = XtWidth(ew) - 2 * (1 + XtWidth(ew->expr.open_quote_widget));
    XtResizeWidget(tw, text_width, XtHeight(ew), 0);
    XtMoveWidget(tw, XtWidth(ew->expr.close_quote_widget) + 1, 0);
    XtMoveWidget(ew->expr.close_quote_widget, XtWidth(ew) - XtWidth(ew->expr.close_quote_widget),
		 0);
    XtManageChild(ew->expr.open_quote_widget);
    XtManageChild(ew->expr.close_quote_widget);
  } else {
    XtMoveWidget(tw, 0, 0);
    XtResizeWidget(tw, XtWidth(ew), XtHeight(ew), 0);
    if (ew->expr.open_quote_widget) {
      XtUnmanageChild(ew->expr.open_quote_widget);
      XtUnmanageChild(ew->expr.close_quote_widget);
      XtMoveWidget(ew->expr.close_quote_widget, XtWidth(ew) - XtWidth(ew->expr.close_quote_widget),
		   0);
    }
  }
}

static Boolean SetValues(Widget old, Widget req, Widget new, ArgList args, Cardinal * arg_count)
{
  XmdsExprWidget old_ew = (XmdsExprWidget) old;
  XmdsExprWidget req_ew = (XmdsExprWidget) req;
  XmdsExprWidget new_ew = (XmdsExprWidget) new;
  XtSetValues(new_ew->expr.text_widget, args, *arg_count);
  if ((old_ew->expr.nid != req_ew->expr.nid) ||
      (old_ew->expr.nid_offset != req_ew->expr.nid_offset))
    XmdsExprSetNid((Widget) new_ew, new_ew->expr.nid, new_ew->expr.nid_offset);
  else if (old_ew->expr.default_nid != req_ew->expr.default_nid) {
    new_ew->expr.default_nid = 0;
    XmdsExprSetDefaultNid((Widget) new_ew, req_ew->expr.default_nid);
  } else if (old_ew->expr.xd != req_ew->expr.xd) {
    if (old_ew->expr.xd) {
      MdsFree1Dx(old_ew->expr.xd, 0);
      XtFree((char *)old_ew->expr.xd);
    };
    new_ew->expr.xd = 0;
    XmdsExprSetXd((Widget) new_ew, (struct descriptor *)req_ew->expr.xd);
  }
  if (req_ew->expr.auto_quote != old_ew->expr.auto_quote) {
    if (req_ew->expr.auto_quote == 0) {
      XtDestroyWidget(new_ew->expr.open_quote_widget);
      XtDestroyWidget(new_ew->expr.close_quote_widget);
      new_ew->expr.open_quote_widget = 0;
      new_ew->expr.close_quote_widget = 0;
    } else {
      static XtCallbackRec change_quotes_callback_list[] =
	  { {(XtCallbackProc) ChangeQuotes, (XtPointer) 0}, {0, 0} };
      static Arg quote_args[] = {
	{XmNlabelString, 0},
	{XmNx, 0},
	{XmNheight, 20},
	{XmNactivateCallback, (XtArgVal) change_quotes_callback_list},
	{XmNtraversalOn, FALSE}
      };
      change_quotes_callback_list[0].closure = (XtPointer) new_ew;
      quote_args[0].value = (XtArgVal) XmStringCreateSimple("\"");
      new_ew->expr.open_quote_widget =
	  XmCreatePushButton((Widget) new_ew, "open_quote", quote_args, XtNumber(quote_args));
      new_ew->expr.close_quote_widget =
	  XmCreatePushButton((Widget) new_ew, "open_quote", quote_args, XtNumber(quote_args));
      XmStringFree((XmString) quote_args[0].value);
      SetEnclosures(new_ew, (struct descriptor *)new_ew->expr.xd);
    }
    if (req_ew->expr.default_quote != old_ew->expr.default_quote && new_ew->expr.auto_quote)
      SetEnclosures(new_ew, (struct descriptor *)new_ew->expr.xd);
  }
  Resize((Widget) new_ew);
  return 0;
}

static void ChangeQuotes(Widget q_w, XmdsExprWidget e_w)
{
  char *text = GetString(e_w->expr.text_widget);
  int text_len = strlen(text);
  char *new_text = XtMalloc(text_len + 3);
  new_text[0] = '\"';
  strcpy(&new_text[1], text);
  new_text[text_len + 1] = '\"';
  new_text[text_len + 2] = 0;
  SetString(e_w->expr.text_widget, new_text);
  XtFree(new_text);
  XtFree(text);
  e_w->expr.is_text = 0;
  Resize((Widget) e_w);
}

static int GetDefaultNid(XmdsExprWidget ew)
{
  int ans = ew->expr.default_nid;
  if (ans == -1) {
    int nid = ew->expr.nid + ew->expr.nid_offset;
    if (nid) {
      if (ConglomerateElt(nid))
	ans = NodeParent(ConglomerateHead(nid));
      else
	ans = NodeParent(nid);
    }
  }
  return ans;
}

static void Focus_In(Widget w, XEvent * event, String * params, Cardinal num_params)
{
  XmdsExprWidget ew = (XmdsExprWidget) w;
  XtCallActionProc(ew->expr.text_widget, "grab-focus", event, params, num_params);
}

static void LoadExpr(XmdsExprWidget w, struct descriptor *dsc)
{
  struct descriptor_xd *xd = (struct descriptor_xd *)dsc;
  int status;
  for (; xd && xd->dtype == DTYPE_DSC; xd = (struct descriptor_xd *)xd->pointer) ;
  if (xd && ((xd->class == CLASS_R) ||
	     (((xd->class == CLASS_XD) || (xd->class == CLASS_XS)) ? xd->l_length : xd->length))) {
    if (w->expr.auto_quote && IsText(xd)) {
      char *c_text = DescToNull((struct descriptor_s *)xd);
      w->expr.is_text = 1;
      SetString(w->expr.text_widget, c_text);
      XmTextSetInsertionPosition(w->expr.text_widget, (XmTextPosition) 0);
      XtFree(c_text);
    } else {
      static struct descriptor_d text = { 0, DTYPE_T, CLASS_D, 0 };
      int old_def;
      int def_nid = GetDefaultNid(w);
      if (def_nid != -1) {
	TreeGetDefaultNid(&old_def);
	TreeSetDefaultNid(def_nid);
      }
      status = (*w->expr.decompile) (xd, &text MDS_END_ARG);
      w->expr.is_text = 0;
      if (status & 1) {
	char *c_text = DescToNull((struct descriptor_s *)&text);
	SetString(w->expr.text_widget, c_text);
	XmTextSetInsertionPosition(w->expr.text_widget, (XmTextPosition) 0);
	XtFree(c_text);
      } else
	SetString(w->expr.text_widget, "");
      StrFree1Dx(&text);
      if (def_nid != -1)
	TreeSetDefaultNid(old_def);
    }
  } else {
    SetString(w->expr.text_widget, "");
    w->expr.is_text = w->expr.auto_quote && w->expr.default_quote;
  }
}

static void SetEnclosures(XmdsExprWidget w, struct descriptor *dsc)
{
  struct descriptor *ptr;
  for (ptr = dsc; ptr && (ptr->dtype == DTYPE_DSC); ptr = (struct descriptor *)ptr->pointer) ;
  w->expr.is_text = (ptr && IsText(ptr)) || ((ptr == 0) && w->expr.default_quote);
  if (w->expr.is_text) {
    char *chrs = GetString(w->expr.text_widget);
    if (chrs && strlen(chrs)) {
      chrs[strlen(chrs) - 1] = 0;
      SetString(w->expr.text_widget, &chrs[1]);
      XtFree(chrs);
    }
  }
}

char *DescToNull(struct descriptor_s *text)
{
  char *answer = memcpy(XtMalloc(text->length + 1), text->pointer, text->length);
  answer[text->length] = '\0';
  return answer;
}
