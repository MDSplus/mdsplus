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
/*      Tdi1Bsearch.C
        Binary search for offset in a table, including iNEQ_uality searches.
                status = TdiBsearch(&list_dsc, &table_dsc, [&mode_dsc, [&upcase_dsc,]] &output_xd);
        or      output = BSEARCH(list, table, [mode, [upcase,]])

        list    values to look up
        table   strictly ascending sorted values, table[j] < table[j+1]
        mode    equality:       ==0 for table[j] == list[k],            result 0 (bottom) to n-1 (top), no match is -1
                less/equal:     <0 for table[j-1] < list[k] <= table[j], result 0 (bottom or below) to n (above top)
                greater/equal:  >0 for table[j] <= list[k] < table[j+1], result -1 (below bottom) to n-1 (top or above)
        upcase  0 or missing    case sensitive
                1               case insensitive, compares text as uppercase.

        Method: "Numerical Recipes" pp.89-92, Knuth Vol.3 pp.406-411.
        Note that N.R. does not clearly define case of equality but does allow descending table.
        The greater/equal form is used and adjusted when search finishes.

        Strictly speaking COMPLEX numbers cannot be sorted, but we provide routines
        so that UNION, which should be able to work with complex, will not stub its toe.
        The complex "sort" orders first by real part and then by imaginary part.

        Ken Klare, LANL P-4     (c)1990,1992
        KK      21-Oct-1992     upcase options for BSEARCH/SORT/SORTVAL
*/
#include <STATICdef.h>
#include "tdinelements.h"
#include "tdirefcat.h"
#include "tdirefstandard.h"
#include <strroutines.h>
#include <tdishr_messages.h>
#include <stdlib.h>
#include <mdsshr.h>
#include <string.h>
#include <mdstypes.h>



#define _MOVC3(a,b,c) memcpy(c,b,a)
extern unsigned short OpcVector;

extern int TdiGetArgs();
extern int TdiCvtArgs();
extern int TdiMasterData();
extern int TdiGetLong();
extern int TdiGetShape();
extern int Tdi2Bsearch();
extern int TdiBsearch();
extern int TdiGe();
extern int TdiSort();
extern int TdiEvaluate();
extern int TdiMap();
extern int TdiIntrinsic();
extern int TdiSortVal();

/*--------------------------------------------------
        Greater than tests.
        >0 implies a > b, <=0 implies a <= b.
        Some are negative and some are zero for a < b.
        Some tests are VAX dependent.
        Equality test where different.
*/
#define NEQ_BU GTR_BU
STATIC_ROUTINE int GTR_BU(unsigned char *a, unsigned char *b)
{
  return *a - *b;
}

#define NEQ_WU GTR_WU
STATIC_ROUTINE int GTR_WU(unsigned short *a, unsigned short *b)
{
  return *a - *b;
}

#define NEQ_LU NEQ_L
STATIC_ROUTINE int GTR_LU(unsigned int *a, unsigned int *b)
{
  return *a > *b;
}

#define NEQ_QU NEQ_Q
STATIC_ROUTINE int GTR_QU(unsigned int a[2], unsigned int b[2])
{
  return a[1] > b[1] || (a[1] == b[1] && a[0] > b[0]);
}

#define NEQ_OU NEQ_O
STATIC_ROUTINE int GTR_OU(unsigned int a[4], unsigned int b[4])
{
  return a[3] > b[3] ||
      (a[3] == b[3] &&
       (a[2] > b[2] || (a[2] == b[2] && (a[1] > b[1] || (a[1] == b[1] && a[0] > b[0])))));
}

#define NEQ_B GTR_B
STATIC_ROUTINE int GTR_B(char *a, char *b)
{
  return *a - *b;
}

#define NEQ_W GTR_W
STATIC_ROUTINE int GTR_W(short *a, short *b)
{
  return *a - *b;
}

STATIC_ROUTINE int NEQ_L(int *a, int *b)
{
  return *a != *b;
}

STATIC_ROUTINE int GTR_L(int *a, int *b)
{
  return *a > *b;
}

STATIC_ROUTINE int NEQ_Q(unsigned int a[2], unsigned int b[2])
{
  return a[0] != b[0] || a[1] != b[1];
}

STATIC_ROUTINE int GTR_Q(unsigned int a[2], unsigned int b[2])
{
  return (int)a[1] > (int)b[1] || (a[1] == b[1] && a[0] > b[0]);
}

STATIC_ROUTINE int NEQ_O(unsigned int a[4], unsigned int b[4])
{
  return a[0] != b[0] || a[1] != b[1] || a[2] != b[2] || a[3] != b[3];
}

STATIC_ROUTINE int GTR_O(unsigned int a[4], unsigned int b[4])
{
  return (int)a[3] > (int)b[3] || (a[3] == b[3]
				   && (a[2] > b[2]
				       || (a[2] == b[2]
					   && (a[1] > b[1]
					       || (a[1] == b[1]
						   && a[0] > b[0])))));
}

extern int CvtConvertFloat();

STATIC_ROUTINE int GtrFloat(int dtype, void *a, void *b)
{
  int ans=0;
  float a_local;
  float b_local;
  if (CvtConvertFloat(a, dtype, &a_local, DTYPE_NATIVE_FLOAT, 0) &&
      CvtConvertFloat(b, dtype, &b_local, DTYPE_NATIVE_FLOAT, 0))
    ans = a_local > b_local;
  return ans;
}

STATIC_ROUTINE int GtrDouble(int dtype, void *a, void *b)
{
  int ans=0;
  double a_local;
  double b_local;
  if (CvtConvertFloat(a, dtype, &a_local, DTYPE_NATIVE_DOUBLE, 0) &&
      CvtConvertFloat(b, dtype, &b_local, DTYPE_NATIVE_DOUBLE, 0))
    ans = a_local > b_local;
  return ans;
}

#define NEQ_F NEQ_L
STATIC_ROUTINE int GTR_F(void *a, void *b)
{
  return GtrFloat(DTYPE_F, a, b);
}

#define NEQ_FS NEQ_L
STATIC_ROUTINE int GTR_FS(void *a, void *b)
{
  return GtrFloat(DTYPE_FS, a, b);
}

#define NEQ_D NEQ_Q
STATIC_ROUTINE int GTR_D(void *a, void *b)
{
  return GtrDouble(DTYPE_D, a, b);
}

#define NEQ_G NEQ_Q
STATIC_ROUTINE int GTR_G(void *a, void *b)
{
  return GtrDouble(DTYPE_G, a, b);
}

#define NEQ_FT NEQ_Q
STATIC_ROUTINE int GTR_FT(void *a, void *b)
{
  return GtrDouble(DTYPE_FT, a, b);
}

#define NEQ_FC NEQ_Q
STATIC_ROUTINE int GTR_FC(float *a, float *b)
{
  if (NEQ_F((int *)a, (int *)b))
    return GTR_F(&a[0], &b[0]);
  return GTR_F(&a[1], &b[1]);
}

#define NEQ_FSC NEQ_Q
STATIC_ROUTINE int GTR_FSC(float *a, float *b)
{
  if (NEQ_FS((int *)a, (int *)b))
    return GTR_FS(&a[0], &b[0]);
  return GTR_FS(&a[1], &b[1]);
}

#define NEQ_DC NEQ_O
STATIC_ROUTINE int GTR_DC(double *a, double *b)
{
  if (NEQ_D((unsigned int *)a, (unsigned int *)b))
    return GTR_D(&a[0], &b[0]);
  return GTR_D(&a[1], &b[1]);
}

#define NEQ_GC NEQ_O
STATIC_ROUTINE int GTR_GC(double *a, double *b)
{
  if (NEQ_G((unsigned int *)a, (unsigned int *)b))
    return GTR_G(&a[0], &b[0]);
  return GTR_G(&a[1], &b[1]);
}

#define NEQ_FTC NEQ_O
STATIC_ROUTINE int GTR_FTC(double *a, double *b)
{
  if (NEQ_FT((unsigned int *)a, (unsigned int *)b))
    return GTR_FT(&a[0], &b[0]);
  return GTR_FT(&a[1], &b[1]);
}

#define NEQ_T GTR_T
STATIC_ROUTINE int GTR_T(unsigned char *a, unsigned char *b, int len)
{
  register char *pa = (char *)a, *pb = (char *)b;
  register int n = len;
  for (; --n >= 0;)
    if (*pa++ != *pb++)
      break;
  return (*--pa - *--pb);
}

int Tdi1Bsearch(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  int inc, hi, lo = -2, mid, *poutput;
  int upcase = 0, cmode = -1, len, mode = 0, ni = 0, nt = 0;
  char *pinput, *ptable;
  int (*neq) () = 0, (*gtr) () = 0;
  struct descriptor_xd sig[2], uni[2], dat[2];
  struct TdiCatStruct cats[3];

  status = TdiGetArgs(opcode, 2, list, sig, uni, dat, cats);
  if (narg > 2 && STATUS_OK)
    status = TdiGetLong(list[2], &mode);
  if STATUS_OK
    status = Tdi2Bsearch(2, uni, dat, cats);
  if STATUS_OK
    status = TdiCvtArgs(2, dat, cats);
  if STATUS_OK {
    N_ELEMENTS(dat[0].pointer, ni);
  }
  if STATUS_OK {
    N_ELEMENTS(dat[1].pointer, nt);
  }
  if STATUS_OK
    status = TdiGetShape(1, &dat[0], sizeof(int), DTYPE_L, &cmode, out_ptr);
  if STATUS_OK {
    if (ni <= 0) {		/* null output */
    } else if (nt <= 0) {
      status = TdiNULL_PTR;	/* cannot lookup */
    } else {
      switch (dat[0].pointer->dtype) {
      case DTYPE_BU:
	neq = (int (*)())&NEQ_BU;
	gtr = (int (*)())&GTR_BU;
	break;
      case DTYPE_WU:
	neq = (int (*)())&NEQ_WU;
	gtr = (int (*)())&GTR_WU;
	break;
      case DTYPE_NID:
      case DTYPE_LU:
	neq = (int (*)())&NEQ_LU;
	gtr = (int (*)())&GTR_LU;
	break;
      case DTYPE_QU:
	neq = (int (*)())&NEQ_QU;
	gtr = (int (*)())&GTR_QU;
	break;
      case DTYPE_OU:
	neq = (int (*)())&NEQ_OU;
	gtr = (int (*)())&GTR_OU;
	break;

      case DTYPE_B:
	neq = (int (*)())&NEQ_B;
	gtr = (int (*)())&GTR_B;
	break;
      case DTYPE_W:
	neq = (int (*)())&NEQ_W;
	gtr = (int (*)())&GTR_W;
	break;
      case DTYPE_L:
	neq = (int (*)())&NEQ_L;
	gtr = (int (*)())&GTR_L;
	break;
      case DTYPE_Q:
	neq = (int (*)())&NEQ_Q;
	gtr = (int (*)())&GTR_Q;
	break;
      case DTYPE_O:
	neq = (int (*)())&NEQ_O;
	gtr = (int (*)())&GTR_O;
	break;

      case DTYPE_F:
	neq = (int (*)())&NEQ_F;
	gtr = (int (*)())&GTR_F;
	break;
      case DTYPE_FS:
	neq = (int (*)())&NEQ_FS;
	gtr = (int (*)())&GTR_FS;
	break;
      case DTYPE_D:
	neq = (int (*)())&NEQ_D;
	gtr = (int (*)())&GTR_D;
	break;
      case DTYPE_G:
	neq = (int (*)())&NEQ_G;
	gtr = (int (*)())&GTR_G;
	break;
      case DTYPE_FT:
	neq = (int (*)())&NEQ_FT;
	gtr = (int (*)())&GTR_FT;
	break;

      case DTYPE_FC:
	neq = (int (*)())&NEQ_FC;
	gtr = (int (*)())&GTR_FC;
	break;
      case DTYPE_FSC:
	neq = (int (*)())&NEQ_FSC;
	gtr = (int (*)())&GTR_FSC;
	break;
      case DTYPE_DC:
	neq = (int (*)())&NEQ_DC;
	gtr = (int (*)())&GTR_DC;
	break;
      case DTYPE_GC:
	neq = (int (*)())&NEQ_GC;
	gtr = (int (*)())&GTR_GC;
	break;
      case DTYPE_FTC:
	neq = (int (*)())&NEQ_FTC;
	gtr = (int (*)())&GTR_FTC;
	break;

      case DTYPE_PATH:
      case DTYPE_EVENT:
      case DTYPE_T:
	neq = (int (*)())&NEQ_T;
	gtr = (int (*)())&GTR_T;
	if (narg > 3)
	  status = TdiGetLong(list[3], &upcase);
	if (upcase & 1) {
	  if STATUS_OK
	    status = StrUpcase(dat[0].pointer, dat[0].pointer);
	  if STATUS_OK
	    status = StrUpcase(dat[1].pointer, dat[1].pointer);
	}
	break;
      default:
	if (ni > 0)
	  status = TdiINVDTYDSC;
	break;
      }
    }
  }
  if STATUS_OK {
    len = dat[0].pointer->length;
    pinput = dat[0].pointer->pointer;
    ptable = dat[1].pointer->pointer;
    poutput = (int *)out_ptr->pointer->pointer;
    for (; --ni >= 0; pinput += len) {
			/**********************************
                        Out of bounds does straight search.
                        **********************************/
      if (lo < -1 || lo >= nt) {
	lo = -1;
	hi = nt;
      }
			/*********************************
                        Expand search from previous point.
                        Adjust upper limit above previous.
                        *********************************/
      else if (lo < 0 || (*gtr) (ptable + len * lo, pinput, len) <= 0) {
	inc = 1;
	while ((hi = lo + inc) < nt && (*gtr) (ptable + len * hi, pinput, len) <= 0) {
	  lo = hi;
	  inc <<= 1;
	}
	if (hi > nt)
	  hi = nt;
      }
			/*********************************
                        Adjust lower limit below previous.
                        *********************************/
      else {
	inc = 1;
	while (hi = lo, (lo -= inc) >= 0 && (*gtr) (ptable + len * lo, pinput, len) > 0)
	  inc <<= 1;
	if (lo < 0)
	  lo = -1;
      }
			/*****************
                        The binary search.
                        *****************/
      while (hi - lo > 1) {
	mid = (hi + lo) >> 1;
	if ((*gtr) (ptable + len * mid, pinput, len) <= 0)
	  lo = mid;
	else
	  hi = mid;
      }
			/**************
                        Equality check.
                        **************/
      if (mode > 0 || (lo >= 0 && (*neq) (pinput, ptable + len * lo, len) == 0))
	*poutput++ = lo;
      else if (mode == 0)
	*poutput++ = -1;
      else
	*poutput++ = lo + 1;
    }
  }
  if (sig[1].pointer)
    MdsFree1Dx(&sig[1], NULL);
  if (uni[1].pointer)
    MdsFree1Dx(&uni[1], NULL);
  if (uni[0].pointer)
    MdsFree1Dx(&uni[0], NULL);
  MdsFree1Dx(&dat[1], NULL);
  MdsFree1Dx(&dat[0], NULL);
  if STATUS_OK
    status = TdiMasterData(1, sig, uni, &cmode, out_ptr);
  if (sig[0].pointer)
    MdsFree1Dx(&sig[0], NULL);
  return status;
}

/*--------------------------------------------------------------
        list = SORT(array, upcase)

        array[list[j]] <= array[list[j+1]]
        Same signality, no units.

        Hoare Quicksort with randomized partition.
        Fastest sorting on the average, normally n*log2(n), exceptional case is n*n.
        Method: "Numerical Recipes" pp.235-6 based on Knuth Vol.3.
        Limitation: internal stack limit is 2**32 elements.
        Limitation: does not preserve order of equal values, no n*log2(n) does, I think.
        It could be done by sorting indices of equal value elements.
*/
int Tdi1Sort(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  STATIC_THREADSAFE int64_t ran = 0;
  int i, j, keep, l, r, jstack, *ndx;
  int upcase = 0, cmode = -1, len, n = 0;
  int (*gtr) () = 0;
  char *pinput, *pkeep;
  struct descriptor_xd sig[1], uni[1], dat[1];
  struct TdiCatStruct cats[2];
  int stack[64];

  status = TdiGetArgs(opcode, 1, list, sig, uni, dat, cats);
  if (uni[0].pointer)
    MdsFree1Dx(&uni[0], NULL);
  if STATUS_OK
    switch (dat[0].pointer->dtype) {
    case DTYPE_BU:
      gtr = (int (*)())&GTR_BU;
      break;
    case DTYPE_WU:
      gtr = (int (*)())&GTR_WU;
      break;
    case DTYPE_LU:
      gtr = (int (*)())&GTR_LU;
      break;
    case DTYPE_QU:
      gtr = (int (*)())&GTR_QU;
      break;
    case DTYPE_OU:
      gtr = (int (*)())&GTR_OU;
      break;

    case DTYPE_B:
      gtr = (int (*)())&GTR_B;
      break;
    case DTYPE_W:
      gtr = (int (*)())&GTR_W;
      break;
    case DTYPE_L:
      gtr = (int (*)())&GTR_L;
      break;
    case DTYPE_Q:
      gtr = (int (*)())&GTR_Q;
      break;
    case DTYPE_O:
      gtr = (int (*)())&GTR_O;
      break;

    case DTYPE_F:
      gtr = (int (*)())&GTR_F;
      break;
    case DTYPE_FS:
      gtr = (int (*)())&GTR_FS;
      break;
    case DTYPE_D:
      gtr = (int (*)())&GTR_D;
      break;
    case DTYPE_G:
      gtr = (int (*)())&GTR_G;
      break;
    case DTYPE_FT:
      gtr = (int (*)())&GTR_FT;
      break;

    case DTYPE_FC:
      gtr = (int (*)())&GTR_FC;
      break;
    case DTYPE_DC:
      gtr = (int (*)())&GTR_DC;
      break;
    case DTYPE_GC:
      gtr = (int (*)())&GTR_GC;
      break;
    case DTYPE_FTC:
      gtr = (int (*)())&GTR_FTC;
      break;

    case DTYPE_T:
      gtr = (int (*)())&GTR_T;
      if (narg > 1)
	status = TdiGetLong(list[1], &upcase);
      if (STATUS_OK && IS_OK(upcase))
	status = StrUpcase(dat[0].pointer, dat[0].pointer);
      break;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  if STATUS_OK
    status = TdiGetShape(1, &dat[0], sizeof(int), DTYPE_L, &cmode, out_ptr);
  if STATUS_OK {
    N_ELEMENTS(dat[0].pointer, n);
  }
  if (STATUS_NOT_OK || n <= 0)
    goto done;

  len = dat[0].pointer->length;
  pinput = dat[0].pointer->pointer;
  ndx = (int *)out_ptr->pointer->pointer;
	/******************
        Initialize indices.
        ******************/
  jstack = 0;
  l = 0;
  r = n - 1;
  for (i = 0; i < n; ++i)
    *ndx++ = i;
  ndx = (int *)out_ptr->pointer->pointer;
  for (;;) {
		/*********************************************
                Insertion sort for small ranges. Say, 7 to 13.
                *********************************************/
    while (r - l < 11) {
      for (j = l; ++j <= r;) {
	keep = *(ndx + j);
	pkeep = pinput + len * keep;
	for (i = j; --i >= l && (*gtr) (pinput + len * *(ndx + i), pkeep, len) > 0;)
	  *(ndx + i + 1) = *(ndx + i);
	*(ndx + i + 1) = keep;
      }
			/**************************
                        New region to sort, if any.
                        Two-level breakout.
                        **************************/
      if (jstack <= 0)
	goto done;
      r = stack[--jstack];
      l = stack[--jstack];
    }
		/***********************************
                Randomly select the partition value.
                Choose an index between l and r.
                ***********************************/
    ran = (ran * 211 + 1663) % 7875;
    i = (int)(l + (r - l + 1) * ran / 7875);
    keep = *(ndx + i);
    pkeep = pinput + len * keep;
    *(ndx + i) = *(ndx + l);
    i = l;
    j = r;
		/************************************************
                Those at higher location with bigger value stick.
                Those at lower location with lower value stick.
                Stuff misfits in opposite side.
                ************************************************/
    for (;;) {
      while (i < j && (*gtr) (pinput + len * *(ndx + j), pkeep, len) > 0)
	--j;
      if (i == j)
	break;
      *(ndx + i) = *(ndx + j);
      ++i;
      while (i < j && (*gtr) (pkeep, pinput + len * *(ndx + i), len) > 0)
	++i;
      if (i == j)
	break;
      *(ndx + j) = *(ndx + i);
      --j;
    }
    *(ndx + i) = keep;
		/**********************************************************
                With i as breakpoint, put larger block on stack for latter.
                **********************************************************/
    if (i - l <= r - i) {
      stack[jstack++] = i + 1;
      stack[jstack++] = r;
      r = i - 1;
    } else {
      stack[jstack++] = l;
      stack[jstack++] = i - 1;
      l = i + 1;
    }
  }				/*for(;;) */
 done:
  if (uni[0].pointer)
    MdsFree1Dx(&uni[0], NULL);
  MdsFree1Dx(&dat[0], NULL);
  if (sig[0].pointer) {
    if STATUS_OK
      status = TdiMasterData(1, &sig[0], uni, &cmode, out_ptr);
    MdsFree1Dx(&sig[0], NULL);
  }
  return status;
}

/*--------------------------------------------------------------
        new = SORTVAL(array, [upcase])

        Sort in place.
        Method: MAP(array, SORT(array, [upcase]))
*/
int Tdi1SortVal(int opcode __attribute__ ((unused)), int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;

  status = TdiEvaluate(list[0], &tmp MDS_END_ARG);
  if STATUS_OK
    status = TdiSort(tmp.pointer, narg > 1 ? list[1] : 0, out_ptr MDS_END_ARG);
  if STATUS_OK
    status = TdiMap(&tmp, out_ptr, out_ptr MDS_END_ARG);
  if (tmp.pointer)
    MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Form unduplicated union of argument sets.
        There may any number or arguments.
        The signality is removed and units are joined by VECTOR.
*/
int Tdi1Union(int opcode __attribute__ ((unused)), int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  int j, n, len;
  char *pi, *po;

  status = TdiIntrinsic(OpcVector, narg, list, out_ptr);
  if STATUS_OK
    status = TdiSortVal(out_ptr, out_ptr MDS_END_ARG);
  if STATUS_OK {
    struct descriptor_with_units *pdwu = (struct descriptor_with_units *)out_ptr->pointer;
    struct descriptor_a *pdo =
	pdwu->dtype ==
	DTYPE_WITH_UNITS ? (struct descriptor_a *)pdwu->data : (struct descriptor_a *)pdwu;
    char *cptri, *cptro;
    short *sptri, *sptro;
    int *lptri, *lptro;
    if (pdo->class != CLASS_A)
      return status;
    len = pdo->length;
    po = pi = pdo->pointer;
    n = (int)pdo->arsize / (int)pdo->length;
    switch (len) {
    case 1:
      for (cptri = (char *)pi, cptro = (char *)po; --n > 0;)
	if (*++cptri != *cptro)
	  *++cptro = *cptri;
      po = (char *)cptro;
      break;
    case 2:
      for (sptri = (short *)pi, sptro = (short *)po; --n > 0;)
	if (*++sptri != *sptro)
	  *++sptro = *sptri;
      po = (char *)sptro;
      break;
    case 4:
      for (lptri = (int *)pi, lptro = (int *)po; --n > 0;)
	if (*++lptri != *lptro)
	  *++lptro = *lptri;
      po = (char *)lptro;
      break;
    default:
      switch (len & 3) {
      default:
	for (; --n > 0;) {
	  pi += len;
	  for (j = 0; j < len; j += 1)
	    if (*(char *)(pi + j) != *(char *)(po + j))
	      break;
	  if (j < len) {
	    po += len;
	    _MOVC3(len, pi, po);
	  }
	}
	break;
      case 2:
	for (; --n > 0;) {
	  pi += len;
	  for (j = 0; j < len; j += 2)
	    if (*(short *)(pi + j) != *(short *)(po + j))
	      break;
	  if (j < len) {
	    po += len;
	    _MOVC3(len, pi, po);
	  }
	}
	break;
      case 0:
	for (; --n > 0;) {
	  pi += len;
	  for (j = 0; j < len; j += 4)
	    if (*(int *)(pi + j) != *(int *)(po + j))
	      break;
	  if (j < len) {
	    po += len;
	    _MOVC3(len, pi, po);
	  }
	}
	break;
      }
      break;
    }
    pdo->arsize = (po - pdo->pointer) + len;
    pdo->aflags.coeff = 0;
    pdo->aflags.bounds = 0;
    pdo->dimct = 1;
  }
  return status;
}

/*--------------------------------------------------------------
        logical = element IS_IN list
        logical = IS_IN(element, list, [upcase])

        element scalar or array of items to test
        list    vector of valid values
        upcase  compare in uppercase (for text only)
*/
int Tdi1IsIn(int opcode __attribute__ ((unused)), int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor *pupcase = narg > 2 ? list[2] : 0;

  status = TdiSortVal(list[1], pupcase, out_ptr MDS_END_ARG);
  if STATUS_OK
    status = TdiBsearch(list[0], out_ptr, 0, pupcase, out_ptr MDS_END_ARG);
  if STATUS_OK
    status = TdiGe(out_ptr, 0, out_ptr MDS_END_ARG);
  return status;
}
