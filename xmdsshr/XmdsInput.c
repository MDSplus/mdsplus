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
Widget XmdsCreateInput(Widget parent,char *name,ArgList args,int argcount)
{
  Widget widg;
  static char *hierarchy_name[] = {"XmdsInput.uid"};
  MrmHierarchy hierarchy = 0;
  MrmType class;
  MrmOpenHierarchy(1,hierarchy_name,0,&hierarchy);
  MrmFetchWidgetOverride(hierarchy,"input_box",parent,name,args,argcount,&widg,&class);
  MrmCloseHierarchy(hierarchy);
  return widg;
}

void XmdsInputSetNid(Widget w,int nid)
{
  WidgetList children;
  int user_data;
  XtVaGetValues(w, XtNchildren, &children, NULL);
  XtVaSetValues(children[START_IDX],XmdsNnid,nid,NULL);
  XtVaSetValues(children[END_IDX],XmdsNnid,nid,NULL);
  XtVaGetValues(children[PATH],XmNuserData,&user_data,NULL);
  user_data += nid;
  XtVaSetValues(children[ON_OFF],XmNuserData,user_data,NULL);
  XtVaSetValues(children[PATH],XmNuserData,user_data,NULL);
  XtVaGetValues(children[IDX_TIME],XmNuserData,&user_data,NULL);
  user_data += nid;
  XtVaSetValues(children[IDX_TIME],XmNuserData,user_data,NULL);
}

void XmdsInputReset(Widget w)
{
  WidgetList children;
  static int nid;
  char *path;
  XmString label;

  XtVaGetValues(w, XtNchildren, &children, NULL);
  XmdsResetAllXds(w);
  XtVaGetValues(children[ON_OFF],XmNuserData,&nid,NULL);
  XmToggleButtonSetState(children[ON_OFF],(TreeIsOn(nid) & 1),(Boolean) 0);
  XtVaGetValues(children[IDX_TIME],XmNuserData,&nid,NULL);
  XmToggleButtonSetState(children[IDX_TIME],XmdsGetNidBooleanValue(nid, 1) & 1,(Boolean) 0);
  XtVaGetValues(children[PATH],XmNuserData,&nid,NULL);
  label = XmStringCreateSimple(path = TreeGetMinimumPath(0,nid));
  TreeFree(path);
  XtVaSetValues(children[PATH],XmNlabelString,label,NULL);
  XmStringFree(label);
}

void XmdsInputPut(Widget w)
{
  WidgetList children;
  int nid;
  XtVaGetValues(w, XtNchildren, &children, NULL);
  XtVaGetValues(children[ON_OFF],XmNuserData,&nid,NULL);
  XmdsSetState(nid,children[ON_OFF]);
  XtVaGetValues(children[IDX_TIME],XmNuserData,&nid,NULL);
  XmdsPutNidToggleButton(children[IDX_TIME],nid,1 & XmdsGetNidBooleanValue(nid, 1));
  XmdsExprPut(children[START_IDX]);
  XmdsExprPut(children[END_IDX]);
}
