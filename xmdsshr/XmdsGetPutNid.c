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
/*  CMS REPLACEMENT HISTORY, Element XMDSGETPUTNID.C */
/*  *23   26-AUG-1997 09:07:09 TWF "Do not change with parent on/off status" */
/*  *22   23-JUN-1994 09:56:29 TWF "Motif 1.2" */
/*  *21   22-FEB-1994 14:59:41 TWF "remove NO_X_GBLS" */
/*  *20    3-MAR-1993 09:18:52 JAS "use prototypes" */
/*  *19   24-FEB-1993 12:09:32 JAS "port to decc" */
/*  *18    4-DEC-1992 15:21:33 JAS "Don't have to apply nid option menus since their Xdboxes do it" */
/*  *17   12-FEB-1992 13:13:50 TWF "Apply returns status" */
/*  *16   11-FEB-1992 17:51:58 TWF "Apply all xds not returning true" */
/*  *15   28-JAN-1992 16:24:39 TWF "Add NidOptionMenu widge" */
/*  *14   17-JAN-1992 12:43:14 JAS "typo" */
/*  *13   17-JAN-1992 12:38:16 JAS "have to  make sure they are at least core before doing popups" */
/*  *12   17-JAN-1992 11:50:51 JAS "typo" */
/*  *11   17-JAN-1992 11:47:29 JAS "typo" */
/*  *10   17-JAN-1992 11:40:18 JAS "look through the popup lists also" */
/*  *9    16-JAN-1992 11:44:07 TWF "Call XmdsCreateXdBoxDialog" */
/*  *8    14-JAN-1992 14:41:56 TWF "Add XmdsOnOffToggleButton widget" */
/*  *7    14-JAN-1992 12:15:13 TWF "Add XmdsIsDigChans" */
/*  *6    13-JAN-1992 13:03:13 TWF "Add include" */
/*  *5    13-JAN-1992 12:39:42 TWF "New xd class widgets" */
/*  *4     6-DEC-1991 10:09:39 TWF "Fix XmdsResetAllXds" */
/*  *3     6-DEC-1991 10:03:33 TWF "Add XmdsSetState, XmdsResetAllXds, XmdsXdsAreValid" */
/*  *2     5-DEC-1991 16:40:14 TWF "Change call to TDI$DEBUG" */
/*  *1     5-DEC-1991 16:00:52 TWF "Support routines" */
/*  CMS REPLACEMENT HISTORY, Element XMDSGETPUTNID.C */
/*------------------------------------------------------------------------------

		Name:   XmdsGetPutNid

		Type:   C function

		Author:	TOM FREDIAN

		Date:    9-MAR-1990

		Purpose: X Support routines for getting and putting values
			 in MDS tree nids

------------------------------------------------------------------------------

	Call sequence:

int XmdsGetNidLongValue(int nid,int minVal,int maxVal, int defaultVal);
int XmdsGetNidFloatOption(int nid, float *options, int numOptions);
int XmdsGetNidIntOption(int nid, int *options, int numOptions);
char *XmdsGetNidText(int nid);
int XmdsPutNidValue(int nid,unsigned short length,unsigned char dtype,char *pointer,char *originalValue);
int XmdsPutNidToggleButton(Widget w,int nid,int originalValue);
int XmdsPutNidScale(Widget w,int nid, int originalValue);
int XmdsPutNidSText(Widget w,int nid,char *originalValue);
int XmdsSetState(int nid, Widget w);
int XmdsGetNidBooleanValue(int nid, int default);
int XmdsIsOn(int nid)
int XmdsResetAllXds(Widget w)
int XmdsXdsAreValid(Widget w)

------------------------------------------------------------------------------
   Copyright (c) 1990
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------*/
#include <mdsplus/mdsconfig.h>
#include <mdsdescrip.h>
#include <treeshr.h>
#include <mdsshr.h>
#include <xmdsshr.h>
#include <Xm/XmP.h>
#include <X11/Composite.h>
#include <Xm/ToggleB.h>
#include <Xm/Scale.h>
#include <Xm/Text.h>
#include <Xmds/XmdsExpr.h>
#include <Xmds/XmdsExprField.h>
#include <Xmds/XmdsDigChans.h>
#include <Xmds/XmdsNidOptionMenu.h>
#include <Xmds/XmdsOnOffToggleButton.h>
#include <Xmds/XmdsXdBox.h>
#include <Xmds/XmdsXdBoxOnOffButton.h>

int XmdsGetNidLongValue(int nid, int minVal, int maxVal, int defaultVal);
int XmdsGetNidFloatOption(int nid, float *options, int numOptions);
int XmdsGetNidIntOption(int nid, int *options, int numOptions);
int XmdsPutNidValue(int nid, unsigned short length, unsigned char dtype, char *pointer,
		    char *originalValue);
char *XmdsGetNidText(int nid);
int XmdsPutNidToggleButton(Widget w, int nid, int originalValue);
int XmdsPutNidScale(Widget w, int nid, int originalValue);
int XmdsPutNidSText(Widget w, int nid, char *OriginalValue);
static int GetLong(int nid, int *intptr);

/*------------------------------------------------------------------------------

 External functions or symbols referenced:                                    */

extern int TdiGetFloat();

/*------------------------------------------------------------------------------

 Subroutines referenced:                                                      */

/*------------------------------------------------------------------------------

 Macros:                                                                      */

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) < (b)) ? (b) : (a))

/*------------------------------------------------------------------------------

 Global variables:                                                            */

/*------------------------------------------------------------------------------

 Local variables:                                                             */

/*------------------------------------------------------------------------------

 Executable:                                                                  */

int XmdsGetNidLongValue(int nid, int minVal, int maxVal, int defaultVal)
{
  int value;
  return (GetLong(nid, &value) & 1) ? max(minVal, min(maxVal, value)) : defaultVal;
}

int XmdsGetNidFloatOption(int nid, float *options, int numOptions)
{
  static struct descriptor_xd xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
  int option = numOptions;
  if (TreeGetRecord(nid, &xd) & 1) {
    if (xd.pointer->dtype == DTYPE_F) {
      float f = *((float *)xd.pointer->pointer);
      int i;
      for (i = 0; i < numOptions; i++)
	if (f <= options[i])
	  break;
      option = min(numOptions - 1, i);
    }
  }
  MdsFree1Dx(&xd, 0);
  return option;
}

int XmdsGetNidIntOption(int nid, int *options, int numOptions)
{
  static struct descriptor_xd xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
  int option = numOptions;
  if (TreeGetRecord(nid, &xd) & 1) {
    if (xd.pointer->dtype == DTYPE_L) {
      int f = *((int *)xd.pointer->pointer);
      int i;
      for (i = 0; i < numOptions; i++)
	if (f <= options[i])
	  break;
      option = min(numOptions - 1, i);
    }
  }
  MdsFree1Dx(&xd, 0);
  return option;
}

char *XmdsGetNidText(int nid)
{
  static struct descriptor_xd xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
  char *value;
  if (TreeGetRecord(nid, &xd) & 1) {
    if (xd.pointer->dtype == DTYPE_T) {
      value = XtMalloc(xd.pointer->length + 1);
      strncpy(value, xd.pointer->pointer, xd.pointer->length);
      value[xd.pointer->length] = 0;
    } else
      value = XtNewString("");
  } else
    value = XtNewString("");
  MdsFree1Dx(&xd, 0);
  return value;
}

int XmdsPutNidValue(int nid, unsigned short length, unsigned char dtype, char *pointer,
		    char *originalValue)
{
  if (memcmp(pointer, originalValue, length)) {
    struct descriptor dsc = { 0, 0, CLASS_S, (char *)0 };
    dsc.length = length;
    dsc.dtype = dtype;
    dsc.pointer = pointer;
    return TreePutRecord(nid, &dsc, 0);
  } else
    return 1;
}

int XmdsPutNidToggleButton(Widget w, int nid, int originalValue)
{
  int value = XmToggleButtonGetState(w);
  return XmdsPutNidValue(nid, 4, DTYPE_L, (char *)&value, (char *)&originalValue);
}

int XmdsPutNidScale(Widget w, int nid, int originalValue)
{
  int value;
  XmScaleGetValue(w, &value);
  return XmdsPutNidValue(nid, 4, DTYPE_L, (char *)&value, (char *)&originalValue);
}

int XmdsPutNidSText(Widget w, int nid, char *originalValue)
{
  char *value = XmTextGetString(w);
  int status = 1;
  if (strcmp(value, originalValue)) {
    struct descriptor dsc = { 0, DTYPE_T, CLASS_S, (char *)0 };
    dsc.length = strlen(value);
    dsc.pointer = value;
    status = TreePutRecord(nid, &dsc, 0);
  }
  return status;
}

static int GetLong(int nid, int *intptr)
{
  struct descriptor niddsc = { 4, DTYPE_NID, CLASS_S, (char *)0 };
  int status;
  float x;
  niddsc.pointer = (char *)&nid;
  if ((status = TdiGetFloat(&niddsc, &x)) & 1)
    *intptr = x + .4999;
  return status;
}

int XmdsSetState(int nid, Widget w)
{
  return (XmToggleButtonGetState(w)) ? TreeTurnOn(nid) : TreeTurnOff(nid);
}

int XmdsGetNidBooleanValue(int nid, int defaultVal)
{
  int val;
  return (GetLong(nid, &val) & 1) ? val & 1 : defaultVal;
}

int XmdsIsOn(int nid)
{
  int status;
  return (((status = TreeIsOn(nid)) == TreeON) || (status == TreePARENT_OFF));
}

Boolean XmdsXdsAreValid(Widget w)
{
  int status = True;
  if (XmdsIsExpr(w)) {
    struct descriptor_xd *xd = (struct descriptor_xd *)XmdsExprGetXd(w);
    if (xd) {
      MdsFree1Dx(xd, 0);
      XtFree((char *)xd);
    } else
      status = False;
  } else if (XmdsIsExprField(w)) {
    struct descriptor_xd *xd = (struct descriptor_xd *)XmdsExprFieldGetXd(w);
    if (xd) {
      MdsFree1Dx(xd, 0);
      XtFree((char *)xd);
    } else
      status = False;
  } else if (XmdsIsXdBox(w)) {
    struct descriptor_xd *xd = (struct descriptor_xd *)XmdsXdBoxGetXd(w);
    if (xd) {
      MdsFree1Dx(xd, 0);
      XtFree((char *)xd);
    } else
      status = False;
  } else if (XtIsComposite(w)) {
    Widget *children;
    int num;
    int i;
    XtVaGetValues(w, XtNchildren, &children, XtNnumChildren, &num, NULL);
    for (i = 0; i < num && status; i++)
      status = XmdsXdsAreValid(children[i]);
  }
  if (XtIsWidget(w) && w->core.num_popups) {
    Widget *popups = w->core.popup_list;
    int num = w->core.num_popups;
    int i;
    for (i = 0; i < num && status; i++)
      status = XmdsXdsAreValid(popups[i]);
  }
  return status;
}

void XmdsResetAllXds(Widget w)
{
  if (XmdsIsExpr(w))
    XmdsExprReset(w);
  else if (XmdsIsExprField(w))
    XmdsExprFieldReset(w);
  else if (XmdsIsDigChans(w))
    XmdsDigChansReset(w);
  else if (XmdsIsOnOffToggleButton(w))
    XmdsOnOffToggleButtonReset(w);
  else if (XmdsIsXdBox(w))
    XmdsXdBoxReset(w);
  else if (XmdsIsNidOptionMenu(w))
    XmdsNidOptionMenuReset(w);
  else if (XmdsIsXdBoxOnOffButton(w))
    XmdsXdBoxOnOffButtonReset(w);
  else if (XtIsComposite(w)) {
    Widget *children;
    int num;
    int i;
    XtVaGetValues(w, XtNchildren, &children, XtNnumChildren, &num, NULL);
    for (i = 0; i < num; i++)
      XmdsResetAllXds(children[i]);
  }
  if (XtIsWidget(w) && w->core.num_popups) {
    Widget *popups = w->core.popup_list;
    int num = w->core.num_popups;
    int i;
    for (i = 0; i < num; i++)
      XmdsResetAllXds(popups[i]);
  }
}

Boolean XmdsApplyAllXds(Widget w)
{
  int status = 1;
  if (XmdsIsExpr(w))
    return XmdsExprApply(w);
  else if (XmdsIsExprField(w))
    return XmdsExprFieldApply(w);
  else if (XmdsIsDigChans(w))
    return XmdsDigChansApply(w);
  else if (XmdsIsOnOffToggleButton(w))
    return XmdsOnOffToggleButtonApply(w);
  else if (XmdsIsXdBox(w))
    return XmdsXdBoxApply(w);
  else if (XmdsIsNidOptionMenu(w))
    return XmdsNidOptionMenuApply(w);
  else if (XmdsIsXdBoxOnOffButton(w))
    return XmdsXdBoxOnOffButtonApply(w);
  else if (XtIsComposite(w)) {
    Widget *children;
    int num;
    int i;
    XtVaGetValues(w, XtNchildren, &children, XtNnumChildren, &num, NULL);
    for (i = 0; i < num && status; i++)
      status = XmdsApplyAllXds(children[i]);
  }
  if (XtIsWidget(w) && w->core.num_popups) {
    Widget *popups = w->core.popup_list;
    int num = w->core.num_popups;
    int i;
    for (i = 0; i < num && status; i++)
      status = XmdsApplyAllXds(popups[i]);
  }
  return status & 1;
}
