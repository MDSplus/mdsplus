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
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <ncidef.h>
#include <usagedef.h>
#include <dcl.h>
#include <mdsshr.h>
#include <treeshr.h>

#include "tcl_p.h"

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
EXPORT int TclDeleteNode(void *ctx, char **error, char **output)
{
  int count = 0;
  int nids = 0;
  int reset = 1;
  int nid;
  int usageMask = -1;
  char *nodename = 0;
  char *pathnam;
  void *ctx_fn;
  int dryrun = cli_present(ctx, "DRYRUN") & 1;
  int log = cli_present(ctx, "LOG") & 1;
  int confirm = cli_present(ctx, "CONFIRM") & 1;
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
    *error = strdup("Error: The tree must be open for edit before you can deleted nodes.\n");
    return 1;
  }
  if (!count) {
    *error = strdup("Error: No nodes found.\n");
    return 1;
  }
  if ((nids != count) && !(dryrun || confirm)) {
    *error = strdup("Warning: This operation will result in deleting additional descendant\n"
		    "nodes and/or nodes associcated with a device/conglomerate. If you really\n"
		    "intend to delete these nodes, reissue the same command using the /CONFIRM\n"
		    "qualifier.\n");
    return 1;
  }
  *output = strdup("");
  if (log || dryrun) {
    nid = 0;
    while (TreeDeleteNodeGetNid(&nid) & 1) {
      char *msg;
      pathnam = TreeGetPath(nid);
      msg = malloc(strlen(pathnam) + 100);
      sprintf(msg, "%sDeleted node: %s\n", dryrun ? "(Dryrun)" : "", pathnam);
      tclAppend(output, msg);
      free(msg);
      TreeFree(pathnam);
    }
  }
  if (~cli_present(ctx, "DRYRUN") & 1) {
    InitTouchNodes();
    TreeDeleteNodeExecute();
    TouchNodes();
  }
  return 1;
}
