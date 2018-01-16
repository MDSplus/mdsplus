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
/*  CMS REPLACEMENT HISTORY, Element BESSEL.C */
/*  *44   20-SEP-2001 14:15:14 MDSPLUS "Fix s3 parameter di BessJ0" */
/*  *43   16-SEP-1996 10:22:07 MDSPLUS "Add total power" */
/*  *42   15-MAR-1995 11:19:22 MDSPLUS "Compatibility" */
/*  *41   14-MAR-1995 16:57:51 MDSPLUS "ANSI C" */
/*  *40   13-MAR-1995 17:31:45 MDSPLUS "New Version" */
/*  *39   23-JAN-1995 15:28:11 MDSPLUS "ADd TORUS_RADIUS" */
/*  *38   15-DEC-1994 17:19:23 MDSPLUS "Proceede" */
/*  *37   13-DEC-1994 15:34:37 MDSPLUS "fix" */
/*  *36   13-DEC-1994 15:08:24 MDSPLUS "Add BessPol" */
/*  *35   16-NOV-1994 12:44:03 MDSPLUS "OSF/1 compatibile" */
/*  *34   15-NOV-1994 16:31:54 MDSPLUS "Ported to OSF1" */
/*  *33   15-NOV-1994 14:46:38 MDSPLUS "Move to OSF1" */
/*  *32   11-NOV-1994 13:41:10 MDSPLUS "Proceede" */
/*  *31   10-NOV-1994 17:36:44 MDSPLUS "Proceede" */
/*  *30    8-NOV-1994 18:35:56 MDSPLUS "Proceede" */
/*  *29    8-NOV-1994 18:11:33 MDSPLUS "Proceede" */
/*  *28    8-NOV-1994 17:55:50 MDSPLUS "Proceede" */
/*  *27    8-NOV-1994 17:31:15 MDSPLUS "Proceede" */
/*  *26    8-NOV-1994 17:18:02 MDSPLUS "Proceede" */
/*  *25    7-NOV-1994 13:48:06 MDSPLUS "Proceede" */
/*  *24    7-NOV-1994 11:43:51 MDSPLUS "Proceede" */
/*  *23    5-OCT-1994 14:09:10 MDSPLUS "Fix" */
/*  *22    5-OCT-1994 13:56:05 MDSPLUS "Fix" */
/*  *21    5-OCT-1994 10:36:59 MDSPLUS "Fix" */
/*  *20    5-OCT-1994 10:18:29 MDSPLUS "Fix" */
/*  *19    5-OCT-1994 09:58:59 MDSPLUS "Fix" */
/*  *18   27-SEP-1994 16:55:31 MDSPLUS "Proceede" */
/*  *17   22-SEP-1994 18:54:57 MDSPLUS "" */
/*  *16   22-SEP-1994 18:23:13 MDSPLUS "Proceede" */
/*  *15   22-SEP-1994 18:19:56 MDSPLUS "Proceede" */
/*  *14   22-SEP-1994 18:13:07 MDSPLUS "Proceede" */
/*  *13   22-SEP-1994 18:10:38 MDSPLUS "Proceede" */
/*  *12   22-SEP-1994 18:05:47 MDSPLUS "Proceede" */
/*  *11   22-SEP-1994 16:38:46 MDSPLUS "Proceede" */
/*  *10   22-SEP-1994 16:36:23 MDSPLUS "Proceede" */
/*  *9    22-SEP-1994 16:34:59 MDSPLUS "Proceede" */
/*  *8    22-SEP-1994 16:34:02 MDSPLUS "Proceede" */
/*  *7    22-SEP-1994 16:29:29 MDSPLUS "Proceede" */
/*  *6    22-SEP-1994 16:19:53 MDSPLUS "Proceede" */
/*  *5    22-SEP-1994 16:15:49 MDSPLUS "Proceede" */
/*  *4    22-SEP-1994 16:03:35 MDSPLUS "Proceede" */
/*  *3    22-SEP-1994 15:52:54 MDSPLUS "" */
/*  *2    21-SEP-1994 18:51:39 MDSPLUS "Fix" */
/*  *1    21-SEP-1994 18:46:27 MDSPLUS "Bessel functions management" */
/*  CMS REPLACEMENT HISTORY, Element BESSEL.C */
/*------------------------------------------------------------------------------

	Name:	BESSEL   

	Type:   C function

	Author:	Gabriele Manduchi
		Istituto Gas Ionizzati del CNR - Padova (Italy)

	Date:   21-SEP-1994

	Purpose: Bessel related routines
--------------------------------------------------------------------------------

 Description: This file contains the following routines:

double BessJ0(double x)			Computes J0(x)

double BessJ1(double x)			Computes J1(x)

double BessJn(int n, double x)	Computes Jm(x)

double BessJmD(double x)		Computes Jm'(x)

void BessRoots(char *filename)		Computes and store in a file the first 40 real roots of J0, J1, J2

Complex *BessCRoots(int n, int *num_poles) Computes the Complex poles of a bessel polinomial of order n, up to n=10

double **Bessel(int mc_max, int ms_max, int l_max, int num_chords, double *p, double *fi)
					Compute Bessel Matrix for Tomoggraphic inversion, where:
    int mc_max: maximum angolar harmonic for cosine theta expansion
    int ms_max: maximum angolar harmonic for sine theta expansion
    int l_max: maximum radial harmonic for Bessel expansion
    int num_chords: number of measured (or null) chord mesaurements. Geometry is assumed already resolved.
    double *p: p[i] is the p value for the i-th chord
    double *fi: f[i] is the fi value for the i-th chord

To rebuild emissivity, given a fixed r and a variable theta, BesselStartRebuild must be called
once and then BesselRebuild returns the emissivity at various values of theta.
Bessel FreeRebuilds frees temporary storage allocated by ZernickStartRebuild.

When emissivity is requested for a different value of r, BesselStartRebuild must be called again.

double **BesselStartRebuild(double r, int mc_max, int ms_max, int l_max)
Computes a temporary matrix which is used in subsequent calls to ZernickRebuild for the same value of r

double BesselRebuild(double r, double theta, double *a, int mc_max, int ms_max, int l_max, double **j_temp)
Returns the emissivity value, given the computed coefficients a and the temporary matrix returned by BesselStartRebuild
    r, theta: coordinates
    a: cofficients vector
    int mc_max: maximum angolar harmonic for cosine theta expansion
    int ms_max: maximum angolar harmonic for sine theta expansion
    int l_max: maximum radial harmonic for Bessel expansion

void BesselEndRebuild(double **z_temp, int mc_max, int ms_max)
frees temporary storage allocated by BesselStartRebuild. It must be called before calling again BesselStartRebuild.

double BesselTotalPower(double *a, int l_max)
Computes the surface integral for mode 0 of rebuilt emissivity

------------------------------------------------------------------------------*/

#define maximum(a,b) (((a)>(b))?(a):(b))

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include "definitions.h"
#include "bessel.h"
#include "filter.h"
#include <mdsplus/mdsconfig.h>
#define STEP_ROOT	1E-1
#define TOLERANCE	1E-8

#define SameSign(x, y) (((x)>0 && (y)>0)||((x)<0 && (y)<0))

/* Private Routines */
static double BessJ0(double x);
static double BessJ1(double x);

static double FindRoot(int n, double *x);
static double Bisection(int n, double x1, double x2, double y1, double y2);
static Complex *FindBessPoles(double Wp, double Ws, double ap, double as, double fc, int *N,
			      double *gain);
Filter *Invariant(double fp, double fs, double ap, double as, double fc, int *out_n,
		  Complex * (*FindPoles) ());
/*
static int ConvertCoord(float x, float y, float *out_r, float *out_theta)
{
  float r, th;

  r = sqrt(x * x + y * y);
  if (r > 1)
    return 0;
  if (r < 1E-8)
    th = 1;
  else {
    if (x <= 0)
      th = PI - asin(y / r);
    else if (y >= 0)
      th = asin(y / r);
    else
      th = 2 * PI + asin(y / r);
  }
  *out_r = r;
  *out_theta = th;
  return 1;
}
*/

EXPORT double BessJ0(double x)
{
  double result, y, ax, z, xx;
  static double
      p1 = 1.,
      p2 = -0.1098628627E-2,
      p3 = 0.2734510407E-4,
      p4 = -0.2073370639E-5,
      p5 = 0.2093887211E-6,
      q1 = -0.1562499995E-1,
      q2 = 0.1430488765E-3,
      q3 = -0.6911147651E-5,
      q4 = 0.7621095161E-6,
      q5 = -0.934945152E-7,
      r1 = 57568490574.,
      r2 = -13362590354.,
      r3 = 651619640.7,
      r4 = -11214424.18,
      r5 = 77392.33017,
      r6 = -184.9052456,
      s1 = 57568490411.,
      s2 = 1029532985., s3 = 9494680.718, s4 = 59272.64853, s5 = 267.8532712, s6 = 1.;

  ax = fabs(x);
  if (ax < 8.) {
    y = x * x;
    result =
	(r1 + y * (r2 + y * (r3 + y * (r4 + y * (r5 + y * r6))))) / (s1 +
								     y * (s2 +
									  y * (s3 +
									       y * (s4 +
										    y * (s5 +
											 y *
											 s6)))));
  } else {
    z = 8 / ax;
    y = z * z;
    xx = ax - 0.785398164;
    result =
	sqrt(0.636619772 / ax) * (cos(xx) * (p1 + y * (p2 + y * (p3 + y * (p4 + y * p5)))) -
				  z * sin(xx) * (q1 + y * (q2 + y * (q3 + y * (q4 + y * q5)))));
  }
  return result;
}

EXPORT double BessJ1(double x)
{
  double result, y, ax, z, xx;
  static double
      r1 = 72362614232.,
      r2 = -7895059235.,
      r3 = 242396853.1,
      r4 = -2972611.439,
      r5 = 15704.48260,
      r6 = -30.16036606,
      s1 = 144725228442.,
      s2 = 2300535178.,
      s3 = 18583304.74,
      s4 = 99447.43394,
      s5 = 376.9991397,
      s6 = 1.,
      p1 = 1.,
      p2 = 0.183105E-2,
      p3 = -0.3516396496E-4,
      p4 = 0.2457520174E-5,
      p5 = -0.240337019E-6,
      q1 = 0.04687499995,
      q2 = -0.2002690873E-3, q3 = 0.8449199096E-5, q4 = -0.88228987E-6, q5 = 0.105787412E-6;

  ax = fabs(x);
  if (ax < 8) {
    y = x * x;
    result =
	x * (r1 + y * (r2 + y * (r3 + y * (r4 + y * (r5 + y * r6))))) / (s1 +
									 y * (s2 +
									      y * (s3 +
										   y * (s4 +
											y * (s5 +
											     y *
											     s6)))));
  } else {
    z = 8 / ax;
    y = z * z;
    xx = ax - 2.356194491;
    result =
	sqrt(.636619772 / ax) * (cos(xx) * (p1 + y * (p2 + y * (p3 + y * (p4 + y * p5)))) -
				 z * sin(xx) * (q1 + y * (q2 + y * (q3 + y * (q4 + y * q5)))));
    if (x < 0)
      result = -result;
  }
  return result;
}

EXPORT double BessJn(int n, double x)
{
  int iacc = 40;
  static double bigno = 1E10, bigni = 1E-10;
  double ax, tox, bjm, bj, bjp, jsum, sum, bessj;
  int j, m;

  if (n < 0) {
    printf("Bad n argument in BessJn");
    return 0;
  }
  if (n == 0)
    return BessJ0(x);
  if (n == 1)
    return BessJ1(x);
  ax = fabs(x);
  if (ax == 0)
    bessj = 0;
  else {
    if (ax > n) {
      tox = 2 / ax;
      bjm = BessJ0(ax);
      bj = BessJ1(ax);
      for (j = 1; j < n; j++) {
	bjp = j * tox * bj - bjm;
	bjm = bj;
	bj = bjp;
      }
      bessj = bj;
    } else {
      tox = 2 / ax;
      m = 2 * ((n + (int)sqrt(iacc * n)) / 2);
      bessj = 0;
      jsum = 0;
      sum = 0;
      bjp = 0;
      bj = 1;
      for (j = m; j >= 1; j--) {
	bjm = j * tox * bj - bjp;
	bjp = bj;
	bj = bjm;
	if (fabs(bj) > bigno) {
	  bj = bj * bigni;
	  bjp = bjp * bigni;
	  bessj = bessj * bigni;
	  sum = sum * bigni;
	}
	if (jsum != 0)
	  sum = sum + bj;
	jsum = 1 - jsum;
	if (j == n)
	  bessj = bjp;
      }
      sum = 2 * sum - bj;
      bessj = bessj / sum;
    }
    if ((x < 0) && (n % 2))
      bessj = -bessj;
  }
  return bessj;
}

EXPORT double BessJnD(int n, double x)
{
/*  Computes Jn'(x) using relations:
	2 * Jn'(x) = Jn-1(x) - Jn+1(x)
    and
	J0'(x) = -J1(x)	    */

  if (n == 0)
    return -BessJ1(x);
  else
    return (BessJn(n - 1, x) - BessJn(n + 1, x)) / 2;
}

EXPORT void BessRoots()
{
  char filename[256];
  int idx, root_idx;
  FILE *fil_ptr = 0;
  double curr_root, curr_x;

  printf("\nComputation of Roots for Bessel functions\n\nOut File (0 for screen):");
  scanf("%s", filename);
  if (strcmp(filename, "0")) {
    if ((fil_ptr = fopen(filename, "w")) == 0) {
      printf("\nCannot open file %s\n", filename);
      return;
    }
  }
  for (idx = 0; idx <= MAX_BESSEL_IDX; idx++) {
    curr_x = 0;
    for (root_idx = 0; root_idx < NUM_ROOTS; root_idx++) {
      if (idx > 0 && root_idx == 0)
	curr_root = 0;
      else
	curr_root = FindRoot(idx, &curr_x);
      if (fil_ptr) {
	fprintf(fil_ptr, "%4.8f", curr_root);
	if (root_idx < NUM_ROOTS - 1)
	  fprintf(fil_ptr, ", ");
	else
	  fprintf(fil_ptr, "\n");
      } else {
	printf("%4.8f", curr_root);
	if (root_idx < NUM_ROOTS - 1)
	  printf(", ");
	else
	  printf("\n");
      }
    }
  }

}

static double FindRoot(int n, double *x)
{
  double y1, y2, curr_x, result;

  curr_x = *x;
  do {
    curr_x += STEP_ROOT;
    y1 = BessJn(n, curr_x);
    y2 = BessJn(n, curr_x + STEP_ROOT);
  } while SameSign
  (y1, y2);			/* Find interval containing root */
  result = Bisection(n, curr_x, curr_x + STEP_ROOT, y1, y2);
  *x = curr_x + STEP_ROOT;
  return result;
}

static double Bisection(int n, double x1, double x2, double y1, double y2)
{
  double x, y;
  x = (x1 + x2) / 2;
  if ((x2 - x1) < TOLERANCE)
    return x;
  else {
    y = BessJn(n, x);
    if (SameSign(y1, y))
      return Bisection(n, x, x2, y, y2);
    else
      return Bisection(n, x1, x, y1, y);
  }
}

EXPORT double **Bessel(int mc_max, int ms_max, int l_max, int num_chords, double *p, double *fi)
{
  int ms, mc, l, c, num_cols, curr_col, m, n, m_max;
  double **w, **w_temp, inv_cos, sum, curr_val;

  m_max = maximum(mc_max, ms_max) + 1;
  num_cols = (1 + mc_max + ms_max) * (l_max + 1);

/* Allocate storage for results  and temporary matrix */
  w = (double **)malloc(num_chords * sizeof(double *));
  for (c = 0; c < num_chords; c++)
    w[c] = (double *)malloc(num_cols * sizeof(double));
  w_temp = (double **)malloc(m_max * sizeof(double *));
  for (c = 0; c < m_max; c++)
    w_temp[c] = (double *)malloc((l_max + 1) * sizeof(double));

/* Compute values */
  for (c = 0; c < num_chords; c++) {
/* Compute Fml(p) in temporary matrix */
    inv_cos = acos(p[c]);
    for (m = 0; m < m_max; m++) {
      for (l = 0; l <= l_max; l++) {
	for (sum = 0, n = 0; n < MAX_BESSEL_EXPANSION; n++)
	  if (n != m) {
	    curr_val = BessJn(n, bessel_root[m][l]) * sin(n * PI / 2 - bessel_root[m][l] * p[c]) *
		(sin((m + n) * inv_cos) / (m + n) + sin((m - n) * inv_cos) / (m - n)) / sqrt(1 -
											     p[c] *
											     p[c]);
	    if (n == 0)
	      sum += curr_val / 2;
	    else
	      sum += curr_val;
	  }
	w_temp[m][l] = sum;
      }
    }
/* Compute result matrix */
    for (mc = 0, curr_col = 0; mc <= mc_max; mc++)
      for (l = 0; l <= l_max; l++)
	w[c][curr_col++] = -2 * sqrt(1 - p[c] * p[c]) * BessJnD(mc, bessel_root[mc][l]) *
	    w_temp[mc][l] * cos(mc * fi[c]);
    for (ms = 1; ms <= ms_max; ms++)
      for (l = 0; l <= l_max; l++)
	w[c][curr_col++] = -2 * sqrt(1 - p[c] * p[c]) * BessJnD(ms, bessel_root[ms][l]) *
	    w_temp[ms][l] * sin(ms * fi[c]);
  }
/* free temporary storage */
  for (c = 0; c < m_max; c++)
    free((char *)w_temp[c]);
  free((char *)w_temp);

  return w;
}

EXPORT double **BesselStartRebuild(double r, int mc_max, int ms_max, int l_max)
{
  double **j_temp;
  int m_max, m, l;

/* Compute in a temporary array Jm(Xml*r) */
/* Allocate array */
  m_max = maximum(mc_max, ms_max) + 1;
  j_temp = (double **)malloc(m_max * sizeof(double *));
  for (m = 0; m < m_max; m++)
    j_temp[m] = (double *)malloc((l_max + 1) * sizeof(double));
/* Compute it */
  for (m = 0; m < m_max; m++)
    for (l = 0; l <= l_max; l++)
      j_temp[m][l] = BessJn(m, bessel_root[m][l] * r);

  return j_temp;
}

EXPORT double BesselRebuildModes(double *a, int mc_max, int ms_max __attribute__ ((unused)), int l_max, double **j_temp, int mode,
			  int is_cosine)
{
  double result = 0;
  int l, curr_idx;

/* Compute emissivity */

  if (is_cosine)
    curr_idx = mode * (l_max + 1);
  else
    curr_idx = (mc_max + mode) * (l_max + 1);

  for (l = 0; l <= l_max; l++)
    result += a[curr_idx++] * j_temp[mode][l];

  return result;
}

EXPORT double BesselRebuild(double r __attribute__ ((unused)), double theta, double *a, int mc_max, int ms_max, int l_max,
		     double **j_temp)
{
  double result = 0, sum;
  int m, l, curr_idx;

/* Compute emissivity */
  for (m = curr_idx = 0; m <= mc_max; m++) {
    for (sum = 0, l = 0; l <= l_max; l++)
      sum += a[curr_idx++] * j_temp[m][l];
    result += sum * cos(m * theta);
  }
  for (m = 1; m <= ms_max; m++) {
    for (sum = 0, l = 0; l <= l_max; l++)
      sum += a[curr_idx++] * j_temp[m][l];
    result += sum * sin(m * theta);
  }

  return result;
}

EXPORT void BesselEndRebuild(double **j_temp, int mc_max, int ms_max)
/* free temporary storage */
{
  int m, m_max;

  m_max = maximum(mc_max, ms_max) + 1;
  for (m = 0; m < m_max; m++)
    free((char *)j_temp[m]);
  free((char *)j_temp);
}

static double bess_delay;

EXPORT Complex *BessCRoots(int n, double *g)
{
  int n_poles, i, j;
  Complex *poles;
  double a, b, c;

  n_poles = n;
  poles = (Complex *) malloc(n_poles * sizeof(Complex));
  if (n_poles % 2) {
    poles[0].re = -BesselFactors[n - 1][1] / bess_delay;
    poles[0].im = 0;
    j = 2;
    i = 1;
  } else {
    j = 0;
    i = 0;
  }
  while (i < n_poles) {
    b = BesselFactors[n - 1][j++];
    c = BesselFactors[n - 1][j++];
    if (n >= 5) {
      c = c * c + b * b;
      b *= 2;
    }
    a = b * b - 4 * c;
    if (a >= 0) {
      printf("Internal error in BessCRoots\n");
      exit(1);
    }
    poles[i].re = -b / (2 * bess_delay);
    poles[i++].im = sqrt(-a) / (2 * bess_delay);
    poles[i].re = poles[i - 1].re;
    poles[i].im = -poles[i - 1].im;
    i++;
  }
  *g = 1;
  return poles;
}

EXPORT Filter *BessInvar(float *fp, float *fs, float *ap, float *as, float *fc, float *delay, int *out_n)
/* Unlike Butterworth and Chebyshev filters, fs is the maximum frequency at which we want an error of as in phase */
{
  bess_delay = *delay;
  return Invariant(*fp, *fs, *ap, *as, *fc, out_n, FindBessPoles);
}

static Complex *FindBessPoles(double Wp, double Ws, double ap, double as, double fc __attribute__ ((unused)), int *N,
			      double *gain)
{
  double norm_wp, norm_ws;
  int n1, n2, n, i;

  norm_wp = Wp * bess_delay;
  norm_ws = Ws * bess_delay;

  if (*N == 0) {
    for (n1 = 3; n1 < 10; n1++) {
      for (i = 1; i < NUM_STEP_MAG_ATT && (norm_wp > i * 0.5); i++) ;
      if (MagnitudeAtt[n1 - 3][i - 1] <= ap)
	break;			/* current value of n1 satisfies attenuation requirements */
    }

    for (n2 = 3; n2 < 10; n2++) {
      for (i = 1; i < NUM_STEP_DEL_ERR && (norm_ws > i * 0.5); i++) ;
      if (DelayErr[n2 - 3][i - 1] <= as)
	break;			/* current value of n2 satisfies delay requirements */
    }
    n = (n1 > n2) ? n1 : n2;
    *N = n;
  } else
    n = *N;
  return BessCRoots(n, gain);
}

/*
double BessPol(float x, float y, float *parameters)
{
    float t, r, th, res, is_cosine;
    int k, toggle;
    int m, l;

    m = parameters[0] + 0.5;
    l = parameters[1] + 0.5;
    is_cosine = parameters[2];
    if(m > MAX_BESSEL_IDX) 
    {
	printf("M is too high\n");
	return 0;   
    }

    if(l > NUM_ROOTS) 
    {
	printf("L is too high\n");
	return 0;   
    }

    if(!ConvertCoord(x, y, &r, &th))
	return 0;

    res = BessJn(m, bessel_root[m][l] * r);

    if(m == 0)
	return res;
    if(is_cosine)
	res *= cos(m * th);
    else
	res *= sin(m * th);
    return res;
}

#define POWER_STEPS 100

double BesselTotalPower(double *a, int l_max)
{
    double result = 0, sum, r, step, j_temp;
    int l;

    step = 1./POWER_STEPS;
    for(r = 0, result = 0; r < 1; r += step)
    {
	for(l = 0, sum = 0; l <= l_max; l++)
	{
	    j_temp = BessJn(0, bessel_root[0][l] * r);
	    sum += a[l] * j_temp;
	}
	result += sum * step * 2 * PI * r;
    }
    return result;
}
    
*/
