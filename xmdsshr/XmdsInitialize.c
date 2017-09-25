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
/*  CMS REPLACEMENT HISTORY, Element XMDSINITIALIZE.C */
/*  *30   17-JUL-1997 14:19:47 TWF "Fix for no xds" */
/*  *29   17-JUL-1997 14:18:40 TWF "Fix for no xds" */
/*  *28   18-MAR-1997 10:41:00 TWF "Fix for DECC 5.3" */
/*  *27   18-MAR-1997 09:33:23 TWF "Fix for DECC 5.3" */
/*  *26   24-MAR-1994 15:21:11 TWF "Remove debug" */
/*  *25   24-MAR-1994 15:16:39 TWF "Put in debug printout" */
/*  *24    8-APR-1993 11:34:11 TWF "Use MrmPublic (much smaller)" */
/*  *23   24-FEB-1993 15:23:45 JAS "port to decc" */
/*  *22   19-JAN-1993 09:42:54 JAS "adding xmdsdisplay" */
/*  *21    6-MAY-1992 16:02:01 JAS "adding XmdsPath" */
/*  *20    6-MAY-1992 15:57:18 JAS "adding XmdsPath" */
/*  *19   28-JAN-1992 16:12:29 TWF "Add NidOptionMenu widge" */
/*  *18   23-JAN-1992 09:55:29 TWF "make okbuttons widgets (vuit can't deal with gadgets)" */
/*  *17   23-JAN-1992 09:51:32 TWF "make okbuttons widgets (vuit can't deal with gadgets)" */
/*  *16   23-JAN-1992 09:12:27 TWF "Make okbuttons gadgets" */
/*  *15   23-JAN-1992 09:05:38 TWF "Make okbuttons gadgets" */
/*  *14   21-JAN-1992 10:46:02 TWF "" */
/*  *13   16-JAN-1992 11:35:58 JAS "adding Xdbox and xdboxdialog" */
/*  *12   16-JAN-1992 09:30:41 TWF "Add XmdsOkBox widget" */
/*  *11   15-JAN-1992 14:36:06 TWF "Fix compile error" */
/*  *10   15-JAN-1992 14:24:47 TWF "Add XmdsXdBox buttons" */
/*  *9    14-JAN-1992 16:34:31 TWF "Add Apply,Ok,Reset,Cancel callbacks" */
/*  *8    10-JAN-1992 15:00:25 JAS "adding ExprField" */
/*  *7     7-JAN-1992 14:53:59 TWF "Add XmdsDigChans" */
/*  *6     2-JAN-1992 17:01:13 TWF "Add XmdsInput" */
/*  *5     2-JAN-1992 16:57:10 TWF "Add XmdsInput" */
/*  *4    18-DEC-1991 10:57:56 TWF "Add Wavedraw widget" */
/*  *3     5-DEC-1991 15:35:08 TWF "Add Expr widget" */
/*  *2     8-OCT-1991 16:48:05 TWF "Add convenience callback routines" */
/*  *1     8-OCT-1991 16:19:33 TWF "Initialize Xmds widget set" */
/*  CMS REPLACEMENT HISTORY, Element XMDSINITIALIZE.C */
/*------------------------------------------------------------------------------

		Name:   XMDSINITIALIZE

		Type:   C function

		Author:	TOM FREDIAN

		Date:    8-OCT-1991

		Purpose: Initialize Xmds widget set

------------------------------------------------------------------------------

	Call sequence:

int XmdsInitialize( )

------------------------------------------------------------------------------
   Copyright (c) 1991
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------*/

#include <stdio.h>
#include <Mrm/MrmPublic.h>
#include <Xm/BulletinB.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#ifndef _NO_XDS
#include <xmdsshr.h>
#include <Xmds/XmdsCallbacks.h>
#include <Xmds/XmdsDigChans.h>
#include <Xmds/XmdsDisplay.h>
#include <Xmds/XmdsExpr.h>
#include <Xmds/XmdsExprField.h>
#include <Xmds/XmdsInput.h>
#include <Xmds/XmdsInputs.h>
#include <Xmds/XmdsNidOptionMenu.h>
#include <Xmds/XmdsOkButtons.h>
#include <Xmds/XmdsOnOffToggleButton.h>
#include <Xmds/XmdsPath.h>
#include <Xmds/XmdsSupport.h>
#include <Xmds/XmdsWavedraw.h>
#include <Xmds/XmdsXdBox.h>
#include <Xmds/XmdsXdBoxDialog.h>
#include <Xmds/XmdsXdBoxDialogButton.h>
#include <Xmds/XmdsXdBoxOnOffButton.h>
#endif

#include <Xmds/XmdsWaveform.h>


EXPORT extern void XmdsInitialize()
{
#ifndef _NO_XDS
  static MrmRegisterArg callbacks[] = {
    {"XmdsApplyCallback", (char *)XmdsApplyCallback},
    {"XmdsCancelCallback", (char *)XmdsCancelCallback},
    {"XmdsDestroyWidgetCallback", (char *)XmdsDestroyWidgetCallback},
    {"XmdsManageChildCallback", (char *)XmdsManageChildCallback},
    {"XmdsOkCallback", (char *)XmdsOkCallback},
    {"XmdsRegisterWidgetCallback", (char *)XmdsRegisterWidgetCallback},
    {"XmdsResetCallback", (char *)XmdsResetCallback},
    {"XmdsUnmanageChildCallback", (char *)XmdsUnmanageChildCallback}
  };

#ifndef __VAX
  xmdsExprWidgetClass = (WidgetClass) & xmdsExprClassRec;
  xmdsExprFieldWidgetClass = (WidgetClass) & xmdsExprClassRec;
  xmdsWavedrawWidgetClass = (WidgetClass) & xmdsWavedrawClassRec;
  xmdsWaveformWidgetClass = (WidgetClass) & xmdsWaveformClassRec;
  xmdsXdBoxWidgetClass = (WidgetClass) & xmdsXdBoxClassRec;
#endif

  MrmRegisterClass(MrmwcUnknown, "XmdsApplyButtonClass", "XmdsCreateApplyButton",
		   (Widget (*)(void))XmdsCreateApplyButton, xmPushButtonWidgetClass);
  MrmRegisterClass(MrmwcUnknown, "XmdsCancelButtonClass", "XmdsCreateCancelButton",
		   (Widget (*)(void))XmdsCreateCancelButton, xmPushButtonWidgetClass);
  MrmRegisterClass(MrmwcUnknown, "XmdsDigChansWidgetClass", "XmdsCreateDigChans",
		   (Widget (*)(void))XmdsCreateDigChans, xmFormWidgetClass);
  MrmRegisterClass(MrmwcUnknown, "XmdsExprWidgetClass", "XmdsCreateExpr", 
           (Widget (*)(void))XmdsCreateExpr, xmdsExprWidgetClass);
  MrmRegisterClass(MrmwcUnknown, "XmdsExprFieldWidgetClass", "XmdsCreateExprField",
		   (Widget (*)(void))XmdsCreateExprField, xmdsExprFieldWidgetClass);
  MrmRegisterClass(MrmwcUnknown, "XmdsInputWidgetClass", "XmdsCreateInput", 
           (Widget (*)(void))XmdsCreateInput, xmBulletinBoardWidgetClass);
  MrmRegisterClass(MrmwcUnknown, "XmdsOkButtonClass", "XmdsCreateOkButton", 
           (Widget (*)(void))XmdsCreateOkButton, xmPushButtonWidgetClass);
  MrmRegisterClass(MrmwcUnknown, "XmdsOnOffToggleButtonWidgetClass", "XmdsCreateOnOffToggleButton",
		   (Widget (*)(void))XmdsCreateOnOffToggleButton, xmToggleButtonWidgetClass);
  MrmRegisterClass(MrmwcUnknown, "XmdsNidOptionMenuWidgetClass", "XmdsCreateNidOptionMenu",
		   (Widget (*)(void))XmdsCreateNidOptionMenu, xmRowColumnWidgetClass);
  MrmRegisterClass(MrmwcUnknown, "XmdsResetButtonClass", "XmdsCreateResetButton",
		   (Widget (*)(void))XmdsCreateResetButton, xmPushButtonWidgetClass);
  MrmRegisterClass(MrmwcUnknown, "XmdsWavedrawWidgetClass", "XmdsCreateWavedraw",
		   (Widget (*)(void))XmdsCreateWavedraw, xmdsWavedrawWidgetClass);
  MrmRegisterClass(MrmwcUnknown, "XmdsXdBoxDialogButtonWidgetClass", "XmdsCreateXdBoxDialogButton",
		   (Widget (*)(void))XmdsCreateXdBoxDialogButton, xmPushButtonWidgetClass);
  MrmRegisterClass(MrmwcUnknown, "XmdsXdBoxOnOffButtonWidgetClass", "XmdsCreateXdBoxOnOffButton",
		   (Widget (*)(void))XmdsCreateXdBoxOnOffButton, xmRowColumnWidgetClass);
  MrmRegisterClass(MrmwcUnknown, "XmdsXdBoxDialogWidgetClass", "XmdsCreateXdBoxDialog",
		   (Widget (*)(void))XmdsCreateXdBoxDialog, xmdsXdBoxWidgetClass);
  MrmRegisterClass(MrmwcUnknown, "XmdsXdBoxWidgetClass", "XmdsCreateXdBox", 
           (Widget (*)(void))XmdsCreateXdBox, xmdsXdBoxWidgetClass);
  MrmRegisterClass(MrmwcUnknown, "XmdsPathClass", "XmdsCreatePath", 
           (Widget (*)(void))XmdsCreatePath, xmLabelWidgetClass);
  MrmRegisterClass(MrmwcUnknown, "XmdsDisplayClass", "XmdsCreateDisplay", 
           (Widget (*)(void))XmdsCreateDisplay, xmLabelWidgetClass);
  MrmRegisterNames(callbacks, XtNumber(callbacks));
#endif
  MrmRegisterClass(MrmwcUnknown, "XmdsWaveformWidgetClass", "XmdsCreateWaveform",
		   (Widget (*)(void))XmdsCreateWaveform, xmdsWaveformWidgetClass);
}
