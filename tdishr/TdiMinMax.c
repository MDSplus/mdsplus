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
/*      Tdi1MinMax.C
        Multi-argument routines: MAX, MIN pairwise compared.
                out = MAX(in1, in2, ...)

        Ken Klare, LANL CTR-7   (c)1989,1990
*/
#include <STATICdef.h>
#include <status.h>
#include "tdirefstandard.h"
#include <stdlib.h>
#include <mdsshr.h>



extern int TdiEvaluate();
extern int Tdi1Same();
extern int TdiData();
extern int TdiGetLong();

int Tdi1MinMax(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp, *newlist[2];
  int j;

  newlist[0] = &tmp;
  status = TdiEvaluate(list[0], out_ptr MDS_END_ARG);
  for (j = 1; STATUS_OK && j < narg; j++) {
    tmp = *out_ptr;
    *out_ptr = EMPTY_XD;
    newlist[1] = (struct descriptor_xd *)list[j];
    status = Tdi1Same(opcode, 2, newlist, out_ptr);
    MdsFree1Dx(&tmp, NULL);
  }
  return status;
}

/*----------------------------------------------------------------
        Tdi1Conditional.C
        Test for scalar conditional and return true or false source.
                out = cond ? true : false

        If the condition is not scalar use MERGE(true, false, cond).
        Note that is cond is a scalar, the units and signality are not conformed.
*/
int Tdi1Conditional(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;

  status = TdiData(list[2], out_ptr MDS_END_ARG);
  if STATUS_OK {
    if (out_ptr->pointer && out_ptr->pointer->class == CLASS_A) {
      struct descriptor_xd tmp = *out_ptr;
      struct descriptor *new[3];
      new[0] = list[0];
      new[1] = list[1];
      new[2] = (struct descriptor *)&tmp;
      *out_ptr = EMPTY_XD;
      status = Tdi1Same(opcode, narg, new, out_ptr);
      MdsFree1Dx(&tmp, NULL);
    } else {
      int truth;
      status = TdiGetLong(out_ptr, &truth);
      if STATUS_OK {
	if (truth & 1)
	  status = TdiEvaluate(list[0], out_ptr MDS_END_ARG);
	else
	  status = TdiEvaluate(list[1], out_ptr MDS_END_ARG);
      }
    }
  }
  return status;
}
