#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <treeshr.h>
#include "joerger_adc_gen.h"
#include "devroutines.h"



#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;
static int one = 1;
EXPORT int joerger_adc___store(int *niddsc __attribute__ ((unused)), InStoreStruct * setup)
{
  int status = 1;
  int data_nid = setup->head_nid + JOERGER_ADC_N_DATA;
  if (TreeIsOn(data_nid) & 1) {
    static short raw[64];
    static ARRAY_BOUNDS(short, 1) raw_d = {
      4, DTYPE_W, CLASS_A, raw, 0, 0, {
	0, 0, 1, 1, 1}, 1, sizeof(raw), raw - 1, {32}, {{1, 32}}};
    static int vstrap_nid;
    static DESCRIPTOR_NID(vstrap_d, &vstrap_nid);
    static EMPTYXD(xd);
    static DESCRIPTOR(expr, "BUILD_WITH_UNITS($ * $/4095,'VOLTS'");
    int bytcnt;
    vstrap_nid = setup->head_nid + JOERGER_ADC_N_VSTRAP;
    return_on_error(DevCamChk(CamQstopw(setup->name, 0, 2, 64, &raw, 16, 0), &one, 0), status);
    bytcnt = CamBytcnt(0);
    raw_d.arsize = bytcnt;
    raw_d.m[0] = raw_d.bounds[0].u = bytcnt / 2;
    TdiCompile((struct descriptor *)&expr, &raw_d, &vstrap_d, &xd MDS_END_ARG);
    status = TreePutRecord(data_nid, (struct descriptor *)&xd, 0);
  }
  return status;
}
