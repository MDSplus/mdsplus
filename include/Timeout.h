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
#ifndef TIMEOUT_H_
#define TIMEOUT_H_
#include "SystemSpecific.h"
class EXPORT MdsTimeout {
  long secs;
  long nanoSecs;
 public:
   MdsTimeout(long secs, long nanoSecs) {
    this->secs = secs + nanoSecs / 1000000000;
    this->nanoSecs = nanoSecs % 1000000000;
  } MdsTimeout(long milliSecs) {
    this->secs = milliSecs / 1000;
    this->nanoSecs = (milliSecs % 1000) * 1000000;
  }
  long getSecs() {
    return secs;
  }
  long getNanoSecs() {
    return nanoSecs;
  }
  long getTotMilliSecs() {
    return secs * 1000 + nanoSecs / 1000000;
  }
};

#endif				/*TIMEOUT_H_ */
