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
/*  DEC/CMS REPLACEMENT HISTORY, Element IIR.C */
/*  *18   15-MAR-1995 11:21:11 MDSPLUS "Compatibility" */
/*  *17   14-MAR-1995 17:04:48 MDSPLUS "ANSI C" */
/*  *16   13-MAR-1995 17:32:50 MDSPLUS "New Version" */
/*  *15   16-NOV-1994 12:44:35 MDSPLUS "OSF/1 compatibile" */
/*  *14   15-NOV-1994 16:32:32 MDSPLUS "Ported to OSF1" */
/*  *13   15-NOV-1994 15:34:12 MDSPLUS "Move to OSF1" */
/*  *12   10-NOV-1994 16:44:57 MDSPLUS "Proceede" */
/*  *11   10-NOV-1994 16:30:13 MDSPLUS "Proceede" */
/*  *10   10-NOV-1994 16:02:46 MDSPLUS "Proceede" */
/*  *9    10-NOV-1994 15:57:33 MDSPLUS "Proceede" */
/*  *8    10-NOV-1994 15:48:32 MDSPLUS "Proceede" */
/*  *7    10-NOV-1994 15:25:44 MDSPLUS "Proceede" */
/*  *6     9-NOV-1994 14:34:48 MDSPLUS "Proceede" */
/*  *5     8-NOV-1994 18:42:26 MDSPLUS "Proceede" */
/*  *4     8-NOV-1994 18:29:13 MDSPLUS "Proceede" */
/*  *3     8-NOV-1994 18:21:58 MDSPLUS "Proceede" */
/*  *2     7-NOV-1994 17:32:29 MDSPLUS "Proceede" */
/*  *1     7-NOV-1994 11:54:23 MDSPLUS "IIR management routines" */
/*  DEC/CMS REPLACEMENT HISTORY, Element IIR.C */
/*------------------------------------------------------------------------------

	Name:	IIR   

	Type:   C function

	Author:	Gabriele Manduchi
		Istituto Gas Ionizzati del CNR - padova (Italy)

	Date:    7-NOV-1994

	Purpose: IIR managenent routines.

--------------------------------------------------------------------------------

 Description:
    The digital filter is obtained from the analog filter using the following methods:

1) Impulse Invariance: Filter *ButtwInvar(double fp, double fs, double ap, double as, double fc, int *out_n)

2) Bilinear transformation: Filter *ButtwInvar(double fp, double fs, double ap, double as, double fc, int *out_n)

thet parameters for the three routines are:
fp: bandwidth (Hz)
fp - fs: transition band
ap: low frequency attenuation (f < fp)  (dB)
as: high frequency attenuation (f > fs) (dB)
fc: sampling frequency
n: output parameter: the order N of the analog butterworth filter to achieve
    desired performance

they return an internal structure which is then used by common routine Filter
which actually perform digital filtering.

------------------------------------------------------------------------------*/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "complex.h"
#include "filter.h"
#include <mdsplus/mdsconfig.h>

EXPORT Complex *FindFactors(Complex * poles, double fc, int n, double gain __attribute__ ((unused)))
{
  int i, j;
  double currA;
  Complex *Ak;
/*
    double A;
    if(n % 2)
	for(A = -poles[0].re/fc, i = 1; i < n; i+=2)
	    A *= poles[i].re * poles[i].re + poles[i].im * poles[i].im;
    else
	for(A = 1/fc, i = 0; i < n; i+=2)
	    A *= poles[i].re * poles[i].re + poles[i].im * poles[i].im;
*/

/* Ak :denominators of single factors */
  Ak = (Complex *) malloc(2 * n * sizeof(Complex));

  for (i = 0; i < n; i++) {
    Ak[i].re = 1;
    Ak[i].im = 0;
    for (j = 0; j < n; j++) {
      if (j != i) {
	Ak[i] = MulC(Ak[i], SubC(poles[i], poles[j]));
      }

      if ((n % 2 && (j % 2 || j == 0)) || (!(n % 2) && !(j % 2))) {
	if (j == 0 && n % 2)
	  currA = -poles[j].re;
	else
	  currA = poles[j].re * poles[j].re + poles[j].im * poles[j].im;

	Ak[i].re /= currA;
	Ak[i].im /= currA;
      }
    }
    Ak[i].re *= fc;
    Ak[i].im *= fc;

  }
  return Ak;
}

EXPORT Filter *Invariant(double fp, double fs, double ap, double as, double fc, int *out_n,
		  Complex * (*FindPoles) ())
{
  Complex *poles, *Ak, cplx1, cplx2;
  Filter *filter;
  int n, i;
  double T, Ws, Wp, gain;
  Complex Tc;

  Ws = 2 * PI * fs;
  Wp = 2 * PI * fp;
  T = 1 / fc;
  Tc.re = T;
  Tc.im = 0;
  poles = FindPoles(Wp, Ws, ap, as, fc, out_n, &gain);
  n = *out_n;
  Ak = FindFactors(poles, fc, n, gain);

  filter = (Filter *) malloc(sizeof(Filter));
/* Allocate storage for output filter */
  if (n % 2) {
/* If n is odd there is a real pole */
    filter->num_parallels = (n + 1) / 2;
    filter->units = (FilterUnit *) malloc(sizeof(FilterUnit) * (n + 1) / 2);
    filter->units[0].num_degree = 1;
    filter->units[0].num = (double *)malloc(sizeof(double));
    filter->units[0].den_degree = 2;
    filter->units[0].den = (double *)malloc(sizeof(double) * 2);
    for (i = 1; i < (n + 1) / 2; i++) {	/* For each pair of coniugate poles */
      filter->units[i].num_degree = 2;
      filter->units[i].num = (double *)malloc(sizeof(double) * 2);
      filter->units[i].den_degree = 3;
      filter->units[i].den = (double *)malloc(sizeof(double) * 3);
    }
  } else
/* In n is even there are only pairs of coniugate poles */
  {
    filter->num_parallels = n / 2;
    filter->units = (FilterUnit *) malloc(sizeof(FilterUnit) * n / 2);
    for (i = 0; i < n / 2; i++) {	/* For each pair of coniugate poles */
      filter->units[i].num_degree = 2;
      filter->units[i].num = (double *)malloc(sizeof(double) * 2);
      filter->units[i].den_degree = 3;
      filter->units[i].den = (double *)malloc(sizeof(double) * 3);
    }
  }

/* Compute filter coefficients */
  if (n % 2) {
    filter->units[0].num[0] = 1 / Ak[0].re;
    filter->units[0].den[0] = 1;
    filter->units[0].den[1] = -exp(poles[0].re * T);
    for (i = 1; i < (n + 1) / 2; i++) {
      cplx1 = MulC(Ak[2 * i], Ak[2 * i - 1]);
      cplx2 = AddC(Ak[2 * i], Ak[2 * i - 1]);
      if (fabs(cplx1.im) > 1E-4 || fabs(cplx2.im) > 1E-4) {	/* Must be real */
	printf("\nInternal error in Invariant conversion");
      }
      filter->units[i].num[0] = cplx2.re / cplx1.re;
      cplx2 =
	  AddC(MulC(Ak[2 * i], ExpC(MulC(Tc, poles[2 * i]))),
	       MulC(Ak[2 * i - 1], ExpC(MulC(Tc, poles[2 * i - 1]))));
      if (fabs(cplx2.im) > 1E-4) {	/* Must be real */
	printf("\nInternal error in Invariant conversion");
      }
      filter->units[i].num[1] = -cplx2.re / cplx1.re;

      filter->units[i].den[0] = 1;
      filter->units[i].den[1] = -2 * exp(poles[2 * i].re * T) * cos(poles[2 * i].im * T);
      filter->units[i].den[2] = exp(2 * poles[2 * i].re * T);
    }
  } else			/* n even */
    for (i = 0; i < n / 2; i++) {
      cplx1 = MulC(Ak[2 * i], Ak[2 * i + 1]);
      cplx2 = AddC(Ak[2 * i], Ak[2 * i + 1]);
      if (fabs(cplx1.im) > 1E-4 || fabs(cplx2.im) > 1E-4) {	/* Must be real */
	printf("\nInternal error in Invariant conversion");
      }
      filter->units[i].num[0] = cplx2.re / cplx1.re;
      cplx2 =
	  AddC(MulC(Ak[2 * i], ExpC(MulC(Tc, poles[2 * i]))),
	       MulC(Ak[2 * i + 1], ExpC(MulC(Tc, poles[2 * i + 1]))));
      if (fabs(cplx2.im) > 1E-4) {	/* Must be real */
	printf("\nInternal error in Invariant conversion");
      }
      filter->units[i].num[1] = -cplx2.re / cplx1.re;

      filter->units[i].den[0] = 1;
      filter->units[i].den[1] = -2 * exp(poles[2 * i].re * T) * cos(poles[2 * i].im * T);
      filter->units[i].den[2] = exp(2 * poles[2 * i].re * T);
    }
  free((char *)poles);
  free((char *)Ak);
  return filter;
}

EXPORT Filter *Bilinear(double fp, double fs, double ap, double as, double fc, int *out_n,
		 Complex * (*FindPoles) ())
{
  Complex *poles, *Ak, cplx;
  Filter *filter;
  int n, i;
  double T, T2, Ws, Wp, ws, wp, gain;
  Complex ac, a1c, bc, b1c, Ac, A1c, T2c;

/* Pre distorsion */
  wp = 2 * PI * fp / fc;
  ws = 2 * PI * fs / fc;

  Wp = 2 * tan(wp / 2) * fc;
  Ws = 2 * tan(ws / 2) * fc;

  T = 1 / fc;
  T2 = T / 2;

  poles = FindPoles(Wp, Ws, ap, as, fc, out_n, &gain);
  n = *out_n;
  Ak = FindFactors(poles, fc, n, gain);	/* Remind that 1/Ak are normalized such that H(0) = T */
  for (i = 0; i < n; i++) {
    Ak[i].re *= T;
    Ak[i].im *= T;
  }
  filter = (Filter *) malloc(sizeof(Filter));
/* Allocate storage for output filter */
  if (n % 2) {
/* If n is odd there is a real pole */
    filter->num_parallels = (n + 1) / 2;
    filter->units = (FilterUnit *) malloc(sizeof(FilterUnit) * (n + 1) / 2);
    filter->units[0].num_degree = 2;
    filter->units[0].num = (double *)malloc(sizeof(double) * 2);
    filter->units[0].den_degree = 2;
    filter->units[0].den = (double *)malloc(sizeof(double) * 2);
    for (i = 1; i < (n + 1) / 2; i++) {	/* For each pair of coniugate poles */
      filter->units[i].num_degree = 3;
      filter->units[i].num = (double *)malloc(sizeof(double) * 3);
      filter->units[i].den_degree = 3;
      filter->units[i].den = (double *)malloc(sizeof(double) * 3);
    }
  } else
/* In n is even there are only pairs of coniugate poles */
  {
    filter->num_parallels = n / 2;
    filter->units = (FilterUnit *) malloc(sizeof(FilterUnit) * n / 2);
    for (i = 0; i < n / 2; i++) {	/* For each pair of coniugate poles */
      filter->units[i].num_degree = 3;
      filter->units[i].num = (double *)malloc(sizeof(double) * 3);
      filter->units[i].den_degree = 3;
      filter->units[i].den = (double *)malloc(sizeof(double) * 3);
    }
  }

/* Compute filter coefficients */
  if (n % 2) {
    filter->units[0].num[0] = filter->units[0].num[1] = T2 / Ak[0].re;
    filter->units[0].den[0] = 1 - poles[0].re * T2;
    filter->units[0].den[1] = -1 - poles[0].re * T2;
    T2c.re = T2;
    T2c.im = 0;
    for (i = 1; i < (n + 1) / 2; i++) {
      Ac = DivC(T2c, Ak[2 * i]);
      A1c = DivC(T2c, Ak[2 * i - 1]);
      ac.re = 1 - poles[2 * i].re * T2;
      ac.im = -poles[2 * i].im * T2;
      a1c.re = 1 - poles[2 * i - 1].re * T2;
      a1c.im = -poles[2 * i - 1].im * T2;
      bc.re = 1 + poles[2 * i].re * T2;
      bc.im = poles[2 * i].im * T2;
      b1c.re = 1 + poles[2 * i - 1].re * T2;
      b1c.im = poles[2 * i - 1].im * T2;
      cplx = AddC(MulC(A1c, ac), MulC(Ac, a1c));
      if (fabs(cplx.im) > 1E-4) {
	printf("\nInternal error in Bilinear conversion");
      }
      filter->units[i].num[0] = cplx.re;

      cplx = SubC(AddC(MulC(A1c, ac), MulC(Ac, a1c)), AddC(MulC(A1c, bc), MulC(Ac, b1c)));
      if (fabs(cplx.im) > 1E-4) {
	printf("\nInternal error in Bilinear conversion");
      }
      filter->units[i].num[1] = cplx.re;

      cplx = AddC(MulC(A1c, bc), MulC(Ac, b1c));
      if (fabs(cplx.im) > 1E-4) {
	printf("\nInternal error in Bilinear conversion");
      }
      filter->units[i].num[2] = -cplx.re;

      cplx = MulC(ac, a1c);
      if (fabs(cplx.im) > 1E-4) {
	printf("\nInternal error in Bilinear conversion");
      }
      filter->units[i].den[0] = cplx.re;

      cplx = AddC(MulC(a1c, bc), MulC(b1c, ac));
      if (fabs(cplx.im) > 1E-4) {
	printf("\nInternal error in Bilinear conversion");
      }
      filter->units[i].den[1] = -cplx.re;

      cplx = MulC(bc, b1c);
      if (fabs(cplx.im) > 1E-4) {
	printf("\nInternal error in Bilinear conversion");
      }
      filter->units[i].den[2] = cplx.re;
    }
  } else {			/* n even */

    T2c.re = T2;
    T2c.im = 0;
    for (i = 0; i < n / 2; i++) {
      Ac = DivC(T2c, Ak[2 * i]);
      A1c = DivC(T2c, Ak[2 * i + 1]);
      ac.re = 1 - poles[2 * i].re * T2;
      ac.im = -poles[2 * i].im * T2;
      a1c.re = 1 - poles[2 * i + 1].re * T2;
      a1c.im = -poles[2 * i + 1].im * T2;
      bc.re = 1 + poles[2 * i].re * T2;
      bc.im = poles[2 * i].im * T2;
      b1c.re = 1 + poles[2 * i + 1].re * T2;
      b1c.im = poles[2 * i + 1].im * T2;
      cplx = AddC(MulC(A1c, ac), MulC(Ac, a1c));
      if (fabs(cplx.im) > 1E-5) {
	printf("\nInternal error in Bilinear conversion");
      }
      filter->units[i].num[0] = cplx.re;

      cplx = SubC(AddC(MulC(A1c, ac), MulC(Ac, a1c)), AddC(MulC(A1c, bc), MulC(Ac, b1c)));
      if (fabs(cplx.im) > 1E-5) {
	printf("\nInternal error in Bilinear conversion");
      }
      filter->units[i].num[1] = cplx.re;

      cplx = AddC(MulC(A1c, bc), MulC(Ac, b1c));
      if (fabs(cplx.im) > 1E-5) {
	printf("\nInternal error in Bilinear conversion");
      }
      filter->units[i].num[2] = -cplx.re;

      cplx = MulC(ac, a1c);
      if (fabs(cplx.im) > 1E-5) {
	printf("\nInternal error in Bilinear conversion");
      }
      filter->units[i].den[0] = cplx.re;

      cplx = AddC(MulC(a1c, bc), MulC(b1c, ac));
      if (fabs(cplx.im) > 1E-5) {
	printf("\nInternal error in Bilinear conversion");
      }
      filter->units[i].den[1] = -cplx.re;

      cplx = MulC(bc, b1c);
      if (fabs(cplx.im) > 1E-5) {
	printf("\nInternal error in Bilinear conversion");
      }
      filter->units[i].den[2] = cplx.re;
    }
  }
  free((char *)poles);
  free((char *)Ak);
  return filter;
}
