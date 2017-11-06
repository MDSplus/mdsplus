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
#include <stdio.h>
#include <mdsplus/mdsplus.h>
#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include "l8212_04_gen.h"
#include <ncidef.h>

#include <Mrm/MrmPublic.h>
#include <Xm/Scale.h>
#include <Xmds/XmdsCallbacks.h>
#include <Xmds/XmdsDigChans.h>
#include <Xmds/XmdsNidOptionMenu.h>
#include <xmdsshr.h>
#include "libroutines.h"
#include "devroutines.h"

/*
   All the init routines use one common function which takes the number
   of channels as an argument.
*/
static int L8212__INIT(struct descriptor *niddsc_ptr __attribute__ ((unused)), InInitStruct * setup, int chans);
int l8212_04___init(struct descriptor *niddsc __attribute__ ((unused)), InInitStruct * setup)
{
  return L8212__INIT(niddsc, setup, 4);
}

int l8212_08___init(struct descriptor *niddsc __attribute__ ((unused)), InInitStruct * setup)
{
  return L8212__INIT(niddsc, setup, 8);
}

int l8212_16___init(struct descriptor *niddsc __attribute__ ((unused)), InInitStruct * setup)
{
  return L8212__INIT(niddsc, setup, 16);
}

int l8212_32___init(struct descriptor *niddsc __attribute__ ((unused)), InInitStruct * setup)
{
  return L8212__INIT(niddsc, setup, 32);
}

/*
   All the trigger routines use the same common function.
*/
static int L8212__TRIGGER(struct descriptor *niddsc_ptr __attribute__ ((unused)), InTriggerStruct * setup);
int l8212_04___trigger(struct descriptor *niddsc __attribute__ ((unused)), InTriggerStruct * setup)
{
  return L8212__TRIGGER(niddsc, setup);
}

int l8212_08___trigger(struct descriptor *niddsc __attribute__ ((unused)), InTriggerStruct * setup)
{
  return L8212__TRIGGER(niddsc, setup);
}

int l8212_16___trigger(struct descriptor *niddsc __attribute__ ((unused)), InTriggerStruct * setup)
{
  return L8212__TRIGGER(niddsc, setup);
}

int l8212_32___trigger(struct descriptor *niddsc __attribute__ ((unused)), InTriggerStruct * setup)
{
  return L8212__TRIGGER(niddsc, setup);
}

/*
   All the store routines use one common function which takes the number
   of channels as an argument.
*/
static int L8212_SETUP(struct descriptor *niddsc __attribute__ ((unused)), Widget parent, int chans);
static int L8212__STORE(struct descriptor *niddsc_ptr __attribute__ ((unused)), InStoreStruct * setup, int chans);
int l8212_04___store(struct descriptor *niddsc __attribute__ ((unused)), InStoreStruct * setup)
{
  return L8212__STORE(niddsc, setup, 4);
}

int l8212_08___store(struct descriptor *niddsc __attribute__ ((unused)), InStoreStruct * setup)
{
  return L8212__STORE(niddsc, setup, 8);
}

int l8212_16___store(struct descriptor *niddsc __attribute__ ((unused)), InStoreStruct * setup)
{
  return L8212__STORE(niddsc, setup, 16);
}

int l8212_32___store(struct descriptor *niddsc __attribute__ ((unused)), InStoreStruct * setup)
{
  return L8212__STORE(niddsc, setup, 32);
}

/*
   All the store routines use one common function which takes the number
   of channels as an argument.
*/
EXPORT int l8212_32__dw_setup(struct descriptor *niddsc __attribute__ ((unused)), struct descriptor *methoddsc __attribute__ ((unused)), Widget parent)
{
  return L8212_SETUP(niddsc, parent, 32);
}

EXPORT int l8212_16__dw_setup(struct descriptor *niddsc __attribute__ ((unused)), struct descriptor *methoddsc __attribute__ ((unused)), Widget parent)
{
  return L8212_SETUP(niddsc, parent, 16);
}

EXPORT int l8212_08__dw_setup(struct descriptor *niddsc __attribute__ ((unused)), struct descriptor *methoddsc __attribute__ ((unused)), Widget parent)
{
  return L8212_SETUP(niddsc, parent, 8);
}

EXPORT int l8212_04__dw_setup(struct descriptor *niddsc __attribute__ ((unused)), struct descriptor *methoddsc __attribute__ ((unused)), Widget parent)
{
  return L8212_SETUP(niddsc, parent, 4);
}








static int FreqToClock(int chans, int noc, int freq_nid, int *clock);
static int NOCToCode(int chans, int noc, int *noc_code);

#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;

static int one = 1;
static int zero = 0;
static int four = 4;

#define pio(f,a) {\
  return_on_error(DevCamChk(CamPiow(setup->name,a,f,0,16,0),&one,0),status);}

#define piod(f,a, dv) {int d = dv;\
  return_on_error(DevCamChk(CamPiow(setup->name,a,f,&d,16,0),&one,0),status);}

#define pioq(f,a) {\
  return_on_error(DevCamChk(CamPiow(setup->name,a,f,0,16,0),&one,&one),status);}

static int L8212__INIT(struct descriptor *niddsc_ptr __attribute__ ((unused)), InInitStruct * setup, int chans)
{

  int status;
  int ctrl = 0;
  int noc_code;
  int clock;

/*------------------------------------------------------------------------------

 Executable:
*/

  /* build a setup word */
  return_on_error(NOCToCode(chans, setup->noc, &noc_code), status);
  return_on_error(FreqToClock(chans, setup->noc, setup->head_nid + L8212_04_N_FREQ, &clock),
		  status);
  ctrl = noc_code | (clock << 2) | (setup->pts << 5);

  /* enable lams */
  pio(26, 0);
  /* write the setup word */
  piod(17, 0, ctrl);

  /* start it going */
  pio(9, 0);

  return status;
}

/**************************************
  Routine to get the clock information
  for this module and make a clock code
  from it.
***************************************/
static int FreqToClock(int chans, int noc, int freq_nid, int *clock)
{
  static float freqs[2][8] = { {0, .5, 2.5, 5.0, 12.5, 25, 50, 100}, {0, .2, 1, 2, 5, 10, 20, 40} };
  int master_clock = (chans <= 16) ? 0 : 1;
  float freq;
  int status = 1;

  status = DevFloat(&freq_nid, &freq);
  if ((status & 1) == 0) {
    *clock = 0;
    status = 1;
  } else {
    for (*clock = 0; ((freqs[master_clock][*clock] < freq) && (*clock < 7)); (*clock)++) ;
    switch (master_clock) {
    case 0:
      switch (*clock) {
      case 4:
	if (noc > 8)
	  status = L8212$_FREQ_TO_HIGH;
	break;
      case 5:
	if (noc > 4)
	  status = L8212$_FREQ_TO_HIGH;
	break;
      case 6:
	if (noc > 2)
	  status = L8212$_FREQ_TO_HIGH;
	break;
      case 7:
	if (noc > 1)
	  status = L8212$_FREQ_TO_HIGH;
	break;
      }
      break;
    case 1:
      switch (*clock) {
      case 4:
	if (noc > 32)
	  status = L8212$_FREQ_TO_HIGH;
	break;
      case 5:
	if (noc > 16)
	  status = L8212$_FREQ_TO_HIGH;
	break;
      case 6:
	if (noc > 8)
	  status = L8212$_FREQ_TO_HIGH;
	break;
      case 7:
	if (noc > 4)
	  status = L8212$_FREQ_TO_HIGH;
	break;
      }
      break;
    }
  }
  return status;
}

/*********************************************
  Routine to get the NOC (Number of Channels)
  for this module and make a NOC code from it.
**********************************************/
static int NOCToCode(int chans, int noc, int *noc_code)
{
  int model = (chans == 4) ? 0 : (chans == 8) ? 1 : (chans == 16) ? 2 : 3;
  static int nocs[4][4] = { {1, 2, 4, 8},
  {1, 2, 4, 8},
  {2, 4, 8, 16},
  {4, 8, 16, 32}
  };

  if (noc > chans)
    return L8212$_INVALID_NOC;
  for (*noc_code = 0; ((*noc_code < 4) && (noc != nocs[model][*noc_code])); (*noc_code)++) ;
  if (*noc_code == 4)
    (*noc_code)--;

  return 1;
}

static int L8212__TRIGGER(struct descriptor *niddsc_ptr __attribute__ ((unused)), InTriggerStruct * setup)
{
  int status;
  pio(25, 0);
  return 1;
}

#include <treeshr.h>
#include <mdsshr.h>
#include <libroutines.h>
#include <strroutines.h>

//extern unsigned short OpcAdd;
//extern unsigned short OpcMultiply;
//extern unsigned short OpcValue;

static void L8212$HeaderToPTS(int memories, char *header_ptr, int *pts_ptr);
static void PTStoMinMax(int chans, int mems, char *header, int pts, int *min_ptr, int *max_ptr);
static int ReadSetup(int chans,
		     char *name,
		     int memories,
		     char *head_ptr,
		     int *samples_ptr,
		     int *min_idx_ptr, int *max_idx_ptr, float *freq_ptr, int *noc_ptr);

static int ReadChannel(char *name, int max_samps, int chan, short *data_ptr, int use_qrep);

#define max(a,b) (a) >= (b) ? (a) : (b)
#define min(a,b) (a) <= (b) ? (a) : (b)
#define L8212_N_CHANNELS L8212_04_N_INPUT_01
#define L8212_K_NODES_PER_CHANNEL 3
#define L8212_N_CHAN_HEAD 0
#define L8212_N_CHAN_STARTIDX 1
#define L8212_N_CHAN_ENDIDX 2
#define CHAN_NID(chan, field) setup->head_nid+L8212_N_CHANNELS+chan*L8212_K_NODES_PER_CHANNEL+field
#define NocToChans(chans, noc, chans_ptr) {\
	  static int channels[4][4] = {{1,2,4,8},{1,2,4,8},{2,4,8,16},{4,8,16,32}};\
          int chan_index;\
	  int base = chans>>2;\
          libffs(&zero,&four,(char *)&base,&chan_index);	\
	  *chans_ptr = channels[chan_index][noc];\
	}
#define ClockToDt(chans,period,freq_ptr)\
	{\
	  static float freqs[2][8] = {{0.0, 1./500., 1./2500., 1./5000., 1./12500., 1./25000., 1./50000.0, 1./100000.0},\
	  				    {0.0, 1./200., 1./1000., 1./2000., 1./5000.,  1./10000., 1./20000.0,  1./40000.0}};\
          *freq_ptr = freqs[(chans<32)?0:1][period];\
	}

static int L8212__STORE(struct descriptor *niddsc_ptr __attribute__ ((unused)), InStoreStruct * setup, int chans)
{
  static DESCRIPTOR_A_BOUNDS(raw, sizeof(short), DTYPE_W, 0, 1, 0);
  static DESCRIPTOR(counts_str, "counts");
  static DESCRIPTOR_WITH_UNITS(counts, &raw, &counts_str);
  static DESCRIPTOR_LONG(start_d, &raw.bounds[0].l);
  static DESCRIPTOR_LONG(end_d, &raw.bounds[0].u);
  static int trigger_nid;
  static DESCRIPTOR_NID(trigger_d, &trigger_nid);
  static float frequency;
  static DESCRIPTOR_FLOAT(frequency_d, &frequency);
  static DESCRIPTOR_RANGE(int_clock_d, 0, 0, &frequency_d);
  static int ext_clock_nid;
  static DESCRIPTOR_NID(ext_clock_d, &ext_clock_nid);
  static float coefficient = 10.0 / 4096;
  static DESCRIPTOR_FLOAT(coef_d, &coefficient);
  static short offset = 0;
  static struct descriptor_s offset_d = { 2, DTYPE_W, CLASS_S, (char *)&offset };
#define DESCRIPTOR_VALUE(name)\
  struct descriptor_function_0 { RECORD_HEAD } name =\
   {2, DTYPE_FUNCTION, CLASS_R, (unsigned char *)&OpcValue, 0}
  static DESCRIPTOR_VALUE(value);
  static DESCRIPTOR_FUNCTION_2(add_exp, (unsigned char *)&OpcAdd, &offset_d, &value);
  static DESCRIPTOR_FUNCTION_2(mult_exp, (unsigned char *)&OpcMultiply, &coef_d, &add_exp);
  static DESCRIPTOR(volts_str, "volts");
  static DESCRIPTOR_WITH_UNITS(volts, &mult_exp, &volts_str);
  static DESCRIPTOR_WINDOW(window, &start_d, &end_d, &trigger_d);
  static DESCRIPTOR_DIMENSION(dimension, &window, 0);
  static DESCRIPTOR(time_str, "seconds");
  static DESCRIPTOR_WITH_UNITS(time, &dimension, &time_str);
  static DESCRIPTOR_SIGNAL_1(signal, &volts, &counts, &time);

  int samples_per_channel;
  int min_idx;
  int max_idx;
  int num_chans;
  int vm_size;
  short *channel_data_ptr;
  int status;
  int chan;
  //int samples_to_read;
  //int i;
  //int pts;
  int stop_trig_nid;
  int use_qrep = 0;
  int name_nid;

  name_nid = setup->head_nid + L8212_04_N_NAME;
  use_qrep = TreeIsOn(name_nid) & 1;

  offset = setup->offset;
  trigger_nid = setup->head_nid + L8212_04_N_STOP_TRIG;
  ext_clock_nid = setup->head_nid + L8212_04_N_EXT_CLOCK_IN;
  stop_trig_nid = setup->head_nid + L8212_04_N_STOP_TRIG;

  pio(8, 0);
  if ((CamXandQ(0) & 1) == 0)
    return DEV$_NOT_TRIGGERED;
  return_on_error(ReadSetup
		  (chans, setup->name, setup->memories, setup->header, &samples_per_channel,
		   &min_idx, &max_idx, &frequency, &num_chans), status);
  dimension.axis =
      frequency == 0 ? (struct descriptor *)&ext_clock_d : (struct descriptor *)&int_clock_d;
  status = 1;
  vm_size = samples_per_channel * 2;
  channel_data_ptr = malloc(vm_size);
  for (chan = 0; ((chan < num_chans) && (status & 1)); chan++) {
    int chan_head = CHAN_NID(chan, L8212_N_CHAN_HEAD);
    if (TreeIsOn(chan_head) & 1) {
      int chan_start_idx = CHAN_NID(chan, L8212_N_CHAN_STARTIDX);
      int chan_end_idx = CHAN_NID(chan, L8212_N_CHAN_ENDIDX);
      status = DevLong(&chan_start_idx, (int *)&raw.bounds[0].l);
      if (status & 1)
	raw.bounds[0].l = min(max_idx, max(min_idx, raw.bounds[0].l));
      else
	raw.bounds[0].l = min_idx;

      status = DevLong(&chan_end_idx, (int *)&raw.bounds[0].u);
      if (status & 1)
	raw.bounds[0].u = min(max_idx, max(min_idx, raw.bounds[0].u));
      else
	raw.bounds[0].u = max_idx;

      raw.m[0] = raw.bounds[0].u - raw.bounds[0].l + 1;
      if (raw.m[0] > 0) {
	//samples_to_read = raw.bounds[0].u - min_idx + 1;
	status = ReadChannel(setup->name, samples_per_channel, chan, channel_data_ptr, use_qrep);
	if (status & 1) {
	  raw.pointer = (char *)(channel_data_ptr + (raw.bounds[0].l - min_idx));
	  raw.a0 = raw.pointer - raw.bounds[0].l * sizeof(*channel_data_ptr);
	  raw.arsize = raw.m[0] * 2;
	  trigger_d.pointer = (char *)&stop_trig_nid;
	  status = TreePutRecord(chan_head, (struct descriptor *)&signal, 0);
	}
      }
    }
  }
  free(channel_data_ptr);
  return status;
}

#ifdef pio
#undef pio
#endif
#define pio(f,a,d) {\
  return_on_error(DevCamChk(CamPiow(name,a,f,d,16,0),&one,0),status);}

static void PTStoMinMax(int chans, int mems, char *header, int pts, int *min_ptr, int *max_ptr)
{
  int PTS[8];
  L8212$HeaderToPTS(mems, header, PTS);
  *min_ptr = -(mems * 32768 / chans - PTS[pts]);
  *max_ptr = PTS[pts] - 1;
}

static int ReadSetup(int chans,
		     char *name,
		     int memories,
		     char *head_ptr,
		     int *samples_ptr,
		     int *min_idx_ptr, int *max_idx_ptr, float *freq_ptr, int *noc_ptr)
{
  struct l8212_setup {
    unsigned l8212_setup_v_noc:2;
    unsigned l8212_setup_v_period:3;
    unsigned l8212_setup_v_pts:3;
  } setup;
  int status;

  pio(3, 0, (short *)&setup);

  ClockToDt(chans, setup.l8212_setup_v_period, freq_ptr);
  NocToChans(chans, setup.l8212_setup_v_noc, noc_ptr);
  PTStoMinMax(*noc_ptr, memories, head_ptr, setup.l8212_setup_v_pts, min_idx_ptr, max_idx_ptr);
  *samples_ptr = *max_idx_ptr - *min_idx_ptr + 1;

  return status;
}

#define stop(f,a,c, dp) {\
    return_on_error(DevCamChk(CamStopw(name,a,f,c,dp,16,0),&one,0),status);}

#define qrep(f,a,c, dp) {\
    return_on_error(DevCamChk(CamQrepw(name,a,f,c,dp,16,0),&one,0),status);}

static int ReadChannel(char *name, int max_samps, int chan, short *data_ptr, int use_qrep)
{

  short *in_ptr = data_ptr;
  short *end_ptr = data_ptr + max_samps;
  int pnts_to_read;
  struct {
    unsigned short status;
    unsigned short bytcnt;
    unsigned short fill[2];
  } iosb;
  //int lamchks = 10;
  //unsigned short dum_read;
  int status;

  pio(16, 0, &chan)
      while ((pnts_to_read = max(0, min(32767, end_ptr - in_ptr)))) {
    if (use_qrep)
      qrep(2, 0, pnts_to_read, in_ptr)
	  else
      stop(2, 0, pnts_to_read, in_ptr)
	  CamGetStat((unsigned short *)&iosb);
    if ((!(CamQ((unsigned short *)&iosb) & 1)) && (iosb.bytcnt == 0))
      in_ptr = end_ptr;
    else
      in_ptr += iosb.bytcnt / 2;
  }
  return 1;
}

static void clock_initialize_proc(Widget parent, Widget clock_menu);
static void pts_initialize_proc(Widget w);

static void chans_changed_proc(Widget w, int *tag __attribute__ ((unused)), XmRowColumnCallbackStruct * reason);
static void clock_changed_proc(Widget w, int *tag __attribute__ ((unused)), XmRowColumnCallbackStruct * reason);
static void header_changed_proc(Widget w, int *tag __attribute__ ((unused)), XmRowColumnCallbackStruct * reason);
static void mems_changed_proc(Widget w, int *tag __attribute__ ((unused)), XmScaleCallbackStruct * reason);
static Boolean apply_proc(Widget w);
static void reset_proc(Widget w);
static void reset(Widget w);
static void L8212$HeaderToPTS(int mems, char *header, int pts[]);
static void ChangePts(Widget pts_menu, char *header, int mems);

static int L8212_SETUP(struct descriptor *niddsc __attribute__ ((unused)), Widget parent, int chans)
{
  Widget dbox;
  static String uids[] = { "L8212.uid", "L8212_HEADERS.uid" };
  static int nid;
  unsigned int i;
  static NCI_ITM nci[] =
      { {4, NciCONGLOMERATE_NIDS, (unsigned char *)&nid, 0}, {0, NciEND_OF_LIST, 0, 0} };
  static char *clock_vals_slow[] = { ".2", "1.", "2.", "5.", "10.", "20.", "40.", "0." };
  static char *clock_strings_slow[] =
      { " 200 Hz", " 1 KHz", " 2 KHz", " 5 KHz", " 10 KHz", " 20 KHz", " 40 KHz", " External" };
  static char *clock_vals_fast[] = { ".5", "2.5", "5.0", "12.5", "25.", "50.", "100.", "0." };
  static char *clock_strings_fast[] =
      { " 500 Hz", "   2.5 KHz", "   5   KHz", "  12.5 KHz", "  25   KHz", "  50   KHz",
" 100   KHz", " External" };
  static char *clock_vals_fast_16[] = { ".5", "2.5", "5.0", "12.5", "25.", "50.", "0." };
  static char *clock_strings_fast_16[] =
      { " 500 Hz", "   2.5 KHz", "   5   KHz", "  12.5 KHz", "  25   KHz", "  50   KHz",
" External" };
  static char *chan_vals_32[] = { "4BU", "8BU", "16BU", "32BU" };
  static char *chan_strings_32[] = { " 4", " 8", "16", "32" };
  static char *chan_vals_16[] = { "2BU", "4BU", "8BU", "16BU" };
  static char *chan_strings_16[] = { " 2", " 4", " 8", "16" };
  static char *chan_vals_08[] = { "1BU", "2BU", "4BU", "8BU" };
  static char *chan_strings_08[] = { " 1", " 2", " 4", " 8" };
  static char *chan_vals_04[] = { "1BU", "2BU", "4BU" };
  static char *chan_strings_04[] = { " 1", " 2", " 4" };
  static XmString chan_vals[5];
  static XmString chan_strings[5];
  static XmString clock_vals[9];
  static XmString clock_strings[9];
  static MrmRegisterArg uilnames[] = { {"mems_nid", 0},
  {"num_channels", 0},
  {"chan_vals", (XtPointer) chan_vals},
  {"chan_strings", (XtPointer) chan_strings},
  {"clock_vals", (XtPointer) clock_vals},
  {"clock_strings", (XtPointer) clock_strings},
  {"mems_changed_proc", (void *)mems_changed_proc},
  {"chans_changed_proc", (void *)chans_changed_proc},
  {"clock_changed_proc", (void *)clock_changed_proc},
  {"header_changed_proc", (void *)header_changed_proc},
  {"pts_initialize_proc", (void *)pts_initialize_proc},
  {"apply_proc", (void *)apply_proc},
  {"reset_proc", (void *)reset_proc}
  };
  int status;
  TreeGetNci(*(int *)niddsc->pointer, nci);
  uilnames[0].value = (char *)0 + (nid + L8212_04_N_MEMORIES);
  uilnames[1].value = (char *)0 + chans;
  switch (chans) {
  case 32:{
      for (i = 0; i < XtNumber(clock_vals_slow); i++) {
	clock_vals[i] = XmStringCreateSimple(clock_vals_slow[i]);
	clock_strings[i] = XmStringCreateSimple(clock_strings_slow[i]);
      };
      clock_vals[i] = (XmString) 0;
      clock_strings[i] = (XmString) 0;
      for (i = 0; i < XtNumber(chan_vals_32); i++) {
	chan_vals[i] = XmStringCreateSimple(chan_vals_32[i]);
	chan_strings[i] = XmStringCreateSimple(chan_strings_32[i]);
      };
      chan_vals[i] = (XmString) 0;
      chan_strings[i] = (XmString) 0;
    };
    break;
  case 16:{
      for (i = 0; i < XtNumber(clock_vals_fast_16); i++) {
	clock_vals[i] = XmStringCreateSimple(clock_vals_fast_16[i]);
	clock_strings[i] = XmStringCreateSimple(clock_strings_fast_16[i]);
      };
      clock_vals[i] = (XmString) 0;
      clock_strings[i] = (XmString) 0;
      for (i = 0; i < XtNumber(chan_vals_16); i++) {
	chan_vals[i] = XmStringCreateSimple(chan_vals_16[i]);
	chan_strings[i] = XmStringCreateSimple(chan_strings_16[i]);
      };
      chan_vals[i] = (XmString) 0;
      chan_strings[i] = (XmString) 0;
    };
    break;
  case 8:{
      for (i = 0; i < XtNumber(clock_vals_fast); i++) {
	clock_vals[i] = XmStringCreateSimple(clock_vals_fast[i]);
	clock_strings[i] = XmStringCreateSimple(clock_strings_fast[i]);
      };
      clock_vals[i] = (XmString) 0;
      clock_strings[i] = (XmString) 0;
      for (i = 0; i < XtNumber(chan_vals_08); i++) {
	chan_vals[i] = XmStringCreateSimple(chan_vals_08[i]);
	chan_strings[i] = XmStringCreateSimple(chan_strings_08[i]);
      };
      chan_vals[i] = (XmString) 0;
      chan_strings[i] = (XmString) 0;
    };
    break;
  case 4:{
      for (i = 0; i < XtNumber(clock_vals_fast); i++) {
	clock_vals[i] = XmStringCreateSimple(clock_vals_fast[i]);
	clock_strings[i] = XmStringCreateSimple(clock_strings_fast[i]);
      };
      clock_vals[i] = (XmString) 0;
      clock_strings[i] = (XmString) 0;
      for (i = 0; i < XtNumber(chan_vals_04); i++) {
	chan_vals[i] = XmStringCreateSimple(chan_vals_04[i]);
	chan_strings[i] = XmStringCreateSimple(chan_strings_04[i]);
      };
      chan_vals[i] = (XmString) 0;
      chan_strings[i] = (XmString) 0;
    };
    break;
  }
  status =
      XmdsDeviceSetup(parent, (int *)niddsc->pointer, uids, XtNumber(uids), "L8212", uilnames,
		      XtNumber(uilnames), &dbox);
  reset(dbox);
  return status;
}

static void mems_changed_proc(Widget w, int *tag __attribute__ ((unused)), XmScaleCallbackStruct * reason)
{
  Widget pts_menu = XtNameToWidget(XtParent(w), "pts_menu");
  struct descriptor_xd *header_xd =
      XmdsNidOptionMenuGetXd(XtNameToWidget(XtParent(w), "header_menu"));
  static char header[11];
  static struct descriptor_s header_dsc = { sizeof(header), DTYPE_T, CLASS_S, header };
  int status = TdiText((struct descriptor *)header_xd, &header_dsc MDS_END_ARG);
  if (status & 1) {
    ChangePts(pts_menu, header, reason->value);
  }
}

static void chans_changed_proc(Widget w, int *tag __attribute__ ((unused)), XmRowColumnCallbackStruct * reason)
{
  Widget parent = XtParent(XtParent(w));
  Widget clock_menu = XtNameToWidget(parent, "clock_menu");
  int num_buttons;
  Widget *buttons = (Widget *) XmdsNidOptionMenuGetButtons(clock_menu, &num_buttons);
  int i;
  for (i = 0; i < num_buttons; i++)
    XtSetSensitive(buttons[i], 1);
  switch (*(int *)&reason->data) {
  case 3:
    XtSetSensitive(buttons[num_buttons - 5], 0);
    MDS_ATTR_FALLTHROUGH
  case 2:
    XtSetSensitive(buttons[num_buttons - 4], 0);
    MDS_ATTR_FALLTHROUGH
  case 1:
    XtSetSensitive(buttons[num_buttons - 3], 0);
    break;
  }
}

static void clock_changed_proc(Widget w, int *tag __attribute__ ((unused)), XmRowColumnCallbackStruct * reason)
{
  Widget parent = XtParent(XtParent(w));
  Widget clock_menu = XtNameToWidget(parent, "clock_menu");
  int num_buttons;
  int *buttons __attribute__ ((unused)) = XmdsNidOptionMenuGetButtons(clock_menu, &num_buttons);
  if (*(int *)&reason->data == (num_buttons - 2))
    XtManageChild(XtNameToWidget(parent, "external_clock"));
  else
    XtUnmanageChild(XtNameToWidget(parent, "external_clock"));
}

static void header_changed_proc(Widget w, int *tag __attribute__ ((unused)), XmRowColumnCallbackStruct * reason)
{
  Widget pts_menu = XtNameToWidget(XtParent(XtParent(w)), "pts_menu");
  int mems;
  struct descriptor_xd *header_xd =
      XmdsNidOptionMenuIdxGetXd(XtNameToWidget(XtParent(XtParent(w)), "header_menu"),
				*(int *)&reason->data);
  static char header[11];
  static struct descriptor_s header_dsc = { sizeof(header), DTYPE_T, CLASS_S, header };
  int status = TdiText((struct descriptor *)header_xd, &header_dsc MDS_END_ARG);
  if (status & 1) {
    XmScaleGetValue(XtNameToWidget(XtParent(XtParent(w)), "mems_scale"), &mems);
    ChangePts(pts_menu, header, mems);
  }
}

static void ChangePts(Widget pts_menu, char *header, int mems)
{
  Widget history;
  int pts[8];
  int i;
  L8212$HeaderToPTS(mems, header, pts);
  for (i = 0; i < 8; i++) {
    char label[20];
    sprintf(label, "%d", pts[i]);
    XmdsNidOptionMenuSetButton(pts_menu, i, label);
  }
  XtVaGetValues(pts_menu, XmNmenuHistory, &history, NULL);
  XtVaSetValues(pts_menu, XmNmenuHistory, history, NULL);
}

static void L8212$HeaderToPTS(int mems, char *header, int pts[])
{
  int i;
  int pin;
  int lowbit = -1;
  int middlebit = -1;
  int highbit = -1;
  char pins[11];
  for (pin = 0; pin < 11; pin++)
    switch (header[pin]) {
    case '9':
      pins[pin] = 0;
      break;
    case '4':
      pins[pin] = 1;
      break;
    case '1':
      pins[pin] = 0;
      lowbit = pin;
      break;
    case '2':
      pins[pin] = 0;
      middlebit = pin;
      break;
    case '3':
      pins[pin] = 0;
      highbit = pin;
      break;
    };

  for (i = 0; i < 8; i++) {
    int ptsc;
    if (lowbit != -1)
      pins[lowbit] = (i & 1);
    if (middlebit != -1)
      pins[middlebit] = ((i >> 1) & 1);
    if (highbit != -1)
      pins[highbit] = ((i >> 2) & 1);
    ptsc = (pins[1] ? 32768 : 0) |
	(pins[5] ? 16384 : 0) |
	(pins[6] ? 8192 : 0) |
	(pins[7] ? 4096 : 0) |
	(pins[8] ? 2048 : 0) |
	(pins[9] ? 1024 : 0) |
	(pins[10] ? 512 : 0) |
	(pins[3] ? 256 : 0) |
	(pins[4] ? ((mems <= 4) ? 128 : 65536) : 0) |
	(pins[0] ? ((mems <= 4) ? 112 : 240) : 0) | (pins[2] ? 15 : 0);

    pts[i] = mems * 16384 - ptsc;
  }
}

static void reset_proc(Widget w)
{
  reset(XtParent(w));
}

static void reset(Widget w)
{
  XtPointer user_data;
  int nid;
  int mems;
  Widget mems_scale = XtNameToWidget(w, "mems_scale");
  XtVaGetValues(mems_scale, XmNuserData, &user_data, NULL);
  nid = (intptr_t) user_data;
  DevLong(&nid, &mems);
  XmdsResetAllXds(w);
  XmScaleSetValue(mems_scale, mems);
  clock_initialize_proc(w, XtNameToWidget(w, "*clock_menu"));
  pts_initialize_proc(XtNameToWidget(w, "*pts_menu"));
}

static void clock_initialize_proc(Widget parent, Widget clock_menu)
{
  Widget chans_menu = XtNameToWidget(parent, "chans_menu");
  int num_buttons;
  Widget *buttons = (Widget *) XmdsNidOptionMenuGetButtons(clock_menu, &num_buttons);
  int i;
  int idx = XmdsGetOptionIdx(chans_menu);
  int clk_idx = XmdsGetOptionIdx(clock_menu);
  for (i = 0; i < num_buttons; i++)
    XtSetSensitive(buttons[i], 1);
  switch (idx) {
  case 3:
    XtSetSensitive(buttons[num_buttons - 5], 0);
    MDS_ATTR_FALLTHROUGH
  case 2:
    XtSetSensitive(buttons[num_buttons - 4], 0);
    MDS_ATTR_FALLTHROUGH
  case 1:
    XtSetSensitive(buttons[num_buttons - 3], 0);
    break;
  }
  if (clk_idx == (num_buttons - 2))
    XtManageChild(XtNameToWidget(parent, "external_clock"));
}

static void pts_initialize_proc(Widget pts_menu)
{
  int mems;
  struct descriptor_xd *header_xd =
      XmdsNidOptionMenuGetXd(XtNameToWidget(XtParent(pts_menu), "header_menu"));
  static char header[11];
  static struct descriptor_s header_dsc = { sizeof(header), DTYPE_T, CLASS_S, header };
  int status = TdiText((struct descriptor *)header_xd, &header_dsc MDS_END_ARG);
  if (status & 1) {
    XmScaleGetValue(XtNameToWidget(XtParent(pts_menu), "mems_scale"), &mems);
    ChangePts(pts_menu, header, mems);
  }
}

static Boolean apply_proc(Widget w)
{
  int status;
  Widget mem_scale = XtNameToWidget(XtParent(w), "mems_scale");
  static int mems;
  struct descriptor_s mems_dsc = { sizeof(int), DTYPE_L, CLASS_S, (char *)&mems };
  XtPointer user_data;
  int nid;
  XtVaGetValues(mem_scale, XmNuserData, &user_data, NULL);
  nid = (intptr_t) user_data;
  XmScaleGetValue(mem_scale, &mems);
  status = TreePutRecord(nid, (struct descriptor *)&mems_dsc, 0) & 1;
  if (!status)
    XmdsComplain(XtParent(w), "Error writing num memories");
  if (status)
    XmdsApplyAllXds(XtParent(w));
  return status & 1;
}
