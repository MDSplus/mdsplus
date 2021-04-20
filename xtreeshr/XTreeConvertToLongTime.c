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
#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <mdstypes.h>
#include <pthread_port.h>
#include <stdio.h>
#include <tdishr.h>
#include <xtreeshr.h>
#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#endif

// Convert a time expression to 64 bit integer
#define L9223372036854775807L 0x7fffffffffffffffL
EXPORT int XTreeConvertToLongTime(mdsdsc_t *timeD, int64_t *retTime)
{
  int status;
  EMPTYXD(xd);
  FREEXD_ON_EXIT(&xd);
  status = TdiData(timeD, &xd MDS_END_ARG);
  if (STATUS_NOT_OK)
    return status;
  if (!xd.pointer || xd.pointer->class != CLASS_S)
    status = MDSplusERROR; // InvalidTimeFormat
  else
    switch (xd.pointer->dtype)
    {
    case DTYPE_B:
      *retTime = ((int64_t) * (int8_t *)xd.pointer->pointer) * 1000000000L;
      break;
    case DTYPE_BU:
      *retTime = ((int64_t) * (uint8_t *)xd.pointer->pointer) * 1000000000L;
      break;
    case DTYPE_W:
      *retTime = ((int64_t) * (int16_t *)xd.pointer->pointer) * 1000000000L;
      break;
    case DTYPE_WU:
      *retTime = ((int64_t) * (uint16_t *)xd.pointer->pointer) * 1000000000L;
      break;
    case DTYPE_L:
      *retTime = ((int64_t) * (int32_t *)xd.pointer->pointer) * 1000000000L;
      break;
    case DTYPE_LU:
      *retTime = ((int64_t) * (uint32_t *)xd.pointer->pointer) * 1000000000L;
      break;
    case DTYPE_Q:
      *retTime = *(int64_t *)xd.pointer->pointer;
      break;
    case DTYPE_QU:
      if ((*(uint64_t *)xd.pointer->pointer) > L9223372036854775807L)
        *retTime = L9223372036854775807L;
      else
        *retTime = *(int64_t *)xd.pointer->pointer;
      break;
    default:
    {
      // Not a 64 bit integer, try to convert it via float or double * 1e9
      status = TdiFloat((mdsdsc_t *)&xd, &xd MDS_END_ARG);
      if (STATUS_OK)
      {
        double dbl;
        if (xd.pointer->dtype == DTYPE_DOUBLE)
          dbl = *(double *)xd.pointer->pointer;
        else
          dbl = *(float *)xd.pointer->pointer;
        if (dbl < -9223372036.854775807)
          *retTime = -L9223372036854775807L - 1;
        else if (dbl > 9223372036.854775806)
          *retTime = L9223372036854775807L;
        else
          *retTime = (int64_t)(dbl * 1e9);
      }
    }
    }
  FREEXD_NOW(&xd);
  return status;
}

EXPORT int XTreeConvertToDouble(mdsdsc_t *const timeD, double *const retTime)
{
  int status;
  EMPTYXD(xd);
  FREEXD_ON_EXIT(&xd);
  status = TdiData(timeD, &xd MDS_END_ARG);
  if (STATUS_NOT_OK)
    return status;
  if (!xd.pointer || xd.pointer->class != CLASS_S)
    status = MDSplusERROR; // InvalidTimeFormat
  else
    switch (xd.pointer->dtype)
    {
    case DTYPE_B:
      *retTime = (double)*(int8_t *)xd.pointer->pointer;
      break;
    case DTYPE_BU:
      *retTime = (double)*(uint8_t *)xd.pointer->pointer;
      break;
    case DTYPE_W:
      *retTime = (double)*(int16_t *)xd.pointer->pointer;
      break;
    case DTYPE_WU:
      *retTime = (double)*(uint16_t *)xd.pointer->pointer;
      break;
    case DTYPE_L:
      *retTime = (double)*(int32_t *)xd.pointer->pointer;
      break;
    case DTYPE_LU:
      *retTime = (double)*(uint32_t *)xd.pointer->pointer;
      break;
    case DTYPE_Q:
      *retTime = ((double)*(int64_t *)xd.pointer->pointer) / 1e9;
      break;
    case DTYPE_QU:
      *retTime = ((double)*(uint64_t *)xd.pointer->pointer) / 1e9;
      break;
    case DTYPE_FLOAT:
      *retTime = (double)*(float *)xd.pointer->pointer;
      break;
    case DTYPE_DOUBLE:
      *retTime = *(double *)xd.pointer->pointer;
      break;
    default:
    {
      status = TdiFloat((mdsdsc_t *)&xd, &xd MDS_END_ARG);
      if (STATUS_OK)
      {
        if (xd.pointer->dtype == DTYPE_DOUBLE)
          *retTime = *(double *)xd.pointer->pointer;
        else
          *retTime = *(float *)xd.pointer->pointer;
      }
    }
    }
  FREEXD_NOW(&xd);
  return status;
}
