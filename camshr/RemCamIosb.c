#include <stdlib.h>
#include <string.h>
#include <config.h>

extern unsigned short RemCamLastIosb[4];

int RemCamVerbose(int flag __attribute__ ((unused)))
{
  return 1;
}

int RemCamBytcnt(unsigned short *iosb)
{
  return (int)(iosb ? iosb[1] : RemCamLastIosb[1]);
}

int RemCamError(int *xexp, int *qexp, unsigned short *iosb_in)
{
  unsigned short *iosb = iosb_in ? iosb_in : RemCamLastIosb;
  return ((!(iosb[0] & 1)) ||
	  (xexp && ((*xexp & 1) != (iosb[2] & 1))) ||
	  (qexp && ((*qexp & 1) != ((iosb[2] >> 1) & 1))));
}

int RemCamX(unsigned short *iosb_in)
{
  unsigned short *iosb = iosb_in ? iosb_in : RemCamLastIosb;
  return ((iosb[0] & 1) && (iosb[2] & 1));
}

int RemCamQ(unsigned short *iosb_in)
{
  unsigned short *iosb = iosb_in ? iosb_in : RemCamLastIosb;
  return ((iosb[0] & 1) && (iosb[2] & 2));
}

int RemCamStatus(unsigned short *iosb_in)
{
  unsigned short *iosb = iosb_in ? iosb_in : RemCamLastIosb;
  return (int)iosb[7];
}

int RemCamGetStat(unsigned short *iosb_in)
{
  memcpy(iosb_in, RemCamLastIosb, sizeof(RemCamLastIosb));
  return 1;
}

int RemCamXandQ(unsigned short *iosb_in)
{
  unsigned short *iosb = iosb_in ? iosb_in : RemCamLastIosb;
  return ((iosb[0] & 1) && ((iosb[2] & 3) == 3));
}
