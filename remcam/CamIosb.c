#include <stdlib.h>
#include <string.h>

extern unsigned short RemCamLastIosb[4];

int CamBytcnt(unsigned short *iosb)
{
  return (int)(iosb ? iosb[1] : RemCamLastIosb[1]);
}

int CamError(int *xexp, int *qexp, unsigned short *iosb_in)
{
  unsigned short *iosb = iosb_in ? iosb_in : RemCamLastIosb;
  return ((!(iosb[0] & 1)) ||
     (xexp && ((*xexp & 1) != (iosb[2] & 1))) ||
     (qexp && ((*qexp & 1) != ((iosb[2] >> 1) & 1))));
}    

int CamX(unsigned short *iosb_in)
{
  unsigned short *iosb = iosb_in ? iosb_in : RemCamLastIosb;
  return ((iosb[0] & 1) && (iosb[2] & 1));
}

int CamQ(unsigned short *iosb_in)
{
  unsigned short *iosb = iosb_in ? iosb_in : RemCamLastIosb;
  return ((iosb[0] & 1) && (iosb[2] & 2));
}

int CamGetStat(unsigned short *iosb_in)
{
  memcpy(iosb_in,RemCamLastIosb,sizeof(RemCamLastIosb));
}

int CamXandQ(unsigned short *iosb_in)
{  unsigned short *iosb = iosb_in ? iosb_in : RemCamLastIosb;
  return ((iosb[0] & 1) && ((iosb[2] & 3) == 3));
}
