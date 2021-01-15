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
#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>

#include <mdsshr.h>
#include "j412_gen.h"
#include "devroutines.h"

static int one = 1;
#define min(a,b) ((a) < (b)) ? (a) : (b)
#define max(a,b) ((a) < (b)) ? (b) : (a)
#define return_on_error(f) if (!((status = f) & 1)) goto on_error;
#define pio(f,a,d)  return_on_error(DevCamChk(CamPiow(in_struct->name, a, f, d, 16, 0), &one, &one))
#define stop(f,a,n,d)  return_on_error(DevCamChk(CamStopw(in_struct->name, a, f, n, d, 24, 0), &one, &one))

EXPORT int j412___init(struct descriptor *nid_d_ptr __attribute__ ((unused)), InInitStruct * in_struct)
{
  struct descriptor_xd xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
  int status;
  status = TdiLong((struct descriptor *)in_struct->set_points, &xd MDS_END_ARG);
  if (status & 1)
    status = TdiData((struct descriptor *)&xd, &xd MDS_END_ARG);
  if (status & 1) {
    struct descriptor_a *a_ptr = (struct descriptor_a *)xd.pointer;
    int num = a_ptr->arsize / a_ptr->length;
    int *buff = (int *)a_ptr->pointer;
    int i;

    num = max(min(1024, num), 0);

    if (num) {
      status = status & 1;
      for (i = 1; (i < num) && status; i++)
	status = buff[i] > buff[i - 1];

      if (status) {
	pio(24, 0, 0);		/* disable module */
	pio(16, 1, &in_struct->num_cycles);	/* set recycle mode */
	pio(16, 2, 0);
	stop(16, 0, num, buff);
	pio(26, 0, 0);		/* enable module */
      } else
	status = J412$_NOT_SORTED;
    } else
      status = J412$_NO_DATA;
 on_error:
    MdsFree1Dx(&xd, 0);
  }
  return status;
}
