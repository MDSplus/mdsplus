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
/*------------------------------------------------------------------------------

		Name:   PREAMP_ADD   

		Type:   C function

     		Author:	Josh Stillerman

		Date:    4-JAN-1993

    		Purpose: Support routines for MIT PREAMP.

------------------------------------------------------------------------------

	Call sequence: 

EXPORT int PREAMP_ADD(struct descriptor *name,
                     struct descriptor *qualifiers,
                     int                   *head_nid)

------------------------------------------------------------------------------
   Copyright (c) 1993
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

 	Description:

  Support for 16 channel MIT designed preamplifier.

------------------------------------------------------------------------------*/
#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <treeshr.h>
#include <ncidef.h>
#include "preamp_gen.h"
#include <Mrm/MrmPublic.h>
#include <Xm/Xm.h>
#include <xmdsshr.h>
#include <Xmds/XmdsExpr.h>
#include <math.h>
#include "devroutines.h"

#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;
#define INPUT_NID(head_nid,part,i) head_nid + PREAMP_N_##part + i * (PREAMP_N_INPUT_02 - PREAMP_N_INPUT_01)

EXPORT int preamp___add(int *head_nid)
{
  int status;
  int i;
  for (i = 0; i < 16; i++) {
    static EMPTYXD(xd);
    static int g1_nid;
    static int g2_nid;
    static int offset_nid;
    static int output_nid;
    static DESCRIPTOR_NID(g1_dsc, &g1_nid);
    static DESCRIPTOR_NID(g2_dsc, &g2_nid);
    static DESCRIPTOR_NID(offset_dsc, &offset_nid);
    static DESCRIPTOR_NID(output_dsc, &output_nid);
    static DESCRIPTOR(expression, "(-2*$1/$2)+(2/($2*$3))*$4");
    int input_nid = INPUT_NID((*head_nid), INPUT_01, i);
    g1_nid = INPUT_NID((*head_nid), INPUT_01_GAIN1, i);
    g2_nid = INPUT_NID((*head_nid), INPUT_01_GAIN2, i);
    offset_nid = INPUT_NID((*head_nid), INPUT_01_OFFSET, i);
    output_nid = INPUT_NID((*head_nid), INPUT_01_OUTPUT, i);
    TdiCompile((struct descriptor *)&expression, &offset_dsc, &g1_dsc, &g2_dsc, &output_dsc, &xd MDS_END_ARG);
    status = TreePutRecord(input_nid, (struct descriptor *)&xd, 0);
  }
  return status;
}

static void ask_incaa_proc(Widget w);
static Boolean ask_incaa_button(Widget w);
static void ask_incaa_create(Widget w);
EXPORT int preamp__dw_setup(struct descriptor *niddsc __attribute__ ((unused)), struct descriptor *methoddsc __attribute__ ((unused)), Widget parent)
{
  Widget dbox;
  static String uids[] = { "PREAMP.uid" };
  static int nid;
  static NCI_ITM nci[] =
      { {4, NciCONGLOMERATE_NIDS, (unsigned char *)&nid, 0}, {0, NciEND_OF_LIST, 0, 0} };
  static MrmRegisterArg uilnames[] = { {"ask_incaa_proc", (char *)ask_incaa_proc},
  {"ask_incaa_create", (char *)ask_incaa_create},
  {"ask_incaa_button", (char *)ask_incaa_button}
  };
  int status;
  TreeGetNci(*(int *)niddsc->pointer, nci);
  status =
      XmdsDeviceSetup(parent, (int *)niddsc->pointer, uids, XtNumber(uids), "PREAMP", uilnames,
		      XtNumber(uilnames), &dbox);
  return status;
}

static void ask_incaa_create(Widget w)
{
  int device_nid = XmdsGetDeviceNid();
  XtVaSetValues(w, XmNuserData, &device_nid, NULL);
}

static void ask_incaa_proc(Widget w)
{
  Widget par = XtParent(w);
  Widget question = XtNameToWidget(par, "*.incaa_dlog");
  XtManageChild(question);
}

#define InInitStruct IncaaInInitStruct
#include "incaa16_gen.h"
#undef InInitStruct

static Boolean ask_incaa_button(Widget w)
{
  Widget parent = XtParent(XtParent(w));
  int i;
  static int incaa_nid;
  static DESCRIPTOR_NID(incaa_nid_dsc, &incaa_nid);
  int status;
  int *device_nid;
  XmString incaa_string;
  String incaa_name;
  int c_nids[INCAA16_K_CONG_NODES];
  XtVaGetValues(w, XmNuserData, &device_nid, XmNtextString, &incaa_string, NULL);
  incaa_name = XmStringUnparse(incaa_string, NULL, 0, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
  status = TreeFindNode(incaa_name, &incaa_nid);
  if (status & 1) {
    status = DevNids(&incaa_nid_dsc, sizeof(c_nids), c_nids);
    if (status & 1) {
      for (i = 1; i <= 16; i++) {
	DESCRIPTOR_NID(nid_dsc, 0);
	char name[] = { '*', '.', 'o', 'u', 't', 'p', 'u', 't', '_', '0', '0', 0 };
	nid_dsc.pointer =
	    (char *)&c_nids[INCAA16_N_INPUT_01 +
			    (i - 1) * (INCAA16_N_INPUT_02 - INCAA16_N_INPUT_01)];
	name[9] += i / 10;
	name[10] += i % 10;
	XmdsExprSetXd(XtNameToWidget(parent, name), &nid_dsc);
      }
    } else
      XmdsComplain(parent, "Could not find specified INCAA");
  } else
    XmdsComplain(parent, "Could not find specified INCAA");
  return status & 1;
}

static int one = 1;

EXPORT int preamp___init(struct descriptor *niddsc __attribute__ ((unused)), InInitStruct * setup)
{
  int chan;
  int status=1;
  static float gains[] = { 1., 2., 4., 8., 16. };
  for (chan = 0; chan < 16; chan++) {
    int input_nid = INPUT_NID(setup->head_nid, INPUT_01, chan);
    if (TreeIsOn(input_nid) & 1) {
      float gain1;
      float gain2;
      float offset;
      unsigned short g1;
      unsigned short g2;
      unsigned long ctl;
      int g1_nid = INPUT_NID(setup->head_nid, INPUT_01_GAIN1, chan);
      int g2_nid = INPUT_NID(setup->head_nid, INPUT_01_GAIN2, chan);
      int offset_nid = INPUT_NID(setup->head_nid, INPUT_01_OFFSET, chan);
      return_on_error(DevFloat(&g1_nid, &gain1), DEV$_BAD_GAIN);
      return_on_error(DevFloat(&g2_nid, &gain2), DEV$_BAD_GAIN);
      return_on_error(DevFloat(&offset_nid, &offset), DEV$_BAD_OFFSET);
      for (g1 = 0; g1 < 5 && fabs(gains[g1] - gain1) > .1; g1++) ;
      if (g1 == 5)
	return DEV$_BAD_GAIN;
      for (g2 = 0; g2 < 5 && fabs(gains[g2] - gain2) > .1; g2++) ;
      if (g2 == 5)
	return DEV$_BAD_GAIN;
      ctl = (unsigned int)(4095. * (5. - offset) / 10. + .5) | g1 << 12 | g2 << 15;
      return_on_error(DevCamChk(CamPiow(setup->name, chan, 16, &ctl, 24, 0), &one, &one), status);
    }
  }
  return status;
}
