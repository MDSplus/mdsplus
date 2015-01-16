#include        "tclsysdef.h"
#include        <ctype.h>
#include        <ncidef.h>
#include        <usagedef.h>
#include        <string.h>

/**********************************************************************
* TCL_DELETE_NODE.C --
*
* TclDeleteNode:  Delete a node
*
* History:
*  15-Apr-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/

typedef struct _nidlist {
  int nid;
  struct _nidlist *next;
} NidList;

static NidList *nid_list = 0;

	/***************************************************************
	 * InitTouchNodes:
	 ***************************************************************/
static void InitTouchNodes()
{
  int nid = 0;
  while (TreeDeleteNodeGetNid(&nid) & 1) {
    NidList *new = malloc(sizeof(NidList));
    new->nid = nid;
    new->next = nid_list;
    nid_list = new;
  }
}

	/***************************************************************
	 * TouchNodes:
	 ***************************************************************/
static void TouchNodes()
{
  while (nid_list) {
    NidList *first = nid_list;
    TclNodeTouched(first->nid, delete);
    nid_list = first->next;
    free(first);
  }
}

	/*****************************************************************
	 * TclDeleteNode:
	 * Delete a node
	 ****************************************************************/
int TclDeleteNode(void *ctx)
{
  int count = 0;
  int nids = 0;
  int reset = 1;
  int nid;
  int usageMask = -1;
  char *nodename=0;
  char *pathnam;
  void *ctx_fn;
  int status = 1;
  while (cli_get_value(ctx, "NODENAME", &nodename) & 1) {
    ctx_fn = 0;
    while (TreeFindNodeWild(nodename, &nid, &ctx_fn, usageMask) & 1 && (status & 1)) {
      nids++;
      status = TreeDeleteNodeInitialize(nid, &count, reset);
      reset = 0;
    }
    TreeFindNodeEnd(&ctx_fn);
    free(nodename);
  }
  if (status == TreeNOEDIT) {
    TclTextOut("Tree must be open for edit to delete nodes.");
    return 1;
  }
  if (!count) {
    TclTextOut("No nodes found.");
    return 1;
  }
  if ((nids != count) && (cli_present(ctx, "CONFIRM") & 1)) {
    char ans;
    static char message[] = "Additional descendent and/or device\
 nodes will be deleted";
    struct descriptor dsc_answer={0, DTYPE_T, CLASS_D, 0};
    static char prompt[] = "Respond D-delete,L-list,Q-quit [Q]: ";

    TclTextOut(message);
    ans = 0;
    while (dsc_answer.length ? (ans != 'D' && ans != 'Q') : 1) {
      str_free1_dx(&dsc_answer);
      printf("Deal with mdsdcl_get_input\n");
      //      mdsdcl_get_input(prompt, &dsc_answer);
      if (dsc_answer.length == 0)
	str_append(&dsc_answer, "Q");
      ans = toupper(*dsc_answer.pointer);
      if (ans == 'L') {
	nid = 0;
	while (TreeDeleteNodeGetNid(&nid) & 1) {
	  pathnam = TreeGetPath(nid);
	  TclTextOut(pathnam);
	  TreeFree(pathnam);
	}
      }
    }
    if (ans == 'Q')
      return 1;
  }
  if (cli_present(ctx, "LOG") & 1) {
    nid = 0;
    while (TreeDeleteNodeGetNid(&nid) & 1) {
      char *p;
      pathnam = TreeGetPath(nid);
      p=strcpy(malloc(strlen(pathnam)+40), "TCL-I-DELETE, deleted node ");
      strcat(p,pathnam);
      TclTextOut(p);
      free(p);
      TreeFree(pathnam);
    }
  }
  InitTouchNodes();
  TreeDeleteNodeExecute();
  TouchNodes();
  return 1;
}
