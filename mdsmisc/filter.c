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
/*  DEC/CMS REPLACEMENT HISTORY, Element FILTER.C */
/*  *38   20-JUL-1995 09:17:34 MDSPLUS "Ansi C" */
/*  *37   20-JUL-1995 09:07:09 MDSPLUS "Latest" */
/*  *36   27-MAR-1995 11:59:24 MDSPLUS "Ansi C" */
/*  *35   15-MAR-1995 11:20:15 MDSPLUS "Compatibility" */
/*  *34   14-MAR-1995 16:59:29 MDSPLUS "ANSI C" */
/*  *33   13-MAR-1995 17:32:19 MDSPLUS "New Version" */
/*  *32    1-FEB-1995 14:20:59 MDSPLUS "Add VME stuff" */
/*  *31   22-NOV-1994 14:19:38 MDSPLUS "Fix Alpha C compiler bug" */
/*  *30   16-NOV-1994 12:44:24 MDSPLUS "OSF/1 compatibile" */
/*  *29   15-NOV-1994 16:32:19 MDSPLUS "Ported to OSF1" */
/*  *28   15-NOV-1994 16:11:26 MDSPLUS "" */
/*  *27   15-NOV-1994 15:16:12 MDSPLUS "Move to OSF1" */
/*  *26   11-NOV-1994 18:29:21 MDSPLUS "Proceede" */
/*  *25   11-NOV-1994 18:12:43 MDSPLUS "Proceede" */
/*  *24   11-NOV-1994 18:09:07 MDSPLUS "Proceede" */
/*  *23   11-NOV-1994 16:45:31 MDSPLUS "Proceede" */
/*  *22   11-NOV-1994 16:42:35 MDSPLUS "Proceede" */
/*  *21   11-NOV-1994 15:43:08 MDSPLUS "Proceede" */
/*  *20   11-NOV-1994 15:37:14 MDSPLUS "Proceede" */
/*  *19   11-NOV-1994 14:19:38 MDSPLUS "Proceede" */
/*  *18   11-NOV-1994 14:17:44 MDSPLUS "Proceede" */
/*  *17    3-NOV-1994 10:07:48 MDSPLUS "Fix" */
/*  *16    2-NOV-1994 19:04:08 MDSPLUS "Proceede" */
/*  *15    2-NOV-1994 18:58:09 MDSPLUS "Proceede" */
/*  *14    2-NOV-1994 18:34:07 MDSPLUS "Proceede" */
/*  *13   28-OCT-1994 16:22:06 MDSPLUS "Proceede" */
/*  *12   27-OCT-1994 13:49:51 MDSPLUS "Proceede" */
/*  *11   27-OCT-1994 13:09:44 MDSPLUS "Proceede" */
/*  *10   27-OCT-1994 12:55:07 MDSPLUS "Proceede" */
/*  *9    27-OCT-1994 11:52:04 MDSPLUS "Proceede" */
/*  *8    27-OCT-1994 10:54:21 MDSPLUS "Proceede" */
/*  *7    26-OCT-1994 15:19:48 MDSPLUS "Proceede" */
/*  *6    26-OCT-1994 14:43:48 MDSPLUS "Proceede" */
/*  *5    26-OCT-1994 13:24:05 MDSPLUS "Proceede" */
/*  *4    26-OCT-1994 12:01:55 MDSPLUS "Proceede" */
/*  *3    25-OCT-1994 17:19:36 MDSPLUS "Proceede" */
/*  *2    24-OCT-1994 19:31:03 MDSPLUS "Proceede" */
/*  *1    20-OCT-1994 17:41:45 MDSPLUS "General purpose filter routines" */
/*  DEC/CMS REPLACEMENT HISTORY, Element FILTER.C */
/*------------------------------------------------------------------------------

	Name:	FILTER   

	Type:   C function

	Author:	Gabriele Manduchi
		Istituto Gas Ionizzati del CNR -Padova (Italy)

	Date:   20-OCT-1994

	Purpose: Digital filter implementation and test.
--------------------------------------------------------------------------------

 Description: public routines are:

	DoFilter(Filter *filter, float *in, float *out, int *n_samples)
	    perform digital filtering as specified in structure filter. in and out must be already allocated.

	DoFilterResample(Filter *filter, float *in, float *out, int *n_samples, int *start_idx, int *delta_idx, 
		int *max_out_samples)
	    perform digital filtering as specified in structure filter. but only samples corresponding
	    to start_idx + N*delta_idx (N >=0) are stored in out.
	    In and out must be already allocated.

	DoFilterResampleVME(Filter *filter, short *in, float *out, int *n_samples, int *start_idx, int *delta_idx, 
		int *max_out_samples, int step_raw)
	    same as DoFilterResample, except that input are 12 bit representation of an integer number

	TestFilter(Filter *filter, double fc, int n_points, double *module, double *phase)
	    evaluates module and phase of the filter as specified by structure filter.
	    module and phase must be already allocated

------------------------------------------------------------------------------*/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mdsplus/mdsconfig.h>
#include "filter.h"

#define N_POINTS_TEST 500	/* NEVER less than 100 */

static void NormalizeFilter(Filter * filter);

EXPORT void DoFilter(Filter * filter, float *in, float *out, int *n_s)
{
  int start_idx = 0, delta_idx = 1, out_s;
  out_s = *n_s;
  DoFilterResample(filter, in, out, n_s, &start_idx, &delta_idx, &out_s);
}

EXPORT void DoFilterResample(Filter * filter, float *in, float *out, int *n_s, int *start_idx,
		      int *delta_idx, int *max_out_samples)
{
  int i, j, curr_sample, k_num, k_den, n_samples, start, delta, *curr_idx, out_idx, idx;
  float **history, *curr_out;

  out_idx = 0;
  n_samples = *n_s;
  start = *start_idx;
  delta = *delta_idx;

/* normalize if not already done */
  NormalizeFilter(filter);

  history = (float **)malloc(filter->num_parallels * sizeof(float *));
  curr_idx = (int *)malloc(filter->num_parallels * sizeof(int));
  curr_out = (float *)malloc(filter->num_parallels * sizeof(float));

  for (i = 0; i < filter->num_parallels; i++) {
    if (filter->units[i].den_degree > 1) {
      history[i] = (float *)calloc(filter->units[i].den_degree * sizeof(float), 1);
      curr_idx[i] = filter->units[i].den_degree - 1;
    }
  }

  for (curr_sample = 0; curr_sample < n_samples && out_idx < *max_out_samples; curr_sample++) {
    for (i = 0; i < filter->num_parallels; i++) {
      curr_out[i] = 0;
      if (curr_sample + 1 >= filter->units[i].num_degree)
	k_num = filter->units[i].num_degree;
      else
	k_num = curr_sample + 1;
      if (curr_sample + 1 >= filter->units[i].den_degree)
	k_den = filter->units[i].den_degree;
      else
	k_den = curr_sample + 1;
      for (j = 0; j < k_num; j++)
	curr_out[i] += filter->units[i].num[j] * in[curr_sample - j];
      for (j = 1; j < k_den; j++) {
	idx = (curr_idx[i] - 1 + j) % filter->units[i].den_degree;
	curr_out[i] -= filter->units[i].den[j] * history[i][idx];
      }

/* Update history */
/* move marker in circolar buffer */
      if (filter->units[i].den_degree > 1) {
	curr_idx[i]--;
	if (curr_idx[i] < 0)
	  curr_idx[i] = filter->units[i].den_degree - 1;
	history[i][curr_idx[i]] = curr_out[i];
      }
    }

/* store sample in out if needed */
    if (curr_sample >= start && !((curr_sample - start) % delta)) {
      for (i = 0, out[out_idx] = 0; i < filter->num_parallels; i++)
	out[out_idx] += curr_out[i];
      out_idx++;
    }
  }

  *max_out_samples = out_idx;

/* Free storage */
  for (i = 0; i < filter->num_parallels; i++)
    if (filter->units[i].den_degree > 0)
      free((char *)history[i]);
  free((char *)history);
  free((char *)curr_out);
  free((char *)curr_idx);

/* First sample equal for input and output */
  if (start == 0)
    out[0] = in[start];
}

EXPORT void DoFilterResampleVME(Filter * filter, short *in, float *out, int *n_s, int *start_idx,
			 int *delta_idx, int *max_out_samples, int step_raw)
{
  int i, j, k, curr_sample, k_num, k_den, n_samples, start, delta, *curr_idx, out_idx, idx;
  float **history, *curr_out;

  out_idx = 0;
  n_samples = *n_s;
  start = *start_idx;
  delta = *delta_idx;

/* normalize if not already done */
  NormalizeFilter(filter);

  history = (float **)malloc(filter->num_parallels * sizeof(float *));
  curr_idx = (int *)malloc(filter->num_parallels * sizeof(int));
  curr_out = (float *)malloc(filter->num_parallels * sizeof(float));

  for (i = 0; i < filter->num_parallels; i++) {
    if (filter->units[i].den_degree > 1) {
      history[i] = (float *)calloc(filter->units[i].den_degree * sizeof(float), 1);
      curr_idx[i] = filter->units[i].den_degree - 1;
    }
  }

  for (curr_sample = 0; curr_sample < n_samples && out_idx < *max_out_samples;
       curr_sample += step_raw) {
    for (i = 0; i < filter->num_parallels; i++) {
      curr_out[i] = 0;
      if (curr_sample / step_raw + 1 >= filter->units[i].num_degree)
	k_num = filter->units[i].num_degree;
      else
	k_num = curr_sample / step_raw + 1;
      if (curr_sample / step_raw + 1 >= filter->units[i].den_degree)
	k_den = filter->units[i].den_degree;
      else
	k_den = curr_sample / step_raw + 1;
      for (j = k = 0; k < k_num; j += step_raw, k++)
	curr_out[i] += filter->units[i].num[k] * in[curr_sample - j];
      for (j = 1; j < k_den; j++) {
	idx = (curr_idx[i] - 1 + j) % filter->units[i].den_degree;
	curr_out[i] -= filter->units[i].den[j] * history[i][idx];
      }

/* Update history */
/* move marker in circolar buffer */
      if (filter->units[i].den_degree > 1) {
	curr_idx[i]--;
	if (curr_idx[i] < 0)
	  curr_idx[i] = filter->units[i].den_degree - 1;
	history[i][curr_idx[i]] = curr_out[i];
      }
    }

/* store sample in out if needed */
    if (curr_sample >= start && !((curr_sample - start) % delta)) {
      for (i = 0, out[out_idx] = 0; i < filter->num_parallels; i++)
	out[out_idx] += curr_out[i];
      out_idx++;
    }
  }

  *max_out_samples = out_idx;

/* Free storage */
  for (i = 0; i < filter->num_parallels; i++)
    if (filter->units[i].den_degree > 0)
      free((char *)history[i]);
  free((char *)history);
  free((char *)curr_out);
  free((char *)curr_idx);
/* First sample equal for input and output */
  if (start == 0)
    out[0] = in[start];
}

EXPORT void TestFilter(Filter * filter, float fc, int n_points, float *module, float *phase)
{
  float curr_f, step_f;
  int i, j, k, idx;
  Complex curr_fc, curr_c, curr_fac, curr_num, curr_den, curr_z;

/* normalize if not already done */
  NormalizeFilter(filter);

  step_f = fc / (2 * (n_points + 1));
  for (curr_f = step_f, idx = 0; idx < n_points; idx++, curr_f += step_f) {
    curr_fc.re = cos(2 * PI * curr_f / fc);
    curr_fc.im = sin(2 * PI * curr_f / fc);
    curr_z.im = curr_z.re = 0;
    for (i = 0; i < filter->num_parallels; i++) {
      curr_num.re = curr_num.im = 0;
      for (j = 0; j < filter->units[i].num_degree; j++) {
	curr_c.re = 1;
	curr_c.im = 0;
	curr_fac.im = 0;
	curr_fac.re = filter->units[i].num[j];
	for (k = 0; k < j; k++)
	  curr_c = MulC(curr_c, curr_fc);
	curr_num = AddC(curr_num, MulC(curr_c, curr_fac));
      }
      curr_den.re = curr_den.im = 0;
      for (j = 0; j < filter->units[i].den_degree; j++) {
	curr_c.re = 1;
	curr_c.im = 0;
	curr_fac.im = 0;
	curr_fac.re = filter->units[i].den[j];
	for (k = 0; k < j; k++)
	  curr_c = MulC(curr_c, curr_fc);
	curr_den = AddC(curr_den, MulC(curr_c, curr_fac));
      }
      if (filter->units[i].den_degree > 0)
	curr_z = AddC(curr_z, DivC(curr_num, curr_den));
      else
	curr_z = AddC(curr_z, curr_num);
    }
    module[idx] = sqrt(Mod2(curr_z));

    //This ckeck is required to avoid floating point underflow!!
    if (fabs(curr_z.re) < 1E-10)
      curr_z.re = 0;
    if (fabs(curr_z.im) < 1E-10)
      curr_z.im = 0;
    if (curr_z.re == 0 && curr_z.im == 0) {
      phase[idx] = 0;
      continue;
    }

    if (curr_z.re < 0) {
      if (curr_z.im >= 0)
	if (fabs(curr_z.re) > fabs(curr_z.im))
	  phase[idx] = PI + atan(curr_z.im / curr_z.re);
	else
	  phase[idx] = PI / 2 - atan(curr_z.re / curr_z.im);
      else if (fabs(curr_z.re) > fabs(curr_z.im))
	phase[idx] = PI + atan(curr_z.im / curr_z.re);
      else
	phase[idx] = 3 * PI / 2 - atan(curr_z.re / curr_z.im);
    } else {			/* curr_z.re > 0 */

      if (curr_z.im >= 0)
	if (fabs(curr_z.re) > fabs(curr_z.im))
	  phase[idx] = atan(curr_z.im / curr_z.re);
	else
	  phase[idx] = PI / 2 - atan(curr_z.re / curr_z.im);
      else if (fabs(curr_z.re) > fabs(curr_z.im))
	phase[idx] = 2 * PI + atan(curr_z.im / curr_z.re);
      else
	phase[idx] = 3 * PI / 2 - atan(curr_z.re / curr_z.im);
    }
    phase[idx] = -phase[idx];
  }
}

EXPORT void FreeFilter(Filter * filter)
{
  int i;

  for (i = 0; i < filter->num_parallels; i++) {
    free((char *)filter->units[i].num);
    if (filter->units[i].den_degree)
      free((char *)filter->units[i].den);
  }
  free((char *)filter->units);
  free((char *)filter);
}

static void NormalizeFilter(Filter * filter)
{
  int i, j;

  for (i = 0; i < filter->num_parallels; i++) {
    if (filter->units[i].den_degree > 0) {
      for (j = 0; j < filter->units[i].num_degree; j++)
	filter->units[i].num[j] /= filter->units[i].den[0];
      for (j = 1; j < filter->units[i].den_degree; j++)
	filter->units[i].den[j] /= filter->units[i].den[0];
      filter->units[i].den[0] = 1;
    }
  }
}

//Prepare the description of a butterworth 
EXPORT Filter *prepareFilter(float cutFreq, float samplingFreq, int numPoles)
{
  Filter *outFilter;
  float zero = 0;
  outFilter = (Filter *) ButtwInvar(&cutFreq, &zero, &zero, &zero, &samplingFreq, &numPoles);
  NormalizeFilter(outFilter);
  return outFilter;
}

//Filter *ButtwInvar(float *fp, float *fs, float *ap, float *as, float *fc, int *out_n);

//Initialize then data structures whichb are required for real-time filtering
EXPORT void initializeRunTimeFilter(RunTimeFilter * rtf)
{
  RunTimeFilter *runTimeFilter = (RunTimeFilter *) rtf;
  memset(runTimeFilter->oldX, 0, MAX_FILTER_BUF * sizeof(double));
  memset(runTimeFilter->oldY, 0, MAX_FILTER_UNITS * MAX_FILTER_BUF * sizeof(double));
  runTimeFilter->idx = 0;
}

/* Perform step filtering */
EXPORT double getFiltered(double in, Filter * flt, RunTimeFilter * rtf)
{
  int i, j;
  double totOut = 0, currOut;
  Filter *filter = (Filter *) flt;
  RunTimeFilter *runFilter = (RunTimeFilter *) rtf;

  int idx;
  int currIdx;
  double *oldX;
  idx = runFilter->idx;
  oldX = runFilter->oldX;

  for (i = 0; i < filter->num_parallels; i++) {

    currOut = filter->units[i].num[0] * in;
    for (j = 1; j < filter->units[i].num_degree; j++) {
      currIdx = idx - j;
      if (currIdx < 0)
	currIdx += MAX_FILTER_BUF;
      currOut += filter->units[i].num[j] * oldX[currIdx];
    }
    for (j = 1; j < filter->units[i].den_degree; j++) {
      currIdx = idx - j;
      if (currIdx < 0)
	currIdx += MAX_FILTER_BUF;
      currOut -= filter->units[i].den[j] * runFilter->oldY[i][currIdx];
    }
    runFilter->oldY[i][idx] = currOut;
    totOut += currOut;

  }
  oldX[idx] = in;
  idx++;
  if (idx >= MAX_FILTER_BUF)
    idx -= MAX_FILTER_BUF;
  runFilter->idx = idx;
  return totOut;
}

