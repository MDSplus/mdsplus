/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSBELLS.C */
/*  *2    23-FEB-1993 17:56:52 JAS "port to decc" */
/*  *1     5-MAY-1992 09:10:14 TWF "Ring bells" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSBELLS.C */
/*------------------------------------------------------------------------------

		Name:   XmdsBells

		Type:   C function

		Author:	TOM FREDIAN

		Date:    5-MAY-1992

		Purpose: Ring Bells

------------------------------------------------------------------------------

	Call sequence:

int XmdsBells(int *number_of_bells )

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:


------------------------------------------------------------------------------*/

#include <X11/Xlib.h>
int XmdsBells(int *num_bells)
{
  static Display *display = 0;
  int i;
  if (!display)
    display = XOpenDisplay(0);
  for (i = 0; display && i < *num_bells; i++)
    XBell(display,0);
  XFlush(display);
  return display != 0;
}
