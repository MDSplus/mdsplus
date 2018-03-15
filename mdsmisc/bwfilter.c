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
/*  CMS REPLACEMENT HISTORY, Element BWFILTER.C */
/*  *1    22-DEC-1997 10:45:22 TWF "Butterworth filter" */
/*  CMS REPLACEMENT HISTORY, Element BWFILTER.C */
#include <stdio.h>
#include	<math.h>
#include <mdsplus/mdsconfig.h>

#define	PI 3.14159265359
#define	max_order 10

EXPORT int bwfilter(float *w_cut_in, int *order_in, int *num, float *in, float *out)
{
  int M, N;
  int p, q, order = *order_in;
  double num_coeff, den_coeff, c[max_order + 1], d[max_order + 1];
  double coeff[2][max_order + 1] = {{0},{0}};
  double w_cut = (double)*w_cut_in, omega_cut;
  int upto, i, j, k;
  int in_idx = 0, out_idx;
  double x_current[max_order + 1], y_filtered[max_order + 1];
  double sum_c, sum_d;

  if (order > max_order) {
    printf("Maximum order is 10\n");
    return 0;
  }
  M = N = order;
  omega_cut = 2.0 * tan(w_cut * PI);
  num_coeff = omega_cut / (2.0 + omega_cut);
  den_coeff = -(2.0 - omega_cut) / (2.0 + omega_cut);

  coeff[0][0] = coeff[0][1] = 1.0;
  for (p = 2; p <= order; p++) {
    coeff[1][0] = coeff[1][p] = 1.0;
    for (q = 1; q <= p - 1; q++) {
      coeff[1][q] = coeff[0][q] + coeff[0][q - 1];
    }
    for (q = 0; q <= p; q++) {
      coeff[0][q] = coeff[1][q];
    }
  }

  c[0] = pow(num_coeff, (double)order);
  for (q = 1; q <= order; q++) {
    c[q] = c[0] * coeff[1][q];
    d[q] = pow(den_coeff, (double)q) * coeff[1][q];
  }

  upto = M >= N ? M : N;

  for (i = 0; i <= upto; i++) {
    x_current[upto - i] = (double)in[in_idx++];
  }

  for (j = 1; j <= N; j++) {
    y_filtered[j] = x_current[j];
  }

/* Do dynamic filtering in time domain until input stream stops */

  for (out_idx = 0; out_idx < *num && in_idx < *num; out_idx++) {
    y_filtered[0] = 0.0;
    sum_c = 0.0;
    sum_d = 0.0;

    for (k = 0; k <= M; k++) {
      sum_c += c[k] * x_current[k];
    }
    for (j = 1; j <= N; j++) {
      sum_d += -d[j] * y_filtered[j];
    }
    y_filtered[0] = sum_c + sum_d;
    out[out_idx] = (float)y_filtered[0];

    for (i = M; i >= 1; i--) {
      x_current[i] = x_current[i - 1];
    }
    for (j = N-1; j > 1; j--) {
      y_filtered[j+1] = y_filtered[j];
    }

    x_current[0] = in[in_idx++];

  }

  return 1;
}
