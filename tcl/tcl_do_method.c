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

#include <libroutines.h>
#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <usagedef.h>
#include <dcl.h>
#include <treeshr.h>

/**********************************************************************
* TCL_DO_METHOD.C --
*
* TclDoMethod:  Do something.
*
* History:
*  05-Mar-1998  TRG  Create.  Ported from original MDSplus code.
*
************************************************************************/

extern int TdiExecute();
extern int TdiCompile();

	/****************************************************************
	 * TclDoMethod:
	 ****************************************************************/
EXPORT int TclDoMethod(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int i;
  int argc;
  int sts;
  unsigned char do_it;
  struct descriptor_xd xdarg[255];
  int nid;
  unsigned short boolVal;
  struct descriptor_s bool_dsc = { sizeof(boolVal), DTYPE_W, CLASS_S, (char *)&boolVal };
  struct descriptor nid_dsc = { 4, DTYPE_NID, CLASS_S, (char *)&nid };
  struct descriptor_xd empty_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
  char *arg = 0;
  char *if_clause = 0;
  char *method = 0;
  char *object = 0;
  struct descriptor method_dsc = { 0, DTYPE_T, CLASS_S, 0 };
  void *arglist[256] = { (void *)2, &nid_dsc, &method_dsc };

  cli_get_value(ctx, "OBJECT", &object);
  sts = TreeFindNode(object, &nid);
  if (sts & 1) {
    do_it = (TreeIsOn(nid) | cli_present(ctx, "OVERRIDE")) & 1;
    if (cli_present(ctx, "IF") & 1) {
      struct descriptor if_clause_dsc = { 0, DTYPE_T, CLASS_S, 0 };
      cli_get_value(ctx, "IF", &if_clause);
      if_clause_dsc.length = strlen(if_clause);
      if_clause_dsc.pointer = if_clause;
      sts = TdiExecute(&if_clause_dsc, &bool_dsc MDS_END_ARG);
      free(if_clause);
      if (sts & 1)
	do_it = do_it && boolVal;
      else
	do_it = 0;
    }
    if (do_it) {
      int dometh_stat;
      DESCRIPTOR_LONG(dometh_stat_d, 0);
      cli_get_value(ctx, "METHOD", &method);
      method_dsc.length = strlen(method);
      method_dsc.pointer = method;
      argc = 0;
      if (cli_present(ctx, "ARGUMENT") & 1) {
	while (cli_get_value(ctx, "ARGUMENT", &arg) & 1) {
	  struct descriptor arg_dsc = { strlen(arg), DTYPE_T, CLASS_S, arg };
	  xdarg[argc] = empty_xd;
	  sts = TdiCompile(&arg_dsc, &xdarg[argc] MDS_END_ARG);
	  free(arg);
	  if (sts & 1) {
	    arglist[argc + 3] = xdarg[argc].pointer;
	    argc++;
	  } else
	    break;
	}
      }
      if (sts & 1) {
	dometh_stat_d.pointer = (char *)&dometh_stat;
	arglist[argc + 3] = &dometh_stat_d;
	arglist[argc + 4] = MdsEND_ARG;
	arglist[0] = (argc + 4) + (char *)0;
	sts = (char *)LibCallg(arglist, TreeDoMethod) - (char *)0;
	if (sts & 1)
	  sts = dometh_stat;
      }
      if (method)
	free(method);
      for (i = 0; i < argc; i++)
	MdsFree1Dx(&xdarg[i], NULL);
    }
  }
  if (object)
    free(object);
  if (!(sts & 1)) {
    char *msg = MdsGetMsg(sts);
    *error = malloc(strlen(msg) + 100);
    sprintf(*error, "Error executing device method\n" "Error message was: %s\n", msg);
  }
  return sts;
}
