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
#include <string.h>
#include <stdlib.h>

#include <dcl.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <mdsdcl_messages.h>

#include "tcl_p.h"

/***********************************************************************
* TCL_RENAME.C --
*
* Rename a node or subtree.
* History:
*  20-Mar-1998  TRG  Create.  Port from original mdsPlus code.
*
************************************************************************/

	/***************************************************************
	 * TclRename:
	 * Rename a node or subtree.
	 ***************************************************************/
EXPORT int TclRename(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int nid;
  int sts = MdsdclIVVERB;
  char *srcnam = 0;
  char *destnam = 0;

  cli_get_value(ctx, "SOURCE", &srcnam);
  if (srcnam && (strlen(srcnam) > 0)) {
    cli_get_value(ctx, "DEST", &destnam);
    if (destnam && (strlen(destnam) > 0)) {
      sts = TreeFindNode(srcnam, &nid);
      if (sts & 1) {
	sts = TreeRenameNode(nid, destnam);
	if (sts & 1)
	  TclNodeTouched(nid, rename_node);
	else {
	  char *msg = MdsGetMsg(sts);
	  *error = malloc(strlen(srcnam) + strlen(destnam) + strlen(msg) + 100);
	  sprintf(*error, "Error: Problem renaming node %s to %s\n"
		  "Error message was: %s\n", srcnam, destnam, msg);
	}
      } else {
	*error = malloc(strlen(srcnam) + 100);
	sprintf(*error, "Error: Cannot find node %s\n", srcnam);
      }
    } else {
      *error = strdup("Error: destination node parameter missing\n");
    }
  } else {
    *error = strdup("Error: source node parameter missing\n");
  }
  if (srcnam)
    free(srcnam);
  if (destnam)
    free(destnam);

  return sts;
}
