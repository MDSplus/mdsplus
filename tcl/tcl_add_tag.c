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
#include <mdsshr.h>
#include <string.h>
#include <stdlib.h>
#include <dcl.h>
#include <treeshr.h>

/**********************************************************************
* TCL_ADD_TAG.C --
*
* TclAddTag:  Add a tag name to a node
*
* History:
*  30-Jan-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/

	/*****************************************************************
	 * TclAddTag:
	 * Add a tag name to a node
	 *****************************************************************/
EXPORT int TclAddTag(void *ctx, char **error, char **output __attribute__ ((unused)))
{				/* Return: status                 */
  int nid;
  int sts;
  char *nodnam = 0;
  char *tagnam = 0;

  cli_get_value(ctx, "NODE", &nodnam);
  cli_get_value(ctx, "TAGNAME", &tagnam);
  sts = TreeFindNode(nodnam, &nid);
  if (sts & 1)
    sts = TreeAddTag(nid, tagnam);
  if (!(sts & 1)) {
    char *msg = MdsGetMsg(sts);
    *error = malloc(strlen(tagnam) + strlen(msg) + 100);
    sprintf(*error, "Error adding tag %s\nError message was: %s\n", tagnam, msg);
  }
  if (nodnam)
    free(nodnam);
  if (tagnam)
    free(tagnam);
  return sts;
}
