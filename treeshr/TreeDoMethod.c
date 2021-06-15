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

                Name:   TREE$DO_METHOD

                Type:   C function

                Author:	Josh Stillerman

                Date:   27-APR-1989

                Purpose: Do method on nid

------------------------------------------------------------------------------

        Call sequence:

int TreeDoMethod( nid_dsc, method_dsc [,args]...)

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

        Description:

------------------------------------------------------------------------------*/
#include "treeshrp.h"
#include <ctype.h>
#include <libroutines.h>
#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <mdsshr_messages.h>
#include <ncidef.h>
#include <string.h>
#include <strroutines.h>
#include <tdishr_messages.h>
#include <treeshr.h>

extern void **TreeCtx();

static int (*_TdiIntrinsic)() = NULL;

inline static int strdcmp(const struct descriptor *strd, const char *cmp)
{
  return strd && strd->length == strlen(cmp) &&
         strncmp(strd->pointer, cmp, strlen(cmp)) == 0;
}

static inline int _LibCallg(void **ctx, void **arglist, void *addr)
{
  int status;
  CTX_PUSH(ctx);
  status = (int)(intptr_t)LibCallg(arglist, addr);
  CTX_POP(ctx);
  return status;
}

int TreeDoMethodA(mdsdsc_t *nid_dsc, mdsdsc_t *method_ptr, int nargs_in,
                  mdsdsc_t **arglist_in, mdsdsc_xd_t *out_ptr)
{
  return _TreeDoMethodA(*TreeCtx(), nid_dsc, method_ptr, nargs_in, arglist_in,
                        out_ptr);
}
int _TreeDoMethodA(void *dbid, mdsdsc_t *nid_dsc, mdsdsc_t *method_ptr,
                   int nargs_in, mdsdsc_t **arglist_in, mdsdsc_xd_t *out_ptr)
{
  if (nargs_in > 250)
    return TdiEXTRA_ARG;
  INIT_STATUS;
  short conglomerate_elt;
  unsigned char data_type;
  int head_nid;
  NCI_ITM itmlst[] = {
      {1, NciDTYPE, &data_type, 0},
      {2, NciCONGLOMERATE_ELT, (unsigned char *)&conglomerate_elt, 0},
      {4, NciCONGLOMERATE_NIDS, (unsigned char *)&head_nid, 0},
      {0, NciEND_OF_LIST, 0, 0}};
  if (!nid_dsc->pointer ||
      (nid_dsc->dtype != DTYPE_NID && nid_dsc->dtype != DTYPE_L &&
       nid_dsc->dtype != DTYPE_LU))
    return TreeNOMETHOD;
  head_nid = 0;
  status = _TreeGetNci(dbid, *(int *)nid_dsc->pointer, itmlst);
  if (STATUS_NOT_OK)
    return status;
  if (!conglomerate_elt && (data_type != DTYPE_CONGLOM))
    return TreeNOMETHOD;
  struct descriptor_conglom *conglom_ptr;
  struct descriptor_xd xd = {0, 0, CLASS_XD, 0, 0};
  struct descriptor_d method = {0, DTYPE_T, CLASS_D, 0};
  FREEXD_ON_EXIT(&xd);
  FREED_ON_EXIT(&method);
  status = _TreeGetRecord(
      dbid, head_nid ? head_nid : *((int *)nid_dsc->pointer), &xd);
  if (STATUS_NOT_OK)
    goto end;
  conglom_ptr = (struct descriptor_conglom *)xd.pointer;
  if (conglom_ptr->dtype != DTYPE_CONGLOM)
  {
    status = TreeNOT_CONGLOM;
    goto end;
  }
  const short OpcExtFunction = OPC_EXT_FUNCTION;
  DESCRIPTOR_FUNCTION(fun, &OpcExtFunction, 255);
  int i, nargs = nargs_in + 3;
  void **arglist = (void **)&fun.arguments[1];
  // VA_LIST_MDS_END_ARG(fun.arguments,nargs,4,-2,method_ptr);
  arglist[0] = (void *)(intptr_t)nargs; // if called with LibCallg
  arglist[1] = nid_dsc;
  arglist[2] = method_ptr;
  for (i = 0; i < nargs_in; i++)
    arglist[i + 3] = arglist_in[i];
  /**** Try python class ***/
  if (strdcmp(conglom_ptr->image, "__python__"))
  {
    status = LibFindImageSymbol_C("TdiShr", "_TdiIntrinsic", &_TdiIntrinsic);
    if (STATUS_OK)
    {
      if (nargs == 4 && strdcmp(method_ptr, "DW_SETUP"))
        nargs--;
      char exp[1024];
      DESCRIPTOR(exp_dsc, exp);
      strcpy(exp, "PyDoMethod(");
      for (i = 1; i < nargs - 1; i++)
        strcat(exp, "$,");
      strcat(exp, "$)");
      exp_dsc.length = strlen(exp);
      arglist[0] = &exp_dsc;
      status = _TdiIntrinsic(&dbid, OPC_EXECUTE, nargs, arglist, out_ptr);
    }
    goto end;
  }
  const DESCRIPTOR(underunder, "__");
  StrConcat((struct descriptor *)&method, conglom_ptr->model,
            (struct descriptor *)&underunder, method_ptr MDS_END_ARG);
  /**** Try lib call ***/
  if (conglom_ptr->image)
  {
    for (i = 0; i < method.length; i++)
      method.pointer[i] = (char)tolower(method.pointer[i]);
    if (conglom_ptr->image && conglom_ptr->image->dtype != DTYPE_T)
    {
      status = TdiINVDTYDSC;
      goto end;
    }
    void (*addr)();
    status = LibFindImageSymbol(conglom_ptr->image, &method, &addr);
    if (STATUS_OK)
    {
      arglist[nargs] = out_ptr;
      status = (int)(intptr_t)_LibCallg(&dbid, arglist, addr);
      goto end;
    }
  }
  /**** Try tdi fun ***/
  status = LibFindImageSymbol_C("TdiShr", "_TdiIntrinsic", &_TdiIntrinsic);
  if (STATUS_NOT_OK)
    goto end;
  void *funarglist[] = {(void *)&fun};
  fun.ndesc = nargs > 254 ? 255 : (unsigned char)(nargs + 1); // add NULL
  fun.arguments[0] = NULL;
  fun.arguments[1] = (struct descriptor *)&method; // aka arglist[0]
  status = _TdiIntrinsic(&dbid, OPC_EVALUATE, 1, funarglist, out_ptr);
  if (status == TdiUNKNOWN_VAR)
    status = TreeNOMETHOD;
end:;
  FREED_NOW(method);
  FREEXD_NOW(xd);
  return status;
}

int _TreeDoMethod(void *dbid, mdsdsc_t *nid_dsc, mdsdsc_t *method_ptr, ...)
{
  int nargs;
  void *arglist[255];
  arglist[0] = NULL;
  VA_LIST_MDS_END_ARG(arglist, nargs, 0, -1, method_ptr);
  return _TreeDoMethodA(dbid, nid_dsc, method_ptr, nargs, (mdsdsc_t **)arglist,
                        (mdsdsc_xd_t *)arglist[nargs]);
}
int TreeDoMethod(mdsdsc_t *nid_dsc, mdsdsc_t *method_ptr, ...)
{
  int nargs;
  void *arglist[255];
  arglist[0] = NULL;
  VA_LIST_MDS_END_ARG(arglist, nargs, 0, -1, method_ptr);
  return _TreeDoMethodA(*TreeCtx(), nid_dsc, method_ptr, nargs,
                        (mdsdsc_t **)arglist, (mdsdsc_xd_t *)arglist[nargs]);
}
