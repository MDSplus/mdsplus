/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSSETSUBVALUES.C */
/*  *5    22-FEB-1994 15:06:42 TWF "remove NO_X_GBLS" */
/*  *4     8-JUL-1993 12:08:57 TWF "Make routine void" */
/*  *3    25-FEB-1993 10:42:28 JAS "port to decc" */
/*  *2    14-JAN-1992 17:18:52 TWF "Fix check on xmdsshr" */
/*  *1    14-JAN-1992 12:58:14 TWF "Initializze Resource list and do XtSetSubvalues" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSSETSUBVALUES.C */
/*------------------------------------------------------------------------------

		Name:   XmdsSetSubvalues

		Type:   C function

		Author:	TOM FREDIAN

		Date:   14-JAN-1992

		Purpose: Init resources if necessary and set subvalues

------------------------------------------------------------------------------

	Call sequence:

int XmdsSetSubvalues(XtPointer record, XtResourceList resources, Cardinal num_resources, Arglist args, Cardinal argcount )

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------*/

#include <Xm/Xm.h>

int XmdsSetSubvalues(XtPointer record, XtResourceList resources, Cardinal num_resources,
		     ArgList args, Cardinal argcount)
{
  Cardinal i;
  for (i = 0; i < num_resources; i++) {
    if (*((int *)&resources[i].resource_offset) >= 0) {
      resources[i].resource_name = XrmStringToQuark(resources[i].resource_name) + (char *)0;
      resources[i].resource_class = XrmStringToQuark(resources[i].resource_class) + (char *)0;
      resources[i].resource_type = XrmStringToQuark(resources[i].resource_type) + (char *)0;
      resources[i].default_type = XrmStringToQuark(resources[i].default_type) + (char *)0;
      resources[i].resource_offset = -(resources[i].resource_offset + 1);
    }
  }
  XtSetSubvalues(record, resources, num_resources, args, argcount);
  return 1;
}
