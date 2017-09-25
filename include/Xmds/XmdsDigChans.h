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
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSDIGCHANS.H */
/*  *5     3-MAR-1993 13:35:27 JAS "Add prottypes" */
/*  *4    22-FEB-1993 18:12:23 JAS "port to decc" */
/*  *3    14-JAN-1992 12:41:15 TWF "Change to nid and nid_offset" */
/*  *2    14-JAN-1992 12:14:55 TWF "Add XmdsIsDigChans" */
/*  *1     7-JAN-1992 14:25:29 TWF "Digitizer channels box" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSDIGCHANS.H */
/*------------------------------------------------------------------------------

		Name:   XmdsDigChans.H

		Type:   C include

		Author:	Thomas W. Fredian

		Date:   7-JAN-1992

		Purpose: Public declarations for XmdsDigChans widget

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
----------------------------------------------------------------------------*/

#ifndef _XmdsDigChans_h
#define _XmdsDigChans_h

#ifndef _Xm_h
#include <Xm/Xm.h>
#endif

/* Resources */

#define XmdsNchannels               "channels"	/* Number of digitizer channels */
#define XmdsNnid		    "nid"	/* Nid of first channel */
#define XmdsNnidOffset		    "nidOffset"	/* Nid offset */
#define XmdsNdataNidOffset          "dataNidOffset"	/* Offset from channel head to data */
#define XmdsNstartIdxNidOffset      "startIdxNidOffset"	/* Offset from channel head to startidx */
#define XmdsNendIdxNidOffset        "endIdxNidOffset"	/* Offset from channel head to endidx */
#define XmdsNnodesPerChannel        "nodesPerChannel"	/* Number of nodes per digitizer channel */
#define XmdsNputOnApply             "putOnApply"	/* Put when a XmdsApplyAllXds is called */

/* External creation routines:
*/
#ifdef _NO_PROTO

extern Widget XmdsCreateDigChans();
extern void XmdsDigChansReset();
extern int XmdsDigChansPut();
extern int XmdsDigChansApply();
extern Boolean XmdsIsDigChans();

#else				/* _NO_PROTO */

extern Widget XmdsCreateDigChans(Widget parent, String name, ArgList args, Cardinal argcount);
extern int XmdsDigChansApply(Widget w);
extern int XmdsDigChansPut(Widget w);
extern void XmdsDigChansReset(Widget w);
extern Boolean XmdsIsDigChans(Widget w);

#endif				/* _NO_PROTO */

#endif				/* _XmdsDigChans */
