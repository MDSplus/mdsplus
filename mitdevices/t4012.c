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
#include <usagedef.h>
#include <libroutines.h>
#include <strroutines.h>
#include "t4012_gen.h"
#include <Mrm/MrmPublic.h>
#include <Xm/Xm.h>
#include <xmdsshr.h>
#include <Xm/List.h>
#include <stdio.h>
#include <time.h>
#include "devroutines.h"

static void Load(Widget w);
static unsigned int Input(InStoreStruct * setup, int code);
static int AccessTraq(InStoreStruct * setup, int data, int memsize, void *arglist,
		      int (*routine) (struct descriptor *,...));
static int ReadChannel(InStoreStruct * setup, int chunk, int samples, unsigned short *buffer,
		       int *samples_read, int *nid, float *calib);

extern int DevWait(float);

#define T28XX_N_HEAD              0
#define T28XX_K_NODES_PER_INP     4
#define T28XX_N_INP_HEAD          0
#define T28XX_N_INP_CALIBRATION   1
#define T28XX_N_INP_STARTIDX      2
#define T28XX_N_INP_ENDIDX        3
#define CNID(chan,field) dig_nids[1+chan*T28XX_K_NODES_PER_INP+T28XX_N_INP_##field]
#define max_chunks_per_io 31
#define chunksize (((max_chunks_per_io * 1024) < (samples-*samples_read)) ? (max_chunks_per_io * 1024) : (samples-*samples_read))

#define min(a,b) (((a)<(b)) ? (a) : (b))
#define max(a,b) (((a)>(b)) ? (a) : (b))
#define arm_on_error(f,retstatus) if (!((status = f) & 1))\
                                     { printf("T4012 - error sending setup informaiton - arming anyway\n");\
                                       goto arm;}
#define return_on_error(f,retstatus) if (!((status = f) & 1)) {max_time=-1; return retstatus;}

#define retry_on_error(f, retstatus) if (!((status = f) & 1)) {if (status == DEV$_BAD_MODE && retry != 0) {retry=0; goto retry;} else return retstatus;}

static int one = 1;

#define pio(f,a,d)  return_on_error(DevCamChk(CamPiow(setup->name,a,f,d,16,0),&one,0),status);
#define piomem(f,a,d,mem)  return_on_error(DevCamChk(CamPiow(setup->name,a,f,d,mem, 0),&one,0),status);
#define qrep(f,a,n,d)  return_on_error(DevCamChk(CamQrepw(setup->name,a,f,n,d,16,0),&one,0),status);

#define T$REMOTE 0x8001
#define T$LOCAL 0x8000

static time_t start_time;
static time_t max_time = -1;

EXPORT int t4012___init(struct descriptor *nid __attribute__ ((unused)), InInitStruct * setup)
{
  int try;
  int status;
  int i;
  unsigned int set[8];
  unsigned short sampling;
  float freq;
  static float freqs[] =
      { 5000, 2000, 1000, 500, 200, 100, 50, 20, 10, 5, 2, 1, .5, .2, .1, .05, .02, .01 };
  static struct descriptor_xd xd = { 0, DTYPE_T, CLASS_XD, 0, 0 };
  int frequency_1_nid = setup->head_nid + T4012_N_FREQUENCY_1;
  int frequency_2_nid = setup->head_nid + T4012_N_FREQUENCY_2;
  int retry = 1;
 retry:
  start_time = time(0);
  max_time = 3;
  set[0] = setup->channels | 0x1000;
  set[1] = setup->post_samples | 0x2000;
  set[2] = (setup->smp_per_chan + 1023) / 1024 | 0x3000;
  if (TreeGetRecord(frequency_1_nid, &xd) & 1) {
    retry_on_error(DevFloat(&frequency_1_nid, &freq), status);
    for (i = 0; i < 16; i++)
      if (freq >= freqs[i])
	break;
    set[3] = i;
    set[3] |= 0x4000;
    set[5] = (setup->switch_mode < 3 ? setup->switch_mode : 0) | 0x6000;
    if (set[5] != 0x6000) {
      retry_on_error(DevFloat(&frequency_2_nid, &freq), status);
      for (i = 0; i < 16; i++)
	if (freq >= freqs[i])
	  break;
      set[4] = i;
      set[4] |= 0x5000;
    } else
      set[4] = 0x5000 | 17;
    set[6] = 0x7002;
  } else {
    set[3] = 0x5000 | 17;
    set[4] = 0x5000 | 17;
    set[5] = 0x6000;
    set[6] = 0x7001;
  }
  set[7] = min(setup->channels, max(1, setup->display_chan)) | 0x8000;
  MdsFree1Dx(&xd, 0);

  /* added 3/29/04 as per Ammanda hubbard,  This sequence of commands
     fixes the traqs when they get confused.

     my guess is that what is really happening is AccessTraq is failing, and 
     the remaining setup information is not being sent when the errors occur.
   */
  {
    int data;
    data = 0x2008;
    pio(17, 0, &data);
    DevWait(1.);
    data = 0x2;
    pio(17, 0, &data);
    DevWait(.2);
    pio(0, 0, &data);
    if (data != 8) {
      printf("F0 at end of Amanda's fix returned %d\n", data);
    }
  }

  arm_on_error(AccessTraq((InStoreStruct *) setup, T$REMOTE, 16, 0, 0), status);	/* Remote control */
  for (i = 0; i < 7; i++) {
    arm_on_error(AccessTraq((InStoreStruct *) setup, set[i], 16, 0, 0), status);
  }
 arm:
  /* until it starts scanning, clear the LAM & start it scanning */
  for (sampling = 0; !sampling;) {
    pio(10, 0, 0);
    pio(9, 0, 0);
    retry_on_error(AccessTraq((InStoreStruct *) setup, 14, 16, 0, 0), status);
    pio(0, 0, &sampling);
    for (try = 0; (try < 30) && (!(CamQ(0) & 1)) && (status & 1); try++) {
      DevWait((float).001);
      pio(0, 0, &sampling);
    }
    if (try == 30) {
      max_time = -1;
      if (retry != 0) {
	retry = 0;
	goto retry;
      }
      return DEV$_CAM_NOSQ;
    }
  }
  retry_on_error(AccessTraq((InStoreStruct *) setup, T$LOCAL, 16, 0, 0), status);	/* Remote control */
  max_time = -1;
  return status;
}

EXPORT int t4012___trigger(struct descriptor *nid __attribute__ ((unused)), InTriggerStruct * setup)
{
  int status;
  pio(25, 0, 0);
  return status;
}

//static float setupfreqs[] =
//    { 5000, 2000, 1000, 500, 200, 100, 50, 20, 10, 5, 2, 1, .5, .2, .1, .05, .02, .01 };
static float freqs[] =
    { 2E-7, 5E-7, 1E-6, 2E-6, 5E-6, 1E-5, 2E-5, 5E-5, 1E-4, 2E-4, 5E-4, 1E-3, 2E-3, 5E-3, 1E-2,
2E-2, 5E-2, 1E-1 };

EXPORT int t4012___store(int *niddsc __attribute__ ((unused)), InStoreStruct * setup)
{
  int channels;
  int pts;
  int memPerChannel;
  int channels_read;
  int dig;
  int dig_nid;
  static int memsize = 0;
  static unsigned short *mem;
  int idxmin;
  int idxmax;
  char digname[512];
  char *nodename;
  int chan_nid = 0;
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
  int status;
  static short offset;
  static float coefficient;
  static float f[2];
  static DESCRIPTOR_A_BOUNDS(raw, sizeof(short), DTYPE_W, 0, 1, 0);
  static int *lbound = &raw.bounds[0].l;
  static int *ubound = &raw.bounds[0].u;
  static unsigned int *acoef = &raw.m[0];
  static DESCRIPTOR_A(f2_d, sizeof(f[0]), DTYPE_NATIVE_FLOAT, f, 8);
  static DESCRIPTOR(counts_str, "counts");
  static DESCRIPTOR(volts_str, "volts");
  static DESCRIPTOR(seconds_str, "seconds");
  static DESCRIPTOR_LONG(start_d, &raw.bounds[0].l);
  static DESCRIPTOR_LONG(end_d, &raw.bounds[0].u);
  static int trigger_nid;
  static DESCRIPTOR_NID(stop_d, &trigger_nid);
  static int switch_trig_nid;
  static DESCRIPTOR_NID(swi_d, &switch_trig_nid);
  static int extern_clock_nid;
  static DESCRIPTOR_NID(ext_clock_d, &extern_clock_nid);
  static struct descriptor offset_d = { 2, DTYPE_W, CLASS_S, (char *)&offset };
  static DESCRIPTOR_FLOAT(coef_d, &coefficient);
  static DESCRIPTOR_FLOAT(f1_d, f);
  static int _roprand = 32768;
  static DESCRIPTOR_FLOAT(roprand, &_roprand);
  static FUNCTION(1) value = {
    2, DTYPE_FUNCTION, CLASS_R, (unsigned char *)&OpcValue, 0, {0}};
  static DESCRIPTOR_FUNCTION_2(subtract_exp, (unsigned char *)&OpcSubtract, &value, &offset_d);
  static DESCRIPTOR_FUNCTION_2(mult_exp, (unsigned char *)&OpcMultiply, &coef_d, &subtract_exp);
  static DESCRIPTOR_WITH_UNITS(counts, &raw, &counts_str);
  static DESCRIPTOR_WITH_UNITS(volts, &mult_exp, &volts_str);
  static DESCRIPTOR_FUNCTION_2(rangesub, (unsigned char *)&OpcSubtract, 0, &f1_d);
  static DESCRIPTOR_WINDOW(window, &start_d, &end_d, &stop_d);
  static struct descriptor *begin_ptrs[] = { &roprand, 0 };
  static struct descriptor *end_ptrs[] = { (struct descriptor *)&rangesub, &roprand };
  static DESCRIPTOR_APD(begin_apd, 0, begin_ptrs, 2);
  static DESCRIPTOR_APD(end_apd, 0, end_ptrs, 2);
  static DESCRIPTOR_RANGE(int_clock1_d, 0, 0, &f1_d);
  static DESCRIPTOR_RANGE(int_clock2_d, &begin_apd, &end_apd, &f2_d);
  static int clock_out_nid;
  static DESCRIPTOR_NID(clock_out_d, &clock_out_nid);
  static DESCRIPTOR_DIMENSION(dimension, &window, &clock_out_d);
  static DESCRIPTOR_WITH_UNITS(seconds, &dimension, &seconds_str);
  static DESCRIPTOR_SIGNAL_1(signal, &volts, &counts, &seconds);
  void *ctx = 0;
  max_time = -1;
  trigger_nid = setup->head_nid + T4012_N_TRIGGER;
  switch_trig_nid = setup->head_nid + T4012_N_SWITCH_TRIG;
  extern_clock_nid = setup->head_nid + T4012_N_EXTERN_CLOCK;
  clock_out_nid = setup->head_nid + T4012_N_CLOCK_OUT;
  pio(8, 0, 0);
  status = Input(setup, 14);
  dig_status = *(struct _t4012_status *)&status;
  if (dig_status.sampling) {
    return DEV$_NOT_TRIGGERED;
  }
  channels = Input(setup, 1);
  pts = Input(setup, 2);
  memPerChannel = Input(setup, 3) * 1024;

  if (Input(setup, 7) == 1)
    TreePutRecord(clock_out_nid, (struct descriptor *)&ext_clock_d, 0);
  else {
    int shift = Input(setup, 6);
    f[0] = freqs[Input(setup, 4)];
    if (shift) {
      f[1] = freqs[Input(setup, 5)];
      rangesub.arguments[0] = begin_ptrs[1] = (shift == 1) ? &swi_d : &stop_d;
      TreePutRecord(clock_out_nid, (struct descriptor *)&int_clock2_d, 0);
    } else
      TreePutRecord(clock_out_nid, (struct descriptor *)&int_clock1_d, 0);
  }
  idxmin = (pts - 8.) / 8. * memPerChannel;
  idxmax = idxmin + memPerChannel - 1;
  if (memsize < (memPerChannel * 2)) {
    if (memsize)
      free(mem);
    memsize = memPerChannel * 2;
    mem = malloc(memsize);
  }
  return_on_error(AccessTraq(setup, 0x8001, 16, 0, 0), status);	/* Remote control */
  nodename = TreeGetPath(setup->head_nid);
  strcpy(digname, nodename);
  TreeFree(nodename);
  strcat(digname, ":T28%%_%%");
  status = TreeFindNodeWild(digname, &dig_nid, &ctx, 1 << TreeUSAGE_DEVICE);
  for (dig = 1, channels_read = 0; (channels_read < channels) && (status & 1); dig++) {
    static int dig_nids[1 + 8 * T28XX_K_NODES_PER_INP];
    static int nidlen;
    static NCI_ITM itmlst[] =
	{ {sizeof(dig_nids), NciCONGLOMERATE_NIDS, (unsigned char *)&dig_nids, &nidlen},
    {0, NciEND_OF_LIST, 0, 0}
    };
    if (status & 1) {
      int i;
      int digchannels;
      status = TreeGetNci(dig_nid, itmlst);
      digchannels = (nidlen / sizeof(dig_nid) - 1) / T28XX_K_NODES_PER_INP;
      for (i = 0; i < digchannels && (status & 1) && channels_read < channels; i++) {
	if (TreeIsOn(CNID(i, HEAD)) & 1) {
	  int channel_select = 0x0A000 | (channels_read + 1);
	  AccessTraq(setup, channel_select, 24, 0, 0);
	  if (chan_nid && (*acoef > 1)) {
	    return_on_error(TreePutRecord(chan_nid, (struct descriptor *)&signal, 0), status);
	    chan_nid = 0;
	  } else
	    DevWait((float).005);

	  chan_nid = CNID(i, HEAD);
	  *lbound =
	      (DevLong(&CNID(i, STARTIDX), (int *)lbound) & 1) ? min(idxmax,
								     max(idxmin, *lbound)) : idxmin;
	  *ubound =
	      (DevLong(&CNID(i, ENDIDX), (int *)ubound) & 1) ? min(idxmax,
								   max(idxmin, *ubound)) : idxmax;
	  *acoef = *ubound - *lbound + 1;
	  if (*acoef > 0) {
	    int points_read = 0;
	    int first_sample_offset = *lbound - idxmin;
	    int chunk = first_sample_offset / 1024;
	    int chunk_offset = first_sample_offset % 1024;
	    float calib[] = { 0, 0 };
	    status =
		ReadChannel(setup, chunk, *acoef + chunk_offset, mem, &points_read,
			    &CNID(i, CALIBRATION), calib);
	    if (status & 1) {
	      offset = calib[0];
	      if (calib[0] == calib[1])
		coefficient = (offset > 1000) ? 10. / 4096 : 5. / 4096.;
	      else
		coefficient = calib[1];
	      raw.pointer = (char *)(mem + chunk_offset);
	      raw.a0 = raw.pointer - *lbound * sizeof(*mem);
	      *ubound = (points_read - chunk_offset) + *lbound - 1;
	      *acoef = (points_read - chunk_offset);
	      raw.arsize = *acoef * 2;
	    }
	  }
	}
	channels_read++;
      }
    }
    if (channels_read < channels && (status & 1))
      status = TreeFindNodeWild(digname, &dig_nid, &ctx, 1 << TreeUSAGE_DEVICE);
  }
  TreeFindNodeEnd(&ctx);
  if (chan_nid && (*acoef > 1))
    return_on_error(TreePutRecord(chan_nid, (struct descriptor *)&signal, 0), status);
  return status;
}

static unsigned int Input(InStoreStruct * setup, int code)
{
  int status;
  unsigned short i;
  AccessTraq(setup, code, 16, 0, 0);
  DevWait((float).005);
  pio(0, 0, &i);
  return i;
}

static int ReadChannel(InStoreStruct * setup, int chunk, int samples, unsigned short *buffer,
		       int *samples_read, int *nid, float *calib)
{
  int chunk_address = 0x0B000 | chunk;
  int points_to_read;
  int status = 1;
  //int tries;
  for (points_to_read = chunksize; status & 1 && points_to_read; points_to_read = chunksize) {
    struct {
      unsigned short status;
      unsigned short bytcnt;
      unsigned int dummy;
    } iosb = {
      0, 0, 0};
    int try;
    static DESCRIPTOR_A(calib_a, sizeof(*calib), DTYPE_NATIVE_FLOAT, 0, 2 * sizeof(*calib));
    static DESCRIPTOR_NID(nid_dsc, 0);
    void *arglist[] = { 0, &nid_dsc, &calib_a MDS_END_ARG };
    calib_a.pointer = (char *)calib;
    nid_dsc.pointer = (char *)nid;
    arglist[0] = (void *)(sizeof(arglist) / sizeof(arglist[0]));
    AccessTraq(setup, chunk_address, 24, arglist, TdiData);
    pio(8, 0, 0);
    for (try = 0; (try < 20) && (!(CamQ(0) & 1)) && (status & 1); try++) {
      pio(8, 0, 0);
    }
    pio(10, 0, 0);
    return_on_error(DevCamChk
		    (CamQstopw
		     (setup->name, 0, 2, points_to_read, buffer + *samples_read, 16,
		      (unsigned short *)&iosb), &one, 0), status);
    status = status & 1 ? iosb.status : status;
    *samples_read += iosb.bytcnt / 2;
    if (iosb.bytcnt / 2 != points_to_read)
      break;
    chunk_address += max_chunks_per_io;
  }
  return status;
}

static int AccessTraq(InStoreStruct * setup, int data, int memsize, void *arglist,
		      int (*routine) (struct descriptor *,...))
{
  int try;
  int status;
  int called = 0;
  if (max_time > 0) {
    if ((time(0) - start_time) > max_time) {
      printf("T4012 AccessTraq timeout, data=%d\n", data);
      return DEV$_BAD_MODE;
    }
  }
  piomem(17, 0, &data, memsize);
  for (try = 0; (try < 30) && (!(CamQ(0) & 1)) && (status & 1); try++) {
    if (arglist && !called) {
      called = 1;
      LibCallg(arglist, routine);
    } else
      DevWait((float).001);
    piomem(17, 0, &data, memsize);
  }
  if (try == 30)
    status = DEV$_CAM_NOSQ;
  if (arglist && !called)
    LibCallg(arglist, routine);
  return status;
}

EXPORT int t4012__dw_setup(struct descriptor *niddsc __attribute__ ((unused)), struct descriptor *methoddsc __attribute__ ((unused)), Widget parent)
{
  static String uids[] = { "T4012.uid" };
  static int nid;
  static MrmRegisterArg uilnames[] = { {"nid", (XtPointer) 0}, {"Load", (XtPointer) Load} };
  static NCI_ITM nci[] =
      { {4, NciCONGLOMERATE_NIDS, (unsigned char *)&nid, 0}, {0, NciEND_OF_LIST, 0, 0} };
  TreeGetNci(*(int *)niddsc->pointer, nci);
  uilnames[0].value = (char *)0 + nid;
  return XmdsDeviceSetup(parent, (int *)niddsc->pointer, uids, XtNumber(uids), "T4012", uilnames,
			 XtNumber(uilnames), 0);
}

static void Load(Widget w)
{
  char *t4012name;
  char dignam[512];
  int i;
  XtPointer user_data;
  int nid;
  int found = False;
  XtVaGetValues(w, XmNuserData, &user_data, NULL);
  nid = (intptr_t) user_data;
  t4012name = TreeGetPath(nid);
  strcpy(dignam, t4012name);
  strcat(dignam, ":T28%%_");
  TreeFree(t4012name);
  XmListDeleteAllItems(w);
  for (i = 1; i < 17; i++) {
    int dig_nid;
    int status;
    XmString item;
    int len = strlen(dignam);
    dignam[len++] = i < 10 ? '0' : '1';
    dignam[len++] = '0' + (i % 10);
    dignam[len++] = 0;
    status = TreeFindNode(dignam, &dig_nid);
    if (status & 1) {
      NCI_ITM itmlst[] = { {512, NciNODE_NAME, 0, 0}
      , {0, 0, 0, 0}
      };
      itmlst[0].pointer = dignam;
      TreeGetNci(dig_nid, itmlst);
      item = XmStringCreateSimple(dignam);
      XmListAddItem(w, item, 0);
      XmStringFree(item);
      found = True;
    } else
      break;
  }
  if (!found) {
    XmString item = XmStringCreateSimple("Add T28xx_01");
    XmListAddItem(w, item, 0);
    XmStringFree(item);
  }
}
