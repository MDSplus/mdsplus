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

		Name:   HV4032   

		Type:   C function

     		Author:	JOSH STILLERMAN

		Date:   12-MAY-1992

    		Purpose: Support for Lecroy HV4032 power supply 

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

service for LeCroy HV4032 High Voltage mainframe.
module services up to 8 pods of 4 chans each.
comunications via LeCroy 2132 module.

Assumptions:
1) all pods are either 3.3kV 4 channel or empty
2) one mainframe only, with address set to 16
3) one to eight pods, inserted in order

      -- Jeff Casey    8/5/91  (FORTRAN VERSION, perhaps diff in C version)

		HV4032 communication protocols:
	           C_S is HV channel number (0-31 for HV pod channels,
	              32 for chan33 (all lo-volt pods), 33 for chan43,
	              34 for chan47 (all hi-volt pods), 35 for chan70,
	              36 for chan70c, 37 for chan99 (ramp rate).
	           C_S is also for switch value (on/off items, such as 
	              HV on/off status, etc.)
	           V_M is mainframe number (1-16 or 0 for all), or voltage
	              value (in volts for 3.3 kV pods, in 2volts for 7kV)
	           T is tag word:   (command or data type)
 C-M-0   modify one channel voltage (Chan/Mainframe Select)
              no response, waiting for V-1 command
   V-1   Set value.  3.3kV voltages, 7kV voltage/2, 7kV current*8
              must follow T=0, T=2, T=7, T=12.  responds T=11 if enabled.
 X-M-2   modify all 3.3 kV channels (Mainframe Select)
             no response, waiting for N V-1 commands, N=# 3.3kV/empty channels
 C-M-3   Read true voltage of addressed channel, responds with V-3 data
 X-M-4   Read true voltage of all 32 channels, responds with 32 V-3 words.
 S-M-5   HV on/off status set, responds T=11 if enabled.
 C-M-6   Restore addressed channel, (see zero), responds T=11 if enabled.
 X-M-7   Preset all 3.3 kV channels (mainframe select) if HV off
             no response, waiting for V-1 data.
 C-M-8   Read demand value of addressed channel, responds V-8 data.
 X-M-9   Status request, responds one T=9 word (C-M-9), no L2, for each 
             failed channel in module, followed by T=5 (S-M-5) with L2.
             S for T=5:  bit1=HV on/off, bit2=calib/regulate, 
             bit3=idle down/normal.
 X-M-10  Read demand value of all 32 channels, responds 32 V-8 voltages.
 S-M-11  Enable/Disable FINISH response, no response.
 N-M-12  Preset all 7kV channels.  no response, waiting for V-1 data.
             (received X-M-12 at any time is a parity error.)
 N-M-13  Set individual voltage/current demands for all 7kV channels.
             no response, waiting for X V-1 data, X=# 7kV channels.
 C-M-14  Zero addressed element (see restore), responds T=11 if enabled.
             (received X-M-14 at any time is an interlock trip or panic off).
 X-M-15  Identify Pod complement, responds P-10.
             (received X-X-15 at any time is transmission error).
  where          C=bits 11-16, channel address
                 X=don't care bits 11-16
                 S=Switch bit 11
                 N=Switch bit 11 (1=current, 0=voltage)
                 M=bits 5-10, mainframe address
                 V=bits 5-16, voltage (or current, ramp, etc.)
                 P=bits 5-12, pod ID 1-8, each bit 0=3.3kV, 1=7kV
                 #=bits 1-4, tag bit

    CAMAC:   F0 A0 - read LAM register, bit0=lam1 (serious), bit1=lam2 (done)
             F2 A0 - read DATA buffer, valid if Q
             F9 A0 - clear buffers, LAM1, LAM2
             F10   - clear LAM, A0 LAM1, A1 LAM2
             F16 A0 - write to output buffer, Q if accepted, see above formats
             F24   - disable LAM, A0 LAM1, A1 LAM2
             F26   - enable LAM, A0 LAM1, A1 LAM2
             F27   - test LAM, A0 LAM1, A1 LAM2, Q if on

------------------------------------------------------------------------------*/

#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <treeshr.h>
#include <mdsshr.h>
#include "hv4032_gen.h"
#include "devroutines.h"




static void GetPodSettings(int nid, int *settings);

#define HV4032_K_MAX_CHANNELS   32
#define HV4032_K_MAX_PODS       8
#define HV4032_K_CHANS_PER_POD  4

#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;
#define pio(f,a,d)  return_on_error(DevCamChk(CamPiow(setup->name, a, f, d, 16, 0), &one, 0),status)
#define pioq(f,a,d)  return_on_error(DevCamChk(CamPiow(setup->name, a, f, d, 16, 0), &one, &one),status)
#define stopw(f,a,count,d)  return_on_error(DevCamChk(CamStopw(setup->name, a, f, count, d, 16, 0), &one, 0),status)
#define lamwait			//CamLamwait(setup->name, 30);

#define send_hv2(cs,vm,t) {\
				 unsigned short ndata=1024*cs + 16*vm+t;\
				 pio(10,1,0);\
				 pioq(16,0,&ndata);\
				}

#define wait_hv2 {\
        int buf[33];\
	lamwait;\
	stopw(2,0,33,buf);\
	}

static int one = 1;
extern int DevWait(float);

EXPORT int hv4032___init(struct descriptor *niddsc __attribute__ ((unused)), InInitStruct * setup)
{
  int status = 1;
  int pod;
  int ind;
  int i;
  static int settings[HV4032_K_MAX_CHANNELS];

  for (pod = 0, ind = 0; pod < HV4032_K_MAX_PODS; pod++, ind += HV4032_K_CHANS_PER_POD)
    GetPodSettings(setup->head_nid + HV4032_N_POD_1 + pod, &settings[ind]);

  pio(9, 0, 0);			/* clear the interface */
  pio(26, 1, 0);		/* enable lam2 */
  send_hv2(37, 0, 0);		/* send data to to chan 99 all frames */
  send_hv2(0, 3, 1);		/* 125 V/sec ramp */
  wait_hv2;			/* wait for it to complete */
  send_hv2(0, 16, 2)		/* send 32 channels to frame 16 */
      for (i = 0; i < HV4032_K_MAX_CHANNELS; i++) {
    if (settings[i] != -1) {
      send_hv2(0, settings[i], 1);	/* write the settings for this channel */
    } else {
      send_hv2(0, 0, 1);	/* write zero for this channel */
    }
  }
  wait_hv2;			/* wait for it to complete */
  /* lib$wait(&15.); */
  return status;
}

static void GetPodSettings(int nid, int *settings)
{
  static int dev_nid;
  int status = DevNid(&nid, &dev_nid);
  if (status & 1) {
    static DESCRIPTOR(get_settings, "GET_SETTINGS");
    static DESCRIPTOR_NID(nid_dsc, &dev_nid);
    status =
	TreeDoMethod(&nid_dsc, (struct descriptor *)&get_settings, HV4032_K_CHANS_PER_POD,
		     settings MDS_END_ARG);
  }
  if ((status & 1) == 0) {
    int i;
    for (i = 0; i < HV4032_K_CHANS_PER_POD; i++)
      settings[i] = 0;
  }
  return;
}

EXPORT int hv4032___store(struct descriptor *niddsc __attribute__ ((unused)), InStoreStruct * setup)
{
  int status = 1;
  int i;
  static int settings[HV4032_K_MAX_CHANNELS];
  static DESCRIPTOR(out_dsc, "BUILD_WITH_UNITS($, 'Volts')");
  static DESCRIPTOR_A(settings_dsc, sizeof(int), DTYPE_L, settings, sizeof(settings));
  static struct descriptor_xd out_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
  int readout_nid = setup->head_nid + HV4032_N_READOUT;
  pio(9, 0, 0);
  pio(26, 1, 0);
  send_hv2(0, 16, 4);
  DevWait((float).01);
  for (i = 0; i < HV4032_K_MAX_CHANNELS; i++) {
    int ivolt;
    unsigned short attempts;
    for (ivolt = 0, attempts = 0; ivolt % 16 != 3 && attempts < 40; attempts++) {
      pio(2, 0, &ivolt);
      settings[i] = (ivolt - 3) / 16;
      DevWait((float).005);
    }
    if (attempts == 40)
      settings[i] = -1;
  }
  wait_hv2;
  return_on_error(TdiCompile((struct descriptor *)&out_dsc, &settings_dsc, &out_xd MDS_END_ARG), status);
  return_on_error(TreePutRecord(readout_nid, (struct descriptor *)&out_xd, 0), status);
  MdsFree1Dx(&out_xd, 0);
  return status;
}

EXPORT int hv4032___on(struct descriptor *niddsc __attribute__ ((unused)), InOnStruct * setup)
{
  int status = 1;
  send_hv2(1, 0, 5);
  wait_hv2;
  return status;
}

EXPORT int hv4032___off(struct descriptor *niddsc_ptr __attribute__ ((unused)), InOffStruct * setup)
{
  int status = 1;
  send_hv2(0, 0, 5);
  wait_hv2;
  return status;
}
