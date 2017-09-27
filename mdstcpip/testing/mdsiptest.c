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
/*  CMS REPLACEMENT HISTORY, Element T.C */
/*  *3    16-OCT-1995 13:31:44 TWF "Update from msdos" */
/*  *2     5-JAN-1995 14:07:43 TWF "new definitions" */
/*  *1    28-NOV-1994 15:30:39 TWF "Test program for mdstcpip" */
/*  CMS REPLACEMENT HISTORY, Element T.C */
#include <ipdesc.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char **argv)
{
  int status;
  struct descrip ans;
  float val = 9876;
  struct descrip vald = { DTYPE_FLOAT, 0, {0}, 0, 0 };
  long sock = ConnectToMds((argc > 1) ? argv[1] : "lost.pfc.mit.edu:9000");
  if (sock != -1) {
    printf("status from MdsOpen = %d\n", MdsOpen(sock, "main", -1));
    ans.ptr = 0;
    if (MdsValue(sock, "f_float(member)", &ans, NULL) & 1) {
      printf("%g\n", *(float *)ans.ptr);
      val = *(float *)ans.ptr;
      val = val + (float)1.;
    } else
      printf("%s\n", (char *)ans.ptr);
    if (ans.ptr) {
      free(ans.ptr);
      ans.ptr = 0;
    }
    vald.ptr = (void *)&val;
    status = MdsPut(sock, "member", "$", &vald, NULL);
    if (!(status & 1))
      printf("Error during put %d\n", status);
    if (MdsValue(sock, "42.0", &ans, NULL) & 1)
      printf("%g\n", *(float *)ans.ptr);
    else
      printf("%s\n", (char *)ans.ptr);
    if (ans.ptr)
      free(ans.ptr);
  }
  return 1;
}


