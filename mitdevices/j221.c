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
#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <treeshr.h>
#include <mdsshr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "j221_gen.h"
#include "devroutines.h"

static int one = 1;
static short zero = 0;
#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;
#define do_if_no_error(f,retstatus) if (status & 1) retstatus = f;
#define pio(f,a,d)  status = DevCamChk(CamPiow(setup->name,a,f,d,16,0),&one,&one);
#define pion(f,a,d)  do_if_no_error(DevCamChk(CamPiow(setup->name,a,f,d,16,0),&one,&one),status);

static int merge_data(int nid, int **data, int **times, int *ndata);

EXPORT int j221___init(struct descriptor *nid __attribute__ ((unused)), InInitStruct * setup)
{
  int status = 1;
  int merge_status;
  int num, *time, *bit;
  static float dt = 1e-6;
  static DESCRIPTOR_FLOAT(dt_d, &dt);
  static DESCRIPTOR_RANGE(int_clock, 0, 0, &dt_d);
  float trigger;
  int clock_nid = setup->head_nid + J221_N_CLOCK;
  int clock_divide_nid = setup->head_nid + J221_N_CLOCK_DIVIDE;
  struct j221_registers {
    unsigned j221_setup_v_output:1;
    unsigned j221_setup_v_sequence:1;
    unsigned j221_setup_v_start:1;
    unsigned j221_setup_v_active:1;
    unsigned j221_setup_v_hold:1;
    unsigned j221_setup_v_divide:1;
    unsigned j221_setup_v_clock:1;
    unsigned:25;
  } registers;
  pio(9, 0, 0);			/* Clear the memory address */
  pio(1, 0, (short *)&registers);
  if (registers.j221_setup_v_clock == 0) {
    TreePutRecord(clock_nid, (struct descriptor *)&int_clock, 0);
  }
  if (registers.j221_setup_v_divide != 0) {
    static int divide = 10;
    static DESCRIPTOR_LONG(divide_dsc, &divide);
    TreePutRecord(clock_divide_nid, (struct descriptor *)&divide_dsc, 0);
  }

  merge_status = merge_data(setup->head_nid + J221_N_OUTPUT_01, &bit, &time, &num);
  if (num) {
    int start_trig_nid = setup->head_nid + J221_N_START_TRIG;
    if (status & 1) {
      int i;
      for (i = 0, status = 0; (i < 10) && !(status & 1); i++) {
	pio(16, 2, &zero);	/* Clear the memory address */
	if (status)
	  status = CamStopw(setup->name, 1, 16, num, time, 24, 0);	/* Load trigger times */
      }
      if (i > 1)
	printf("******J221 needed %d tries on the first STOPW******\n", i);
    }
    if (status & 1) {
      int i;
      for (i = 0, status = 0; (i < 10) && !(status & 1); i++) {
	static short zero = 0;
	pio(16, 2, &zero);	/* Clear the memory address */
	if (status)
	  status = CamStopw(setup->name, 0, 16, num, bit, 16, 0);	/* Load Output words */
      }
      if (i > 1)
	printf("******J221 needed %d tries on the second STOPW******\n", i);
    }
    pion(9, 0, 0);		/* Clear the memory address */
    pion(26, 0, 0);		/* Enable the Lam */
    pion(26, 1, 0);		/* Enable the output word */
    if (DevFloat(&start_trig_nid, &trigger) & 1) {	/* If  trigger then  */
      pion(26, 2, 0);		/*   Enable the external trigger */
    }
    free(time);
    free(bit);
  }
  return status & 1 ? merge_status : status;
}

static int merge_data(int nid, int **data, int **times, int *ndata)
{
  int i, j, xdsize, *wave_pointer[12], *orig_wave_pointer[12], wave_arsize[12];
  short *output;
  int *setpoint, next, status = 1;

  *ndata = 0;
  for (i = 0; i < 12; i++) {
    static struct descriptor_xd xd = { 0, DTYPE_T, CLASS_XD, 0, 0 };
    int chan_nid =
	nid + i * (J221_N_OUTPUT_02 - J221_N_OUTPUT_01) + J221_N_OUTPUT_01_SET_POINTS -
	J221_N_OUTPUT_01;
    int mode_nid =
	nid + i * (J221_N_OUTPUT_02 - J221_N_OUTPUT_01) + J221_N_OUTPUT_01_MODE - J221_N_OUTPUT_01;
    int width = 0;
    wave_arsize[i] = 0;
    if (TreeIsOn(chan_nid) & 1 && TreeGetRecord(chan_nid, &xd) != TreeNODATA) {
      if (TdiData((struct descriptor *)&xd, &xd MDS_END_ARG) & 1) {
	if (TdiLong((struct descriptor *)&xd, &xd MDS_END_ARG) & 1) {
	  if (xd.pointer->class == CLASS_A) {	/* Not array type? */
	    if (!(DevLong(&mode_nid, &width) & 1))
	      width = 0;
	    xdsize = (((struct descriptor_a *)xd.pointer)->arsize / sizeof(int));
	    if (xdsize) {
	      int ix, last = -1;
	      for (j = 0; j < xdsize * 4; j += 4) {	/* Test for unordered or overlapping data */
		ix = *(int *)(((struct descriptor_a *)xd.pointer)->pointer + j);
		if (ix <= last) {
		  wave_arsize[i] = -1;
		  break;
		}
		last = ix + width;
	      }
	      if (!wave_arsize[i]) {
		if (width)
		  xdsize = 2 * xdsize;
		wave_pointer[i] = malloc((xdsize + 1) * 4);
		orig_wave_pointer[i] = wave_pointer[i];
		wave_arsize[i] = xdsize;
		if (!width)
		  memcpy(wave_pointer[i], ((struct descriptor_a *)xd.pointer)->pointer,
			 wave_arsize[i] * 4);
		else
		  for (j = 0; j < xdsize; j += 2)
		    wave_pointer[i][j + 1] = width +
			(wave_pointer[i][j] =
			 *(int *)(((struct descriptor_a *)xd.pointer)->pointer + j * 2));
		wave_pointer[i][xdsize] = 16777215;
		*ndata += xdsize;
	      }
	    }
	  } else
	    wave_arsize[i] = -1;
	} else
	  wave_arsize[i] = -1;
      } else
	wave_arsize[i] = -1;
      MdsFree1Dx(&xd, 0);
    }
    if (wave_arsize[i] == -1) {
      printf("Bad j221 channel data\n");
      status = J221$_INVALID_DATA;
      wave_arsize[i] = 0;
    }
  }
  if (!*ndata)
    return status;
  *ndata += 1;			/* Make sure space for 16777215 */
  output = malloc(*ndata * 2);
  setpoint = malloc(*ndata * 4);
  *data = (int *)output;
  *times = (int *)setpoint;
  j = 0;
  output[j] = 0;
  next = 0;
  while (next < 16777215) {
    next = 16777215;
    for (i = 0; i < 12; i++)
      if (wave_arsize[i] && *wave_pointer[i] < next)
	next = *wave_pointer[i];
    if (next < 16777215) {
      for (i = 0; i < 12; i++)
	if (wave_arsize[i] && *wave_pointer[i] == next) {
	  output[j] = output[j] ^ (1 << i);
	  wave_pointer[i]++;
	}
      setpoint[j++] = next;
      output[j] = output[j - 1];
    }
  }
  setpoint[j] = 16777215;
  *ndata = j + 1;
  for (i = 0; i < 12; i++)
    if (wave_arsize[i]) {
      free(orig_wave_pointer[i]);
    }
  return status;
}

EXPORT int j221___add(int *head_nid)
{
  int i;
  static int len;
  static DESCRIPTOR_LONG(len_dsc, &len);
  static int c_nid;
  static DESCRIPTOR_NID(nid_dsc, &c_nid);
  static DESCRIPTOR(check, "GETNCI($,\"LENGTH\")");
  int status;
  c_nid = *head_nid + J221_N_OUTPUT_01;
  status = TdiExecute((struct descriptor *)&check, &nid_dsc, &len_dsc MDS_END_ARG);
  if ((status & 1) && (len > 0))
    return status;
  for (i = 0; i < 12; i++) {
    static DESCRIPTOR(trigs, "I_TO_X(BUILD_DIM(,$1),($2 > 0 ? $3 : $3[0:*:2]) ) * $4 + $5");
    static DESCRIPTOR(gates,
		      "I_TO_X(BUILD_DIM(,$1),($2 > 0 ? SPREAD([0,$2],1,SIZE($3))+SPREAD($3,0,2) : $3) ) * $4 + $5");
    static DESCRIPTOR(signal,
		      "2*BUILD_SIGNAL([0,REPLICATE([1,0],0,ESIZE($1)/2),ESIZE($1) MOD 2 ? [1,1] : 0],\
           *,[MINVAL($1)-.1*(MAXVAL($1)-MINVAL($1)),$1,MAXVAL($1)+.1*(MAXVAL($1)-MINVAL($1))])");
    static int clock_nid;
    static DESCRIPTOR_NID(clock_nid_d, &clock_nid);
    static int mode_nid;
    static DESCRIPTOR_NID(mode_nid_d, &mode_nid);
    static int setpoints_nid;
    static DESCRIPTOR_NID(set_points_nid_d, &setpoints_nid);
    static int divide_nid;
    static DESCRIPTOR_NID(divide_nid_d, &divide_nid);
    static int start_trig_nid;
    static DESCRIPTOR_NID(start_trig_nid_d, &start_trig_nid);
    static int gates_nid;
    static DESCRIPTOR_NID(gates_nid_d, &gates_nid);
    static EMPTYXD(xd);
    int chan_nid = *head_nid + J221_N_OUTPUT_01 + (J221_N_OUTPUT_02 - J221_N_OUTPUT_01) * i;
    int triggers_nid = chan_nid + J221_N_OUTPUT_01_TRIGGERS - J221_N_OUTPUT_01;
    gates_nid = chan_nid + J221_N_OUTPUT_01_GATES - J221_N_OUTPUT_01;
    clock_nid = *head_nid + J221_N_CLOCK;
    mode_nid = chan_nid + J221_N_OUTPUT_01_MODE - J221_N_OUTPUT_01;
    setpoints_nid = chan_nid + J221_N_OUTPUT_01_SET_POINTS - J221_N_OUTPUT_01;
    divide_nid = *head_nid + J221_N_CLOCK_DIVIDE;
    start_trig_nid = *head_nid + J221_N_START_TRIG;
    TdiCompile((struct descriptor *)&trigs, &clock_nid_d, &mode_nid_d, &set_points_nid_d, &divide_nid_d,
	       &start_trig_nid_d, &xd MDS_END_ARG);
    TreePutRecord(triggers_nid, (struct descriptor *)&xd, 0);
    TdiCompile((struct descriptor *)&gates, &clock_nid_d, &mode_nid_d, &set_points_nid_d, &divide_nid_d,
	       &start_trig_nid_d, &xd MDS_END_ARG);
    TreePutRecord(gates_nid, (struct descriptor *)&xd, 0);
    TdiCompile((struct descriptor *)&signal, &gates_nid_d, &xd MDS_END_ARG);
    TreePutRecord(chan_nid, (struct descriptor *)&xd, 0);
  }
  return status;
}

EXPORT int j221___trigger(struct descriptor *nid __attribute__ ((unused)), InTriggerStruct * setup)
{
  int status;
  pio(25, 0, 0);		/* Start it running */
  return status;
}
