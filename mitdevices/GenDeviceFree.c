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

		Name:   GEN_DEVICE$FREE   

		Type:   C function

     		Author:	Gabriele Manduchi
			Istituto Gas Ionizzati del CNR - Padova (Italy)

		Date:   17-SEP-1993

    		Purpose: Free dynamic data structures allocated by routines generatted by GEN_DEVICE 

------------------------------------------------------------------------------*/
#include <mdsdescrip.h>
#include <stdlib.h>
#include <mdsshr.h>
#include "gen_device.h"

EXPORT int GenDeviceFree(CommonInStruct * in_struct)
{
  int i, status = 1;
  for (i = 0; (status & 1) && (i < in_struct->num_xds); i++)
    status = MdsFree1Dx(&in_struct->xds[i], 0);
  if (in_struct->num_xds) {
    free((char *)in_struct->xds);
    in_struct->num_xds = 0;
  }
  return status;
}
