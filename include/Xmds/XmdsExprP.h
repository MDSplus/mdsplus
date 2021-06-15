/*  CMS REPLACEMENT HISTORY, Element XMDSEXPRP.H */
/*  *5    18-MAR-1997 09:59:16 TWF "Fix for DECC 5.3" */
/*  *4    23-JUN-1994 09:27:39 TWF "Motif 1.2" */
/*  *3    10-JAN-1992 13:01:52 JAS "Add put on apply and fix some stuff" */
/*  *2     9-JAN-1992 17:44:50 JAS "Add Geometry Stuff" */
/*  *1     5-DEC-1991 15:13:22 TWF "Expr Widget" */
/*  CMS REPLACEMENT HISTORY, Element XMDSEXPRP.H */
/*------------------------------------------------------------------------------

                Name:   XmdsExprP.H

                Type:   C include

                Author:	Josh Stillerman

                Date:   23-AUG-1989

                Purpose: Private declarations for  Expr widget

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
----------------------------------------------------------------------------*/

#ifndef XmdsExprP_H
#define XmdsExprP_H

#include <Xm/XmP.h>
#include <Xmds/XmdsExpr.h>

#ifndef XmMANAGER
#include <Xm/ManagerP.h>
#endif

/* New fields for the  Expr Widget record */

typedef struct _XmdsExprPart
{
  struct descriptor_xd *xd;
  int nid;
  int nid_offset;
  int default_nid;

  Boolean (*compile)();
  Boolean (*decompile)();

  Widget open_quote_widget;
  Widget text_widget;
  Widget close_quote_widget;
  Boolean is_text;
  Boolean auto_quote;
  Boolean default_quote;
  Boolean put_on_apply;
  int extension;
} XmdsExprPart;

/*
 * now define the actual widget data struct
 */
typedef struct _XmdsExprWidgetRec
{
  CorePart core; /* basic widget */
  CompositePart composite;
  ConstraintPart constraint;
  XmManagerPart manager;
  XmdsExprPart expr; /*  Expr specific */
} XmdsExprWidgetRec;

/*
 * Expr Class part is empty.
 */

typedef struct _XmdsExprClassPart
{
  caddr_t extension; /* Pointer to extension record */
} XmdsExprClassPart;

/*
 * The Expr Class record is a Dialog box class record plus
 * the empty ExprClassPart
 */
typedef struct _XmdsExprClassRec
{
  CoreClassPart core_class;
  CompositeClassPart composite;
  ConstraintClassPart constraint;
  XmManagerClassPart manager;
  XmdsExprClassPart expr_class;
} XmdsExprClassRec;

externalref XmdsExprClassRec xmdsExprClassRec;

#endif /* XmdsExprP_H */
/* DON'T ADD ANYTHING AFTER THIS #endif */
