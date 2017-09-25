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
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSOKBUTTONS.H */
/*  *3     3-MAR-1993 14:00:12 JAS "Add prottypes" */
/*  *2    24-FEB-1993 15:16:40 JAS "port to decc" */
/*  *1    24-FEB-1993 15:12:25 JAS "include file of okbuttons" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSOKBUTTONS.H */
/*------------------------------------------------------------------------------

		Name:   XmdsOkButtons.H

		Type:   C include

     		Author:	Josh Stillerman

		Date:   24-FEB-1993

    		Purpose: Public declarations for  okbutton widget

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
----------------------------------------------------------------------------*/

#ifndef _XmdsOkButtons_H
#define _XmdsOkButtons_H

#ifdef _NO_PROTO
Widget XmdsCreateApplyButton();
Widget XmdsCreateCancelButton();
Widget XmdsCreateOkButton();
Widget XmdsCreateResetButton();
#else
Widget XmdsCreateApplyButton(Widget parent, String name, ArgList args, Cardinal numargs);
Widget XmdsCreateCancelButton(Widget parent, String name, ArgList args, Cardinal numargs);
Widget XmdsCreateOkButton(Widget parent, String name, ArgList args, Cardinal numargs);
Widget XmdsCreateResetButton(Widget parent, String name, ArgList args, Cardinal numargs);
#endif
#endif
