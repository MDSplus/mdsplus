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
#include <Mrm/MrmPublic.h>
#include <Xm/Xm.h>
#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <ncidef.h>
#include <treeshr.h>
#include <mdsshr.h>
#include <xmdsshr.h>
#include <libroutines.h>
#include <stdlib.h>
#include "dsp2904_gen.h"
#include "devroutines.h"

static int one = 1;
#ifdef pio
#undef pio
#endif
#define pio(f,a,d,mem)  return_on_error(DevCamChk(CamPiow(setup->name, a, f, d, mem, 0), &one, 0),status)
#define return_on_error(f,retstatus) if (!((status = f) & 1)) {return retstatus;}

static unsigned int Input(InStoreStruct * setup, int code);
static int AccessTraq(InStoreStruct * setup, int data, int memsize);
static int ReadChannel(InStoreStruct * setup, int channel, int num, unsigned short *buffer);
static void FixMenu(Widget w);


extern int DevWait(float);
EXPORT int dsp2904___add(int *head_nid)
{
  int counter_bits_nid = *head_nid + DSP2904_N_COUNTER_BITS_32;
  int timer_bits_nid = *head_nid + DSP2904_N_TIMER_BITS_32;
  TreeTurnOff(counter_bits_nid);
  TreeTurnOff(timer_bits_nid);
  return 1;
}

EXPORT int dsp2904___init(struct descriptor *niddsc_ptr __attribute__ ((unused)), InInitStruct * setup)
{
  int status;
  typedef struct _ModeReg {
    unsigned gate:1;
    unsigned wrap:1;
    unsigned bits32:1;
    unsigned time_of_count:1;
    unsigned idx_divider:1;
    unsigned:11;
  } ModeReg;
  static ModeReg mode;
  float rate;
  int rate_nid = setup->head_nid + DSP2904_N_RATE;
  int rateidx;
  int c_preset;
  int counter_preset_nid = setup->head_nid + DSP2904_N_COUNTER_PRESET;
  int t_preset;
  int timer_preset_nid = setup->head_nid + DSP2904_N_TIMER_PRESET;
  //float option;
  mode.gate = XmdsIsOn(setup->head_nid + DSP2904_N_COUNTER_GATE);
  mode.wrap = XmdsIsOn(setup->head_nid + DSP2904_N_COUNTER_WRAP);
  mode.bits32 = XmdsIsOn(setup->head_nid + DSP2904_N_COUNTER_BITS_32);
  mode.time_of_count = 0;
  mode.idx_divider = XmdsIsOn(setup->head_nid + DSP2904_N_IDX_DIVIDER);
  pio(16, 0, &mode, 16);
  mode.gate = XmdsIsOn(setup->head_nid + DSP2904_N_TIMER_GATE);
  mode.wrap = XmdsIsOn(setup->head_nid + DSP2904_N_TIMER_WRAP);
  mode.bits32 = XmdsIsOn(setup->head_nid + DSP2904_N_TIMER_BITS_32);
  mode.time_of_count = XmdsIsOn(setup->head_nid + DSP2904_N_TIME_OF_CNT);
  mode.idx_divider = 0;
  pio(16, 1, &mode, 16);
  if ((status = DevFloat(&rate_nid, &rate)) & 1) {
    float option;
    for (rateidx = 0, option = 100E-9; (rateidx < 32) && (rate > option); rateidx++, option *= 2) ;
    if (rateidx == 32)
      rateidx--;
  } else if (status == TreeNODATA)
    rateidx = 32;
  pio(16, 2, &rateidx, 16);
  if (!(DevLong(&timer_preset_nid, &t_preset) & 1))
    t_preset = 0;
  pio(17, 0, ((short *)&t_preset) + 1, 16);
  pio(17, 1, &t_preset, 16);
  if (!(DevLong(&counter_preset_nid, &c_preset) & 1))
    c_preset = 0;
  pio(17, 2, ((short *)&c_preset) + 1, 16);
  pio(17, 3, &c_preset, 16);
  return status;
}

#ifdef pio
#undef pio
#endif
#define pio(f,a,d,mem)  return_on_error(DevCamChk(CamPiow(setup->traq_name, a, f, d, mem,0), &one, 0),status)
EXPORT int dsp2904___store(struct descriptor *niddsc __attribute__ ((unused)), InStoreStruct * setup)
{
  int status;
  int t_bits_32;
  int c_bits_32;
  struct _t4012_status {
    unsigned sampling:1;
    unsigned calibrate:1;
    unsigned master_armed:1;
    unsigned master_enabled:1;
    unsigned stop_received:1;
    unsigned triggered:1;
    unsigned t4012p:1;
    unsigned cal_mem:1;
    unsigned:24;
  } dig_status;
  int samples;
  unsigned short *low_word = 0;
  unsigned short *high_word = 0;
  unsigned short (*data_long)[2] = 0;
  //int channel;
  int timer_bits_32 = setup->head_nid + DSP2904_N_TIMER_BITS_32;
  int counter_bits_32 = setup->head_nid + DSP2904_N_COUNTER_BITS_32;
  int timer_nid = setup->head_nid + DSP2904_N_TIMER;
  int counter_nid = setup->head_nid + DSP2904_N_COUNTER;
  status = Input(setup, 14);
  dig_status = *(struct _t4012_status *)&status;
  if (dig_status.sampling) {
    return DEV$_NOT_TRIGGERED;
  }
  samples = Input(setup, 3) * 1024;
  if (samples <= 0)
    return DEV$_NOT_TRIGGERED;
  t_bits_32 = TreeIsOn(timer_bits_32) & 1;
  c_bits_32 = TreeIsOn(counter_bits_32) & 1;
  low_word = malloc(samples * 2);
  if (t_bits_32 || c_bits_32) {
    high_word = malloc(samples * 2);
    data_long = malloc(samples * 4);
  }
  if (TreeIsOn(timer_nid) & 1) {
    int channel_nid = setup->head_nid + DSP2904_N_TIMER_CHANNEL;
    int channel;
    return_on_error(DevLong(&channel_nid, &channel), status);
    if ((status = ReadChannel(setup, channel++, samples, low_word)) & 1) {
      if (t_bits_32) {
	if ((status = ReadChannel(setup, channel++, samples, high_word)) & 1) {
	  int i;
	  DESCRIPTOR_A(raw, sizeof(*data_long), DTYPE_LU, 0, 0);
	  raw.pointer = (char *)data_long;
	  raw.arsize = samples * sizeof(*data_long);
	  for (i = 0; i < samples; i++) {
	    data_long[i][0] = low_word[i];
	    data_long[i][1] = high_word[i];
	  }
	  status = TreePutRecord(timer_nid, (struct descriptor *)&raw, 0);
	}
      } else {
	DESCRIPTOR_A(raw, sizeof(*low_word), DTYPE_WU, 0, 0);
	raw.pointer = (char *)low_word;
	raw.arsize = samples * sizeof(*low_word);
	status = TreePutRecord(timer_nid, (struct descriptor *)&raw, 0);
      }
    }
  }
  if ((status & 1) && (TreeIsOn(counter_nid) & 1)) {
    int channel_nid = setup->head_nid + DSP2904_N_COUNTER_CHANNEL;
    int channel;
    return_on_error(DevLong(&channel_nid, &channel), status);
    if ((status = ReadChannel(setup, channel++, samples, low_word)) & 1) {
      if (c_bits_32) {
	if ((status = ReadChannel(setup, channel++, samples, high_word)) & 1) {
	  int i;
	  DESCRIPTOR_A(raw, sizeof(*data_long), DTYPE_LU, 0, 0);
	  raw.pointer = (char *)data_long;
	  raw.arsize = samples * sizeof(*data_long);
	  for (i = 0; i < samples; i++) {
	    data_long[i][0] = low_word[i];
	    data_long[i][1] = high_word[i];
	  }
	  status = TreePutRecord(counter_nid, (struct descriptor *)&raw, 0);
	}
      } else {
	DESCRIPTOR_A(raw, sizeof(*low_word), DTYPE_WU, 0, 0);
	raw.pointer = (char *)low_word;
	raw.arsize = samples * sizeof(*low_word);
	status = TreePutRecord(counter_nid, (struct descriptor *)&raw, 0);
      }
    }
  }
  free(low_word);
  if (t_bits_32 || c_bits_32) {
    free(high_word);
    free(data_long);
  }
  return status;
}

static int ReadChannel(InStoreStruct * setup, int channel, int num, unsigned short *buffer)
{
  int status;
  int points_to_read;
  int points_read = 0;
  struct {
    unsigned short status;
    unsigned short bytcnt;
    unsigned int dummy;
  } iosb = {
    0, 0, 0};
  int try;
  int try_outer;
  for (try_outer = 0; try_outer < 3; try_outer++) {
    AccessTraq(setup, 0x0A000 | channel, 24);
    AccessTraq(setup, 0x0B000, 24);
    pio(8, 0, 0, 16);
    for (try = 0; (try < 20) && (!(CamQ(0) & 1)) && (status & 1); try++) {
      pio(8, 0, 0, 16);
    }
    pio(10, 0, 0, 16);
    for (points_to_read = num; points_to_read && (status & 1); points_to_read = num - points_read) {
      int count = points_to_read > 32767 ? 32767 : points_to_read;
      status = CamQstopw(setup->traq_name, 0, 2, count, buffer + points_read, 16, (unsigned short *)&iosb);
      status = (status & 1) ? iosb.status : status;
      if (iosb.bytcnt == 0)
	break;
      points_read += iosb.bytcnt / 2;
    }
    if (points_to_read == 0)
      break;
  }
  if (try_outer >= 3)
    status = DSP2904$_CHANNEL_READ_ERROR;
  return status;
}

static unsigned int Input(InStoreStruct * setup, int code)
{
  int status;
  unsigned short i;
  AccessTraq(setup, code, 16);
  DevWait((float).005);
  pio(0, 0, &i, 16);
  return i;
}

static int AccessTraq(InStoreStruct * setup, int data, int memsize)
{
  int try;
  int status;
  pio(17, 0, &data, memsize);
  for (try = 0; (try < 20) && (!(CamQ(0) & 1)) && (status & 1); try++) {
    DevWait((float).0005);
    pio(17, 0, &data, memsize);
  }
  return status;
}

EXPORT int dsp2904__dw_setup(struct descriptor *niddsc __attribute__ ((unused)), struct descriptor *methoddsc __attribute__ ((unused)), Widget parent)
{
  static String uids[] = { "DSP2904.uid" };
  static MrmRegisterArg uilnames[] = { {"FixMenu", (XtPointer) FixMenu} };
  return XmdsDeviceSetup(parent, (int *)niddsc->pointer, uids, XtNumber(uids), "DSP2904", uilnames,
			 XtNumber(uilnames), 0);
}

static void FixMenu(Widget w)
{
  Widget p_w;
  XtVaGetValues(w, XmNsubMenuId, &p_w, NULL);
  XtVaSetValues(p_w, XmNorientation, XmVERTICAL, XmNnumColumns, 5, XmNpacking, XmPACK_COLUMN, NULL);
}
