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

#define XmdsNnid		    "nid"	/* Nid of first channel */
#define XmdsNnidOffset		    "nidOffset"	/* Nid offset */

/* External creation routines:
*/
#ifdef _NO_PROTO

extern Widget XmdsCreateDisplay();
extern Boolean XmdsIsDisplay();

#else				/* _NO_PROTO */

extern Widget XmdsCreateDisplay(Widget parent, String name, ArgList args, Cardinal argcount);
extern Boolean XmdsIsDisplay(Widget w);

#endif				/* _NO_PROTO */

#endif				/* _XmdsDisplay */
