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
#include <xmdsshr.h>

void XmdsSetDeviceNid(int nid);

EXPORT int XmdsDeviceSetup(Widget parent, int *nid, String uids[], Cardinal num_uids, String ident,
		    MrmRegisterArglist reglist, MrmCount regnum, Widget * widget_return)
{
  static int device_nid;
  static NCI_ITM nci[] =
      { {sizeof(int), NciCONGLOMERATE_NIDS, (unsigned char *)&device_nid, 0}, {0, NciEND_OF_LIST, 0,
									       0} };
  MrmType class;
  MrmHierarchy drm_hierarchy;
  Widget w;
  int status;
  TreeGetNci(*nid, nci);
  XmdsSetDeviceNid(device_nid);
  status = MrmOpenHierarchy(num_uids, uids, 0, &drm_hierarchy);
  if (status == MrmSUCCESS) {
    if (regnum)
      MrmRegisterNamesInHierarchy(drm_hierarchy, reglist, regnum);
    status = MrmFetchWidget(drm_hierarchy, ident, parent, &w, &class);
    MrmCloseHierarchy(drm_hierarchy);
    if (status == MrmSUCCESS)
      XtManageChild(w);
  }
  XmdsSetDeviceNid(0);
  if (widget_return)
    *widget_return = w;
  return status;
}

static int device_nid = 0;

EXPORT void XmdsSetDeviceNid(int nid)
{
  device_nid = nid;
}

EXPORT int XmdsGetDeviceNid()
{
  return device_nid;
}
