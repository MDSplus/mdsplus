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

		Name:   FERA$ROUTINES   

		Type:   C function

     		Author:	JOSH STILLERMAN

		Date:   23-APR-1992

    		Purpose: Support for FERA modules. 

------------------------------------------------------------------------------

	Call sequence: 

  INIT, STORE, methods for FERA

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

INIT and STORE service for FERA (LeCroy Fast Encoding Readout Adc) modules:  

4300 charge sensitive (current integrating) 16-channel .25pC/count digitizers
4301 controller (tester, fan-in, fan-out)
4302 memories (16k words)
---------------------------------------------------------------------------

Assumption:  all 4300 modules in use are 4300B/610.  i.e.  they are 11 bit 
   50 Ohm modules.  (Not really important, but the valid data checks and 
   timing notes below are for 11 bit modules).

Assumption:  4300 modules and 4301 controller have command and data ECLport 
   buses connected.  Single 4300 systems have 4301 REO connected to 4300 REN,
   and 4300 PASS connected to 4301 CLI.  Multiple 4300 systems are daisy 
   chained:  4301 REO to first 4300 REN, first 4300 PASS to next 4300 REN, etc;
   and last 4300 PASS back to 4301 CLI.

Assumption:  Only the last (furthest) 4300 has the pull down resistors 
   installed in the ECLport buses.  All the intermediate 4300 modules have 
   the PD resistors removed.  The front panel PD lights are on if the 
   resistors are installed.

Assumption:  4301 controller ECLport WSO line is connected to all 4302 WSI 
   ECL inputs.  All ECL 56 Ohm termination resistors except for the last are 
   removed from 4302 modules.  The 4301 NIM WSO line is connected back to the 
   4302 NIM WAI line via a 10-15 foot coax cable (15-20 nsec delay).
The 4301 and all 4302s ECL databus are connected, and all but the last 4302 
   modules have the 56 Ohm termination resistors removed.
Single 4302 systems have no additional connections.  Multiple 4302 systems are 
   daisy chained:  the FULL-bar output from the first 4302 is connected to the 
   VETO line of the second 4302, the FULL-bar of the second to the VETO of the 
   third, etc.

The gating pulse for the entire system is introduced at the 4301 GAI NIM input, 
   and should be of NIM signal levels.

This routine will set up the system so that incoming gates to the 4301 
   controller will generate samples on all (16 channels * number of 4300 
   units) FERA channels, which are read over to the 4302 memory(s) in 
   8.7 + 1.6*(number of 4300s) microseconds.  After the data handshaking is
   done, the 4300 modules will become active again, ready for the next gate.  
   The 4301 is not retriggerable, and gates arriving during the acquisition 
   cycle are lost.

The data is dumped into the 4302 memory modules without using the hardware 
   pedestal subtraction or zero suppression capabilities.  The memory 
   capability is 16k per memory module, since the hardware design by LeCroy 
   has a kludge deadspace.  The size of each module is set by switches on the 
   side (to 12k, 14k, 15k, or 15.5k).

        Jeff Casey     8/6/91 , mods 10/23/91, 5/92.

------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <strroutines.h>

#include <treeshr.h>
#include <mdsshr.h>
#include "fera_gen.h"
#include "devroutines.h"




/*------------------------------------------------------------------------------

 External functions or symbols referenced:                                    */

/*------------------------------------------------------------------------------

 Global variables:                                                            */

static int last_nid = 0;
static int zero = 0;
static int one = 1;
static int three = 3;
/*------------------------------------------------------------------------------

 Subroutines referenced:                                                      */

static int Unpack(unsigned short **buffer, int *num_pts, int num_mems, int chans, int pts_per_chan);
static int NElements(int nid);
static char *ArrayRef(int nid, int num);
static int Put(unsigned short int *buffer, int pts_per_chan, int chans, int clock_nid, int nid);

EXPORT int fera___init(struct descriptor *niddsc_ptr __attribute__ ((unused)), InInitStruct * setup)
{

  int status = 1;
  int num_dig;
  int num_mem;
#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;

  last_nid = 0;

  /* Reset the controler */
  return_on_error(DevCamChk(CamPiow(setup->cntrl_name, 0, 9, 0, 16, 0), &one, &one), status);

  /* setup the digitizers */
  if ((num_dig = NElements(setup->head_nid + FERA_N_DIG_NAME))) {
    int i;
    for (i = 0; i < num_dig; i++) {
      char *name = ArrayRef(setup->head_nid + FERA_N_DIG_NAME, i);
      if (name) {
	static short ecl_cam = 0x2400;
	return_on_error(DevCamChk(CamPiow(name, 0, 9, 0, 16, 0), &one, &one), status);	/* reset digitizer */
	return_on_error(DevCamChk(CamPiow(name, 0, 16, &ecl_cam, 16, 0), &one, &one), status);	/* goto ECL/CAM mode */
      } else {
	status = FERA$_DIGNOTSTRARRAY;
	break;
      }
    }
  } else
    status = FERA$_NODIG;

  /* setup the memory modules */
  if (status & 1) {
    if ((num_mem = NElements(setup->head_nid + FERA_N_MEM_NAME))) {
      int i;
      for (i = 0; i < num_mem; i++) {
	char *name = ArrayRef(setup->head_nid + FERA_N_MEM_NAME, i);
	if (name) {
	  return_on_error(DevCamChk(CamPiow(name, 1, 17, &one, 16, 0), &one, &one), status);
	  return_on_error(DevCamChk(CamPiow(name, 0, 24, 0, 16, 0), &one, &one), status);
	  return_on_error(DevCamChk(CamPiow(name, 0, 17, &zero, 16, 0), &one, &one), status);
	  return_on_error(DevCamChk(CamPiow(name, 1, 17, &three, 16, 0), &one, &one), status);
	} else {
	  status = FERA$_MEMNOTSTRARRAY;
	  break;
	}
      }
    } else
      status = FERA$_NOMEM;
  }

  /* Reset the controler */
  return_on_error(DevCamChk(CamPiow(setup->cntrl_name, 0, 9, 0, 16, 0), &one, &one), status);

  return status;
}

EXPORT int fera___store(struct descriptor *niddsc_ptr __attribute__ ((unused)), InStoreStruct * setup)
{

  int mem_status = 1;
  int put_status = 1;
  int status;
  int num_dig;
  int num_mem;
  int i;
  int num_chan;
  int total_data;
  int *num_pts;
  unsigned short *buffer;
  int ind;
#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;

  last_nid = 0;

  num_dig = NElements(setup->head_nid + FERA_N_DIG_NAME);
  if (num_dig == 0)
    return FERA$_NODIG;

  num_chan = num_dig * 16;
  total_data = 0;

  num_mem = NElements(setup->head_nid + FERA_N_MEM_NAME);
  if (num_mem == 0)
    return FERA$_NOMEM;

  num_pts = malloc(sizeof(int) * num_mem);
  for (i = 0; i < num_mem; i++) {
    char *name = ArrayRef(setup->head_nid + FERA_N_MEM_NAME, i);
    num_pts[i] = 0;
    if (name) {
      return_on_error(DevCamChk(CamPiow(name, 1, 17, &one, 16, 0), &one, &one), status);
      return_on_error(DevCamChk(CamPiow(name, 0, 2, &num_pts[i], 16, 0), &one, &one), status);
      if (num_pts[i] >= 16 * 1024) {
	num_pts[i] = 16 * 1024;
	if (i == num_mem - 1)
	  mem_status = FERA$_OVER_RUN;
      }
      total_data += num_pts[i];
    }
  }
  if (total_data % num_chan != 0) {
    status = FERA$_PHASE_LOST;
    return status;
  }
  buffer = malloc(total_data * sizeof(short));
  for (i = 0, ind = 0; i < num_mem; ind += num_pts[i], i++) {
    if (num_pts[i]) {
      char *name = ArrayRef(setup->head_nid + FERA_N_MEM_NAME, i);
      if (name) {
	return_on_error(DevCamChk
			(CamQstopw(name, 0, 2, num_pts[i], &buffer[ind], 16, 0), &one, &one),
			status)
      }
    }
  }

  status = Unpack(&buffer, num_pts, num_mem, num_chan, total_data / num_chan);
  if ((status & 1) || (status == FERA$_CONFUSED) || (status == FERA$_OVERFLOW))
    put_status =
	Put(buffer, total_data / num_chan, num_chan, setup->head_nid + FERA_N_EXT_CLOCK,
	    setup->head_nid + FERA_N_OUTPUT);
  free(buffer);
  return (put_status & 1) ? ((status & 1) ? mem_status : status) : put_status;
}

static int Unpack(unsigned short **buffer, int *num_pts, int num_mems, int chans, int pts_per_chan)
{
  int status = 1;
  unsigned short *output = malloc(sizeof(short) * chans * pts_per_chan);
  int mem;
  int pt = 0;
  int chan = 0;

  for (mem = 0; mem < num_mems; mem++) {
    int i;
    for (i = 1; i <= num_pts[mem]; i++) {
      short data = (*buffer)[mem * 16384 + num_pts[mem] - i];
      if (data == 2047) {
	status = FERA$_OVERFLOW;
      } else if (data > 1919) {
	status = FERA$_CONFUSED;
      }
      output[chan * pts_per_chan + pt] = data;
      chan++;
      if (chan == chans) {
	chan = 0;
	pt++;
      }
    }
  }
  free(*buffer);
  *buffer = output;
  return status;
}

static int NElements(int nid)
{
  static int num;
  static struct descriptor_s num_dsc = { sizeof(int), DTYPE_L, CLASS_S, (char *)&num };
  static DESCRIPTOR(set_var, "_TextArray = $1");
  static struct descriptor_xd dummy_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
  struct descriptor_s nid_dsc = { sizeof(int), DTYPE_NID, CLASS_S, (char *)0 };
  int status = 1;
  nid_dsc.pointer = (char *)&nid;
  if (last_nid != nid) {
    last_nid = 0;
    status = TdiExecute((struct descriptor *)&set_var, &nid_dsc, &dummy_xd MDS_END_ARG);
  }
  if (status & 1) {
    static DESCRIPTOR(size_expr, "SIZE(_TextArray)");
    last_nid = nid;
    status = TdiExecute((struct descriptor *)&size_expr, &num_dsc MDS_END_ARG);
  }
  return (status & 1) ? num : 0;
}

static char *ArrayRef(int nid, int num)
{
  //  struct descriptor_s num_dsc = { sizeof(int), DTYPE_L, CLASS_S, (char *)0 };
  static DESCRIPTOR(set_var, "_TextArray = $1");
  static struct descriptor_xd dummy_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
  struct descriptor_s nid_dsc = { sizeof(int), DTYPE_NID, CLASS_S, (char *)0 };
  static struct descriptor_d empty_string = { 0, DTYPE_T, CLASS_D, 0 };
  int status = 1;
  //num_dsc.pointer = (char *)&num;
  //nid_dsc.pointer = (char *)&nid;
  if (last_nid != nid) {
    last_nid = 0;
    status = TdiExecute((struct descriptor *)&set_var, &nid_dsc, &dummy_xd MDS_END_ARG);
  }
  if (status & 1) {
    static DESCRIPTOR(subscript_expr, "_TextArray[$]//\"\\0\"");
    struct descriptor_s num_dsc = { sizeof(int), DTYPE_L, CLASS_S, (char *)0 };
    num_dsc.pointer = (char *)&num;
    status = TdiExecute((struct descriptor *)&subscript_expr, &num_dsc, &empty_string MDS_END_ARG);
  }
  return ((status & 1) == 0) ? 0 : empty_string.pointer;
}

static int Put(unsigned short int *buffer, int pts_per_chan, int chans, int clock_nid, int nid)
{
  DESCRIPTOR_A_COEFF_2(a_dsc, sizeof(short), DTYPE_W, (char *)0, 0, 0, 0);
  DESCRIPTOR(expr,
	     "BUILD_SIGNAL(BUILD_WITH_UNITS($VALUE*.25E-12,'Coulombs'), build_with_units($,'counts'), $, \
                           build_with_units(0 : $ : 1, 'Channel'))");
  struct descriptor_s clock_nid_dsc = { sizeof(int), DTYPE_NID, CLASS_S, (char *)0 };
  int max_chan_num = chans - 1;
  struct descriptor_s num_dsc = { sizeof(int), DTYPE_L, CLASS_S, (char *)0 };
  static struct descriptor_xd output_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
  int status;
  a_dsc.pointer = (char *)buffer;
  a_dsc.arsize = pts_per_chan * chans * sizeof(short);
  a_dsc.m[0] = pts_per_chan;
  a_dsc.m[1] = chans;
  clock_nid_dsc.pointer = (char *)&clock_nid;
  num_dsc.pointer = (char *)&max_chan_num;
  status = TdiCompile((struct descriptor *)&expr, &a_dsc, &clock_nid_dsc, &num_dsc, &output_xd MDS_END_ARG);
  if (status & 1) {
    status = TreePutRecord(nid, (struct descriptor *)&output_xd, 0);
    MdsFree1Dx(&output_xd, 0);
  }
  return status;
}
