/*------------------------------------------------------------------------------

		Name:   H911$METHODS   

		Type:   C function

     		Author:	JOSH STILLERMAN

		Date:    1-OCT-1993

    		Purpose: Routines to Arm and store H911 32 channel latching scaler 

------------------------------------------------------------------------------

	Call sequence: 

------------------------------------------------------------------------------
   Copyright (c) 1993
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

 	Description:

------------------------------------------------------------------------------*/
#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <treeshr.h>
#include <stdlib.h>
#include "h911_gen.h"
#include "devroutines.h"
static int one = 1;
#define pio(f,a) {int status; \
                  if (!((status = DevCamChk(CamPiow(in->name, a, f, 0, 16, 0), &one, &one)) &1)) return status; }
EXPORT int h911___init(struct descriptor *nid_d_ptr __attribute__ ((unused)), InInitStruct * in)
{
  pio(24, 0);			/* standby mode */
  pio(26, 0);			/* armed mode */
  return (1);
}

#undef pio

#define pio(f,a, d) {int status; \
                  if (!((status = DevCamChk(CamPiow(in->name, a, f, d, 16, 0), &one, &one)) &1)) return status; }
#define stop(f, a, c, d) { int status; \
                              if (!((status = DevCamChk(CamStopw(in->name, a, f, c, d, 16, 0), &one, &one)) &1))  \
                                return status; }

EXPORT int h911___store(struct descriptor *nid_d_ptr __attribute__ ((unused)), InStoreStruct * in)
{

  struct _status_reg {
    unsigned mode:2;
    unsigned over:1;
    unsigned full:1;
    unsigned:20;
  } status_reg;
  short mems;
  short chans;
  int samples;
  int status;
  int i;
  short *buffer;
  pio(0, 2, (short *)&status_reg)	/* read back the status register */
      if (!status_reg.full)
    return DEV$_NOT_TRIGGERED;
  pio(0, 3, &mems);
  mems++;
  pio(0, 4, &chans);
  chans++;
  samples = mems * 32768 / chans - 1;
  buffer = malloc(samples * sizeof(short));
  {

    static DESCRIPTOR_A(data_dsc, sizeof(short), DTYPE_W, 0, 0);
    static DESCRIPTOR(counts_str, "counts");
    static DESCRIPTOR_WITH_UNITS(counts, &data_dsc, &counts_str);
    static int clk_nid;
    static DESCRIPTOR_NID(clock_dsc, &clk_nid);
    static DESCRIPTOR_SIGNAL_1(sig, &counts, 0, &clock_dsc);
    data_dsc.pointer = (char *)buffer;
    data_dsc.arsize = samples * sizeof(short);
    clk_nid = in->head_nid + H911_N_EXT_CLOCK;
    for (i = 0, status = 1; ((status & 1) && (i < chans)); i++) {
      int nid = in->head_nid + H911_N_CHANNEL_01 + i;
      if (TreeIsOn(nid)) {
	short addr = i;
	pio(17, 1, &addr);	/* readback mode 1 chan at a time starting with  sample 1 of this channel */
	stop(0, 0, samples, buffer);
	status = TreePutRecord(nid, (struct descriptor *)&sig, 0);
      }
    }
  }
  free(buffer);
  return (status);
}

#undef pio
#undef stop
