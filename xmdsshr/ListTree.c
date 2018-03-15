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
/*-----------------------------------------------------------------------------
 * ListTree	A list widget that displays a file manager style tree
 *
 * Copyright (c) 1996 Robert W. McMullen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * Author: Rob McMullen <rwmcm@mail.ae.utexas.edu>
 *         http://www.ae.utexas.edu/~rwmcm
 */

#define _ListTree_

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <stdio.h>
#include <stdlib.h>
#include <mdsplus/mdsconfig.h>
#include <Xmds/ListTreeP.h>
/* #define DEBUG */

#ifndef __GNUC__
#ifndef __FUNCTION__
#define __FUNCTION__ "-unknown-"
#endif
#endif

#ifdef DEBUG
#include <stdlib.h>
#include <stdarg.h>
void DBG(int line, const char *fcn, const char *fmt, ...)
{
  va_list ap;

  fprintf(stderr, "%s:%d %s()  ", __FILE__, line, fcn);
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
}

#define DARG __LINE__,__FUNCTION__
#define DBGW(a) fprintf(stderr,"%s:%d %s()   %s\n",__FILE__,__LINE__,__FUNCTION__, a)
#else
void DBG(int line, const char *fcn, const char *fmt, ...)
{
}

#define DARG __LINE__,__FUNCTION__
#define DBGW(a)
#endif

#define folder_width 16
#define folder_height 12
static unsigned char folder_bits[] = {
  0x00, 0x1f, 0x80, 0x20, 0x7c, 0x5f, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40,
  0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0xfc, 0x3f,
};

#define folderopen_width 16
#define folderopen_height 12
static unsigned char folderopen_bits[] = {
  0x00, 0x3e, 0x00, 0x41, 0xf8, 0xd5, 0xac, 0xaa, 0x54, 0xd5, 0xfe, 0xaf,
  0x01, 0xd0, 0x02, 0xa0, 0x02, 0xe0, 0x04, 0xc0, 0x04, 0xc0, 0xf8, 0x7f,
};

#define document_width 9
#define document_height 14
static unsigned char document_bits[] = {
  0x1f, 0x00, 0x31, 0x00, 0x51, 0x00, 0x91, 0x00, 0xf1, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0xff, 0x01,
};

#define offset(field) XtOffsetOf(ListTreeRec, list.field)
static XtResource resources[] = {
  {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
   offset(foreground_pixel), XtRString, XtDefaultForeground}
  ,
  {XtNmargin, XtCMargin, XtRDimension, sizeof(Dimension),
   offset(Margin), XtRImmediate, (XtPointer) 2}
  ,
  {XtNindent, XtCMargin, XtRDimension, sizeof(Dimension),
   offset(Indent), XtRImmediate, (XtPointer) 0}
  ,
  {XtNhorizontalSpacing, XtCMargin, XtRDimension, sizeof(Dimension),
   offset(HSpacing), XtRImmediate, (XtPointer) 2}
  ,
  {XtNverticalSpacing, XtCMargin, XtRDimension, sizeof(Dimension),
   offset(VSpacing), XtRImmediate, (XtPointer) 0}
  ,
  {XtNlineWidth, XtCMargin, XtRDimension, sizeof(Dimension),
   offset(LineWidth), XtRImmediate, (XtPointer) 0}
  ,
  {XtNfont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
   offset(font), XtRString, XtDefaultFont}
  ,
  {XtNhighlightPath, XtCBoolean, XtRBoolean, sizeof(Boolean),
   offset(HighlightPath), XtRImmediate, (XtPointer) False}
  ,
  {XtNclickPixmapToOpen, XtCBoolean, XtRBoolean, sizeof(Boolean),
   offset(ClickPixmapToOpen), XtRImmediate, (XtPointer) True}
  ,
  {XtNdoIncrementalHighlightCallback, XtCBoolean, XtRBoolean, sizeof(Boolean),
   offset(DoIncrementalHighlightCallback), XtRImmediate, (XtPointer) False}
  ,
  {XtNbranchPixmap, XtCPixmap, XtRBitmap, sizeof(Pixmap),
   offset(Closed.bitmap), XtRImmediate, (XtPointer) XtUnspecifiedPixmap}
  ,
  {XtNbranchOpenPixmap, XtCPixmap, XtRBitmap, sizeof(Pixmap),
   offset(Open.bitmap), XtRImmediate, (XtPointer) XtUnspecifiedPixmap}
  ,
  {XtNleafPixmap, XtCPixmap, XtRBitmap, sizeof(Pixmap),
   offset(Leaf.bitmap), XtRImmediate, (XtPointer) XtUnspecifiedPixmap}
  ,
  {XtNleafOpenPixmap, XtCPixmap, XtRBitmap, sizeof(Pixmap),
   offset(LeafOpen.bitmap), XtRImmediate, (XtPointer) XtUnspecifiedPixmap}
  ,
  {XtNhighlightCallback, XtCCallback, XtRCallback, sizeof(XtPointer),
   offset(HighlightCallback), XtRCallback, NULL}
  ,
  {XtNactivateCallback, XtCCallback, XtRCallback, sizeof(XtPointer),
   offset(ActivateCallback), XtRCallback, NULL}
  ,
  {XtNmenuCallback, XtCCallback, XtRCallback, sizeof(XtPointer),
   offset(MenuCallback), XtRCallback, NULL}
  ,
  {XtNdestroyItemCallback, XtCCallback, XtRCallback, sizeof(XtPointer),
   offset(DestroyItemCallback), XtRCallback, NULL}
  ,
};

#undef offset

static void Initialize();
static void Destroy();
static void Redisplay();
static void Resize();
static Boolean SetValues();
static void Realize();
static XtGeometryResult QueryGeometry();

static void Draw();
static void DrawAll();
static void DrawChanged();
static void DrawItemHighlight();
static void DrawItemHighlightClear();
static ListTreeItem *GetItem();

static void DeleteChildren();
static void CountAll();
static void GotoPosition();
static void VSBCallback(Widget w, XtPointer client_data, XtPointer call_data);
static void HSBCallback(Widget w, XtPointer client_data, XtPointer call_data);
void ListTreeGetHighlighted(ListTreeWidget w, ListTreeMultiReturnStruct * ret);
void ListTreeSetHighlighted(ListTreeWidget w, ListTreeItem ** items, int count, Boolean clear);

/* Actions */
static void focus_in();
static void focus_out();
static void notify();
static void unset();
static void menu();
static void select_start();
static void extend_select_start();
static void extend_select();
static void keypress();

#ifdef max			/* just in case--we don't know, but these are commonly set */
#undef max			/* by arbitrary unix systems.  Also, we cast to int! */
#endif
/* redefine "max" and "min" macros to take into account "unsigned" values */
#define max(a,b) ((int)(a)>(int)(b)?(int)(a):(int)(b))
#define min(a,b) ((int)(a)<(int)(b)?(int)(a):(int)(b))

/* Font convenience macros */

#define FontHeight(f)  (int)(f->max_bounds.ascent + f->max_bounds.descent)
#define FontDescent(f) (int)(f->max_bounds.descent)
#define FontAscent(f)  (int)(f->max_bounds.ascent)
#define FontTextWidth(f,c) (int)XTextWidth(f, c, strlen(c))

static char defaultTranslations[] = "\
<FocusIn>:		focus-in()\n\
<FocusOut>:		focus-out()\n\
<Btn3Down>:             menu()\n\
<Key>Return:            keypress()\n\
<Key>osfUp:             keypress()\n\
Shift <Btn1Down>:	extend-select-start()\n\
<Btn1Down>:		select-start()\n\
<Btn1Up>:		notify()\n\
Button1 <Btn1Motion>:	extend-select()\n\
";

static XtActionsRec actions[] = {
  {"focus-in", focus_in},
  {"focus-out", focus_out},
  {"notify", notify},
  {"select-start", select_start},
  {"extend-select", extend_select},
  {"extend-select-start", extend_select_start},
  {"unset", unset},
  {"menu", menu},
  {"keypress", keypress},
};

static XmBaseClassExtRec listtreeCoreClassExtRec = {
  /* next_extension            */ NULL,
  /* record_type               */ NULLQUARK,
  /* version                   */ XmBaseClassExtVersion,
  /* size                      */ sizeof(XmBaseClassExtRec),
					/* initialize_prehook        */ NULL,
					/* FIXME */
					/* set_values_prehook        */ NULL,
					/* FIXME */
					/* initialize_posthook       */ NULL,
					/* FIXME */
					/* set_values_posthook       */ NULL,
					/* FIXME */
					/* secondary_object_class    */ NULL,
					/* FIXME */
					/* secondary_object_create   */ NULL,
					/* FIXME */
					/* get_secondary_resources   */ NULL,
					/* FIXME */
  /* fast_subclass             */ {0}
  ,				/* FIXME */
					/* get_values_prehook        */ NULL,
					/* FIXME */
					/* get_values_posthook       */ NULL,
					/* FIXME */
  /* class_part_init_prehook   */ NULL,
  /* class_part_init_posthook  */ NULL,
  /* ext_resources             */ NULL,
  /* compiled_ext_resources    */ NULL,
  /* num_ext_resources         */ 0,
  /* use_sub_resources         */ FALSE,
  /* widget_navigable          */ XmInheritWidgetNavigable,
  /* focus_change              */ XmInheritFocusChange,
  /* wrapper_data              */ NULL
};

XmPrimitiveClassExtRec listtreePrimClassExtRec = {
  /* next_extension      */ NULL,
  /* record_type         */ NULLQUARK,
  /* version             */ XmPrimitiveClassExtVersion,
  /* record_size         */ sizeof(XmPrimitiveClassExtRec),
  /* widget_baseline     */ NULL,
  /* widget_display_rect */ NULL,
  /* widget_margins      */ NULL
};

ListTreeClassRec listtreeClassRec = {
  {
   /* core_class fields     */
   /* MOTIF superclass      */ (WidgetClass) & xmPrimitiveClassRec,
   /* class_name            */ "ListTree",
   /* widget_size           */ sizeof(ListTreeRec),
   /* class_initialize      */ NULL,
   /* class_part_initialize */ NULL,
   /* class_inited          */ False,
   /* initialize            */ Initialize,
   /* initialize_hook       */ NULL,
   /* realize               */ Realize,
   /* actions               */ actions,
   /* num_actions           */ XtNumber(actions),
   /* resources             */ resources,
   /* num_resources         */ XtNumber(resources),
   /* xrm_class             */ NULLQUARK,
   /* compress_motion       */ True,
   /* compress_exposure     */ XtExposeCompressMultiple,
   /* compress_enterleave   */ True,
   /* visible_interest      */ True,
   /* destroy               */ Destroy,
   /* resize                */ Resize,
   /* expose                */ Redisplay,
   /* set_values            */ SetValues,
   /* set_values_hook       */ NULL,
   /* set_values_almost     */ XtInheritSetValuesAlmost,
   /* get_values_hook       */ NULL,
   /* accept_focus          */ NULL,
   /* version               */ XtVersion,
   /* callback_private      */ NULL,
   /* tm_table              */ defaultTranslations,
   /* query_geometry        */ QueryGeometry,
   /* display_accelerator   */ XtInheritDisplayAccelerator,
   /* extension             */ (XtPointer) & listtreeCoreClassExtRec
   }
  ,
  /* Primitive Class part */
  {
   /* border_highlight      */ XmInheritBorderHighlight,
   /* border_unhighlight    */ XmInheritBorderUnhighlight,
   /* translations          */ NULL,
   /* arm_and_activate_proc */ XmInheritArmAndActivate,
   /* synthetic resources   */ NULL,
   /* num syn res           */ 0,
   /* extension             */ (XtPointer) & listtreePrimClassExtRec,
   }
  ,
  {
   /* some stupid compilers barf on empty structures */ 0
   }
};

EXPORT WidgetClass listtreeWidgetClass = (WidgetClass) & listtreeClassRec;

static void MakePixmap(ListTreeWidget w, Pixinfo * pix)
{
  Window root;
  int x, y;
  unsigned int width, height, bw, depth;

  if (pix->bitmap && XGetGeometry(XtDisplay((Widget) w), pix->bitmap,
				  &root, &x, &y, &width, &height, &bw, &depth)) {
    pix->width = (int)width;
    pix->height = (int)height;
    if (pix->height > w->list.maxPixHeight)
      w->list.maxPixHeight = pix->height;

    /* Xmu dependency removed by Alan Marcinkowski */
    if (depth == 1) {
      GC gc;
      XGCValues gcv;

      gcv.background = w->core.background_pixel;
      gcv.foreground = w->list.foreground_pixel;
      gc = XCreateGC(XtDisplay((Widget) w),
		     RootWindowOfScreen(XtScreen((Widget) w)), GCForeground | GCBackground, &gcv);
      pix->pix = XCreatePixmap(XtDisplay((Widget) w),
			       RootWindowOfScreen(XtScreen((Widget) w)),
			       width, height, w->core.depth);
      XCopyPlane(XtDisplay((Widget) w), pix->bitmap, pix->pix, gc, 0, 0, width, height, 0, 0, 1);
      XFreeGC(XtDisplay((Widget) w), gc);
    } else
      pix->pix = pix->bitmap;
  } else {
    pix->width = pix->height = 0;
    pix->pix = (Pixmap) NULL;
  }
}

static void FreePixmap(ListTreeWidget w, Pixinfo * pix)
{
  if (pix->pix)
    XFreePixmap(XtDisplay((Widget) w), pix->pix);
}

static void InitializePixmaps(ListTreeWidget w)
{
  w->list.maxPixHeight = 0;

  if (w->list.Closed.bitmap == XtUnspecifiedPixmap)
    w->list.Closed.bitmap = XCreateBitmapFromData(XtDisplay((Widget) w),
						  RootWindowOfScreen(XtScreen((Widget) w)),
						  (char *)folder_bits, folder_width, folder_height);
  MakePixmap(w, &w->list.Closed);

  if (w->list.Open.bitmap == XtUnspecifiedPixmap)
    w->list.Open.bitmap = XCreateBitmapFromData(XtDisplay((Widget) w),
						RootWindowOfScreen(XtScreen((Widget) w)),
						(char *)folderopen_bits, folderopen_width,
						folderopen_height);
  MakePixmap(w, &w->list.Open);

  if (w->list.Leaf.bitmap == XtUnspecifiedPixmap)
    w->list.Leaf.bitmap = XCreateBitmapFromData(XtDisplay((Widget) w),
						RootWindowOfScreen(XtScreen((Widget) w)),
						(char *)document_bits, document_width,
						document_height);
  MakePixmap(w, &w->list.Leaf);

  if (w->list.LeafOpen.bitmap == XtUnspecifiedPixmap)
    w->list.LeafOpen.bitmap = XCreateBitmapFromData(XtDisplay((Widget) w),
						    RootWindowOfScreen(XtScreen((Widget) w)),
						    (char *)document_bits, document_width,
						    document_height);
  MakePixmap(w, &w->list.LeafOpen);

  w->list.pixWidth = w->list.Closed.width;
  if (w->list.Open.width > w->list.pixWidth)
    w->list.pixWidth = w->list.Open.width;
  if (w->list.Leaf.width > w->list.pixWidth)
    w->list.pixWidth = w->list.Leaf.width;
  if (w->list.LeafOpen.width > w->list.pixWidth)
    w->list.pixWidth = w->list.LeafOpen.width;
  w->list.Closed.xoff = (w->list.pixWidth - w->list.Closed.width) / 2;
  w->list.Open.xoff = (w->list.pixWidth - w->list.Open.width) / 2;
  w->list.Leaf.xoff = (w->list.pixWidth - w->list.Leaf.width) / 2;
  w->list.LeafOpen.xoff = (w->list.pixWidth - w->list.LeafOpen.width) / 2;
}

static void InitializeGC(ListTreeWidget w)
{
  XGCValues values;
  XtGCMask mask;

  values.line_style = LineSolid;
  values.line_width = w->list.LineWidth;
  values.fill_style = FillSolid;
  values.font = w->list.font->fid;
  values.background = w->core.background_pixel;
  values.foreground = w->list.foreground_pixel;

  mask = GCLineStyle | GCLineWidth | GCFillStyle | GCForeground | GCBackground | GCFont;
  w->list.drawGC = XtGetGC((Widget) w, mask, &values);

  values.function = GXinvert;
  mask =
      GCLineStyle | GCLineWidth | GCFillStyle | GCForeground | GCBackground | GCFont | GCFunction;
  w->list.eorGC = XtGetGC((Widget) w, mask, &values);

  values.background = w->list.foreground_pixel;
  values.foreground = w->core.background_pixel;
  mask = GCLineStyle | GCLineWidth | GCFillStyle | GCForeground | GCBackground | GCFont;
  w->list.highlightGC = XtGetGC((Widget) w, mask, &values);
}

static void InitializeScrollBars(ListTreeWidget w)
{
  if (XmIsScrolledWindow(XtParent(w)))
    w->list.mom = XtParent(w);
  else
    w->list.mom = NULL;

  if (w->list.mom) {
    char *name = XtMalloc(strlen(XtName((Widget) w)) + 4);

    strcpy(name, XtName((Widget) w));
    strcat(name, "HSB");
    w->list.hsb = XtVaCreateManagedWidget(name,
					  xmScrollBarWidgetClass, w->list.mom,
					  XmNorientation, XmHORIZONTAL, NULL);
    XtAddCallback(w->list.hsb, XmNdecrementCallback, HSBCallback, (XtPointer) w);
    XtAddCallback(w->list.hsb, XmNdragCallback, HSBCallback, (XtPointer) w);
    XtAddCallback(w->list.hsb, XmNincrementCallback, HSBCallback, (XtPointer) w);
    XtAddCallback(w->list.hsb, XmNpageDecrementCallback, HSBCallback, (XtPointer) w);
    XtAddCallback(w->list.hsb, XmNpageIncrementCallback, HSBCallback, (XtPointer) w);
    XtAddCallback(w->list.hsb, XmNtoBottomCallback, HSBCallback, (XtPointer) w);
    XtAddCallback(w->list.hsb, XmNtoTopCallback, HSBCallback, (XtPointer) w);
    XtAddCallback(w->list.hsb, XmNvalueChangedCallback, HSBCallback, (XtPointer) w);

    strcpy(name, XtName((Widget) w));
    strcat(name, "VSB");
    w->list.vsb = XtVaCreateManagedWidget(name, xmScrollBarWidgetClass, XtParent(w), NULL);
    XtAddCallback(w->list.vsb, XmNdecrementCallback, VSBCallback, (XtPointer) w);
    XtAddCallback(w->list.vsb, XmNdragCallback, VSBCallback, (XtPointer) w);
    XtAddCallback(w->list.vsb, XmNincrementCallback, VSBCallback, (XtPointer) w);
    XtAddCallback(w->list.vsb, XmNpageDecrementCallback, VSBCallback, (XtPointer) w);
    XtAddCallback(w->list.vsb, XmNpageIncrementCallback, VSBCallback, (XtPointer) w);
    XtAddCallback(w->list.vsb, XmNtoBottomCallback, VSBCallback, (XtPointer) w);
    XtAddCallback(w->list.vsb, XmNtoTopCallback, VSBCallback, (XtPointer) w);
    XtAddCallback(w->list.vsb, XmNvalueChangedCallback, VSBCallback, (XtPointer) w);

    XtVaSetValues(w->list.mom,
		  XmNscrollBarDisplayPolicy, XmSTATIC,
		  XmNscrollingPolicy, XmAPPLICATION_DEFINED, XmNvisualPolicy, XmVARIABLE,
		  /* Instead of call to XmScrolledWindowSetAreas() */
		  XmNworkWindow, w,
		  XmNhorizontalScrollBar, w->list.hsb, XmNverticalScrollBar, w->list.vsb, NULL);
    XtFree(name);
  }
}

static void InitializeGeometry(ListTreeWidget w)
{
  w->list.XOffset = 0;

  if (XtHeight(w) < 10) {
    int working;

    working = FontHeight(w->list.font);
    if (w->list.maxPixHeight > working)
      working = w->list.maxPixHeight;
    working += w->list.VSpacing;

    if (w->list.visibleCount == 0)
      w->list.visibleCount = 1;

    w->list.preferredHeight = working * w->list.visibleCount;
    w->list.preferredWidth = 200;
    XtWidth(w) = w->list.preferredWidth + 2 * Prim_ShadowThickness(w)
	+ 2 * Prim_HighlightThickness(w);
    XtHeight(w) = w->list.preferredHeight + 2 * Prim_ShadowThickness(w)
	+ 2 * Prim_HighlightThickness(w);
  } else {
    w->list.preferredWidth = XtWidth(w) - 2 * Prim_ShadowThickness(w)
	- 2 * Prim_HighlightThickness(w);
    w->list.preferredHeight = XtHeight(w) - 2 * Prim_ShadowThickness(w)
	- 2 * Prim_HighlightThickness(w);
  }
  DBG(DARG, "prefWidth=%d prefHeight=%d\n", w->list.preferredWidth, w->list.preferredHeight);
}

static void Initialize(Widget request, Widget tnew, ArgList args, Cardinal * num)
{
  ListTreeWidget w;

  w = (ListTreeWidget) tnew;

  w->list.ret_item_list = NULL;
  w->list.ret_item_alloc = 0;
  w->list.first = w->list.highlighted = w->list.topItem = NULL;
  w->list.topItemPos = w->list.lastItemPos = w->list.bottomItemPos =
      w->list.itemCount = w->list.visibleCount = 0;

  w->list.Refresh = True;
  w->list.HasFocus = False;

  w->list.timer_id = (XtIntervalId) 0;
  w->list.multi_click_time = XtGetMultiClickTime(XtDisplay((Widget) w)) * 2;

  w->list.hsb = w->list.vsb = NULL;
  w->list.hsbPos = 0;
  w->list.hsbMax = 1;

  InitializeScrollBars(w);

  InitializeGC(w);

  InitializePixmaps(w);

  w->list.visibleCount = 10;
  InitializeGeometry(w);
}

static void Destroy(ListTreeWidget w)
{
  ListTreeItem *item, *sibling;

  XtReleaseGC((Widget) w, w->list.drawGC);
  XtReleaseGC((Widget) w, w->list.highlightGC);
  item = w->list.first;
  while (item) {
    if (item->firstchild) {
      DeleteChildren(w, item->firstchild);
    }
    sibling = item->nextsibling;
    XtFree((char *)item->text);
    XtFree((char *)item);
    item = sibling;
  }
  FreePixmap(w, &w->list.Closed);
  FreePixmap(w, &w->list.Open);
  FreePixmap(w, &w->list.Leaf);
  FreePixmap(w, &w->list.LeafOpen);
}

static void Redisplay(Widget aw, XExposeEvent * event, Region region)
{
  ListTreeWidget w = (ListTreeWidget) aw;

  if (!XtIsRealized((Widget) w))
    return;

  if (event) {
    Draw(w, (int)event->y, (int)event->height);
  } else {			/* event==NULL ==> repaint the entire list */
    DrawChanged(w);
  }

  _XmDrawShadows(XtDisplay(w),
		 XtWindow(w),
		 Prim_TopShadowGC(w),
		 Prim_BottomShadowGC(w),
		 Prim_HighlightThickness(w), Prim_HighlightThickness(w),
		 XtWidth(w) - 2 * Prim_HighlightThickness(w),
		 XtHeight(w) - 2 * Prim_HighlightThickness(w),
		 Prim_ShadowThickness(w), XmSHADOW_IN);
}

static Boolean
SetValues(Widget current, Widget request, Widget reply, ArgList args, Cardinal * nargs)
{
  if (!XtIsRealized(current))
    return False;

  return True;
}

static void ResizeStuff(ListTreeWidget w)
{
  XRectangle clip;

  w->list.viewWidth = w->core.width - 2 * Prim_ShadowThickness(w)
      - 2 * Prim_HighlightThickness(w);
  w->list.viewHeight = w->core.height - 2 * Prim_ShadowThickness(w)
      - 2 * Prim_HighlightThickness(w);

  w->list.viewX = Prim_ShadowThickness(w) + Prim_HighlightThickness(w);
  w->list.viewY = Prim_ShadowThickness(w) + Prim_HighlightThickness(w);

  clip.x = w->list.viewX;
  clip.y = w->list.viewY;
  clip.width = w->list.viewWidth;
  clip.height = w->list.viewHeight;
  XSetClipRectangles(XtDisplay((Widget) w), w->list.drawGC, 0, 0, &clip, 1, Unsorted);
  XSetClipRectangles(XtDisplay((Widget) w), w->list.eorGC, 0, 0, &clip, 1, Unsorted);
  XSetClipRectangles(XtDisplay((Widget) w), w->list.highlightGC, 0, 0, &clip, 1, Unsorted);

  CountAll(w);

  w->list.visibleCount = 1;
  if (w->list.itemHeight > 0) {
    w->list.visibleCount = w->list.viewHeight / (w->list.itemHeight + w->list.VSpacing);
  }
}

#define HSB2X(w) w->list.XOffset=-((int) w->list.Margin - w->list.Indent + \
    (w->list.Indent + w->list.pixWidth)*w->list.hsbPos);

static void SetScrollbars(ListTreeWidget w)
{
  if (w->list.vsb) {
    if (w->list.itemCount == 0) {
      XtVaSetValues(w->list.vsb,
		    XmNvalue, 0, XmNsliderSize, 1, XmNpageIncrement, 1, XmNmaximum, 1, NULL);
    } else {
      int top, bot, size;

      top = w->list.topItemPos;
      bot = w->list.itemCount;
      size = w->list.visibleCount;
      DBG(DARG, "BEFORE: top=%d bot=%d size=%d ", top, bot, size);
      if (top + size > bot)
	bot = top + size;
      DBG(DARG, "  AFTER: bot=%d\n", bot);

      XtVaSetValues(w->list.vsb,
		    XmNvalue, top,
		    XmNsliderSize, size,
		    XmNpageIncrement, w->list.visibleCount, XmNmaximum, bot, NULL);
      if (size == bot)
	XmScrollBarSetValues(w->list.vsb, top, size, 1, size, False);
    }
  }

  if (w->list.hsb) {
    int divisor, view;

    divisor = w->list.Indent + w->list.pixWidth;
    view = (w->list.viewWidth + divisor - 1) / divisor;
    w->list.hsbMax = (w->list.preferredWidth + divisor - 1) / divisor;
    if (w->list.hsbPos > 0 && w->list.hsbPos + view > w->list.hsbMax) {
      int save = w->list.hsbPos;

      w->list.hsbPos = w->list.hsbMax - view;
      if (w->list.hsbPos < 0)
	w->list.hsbPos = 0;
      if (save != w->list.hsbPos) {
	HSB2X(w);
	DrawAll(w);
      }
    }
    if (w->list.itemCount == 0 || w->list.preferredWidth == 0) {
      XtVaSetValues(w->list.hsb,
		    XmNvalue, 0, XmNsliderSize, 1, XmNpageIncrement, 1, XmNmaximum, 1, NULL);
    } else {
      XtVaSetValues(w->list.hsb,
		    XmNvalue, w->list.hsbPos,
		    XmNsliderSize, min(w->list.hsbMax, view),
		    XmNpageIncrement, view, XmNmaximum, w->list.hsbMax, NULL);
    }
  }

  DBG(DARG, "item=%d visible=%d\n", w->list.itemCount, w->list.visibleCount);
}

static void VSBCallback(Widget scrollbar, XtPointer client_data, XtPointer call_data)
{
  ListTreeWidget w = (ListTreeWidget) client_data;
  XmScrollBarCallbackStruct *cbs = (XmScrollBarCallbackStruct *) call_data;

  w->list.topItemPos = cbs->value;

  DBG(DARG, "topItemPos=%d\n", w->list.topItemPos);
#if 0
  DBG(DARG, "VSBCallback: cbs->reason=%d ", cbs->reason);
  if (cbs->reason == XmCR_INCREMENT) {
    DBG(DARG, "increment\n");
  } else if (cbs->reason == XmCR_DECREMENT) {
    DBG(DARG, "decrement\n");
  } else if (cbs->reason == XmCR_VALUE_CHANGED) {
    DBG(DARG, "value_changed\n");
    SetScrollbars(w);
  }
#else
  if (w->list.topItemPos != w->list.lastItemPos) {
    GotoPosition(w);
    DrawAll(w);
    SetScrollbars(w);
  }
#endif
}

static void HSBCallback(Widget scrollbar, XtPointer client_data, XtPointer call_data)
{
  ListTreeWidget w = (ListTreeWidget) client_data;
  XmScrollBarCallbackStruct *cbs = (XmScrollBarCallbackStruct *) call_data;

  w->list.hsbPos = cbs->value;
  HSB2X(w);

  DBG(DARG, "XOffset=%d prefWidth=%d viewWidth=%d\n",
      w->list.XOffset, w->list.preferredWidth, w->list.viewWidth);
  if (w->list.XOffset != w->list.lastXOffset) {
    DrawAll(w);
  }
}

static void Resize(ListTreeWidget w)
{
  if (!XtIsRealized((Widget) w))
    return;

  ResizeStuff(w);
  SetScrollbars(w);
}

static XtGeometryResult
QueryGeometry(ListTreeWidget w, XtWidgetGeometry * proposed, XtWidgetGeometry * answer)
{
  answer->request_mode = CWWidth | CWHeight;
  answer->width = w->list.preferredWidth + 2 * Prim_ShadowThickness(w)
      + 2 * Prim_HighlightThickness(w);
  answer->height = w->list.preferredHeight + 2 * Prim_ShadowThickness(w)
      + 2 * Prim_HighlightThickness(w);

  DBG(DARG, "w=%d h=%d\n", answer->width, answer->height);

  if (proposed->width >= answer->width && proposed->height >= answer->height)
    return XtGeometryYes;
  else if (answer->width == XtWidth(w) && answer->height == XtHeight(w)) {
    answer->request_mode = 0;
    return XtGeometryNo;
  } else
    return XtGeometryAlmost;
}

static void Realize(Widget aw, XtValueMask * value_mask, XSetWindowAttributes * attributes)
{
  ListTreeWidget w = (ListTreeWidget) aw;

#define	superclass	(&xmPrimitiveClassRec)
  (*superclass->core_class.realize) (aw, value_mask, attributes);
#undef	superclass

  ResizeStuff(w);
  SetScrollbars(w);
}

/* DEBUGGING FUNCTIONS */
#ifdef DEBUG_TREE
void ItemCheck(ListTreeWidget w, ListTreeItem * item)
{
  ListTreeItem *p;
  char text[1024];

  p = item;
/*      if (p->parent) fprintf(stderr,"%x %x \t",p,p->parent); */
/*      else fprintf(stderr,"%x 00000000 \t",p); */
/*      while (p) { fprintf(stderr," "); p=p->parent; } */
/*      p=item; */
/*      while (p) { */
/*              fprintf(stderr,"%s/",p->text); */
/*              p=p->parent; */
/*      } */
/*      fprintf(stderr,"\n"); */

  if (strcmp(item->text, "pixmaps") == 0) {
    fprintf(stderr, "parent:      %x\n", item->parent);
    fprintf(stderr, "firstchild:  %x\n", item->firstchild);
    fprintf(stderr, "prevsibling: %x\n", item->prevsibling);
    fprintf(stderr, "nextsibling: %x\n", item->nextsibling);
  }
}

void ChildrenCheck(ListTreeWidget w, ListTreeItem * item)
{
  while (item) {
    ItemCheck(w, item);
    if (item->firstchild)
      ChildrenCheck(w, item->firstchild);
    item = item->nextsibling;
  }
}

void TreeCheck(ListTreeWidget w, char *txt)
{
  ListTreeItem *item;

  fprintf(stderr, "\n\n%s\n", txt);
  item = w->list.first;
  while (item) {
    ItemCheck(w, item);
    if (item->firstchild)
      ChildrenCheck(w, item->firstchild);
    item = item->nextsibling;
  }
}
#else
#define TreeCheck(a,b)
#endif

/* Highlighting Utilities ----------------------------------------------- */

static void HighlightItem(ListTreeWidget w, ListTreeItem * item, Boolean state, Boolean draw)
{
  if (item) {
    if (item == w->list.highlighted && !state) {
      w->list.highlighted = NULL;
      if (draw && item->count >= w->list.topItemPos)
	DrawItemHighlightClear(w, item);
    } else if (state != item->highlighted) {
      /*      printf("Highlighting '%s' state=%d x=%d y=%d\n", item->text, draw, item->x, item->ytext); */
      item->highlighted = state;
      if (draw && item->count >= w->list.topItemPos && item->count <= w->list.bottomItemPos)
	DrawItemHighlightClear(w, item);

    }
  }
}

static void HighlightChildren(ListTreeWidget w, ListTreeItem * item, Boolean state, Boolean draw)
{
  while (item) {
    HighlightItem(w, item, state, draw);
    if (item->firstchild) {
      Boolean drawkids;

      if (item->open)
	drawkids = draw;
      else
	drawkids = False;
      HighlightChildren(w, item->firstchild, state, drawkids);
    }
    item = item->nextsibling;
  }
}

static void HighlightAll(ListTreeWidget w, Boolean state, Boolean draw)
{
  HighlightChildren(w, w->list.first, state, draw);
}

static void
HighlightVisibleChildren(ListTreeWidget w, ListTreeItem * item, Boolean state, Boolean draw)
{
  while (item) {
    HighlightItem(w, item, state, draw);
    if (item->firstchild && item->open) {
      HighlightVisibleChildren(w, item->firstchild, state, draw);
    }
    item = item->nextsibling;
  }
}

static void HighlightAllVisible(ListTreeWidget w, Boolean state, Boolean draw)
{
  ListTreeItem *item;

  item = w->list.first;
  while (item) {
    HighlightItem(w, item, state, draw);
    if (item->firstchild && item->open) {
      HighlightVisibleChildren(w, item->firstchild, state, draw);
    }
    item = item->nextsibling;
  }
}

static void AddItemToReturnList(ListTreeWidget w, ListTreeItem * item, int loc)
{
  if (loc >= w->list.ret_item_alloc) {
    w->list.ret_item_alloc += ListTreeRET_ALLOC;
    w->list.ret_item_list =
	(ListTreeItem **) XtRealloc((char *)w->list.ret_item_list,
				    w->list.ret_item_alloc * sizeof(ListTreeItem *));
  }
  w->list.ret_item_list[loc] = item;
}

static void MultiAddToReturn(ListTreeWidget w, ListTreeItem * item, ListTreeMultiReturnStruct * ret)
{
  AddItemToReturnList(w, item, ret->count);
  ret->items = w->list.ret_item_list;
  ret->count++;
}

static void HighlightCount(ListTreeWidget w, ListTreeItem * item, ListTreeMultiReturnStruct * ret)
{
  while (item) {
    if (item->highlighted)
      MultiAddToReturn(w, item, ret);
    if (item->firstchild && item->open)
      HighlightCount(w, item->firstchild, ret);
    item = item->nextsibling;
  }
}

static void MakeMultiCallbackStruct(ListTreeWidget w, ListTreeMultiReturnStruct * ret)
{
  ListTreeItem *item;

  ret->items = NULL;
  ret->count = 0;
  item = w->list.first;
  while (item) {
    if (item->highlighted)
      MultiAddToReturn(w, item, ret);
    if (item->firstchild && item->open)
      HighlightCount(w, item->firstchild, ret);
    item = item->nextsibling;
  }
}

static void HighlightDoCallback(ListTreeWidget w)
{
  ListTreeMultiReturnStruct ret;

  if (w->list.HighlightCallback) {
    MakeMultiCallbackStruct(w, &ret);
    XtCallCallbacks((Widget) w, XtNhighlightCallback, &ret);
  }
}

/* Events ------------------------------------------------------------------ */

static void
MakeActivateCallbackStruct(ListTreeWidget w, ListTreeItem * item, ListTreeActivateStruct * ret)
{
  int count;
  ListTreeItem *parent;

  count = 1;
  parent = item;
  while (parent->parent) {
    parent = parent->parent;
    count++;
  }

  ret->item = item;
  ret->count = count;
  ret->open = item->open;
  if (item->firstchild)
    ret->reason = XtBRANCH;
  else
    ret->reason = XtLEAF;
  while (count > 0) {
    count--;
    AddItemToReturnList(w, item, count);
    item = item->parent;
  }
  ret->path = w->list.ret_item_list;
}

static void SelectDouble(ListTreeWidget w)
{
  ListTreeActivateStruct ret;

  TreeCheck(w, "in SelectDouble");
  if (w->list.timer_item) {
    w->list.timer_type = TIMER_DOUBLE;
    w->list.timer_item->open = !w->list.timer_item->open;
    w->list.highlighted = w->list.timer_item;
    HighlightAll(w, False, True);

    MakeActivateCallbackStruct(w, w->list.timer_item, &ret);

    /* Highlight the path if we need to */
    if (w->list.HighlightPath) {
      Boolean save;

      save = w->list.Refresh;
      w->list.Refresh = False;
      ListTreeSetHighlighted(w, ret.path, ret.count, True);
      w->list.Refresh = save;
/*       ListTreeGetHighlighted(w,&ret2); */
/*       ListTreeSetHighlighted(w,ret2.items,ret2.count,True); */
    }

    if (w->list.ActivateCallback) {
      XtCallCallbacks((Widget) w, XtNactivateCallback, (XtPointer) & ret);
    }

    w->list.timer_item->highlighted = True;
    w->list.recount = True;
    DrawChanged(w);
  }
  TreeCheck(w, "exiting SelectDouble");
}

/* ARGSUSED */
static void SelectSingle(XtPointer client_data, XtIntervalId * idp)
{
  ListTreeWidget w = (ListTreeWidget) client_data;

  w->list.timer_id = (XtIntervalId) 0;
  if (w->list.timer_item) {
    if (w->list.ClickPixmapToOpen && w->list.timer_x < w->list.timer_item->x) {
      SelectDouble(w);
    } else {
      HighlightAll(w, False, True);
      HighlightItem(w, w->list.timer_item, True, True);
      if (w->list.timer_type != TIMER_CLEAR && w->list.DoIncrementalHighlightCallback)
	HighlightDoCallback(w);
      w->list.timer_type = TIMER_SINGLE;
    }
  }
}

/* ARGSUSED */
static void select_start(Widget aw, XEvent * event, String * params, Cardinal * num_params)
{
  ListTreeWidget w = (ListTreeWidget) aw;

  w->list.timer_item = NULL;
  w->list.timer_x = event->xbutton.x - w->list.XOffset;
  w->list.timer_y = event->xbutton.y;
  w->list.timer_type = TIMER_WAITING;
  w->list.timer_item = GetItem(w, event->xbutton.y);

  if (!w->list.timer_item) {
    if (w->list.timer_id) {
      XtRemoveTimeOut(w->list.timer_id);
      w->list.timer_id = (XtIntervalId) 0;
    }
  } else {
    if (w->list.timer_id) {
      XtRemoveTimeOut(w->list.timer_id);
      w->list.timer_id = (XtIntervalId) 0;
      SelectDouble(w);
    } else {
      w->list.timer_id = XtAppAddTimeOut(XtWidgetToApplicationContext((Widget) w),
					 (unsigned long)w->list.multi_click_time,
					 SelectSingle, (XtPointer) w);

    }
  }
}

/* ARGSUSED */
static void extend_select_start(Widget aw, XEvent * event, String * params, Cardinal * num_params)
{
  ListTreeWidget w = (ListTreeWidget) aw;

  w->list.timer_item = NULL;
  w->list.timer_x = event->xbutton.x;
  w->list.timer_y = event->xbutton.y;
  w->list.timer_type = TIMER_WAITING;
  w->list.timer_item = GetItem(w, event->xbutton.y);
  w->list.timer_id = (XtIntervalId) 0;
  HighlightItem(w, w->list.timer_item, True, True);
  if (w->list.timer_type != TIMER_CLEAR && w->list.DoIncrementalHighlightCallback)
    HighlightDoCallback(w);
}

/* ARGSUSED */
static void extend_select(Widget aw, XEvent * event, String * params, Cardinal * num_params)
{
  ListTreeItem *item;
  ListTreeWidget w = (ListTreeWidget) aw;
  int y, yend;

/* If we are waiting for a double click, return before doing anything */
  if (w->list.timer_id)
    return;

/* We need the timer_item to be pointing to the first selection in this */
/* group.  If we got here without it being set, something is very wrong. */
  if (w->list.timer_item) {
    y = w->list.timer_y;
    yend = event->xbutton.y;
    item = GetItem(w, y);
    if (y < yend) {
      while (item && y < yend && y < w->list.viewY + w->list.viewHeight) {
	if (item) {
	  DBG(DARG, "Highlighting y=%d item=%s\n", y, item->text);
	  HighlightItem(w, item, True, True);
	  y += item->height + w->list.VSpacing;
	}
	item = GetItem(w, y);
      }
    } else {
      while (item && y > yend && y > 0) {
	if (item) {
	  DBG(DARG, "Highlighting y=%d item=%s\n", y, item->text);
	  HighlightItem(w, item, True, True);
	  y -= item->height + w->list.VSpacing;
	}
	item = GetItem(w, y);
      }
    }
    if (w->list.timer_type != TIMER_CLEAR && w->list.DoIncrementalHighlightCallback)
      HighlightDoCallback(w);
  }
}

/* ARGSUSED */
static void unset(Widget aw, XEvent * event, String * params, Cardinal * num_params)
{
  ListTreeItem *item;
  ListTreeWidget w = (ListTreeWidget) aw;

  item = GetItem(w, event->xbutton.y);
  if (item) {
/*              item->open=False; */
/*              lw->list.highlighted=item; */
/*              DrawAll(lw); */
/*              ListTreeDelete(lw,item); */
  }
}

/* ARGSUSED */
static void notify(Widget aw, XEvent * event, String * params, Cardinal * num_params)
{
  ListTreeWidget w = (ListTreeWidget) aw;

  if (w->list.timer_id) {
    /* don't call highlightCallback if we are waiting for a double click */
  } else if (w->list.timer_type != TIMER_CLEAR && !w->list.DoIncrementalHighlightCallback) {
    HighlightDoCallback(w);
    w->list.timer_type = TIMER_CLEAR;
  }
}

/* ARGSUSED */
static void focus_in(Widget aw, XEvent * event, String * params, Cardinal * num_params)
{
  ListTreeWidget w = (ListTreeWidget) aw;

  DBGW("focus_in");

  if (!w->list.HasFocus) {
    XtCallActionProc(aw, "PrimitiveFocusIn", event, params, *num_params);

    w->list.HasFocus = True;
  }
}

/* ARGSUSED */
static void focus_out(Widget aw, XEvent * event, String * params, Cardinal * num_params)
{
  ListTreeWidget w = (ListTreeWidget) aw;

  DBGW("focus_out");

  if (w->list.HasFocus) {
    XtCallActionProc(aw, "PrimitiveFocusOut", event, params, *num_params);

    w->list.HasFocus = False;
  }
}

/* ARGSUSED */
static void menu(aw, event, params, num_params)
Widget aw;
XEvent *event;
String *params;
Cardinal *num_params;
{
  ListTreeWidget w = (ListTreeWidget) aw;
  ListTreeItem *item;
  ListTreeItemReturnStruct ret;

  if (w->list.MenuCallback) {

    /* See if there is an item at the position of the click */
    item = GetItem(w, event->xbutton.y);

    if (item) {
      ret.reason = XtMENU;
      ret.item = item;
      ret.event = event;
      XtCallCallbacks((Widget) w, XtNmenuCallback, &ret);
    }
  }
}

/* ARGSUSED */
static void keypress(aw, event, params, num_params)
Widget aw;
XEvent *event;
String *params;
Cardinal *num_params;
{
  DBG(DARG, "keypress\n");
}

/* ListTree private drawing functions ------------------------------------- */

/* Select the pixmap to use, if any */
static Pixinfo *GetItemPix(ListTreeWidget w, ListTreeItem * item)
{
  Pixinfo *pix;

  pix = NULL;

  /* Another enhancement from Alan Marcinkowski */
  if (item->openPixmap || item->closedPixmap) {
    /* Guess that it is closed. */
    Pixmap pixmap = item->closedPixmap;

    Window root;
    unsigned int pixwidth, pixheight, pixbw, pixdepth, pixx, pixy;

    /* If it is not closed and there is a pixmap for it, then use that one
     * instead.
     */
    if (item->open && item->openPixmap) {
      pixmap = item->openPixmap;
    }

    /* Make sure we got one. */
    if (pixmap) {
      /* Get the geometry of the pixmap. */
      XGetGeometry(XtDisplay((Widget) w), pixmap, &root, (int *)&pixx, (int *)&pixy,
		   &pixwidth, &pixheight, &pixbw, &pixdepth);

      /* Setup the temporary one that will be used and point to it. */
      w->list.ItemPix.width = (int)pixwidth;
      w->list.ItemPix.height = (int)pixheight;
      w->list.ItemPix.xoff = 0;
      w->list.ItemPix.pix = pixmap;
      pix = &w->list.ItemPix;
    }
  }

  /* If we don't have a pixmap yet... */
  if (!pix) {
    if (item->firstchild || item->type == ItemBranchType) {
      if (item->open)
	pix = &w->list.Open;
      else
	pix = &w->list.Closed;
    } else {
      if (item->open)
	pix = &w->list.LeafOpen;
      else
	pix = &w->list.Leaf;
    }
  }

  return pix;
}

static void DrawItemHighlight(ListTreeWidget w, ListTreeItem * item)
{
  int width;

  if (item->highlighted || item == w->list.highlighted) {
    width = w->core.width - item->x - w->list.XOffset;
    XFillRectangle(XtDisplay(w), XtWindow(w),
		   w->list.drawGC,
		   item->x + w->list.XOffset, item->ytext, width, FontHeight(w->list.font));
    XDrawString(XtDisplay(w), XtWindow(w), w->list.highlightGC,
		item->x + w->list.XOffset, item->ytext + FontAscent(w->list.font),
		item->text, item->length);
  } else {
    XDrawString(XtDisplay(w), XtWindow(w), w->list.drawGC,
		item->x + w->list.XOffset, item->ytext + FontAscent(w->list.font),
		item->text, item->length);
  }
}

static void DrawItemHighlightClear(ListTreeWidget w, ListTreeItem * item)
{
  int width;

  width = w->core.width - item->x - w->list.XOffset;
  if (item->highlighted || item == w->list.highlighted) {
    XFillRectangle(XtDisplay(w), XtWindow(w),
		   w->list.drawGC,
		   item->x + w->list.XOffset, item->ytext, width, FontHeight(w->list.font));
    XDrawString(XtDisplay(w), XtWindow(w), w->list.highlightGC,
		item->x + w->list.XOffset, item->ytext + FontAscent(w->list.font),
		item->text, item->length);
  } else {
    XFillRectangle(XtDisplay(w), XtWindow(w),
		   w->list.highlightGC,
		   item->x + w->list.XOffset, item->ytext, width, FontHeight(w->list.font));
    XDrawString(XtDisplay(w), XtWindow(w), w->list.drawGC,
		item->x + w->list.XOffset, item->ytext + FontAscent(w->list.font),
		item->text, item->length);
  }
}

static void
DrawItem(ListTreeWidget w, ListTreeItem * item,
	 int y, int *xroot, int *yroot, int *retwidth, int *retheight)
{
  int height, xpix, ypix, xbranch, ybranch, xtext, ytext, yline;
  Pixinfo *pix;

  if (item->count < w->list.topItemPos) {
    *xroot = item->x - (int)w->list.HSpacing - w->list.pixWidth / 2;
    *yroot = 0;
    *retwidth = *retheight = 0;
    return;
  }

  pix = GetItemPix(w, item);

/* Compute the height of this line */
  height = FontHeight(w->list.font);
  xtext = item->x;
  xpix = xtext - (int)w->list.HSpacing - w->list.pixWidth + pix->xoff;

  if (pix) {
    if (pix->height > height) {
      ytext = y + ((pix->height - height) / 2);
      height = pix->height;
      ypix = y;
    } else {
      ytext = y;
      ypix = y + ((height - pix->height) / 2);
    }
    ybranch = ypix + pix->height;
    yline = ypix + (pix->height / 2);
  } else {
    ypix = ytext = y;
    yline = ybranch = ypix + (height / 2);
    yline = ypix + (height / 2);
  }
  xbranch = item->x - (int)w->list.HSpacing - w->list.pixWidth / 2;

/* Save the basic graphics info for use by other functions */
  item->y = y;
  item->ytext = ytext;
  item->height = (Dimension) height;

  if ((*xroot >= 0) &&
      ((*yroot >= w->list.exposeTop && *yroot <= w->list.exposeBot) ||
       (yline >= w->list.exposeTop && yline <= w->list.exposeBot) ||
       (*yroot < w->list.exposeTop && yline > w->list.exposeBot)))
    XDrawLine(XtDisplay(w), XtWindow(w), w->list.drawGC,
	      *xroot + w->list.XOffset, *yroot, *xroot + w->list.XOffset, yline);
  if (y >= w->list.exposeTop && y <= w->list.exposeBot) {
    if (*xroot >= 0)
      XDrawLine(XtDisplay(w), XtWindow(w), w->list.drawGC,
		*xroot + w->list.XOffset, yline, xbranch + w->list.XOffset, yline);
    if (pix && pix->pix)
      XCopyArea(XtDisplay(w), pix->pix, XtWindow(w),
		w->list.drawGC, 0, 0, pix->width, pix->height, xpix + w->list.XOffset, ypix);
    DrawItemHighlight(w, item);
  }
  *xroot = xbranch;
  *yroot = ybranch;
  *retwidth = FontTextWidth(w->list.font, item->text);
  *retheight = height;
}

static int
DrawChildren(ListTreeWidget w, ListTreeItem * item, ListTreeItem ** last,
	     int y, int xroot, int yroot)
{
  int width, height;
  int xbranch, ybranch;

  while (item && y < w->list.exposeBot) {
    xbranch = xroot;
    ybranch = yroot;
    DrawItem(w, item, y, &xbranch, &ybranch, &width, &height);

    width += item->x + (int)w->list.Margin;

    if (width > w->list.preferredWidth)
      w->list.preferredWidth = width;

    if (height > 0)
      y += height + (int)w->list.VSpacing;

    if (last)
      *last = item;

    if ((item->firstchild) && (item->open))
      y = DrawChildren(w, item->firstchild, last, y, xbranch, ybranch);

    item = item->nextsibling;
  }
  return y;
}

/*
 * Draws vertical lines connecting items to their siblings below the last
 * visible item
 */
static void DrawVertical(ListTreeWidget w, ListTreeItem * item)
{
  int xroot;
  int yroot;

  while (item->parent) {
    /* If this parent has another child, that means that a line extends off
     * the screen to the bottom. */
    if (item->nextsibling) {
      xroot = item->parent->x - (int)w->list.HSpacing - w->list.pixWidth / 2;
      if (item->parent->count < w->list.topItemPos)
	yroot = 0;
      else
	yroot = item->parent->y + item->parent->height;

      DBG(DARG, "parent=%s drawing x=%d y=%d\n", item->parent->text, xroot, yroot);
      XDrawLine(XtDisplay(w), XtWindow(w), w->list.drawGC,
		xroot + w->list.XOffset, yroot, xroot + w->list.XOffset, w->list.exposeBot);
    } else {
      DBG(DARG, "parent=%s  NOT DRAWING\n", item->parent->text);
    }

    item = item->parent;
  }
}

/* Draws items starting from topItemPos */
static void Draw(ListTreeWidget w, int yevent, int hevent)
{
  int y, xbranch, ybranch;
  ListTreeItem *item, *lastdrawn;

  TreeCheck(w, "Draw");

  if (w->list.recount)
    CountAll(w);

/* Overestimate the expose region to be sure to draw an item that gets */
/* cut by the region */
  w->list.exposeTop = yevent - FontHeight(w->list.font);
  w->list.exposeBot = yevent + hevent + FontHeight(w->list.font);
  w->list.preferredWidth = 0;

  item = w->list.topItem;
  if (!item)
    return;			/* skip if this is an empty list */

  while (item->parent)
    item = item->parent;
  y = (int)w->list.viewY + (int)w->list.Margin;
  xbranch = -1;
  ybranch = 0;

  DrawChildren(w, item, &lastdrawn, y, xbranch, ybranch);

  DBG(DARG, "lastdrawn=%s\n", lastdrawn->text);
  w->list.bottomItemPos = lastdrawn->count;

  DrawVertical(w, lastdrawn);

/*   SetScrollbars(w); */

  w->list.lastItemPos = w->list.topItemPos;
  w->list.lastXOffset = w->list.XOffset;
}

static void DrawAll(ListTreeWidget w)
{
  XClearArea(XtDisplay((Widget) w), XtWindow((Widget) w),
	     w->list.viewX, w->list.viewY, w->list.viewWidth, w->list.viewHeight, False);
  if (w->list.recount)
    CountAll(w);
  Draw(w, w->list.viewY, w->list.viewY + w->list.viewHeight);
}

static void DrawChanged(ListTreeWidget w)
{
  DrawAll(w);
  SetScrollbars(w);
}

/* Counting functions ------------------------------------------------------- */
static int GotoPositionChildren(ListTreeWidget w, ListTreeItem * item, int i)
{
  while (item && i < w->list.topItemPos) {
    i++;
    w->list.topItem = item;

    if (item->firstchild && item->open && i < w->list.topItemPos)
      i = GotoPositionChildren(w, item->firstchild, i);

    item = item->nextsibling;
  }
  return i;
}

static void GotoPosition(ListTreeWidget w)
{
  w->list.topItem = w->list.first;
  GotoPositionChildren(w, w->list.topItem, -1);
}

static int CountItem(ListTreeWidget w, ListTreeItem * item, int x, int y)
{
  int height;
  int xtext;
  Pixinfo *pix;

  item->count = w->list.itemCount;
  w->list.itemCount++;

/* Select the pixmap to use, if any */
  pix = GetItemPix(w, item);

/* Compute the height of this line */
  height = FontHeight(w->list.font);
  xtext = x + (int)w->list.HSpacing;
  if (pix && pix->height > height) {
    height = pix->height;
  }

/* Save the basic graphics info for use by other functions */
  item->x = xtext;
  item->y = item->ytext = -1;
  item->height = (Dimension) height;

  if (item->height > w->list.itemHeight)
    w->list.itemHeight = item->height;

  return height;
}

static int CountChildren(ListTreeWidget w, ListTreeItem * item, int x, int y)
{
  int height;

  x += (int)w->list.Indent + w->list.pixWidth;
  while (item) {
    height = CountItem(w, item, x, y);

    y += height + (int)w->list.VSpacing;
    if ((item->firstchild) && (item->open))
      y = CountChildren(w, item->firstchild, x, y);

    item = item->nextsibling;
  }
  return y;
}

static void CountAll(ListTreeWidget w)
{
  int x, y;

  w->list.itemCount = 0;
  w->list.itemHeight = 0;
  w->list.recount = False;

  /*
   * the first x must be w->list.Margin + w->list.pixWidth, so set it up
   * so CountChildren gets it right for the root items
   */
  x = (int)w->list.viewX + (int)w->list.Margin - w->list.Indent;
  y = (int)w->list.viewY + (int)w->list.Margin;
  CountChildren(w, w->list.first, x, y);
}

/* Private Functions --------------------------------------------------------- */

/* This function removes the specified item from the linked list.  It does */
/* not do anything with the data contained in the item, though. */
static void RemoveReference(w, item)
ListTreeWidget w;
ListTreeItem *item;
{

/* If there exists a previous sibling, just skip over item to be dereferenced */
  if (item->prevsibling) {
    item->prevsibling->nextsibling = item->nextsibling;
    if (item->nextsibling)
      item->nextsibling->prevsibling = item->prevsibling;
  }
/* If not, then the deleted item is the first item in some branch. */
  else {
    if (item->parent)
      item->parent->firstchild = item->nextsibling;
    else
      w->list.first = item->nextsibling;
    if (item->nextsibling)
      item->nextsibling->prevsibling = NULL;
  }

/* Don't forget to update topItem (Paul Newton <pkn@Cs.Nott.AC.UK> caught this )*/
  if (item == w->list.topItem)
    w->list.topItem = item->nextsibling;
}

static void DeleteChildren(w, item)
ListTreeWidget w;
ListTreeItem *item;
{
  ListTreeItem *sibling;
  ListTreeItemReturnStruct ret;

  while (item) {
    if (item->firstchild) {
      DeleteChildren(w, item->firstchild);
      item->firstchild = NULL;
    }
    sibling = item->nextsibling;

    if (w->list.DestroyItemCallback) {
      ret.reason = XtDESTROY;
      ret.item = item;
      ret.event = NULL;
      XtCallCallbacks((Widget) w, XtNdestroyItemCallback, &ret);
    }

    XtFree((char *)item->text);
    XtFree((char *)item);
    item = sibling;
  }
}

static void InsertChild(w, parent, item)
ListTreeWidget w;
ListTreeItem *parent;
ListTreeItem *item;
{
  ListTreeItem *i;

  item->parent = parent;
  item->nextsibling = item->prevsibling = NULL;
  if (parent) {
    if (parent->firstchild) {
      i = parent->firstchild;
      while (i->nextsibling) {
	i = i->nextsibling;
      }
      i->nextsibling = item;
      item->prevsibling = i;
    } else {
      parent->firstchild = item;
    }

  } else {			/* if parent==NULL, this is a top level entry */
    if (w->list.first) {
      i = w->list.first;
      while (i->nextsibling) {
	i = i->nextsibling;
      }
      i->nextsibling = item;
      item->prevsibling = i;
    } else {
      w->list.first = w->list.topItem = item;
    }
  }
  w->list.recount = True;
}

/* Insert a list of ALREADY LINKED children into another list */
static void InsertChildren(w, parent, item)
ListTreeWidget w;
ListTreeItem *parent;
ListTreeItem *item;
{
  ListTreeItem *next, *newnext;

/*      while (item) { */
/*              next=item->nextsibling; */
/*              InsertChild(w,parent,item); */
/*              item=next; */
/*      } */
/*      return; */

/* Save the reference for the next item in the new list */
  next = item->nextsibling;

/* Insert the first item in the new list into the existing list */
  InsertChild(w, parent, item);

/* The first item is inserted, with its prev and next siblings updated */
/* to fit into the existing list.  So, save the existing list reference */
  newnext = item->nextsibling;

/* Now, mark the first item's next sibling to point back to the new list */
  item->nextsibling = next;

/* Mark the parents of the new list to the new parent.  The order of the */
/* rest of the new list should be OK, and the second item should still */
/* point to the first, even though the first was reparented. */
  while (item->nextsibling) {
    item->parent = parent;
    item = item->nextsibling;
  }

/* Fit the end of the new list back into the existing list */
  item->nextsibling = newnext;
  if (newnext)
    newnext->prevsibling = item;
}

static int
SearchChildren(ListTreeWidget w, ListTreeItem * item, ListTreeItem ** last,
	       int y, int findy, ListTreeItem ** finditem)
{
  while (item) {
    DBG(DARG, "searching y=%d item=%s\n", y, item->text);
    if (findy >= y && findy <= y + item->height + w->list.VSpacing) {
      *finditem = item;
      return -1;
    }
    y += item->height + (int)w->list.VSpacing;
    if ((item->firstchild) && (item->open)) {
      y = SearchChildren(w, item->firstchild, NULL, y, findy, finditem);
      if (*finditem)
	return -1;
    }
    if (last)
      *last = item;
    item = item->nextsibling;
  }
  return y;
}

static ListTreeItem *GetItem(w, findy)
ListTreeWidget w;
int findy;
{
  int y;
  ListTreeItem *item, *finditem, *lastdrawn;

  TreeCheck(w, "in GetItem");
  y = (int)w->list.viewY + (int)w->list.Margin;
  item = w->list.topItem;
  finditem = NULL;
  lastdrawn = item;
  while (!finditem && lastdrawn && y < w->core.height) {
    y = SearchChildren(w, item, &lastdrawn, y, findy, &finditem);

    /*
     * If there are still more items to draw, but the previous group of
     * siblings ran out, start checking up through the parents for more
     * items.
     */
    if (lastdrawn->parent && y < w->core.height) {
      ListTreeItem *parent;

      /* continue with the item after the parent of the previous group */
      parent = lastdrawn;
      do {
	parent = parent->parent;
	if (parent)
	  item = parent->nextsibling;
	else
	  item = NULL;
      } while (parent && !item);
      if (!item)
	lastdrawn = NULL;
    } else
      lastdrawn = NULL;
  }
  TreeCheck(w, "exiting GetItem");
  return finditem;
}

static int SearchPosition(w, item, y, finditem, found)
ListTreeWidget w;
ListTreeItem *item, *finditem;
int y;
Boolean *found;
{
  int height;
  Pixinfo *pix;

  while (item) {
/*              DBG(DARG,"Checking y=%d  item=%s\n",y,item->text); */
    if (item == finditem) {
      *found = True;
      return y;
    }

    pix = GetItemPix(w, item);

/* Compute the height of this line */
    height = FontHeight(w->list.font);
    if (pix && pix->height > height)
      height = pix->height;

    y += height + (int)w->list.VSpacing;
    if ((item->firstchild) && (item->open)) {
      y = SearchPosition(w, item->firstchild, y, finditem, found);
      if (*found)
	return y;
    }
    item = item->nextsibling;
  }
  return y;
}

static Position GetPosition(w, finditem)
ListTreeWidget w;
ListTreeItem *finditem;
{
  int y, height;
  ListTreeItem *item;
  Pixinfo *pix;
  Boolean found;

  TreeCheck(w, "in GetPosition");
  y = (int)w->list.viewY + (int)w->list.Margin;
  item = w->list.first;
  found = False;
  while (item && item != finditem) {

    pix = GetItemPix(w, item);

/* Compute the height of this line */
    height = FontHeight(w->list.font);
    if (pix && pix->height > height)
      height = pix->height;

    y += height + (int)w->list.VSpacing;
    if ((item->firstchild) && (item->open)) {
      y = SearchPosition(w, item->firstchild, y, finditem, &found);
      if (found)
	return (Position) y;
    }
    item = item->nextsibling;
  }
  TreeCheck(w, "exiting GetPosition");
  if (item != finditem)
    y = 0;
  return (Position) y;
}

/* Public Functions --------------------------------------------------------- */

void ListTreeRefresh(ListTreeWidget w)
{
  if (XtIsRealized((Widget) w) && w->list.Refresh)
    DrawChanged(w);
}

void ListTreeRefreshOff(ListTreeWidget w)
{
  w->list.Refresh = False;
}

void ListTreeRefreshOn(ListTreeWidget w)
{
  w->list.Refresh = True;
  ListTreeRefresh(w);
}

static ListTreeItem *AddItem(ListTreeWidget w, ListTreeItem * parent, char *string,
			     ListTreeItemType type)
{
  ListTreeItem *item;
  int len;
  char *copy;

  TreeCheck(w, "in ListTreeAdd");
  len = strlen(string);
  item = (ListTreeItem *) XtMalloc(sizeof(ListTreeItem));
  copy = (char *)XtMalloc(len + 1);
  strcpy(copy, string);
  item->text = copy;
  item->length = len;
  item->type = type;
  item->parent = parent;
  item->open = False;
  item->highlighted = False;
  item->openPixmap = item->closedPixmap = (Pixmap) NULL;
  item->firstchild = item->prevsibling = item->nextsibling = NULL;
  InsertChild(w, parent, item);

  ListTreeRefresh(w);

  return item;
}

ListTreeItem *ListTreeAdd(ListTreeWidget w, ListTreeItem * parent, char *string)
{
  return (AddItem(w, parent, string, ItemDetermineType));
}

ListTreeItem *ListTreeAddType(ListTreeWidget w, ListTreeItem * parent, char *string,
			      ListTreeItemType type)
{
  return (AddItem(w, parent, string, type));
}

ListTreeItem *ListTreeAddBranch(ListTreeWidget w, ListTreeItem * parent, char *string)
{
  return (AddItem(w, parent, string, ItemBranchType));
}

ListTreeItem *ListTreeAddLeaf(ListTreeWidget w, ListTreeItem * parent, char *string)
{
  return (AddItem(w, parent, string, ItemLeafType));
}

void
ListTreeSetItemPixmaps(ListTreeWidget w, ListTreeItem * item,
		       Pixmap openPixmap, Pixmap closedPixmap)
{
  item->openPixmap = openPixmap;
  item->closedPixmap = closedPixmap;
}

void ListTreeRenameItem(ListTreeWidget w, ListTreeItem * item, char *string)
{
  int len;
  char *copy;

  TreeCheck(w, "in ListTreeRename");
  XtFree(item->text);
  len = strlen(string);
  copy = (char *)XtMalloc(len + 1);
  strcpy(copy, string);
  item->text = copy;
  item->length = len;

  ListTreeRefresh(w);
}

int ListTreeDelete(ListTreeWidget w, ListTreeItem * item)
{
  if (item->firstchild)
    DeleteChildren(w, item->firstchild);
  item->firstchild = NULL;

  RemoveReference(w, item);

  XtFree((char *)item->text);
  XtFree((char *)item);

  ListTreeRefresh(w);

  return 1;
}

int ListTreeDeleteChildren(ListTreeWidget w, ListTreeItem * item)
{
  if (item->firstchild)
    DeleteChildren(w, item->firstchild);
  item->firstchild = NULL;

  ListTreeRefresh(w);

  return 1;
}

int ListTreeReparent(ListTreeWidget w, ListTreeItem * item, ListTreeItem * newparent)
{
  TreeCheck(w, "in ListTreeReparent");
/* Remove the item from its old location. */
  RemoveReference(w, item);

/* The item is now unattached.  Reparent it.                     */
  InsertChild(w, newparent, item);

  ListTreeRefresh(w);

  return 1;
}

int ListTreeReparentChildren(ListTreeWidget w, ListTreeItem * item, ListTreeItem * newparent)
{
  ListTreeItem *first;

  TreeCheck(w, "in ListTreeReparentChildren");
  if (item->firstchild) {
    first = item->firstchild;
    item->firstchild = NULL;

    InsertChildren(w, newparent, first);

    ListTreeRefresh(w);
    return 1;
  }
  return 0;
}

int AlphabetizeItems(const void *item1, const void *item2)
{
  return strcmp((*((ListTreeItem **) item1))->text, (*((ListTreeItem **) item2))->text);
}

int ListTreeUserOrderSiblings(ListTreeWidget w, ListTreeItem * item, int (*func) ())
{
  ListTreeItem *first, *parent, **list;
  size_t i, count, size;

  TreeCheck(w, "in ListTreeUserOrderSiblings");
/* Get first child in list; */
  while (item->prevsibling)
    item = item->prevsibling;
  first = item;
  parent = first->parent;

/* Count the children */
  count = 1;
  while (item->nextsibling)
    item = item->nextsibling, count++;
  if (count <= 1)
    return 1;

  size = sizeof(ListTreeItem *);
  list = (ListTreeItem **) XtMalloc(size * count);
  list[0] = first;
  count = 1;
  while (first->nextsibling) {
    list[count] = first->nextsibling;
    count++;
    first = first->nextsibling;
  }

  qsort(list, count, size, func);

  list[0]->prevsibling = NULL;
  for (i = 0; i < count; i++) {
    if (i < count - 1)
      list[i]->nextsibling = list[i + 1];
    if (i > 0)
      list[i]->prevsibling = list[i - 1];
  }
  list[count - 1]->nextsibling = NULL;
  if (parent)
    parent->firstchild = list[0];
  else
    w->list.first = list[0];
  XtFree((char *)list);

  ListTreeRefresh(w);
  TreeCheck(w, "exiting ListTreeOrderSiblings");

  return 1;
}

int ListTreeOrderSiblings(ListTreeWidget w, ListTreeItem * item)
{
  TreeCheck(w, "in ListTreeOrderSiblings");
  return ListTreeUserOrderSiblings(w, item, AlphabetizeItems);
}

int ListTreeUserOrderChildren(ListTreeWidget w, ListTreeItem * item, int (*func) ())
{
  ListTreeItem *first;

  TreeCheck(w, "in ListTreeUserOrderChildren");
  if (item) {
    first = item->firstchild;
    if (first)
      ListTreeUserOrderSiblings(w, first, func);
  } else {
    if (w->list.first)
      ListTreeUserOrderSiblings(w, w->list.first, func);
  }
  TreeCheck(w, "exiting ListTreeUserOrderChildren");
  return 1;
}

int ListTreeOrderChildren(ListTreeWidget w, ListTreeItem * item)
{
  ListTreeItem *first;

  TreeCheck(w, "in ListTreeOrderChildren");
  if (item) {
    first = item->firstchild;
    if (first)
      ListTreeOrderSiblings(w, first);
  } else {
    if (w->list.first)
      ListTreeOrderSiblings(w, w->list.first);
  }
  TreeCheck(w, "exiting ListTreeOrderChildren");
  return 1;
}

ListTreeItem *ListTreeFindSiblingName(ListTreeWidget w, ListTreeItem * item, char *name)
{
  TreeCheck(w, "in ListTreeFindSiblingName");
/* Get first child in list; */
  if (item) {
    while (item->prevsibling)
      item = item->prevsibling;

    while (item) {
      if (strcmp(item->text, name) == 0)
	return item;
      item = item->nextsibling;
    }
    return item;
  }
  return NULL;
}

ListTreeItem *ListTreeFindChildName(ListTreeWidget w, ListTreeItem * item, char *name)
{
  TreeCheck(w, "in ListTreeFindChildName");
/* Get first child in list; */
  if (item && item->firstchild) {
    item = item->firstchild;
  } else if (!item && w->list.first) {
    item = w->list.first;
  } else
    item = NULL;

  while (item) {
    if (strcmp(item->text, name) == 0)
      return item;
    item = item->nextsibling;
  }
  return NULL;
}

void ListTreeHighlightItem(ListTreeWidget w, ListTreeItem * item)
{
  HighlightAll(w, False, False);
  HighlightItem(w, item, True, False);
  ListTreeRefresh(w);
}

void ListTreeHighlightAll(ListTreeWidget w)
{
  HighlightAllVisible(w, True, False);
  ListTreeRefresh(w);
}

void ListTreeClearHighlighted(ListTreeWidget w)
{
  HighlightAll(w, False, False);
  ListTreeRefresh(w);
}

void ListTreeGetHighlighted(ListTreeWidget w, ListTreeMultiReturnStruct * ret)
{
  if (ret)
    MakeMultiCallbackStruct(w, ret);
}

void ListTreeSetHighlighted(ListTreeWidget w, ListTreeItem ** items, int count, Boolean clear)
{
  if (clear)
    HighlightAll(w, False, False);
  if (count < 0) {
    while (*items) {
      HighlightItem(w, *items, True, False);
      items++;
    }
  } else {
    int i;

    for (i = 0; i < count; i++) {
      HighlightItem(w, items[i], True, False);
    }
  }
  ListTreeRefresh(w);
}

ListTreeItem *ListTreeFirstItem(w)
ListTreeWidget w;
{
  ListTreeItem *first;

/* Get first child in widget */
  first = w->list.first;
  return first;
}

Position ListTreeGetItemPosition(ListTreeWidget w, ListTreeItem * item)
{
  return GetPosition(w, item);
}

void ListTreeGetPathname(ListTreeReturnStruct * ret, char *dir)
{
  int count;

  if (*ret->path[0]->text != '/')
    strcpy(dir, "/");
  else
    strcpy(dir, "");
  strcat(dir, ret->path[0]->text);
  count = 1;
  while (count < ret->count) {
    strcat(dir, "/");
    strcat(dir, ret->path[count]->text);
    count++;
  }
}

void ListTreeGetPathnameFromItem(ListTreeItem * item, char *dir)
{
  char tmppath[1024];

  *dir = '\0';
  while (item) {
    sprintf(tmppath, "/%s%s", item->text, dir);
    strcpy(dir, tmppath);
    item = item->parent;
  }
}

Widget XmCreateScrolledListTree(Widget parent, char *name, Arg * args, Cardinal count)
{
  Widget sw;
  char *sname;
  Cardinal i;
  Arg *al;

  sname = XtMalloc(strlen(name) + 3);
  strcpy(sname, name);
  strcat(sname, "SW");

  al = (Arg *) XtCalloc(count + 4, sizeof(Arg));
  for (i = 0; i < count; i++) {
    al[i].name = args[i].name;
    al[i].value = args[i].value;
  }

  XtSetArg(al[i], XmNscrollingPolicy, XmAPPLICATION_DEFINED);
  i++;
  XtSetArg(al[i], XmNvisualPolicy, XmVARIABLE);
  i++;
  XtSetArg(al[i], XmNscrollBarDisplayPolicy, XmSTATIC);
  i++;
  XtSetArg(al[i], XmNshadowThickness, 0);
  i++;

  sw = XtCreateManagedWidget(sname, xmScrolledWindowWidgetClass, parent, al, i);
  XtFree((XtPointer) al);

  return XtCreateWidget(name, listtreeWidgetClass, sw, args, count);
}
