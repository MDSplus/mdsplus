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

#include "tcl_p.h"

/**********************************************************************
* TCL_SET_DEFAULT.C --
*
* TclSetDefault:  Set default
* TclShowDefault:  Show default
*
* History:
*  15-Jan-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/

	/****************************************************************
	 * TclSetDefault:
	 ****************************************************************/
EXPORT int TclSetDefault(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int nid;
  int sts;
  char *nodename;

  cli_get_value(ctx, "NODE", &nodename);
  sts = TreeSetDefault(nodename, &nid);
  if (sts & 1)
    TclNodeTouched(nid, set_def);
  else {
    char *msg = MdsGetMsg(sts);
    *error = malloc(strlen(msg) + strlen(nodename) + 100);
    sprintf(*error,"Error: Problem setting default to node '%s'\n"
	    "Error message was: %s\n", nodename, msg);
  }
  if (nodename)
    free(nodename);
  return sts;
}

	/***************************************************************
	 * TclShowDefault:
	 ***************************************************************/
EXPORT int TclShowDefault(void *ctx __attribute__ ((unused)),
			  char **error __attribute__ ((unused)), char **output)
{				/* Returns: status                        */
  char *p;
  int nid;
  TreeGetDefaultNid(&nid);
  if ((p = TreeGetPath(nid))) {
    *output = malloc(strlen(p) + 10);
    sprintf(*output, "%s\n", p);
    TreeFree(p);
  }
  return (p ? 1 : 0);
}
