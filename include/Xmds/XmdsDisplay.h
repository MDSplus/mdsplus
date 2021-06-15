/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSDISPLAY.H */
/*  *2     3-MAR-1993 13:36:28 JAS "Add prottypes" */
/*  *1    19-JAN-1993 09:30:02 JAS "new widget to display decompile(value())" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSDISPLAY.H */
/*------------------------------------------------------------------------------

                Name:   XmdsDisplay.H

                Type:   C include

                Author:	Josh Stillerman

                Date:   6-MAY-1992

                Purpose: Public declarations for XmdsDisplay widget

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
----------------------------------------------------------------------------*/

#ifndef _XmdsDisplay_h
#define _XmdsDisplay_h

#ifndef _Xm_h
#include <Xm/Xm.h>
#endif

/* Resources */

#define XmdsNnid "nid"             /* Nid of first channel */
#define XmdsNnidOffset "nidOffset" /* Nid offset */

/* External creation routines:
 */
#ifdef _NO_PROTO

extern Widget XmdsCreateDisplay();
extern Boolean XmdsIsDisplay();

#else /* _NO_PROTO */

extern Widget XmdsCreateDisplay(Widget parent, String name, ArgList args,
                                Cardinal argcount);
extern Boolean XmdsIsDisplay(Widget w);

#endif /* _NO_PROTO */

#endif /* _XmdsDisplay */
