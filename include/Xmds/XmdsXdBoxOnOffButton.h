/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSXDBOXONOFFBUTTON.H */
/*  *3     3-MAR-1993 14:54:39 JAS "Add prottypes" */
/*  *2    24-FEB-1993 12:36:30 JAS "port to decc" */
/*  *1    15-JAN-1992 14:27:35 TWF "XmdsXdBoxOnOffButton widget" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSXDBOXONOFFBUTTON.H */
/*------------------------------------------------------------------------------

		Name:   XmdsXdBoxOnOffButton.H

		Type:   C include

		Author:	Thomas W. Fredian

		Date:   15-JAN-1992

		Purpose: Public declarations for XmdsXdBoxOnOffButton widget

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
----------------------------------------------------------------------------*/

#ifndef _XmdsXdBoxOnOffButton_h
#define _XmdsXdBoxOnOffButton_h

#ifndef _Xm_h
#include <Xm/Xm.h>
#endif

/* Resources */

#define XmdsNnid               "nid"	/* Nid */
#define XmdsNnidOffset         "nidOffset"	/* Nid offset */
#define XmdsNputOnApply        "putOnApply"	/* Put when a XmdsApplyAllXds is called */

/* External creation routines:
*/
#ifdef _NO_PROTO

extern Widget XmdsCreateXdBoxOnOffButton();
extern Boolean XmdsIsXdBoxOnOffButton();
extern int XmdsXdBoxOnOffButtonApply();
extern int XmdsXdBoxOnOffButtonPut();
extern void XmdsXdBoxOnOffButtonReset();

#else				/* _NO_PROTO */

extern Widget XmdsCreateXdBoxOnOffButton(Widget parent, String name, ArgList args,
					 Cardinal argcount);
extern Boolean XmdsIsXdBoxOnOffButton(Widget w);
extern int XmdsXdBoxOnOffButtonApply(Widget w);
extern int XmdsXdBoxOnOffButtonPut(Widget w);
extern void XmdsXdBoxOnOffButtonReset(Widget w);

#endif				/* _NO_PROTO */

#endif				/* _XmdsXdBoxOnOffButton */
