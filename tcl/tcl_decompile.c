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
#include 	<mdsplus/mdsconfig.h>
#include 	<stdlib.h>
#ifdef HAVE_ALLOCA_H
#include        <alloca.h>
#endif
#include        <string.h>
#include        <dcl.h>
#include        <mds_stdarg.h>
#include        <mdsshr.h>
#include        <treeshr.h>
#include        <mdsdcl_messages.h>

/***********************************************************************
* TCL_DECOMPILE.C --
*
* Interpret a tdi expression ...
* History:
*  18-Mar-1998  TRG  Create.  Port from original mdsPlus code.
*
************************************************************************/

extern int TdiDecompile();


extern int StrFree1Dx();

	/****************************************************************
	 * TclDecompile:
	 ****************************************************************/
EXPORT int TclDecompile(void *ctx, char **error, char **output)
{
  int nid;
  int sts = MdsdclIVVERB;
  char *path = 0;
  struct descriptor dsc_string = { 0, DTYPE_T, CLASS_D, 0 };
  static struct descriptor_xd xd = { 0, 0, CLASS_XD, 0, 0 };

  cli_get_value(ctx, "PATH", &path);
  if (path != NULL) {
    sts = TreeFindNode(path, &nid);
    free(path);
    if (sts & 1) {
      sts = TreeGetRecord(nid, &xd);
      if (sts & 1) {
	sts = TdiDecompile(&xd, &dsc_string MDS_END_ARG);
	if (sts & 1) {
	  *output = malloc(dsc_string.length + 100);
	  sprintf(*output, "%.*s\n", dsc_string.length, dsc_string.pointer);
	}
	StrFree1Dx(&dsc_string);
      }
    }
    if (~sts & 1) {
      char *msg = MdsGetMsg(sts);
      *error = malloc(strlen(msg) + 100);
      sprintf(*error, "Error: Unable to get data for this node.\n" "Error message was: %s\n", msg);
    }
  }
  return sts;
}
