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
/*  DEC/CMS REPLACEMENT HISTORY, Element BUTTERWORTH.C */
/*  *63   15-MAR-1995 11:19:49 MDSPLUS "Compatibility" */
/*  *62   14-MAR-1995 16:58:35 MDSPLUS "ANSI C" */
/*  *61   13-MAR-1995 17:31:59 MDSPLUS "New Version" */
/*  *60   16-NOV-1994 12:44:12 MDSPLUS "OSF/1 compatibile" */
/*  *59   15-NOV-1994 16:32:04 MDSPLUS "Ported to OSF1" */
/*  *58   15-NOV-1994 14:59:23 MDSPLUS "Move to OSF1" */
/*  *57   11-NOV-1994 13:39:58 MDSPLUS "Proceede" */
/*  *56   10-NOV-1994 18:17:59 MDSPLUS "Proceede" */
/*  *55   10-NOV-1994 17:03:10 MDSPLUS "Proceede" */
/*  *54    7-NOV-1994 11:52:45 MDSPLUS "Proceede" */
/*  *53    3-NOV-1994 10:20:15 MDSPLUS "Proceede" */
/*  *52    2-NOV-1994 17:33:23 MDSPLUS "Proceede" */
/*  *51    2-NOV-1994 10:39:24 MDSPLUS "Proceede" */
/*  *50    2-NOV-1994 10:17:22 MDSPLUS "Proceede" */
/*  *49    2-NOV-1994 10:13:16 MDSPLUS "Proceede" */
/*  *48    2-NOV-1994 09:52:26 MDSPLUS "Proceede" */
/*  *47   28-OCT-1994 19:19:39 MDSPLUS "Proceede" */
/*  *46   28-OCT-1994 19:12:18 MDSPLUS "Proceede" */
/*  *45   28-OCT-1994 19:02:41 MDSPLUS "Proceede" */
/*  *44   28-OCT-1994 18:29:05 MDSPLUS "Proceede" */
/*  *43   28-OCT-1994 18:03:14 MDSPLUS "Proceede" */
/*  *42   28-OCT-1994 16:20:34 MDSPLUS "Proceede" */
/*  *41   28-OCT-1994 16:01:10 MDSPLUS "Proceede" */
/*  *40   28-OCT-1994 15:59:08 MDSPLUS "Proceede" */
/*  *39   28-OCT-1994 15:48:11 MDSPLUS "Proceede" */
/*  *38   28-OCT-1994 15:35:40 MDSPLUS "Proceede" */
/*  *37   28-OCT-1994 15:19:33 MDSPLUS "Proceede" */
/*  *36   28-OCT-1994 15:11:48 MDSPLUS "Proceede" */
/*  *35   28-OCT-1994 15:03:18 MDSPLUS "Proceede" */
/*  *34   28-OCT-1994 12:16:42 MDSPLUS "Proceede" */
/*  *33   28-OCT-1994 11:57:20 MDSPLUS "Proceede" */
/*  *32   28-OCT-1994 10:34:03 MDSPLUS "Proceede" */
/*  *31   28-OCT-1994 10:33:27 MDSPLUS "Proceede" */
/*  *30   28-OCT-1994 10:28:37 MDSPLUS "Proceede" */
/*  *29   28-OCT-1994 10:07:21 MDSPLUS "Proceede" */
/*  *28   27-OCT-1994 18:34:49 MDSPLUS "Proceede" */
/*  *27   27-OCT-1994 18:16:41 MDSPLUS "Proceede" */
/*  *26   27-OCT-1994 18:09:29 MDSPLUS "Proceede" */
/*  *25   27-OCT-1994 18:01:52 MDSPLUS "Proceede" */
/*  *24   27-OCT-1994 17:48:11 MDSPLUS "Proceede" */
/*  *23   27-OCT-1994 17:30:44 MDSPLUS "Proceede" */
/*  *22   27-OCT-1994 17:28:11 MDSPLUS "Proceede" */
/*  *21   27-OCT-1994 17:16:48 MDSPLUS "Proceede" */
/*  *20   27-OCT-1994 17:05:26 MDSPLUS "Proceede" */
/*  *19   27-OCT-1994 16:51:21 MDSPLUS "Prtoceede" */
/*  *18   27-OCT-1994 16:45:15 MDSPLUS "Prtoceede" */
/*  *17   27-OCT-1994 15:06:18 MDSPLUS "Proceede" */
/*  *16   26-OCT-1994 13:20:08 MDSPLUS "Proceede" */
/*  *15   19-OCT-1994 19:07:50 MDSPLUS "Proceede" */
/*  *14   19-OCT-1994 18:42:45 MDSPLUS "Proceede" */
/*  *13   19-OCT-1994 16:28:53 MDSPLUS "Fix" */
/*  *12   19-OCT-1994 16:22:25 MDSPLUS "Fix" */
/*  *11   19-OCT-1994 16:18:35 MDSPLUS "Fix" */
/*  *10   19-OCT-1994 16:11:26 MDSPLUS "Fix" */
/*  *9    18-OCT-1994 18:39:08 MDSPLUS "Proceede" */
/*  *8    18-OCT-1994 18:35:08 MDSPLUS "Proceede" */
/*  *7    18-OCT-1994 18:18:09 MDSPLUS "Proceede" */
/*  *6    18-OCT-1994 18:14:43 MDSPLUS "Proceede" */
/*  *5    18-OCT-1994 18:10:10 MDSPLUS "Proceede" */
/*  *4    18-OCT-1994 17:52:15 MDSPLUS "Proceede" */
/*  *3    18-OCT-1994 17:21:15 MDSPLUS "Proceede" */
/*  *2    18-OCT-1994 17:08:24 MDSPLUS "Proceede" */
/*  *1    17-OCT-1994 18:57:46 MDSPLUS "Butterwoth low pass filter" */
/*  DEC/CMS REPLACEMENT HISTORY, Element BUTTERWORTH.C */
  /*------------------------------------------------------------------------------

	Name:	BUTTERWORTH   

	Type:   C function

	Author:	Gabriele Manduchi
		Istituto Gas Ionizzati del CNR - Padova (Italy)

	Date:   17-OCT-1994

	Purpose: Low pass filtering using digitalization of Butterworth filter.

-------------------------------------------------------------------------------- */
#include "complex.h"
#include "filter.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <mdsplus/mdsconfig.h>

static Complex *FindChebPoles(double fp, double fs, double ap, double as, double fc, int *N,
			      double *gain);
static Complex *FindButtwPoles(double fp, double fs, double ap, double as, double fc, int *N,
			       double *gain);

Filter *ButtwInvar(float *fp, float *fs, float *ap, float *as, float *fc, int *out_n)
{
  return Invariant(*fp, *fs, *ap, *as, *fc, out_n, FindButtwPoles);
}

EXPORT Filter *ButtwBilinear(float *fp, float *fs, float *ap, float *as, float *fc, int *out_n)
{
  return Bilinear(*fp, *fs, *ap, *as, *fc, out_n, FindButtwPoles);
}

EXPORT Filter *ChebInvar(float *fp, float *fs, float *ap, float *as, float *fc, int *out_n)
{
  return Invariant(*fp, *fs, *ap, *as, *fc, out_n, FindChebPoles);
}

EXPORT Filter *ChebBilinear(float *fp, float *fs, float *ap, float *as, float *fc, int *out_n)
{
  return Bilinear(*fp, *fs, *ap, *as, *fc, out_n, FindChebPoles);
}

static Complex *FindButtwPoles(double Wp, double Ws, double ap, double as, double fc __attribute__ ((unused)), int *N,
			       double *gain)
{
  double n_real, Wc, l10;
  int n, j, i;
  Complex *poles;

  l10 = log(10.);

  if (*N == 0) {
    *gain = 1;
    n_real =
	0.5 * (log10(exp(l10 * as / 10) - 1) - log10(exp(l10 * ap / 10) - 1)) / (log10(Ws) -
										 log10(Wp));
    if (n_real - (int)n_real)	/* if not integer */
      n = (int)(n_real + 1);	/* immediate following integer */
    else
      n = n_real;
    Wc = exp(l10 * (log10(Wp) - log10(exp(l10 * ap / 10) - 1) / (2. * n)));
  } else {
    n = *N;
    Wc = Wp;
  }
/* Find poles */
  poles = (Complex *) malloc(n * sizeof(Complex));
  j = 0;
  if (n % 2) {			/* odd N */
    poles[j].re = -Wc;
    poles[j++].im = 0;
    for (i = 1; i < (n + 1) / 2; i++) {
      poles[j].re = -Wc * cos(PI * i / n);
      poles[j++].im = Wc * sin(PI * i / n);
      poles[j].re = -Wc * cos(PI * i / n);
      poles[j++].im = -Wc * sin(PI * i / n);
    }
  } else			/* even N */
    for (i = 0; i < n / 2; i++) {
      poles[j].re = -Wc * cos(PI * (i + 0.5) / n);
      poles[j++].im = Wc * sin(PI * (i + 0.5) / n);
      poles[j].re = -Wc * cos(PI * (i + 0.5) / n);
      poles[j++].im = -Wc * sin(PI * (i + 0.5) / n);
    }

  *N = n;
  return poles;
}

static Complex *FindChebPoles(double Wp, double Ws, double ap, double as, double fc __attribute__ ((unused)), int *N,
			      double *gain)
{
  double eps, Wc, alpha, a, b, l10, treshold, curr_val, angle, V, Vprev, Vnew;
  int n, i, j;
  Complex *poles;

  l10 = log(10.);

  Wc = Wp;
  eps = sqrt(exp(l10 * ap / 10) - 1);

  treshold = exp(l10 * as / 10);

  if (*N == 0) {
    n = 1;
    Vprev = 1;
    V = Ws / Wp;
    do {
      n++;
      Vnew = 2 * V * Ws / Wp - Vprev;
      curr_val = 1 + eps * eps * Vnew * Vnew;
      Vprev = V;
      V = Vnew;
    } while (curr_val < treshold);
    *N = n;
  } else
    n = *N;

  alpha = 1 / eps + sqrt(1 + 1 / (eps * eps));
  a = 0.5 * Wc * (exp(log(alpha) / n) - exp(-log(alpha) / n));
  b = 0.5 * Wc * (exp(log(alpha) / n) + exp(-log(alpha) / n));

  poles = (Complex *) malloc(n * sizeof(Complex));
  if (n % 2) {
    poles[0].re = -a;
    poles[0].im = 0;
    j = 1;
    for (i = 1; i < (n + 1) / 2; i++) {
      angle = i * PI / n;
      poles[j].im = -b * sin(angle);
      poles[j].re = -a * sqrt(1 - poles[j].im * poles[j].im / (b * b));
      j++;
      poles[j].re = poles[j - 1].re;
      poles[j].im = -poles[j - 1].im;
      j++;
    }
  } else {
    j = 0;
    for (i = 0; i < n / 2; i++) {
      angle = (i + 0.5) * PI / n;
      poles[j].im = b * sin(angle);
      poles[j].re = -a * sqrt(1 - poles[j].im * poles[j].im / (b * b));
      j++;
      poles[j].re = poles[j - 1].re;
      poles[j].im = -poles[j - 1].im;
      j++;
    }
  }
  *gain = 1 / (1 - eps);
  return poles;
}

