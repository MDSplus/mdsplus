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
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSONOFFTOGGLEBUTTON.H */
/*  *4     3-MAR-1993 14:01:43 JAS "Add prottypes" */
/*  *3    24-FEB-1993 11:52:07 JAS "port to decc" */
/*  *2    10-MAR-1992 10:48:14 JAS "add showpath resource" */
/*  *1    14-JAN-1992 14:38:46 TWF "XmdsOnOffToggleButton gadget" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSONOFFTOGGLEBUTTON.H */
/*------------------------------------------------------------------------------

		Name:   XmdsOnOffToggleButton.H

		Type:   C include

		Author:	Thomas W. Fredian

		Date:   14-JAN-1992

		Purpose: Public declarations for XmdsOnOffToggleButton widget

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
----------------------------------------------------------------------------*/

#ifndef _XmdsOnOffToggleButton_h
#define _XmdsOnOffToggleButton_h

#ifndef _Xm_h
#include <Xm/Xm.h>
#endif

/* Resources */

#define XmdsNnid               "nid"	/* Nid */
#define XmdsNnidOffset         "nidOffset"	/* Nid offset */
#define XmdsNshowPath          "showPath"	/* Nid */
#define XmdsNputOnApply        "putOnApply"	/* Put when a XmdsApplyAllXds is called */

/* External creation routines:
*/
#ifdef _NO_PROTO

extern Widget XmdsCreateOnOffToggleButton();
extern Boolean XmdsIsOnOffToggleButton();
extern int XmdsOnOffToggleButtonApply();
extern int XmdsOnOffToggleButtonPut();
extern void XmdsOnOffToggleButtonReset();

#else				/* _NO_PROTO */

extern Widget XmdsCreateOnOffToggleButton(Widget parent, String name, ArgList args,
					  Cardinal argcount);
extern Boolean XmdsIsOnOffToggleButton(Widget w);
extern int XmdsOnOffToggleButtonApply(Widget w);
extern int XmdsOnOffToggleButtonPut(Widget w);
extern void XmdsOnOffToggleButtonReset(Widget w);

#endif				/* _NO_PROTO */

#endif				/* _XmdsOnOffToggleButton */
