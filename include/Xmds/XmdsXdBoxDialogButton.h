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
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSXDBOXDIALOGBUTTON.H */
/*  *4     3-MAR-1993 14:52:48 JAS "Add prottypes" */
/*  *3    24-FEB-1993 17:40:53 JAS "port to DECC" */
/*  *2    24-FEB-1993 12:30:41 JAS "port to decc" */
/*  *1    15-JAN-1992 12:33:43 TWF "Add XmdsXdBoxDialogButton" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSXDBOXDIALOGBUTTON.H */
/*------------------------------------------------------------------------------

		Name:   XmdsXdBoxDialogButton.H

		Type:   C include

		Author:	Thomas W. Fredian

		Date:   15-JAN-1992

		Purpose: Public declarations for XmdsXdBoxDialogButton widget

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
----------------------------------------------------------------------------*/

#ifndef _XmdsXdBoxDialogButton_h
#define _XmdsXdBoxDialogButton_h

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

extern Widget XmdsCreateXdBoxDialogButton();
extern Boolean XmdsIsXdBoxDialogButton();
extern int XmdsXdBoxDialogButtonApply();
extern Widget XmdsXdBoxDialogButtonGetXdBox();
extern int XmdsXdBoxDialogButtonPut();
extern void XmdsXdBoxDialogButtonReset();

#else				/* _NO_PROTO */

extern Widget XmdsCreateXdBoxDialogButton(Widget parent, String name, ArgList args,
					  Cardinal argcount);
extern Boolean XmdsIsXdBoxDialogButton(Widget w);
extern int XmdsXdBoxDialogButtonApply(Widget w);
extern Widget XmdsXdBoxDialogButtonGetXdBox(Widget w);
extern int XmdsXdBoxDialogButtonPut(Widget w);
extern void XmdsXdBoxDialogButtonReset(Widget w);

#endif				/* _NO_PROTO */

#endif				/* _XmdsXdBoxDialogButton */
