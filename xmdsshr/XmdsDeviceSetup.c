/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSDEVICESETUP.C */
/*  *9     8-APR-1993 11:34:06 TWF "Use MrmPublic (much smaller)" */
/*  *8     3-MAR-1993 09:11:39 JAS "use prototypes" */
/*  *7    23-FEB-1993 13:29:16 JAS "port to decc" */
/*  *6    22-FEB-1993 11:06:01 JAS "port to decc" */
/*  *5     4-DEC-1992 16:52:47 JAS "add optional last arg to return the widget fetched in" */
/*  *4    30-JAN-1992 08:53:00 TWF "Add uil names argument to devicesetup" */
/*  *3    21-JAN-1992 11:03:08 JAS "compilation problem" */
/*  *2    21-JAN-1992 10:39:30 TWF "Support -1 device_nid" */
/*  *1    14-JAN-1992 17:21:15 TWF "Generic Device Setup routine" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSDEVICESETUP.C */
/*------------------------------------------------------------------------------

		Name:   XmdsDeviceSetup

		Type:   C function

		Author:	TOM FREDIAN

		Date:   14-JAN-1992

		Purpose:  Generic Device Setup Routine

------------------------------------------------------------------------------

	Call sequence:

int XmdsDeviceSetup(Widget parent, int *nid, String uids[], Cardinal num_uids, String ident, MrmRegisterArglist reglist,
			 MrmCount regnum [,widget_return])

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:


------------------------------------------------------------------------------*/

#include <ncidef.h>
#include <Mrm/MrmPublic.h>
#include <Xm/Xm.h>
#include <treeshr.h>

void XmdsSetDeviceNid(int nid);

int XmdsDeviceSetup(Widget parent,int *nid,String uids[],Cardinal num_uids,String ident,MrmRegisterArglist reglist,
		    MrmCount regnum,Widget *widget_return)
{
  static int device_nid;
  static NCI_ITM nci[] = {{sizeof(int), NciCONGLOMERATE_NIDS, (unsigned char *) &device_nid, 0}, {0, NciEND_OF_LIST, 0, 0}};
  MrmType class;
  MrmHierarchy drm_hierarchy;
  Widget w;
  int status;
  TreeGetNci(*nid,nci);
  XmdsSetDeviceNid(device_nid);
  status = MrmOpenHierarchy(num_uids,uids,0,&drm_hierarchy);
  if (status == MrmSUCCESS)
  {
    if (regnum)
      MrmRegisterNamesInHierarchy(drm_hierarchy,reglist,regnum);
    status = MrmFetchWidget(drm_hierarchy,ident,parent,&w,&class);
    MrmCloseHierarchy(drm_hierarchy);
    if (status == MrmSUCCESS)
      XtManageChild(w);
  }
  XmdsSetDeviceNid(0);
  if (widget_return) *widget_return = w;
  return status;
}

static int device_nid = 0;

void XmdsSetDeviceNid(int nid)
{
  device_nid = nid;
}

int XmdsGetDeviceNid()
{
  return device_nid;
}
