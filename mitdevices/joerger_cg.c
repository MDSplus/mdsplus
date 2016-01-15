#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <strroutines.h>
#include <treeshr.h>
#include <mdsshr.h>
#include "joerger_cg_gen.h"
#include "devroutines.h"


static int one = 1;
#define pio(f,a,d,q) {unsigned short dv=d;\
 if (!((status = DevCamChk(CamPiow(setup->name,a,f,&dv,16,0),&one,q)) & 1)) return status;}

static float slopes[] =
    { 1E9, 1, 1E-1, 1E-2, 1E-3, 1E-4, 2E-5, 1E-5, 4E-6, 2E-6, 1E-6, 4E-7, 2E-7, 1E-7, 5E-8,
2.5E-8 };

static int SlopeToFreq(float slope, float *actslope)
{
  int i;
  for (i = 0; i < 16 && slope < (.95 * slopes[i]); i++) ;
  i = i < 16 ? i : 15;
  *actslope = slopes[i];
  return i;
}

static int SetChannel(InInitStruct * setup, struct descriptor *channel_value, int channel,
		      int *polarities)
{
  static EMPTYXD(xd);
  int status;
  int p = *polarities;
  int nid = setup->head_nid + JOERGER_CG_N_CHANNEL_1 + channel * 2 - 2;
  float actslope;
  static float slope;
  static DESCRIPTOR_FLOAT(slope_d, &slope);
  status = TdiSlopeOf(channel_value, &slope_d MDS_END_ARG);
  pio(17, channel - 1, SlopeToFreq(slope, &actslope), &one);
  if (slope != actslope) {
    static DESCRIPTOR(range, "*:*:$");
    slope = actslope;
    TdiCompile((struct descriptor *)&range, &slope_d, &xd MDS_END_ARG);
    TreePutRecord(nid, (struct descriptor *)&xd, 0);
  }
  nid++;
  p |= (TreeIsOn(nid) & 1) << (channel - 1);
  *polarities = p;
  return status;
}

int joerger_cg___init(struct descriptor *niddsc, InInitStruct * setup)
{
  int status;
  int polarities = 0;
  int clock_mode = 0;
  pio(9, 1, 0, 0);
  SetChannel(setup, setup->channel_1, 1, &polarities);
  SetChannel(setup, setup->channel_2, 2, &polarities);
  SetChannel(setup, setup->channel_3, 3, &polarities);
  SetChannel(setup, setup->channel_4, 4, &polarities);
  pio(17, 4, clock_mode, &one);
  pio(17, 5, polarities, &one);
  pio(26, 1, 0, 0)
      return status;
}

int joerger_cg___stop(struct descriptor *niddsc, InStopStruct * setup)
{
  int status;
  pio(9, 1, 0, 0)
      return status;
}
