#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <treeshr.h>
#include "l8201_gen.h"
#include "devroutines.h"

static int one = 1;
static int zero = 0;

#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;
#define pio(f,d) return_on_error(DevCamChk(CamPiow(setup->name,0,f,d, 16, 0),&one,0),status)
#define stop(f)  return_on_error(DevCamChk(CamStopw(setup->name,0,f,16384,buffer,16,0),&one,0),status)



static short buffer[16384];
static DESCRIPTOR_A(data, sizeof(short), DTYPE_W, buffer, sizeof(buffer));

EXPORT int l8201___init(struct descriptor *niddsc_ptr __attribute__ ((unused)), InInitStruct * setup)
{
  int status;
  return_on_error(TdiData(setup->download, &data MDS_END_ARG), status);
  pio(9, 0);
  stop(16);
  pio(11, &zero);
  pio(26, 0);
  return status;
}

EXPORT int l8201___store(struct descriptor *niddsc_ptr __attribute__ ((unused)), InStoreStruct * setup)
{
  int status;
  int upload_nid = setup->head_nid + L8201_N_UPLOAD;
  pio(9, 0);
  pio(2, &buffer[0]);
  stop(2);
  pio(11, &zero);
  pio(26, 0);
  if (TreeIsOn(upload_nid) & 1)
    status = TreePutRecord(upload_nid, (struct descriptor *)&data, 0);
  return status;
}
