/*------------------------------------------------------------------------------

       Name:    TdiUnary

       Type:    C function

       Author:  Mark London
               MIT Plasma Fusion Center

       Date:    20-SEP-1993

       Purpose: Checks descriptors for valid classes and equal lengths.
                Returns the count of the number of elements if the
                descriptors points to an array, else returns 1 for scalar
                or string.

       Call sequence:
               struct descriptor *in_ptr;
               struct descriptor *out_ptr;
               int  *out_count;

               status = TdiUnary(in_ptr,out_ptr,out_count);
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
#include <STATICdef.h>
#include <mdsdescrip.h>
#include <tdishr_messages.h>



int TdiUnary(struct descriptor *in_ptr, struct descriptor *out_ptr, unsigned int *out_count)
{
  struct descriptor_a *a;

  switch (out_ptr->class) {
  case CLASS_S:
  case CLASS_D:
    *out_count = 1;
    break;
  case CLASS_A:
    a = (struct descriptor_a *)out_ptr;
    if ((*out_count = out_ptr->length ? a->arsize / out_ptr->length : 0) <= 0)
      return MDSplusSUCCESS;
    break;
  default:
    return TdiINVCLADSC;
  }

  switch (in_ptr->class) {
  case CLASS_S:
  case CLASS_D:
    break;
  case CLASS_A:
    a = (struct descriptor_a *)in_ptr;
    if (a->arsize / in_ptr->length < *out_count)
      return TdiINV_SIZE;
    break;
  default:
    return TdiINVCLADSC;
  }

  return MDSplusSUCCESS;
}

/*  CMS REPLACEMENT HISTORY, Element Tdi3$UNARY.C */
/*  *11   28-AUG-1996 08:14:47 TWF "Take out LibEstablish call" */
/*  *10    2-AUG-1996 08:12:56 TWF "Use int instead of long" */
/*  *9    26-JUL-1996 12:20:50 TWF "Special handling for alpha and vms" */
/*  *8    25-JUN-1996 10:32:27 TWF "Port to Unix/Windows" */
/*  *7    17-OCT-1995 16:16:46 TWF "use <builtins.h> form" */
/*  *6    19-OCT-1994 12:25:57 TWF "Use TDI$MESSAGES" */
/*  *5    19-OCT-1994 09:56:57 TWF "No longer support VAXC" */
/*  *4    15-NOV-1993 10:09:25 TWF "Add memory block" */
/*  *3    15-NOV-1993 09:42:03 TWF "Add memory block" */
/*  *2     1-OCT-1993 10:43:01 MRL "set TdiFault = 0" */
/*  *1    24-SEP-1993 11:17:49 MRL "C version TDI$JSB_UNARY for AXP" */
/*  CMS REPLACEMENT HISTORY, Element Tdi3$UNARY.C */
