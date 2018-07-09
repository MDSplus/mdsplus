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
/*  CMS REPLACEMENT HISTORY, Element XMDSXDBOX.C */
/*  *71   18-MAR-1997 10:22:32 TWF "Fix for DECC 5.3" */
/*  *70   18-MAR-1997 09:33:14 TWF "Fix for DECC 5.3" */
/*  *69   21-JUL-1995 09:32:54 TWF "Fix XtCallCallbackList" */
/*  *68    3-JAN-1995 17:23:58 JAS "fix empty" */
/*  *67   27-DEC-1994 09:59:26 JAS "fix calling of callbacks" */
/*  *66   19-OCT-1994 13:35:11 TWF "Nolonger support VAXC" */
/*  *65    1-JUL-1994 12:11:25 TWF "Fix cancel" */
/*  *64   23-JUN-1994 10:39:48 TWF "Motif 1.2" */
/*  *63   22-FEB-1994 17:08:43 TWF "Change it back" */
/*  *62   22-FEB-1994 16:29:37 TWF "Use globalvalue for widgetrec" */
/*  *61   22-FEB-1994 15:18:20 TWF "remove NO_X_GBLS" */
/*  *60   26-JUL-1993 09:26:25 TWF "Fix put of empty descriptor" */
/*  *59    8-JUL-1993 12:35:42 TWF "Fix missing cast" */
/*  *58   28-JUN-1993 17:34:15 JAS "fix error handling" */
/*  *57   22-JUN-1993 13:03:13 JAS "fix tag modification" */
/*  *56   21-JUN-1993 13:00:52 TWF "Add notify" */
/*  *55   21-JUN-1993 12:47:04 TWF "Add notify" */
/*  *54   21-JUN-1993 12:44:23 TWF "Add notify" */
/*  *53   21-JUN-1993 12:17:02 TWF "Add notify" */
/*  *52   21-JUN-1993 12:13:55 TWF "Add notify" */
/*  *51   10-MAY-1993 14:17:01 TWF "Make it shareable" */
/*  *50   15-APR-1993 12:21:18 TWF "Fix WidgetClasses" */
/*  *49    7-APR-1993 13:50:01 TWF "Take out unions" */
/*  *48   18-MAR-1993 16:49:25 JAS "fix Dtype Task" */
/*  *47    4-MAR-1993 15:39:23 JAS "make it complile with vaxc" */
/*  *46    3-MAR-1993 16:37:44 JAS "use xmdsshr.h" */
/*  *45    2-MAR-1993 18:44:00 JAS "do decc" */
/*  *44    2-MAR-1993 12:09:36 JAS "port to decc" */
/*  *43   22-DEC-1992 12:18:01 JAS "fix getstate /setstate visa-vi  managed and loaded" */
/*  *42   21-DEC-1992 16:27:35 JAS "addign setstate and getstate" */
/*  *41    4-DEC-1992 15:24:22 JAS "Do load explictly instead of in changemanaged" */
/*  *40    2-DEC-1992 11:22:09 JAS "fix put" */
/*  *39   30-NOV-1992 15:50:02 JAS "fix on_off (change whole I/O - Xd scheme" */
/*  *38   24-NOV-1992 10:36:57 JAS "not quite" */
/*  *37   23-NOV-1992 16:28:19 JAS "Add a loaded flag to the widget" */
/*  *36   23-NOV-1992 16:27:24 JAS "delay the load until management" */
/*  *35   28-AUG-1992 14:37:40 JAS "if XmdsGetXd returns zero then put should return zero" */
/*  *34   12-AUG-1992 17:10:22 JAS "add more arguments" */
/*  *33   12-AUG-1992 14:11:10 JAS "Allow for more arguments" */
/*  *32   12-AUG-1992 09:21:30 JAS "complaint has to be parented by a widget that will exist" */
/*  *31   12-AUG-1992 09:05:47 JAS "fix modify of structure usages error" */
/*  *30   10-AUG-1992 15:28:26 TWF "Fix routine arguments" */
/*  *29    4-AUG-1992 12:10:56 TWF "fix build_routine" */
/*  *28    4-AUG-1992 10:50:07 TWF "Do not convert phase to code" */
/*  *27    3-AUG-1992 12:51:17 TWF "Do not convert phase to code" */
/*  *26   23-JUN-1992 15:51:49 JAS "Manage the expression's units" */
/*  *25    9-APR-1992 11:57:27 JAS "Fix default node behavior" */
/*  *24   18-MAR-1992 17:05:51 TWF "Make it use forms and resize" */
/*  *23   18-MAR-1992 08:07:23 TWF "Resize the child" */
/*  *22   30-JAN-1992 17:39:23 JAS "Make it a manager" */
/*  *21   28-JAN-1992 15:35:35 JAS "uid file name wrong" */
/*  *20   28-JAN-1992 15:14:04 JAS "add some more data types" */
/*  *19   22-JAN-1992 11:32:57 JAS "fix setbuttonstate and apply callback" */
/*  *18   21-JAN-1992 12:11:05 JAS "add XmdsXdBoxSetState" */
/*  *17   21-JAN-1992 11:39:12 JAS "add the on/off state" */
/*  *16   21-JAN-1992 11:11:35 JAS "compilation problem" */
/*  *15   21-JAN-1992 10:35:42 JAS "add XmdsGetDeviceNid if nid == -1" */
/*  *14   17-JAN-1992 16:31:25 JAS "typo" */
/*  *13   17-JAN-1992 16:27:59 JAS "typo" */
/*  *12   17-JAN-1992 16:23:54 JAS "apply needs to save the tags and on_off" */
/*  *11   17-JAN-1992 16:04:31 JAS "typo" */
/*  *10   17-JAN-1992 15:39:00 JAS "vasetvalues needs null" */
/*  *9    17-JAN-1992 15:22:21 JAS "Fix generic stuff" */
/*  *8    16-JAN-1992 18:52:40 JAS "typo" */
/*  *7    16-JAN-1992 18:37:01 JAS "Change Reset, Apply and Put" */
/*  *6    16-JAN-1992 16:48:39 TWF "Do not default autoput" */
/*  *5    16-JAN-1992 16:43:51 TWF "Do not default autoput" */
/*  *4    16-JAN-1992 16:42:39 TWF "Do not make with units if units blank" */
/*  *3    16-JAN-1992 12:33:54 JAS "fix put_on" */
/*  *2    16-JAN-1992 12:13:35 JAS "add apply" */
/*  *1    16-JAN-1992 10:59:53 JAS "xdbox widget" */
/*  CMS REPLACEMENT HISTORY, Element XMDSXDBOX.C */
/*------------------------------------------------------------------------------

		Name:   XmdsXdbox

		Type:   C function

		Author:	Josh Stillerman

		Date:   12-NOV-1991

		Purpose: This module impliments an  XmdsXdBox widget

------------------------------------------------------------------------------

	Call sequence:
		Widget XmdsCreateXdBox (Widget parent char *name, ArgList args, Cardinal arg_count);
		Boolean XmdsXdBoxApply(Widget w);
		void XmdsXdBoxGetXd(Widget w, struct dsc$descriptor *dsc);
		void XmdsXdBoxLoad(Widget w);
		void XmdsXdBoxPut(Widget w);
		void XmdsXdBoxReset(Widget w);
		void XmdsXdBoxSetDefaultNid(Widget w int def_nid);
		void XmdsXdBoxSetNid(Widget w, int nid);
		void XmdsXdBoxSetState(Widget w, Boolean state);
		Boolean XmdsXdBoxGetState(Widget w);
		void XmdsXdBoxSetXd(Widget w, struct dsc$descriptor *dsc);

------------------------------------------------------------------------------
   Copyright (c) 1991
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This expr cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------*/
#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <strroutines.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <ncidef.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/ToggleBG.h>
#include <Xm/TextF.h>
#include <Xmds/XmdsXdBoxP.h>
#include <Xmds/XmdsSupport.h>
#include <mds_stdarg.h>
extern char *DescToNull(struct descriptor_s *desc);
/*------------------------------------------------------------------------------

 External functions or symbols referenced:                                    */

#include <Xmds/XmdsExpr.h>
#include <Xmds/XmdsExprField.h>
#include <xmdsshr.h>

extern int TdiCompile();
/*------------------------------------------------------------------------------

 Functions and symbols defined:						      */

/*------------------------------------------------------------------------------

 Internal functions or symbols:                                              */

/*
 * Generic Class methods.
 */
static void ChangeManaged(Widget w);
static void ClassInitialize();
static void Destroy(Widget w);
static XtGeometryResult GeometryManager(Widget w, XtWidgetGeometry * desired,
					XtWidgetGeometry * allowed);
static void Initialize(Widget req, Widget new, ArgList args, Cardinal * argcount);
static Boolean SetValues();
static void Resize();
/*
 * Expression instances use:
 */
static void ExpressionCreate(XmdsXdBoxWidget w, ArgList args, Cardinal argcount);
static void ExpressionLoad(Widget w, struct descriptor_xd *xd);
static struct descriptor_xd *ExpressionUnload(Widget w);

static void ActionCreate(XmdsXdBoxWidget w, ArgList args, Cardinal argcount);
static void ActionLoad(Widget w, struct descriptor_xd *xd);
struct descriptor_xd *ActionUnload(Widget w);

static void AxisCreate(XmdsXdBoxWidget w, ArgList args, Cardinal argcount);
static void AxisLoad(Widget w, struct descriptor_xd *xd);
struct descriptor_xd *AxisUnload(Widget w);

static void DispatchCreate(XmdsXdBoxWidget w, ArgList args, Cardinal argcount);
static void DispatchLoad(Widget w, struct descriptor_xd *xd);
struct descriptor_xd *DispatchUnload(Widget w);

static void TaskCreate(XmdsXdBoxWidget w, ArgList args, Cardinal argcount);
static void TaskLoad(Widget w, struct descriptor_xd *xd);
struct descriptor_xd *TaskUnload(Widget w);

static void WindowCreate(XmdsXdBoxWidget w, ArgList args, Cardinal argcount);
static void WindowLoad(Widget w, struct descriptor_xd *xd);
struct descriptor_xd *WindowUnload(Widget w);
/*
 * Generic Widget Callbacks
 */
static void apply_button_proc(Widget w);
static void cancel_button_proc(Widget w);
static void ok_button_proc(Widget w);
static void reset_button_proc(Widget w);
/*
 * Type specific widget Callbacks.
 */
static void action_change_type_proc(Widget w, int *tag);
static void axis_change_type_proc(Widget w, int *tag);
static void dispatch_change_type_proc(Widget w, int *tag);
static void expression_change_type_proc(Widget w, int *tag);
static void task_change_type_proc(Widget w, int *tag);
static void window_change_type_proc(Widget w, int *tag);
/*
 * General support.
 */
static Boolean Apply(XmdsXdBoxWidget w);
static void CalculateNewSize(XmdsXdBoxWidget w, unsigned short *width, unsigned short *height);
static XmdsXdBoxWidget FindXdBoxWidget(Widget w);
static void GenericGet(XmdsXdBoxWidget w);
static void GenericLoad(XmdsXdBoxWidget w);
static void Load(Widget w, struct descriptor_xd *xd);
static Boolean NotEmptyXd(struct descriptor_xd *xd);
static Boolean Put(XmdsXdBoxWidget w);
static int SetDefault(Widget w);
static struct descriptor_xd *Unload(Widget w);
static void UpdateTags(Widget w, int nid, char *tags);
/*------------------------------------------------------------------------------

 Subroutines referenced:                                                      */

/*------------------------------------------------------------------------------

 Macros:                                                                      */

/*------------------------------------------------------------------------------

 Local variables:                                                             */

static struct descriptor_xd empty_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };

/*------------------------------------------------------------------------------

 Global variables:                                                            */

static XtResource resources[] = {
  {XmdsNapplyCallback, "XdBoxCallbacks", XtRCallback, sizeof(XtCallbackList),
   XtOffset(XmdsXdBoxWidget, xdbox.apply_callback),
   XtRImmediate, 0}
  ,
  {XmdsNcancelCallback, "XdBoxCallbacks", XtRCallback, sizeof(XtCallbackList),
   XtOffset(XmdsXdBoxWidget, xdbox.cancel_callback),
   XtRImmediate, 0}
  ,
  {XmdsNokCallback, "XdBoxCallbacks", XtRCallback, sizeof(XtCallbackList),
   XtOffset(XmdsXdBoxWidget, xdbox.ok_callback),
   XtRImmediate, 0}
  ,
  {XmdsNresetCallback, "XdBoxCallbacks", XtRCallback, sizeof(XtCallbackList),
   XtOffset(XmdsXdBoxWidget, xdbox.reset_callback),
   XtRImmediate, 0}
  ,
  {XmdsNdefaultNid, "Nid", XtRInt, sizeof(int), XtOffset(XmdsXdBoxWidget, xdbox.default_nid),
   XtRImmediate, (void *)-1},
  {XmdsNnid, "Nid", XtRInt, sizeof(int), XtOffset(XmdsXdBoxWidget, xdbox.nid), XtRImmediate, 0},
  {XmdsNnidOffset, "Nid", XtRInt, sizeof(int), XtOffset(XmdsXdBoxWidget, xdbox.nid_offset),
   XtRImmediate, 0},
  {XmdsNshowButtons, "ShowButtons", XtRBoolean, sizeof(Boolean),
   XtOffset(XmdsXdBoxWidget, xdbox.show_buttons), XtRImmediate, 0}
  ,
  {XmdsNusage, "Usage", XtRShort, sizeof(unsigned char), XtOffset(XmdsXdBoxWidget, xdbox.usage),
   XtRImmediate, 0},
  {XmdsNxd, "Xd", XtRPointer, sizeof(struct descriptor_xd *), XtOffset(XmdsXdBoxWidget, xdbox.xd),
   XtRImmediate, 0},
  {XmdsNautoDestroy, "AutoDestroy", XtRBoolean, sizeof(Boolean),
   XtOffset(XmdsXdBoxWidget, xdbox.auto_destroy), XtRImmediate, 0}
  ,
  {XmdsNautoUnmanage, "AutoUnmanage", XtRBoolean, sizeof(Boolean),
   XtOffset(XmdsXdBoxWidget, xdbox.auto_unmanage),
   XtRImmediate, (void *)1},
  {XmdsNputOnApply, "PutOnApply", XtRBoolean, sizeof(Boolean),
   XtOffset(XmdsXdBoxWidget, xdbox.put_on_apply),
   XtRImmediate, (void *)1},
  {XmdsNautoPut, "AutoPut", XtRBoolean, sizeof(Boolean), XtOffset(XmdsXdBoxWidget, xdbox.auto_put),
   XtRImmediate, 0}
};

XmdsXdBoxClassRec xmdsXdBoxClassRec = {
  {				/* core_class */
   /*   superclass:           */ (WidgetClass) & xmManagerClassRec,
   /*   class_name:           */ "XmdsXdBox",
   /*   widget_size:          */ sizeof(XmdsXdBoxWidgetRec),
   /*   class_initialize:     */ ClassInitialize,
   /*   class_part_initialize: */ NULL,
   /*   class_inited:         */ FALSE,
   /*   initialize:           */ Initialize,
   /*   initialize_hook:      */ NULL,
   /*   realize:              */ XtInheritRealize,
   /*   actions:              */ NULL,
   /*   num_actions:          */ 0,
   /*   resources:            */ resources,
   /*   num_resources:        */ XtNumber(resources),
   /*   xrm_class:            */ NULLQUARK,
   /*   compress_motion:      */ TRUE,
   /*   compress_exposure:    */ TRUE,
   /*   compress_enterleave:  */ TRUE,
   /*   visible_interest:     */ FALSE,
   /*   destroy:              */ Destroy,
   /*   resize:               */ Resize,
   /*   expose:               */ XtInheritExpose,
   /*   set_values:           */ SetValues,
   /*   set_values_hook:      */ NULL,
   /*   set_values_almost:    */ NULL,
   /*   get_values_hook:      */ NULL,
   /*   accept_focus:         */ NULL,
   /*   version:              */ XtVersionDontCheck,
   /*   callback_private:     */ NULL,
   /*   tm_table:             */ NULL,
   /*   query_geometry:       */ XtInheritQueryGeometry,
   /*   display_accelerator:  */ XtInheritDisplayAccelerator,
   /*   extension:            */ NULL
   }
  ,
  {				/* composite_class */
   /*   geometry_manager      */ GeometryManager,
   /*   change_managed:       */ ChangeManaged,
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
  {				/* XmManagerClassPart */
   /* translations */ NULL,
   /* syn_resources */ NULL,
   /* num_syn_resources */ 0,
   /* syn_constraint_reserces */ NULL,
   /* num_syn_constraint_reserces */ 0,
   /* parent_process */ XmInheritParentProcess,
   /* extension */ NULL
   }
  ,
  {				/*   XmdsXdBox_class */
   /*   drm             */ 0,
   /*   extension        */ 0
   }
};

#ifdef STAND_ALONE
globaldef WidgetClass xmdsXdBoxWidgetClass = &xmdsXdBoxClassRec;
#endif

WidgetClass xmdsXdBoxWidgetClass;

/*------------------------------------------------------------------------------

 Executable:                                                                  */

/****************************************************
  Routine to create a  XdBox Widget.  Use
  resources xd, or nid/nid_offset and buttonCallbacks to setup
  the initial widget.
****************************************************/
EXPORT Widget XmdsCreateXdBox(Widget parent, char *name, ArgList args, Cardinal argcount)
{
  return XtCreateWidget(name, (WidgetClass) & xmdsXdBoxClassRec, parent, args, argcount);
}

EXPORT Boolean XmdsXdBoxApply(Widget w)
{
  XmdsXdBoxWidget xdbw = (XmdsXdBoxWidget) w;
  return xdbw->xdbox.put_on_apply ? XmdsXdBoxPut(w) : 1;
}

EXPORT struct descriptor *XmdsXdBoxGetXd(Widget w)
{
  XmdsXdBoxWidget xdbw = (XmdsXdBoxWidget) w;
  return (struct descriptor *)Unload(xdbw->xdbox.specific_dlog);
}

EXPORT Boolean XmdsXdBoxPut(Widget w)
{
  XmdsXdBoxWidget xdbw = (XmdsXdBoxWidget) w;
  return Put(xdbw);
}

EXPORT void XmdsXdBoxReset(Widget w)
{
  XmdsXdBoxWidget xdbw = (XmdsXdBoxWidget) w;
  if (xdbw->xdbox.nid) {
    int nid = xdbw->xdbox.nid + xdbw->xdbox.nid_offset;
    if (xdbw->xdbox.xd) {
      MdsFree1Dx(xdbw->xdbox.xd, 0);
      XtFree((char *)xdbw->xdbox.xd);
    }
    xdbw->xdbox.xd = (struct descriptor_xd *)TdiGet(nid);
    GenericGet(xdbw);
  }
  if (xdbw->xdbox.loaded) {
    GenericLoad(xdbw);
    Load(xdbw->xdbox.specific_dlog, xdbw->xdbox.xd);
  }
}

EXPORT void XmdsXdBoxSetState(Widget w, Boolean state)
{
  XmdsXdBoxWidget xdbw = (XmdsXdBoxWidget) w;
  XmToggleButtonGadgetSetState(XtNameToWidget(xdbw->xdbox.xdb_dlog, "generic_box.on_off_toggle"),
			       state, 0);
  xdbw->xdbox.on_off = state;
}

EXPORT Boolean XmdsXdBoxGetState(Widget w)
{
  Boolean state;
  XmdsXdBoxWidget xdbw = (XmdsXdBoxWidget) w;
  if (!xdbw->xdbox.loaded)
    state = xdbw->xdbox.on_off;
  else
    state =
	XmToggleButtonGadgetGetState(XtNameToWidget
				     (xdbw->xdbox.xdb_dlog, "generic_box.on_off_toggle"));
  return state;
}

void XmdsXdBoxSetNid(Widget w, int nid)
{
  XmdsXdBoxWidget xdbw = (XmdsXdBoxWidget) w;
  if (xdbw->xdbox.xd)
    MdsFree1Dx(xdbw->xdbox.xd, 0);
  else {
    xdbw->xdbox.xd = (struct descriptor_xd *)XtMalloc(sizeof(struct descriptor_xd));
    *xdbw->xdbox.xd = empty_xd;
  }
  if (nid == -1)
    xdbw->xdbox.nid = XmdsGetDeviceNid();
  else
    xdbw->xdbox.nid = nid;
  TreeGetRecord(xdbw->xdbox.nid, xdbw->xdbox.xd);
  GenericGet(xdbw);
  if (xdbw->xdbox.loaded) {
    Load(xdbw->xdbox.specific_dlog, xdbw->xdbox.xd);
    GenericLoad(xdbw);
  }
}

EXPORT void XmdsXdBoxSetDefaultNid(Widget w, int def_nid)
{
  XmdsXdBoxWidget xdbw = (XmdsXdBoxWidget) w;
  xdbw->xdbox.default_nid = def_nid;
  SetDefault(w);
}

EXPORT void XmdsXdBoxSetXd(Widget w, struct descriptor *dsc)
{
  struct descriptor_xd *xd = (struct descriptor_xd *)dsc;
  XmdsXdBoxWidget xdbw = (XmdsXdBoxWidget) w;
  if (xdbw->xdbox.xd)
    MdsFree1Dx(xdbw->xdbox.xd, 0);
  else {
    xdbw->xdbox.xd = (struct descriptor_xd *)XtMalloc(sizeof(struct descriptor_xd));
    *xdbw->xdbox.xd = empty_xd;
  }
  MdsCopyDxXd((struct descriptor *)xd, xdbw->xdbox.xd);
  if (xdbw->xdbox.loaded)
    Load(xdbw->xdbox.specific_dlog, xdbw->xdbox.xd);
}

EXPORT void XmdsXdBoxLoad(Widget w)
{
  XmdsXdBoxWidget xdbw = (XmdsXdBoxWidget) w;
  if (!xdbw->xdbox.loaded) {
    xdbw->xdbox.loaded = 1;
    Load(xdbw->xdbox.specific_dlog, xdbw->xdbox.xd);
    GenericLoad(xdbw);
  }
}

static void ChangeManaged(Widget w)
{
  XmdsXdBoxWidget xdbw = (XmdsXdBoxWidget) w;
  XtWidgetGeometry request;
  XtGeometryResult result;

  CalculateNewSize(xdbw, &request.width, &request.height);

  if (request.width != XtWidth(xdbw) || request.height != XtHeight(xdbw)) {
    request.request_mode = CWWidth | CWHeight;
    do {
      result = XtMakeGeometryRequest(w, &request, &request);
    } while (result == XtGeometryAlmost);
  };
}

/**********************************
  ClassInitialize
    - Opens the DRM Hierarchy and
**********************************/
static void ClassInitialize()
{
  static MrmRegisterArg routines[] = {
    {"apply_button_proc", (char *)apply_button_proc},
    {"cancel_button_proc", (char *)cancel_button_proc},
    {"ok_button_proc", (char *)ok_button_proc},
    {"reset_button_proc", (char *)reset_button_proc},
    {"action_change_type_proc", (char *)action_change_type_proc},
    {"axis_change_type_proc", (char *)axis_change_type_proc},
    {"dispatch_change_type_proc", (char *)dispatch_change_type_proc},
    {"expression_change_type_proc", (char *)expression_change_type_proc},
    {"task_change_type_proc", (char *)task_change_type_proc},
    {"window_change_type_proc", (char *)window_change_type_proc},
  };

  static char *hierarchy_name[] = { "XmdsXdBox.uid" };
  MrmOpenHierarchy(1, hierarchy_name, 0, &xmdsXdBoxClassRec.xdbox_class.drm);
  MrmRegisterNamesInHierarchy(xmdsXdBoxClassRec.xdbox_class.drm, routines, XtNumber(routines));
}

static void Destroy(Widget w)
{
  XmdsXdBoxWidget xdbw = (XmdsXdBoxWidget) w;
  Widget par = XtParent(w);
  if (XtIsShell(par))
    XtDestroyWidget(par);
  if (xdbw->xdbox.xd) {
    MdsFree1Dx(xdbw->xdbox.xd, 0);
    XtFree((char *)xdbw->xdbox.xd);
    xdbw->xdbox.xd = 0;
  }
  if (xdbw->xdbox.tag_list)
    XtFree((char *)xdbw->xdbox.tag_list);
  if (xdbw->xdbox.path)
    XtFree((char *)xdbw->xdbox.path);
}

static void Resize(XmdsXdBoxWidget w)
{
  if (w->xdbox.xdb_dlog)
    XtResizeWidget(w->xdbox.xdb_dlog, XtWidth(w), XtHeight(w), XtBorderWidth(w->xdbox.xdb_dlog));
}

static XtGeometryResult GeometryManager(Widget w, XtWidgetGeometry * req, XtWidgetGeometry * ret)
{
  XtGeometryResult res = XtMakeGeometryRequest(XtParent(w), req, ret);
  if (res == XtGeometryYes) {
    if (req->request_mode & CWX)
      w->core.x = XtParent(w)->core.x;
    if (req->request_mode & CWY)
      w->core.y = XtParent(w)->core.y;
    if (req->request_mode & CWWidth)
      w->core.width = XtParent(w)->core.width;
    if (req->request_mode & CWHeight)
      w->core.height = XtParent(w)->core.height;
    if (req->request_mode & CWWidth)
      w->core.width = XtParent(w)->core.width;
    if (req->request_mode & CWBorderWidth)
      w->core.border_width = XtParent(w)->core.border_width;
  } else if (res == XtGeometryAlmost)
    res = GeometryManager(w, ret, ret);
  return res;
}

static void Initialize(Widget req, Widget new, ArgList args, Cardinal * argcount)
{
  XmdsXdBoxWidget w = (XmdsXdBoxWidget) new;
  MrmType class;
  int nid = 0;
  int status = 1;

  w->xdbox.tag_list = 0;
  w->xdbox.path = 0;
  if (w->xdbox.nid) {
    if (w->xdbox.nid == -1)
      w->xdbox.nid = XmdsGetDeviceNid();
    nid = w->xdbox.nid + w->xdbox.nid_offset;
    w->xdbox.xd = (struct descriptor_xd *)TdiGet(nid);
  } else {
    XmdsXdBoxWidget req_xdb = (XmdsXdBoxWidget) req;
    w->xdbox.xd = (struct descriptor_xd *)XtMalloc(sizeof(struct descriptor_xd));
    *w->xdbox.xd = empty_xd;
    MdsCopyDxXd((struct descriptor *)req_xdb->xdbox.xd, w->xdbox.xd);
  }
  MrmFetchWidgetOverride(xmdsXdBoxClassRec.xdbox_class.drm, "xd_box",
			 (Widget) w, "xd_box", args, *argcount, &w->xdbox.xdb_dlog, &class);
  if (nid == 0) {
    XtVaSetValues(XtNameToWidget(w->xdbox.xdb_dlog, "detail_box"), XmNtopAttachment, XmATTACH_FORM,
		  NULL);
    XtUnmanageChild(XtNameToWidget(w->xdbox.xdb_dlog, "generic_box"));
  } else {
    GenericGet(w);
  }
  XtManageChild(w->xdbox.xdb_dlog);
  if (w->xdbox.usage == 0) {
    if (w->xdbox.nid) {
      NCI_ITM itms[] = { {sizeof(w->xdbox.usage), NciUSAGE, 0, 0}
      , {0, 0, 0, 0}
      };
      int nid = w->xdbox.nid + w->xdbox.nid_offset;
      int status;
      itms[0].pointer = (unsigned char *)&w->xdbox.usage;
      status = TreeGetNci(nid, itms);
      if ((status & 1) == 0)
	w->xdbox.usage = 0;
    } else {
      w->xdbox.usage = 0;
    }
  }
  switch (w->xdbox.usage) {
  case TreeUSAGE_WINDOW:
    WindowCreate(w, args, *argcount);
    break;
  case TreeUSAGE_ACTION:
    ActionCreate(w, args, *argcount);
    break;
  case TreeUSAGE_AXIS:
    AxisCreate(w, args, *argcount);
    break;
  case TreeUSAGE_DISPATCH:
    DispatchCreate(w, args, *argcount);
    break;
  case TreeUSAGE_TASK:
    TaskCreate(w, args, *argcount);
    break;
  case TreeUSAGE_ANY:
  case TreeUSAGE_DEVICE:
  case TreeUSAGE_NUMERIC:
  case TreeUSAGE_SIGNAL:
  case TreeUSAGE_TEXT:
  case TreeUSAGE_COMPOUND_DATA:
    ExpressionCreate(w, args, *argcount);
    break;
  case TreeUSAGE_STRUCTURE:
  case TreeUSAGE_SUBTREE:
    XmdsComplain(XtParent(XtParent(req)), "Can not modify nodes of usage STRUCURE");
    XtDestroyWidget(new);
    status = 0;
    break;
  default:
    XmdsComplain(XtParent(XtParent(req)), "Unknown usage can not modify");
    XtDestroyWidget(new);
    status = 0;
    break;
  }
  if (status) {
    w->xdbox.loaded = 0;
    Load(w->xdbox.specific_dlog, w->xdbox.xd);
    w->core.width = XtWidth(w->xdbox.xdb_dlog);
    w->core.height = XtHeight(w->xdbox.xdb_dlog);
  }
}

static Boolean SetValues(Widget old, Widget req, Widget new)
{
  XmdsXdBoxWidget old_xd_w = (XmdsXdBoxWidget) old;
  XmdsXdBoxWidget req_xd_w = (XmdsXdBoxWidget) req;
  XmdsXdBoxWidget new_xd_w = (XmdsXdBoxWidget) new;
  if ((req_xd_w->xdbox.nid != old_xd_w->xdbox.nid)
      || (req_xd_w->xdbox.nid_offset != old_xd_w->xdbox.nid_offset)) {
    if (req_xd_w->xdbox.nid == -1)
      new_xd_w->xdbox.nid = XmdsGetDeviceNid();
    XmdsXdBoxSetNid(new, new_xd_w->xdbox.nid + new_xd_w->xdbox.nid_offset);
  } else if (req_xd_w->xdbox.xd != old_xd_w->xdbox.xd) {
    new_xd_w->xdbox.xd = 0;
    XmdsXdBoxSetXd(new, (struct descriptor *)req_xd_w->xdbox.xd);
  }
  return 0;
}

static void ActionCreate(XmdsXdBoxWidget w, ArgList args, Cardinal argcount)
{
  MrmType class;
  XmdsXdUserPart *user_part = (XmdsXdUserPart *) XtMalloc(sizeof(XmdsXdUserPart));
  Arg dlog_args[] = { {XmNuserData, 0}
  ,
  {XmNtopAttachment, XmATTACH_FORM}
  ,
  {XmNleftAttachment, XmATTACH_FORM}
  ,
  {XmNrightAttachment, XmATTACH_FORM}
  ,
  {XmNbottomAttachment, XmATTACH_FORM}
  };
  Arg dispatch_args[] = { {XmNtopAttachment, XmATTACH_FORM}
  ,
  {XmNleftAttachment, XmATTACH_FORM}
  ,
  {XmNrightAttachment, XmATTACH_POSITION}
  ,
  {XmNrightPosition, 50}
  ,
  {XmNbottomAttachment, XmATTACH_FORM}
  };
  Arg task_args[] = { {XmNtopAttachment, XmATTACH_FORM}
  ,
  {XmNleftAttachment, XmATTACH_POSITION}
  ,
  {XmNleftPosition, 50}
  ,
  {XmNrightAttachment, XmATTACH_FORM}
  ,
  {XmNbottomAttachment, XmATTACH_FORM}
  };
  Widget task;
  Widget dispatch;
  dlog_args[0].value = (XtArgVal) user_part;
  user_part->load_dlog_proc = ActionLoad;
  user_part->unload_dlog_proc = ActionUnload;

  MrmFetchWidgetOverride(xmdsXdBoxClassRec.xdbox_class.drm, "action_dlog",
			 XtNameToWidget(w->xdbox.xdb_dlog, "detail_box"), "action_dlog", dlog_args,
			 XtNumber(dlog_args), &w->xdbox.specific_dlog, &class);
  MrmFetchWidgetOverride(xmdsXdBoxClassRec.xdbox_class.drm, "dispatch_dlog",
			 XtNameToWidget(w->xdbox.specific_dlog, "action_box"), "dispatch_dlog",
			 dispatch_args, XtNumber(dispatch_args), &dispatch, &class);
  MrmFetchWidgetOverride(xmdsXdBoxClassRec.xdbox_class.drm, "task_dlog",
			 XtNameToWidget(w->xdbox.specific_dlog, "action_box"), "task_dlog",
			 task_args, XtNumber(task_args), &task, &class);
  XtManageChild(dispatch);
  XtManageChild(task);
  XtManageChild(w->xdbox.specific_dlog);
}

static void ActionLoad(Widget w, struct descriptor_xd *xd)
{
  XmdsXdBoxWidget xdbw = FindXdBoxWidget(w);
  struct descriptor *ptr = (struct descriptor *)xd;
  Widget menu_widget = XtNameToWidget(w, "action_menu");
  Widget expr_widget = XtNameToWidget(w, "action_expr");
  Widget action_widget = XtNameToWidget(w, "action_box");
  Widget action_notify = XtNameToWidget(w, "*action_notify");
  Widget action_notify_label = XtNameToWidget(w, "*action_notify_label");

  for (ptr = (struct descriptor *)xd;
       ptr && ptr->dtype == DTYPE_DSC; ptr = (struct descriptor *)ptr->pointer) ;
  if (ptr) {
    xd = (struct descriptor_xd *)ptr;
    if (NotEmptyXd(xd)) {
      if (xd->dtype == DTYPE_ACTION) {
	Widget dispatch_w = XtNameToWidget(action_widget, "dispatch_dlog");
	Widget task_w = XtNameToWidget(action_widget, "task_dlog");
	struct descriptor_action *a_dsc = (struct descriptor_action *)xd;
	TaskLoad(task_w, (struct descriptor_xd *)a_dsc->task);
	DispatchLoad(dispatch_w, (struct descriptor_xd *)a_dsc->dispatch);
	if (xdbw->xdbox.loaded) {
	  static EMPTYXD(xd);
	  if (a_dsc->ndesc >= 3)
	    XmdsExprSetXd(action_notify, (struct descriptor *)a_dsc->errorlogs);
	  else
	    XmdsExprSetXd(action_notify, (struct descriptor *)&xd);
	}
	XtManageChild(action_widget);
	XtManageChild(action_notify);
	XtManageChild(action_notify_label);
	XtUnmanageChild(expr_widget);
	XmdsSetOptionIdx(menu_widget, 1);
      } else {
	if (xdbw->xdbox.loaded)
	  XmdsExprSetXd(expr_widget, (struct descriptor *)xd);
	XtUnmanageChild(action_widget);
	XtUnmanageChild(action_notify);
	XtUnmanageChild(action_notify_label);
	XtManageChild(expr_widget);
	XmdsSetOptionIdx(menu_widget, 2);
      }
    } else {
      XmdsSetOptionIdx(menu_widget, 0);
      XtUnmanageChild(expr_widget);
      XtUnmanageChild(action_widget);
      XtUnmanageChild(action_notify);
      XtUnmanageChild(action_notify_label);
    }
  } else {
    XmdsSetOptionIdx(menu_widget, 0);
    XtUnmanageChild(expr_widget);
    XtUnmanageChild(action_widget);
    XtUnmanageChild(action_notify);
    XtUnmanageChild(action_notify_label);
  }
}

struct descriptor_xd *ActionUnload(Widget w)
{
  struct descriptor_xd *ans = (struct descriptor_xd *)XtMalloc(sizeof(struct descriptor_xd));
  struct descriptor_xd *data = 0;
  Widget action_expr = XtNameToWidget(w, "action_expr");
  Widget action_box = XtNameToWidget(w, "action_box");

  *ans = empty_xd;

  if (XtIsManaged(action_expr) || XtIsManaged(action_box)) {
    if (XtIsManaged(action_expr)) {
      data = (struct descriptor_xd *)XmdsExprGetXd(action_expr);
    } else if (XtIsManaged(action_box)) {
      static DESCRIPTOR(action, "BUILD_ACTION($, $, $)");
      Widget dispatch_w = XtNameToWidget(action_box, "dispatch_dlog");
      Widget task_w = XtNameToWidget(action_box, "task_dlog");
      Widget notify_w = XtNameToWidget(w, "*action_notify");
      struct descriptor_xd *dispatch = DispatchUnload(dispatch_w);
      struct descriptor_xd *task = TaskUnload(task_w);
      struct descriptor_xd *notify = (struct descriptor_xd *)XmdsExprGetXd(notify_w);
      data = (struct descriptor_xd *)XtMalloc(sizeof(struct descriptor_xd));
      *data = empty_xd;
      TdiCompile(&action, dispatch, task, notify, data MDS_END_ARG);
    }
    ans = data;
  }
  return ans;
}

/*
 * Routines for Axis type.
 */
static void AxisCreate(XmdsXdBoxWidget w, ArgList args, Cardinal argcount)
{
  MrmType class;
  XmdsXdUserPart *user_part = (XmdsXdUserPart *) XtMalloc(sizeof(XmdsXdUserPart));
  Arg dlog_args[] = { {XmNuserData, 0}
  ,
  {XmNtopAttachment, XmATTACH_FORM}
  ,
  {XmNleftAttachment, XmATTACH_FORM}
  ,
  {XmNrightAttachment, XmATTACH_FORM}
  ,
  {XmNbottomAttachment, XmATTACH_FORM}
  };
  dlog_args[0].value = (XtArgVal) user_part;
  user_part->load_dlog_proc = AxisLoad;
  user_part->unload_dlog_proc = AxisUnload;

  MrmFetchWidgetOverride(xmdsXdBoxClassRec.xdbox_class.drm, "axis_dlog",
			 XtNameToWidget(w->xdbox.xdb_dlog, "detail_box"), "axis_dlog", dlog_args,
			 XtNumber(dlog_args), &w->xdbox.specific_dlog, &class);
  XtManageChild(w->xdbox.specific_dlog);
}

static void AxisLoad(Widget w, struct descriptor_xd *xd)
{
  XmdsXdBoxWidget xdbw = FindXdBoxWidget(w);
  struct descriptor *ptr = (struct descriptor *)xd;
  Widget menu_widget = XtNameToWidget(w, "axis_menu");
  Widget expr_widget = XtNameToWidget(w, "axis_expr");
  Widget axis_widget = XtNameToWidget(w, "axis_box");
  Widget units_widget = XtNameToWidget(w, "axis_units");
  Widget units_label_widget = XtNameToWidget(w, "axis_units_label");

  for (ptr = (struct descriptor *)xd;
       ptr && ptr->dtype == DTYPE_DSC; ptr = (struct descriptor *)ptr->pointer) ;
  if (ptr) {
    if (ptr->dtype == DTYPE_WITH_UNITS) {
      struct descriptor_with_units *w_u = (struct descriptor_with_units *)ptr;
      XmdsExprSetXd(units_widget, w_u->units);
      xd = (struct descriptor_xd *)w_u->data;
    } else {
      XmdsExprSetXd(units_widget, (struct descriptor *)&empty_xd);
      xd = (struct descriptor_xd *)ptr;
    }
    if (NotEmptyXd(xd)) {
      XtManageChild(units_label_widget);
      XtManageChild(units_widget);
      if (xd->dtype == DTYPE_RANGE) {
	Widget start_w = XtNameToWidget(axis_widget, "axis_start");
	Widget end_w = XtNameToWidget(axis_widget, "axis_end");
	Widget inc_w = XtNameToWidget(axis_widget, "axis_inc");
	struct descriptor_range *a_dsc = (struct descriptor_range *)xd;
	if (xdbw->xdbox.loaded) {
	  XmdsExprFieldSetXd(start_w, a_dsc->begin);
	  XmdsExprFieldSetXd(end_w, a_dsc->ending);
	  XmdsExprFieldSetXd(inc_w, a_dsc->deltaval);
	}
	XtManageChild(axis_widget);
	XtUnmanageChild(expr_widget);
	XmdsSetOptionIdx(menu_widget, 1);
      } else {
	if (xdbw->xdbox.loaded)
	  XmdsExprSetXd(expr_widget, (struct descriptor *)xd);
	XtUnmanageChild(axis_widget);
	XtManageChild(expr_widget);
	XmdsSetOptionIdx(menu_widget, 2);
	XtManageChild(units_label_widget);
	XtManageChild(units_widget);
      }
    } else {
      XmdsSetOptionIdx(menu_widget, 0);
      XtUnmanageChild(units_label_widget);
      XtUnmanageChild(units_widget);
      XtUnmanageChild(expr_widget);
      XtUnmanageChild(axis_widget);
    }
  } else {
    XmdsSetOptionIdx(menu_widget, 0);
    XtUnmanageChild(expr_widget);
    XtUnmanageChild(axis_widget);
    XtUnmanageChild(units_label_widget);
    XtUnmanageChild(units_widget);
  }
}

struct descriptor_xd *AxisUnload(Widget w)
{
  struct descriptor_xd *ans = (struct descriptor_xd *)XtMalloc(sizeof(struct descriptor_xd));
  struct descriptor_xd *data = 0;
  struct descriptor_xd *units = 0;
  static DESCRIPTOR(w_units, "BUILD_WITH_UNITS($,$)");
  Widget units_widget = XtNameToWidget(w, "axis_units");
  Widget expr_widget = XtNameToWidget(w, "axis_expr");
  Widget axis_widget = XtNameToWidget(w, "axis_box");

  *ans = empty_xd;

  if (XtIsManaged(expr_widget) || XtIsManaged(axis_widget)) {
    units = (struct descriptor_xd *)XmdsExprGetXd(units_widget);
    if (XtIsManaged(expr_widget)) {
      data = (struct descriptor_xd *)XmdsExprGetXd(expr_widget);
    } else if (XtIsManaged(axis_widget)) {
      static DESCRIPTOR(range, "$ : $ : $");
      Widget start_w = XtNameToWidget(axis_widget, "axis_start");
      Widget end_w = XtNameToWidget(axis_widget, "axis_end");
      Widget inc_w = XtNameToWidget(axis_widget, "axis_inc");
      struct descriptor_xd *start = (struct descriptor_xd *)XmdsExprFieldGetXd(start_w);
      struct descriptor_xd *end = (struct descriptor_xd *)XmdsExprFieldGetXd(end_w);
      struct descriptor_xd *inc = (struct descriptor_xd *)XmdsExprFieldGetXd(inc_w);
      data = (struct descriptor_xd *)XtMalloc(sizeof(struct descriptor_xd));
      *data = empty_xd;
      TdiCompile(&range, start, end, inc, data MDS_END_ARG);
    }
    if (units && units->l_length && (units->pointer->dtype != DTYPE_T || units->pointer->length)) {
      ans = (struct descriptor_xd *)XtMalloc(sizeof(struct descriptor_xd));
      *ans = empty_xd;
      TdiCompile(&w_units, data, units, ans MDS_END_ARG);
      MdsFree1Dx(units, 0);
      MdsFree1Dx(data, 0);
      XtFree((char *)units);
      XtFree((char *)data);
    } else
      ans = data;
  }
  return ans;
}

/*
 * Routines for Dispatch type.
 */
static void DispatchCreate(XmdsXdBoxWidget w, ArgList args, Cardinal argcount)
{
  MrmType class;
  XmdsXdUserPart *user_part = (XmdsXdUserPart *) XtMalloc(sizeof(XmdsXdUserPart));
  Arg dlog_args[] = { {XmNuserData, 0}
  ,
  {XmNtopAttachment, XmATTACH_FORM}
  ,
  {XmNleftAttachment, XmATTACH_FORM}
  ,
  {XmNrightAttachment, XmATTACH_FORM}
  ,
  {XmNbottomAttachment, XmATTACH_FORM}
  };
  dlog_args[0].value = (XtArgVal) user_part;
  user_part->load_dlog_proc = DispatchLoad;
  user_part->unload_dlog_proc = DispatchUnload;

  MrmFetchWidgetOverride(xmdsXdBoxClassRec.xdbox_class.drm, "dispatch_dlog",
			 XtNameToWidget(w->xdbox.xdb_dlog, "detail_box"), "dispatch_dlog",
			 dlog_args, XtNumber(dlog_args), &w->xdbox.specific_dlog, &class);
  XtManageChild(w->xdbox.specific_dlog);
}

static void DispatchLoad(Widget w, struct descriptor_xd *xd)
{
  XmdsXdBoxWidget xdbw = FindXdBoxWidget(w);
  struct descriptor_xd *ptr = xd;
  Widget menu_widget = XtNameToWidget(w, "dispatch_menu");
  Widget expr_widget = XtNameToWidget(w, "dispatch_expr");
  Widget dispatch_widget = XtNameToWidget(w, "dispatch_box");

  for (ptr = xd; ptr && ptr->dtype == DTYPE_DSC; ptr = (struct descriptor_xd *)ptr->pointer) ;
  if (ptr) {
    xd = ptr;
    if (NotEmptyXd(xd)) {
      if (xd->dtype == DTYPE_DISPATCH) {
	Widget ident_w = XtNameToWidget(dispatch_widget, "dispatch_ident");
	Widget phase_w = XtNameToWidget(dispatch_widget, "dispatch_phase");
	Widget when_w = XtNameToWidget(dispatch_widget, "dispatch_when");
	Widget completion_w = XtNameToWidget(dispatch_widget, "dispatch_completion");
	struct descriptor_dispatch *w_dsc = (struct descriptor_dispatch *)xd;
	if (xdbw->xdbox.loaded) {
	  XmdsExprFieldSetXd(ident_w, w_dsc->ident);
	  XmdsExprFieldSetXd(when_w, w_dsc->when);
	  XmdsExprFieldSetXd(completion_w, w_dsc->completion);
	  XmdsExprFieldSetXd(phase_w, w_dsc->phase);
	}
	XtManageChild(dispatch_widget);
	XtUnmanageChild(expr_widget);
	XmdsSetOptionIdx(menu_widget, 1);
      } else {
	if (xdbw->xdbox.loaded)
	  XmdsExprSetXd(expr_widget, (struct descriptor *)xd);
	XtUnmanageChild(dispatch_widget);
	XtManageChild(expr_widget);
	XmdsSetOptionIdx(menu_widget, 2);
      }
    } else {
      XmdsSetOptionIdx(menu_widget, 0);
      XtUnmanageChild(expr_widget);
      XtUnmanageChild(dispatch_widget);
    }
  } else {
    XmdsSetOptionIdx(menu_widget, 0);
    XtUnmanageChild(expr_widget);
    XtUnmanageChild(dispatch_widget);
  }
}

EXPORT struct descriptor_xd *DispatchUnload(Widget w)
{
  struct descriptor_xd *ans = (struct descriptor_xd *)XtMalloc(sizeof(struct descriptor_xd));
  struct descriptor_xd *data = 0;
  Widget dispatch_expr = XtNameToWidget(w, "dispatch_expr");
  Widget dispatch_box = XtNameToWidget(w, "dispatch_box");

  *ans = empty_xd;

  if (XtIsManaged(dispatch_expr) || XtIsManaged(dispatch_box)) {
    if (XtIsManaged(dispatch_expr)) {
      data = (struct descriptor_xd *)XmdsExprGetXd(dispatch_expr);
    } else if (XtIsManaged(dispatch_box)) {
      static int dispatch_type = 2;
      static struct descriptor_s type = { sizeof(int), DTYPE_L, CLASS_S, (char *)&dispatch_type };
      static DESCRIPTOR(dispatch, "BUILD_DISPATCH($, $, $, $, $)");
      Widget ident_w = XtNameToWidget(dispatch_box, "dispatch_ident");
      Widget phase_w = XtNameToWidget(dispatch_box, "dispatch_phase");
      Widget when_w = XtNameToWidget(dispatch_box, "dispatch_when");
      Widget completion_w = XtNameToWidget(dispatch_box, "dispatch_completion");
      struct descriptor_xd *ident = (struct descriptor_xd *)XmdsExprFieldGetXd(ident_w);
      struct descriptor_xd *phase = (struct descriptor_xd *)XmdsExprFieldGetXd(phase_w);
      struct descriptor_xd *when = (struct descriptor_xd *)XmdsExprFieldGetXd(when_w);
      struct descriptor_xd *completion = (struct descriptor_xd *)XmdsExprFieldGetXd(completion_w);
      data = (struct descriptor_xd *)XtMalloc(sizeof(struct descriptor_xd));
      *data = empty_xd;
      TdiCompile(&dispatch, &type, ident, phase, when, completion, data MDS_END_ARG);
    }
    ans = data;
  }
  return ans;
}

static void ExpressionCreate(XmdsXdBoxWidget w, ArgList args, Cardinal argcount)
{
  MrmType class;
  XmdsXdUserPart *user_part = (XmdsXdUserPart *) XtMalloc(sizeof(XmdsXdUserPart));
  Arg dlog_args[] = { {XmNuserData, 0}
  ,
  {XmNtopAttachment, XmATTACH_FORM}
  ,
  {XmNleftAttachment, XmATTACH_FORM}
  ,
  {XmNrightAttachment, XmATTACH_FORM}
  ,
  {XmNbottomAttachment, XmATTACH_FORM}
  };
  dlog_args[0].value = (XtArgVal) user_part;
  user_part->load_dlog_proc = ExpressionLoad;
  user_part->unload_dlog_proc = ExpressionUnload;

  MrmFetchWidgetOverride(xmdsXdBoxClassRec.xdbox_class.drm, "expression_dlog",
			 XtNameToWidget(w->xdbox.xdb_dlog, "detail_box"), "expression_dlog",
			 dlog_args, XtNumber(dlog_args), &w->xdbox.specific_dlog, &class);
  XtManageChild(w->xdbox.specific_dlog);
}

static void ExpressionLoad(Widget w, struct descriptor_xd *xd)
{
  XmdsXdBoxWidget xdbw = FindXdBoxWidget(w);
  struct descriptor_xd *ptr = xd;
  Widget menu_widget = XtNameToWidget(w, "expression_menu");
  Widget units_widget = XtNameToWidget(w, "expression_units");
  Widget units_label_widget = XtNameToWidget(w, "expression_units_label");
  Widget expr_widget = XtNameToWidget(w, "expression_expr");

  for (ptr = xd; ptr && ptr->dtype == DTYPE_DSC; ptr = (struct descriptor_xd *)ptr->pointer) ;
  if (ptr) {
    if (xdbw->xdbox.loaded) {
      if (ptr->dtype == DTYPE_WITH_UNITS) {
	struct descriptor_with_units *w_u = (struct descriptor_with_units *)ptr;
	XmdsExprSetXd(units_widget, w_u->units);
	XmdsExprSetXd(expr_widget, w_u->data);
      } else {
	XmdsExprSetXd(units_widget, (struct descriptor *)&empty_xd);
	XmdsExprSetXd(expr_widget, (struct descriptor *)xd);
      }
    }
    XmdsSetOptionIdx(menu_widget, 1);
    XtManageChild(expr_widget);
    XtManageChild(units_widget);
    XtManageChild(units_label_widget);
  } else {
    XmdsSetOptionIdx(menu_widget, 0);
    XtUnmanageChild(expr_widget);
    XtUnmanageChild(units_widget);
    XtUnmanageChild(units_label_widget);
  }
}

static struct descriptor_xd *ExpressionUnload(Widget w)
{
  struct descriptor_xd *ans = 0;
  struct descriptor_xd *data = 0;
  struct descriptor_xd *units = 0;
  static DESCRIPTOR(w_units, "BUILD_WITH_UNITS($,$)");
  Widget units_widget = XtNameToWidget(w, "expression_units");
  Widget expr_widget = XtNameToWidget(w, "expression_expr");

  if (XtIsManaged(w)) {
    if (XtIsManaged(units_widget))
      units = (struct descriptor_xd *)XmdsExprGetXd(units_widget);
    if (XtIsManaged(expr_widget))
      data = (struct descriptor_xd *)XmdsExprGetXd(expr_widget);
    else {
      data = (struct descriptor_xd *)XtMalloc(sizeof(struct descriptor_xd));
      memcpy(data, &empty_xd, sizeof(struct descriptor_xd));
    }
    if (data &&
	units && units->l_length && (units->pointer->dtype != DTYPE_T || units->pointer->length)) {
      int status;
      ans = (struct descriptor_xd *)XtMalloc(sizeof(struct descriptor_xd));
      *ans = empty_xd;
      status = TdiCompile(&w_units, data, units, ans MDS_END_ARG);
      if (!(status & 1)) {
	TdiComplain(w);
	ans = 0;
      }
      if (units) {
	MdsFree1Dx(units, 0);
	XtFree((char *)units);
      }
      if (data) {
	MdsFree1Dx(data, 0);
	XtFree((char *)data);
      }
    } else
      ans = data;
  } else {
    ans = (struct descriptor_xd *)XtMalloc(sizeof(struct descriptor_xd));
    *ans = empty_xd;
  }
  return ans;
}

/*
 * Routines for Task type.
 */
static void TaskCreate(XmdsXdBoxWidget w, ArgList args, Cardinal argcount)
{
  MrmType class;
  XmdsXdUserPart *user_part = (XmdsXdUserPart *) XtMalloc(sizeof(XmdsXdUserPart));
  Arg dlog_args[] = { {XmNuserData, 0}
  ,
  {XmNtopAttachment, XmATTACH_FORM}
  ,
  {XmNleftAttachment, XmATTACH_FORM}
  ,
  {XmNrightAttachment, XmATTACH_FORM}
  ,
  {XmNbottomAttachment, XmATTACH_FORM}
  };
  dlog_args[0].value = (XtArgVal) user_part;
  user_part->load_dlog_proc = TaskLoad;
  user_part->unload_dlog_proc = TaskUnload;

  MrmFetchWidgetOverride(xmdsXdBoxClassRec.xdbox_class.drm, "task_dlog",
			 XtNameToWidget(w->xdbox.xdb_dlog, "detail_box"), "task_dlog", dlog_args,
			 XtNumber(dlog_args), &w->xdbox.specific_dlog, &class);
  XtManageChild(w->xdbox.specific_dlog);
}

static void TaskLoad(Widget w, struct descriptor_xd *xd)
{
  XmdsXdBoxWidget xdbw = FindXdBoxWidget(w);
  int arg;
  struct descriptor_xd *ptr = xd;
  Widget menu_widget = XtNameToWidget(w, "task_menu");
  Widget expr_widget = XtNameToWidget(w, "task_expr");
  Widget routine_widget = XtNameToWidget(w, "routine_box");
  XmdsXdBoxWidget method_widget = (XmdsXdBoxWidget) XtNameToWidget(w, "method_box");

  for (ptr = xd; ptr && ptr->dtype == DTYPE_DSC; ptr = (struct descriptor_xd *)ptr->pointer) ;
  if (ptr) {
    xd = ptr;
    if (NotEmptyXd(xd)) {
      if (xd->dtype == DTYPE_ROUTINE) {
	Widget timeout_w = XtNameToWidget(routine_widget, "routine_timeout");
	Widget image_w = XtNameToWidget(routine_widget, "routine_image");
	Widget routine_w = XtNameToWidget(routine_widget, "routine_routine");
	Widget arg_box_w = XtNameToWidget(routine_widget, "*arg_box");
	struct descriptor_routine *r_dsc = (struct descriptor_routine *)xd;
	if (xdbw->xdbox.loaded) {
	  XmdsExprFieldSetXd(timeout_w, r_dsc->time_out);
	  XmdsExprFieldSetXd(image_w, r_dsc->image);
	  XmdsExprFieldSetXd(routine_w, r_dsc->routine);
	  for (arg = 0; arg < 8; arg++) {
	    char arg_name[] = { 'a', 'r', 'g', 0, 0 };
	    Widget arg_w;
	    arg_name[3] = '1' + arg;
	    arg_w = XtNameToWidget(arg_box_w, arg_name);
	    if (arg + 4 <= r_dsc->ndesc && r_dsc->arguments[arg] && r_dsc->arguments[arg]->pointer)
	      XmdsExprFieldSetXd(arg_w, r_dsc->arguments[arg]);
	    else
	      XmdsExprFieldSetXd(arg_w, (struct descriptor *)&empty_xd);
	  }
	}
	XtManageChild(routine_widget);
	XtUnmanageChild((Widget) method_widget);
	XtUnmanageChild(expr_widget);
	XmdsSetOptionIdx(menu_widget, 2);
      } else if (xd->dtype == DTYPE_METHOD) {
	Widget timeout_w = XtNameToWidget((Widget) method_widget, "method_timeout");
	Widget object_w = XtNameToWidget((Widget) method_widget, "method_object");
	Widget method_w = XtNameToWidget((Widget) method_widget, "method_method");
	Widget arg_box_w = XtNameToWidget((Widget) method_widget, "*arg_box");
	struct descriptor_method *m_dsc = (struct descriptor_method *)xd;
	if (xdbw->xdbox.loaded) {
	  XmdsExprFieldSetXd(timeout_w, m_dsc->time_out);
	  XmdsExprFieldSetXd(object_w, m_dsc->object);
	  XmdsExprFieldSetXd(method_w, m_dsc->method);
	  for (arg = 0; arg < 8; arg++) {
	    char arg_name[] = { 'a', 'r', 'g', 0, 0 };
	    Widget arg_w;
	    arg_name[3] = '1' + arg;
	    arg_w = XtNameToWidget(arg_box_w, arg_name);
	    if (arg + 4 <= m_dsc->ndesc && m_dsc->arguments[arg] && m_dsc->arguments[arg]->pointer)
	      XmdsExprFieldSetXd(arg_w, m_dsc->arguments[arg]);
	    else
	      XmdsExprFieldSetXd(arg_w, (struct descriptor *)&empty_xd);
	  }
	}
	XtUnmanageChild(routine_widget);
	XtManageChild((Widget) method_widget);
	XtUnmanageChild(expr_widget);
	XmdsSetOptionIdx(menu_widget, 1);
      } else {
	if (xdbw->xdbox.loaded)
	  XmdsExprSetXd(expr_widget, (struct descriptor *)xd);
	XtUnmanageChild(routine_widget);
	XtUnmanageChild((Widget) method_widget);
	XtManageChild(expr_widget);
	XmdsSetOptionIdx(menu_widget, 3);
      }
    } else {
      XtUnmanageChild(routine_widget);
      XtUnmanageChild((Widget) method_widget);
      XtUnmanageChild(expr_widget);
      XmdsSetOptionIdx(menu_widget, 0);
    }
  } else {
    XmdsSetOptionIdx(menu_widget, 0);
    XtUnmanageChild(expr_widget);
    XtUnmanageChild(routine_widget);
    XtUnmanageChild((Widget) method_widget);
  }
}

EXPORT struct descriptor_xd *TaskUnload(Widget w)
{
  struct descriptor_xd *data = 0;
  Widget expr_widget = XtNameToWidget(w, "task_expr");
  Widget routine_widget = XtNameToWidget(w, "routine_box");
  XmdsXdBoxWidget method_widget = (XmdsXdBoxWidget) XtNameToWidget(w, "method_box");

  if (XtIsManaged(expr_widget) || XtIsManaged(routine_widget)
      || XtIsManaged((Widget) method_widget)) {
    if (XtIsManaged(expr_widget)) {
      data = (struct descriptor_xd *)XmdsExprGetXd(expr_widget);
    } else if (XtIsManaged((Widget) method_widget)) {
      int i;
      int nargs = 0;
      Widget timeout_w = XtNameToWidget((Widget) method_widget, "method_timeout");
      Widget object_w = XtNameToWidget((Widget) method_widget, "method_object");
      Widget method_w = XtNameToWidget((Widget) method_widget, "method_method");
      Widget arg_box_w = XtNameToWidget((Widget) method_widget, "*arg_box");
      static struct descriptor_xd *timeout;
      static struct descriptor_xd *object;
      static struct descriptor_xd *method;
      static struct descriptor_xd *args[8];
      timeout = (struct descriptor_xd *)XmdsExprFieldGetXd(timeout_w);
      object = (struct descriptor_xd *)XmdsExprFieldGetXd(object_w);
      method = (struct descriptor_xd *)XmdsExprFieldGetXd(method_w);
      {
	METHOD(8) method_dsc;
	method_dsc.length = 0;
	method_dsc.dtype = DTYPE_METHOD;
	method_dsc.class = CLASS_R;
	method_dsc.pointer = 0;
	method_dsc.time_out = (struct descriptor *)timeout;
	method_dsc.method = (struct descriptor *)method;
	method_dsc.object = (struct descriptor *)object;
	for (i = 0; i < 8; i++) {
	  char arg_name[] = { 'a', 'r', 'g', 0, 0 };
	  Widget arg_w;
	  arg_name[3] = '1' + i;
	  arg_w = XtNameToWidget(arg_box_w, arg_name);
	  args[i] = (struct descriptor_xd *)XmdsExprFieldGetXd(arg_w);
	  if (args[i] && args[i]->pointer)
	    nargs = i + 1;
	  method_dsc.arguments[i] = (struct descriptor *)args[i];
	}
	method_dsc.ndesc = 3 + nargs;
	data = (struct descriptor_xd *)XtMalloc(sizeof(struct descriptor_xd));
	*data = empty_xd;
	MdsCopyDxXd((struct descriptor *)&method_dsc, data);
      }
    } else if (XtIsManaged(routine_widget)) {
      int i;
      int nargs = 0;
      Widget timeout_w = XtNameToWidget(routine_widget, "routine_timeout");
      Widget image_w = XtNameToWidget(routine_widget, "routine_image");
      Widget routine_w = XtNameToWidget(routine_widget, "routine_routine");
      Widget arg_box_w = XtNameToWidget(routine_widget, "*arg_box");
      static struct descriptor_xd *timeout;
      static struct descriptor_xd *image;
      static struct descriptor_xd *routine;
      static struct descriptor_xd *args[8];
      timeout = (struct descriptor_xd *)XmdsExprFieldGetXd(timeout_w);
      image = (struct descriptor_xd *)XmdsExprFieldGetXd(image_w);
      routine = (struct descriptor_xd *)XmdsExprFieldGetXd(routine_w);
      {
	ROUTINE(8) routine_dsc;
	routine_dsc.length = 0;
	routine_dsc.dtype = DTYPE_ROUTINE;
	routine_dsc.class = CLASS_R;
	routine_dsc.pointer = 0;
	routine_dsc.time_out = (struct descriptor *)timeout;
	routine_dsc.image = (struct descriptor *)image;
	routine_dsc.routine = (struct descriptor *)routine;
	for (i = 0; i < 8; i++) {
	  char arg_name[] = { 'a', 'r', 'g', 0, 0 };
	  Widget arg_w;
	  arg_name[3] = '1' + i;
	  arg_w = XtNameToWidget(arg_box_w, arg_name);
	  args[i] = (struct descriptor_xd *)XmdsExprFieldGetXd(arg_w);
	  if (args[i] && args[i]->pointer)
	    nargs = i + 1;
	  routine_dsc.arguments[i] = (struct descriptor *)args[i];
	}
	routine_dsc.ndesc = 3 + nargs;
	data = (struct descriptor_xd *)XtMalloc(sizeof(struct descriptor_xd));
	*data = empty_xd;
	MdsCopyDxXd((struct descriptor *)&routine_dsc, data);
      }
    }
  }
  return data;
}

static void WindowCreate(XmdsXdBoxWidget w, ArgList args, Cardinal argcount)
{
  MrmType class;
  XmdsXdUserPart *user_part = (XmdsXdUserPart *) XtMalloc(sizeof(XmdsXdUserPart));
  Arg dlog_args[] = { {XmNuserData, 0}
  ,
  {XmNtopAttachment, XmATTACH_FORM}
  ,
  {XmNleftAttachment, XmATTACH_FORM}
  ,
  {XmNrightAttachment, XmATTACH_FORM}
  ,
  {XmNbottomAttachment, XmATTACH_FORM}
  };
  dlog_args[0].value = (XtArgVal) user_part;
  user_part->load_dlog_proc = WindowLoad;
  user_part->unload_dlog_proc = WindowUnload;

  MrmFetchWidgetOverride(xmdsXdBoxClassRec.xdbox_class.drm, "window_dlog",
			 XtNameToWidget(w->xdbox.xdb_dlog, "detail_box"), "window_dlog", dlog_args,
			 XtNumber(dlog_args), &w->xdbox.specific_dlog, &class);
  XtManageChild(w->xdbox.specific_dlog);
}

static void WindowLoad(Widget w, struct descriptor_xd *xd)
{
  XmdsXdBoxWidget xdbw = FindXdBoxWidget(w);
  struct descriptor_xd *ptr = xd;
  Widget menu_widget = XtNameToWidget(w, "window_menu");
  Widget expr_widget = XtNameToWidget(w, "window_expr");
  Widget window_widget = XtNameToWidget(w, "window_box");

  for (ptr = xd; ptr && ptr->dtype == DTYPE_DSC; ptr = (struct descriptor_xd *)ptr->pointer) ;
  if (ptr) {
    xd = ptr;
    if (NotEmptyXd(xd)) {
      if (xd->dtype == DTYPE_WINDOW) {
	Widget start_idx_w = XtNameToWidget(window_widget, "window_start_idx");
	Widget end_idx_w = XtNameToWidget(window_widget, "window_end_idx");
	Widget tzero_w = XtNameToWidget(window_widget, "window_tzero");
	struct descriptor_window *w_dsc = (struct descriptor_window *)xd;
	if (xdbw->xdbox.loaded) {
	  XmdsExprFieldSetXd(start_idx_w, w_dsc->startidx);
	  XmdsExprFieldSetXd(end_idx_w, w_dsc->endingidx);
	  XmdsExprFieldSetXd(tzero_w, w_dsc->value_at_idx0);
	}
	XtManageChild(window_widget);
	XtUnmanageChild(expr_widget);
	XmdsSetOptionIdx(menu_widget, 1);
      } else {
	if (xdbw->xdbox.loaded)
	  XmdsExprSetXd(expr_widget, (struct descriptor *)xd);
	XtUnmanageChild(window_widget);
	XtManageChild(expr_widget);
	XmdsSetOptionIdx(menu_widget, 2);
      }
    } else {
      XmdsSetOptionIdx(menu_widget, 0);
      XtUnmanageChild(expr_widget);
      XtUnmanageChild(window_widget);
    }
  } else {
    XmdsSetOptionIdx(menu_widget, 0);
    XtUnmanageChild(expr_widget);
    XtUnmanageChild(window_widget);
  }
}

EXPORT struct descriptor_xd *WindowUnload(Widget w)
{
  struct descriptor_xd *ans = (struct descriptor_xd *)XtMalloc(sizeof(struct descriptor_xd));
  struct descriptor_xd *data = 0;
  Widget window_expr = XtNameToWidget(w, "window_expr");
  Widget window_box = XtNameToWidget(w, "window_box");

  *ans = empty_xd;

  if (XtIsManaged(window_expr) || XtIsManaged(window_box)) {
    if (XtIsManaged(window_expr)) {
      data = (struct descriptor_xd *)XmdsExprGetXd(window_expr);
    } else if (XtIsManaged(window_box)) {
      static DESCRIPTOR(wind, "BUILD_WINDOW($, $, $)");
      Widget start_idx_w = XtNameToWidget(window_box, "window_start_idx");
      Widget end_idx_w = XtNameToWidget(window_box, "window_end_idx");
      Widget tzero_w = XtNameToWidget(window_box, "window_tzero");
      struct descriptor_xd *start = (struct descriptor_xd *)XmdsExprFieldGetXd(start_idx_w);
      struct descriptor_xd *end = (struct descriptor_xd *)XmdsExprFieldGetXd(end_idx_w);
      struct descriptor_xd *tzero = (struct descriptor_xd *)XmdsExprFieldGetXd(tzero_w);
      data = (struct descriptor_xd *)XtMalloc(sizeof(struct descriptor_xd));
      *data = empty_xd;
      TdiCompile(&wind, start, end, tzero, data MDS_END_ARG);
    }
    ans = data;
  }
  return ans;
}

/*
 *  Generic Widget Callbacks.
 */

static void apply_button_proc(Widget w)
{
  XmdsXdBoxWidget xdbw = FindXdBoxWidget(w);
  if (xdbw) {
    if (Apply(xdbw)) {
      if (xdbw->xdbox.apply_callback) {
	XmdsButtonCallbackStruct button_callback_data = { XmCR_APPLY, 0, 0, 0 };
	button_callback_data.xd = xdbw->xdbox.xd;
	button_callback_data.on_off = xdbw->xdbox.on_off;
	XtCallCallbackList((Widget) xdbw, xdbw->xdbox.apply_callback,
			   (XtPointer) & button_callback_data);
      }
    }
  }
}

static void cancel_button_proc(Widget w)
{
  XmdsXdBoxWidget xdbw = FindXdBoxWidget(w);
  if (xdbw) {
    if (xdbw->xdbox.cancel_callback) {
      XmdsButtonCallbackStruct button_callback_data = { XmCR_CANCEL, 0, 0, 0 };
      button_callback_data.on_off = xdbw->xdbox.on_off;
      XtCallCallbackList((Widget) xdbw, xdbw->xdbox.cancel_callback,
			 (XtPointer) & button_callback_data);
    }
    if (xdbw->xdbox.auto_destroy)
      XtDestroyWidget(XtParent(xdbw));
    else if (xdbw->xdbox.auto_unmanage) {
      XtUnmanageChild((Widget) xdbw);
      xdbw->xdbox.loaded = 0;
    }
  }
}

static void reset_button_proc(Widget w)
{
  XmdsXdBoxWidget xdbw = FindXdBoxWidget(w);
  XmdsXdBoxReset((Widget) xdbw);
  if (xdbw->xdbox.reset_callback) {
    XmdsButtonCallbackStruct button_callback_data = { XmCR_OK, 0, 0, 0 };
    button_callback_data.xd = xdbw->xdbox.xd;
    button_callback_data.on_off = xdbw->xdbox.on_off;
    XtCallCallbackList((Widget) xdbw, xdbw->xdbox.reset_callback,
		       (XtPointer) & button_callback_data);
  }
}

static void ok_button_proc(Widget w)
{
  XmdsXdBoxWidget xdbw = FindXdBoxWidget(w);
  if (xdbw) {
    if (Apply(xdbw)) {
      if (xdbw->xdbox.ok_callback) {
	XmdsButtonCallbackStruct button_callback_data = { XmCR_OK, 0, 0, 0 };
	button_callback_data.xd = xdbw->xdbox.xd;
	button_callback_data.on_off = xdbw->xdbox.on_off;
	XtCallCallbackList((Widget) xdbw, xdbw->xdbox.ok_callback,
			   (XtPointer) & button_callback_data);
      }
      if (xdbw->xdbox.auto_destroy)
	XtDestroyWidget(XtParent(xdbw));
      else if (xdbw->xdbox.auto_unmanage) {
	XtUnmanageChild((Widget) xdbw);
	xdbw->xdbox.loaded = 0;
      }
    }
  }
}

/*
 *  Type Specific Widget Callbacks.
 */

static void action_change_type_proc(Widget w, int *tag)
{
  XmdsXdBoxWidget xdbw = FindXdBoxWidget(w);
  if (xdbw) {
    if (*tag == 1) {
      XtManageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.action_dlog.action_box"));
      XtManageChild(XtNameToWidget
		    (xdbw->xdbox.xdb_dlog, "detail_box.action_dlog.action_notify_label"));
      XtManageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.action_dlog.action_notify"));
      XtUnmanageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.action_dlog.action_expr"));
    } else if (*tag == 2) {
      XtUnmanageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.action_dlog.action_box"));
      XtUnmanageChild(XtNameToWidget
		      (xdbw->xdbox.xdb_dlog, "detail_box.action_dlog.action_notify_label"));
      XtUnmanageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.action_dlog.action_notify"));
      XtManageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.action_dlog.action_expr"));
    } else {
      XtUnmanageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.action_dlog.action_box"));
      XtUnmanageChild(XtNameToWidget
		      (xdbw->xdbox.xdb_dlog, "detail_box.action_dlog.action_notify_label"));
      XtUnmanageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.action_dlog.action_notify"));
      XtUnmanageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.action_dlog.action_expr"));
    }
  }
}

static void axis_change_type_proc(Widget w, int *tag)
{
  XmdsXdBoxWidget xdbw = FindXdBoxWidget(w);
  if (xdbw) {
    if (*tag == 1) {
      XtManageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.axis_dlog.axis_units_label"));
      XtManageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.axis_dlog.axis_units"));
      XtManageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.axis_dlog.axis_box"));
      XtUnmanageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.axis_dlog.axis_expr"));
    } else if (*tag == 2) {
      XtManageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.axis_dlog.axis_units_label"));
      XtManageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.axis_dlog.axis_units"));
      XtUnmanageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.axis_dlog.axis_box"));
      XtManageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.axis_dlog.axis_expr"));
    } else {
      XtUnmanageChild(XtNameToWidget
		      (xdbw->xdbox.xdb_dlog, "detail_box.axis_dlog.axis_units_label"));
      XtUnmanageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.axis_dlog.axis_units"));
      XtUnmanageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.axis_dlog.axis_box"));
      XtUnmanageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.axis_dlog.axis_expr"));
    }
  }
}

static int WidgetNameCmp(Widget w, char *string)
{
  return ((w->core.name == 0) || (w->core.name == (char *)0xffffff)
	  || strcmp(w->core.name, string));
}

static void dispatch_change_type_proc(Widget w, int *tag)
{
  Widget dispatch_dlog;
  for (dispatch_dlog = w;
       dispatch_dlog && WidgetNameCmp(dispatch_dlog, "dispatch_dlog");
       dispatch_dlog = dispatch_dlog->core.parent) ;

  if (dispatch_dlog) {
    if (*tag == 1) {
      XtManageChild(XtNameToWidget(dispatch_dlog, "dispatch_box"));
      XtUnmanageChild(XtNameToWidget(dispatch_dlog, "dispatch_expr"));
    } else if (*tag == 2) {
      XtUnmanageChild(XtNameToWidget(dispatch_dlog, "dispatch_box"));
      XtManageChild(XtNameToWidget(dispatch_dlog, "dispatch_expr"));
    } else {
      XtUnmanageChild(XtNameToWidget(dispatch_dlog, "dispatch_box"));
      XtUnmanageChild(XtNameToWidget(dispatch_dlog, "dispatch_expr"));
    }
  }
}

static void expression_change_type_proc(Widget w, int *tag)
{
  Widget expression_dlog;

  for (expression_dlog = w;
       expression_dlog && WidgetNameCmp(expression_dlog, "expression_dlog");
       expression_dlog = expression_dlog->core.parent) ;
  if (expression_dlog) {
    if (*tag) {
      XtManageChild(XtNameToWidget(expression_dlog, "expression_units_label"));
      XtManageChild(XtNameToWidget(expression_dlog, "expression_units"));
      XtManageChild(XtNameToWidget(expression_dlog, "expression_expr"));
    } else {
      XtUnmanageChild(XtNameToWidget(expression_dlog, "expression_units_label"));
      XtUnmanageChild(XtNameToWidget(expression_dlog, "expression_units"));
      XtUnmanageChild(XtNameToWidget(expression_dlog, "expression_expr"));
    }
  }
}

static void task_change_type_proc(Widget w, int *tag)
{
  Widget task_dlog;

  for (task_dlog = w;
       task_dlog && WidgetNameCmp(task_dlog, "task_dlog"); task_dlog = task_dlog->core.parent) ;

  if (task_dlog) {
    if (*tag == 1) {
      XtManageChild(XtNameToWidget(task_dlog, "method_box"));
      XtUnmanageChild(XtNameToWidget(task_dlog, "routine_box"));
      XtUnmanageChild(XtNameToWidget(task_dlog, "task_expr"));
    } else if (*tag == 2) {
      XtUnmanageChild(XtNameToWidget(task_dlog, "method_box"));
      XtManageChild(XtNameToWidget(task_dlog, "routine_box"));
      XtUnmanageChild(XtNameToWidget(task_dlog, "task_expr"));
    } else if (*tag == 3) {
      XtUnmanageChild(XtNameToWidget(task_dlog, "method_box"));
      XtUnmanageChild(XtNameToWidget(task_dlog, "routine_box"));
      XtManageChild(XtNameToWidget(task_dlog, "task_expr"));
    } else {
      XtUnmanageChild(XtNameToWidget(task_dlog, "method_box"));
      XtUnmanageChild(XtNameToWidget(task_dlog, "routine_box"));
      XtUnmanageChild(XtNameToWidget(task_dlog, "task_expr"));
    }
  }
}

static void window_change_type_proc(Widget w, int *tag)
{
  XmdsXdBoxWidget xdbw = FindXdBoxWidget(w);
  if (xdbw) {
    if (*tag == 1) {
      XtManageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.window_dlog.window_box"));
      XtUnmanageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.window_dlog.window_expr"));
    } else if (*tag == 2) {
      XtUnmanageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.window_dlog.window_box"));
      XtManageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.window_dlog.window_expr"));
    } else {
      XtUnmanageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.window_dlog.window_box"));
      XtUnmanageChild(XtNameToWidget(xdbw->xdbox.xdb_dlog, "detail_box.window_dlog.window_expr"));
    }
  }
}

/*
 *  Support routines.
 */

static Boolean Apply(XmdsXdBoxWidget w)
{
  Boolean status = 1;
  if (w->xdbox.loaded) {
    if (w->xdbox.auto_put && w->xdbox.nid)
      status = Put(w);
    else {
      w->xdbox.xd = Unload(w->xdbox.specific_dlog);
      Load(w->xdbox.specific_dlog, w->xdbox.xd);
      if (w->xdbox.nid) {
	w->xdbox.on_off =
	    XmToggleButtonGadgetGetState(XtNameToWidget
					 (w->xdbox.xdb_dlog, "generic_box.on_off_toggle"));
	if (w->xdbox.tag_list) {
	  XtFree((char *)w->xdbox.tag_list);
	  w->xdbox.tag_list = 0;
	}
	w->xdbox.tag_list =
	    XmTextFieldGetString(XtNameToWidget(w->xdbox.xdb_dlog, "generic_box.tag_text"));
	GenericLoad(w);
      }
    }
  }
  return status;
}

static void CalculateNewSize(XmdsXdBoxWidget w, unsigned short *width, unsigned short *height)
{
  Widget *children;
  int num;
  int right, bottom;
  int i;
  *width = *height = 0;
  XtVaGetValues((Widget) w, XmNchildren, &children, XmNnumChildren, &num, NULL);
  for (i = 0; i < num; i++) {
    if (XtIsManaged(children[i])) {
      right = XtX(children[i]) + XtWidth(children[i]) + 2 * children[i]->core.border_width;
      bottom = XtY(children[i]) + XtHeight(children[i]) + 2 * children[i]->core.border_width;
      if (right > *width)
	*width = right;
      if (bottom > *height)
	*height = bottom;
    }
  }
  if (*height == 0)
    *height = 5;
  if (*width == 0)
    *width = 5;
}

static XmdsXdBoxWidget FindXdBoxWidget(Widget w)
{
  Widget xdbw;
  for (xdbw = w; xdbw && WidgetNameCmp(xdbw, "xd_box"); xdbw = XtParent(xdbw)) ;
  if (xdbw)
    xdbw = XtParent(xdbw);
  return (XmdsXdBoxWidget) xdbw;
}

static void GenericGet(XmdsXdBoxWidget w)
{
  if (w->xdbox.nid) {
    int nid = w->xdbox.nid + w->xdbox.nid_offset;
    char *path = TreeGetMinimumPath(0, nid);
    if (path) {
      char *tag;
      static struct descriptor_d tags = { 0, DTYPE_T, CLASS_D, 0 };
      static DESCRIPTOR(comma, ", ");
      void *ctx = 0;
      int status;
      if (w->xdbox.path)
	XtFree((char *)w->xdbox.path);
      w->xdbox.path = strcpy(XtMalloc(strlen(path) + 1), path);
      TreeFree(path);
      status = TreeIsOn(nid);
      if (status == TreeON) {
	w->xdbox.on_off = 1;
	w->xdbox.parent_on_off = 1;
      } else if (status == TreeOFF) {
	w->xdbox.on_off = 0;
	w->xdbox.parent_on_off = 1;
      } else if (status == TreePARENT_OFF) {
	w->xdbox.on_off = 1;
	w->xdbox.parent_on_off = 0;
      } else if (status == TreeBOTH_OFF) {
	w->xdbox.on_off = 0;
	w->xdbox.parent_on_off = 0;
      }

      if (w->xdbox.tag_list) {
	XtFree((char *)w->xdbox.tag_list);
	w->xdbox.tag_list = 0;
      }
      while ((tag = TreeFindNodeTags(nid, &ctx))) {
	static struct descriptor tag_d = { 0, DTYPE_T, CLASS_S, 0 };
	tag_d.length = strlen(tag);
	tag_d.pointer = tag;
	StrConcat((struct descriptor *)&tags, (struct descriptor *)&tags, &comma,
		  &tag_d MDS_END_ARG);
      }
      if (tags.length) {
	tags.length -= 2;
	tags.pointer += 2;
	w->xdbox.tag_list = DescToNull((struct descriptor_s *)&tags);
	tags.length += 2;
	tags.pointer -= 2;
	StrFree1Dx(&tags);
      }
    }
  }
}

static void GenericLoad(XmdsXdBoxWidget w)
{
  if (w->xdbox.path) {
    XmString path_str = XmStringCreateSimple(w->xdbox.path);
    XtVaSetValues(XtNameToWidget(w->xdbox.xdb_dlog, "generic_box.path_label"), XmNlabelString,
		  path_str, NULL);
    XmStringFree(path_str);
  } else {
    XmString path_str = XmStringCreateSimple("No Path");
    XtVaSetValues(XtNameToWidget(w->xdbox.xdb_dlog, "generic_box.path_label"), XmNlabelString,
		  path_str, NULL);
    XmStringFree(path_str);
  }

  if (w->xdbox.tag_list)
    XmTextFieldSetString(XtNameToWidget(w->xdbox.xdb_dlog, "generic_box.tag_text"),
			 w->xdbox.tag_list);
  else
    XmTextFieldSetString(XtNameToWidget(w->xdbox.xdb_dlog, "generic_box.tag_text"), (char *)"");
  if (TreeIsOpen() == TreeOPEN_EDIT)
    XtSetSensitive(XtNameToWidget(w->xdbox.xdb_dlog, "generic_box.tag_text"), 1);
  else
    XtSetSensitive(XtNameToWidget(w->xdbox.xdb_dlog, "generic_box.tag_text"), 0);

  XmToggleButtonGadgetSetState(XtNameToWidget(w->xdbox.xdb_dlog, "generic_box.on_off_toggle"),
			       w->xdbox.on_off, 0);
  XmToggleButtonGadgetSetState(XtNameToWidget
			       (w->xdbox.xdb_dlog, "generic_box.parent_on_off_toggle"),
			       w->xdbox.parent_on_off, 0);
}

static void Load(Widget w, struct descriptor_xd *xd)
{
  int old_def = SetDefault(w);
  XmdsXdUserPart *user;

  XtVaGetValues(w, XmNuserData, &user, NULL);
  (*user->load_dlog_proc) (w, xd);
  if (old_def != -1)
    TreeSetDefaultNid(old_def);
}

static Boolean NotEmptyXd(struct descriptor_xd *xd)
{
  Boolean ans;
  if (xd == 0)
    ans = 0;
  else if ((xd->class == CLASS_S || xd->class == CLASS_D) && xd->length == 0)
    ans = 0;
  else if ((xd->class == CLASS_XS || xd->class == CLASS_XD) && xd->l_length == 0)
    ans = 0;
  else
    ans = 1;
  return ans;
}

static Boolean Put(XmdsXdBoxWidget w)
{
  int status = 1;
  static EMPTYXD(empty_xd);
  int nid = w->xdbox.nid + w->xdbox.nid_offset;
  if (nid) {
    struct descriptor_xd *xd = 0;
    Boolean node_on;
    Boolean editing = (TreeIsOpen() == TreeOPEN_EDIT);
    char *tag_txt = 0;

    if (w->xdbox.loaded) {
      xd = (struct descriptor_xd *)XmdsXdBoxGetXd((Widget) w);
      node_on =
	  XmToggleButtonGadgetGetState(XtNameToWidget
				       (w->xdbox.xdb_dlog, "generic_box.on_off_toggle"));
      if (editing)
	tag_txt = XmTextFieldGetString(XtNameToWidget(w->xdbox.xdb_dlog, "generic_box.tag_text"));
    } else {
      xd = w->xdbox.xd ? w->xdbox.xd : &empty_xd;
      node_on = w->xdbox.on_off;
      if (editing)
	tag_txt = w->xdbox.tag_list;
    }
    if (xd) {
      status = TreeIsOn(nid);
      if (node_on && ((status == TreeOFF) || (status == TreeBOTH_OFF)))
	status = TreeTurnOn(nid);
      else if (!node_on && ((status == TreeON) || (status == TreePARENT_OFF)))
	status = TreeTurnOff(nid);
      else
	status = 1;
      if (status) {
	status = PutIfChanged(nid, xd);
	if (status) {
	  if (editing) {
	    TreeRemoveNodesTags(nid);
	    UpdateTags((Widget) w, nid, tag_txt);
	  }
	} else
	  XmdsComplain((Widget) w, "Error Writing Record to Tree");
      } else
	XmdsComplain((Widget) w, "Error turning node On/Off");
      if (w->xdbox.loaded) {
	MdsFree1Dx(xd, 0);
	XtFree((char *)xd);
      }
    } else
      status = 0;
  }
  return status;
}

static int SetDefault(Widget widg)
{
  int ans;
  int def_nid = -1;

  XmdsXdBoxWidget w = FindXdBoxWidget(widg);
  if (w) {
    if (TreeIsOpen() & 1) {
      if (w->xdbox.nid) {
	if (w->xdbox.default_nid != -1)
	  def_nid = w->xdbox.default_nid;
	else
	  def_nid = DefaultNid(w->xdbox.nid + w->xdbox.nid_offset);
      }
    }
    if (def_nid != -1) {
      TreeGetDefaultNid(&ans);
      TreeSetDefaultNid(def_nid);
    } else
      ans = -1;
  } else
    ans = -1;
  return ans;
}

static struct descriptor_xd *Unload(Widget w)
{
  XmdsXdBoxWidget xdbw = FindXdBoxWidget(w);
  static EMPTYXD(empty_xd);
  struct descriptor_xd *ans;

  if (xdbw->xdbox.loaded) {
    int old_def = SetDefault(w);
    XmdsXdUserPart *user;
    XtVaGetValues(w, XmNuserData, &user, NULL);
    ans = (*user->unload_dlog_proc) (w);
    if (old_def != -1)
      TreeSetDefaultNid(old_def);
  } else {
    ans = (struct descriptor_xd *)XtMalloc(sizeof(struct descriptor_xd));
    *ans = empty_xd;
    MdsCopyDxXd((struct descriptor *)xdbw->xdbox.xd, ans);
  }
  return ans;
}

static void UpdateTags(Widget w, int nid, char *tags)
{
  int status;
  char *t_ptr = strtok(tags, ", ");
  while (t_ptr) {
    status = TreeAddTag(nid, t_ptr);
    if (!status & 1)
      XmdsComplain(w, "Error adding tag %s to node number %d", t_ptr, nid);
    t_ptr = strtok(NULL, ", ");
  }
}
