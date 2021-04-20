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
/*      Tdi1ItoX.C
        Convert between axis and index.

                i = X_TO_I(dimension, [x])
        Signality of second, no units, extrapolates.

                x = I_TO_X(dimension, [i])
        Signality of second, units of dimension, extrapolates.

                same = I_TO_X or X_TO_I(non-dimension, [i or x])

        Internal routines to get bounds (ELBOUND/ESHAPE/ESIZE/EUBOUND/RANGE):
                i_two_vector = X_TO_I(dimension, TdiItoXSpecial)
                x_two_vector = I_TO_X(dimension, TdiItoXSpecial)

        Typical dimension:
   BUILD_DIM(BUILD_WINDOW(-pretrig,posttrig,triggertime),clockstart:clockstop:clockrate)
        Restriction: Dimension may not use complex numbers. What would it mean?

        Ken Klare, LANL P-4     (c)1990,1991,1992
        KK      21-Oct-1992     Text requires exact match.
 */
#include <mdsplus/mdsplus.h>
#define beg 0
#define end 1
#define delta 2
#define HUGE (1 << 28)
extern int IsRoprand();
#define ROPRAND(fptr) IsRoprand(DTYPE_NATIVE_FLOAT, fptr)
#define ROPRAND_TYPED(type, fptr) IsRoprand(type, fptr)
/* #define ROPRAND(fptr) (*(unsigned short *)(fptr) == 0x8000) */

#include "tdinelements.h"
#include "tdirefcat.h"
#include "tdirefstandard.h"
#include "tdithreadstatic.h"
#include <mdsshr.h>
#include <stdlib.h>
#include <string.h>
#include <tdishr_messages.h>

static const mdsdsc_t tdiItoXSpecial;
const mdsdsc_t *TdiItoXSpecial = &tdiItoXSpecial;

extern int tdi_get_data();
extern int TdiGetLong();
extern int TdiUnits();
extern int TdiData();
extern int TdiEvaluate();
extern int TdiGetArgs();
extern int TdiCvtArgs();
extern int Tdi2Range();
extern int TdiDtypeRange();
extern int TdiBsearch();
extern int TdiMap();
extern int TdiAdd();
extern int TdiSetRange();
extern int TdiSort();
extern int TdiLbound();
extern int TdiUbound();
extern int TdiMasterData();
extern int TdiFaultClear();
extern int TdiSubtract();
extern int TdiDivide();
extern int TdiFixRoprand();
extern int TdiDim();
extern int TdiMin();
extern int TdiNint();
extern int TdiGt();
extern int TdiFloor();
extern int TdiMax();
extern int TdiMultiply();

static const float big = (float)1.e37;
static const float mbig = (float)-1.e37;
static const int mhuge = -HUGE;
static const int lhuge = HUGE;
static const int mone = -1;
static const int one = 1;
static mdsdsc_t dmbig = {sizeof(mbig), DTYPE_FS, CLASS_S, (char *)&mbig};
static mdsdsc_t dbig = {sizeof(big), DTYPE_FS, CLASS_S, (char *)&big};
static mdsdsc_t dmhuge = {sizeof(mhuge), DTYPE_L, CLASS_S, (char *)&mhuge};
static mdsdsc_t dhuge = {sizeof(lhuge), DTYPE_L, CLASS_S, (char *)&lhuge};
static mdsdsc_t dmone = {sizeof(mone), DTYPE_L, CLASS_S, (char *)&mone};
static mdsdsc_t done = {sizeof(one), DTYPE_L, CLASS_S, (char *)&one};

extern unsigned short OpcValue, OpcVector;
static DESCRIPTOR_FUNCTION_0(value, &OpcValue);
static DESCRIPTOR_FUNCTION_0(vector0, &OpcVector);
static DESCRIPTOR_A(duo, sizeof(int), DTYPE_L, 0, 2 * sizeof(int));
static DESCRIPTOR_RANGE(fake0, 0, 0, 0);
int Tdi1ItoX(opcode_t opcode, int narg, mdsdsc_t *list[],
             mdsdsc_xd_t *out_ptr)
{
  INIT_STATUS;
  TDITHREADSTATIC_INIT;
  static const dtype_t omits[] = {DTYPE_WITH_UNITS, DTYPE_DIMENSION, 0};
  int j1, left, right, *pcnt = 0, *ptest;
  int k0, k1;
  int special = narg > 1 && list[1] == TdiItoXSpecial;
  int arg1 = narg > 1 && list[1] && list[1] != TdiItoXSpecial;
  int big_beg, big_end, nran, tslo;
  int nseg, jseg, kseg;
  const mdsdsc_t *pmode;
  int cmode = -1, flag = (opcode == OPC_I_TO_X);
  char *plogical;
  mds_dimension_t *pdim;
  mds_window_t *pwin = 0;
  mds_range_t *paxis = 0, fake;
  mds_slope_t *pslope;
  mdsdsc_t dk0 = {sizeof(int), DTYPE_L, CLASS_S, 0};
  mdsdsc_t dk1 = {sizeof(int), DTYPE_L, CLASS_S, 0};
  mdsdsc_t *keep[3];
  mdsdsc_t tst1, del1, int1;
  mdsdsc_xd_t dimen = EMPTY_XD, window = EMPTY_XD, axis = EMPTY_XD,
              xat0 = EMPTY_XD;
  mdsdsc_xd_t cnt = EMPTY_XD, tmp = EMPTY_XD, units = EMPTY_XD;
  mdsdsc_xd_t sig[3] = {EMPTY_XD}, uni[3] = {EMPTY_XD}, dat[3] = {EMPTY_XD};
  mdsdsc_xd_t sig1 = EMPTY_XD, uni1 = EMPTY_XD;
  struct TdiCatStruct cats[4];
  FUNCTION(255 / 3)
  vec[3];
  dk0.pointer = (char *)&k0;
  dk1.pointer = (char *)&k1;
  keep[0] = TDI_RANGE_PTRS[0];
  keep[1] = TDI_RANGE_PTRS[1];
  keep[2] = TDI_RANGE_PTRS[2];
  /************************************************************
  Remove and save outer WITH_UNITS.
  8-Apr-1991 allow BUILD_WITH_UNITS(BUILD_DIM(,range),units).
  Previously allowed BUILD_DIM(,BUILD_WITH_UNITS(range,units)).
   ************************************************************/
  if (!flag)
    status = tdi_get_data(&omits[1], list[0], &dimen);
  else
  {
    status = tdi_get_data(omits, list[0], &dimen);

    if (STATUS_OK && dimen.pointer->dtype == DTYPE_WITH_UNITS)
    {
      status = TdiUnits(dimen.pointer, &units MDS_END_ARG);
      if (STATUS_OK)
        status = tdi_get_data(&omits[1], &dimen, &dimen);
    }
  }
  if (STATUS_OK)
    switch (dimen.pointer->dtype)
    {
      /*********************
They just have a list.
     *********************/
    default:
      k0 = -HUGE;
      k1 = HUGE;
      pwin = 0;
      axis = dimen;
      dimen = EMPTY_XD;
      status = TdiData(0, &xat0 MDS_END_ARG);
      goto plain;
    case DTYPE_DIMENSION:
      pdim = (mds_dimension_t *)dimen.pointer;
      status = TdiEvaluate(pdim->window, &window MDS_END_ARG);
      pwin = (mds_window_t *)window.pointer;
      if (STATUS_OK && pwin && pwin->dtype == DTYPE_WITH_UNITS)
      {
        status = TdiEvaluate(((mds_with_units_t *)pwin)->data, &tmp MDS_END_ARG);
        if (STATUS_OK)
          status = MdsCopyDxXd((mdsdsc_t *)&tmp, &window);
      }
      if (STATUS_OK)
        status = TdiEvaluate(pdim->axis, &axis MDS_END_ARG);
      paxis = (mds_range_t *)axis.pointer;
      if (STATUS_OK && paxis && paxis->dtype == DTYPE_WITH_UNITS)
      {
        status = TdiEvaluate(((mds_with_units_t *)paxis)->data, &tmp MDS_END_ARG);
        if (STATUS_OK)
          status = MdsCopyDxXd((mdsdsc_t *)&tmp, &axis);
      }
      paxis = (mds_range_t *)axis.pointer;
      pwin = (mds_window_t *)window.pointer;
      break;
    }
  if (STATUS_OK && pwin && pwin->dtype == DTYPE_WINDOW)
    ;
  else
    pwin = 0;
  if (STATUS_OK && pwin && pwin->startidx)
    status = TdiGetLong(pwin->startidx, &k0);
  else
    k0 = -HUGE;
  if (STATUS_OK && pwin && pwin->endingidx)
    status = TdiGetLong(pwin->endingidx, &k1);
  else
    k1 = HUGE;
  if (STATUS_OK)
  { /*bug 2/26/91 include status test both ways */
    if (pwin && pwin->value_at_idx0)
      status = TdiData(pwin->value_at_idx0, &xat0 MDS_END_ARG);
    else
      status = TdiData(0, &xat0 MDS_END_ARG);
  }

  if (STATUS_OK)
  {
    switch (paxis->dtype)
    {
    case DTYPE_SLOPE:
      pslope = (mds_slope_t *)paxis;
      paxis = &fake;
      fake = fake0;
      switch (pslope->ndesc)
      {
      case 3:
        fake.ending = pslope->segment[0].ending;
        MDS_ATTR_FALLTHROUGH
      case 2:
        fake.begin = pslope->segment[0].begin;
        MDS_ATTR_FALLTHROUGH
      case 1:
        fake.deltaval = pslope->segment[0].slope;
        break;
      case 0:
        status = TdiINVDTYDSC;
        break;
      default:
        *(mds_function_t *)(fake.begin = (mdsdsc_t *)&vec[0]) = vector0;
        *(mds_function_t *)(fake.ending = (mdsdsc_t *)&vec[1]) = vector0;
        *(mds_function_t *)(fake.deltaval = (mdsdsc_t *)&vec[2]) = vector0;
        nseg = ((pslope->ndesc + 1) / 3);
        vec[0].ndesc = (unsigned char)nseg;
        vec[1].ndesc = (unsigned char)nseg;
        vec[2].ndesc = (unsigned char)nseg;
        for (jseg = nseg; --jseg >= 0;)
          if ((vec[0].arguments[jseg] = pslope->segment[jseg].begin) == 0)
            vec[0].arguments[jseg] = (mdsdsc_t *)&dmbig;
        for (jseg = nseg; --jseg >= 0;)
          if ((vec[1].arguments[jseg] = pslope->segment[jseg].ending) == 0)
            vec[1].arguments[jseg] = (mdsdsc_t *)&dbig;
        for (jseg = nseg; --jseg >= 0;)
          if ((vec[2].arguments[jseg] = pslope->segment[jseg].slope) == 0)
            vec[1].arguments[jseg] = (mdsdsc_t *)&done;
        break;
      }
      if (STATUS_NOT_OK)
        break;
      /*********************
WARNING falls through.
         *********************/
      MDS_ATTR_FALLTHROUGH
    case DTYPE_RANGE:
      status = TdiGetArgs(opcode, nran = paxis->ndesc, &paxis->begin, sig,
                          uni, dat, cats);
      nseg = HUGE;
      if (STATUS_OK)
      { /*bug 2/26/91 include status set in bracket */
        for (j1 = nran; --j1 >= 0;)
          if (dat[j1].pointer->class == CLASS_A)
          {
            jseg = ((mdsdsc_a_t *)dat[j1].pointer)->arsize /
                   (int)dat[j1].pointer->length;
            if (nseg > jseg)
              nseg = jseg;
          }
        if (nseg == HUGE)
          nseg = 1;
        if (nseg <= 0)
          status = TdiNULL_PTR;
      }

      /************************
         * I_TO_X: combined units.
         * X_TO_I: no units.
         ************************/
      if (STATUS_OK)
        status = Tdi2Range(nran, uni, dat, cats, 0);
      if (flag && units.pointer == 0 && uni[0].pointer && STATUS_OK)
        status = MdsCopyDxXd((mdsdsc_t *)&uni[0], &units);
      if (STATUS_NOT_OK)
        goto firstbad;
      tslo = nran > 2 && cats[2].in_dtype != DTYPE_MISSING;

      /*************************************************************************
         * Simple range is best done quickly.
         * I_TO_X: X = I*delta+xat0.
         * X_TO_I: I = (X-xat0)/delta.
         * If given a beg or an end, use it. This takes extra work.
         * If beg not missing, reserved, > -huge*delta: left =
         *floor((beg-xat0)/delta) If end not missing, reserved, < +huge*delta:
         *right = floor((end-xat0)/delta) No beg nor end implies 0<=>xat0.
         * Remember that xat0 is trigger time and next clock tick is index zero.
         * Thus, for xat0 before or at beg, the correct xat0 is beg, and
         *       for xat0 just after beg, the correct xat0 is beg+delta.
         * WARNING need to watch overflow of big_number/small_delta.
         * WARNING may need more careful rounding.
         *************************************************************************/
      big_beg =
          cats[0].in_dtype == DTYPE_MISSING ||
          (cats[0].in_cat & TdiCAT_FLOAT &&
           ROPRAND_TYPED(dat[beg].pointer->dtype, dat[beg].pointer->pointer));
      big_end = cats[1].in_dtype == DTYPE_MISSING ||
                (cats[1].in_cat & TdiCAT_FLOAT &&
                 ROPRAND_TYPED(dat[end].pointer->dtype,
                               dat[end].pointer->pointer +
                                   dat[end].pointer->length * (nseg - 1)));
      if (nseg == 1)
      {
        dat[beg].pointer->class = CLASS_S;
        dat[end].pointer->class = CLASS_S;
        if (tslo)
          dat[delta].pointer->class = CLASS_S;
        if (big_beg && big_end)
          goto select;
        if (big_beg || big_end)
          goto counted;
      }
      status = TdiCvtArgs(nran, dat, cats);
      TdiFaultClear(0);
      if (STATUS_OK)
        status =
            TdiSubtract(dat[end].pointer, dat[beg].pointer, &cnt MDS_END_ARG);
      if (tslo && STATUS_OK)
        status = TdiDivide(&cnt, dat[delta].pointer, &cnt MDS_END_ARG);
      if (!(TdiFaultClear(0) & 1) && STATUS_OK)
        status = TdiFixRoprand(&cnt, &dhuge, &cnt);
      if (STATUS_OK)
        status = TdiDim(&cnt, &dmone, &cnt MDS_END_ARG);
      if (STATUS_OK)
        status = TdiMin(&cnt, &dhuge, &cnt MDS_END_ARG);
      if (STATUS_OK && cnt.pointer->dtype != DTYPE_L)
        status = TdiNint(&cnt, &cnt MDS_END_ARG);

      /*********************************************************
         * Find segment with value at index 0. Always 0 for nseg=1.
         * Find smallest kseg so that (beg[kseg+1]-xat0)/delta > 0.
         * Does not check beg[0], it might be ROPRAND.
         * We use the kseg-th value to find left, hold it a while.
         * Missing window assigns first axis point as index 0.
         *********************************************************/
    counted:
      kseg = 0;
      if (nseg > 1 || !big_beg)
      {
        if (STATUS_OK)
          status =
              TdiSubtract(dat[beg].pointer, xat0.pointer, &tmp MDS_END_ARG);
        if (tslo && STATUS_OK)
          status = TdiDivide(&tmp, dat[delta].pointer, &tmp MDS_END_ARG);
        if (!(TdiFaultClear(0) & 1) && STATUS_OK)
          status = TdiFixRoprand(&tmp, &dmhuge, &tmp);
      }
      left = -HUGE;
      if (!pwin)
        left = 0; /*bug 2/26/91 missing window */
      else if (nseg > 1)
      {
        if (STATUS_OK)
          status = TdiGt(tmp.pointer, 0, out_ptr MDS_END_ARG);
        if (STATUS_OK)
        {
          plogical = out_ptr->pointer->pointer;
          for (; kseg < nseg - 1 && !*++plogical; ++kseg)
            ;
        }
      }

      /**********************************************************************************************
         * With index 0 in kseg, what is index at beg? left[kseg] =
         *floor((beg[kseg]-xat0)/delta[kseg]). Trigger time xat0 implies next
         *clock is at index 0, thus round down, make more negative. Correct xat0
         *for good beg of single segment.
         **********************************************************************************************/
      if ((kseg > 0 || !big_beg) && STATUS_OK)
      {
        if (pwin)
        {
          if (tmp.pointer->dtype == DTYPE_L)
            left = *((int *)tmp.pointer->pointer + kseg);
          else
          {
            tst1 = *tmp.pointer;
            tst1.class = CLASS_S;
            tst1.pointer += tst1.length * kseg;
            status = TdiFloor(&tst1, &uni1 MDS_END_ARG);
            if (STATUS_OK)
              status = TdiMax(&uni1, &dmhuge, &tmp MDS_END_ARG);
            MdsFree1Dx(&uni1, NULL);
            if (STATUS_OK)
              status = TdiMin(&tmp, &dhuge, &tmp MDS_END_ARG);
            if (STATUS_OK)
              status = TdiGetLong(tmp.pointer, &left);
          }
        }
        if (kseg == 0 && left > 0)
          left = 0; /* 1/7/92 trig before clock, use first time */
        if (nseg == 1)
        {
          if (k0 < left)
            k0 = left;
          if (left <= -HUGE)
            big_beg = 1;
          else
          {
            int1 = dk0;
            int1.pointer = (char *)&left;
            if (STATUS_OK)
              status = MdsCopyDxXd(&int1, &tmp);
            if (tslo && STATUS_OK)
              status =
                  TdiMultiply(&tmp, dat[delta].pointer, &tmp MDS_END_ARG);
            if (STATUS_OK)
              status = TdiSubtract(&dat[beg], &tmp, &xat0 MDS_END_ARG);
            if (!big_end)
              right = left + *(int *)cnt.pointer->pointer;
          }
        }
      }

      /******************************************************
         * Then adjust and accumulate counts.
         * left = index of first beg, right = index of last end.
         * If beg[0] undefined, set beg[0]=end[0] with cnt[0]=1.
         * If both are undefined (nseg==1) xat0 will be used.
         ******************************************************/
      if (nseg > 1 && STATUS_OK)
      {
        ptest = pcnt = (int *)cnt.pointer->pointer;
        for (jseg = kseg; --jseg >= 0;)
          left -= *ptest++; /*bug 2/26/91 increment ptest */
        if (k0 < left)
          k0 = left;
        if (left <= -HUGE)
        {
          big_beg = 1;
          right = left + HUGE;
          *pcnt = 1;
          memcpy(dat[beg].pointer->pointer, dat[end].pointer->pointer,
                 dat[end].pointer->length);
        }
        else
          right = left + *pcnt;
      }

      /*****************************************************************
When we can't get left side, use right of first segment for limit.
         *****************************************************************/
      if (kseg == 0 && big_beg && (nseg > 1 || !big_end))
      {
        if (STATUS_OK)
        {
          tst1 = *dat[end].pointer;
          tst1.class = CLASS_S;
          status = TdiSubtract(&tst1, xat0.pointer, &tmp MDS_END_ARG);
        }
        if (tslo && STATUS_OK)
        {
          del1 = *dat[delta].pointer;
          del1.class = CLASS_S;
          status = TdiDivide(&tmp, &del1, &tmp MDS_END_ARG);
        }
        if (STATUS_OK)
        {
          if (tmp.pointer->dtype == DTYPE_L)
            right = *(int *)tmp.pointer->pointer;
          else
          {
            if (!(TdiFaultClear(0) & 1))
              status = TdiFixRoprand(&tmp, &dhuge, &tmp);
            if (STATUS_OK)
              status = TdiFloor(&tmp, &tmp MDS_END_ARG);
            if (STATUS_OK)
              status = TdiMax(&tmp, &dmhuge, &tmp MDS_END_ARG);
            if (STATUS_OK)
              status = TdiMin(&tmp, &dhuge, &tmp MDS_END_ARG);
            if (STATUS_OK)
              status = TdiGetLong(&tmp, &right);
          }
        }
        if (nseg == 1 && right < HUGE)
        {
          int1 = dk0;
          int1.pointer = (char *)&right;
          if (STATUS_OK)
            status = MdsCopyDxXd(&int1, &tmp);
          if (tslo && STATUS_OK)
            status = TdiMultiply(&tmp, &del1, &tmp MDS_END_ARG);
          if (STATUS_OK)
            status = TdiSubtract(&tst1, &tmp, &xat0 MDS_END_ARG);
        }
        ++right;
      }

      /*********************************************
         * Set the breakpoints to the sum of counts.
         * Correct xat0 for good end of single segment.
         *********************************************/
      if (nseg > 1)
      {
        *pcnt = right - *pcnt;
        for (jseg = nseg; --jseg > 0;)
          j1 = *++pcnt, *pcnt = right, right += j1;
      }
      if (!big_end && k1 >= right)
        k1 = right - 1;

      /*******************************
         * Bounds for ESHAPE/CULL/EXTEND.
         * Or see what points they want.
         * GET_ARGS does not free.
         *******************************/
    select:
      if (STATUS_OK)
      {
        if (special)
        {
          int limits[2];
          array_int dlimits = *(array_int *)&duo;
          limits[0] = k0;
          limits[1] = k1;
          dlimits.pointer = limits;
          status = MdsCopyDxXd((mdsdsc_t *)&dlimits, out_ptr);
        }
        else if (arg1)
        {
          mds_range_t *rptr = (mds_range_t *)list[1];
          MdsFree1Dx(out_ptr, NULL);
          TDI_RANGE_PTRS[0] = &dk0;
          TDI_RANGE_PTRS[1] = &dk1;
          TDI_RANGE_PTRS[2] = flag ? 0 : dimen.pointer;
          /******************************************************
               * For subscripts of signals, want range step to be all.
               ******************************************************/
          if (!flag && (rptr->dtype == DTYPE_RANGE) &&
              ((rptr->ndesc == 2) ||
               ((rptr->ndesc == 3) && (rptr->deltaval == 0))))
          {
            DESCRIPTOR_RANGE(fake_range, 0, 0, 0);
            fake_range.begin = rptr->begin;
            fake_range.ending = rptr->ending;
            fake_range.deltaval = (mdsdsc_t *)&value;
            rptr = &fake_range;
            status =
                TdiGetArgs(opcode, 1, &rptr, &sig1, &uni1, out_ptr, cats);
          }
          else
            status =
                TdiGetArgs(opcode, 1, &rptr, &sig1, &uni1, out_ptr, cats);
          TDI_RANGE_PTRS[0] = keep[0];
          TDI_RANGE_PTRS[1] = keep[1];
          TDI_RANGE_PTRS[2] = keep[2];
          arg1 = cats[0].in_dtype != DTYPE_MISSING;
        }
        else
          status = TdiDtypeRange(&dk0, &dk1, out_ptr MDS_END_ARG);

        /************************************************************
             * Hinged, piecewise-linear segments with interpolation between end
             *and next beg. I =   (X-end[0])/delta[0]             + sumcnt[1]-1
             *X<=end[0] (X-beg[1])/(beg[1]-end[0])      + sumcnt[1]     X>end[0]
             *&&     X<=beg[1] (X-beg[1])/delta[1]             + sumcnt[1]
             *X>beg[1] &&     X<=end[1]
             *       ...
             * X =   (I-(sumcnt[1]-1))*delta[0]      + end[0] I<=sumcnt[1]-1
             *       (I-sumcnt[1])*(beg[1]-end[0])   + beg[1] I>sumcnt[1]-1
             *I<=sumcnt[1] (I-sumcnt[1])*delta[1]          + beg[1] I>sumcnt[1]
             *I<=sumcnt[2]-1
             *       ...
             ************************************************************/

        /******************************************************************
             * Approximation neglecting between segment code:
             * I_TO_X: X = (I-cnt[j])*delta[j]+beg[j] for cnt[j] <= I < cnt[j+1]
             *       X = I*delta[j]+xat0, with xat0 = beg[j]-cnt[j]*delta[j]
             * X_TO_I: I = (X-beg[j])/delta[j]+cnt[j] for beg[j] <= X < beg[j+1]
             *       I = (X-xat0)/delta[j]
             * For X=* we have I=k0:k1 and no reason to do subscripting.
             * For multisegment, limit between-segment to its end. 11-Apr-91
             *removed 22-may-91 for roprand
             ******************************************************************/
      }
      if (nseg > 1 && (flag || arg1))
      {
        if (STATUS_OK)
          status = TdiBsearch(out_ptr->pointer,
                              flag ? cnt.pointer : dat[beg].pointer, &done,
                              &tmp MDS_END_ARG);
        if (STATUS_OK && dat[beg].pointer->class == CLASS_A)
          status = TdiMap(&dat[beg], tmp.pointer, &dat[beg] MDS_END_ARG);
        if (STATUS_OK && dat[end].pointer->class == CLASS_A)
          status = TdiMap(&dat[end], tmp.pointer, &dat[end] MDS_END_ARG);
        if (STATUS_OK && cnt.pointer->class == CLASS_A)
          status = TdiMap(&cnt, tmp.pointer, &cnt MDS_END_ARG);
        if (tslo && STATUS_OK && dat[delta].pointer->class == CLASS_A)
          status = TdiMap(&dat[delta], &tmp, &dat[delta] MDS_END_ARG);
        if (tslo && STATUS_OK)
          status = TdiMultiply(&cnt, dat[delta].pointer, &cnt MDS_END_ARG);
        if (STATUS_OK)
          status = TdiSubtract(&dat[beg], &cnt, &xat0 MDS_END_ARG);
      }

      if (flag)
      {
        if (tslo && STATUS_OK)
          status = TdiMultiply(out_ptr, &dat[delta], out_ptr MDS_END_ARG);
        if (STATUS_OK)
          status = TdiAdd(out_ptr, &xat0, out_ptr MDS_END_ARG);
        /*      if (nseg > 1 && STATUS_OK) status = TdiMin(out_ptr, &dat[end],
           * out_ptr MDS_END_ARG); */
      }
      else if (arg1)
      {
        /*      if (nseg > 1 && STATUS_OK) status = TdiMin(out_ptr, &dat[end],
           * out_ptr MDS_END_ARG); */
        if (STATUS_OK)
          status = TdiSubtract(out_ptr, &xat0, out_ptr MDS_END_ARG);
        if (tslo && STATUS_OK)
          status = TdiDivide(out_ptr, &dat[delta], out_ptr MDS_END_ARG);
      }
      MdsFree1Dx(&cnt, NULL);
      MdsFree1Dx(&uni1, NULL);

      /*******************************************************************
For full range, set bounds if lower is nonzero.
For given list, order of operations is must preserve original shape.
MAP preserves shape of second arg and single segment uses scalars.
         *******************************************************************/
      if (STATUS_OK && k0 != 0 && !arg1 && !special)
      {
        DESCRIPTOR_RANGE(range, 0, 0, 0);
        range.ndesc = 2;
        range.begin = &dk0;
        range.ending = &dk1;
        status = TdiSetRange(&range, out_ptr, out_ptr MDS_END_ARG);
      }

      /********************************
Embed result in units and signal.
         ********************************/
      if (STATUS_OK)
        status = TdiMasterData(narg > 1, &sig1, &units, &cmode, out_ptr);
      MdsFree1Dx(&sig1, NULL);
    firstbad:
      for (; --nran >= 0;)
      {
        MdsFree1Dx(&sig[nran], NULL);
        MdsFree1Dx(&uni[nran], NULL);
        MdsFree1Dx(&dat[nran], NULL);
      }
      break;
      /**************************************
General list of values for a dimension.
I_TO_X: just map it, add xat0.
X_TO_I: subtract xat0, sort, search, and map.
1/6/92 added xat0 and window range
Assumes one-dimensional axis.
Cull with range doesn't work for text
because it generates range.
         **************************************/
    default:
    plain:
      status = TdiData(&axis, &axis MDS_END_ARG);
      paxis = (mds_range_t *)axis.pointer;
      pmode = paxis->dtype == DTYPE_T ? NULL : &dmone;
      j1 = paxis->length;
      if (STATUS_OK)
        status = TdiSort(paxis, &tmp MDS_END_ARG);
      if ((pwin || flag || arg1) && STATUS_OK)
      {
        status = TdiMap(paxis, tmp.pointer, &window MDS_END_ARG);
      }
      if (pwin)
      {
        if (STATUS_OK)
          status = TdiBsearch(&xat0, paxis, pmode, &xat0 MDS_END_ARG);
        if (STATUS_OK)
          status = TdiGetLong(&xat0, &left);
        if (STATUS_OK)
        {
          N_ELEMENTS(paxis, right);
          left = -left;
          right += left - 1;
          if (k0 < left)
            k0 = left;
          if (k1 > right)
            k1 = right;
        }
      }
      else
      {
        if (STATUS_OK)
          status = TdiLbound(axis.pointer, &dk0 MDS_END_ARG);
        if (STATUS_OK)
          status = TdiUbound(axis.pointer, &dk1 MDS_END_ARG);
      }
      if (STATUS_OK)
      {
        if (special)
        {
          if (flag)
          {
            unsigned short num = (unsigned short)j1;
            status = MdsGet1DxA((mdsdsc_a_t *)&duo, &num, &paxis->dtype,
                                out_ptr);
            if (STATUS_OK)
            {
              memcpy(out_ptr->pointer->pointer,
                     (char *)window.pointer->pointer, j1);
              memcpy(out_ptr->pointer->pointer + j1,
                     (char *)window.pointer->pointer + j1 * (k1 - k0),
                     j1);
            }
          }
          else
          {
            int limits[2];
            array_int dlimits = *(array_int *)&duo;
            limits[0] = k0;
            limits[1] = k1;
            dlimits.pointer = limits;
            status = MdsCopyDxXd((mdsdsc_t *)&dlimits, out_ptr);
          }
        }
        else if (!arg1)
        {
          if (flag)
            status = MdsCopyDxXd((mdsdsc_t *)&axis,
                                 out_ptr); /*unsorted values */
          else
            status = TdiDtypeRange(&dk0, &dk1,
                                   out_ptr MDS_END_ARG); /*plain indexes */
        }
        else
        {
          if (flag)
          {
            if (k0 != 0 && pwin)
            {
              status = TdiSubtract(
                  list[1], &dk0,
                  &tmp MDS_END_ARG); /*offset of xat0 or array bound */
              status = TdiMap(&axis, &tmp,
                              out_ptr MDS_END_ARG); /*straight mapping */
            }
            else
              status = TdiMap(&axis, list[1],
                              out_ptr MDS_END_ARG); /*straight mapping */
          }
          else
          {
            status = TdiBsearch(list[1], &window, pmode,
                                out_ptr MDS_END_ARG); /*lookup */
            if (k0 != 0 && STATUS_OK)
              status =
                  TdiAdd(out_ptr, &dk0, out_ptr MDS_END_ARG); /*offset */
            if (STATUS_OK)
              status =
                  TdiMap(&tmp, out_ptr, out_ptr MDS_END_ARG); /*unorder */
          }
        }
      }
      if (STATUS_OK && k0 != 0 && out_ptr != NULL &&
          out_ptr->pointer != NULL && out_ptr->pointer->class == CLASS_A)
      {
        DESCRIPTOR_RANGE(range, 0, 0, 0);
        status = TdiSetRange(&range, out_ptr, out_ptr MDS_END_ARG);
      }
      if (STATUS_OK)
        status = TdiMasterData(0, &sig1, &units, &cmode, out_ptr);
      break;
    }
  }
  MdsFree1Dx(&tmp, NULL);
  MdsFree1Dx(&units, NULL);
  MdsFree1Dx(&xat0, NULL);
  MdsFree1Dx(&axis, NULL);
  MdsFree1Dx(&window, NULL);
  MdsFree1Dx(&dimen, NULL);
  return status;
}
