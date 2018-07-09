/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdlib.h>
#include <string.h>
#include <mdsplus/mdsconfig.h>

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
