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
/*------------------------------------------------------------------------------

		Name:   STEP_RESAMPLE   

		Type:   C function

     		Author:	TOM FREDIAN

		Date:   15-AUG-1990

    		Purpose: Resample a signal using stepped interpolation.
                         Extrapolated points are set to zero.

------------------------------------------------------------------------------

	Call sequence: 

struct descriptor_xd *STEP_RESAMPLE(struct descriptor *sig,struct descriptor *x);

------------------------------------------------------------------------------
   Copyright (c) 1990
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

 	Description:

------------------------------------------------------------------------------*/

#include <mdsdescrip.h>
#include <libroutines.h>
#include <mdsshr.h>
#include <mds_stdarg.h>

extern int TdiData();
extern int TdiByte();
extern int TdiDimOf();
extern int TdiCvt();

EXPORT struct descriptor *StepResample(struct descriptor *in_sig, struct descriptor *in_x)
{

#define return_on_error(func) if (!((status = func)&1)) return (struct descriptor *)&emptyxd;

  int status;
  static EMPTYXD(emptyxd);
  static EMPTYXD(sig_x_xd);
  static EMPTYXD(sig_y_xd);
  static EMPTYXD(new_x_xd);
  static EMPTYXD(new_y_xd);
  static EMPTYXD(answer);
  struct descriptor_a *sig_y;
  struct descriptor_a *sig_x;
  struct descriptor_a *new_x;
  struct descriptor_a *new_y;
  char *sig_y_b;
  float *sig_x_f;
  float *new_x_f;
  char *new_y_b;
  static EMPTYXD(xd1);
  static EMPTYXD(xd2);
  struct descriptor_signal *sig = (struct descriptor_signal *)in_sig;
  struct descriptor *x = in_x;
  static float zero = 0.0;
  static struct descriptor float_dsc = { sizeof(float), DTYPE_FLOAT, CLASS_S, (char *)&zero };

  int i;
  int j = 0;
  int new_elements;
  int sig_elements;

  while (sig->dtype == DTYPE_DSC)
    sig = (struct descriptor_signal *)sig->pointer;
  while (x->dtype == DTYPE_DSC)
    x = (struct descriptor *)x->pointer;
  return_on_error(TdiData(sig, &xd1 MDS_END_ARG));
  return_on_error(TdiByte(&xd1, &sig_y_xd MDS_END_ARG));
  sig_y = (struct descriptor_a *)sig_y_xd.pointer;
  if (sig_y->class != CLASS_A)
    return 0;
  sig_y_b = sig_y->pointer;
  return_on_error(TdiDimOf(sig, &xd1 MDS_END_ARG));
  return_on_error(TdiData(&xd1, &xd2 MDS_END_ARG));
  return_on_error(TdiCvt(&xd2, &float_dsc, &sig_x_xd MDS_END_ARG));
  sig_x = (struct descriptor_a *)sig_x_xd.pointer;
  if (sig_x->class != CLASS_A)
    return 0;
  sig_x_f = (float *)sig_x->pointer;
  return_on_error(TdiData(x, &xd1 MDS_END_ARG));
  return_on_error(TdiCvt(&xd1, &float_dsc, &new_x_xd MDS_END_ARG));
  new_x = (struct descriptor_a *)new_x_xd.pointer;
  if (new_x->class != CLASS_A)
    return 0;
  new_x_f = (float *)new_x->pointer;
  MdsCopyDxXd((struct descriptor *)new_x, &new_y_xd);
  new_elements = new_x->arsize / new_x->length;
  new_y = (struct descriptor_a *)new_y_xd.pointer;
  new_y->dtype = DTYPE_B;
  new_y->length = 1;
  new_y->arsize = new_elements;
  new_y_b = new_y->pointer;
  sig_elements = sig_y->arsize / sig_y->length;
  if (!sig_elements || (sig_elements != (int)(sig_x->arsize / sig_x->length)))
    return 0;
  sig_y_b[sig_elements - 1] = sig_y_b[sig_elements - 2];
  for (i = 0; i < new_elements; i++) {
    if (new_x_f[i] <= sig_x_f[0])
      new_y_b[i] = 0;
    else if (new_x_f[i] >= sig_x_f[sig_elements - 1])
      new_y_b[i] = 0;
    else {
      for (; j < sig_elements; j++)
	if (new_x_f[i] < sig_x_f[j])
	  break;
      new_y_b[i] = sig_y_b[j - 1];
    }
  }
  {
    static DESCRIPTOR_SIGNAL(sig_out, 1, 0, 0);
    sig_out.data = (struct descriptor *)new_y;
    sig_out.dimensions[0] = (struct descriptor *)new_x;
    MdsCopyDxXd((struct descriptor *)&sig_out, &answer);
  }
  MdsFree1Dx(&sig_x_xd, 0);
  MdsFree1Dx(&sig_y_xd, 0);
  MdsFree1Dx(&new_x_xd, 0);
  MdsFree1Dx(&new_y_xd, 0);
  MdsFree1Dx(&xd1, 0);
  MdsFree1Dx(&xd2, 0);
  return (struct descriptor *)&answer;
}
