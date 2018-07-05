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
/*  CMS REPLACEMENT HISTORY, Element L8590_MEM.C */
#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <treeshr.h>
#include <stdlib.h>
#include <ncidef.h>
#include <strroutines.h>
#include <libroutines.h>
#include <Mrm/MrmPublic.h>
#include <Xm/Xm.h>
#include <Xm/List.h>
#include <xmdsshr.h>
#include "l8590_mem_gen.h"
#include "l8590_sclr_gen.h"
#include "devroutines.h"

//extern unsigned short OpcValue;

extern int l8590_sclr___get_setup(struct descriptor *niddsc __attribute__ ((unused)), InGet_setupStruct * setup);
extern int GenDeviceFree();

#define return_on_error(f,retstatus) if (!((status = f) & 1)) {\
   if (setup_status & 1) GenDeviceFree(&sclr_setup);\
   TreeSetDefaultNid(old_def); return retstatus; }

static int one = 1;
static int zero = 0;

#define min(a,b) ((a) <= (b)) ? (a) : (b)
#define max(a,b) ((a) >= (b)) ? (a) : (b)
#define L8590_MEM_K_MAX_SCALERS    9
#define pio(name,f,a,d) return_on_error(DevCamChk(CamPiow(name,a,f,d,16,0),&one,0),status);
#define stop(name,f,a,cnt,d) return_on_error(DevCamChk(CamStopw(name,a,f,cnt,d,16,0),&one,0),status);

int l8590_mem___init(struct descriptor *niddsc_ptr __attribute__ ((unused)), InInitStruct * setup)
{
  //static DESCRIPTOR(sclr_wild, "L8590_%");
  int status;
  int old_def;
  void *ctx;
  int setup_status = 0;
  static InGet_setupStruct sclr_setup;
  static int sclr_nid;
  static DESCRIPTOR_NID(sclr_niddsc, &sclr_nid);
  TreeGetDefaultNid(&old_def);
  TreeSetDefaultNid(*(int *)niddsc_ptr->pointer);
  for (ctx = 0; TreeFindNodeWild("L85090_%", &sclr_nid, &ctx, -1) & 1;) {
    setup_status = l8590_sclr___get_setup(&sclr_niddsc, &sclr_setup);
    if (setup_status & 1) {
      pio(sclr_setup.name, 9, 0, 0);
      pio(sclr_setup.name, 10, 0, 0);
      pio(sclr_setup.name, 18, 0, &sclr_setup.num_active_convert);
      pio(sclr_setup.name, 26, 0, 0);
      pio(sclr_setup.name, 18, 0, &sclr_setup.num_active_convert);
      pio(sclr_setup.name, 26, 0, 0);
      GenDeviceFree(&sclr_setup);
    } else {
      return_on_error(setup_status, status);
    }
  }
  pio(setup->name, 9, 0, 0);
  pio(setup->name, 25, 0, 0);
  pio(setup->name, 11, 0, 0);
  TreeSetDefaultNid(old_def);
  return status;
}

int l8590_mem___store(struct descriptor *niddsc_ptr __attribute__ ((unused)), InStoreStruct * setup)
{
  int total_chans = 0;
  int total_samps = 0;
  void *ctx;
  int sclrs;
  int sclr_nids[L8590_MEM_K_MAX_SCALERS];
  int active[L8590_MEM_K_MAX_SCALERS];
  int samples[L8590_MEM_K_MAX_SCALERS];
  int old_def;
  int status=1;
  static DESCRIPTOR_A_BOUNDS(raw, sizeof(unsigned short), DTYPE_WU, 0, 1, 0);
  static DESCRIPTOR(counts_str, "counts");
  static DESCRIPTOR_WITH_UNITS(counts, &raw, &counts_str);
  static InGet_setupStruct sclr_setup;
  static DESCRIPTOR_NID(sclr_niddsc, 0);
  static int latch_nid;
  static DESCRIPTOR_NID(latch, &latch_nid);
  static FUNCTION(1) dvalue = {
    2, DTYPE_FUNCTION, CLASS_R, (unsigned char *)&OpcValue, 0, {0}};
  static DESCRIPTOR_SIGNAL_1(signal, &dvalue, &counts, &latch);
  int setup_status = 0;
  latch_nid = setup->head_nid + L8590_MEM_N_LATCH;
  TreeGetDefaultNid(&old_def);
  TreeSetDefaultNid(*(int *)niddsc_ptr->pointer);
  for (ctx = 0, sclrs = 0; TreeFindNodeWild("L8590_%", &sclr_nids[sclrs], &ctx, -1) & 1; sclrs++) {
    sclr_niddsc.pointer = (char *)&sclr_nids[sclrs];
    setup_status = l8590_sclr___get_setup(&sclr_niddsc, &sclr_setup);
    if (setup_status & 1) {
      samples[sclrs] = 0;
      active[sclrs] = sclr_setup.num_active;
      pio(sclr_setup.name, 2, 0, &samples[sclrs]);
      total_samps += samples[sclrs];
      total_chans += active[sclrs];
      GenDeviceFree(&sclr_setup);
    } else {
      return_on_error(setup_status, status);
    }
  }
  if (total_samps > 32766) {
    printf("Total samples too large for L8590_MEM: %s --- %d\n", setup->name, total_samps);
    total_samps = 32767;
  }

  if (!(TreeIsOn(setup->head_nid + L8590_MEM_N_COMMENT) & 1)) {
    total_samps = total_chans * 2000;
  }

  if (total_samps) {
    int chan;
    int chan_idx;
    unsigned short *values = calloc(total_samps * 2, sizeof(*values));
    unsigned short *cdata = values + total_samps;
    int samps_per_chan = total_samps / total_chans;
    int min_idx = 0;
    int max_idx = samps_per_chan - 1;
    int i;
    int sclr;
    pio(setup->name, 19, 0, &zero);
    pio(setup->name, 18, 0, &zero);
    pio(setup->name, 25, 0, 0);
    pio(setup->name, 2, 0, &values[0]);
    pio(setup->name, 19, 0, &zero);
    stop(setup->name, 2, 0, total_samps, values);
    for (i = 0; i < total_samps; i++)
      cdata[i / total_chans + (i % total_chans) * samps_per_chan] = values[i];
    for (chan_idx = 0, sclr = 0; sclr < sclrs; sclr++) {
      for (chan = 0; chan < active[sclr]; chan++, chan_idx++) {
	int data_nid =
	    sclr_nids[sclr] + L8590_SCLR_N_INPUT_1 + (L8590_SCLR_N_INPUT_2 -
						      L8590_SCLR_N_INPUT_1) * chan;
	int start_nid = data_nid + L8590_SCLR_N_INPUT_1_STARTIDX - L8590_SCLR_N_INPUT_1;
	int end_nid = data_nid + L8590_SCLR_N_INPUT_1_ENDIDX - L8590_SCLR_N_INPUT_1;
	if (TreeIsOn(data_nid) & 1) {
	  status = DevLong(&start_nid, (int *)&raw.bounds[0].l);
	  if (status & 1)
	    raw.bounds[0].l = min(max_idx, max(min_idx, raw.bounds[0].l));
	  else
	    raw.bounds[0].l = min_idx;
	  status = DevLong(&end_nid, (int *)&raw.bounds[0].u);
	  if (status & 1)
	    raw.bounds[0].u = min(max_idx, max(min_idx, raw.bounds[0].u));
	  else
	    raw.bounds[0].u = max_idx;
	  raw.m[0] = raw.bounds[0].u - raw.bounds[0].l + 1;
	  if (raw.m[0] > 0) {
	    raw.pointer = (char *)(cdata + chan_idx * samps_per_chan + raw.bounds[0].l);
	    raw.a0 = raw.pointer - raw.bounds[0].l * sizeof(*cdata);
	    raw.arsize = raw.m[0] * 2;
	    status = TreePutRecord(data_nid, (struct descriptor *)&signal, 0);
	  }
	}
      }
    }
    free(values);
  }
  TreeSetDefaultNid(old_def);
  return status;
}

static void Load(Widget w);

EXPORT int l8590_mem__dw_setup(struct descriptor *niddsc __attribute__ ((unused)), struct descriptor *methoddsc __attribute__ ((unused)), Widget parent)
{
  static String uids[] = { "L8590_MEM.uid" };
  static int nid;
  static MrmRegisterArg uilnames[] = { {"nid", (XtPointer) 0}, {"Load", (XtPointer) Load} };
  static NCI_ITM nci[] =
      { {4, NciCONGLOMERATE_NIDS, (unsigned char *)&nid, 0}, {0, NciEND_OF_LIST, 0, 0} };
  TreeGetNci(*(int *)niddsc->pointer, nci);
  uilnames[0].value = (char *)0 + nid;
  return XmdsDeviceSetup(parent, (int *)niddsc->pointer, uids, XtNumber(uids), "L8590_MEM",
			 uilnames, XtNumber(uilnames), 0);
}

static void Load(Widget w)
{
  char *l8590_memname;
  static char nodename[13];
  static NCI_ITM itmlst[] = { {12, NciNODE_NAME, nodename, 0}, {0, 0, 0, 0} };
  int i;
  XtPointer temp;
  int nid;
  int found = False;
  XtVaGetValues(w, XmNuserData, &temp, NULL);
  nid = (intptr_t) temp;
  l8590_memname = TreeGetPath(nid);
  XmListDeleteAllItems(w);
  for (i = 1; i < 17; i++) {
    //int ctx = 0;
    int dig_nid;
    int status;
    XmString item;
    char digname[512];
    sprintf(digname, "%s:L8590_%d", l8590_memname, i);
    status = TreeFindNode(digname, &dig_nid);
    if (status & 1) {
      TreeGetNci(dig_nid, itmlst);
      item = XmStringCreateSimple(nodename);
      XmListAddItem(w, item, 0);
      XmStringFree(item);
      found = True;
    } else
      break;
  }
  TreeFree(l8590_memname);
  if (!found) {
    XmString item = XmStringCreateSimple("Add L8590_1");
    XmListAddItem(w, item, 0);
    XmStringFree(item);
  }
}
