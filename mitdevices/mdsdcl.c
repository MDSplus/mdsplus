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
#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <treeshr.h>
#include <ncidef.h>
#include <libroutines.h>
#include <strroutines.h>
#include <Mrm/MrmPublic.h>
#include <Xm/Xm.h>
#include <Xm/List.h>
#include <xmdsshr.h>
#include "mdsdcl_gen.h"


extern int mdsdcl_do_command();
static int Apply();
static void Reset();

#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;

EXPORT int mdsdcl___execute(struct descriptor *niddsc_ptr __attribute__ ((unused)), InExecuteStruct * setup)
{
  int status = 1;
  char *line;
  for (line = strtok(setup->verbs, "\n"); line; line = strtok(0, "\n")) {
    char cmd[128];
    strcpy(cmd, "set command ");
    strncat(cmd, line, 125);
    return_on_error(mdsdcl_do_command(cmd), status);
  }
  for (line = strtok(setup->commands, "\n"); line; line = strtok(0, "\n")) {
    if (strlen(line))
      return_on_error(mdsdcl_do_command(line), status);
  }
  return status;
}

EXPORT int mdsdcl__dw_setup(struct descriptor *niddsc __attribute__ ((unused)), struct descriptor *methoddsc __attribute__ ((unused)), Widget parent)
{
  static String uids[] = { "MDSDCL.uid" };
  static int nid;
  static MrmRegisterArg uilnames[] = { {"selections_nid", (XtPointer) 0},
  {"Apply", (XtPointer) Apply},
  {"Reset", (XtPointer) Reset}
  };
  static NCI_ITM nci[] =
      { {4, NciCONGLOMERATE_NIDS, (unsigned char *)&nid, 0}, {0, NciEND_OF_LIST, 0, 0} };
  //String verbs;

/*------------------------------------------------------------------------------

 Executable:                                                                  */

  TreeGetNci(*(int *)niddsc->pointer, nci);
  uilnames[0].value = (char *)0 + (nid + MDSDCL_N_VERBS);
  XmdsDeviceSetup(parent, (int *)niddsc->pointer, uids, XtNumber(uids), "MDSDCL", uilnames,
		  XtNumber(uilnames), 0);
  return 1;
}

static int Apply(Widget w)
{
  Widget list_w = XtNameToWidget(XtParent(w), "*interpreters");
  XtPointer user_data;
  int nid;
  XmString *selections = NULL;
  String list = 0;
  String old_list;
  int num;
  int i;
  int status = 1;
  XtVaGetValues(list_w, XmNuserData, &user_data, NULL);
  nid = (intptr_t) user_data;
  old_list = XmdsGetNidText(nid);
  XtVaGetValues(list_w, XmNselectedItems, &selections, XmNselectedItemCount, &num, NULL);
  for (i = 0; i < num; i++) {
    String item;
    item = XmStringUnparse(selections[i], NULL, 0, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
    if (item) {
      if (list) {
	list = XtRealloc(list, strlen(list) + strlen(item) + 2);
	strcat(list, item);
      } else {
	list = XtMalloc(strlen(item) + 2);
	strcpy(list, item);
      }
      strcat(list, "\n");
      XtFree(item);
    }
  }
  if ((list && (old_list ? strcmp(old_list, list) : 1)) ||
      (!list && (old_list ? strlen(old_list) : 0))) {
    if (list) {
      static struct descriptor list_dsc = { 0, DTYPE_T, CLASS_S, 0 };
      list_dsc.length = strlen(list);
      list_dsc.pointer = list;
      status = TreePutRecord(nid, &list_dsc, 0) & 1;
      if (!status)
	XmdsComplain(XtParent(w), "Error writing interpreter selections");
    } else {
      struct descriptor list_dsc = { 0, DTYPE_T, CLASS_S, 0 };
      status = TreePutRecord(nid, &list_dsc, 0) & 1;
      if (!status)
	XmdsComplain(XtParent(w), "Error writing interpreter selections");
    }
  }
  XtFree(list);
  XtFree(old_list);
  if (status)
    XmdsApplyAllXds(XtParent(w));
  return status & 1;
}

static void Reset(Widget w)
{
  int status;
  Widget list_w = XtNameToWidget(XtParent(w), "*interpreters");
  static struct descriptor_d cli = { 0, DTYPE_T, CLASS_D, 0 };
  static DESCRIPTOR(initial, "_I=0");
  static DESCRIPTOR(clis, "(clis()[_I++]//\"\\0\")");
  XtPointer user_data;
  int nid;
  String selections;
  String selection;
  //int ctx = 0;
  int first = 1;
  XtVaGetValues(list_w, XmNuserData, &user_data, NULL);
  nid = (intptr_t) user_data;
  XmListDeselectAllItems(list_w);
  XmListDeleteAllItems(list_w);
  TdiExecute((struct descriptor *)&initial, &cli MDS_END_ARG);
  while ((status = TdiExecute((struct descriptor *)&clis, &cli MDS_END_ARG) & 1 && cli.length > 0
	  && strlen(cli.pointer) > 0)) {
    XmString item;
    item = XmStringCreateSimple(cli.pointer);
    if (!XmListItemExists(list_w, item))
      XmListAddItem(list_w, item, 0);
    XmStringFree(item);
  }
  selections = XmdsGetNidText(nid);
  for (selection = strtok(selections, "\n"); selection; selection = strtok(0, "\n")) {
    XmString item = XmStringCreateSimple(selection);
    XmListSelectItem(list_w, item, 0);
    if (first) {
      first = 0;
      XmListSetItem(list_w, item);
    }
    XmStringFree(item);
  }
  XtFree(selections);
  return;
}
