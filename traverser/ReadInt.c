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
#include <status.h>
#include <stdlib.h>
#include <string.h>

extern int TdiCompile();
extern int TdiEvaluate();
extern int TdiExecute();

int ReadInt(char *expr, ...) {
  INIT_STATUS;
  static struct descriptor expr_dsc = {0, DTYPE_T, CLASS_S, 0};
  int ans;
  static struct descriptor_xd ans_xd = {0, DTYPE_DSC, CLASS_XD, 0, 0};
  expr_dsc.length = strlen(expr);
  expr_dsc.pointer = expr;
  int numargs;
  struct descriptor *dsc_ptrs[3];
  VA_LIST_MDS_END_ARG(dsc_ptrs, numargs, 0, 0, expr);
  switch (numargs) {
  case 0:
    status = TdiExecute(&expr_dsc, &ans_xd MDS_END_ARG);
    break;
  case 1:
    status = TdiExecute(&expr_dsc, dsc_ptrs[0], &ans_xd MDS_END_ARG);
    break;
  case 2:
    status =
        TdiExecute(&expr_dsc, dsc_ptrs[0], dsc_ptrs[1], &ans_xd MDS_END_ARG);
    break;
  default:
    return 0;
  }
  if
    STATUS_OK {
      /*    status = TdiEvaluate(&ans_xd, &ans_xd); */
      if
        STATUS_OK {
          struct descriptor *d_ptr;
          for (d_ptr = (struct descriptor *)&ans_xd; d_ptr->dtype == DTYPE_DSC;
               d_ptr = (struct descriptor *)d_ptr->pointer)
            ;
          if (d_ptr->dtype == DTYPE_L) {
            /*
               ans = (int)malloc(sizeof(int));
             */
            ans = *(int *)d_ptr->pointer;
          } else if (d_ptr->dtype == DTYPE_NID) {
            /*
               ans = (int)malloc(sizeof(int));
             */
            ans = *(int *)d_ptr->pointer;
          } else if (d_ptr->dtype == DTYPE_WU) {
            /*
               ans = (int)malloc(sizeof(int));
             */
            ans = *(unsigned short *)d_ptr->pointer;
          } else if (d_ptr->dtype == DTYPE_W) {
            /*
               ans = (int)malloc(sizeof(int));
             */
            ans = *(short *)d_ptr->pointer;
          } else if (d_ptr->dtype == DTYPE_BU) {
            /*
               ans = (int)malloc(sizeof(int));
             */
            ans = *(char *)d_ptr->pointer;
          } else
            ans = 0;
        }
      else
        ans = 0;
      MdsFree1Dx(&ans_xd, 0);
    }
  else
    ans = 0;
  return ans;
}
