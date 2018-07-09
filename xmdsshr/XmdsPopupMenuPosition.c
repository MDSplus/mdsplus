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
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSPOPUPMENUPOSITION.C */
/*  *3    22-FEB-1994 14:34:25 TWF "Remove NO_X_GBLS" */
/*  *2    25-FEB-1993 10:39:55 JAS "port to decc" */
/*  *1     8-OCT-1991 13:36:31 TWF "Position popup menu putting menu history widget under pointer" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSPOPUPMENUPOSITION.C */
/*------------------------------------------------------------------------------

		Name:   XMDSPOPUPMENUPOSITION

		Type:   C function

		Author:	TOM FREDIAN

		Date:    8-OCT-1991

		Purpose: Position popup menus according to menu history

------------------------------------------------------------------------------

	Call sequence:

void      XmdsPopupMenuPosition(XmRowColumnWidget w, XButtonEvent *event)

------------------------------------------------------------------------------
   Copyright (c) 1991
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

 External functions or symbols referenced:                                    */

/*------------------------------------------------------------------------------

 Subroutines referenced:                                                      */

/*------------------------------------------------------------------------------

 Macros:                                                                      */

/*------------------------------------------------------------------------------

 Global variables:                                                            */

#include <Xm/RowColumnP.h>
#include <mdsplus/mdsconfig.h>

/*------------------------------------------------------------------------------

 Local variables:                                                             */

/*------------------------------------------------------------------------------

 Executable:                                                                  */

EXPORT void XmdsPopupMenuPosition(XmRowColumnWidget w, XButtonEvent * event)
{
  Widget option = w->row_column.memory_subwidget;
  XtX(w) = event->x_root - (option ? XtX(option) + XtWidth(option) / 2 : 0);
  XtY(w) = event->y_root - (option ? XtY(option) + XtHeight(option) / 2 : 0);
  RC_SetWidgetMoved(w, 1);
  RC_CascadeBtn(w) = XtWindowToWidget(XtDisplay(w), event->window);
}
