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
#include <mdsplus/mdsconfig.h>

EXPORT int boxsmooth(int *num, float *in, int *width, float *out)
{
  int i;
  int j;
  int w = *width;
  int n = *num;
  int *in_int = (int *)in;
  int *out_int = (int *)out;
  if (n <= 0)
    return 0;
  if (n < w) {
    memcpy(out, in, n * sizeof(float));
    return 1;
  }
  w += (w % 2) ? 0 : 1;
  memcpy(out, in, w / 2 * sizeof(float));
  for (i = w / 2; i < n - w / 2; i++) {
    int div;
    double total = 0.;
    for (div = 0, j = 0; j < w; j++) {
      int idx = i + j - w / 2;
      if (in_int[idx] != 32768) {
	total += in[i + j - w / 2];
	div++;
      }
    }
    if (div > 0)
      out[i] = total / div;
    else
      out_int[i] = 32768;
  }
  memcpy(&out[n - w / 2], &in[n - w / 2], w / 2 * sizeof(float));
  return 1;
}
