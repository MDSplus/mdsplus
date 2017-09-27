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
#include <ncidef.h>

#include <treeshr.h>
#include <Mrm/MrmPublic.h>
#include <Xm/ToggleBG.h>
#include <Xmds/XmdsExpr.h>
#include <xmdsshr.h>
#include <stdlib.h>
#include "reticon120_gen.h"
#include "devroutines.h"

static void Reset();
static int Apply();
static void PopupPixels();

static int one = 1;
static int four = 4;

#define Piow(name,f,a,d,mem)  ReturnOnError(DevCamChk(CamPiow(name, a, f, d, mem, 0), &one, 0),status)
#define Stopw(name,f,a,count,d)  ReturnOnError(DevCamChk(CamStopw(name, a, f, count, d, 16, 0), &one, 0),status)
#define ReturnOnError(f,retstatus) if (!((status = f) & 1)) return retstatus
#define min(a,b) ((a) < (b) ? (a) : (b))

EXPORT int reticon120___init(struct descriptor *niddsc_ptr __attribute__ ((unused)), InInitStruct * setup)
{
  static float periods[] = { .5, 1, 2, 4, 8, 26, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192 };
  static int intervals[] = { 1, 2, 4, 8, 16, 32, 64 };
  int i;
  int s;
  int status;
  float period;
  //int num_states;
  static char zero = 0;
  static struct descriptor zero_d = { 1, DTYPE_BU, CLASS_S, &zero };
  //static EMPTYXD(xd);
  static unsigned short pixels[256];
  static DESCRIPTOR_A(pixels_d, sizeof(unsigned short), DTYPE_WU, (char *)pixels, 512);
  int frames;
  int frame_select;
  struct {
    short period;
    union {
      struct {
	unsigned __attribute__ ((packed)) frames:11;
	unsigned __attribute__ ((packed)) stop:1;
	unsigned __attribute__ ((packed)) fill:4;
      } s;
      short frames_short;
    } u;
    short frame_select;
  } state[4];
  ReturnOnError(TdiEq(setup->pixel_select, &zero_d, &pixels_d MDS_END_ARG),
		RETICON$_BAD_PIXEL_SELECT);
  for (s = 0; s < setup->num_states; s++) {
    int p_nid = setup->head_nid + RETICON120_N_STATE_0 + s * 4 + 1;
    int f_nid = p_nid + 1;
    int fs_nid = f_nid + 1;
    ReturnOnError(DevFloat(&p_nid, &period), RETICON$_BAD_PERIOD);
    for (i = 0; periods[i] < period && i < 15; i++) ;
    state[s].period = (i == 15) ? 14 : i;
    ReturnOnError(DevLong(&f_nid, &frames), RETICON$_BAD_FRAMES);
    if (frames > 2048)
      return RETICON$_BAD_FRAMES;
    state[s].u.frames_short = 0;
    state[s].u.s.frames = frames;
    state[s].u.s.stop = s == (setup->num_states - 1);
    ReturnOnError(DevLong(&fs_nid, &frame_select), RETICON$_BAD_FRAME_SELECT);
    for (i = 0; intervals[i] != frame_select && i < 7; i++) ;
    if (i == 7)
      return RETICON$_BAD_FRAME_SELECT;
    state[s].frame_select = i + 1;
  }
  Piow(setup->name, 26, 0, 0, 16);	/* abort acquisition cycle */
  Piow(setup->name, 27, 0, 0, 16);	/* set system to load pixel select memory */
  Stopw(setup->name, 19, 0, 256, pixels);	/* load pixel select memory */
  for (s = 0; s < setup->num_states; s++) {
    Piow(setup->name, 16, s, &state[s].period, 16);	/* load integration period */
    Piow(setup->name, 17, s, &state[s].u.frames_short, 16);	/* load frame count */
    Piow(setup->name, 18, s, &state[s].frame_select, 16);	/* load frame select */
  }
  Piow(setup->name, 24, 0, 0, 16);	/* Init and arm system */
  Piow(setup->memory_name, 9, 0, 0, 16);	/* Clear memory */
  Piow(setup->memory_name, 11, 1, 0, 16);	/* Clear external memory address register */
  Piow(setup->memory_name, 17, 1, &four, 16);	/* Enable external memory addressing */
  return status;
}

EXPORT int reticon120__dw_setup(struct descriptor *niddsc __attribute__ ((unused)), struct descriptor *methoddsc __attribute__ ((unused)), Widget parent)
{
  static String uids[] = { "RETICON120.uid" };
  static int nid;
  static NCI_ITM nci[] =
      { {4, NciCONGLOMERATE_NIDS, (unsigned char *)&nid, 0}, {0, NciEND_OF_LIST, 0, 0} };
  static MrmRegisterArg uilnames[] = { {"nid", 0},
  {"Apply", (XtPointer) Apply},
  {"Reset", (XtPointer) Reset},
  {"PopupPixels", (XtPointer) PopupPixels}
  };
  TreeGetNci(*(int *)niddsc->pointer, nci);
  uilnames[0].value = (char *)0 + (nid + RETICON120_N_PIXEL_SELECT);
  return XmdsDeviceSetup(parent, (int *)niddsc->pointer, uids, XtNumber(uids), "RETICON120",
			 uilnames, XtNumber(uilnames), 0);
}

static void Reset(Widget w)
{
  Widget p_w = XtNameToWidget(XtParent(w), "*pixel_menu");
  static EMPTYXD(xd);
  int status;
  int i;
  Widget *t_w;
  XtPointer user_data;
  static int nid;
  static struct descriptor niddsc = { 4, DTYPE_NID, CLASS_S, (char *)&nid };
  struct descriptor zerod = { 1, DTYPE_B, CLASS_S, "0" };
  char pixels[256];
  XtVaGetValues(p_w, XmNuserData, &user_data, XtNchildren, &t_w, NULL);
  nid = (intptr_t) user_data;
  if ((status = TdiNe(&niddsc, &zerod, &xd MDS_END_ARG)) & 1) {
    array *a = (array *) xd.pointer;
    if (a && a->dtype == DTYPE_BU && a->class == CLASS_A && a->arsize == 256)
      memcpy(pixels, a->pointer, 256);
    else
      memset(pixels, 1, 256);
  } else
    memset(pixels, 1, 256);
  for (i = 0; i < 256; i++, t_w++) {
    while (!XmIsToggleButtonGadget(*t_w))
      t_w++;
    XmToggleButtonGadgetSetState(*t_w, pixels[i], 0);
  }
}

static int Apply(Widget w)
{
  int status;
  static char pixels[256];
  static DESCRIPTOR_A(pixels_d, 1, DTYPE_BU, (char *)pixels, 256);
  Widget p_w = XtNameToWidget(XtParent(w), "*pixel_menu");
  Widget *t_w;
  int num_states;
  int num_pix = 0;
  int state;
  int words;
  int total_frames = 0;
  int i;
  XtPointer user_data;
  static EMPTYXD(xd);
  static int nid;
  static struct descriptor niddsc = { 4, DTYPE_NID, CLASS_S, (char *)&nid };
  struct descriptor zerod = { 1, DTYPE_B, CLASS_S, "0" };
  Boolean do_put = 1;
  XtVaGetValues(p_w, XtNchildren, &t_w, NULL);
  for (i = 0; i < 256; i++, t_w++) {
    while (!XmIsToggleButtonGadget(*t_w))
      t_w++;
    pixels[i] = XmToggleButtonGadgetGetState(*t_w);
    num_pix += pixels[i];
  }
  num_states = XmdsGetOptionIdx(XtNameToWidget(XtParent(w), "*number_of_states")) + 1;
  for (state = 0; state < num_states; state++) {
    struct descriptor_xd *xd;
    int num_frames;
    char pname[] = "*p0";
    char fname[] = "*f0";
    int idx = XmdsGetOptionIdx(XtNameToWidget(XtParent(w), pname));
    pname[2] += state;
    fname[2] += state;
    if ((idx == 2 && num_pix > 249) || (idx == 1 && num_pix > 121) || (idx == 0 && num_pix > 60)) {
      XmdsComplain(XtParent(w),
		   "State %d contains invalid integration period for %d pixel groups selected\n"
		   "Maximum of 60,121 and 249 pixel groups for .5,1, and 2 msec integration periods",
		   state, num_pix);
      return 0;
    } else {
      xd = (struct descriptor_xd *)XmdsExprGetXd(XtNameToWidget(XtParent(w), fname));
      if (!xd || (!(TdiGetLong(xd->pointer, &num_frames) & 1)) || (num_frames > 2047)
	  || !num_frames) {
	XmdsComplain(XtParent(w),
		     "Invalid frame count specified for state %d\n specify a number between 1 and 2047",
		     state);
	return 0;
      } else
	total_frames += num_frames;
    }
  }
  words = total_frames * (num_pix * 4 + 2);
  if (words > 131072) {
    XmdsComplain(XtParent(w), "Setup requires %d words of memory. Only 131072 are available.\n"
		 "With the %d pixel groups you have selected you can only have a total of %d frames.\n"
		 "With %d total frames you can only select %d pixel groups.", words,
		 num_pix, 131072 / (num_pix * 4 + 2),
		 total_frames, (131072 / total_frames - 2) / 4);
    return 0;
  }
  XtVaGetValues(p_w, XmNuserData, &user_data, NULL);
  nid = (intptr_t) user_data;
  if ((status = TdiNe(&niddsc, &zerod, &xd MDS_END_ARG)) & 1) {
    array *a = (array *) xd.pointer;
    if (a && a->dtype == DTYPE_BU && a->class == CLASS_A && a->arsize == 256)
      do_put = memcmp(pixels, a->pointer, 256);
  }
  if (do_put) {
    status = TreePutRecord(nid, (struct descriptor *)&pixels_d, 0) & 1;
    if (!(status & 1))
      XmdsComplain(XtParent(w), "Error writing pixel selections");
  }
  return status;
}

static void PopupPixels(Widget w)
{
  XtManageChild(XtParent(XtNameToWidget(XtParent(w), "*pixel_menu")));
}

EXPORT int reticon120___store(struct descriptor *niddsc_ptr __attribute__ ((unused)), InStoreStruct * setup)
{
  int status;
  //float period;
  //static int pixels = 0;
  //static DESCRIPTOR_LONG(pixel_d, &pixels);
  //unsigned int frames;
  //unsigned int frame_select;

  typedef struct {
 unsigned __attribute__ ((packed)) value:12;
 unsigned __attribute__ ((packed)) state_switch:1;
 unsigned __attribute__ ((packed)) event_latch:1;
 unsigned __attribute__ ((packed)) pixel_group:1;
 unsigned __attribute__ ((packed)) frame_start:1;
  } buf;

  buf *buffer;

  struct {
    unsigned short status;
    unsigned short bytcnt;
    unsigned short flags;
    unsigned short fill;
  } iosb;

  static int words;

  Piow(setup->name, 26, 0, 0, 16);	/* abort acquisition cycle */
  Piow(setup->memory_name, 9, 0, 0, 16);	/* Reset memory */
  Piow(setup->memory_name, 24, 0, 0, 16);	/* Disable LAM */
  Piow(setup->memory_name, 0, 1, &words, 24);	/* Get number of samples stored */
  if (words) {
    static float times[8192];
    static short frame_index[8192];
    int words_to_read = words;
    int is_time;
    short *data = malloc(words * sizeof(*data));
    int frame_count;
    int frame_size;
    int pixel_count;
    static float latch_time;
    int i;
    int j = 0;
    int comment_nid = setup->head_nid + RETICON120_N_COMMENT;
    int fast = TreeIsOn(comment_nid) & 1;
    short *bptr;
    latch_time = 0.0;
    bptr = (short *)(buffer = malloc(words * sizeof(*buffer)));
    for (; words_to_read && (status & 1); words_to_read -= iosb.bytcnt / 2) {
      int count = min(words_to_read, 32767);
      if (fast && words_to_read < 32767)
	status = CamFStopw(setup->memory_name, 0, 0, count, bptr, 16, (unsigned short *)&iosb);
      else
	status = CamStopw(setup->memory_name, 0, 0, count, bptr, 16, (unsigned short *)&iosb);
      bptr += iosb.bytcnt / 2;
      status = status & 1 ? iosb.status : status;
    }
    for (i = 0, is_time = 0, frame_count = 0, pixel_count = 0, frame_size = 0, latch_time = 0, j =
	 0; i < words && (status & 1); i++) {
      if (buffer[i].frame_start) {
	if (is_time) {
	  if (frame_count >= 8192) {
	    status = RETICON$_TOO_MANY_FRAMES;
	  } else {
	    times[frame_count++] = buffer[i].value * .001;
	    pixel_count = 0;
	  }
	} else {
	  if (frame_count) {
	    if (frame_size) {
	      if (pixel_count != frame_size) {
		status = RETICON$_DATA_CORRUPTED;
	      }
	    } else
	      frame_size = pixel_count;
	  }
	  frame_index[frame_count] = buffer[i].value;
	}
	is_time = !is_time;
      } else {
	if (buffer[i].event_latch && (latch_time != 0.0))
	  latch_time = times[frame_count - 1];
	if (frame_count)
	  data[j++] = buffer[i].value;
	pixel_count++;
      }
    }
    while ((frame_count * frame_size) > words)
      frame_count--;
    for (i = j; status & 1 && i < (frame_count * frame_size); i++)
      data[i] = 0;
    if (status & 1) {
      static DESCRIPTOR_A(time_array, sizeof(float), DTYPE_NATIVE_FLOAT, times, 0);
      static DESCRIPTOR_A(frame_idx_array, sizeof(short), DTYPE_W, frame_index, 0);
      static DESCRIPTOR(seconds, "sec");
      static DESCRIPTOR_WITH_UNITS(time, &time_array, &seconds);
      //static int range_val[] = { 0, 255 };
      //static DESCRIPTOR_LONG(zero, &range_val[0]);
      //static DESCRIPTOR_LONG(two_fifty_five, &range_val[1]);
      //static DESCRIPTOR_RANGE(range, &zero, &two_fifty_five, 0);
      static int pixel_axis_nid;
      static DESCRIPTOR_NID(pixel, &pixel_axis_nid);
      static DESCRIPTOR_A_COEFF(data_array, sizeof(short), DTYPE_W, 0, 2, 0);
      static DESCRIPTOR_SIGNAL_2(signal, &data_array, 0, &pixel, &time);
      int data_nid = setup->head_nid + RETICON120_N_DATA;
      int frame_index_nid = setup->head_nid + RETICON120_N_FRAME_INDEX;
      int event_latch_nid = setup->head_nid + RETICON120_N_EVENT_LATCH;
      pixel_axis_nid = setup->head_nid + RETICON120_N_PIXEL_AXIS;
      time_array.arsize = frame_count * sizeof(float);
      frame_idx_array.arsize = frame_count * sizeof(short);
      data_array.pointer = data_array.a0 = (char *)data;
      data_array.arsize = frame_count * frame_size * sizeof(short);
      data_array.m[0] = frame_size;
      data_array.m[1] = frame_count;
      status = TreePutRecord(data_nid, (struct descriptor *)&signal, 0);
      if (status & 1) {
	status = TreePutRecord(frame_index_nid, (struct descriptor *)&frame_idx_array, 0);
	if ((status & 1) && (latch_time != 0.0)) {
	  DESCRIPTOR_FLOAT(latch, &latch_time);
	  status = TreePutRecord(event_latch_nid, &latch, 0);
	}
      }
    }
    free(buffer);
    free(data);
  }
  return status;
}

EXPORT int reticon120___trigger(struct descriptor *niddsc_ptr __attribute__ ((unused)), InTriggerStruct * setup)
{
  int status;
  Piow(setup->name, 25, 0, 0, 16);	/* trigger the module */
  return status;
}
