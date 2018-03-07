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
/*      Tdi1Apd.C
        Converts a list of Descriptors into a List Dict or Tuple.
        Timo Schröder, IPP      2017
*/

#include "STATICdef.h"
#include "tdirefcat.h"
#include "tdirefstandard.h"
#include "tdinelements.h"
#include <libroutines.h>
#include <tdishr_messages.h>
#include <stdlib.h>
#include <string.h>
#include <mdsshr.h>

extern int TdiData();
extern int TdiEvaluate();
extern unsigned short OpcComma;
int unwrapCommaCount(int ndesc, struct descriptor *list[]) {
  if (list[0] && list[0]->class==CLASS_R && list[0]->dtype==DTYPE_FUNCTION && list[0]->length==2 && *(short*)list[0]->pointer==OpcComma)
    return unwrapCommaCount(((struct descriptor_r*)list[0])->ndesc, ((struct descriptor_r*)list[0])->dscptrs) + ndesc-1;
  return ndesc;
}

int UnwrapCommaDesc(int ndesc, struct descriptor *list[], int *nout, struct descriptor *list_out[]){
  INIT_STATUS;
  struct descriptor_xd xd = EMPTY_XD;
  if (list[0] && list[0]->class==CLASS_R && list[0]->dtype==DTYPE_FUNCTION && list[0]->length==2 && *(short*)list[0]->pointer==OpcComma)
    status = UnwrapCommaDesc(((struct descriptor_r*)list[0])->ndesc, ((struct descriptor_r*)list[0])->dscptrs, nout, list_out);
  else {
    status = TdiData(list[0], &xd MDS_END_ARG);
    if STATUS_OK list_out[(*nout)++] = xd.pointer;
  }
  int i;
  for ( i=1 ; STATUS_OK && i<ndesc ; i++ ) {
    xd.pointer = NULL;
    status = TdiEvaluate(list[i], &xd MDS_END_ARG);
    list_out[(*nout)++] = xd.pointer;
  }
  return status;
}

int UnwrapComma(int narg, struct descriptor *list[], int *nout_p, struct descriptor **list_ptr[]) {
  *nout_p = unwrapCommaCount(narg, list);
  int nout = 0;
  *list_ptr = malloc(*nout_p * sizeof(void*));
  int status = UnwrapCommaDesc(narg, list, &nout, *list_ptr);
  if STATUS_NOT_OK {
    int i;
    for ( i=0 ; i<nout ; i++ )
      if ((*list_ptr)[i])
        free((*list_ptr)[i]);
    free(*list_ptr);
  }
  return status;
}

int Tdi1Apd(int dtype, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr){
  struct descriptor_a arr = { sizeof(void*), (unsigned char)(dtype & 0xff), CLASS_APD, (void*)0, 0, 0, {0}, 1, 0};
  if (narg==0) return MdsCopyDxXd((struct descriptor*)&arr, out_ptr);
  struct descriptor **alist, **olist;
  int osize, asize, alen;
  int status = UnwrapComma(narg, list, &alen, &alist);
  if STATUS_NOT_OK return status;
  asize = (alen-1)*sizeof(void*);
  struct descriptor_a *oarr = (struct descriptor_a*)alist[0];
  if (oarr && oarr->dtype) {
    if (oarr->class!=CLASS_APD || oarr->length!=sizeof(void*)) {
      status = ApdAPD_APPEND;
      goto free_alist;
    }
    osize = oarr->arsize;
    olist = (struct descriptor**)oarr->pointer;
  } else {
    osize = 0;
    olist = NULL;
  }
  if (arr.dtype==DTYPE_DICTIONARY) {
    if (!(alen&1 || (osize/arr.length)&1)) {
      status = ApdDICT_KEYVALPAIR;
      goto free_alist;
    }
    int i;
    for (i=1 ; i<alen ; i+=2) {
      if (alist[i]->class!=CLASS_S) {
	status = ApdDICT_KEYCLS;
        goto free_alist;
      }
    }
  }//TODO: Dict requires unique keys
  arr.arsize = osize+asize;
  arr.pointer = malloc(arr.arsize);
  if (olist)
    memcpy(arr.pointer,olist,osize);
  memcpy(arr.pointer+osize,alist+1,asize);
  status = MdsCopyDxXd((struct descriptor*)&arr, out_ptr);
  free(arr.pointer);
free_alist :;
  int i;
  for ( i=0 ; i<alen ; i++ )
    if (alist[i])
      free(alist[i]);
  free(alist);
  return status;
}

int Tdi1List(int opcode __attribute__((unused)), int narg, struct descriptor *list[], struct descriptor_xd *out_ptr){
   return Tdi1Apd(DTYPE_LIST, narg, list, out_ptr);
}
int Tdi1Tuple(int opcode __attribute__((unused)), int narg, struct descriptor *list[], struct descriptor_xd *out_ptr){
   return Tdi1Apd(DTYPE_TUPLE, narg, list, out_ptr);
}
int Tdi1Dict(int opcode __attribute__((unused)), int narg, struct descriptor *list[], struct descriptor_xd *out_ptr){
   return Tdi1Apd(DTYPE_DICTIONARY, narg, list, out_ptr);
}
