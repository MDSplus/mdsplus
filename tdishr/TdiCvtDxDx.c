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
/*      TdiCvtDxDx
        Convert to desired data type.

        status = TdiCvtDxDx(&in_dsc,&dtype,&out_dsc)

        Thomas W. Fredian, MIT-PFC      07-MAR-1989     Copyrighted
        Ken Klare, LANL CTR-7   (c)1989,1990
        NEED to handle array of descriptors.
*/
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <status.h>
#include <STATICdef.h>



extern int TdiConvert();
extern int TdiGetShape();

int TdiCvtDxDx(struct descriptor *in_ptr, unsigned char *dtype_ptr, struct descriptor_xd *out_ptr)
{
  INIT_STATUS;

	/*********************************
        No conversion needed, just memory.
        *********************************/
  if (in_ptr->dtype == *dtype_ptr)
    status = MdsCopyDxXd(in_ptr, out_ptr);

	/*******************
        Remove a descriptor.
        *******************/
  else if (in_ptr->dtype == DTYPE_DSC)
    status = TdiCvtDxDx((struct descriptor *)in_ptr->pointer, dtype_ptr, out_ptr);

	/******************************
        Make space and convert into it.
        ******************************/
  else {
    int mode = -1;
    struct descriptor_xd xs = { 0, DTYPE_DSC, CLASS_XS, 0, 0 };
    xs.pointer = in_ptr;
    status = TdiGetShape(1, &xs, 0, *dtype_ptr, &mode, out_ptr);
    if STATUS_OK
      status = TdiConvert(in_ptr, out_ptr->pointer);
  }
  return status;
}
