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
