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
/*  CMS REPLACEMENT HISTORY, Element Tdi3$BINARY.C */
/*  *6    28-AUG-1996 08:22:46 TWF "Take out LibEstablish call" */
/*  *5     1-AUG-1996 17:20:50 TWF "Use int instead of long" */
/*  *4    25-JUN-1996 10:11:30 TWF "Port to Unix/Windows" */
/*  *3    19-OCT-1994 12:25:21 TWF "Use TDI$MESSAGES" */
/*  *2    19-OCT-1994 09:52:24 TWF "No longer support VAXC" */
/*  *1    13-SEP-1993 16:13:53 TWF "C version TDI$JSB_BINARY for AXP" */
/*  CMS REPLACEMENT HISTORY, Element Tdi3$BINARY.C */
/*------------------------------------------------------------------------------

       Name:    TdiBinary

       Type:    C function

       Author:  Mark London
               MIT Plasma Fusion Center

       Date:    7-SEP-1993

       Purpose: Checks descriptors for valid classes and equal lengths.
                Returns the count of the number of elements if the
                descriptors points to an array, else returns 1 for scalar
                or string.

       Call sequence:
               struct descriptor *in1_ptr;
               struct descriptor *in2_ptr;
               struct descriptor *out_ptr;
               int  *out_count;

               status = TdiBinary(in1_ptr,in2_ptr,out_ptr,out_count);
       returns:
                TdiINVCLADSC - if unsupported class
                TdiINV_SIZE - if descriptor length mismatch
                1 - if supported class and lengths match

------------------------------------------------------------------------------
       Copyright (c) 1993
       Property of Massachusetts Institute of Technology, Cambridge MA 02139.
       This program cannot be copied or distributed in any form for non-MIT
       use without specific written approval of MIT Plasma Fusion Center
       Management.
------------------------------------------------------------------------------*/
#include <mdsdescrip.h>
#include <tdishr_messages.h>
#include <STATICdef.h>



int TdiBinary(struct descriptor *in1_ptr, struct descriptor *in2_ptr,
	      struct descriptor *out_ptr, unsigned int *out_count)
{
  struct descriptor_a *a;
  switch (out_ptr->class) {
  case CLASS_S:
  case CLASS_D:
    *out_count = 1;
    break;
  case CLASS_A:
    a = (struct descriptor_a *)out_ptr;
    if ((*out_count = (a->arsize / out_ptr->length)) <= 0)
      return MDSplusSUCCESS;
    break;
  default:
    return TdiINVCLADSC;
  }

  switch (in1_ptr->class) {
  case CLASS_S:
  case CLASS_D:
    break;
  case CLASS_A:
    a = (struct descriptor_a *)in1_ptr;
    if (a->arsize / in1_ptr->length < *out_count)
      return TdiINV_SIZE;
    break;
  default:
    return TdiINVCLADSC;
  }

  switch (in2_ptr->class) {
  case CLASS_S:
  case CLASS_D:
    break;
  case CLASS_A:
    a = (struct descriptor_a *)in2_ptr;
    if (a->arsize / in2_ptr->length < *out_count)
      return TdiINV_SIZE;
    break;
  default:
    return TdiINVCLADSC;
  }

  return MDSplusSUCCESS;
}
