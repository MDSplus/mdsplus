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
#ifndef _WIN32
#include <mdsdescrip.h>
#include <treeshr.h>
#include <ncidef.h>
#include <Mrm/MrmPublic.h>
#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <xmdsshr.h>

static void ManageChild(Widget w, char *name);
static void UnManageChild(Widget w, char *name);

EXPORT int dt_acq16__dw_setup(struct descriptor *niddsc __attribute__ ((unused)), struct descriptor *methoddsc __attribute__ ((unused)), Widget parent)
{
  static String uids[] = { "DT_ACQ16.uid" };
  static int nid;
  static MrmRegisterArg uilnames[] = {
    {"nid", (XtPointer) 0},
    {"ManageChild", (XtPointer) ManageChild},
    {"UnManageChild", (XtPointer) UnManageChild},
  };
  static NCI_ITM nci[] =
      { {4, NciCONGLOMERATE_NIDS, (unsigned char *)&nid, 0}, {0, NciEND_OF_LIST, 0, 0} };
  TreeGetNci(nid, nci);
  uilnames[0].value = (XtPointer) (nid + (char *)0);
  return XmdsDeviceSetup(parent, (int *)niddsc->pointer, uids, XtNumber(uids), "DT_ACQ16", uilnames,
			 XtNumber(uilnames), 0);
}

static Widget FindTop(Widget w)
{
  for (; w && ((w->core.name == 0) || strcmp(w->core.name, "DT_ACQ16")); w = XtParent(w)) ;
  return w;
}

static void ManageChild(Widget w, char *name)
{
  Widget par = FindTop(w);
  if (par) {
    XtManageChild(XtNameToWidget(par, name));
  }
}

static void UnManageChild(Widget w, char *name)
{
  Widget par = FindTop(w);
  if (par) {
    XtUnmanageChild(XtNameToWidget(par, name));
  }
}
#endif
