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

/*------------------------------------------------------------------------------

 Local variables:                                                             */

/*------------------------------------------------------------------------------

 Executable:                                                                  */

void XmdsPopupMenuPosition(XmRowColumnWidget w,XButtonEvent *event)
{
  Widget option = w->row_column.memory_subwidget;
  XtX(w) = event->x_root - (option ? XtX(option) + XtWidth(option) / 2 : 0);
  XtY(w) = event->y_root - (option ? XtY(option) + XtHeight(option) / 2 : 0);
  RC_SetWidgetMoved(w,1);
  RC_CascadeBtn(w) = XtWindowToWidget(XtDisplay(w),event->window);
}
