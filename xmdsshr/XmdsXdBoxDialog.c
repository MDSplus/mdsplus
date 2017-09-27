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
