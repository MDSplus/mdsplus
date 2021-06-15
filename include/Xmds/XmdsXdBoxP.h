/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSXDBOXP.H */
/*  *9    23-JUN-1994 10:23:25 TWF "Motif" */
/*  *8    23-JUN-1994 10:12:02 TWF "Motif 1.2" */
/*  *7     2-MAR-1993 12:09:54 JAS "port to decc" */
/*  *6    23-NOV-1992 16:27:44 JAS "Add a loaded flag to the widget" */
/*  *5    30-JAN-1992 17:40:04 JAS "Make it a manager" */
/*  *4    28-JAN-1992 15:16:32 JAS "add some more data types" */
/*  *3    17-JAN-1992 15:23:06 JAS "Fix generic stuff" */
/*  *2    16-JAN-1992 12:32:07 JAS "add putonapply" */
/*  *1    16-JAN-1992 11:02:05 JAS "Private include for XmdsXdBox widgets" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSXDBOXP.H */
/*------------------------------------------------------------------------------

                Name:   XmdsXdBoxP.H

                Type:   C include

                Author:	Josh Stillerman

                Date:   12-NOV-1991

                Purpose: Private declarations for  XdBox widget class widgets.

------------------------------------------------------------------------------
   Copyright (c) 1991
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
----------------------------------------------------------------------------*/

#ifndef XmdsXdBoxP_H
#define XmdsXdBoxP_H

#include <Mrm/MrmPublic.h>
#include <Xm/XmP.h>
#include <Xmds/XmdsCallbacks.h>
#include <Xmds/XmdsXdBox.h>

#ifndef XmMANAGER
#include <Xm/ManagerP.h>
#endif

/* New fields for the  Xd Widget record */

typedef struct _XmdsXdUserPart
{
  void (*load_dlog_proc)();
  struct descriptor_xd *(*unload_dlog_proc)();
} XmdsXdUserPart;

typedef struct _XmdsXdBoxPart
{
  int nid;
  int nid_offset;
  int default_nid;
  unsigned char usage;
  struct descriptor_xd *xd;

  Boolean show_buttons;
  Boolean display_only;

  Boolean auto_destroy;
  Boolean auto_put;
  Boolean auto_unmanage;
  Boolean put_on_apply;

  XtCallbackList apply_callback;
  XtCallbackList cancel_callback;
  XtCallbackList ok_callback;
  XtCallbackList reset_callback;

  Widget xdb_dlog;
  Widget specific_dlog;

  Boolean on_off;
  Boolean parent_on_off;
  char *tag_list;
  char *path;
  Boolean loaded;
} XmdsXdBoxPart;

/*
 * now define the actual widget data struct
 */
typedef struct _XmdsXdBoxWidgetRec
{
  CorePart core;           /*  basic widget */
  CompositePart composite; /*  composite specific data */
  ConstraintPart constraint;
  XmManagerPart manager;
  XmdsXdBoxPart xdbox; /*  Xd specific */
} XmdsXdBoxWidgetRec;

/*
 * Xd Class part contains the DRM Hierarchy for the xd
 * dialog box.
 */

typedef struct _XmdsXdBoxClassPart
{
  MrmHierarchy drm;
  caddr_t extension; /* Pointer to extension record */
} XmdsXdBoxClassPart;

/*
 * The XdBox Class record is a composite class plus
 * the XdBoxClassPart
 */
typedef struct _XmdsXdBoxClassRec
{
  CoreClassPart core_class;
  CompositeClassPart composite_class;
  ConstraintClassPart constraint_class;
  XmManagerClassPart manager_class;
  XmdsXdBoxClassPart xdbox_class;
} XmdsXdBoxClassRec;

externalref XmdsXdBoxClassRec xmdsXdBoxClassRec;

#endif /* XmdsXdBoxP_H */
/* DON'T ADD STUFF AFTER THIS #endif */
