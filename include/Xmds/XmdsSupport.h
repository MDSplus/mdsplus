/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSSUPPORT.H */
/*  *4    22-FEB-1993 11:35:50 JAS "port to decc" */
/*  *3    22-FEB-1993 11:15:34 JAS "port to decc" */
/*  *2    16-JAN-1992 18:36:45 JAS "Add a putifChanged" */
/*  *1     5-DEC-1991 15:28:43 TWF "Support modules" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSSUPPORT.H */
/*------------------------------------------------------------------------------

                Name:   XMDSSUPPORT.H

                Type:   C Include file

                Author:	JOSH STILLERMAN

                Date:   16-JAN-1992

                Purpose: Include file for XmdsSupport routines

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

        Description:

------------------------------------------------------------------------------*/
#ifndef XmdsSupport_h
#include <X11/Intrinsic.h>
#include <mdsdescrip.h>
extern Boolean ConglomerateElt(int nid);
extern int ConglomerateHead(int nid);
extern int DefaultNid(int nid);
extern int NodeParent(int nid);
extern void ResetErrors();
extern void TdiComplain(Widget w);
extern struct descriptor *TdiGet(int nid);
extern Boolean PutIfChanged(int nid, struct descriptor_xd *xd);
#endif /* XmdsSupport_h */
