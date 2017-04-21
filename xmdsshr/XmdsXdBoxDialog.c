/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSXDBOXDIALOG.C */
/*  *5    23-JUN-1994 10:45:09 TWF "Motif 1.2" */
/*  *4    22-FEB-1994 15:18:25 TWF "remove NO_X_GBLS" */
/*  *3     2-MAR-1993 12:25:08 JAS "port to decc" */
/*  *2     4-DEC-1992 15:25:54 JAS "use xdboxload in the popup callback" */
/*  *1    16-JAN-1992 11:01:27 JAS "xdboxdialog convenience routine" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSXDBOXDIALOG.C */
/*------------------------------------------------------------------------------

		Name:   XMDSXDBOXDIALOG

		Type:   C function

		Author:	JOSH STILLERMAN

		Date:   16-JAN-1992

		Purpose: Convienence routine for XdBoxDialogs

------------------------------------------------------------------------------

	Call sequence:

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------*/

#include <Xm/DialogS.h>
#include <Xmds/XmdsXdBoxP.h>
static void LoadDialog(Widget shell, Widget xdbw);
extern void XmdsXdBoxLoad();
#include <xmdsshr.h>
/****************************************************
  Routine to create a Popup Xd Widget.  Use
  resources originalXd, and buttonCallback to setup
  the initial widget.
****************************************************/
EXPORT Widget XmdsCreateXdBoxDialog(Widget parent, char *name, ArgList args, Cardinal argcount)
{

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

  Widget hidden;
  Widget widg;

  static Arg shell_args[] = { {XmNallowShellResize, 1},
  {XmNoverrideRedirect, 0},
  {XmNdeleteResponse, XmDESTROY}
  };

  Arg *sub_args;
  Cardinal i, cnt;

  String new_name = XtMalloc(strlen(name) + 7);

  /*------------------------------------------------------------------------------
  
   Executable:                                                                  */

  strcpy((char *)new_name, name);
  strcpy((char *)&new_name[strlen(name)], "_popup");

  hidden = XmCreateDialogShell(parent, (char *)new_name, shell_args, XtNumber(shell_args));
  XtFree(new_name);
  XtSetValues(hidden, args, argcount);
  /**************************************
    Create the widget with the
    users arguments.  And realize the
    hidden shell.

    first copy the input arguments removing
    XmNx and XmNy
   *************************************/
  sub_args = (Arg *) XtMalloc(sizeof(Arg) * argcount);
  for (i = 0, cnt = 0; i < argcount; i++) {
    if ((strcmp(args[i].name, XmNx) != 0) && (strcmp(args[i].name, XmNy) != 0)) {
      sub_args[cnt].name = args[i].name;
      sub_args[cnt++].value = args[i].value;
    }
  }
  widg = XtCreateWidget(name, xmdsXdBoxWidgetClass, hidden, sub_args, cnt);
  XtFree((XtPointer) sub_args);

  /*************************************
     Add a callback to load the dialog
     box when it is popped up
   *************************************/
  XtAddCallback(hidden, XmNpopupCallback, (XtCallbackProc) LoadDialog, widg);
  XtRealizeWidget(hidden);
  return widg;
}

static void LoadDialog(Widget shell, Widget xdbw)
{
  XmdsXdBoxLoad(xdbw);
}
