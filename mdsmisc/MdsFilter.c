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
/*  DEC/CMS REPLACEMENT HISTORY, Element MDS$FILTER.C */
/*  *10   13-SEP-1996 16:13:43 AI "" */
/*  *9    14-MAR-1995 17:14:59 MDSPLUS "Ansi C" */
/*  *8    22-NOV-1994 14:39:19 MDSPLUS "Change signal format" */
/*  *7    21-NOV-1994 14:26:27 MDSPLUS "Test" */
/*  *6    21-NOV-1994 14:24:57 MDSPLUS "Boh?" */
/*  *5    21-NOV-1994 14:13:43 MDSPLUS "Boh?" */
/*  *4    21-NOV-1994 14:08:37 MDSPLUS "Boh?" */
/*  *3    21-NOV-1994 14:07:40 MDSPLUS "Boh?" */
/*  *2    21-NOV-1994 13:53:26 MDSPLUS "Boh?" */
/*  *1    21-NOV-1994 13:11:53 MDSPLUS "Low pass filter (10 poles Butterworth)" */
/*  DEC/CMS REPLACEMENT HISTORY, Element MDS$FILTER.C */
/*------------------------------------------------------------------------------

	Name:	MDS$FILTER   

	Type:   C function

	Author:	Gabriele Manduchi
		Istituto Gas Ionizzati del CNR - Padova (Italy)

	Date:   21-NOV-1994

	Purpose: Filter a signal using a 10 poles Butterworth filter and Impulse Invariance Method
-------------------------------------------------------------------------------- */
#include <mdsdescrip.h>
#include <stdlib.h>
#include <mdsshr.h>
#include <stdio.h>
#include <math.h>
#include "filter.h"

EXPORT struct descriptor_xd *MdsFilter(float *in_data, float *in_dim, int *size, float *cut_off,
				int *num_in_poles)
{
  static struct descriptor_xd out_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };

  DESCRIPTOR_A(data_d, sizeof(float), DTYPE_FLOAT, 0, 0);
  DESCRIPTOR_SIGNAL(signal_d, 1, 0, 0);
  DESCRIPTOR_DIMENSION(dimension_d, 0, 0);
  DESCRIPTOR_WINDOW(window_d, 0, 0, 0);
  DESCRIPTOR_RANGE(range_d, 0, 0, 0);

  struct descriptor start_d = { sizeof(float), DTYPE_FLOAT, CLASS_S, 0 }, end_d = {
  sizeof(float), DTYPE_FLOAT, CLASS_S, 0}, delta_d = {
  sizeof(float), DTYPE_FLOAT, CLASS_S, 0}, start_idx_d = {
  sizeof(int), DTYPE_L, CLASS_S, 0}, end_idx_d = {
  sizeof(int), DTYPE_L, CLASS_S, 0}, time_at_0_d = {
  sizeof(float), DTYPE_FLOAT, CLASS_S, 0};

  int num_samples, num_poles, start_idx, end_idx, i;
  float fc, delta, dummy, *filtered_data, start, end, time_at_0;
  float phs_steep;
  float delay = 0.0f;
  static Filter *filter;

  if (*num_in_poles > 0)
    num_poles = *num_in_poles;
  else
    num_poles = 10;

  signal_d.data = (struct descriptor *)&data_d;
  signal_d.dimensions[0] = (struct descriptor *)&dimension_d;
  dimension_d.window = (struct descriptor *)&window_d;
  dimension_d.axis = (struct descriptor *)&range_d;
  window_d.startidx = (struct descriptor *)&start_idx_d;
  window_d.endingidx = (struct descriptor *)&end_idx_d;
  window_d.value_at_idx0 = (struct descriptor *)&time_at_0_d;
  start_idx_d.pointer = (char *)&start_idx;
  end_idx_d.pointer = (char *)&end_idx;
  time_at_0_d.pointer = (char *)&time_at_0;
  range_d.begin = (struct descriptor *)&start_d;
  range_d.ending = (struct descriptor *)&end_d;
  range_d.deltaval = (struct descriptor *)&delta_d;
  start_d.pointer = (char *)&start;
  end_d.pointer = (char *)&end;
  delta_d.pointer = (char *)&delta;

  num_samples = *size;

  fc = 1 / (in_dim[1] - in_dim[0]);
  filter = ButtwInvar(cut_off, &dummy, &dummy, &dummy, &fc, &num_poles);

  filtered_data = (float *)malloc(num_samples * sizeof(float));
  float mod[1000];
  float phs[1000];
  TestFilter(filter, fc, 1000, mod, phs);

  for (i = 1; i < 1000 - 1 && !isnan(phs[i]) && !isnan(phs[i + 1]) && phs[i] > phs[i + 1]; i++) ;

  if (i > 1) {
    phs_steep = (phs[1] - phs[i]) / ((i / 1000.) * fc / 2.);
    delay = phs_steep / (2 * PI);
  }

  DoFilter(filter, in_data, filtered_data, &num_samples);
  FreeFilter(filter);
  data_d.pointer = (char *)filtered_data;
  data_d.arsize = num_samples * sizeof(float);
  start = in_dim[0] - delay;
  end = in_dim[num_samples - 1] - delay;
  delta = in_dim[1] - in_dim[0];
  start_idx = 0;
  end_idx = num_samples - 1;
  time_at_0 = in_dim[0] - delay;

  MdsCopyDxXd((struct descriptor *)&signal_d, &out_xd);
  free((char *)filtered_data);

  return &out_xd;
}

EXPORT void PrintFilter(Filter * filter)
{
  int i, j;

  for (i = 0; i < filter->num_parallels; i++) {
    //if(filter->units[i].den_degree > 0)
    {
      for (j = 0; j < filter->units[i].num_degree; j++)
	printf("%f ", filter->units[i].num[j]);
      for (j = 0; j < filter->units[i].den_degree; j++)
	printf("%f ", filter->units[i].den[j]);

      printf("\n");
    }
  }
}

