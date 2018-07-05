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
#include <mdsplus/mdsconfig.h>
#ifdef HAVE_ALLOCA_H
#include 	<alloca.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <dcl.h>
#include <mds_stdarg.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <strroutines.h>

#include "tcl_p.h"


extern int TdiDecompile();
extern int TdiSortVal();

/**********************************************************************
* TCL_SHOW_ATTRIBUTE.C --
*
* TclShowAttribute:  Display node attributes.
*
*
************************************************************************/

	/***************************************************************
	 * TclShowAttribute:
	 ***************************************************************/
EXPORT int TclShowAttribute(void *ctx, char **error, char **output)
{
  int status;
  int nid;
  EMPTYXD(xd);
  char *node = 0;
  char *attr = 0;
  struct descriptor_d dsc_string = { 0, DTYPE_T, CLASS_D, 0 };
  cli_get_value(ctx, "NODE", &node);
  status = TreeFindNode(node, &nid);
  if (status & 1) {
    status = cli_get_value(ctx, "NAME", &attr);
    if (status & 1) {
      status = TreeGetXNci(nid, attr, &xd);
      if (status & 1) {
	status = TdiDecompile(&xd, &dsc_string MDS_END_ARG);
	if (status & 1) {
	  *output = strncpy(malloc(dsc_string.length + 100), dsc_string.pointer, dsc_string.length);
	  (*output)[dsc_string.length] = '\n';
	  (*output)[dsc_string.length + 1] = '\0';
	}
	StrFree1Dx(&dsc_string);
	MdsFree1Dx(&xd, 0);
      }
    } else {
      if (TreeGetXNci(nid, "attributenames", &xd) & 1) {
	TdiSortVal(&xd, &xd MDS_END_ARG);
	if (xd.pointer && xd.pointer->class == CLASS_A) {
	  typedef ARRAY(char) ARRAY_DSC;
	  ARRAY_DSC *array = (ARRAY_DSC *) xd.pointer;
	  char *name = array->pointer;
	  *output = strdup("Defined attributes for this node:\n");
	  for (name = array->pointer; name < array->pointer + array->arsize; name += array->length) {
	    char *out = alloca(array->length + 100);
	    sprintf(out, "    %.*s\n", array->length, name);
	    tclAppend(output, out);
	  }
	} else {
	  *output = strdup("No attributes defined for this node\n");
	}
	MdsFree1Dx(&xd, 0);
      } else {
	*output = strdup("No attributes defined for this node\n");
      }
      status = 1;
    }
  }
  if (!(status & 1)) {
    char *msg = MdsGetMsg(status);
    *error = malloc(strlen(msg)+strlen(node)+100);
    sprintf(*error,"Error: Unable to get attributes for node '%s'\n"
	    "Error message was: %s\n",node, msg);
  }
  if (node)
    free(node);
  if (attr)
    free(attr);
  return status;
}

extern int TdiCompile();

EXPORT int TclSetAttribute(void *ctx, char **error, char **output __attribute__ ((unused)), char *(*getline)(), void *getlineinfo)
{
  char *nodnam = 0;
  char *attname = 0;
  char *ascValue = 0;
  struct descriptor dsc_ascValue = { 0, DTYPE_T, CLASS_S, 0 };
  struct descriptor_xd value_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
  int sts;
  int nid;

  cli_get_value(ctx, "NODE", &nodnam);
  cli_get_value(ctx, "NAME", &attname);
  sts = TreeFindNode(nodnam, &nid);
  if (sts & 1) {
    if (cli_present(ctx, "EXTENDED") & 1) {
      int use_lf = cli_present(ctx, "LF") & 1;
      char *line=0;
      while ((line = (getline ? getline(getlineinfo) : readline("ATT> ")))
	     && (strlen(line)) > 0) {
	if (getline == NULL)
	  add_history(line);
	if (strlen(line)==0) {
	  free(line);
	  line=0;
	  break;
	}
	tclAppend(&ascValue, line);
	free(line);
	line=0;
	if (use_lf)
	  tclAppend(&ascValue, "\n");
      }
      if (line)
	free(line);
    } else
      cli_get_value(ctx, "VALUE", &ascValue);
    dsc_ascValue.length = strlen(ascValue);
    dsc_ascValue.pointer = ascValue;
    sts = TdiCompile(&dsc_ascValue, &value_xd MDS_END_ARG);
    if (sts & 1) {
      if (!value_xd.l_length)
	value_xd.dtype = DTYPE_DSC;
      sts = TreeSetXNci(nid, attname, (struct descriptor *)&value_xd);
    }
  }
  MdsFree1Dx(&value_xd, NULL);
  if (~sts & 1) {
    char *msg = MdsGetMsg(sts);
    *error = malloc(strlen(msg)+strlen(nodnam)+100);
    sprintf(*error,"Error: Unable to set attribute on node '%s'\n"
	    "Error message was: %s\n", nodnam, msg);
  }
  if (nodnam)
    free(nodnam);
  if (attname)
    free(attname);
  if (ascValue)
    free(ascValue);
  return sts;
}
