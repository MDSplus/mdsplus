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
#include <mdsplus/mdsplus.h>
#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <treeshr.h>
#include "l8501_gen.h"
#include <ncidef.h>
#include <Mrm/MrmPublic.h>
#include <Xm/Xm.h>
#include <Xmds/XmdsNidOptionMenu.h>
#include <xmdsshr.h>
#include "devroutines.h"

#define GET_FREQ_IDX(index, nid_idx) \
  { int nid = setup->head_nid + nid_idx; \
  return_on_error(DevFloat(&nid, &freq),status);\
  for (i=0; freqs[i] != freq && i < 19; i++);\
  if (i==19) return DEV$_BAD_FREQ;\
  clk[index] = i;}

static int one = 1;

#define pio(a,f,d)  return_on_error(DevCamChk(CamPiow(setup->name, a, f, d, 16, 0), &one, 0),status)
#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;

EXPORT int l8501___init(struct descriptor *niddsc_ptr __attribute__ ((unused)), InInitStruct * setup)
{
  static float freqs[] = { 0.02, 0.05,
    0.1, 0.2, 0.5,
    1.0, 2.0, 5.0,
    10.0, 20.0, 50.0,
    100.0, 200.0, 500.0,
    1000.0, 2000.0, 5000.0,
    10000.0, 20000.0
  };

  float freq;
  int count;
  int i;
  short range;
  short clk[3];
  unsigned short clk_word;
  int status;
  int f2_count_nid = setup->head_nid + L8501_N_F2_COUNT;
  int f3_count_nid = setup->head_nid + L8501_N_F3_COUNT;

  //static int retlen;

  /*********************************************
    Read in the name and mode records.
    If any problem is encountered
    then return the error status.
  **********************************************/

  /***************************************************
    Switch on the mode.
      0 -  multi frequency f2 and f3 counted.
      1 -  multi frequency trigerable frequency shift.
      2 -  Interleaved clock (single frequency
      3 -  Burst mode (f2 and f2_count)
 
      NOTE:  No break at end of case 0.  The only difference 
      between 0 and 1 is that 0 needs the counts.
   ****************************************************/
  switch (setup->mode_convert) {
  case 0:
    return_on_error(DevLong(&f2_count_nid, &count), status);
    pio(0, 16, &count);
    return_on_error(DevLong(&f3_count_nid, &count), status);
    pio(1, 16, &count);
    MDS_ATTR_FALLTHROUGH
  case 1:
    GET_FREQ_IDX(0, L8501_N_FREQ1);
    GET_FREQ_IDX(1, L8501_N_FREQ2);
    GET_FREQ_IDX(2, L8501_N_FREQ3);
    if ((clk[0] < 3) || (clk[1] < 3) || (clk[2] < 3))
      if ((clk[0] > 15) || (clk[1] > 15) || (clk[2] > 15))
	return DEV$_BAD_FREQ;
      else {
	range = 2;
	clk_word = clk[0] | (clk[1] << 4) | (clk[2] << 8);
    } else {
      range = 1;
      clk_word = (clk[0] - 3) | ((clk[1] - 3) << 4) | ((clk[2] - 3) << 8);
    }
    break;

  case 2:
    GET_FREQ_IDX(0, L8501_N_FREQ1);
    if (clk[0] <= 3) {
      range = 1;
      clk_word = clk[0] - 3;
    } else {
      range = 2;
      clk_word = clk[0];
    }
    break;

  case 3:
    return_on_error(DevLong(&f2_count_nid, &count), status);
    pio(0, 16, &count);
    GET_FREQ_IDX(1, L8501_N_FREQ2);
    if (clk[1] <= 3) {
      range = 2;
      clk_word = clk[1] << 4;
    } else {
      range = 1;
      clk_word = (clk[1] - 3) << 4;
    }
    break;
  }

  /***************************************
    Write frequency range to the module
   **************************************/
  pio(2, 16, &range);

  /***************************************
    Write the frequency control word to 
    the module.
  ***************************************/

  pio(3, 16, &clk_word);

  /***************************************
    Set the mode
  ***************************************/

  pio(setup->mode_convert, 26, 0);

  /***************************************
    Reset sequence
  ***************************************/

  pio(0, 25, 0);

  return status;
}

EXPORT int l8501___store(struct descriptor *niddsc_ptr __attribute__ ((unused)), InStoreStruct * setup)
{
  static float one_thous = 1.E-3;
  static DESCRIPTOR_FLOAT(one_thousandth, &one_thous);
  static int freq1_nid;
  static DESCRIPTOR_NID(freq1, &freq1_nid);
  static int freq2_nid;
  static DESCRIPTOR_NID(freq2, &freq2_nid);
  static int freq3_nid;
  static DESCRIPTOR_NID(freq3, &freq3_nid);
  static int f2_count_nid;
  static DESCRIPTOR_NID(f2_count, &f2_count_nid);
  static int f3_count_nid;
  static DESCRIPTOR_NID(f3_count, &f3_count_nid);
  static int trigger1_nid;
  static DESCRIPTOR_NID(trigger1, &trigger1_nid);
  static int trigger2_nid;
  static DESCRIPTOR_NID(trigger2, &trigger2_nid);
  static int trigger3_nid;
  static DESCRIPTOR_NID(trigger3, &trigger3_nid);
  static DESCRIPTOR(seconds, "seconds");
  //static int mode;
  unsigned short lam;
  int status=1;
  static DESCRIPTOR_FUNCTION_2(dt1, (unsigned char *)&OpcDivide, &one_thousandth, &freq1);
  static DESCRIPTOR_FUNCTION_2(dt2, (unsigned char *)&OpcDivide, &one_thousandth, &freq2);
  static DESCRIPTOR_FUNCTION_2(dt3, (unsigned char *)&OpcDivide, &one_thousandth, &freq3);
  static float past_val = -1.E30;
  static DESCRIPTOR_FLOAT(past, &past_val);
  static float future_val = 1.E30;
  static DESCRIPTOR_FLOAT(future, &future_val);
  int clock_out_nid = setup->head_nid + L8501_N_CLOCK_OUT;
  int stop_out_nid = setup->head_nid + L8501_N_STOP_OUT;

  freq1_nid = setup->head_nid + L8501_N_FREQ1;
  freq2_nid = setup->head_nid + L8501_N_FREQ2;
  freq3_nid = setup->head_nid + L8501_N_FREQ3;
  f2_count_nid = setup->head_nid + L8501_N_F2_COUNT;
  f3_count_nid = setup->head_nid + L8501_N_F3_COUNT;
  trigger1_nid = setup->head_nid + L8501_N_TRIGGER1;
  trigger2_nid = setup->head_nid + L8501_N_TRIGGER2;
  trigger3_nid = setup->head_nid + L8501_N_TRIGGER3;

  /*********************************************
    Read in the clock mode record.
  **********************************************/
  switch (setup->mode_convert) {

  /**************************************
    Mode 1 is Counted frequency shifts.
    clock runs at F1 until Trigger 1 
    then runs at F2 for a preset number
    of pulses and then switches to F3
    for a preset number of pulses and then
    generates a stop trigger.
  ****************************************/
  case 0:
    if (TreeIsOn(clock_out_nid) & 1) {
      static DESCRIPTOR_FUNCTION_2(mult, (unsigned char *)&OpcMultiply, &dt2, &f2_count);
      static DESCRIPTOR_FUNCTION_2(fswitch, (unsigned char *)&OpcAdd, &mult, &trigger1);
      static FUNCTION(3) r_start = {
      sizeof(unsigned short), DTYPE_FUNCTION, CLASS_R, (unsigned char *)&OpcVector, 3,
      { __fill_name__(struct descriptor *) & past, (struct descriptor *)&trigger1,
	(struct descriptor *)&fswitch}};
      static FUNCTION(3) r_end = {
      sizeof(unsigned short), DTYPE_FUNCTION, CLASS_R, (unsigned char *)&OpcVector, 3,
      {__fill_name__(struct descriptor *) & trigger1, (struct descriptor *)&fswitch,
       (struct descriptor *)&future}};
      static FUNCTION(3) r_delta = {
      sizeof(unsigned short), DTYPE_FUNCTION, CLASS_R, (unsigned char *)&OpcVector, 3,
      {__fill_name__(struct descriptor *) & dt1, (struct descriptor *)&dt2,
       (struct descriptor *)&dt3}};
      static DESCRIPTOR_RANGE(range, &r_start, &r_end, &r_delta);
      static DESCRIPTOR_WITH_UNITS(clock, &range, &seconds);
      return_on_error(TreePutRecord(clock_out_nid, (struct descriptor *)&clock, 0), status);
	/******************************
	 If stop trigger is wanted then
         declare an expression for the
         time of the stop trigger and 
         write it out.
        ******************************/
      if (status & 1 && TreeIsOn(stop_out_nid) & 1) {
	static DESCRIPTOR_FUNCTION_2(trig_mult_exp, (unsigned char *)&OpcMultiply, &dt3, &f3_count);
	static DESCRIPTOR_FUNCTION_2(trig_add_exp, (unsigned char *)&OpcAdd, &fswitch,
				     &trig_mult_exp);
	static DESCRIPTOR_WITH_UNITS(stop, &trig_add_exp, &seconds);
	return_on_error(TreePutRecord(stop_out_nid, (struct descriptor *)&stop, 0), status);
      }

    }
    break;

  /************************************
    Mode 2 is triggered frequency shift
    mode. 
    clock runs at F1 until Trigger 1 
    then runs at F2 until Trigger 2
    and then switches to F3.
  *************************************/
  case 1:
    if (TreeIsOn(clock_out_nid) & 1) {
	/**********************************
          Read the lam register.  If there
	  were overflows then store 
          information based on the triggers.
        ************************************/
      pio(2, 0, &lam);
      if (lam & 6) {
	static FUNCTION(3) r_start = {
	sizeof(unsigned short), DTYPE_FUNCTION, CLASS_R, (unsigned char *)&OpcVector, 3,
	{__fill_name__(struct descriptor *) & past, (struct descriptor *)&trigger1,
	 (struct descriptor *)&trigger2}};
	static FUNCTION(3) r_end = {
	sizeof(unsigned short), DTYPE_FUNCTION, CLASS_R, (unsigned char *)&OpcVector, 3,
	{__fill_name__(struct descriptor *) & trigger1, (struct descriptor *)&trigger2,
	 (struct descriptor *)&future}};
	static FUNCTION(3) r_delta = {
	sizeof(unsigned short), DTYPE_FUNCTION, CLASS_R, (unsigned char *)&OpcVector, 3,
	{__fill_name__(struct descriptor *) & dt1, (struct descriptor *)&dt2,
	 (struct descriptor *)&dt3}};
	static DESCRIPTOR_RANGE(range, &r_start, &r_end, &r_delta);
	static DESCRIPTOR_WITH_UNITS(clock, &range, &seconds);
	return_on_error(TreePutRecord(clock_out_nid, (struct descriptor *)&clock, 0), status);

	  /***************************
            write out the stop trigger
            record.
          ****************************/
	if (TreeIsOn(stop_out_nid) & 1) {
	  DESCRIPTOR_WITH_UNITS(stop, &trigger3, &seconds);
	  return_on_error(TreePutRecord(stop_out_nid, (struct descriptor *)&stop, 0), status);
	}
      } else
	  /****************************
            otherwise store information
            based on the counters.
          *****************************/
      {
	static short f2_count_act;
	//static struct descriptor f2_count_actual =
	//    { sizeof(f2_count_act), DTYPE_W, CLASS_S, (char *)&f2_count_act };
	static DESCRIPTOR_FUNCTION_2(mult, (unsigned char *)&OpcMultiply, &dt2, &f2_count_act);
	static DESCRIPTOR_FUNCTION_2(fswitch, (unsigned char *)&OpcAdd, &mult, &trigger1);
	static FUNCTION(3) r_start = {
	sizeof(unsigned short), DTYPE_FUNCTION, CLASS_R, (unsigned char *)&OpcVector, 3,
	{ __fill_name__(struct descriptor *) & past, (struct descriptor *)&trigger1,
	  (struct descriptor *)&fswitch}};
	static FUNCTION(3) r_end = {
	sizeof(unsigned short), DTYPE_FUNCTION, CLASS_R, (unsigned char *)&OpcVector, 3,
	{__fill_name__(struct descriptor *) & trigger1, (struct descriptor *)&fswitch,
	 (struct descriptor *)&future}};
	static FUNCTION(3) r_delta = {
	sizeof(unsigned short), DTYPE_FUNCTION, CLASS_R, (unsigned char *)&OpcVector, 3,
	{__fill_name__(struct descriptor *) & dt1, (struct descriptor *)&dt2,
	 (struct descriptor *)&dt3}};
	static DESCRIPTOR_RANGE(range, (struct descriptor *)&r_start, (struct descriptor *)&r_end,
				(struct descriptor *)&r_delta);
	static DESCRIPTOR_WITH_UNITS(clock, &range, &seconds);
	pio(0, 0, &f2_count_act);
	return_on_error(TreePutRecord(clock_out_nid, (struct descriptor *)&clock, 0), status);

	  /******************************
	   if stop trigger is wanted then 
             declare an expression for the
             time of the stop trigger and
             write it out.
          ******************************/
	if (TreeIsOn(stop_out_nid) & 1) {
	  static short f3_count_act;
	  //static struct descriptor f3_count_actual =
	  //    { sizeof(f3_count_act), DTYPE_W, CLASS_S, (char *)&f3_count_act };
	  static DESCRIPTOR_FUNCTION_2(mult, (unsigned char *)&OpcMultiply, &dt3, &f3_count_act);
	  static DESCRIPTOR_FUNCTION_2(fswitch2, (unsigned char *)&OpcAdd, &mult, &fswitch);
	  static DESCRIPTOR_WITH_UNITS(stop, &fswitch2, &seconds);
	  pio(1, 0, &f3_count);
	  return_on_error(TreePutRecord(stop_out_nid, (struct descriptor *)&stop, 0), status);
	}
      }
    }
    break;

  /***************************************
    Mode 3 - single frequency clock mode.
  ***************************************/
  case 2:
    if (TreeIsOn(clock_out_nid) & 1) {
      static DESCRIPTOR_RANGE(range, 0, 0, &dt1);
      return_on_error(TreePutRecord(clock_out_nid, (struct descriptor *)&range, 0), status);
    }
    break;

  /**********************
    Mode 4 - Burst mode.
  **********************/
  case 3:
    if (TreeIsOn(clock_out_nid) & 1) {
      static float one = 1.0;
      static DESCRIPTOR_FLOAT(one_dsc, &one);
      static DESCRIPTOR_FUNCTION_1(double_dt, (unsigned char *)&OpcDble, &dt2);
      static DESCRIPTOR_FUNCTION_2(counts, (unsigned char *)&OpcSubtract, &f2_count, &one_dsc);
      static DESCRIPTOR_FUNCTION_2(duration, (unsigned char *)&OpcMultiply, &counts, &double_dt);
      static DESCRIPTOR_FUNCTION_2(r_end, (unsigned char *)&OpcAdd, &duration, &trigger2);
      static DESCRIPTOR_RANGE(range, &trigger2, &r_end, &double_dt);
      static DESCRIPTOR_WITH_UNITS(clock, &range, &seconds);
      return_on_error(TreePutRecord(clock_out_nid, (struct descriptor *)&clock, 0), status);
      if (TreeIsOn(stop_out_nid) & 1) {
	static DESCRIPTOR_WITH_UNITS(stop, &r_end, &seconds);
	return_on_error(TreePutRecord(stop_out_nid, (struct descriptor *)&stop, 0), status);
      }
    }
    break;

  /***********************
    something is seriously
    wrong !!
  ***********************/
  default:
    status = DEV$_BAD_MODE;
    break;
  }
  return status;
}

static int Check(Widget w);

EXPORT int l8501__dw_setup(struct descriptor *niddsc __attribute__ ((unused)), struct descriptor *methoddsc __attribute__ ((unused)), Widget parent)
{
  static String uids[] = { "L8501.uid" };
  static int nid;
  static MrmRegisterArg uilnames[] = { {"nid", (XtPointer) 0}, {"Check", (XtPointer) Check} };
  static NCI_ITM nci[] =
      { {4, NciCONGLOMERATE_NIDS, (unsigned char *)&nid, 0}, {0, NciEND_OF_LIST, 0, 0} };
  TreeGetNci(*(int *)niddsc->pointer, nci);
  uilnames[0].value = (char *)0 + nid;
  return XmdsDeviceSetup(parent, (int *)niddsc->pointer, uids, XtNumber(uids), "L8501", uilnames,
			 XtNumber(uilnames), 0);
}

static int Check(Widget w)
{
  int status = 1;
  static float freq1;
  static float freq2;
  static float freq3;
  static DESCRIPTOR_FLOAT(freq1_d, &freq1);
  static DESCRIPTOR_FLOAT(freq2_d, &freq2);
  static DESCRIPTOR_FLOAT(freq3_d, &freq3);
  static int mode;
  static DESCRIPTOR_LONG(mode_d, &mode);
  struct descriptor_xd *xd;
  if ((xd = XmdsNidOptionMenuGetXd(XtNameToWidget(XtParent(w), "*mode"))) &&
      (TdiData((struct descriptor *)xd, &mode_d MDS_END_ARG) & 1) &&
      (mode == 1 || mode == 2) &&
      (xd = XmdsNidOptionMenuGetXd(XtNameToWidget(XtParent(w), "*freq1")))
      && (TdiData((struct descriptor *)xd, &freq1_d MDS_END_ARG) & 1)
      && (xd = XmdsNidOptionMenuGetXd(XtNameToWidget(XtParent(w), "*freq2")))
      && (TdiData((struct descriptor *)xd, &freq2_d MDS_END_ARG) & 1)
      && (xd = XmdsNidOptionMenuGetXd(XtNameToWidget(XtParent(w), "*freq3")))
      && (TdiData((struct descriptor *)xd, &freq3_d MDS_END_ARG) & 1) && (freq1 < .2 || freq2 < .2 || freq3 < .2)
      && (freq1 > 2000. || freq2 > 2000. || freq3 > 2000.)) {
    status = 0;
    XmdsComplain(w,
		 "Span of frequencies is two great.\nAll frequencies must be in the range of 20Hz - 2MHz\nor 200Hz - 20MHz");
  }
  return status;
}
