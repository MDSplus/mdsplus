#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <treeshr.h>
#include "l8206_gen.h"
#include "devroutines.h"

static int one = 1;
static int zero = 0;


#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;
#define pio(f,d) return_on_error(DevCamChk(CamPiow(setup->name,0,f,d,16,0),&one,0),status)
#define stop(f,count,buffer)  return_on_error(DevCamChk(CamStopw(setup->name,0,f,count,buffer,16,0),&one,0),status)



static short buffer[65536];
static DESCRIPTOR_A(data, sizeof(short), DTYPE_W, buffer, sizeof(buffer));

EXPORT int l8206___init(struct descriptor *niddsc_ptr __attribute__ ((unused)), InInitStruct * setup)
{
  int status;
  int download_nid = setup->head_nid + L8206_N_DOWNLOAD;
  pio(9, 0);
  if (TreeIsOn(download_nid) & 1) {
    data.arsize = sizeof(buffer);
    return_on_error(TdiData(setup->download, &data MDS_END_ARG), status);
    stop(16, 16384, buffer);
    stop(16, 16384, &buffer[16384]);
    stop(16, 16384, &buffer[32768]);
    stop(16, 16384, &buffer[49152]);
  }
  pio(18, &zero);
  pio(11, &zero);
  pio(26, &zero);
  return status;
}

EXPORT int l8206___store(struct descriptor *niddsc_ptr __attribute__ ((unused)), InStoreStruct * setup)
{
  int status;
  int savstatus;
  int upload_nid = setup->head_nid + L8206_N_UPLOAD;
  pio(19, &zero);
  if (TreeIsOn(upload_nid) & 1) {
    int numpoints = 0;
    short *bptr;
    int pread;
    pio(8, 0);
    if (CamQ(0) & 1) {
      numpoints = 65536;
    } else {
      pio(0, &numpoints);
    }
    pio(25, 0);
    pio(18, &zero);
    pio(2, &buffer[0]);		/* junk word */
    for (pread = 0, bptr = buffer; pread < numpoints;) {
      int num = numpoints - pread;
      if (num > 32767)
	num = 32767;
      stop(2, num, bptr);
      pread += num;
      bptr += num;
    }
    if (numpoints > 0) {
      data.arsize = numpoints * sizeof(short);
      status = TreePutRecord(upload_nid, (struct descriptor *)&data, 0);
    } else
      status = L8206$_NODATA;
  }
  savstatus = status;
  pio(18, &zero);
  pio(11, &zero);
  pio(26, &zero);
  if (status & 1)
    status = savstatus;
  return status;
}
