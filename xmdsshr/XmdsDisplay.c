/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSDISPLAY.C */
/*  *9    23-JUN-1994 12:11:18 TWF "Motif 1.2" */
/*  *8    22-FEB-1994 15:18:05 TWF "remove NO_X_GBLS" */
/*  *7    10-MAY-1993 14:08:04 TWF "Make image shareable" */
/*  *6     8-APR-1993 10:13:00 TWF "Fix bugcheck" */
/*  *5     4-MAR-1993 15:08:01 JAS "make it complile with vaxc" */
/*  *4     3-MAR-1993 15:49:54 JAS "use xmdsshr.h" */
/*  *3    22-FEB-1993 18:08:53 JAS "port to decc" */
/*  *2    22-FEB-1993 11:12:27 JAS "port to decc" */
/*  *1    19-JAN-1993 09:29:49 JAS "new widget to display decompile(value())" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSDISPLAY.C */
/*------------------------------------------------------------------------------

		Name:   XMDSDISPLAY   

		Type:   C function

     		Author:	JOSH STILLERMAN

		Date:    6-MAY-1992

    		Purpose: Xmds Widget to display a node's contents. (e.g. the
                         decompile ( value ( node )) 

------------------------------------------------------------------------------

	Call sequence: 

  Widget XmdsCreateDisplay(Widget parent, String name, ArgList args, Cardinal argcount);
  Boolean XmdsIsDisplay(Widget w);


------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

 	Description:


------------------------------------------------------------------------------*/

#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <Mrm/MrmPublic.h>
#include <Xmds/XmdsDisplay.h>
#include <Xm/Label.h>
#define DisplayUserData 0xABCDEFAB


/*------------------------------------------------------------------------------

 External functions or symbols referenced:                                    */

#include <xmdsshr.h>
  extern int TdiEvaluate();
  extern int TdiDecompile();
/*------------------------------------------------------------------------------

 Subroutines referenced:                                                      */

/*------------------------------------------------------------------------------

 Macros:                                                                      */

/*------------------------------------------------------------------------------

 Global variables:                                                            */

/*------------------------------------------------------------------------------

 Local variables:                                                             */

typedef struct _DisplayPart
{
  int       nid;
  int       nid_offset;
}         XmdsDisplayPart;

static XtResource resources[] = 
{
  {XmdsNnid, "Nid", XmRInt, sizeof(int), XtOffsetOf(XmdsDisplayPart, nid), XmRImmediate, 0},
  {XmdsNnidOffset, "NidOffset", XmRInt, sizeof(int), XtOffsetOf(XmdsDisplayPart, nid_offset), XmRImmediate, 0}
};


/*------------------------------------------------------------------------------

 Executable:                                                                  */

Widget XmdsCreateDisplay(Widget parent, String name, ArgList args, Cardinal argcount)
{
  XmdsDisplayPart info = {-1,0};
  Widget w;
  Arg lab_args[] = {{XmNlabelString, 0}, {XmNuserData, DisplayUserData}};
  Arg *merged_args;
  int nid;

  XmdsSetSubvalues(&info, resources, XtNumber(resources), args, argcount);
  if (info.nid == -1)
    info.nid = XmdsGetDeviceNid();
  if (info.nid != -1)
    nid = info.nid + info.nid_offset;
  else
    nid = -1;
  if (nid != -1) {
    static struct descriptor_d display_dsc = {0, DTYPE_T, CLASS_D, 0};
    static struct descriptor_xd xd = {0, DTYPE_DSC, CLASS_XD, 0, 0};
    struct descriptor nid_dsc = {sizeof(int), DTYPE_NID, CLASS_S, (char *)0};
    int status;
    nid_dsc.pointer = (char *)&nid;
    status = TdiEvaluate(&nid_dsc, &xd MDS_END_ARG);
    if (status&1) {
      status = TdiDecompile(&xd, &display_dsc MDS_END_ARG);
      if (status&1) {
        static DESCRIPTOR(zero_dsc, "\0");
        StrConcat(&display_dsc, &display_dsc, &zero_dsc, NULL);
        lab_args[0].value = (long)XmStringCreateSimple(display_dsc.pointer);
      }
      else
        lab_args[0].value = (long)XmStringCreateSimple("Error - Decomp");
    }
    else
      lab_args[0].value = (long)XmStringCreateSimple("Error - Eval");
  }
  merged_args = XtMergeArgLists(args, argcount, lab_args, XtNumber(lab_args));
  w = XmCreateLabel(parent, name, merged_args, XtNumber(lab_args) + argcount);
  XtFree((char *)lab_args[0].value);
  XtFree((char *)merged_args);
  return w;
}

Boolean XmdsIsDisplay(Widget w)
{
  int user_data = 0;
  XtVaGetValues(w, XmNuserData, user_data, NULL);
  if (user_data && (user_data == DisplayUserData))
    return 1;
  else
    return 0;
}
