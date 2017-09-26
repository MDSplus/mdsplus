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
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSDIGCHANS.C */
/*  *14    4-OCT-1993 10:56:51 JAS "Make start and end optional" */
/*  *13   10-MAY-1993 14:16:41 TWF "Make it shareable" */
/*  *12    8-APR-1993 11:25:48 TWF "Use MrmPublic (much smaller)" */
/*  *11    3-MAR-1993 15:48:19 JAS "use xmdsshr.h" */
/*  *10    3-MAR-1993 09:15:09 JAS "use prototypes" */
/*  *9    23-FEB-1993 13:22:49 JAS "port to decc" */
/*  *8    22-FEB-1993 11:11:35 JAS "port to decc" */
/*  *7    21-FEB-1992 09:34:38 TWF "Make apply on put the defaule" */
/*  *6    21-JAN-1992 10:39:37 TWF "Support -1 device_nid" */
/*  *5    14-JAN-1992 13:02:09 TWF "Add XmdsSetSubValues" */
/*  *4    14-JAN-1992 12:41:08 TWF "Change to nid and nid_offset" */
/*  *3    14-JAN-1992 12:14:50 TWF "Add XmdsIsDigChans" */
/*  *2     7-JAN-1992 14:54:57 TWF "Change defaults" */
/*  *1     7-JAN-1992 14:24:45 TWF "Digitizer channels box" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element XMDSDIGCHANS.C */
/*------------------------------------------------------------------------------

		Name:   XMDSDIGCHANS

		Type:   C function

		Author:	JOSH STILLERMAN

		Date:   22-FEB-1993

		Purpose: a pseudo widget for digitizer channels.  With start
			 end, on/off, and path

------------------------------------------------------------------------------

	Call sequence:

Widget XmdsCreateDigChans(Widget parent, String name, ArgList args, Cardinal argcount)
void XmdsDigChansReset(Widget w)
int XmdsDigChansPut(Widget w)
int XmdsDigChansApply(Widget w)
Boolean XmdsIsDigChans(Widget w)

------------------------------------------------------------------------------
   Copyright (c) 1993
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------

 External functions or symbols referenced:                                    */

#include <stdio.h>
#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <ncidef.h>
#include <treeshr.h>
#include <Mrm/MrmPublic.h>
#include <Xmds/XmdsDigChans.h>
#include <Xm/ToggleBG.h>

#include <Xmds/XmdsExpr.h>
#include <xmdsshr.h>
/*------------------------------------------------------------------------------

 Subroutines referenced:                                                      */

/*------------------------------------------------------------------------------

 Macros:                                                                      */

/*------------------------------------------------------------------------------

 Global variables:                                                            */

/*------------------------------------------------------------------------------

 Local variables:                                                             */

void XmdsDigChansReset(Widget w);

typedef struct _DigChansPart {
  int channels;
  int nid;
  int nid_offset;
  int data_nid_offset;
  int startidx_nid_offset;
  int endidx_nid_offset;
  int nodes_per_channel;
  Boolean put_on_apply;
} XmdsDigChansPart;

static XtResource resources[] = {
  {XmdsNchannels, "Channels", XmRInt, sizeof(int), XtOffsetOf(XmdsDigChansPart, channels),
   XmRImmediate, (void *)4},
  {XmdsNnid, "Nid", XmRInt, sizeof(int), XtOffsetOf(XmdsDigChansPart, nid), XmRImmediate, NULL},
  {XmdsNnidOffset, "NidOffset", XmRInt, sizeof(int), XtOffsetOf(XmdsDigChansPart, nid_offset),
   XmRImmediate, NULL},
  {XmdsNdataNidOffset, "NidOffset", XmRInt, sizeof(int),
   XtOffsetOf(XmdsDigChansPart, data_nid_offset), XmRImmediate, NULL},
  {XmdsNstartIdxNidOffset, "NidOffset", XmRInt, sizeof(int),
   XtOffsetOf(XmdsDigChansPart, startidx_nid_offset), XmRImmediate,
   (void *)1},
  {XmdsNendIdxNidOffset, "NidOffset", XmRInt, sizeof(int),
   XtOffsetOf(XmdsDigChansPart, endidx_nid_offset), XmRImmediate,
   (void *)2},
  {XmdsNnodesPerChannel, "Nodes", XmRInt, sizeof(int),
   XtOffsetOf(XmdsDigChansPart, nodes_per_channel), XmRImmediate,
   (void *)3},
  {XmdsNputOnApply, "PutOnApply", XmRBoolean, sizeof(Boolean),
   XtOffsetOf(XmdsDigChansPart, put_on_apply), XmRImmediate,
   (void *)1}
};

/*------------------------------------------------------------------------------

 Executable:                                                                  */

EXPORT Widget XmdsCreateDigChans(Widget parent, String name, ArgList args, Cardinal argcount)
{
  XmdsDigChansPart info = { 4, -1, 0, 0, 1, 2, 3, 1 };
  static String hierarchy_name[] = { "XmdsDigChans.uid" };
  MrmHierarchy drm_hierarchy;
  MrmType class;
  int i;
  Widget rowcol_w;
  Widget channels_w;
  Widget w;
  XmdsSetSubvalues(&info, resources, XtNumber(resources), args, argcount);
  if (info.nid == -1)
    info.nid = XmdsGetDeviceNid();
  MrmOpenHierarchy(XtNumber(hierarchy_name), hierarchy_name, 0, &drm_hierarchy);
  if (info.nodes_per_channel > 1)
    MrmFetchWidgetOverride(drm_hierarchy, "channels", parent, name, args, argcount, &channels_w,
			   &class);
  else
    MrmFetchWidgetOverride(drm_hierarchy, "channels_1", parent, name, args, argcount, &channels_w,
			   &class);
  XtVaSetValues(XtNameToWidget(channels_w, "this_is_a_DigChans_widget"), XmNuserData,
		(char *)0 + info.put_on_apply, NULL);
  rowcol_w = XtNameToWidget(channels_w, "*c_rowcol");
  for (i = 0; i < info.channels; i++) {
    char name_c[4];
    XmString name;
    XmString path;
    /*    static struct descriptor path_s = {0,DTYPE_T,CLASS_D,0}; */
    static unsigned char path_s[256 * 12];
    static int path_len;
    static NCI_ITM nci[] = { {256 * 12, NciMINPATH, path_s, &path_len}, {0, NciEND_OF_LIST, 0, 0} };
    /*    static DESCRIPTOR(zero,"\0"); */
    int head_nid = i * info.nodes_per_channel + info.nid + info.nid_offset;
    int data_nid = head_nid + info.data_nid_offset;
    int startidx_nid = head_nid + info.startidx_nid_offset;
    int endidx_nid = head_nid + info.endidx_nid_offset;
    sprintf(name_c, "%d", i + 1);
    name = XmStringCreateSimple(name_c);
    TreeGetNci(head_nid, nci);
    /*    StrConcat(&path_s,&path_s,&zero MDS_END_ARG); */
    path_s[path_len] = '\0';
    path = XmStringCreateSimple((char *)path_s);
    /* StrFree1Dx(&path_s); */
    {
      static MrmRegisterArg uilnames[] = { {"c_nid", NULL},
      {"c_name", NULL},
      {"c_startidx_nid", NULL},
      {"c_endidx_nid", NULL},
      {"c_path", NULL}
      };
      uilnames[0].value = data_nid + (char *)0;
      uilnames[1].value = name;
      uilnames[2].value = startidx_nid + (char *)0;
      uilnames[3].value = endidx_nid + (char *)0;
      uilnames[4].value = path;
      MrmRegisterNamesInHierarchy(drm_hierarchy, uilnames, XtNumber(uilnames));
      if (info.nodes_per_channel > 1)
	MrmFetchWidget(drm_hierarchy, "channel_dlog", rowcol_w, &w, &class);
      else
	MrmFetchWidget(drm_hierarchy, "channel_dlog_1", rowcol_w, &w, &class);
      XtManageChild(w);
    }
    XmStringFree(name);
    XmStringFree(path);
  }
  MrmCloseHierarchy(drm_hierarchy);
  XmdsDigChansReset(channels_w);
  return channels_w;
}

EXPORT void XmdsDigChansReset(Widget w)
{
  Widget rowcol_w = XtNameToWidget(w, "*c_rowcol");
  int i;
  int num;
  Widget *chan_w;
  XtVaGetValues(rowcol_w, XmNnumChildren, &num, XmNchildren, &chan_w, NULL);
  for (i = 0; i < num; i++) {
    XtPointer userdata;
    int nid;
    XtVaGetValues(chan_w[i], XmNuserData, &userdata, NULL);
    nid = (char *)userdata - (char *)0;
    XmToggleButtonGadgetSetState(XtNameToWidget(chan_w[i], "*on_off_button"), XmdsIsOn((int)nid),
				 FALSE);
  }
  XmdsResetAllXds(rowcol_w);
}

EXPORT int XmdsDigChansPut(Widget w)
{
  Widget rowcol_w = XtNameToWidget(w, "*c_rowcol");
  int i;
  int num;
  int status;
  Widget *chan_w;
  XtVaGetValues(rowcol_w, XmNnumChildren, &num, XmNchildren, &chan_w, NULL);
  if ((status = XmdsXdsAreValid(w))) {
    for (i = 0; i < num; i++) {
      XtPointer user_data;
      int nid;
      int num_ctls;
      Widget *children;
      XtVaGetValues(chan_w[i], XmNnumChildren, &num_ctls, XmNchildren, &children, XmNuserData,
		    &user_data, NULL);
      nid = (char *)user_data - (char *)0;
      if (XmToggleButtonGadgetGetState(children[1]))
	TreeTurnOn(nid);
      else
	TreeTurnOff(nid);
      if (num_ctls > 3) {
	XmdsExprPut(children[2]);
	XmdsExprPut(children[3]);
      }
    }
  }
  return status;
}

EXPORT int XmdsDigChansApply(Widget w)
{
  XtPointer PutOnApply;
  XtVaGetValues(XtNameToWidget(w, "this_is_a_DigChans_widget"), XmNuserData, &PutOnApply, NULL);
  return PutOnApply ? XmdsDigChansPut(w) : 1;
}

EXPORT Boolean XmdsIsDigChans(Widget w)
{
  return XtNameToWidget(w, "this_is_a_DigChans_widget") != 0;
}
