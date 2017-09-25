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
#include <mdsdescrip.h>
#include <X11/Intrinsic.h>
extern Boolean ConglomerateElt(int nid);
extern int ConglomerateHead(int nid);
extern int DefaultNid(int nid);
extern int NodeParent(int nid);
extern void ResetErrors();
extern void TdiComplain(Widget w);
extern struct descriptor *TdiGet(int nid);
extern Boolean PutIfChanged(int nid, struct descriptor_xd *xd);
#endif				/* XmdsSupport_h */
