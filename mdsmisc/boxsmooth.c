/*  CMS REPLACEMENT HISTORY, Element BOXSMOOTH.C */
/*  *7    11-DEC-1997 11:33:33 TWF "Use double" */
/*  *6    11-DEC-1997 11:27:28 TWF "change name of smooth" */
/*  *5    11-DEC-1997 11:26:35 TWF "change name of smooth" */
/*  *4    11-DEC-1997 11:14:17 TWF "Different values than idl" */
/*  *3    11-DEC-1997 11:09:37 TWF "Different values than idl" */
/*  *2    11-DEC-1997 11:04:10 TWF "Different values than idl" */
/*  *1    11-DEC-1997 10:47:08 TWF "Smoothing function ala idl" */
/*  CMS REPLACEMENT HISTORY, Element BOXSMOOTH.C */
/*------------------------------------------------------------------------------

		Name:   BOXSMOOTH   

		Type:   C function

     		Author:	TOM FREDIAN

		Date:   15-AUG-1990

    		Purpose: Smooth a signal using boxcar smoothing

------------------------------------------------------------------------------

	Call sequence: 

int boxsmooth(int *num, float *in, int *width, float *out)

------------------------------------------------------------------------------
   Copyright (c) 1990
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

 	Description:


------------------------------------------------------------------------------*/

#include <string.h>

int boxsmooth(int *num, float *in, int *width, float *out)
{
  int i;
  int j;
  int w = *width;
  int n = *num;
  int *in_int = (int *)in;
  int *out_int = (int *)out;
  w += (w % 2) ? 0 : 1;
  memcpy(out,in,w/2*sizeof(float));
  for (i=w/2;i<n-w/2;i++)
  {
    int div;
    double total = 0.;
    for (div=0,j=0;j<w;j++)
    {
      int idx = i+j-w/2;
      if (in_int[idx] != 32768)
      {
        total += in[i+j-w/2];
        div++;
      }
    }
    if (div > 0)
      out[i] = total/div;
    else
      out_int[i] = 32768;
  }
  memcpy(&out[n-w/2],&in[n-w/2],w/2*sizeof(float));
  return 1;
}
    
	
