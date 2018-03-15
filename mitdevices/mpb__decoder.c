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
#include <mitdevices_msg.h>
#include <mdsdescrip.h>
#include <treeshr.h>
#include <ncidef.h>
#include <stdio.h>
#include <stdlib.h>
#include <Mrm/MrmPublic.h>
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xmds/XmdsExprField.h>
#include <Xmds/XmdsNidOptionMenu.h>
#include <Xm/Text.h>
#include <mdsshr.h>
#include <xmdsshr.h>
#include <libroutines.h>
#include <mds_stdarg.h>
#include "mpb__decoder_gen.h"
#include "devroutines.h"
#include <mds_gendevice.h>

#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) > (b) ? (a) : (b))
#define ChannelNid(headnid,channel,offset) \
    headnid + MPB__DECODER_N_CHANNEL_0 + channel * (MPB__DECODER_N_CHANNEL_1 - MPB__DECODER_N_CHANNEL_0) + offset

#define CHANNEL_MODE (MPB__DECODER_N_CHANNEL_0_MODE - MPB__DECODER_N_CHANNEL_0)
#define CHANNEL_P1 (MPB__DECODER_N_CHANNEL_0_P1 - MPB__DECODER_N_CHANNEL_0)
#define CHANNEL_P2 (MPB__DECODER_N_CHANNEL_0_P2 - MPB__DECODER_N_CHANNEL_0)
#define CHANNEL_P3 (MPB__DECODER_N_CHANNEL_0_P3 - MPB__DECODER_N_CHANNEL_0)
#define CHANNEL_P4 (MPB__DECODER_N_CHANNEL_0_P4 - MPB__DECODER_N_CHANNEL_0)
#define CHANNEL_P5 (MPB__DECODER_N_CHANNEL_0_P5 - MPB__DECODER_N_CHANNEL_0)
#define CHANNEL_P6 (MPB__DECODER_N_CHANNEL_0_P6 - MPB__DECODER_N_CHANNEL_0)
#define CHANNEL_START_LOW (MPB__DECODER_N_CHANNEL_0_START_LOW - MPB__DECODER_N_CHANNEL_0)
#define CHANNEL_EDGES (MPB__DECODER_N_CHANNEL_0_EDGES - MPB__DECODER_N_CHANNEL_0)
#define CHANNEL_EDGES_R (MPB__DECODER_N_CHANNEL_0_EDGES_R - MPB__DECODER_N_CHANNEL_0)
#define CHANNEL_EDGES_F (MPB__DECODER_N_CHANNEL_0_EDGES_F - MPB__DECODER_N_CHANNEL_0)

#define P1_LABEL 2
#define P1       3
#define P2_LABEL 4
#define P2       5
#define P3_LABEL 6
#define P3       7
#define P4_LABEL 8
#define P4       9
#define P5      10

#define CLOCK 0
#define DUAL_SPEED_CLOCK 1
#define GATED_CLOCK 2
#define GATE 3

typedef struct {
  unsigned output_control:3;
  unsigned count_up:1;
  unsigned bcd_count:1;
  unsigned repeat_count:1;
  unsigned double_load:1;
  unsigned special_gate:1;
  unsigned clock_source:4;
  unsigned falling_edge:1;
  unsigned gating:3;
  unsigned short load;
  unsigned short hold;
  unsigned start_high:1;
  unsigned:7;
} DecoderSetup;

typedef struct {
  unsigned:2;
  unsigned compare_1:1;
  unsigned compare_2:1;
  unsigned f_out_src:4;
  unsigned f_out_div:4;
  unsigned f_out_disabled:1;
  unsigned data_bus_width:1;
  unsigned data_ptr_ctrl:1;
  unsigned int_scaler_ctrl:1;
} MasterRegister;

/** output control settings **/
#define ALWAYS_HIGH         0
#define LOW_PULSES          1
#define TOGGLE              2
#define ALWAYS_LOW          4
#define HIGH_PULSES         5
/*        illegal           6
          illegal                         7
*****************************/

/** clock source settings ****/
#define TCN_1               0
#define EXT_1MHZ            1
#define EXT_100KHZ          2
#define EXT_10KHZ           3
#define EXT_1KHZ            4
#define EXT_100HZ           5
#define EVENT_OR_G0         6
#define EVENT_OR_G1         7
#define EVENT_OR_G2         8
#define EVENT_OR_G3         9
#define EVENT_OR_G4        10
#define INT_1MHZ           11
#define INT_100KHZ         12
#define INT_10KHZ          13
#define INT_1KHZ           14
#define INT_100HZ          15
/*****************************/

/******* gating control ******/
#define GATE_NONE           0
#define GATE_TCN_1          1
#define GATE_N_PLUS_1       2
#define GATE_N_1            3
#define GATE_N_HIGH         4
#define GATE_N_LOW          5
#define GATE_RISING         6
#define GATE_FALLING        7
/*****************************/

/** Pulse output modes **********/
#define PO_HIGH_PULSES         0
#define PO_LOW_PULSES          1
#define PO_TOGGLE_INITIAL_HIGH 2
#define PO_TOGGLE_INITIAL_LOW  3
/********************************/

/**** Pulse trigger modes ****/
#define TM_EVENT_TRIGGER    0
#define TM_RISING_TRIGGER   1
#define TM_FALLING_TRIGGER  2
#define TM_SOFTWARE_TRIGGER 3
/*****************************/

static int one = 1;
#define STOP_CHAN 6
#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;
#ifdef DEBUG
#define pio(addr,fcode,data, comment) { unsigned short dataval = (unsigned short)data; \
printf("pio/f=%d/a=%d/d=%x/hex ! (%d)  %s/hex\n",fcode,addr,data,data,comment);\
return_on_error(DevCamChk(CamPiow(setup->name,addr,fcode,&dataval,16,0),&one,0),status);}
#define write(addr,fcode,data, comment) { unsigned short dataval = (unsigned short)data; \
printf("pio/f=%d/a=%d/d=%x/hex ! (%d)  %s/hex\n",fcode,addr,data,data,comment);\
return_on_error(DevCamChk(CamPiow(setup->name,addr,fcode,&dataval,16,0),&one,0),status);}
#else
#define pio(addr,fcode,data, comment) { unsigned short dataval = (unsigned short) data; \
return_on_error(DevCamChk(CamPiow(setup->name,addr,fcode,&dataval,16,0),&one,0),status);}
#define write(addr,fcode,data, comment) { unsigned short dataval = (unsigned short)data; \
return_on_error(DevCamChk(CamPiow(setup->name,addr,fcode,&dataval,16,0),&one,0),status);}
#endif

static int GetInt(Widget w, int headnid, int channel, int offset, int *answer);
static void ModeChange(Widget w, int cnum, XmRowColumnCallbackStruct * cb);
static void Reset(Widget w, XtPointer client_data, XmAnyCallbackStruct * cb);
static void Show(Widget w, Widget top, XmAnyCallbackStruct * cb);
static int GetEvent(int *ref_nid, unsigned int *event_mask);
static int ClockGetSetup(Widget w, int nid, int channel, DecoderSetup * setup_out,
			 unsigned int *event_mask, struct descriptor_xd *edges,
			 struct descriptor_xd *edges_r, struct descriptor_xd *edges_f,
			 String * info);
static int GatedClockGetSetup(Widget w, int nid, int channel, DecoderSetup * setup_out,
			      unsigned int *event_mask, struct descriptor_xd *edges,
			      struct descriptor_xd *edges_r, struct descriptor_xd *edges_f,
			      String * info);
static int GateGetSetup(Widget w, int nid, int channel, DecoderSetup * setup_out,
			unsigned int *event_mask, struct descriptor_xd *edges,
			struct descriptor_xd *edges_r, struct descriptor_xd *edges_f,
			String * info);
static int DualClockGetSetup(Widget w, int nid, int channel, DecoderSetup * setup_out,
			     unsigned int *event_mask, struct descriptor_xd *edges,
			     struct descriptor_xd *edges_r, struct descriptor_xd *edges_f,
			     String * info);

#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;

//static int GetEventNum(char *name, unsigned int *mask);
//static int GetOcta(int event_nid, unsigned int *events);
static void OctaOr(unsigned int *src, unsigned int *dst);
static int OctaFFS(unsigned int *src, int *next_bit);
static int OctaIsSet(unsigned int *base, int bit);
//static int GetSetup(int chan_nid, DecoderSetup * regs);
//static int GetPseudoDevNid(struct descriptor_xd *pseudo_xd, int *dev_nid);

EXPORT int mpb__decoder___init(struct descriptor *niddsc_ptr __attribute__ ((unused)), InInitStruct * setup)
{
  int status;
  //static struct descriptor_xd xd = { 0, DTYPE_T, CLASS_XD, 0, 0 };
  unsigned int events[5][4];
  unsigned int all_events[4] = { 0, 0, 0, 0 };
  unsigned char chan_flags = 0;
  int chan;
  int start_bit;
  int event_count;
  int event_bit;
  int event_ind;
  DecoderSetup regs[5];
  static DecoderSetup channelOffRegs = { 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };	/* always low, start low */
  unsigned short cam_data;
  pio(2, 24, 0, "Disable Lam and code recognizer ");	/* Disable Lam and code recognizer */
  pio(0, 16, 0xFFEF, "Enable 16 bit buses ");	/* Enable 16 bit buses */
  pio(0, 16, 0xFFE8, "disable Pointer sequence ");	/* disable Pointer sequence */
  pio(2, 16, 0x0000, "Reset Lam Mask register ");	/* Reset Lam Mask register */
  pio(0, 10, 0x0000, "Clear Lam requests ");	/* Clear Lam requests */
  pio(0, 16, 0xFFDF, "Disarm all counters ");	/* Disarm all counters */
  pio(0, 16, 0xFF17, "point to master mode register ");	/* point to master mode register */
  write(1, 16, 0x6100, "write master register ");	/* write master register */

  /***************************
    for each channel
       put it in incrementing mode
       step the counters twice
       (this will insure that we are
        not at Terminal Count)
  ***************************/
  for (chan = 0; chan < 5; chan++) {
    static DecoderSetup reg = { 4, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };	/* always low, start low, count up */
    unsigned short step_cmd = 0xFFF0 | (chan + 1);
    unsigned short regv = *(unsigned short *)&reg;
    cam_data = chan + 0xFF01;
    pio(0, 16, cam_data, "point to this channel's mode register ");	/* point to this channel's mode register */
    write(1, 16, regv, "write the mode register ");	/* write the mode register */
    pio(0, 16, step_cmd, "Step the counters");	/* Step the counters */
    pio(0, 16, step_cmd, "Step the counters");	/* Step the counters */
  }

  /***************************
    for each channel get its
    setup by doing a get_setup
    method on it.
  *****************************/
  for (chan = 0; chan < 5; chan++) {
    int dev_nid = ChannelNid(setup->head_nid, chan, 0);
    static EMPTYXD(edges);
    static EMPTYXD(edges_r);
    static EMPTYXD(edges_f);
    unsigned short regsv;
    int edges_nid = ChannelNid(setup->head_nid, chan, CHANNEL_EDGES);
    int edges_r_nid = ChannelNid(setup->head_nid, chan, CHANNEL_EDGES_R);
    int edges_f_nid = ChannelNid(setup->head_nid, chan, CHANNEL_EDGES_F);
    MdsFree1Dx(&edges, 0);
    MdsFree1Dx(&edges_r, 0);
    MdsFree1Dx(&edges_f, 0);
    memset(events[chan], 0, sizeof(int) * 4);
    regs[chan] = channelOffRegs;
    if (TreeIsOn(dev_nid) & 1) {
      //unsigned short cam_data = chan + 1;
      int mode = -1;
      int cstat __attribute__ ((unused)) = 0;
      chan_flags |= (1 << chan);
      GetInt(0, setup->head_nid, chan, CHANNEL_MODE, &mode);
      switch (mode) {
      case CLOCK:
	cstat = ClockGetSetup(0, setup->head_nid, chan, &regs[chan], (unsigned int *)events[chan],
			      &edges, &edges_r, &edges_f, 0);
	break;
      case GATED_CLOCK:
	cstat =
	    GatedClockGetSetup(0, setup->head_nid, chan, &regs[chan], (unsigned int *)events[chan],
			       &edges, &edges_r, &edges_f, 0);
	break;
      case GATE:
	cstat = GateGetSetup(0, setup->head_nid, chan, &regs[chan], (unsigned int *)events[chan],
			     &edges, &edges_r, &edges_f, 0);
	break;
      case DUAL_SPEED_CLOCK:
	cstat =
	    DualClockGetSetup(0, setup->head_nid, chan, &regs[chan], (unsigned int *)events[chan],
			      &edges, &edges_r, &edges_f, 0);
	break;
      default:
	cstat = 0;
	break;
      }
    } else
      regs[chan] = channelOffRegs;
    TreePutRecord(edges_nid, (struct descriptor *)&edges, 0);
    TreePutRecord(edges_r_nid, (struct descriptor *)&edges_r, 0);
    TreePutRecord(edges_f_nid, (struct descriptor *)&edges_f, 0);
    cam_data = chan + 0xFF01;
    pio(0, 16, cam_data, "point to this channel's mode register ");	/* point to this channel's mode register */
    regsv = *(unsigned short *)&regs[chan];
    write(1, 16, regsv, "write the mode register ");	/* write the mode register */
    cam_data = chan + 0XFF09;
    pio(0, 16, cam_data, "point to this channel's load register ");	/* point to this channel's load register */
    write(1, 16, regs[chan].load, "write the load register ");	/* write the load register */
    cam_data = (chan + 0xFF11);
    pio(0, 16, cam_data, "point to this channel's hold register ");	/* point to this channel's hold register */
    write(1, 16, regs[chan].hold, "write the hold register ");	/* write the hold register */
    OctaOr(events[chan], all_events);
  }
  /***************************
     Count them and complain
     if there are to many.
  ****************************/
  start_bit = 0;
  event_count = 0;
  while (OctaFFS(all_events, &start_bit)) {
    event_count++;
    start_bit++;
  }
  if (event_count > 15)
    return (TIMING$_TOO_MANY_EVENTS);

  pio(0, 16, 0xFF5F, "load source CTR 1-5 ")
      /* load source CTR 1-5 */
      for (chan = 0; chan < 5; chan++) {
    if (regs[chan].start_high) {
      unsigned short cam_data = 0xFF00 | (7 << 5) | (chan + 1);
      pio(0, 16, cam_data, "set output high ");	/* set output high */
    } else {
      unsigned short cam_data = 0xFF00 | (29 << 3) | (chan + 1);
      pio(0, 16, cam_data, "set output low ");	/* set output low */
    }
  }

  /********************************
    For each event write it to the
    module with its channel mask
  *********************************/
  event_bit = 0;
  event_ind = 0;
  while (OctaFFS(all_events, &event_bit)) {
    unsigned short event_reg = event_bit;
    for (chan = 0; chan < 5; chan++) {
      if (OctaIsSet(events[chan], event_bit)) {
	event_reg |= (1 << (7 + chan));
	event_reg |= 0x1000;	/* Start scalers on all events */
      }
    }
    pio(event_ind, 17, event_reg, "set code register ");	/* set code register */
    event_ind++;
    event_bit++;
  }

  /*******************************
    Zero out the remaining events
  ********************************/
  for (event_ind = event_count; event_ind < 16; event_ind++) {
    static unsigned short event_reg = 0;
    pio(event_ind, 17, event_reg, "set code register ");	/* set code register */
  }

  /* pio(2,16,0x0020,"Lam Mask register "); *//* Lam Mask register */

  {
    unsigned short cam_data = 0xFF00 | (1 << 5) | chan_flags;
    pio(0, 16, cam_data, "Arm the counters");	/* Arm the counters */
  }
  pio(0, 26, 0, "Enable the code recognizer");	/* Enable the code recognizer */
  return status;
}

static void OctaOr(unsigned int *src, unsigned int *dst)
{
  int i;
  for (i = 0; i < 4; i++)
    dst[i] |= src[i];
  return;
}

static int OctaFFS(unsigned int *src, int *next_bit)
{
  int longword = *next_bit / 32;
  int bit_number = *next_bit % 32;
  int status = 0;
  for (; ((longword < 4) && (status == 0));) {
    int width = 32 - bit_number;
    status = libffs(&bit_number, &width, (char *)&src[longword], next_bit) & 1;
    if (status == 0) {
      bit_number = 0;
      longword++;
    }
  }
  if (status)
    *next_bit = longword * 32 + *next_bit;
  return status;
}

static int OctaIsSet(unsigned int *base, int bit)
{
  int longword = bit / 32;
  int bit_number = bit % 32;
  return (base[longword] & (1 << bit_number)) ? 1 : 0;
}
/*
static int GetEventNum(char *name, unsigned int *mask)
{
  static DESCRIPTOR(expr, "BYTE_UNSIGNED(DATA(EVENT_LOOKUP($)))");
  struct descriptor name_dsc = { 0, DTYPE_T, CLASS_S, 0 };
  static EMPTYXD(xd);
  int status;
  name_dsc.length = strlen(name);
  name_dsc.pointer = name;
  status = TdiExecute((struct descriptor *)&expr, &name_dsc, &xd MDS_END_ARG);
  if (status & 1) {
    unsigned char event = *(unsigned char *)xd.pointer->pointer;
    mask[event / 32] |= 1 << (event % 32);
  }
  return status & 1;
}
*/
EXPORT int mpb__decoder__dw_setup(struct descriptor *niddsc __attribute__ ((unused)), struct descriptor *methoddsc __attribute__ ((unused)), Widget parent)
{
  static String uids[] = { "MPB__DECODER.uid" };
  Widget w;
  int i;
  char name[] = "*c0_mode";
  static MrmRegisterArg uilnames[] = { {"Reset", (XtPointer) Reset},
  {"Show", (XtPointer) Show}
  };
  XmdsDeviceSetup(parent, (int *)niddsc->pointer, uids, XtNumber(uids), "MPB__DECODER", uilnames,
		  XtNumber(uilnames), &w);
  for (i = 0; i < 5; i++) {
    Widget mode_w;
    Widget pulldown_w;
    name[2] = '0' + i;
    mode_w = XtNameToWidget(w, name);
    XtVaGetValues(mode_w, XmNsubMenuId, &pulldown_w, NULL);
    XtAddCallback(pulldown_w, XmNentryCallback, (XtCallbackProc) ModeChange, (char *)0 + i);
  }
  Reset(w, NULL, NULL);
  return 1;
}

static void Reset(Widget w, XtPointer client_data __attribute__ ((unused)), XmAnyCallbackStruct * cb __attribute__ ((unused)))
{
  int i;
  for (i = 0; i < 5; i++) {
    XmRowColumnCallbackStruct rc_cb;
    char name[] = "*c0_mode";
    Widget mode_w;
    name[2] = '0' + i;
    mode_w = XtNameToWidget(XtParent(XtParent(XtParent(w))), name);
    rc_cb.data = (char *)0 + XmdsGetOptionIdx(mode_w);
    ModeChange(mode_w, i, &rc_cb);
  }
}

static void Show(Widget w, Widget top, XmAnyCallbackStruct * cb __attribute__ ((unused)))
{
  int i;
  String text = malloc(4000);
  for (top = w; XtParent(top) && top != XtNameToWidget(XtParent(top), "MPB__DECODER");
       top = XtParent(top)) ;
  text[0] = 0;
  for (i = 0; i < 5; i++) {
    String info;
    int mode;
    char cheading[20];
    sprintf(cheading, "Output E%d ", i);
    strcat(text, cheading);
    GetInt(top, 0, i, CHANNEL_MODE, &mode);
    switch (mode) {
    case CLOCK:
      if (ClockGetSetup(top, 0, i, 0, 0, 0, 0, 0, &info) & 1) {
	strcat(text, info);
	free(info);
      } else
	strcat(text, "has an error in its setup or cannot be evaluated\nat this time.\n\n");
      break;
    case GATED_CLOCK:
      if (GatedClockGetSetup(top, 0, i, 0, 0, 0, 0, 0, &info) & 1) {
	strcat(text, info);
	free(info);
      } else
	strcat(text, "has an error in its setup or cannot be evaluated\nat this time.\n\n");
      break;
    case GATE:
      if (GateGetSetup(top, 0, i, 0, 0, 0, 0, 0, &info) & 1) {
	strcat(text, info);
	free(info);
      } else
	strcat(text, "has an error in its setup or cannot be evaluated\nat this time.\n\n");
      break;
    case DUAL_SPEED_CLOCK:
      if (DualClockGetSetup(top, 0, i, 0, 0, 0, 0, 0, &info) & 1) {
	strcat(text, info);
	free(info);
      } else
	strcat(text, "has an error in its setup or cannot be evaluated\nat this time.\n\n");
      break;
    default:
      strcat(text, "has an invalid mode\nCannot determine setup information\n\n");
    }
  }
  XmTextSetString(XtNameToWidget(top, "*showText"), text);
  XtManageChild(XtNameToWidget(top, "*showDialog"));
  free(text);
}

static void ModeChange(Widget w, int cnum, XmRowColumnCallbackStruct * cb)
{
  char name[] = "*channel_0";
  Widget parent;
  Widget c_w = 0;
  Widget *children;
  static XmString frequency_label;
  static XmString frequency_1_label;
  static XmString frequency_2_label;
  static XmString duty_cycle_label;
  static XmString gate_label;
  static XmString pulse_time_label;
  static XmString duration_label;
  static XmString event_time_label;
  if (!frequency_label) {
    frequency_label = XmStringCreateSimple("Freq (Hz):");
    frequency_1_label = XmStringCreateSimple("Freq 1(Hz):");
    frequency_2_label = XmStringCreateSimple("Freq 2(Hz):");
    duty_cycle_label = XmStringCreateSimple("Duty cycle:");
    gate_label = XmStringCreateSimple("Gate:");
    pulse_time_label = XmStringCreateSimple("Pulse time:");
    duration_label = XmStringCreateSimple("Duration:");
    event_time_label = XmStringCreateSimple("Event time:");
  }
  name[9] = '0' + cnum;
  for (parent = XtParent(w), c_w = XtNameToWidget(parent, name); parent && !c_w;
       parent = XtParent(parent), c_w = XtNameToWidget(parent, name)) ;
  XtVaGetValues(c_w, XmNchildren, &children, NULL);
  switch (*(int *)&cb->data) {
  case CLOCK:
    {
      XtVaSetValues(children[P1_LABEL], XmNlabelString, frequency_label, NULL);
      XtVaSetValues(children[P2_LABEL], XmNlabelString, duty_cycle_label, NULL);
      XtManageChild(children[P1_LABEL]);
      XtManageChild(children[P1]);
      XtManageChild(children[P2_LABEL]);
      XtManageChild(children[P2]);
      XtUnmanageChild(children[P3_LABEL]);
      XtUnmanageChild(children[P3]);
      XtUnmanageChild(children[P4_LABEL]);
      XtUnmanageChild(children[P4]);
      XtUnmanageChild(children[P5]);
      break;
    }
  case DUAL_SPEED_CLOCK:
    {
      XtVaSetValues(children[P1_LABEL], XmNlabelString, frequency_1_label, NULL);
      XtVaSetValues(children[P2_LABEL], XmNlabelString, frequency_2_label, NULL);
      XtVaSetValues(children[P3_LABEL], XmNlabelString, gate_label, NULL);
      XtManageChild(children[P1_LABEL]);
      XtManageChild(children[P1]);
      XtManageChild(children[P2_LABEL]);
      XtManageChild(children[P2]);
      XtManageChild(children[P3_LABEL]);
      XtManageChild(children[P3]);
      XtUnmanageChild(children[P4_LABEL]);
      XtUnmanageChild(children[P4]);
      XtUnmanageChild(children[P5]);
      break;
    }
  case GATED_CLOCK:
    {
      XtVaSetValues(children[P1_LABEL], XmNlabelString, frequency_label, NULL);
      XtVaSetValues(children[P2_LABEL], XmNlabelString, duty_cycle_label, NULL);
      XtVaSetValues(children[P3_LABEL], XmNlabelString, gate_label, NULL);
      XtManageChild(children[P1_LABEL]);
      XtManageChild(children[P1]);
      XtManageChild(children[P2_LABEL]);
      XtManageChild(children[P2]);
      XtManageChild(children[P3_LABEL]);
      XtManageChild(children[P3]);
      XtUnmanageChild(children[P4_LABEL]);
      XtUnmanageChild(children[P4]);
      XtUnmanageChild(children[P5]);
      break;
    }
  case GATE:
    {
      XtVaSetValues(children[P1_LABEL], XmNlabelString, pulse_time_label, NULL);
      XtVaSetValues(children[P2_LABEL], XmNlabelString, duration_label, NULL);
      XtVaSetValues(children[P3_LABEL], XmNlabelString, event_time_label, NULL);
      XtManageChild(children[P1_LABEL]);
      XtManageChild(children[P1]);
      XtManageChild(children[P2_LABEL]);
      XtManageChild(children[P2]);
      XtManageChild(children[P3_LABEL]);
      XtManageChild(children[P3]);
      XtManageChild(children[P4_LABEL]);
      XtManageChild(children[P4]);
      XtManageChild(children[P5]);
      break;
    }
  default:
    {
      XtUnmanageChild(children[P1_LABEL]);
      XtUnmanageChild(children[P1]);
      XtUnmanageChild(children[P2_LABEL]);
      XtUnmanageChild(children[P2]);
      XtUnmanageChild(children[P3_LABEL]);
      XtUnmanageChild(children[P3]);
      XtUnmanageChild(children[P4_LABEL]);
      XtUnmanageChild(children[P4]);
      XtUnmanageChild(children[P5]);
      break;
    }
  }
}

static int StartHigh(Widget w, int headnid, int channel)
{
  if (headnid) {
    int start_low_nid = ChannelNid(headnid, channel, CHANNEL_START_LOW);
    int invert = TreeIsOn(start_low_nid);
    return (invert == TreeOFF) || (invert == TreeBOTH_OFF);
  } else {
    char name[] = "*c0_start_low";
    name[2] = '0' + channel;
    return !XmToggleButtonGetState(XtNameToWidget(w, name));
  }
}

static void GetClockSet(float dt, int *source, float *period)
{
  for (*source = EXT_1MHZ, *period = 1E-6; *period * 65534 < dt && *source <= EXT_100HZ;
       (*source)++, *period *= 10) ;
}

static int GetFloat(Widget w, int headnid, int channel, int offset, float *answer)
{
  if (headnid) {
    int nid = ChannelNid(headnid, channel, offset);
    return DevFloat(&nid, answer);
  } else {
    char name[40];
    char *parts[] = { "", "_mode", "_p1", "_p2", "_p3", "_p4", "_p5" };
    sprintf(name, "*c%d%s", channel, parts[offset]);
    return TdiGetFloat((struct descriptor *)XmdsExprFieldGetXd(XtNameToWidget(w, name)), answer);
  }
}

static int GetInt(Widget w, int headnid, int channel, int offset, int *answer)
{
  if (headnid) {
    int nid = ChannelNid(headnid, channel, offset);
    return DevLong(&nid, answer);
  } else {
    static DESCRIPTOR_LONG(ansdsc, 0);
    char name[40];
    char *parts[] = { "", "_mode", "_p1", "_p2", "_p3", "_p4", "_p5" };
    ansdsc.pointer = (char *)answer;
    sprintf(name, "*c%d%s", channel, parts[offset]);
    return TdiEvaluate((struct descriptor *)XmdsNidOptionMenuGetXd(XtNameToWidget(w, name)), &ansdsc MDS_END_ARG);
  }
}

static int GetEvent(int *ref_nid, unsigned int *event_mask)
{
  static DESCRIPTOR_NID(nid_dsc, 0);
  unsigned int i;
  static EMPTYXD(xd);
  static DESCRIPTOR(expression, "BYTE_UNSIGNED(DATA(EVENT_LOOKUP($)))");
  int status;
  nid_dsc.pointer = (char *)ref_nid;
  status = TdiExecute((struct descriptor *)&expression, &nid_dsc, &xd MDS_END_ARG);
  if (status & 1) {
    if (xd.pointer->class == CLASS_A) {
      struct descriptor_a *array = (struct descriptor_a *)xd.pointer;
      char *event = array->pointer;
      unsigned int num_events = array->arsize;
      for (i = 0; i < num_events; i++, event++)
	event_mask[(*event) / 32] |= 1 << ((*event) % 32);
    } else {
      unsigned char event = *(unsigned char *)xd.pointer->pointer;
      event_mask[event / 32] |= 1 << (event % 32);
    }
  }
  MdsFree1Dx(&xd, 0);
  return status;
}

/******************************* MODE specific code ********************************************************************/

static int ClockGetSetup(Widget w, int nid, int channel, DecoderSetup * setup_out,
			 unsigned int *event_mask, struct descriptor_xd *edges,
			 struct descriptor_xd *edges_r, struct descriptor_xd *edges_f,
			 String * info)
{
  DecoderSetup setup;
  int status;
  float dt;
  float period;
  int pulses;
  int source;
  static float dt1;
  static float dt2;
  static float dt3;
  static float frequency;
  static float duty_cycle;
  status = GetFloat(w, nid, channel, CHANNEL_P1, &frequency);
  if ((!(status & 1)) || (frequency <= 0.0))
    return TIMING$_INVCLKFRQ;
  status = GetFloat(w, nid, channel, CHANNEL_P2, &duty_cycle);
  if (!(status & 1))
    return TIMING$_INVDUTY;
  dt = 1 / frequency;
  GetClockSet(dt, &source, &period);
  setup.output_control = TOGGLE;
  setup.start_high = StartHigh(w, nid, channel);
  setup.count_up = 0;
  setup.bcd_count = 0;
  setup.repeat_count = 1;
  setup.double_load = 1;
  setup.special_gate = 0;
  setup.clock_source = source;
  setup.falling_edge = 0;
  setup.gating = GATE_NONE;
  pulses = dt / period + .5;
  setup.load = pulses * duty_cycle + .5;
  setup.hold = pulses - setup.load;
  if (setup.load == 0) {
    setup.load++;
    setup.hold--;
  }
  if (setup.hold == 0) {
    setup.load--;
    setup.hold++;
  }
  dt2 = (setup.load + setup.hold) * period;
  dt1 = setup.start_high ? dt2 : (setup.load * period);
  dt3 = setup.start_high ? (setup.load * period) : dt2;
  if (nid) {
    static int snid;
    static DESCRIPTOR_NID(snid_dsc, &snid);
    static DESCRIPTOR(range, "$ + $ : * : $");
    int event_nid = nid + MPB__DECODER_N_START_EVENT;
    static DESCRIPTOR_FLOAT(dt1_dsc, &dt1);
    static DESCRIPTOR_FLOAT(dt2_dsc, &dt2);
    static DESCRIPTOR_FLOAT(dt3_dsc, &dt3);
    snid = nid + MPB__DECODER_N_START_TIME;
    TdiCompile((struct descriptor *)&range, &snid_dsc, &dt1_dsc, &dt2_dsc, edges_r MDS_END_ARG);
    TdiCompile((struct descriptor *)&range, &snid_dsc, &dt3_dsc, &dt2_dsc, edges_f MDS_END_ARG);
    MdsFree1Dx(edges, 0);
    *setup_out = setup;
    status = GetEvent(&event_nid, event_mask);
    if (!(status & 1))
      return status;
  } else {
    String text = (*info = malloc(500));
    sprintf(text,
	    "is a single-speed clock with a frequency of %g (Hz)\n and a duty cycle of %g. The output signal starts %s.\n\n",
	    1 / dt2, (float)setup.load / (float)(setup.load + setup.hold),
	    setup.start_high ? "high" : "low");
  }
  return status;
}

static int GatedClockGetSetup(Widget w, int nid, int channel, DecoderSetup * setup_out,
			      unsigned int *event_mask, struct descriptor_xd *edges,
			      struct descriptor_xd *edges_r, struct descriptor_xd *edges_f,
			      String * info)
{
  DecoderSetup setup;
  int status;
  float dt;
  float period;
  int pulses;
  int source;
  static float dt1;
  static float dt2;
  static float dt3;
  static float frequency;
  static float duty_cycle;
  status = GetFloat(w, nid, channel, CHANNEL_P1, &frequency);
  if ((!(status & 1)) || (frequency <= 0.0))
    return TIMING$_INVCLKFRQ;
  status = GetFloat(w, nid, channel, CHANNEL_P2, &duty_cycle);
  if (!(status & 1))
    return TIMING$_INVDUTY;
  dt = 1 / frequency;
  GetClockSet(dt, &source, &period);
  setup.output_control = TOGGLE;
  setup.start_high = StartHigh(w, nid, channel);
  setup.count_up = 0;
  setup.bcd_count = 0;
  setup.repeat_count = 1;
  setup.double_load = 1;
  setup.special_gate = 0;
  setup.clock_source = source;
  setup.falling_edge = 0;
  setup.gating = GATE_N_HIGH;
  pulses = dt / period + .5;
  setup.load = pulses * duty_cycle + .5;
  setup.hold = pulses - setup.load;
  if (setup.load == 0) {
    setup.load++;
    setup.hold--;
  }
  if (setup.hold == 0) {
    setup.load--;
    setup.hold++;
  }
  dt2 = (setup.load + setup.hold) * period;
  dt1 = setup.start_high ? dt2 : (setup.load * period);
  dt3 = setup.start_high ? (setup.load * period) : dt2;
  if (nid) {
    static int gnid;
    static DESCRIPTOR_NID(gate, &gnid);
    static DESCRIPTOR(output_exp, "PACK($1+$2,REPLICATE([1,0],0,SIZE($1)/2)) : \
                                       PACK($1,REPLICATE([0,1],0,SIZE($1)/2)) : \
                                       $3");
    int event_nid = nid + MPB__DECODER_N_START_EVENT;
    static DESCRIPTOR_FLOAT(dt1_dsc, &dt1);
    static DESCRIPTOR_FLOAT(dt2_dsc, &dt2);
    static DESCRIPTOR_FLOAT(dt3_dsc, &dt3);
    gnid = ChannelNid(nid, channel, CHANNEL_P3);
    TdiCompile((struct descriptor *)&output_exp, &gate, &dt1_dsc, &dt2_dsc, edges_r MDS_END_ARG);
    TdiCompile((struct descriptor *)&output_exp, &gate, &dt3_dsc, &dt2_dsc, edges_f MDS_END_ARG);
    MdsFree1Dx(edges, 0);
    *setup_out = setup;
    status = GetEvent(&event_nid, event_mask);
    if (!(status & 1))
      return status;
  } else {
    String text = (*info = malloc(500));
    sprintf(text,
	    "is a gated clock with a frequency of %g (Hz)\n and a duty cycle of %g. The output signal starts %s.\n\n",
	    1 / dt2, (float)setup.load / (float)(setup.load + setup.hold),
	    setup.start_high ? "high" : "low");
  }
  return status;
}

static int GateGetSetup(Widget w, int nid, int channel, DecoderSetup * setup_out,
			unsigned int *event_mask, struct descriptor_xd *edges,
			struct descriptor_xd *edges_r, struct descriptor_xd *edges_f, String * info)
{
  DecoderSetup setup;
  int status;
  float dt;
  float period;
  //int pulses;
  int source;
  static float dt1;
  static float dt2;
  //static float dt3;
  float delay;
  float duration;
  float pulse_time;
  int trigger_mode;
  status = GetFloat(w, nid, channel, CHANNEL_P1, &delay);
  if (!(status & 1))
    return TIMING$_INVDELDUR;
  status = GetFloat(w, nid, channel, CHANNEL_P2, &duration);
  if (!(status & 1))
    return TIMING$_INVDELDUR;
  status = GetFloat(w, nid, channel, CHANNEL_P3, &pulse_time);
  if (!(status & 1))
    return TIMING$_INVDELDUR;
  delay = delay - pulse_time;
  dt = max(delay, duration);
  GetClockSet(dt, &source, &period);
  setup.output_control = TOGGLE;
  setup.start_high = StartHigh(w, nid, channel);
  setup.count_up = 0;
  setup.bcd_count = 0;
  setup.repeat_count = 0;
  setup.double_load = 1;
  setup.special_gate = 0;
  setup.clock_source = source;
  setup.falling_edge = 0;
  status = GetInt(w, nid, channel, CHANNEL_P5, &trigger_mode);
  if (!(status & 1))
    return TIMING$_INVTRGMOD;
  switch (trigger_mode) {
  case TM_EVENT_TRIGGER:
  case TM_RISING_TRIGGER:
    setup.gating = GATE_RISING;
    break;
  case TM_FALLING_TRIGGER:
    setup.gating = GATE_FALLING;
    break;
  case TM_SOFTWARE_TRIGGER:
    setup.gating = GATE_NONE;
    break;
  }
  setup.load = delay / period + .5;
  setup.hold = duration / period + .5;
  if (setup.load < 3)
    setup.load = 3;
  if (setup.hold < 3)
    setup.hold = 3;
  dt1 = period * (setup.start_high ? setup.load + setup.hold : setup.load);
  dt2 = period * (setup.start_high ? setup.load : setup.load + setup.hold);
  if (nid) {
    static int snid;
    static DESCRIPTOR_NID(snid_dsc, &snid);
    static DESCRIPTOR(single_exp, "$1 + $2");
    static DESCRIPTOR(double_exp, "$1 + [$2,$3]");
    //static DESCRIPTOR(output_exp,
    //		      "PACK($1+$2,REPLICATE([1,0],0,SIZE($1)/2)) : PACK($1,REPLICATE([0,1],0,SIZE($1)/2)) : $3");
    int event_nid;
    static DESCRIPTOR_FLOAT(dt1_dsc, &dt1);
    static DESCRIPTOR_FLOAT(dt2_dsc, &dt2);
    snid = ChannelNid(nid, channel, CHANNEL_P3);
    TdiCompile((struct descriptor *)&single_exp, &snid_dsc, &dt1_dsc, edges_r MDS_END_ARG);
    TdiCompile((struct descriptor *)&single_exp, &snid_dsc, &dt2_dsc, edges_f MDS_END_ARG);
    TdiCompile((struct descriptor *)&double_exp, &snid_dsc, &dt1_dsc, &dt2_dsc, edges MDS_END_ARG);
    *setup_out = setup;
    if (trigger_mode == TM_EVENT_TRIGGER)
      event_nid = ChannelNid(nid, channel, CHANNEL_P4);
    else
      event_nid = nid + MPB__DECODER_N_START_EVENT;
    status = GetEvent(&event_nid, event_mask);
    if (!(status & 1))
      return status;
  } else {
    String text = (*info = malloc(500));
    String direction = setup.start_high ? "low" : "high";
    sprintf(text,
	    "is a gate which goes %s at Event time + %g seconds\nand stays %s for %g seconds\n\n",
	    direction, setup.load * period, direction, setup.hold * period);
  }
  return status;
}

static int DualClockGetSetup(Widget w, int nid, int channel, DecoderSetup * setup_out,
			     unsigned int *event_mask, struct descriptor_xd *edges,
			     struct descriptor_xd *edges_r, struct descriptor_xd *edges_f,
			     String * info)
{
  DecoderSetup setup;
  int status;
  float dt;
  float period;
  //int pulses;
  int source;
  //static float dt1;
  //static float dt2;
  //static float dt3;
  static float frequency_1;
  static float frequency_2;
  status = GetFloat(w, nid, channel, CHANNEL_P1, &frequency_1);
  if ((!(status & 1)) || (frequency_1 <= 0.0))
    return TIMING$_INVCLKFRQ;
  status = GetFloat(w, nid, channel, CHANNEL_P2, &frequency_2);
  if ((!(status & 1)) || (frequency_2 <= 0.0))
    return TIMING$_INVCLKFRQ;
  dt = 1 / min(frequency_1, frequency_2);
  GetClockSet(dt, &source, &period);
  setup.output_control = TOGGLE;
  setup.start_high = StartHigh(w, nid, channel);
  setup.count_up = 0;
  setup.bcd_count = 0;
  setup.repeat_count = 1;
  setup.double_load = 1;
  setup.special_gate = 1;
  setup.clock_source = source;
  setup.falling_edge = 0;
  setup.gating = GATE_NONE;
  //pulses = dt / period + .5;
  setup.load = 1 / frequency_1 / period / 2 + .5;
  setup.hold = 1 / frequency_2 / period / 2 + .5;
  if (setup.load == 0)
    setup.load++;
  if (setup.hold == 0)
    setup.hold++;
  if (nid) {
    static int snid;
    static DESCRIPTOR_NID(start, &snid);
    static float dt1_f;
    static DESCRIPTOR_FLOAT(dt1, &dt1_f);
    static float dt2_f;
    static DESCRIPTOR_FLOAT(dt2, &dt2_f);
    static int start_low_v;
    static DESCRIPTOR_LONG(start_low, &start_low_v);
    static int gnid;
    static DESCRIPTOR_NID(gate, &gnid);
    static int rising;
    static DESCRIPTOR_LONG(rising_dsc, &rising);
    static DESCRIPTOR(output_exp, "MPB__DECODER_DCLOCK($1,$2,$3,$4,$5,$6)");
    int event_nid = nid + MPB__DECODER_N_START_EVENT;
    snid = nid + MPB__DECODER_N_START_TIME;
    dt1_f = period * setup.load * 2;
    dt2_f = period * setup.hold * 2;
    start_low_v = !setup.start_high;
    gnid = ChannelNid(nid, channel, CHANNEL_P3);
    rising = 1;
    TdiCompile((struct descriptor *)&output_exp, &start, &dt1, &dt2, &start_low, &gate, &rising_dsc,
	       edges_r MDS_END_ARG);
    rising = 0;
    TdiCompile((struct descriptor *)&output_exp, &start, &dt1, &dt2, &start_low, &gate, &rising_dsc,
	       edges_f MDS_END_ARG);
    MdsFree1Dx(edges, 0);
    *setup_out = setup;
    status = GetEvent(&event_nid, event_mask);
    if (!(status & 1))
      return status;
  } else {
    String text = (*info = malloc(500));
    sprintf(text,
	    "is a dual-speed clock with frequencies of %g (Hz)\n and %g. The output signal starts %s.\n\n",
	    1 / (setup.load * period * 2), 1 / (setup.hold * period * 2),
	    setup.start_high ? "high" : "low");
  }
  return status;
}
