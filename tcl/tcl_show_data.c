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
#include <alloca.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <dcl.h>
#include <mds_stdarg.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <usagedef.h>
#include <tdishr.h>

#include "tcl_p.h"


/**********************************************************************
* TCL_SHOW_DATA.C --
*
* TclShowData:  Show data in mdsPlus node.
*
* History:
*  20-Feb-1998  TRG  Create.  Ported from original mdsPlus code.
*
************************************************************************/

		/*=======================================================
		 * Function prototypes:
		 *======================================================*/
static int CvtDxT(struct descriptor *in_dsc_ptr, int depth, char **error, char **output);


	/****************************************************************
	 * lib_cvt_dx_dx:
	 * Convert data for display ...
	 ****************************************************************/

	/****************************************************************
	 * TclDtypeString:
	 * Simply a re-formatted version of MdsDtypeString ...
	 ****************************************************************/
static char *TclDtypeString(	/* Returns:  address of formatted string */
			     char dtype	/* <r> data type code                   */
    )
{
  char *string = malloc(100);
  if (string) sprintf(string, "%-30s: ", MdsDtypeString(dtype));
  return string;
}

	/****************************************************************
	 * CvtIdentT:
	 ****************************************************************/
static int CvtIdentT(struct descriptor *in_dsc_ptr, int depth, char **output)
{
  char *ident = MdsDescrToCstring(in_dsc_ptr);
  char *dtype = TclDtypeString(in_dsc_ptr->dtype);
  char *out_str = alloca(strlen(ident) + strlen(dtype) + depth + 10);
  sprintf(out_str, "%*s%s\n", (int)(depth + strlen(dtype)), dtype, ident);
  tclAppend(output, out_str);
  free(dtype);
  free(ident);
  return 1;
}

	/****************************************************************
	 * CvtNidT:
	 ****************************************************************/
static int CvtNidT(struct descriptor *in_dsc_ptr, int depth, char **error, char **output)
{
  int nid = *(int *)in_dsc_ptr->pointer;
  int sts = 0;
  char *dstr = TclDtypeString(in_dsc_ptr->dtype);

  if (nid == 0) {
    char *out_str = alloca(strlen(dstr) + strlen("$VALUE") + depth + 10);
    sprintf(out_str, "%*s$VALUE\n", (int)(strlen(dstr) + depth), dstr);
    tclAppend(output, out_str);
    sts = 1;
  } else {
    char *pathname = 0;
    if ((pathname = TreeGetPath(nid))) {
      struct descriptor_xd lxd = { 0, 0, CLASS_XD, 0, 0 };
      char *out_str = alloca(strlen(dstr) + strlen(pathname) + depth + 10);
      sprintf(out_str, "%*s%s\n", (int)(strlen(dstr) + depth), dstr, pathname);
      tclAppend(output, out_str);
      TreeFree(pathname);
      sts = TreeGetRecord(nid, &lxd);
      if (sts & 1) {
	sts = CvtDxT((struct descriptor *)(&lxd), depth + 4, error, output);
	MdsFree1Dx(&lxd, NULL);
      } else
	sts = 1;
    } else {
      char *out_str = alloca(strlen("***** Bad Nid Reference ********") + depth + strlen(dstr) + 10);
      sprintf(out_str, "%*s***** Bad Nid Reference ********\n", (int)(strlen(dstr) + depth), dstr);
      tclAppend(output, out_str);
      sts = 1;
    }
  }
  free(dstr);
  return sts;
}

	/**************************************************************
	 * CvtNumericT:
	 **************************************************************/
static int CvtNumericT(struct descriptor *in_dsc_ptr, int depth, char **output)
{
  int sts;
  char *dstr = TclDtypeString(in_dsc_ptr->dtype);
#define STATIC_STRING_LEN 132
  char str_chars[STATIC_STRING_LEN + 1];
  struct descriptor str = { STATIC_STRING_LEN, DTYPE_T, CLASS_S, str_chars };
  str_chars[STATIC_STRING_LEN] = 0;
  sts = TdiDecompile(in_dsc_ptr, &str MDS_END_ARG);
  if (sts & 1) {
    char *out_str = alloca(strlen(dstr) + STATIC_STRING_LEN + depth + 10);
    int i;
    for (i = STATIC_STRING_LEN; i; i--) {
      if (str_chars[i - 1] == ' ')
	str_chars[i] = 0;
      else
	break;
    }
    sprintf(out_str, "%*s%s\n", (int)(strlen(dstr) + depth), dstr, str_chars);
    tclAppend(output, out_str);
  } else {
    char *out_str =
	alloca(strlen(dstr) + strlen("There is no primative display for this datatype") + depth +
	       10);
    sprintf(out_str, "%*s%s\n", (int)(strlen(dstr) + depth), dstr,
	    "There is no primative display for this datatype");
    tclAppend(output, out_str);
  }
  free(dstr);
  return 1;
}

	/***************************************************************
	 * CvtPathT:
	 ***************************************************************/
static int CvtPathT(struct descriptor *in_dsc_ptr, int depth, char **error, char **output)
{
  int sts;
  int nid;
  char *dstr = TclDtypeString(in_dsc_ptr->dtype);
  struct descriptor_xd xd = { 0, 0, CLASS_XD, 0, 0 };
  char *ident = MdsDescrToCstring(in_dsc_ptr);
  char *out_str = alloca(strlen(dstr) + strlen(ident) + depth + 10);
  sprintf(out_str, "%*s%s\n", (int)(strlen(dstr) + depth), dstr, ident);
  tclAppend(output, out_str);
  sts = TreeFindNode(ident, &nid);
  free(ident);
  if (sts & 1) {
    sts = TreeGetRecord(nid, &xd);
    if (sts & 1) {
      sts = CvtDxT((struct descriptor *)&xd, depth + 4, error, output);
      MdsFree1Dx(&xd, NULL);
    } else {
      char *out_str = alloca(strlen("Error reading data record") + depth + 10);
      sprintf(out_str, "%*s\n", (int)(strlen("Error reading data record") + depth + 4),
	      "Error reading data record");
      tclAppend(output, out_str);
    }
  } else {
    char *out_str = alloca(strlen("Record not found") + depth + 10);
    sprintf(out_str, "%*s\n", (int)(strlen("Record not found") + depth + 4), "Record not found");
    tclAppend(output, out_str);
  }
  free(dstr);
  return 1;
}

	/*************************************************************
	 * CvtDdscT:
	 *************************************************************/
static int CvtDdscT(struct descriptor *in_dsc_ptr, int depth, char **error, char **output)
{
  int sts;
  char *dstr = TclDtypeString(in_dsc_ptr->dtype);
  char *out_str;
  switch (in_dsc_ptr->dtype) {

  case DTYPE_DSC:
    sts = CvtDxT((struct descriptor *)in_dsc_ptr->pointer, depth, error, output);
    break;

  case DTYPE_T:
    out_str = alloca(strlen(dstr) + in_dsc_ptr->length + depth + 10);
    sprintf(out_str, "%*s%.*s\n", (int)(strlen(dstr) + depth), dstr, in_dsc_ptr->length,
	    in_dsc_ptr->pointer);
    tclAppend(output, out_str);
    sts = 1;
    break;

  case DTYPE_NID:
    sts = CvtNidT(in_dsc_ptr, depth, error, output);
    break;

  case DTYPE_PATH:
    sts = CvtPathT(in_dsc_ptr, depth, error, output);
    break;

  case DTYPE_IDENT:
    sts = CvtIdentT(in_dsc_ptr, depth, output);
    break;

  default:
    sts = CvtNumericT(in_dsc_ptr, depth, output);
    break;
  }
  free(dstr);
  return sts;
}

	/***************************************************************
	 * CvtGenericRT:
	 ***************************************************************/
static int CvtGenericRT(struct descriptor_r *in_dsc_ptr, int depth, char **error, char **output)
{

  int i;
  int sts = 1;

  for (i = 0; sts && i < in_dsc_ptr->ndesc; i++) {
    if (in_dsc_ptr->dscptrs[i]) {
      sts = CvtDxT(in_dsc_ptr->dscptrs[i], depth + 4, error, output);
    } else {
      char *out_str = alloca(strlen("*** EMPTY ****") + depth + 10);
      sprintf(out_str, "%*s\n", (int)(strlen("*** EMPTY ****") + depth + 4), "*** EMPTY ****");
      tclAppend(output, out_str);
      sts = 1;
    }
  }
  return sts;
}

	/***************************************************************
	 * CvtFunctionT:
	 ***************************************************************/
static int CvtFunctionT(struct descriptor *in_dsc_ptr, int depth, char **error, char **output)
{
  int sts;
  char *dstr = TclDtypeString(in_dsc_ptr->dtype);
  static struct descriptor_xd ostr = { 0, DTYPE_T, CLASS_XD, 0, 0 };
  struct descriptor opcode_dsc = { 2, DTYPE_WU, CLASS_S, (char *)0 };

  opcode_dsc.pointer = (char *)in_dsc_ptr->pointer;
  sts = TdiOpcodeString(&opcode_dsc, &ostr MDS_END_ARG);
  if (sts & 1) {
    char *out_str = alloca(strlen(dstr) + depth + ostr.pointer->length + 10);
    sprintf(out_str, "%*s%.*s\n", (int)(strlen(dstr) + depth), dstr, ostr.pointer->length,
	    ostr.pointer->pointer);
    tclAppend(output, out_str);
    sts = CvtGenericRT((struct descriptor_r *)in_dsc_ptr, depth, error, output);
  }
  MdsFree1Dx(&ostr, NULL);
  free(dstr);
  return sts;
}

	/***************************************************************
	 * CvtRdscT:
	 ***************************************************************/
static int CvtRdscT(struct descriptor *in_dsc_ptr, int depth, char **error, char **output)
{
  int sts;
  char *dstr = TclDtypeString(in_dsc_ptr->dtype);
  if (in_dsc_ptr->dtype == DTYPE_FUNCTION)
    sts = CvtFunctionT(in_dsc_ptr, depth, error, output);
  else {
    char *out_str = alloca(strlen(dstr) + depth + 10);
    sprintf(out_str, "%*s\n", (int)(strlen(dstr) + depth), dstr);
    tclAppend(output, out_str);
    sts = CvtGenericRT((struct descriptor_r *)in_dsc_ptr, depth, error, output);
  }
  free(dstr);
  return sts;
}

	/****************************************************************
	 * CvtAdscT:
	 ****************************************************************/
static int CvtAdscT(struct descriptor_a *in_dsc_ptr, int depth, char **output)
{
  char *dstr = TclDtypeString(in_dsc_ptr->dtype);
  int *bptr;
  int *lbptr;
  int *ubptr;
  int dim;
#define BOUNDS_LENGTH 32	/* formerly 16  */
  char bchars[BOUNDS_LENGTH];
  char *out_str = malloc(strlen(dstr) + depth + 20);
  sprintf(out_str, "%*s Array [ ", (int)(strlen(dstr) + depth), dstr);
  if (in_dsc_ptr->aflags.bounds) {
    bptr = (int *)((char *)in_dsc_ptr + sizeof(struct descriptor_a) +
		   sizeof(void *) + (in_dsc_ptr->dimct) * sizeof(int));
    for (dim = 0; dim < in_dsc_ptr->dimct; dim++) {
      lbptr = bptr++;
      ubptr = bptr++;
      sprintf(bchars, "%d:%d%s", *lbptr, *ubptr, (dim < in_dsc_ptr->dimct - 1) ? "," : "");
      out_str = realloc(out_str, strlen(out_str) + strlen(bchars) + 10);
      strcat(out_str, bchars);
    }
  } else if (in_dsc_ptr->aflags.coeff) {
    bptr = (int *)((char *)in_dsc_ptr + sizeof(struct descriptor_a) + sizeof(void *));
    for (dim = 0; dim < in_dsc_ptr->dimct; dim++) {
      sprintf(bchars, "%d%s", *bptr++, (dim < in_dsc_ptr->dimct - 1) ? "," : "");
      out_str = realloc(out_str, strlen(out_str) + strlen(bchars) + 10);
      strcat(out_str, bchars);
    }
  } else {
    sprintf(bchars, "%d", in_dsc_ptr->arsize / ((in_dsc_ptr->length)?in_dsc_ptr->length:1));
    out_str = realloc(out_str, strlen(out_str) + strlen(bchars) + 10);
    strcat(out_str, bchars);
  }
  strcat(out_str, " ]\n");
  tclAppend(output, out_str);
  if (out_str)
    free(out_str);
  free(dstr);
  return 1;
}

	/****************************************************************
	 * CvtDxT:
	 ****************************************************************/
static int CvtDxT(struct descriptor *in_dsc_ptr, int depth, char **error, char **output)
{
  int sts=0;
  switch (in_dsc_ptr->class) {
  case CLASS_XD:
  case CLASS_XS:
    {
      if (in_dsc_ptr->dtype == DTYPE_DSC)
	sts = CvtDxT((struct descriptor *)in_dsc_ptr->pointer, depth, error, output);
      else {
	const char *errormsg = "Invalid Dtype for XD or XS must be DSC\n";
	char *out_str = alloca(strlen(errormsg) + depth + 10);
	sprintf(out_str, "%*s\n", (int)(strlen(errormsg) + depth), errormsg);
	tclAppend(output, out_str);
      }
    }
    break;

  case CLASS_D:
  case CLASS_S:
    sts = CvtDdscT(in_dsc_ptr, depth, error, output);
    break;

  case CLASS_R:
    sts = CvtRdscT(in_dsc_ptr, depth, error, output);
    break;

  case CLASS_A:
  case CLASS_CA:
  case CLASS_APD:
    sts = CvtAdscT((struct descriptor_a *)in_dsc_ptr, depth, output);
    break;
  }
  return sts;
}

	/****************************************************************
	 * TclShowData:
	 ****************************************************************/
EXPORT int TclShowData(void *ctx, char **error, char **output)
{
  int nid;
  int sts;
  int usageMask;
  char *pathnam;
  char *nodnam = 0;
  void *ctx1 = 0;
  struct descriptor_xd data = { 0, 0, CLASS_XD, 0, 0 };

  usageMask = -1;
  while (cli_get_value(ctx, "NODE", &nodnam) & 1) {
    while (TreeFindNodeWild(nodnam, &nid, &ctx1, usageMask) & 1) {
      char *line;
      if (*output == NULL)
	*output = strdup("");
      pathnam = TreeGetPath(nid);
      line = malloc(strlen(pathnam) + 10);
      sprintf(line, "%s\n", pathnam);
      tclAppend(output, line);
      free(line);
      TreeFree(pathnam);
      sts = TreeGetRecord(nid, &data);
      if (sts & 1) {
	sts = CvtDxT((struct descriptor *)(&data), 1, error, output);
	MdsFree1Dx(&data, 0);
	if ((sts & 1) == 0)
	  tclAppend(output, "   Error displaying data\n");
      } else
	tclAppend(output, "   No data found\n");
    }
    TreeFindNodeEnd(&ctx1);
  }
  if (nodnam)
    free(nodnam);
  return 1;
}
