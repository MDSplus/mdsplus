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
/*      Tdi3Power.C
        Raise number to a power (integral, real, or complex).

        WARNING can destroy inputs, which must differ from output.
        WARNING 0^0 is undefined, for r^r and c^c ROPRAND, for i^i 1. (OTS$POWJJ gives 0.)
        WARNING accuracy of float base not as good as OTS$ routines, which use double.
        Ken Klare, LANL P-4     (c)1990,1991
*/
#include <STATICdef.h>
#include <string.h>
#define _MOVC3(a,b,c) memcpy(c,b,a)
#include <mdsdescrip.h>
#include "tdinelements.h"
#include <tdishr_messages.h>



STATIC_CONSTANT int one = 1;
STATIC_CONSTANT struct descriptor one_dsc = { sizeof(one), DTYPE_L, CLASS_S, (char *)&one };

extern int Tdi3Log();
extern int TdiConvert();
extern int Tdi3Multiply();
extern int Tdi3Exp();
extern int Tdi3Divide();

int Tdi3Power(struct descriptor *x, struct descriptor *y, struct descriptor_a *z)
{
  INIT_STATUS;
  int yy;
  char uno[32];
  struct descriptor duno;

	/*******************************************
        For real/complex exponent, z = EXP(LOG(x)*y)
        Need HC routines, not available on VAX.
        *******************************************/
  if (y->dtype != DTYPE_L) {
    if (x->class != CLASS_A && z->class == CLASS_A) {
      status = TdiConvert(x, z);
      status = Tdi3Log(z, z);
    } else
      status = Tdi3Log(x, z);
    if STATUS_OK
      status = Tdi3Multiply(y, z, z);
    if STATUS_OK
      status = Tdi3Exp(z, z);
  }
	/******************************************
        Scalar integer exponent, look once at bits.
        Clobbers x and x&z must be different.
        ******************************************/
  else if (y->class != CLASS_A) {
    if ((yy = *(int *)y->pointer) <= 0) {
      if (yy == 0)
	return TdiConvert(&one_dsc, z);
      yy = -yy;
      duno = *x;
      duno.pointer = uno;
      status = TdiConvert(&one_dsc, &duno);
      if STATUS_OK
	status = Tdi3Divide(&duno, x, x);
    }
    for (; !(yy & 1) && STATUS_OK; yy >>= 1)
      status = Tdi3Multiply(x, x, x);
    if (x->class != CLASS_A)
      _MOVC3(z->length, x->pointer, z->pointer);
    else
      _MOVC3(z->arsize, x->pointer, z->pointer);
    for (; (yy >>= 1) > 0 && STATUS_OK;) {
      status = Tdi3Multiply(x, x, x);
      if (yy & 1 && STATUS_OK)
	status = Tdi3Multiply(x, z, z);
    }
  }
	/************************
        Must do it the hard way.
        Copy to keep scalar base.
        ************************/
  else {
    char xx[32];
    struct descriptor dx = { 0, 0, CLASS_S, 0 };
    char *px = x->pointer;
    int incx = x->class == CLASS_A ? x->length : 0;
    int *py = (int *)y->pointer;
    struct descriptor dz = *(struct descriptor *)z;
    int incz = dz.length;
    int n;

    dx.length = x->length;
    dx.dtype = x->dtype;
    dx.pointer = xx;
    N_ELEMENTS(z, n);
    dz.class = CLASS_S;
    duno = dx;
    duno.pointer = uno;
    if STATUS_OK
      status = TdiConvert(&one_dsc, &duno);
    for (; --n >= 0 && STATUS_OK; px += incx, dz.pointer += incz) {
      _MOVC3(dx.length, px, xx);
      if ((int)(yy = *py++) <= 0) {
	if (yy == 0) {
	  _MOVC3(dz.length, uno, dz.pointer);
	  continue;
	}
	yy = -yy;
	status = Tdi3Divide(&duno, &dx, &dx);
      }
      for (; !(yy & 1) && STATUS_OK; yy >>= 1)
	status = Tdi3Multiply(&dx, &dx, &dx);
      _MOVC3(dz.length, dx.pointer, dz.pointer);
      for (; (yy >>= 1) > 0 && STATUS_OK;) {
	status = Tdi3Multiply(&dx, &dx, &dx);
	if (yy & 1 && STATUS_OK)
	  status = Tdi3Multiply(&dx, &dz, &dz);
      }
    }
  }
  return status;
}

/*-------------------------------------------------------------
        Tdi3Merge.C
        Assign value from either true or false source according to mask.
*/
typedef struct {
  int q0, q1;
} lquad;

int Tdi3Merge(struct descriptor_a *pdtrue, struct descriptor_a *pdfalse,
	      struct descriptor_a *pdmask, struct descriptor_a *pdout)
{
  INIT_STATUS;
  int len = pdout->length;
  char *po = pdout->pointer;
  int n = 0;

  int stept = pdtrue->class == CLASS_A ? len : 0;
  char *pt = pdtrue->pointer;
  int stepf = pdfalse->class == CLASS_A ? len : 0;
  char *pf = pdfalse->pointer;

  int stepm = pdmask->class == CLASS_A ? pdmask->length : 0;
#ifdef WORDS_BIGENDIAN
  char *pm = pdmask->pointer + pdmask->length - 1;
#else
  char *pm = pdmask->pointer;
#endif
  char *pc;
  short *ps;
  int *pl;
  lquad *pq;

  N_ELEMENTS(pdout, n);
  if STATUS_OK
    switch (len) {
    case 1:
      for (pc = (char *)po; --n >= 0; pm += stepm, pt += stept, pf += stepf)
	if (*pm & 1)
	  *pc++ = *(char *)pt;
	else
	  *pc++ = *(char *)pf;
      break;
    case 2:
      for (ps = (short *)po; --n >= 0; pm += stepm, pt += stept, pf += stepf)
	if (*pm & 1)
	  *ps++ = *(short *)pt;
	else
	  *ps++ = *(short *)pf;
      break;
    case 4:
      for (pl = (int *)po; --n >= 0; pm += stepm, pt += stept, pf += stepf)
	if (*pm & 1)
	  *pl++ = *(int *)pt;
	else
	  *pl++ = *(int *)pf;
      break;
    case 8:
      for (pq = (lquad *) po; --n >= 0; pm += stepm, pt += stept, pf += stepf)
	if (*pm & 1)
	  *pq++ = *(lquad *) pt;
	else
	  *pq++ = *(lquad *) pf;
      break;
    default:
      {
	int sm = stepm, st = stept, sf = stepf;
	for (; --n >= 0; pm += sm, pt += st, pf += sf, po += len)
	  if (*pm & 1)
	    _MOVC3(len, pt, po);
	  else
	    _MOVC3(len, pf, po);
      }
      break;
    }
  return status;
}
