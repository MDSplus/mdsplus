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

		Name:   ACTIONS   

		Type:   C function

     		Author:	TOM FREDIAN

		Date:   25-FEB-1993

    		Purpose: Action Modification program 

------------------------------------------------------------------------------

	Call sequence: 

MCR ACTIONS -TREE treename

------------------------------------------------------------------------------
   Copyright (c) 1993
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
------------------------------------------------------------------------------*/

#include <mdsdescrip.h>
#include <Xmds/XmdsXdBox.h>
#include <stdio.h>
#include <usagedef.h>
#include <ncidef.h>
#include <treeshr.h>
#include <Mrm/MrmPublic.h>
#include <Xm/List.h>
#include <xmdsshr.h>
#include <string.h>
#include <stdlib.h>
#include <mds_stdarg.h>
#include <strroutines.h>

extern int TdiDispatchOf();
extern int TdiExecute();
extern int TdiGetLong();
extern int TdiData();

static void Modify();
static void ToggleEssential();
static void Exit();
static int OpenTree(String tree, int shot);
static int Refresh();
static Widget mw;
typedef struct _info {
  unsigned char on;
  unsigned char included;
  unsigned int phase;
  unsigned int idx;
  char server[32];
  char path[128];
  int nid;
  int item_idx;
} Info;
static int CompareActions(Info * a, Info * b);
static Info *actions;
static int num_actions;

int main(int argc, String * argv)
{
  static struct {
    String tree;
    int shot;
  } db;
  static String hierarchy_names[] = { "actions.uid" };
  static MrmRegisterArg register_list[] = { {"Modify", (char *)Modify},
  {"Refresh", (char *)Refresh},
  {"Exit", (char *)Exit},
  {"ToggleEssential", (char *)ToggleEssential}
  };
  MrmType class;
  int status;
  static XrmOptionDescRec options[] = { {"-tree", "*tree", XrmoptionSepArg, NULL} };
  static XtResource resources[] = { {"tree", "Tree", XtRString, sizeof(String), 0, XtRString, "CMOD"}
  ,
  {"shot", "Shot", XtRInt, sizeof(int), sizeof(String), XtRImmediate, (void *)-1}
  };
  Widget top;
  XtAppContext app_context;
  MrmHierarchy drm_hierarchy;
  MrmInitialize();
  XmdsInitialize();
  MrmRegisterNames(register_list, XtNumber(register_list));
  top =
      XtVaAppInitialize(&app_context, "Actions", options, XtNumber(options), &argc, argv, NULL,
			NULL);
  XtGetApplicationResources(top, &db, resources, XtNumber(resources), (Arg *) NULL, 0);
  MrmOpenHierarchy(XtNumber(hierarchy_names), hierarchy_names, 0, &drm_hierarchy);
  MrmFetchWidget(drm_hierarchy, "actions", top, &mw, &class);
  MrmCloseHierarchy(drm_hierarchy);
  XtManageChild(mw);
  XtRealizeWidget(top);
  if ((status = OpenTree(db.tree, db.shot)) & 1)
    if ((status = Refresh()) & 1)
      XtAppMainLoop(app_context);
  return status;
}

static void Modify(Widget w, XtPointer tag __attribute__ ((unused)), XmListCallbackStruct * cb)
{
  int i;
  Widget xdbox = XtNameToWidget(XtParent(XtParent(w)), "*modify_xdbox");
  for (i = 0; i < num_actions && actions[i].item_idx != cb->item_position; i++) ;
  if (i < num_actions) {
    XtManageChild(xdbox);
    XtVaSetValues(xdbox, XmdsNnid, actions[i].nid, NULL);
  }
}

static void ToggleEssential(Widget w, XtPointer tag __attribute__ ((unused)), XmListCallbackStruct * cb)
{
  int i;
  for (i = 0; i < num_actions && actions[i].item_idx != cb->item_position; i++) ;
  if (i < num_actions) {
    XmString item;
    char *item_string;
    static int flags;
    static NCI_ITM itmlst[] =
	{ {sizeof(flags), NciGET_FLAGS, (unsigned char *)&flags, 0}, {0, NciEND_OF_LIST, 0, 0} };
    TreeGetNci(actions[i].nid, itmlst);
    item_string = XmStringUnparse(cb->item, NULL, 0, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
    if (flags & NciM_ESSENTIAL) {
      static int flags = NciM_ESSENTIAL;
      static NCI_ITM itmlst[] =
	  { {sizeof(flags), NciCLEAR_FLAGS, (unsigned char *)&flags, 0}, {0, NciEND_OF_LIST, 0,
									  0} };
      TreeSetNci(actions[i].nid, itmlst);
      if (strlen(item_string) > 45)
	item_string[45] = ' ';
    } else {
      static int flags = NciM_ESSENTIAL;
      static NCI_ITM itmlst[] =
	  { {sizeof(flags), NciSET_FLAGS, (unsigned char *)&flags, 0}, {0, NciEND_OF_LIST, 0, 0} };
      TreeSetNci(actions[i].nid, itmlst);
      if (strlen(item_string) > 45)
	item_string[45] = '*';
    }
    item = XmStringCreateSimple(item_string);
    XmListReplaceItems(w, &cb->item, 1, &item);
    XmStringFree(item);
    XtFree(item_string);
  }
}

static void Exit()
{
  exit(0);
}

static int OpenTree(String tree, int shot)
{
  return TreeOpen(tree, shot, 0);
}

static int Refresh()
{
  void *ctx = 0;
  int nid;
  int i;
  Widget w = XtNameToWidget(mw, "*action_list");
  int mask = 1 << TreeUSAGE_ACTION;
  int status = 1;
  unsigned char last_phase = 255;
  unsigned char last_on = 1;
  unsigned char last_included = 1;
  int item_idx;
  if (actions)
    XtFree((char *)actions);
  XmListDeselectAllItems(w);
  XmListDeleteAllItems(w);
  num_actions = 0;
  while (TreeFindNodeWild("***", &nid, &ctx, mask) & 1)
    num_actions++;
  TreeFindNodeEnd(&ctx);
  if (num_actions) {
    actions = (Info *) XtMalloc(sizeof(Info) * num_actions);
    memset(actions, 0, sizeof(Info) * num_actions);
    for (i = 0; (TreeFindNodeWild("***", &actions[i].nid, &ctx, mask) & 1) && i < num_actions; i++) {
      static struct descriptor ident = { 31, DTYPE_T, CLASS_S, 0 };
      static struct descriptor niddsc = { 4, DTYPE_NID, CLASS_S, 0 };
      static EMPTYXD(xd);
      static DESCRIPTOR(blank, " ");
      int parent = actions[i].nid & 0xFF000000;
      static int flags;
      static int p_flags;
      static NCI_ITM itmlst[] = { {126, NciFULLPATH, 0, 0},
      {sizeof(flags), NciGET_FLAGS, &flags, 0}
      ,
      {0, 0, 0, 0}
      };
      static NCI_ITM p_itmlst[] = { {4, NciGET_FLAGS, &p_flags, 0}, {0, 0, 0, 0} };
      itmlst[0].pointer = actions[i].path + 1;
      ident.pointer = actions[i].server;
      niddsc.pointer = (char *)&actions[i].nid;
      actions[i].on = TreeIsOn(actions[i].nid) & 1;
      TreeGetNci(actions[i].nid, itmlst);
      actions[i].path[0] = flags & NciM_ESSENTIAL ? '*' : ' ';
      if (parent)
	TreeGetNci(parent, p_itmlst);
      actions[i].included = parent ? (p_flags & NciM_INCLUDE_IN_PULSE) != 0 : 1;
      StrCopyDx(&ident, (struct descriptor *)&blank);
      actions[i].idx = 0;
      if (TdiDispatchOf(&niddsc, &xd MDS_END_ARG) & 1) {
	struct descriptor_dispatch *dispatch = (struct descriptor_dispatch *)xd.pointer;
	if (dispatch->pointer && (dispatch->pointer[0] == TreeSCHED_SEQ)) {
	  static struct descriptor phase_d = { sizeof(int), DTYPE_L, CLASS_S, 0 };
	  static DESCRIPTOR(phase_lookup, "PHASE_NUMBER_LOOKUP($)");
	  phase_d.pointer = (char *)&actions[i].phase;
	  if (!(TdiExecute(&phase_lookup, dispatch->phase, &phase_d MDS_END_ARG) & 1))
	    actions[i].phase = 255;
	  if (!(TdiGetLong(dispatch->when, &actions[i].idx) & 1))
	    actions[i].phase = 254;
	  if (!(TdiData(dispatch->ident, &ident MDS_END_ARG) & 1))
	    actions[i].phase = 253;
	} else
	  actions[i].phase = 252;
      } else
	actions[i].phase = 251;
    }
    TreeFindNodeEnd(&ctx);
    qsort(actions, num_actions, sizeof(Info), (int (*)(const void *, const void *))CompareActions);
    for (item_idx = 1, i = 0; i < num_actions; i++) {
      char line[256];
      XmString item;
      if (last_included != actions[i].included) {
	item =
	    XmStringCreateSimple
	    ("********************** The following nodes are not included in pulse *********************");
	XmListAddItem(w, item, 0);
	XmStringFree(item);
	item_idx++;
	last_included = actions[i].included;
	last_on = 1;
      }
      if (last_on != actions[i].on) {
	item =
	    XmStringCreateSimple
	    ("****************************** The following nodes are turned off *********************");
	XmListAddItem(w, item, 0);
	XmStringFree(item);
	item_idx++;
	last_on = actions[i].on;
      }
      if (last_phase != actions[i].phase) {
	static String line_s =
	    "************************** Phase: 12345678901234567890123456789012*********************";
	static char line[132];
	struct descriptor phase = { 32, DTYPE_T, CLASS_S, &line[34] };
	struct descriptor error = { 40, DTYPE_T, CLASS_S, &line[27] };
	struct descriptor prefix = { 7, DTYPE_T, CLASS_S, &line[27] };
	static DESCRIPTOR(const not_sequential, "Nodes containing non-sequential actions");
	static DESCRIPTOR(const not_an_action, "Nodes that do not contain valid actions");
	static DESCRIPTOR(const bad_server, "Nodes containing bad server information");
	static DESCRIPTOR(const bad_index, "Nodes containing bad index value");
	static DESCRIPTOR(const bad_phase, "Nodes containing unknown phase");
	static DESCRIPTOR(const phase_str, "Phase:");
	static DESCRIPTOR(const phase_lookup, "PHASE_NAME_LOOKUP($)");
	static struct descriptor phase_d = { sizeof(int), DTYPE_L, CLASS_S, 0 };
	phase_d.pointer = (char *)&actions[i].phase;
	strcpy(line, line_s);
	switch (actions[i].phase) {
	case 0x10000001:
	  StrCopyDx(&error, (struct descriptor *)&bad_phase);
	  break;
	case 0x10000002:
	  StrCopyDx(&error, (struct descriptor *)&bad_index);
	  break;
	case 0x10000003:
	  StrCopyDx(&error, (struct descriptor *)&bad_server);
	  break;
	case 0x10000004:
	  StrCopyDx(&error, (struct descriptor *)&not_sequential);
	  break;
	case 0x10000005:
	  StrCopyDx(&error, (struct descriptor *)&not_an_action);
	  break;
	default:
	  if (TdiExecute(&phase_lookup, &phase_d, &phase MDS_END_ARG) & 1)
	    StrCopyDx(&prefix, (struct descriptor *)&phase_str);
	  else
	    StrCopyDx(&error, (struct descriptor *)&bad_phase);
	  break;
	}
	item = XmStringCreateSimple(line);
	XmListAddItem(w, item, 0);
	XmStringFree(item);
	item_idx++;
	last_phase = actions[i].phase;
      }
      sprintf(line, "%12d %s %s", actions[i].idx, actions[i].server, actions[i].path);
      item = XmStringCreateSimple(line);
      XmListAddItem(w, item, 0);
      XmStringFree(item);
      actions[i].item_idx = item_idx++;
    }
  } else
    status = TreeNNF;
  return status;
}

static int CompareActions(Info * a, Info * b)
{
  return (a->included == b->included) ? ((a->on == b->on) ? ((a->phase == b->phase) ? (int)a->idx - (int)b->idx
							     : (int)a->phase - (int)b->phase)
					 : (int)b->on - (int)a->on)
    : (int)b->included - (int)a->included;
}
