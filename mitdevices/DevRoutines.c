#include <mdsdescrip.h>
#include <mds_stdarg.h>
#include <mitdevices_msg.h>

extern int TdiData();
extern int CamX();
extern int CamQ();

int DevLong(int *nid, int *ans)
{
  DESCRIPTOR_NID(nid_d,0);
  DESCRIPTOR_LONG(ans_d,0);
  nid_d.pointer = (char *)nid;
  ans_d.pointer = (char *)ans;
  return TdiData(&nid_d,&ans_d MDS_END_ARG);
}

int DevFloat(int *nid, float *ans)
{
  DESCRIPTOR_NID(nid_d,0);
  DESCRIPTOR_FLOAT(ans_d,0);
  nid_d.pointer = (char *)nid;
  ans_d.pointer = (char *)ans;
  return TdiData(&nid_d,&ans_d MDS_END_ARG);
}

int DevCamChk(int status, int *expect_x, int *expect_q)
{
  if (!(status & 1)) 
    return status;

  if (expect_x)
  {
    if ((CamX(0) & 1) != (*expect_x & 1))
      return ((CamX(0) & 1) ? DEV$_CAM_SX : DEV$_CAM_NOSX) | 2;
  }

  if (expect_q)
  {
    if ((CamQ(0) & 1) != (*expect_q & 1))
      return ((CamQ(0) & 1) ? DEV$_CAM_SQ : DEV$_CAM_NOSQ) | 2;
  }

  return 1;
}

  
  
  
