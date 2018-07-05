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
#include <stdio.h>
#include <mdstypes.h>
#include <xtreeshr.h>
#include <mdsdescrip.h>
#include <mds_stdarg.h>
#include <mdsshr.h>
#include <tdishr.h>

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#endif

//Convert a time expression to 64 bit integer

EXPORT int XTreeConvertToLongTime(struct descriptor *timeD, int64_t * retTime)
{

  int status;
  EMPTYXD(xd);

  status = TdiData(timeD, &xd MDS_END_ARG);
  if (!(status & 1))
    return status;

  if (!xd.pointer || xd.pointer->class != CLASS_S) {
    MdsFree1Dx(&xd, 0);
    return 0;			//InvalidTimeFormat
  }

  if (xd.pointer->dtype == DTYPE_Q || xd.pointer->dtype == DTYPE_QU) {
    *retTime = *(int64_t *) xd.pointer->pointer;
    MdsFree1Dx(&xd, 0);
    return 1;
  }
  //Not a 64 bit integer, try to convert it to a float and use MdsFloatToTime
  status = TdiFloat((struct descriptor *)&xd, &xd MDS_END_ARG);
  if (!(status & 1))
    return status;

  if (xd.pointer->dtype == DTYPE_DOUBLE)
    MdsFloatToTime(*(double *)xd.pointer->pointer, retTime);
  else
    MdsFloatToTime(*(float *)xd.pointer->pointer, retTime);

//      MdsFloatToTime(*(float *)xd.pointer->pointer, retTime);

  MdsFree1Dx(&xd, 0);
  return status;
}

EXPORT int XTreeConvertToLongDelta(struct descriptor *timeD, int64_t * retTime)
{

  int status;
  EMPTYXD(xd);

  status = TdiData(timeD, &xd MDS_END_ARG);
  if (!(status & 1))
    return status;

  if (!xd.pointer || xd.pointer->class != CLASS_S) {
    MdsFree1Dx(&xd, 0);
    return 0;			//InvalidTimeFormat
  }

  if (xd.pointer->dtype == DTYPE_Q || xd.pointer->dtype == DTYPE_QU) {
    *retTime = *(int64_t *) xd.pointer->pointer;
    MdsFree1Dx(&xd, 0);
    return 1;
  }
  //Not a 64 bit integer, try to convert it to a float and use MdsFloatToTime
  status = TdiFloat((struct descriptor *)&xd, &xd MDS_END_ARG);
  if (!(status & 1))
    return status;
  if (xd.pointer->dtype == DTYPE_DOUBLE)
    MdsFloatToDelta(*(double *)xd.pointer->pointer, retTime);
  else
    MdsFloatToDelta(*(float *)xd.pointer->pointer, retTime);
  MdsFree1Dx(&xd, 0);
  return status;
}
