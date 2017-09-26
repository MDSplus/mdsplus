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
/*------------------------------------------------------------------------------

		Name:   HV1440   

		Type:   C function

     		Author:	JOSH STILLERMAN

		Date:   12-MAY-1992

    		Purpose: Support for Lecroy HV1440 power supply 

------------------------------------------------------------------------------

	Call sequence: 
		DO/METHOD module INIT
		DO/METHOD module ON
		DO/METHOD module STORE
		DO/METHOD module OFF

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

 	Description:

service for LeCroy HV1440 High Voltage mainframe.
module services up to 16 pods of 16 chans each.
comunications via LeCroy 2132 module.

Assumptions:

  Assumes that all modules in 1440 crate are 16 channel
  high voltage, 2500V either polarity, one frame only.

               1440/2132 communication protocols:
       C is 8-bit HV channel number (0-255)                T is 3-bit tag word
       D is 8 bit data                V is 12 bit voltage (NOT 2's complement)
       S is 1 bit sign (1=positive, 0=negative)            # is 5 bit sub-data
               writes w/ count & data:  COUNT + WRITE COMMAND + N*DATA

 C-02-T=0  select mainframe 1, channel C
 C-00-T=1  write demand voltage, count doesn't increment channel
 C-01-T=1  write backup voltage, count doesn't increment channel
 C-02-T=1  write demand voltage, count increments channel
 C-03-T=1  write backup voltage, count increments channel
 S-0V-T=2  voltage data, following write
 C-00-T=3  read demand voltage, channel C
           returns S-0V-T=2 voltage value, N times
           returns C-02-T=3 demand buffer id after last data
 C-04-T=3  read backup voltage, channel C
           returns S-0V-T=2 voltage value, N times
           returns C-03-T=3 backup buffer id after last data
 C-08-T=3  read measured voltage, channel C
           returns S-0V-T=2 voltage value, N times
           returns C-02-T=4 measured buffer id after last data
 C-12-T=3  read mainframe: non-updated channel, C ignored
           returns C-3-T=6; C = # of non-updateable channels, 256==>255
           followed by C-2-T=6 for up to 31 channels
 C-16-T=3  read mainframe: current limit pos, C ignored
           returns D-3-T=5
 C-20-T=3  read mainframe: current limit neg, C ignored
           returns D-2-T=5
 C-24-T=3  read mainframe: empty slots, C ignored
           returns C-03-T=4;  C is lowest chan empty slot (4LSB=0), 255=full
 C-28-T=3  read mainframe: status, C ignored
           returns D-00-T=0: bit0=1 HVon, =0 off; bit1=1 enabled, =0 disabled;
                     bit2=0 1441 operating; bit3=1442 low on; bit4=1442 hi on
 D-00-T=4  count word:  data=0 means 256
 D-00-T=5  write current limit negative to selected mainframe
 D-16-T=5  write current limit positive to selected mainframe
 D-00-T=6  turn off HV
 D-04-T=6  turn on HV
 D-08-T=6  clear faults
 D-12-T=6  disable finished response
 D-16-T=6  swap buffer
 D-20-T=6  copy buffer
 D-24-T=6  update buffer
 D-28-T=6  enable finished response
 D-01-T=6  reboot system
     ALL tag=6 commands:  ignore D, use selected frame only
                          will use all frames if #=#+2
                          return D-2-1  (with D=#/4) if response enabled

  at any time:  received errors (mainframe 1)
    0000 0000 0001 0111 - 1440 syntax error
    0000 0001 0001 0111 - 1440 hardware error
    xxxx xxxx xxxx 1111 - 2132 error

    CAMAC:   F0 A0 - read LAM register, bit0=lam1 (serious), bit1=lam2 (done)
             F2 A0 - read DATA buffer, valid if Q
             F9 A0 - clear buffers, LAM1, LAM2
             F10   - clear LAM, A0 LAM1, A1 LAM2
             F16 A0 - write to output buffer, Q if accepted, see above formats
             F24   - disable LAM, A0 LAM1, A1 LAM2
             F26   - enable LAM, A0 LAM1, A1 LAM2
             F27   - test LAM, A0 LAM1, A1 LAM2, Q if on

  Ported from Jeff Casey's FORTRAN (JAS)
------------------------------------------------------------------------------*/

#include <stdio.h>
#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <treeshr.h>
#include <mdsshr.h>
#include <libroutines.h>
#include "hv1440_gen.h"
#include "devroutines.h"




static int GetPodSettings(int nid, int *settings);
static int one = 1;
#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;
#define pio(f,a,d) \
  return_on_error(DevCamChk(CamPiow(setup->name, a, f, d, 16,0), &one, 0),status)

#define pioq(f,a,d)  return_on_error(DevCamChk(CamPiow(setup->name, a, f, d, 16, 0), &one, &one),status)
#define stopw(f,a,count,d)  return_on_error(DevCamChk(CamStopw(setup->name, a, f, count, d, 16, 0), &one, 0),status)
/* send_hv(s, c_d, v_n, t)
  s is sign bit for voltages
  c_d is either channel# or data (8 bit)
  v_n is either 12 bit voltage or 5 bit number
  t is tag value (3 bit)
*/
#define send_hv(s,c_d,v_n,t) {\
  int count = 0;\
  unsigned short ndata = 32*1024*(s) + 256*(c_d) +8*(v_n)+t;\
  int q = 0;\
  pio(10,1,0);\
  while( !q && (count < 100)) {\
    pio(16, 0, &ndata);\
    q = CamQ(0);\
    count++;\
  }\
/*  if (!q) return HV1440$_STUCK; */ \
  if (count > 1) printf("send_hv %d, %d %d %d took %d tries\n", s, c_d, v_n, t, count);\
}

#define abs(a) ((a) < 0) ? -(a) : (a)

#define HV1440_K_MAX_CHANNELS   256
#define HV1440_K_MAX_PODS       16
#define HV1440_K_CHANS_PER_POD  16

EXPORT int hv1440___init(struct descriptor *niddsc __attribute__ ((unused)), InInitStruct * setup)
{
  int pods[HV1440_K_MAX_PODS];
  int status = 1;
  int pod;
  int ind;
  int i;
  int j;
  int vmax;
  int vfmax = 0;
  int nfmax = 0;

  static int settings[HV1440_K_MAX_CHANNELS];

  vmax = ((setup->range == 1.0) ||
	  (setup->range == .625)) ? 2500 : (setup->range == .5) ? 2046 : (setup->range ==
									  .375) ? 1534 : 0;
  if (!vmax)
    return HV1440$_BAD_RANGE;

  for (pod = 0, ind = 0; pod < HV1440_K_MAX_PODS; pod++, ind += HV1440_K_CHANS_PER_POD)
    pods[pod] = GetPodSettings(setup->head_nid + HV1440_N_POD_01 + pod, &settings[ind]);

  for (i = 0; i < HV1440_K_MAX_CHANNELS; i++) {
    int abset = abs(settings[i]);
    if (abset > vmax)
      return HV1440$_OUTRNG;
    if (abset > vfmax) {
      vfmax = abset;
      nfmax = i;
    }
  }
  pio(9, 0, 0);			/* clear the interface */
  /*  pio(10,0,0); *//* enable lam1 */
  /*   pio(26,1,0); *//* enable lam2 */
  send_hv(0, nfmax, setup->frame * 2, 0);	/* select the frame */
  for (i = 0; i < HV1440_K_MAX_PODS; i++) {
    if (pods[i]) {
      int i0 = i * 16;
      send_hv(0, i0, 2, 1)
	  for (j = 0; j < HV1440_K_CHANS_PER_POD; j++) {
	int iv = abs(settings[i * HV1440_K_CHANS_PER_POD + j]) / setup->range + .5;
	int sign = (iv > 0);
	send_hv(sign, 0, iv, 2);
      }
/*       wait_hv;  */
    }
  }
  return 1;
}

static int GetPodSettings(int nid, int *settings)
{
  static int dev_nid;
  int status = DevNid(&nid, &dev_nid);
  if (status & 1) {
    static DESCRIPTOR(get_settings, "GET_SETTINGS");
    static DESCRIPTOR_NID(nid_dsc, &dev_nid);
    status =
	TreeDoMethod(&nid_dsc, (struct descriptor *)&get_settings, HV1440_K_CHANS_PER_POD,
		     settings MDS_END_ARG);
  }
  if ((status & 1) == 0) {
    int i;
    for (i = 0; i < HV1440_K_CHANS_PER_POD; i++)
      settings[i] = 0;
  }
  return status & 1;
}

EXPORT int hv1440___on(struct descriptor *niddsc __attribute__ ((unused)), InOnStruct * setup)
{
  int status = 1;
  send_hv(0, 0, 4, 6);
/*  wait_hv; */
  return 1;
}

EXPORT int hv1440___off(struct descriptor *niddsc __attribute__ ((unused)), InOffStruct * setup)
{
  int status = 1;
  send_hv(0, 0, 0, 6);
/*   wait_hv; */
  return 1;
}

EXPORT int hv1440___store(struct descriptor *niddsc __attribute__ ((unused)), InStoreStruct * setup)
{
  int count;
  int need_pod = 0;
  int status = 1;
  int pod;
  int ind;
  int i;
  int j;
  //float volt;
  int vmax;
  int vfmax = 0;
  int nfmax = 0;
  int pods[HV1440_K_MAX_PODS];
  int f2;

  static int settings[HV1440_K_MAX_CHANNELS];
  static float f_settings[HV1440_K_MAX_CHANNELS];
  static DESCRIPTOR(out_dsc, "BUILD_WITH_UNITS($, 'Volts')");
  static DESCRIPTOR_A(settings_dsc, sizeof(float), DTYPE_F, f_settings, sizeof(f_settings));
  static struct descriptor_xd out_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };

  vmax = ((setup->range == 1.0) ||
	  (setup->range == .625)) ? 2500 : (setup->range == .5) ? 2046 : (setup->range ==
									  .375) ? 1534 : 0;
  if (!vmax)
    return HV1440$_BAD_RANGE;

  for (pod = 0, ind = 0; pod < HV1440_K_MAX_PODS; pod++, ind += HV1440_K_CHANS_PER_POD)
    pods[pod] = GetPodSettings(setup->head_nid + HV1440_N_POD_01 + pod, &settings[ind]);

  for (i = 0; i < HV1440_K_MAX_CHANNELS; i++) {
    int abset = abs(settings[i]);
    if (abset > vmax)
      return HV1440$_OUTRNG;
    if (abset > vfmax) {
      vfmax = abset;
      nfmax = i;
    }
  }
  pio(9, 0, 0);
  pio(10, 0, 0);
  pio(26, 1, 0);
  f2 = setup->frame * 2;
  send_hv(0, nfmax, f2, 0);
  for (i = 0; i < HV1440_K_MAX_PODS; i++) {
    if (pods[i]) {
      need_pod = i + 1;
      for (j = 0; j < HV1440_K_CHANS_PER_POD; j++) {
	struct v_read {
	  unsigned tag:3;
	  unsigned voltage:12;
	  unsigned sign_bit:1;
	} voltage;
	int i0 = i * HV1440_K_CHANS_PER_POD + j;
	send_hv(0, i0, 8, 3);
	for (voltage.tag = 0, count = 0; ((count < 25) && (voltage.tag != 2)); count++) {
	  static float time = .1;
	  LibWait(&time);
	  pio(2, 0, (short *)&voltage);
	}
	if (count > 1)
	  printf("Readback took %d tries\n", count);
	if (count == 25)
	  return HV1440$_STUCK;
	f_settings[i * HV1440_K_CHANS_PER_POD + j] =
	    voltage.voltage * setup->range * (voltage.sign_bit ? 1. : -1.);
      }
    } else
      for (j = 0; j < HV1440_K_CHANS_PER_POD; j++)
	f_settings[i * HV1440_K_CHANS_PER_POD + j] = 0;
  }
  settings_dsc.arsize = sizeof(float) * need_pod * HV1440_K_CHANS_PER_POD;
  if (need_pod) {
    int readout_nid = setup->head_nid + HV1440_N_READOUT;
    return_on_error(TdiCompile((struct descriptor *)&out_dsc, &settings_dsc, &out_xd MDS_END_ARG), status);
    return_on_error(TreePutRecord(readout_nid, (struct descriptor *)&out_xd, 0), status);
    MdsFree1Dx(&out_xd, 0);
  }
  return 1;
}
