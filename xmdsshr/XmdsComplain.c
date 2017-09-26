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
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSCOMPLAIN.C */
/*  *9    23-JUN-1994 09:20:32 TWF "Motif 1.2" */
/*  *8    22-FEB-1994 14:53:44 TWF "remove NO_X_GBLS" */
/*  *7     4-MAR-1993 16:31:00 TWF "Check va_count" */
/*  *6     4-MAR-1993 15:07:09 JAS "make it complile with vaxc" */
/*  *5    23-FEB-1993 17:55:47 JAS "port to decc" */
/*  *4    10-AUG-1992 15:56:42 JAS "typo" */
/*  *3    10-AUG-1992 15:30:42 JAS "Make sure the parent is a widget" */
/*  *2    10-AUG-1992 15:17:46 JAS "Make sure the parent is a widget" */
/*  *1     9-OCT-1991 09:54:01 JAS "Routine to put complaints on the screen" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSCOMPLAIN.C */
/*------------------------------------------------------------------------------

		Name:   XmdsComplain

		Type:   C function

		Author:	Josh Stillerman

		Date:   9-OCT-1991

		Purpose: put up a decwindows message box with
			a string in it.

------------------------------------------------------------------------------

	Call sequence:
void XmdsComplain(parent, string[, sprintf args,...])
Widget parent;
char *string;
{

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------*/
#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/MessageB.h>
#include <xmdsshr.h>

EXPORT void XmdsComplain(Widget parent, char *string, ...)
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

  Widget new_parent;
  Widget w;
  /*------------------------------------------------------------------------------
  
   Executable:                                                                  */

  for (new_parent = parent; new_parent && !XtIsWidget(new_parent);
       new_parent = XtParent(new_parent)) ;
  if (new_parent) {
    static XtCallbackRec ok_callback_list[] = { {(XtCallbackProc) XtDestroyWidget, 0}, {0, 0} };
    Arg args[] = { {XmNmessageString, (long)0},
    {XmNdialogTitle, (long)0},
    {XmNokLabelString, (long)0},
    {XmNmessageAlignment, XmALIGNMENT_BEGINNING},
    {XmNminimizeButtons, TRUE},
    {XmNokCallback, (long)ok_callback_list},
    {XmNdefaultPosition, 1}
    };
    args[0].value = (long)XmStringCreateLtoR(string, XmSTRING_DEFAULT_CHARSET);
    args[1].value = (long)XmStringCreateLtoR("Error", XmSTRING_DEFAULT_CHARSET);
    args[2].value = (long)XmStringCreateLtoR("Dismiss", XmSTRING_DEFAULT_CHARSET);
    w = XmCreateErrorDialog(new_parent, "XmdsComplain", args, XtNumber(args));
    XtDestroyWidget(XmMessageBoxGetChild(w, XmDIALOG_CANCEL_BUTTON));
    XtDestroyWidget(XmMessageBoxGetChild(w, XmDIALOG_HELP_BUTTON));
    XtManageChild(w);
    XmStringFree((XmString) args[0].value);
    XmStringFree((XmString) args[1].value);
    XmStringFree((XmString) args[2].value);
  } else
    printf
	("Error displaying dialog box\nCould not find widget to 'parent' box\nError message was:\n\t%s\n",
	 string);
}
