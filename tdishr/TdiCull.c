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
/*      Tdi1Cull.C
        CULL removes bad indices and EXTEND replaces them with the limit values.
                subscripts = CULL(dimension or array, [dim], [subscripts])
                subscripts = EXTEND(dimension or array, [dim], [subscripts])
                same = CULL or EXTEND(*, *, [subscripts])

        See also Tdi1ItoX.C for index to axis conversion and vice versa.
        See also Tdi1Subscript.C for subscripting and mapping.

        Ken Klare, LANL P-4     (c)1990,1991,1992
        KK      21-Oct-1992     remove text mismatches, NOT FOR EXTEND
*/
#include "tdinelements.h"
#include "tdirefcat.h"
#include "tdirefstandard.h"
#include "tdirefzone.h"
#include <mdsshr.h>
#include <mdsshr_messages.h>
#include <stdlib.h>
#include <string.h>
#include <tdishr_messages.h>

extern struct descriptor *TdiItoXSpecial;

extern int tdi_get_data();
extern int TdiUnits();
extern int TdiData();
extern int TdiCvtArgs();
extern int TdiGe();
extern int TdiLe();
extern int TdiMasterData();
extern int TdiGetArgs();
extern int TdiIsIn();
extern int TdiItoX();
extern int TdiGetLong();
extern int Tdi2Range();

extern unsigned short OpcValue;
static const DESCRIPTOR_FUNCTION_0(value, &OpcValue);
static const DESCRIPTOR_RANGE(EMPTY_RANGE, 0, 0, (struct descriptor *)&value);

/**********************************************
        Redo culled array or scalar.
*/
int TdiRecull(struct descriptor_xd *out_ptr) {
  INIT_STATUS;
  struct descriptor_a *px = (struct descriptor_a *)out_ptr->pointer;

  /****************************
  Culled scalar is null vector.
  ****************************/
  if (px->class != CLASS_A) {
    DESCRIPTOR_A(arr0, 0, 0, 0, 0);
    arr0.length = px->length;
    arr0.dtype = px->dtype;
    status = MdsCopyDxXd((struct descriptor *)&arr0, out_ptr);
  } else {
    px->dimct = 1;
    px->aflags.coeff = 0;
    px->aflags.bounds = 0;
  }
  return status;
}

/*---------------------------------------------
        Remove out-of-bounds integer-limited elements.
*/
int TdiIcull(int left, int right, struct descriptor_a *px) {
  INIT_STATUS;
  int *pi = (int *)px->pointer, *po = pi;
  int n;

  N_ELEMENTS(px, n);
  if
    STATUS_OK
  for (; --n >= 0; ++pi)
    if (*pi >= left && *pi <= right)
      *po++ = *pi;
  if (pi != po) {
    /********************************
    Scalars must be nulled elsewhere.
    ********************************/
    if (px->class == CLASS_A)
      px->arsize = (char *)po - px->pointer;
    status = SsINTERNAL;
  }
  return status;
}

/*---------------------------------------------
        Replace out-of-bounds integer-limited elements.
*/
int TdiIextend(int left, int right, struct descriptor_a *px) {
  INIT_STATUS;
  int *pi = (int *)px->pointer;
  int n;

  N_ELEMENTS(px, n);
  if
    STATUS_OK
  for (; --n >= 0; ++pi) {
    if (*pi < left)
      *pi = left;
    else if (*pi > right)
      *pi = right;
  }
  return status;
}

/*---------------------------------------------
        Remove elements not satisfying mask.
*/
static int rcull(struct descriptor *pnew __attribute__((unused)),
                 struct descriptor_a *pmask, struct descriptor_a *px) {
  INIT_STATUS;
  char *pm = pmask->pointer, *pi = px->pointer, *po = pi;
  int n, len = px->length;
  N_ELEMENTS(px, n);
  if
    STATUS_OK
  for (; --n >= 0; pi += len)
    if (*pm++) {
      if (po < pi)
        memcpy(po, pi, len);
      po += len;
    }
  if (pi != po) {
    status = SsINTERNAL;
    /********************************
     Scalars must be nulled elsewhere.
     ********************************/
    if (px->class == CLASS_A) {
      px->arsize = (unsigned int)((char *)po - px->pointer);
      px->dimct = 1;
      px->aflags.coeff = 0;
      px->aflags.bounds = 0;
    }
  }
  return status;
}

/*---------------------------------------------
        Replace elements not in mask.
*/
static int rextend(struct descriptor *pnew, struct descriptor_a *pmask,
                   struct descriptor_a *px) {
  INIT_STATUS;
  char *pn = pnew->pointer, *pm = pmask->pointer, *pi = px->pointer;
  int n, len = px->length;
  N_ELEMENTS(px, n);
  if
    STATUS_OK
  for (; --n >= 0; pi += len)
    if (!*pm++)
      memcpy(pi, pn, len);
  return status;
}

/**********************************************
        Going to find out who is naughty and nice.
*/
static int work(int rroutine(struct descriptor *, struct descriptor_a *,
                             struct descriptor_a *),
                opcode_t opcode, int narg, struct descriptor *list[3],
                struct descriptor_xd *out_ptr) {
  INIT_STATUS;
  TDITHREADSTATIC_INIT;
  struct descriptor_xd in = EMPTY_XD, tmp = EMPTY_XD, units = EMPTY_XD;
  int cmode = -1, dim, s1 = 1;
  struct descriptor_range *new[3];
  struct descriptor_with_units *pwu;
  struct descriptor_signal *psig = 0;
  struct descriptor_range fake_range;
  struct descriptor dx0, dx1;
  struct descriptor_xd sig[3] = {EMPTY_XD}, uni[3] = {EMPTY_XD},
                       dat[3] = {EMPTY_XD};
  struct TdiCatStruct cats[4];
  static const unsigned char omits[] = {DTYPE_DIMENSION, DTYPE_SIGNAL,
                                        DTYPE_DIMENSION, 0};
  static const unsigned char omitd[] = {DTYPE_WITH_UNITS, DTYPE_DIMENSION, 0};
  status = tdi_get_data(omits, list[0], &in);
  if (STATUS_OK && in.pointer->dtype == DTYPE_WITH_UNITS) {
    status = TdiUnits(in.pointer, &units MDS_END_ARG);
    if
      STATUS_OK
    status = tdi_get_data(&omits[1], &in, &in);
  }
  if (STATUS_OK && narg > 1 && list[1])
    status = TdiGetLong(list[1], &dim);
  else
    dim = 0;

  /*****************************
  Convert signal to a dimension.
  *****************************/
  if (STATUS_OK && in.pointer->dtype == DTYPE_SIGNAL) {
    tmp = in;
    in = EMPTY_XD;
    psig = (struct descriptor_signal *)tmp.pointer;
    if (dim >= psig->ndesc - 2)
      status = TdiBAD_INDEX;
    else
      status = tdi_get_data(omitd, psig->dimensions[dim], &in);
    if (STATUS_OK && in.pointer->dtype == DTYPE_WITH_UNITS) {
      status = TdiUnits(in.pointer, &units MDS_END_ARG);
      if
        STATUS_OK
      status = tdi_get_data(&omitd[1], &in, &in);
    } else
      MdsFree1Dx(&units, NULL);
    dim = 0;
  }
  /*************************
  Get dimension information.
  *************************/
  if
    STATUS_OK
  switch (in.pointer->dtype) {
  case DTYPE_MISSING:
    if (narg > 2 && list[2])
      status = MdsCopyDxXd(list[2], out_ptr);
    else
      MdsFree1Dx(out_ptr, NULL);
    break;
  case DTYPE_T:
    if (!(narg > 2 && list[2])) {
      if
        STATUS_OK
      status = TdiData(&in, out_ptr MDS_END_ARG);
      break;
    }
    MdsFree1Dx(out_ptr, NULL);
    status = TdiGetArgs(opcode, 1, &list[2], sig, uni, dat, cats);
    if (STATUS_OK && units.pointer == 0)
      status = MdsCopyDxXd((struct descriptor *)&uni[0], &units);
    if
      STATUS_OK
    status = TdiIsIn(dat[0].pointer, &in, &tmp MDS_END_ARG);
    if
      STATUS_OK
    status = rroutine(dat[0].pointer, (struct descriptor_a *)tmp.pointer,
                      (struct descriptor_a *)dat[0].pointer);
    if (status == SsINTERNAL)
      status = TdiRecull(&dat[0]);
    MdsFree1Dx(&sig[0], NULL);
    MdsFree1Dx(&uni[0], NULL);
    if
      STATUS_OK
    *out_ptr = dat[0];
    break;
  default:
    if (!(narg > 2 && list[2])) {
      if
        STATUS_OK
      status = TdiData(&in, out_ptr MDS_END_ARG);
      break;
    }
    /********************************
    Special conversion to get limits.
    Could use some tests to speed up.
    ********************************/
    MdsFree1Dx(out_ptr, NULL);
    status = TdiItoX(in.pointer, TdiItoXSpecial, &tmp MDS_END_ARG);
    if
      STATUS_OK {
        pwu = (struct descriptor_with_units *)tmp.pointer;
        if (pwu->dtype == DTYPE_WITH_UNITS)
          dx0 = *pwu->data;
        else
          dx0 = *tmp.pointer;
        dx0.class = CLASS_S;
        dx1 = dx0;
        dx1.pointer += dx1.length;
        new[0] = (struct descriptor_range *)&dx0;
        new[1] = (struct descriptor_range *)&dx1;
        new[2] = (struct descriptor_range *)list[2];
        while (new[2] && new[2]->class == CLASS_XD)
          new[2] = (struct descriptor_range *)new[2]->pointer;
        struct descriptor *keep[3];
        memcpy(keep, TDI_RANGE_PTRS, sizeof(keep));
        TDI_RANGE_PTRS[0] = &dx0;
        TDI_RANGE_PTRS[1] = &dx1;
        TDI_RANGE_PTRS[2] = 0;
        if (in.pointer->dtype == DTYPE_DIMENSION) {
          TDI_RANGE_PTRS[2] = in.pointer;
          /************************************************************
          Dimensions conversion with missing increment uses all values.
          ************************************************************/
          if (new[2]->dtype == DTYPE_RANGE &&
              (new[2]->ndesc == 2 ||
               (new[2]->ndesc == 3 && new[2]->deltaval == 0))) {
            fake_range = EMPTY_RANGE;
            fake_range.begin = new[2]->begin;
            fake_range.ending = new[2]->ending;
            new[2] = &fake_range;
          }
        }
        status = TdiGetArgs(opcode, 3, new, sig, uni, dat, cats);
        memcpy(TDI_RANGE_PTRS, keep, sizeof(keep));
      }
    if
      STATUS_OK
    status = Tdi2Range(3, uni, dat, cats, 0);
    if (STATUS_OK && units.pointer == 0)
      status = MdsCopyDxXd((struct descriptor *)&uni[0], &units);
    if
      STATUS_OK
    status = TdiCvtArgs(3, dat, cats);
    if
      STATUS_OK
    status = TdiGe(dat[2].pointer, dat[0].pointer, &tmp MDS_END_ARG);
    if
      STATUS_OK
    s1 = (status = rroutine(dat[0].pointer, (struct descriptor_a *)tmp.pointer,
                            (struct descriptor_a *)dat[2].pointer)) ==
         SsINTERNAL;
    if (STATUS_OK || s1 == SsINTERNAL)
      status = TdiLe(dat[2].pointer, dat[1].pointer, &tmp MDS_END_ARG);
    if
      STATUS_OK
    s1 |= (status = rroutine(dat[1].pointer, (struct descriptor_a *)tmp.pointer,
                             (struct descriptor_a *)dat[2].pointer)) ==
          SsINTERNAL;
    if (STATUS_OK && s1)
      status = TdiRecull(&dat[2]);
    MdsFree1Dx(&sig[0], NULL);
    MdsFree1Dx(&sig[1], NULL);
    MdsFree1Dx(&sig[2], NULL);
    MdsFree1Dx(&uni[0], NULL);
    MdsFree1Dx(&uni[1], NULL);
    MdsFree1Dx(&uni[2], NULL);
    MdsFree1Dx(&dat[0], NULL);
    MdsFree1Dx(&dat[1], NULL);
    if
      STATUS_OK
    *out_ptr = dat[2];
    else MdsFree1Dx(&dat[2], NULL);
    break;
  }
  MdsFree1Dx(&tmp, NULL);
  MdsFree1Dx(&in, NULL);
  if (STATUS_OK && units.pointer)
    status = TdiMasterData(0, sig, &units, &cmode, out_ptr);
  MdsFree1Dx(&units, NULL);
  return status;
}

/***********************************************/
int Tdi1Cull(opcode_t opcode, int narg, struct descriptor *list[],
             struct descriptor_xd *out_ptr) {
  return work(rcull, opcode, narg, list, out_ptr);
}

/***********************************************/
int Tdi1Extend(opcode_t opcode, int narg, struct descriptor *list[],
               struct descriptor_xd *out_ptr) {
  return work(rextend, opcode, narg, list, out_ptr);
}
