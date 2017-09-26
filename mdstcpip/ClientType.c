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
#include "mdsip_connections.h"

//#define VMS_CLIENT       1
//#define IEEE_CLIENT      2
//#define JAVA_CLIENT      3
//#define VMSG_CLIENT      4
//#define CRAY_IEEE_CLIENT 7
//#define CRAY_CLIENT      8

/// 
/// Gets the id of this client machine using internal test function.
/// The client types discovered by this function are the followings:
/// 
/// |client type       | value |
/// |:-----------------|-------|
/// | VMS_CLIENT       |   1   |
/// | IEEE_CLIENT      |   2   |
/// | VMSG_CLIENT      |   4   |
/// | CRAY_IEEE_CLIENT |   7   |
/// | CRAY_CLIENT      |   8   |
/// 
/// 
char ClientType(void)
{
  static char ctype = 0;
  if (!ctype) {
    union {
      int i bits32;
      char c[sizeof(double)];
      float x;
      double d;
    } client_test;
    client_test.x = 1.;
    if (client_test.i == 0x4080) {
      client_test.d = 12345678;
      if (client_test.c[5])
    ctype = VMSG_CLIENT;
      else
    ctype = VMS_CLIENT;
    } else if (client_test.i == 0x3F800000) {
      if (sizeof(int) == 8)
    ctype = CRAY_IEEE_CLIENT;
      else
    ctype = IEEE_CLIENT;
    } else
      ctype = CRAY_CLIENT;
    client_test.i = 1;
    if (!client_test.c[0])
      ctype |= BigEndian;
  }
  return ctype;
}
