/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSEXPRFIELDP.H */
/*  *1    10-JAN-1992 14:54:12 JAS "ExprField private include" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSEXPRFIELDP.H */
/*------------------------------------------------------------------------------

		Name:   XmdsExprFieldP.H

		Type:   C include

     		Author:	Josh Stillerman

		Date:   23-AUG-1989

    		Purpose: Private declarations for  ExprField widget

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
----------------------------------------------------------------------------*/

#ifndef XmdsExprFieldP_H
#define XmdsExprFieldP_H

#ifdef VMS
#include <DECW$INCLUDE:XmP.h>
#include <Xmds:XmdsExprField.h>
#else
#include <Xm/XmP.h>
#include <Xmds/XmdsExprField.h>
#endif

/* New fields for the  ExprField Widget record */

typedef struct _XmdsExprFieldPart
{
    struct dsc$descriptor_xd *xd;
    int			  nid;
    int                   nid_offset;
    int                   default_nid;

    Boolean		(*compile)();
    Boolean		(*decompile)();

    Widget		open_quote_widget;
    Widget		text_widget;
    Widget		close_quote_widget;
    Boolean		is_text;
    Boolean		auto_quote;
    Boolean		default_quote;
    Boolean		put_on_apply;
    int extension;
} XmdsExprFieldPart;

/*
 * now define the actual widget data struct
 */
typedef struct _XmdsExprFieldWidgetRec {
    CorePart		    core;		/* basic widget */
    CompositePart           composite;
    ConstraintPart	    constraint;
    XmManagerPart           manager;
    XmdsExprFieldPart	    expr_field;		/*  ExprField specific */
} XmdsExprFieldWidgetRec;

/*
 * ExprField Class part is empty.
 */

typedef struct _XmdsExprFieldClassPart{
    caddr_t		extension;	/* Pointer to extension record */
}   XmdsExprFieldClassPart;


/*
 * The ExprField Class record is a Dialog box class record plus
 * the empty ExprFieldClassPart
*/
typedef struct _ExprFieldClassRec 
{
    CoreClassPart	    core_class;
    CompositeClassPart      composite;
    ConstraintClassPart	    constraint;
    XmManagerClassPart      manager;
    XmdsExprFieldClassPart  expr_field_class;
} XmdsExprFieldClassRec;

externalref XmdsExprFieldClassRec xmdsExprFieldClassRec;

#endif /* XmdsExprFieldP_H */
/* DON'T ADD ANYTHING AFTER THIS #endif */
