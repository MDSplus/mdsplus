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
#include <ncidef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "l8210_gen.h"
#include <Mrm/MrmPublic.h>
#include <Xm/Xm.h>
#include <Xm/Scale.h>
#include <xmdsshr.h>
#include <Xmds/XmdsNidOptionMenu.h>
#include "devroutines.h"




extern int DevWait(float);
static int one = 1;
static int zero = 0;

#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;
#define pio(f,a,data) return_on_error(DevCamChk(CamPiow(setup->name,a,f,data,16,0),&one,0),status);
#define min(a,b) ((a) <= (b)) ? (a) : (b)
#define max(a,b) ((a) >= (b)) ? (a) : (b)

static void reset(Widget w);
static void reset_proc(Widget w);
static Boolean apply_proc(Widget w);
static void pts_activate_proc(Widget w);
static void pts_dismiss_proc(Widget w);

EXPORT int l8210___init(struct descriptor *niddsc_ptr __attribute__ ((unused)), InInitStruct * setup)
{
  int status;
  pio(9, 0, 0);
  return status;
}

void L8210HeaderToPTS(char *header_ptr, char *memories_ptr, int *pts_ptr);
static int ReadSetup(InStoreStruct * setup, int *mem_ptr, char *head_ptr, int *samples_ptr,
		     int *min_idx_ptr, int *max_idx_ptr, float *freq_ptr, int *noc_ptr);
static int ReadChannel(InStoreStruct * setup, int *max_samps_ptr, int chan, int *samples_ptr,
		       short *data_ptr);
static int DevXToI(float start_time, float end_time, struct descriptor_dimension *dimension,
		   int min_idx, int max_idx, int *start_idx, int *end_idx);

EXPORT int l8210___store(struct descriptor *niddsc_ptr __attribute__ ((unused)), InStoreStruct * setup)
{
  static DESCRIPTOR_A_BOUNDS(raw, sizeof(short), DTYPE_W, 0, 1, 0);
  static DESCRIPTOR(counts_str, "counts");
  static DESCRIPTOR_WITH_UNITS(counts, &raw, &counts_str);
  static DESCRIPTOR_LONG(start_d, &raw.bounds[0].l);
  static DESCRIPTOR_LONG(end_d, &raw.bounds[0].u);
  static int stop_trig_nid;
  static DESCRIPTOR_NID(trigger_d, &stop_trig_nid);
  static float frequency;
  static DESCRIPTOR_FLOAT(frequency_d, &frequency);
  static DESCRIPTOR_RANGE(int_clock_d, 0, 0, &frequency_d);
  static int ext_clock_in_nid;
  static DESCRIPTOR_NID(ext_clock_d, &ext_clock_in_nid);
  static float coefficient = 10.0 / 1024;
  static DESCRIPTOR_FLOAT(coef_d, &coefficient);
  static short offset = -512;
  static struct descriptor offset_d = { 2, DTYPE_W, CLASS_S, (char *)&offset };
#define DESCRIPTOR_VALUE(name)\
  struct descriptor_function_0 { RECORD_HEAD } name =\
   {2, DTYPE_FUNCTION, CLASS_R, (unsigned char *)&OpcValue, 0}
  static DESCRIPTOR_VALUE(value);
  static DESCRIPTOR_FUNCTION_2(add_exp, (unsigned char *)&OpcAdd, &offset_d, &value);
  static DESCRIPTOR_FUNCTION_2(mult_exp, (unsigned char *)&OpcMultiply, &coef_d, &add_exp);
  static DESCRIPTOR(volts_str, "volts");
  static DESCRIPTOR_WITH_UNITS(volts, &mult_exp, &volts_str);
  static DESCRIPTOR_WINDOW(window, &start_d, &end_d, &trigger_d);
  static int clock_out_nid;
  static DESCRIPTOR_NID(clock_out, &clock_out_nid);
  static DESCRIPTOR_DIMENSION(dimension, &window, &clock_out);
  static DESCRIPTOR(time_str, "seconds");
  static DESCRIPTOR_WITH_UNITS(time, &dimension, &time_str);
  static DESCRIPTOR_SIGNAL_1(signal, &volts, &counts, &time);
  int samples_per_channel;
  int min_idx;
  int max_idx;
  int num_chans;
  //int vm_size;
  short *channel_data_ptr;
  int status;
  int chan;
  int samples_to_read;
  //int i;
  float wait_time;
  stop_trig_nid = setup->head_nid + L8210_N_STOP_TRIG;
  ext_clock_in_nid = setup->head_nid + L8210_N_EXT_CLOCK_IN;
  clock_out_nid = setup->head_nid + L8210_N_CLOCK_OUT;
  pio(26, 0, 0);
  wait_time = setup->memories * 30E-3;
  DevWait(wait_time);
  pio(8, 0, 0);
  if ((CamXandQ(0) & 1) == 0)
    return DEV$_NOT_TRIGGERED;
  return_on_error(ReadSetup(setup, &setup->memories, setup->header, &samples_per_channel,
			    &min_idx, &max_idx, &frequency, &num_chans), status);
  channel_data_ptr = malloc(samples_per_channel * sizeof(short));
  status = TreePutRecord(clock_out_nid,
			 (frequency ==
			  0.0) ? (struct descriptor *)&ext_clock_d : (struct descriptor
								      *)(&int_clock_d), 0);
  for (chan = 0; ((chan < num_chans) && (status & 1)); chan++) {
    int channel_nid =
	setup->head_nid + L8210_N_INPUT_1 + chan * (L8210_N_INPUT_2 - L8210_N_INPUT_1);
    int usetimes_nid = channel_nid + L8210_N_INPUT_1_USETIMES - L8210_N_INPUT_1;
    int startidx_nid = channel_nid + L8210_N_INPUT_1_STARTIDX - L8210_N_INPUT_1;
    int endidx_nid = channel_nid + L8210_N_INPUT_1_ENDIDX - L8210_N_INPUT_1;
    if (TreeIsOn(channel_nid) & 1) {
      int use_times = 0;
      DevLong(&usetimes_nid, &use_times);
      if (use_times) {
	float start_time, end_time;
	raw.bounds[0].l = min_idx;
	raw.bounds[0].u = max_idx;
	status = DevFloat(&startidx_nid, &start_time);
	if (~status & 1)
	  start_time = -1;

	status = DevFloat(&endidx_nid, &end_time);
	if (~status & 1)
	  end_time = -1;

	status = DevXToI(start_time, end_time, &dimension, min_idx, max_idx, &raw.bounds[0].l,
			 &raw.bounds[0].u);
	if (~status & 1) {
	  raw.bounds[0].l = min_idx;
	  raw.bounds[0].u = max_idx;
	}
      } else {
	status = DevLong(&startidx_nid, (int *)&raw.bounds[0].l);
	if (status & 1)
	  raw.bounds[0].l = min(max_idx, max(min_idx, raw.bounds[0].l));
	else
	  raw.bounds[0].l = min_idx;

	status = DevLong(&endidx_nid, (int *)&raw.bounds[0].u);
	if (status & 1)
	  raw.bounds[0].u = min(max_idx, max(min_idx, raw.bounds[0].u));
	else
	  raw.bounds[0].u = max_idx;
      }
      raw.m[0] = raw.bounds[0].u - raw.bounds[0].l + 1;
      if (raw.m[0] > 0) {
	samples_to_read = raw.bounds[0].u - min_idx + 1;

	status = ReadChannel(setup, &samples_per_channel, chan, &samples_to_read, channel_data_ptr);
	if (status & 1) {
	  raw.pointer = (char *)(channel_data_ptr + (raw.bounds[0].l - min_idx));
	  raw.a0 = raw.pointer - raw.bounds[0].l * sizeof(*channel_data_ptr);
	  raw.arsize = raw.m[0] * 2;
	  status = TreePutRecord(channel_nid, (struct descriptor *)&signal, 0);
	}
      }
    }
  }
  free(channel_data_ptr);
  return status;
}

static int ReadSetup(InStoreStruct * setup, int *mem_ptr, char *head_ptr, int *samples_ptr,
		     int *min_idx_ptr, int *max_idx_ptr, float *freq_ptr, int *noc_ptr)
{
  struct l8210_setup {
    unsigned l8210_setup_v_pts:3;
    unsigned l8210_setup_v_period:3;
    unsigned l8210_setup_v_noc:2;
  } l8210_setup;
  int pts[8];			/* post trigger samples */
  static float freq[8] = { 0, 1.E-4, 4.E-5, 2.E-5, 1.E-5, 4.E-6, 2.E-6, 1.E-6 };
  int status;
  pio(1, 0, (short *)&l8210_setup);
  if (status & 1) {
    *noc_ptr = l8210_setup.l8210_setup_v_noc + 1;
    *freq_ptr = freq[l8210_setup.l8210_setup_v_period];
    *samples_ptr = 32768 * *mem_ptr / *noc_ptr;
    L8210HeaderToPTS(head_ptr, (char *)mem_ptr, pts);
    *min_idx_ptr = pts[7 - l8210_setup.l8210_setup_v_pts] - *samples_ptr;
    *max_idx_ptr = pts[7 - l8210_setup.l8210_setup_v_pts] - 1;
  }
  return status;
}

void L8210HeaderToPTS(char *header_ptr, char *memories_ptr, int *pts_ptr)
{
  int ptsl;
  int pin;
  char pts_idx[8][5] = { {" "}, {"A "}, {"B "}, {"AB "}, {"C "}, {"AC "}, {"BC "}, {"ABC "} };

  for (ptsl = 0; ptsl < 8; ptsl++) {
    pts_ptr[ptsl] = 1023;
    for (pin = 0; pin < 6; pin++)
      if (strchr((char *)&pts_idx[7 - ptsl], header_ptr[pin]))
	if ((pin < 5) || ((*memories_ptr) > 1))
	  pts_ptr[ptsl] = pts_ptr[ptsl] + (1 << (pin + 10));
    pts_ptr[ptsl] = 32768 * (*memories_ptr) / 2 - pts_ptr[ptsl];
  }
  return;
}

static int ReadChannel(InStoreStruct * setup, int *max_samps_ptr, int chan, int *samples_ptr,
		       short *data_ptr)
{
  short *in_ptr = data_ptr;
  short *end_ptr = data_ptr + *samples_ptr;
  int pnts_to_read;
  struct {
    unsigned short status;
    unsigned short bytcnt;
    unsigned short fill[2];
  } iosb;
  int lamchks = 10;
  unsigned short dum_read;
  int status;
  pio(10, 0, 0);
  pio(16, chan, &zero);
  while ((pnts_to_read = max(0, min(32767, end_ptr - in_ptr)))) {
    return_on_error(DevCamChk(CamQrepw(setup->name, 0, 2, pnts_to_read, in_ptr, 16, 0), &one, 0),
		    status);
    CamGetStat((unsigned short *)&iosb);
    if ((!(CamQ((unsigned short *)&iosb) & 1)) && (iosb.bytcnt == 0))
      in_ptr = end_ptr;
    else
      in_ptr += iosb.bytcnt / 2;
  }
  if (*max_samps_ptr != *samples_ptr) {
    pio(24, 0, 0);
    pio(2, 0, &dum_read);
    pio(26, 0, 0);
  }
  while (lamchks) {
    pio(8, 0, 0);
    if (CamQ(0) & 1)
      lamchks = 0;
    else
      lamchks--;
  }
  return 1;
}

static int DevXToI(float start_time,
		   float end_time,
		   struct descriptor_dimension *dimension,
		   int min_idx, int max_idx, int *start_idx, int *end_idx)
{
  float times[2];
  unsigned int nostart = (start_time == -1);
  unsigned int noend = (end_time == -1);
  int num_times = 0;
  int status = 1;
  static struct descriptor_xd idxs_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
  if (!nostart)
    times[num_times++] = start_time;
  if (!noend)
    times[num_times++] = end_time;
  if (num_times) {
    static struct descriptor_a time_array_dsc =
	{ sizeof(float), DTYPE_NATIVE_FLOAT, CLASS_A, 0, 0, 0, {0, 0, 0, 0, 0}, 1, 0 };
    time_array_dsc.pointer = (char *)times;
    time_array_dsc.arsize = num_times * sizeof(float);
    status = TdiXtoI((struct descriptor *)dimension, &time_array_dsc, &idxs_xd MDS_END_ARG);
    if (status & 1) {
      status = TdiNint((struct descriptor *)&idxs_xd, &idxs_xd MDS_END_ARG);
      if (status & 1) {
	status = TdiLong((struct descriptor *)&idxs_xd, &idxs_xd MDS_END_ARG);
	if (status & 1) {
	  struct descriptor_a *a_ptr = (struct descriptor_a *)idxs_xd.pointer;
	  int *i_ptr = (int *)a_ptr->pointer;
	  if (!nostart) {
	    *start_idx = min(max(min_idx, *i_ptr), max_idx);
	    i_ptr++;
	  } else
	    *start_idx = min_idx;
	  if (!noend)
	    *end_idx = min(max(min_idx, *i_ptr), max_idx);
	  else
	    *end_idx = max_idx;
	} else {
	  *end_idx = max_idx;
	  *start_idx = min_idx;
	}
      } else {
	*end_idx = max_idx;
	*start_idx = min_idx;
      }
    } else {
      *end_idx = max_idx;
      *start_idx = min_idx;
    }
    MdsFree1Dx(&idxs_xd, 0);
  } else {
    *start_idx = min_idx;
    *end_idx = max_idx;
  }
  return status;
}

/*------------------------------------------------------------------------------

 Macros:                                                                      */

/*------------------------------------------------------------------------------

 Global variables:                                                            */

/*------------------------------------------------------------------------------

 Executable:                                                                  */

EXPORT int l8210__dw_setup(struct descriptor *niddsc __attribute__ ((unused)), struct descriptor *methoddsc __attribute__ ((unused)), Widget parent)
{
  Widget dbox;
  static String uids[] = { "L8210.uid", "L8210_HEADERS.uid" };
  static int nid;
  static NCI_ITM nci[] =
      { {4, NciCONGLOMERATE_NIDS, (unsigned char *)&nid, 0}, {0, NciEND_OF_LIST, 0, 0} };
  static MrmRegisterArg uilnames[] = { {"mems_nid", 0},
  {"apply_proc", (XtPointer) apply_proc},
  {"reset_proc", (XtPointer) reset_proc},
  {"pts_activate_proc", (XtPointer) pts_activate_proc},
  {"pts_dismiss_proc", (XtPointer) pts_dismiss_proc}
  };
  int status;
  TreeGetNci(*(int *)niddsc->pointer, nci);
  uilnames[0].value = (char *)0 + (nid + L8210_N_MEMORIES);
  status =
      XmdsDeviceSetup(parent, (int *)niddsc->pointer, uids, XtNumber(uids), "L8210", uilnames,
		      XtNumber(uilnames), &dbox);
  reset(dbox);
  return status;
}

static Boolean apply_proc(Widget w)
{
  int status;
  Widget mem_scale = XtNameToWidget(XtParent(w), "mems_scale");
  static int mems;
  static DESCRIPTOR_LONG(mems_dsc, &mems);
  XtPointer user_data;
  int nid;
  XtVaGetValues(mem_scale, XmNuserData, &user_data, NULL);
  nid = (intptr_t) user_data;
  XmScaleGetValue(mem_scale, &mems);
  status = TreePutRecord(nid, &mems_dsc, 0) & 1;
  if (!status)
    XmdsComplain(XtParent(w), "Error writing num memories");
  if (status)
    XmdsApplyAllXds(XtParent(w));
  return status & 1;
}

static void pts_activate_proc(Widget w)
{
  Widget parent_w = XtParent(w);
  Widget mems_w = XtNameToWidget(parent_w, "mems_scale");
  Widget header_w = XtNameToWidget(parent_w, "header_menu");
  Widget pts_dlog = XtNameToWidget(parent_w, "*pts_dlog");
  int pts[8];
  int mems;
  struct descriptor_xd *header_xd = XmdsNidOptionMenuGetXd(header_w);
  static char header[7];
  static struct descriptor header_dsc = { sizeof(header), DTYPE_T, CLASS_S, header };
  int status __attribute__ ((unused)) = TdiText((struct descriptor *)header_xd, &header_dsc MDS_END_ARG);
  int i;
  XmScaleGetValue(mems_w, &mems);
  L8210HeaderToPTS(header, (char *)&mems, pts);
  for (i = 0; i < 8; i++) {
    char pts_str[10];
    char name_str[17];
    XmString lab;
    sprintf(pts_str, "%d", pts[i]);
    sprintf(name_str, "*.pts_%d", i);
    lab = XmStringCreateSimple(pts_str);
    XtVaSetValues(XtNameToWidget(pts_dlog, name_str), XmNlabelString, lab, NULL);
    XmStringFree(lab);
  }
  XtManageChild(pts_dlog);
}

static void pts_dismiss_proc(Widget w)
{
  XtUnmanageChild(XtParent(w));
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
}

static void reset_proc(Widget w)
{
  reset(XtParent(w));
}
