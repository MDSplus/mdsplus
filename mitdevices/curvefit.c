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
/* 
 * Curve fitting routines for DT200 CPCI digitizers.
 * adapted from B5910 code.
 *
 * Josh Stillerman 2/21/02
 */

#include <stdlib.h>
#include <mdsplus/mdsconfig.h>
extern void csakm_();
extern float csval_();

EXPORT int LinFit(int *num_knots, float *knots_x, float *knots_y, int *num_v, float *x, float *y)
{
  float slope;
  float intercept;
  int i;
  int knot_idx = 1;
  float high_x = 0.0;
  if (*num_knots > 1) {
    slope = (knots_x[1] == knots_x[0]) ? 0 : (knots_y[1] - knots_y[0]) / (knots_x[1] - knots_x[0]);
    intercept = knots_y[1] - slope * knots_x[1];
    high_x = knots_x[1];
  } else if (*num_knots == 1) {
    slope = 0.0;
    intercept = knots_y[0];
  } else {
    slope = 0.0;
    intercept = 0.0;
  }
  if (*num_v > 0) {
    for (i = 0; i < *num_v; i++) {
      while ((x[i] > high_x) && (knot_idx < (*num_knots - 1))) {
	knot_idx++;
	slope = (knots_x[knot_idx] == knots_x[knot_idx - 1]) ? 0 :
	    (knots_y[knot_idx] - knots_y[knot_idx - 1]) / (knots_x[knot_idx] -
							   knots_x[knot_idx - 1]);
	intercept = knots_y[knot_idx] - slope * knots_x[knot_idx];
	high_x = knots_x[knot_idx];
      }
      y[i] = slope * x[i] + intercept;
    }
  } else {
    float min_x = x[0];
    float max_x = x[-(*num_v) - 1];
    int j = 0;
    while ((min_x > high_x) && (knot_idx < (*num_knots - 1))) {
      knot_idx++;
      slope = (knots_x[knot_idx] == knots_x[knot_idx - 1]) ? 0 :
	  (knots_y[knot_idx] - knots_y[knot_idx - 1]) / (knots_x[knot_idx] - knots_x[knot_idx - 1]);
      intercept = knots_y[knot_idx] - slope * knots_x[knot_idx];
      high_x = knots_x[knot_idx];
    }
    y[j++] = slope * min_x + intercept;
    while ((max_x > high_x) && (knot_idx < (*num_knots - 1))) {
      y[j] = knots_y[knot_idx];
      x[j++] = knots_x[knot_idx];
      knot_idx++;
      slope = (knots_x[knot_idx] == knots_x[knot_idx - 1]) ? 0 :
	  (knots_y[knot_idx] - knots_y[knot_idx - 1]) / (knots_x[knot_idx] - knots_x[knot_idx - 1]);
      intercept = knots_y[knot_idx] - slope * knots_x[knot_idx];
      high_x = knots_x[knot_idx];
    }
    y[j] = slope * max_x + intercept;
    x[j++] = max_x;
    *num_v = j;
  }
  return 1;
}

EXPORT int SplineFit(int *num_knots, float *knots_x, float *knots_y, int *num_v, float *x, float *y)
{
  int i;
  if (*num_knots > 2) {
    float *fbreak = (float *)calloc(*num_knots, sizeof(float));
    float *cscoef = (float *)calloc(*num_knots * 4, sizeof(float));
    csakm_(num_knots, knots_x, knots_y, fbreak, cscoef);
    for (i = 0; i < *num_v; i++)
      y[i] = csval_(&x[i], num_knots, fbreak, cscoef);
    free((char *)fbreak);
    free((char *)cscoef);
    return 1;
  } else
    return LinFit(num_knots, knots_x, knots_y, num_v, x, y);
}
