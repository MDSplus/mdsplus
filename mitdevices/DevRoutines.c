#include <mdsdescrip.h>
#include <mds_stdarg.h>
#include <mitdevices_msg.h>
#include <treeshr.h>
#include <ncidef.h>

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

int DevNids(struct descriptor *niddsc,int size, char *buffer)
{
  NCI_ITM nci_lst[] = {{0,NciCONGLOMERATE_NIDS,0,0},{0,NciEND_OF_LIST,0,0}};
  nci_lst[0].buffer_length = size;
  nci_lst[0].pointer = (unsigned char *)buffer;
  return TreeGetNci(*(int *)niddsc->pointer, nci_lst);
}

int DevText(int *nid,struct descriptor *out)
{
  DESCRIPTOR_NID(niddsc,0);
  niddsc.pointer=(char *)nid;
  return TdiText(&niddsc,out MDS_END_ARG);
}

int DevNid(int *nid_in, int *nid_out)
{
  EMPTYXD(xd);
  int status = TreeGetRecord(*nid_in,&xd);
  if (status & 1)
  {
    switch (xd.pointer->dtype)
    {
    case DTYPE_NID: *nid_out = *(int *)xd.pointer->pointer; break; 
    case DTYPE_PATH: {
      char name[512];
      strncpy(name,xd.pointer->pointer,xd.pointer->length);
      name[xd.pointer->length]=0;
      status = TreeFindNode(name,nid_out); break;
    }
    default:
      status = 0;
    }
  }
  return status;
}
  
  
  
