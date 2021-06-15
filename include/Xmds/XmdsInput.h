/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSINPUT.H */
/*  *2     3-MAR-1993 13:51:46 JAS "Add prottypes" */
/*  *1    24-FEB-1993 14:41:05 JAS "include file for xmdsinput widget" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSINPUT.H */
/*------------------------------------------------------------------------------

                Name:   XmdsInput

                Type:   C include file

                Author:	JOSH STILLERMAN

                Date:    24-FEB-1993

                Purpose: Defines the data structures and routines for
                         generic Input or Channel support.

--------------------------------------------------------------------------------

   Copyright (c) 1993
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.

--------------------------------------------------------------------------------
  Type declarations :                                                         */

/*------------------------------------------------------------------------------
  External Entry points                                                       */

#ifndef _XmdsInput_H
#define _XmdsInput_H

#ifdef _NO_PROTO

extern Widget XmdsCreateInput();
extern void XmdsInputSetNid();
extern void XmdsInputReset();
extern void XmdsInputPut();

#else

extern Widget XmdsCreateInput(Widget parent, char *name, ArgList args,
                              int argcount);
extern void XmdsInputSetNid(Widget w, int nid);
extern void XmdsInputReset(Widget w);
extern void XmdsInputPut(Widget w);

#endif
#endif
