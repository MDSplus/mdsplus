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
  if (STATUS_NOT_OK) {
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
      status = TdiINVCLADSC;
      goto free_alist;
    }
    osize = oarr->arsize;
    olist = (struct descriptor**)oarr->pointer;
  } else {
    osize = 0;
    olist = NULL;
  }
  arr.arsize = osize+asize;
  if (arr.dtype==DTYPE_DICTIONARY && (!(alen & 1) || (osize/arr.length) & 1)) {
    // dict requires key-value-pairs
    status = TdiMISS_ARG;
    goto free_alist;
  }//TODO: Dict requires unique keys
  arr.pointer = malloc(arr.arsize);
  memcpy(memcpy(arr.pointer,olist,osize)+osize,alist+1,asize);
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
