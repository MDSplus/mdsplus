/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSEXPRFIELD.H */
/*  *3     3-MAR-1993 13:45:12 JAS "Add prottypes" */
/*  *2    13-JAN-1992 12:39:33 TWF "Add XmdsExprFieldApply" */
/*  *1    10-JAN-1992 14:53:55 JAS "ExprField public include" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSEXPRFIELD.H */
/*------------------------------------------------------------------------------

                Name:   XmdsExprField.H

                Type:   C include

                Author:	Josh Stillerman

                Date:   23-NOV-1989

                Purpose: Public declarations for  XmdsExprField widget

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
----------------------------------------------------------------------------*/

#ifndef _XmdsExprField_H
#define _XmdsExprField_H

#ifndef _Xm_H
#include <Xm/Xm.h>
#endif

#ifndef XmdsIsExprField
#define XmdsIsExprField(w) XtIsSubclass(w, xmdsExprFieldWidgetClass)
#endif /* XmdsIsExprField */

/* XmdsExprField Widget Class */
externalref WidgetClass xmdsExprFieldWidgetClass;

typedef struct _XmdsExprFieldClassRec *XmdsExprFieldWidgetClass;
typedef struct _XmdsExprFieldWidgetRec *XmdsExprFieldWidget;

#ifdef _NO_PROTO

extern Widget XmdsCreateExprField();
extern Boolean XmdsExprFieldApply();
extern struct descriptor *XmdsExprFieldGetXd();
extern Boolean XmdsExprFieldPut();
extern void XmdsExprFieldRegister();
extern void XmdsExprFieldReset();
extern void XmdsExprFieldSetDefaultNid();
extern void XmdsExprFieldSetNid();
extern void XmdsExprFieldSetXd();

#else /* _NO_PROTO */

extern Widget XmdsCreateExprField(Widget parent, char *name, ArgList args,
                                  Cardinal argcount);
extern Boolean XmdsExprFieldApply(Widget w);
extern struct descriptor *XmdsExprFieldGetXd(Widget w);
extern Boolean XmdsExprFieldPut(Widget w);
extern void XmdsExprFieldRegister();
extern void XmdsExprFieldReset(Widget w);
extern void XmdsExprFieldSetDefaultNid(Widget w, int nid);
extern void XmdsExprFieldSetNid(Widget w, int nid, int offset);
extern void XmdsExprFieldSetXd(Widget w, struct descriptor *dsc);

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

externalref WidgetClass xmdsExprFieldWidgetClass;

#endif /* _XmdsExprField_H */
/* DON'T ADD ANYTHING AFTER THIS #endif */
