/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSPATH.H */
/*  *2     3-MAR-1993 14:02:27 JAS "Add prottypes" */
/*  *1     6-MAY-1992 14:48:15 JAS "include file for path widget" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSPATH.H */
/*------------------------------------------------------------------------------

                Name:   XmdsPath.H

                Type:   C include

                Author:	Josh Stillerman

                Date:   6-MAY-1992

                Purpose: Public declarations for XmdsPath widget

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
----------------------------------------------------------------------------*/

#ifndef _XmdsPath_h
#define _XmdsPath_h

#ifndef _Xm_h
#include <Xm/Xm.h>
#endif

typedef enum pathType
{
  NciABSOLUTE_PATH,
  NciRELATIVE_PATH
} XmdsPathType;
/* Resources */

#define XmdsNnid "nid"             /* Nid of first channel */
#define XmdsNnidOffset "nidOffset" /* Nid offset */
#define XmdsNpathType "pathType"   /* TreeABS_PATH or TreeREL_PATH */

/* External creation routines:
 */
#ifdef _NO_PROTO

extern Widget XmdsCreatePath();
extern Boolean XmdsIsPath();

#else /* _NO_PROTO */

extern Widget XmdsCreatePath(Widget parent, String name, ArgList args,
                             Cardinal argcount);
extern Boolean XmdsIsPath(Widget w);

#endif /* _NO_PROTO */

#endif /* _XmdsPath */
