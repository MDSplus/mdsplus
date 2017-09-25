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
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSINPUT.C */
/*  *8    10-MAY-1993 14:16:44 TWF "Make it shareable" */
/*  *7     8-APR-1993 11:34:15 TWF "Use MrmPublic (much smaller)" */
/*  *6     3-MAR-1993 15:56:22 JAS "use xmdsshr.h" */
/*  *5     3-MAR-1993 09:22:30 JAS "use prototypes" */
/*  *4    24-FEB-1993 15:43:39 JAS "port to decc" */
/*  *3    24-FEB-1993 14:42:15 JAS "port to decc" */
/*  *2     2-JAN-1992 17:23:28 TWF "Fix routine names" */
/*  *1     2-JAN-1992 16:59:45 TWF "Digitizer channel widget" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSINPUT.C */
/*------------------------------------------------------------------------------

		Name:   XmdsInput

		Type:   Widget (actually an attached dialog box widget)

		Author:	JOSH STILLERMAN

		Date:   24-JUL-1990

		Purpose:Impliments a digitizer input widiget for the control
			of digitizer channels.

------------------------------------------------------------------------------

	Call sequence:

  Widget INPUT$CREATE( Widget parent, struct dsc$descriptor *name, ArgList args, int *argcount );

  Widget InputCreate( Widget parent, char *name, ArgList args, int argcount );
  void InputPut(Widget w);
  void InputReset(Widget w);
  void InputSetNid(Widget w, int nid);

------------------------------------------------------------------------------
   Copyright (c) 1990
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

  To use this widget the following identifiers must be registered with
  DwtRegisterNames so that the Reset and Put routines will know the offsets
  in the congomerate from the input head which contain the various pieces
  of the input.

	   data_offset;
	   end_offset;
	   idx_time_offset;
	   start_offset;

  For example the 8210 DW_SETUP routine contains the following code fragment:
    Arg uilnames[] = {{"Destroy", Destroy},
		      ...
		      {"data_offset", L8210$N_INP_HEAD},
		      {"end_offset", L8210$N_INP_ENDIDX},
		      {"idx_time_offset", L8210$N_INP_USE_TIMES},
		      {"start_offset", L8210$N_INP_STARTIDX}};
    MrmRegisterNames(uilnames,XtNumber(uilnames));
    ...
    MrmFetchWidget(...)
    for(chan=0; chan<4; chan++) {
      inputs_name[19] = '1' + chan;
      InputSetNid(XtNameToWidget(ctx->l8210_w, inputs_name), ctx->nid+L8210$N_INPUTS+chan*L8210$K_NODES_PER_INP);
    }
    ...

    The InputSetNid call attaches an instance of this widget to a particular input
    nid in the device conglomerate.

    The Device Reset routine should call InputReset for each channel.
    -- Note that the device Reset routine must NOT BE CALLED before the Input widgets
    are realized or some of the toggle buttons will not function correctly.  A good
    place to call Reset initially is in the map callback for the top level widget of the
    device.

------------------------------------------------------------------------------*/
#include <mdsdescrip.h>
#include <strroutines.h>
#include <treeshr.h>
#include <Mrm/MrmPublic.h>
#include <Xm/Xm.h>
#include <Xm/ToggleB.h>
#include <Xmds/XmdsExpr.h>
#include <xmdsshr.h>
/*------------------------------------------------------------------------------

 Internal functions or symbols:                                               */

#include <Xmds/XmdsInput.h>

/*------------------------------------------------------------------------------

 Subroutines referenced:                                                      */

/*------------------------------------------------------------------------------

 Macros:                                                                      */

#define ON_OFF 0
#define IDX_TIME 1
#define START_IDX 2
#define END_IDX 3
#define PATH 4

/*------------------------------------------------------------------------------

 Global variables:                                                            */

/*------------------------------------------------------------------------------

 Local variables:                                                             */


/*------------------------------------------------------------------------------

 Executable:                                                                  */

/****************************************************
  Routine to create an Input Widget.  Use
  InputSetNid, InputReset, and InputPut to attach to
  the nids of a particular channel.
****************************************************/
EXPORT Widget XmdsCreateInput(Widget parent, char *name, ArgList args, int argcount)
{
  Widget widg;
  static char *hierarchy_name[] = { "XmdsInput.uid" };
  MrmHierarchy hierarchy = 0;
  MrmType class;
  MrmOpenHierarchy(1, hierarchy_name, 0, &hierarchy);
  MrmFetchWidgetOverride(hierarchy, "input_box", parent, name, args, argcount, &widg, &class);
  MrmCloseHierarchy(hierarchy);
  return widg;
}

EXPORT void XmdsInputSetNid(Widget w, int nid)
{
  WidgetList children;
  XtPointer user_data;
  XtVaGetValues(w, XtNchildren, &children, NULL);
  XtVaSetValues(children[START_IDX], XmdsNnid, nid, NULL);
  XtVaSetValues(children[END_IDX], XmdsNnid, nid, NULL);
  XtVaGetValues(children[PATH], XmNuserData, &user_data, NULL);
  user_data += nid;
  XtVaSetValues(children[ON_OFF], XmNuserData, user_data, NULL);
  XtVaSetValues(children[PATH], XmNuserData, user_data, NULL);
  XtVaGetValues(children[IDX_TIME], XmNuserData, &user_data, NULL);
  user_data += nid;
  XtVaSetValues(children[IDX_TIME], XmNuserData, user_data, NULL);
}

EXPORT void XmdsInputReset(Widget w)
{
  WidgetList children;
  XtPointer userdata;
  int nid;
  char *path;
  XmString label;

  XtVaGetValues(w, XtNchildren, &children, NULL);
  XmdsResetAllXds(w);
  XtVaGetValues(children[ON_OFF], XmNuserData, &userdata, NULL);
  nid = (char *)userdata - (char *)0;
  XmToggleButtonSetState(children[ON_OFF], (TreeIsOn((int)nid) & 1), (Boolean) 0);
  XtVaGetValues(children[IDX_TIME], XmNuserData, &userdata, NULL);
  nid = (char *)userdata - (char *)0;
  XmToggleButtonSetState(children[IDX_TIME], XmdsGetNidBooleanValue(nid, 1) & 1, (Boolean) 0);
  XtVaGetValues(children[PATH], XmNuserData, &userdata, NULL);
  nid = (char *)userdata - (char *)0;
  label = XmStringCreateSimple(path = TreeGetMinimumPath(0, nid));
  TreeFree(path);
  XtVaSetValues(children[PATH], XmNlabelString, label, NULL);
  XmStringFree(label);
}

EXPORT void XmdsInputPut(Widget w)
{
  WidgetList children;
  XtPointer userdata;
  int nid;
  XtVaGetValues(w, XtNchildren, &children, NULL);
  XtVaGetValues(children[ON_OFF], XmNuserData, &userdata, NULL);
  nid = (char *)userdata - (char *)0;
  XmdsSetState(nid, children[ON_OFF]);
  XtVaGetValues(children[IDX_TIME], XmNuserData, &userdata, NULL);
  nid = (char *)userdata - (char *)0;
  XmdsPutNidToggleButton(children[IDX_TIME], nid, 1 & XmdsGetNidBooleanValue(nid, 1));
  XmdsExprPut(children[START_IDX]);
  XmdsExprPut(children[END_IDX]);
}
