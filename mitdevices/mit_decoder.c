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
#include <string.h>
#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <libroutines.h>
#include "mit_decoder_gen.h"
#include "decoder.h"
#include "devroutines.h"


static int GetEvent(char *name, EventMask * mask);
//static int GetOcta(int event_nid, EventMask * events);
static void OctaOr(EventMask * src, EventMask * dst);
static int OctaFFS(EventMask * src, int *next_bit);
static int OctaIsSet(EventMask * base, int bit);
//static int GetSetup(int chan_nid, DecoderSetup * regs);
static int GetPseudoDevNid(struct descriptor_xd *pseudo_xd, int *dev_nid);
static int one = 1;
#define STOP_CHAN 6
#define START_CHAN 5
#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;
#ifdef DEBUG
#define pio(addr,fcode,datav, comment) {short data = datav; \
printf("pio/f=%d/a=%d/d=%x/hex ! (%d)  %s/hex\n",fcode,addr,data,data,comment); \
return_on_error(DevCamChk(CamPiow(setup->name,addr,fcode,&data,16,0),&one,0),status);}
#else
#define pio(addr,fcode,datav, comment) {short data = datav; \
return_on_error(DevCamChk(CamPiow(setup->name,addr,fcode,&data,16,0),&one,0),status);}
#endif

int mit_decoder___init(struct descriptor *niddsc_ptr __attribute__ ((unused)), InInitStruct * setup)
{
  int status;
  //static struct descriptor_xd xd = { 0, DTYPE_T, CLASS_XD, 0, 0 };
  EventMask events[7];
  EventMask all_events = { {0, 0, 0, 0} };
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
  pio(1, 16, 0x6100, "write master register ");	/* write master register */

  /***************************
    for each channel get its
    setup by doing a get_setup
    method on it.
  *****************************/
  for (chan = 0; chan < 5; chan++) {
    struct descriptor *out_dsc_ptr;
    static DESCRIPTOR(GET_SETUP, "get_setup");
    static int dev_nid;
    static DESCRIPTOR_NID(nid_dsc, &dev_nid);
    static struct descriptor_xd pseudo_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
    unsigned short temp;
    int c_nid =
	setup->head_nid + MIT_DECODER_N_CHANNEL_0 + (MIT_DECODER_N_CHANNEL_1 -
						     MIT_DECODER_N_CHANNEL_0) * chan;
    dev_nid = c_nid + MIT_DECODER_N_CHANNEL_0_PSEUDO_DEV - MIT_DECODER_N_CHANNEL_0;
    memset(&events[chan], 0, sizeof(EventMask));
    regs[chan] = channelOffRegs;
    if (TreeIsOn(dev_nid) & 1) {
      //unsigned short cam_data = chan + 1;
      chan_flags |= (1 << chan);
      status = TreeGetRecord(dev_nid, &pseudo_xd);
      if (!(status & 1)) {
	if (status != TreeNODATA)
	  return status;
      } else {
	return_on_error(GetPseudoDevNid(&pseudo_xd, &dev_nid), status);
	if (status != TIMING$_NOPSEUDODEV) {
	  return_on_error(TreeDoMethod
			  (&nid_dsc, (struct descriptor *)&GET_SETUP, &regs[chan], &events[chan],
			   &out_dsc_ptr MDS_END_ARG), status);
	  return_on_error(TreePutRecord(c_nid, (struct descriptor *)out_dsc_ptr, 0), status);
	}
      }
    } else
      regs[chan] = channelOffRegs;
    cam_data = chan + 0xFF01;
    pio(0, 16, cam_data, "point to this channel's mode register ");	/* point to this channel's mode register */
    temp = *(short *)&regs[chan];
    pio(1, 16, temp, "write the mode register ");	/* write the mode register */
    cam_data = chan + 0XFF09;
    pio(0, 16, cam_data, "point to this channel's load register ");	/* point to this channel's load register */
    pio(1, 16, regs[chan].load, "write the load register ");	/* write the load register */
    cam_data = (chan + 0xFF11);
    pio(0, 16, cam_data, "point to this channel's hold register ");	/* point to this channel's hold register */
    pio(1, 16, regs[chan].hold, "write the hold register ");	/* write the hold register */
    OctaOr(&events[chan], &all_events);
  }
  memset(&events[STOP_CHAN], 0, sizeof(EventMask));
  status = GetEvent("STOP_SCALERS", &events[STOP_CHAN]);
  if (status & 1)
    OctaOr(&events[STOP_CHAN], &all_events);

/********** Scalers automatically start on recognizing channel event *****
  memset(&events[START_CHAN],0,sizeof(EventMask));
  status = GetEvent("START_SCALERS", &events[START_CHAN]);
  if (status&1)
    OctaOr(&events[START_CHAN], &all_events);
**************************************************************************/

  /***************************
     Count them and complain
     if there are to many.
  ****************************/
  start_bit = 0;
  event_count = 0;
  while (OctaFFS(&all_events, &start_bit)) {
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
  while (OctaFFS(&all_events, &event_bit)) {
    unsigned short event_reg = event_bit;
    for (chan = 0; chan < 5 + 2; chan++) {
      if (OctaIsSet(&events[chan], event_bit)) {
	event_reg |= (1 << (7 + chan));
	if (chan < 4)
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

static void OctaOr(EventMask * src, EventMask * dst)
{
  int i;
  for (i = 0; i < 4; i++)
    dst->bits[i] |= src->bits[i];
  return;
}

static int OctaFFS(EventMask * src, int *next_bit)
{
  int longword = *next_bit / 32;
  int bit_number = *next_bit % 32;
  int status = 0;
  for (; ((longword < 4) && (status == 0));) {
    int width = 32 - bit_number;
    status = libffs(&bit_number, &width, (char *)&src->bits[longword], next_bit);
    if (status == 0) {
      bit_number = 0;
      longword++;
    }
  }
  if (status)
    *next_bit = longword * 32 + *next_bit;
  return status;
}

static int OctaIsSet(EventMask * base, int bit)
{
  int longword = bit / 32;
  int bit_number = bit % 32;
  return (base->bits[longword] & (1 << bit_number)) ? 1 : 0;
}

static int GetPseudoDevNid(struct descriptor_xd *pseudo_xd, int *dev_nid)
{
  unsigned int status;
  struct descriptor *d_ptr = (struct descriptor *)pseudo_xd;
  while (d_ptr && d_ptr->dtype == DTYPE_DSC)
    d_ptr = (struct descriptor *)d_ptr->pointer;
  if (d_ptr) {
    status = 1;
    switch (d_ptr->dtype) {
    case DTYPE_NID:
      *dev_nid = *(int *)d_ptr->pointer;
      break;
    case DTYPE_PATH:{
	char *name = strncpy(malloc(d_ptr->length + 1), d_ptr->pointer, d_ptr->length);
	name[d_ptr->length] = 0;
	status = TreeFindNode(name, dev_nid);
	free(name);
	break;
      }
    default:
      status = TIMING$_INVPSEUDODEV;
    }
  } else
    return TIMING$_NOPSEUDODEV;
  return status;
}

static int GetEvent(char *name, EventMask * mask)
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
    mask->bits[event / 32] |= 1 << (event % 32);
  }
  return status & 1;
}

EXPORT int mit_decoder__get_event(int *ref_nid, unsigned int *event_mask)
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
