#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include "joerger_dac16.h"
#include <libroutines.h>
#include <strroutines.h>
#include <treeshr.h>
#include <mdsshr.h>
static int one=1;
#define return_on_error(f) if (!((status = f) & 1)) return status;
#define pio(f,a,d)  return_on_error(DevCamChk(CamPiow(setup->name, a, f, d, 16, 0), &one, 0))
#define pio24(f,a,d)  return_on_error(DevCamChk(CamPiow(setup->name, a, f, d, 24, 0), &one, 0))

extern int DevCamChk();
extern int CamPiow();
int joerger_dac16___init(struct descriptor *nid_dsc, InInitStruct *setup)
{
  int i;
  static int c_nids[JOERGER_DAC16_K_CONG_NODES];
  int status;
  unsigned int module_id;
  int range;
  short int bad_chans = 0;
  float *outputs;
  outputs = &setup->output_01;
  return_on_error(DevNids(nid_dsc,sizeof(c_nids),c_nids));
  pio24(1, 15, &module_id);
  range = module_id & 3;      
  for (i=0; i < 16; i++) {
    if (TreeIsOn(c_nids[JOERGER_DAC16_N_OUTPUT_01+i])&1) {
      switch (range) {
      case 0: if ((outputs[i] >= 0) && (outputs[i] <= 10)) {
                short int data = outputs[i]/10.0*4096;
                pio(16, i, &data);
              }
              else bad_chans |= 1<<i;
              break;
      case 1: if ((outputs[i] >= 0) && (outputs[i] <= 5)) {
                short int data = outputs[i]/5.0*4096;
                pio(16, i, &data);
              }
              else bad_chans |= 1<<i;
              break;
      case 2: if ((outputs[i] >= -10) && (outputs[i] <= 10)) {
                short int data = (outputs[i]+10)/20.0*4096;
                pio(16, i, &data);
              }
              else bad_chans |= 1<<i;
              break;
      case 3: if ((outputs[i] >= -5) && (outputs[i] <= 5)) {
                short int data = (outputs[i]+5)/10.0*4096;
                pio(16, i, &data);
              }
              else bad_chans |= 1<<i;
              break;
      }
    }
  }
  return (bad_chans != 0) ? J_DAC$_OUTRNG : 1;
}
