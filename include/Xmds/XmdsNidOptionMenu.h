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
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSNIDOPTIONMENU.H */
/*  *6     4-NOV-1994 10:32:26 JAS "need to declare XmdsNidOptionMenuIdxGetXd in the .H file" */
/*  *5     3-MAR-1993 13:58:30 JAS "Add prottypes" */
/*  *4    24-FEB-1993 16:38:39 JAS "port to decc" */
/*  *3    24-FEB-1993 11:51:37 JAS "port to decc" */
/*  *2    26-JUN-1992 15:21:37 JAS "Add menu callbacks" */
/*  *1    28-JAN-1992 15:54:43 TWF "NidOptionMenu Pseudo widget" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSNIDOPTIONMENU.H */
/*------------------------------------------------------------------------------

		Name:   XmdsNidOptionMenu.H

		Type:   C include

		Author:	Thomas W. Fredian

		Date:   28-JAN-1992

		Purpose: Public declarations for XmdsNidOptionMenu widget

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
----------------------------------------------------------------------------*/

#ifndef _XmdsNidOptionMenu_h
#define _XmdsNidOptionMenu_h

#ifndef _Xm_h
#include <Xm/Xm.h>
#endif

/* Resources */

#define XmdsNnid                    "nid"	/* Nid */
#define XmdsNnidOffset              "nidOffset"	/* Nid offset */
#define XmdsNputOnApply             "putOnApply"	/* Put when a XmdsApplyAllXds is called */
#define XmdsNlabelStrings           "labelStrings"	/* String table of button labels */
#define XmdsNvalueStrings	    "valueStrings"	/* String table of value expressions */
#define XmdsNresetCallback	    "resetCallback"	/* User routine to call for resets (no nid) */
#define XmdsNvalueChangedCallback   "valueChangedCallback"	/* User routine to call when the value changes */

/* External creation routines:
*/
#ifdef _NO_PROTO

extern Widget XmdsCreateNidOptionMenu();
extern Boolean XmdsIsNidOptionMenu();
extern int *XmdsNidOptionMenuGetButtons();
extern struct descriptor_xd *XmdsNidOptionMenuGetXd();
extern void XmdsNidOptionMenuReset();
extern int XmdsNidOptionMenuPut();
extern int XmdsNidOptionMenuApply();
extern void XmdsNidOptionMenuSetButton();
struct descriptor_xd *XmdsNidOptionMenuIdxGetXd();

#else				/* _NO_PROTO */

extern Widget XmdsCreateNidOptionMenu(Widget parent, char *name, ArgList al, Cardinal ac);
extern Boolean XmdsIsNidOptionMenu(Widget w);
extern int *XmdsNidOptionMenuGetButtons(Widget w, int *num);
extern struct descriptor_xd *XmdsNidOptionMenuGetXd(Widget w);
extern void XmdsNidOptionMenuReset(Widget w);
extern int XmdsNidOptionMenuPut(Widget w);
extern int XmdsNidOptionMenuApply(Widget w);
extern void XmdsNidOptionMenuSetButton(Widget w, int idx, String text);
struct descriptor_xd *XmdsNidOptionMenuIdxGetXd(Widget w, int selected);

#endif				/* _NO_PROTO */

#endif				/* _XmdsNidOptionMenu */
