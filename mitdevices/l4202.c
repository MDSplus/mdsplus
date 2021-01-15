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
#include <strroutines.h>
#include <libroutines.h>
#include "l4202_gen.h"
#include "devroutines.h"

//extern unsigned short OpcValue;


static int one = 1;
static int zero = 0;
#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;
#define pio(name,f,a,d)  return_on_error(DevCamChk(CamPiow(name, a, f, d, 24, 0), &one, 0),status)
#define max_tries 30
#define stop(name,f,a,c,d)  return_on_error(DevCamChk(CamStopw(name, a, f, c, d, 24, 0), &one, 0),status)
#define min(a,b) (((a) < (b)) ? (a) : (b))

EXPORT int l4202___init(struct descriptor *niddsc __attribute__ ((unused)), InInitStruct * setup)
{
  int status;
  int tries;
  struct {
    unsigned char bursts;
    unsigned increment:15;
    unsigned loop:1;
  } setup1;
  struct {
    unsigned range:4;
    unsigned overflow:20;
  } setup2;

  setup1.bursts = setup->num_spectra - 1;
  setup1.increment = setup->router_inc / 256;
  setup1.loop = 1;
  pio(setup->name, 17, 0, (int *)&setup1);	/* Write router command */
  pio(setup->name, 16, 0, &setup->offset);	/* Write offset register */
  for (setup2.range = 1;
       setup2.range < 15 && ((1 << (setup2.range - 1)) * .15625) < setup->resolution;
       setup2.range++) ;
  setup2.overflow = setup->router_inc * setup->resolution / 160 + .499999;
  pio(setup->name, 18, 0, (int *)&setup2);	/* Write overflow register */
  pio(setup->name, 9, 0, 0);	/* reset memory */
  pio(setup->name, 10, 0, 0);	/* reset LAM    */
  pio(setup->name, 10, 0, 0);	/* reset LAM    */
  pio(setup->name, 26, 0, 0);	/* enable LAM   */
  pio(setup->memory_name, 10, 0, 0);	/* reset LAM    */
  pio(setup->memory_name, 9, 0, 0);	/* reset memory */
  pio(setup->memory_name, 26, 0, 0);	/* enable LAM   */
  pio(setup->memory_name, 8, 0, 0);
  for (tries = 0; (!(CamXandQ(0) & 1)) && tries < max_tries; tries++) {
    float wait = .5;
    pio(setup->memory_name, 8, 0, 0);
    LibWait(&wait);
  }				/* wait for LAM */
  pio(setup->memory_name, 10, 0, 0);	/* reset LAM    */
  pio(setup->memory_name, 11, 0, 0);	/* enable front panel histogram */
  return status;
}

EXPORT int l4202___store(struct descriptor *niddsc __attribute__ ((unused)), InStoreStruct * setup)
{
  struct {
    unsigned first:12;
    unsigned second:12;
    unsigned fill:8;
  } in_data[16384];
  unsigned short out_data[32768];
  int status;
  int i;
  pio(setup->memory_name, 19, 0, &zero);	/* Disable port for external histogram */
  pio(setup->memory_name, 18, 1, &zero);	/* Set memory to location 0 for reads */
  stop(setup->memory_name, 2, 0, 8192, &in_data[0]);
  stop(setup->memory_name, 2, 0, 8192, &in_data[8192]);
  {
    static DESCRIPTOR_A_COEFF_2(raw, 2, DTYPE_W, 0, 0, 0, 0);
    static DESCRIPTOR_FUNCTION_1(value, (unsigned char *)&OpcValue, 0);
    static int zero_val = 0;
    static DESCRIPTOR_LONG(zero, &zero_val);
    static int endrange_val;
    static DESCRIPTOR_LONG(endrange, &endrange_val);
    static int delta_val = 1;
    static DESCRIPTOR_FLOAT(delta, &delta_val);
    static DESCRIPTOR_RANGE(dim1, &zero, &endrange, &delta);
    static int ext_clock_nid;
    static DESCRIPTOR_NID(dim2, &ext_clock_nid);
    static DESCRIPTOR_SIGNAL_2(signal, &value, &raw, &dim1, &dim2);
    int spectra_nid = setup->head_nid + L4202_N_SPECTRA;
    value.ndesc = 0;
    ext_clock_nid = setup->head_nid + L4202_N_EXT_CLOCK;
    raw.pointer = raw.a0 = (char *)out_data;
    raw.arsize = min(65536, setup->router_inc * setup->num_spectra * sizeof(short));
    raw.m[0] = setup->router_inc;
    raw.m[1] = setup->num_spectra;
    endrange_val = setup->router_inc - 1;
    for (i = 0; i < 16384; i++) {
      out_data[i] = in_data[i].first;
      out_data[i + 16384] = in_data[i].second;
    }
    status = TreePutRecord(spectra_nid, (struct descriptor *)&signal, 0);
  }
  return status;
}
