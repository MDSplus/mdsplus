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
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSONOFFTOGGLEBUTTON.C */
/*  *13   23-JUN-1994 10:05:27 TWF "Motif 1.2" */
/*  *12   22-FEB-1994 15:18:13 TWF "remove NO_X_GBLS" */
/*  *11    3-MAR-1993 16:01:41 JAS "use xmdsshr.h" */
/*  *10    3-MAR-1993 09:33:34 JAS "use prototypes" */
/*  *9    25-FEB-1993 09:41:55 JAS "port to decc" */
/*  *8    10-JUN-1992 14:12:36 JAS "add the label argument" */
/*  *7    10-MAR-1992 10:48:52 JAS "add showpath resource" */
/*  *6    10-MAR-1992 10:48:01 JAS "add showpath resource" */
/*  *5    21-JAN-1992 10:39:46 TWF "Support -1 device_nid" */
/*  *4    15-JAN-1992 11:41:29 TWF "Do not use userdata" */
/*  *3    14-JAN-1992 15:23:12 TWF "Fix include" */
/*  *2    14-JAN-1992 14:52:54 TWF "Add XmdsOnOffToggleButton widget" */
/*  *1    14-JAN-1992 14:39:06 TWF "XmdsOnOffToggleButton widget" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSONOFFTOGGLEBUTTON.C */
/*------------------------------------------------------------------------------

		Name:   XmdsOnOffToggleButton

		Type:   C function

		Author:	TOM FREDIAN

		Date:   14-JAN-1992

		Purpose:  On/Off Toggle button Pseudo-widget

------------------------------------------------------------------------------

	Call sequence:

Widget XmdsCreateOnOffToggleButton(Widget parent, String name, ArgList args, Cardinal argcount);
Boolean XmdsIsOnOffToggleButton(Widget w);
void XmdsOnOffToggleButtonReset(Widget w);
int XmdsOnOffToggleButtonPut(Widget w);
int XmdsOnOffToggleButtonApply(Widget w);

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------*/
#include <mdsplus/mdsconfig.h>
#include <mdsdescrip.h>
#include <strroutines.h>
#include <ncidef.h>
#include <treeshr.h>
#include <Xm/ToggleB.h>
#include <Xmds/XmdsOnOffToggleButton.h>
#include <Mrm/MrmPublic.h>
#include <xmdsshr.h>
Widget XmdsCreateOnOffToggleButton(Widget parent, String name, ArgList args, Cardinal argcount);
Boolean XmdsIsOnOffToggleButton(Widget w);
void XmdsOnOffToggleButtonReset(Widget w);
int XmdsOnOffToggleButtonPut(Widget w);
int XmdsOnOffToggleButtonApply(Widget w);

typedef struct _Resources {
  int nid;
  int nid_offset;
  Boolean show_path;
  XmString label;
  Boolean put_on_apply;
} Resources;

static Resources *GetResources(Widget w);
static void Destroy(Widget w, Resources * info, XtPointer cb);

static XtResource resources[] = {
  {XmdsNnid, "Nid", XmRInt, sizeof(int), XtOffsetOf(Resources, nid), XmRImmediate, 0},
  {XmdsNnidOffset, "Nid", XmRInt, sizeof(int), XtOffsetOf(Resources, nid_offset), XmRImmediate, 0},
  {XmdsNshowPath, "ShowPath", XmRBoolean, sizeof(Boolean), XtOffsetOf(Resources, show_path),
   XmRImmediate, 0}
  ,
  {XmNlabelString, "LabelString", XmRString, sizeof(XmString), XtOffsetOf(Resources, label),
   XmRImmediate, 0}
  ,
  {XmdsNputOnApply, "PutOnApply", XmRBoolean, sizeof(Boolean), XtOffsetOf(Resources, put_on_apply),
   XmRImmediate, (void *)1}
};

EXPORT Widget XmdsCreateOnOffToggleButton(Widget parent, String name, ArgList args, Cardinal argcount)
{
  Widget w;
  Resources *info = (Resources *) XtMalloc(sizeof(Resources));
  Resources default_info = { 0, 0, 0, 0, 1 };
  *info = default_info;
  XmdsSetSubvalues(info, resources, XtNumber(resources), args, argcount);
  if (info->nid == -1)
    info->nid = XmdsGetDeviceNid();
  w = XmCreateToggleButton(parent, name, args, argcount);
  if (info->show_path && info->nid + info->nid_offset) {
    char *path_c;
    XmString path;
    path_c = TreeGetMinimumPath(0, info->nid + info->nid_offset);
    path = XmStringCreateSimple(path_c);
    TreeFree(path_c);
    XtVaSetValues(w, XmNlabelString, path, NULL);
    XmStringFree(path);
  } else if (info->label)
    XtVaSetValues(w, XmNlabelString, info->label, NULL);

  XtAddCallback(w, XmNdestroyCallback, (XtCallbackProc) Destroy, info);
  XmdsOnOffToggleButtonReset(w);
  return w;
}

static void Destroy(Widget w, Resources * info, XtPointer cb)
{
  XtFree((char *)info);
}

EXPORT Boolean XmdsIsOnOffToggleButton(Widget w)
{
  return GetResources(w) != 0;
}

static Resources *GetResources(Widget w)
{
  Resources *answer = 0;
  if (XmIsToggleButton(w) && (XtHasCallbacks(w, XmNdestroyCallback) == XtCallbackHasSome)) {
    XtCallbackList callbacks;
    XtVaGetValues(w, XmNdestroyCallback, &callbacks, NULL);
    for (;
	 callbacks->callback
	 && !(answer =
	      (Resources *) ((callbacks->callback == (XtCallbackProc) Destroy) ? callbacks->
			     closure : 0)); callbacks = callbacks + 1) ;
  }
  return answer;
}

EXPORT void XmdsOnOffToggleButtonReset(Widget w)
{
  Resources *info = GetResources(w);
  if (info)
    XmToggleButtonSetState(w, XmdsIsOn(info->nid + info->nid_offset), 0);
}

EXPORT int XmdsOnOffToggleButtonPut(Widget w)
{
  Resources *info = GetResources(w);
  return info ? XmdsSetState(info->nid + info->nid_offset, w) : 0;
}

EXPORT int XmdsOnOffToggleButtonApply(Widget w)
{
  Resources *info = GetResources(w);
  return info ? (info->put_on_apply ? XmdsOnOffToggleButtonPut(w) : 1) : 0;
}
