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

extern Widget XmdsCreateInput(Widget parent, char *name, ArgList args, int argcount);
extern void XmdsInputSetNid(Widget w, int nid);
extern void XmdsInputReset(Widget w);
extern void XmdsInputPut(Widget w);

#endif
#endif
