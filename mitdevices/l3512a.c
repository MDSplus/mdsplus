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
#include <libroutines.h>
#include "l3512a_gen.h"
#include "devroutines.h"

//extern unsigned short OpcValue;




typedef struct {
  unsigned range:3;
  unsigned mult:4;
  unsigned fill:9;
} DwellCode;

static DwellCode ConvertDwell();

static int one = 1;
static int zero = 0;

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))
#define max_tries 10
#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;
#define pio(name,f,a,d,mem)  return_on_error(DevCamChk(CamPiow(name, a, f, d, mem, 0), &one, 0),status)
#define stop(name,f,a,c,d)  return_on_error(DevCamChk(CamStopw(name, a, f, c, d, 16, 0), &one, 0),status)
#define fstop(name,f,a,c,d)  return_on_error(DevCamChk(CamFStopw(name, a, f, c, d, 16, 0), &one, 0),status)

EXPORT int l3512a___init(struct descriptor *niddsc __attribute__ ((unused)), InInitStruct * setup)
{
  int status;
  float duration;
  DwellCode dwell_code;
  struct {
    unsigned offset:8;
    unsigned gain:3;
    unsigned decrement:1;
    unsigned multi:1;
    unsigned camac:1;
    unsigned not_used:1;
    unsigned disable:1;
  } setup_3512 = {
  0, 0, 0, 0, 1, 0, 0};
  struct {
    unsigned enable:1;
    unsigned autostart:1;
    unsigned rescan:1;
    unsigned fill:13;
  } setup_3587 = {
  1, 0, 0, 0};
  int i;
  int tries;
  static int kind;
  static DESCRIPTOR_LONG(kind_dsc, &kind);
  if ((TdiKind(setup->duration, &kind_dsc MDS_END_ARG) & 1) && (kind <= 10 || kind == DTYPE_FS)) {
    int duration_nid = setup->head_nid + L3512A_N_DURATION;
    return_on_error(DevFloat(&duration_nid, &duration), status);
  } else
    duration = 0.0;
  setup_3512.offset = min(max(setup->offset * setup->num_channels / 256, 0), 255);
  for (i = 8196 / min(max(setup->num_channels, 256), 8192), setup_3512.gain = 0; i;
       setup_3512.gain++, i = i / 2) ;
  setup_3512.disable = 1;
  pio(setup->name, 16, 0, (short *)&setup_3512, 16);
  pio(setup->name, 10, 0, 0, 16);
  setup_3512.disable = 0;
  pio(setup->name, 16, 0, (short *)&setup_3512, 16);
  pio(setup->memory_name, 9, 0, 0, 16);
  pio(setup->memory_name, 26, 0, 0, 16);
  pio(setup->memory_name, 8, 0, 0, 16);
  for (tries = 0; tries < max_tries && !(CamQ(0) & 1); tries++) {
    float wait = .2;
    LibWait(&wait);
    pio(setup->memory_name, 8, 0, 0, 16);
  }
  pio(setup->memory_name, 10, 0, 0, 16);
  pio(setup->memory_name, 17, 0, &zero, 16);
  pio(setup->memory_name, 11, 0, 0, 16);
  pio(setup->router_name, 9, 0, 0, 16);
  pio(setup->router_name, 17, 0, &zero, 16);
  pio(setup->router_name, 17, 1, &setup->num_spectra, 16);
  dwell_code = ConvertDwell(&duration);
  pio(setup->router_name, 17, 2, &dwell_code, 16);
  pio(setup->router_name, 17, 3, (short *)&setup_3587, 16);
  return status;
}

static DwellCode ConvertDwell(float *dwell)
{
  DwellCode answer;
  float range;
  int i;
  answer.fill = 0;
  if (*dwell < 1E-4) {
    answer.range = 0;
    answer.mult = 0;
  } else {
    for (range = 1E-4, i = 1; i < 7 && !(*dwell >= range && *dwell < (range * 9.5));
	 i++, range *= 10.) ;
    answer.range = i;
    answer.mult = min(max(*dwell / range, 0), 9);
  }
  return answer;
}

EXPORT int l3512a___store(struct descriptor *niddsc __attribute__ ((unused)), InStoreStruct * setup)
{

  unsigned short data[32768];
  int status;
  static float duration;
  //int i;
  static int kind;
  static DESCRIPTOR_LONG(kind_dsc, &kind);
  int values;
  int num_to_read;
  int comment_nid = setup->head_nid + L3512A_N_COMMENT;

  if ((TdiKind(setup->duration, &kind_dsc MDS_END_ARG) & 1) && (kind <= 10 || kind == DTYPE_FS)) {
    int duration_nid = setup->head_nid + L3512A_N_DURATION;
    return_on_error(DevFloat(&duration_nid, &duration), status);
  } else
    duration = 0.0;
  pio(setup->memory_name, 19, 0, 0, 16);	/* Disable port for external histogram */
  pio(setup->memory_name, 25, 0, 0, 16);	/* Increment mode */
  pio(setup->memory_name, 18, 1, &zero, 16);	/* Set memory to location 0 for reads */
  values = setup->num_spectra * setup->num_channels;
  num_to_read = min(32767, values);
  if ((TreeIsOn(comment_nid) & 1) && (values < 32768)) {
    fstop(setup->memory_name, 2, 0, num_to_read, data);
  } else {
    stop(setup->memory_name, 2, 0, num_to_read, data);
  }
  if (values > 32767)
    pio(setup->memory_name, 2, 0, &data[32767], 16);
  {
    static int duration_nid;
    static DESCRIPTOR_NID(duration_niddsc, &duration_nid);
    static DESCRIPTOR_A_COEFF_2(raw, 2, DTYPE_W, 0, 0, 0, 0);
    static DESCRIPTOR_FUNCTION_1(value, (unsigned char *)&OpcValue, 0);
    static int zero_val = 0;
    static DESCRIPTOR_LONG(zero, &zero_val);
    static int endrange_val;
    static DESCRIPTOR_LONG(endrange, &endrange_val);
    static int one_val = 1;
    static DESCRIPTOR_LONG(one, &one_val);
    static DESCRIPTOR_RANGE(dim1, &zero, &endrange, &one);
    static float endrange_val2;
    static DESCRIPTOR_FLOAT(endrange2, &endrange_val2);
    static DESCRIPTOR_FLOAT(dwell_d, &duration);
    static DESCRIPTOR_RANGE(dim2, &zero, &endrange2, &dwell_d);
    static DESCRIPTOR_SIGNAL_2(signal, &value, &raw, &dim1, 0);
    int spectra_nid = setup->head_nid + L3512A_N_SPECTRA;
    value.ndesc = 0;
    raw.pointer = raw.a0 = (char *)data;
    raw.arsize = min(65536, setup->num_channels * setup->num_spectra * sizeof(short));
    raw.m[0] = setup->num_channels;
    endrange_val = setup->num_channels - 1;
    endrange_val2 = duration * (setup->num_spectra - 1);
    raw.m[1] = setup->num_spectra;
    duration_nid = setup->head_nid + L3512A_N_DURATION;
    signal.dimensions[1] =
	(duration > 0.0 ? (struct descriptor *)&dim2 : (struct descriptor *)&duration_niddsc);
    status = TreePutRecord(spectra_nid, (struct descriptor *)&signal, 0);
  }
  return status;
}
