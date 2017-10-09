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
#include <mdstypes.h>
#include <string.h>
#include <mdsshr.h>

//Conversion to Float/Doublee to 64 bit int.

//MDSplus specific time conversion. It is the number of nanoseconds 1 day before time 0.

EXPORT void MdsFloatToTime(double floatTime, int64_t * outTime)
{
  int64_t baseTime;
  int64_t currTime;

  baseTime = 1000000000;
  baseTime *= (24 * 3600);
  currTime = (int64_t) (floatTime / 1E-9);
  *outTime = baseTime + currTime;
}

EXPORT void MdsFloatToDelta(double floatTime, int64_t * outTime)
{
  int64_t currTime;

  currTime = (int64_t) (floatTime / 1E-9);
  *outTime = currTime;
}

EXPORT void MdsTimeToFloat(int64_t inTime, float *outFloat)
{
  int64_t baseTime, currTime;

  baseTime = 1000000000;
  baseTime *= (24 * 3600);
  currTime = inTime - baseTime;
  *outFloat = (float)((double)currTime * 1E-9);
}

EXPORT void MdsTimeToDouble(int64_t inTime, double *outFloat)
{
  int64_t baseTime, currTime;

  baseTime = 1000000000;
  baseTime *= (24 * 3600);
  currTime = inTime - baseTime;
  *outFloat = (double)currTime * 1E-9;
}
