/*  VAX/DEC CMS REPLACEMENT HISTORY, Element GEN_DEVICE$HEAD_NID.C */
/*  *1     7-OCT-1993 10:37:18 TWF "Return head nid of conglomerate" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element GEN_DEVICE$HEAD_NID.C */
#include <mdsdescrip.h>
#include <ncidef.h>
#include <treeshr.h>

int GenDeviceHeadNid(struct descriptor *niddsc)
{
  int nid = *(int *)niddsc->pointer;
  static int idx;
  static NCI_ITM itmlst[] = {{sizeof(int),NciCONGLOMERATE_ELT,(unsigned char *)&idx,0},
                             {0,NciEND_OF_LIST,0,0}};
  TreeGetNci(nid, itmlst);
  return nid - idx + 1;
}
