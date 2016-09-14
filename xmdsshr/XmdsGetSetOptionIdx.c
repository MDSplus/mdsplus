/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSGETSETOPTIONIDX.C */
/*  *3    22-FEB-1994 10:50:08 TWF "Take out NO_X_GLBS" */
/*  *2    24-FEB-1993 12:12:54 JAS "port to decc" */
/*  *1     8-OCT-1991 13:35:09 TWF "Option menu convenience routines" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSGETSETOPTIONIDX.C */
/*------------------------------------------------------------------------------

		Name:   XmdsGETSETOPTIONIDX

		Type:   C function

		Author:	TOM FREDIAN

		Date:    9-MAR-1990

		Purpose: Set/Get Option Widget selection by index

------------------------------------------------------------------------------

	Call sequence:

void XmdsSetOptionIdx(Widget w,int idx);
int XmdsGetOptionIdx(Widget w);

------------------------------------------------------------------------------
   Copyright (c) 1990
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------*/

#include <Xm/Xm.h>
#include <config.h>
/*------------------------------------------------------------------------------

 External functions or symbols referenced:                                    */

/*------------------------------------------------------------------------------

 Subroutines referenced:                                                      */

/*------------------------------------------------------------------------------

 Macros:                                                                      */

/*------------------------------------------------------------------------------

 Global variables:                                                            */

/*------------------------------------------------------------------------------

 Local variables:                                                             */


/*------------------------------------------------------------------------------

 Executable:                                                                  */

EXPORT void XmdsSetOptionIdx(Widget w, Cardinal idx)
{
  static Widget pulldown;
  XtVaGetValues(w, XmNsubMenuId, &pulldown, NULL);
  if (pulldown) {
    Widget *options;
    Cardinal num_options;
    XtVaGetValues(pulldown, XmNchildren, &options, XmNnumChildren, &num_options, NULL);
    if (idx < num_options)
      XtVaSetValues(w, XmNmenuHistory, options[idx], NULL);
  }
}

EXPORT int XmdsGetOptionIdx(Widget w)
{
  int idx = -1;
  static Widget pulldown;
  static Widget option;
  XtVaGetValues(w, XmNsubMenuId, &pulldown, XmNmenuHistory, &option, NULL);
  if (pulldown && option) {
    static Widget *options;
    static Cardinal num_options;
    Cardinal i;
    XtVaGetValues(pulldown, XmNchildren, &options, XmNnumChildren, &num_options, NULL);
    for (i = 0; i < num_options; i++)
      if (options[i] == option) {
	idx = i;
	break;
      }
  }
  return idx;
}
