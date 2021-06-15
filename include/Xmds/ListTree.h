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

#ifndef _ListTree_H
#define _ListTree_H

#include <X11/Core.h>
#include <Xm/Xm.h>
#include <Xm/XmP.h>

#define _ListTree_WIDGET_VERSION 3.0

#define XtNmargin "margin"
#define XtNindent "indent"
#define XtNspacing "spacing"
#define XtNhorizontalSpacing "horizontalSpacing"
#define XtNverticalSpacing "verticalSpacing"
#define XtNlineWidth "lineWidth"
#define XtNhighlightPath "highlightPath"
#define XtNclickPixmapToOpen "clickPixmapToOpen"
#define XtNdoIncrementalHighlightCallback "incCallback"
#define XtNbranchPixmap "branchPixmap"
#define XtNbranchOpenPixmap "branchOpenPixmap"
#define XtNleafPixmap "leafPixmap"
#define XtNleafOpenPixmap "leafOpenPixmap"
#define XtNactivateCallback "activateCallback"
#define XtNhighlightCallback "highlightCallback"
#define XtNmenuCallback "menuCallback"
#define XtNdestroyItemCallback "destroyItemCallback"

#define XtBRANCH 1
#define XtLEAF 2
#define XtMENU 3
#define XtDESTROY 4

#ifdef __cplusplus
extern "C"
{
#endif

  extern WidgetClass listtreeWidgetClass;

  typedef struct _ListTreeClassRec *ListTreeWidgetClass;
  typedef struct _ListTreeRec *ListTreeWidget;

  typedef enum _ListTreeItemType
  {
    ItemDetermineType = 0,
    ItemBranchType = XtBRANCH,
    ItemLeafType = XtLEAF
  } ListTreeItemType;

  typedef struct _ListTreeItem
  {
    Boolean open;
    Boolean highlighted;
    char *text;
    int length;
    int x, y, ytext;
    int count;
    Dimension height;
    ListTreeItemType type;
    struct _ListTreeItem *parent, *firstchild, *prevsibling, *nextsibling;
    Pixmap openPixmap, closedPixmap;
    XtPointer user_data;
  } ListTreeItem;

  typedef struct _ListTreeReturnStruct
  {
    int reason;
    ListTreeItem *item;
    ListTreeItem **path;
    int count;
    Boolean open;
  } ListTreeReturnStruct;

  typedef struct _ListTreeMultiReturnStruct
  {
    ListTreeItem **items;
    int count;
  } ListTreeMultiReturnStruct;

  typedef struct _ListTreeActivateStruct
  {
    int reason;
    ListTreeItem *item;
    Boolean open;
    ListTreeItem **path;
    int count;
  } ListTreeActivateStruct;

  typedef struct _ListTreeItemReturnStruct
  {
    int reason;
    ListTreeItem *item;
    XEvent *event;
  } ListTreeItemReturnStruct;

#ifdef __cplusplus
};
#endif

/*
** Public function declarations
*/
#ifndef _ListTree_
#if __STDC__ || defined(__cplusplus)
#define P_(s) s
#else
#define P_(s) ()
#endif

/* ListTree.c */
void ListTreeRefresh P_((Widget w));
void ListTreeRefreshOff P_((Widget w));
void ListTreeRefreshOn P_((Widget w));
ListTreeItem *ListTreeAdd P_((Widget w, ListTreeItem *parent, char *string));
ListTreeItem *ListTreeAddType P_((Widget w, ListTreeItem *parent, char *string,
                                  ListTreeItemType type));
ListTreeItem *ListTreeAddBranch P_((Widget w, ListTreeItem *parent,
                                    char *string));
ListTreeItem *ListTreeAddLeaf P_((Widget w, ListTreeItem *parent,
                                  char *string));
void ListTreeSetItemPixmaps P_((Widget w, ListTreeItem *item, Pixmap openPixmap,
                                Pixmap closedPixmap));
void ListTreeRenameItem P_((Widget w, ListTreeItem *item, char *string));
int ListTreeDelete P_((Widget w, ListTreeItem *item));
int ListTreeDeleteChildren P_((Widget w, ListTreeItem *item));
int ListTreeReparent P_((Widget w, ListTreeItem *item,
                         ListTreeItem *newparent));
int ListTreeReparentChildren P_((Widget w, ListTreeItem *item,
                                 ListTreeItem *newparent));
int ListTreeOrderSiblings P_((Widget w, ListTreeItem *item));
int ListTreeOrderChildren P_((Widget w, ListTreeItem *item));
ListTreeItem *ListTreeFindSiblingName P_((Widget w, ListTreeItem *item,
                                          char *name));
ListTreeItem *ListTreeFindChildName P_((Widget w, ListTreeItem *item,
                                        char *name));
void ListTreeHighlightItem P_((Widget w, ListTreeItem *item));
ListTreeItem *ListTreeFirstItem P_((Widget w));
#ifdef USE_RDD
void ListTreeHighlightDrop P_((Widget w, XEvent *event, String *params,
                               Cardinal *num_params));
ListTreeReturnStruct *ListTreeGetDrop P_((Widget w));
#endif
Widget XmCreateScrolledListTree P_((Widget parent, char *name, Arg *args,
                                    Cardinal count));

#undef P_
#else

extern EXPORT void ListTreeRenameItem(ListTreeWidget w, ListTreeItem *item,
                                      char *string);
extern EXPORT void ListTreeRefresh(ListTreeWidget w);
extern EXPORT void ListTreeRefreshOff(ListTreeWidget w);
extern EXPORT void ListTreeRefreshOn(ListTreeWidget w);
extern EXPORT ListTreeItem *ListTreeFirstItem(ListTreeWidget w);
extern EXPORT int ListTreeDelete(ListTreeWidget w, ListTreeItem *item);
extern EXPORT void ListTreeSetItemPixmaps(ListTreeWidget w, ListTreeItem *item,
                                          Pixmap openPixmap,
                                          Pixmap closedPixmap);
extern EXPORT ListTreeItem *
ListTreeFindChildName(ListTreeWidget w, ListTreeItem *item, char *name);
extern EXPORT int ListTreeOrderChildren(ListTreeWidget w, ListTreeItem *item);
extern EXPORT ListTreeItem *ListTreeAdd(ListTreeWidget w, ListTreeItem *parent,
                                        char *string);

#endif

#endif /* _ListTree_H */
