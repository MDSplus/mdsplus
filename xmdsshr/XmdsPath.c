/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSPATH.C */
/*  *11   23-JUN-1994 12:09:38 TWF "Motif 1.2" */
/*  *10   10-MAY-1993 14:16:37 TWF "Make it shareable" */
/*  *9     8-APR-1993 11:34:27 TWF "Use MrmPublic (much smaller)" */
/*  *8     4-MAR-1993 15:18:16 JAS "make it complile with vaxc" */
/*  *7     3-MAR-1993 16:02:48 JAS "use xmdsshr.h" */
/*  *6     3-MAR-1993 09:34:52 JAS "use prototypes" */
/*  *5    25-FEB-1993 10:37:49 JAS "port to decc" */
/*  *4     6-MAY-1992 17:28:49 JAS "fix" */
/*  *3     6-MAY-1992 16:44:23 JAS "GetNCI call wrong" */
/*  *2     6-MAY-1992 15:10:04 JAS "adding xmdspath" */
/*  *1     6-MAY-1992 14:48:40 JAS "Path widget for displaying node paths" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSPATH.C */
/*------------------------------------------------------------------------------

		Name:   XMDSPATH

		Type:   C function

		Author:	JOSH STILLERMAN

		Date:    6-MAY-1992

		Purpose: Xmds Widget to display a node's path

------------------------------------------------------------------------------

	Call sequence:

  Widget XmdsCreatePath(Widget parent, String name, ArgList args, Cardinal argcount);
  Boolean XmdsIsPath(Widget w);


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
#include <strroutines.h>
#include <ncidef.h>
#include <treeshr.h>
#include <Mrm/MrmPublic.h>
#include <Xm/Xm.h>
#include <Xm/Label.h>
#include <Xmds/XmdsPath.h>
#include <xmdsshr.h>
#define PathUserData 0xAAAAAAAA


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

typedef struct _PathPart
{
  int nid;
  int nid_offset;
  int path_type;
} XmdsPathPart;

static XtResource resources[] = 
{
  {XmdsNnid, "Nid", XmRInt, sizeof(int), XtOffsetOf(XmdsPathPart, nid), XmRImmediate, 0},
  {XmdsNnidOffset, "NidOffset", XmRInt, sizeof(int), XtOffsetOf(XmdsPathPart, nid_offset), XmRImmediate, 0},
  {XmdsNpathType, "PathType", XmRInt, sizeof(int), XtOffsetOf(XmdsPathPart, path_type), XmRImmediate, 0}
};


/*------------------------------------------------------------------------------

 Executable:                                                                  */

Widget XmdsCreatePath(Widget parent,String name,ArgList args,Cardinal argcount)
{
  XmdsPathPart info = {-1,0,0};
  Widget w;
  Arg lab_args[] = {{XmNlabelString, 0}, {XmNuserData, PathUserData}};
  Arg *merged_args;
  int nid;

  XmdsSetSubvalues(&info,resources,XtNumber(resources),args,argcount);
  if (info.nid == -1)
    info.nid = XmdsGetDeviceNid();
  if (info.nid != -1)
    nid = info.nid + info.nid_offset;
  else
    nid = -1;
  if (nid != -1)
  {
    static struct descriptor_d path_dsc = {0, DTYPE_T, CLASS_D, 0};
    NCI_ITM nci[] = {{0, 0, (unsigned char *) &path_dsc, 0}, 
		     {0, NciEND_OF_LIST, 0, 0}};
    int status;
    nci[0].code = (info.path_type == NciABSOLUTE_PATH) ? NciFULLPATH : NciMINPATH;
    nci[0].pointer = (unsigned char *) &path_dsc;
    status = TreeGetNci(nid,nci);
    if (status & 1)
    {
      static DESCRIPTOR(zero_dsc,"\0");
      StrConcat(&path_dsc,&path_dsc,&zero_dsc MDS_END_ARG);
      lab_args[0].value = (long) XmStringCreateSimple(path_dsc.pointer);
    }
    else
      lab_args[0].value = (long) XmStringCreateSimple("Error getting path");
  }
  else
    lab_args[0].value = (long) XmStringCreateSimple("No node");
  merged_args = XtMergeArgLists(args,argcount,lab_args,XtNumber(lab_args));
  w = XmCreateLabel(parent,name,merged_args,XtNumber(lab_args) + argcount);
  XtFree((char *)lab_args[0].value);
  XtFree((char *)merged_args);
  return w;
}

Boolean XmdsIsPath(Widget w)
{
  int user_data = 0;
  XtVaGetValues(w,XmNuserData,user_data,NULL);
  if (user_data && (user_data == PathUserData))
    return 1;
  else
    return 0;
}
