/*  CMS REPLACEMENT HISTORY, Element XMDSXDBOX.H */
/*  *12   18-MAR-1997 09:53:37 TWF "Fix for DECC 5.3" */
/*  *11   18-MAR-1997 09:53:18 TWF "Fix for DECC 5.3" */
/*  *10   20-SEP-1994 08:29:30 TWF "Use DECC" */
/*  *9     8-APR-1993 09:49:17 TWF "Add prototypes" */
/*  *8     3-MAR-1993 14:57:01 JAS "Add prottypes" */
/*  *7    24-FEB-1993 12:08:04 JAS "port to decc" */
/*  *6    24-FEB-1993 11:21:22 JAS "port to decc" */
/*  *5    24-FEB-1993 10:41:49 JAS "port to decc" */
/*  *4    21-JAN-1992 12:07:41 JAS "add XmdsXdBoxSetState" */
/*  *3    16-JAN-1992 12:32:46 JAS "add putonapply" */
/*  *2    16-JAN-1992 12:12:59 JAS "add apply" */
/*  *1    16-JAN-1992 11:00:35 JAS "public include for xdbox widget" */
/*  CMS REPLACEMENT HISTORY, Element XMDSXDBOX.H */
/*------------------------------------------------------------------------------

                Name:   XmdsXdBox.H

                Type:   C include

                Author:	Josh Stillerman

                Date:   11-NOV-1991

                Purpose: Public declarations for  XdBox Widgets.


------------------------------------------------------------------------------
   Copyright (c) 1991
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
----------------------------------------------------------------------------*/

#ifndef _XmdsXdBox_H
#define _XmdsXdBox_H

#ifndef _Xm_H
#include <Xm/Xm.h>
#include <Xmds/XmdsCallbacks.h>
#endif

#ifndef XmdsIsXdBox
#define XmdsIsXdBox(w) XtIsSubclass(w, xmdsXdBoxWidgetClass)
#endif /* XmdsIsXdBox */

/* Xd Widget Class */
externalref WidgetClass xmdsXdBoxWidgetClass;
externalref struct _XmdsXdBoxClassRec xmdsXdBoxClassRec;

typedef struct _XmdsXdBoxClassRec *XmdsXdBoxWidgetClass;
typedef struct _XmdsXdBoxWidgetRec *XmdsXdBoxWidget;

#ifdef _NO_PROTO_

extern Widget XmdsCreateXdBox();
extern Boolean XmdsXdBoxApply();
extern Boolean XmdsXdBoxGetState();
extern struct descriptor *XmdsXdBoxGetXd();
extern void XmdsXdBoxLoad();
extern Boolean XmdsXdBoxPut();
extern void XmdsXdBoxReset();
extern void XmdsXdBoxSetDefaultNid();
extern void XmdsXdBoxSetNid();
extern void XmdsXdBoxSetState();
extern void XmdsXdBoxSetXd();

#else /* _NO_PROTO_ */

extern Widget XmdsCreateXdBox(Widget parent, char *name, ArgList args,
                              Cardinal argcount);
extern Boolean XmdsXdBoxApply(Widget w);
extern Boolean XmdsXdBoxGetState(Widget w);
extern struct descriptor *XmdsXdBoxGetXd(Widget w);
extern void XmdsXdBoxLoad(Widget w);
extern Boolean XmdsXdBoxPut(Widget w);
extern void XmdsXdBoxReset(Widget w);
extern void XmdsXdBoxSetDefaultNid(Widget w, int def_nid);
extern void XmdsXdBoxSetNid(Widget w, int nid);
extern void XmdsXdBoxSetState(Widget w, Boolean state);
extern void XmdsXdBoxSetXd(Widget w, struct descriptor *dsc);

#endif /* _NO_PROTO_ */

/* Resources */

#define XmdsNapplyCallback "applyCallback"
#define XmdsNcancelCallback "cancelCallback"
#define XmdsNokCallback "okCallback"
#define XmdsNresetCallback "resetCallback"

#define XmdsNautoDestroy "autoDestroy"
#define XmdsNautoUnmanage "autoUnmanage"
#define XmdsNautoPut "autoPut"
#define XmdsNputOnApply "putOnApply"

#define XmdsNdefaultNid "defaultNid"
#define XmdsNdisplayOnly "displayOnly"
#define XmdsNnid "nid"
#define XmdsNnidOffset "nidOffset"
#define XmdsNshowButtons "showButtons"
#define XmdsNusage "usage"
#define XmdsNxd "xd"

#include <usagedef.h>
#endif /* _XmdsXdBox_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
