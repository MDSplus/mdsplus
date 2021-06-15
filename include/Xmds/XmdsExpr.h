/*  CMS REPLACEMENT HISTORY, Element XMDSEXPR.H */
/*  *11   18-MAR-1997 09:51:12 TWF "Fix for DECC 5.3" */
/*  *10   18-MAR-1997 09:44:51 TWF "Fix for DECC 5.3" */
/*  *9    22-FEB-1994 10:46:07 TWF "Take out NO_X_GLBS" */
/*  *8     3-MAR-1993 13:40:49 JAS "Add prottypes" */
/*  *7    24-FEB-1993 15:27:16 JAS "port to decc" */
/*  *6    13-JAN-1992 12:39:22 TWF "Add XmdsExprApply" */
/*  *5    10-JAN-1992 13:01:30 JAS "Add put on apply and fix some stuff" */
/*  *4     9-JAN-1992 17:44:32 JAS "Add Geometry Stuff" */
/*  *3     9-DEC-1991 12:20:22 TWF "Check out XmdsExprSetNid" */
/*  *2     6-DEC-1991 10:10:17 TWF "XmdsExprReset declaration" */
/*  *1     5-DEC-1991 15:12:37 TWF "Expr Widget" */
/*  CMS REPLACEMENT HISTORY, Element XMDSEXPR.H */
/*------------------------------------------------------------------------------

                Name:   EXPR.H

                Type:   C include

                Author:	Josh Stillerman

                Date:   23-NOV-1989

                Purpose: Public declarations for  expr widget

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
----------------------------------------------------------------------------*/

#ifndef _XmdsExpr_H
#define _XmdsExpr_H

#include <Xm/Xm.h>

#ifndef XmdsIsExpr
#define XmdsIsExpr(w) XtIsSubclass(w, xmdsExprWidgetClass)
#endif /* XmdsIsExpr */

/* XmdsExpr Widget Class */
externalref WidgetClass xmdsExprWidgetClass;

typedef struct _XmdsExprClassRec *XmdsExprWidgetClass;
typedef struct _XmdsExprWidgetRec *XmdsExprWidget;

#ifdef _NO_PROTO

extern Widget XmdsCreateExpr();
extern struct descriptor *XmdsExprGetXd();
extern Boolean XmdsExprPut();
extern Boolean XmdsExprApply();
extern void XmdsExprRegister();
void XmdsExprReset();
void XmdsExprSetDefaultNid();
extern void XmdsExprSetNid();
extern void XmdsExprSetXd();

#else /* _NO_PROTO */

extern Widget XmdsCreateExpr(Widget parent, char *name, ArgList args,
                             Cardinal argcount);
extern struct descriptor *XmdsExprGetXd(Widget w);
extern Boolean XmdsExprPut(Widget w);
extern Boolean XmdsExprApply(Widget w);
extern void XmdsExprRegister();
void XmdsExprReset(Widget w);
void XmdsExprSetDefaultNid(Widget w, int nid);
extern void XmdsExprSetNid(Widget w, int nid, int offset);
extern void XmdsExprSetXd(Widget w, struct descriptor *dsc);

#endif /* _NO_PROTO */

/* Resources */

#define XmdsNdefaultNid "defaultNid"
#define XmdsNnid "nid"
#define XmdsNnidOffset "nidOffset"
#define XmdsNxd "xd"
#define XmdsNcompile "compile"
#define XmdsNdecompile "decompile"
#define XmdsNautoQuote "autoQuote"
#define XmdsNdefaultQuote "defaultQuote"
#define XmdsNputOnApply "putOnApply"

/* Class record pointer */

externalref WidgetClass xmdsExprWidgetClass;
externalref struct _XmdsExprClassRec xmdsExprClassRec;

#endif /* _XmdsExpr_H */
/* DON'T ADD ANYTHING AFTER THIS #endif */
