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

		Name:   RESAMPLE   

		Type:   C function

     		Author:	TOM FREDIAN

		Date:   15-AUG-1990

    		Purpose: Resample a signal using linear interpolation.
                         Extrapolated points are assumed to be the same
                         value as the extremities.

------------------------------------------------------------------------------

	Call sequence: 

struct dsc$descriptor_xd *RESAMPLE(struct dsc$descriptor *sig,struct dsc$descriptor *x);

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
#include <mdsshr.h>
#include <mds_stdarg.h>

extern int TdiData();
extern int TdiCvt();
extern int TdiDimOf();
struct descriptor *Resample(struct descriptor *in_sig, struct descriptor *in_x);

EXPORT struct descriptor *resample(struct descriptor *in_sig, struct descriptor *in_x)
{
  return Resample(in_sig, in_x);
}

EXPORT struct descriptor *Resample(struct descriptor *in_sig, struct descriptor *in_x)
{

#define return_on_error(func,error) if (!((status = func)&1)) return error
//#define interp(x1,y1,x2,y2,x) (((y2)-(y1))/((x2)-(x1)) * (x) + (((y1)*(x2))-((y2)*(x1)))/((x2)-(x1)))
#define interp(x1,y1,x2,y2,x) (y1)+((y2)-(y1))/((x2)-(x1))*((x)-(x1))

  int status;
  static EMPTYXD(sig_x_xd);
  static EMPTYXD(sig_y_xd);
  static EMPTYXD(new_x_xd);
  static EMPTYXD(new_y_xd);
  static EMPTYXD(answer);
  struct descriptor_a *sig_y;
  struct descriptor_a *sig_x;
  struct descriptor_a *new_x;
  struct descriptor_a *new_y;
  float *sig_y_f;
  float *sig_x_f;
  float *new_x_f;
  float *new_y_f;
  static EMPTYXD(xd1);
  static EMPTYXD(xd2);
  struct descriptor_signal *sig = (struct descriptor_signal *)in_sig;
  struct descriptor *x = in_x;
  static DESCRIPTOR(bad_sig_in, "bad resample signal in");
  static DESCRIPTOR(bad_time_in, "bad time vector in");
  static float zero = 0.0;
  static struct descriptor float_dsc = { sizeof(float), DTYPE_FLOAT, CLASS_S, (char *)&zero };
  int i;
  int j = 0;
  int new_elements;
  int sig_elements;

  MdsFree1Dx(&answer, 0);
  while (sig->dtype == DTYPE_DSC)
    sig = (struct descriptor_signal *)sig->pointer;
  while (x->dtype == DTYPE_DSC)
    x = (struct descriptor *)x->pointer;
  return_on_error(TdiData(sig, &xd1 MDS_END_ARG), ((struct descriptor *)&bad_sig_in));
  return_on_error(TdiCvt(&xd1, &float_dsc, &sig_y_xd MDS_END_ARG),
		  (struct descriptor *)&bad_sig_in);
  sig_y = (struct descriptor_a *)sig_y_xd.pointer;
  if (sig_y->class != CLASS_A)
    return (struct descriptor *)&bad_sig_in;
  sig_y_f = (float *)sig_y->pointer;
  return_on_error(TdiDimOf(sig, &xd1 MDS_END_ARG), (struct descriptor *)&bad_sig_in);
  return_on_error(TdiData(&xd1, &xd2 MDS_END_ARG), (struct descriptor *)&bad_sig_in);
  return_on_error(TdiCvt(&xd2, &float_dsc, &sig_x_xd MDS_END_ARG),
		  (struct descriptor *)&bad_sig_in);
  sig_x = (struct descriptor_a *)sig_x_xd.pointer;
  if (sig_x->class != CLASS_A)
    return (struct descriptor *)&bad_sig_in;
  sig_x_f = (float *)sig_x->pointer;
  return_on_error(TdiData(x, &xd1 MDS_END_ARG), (struct descriptor *)&bad_time_in);
  return_on_error(TdiCvt(&xd1, &float_dsc, &new_x_xd MDS_END_ARG),
		  (struct descriptor *)&bad_time_in);
  new_x = (struct descriptor_a *)new_x_xd.pointer;
  if (new_x->class != CLASS_A)
    return (struct descriptor *)&bad_time_in;
  new_x_f = (float *)new_x->pointer;
  MdsCopyDxXd((struct descriptor *)new_x, &new_y_xd);
  new_y = (struct descriptor_a *)new_y_xd.pointer;
  new_y_f = (float *)new_y->pointer;
  new_elements = new_y->arsize / new_y->length;
  sig_elements = sig_y->arsize / sig_y->length;
  if (!sig_elements || ((unsigned)sig_elements > (sig_x->arsize / sig_x->length)))
    return (struct descriptor *)&bad_sig_in;
  for (i = 0; i < new_elements; i++) {
    if (new_x_f[i] <= sig_x_f[0])
      new_y_f[i] = sig_y_f[0];
    else if (new_x_f[i] >= sig_x_f[sig_elements - 1])
      new_y_f[i] = sig_y_f[sig_elements - 1];
    else {
      for (; j < sig_elements; j++)
	if (new_x_f[i] <= sig_x_f[j])
	  break;
//      if (new_x_f[i]==sig_x_f[j])
//        new_y_f[i]=sig_y_f[j];
//      else
      new_y_f[i] = interp(sig_x_f[j - 1], sig_y_f[j - 1], sig_x_f[j], sig_y_f[j], new_x_f[i]);
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
