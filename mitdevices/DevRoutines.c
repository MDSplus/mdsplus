#include <mdsdescrip.h>
#include <mds_stdarg.h>
#include <mitdevices_msg.h>
#include <treeshr.h>
#include <ncidef.h>
#include <time.h>
#include <string.h>
#include <tdishr.h>
#include <camshr.h>




EXPORT int DevLong(int *nid, int *ans)
{
  DESCRIPTOR_NID(nid_d, 0);
  DESCRIPTOR_LONG(ans_d, 0);
  nid_d.pointer = (char *)nid;
  ans_d.pointer = (char *)ans;
  return TdiData(&nid_d, &ans_d MDS_END_ARG);
}

EXPORT int DevFloat(int *nid, float *ans)
{
  DESCRIPTOR_NID(nid_d, 0);
  DESCRIPTOR_FLOAT(ans_d, 0);
  nid_d.pointer = (char *)nid;
  ans_d.pointer = (char *)ans;
  return TdiData(&nid_d, &ans_d MDS_END_ARG);
}

EXPORT int DevCamChk(int status, int *expect_x, int *expect_q)
{
  if (!(status & 1))
    return status;

  if (expect_x) {
    if ((CamX(0) & 1) != (*expect_x & 1))
      return ((CamX(0) & 1) ? DEV$_CAM_SX : DEV$_CAM_NOSX) | 2;
  }

  if (expect_q) {
    if ((CamQ(0) & 1) != (*expect_q & 1))
      return ((CamQ(0) & 1) ? DEV$_CAM_SQ : DEV$_CAM_NOSQ) | 2;
  }

  return 1;
}

EXPORT int DevNids(struct descriptor *niddsc __attribute__ ((unused)), int size, char *buffer)
{
  NCI_ITM nci_lst[] = { {0, NciCONGLOMERATE_NIDS, 0, 0}, {0, NciEND_OF_LIST, 0, 0} };
  nci_lst[0].buffer_length = size;
  nci_lst[0].pointer = (unsigned char *)buffer;
  return TreeGetNci(*(int *)niddsc->pointer, nci_lst);
}

EXPORT int DevText(int *nid, struct descriptor_d *out)
{
  DESCRIPTOR_NID(niddsc, 0);
  niddsc.pointer = (char *)nid;
  return TdiText(&niddsc, out MDS_END_ARG);
}

EXPORT int DevNid(int *nid_in, int *nid_out)
{
  EMPTYXD(xd);
  int status = TreeGetRecord(*nid_in, &xd);
  if (status & 1) {
    switch (xd.pointer->dtype) {
    case DTYPE_NID:
      *nid_out = *(int *)xd.pointer->pointer;
      break;
    case DTYPE_PATH:{
	char name[512];
	strncpy(name, xd.pointer->pointer, xd.pointer->length);
	name[xd.pointer->length] = 0;
	status = TreeFindNode(name, nid_out);
	break;
      }
    default:
      status = 0;
    }
  }
  return status;
}

EXPORT int DevWait(float time_in)
{
  struct timespec req = { (time_t) time_in, (long)((time_in - (long)time_in) * 1E9) };
  struct timespec rem = { 0, 0 };
  while (nanosleep(&req, &rem) == -1 && (rem.tv_sec != 0 || rem.tv_nsec != 0)) {
    req = rem;
  }
  return 1;
}
