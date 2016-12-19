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
