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
/*  CMS REPLACEMENT HISTORY, Element Tdi3Eq.C */
/*  *15   26-AUG-1996 15:07:24 TWF "remove ints.h" */
/*  *14   16-AUG-1996 08:42:01 TWF "Add ieee support" */
/*  *13   26-JUL-1996 12:46:22 TWF "Special handling for alpha and vms" */
/*  *12   24-JUN-1996 11:32:22 TWF "Port to Unix/Windows" */
/*  *11    7-MAR-1995 08:58:31 TWF "Fix NE on strings" */
/*  *10    7-MAR-1995 08:46:24 TWF "Fix NE on strings" */
/*  *9    19-OCT-1994 12:26:27 TWF "Use TDI$MESSAGES" */
/*  *8    19-OCT-1994 10:33:53 TWF "No longer support VAXC" */
/*  *7    24-SEP-1993 14:30:05 MRL "Fix to allow scalars." */
/*  *6    24-SEP-1993 13:53:37 MRL "from" */
/*  *5    14-SEP-1993 15:22:40 MRL "Fix string comparison." */
/*  *4    14-SEP-1993 14:17:41 MRL " Fix character comparisons.         " */
/*  *3    14-SEP-1993 11:59:31 MRL "typo" */
/*  *2    14-SEP-1993 11:54:55 MRL "Fix compare_n " */
/*  *1    13-SEP-1993 16:12:18 TWF "C version of Tdi3Eq.MAR for AXP" */
/*  CMS REPLACEMENT HISTORY, Element Tdi3Eq.C */
/*------------------------------------------------------------------------------

       Name:    Tdi3_Eq

       Type:    C function

       Author:  Mark London
               MIT Plasma Fusion Center

       Date:    7-SEP-1993

       Purpose:
        To test major VMS data types and classes.
                out = in1 OP in2
        or      out = LLT(in1, in2)

        Test OP may be EQ GE GT LE LT NE.
        F77 functions LGE LGT LLE LLT also.
        Unsigned compares for unsigned types.
        Only EQ or NE are allowed for COMPLEX types.
        Input and output memory overlap produces UNDEFINED results.
        Coincidence of input and output, however, will work here.

       Call sequence:
               struct descriptor *in1_ptr;
               struct descriptor *in2_ptr;
               struct descriptor *out_ptr;

               status = Tdi3Eq(in1_ptr,in2_ptr,out_ptr);
               status = Tdi3Ge(in1_ptr,in2_ptr,out_ptr);
               status = Tdi3Gt(in1_ptr,in2_ptr,out_ptr);
               status = Tdi3Le(in1_ptr,in2_ptr,out_ptr);
               status = Tdi3Lt(in1_ptr,in2_ptr,out_ptr);
               status = Tdi3Ne(in1_ptr,in2_ptr,out_ptr);
       returns:
                TdiINVDTYDSC - if unsupported data type
                1 - if supported data type

------------------------------------------------------------------------------
       Copyright (c) 1993
       Property of Massachusetts Institute of Technology, Cambridge MA 02139.
       This program cannot be copied or distributed in any form for non-MIT
       use without specific written approval of MIT Plasma Fusion Center
       Management.
------------------------------------------------------------------------------*/
#include <stdint.h>
#include <mdsdescrip.h>
#include <mdsdescrip.h>
#include <tdishr_messages.h>
#include <STATICdef.h>



#define OP_EQ 0
#define OP_GE 1
#define OP_GT 2
#define OP_LE 3
#define OP_LT 4
#define OP_NE 5

extern int CvtConvertFloat();
extern int TdiBinary();
extern int Tdi3Not();

#define min(a,b) ((a)<(b)) ? (a) : (b)
#define max(a,b) ((a)<(b)) ? (b) : (a)

#define compare(type,op) \
  if (s1) \
     for (i=0;i<out_count;i++) out[i] = (char)(in1[0] op in2[i]); \
  else if (s2) \
     for (i=0;i<out_count;i++) out[i] = (char)(in1[i] op in2[0]); \
  else \
     for (i=0;i<out_count;i++) out[i] = (char)(in1[i] op in2[i]); \
  break;

#define test(type) {\
  type *in1=(type *)(in1_ptr->pointer), \
       *in2=(type *)(in2_ptr->pointer); \
  char *out=out_ptr->pointer; \
  switch (op) \
  { \
   case OP_EQ: compare(type,==) \
   case OP_GE: compare(type,>=) \
   case OP_GT: compare(type,>) \
   case OP_LE: compare(type,<=) \
   case OP_LT: compare(type,<) \
   case OP_NE: compare(type,!=) \
  } \
} \
break;

#define comparef(type,dtype,native,op) \
  for (i=0;i<out_count;i++,in1 += in1_inc, in2 += in2_inc)\
    if (CvtConvertFloat(in1,dtype,&a,native,0) && CvtConvertFloat(in2,dtype,&b,native,0))\
      out[i] = (char)(a op b);\
    else\
      out[i] = (char)0;\
  break;

#define testf(type,dtype,native) {\
  type *in1=(type *)(in1_ptr->pointer), \
       *in2=(type *)(in2_ptr->pointer); \
  type a,b;\
  char *out=out_ptr->pointer; \
  int in1_inc = s1 ? 0 : 1;\
  int in2_inc = s2 ? 0 : 1;\
  switch (op) \
  { \
   case OP_EQ: comparef(type,dtype,native,==) \
   case OP_GE: comparef(type,dtype,native,>=) \
   case OP_GT: comparef(type,dtype,native,>) \
   case OP_LE: comparef(type,dtype,native,<=) \
   case OP_LT: comparef(type,dtype,native,<) \
   case OP_NE: comparef(type,dtype,native,!=) \
  } \
  break;\
}

/*
  compare_n compares multi-longword data types.  For signed integer,
  all long words are compared using unsigned except for top longword.
*/
#define compare_n(type,op,longwords,signed) \
  for (i=0;i<out_count;i++) {               \
    l = longwords*i; \
    for (j=longwords-1;j>=0;j--) \
      if (!j || in1[s1 ? j : l+j] != in2[s2 ? j : l+j]) {       \
        if (!(signed) || !(j != longwords-1)) \
          {out[i] = (char)(in1[s1 ? j : l+j] op in2[s2 ? j : l+j]); break;} \
        else {i1 = (int *)(in1+(s1 ? j : l+j)), \
              i2 = (int *)(in2+(s2 ? j : l+j)); \
          out[i] = (char)(*i1 op *i2); break;} \
      }}                                       \
   break;

#define testn(type,longwords,signed) {\
  type *in1=(type *)(in1_ptr->pointer), \
       *in2=(type *)(in2_ptr->pointer); \
  char *out=out_ptr->pointer; \
  switch (op) \
  { \
   case OP_EQ: compare_n(type,==,longwords,signed) \
   case OP_GE: compare_n(type,>=,longwords,signed) \
   case OP_GT: compare_n(type,>,longwords,signed) \
   case OP_LE: compare_n(type,<=,longwords,signed) \
   case OP_LT: compare_n(type,<,longwords,signed) \
   case OP_NE: compare_n(type,!=,longwords,signed) \
  } \
} \
break;

/*
   compare_c is used for strings compares.  Characters are compared one by
   one.  Other than NE, compares stop as soon as they find a character which
   results in a false result.  GT and LT operations allow equal comparisons
   to be a valid result for all except the last character in the string.
   Unequal strings are compared by using spaces to fill the smaller string.
   (All this emulates the CMPC macro instruction.)
*/
#define compare_c(type,op,breakout,equal_until_last,not_op) \
  for (i=0;i<out_count;i++) { \
    minlen = min(in1_ptr->length,in2_ptr->length); \
    maxlen = max(in1_ptr->length,in2_ptr->length); \
    k = 0; \
    out[i] = not_op 1; \
    while (k < maxlen) { \
      if (k >= minlen) { \
        type *in = (type *)((k >= in1_ptr->length) ? in2 : in1); \
        if (!(equal_until_last & (k < maxlen-1 && ' ' == in[k]))) {\
          if (!((in == in2) ? (' ' op in[k]) : (in[k] op ' '))) \
            {out[i] = not_op 0; break;} \
          else {breakout}		\
        } \
      } else \
        if (!(equal_until_last & (k < maxlen-1 && in1[k] == in2[k]))) {\
          if (!(in1[k] op in2[k])) \
            {out[i] = not_op 0; break;} \
          else {breakout}		\
        } \
      k++; \
    } \
    if (!s1) in1=(type *)((char *)(in1)+in1_ptr->length); \
    if (!s2) in2=(type *)((char *)(in2)+in2_ptr->length); \
  }\
  break;

#define testc(type) {\
  type *in1=(type *)(in1_ptr->pointer), \
       *in2=(type *)(in2_ptr->pointer); \
  char *out=out_ptr->pointer; \
  switch (op) \
  { \
   case OP_EQ: compare_c(type,==,;,0,(char))\
   case OP_GE: compare_c(type,<,break;,1,(char)!) \
   case OP_GT: compare_c(type,>,break;,1,(char)) \
   case OP_LE: compare_c(type,>,break;,1,(char)!) \
   case OP_LT: compare_c(type,<,break;,1,(char)) \
   case OP_NE: compare_c(type,!=,;,1,(char)) \
  } \
} \
break;

int Tdi3_Eq(struct descriptor *in1_ptr,
	    struct descriptor *in2_ptr, struct descriptor *out_ptr, int op)
{
  INIT_STATUS;
  int out_count = 1;
  register int i, j, k;
  int l, minlen, maxlen, *i1, *i2;
  int s1 = (in1_ptr->class != CLASS_A);
  int s2 = (in2_ptr->class != CLASS_A);

  status = TdiBinary(in1_ptr, in2_ptr, out_ptr, &out_count);
  if STATUS_NOT_OK
    return status;

  switch (in1_ptr->dtype) {
  case DTYPE_T:
    testc(unsigned char)
    case DTYPE_BU:test(uint8_t)
    case DTYPE_WU:test(uint16_t)
    case DTYPE_LU:test(uint32_t)
    case DTYPE_QU:test(uint64_t)
    case DTYPE_OU:testn(unsigned int, 4, 0)
    case DTYPE_B:test(int8_t)
    case DTYPE_W:test(int16_t)
    case DTYPE_L:test(int32_t)
    case DTYPE_Q:test(int64_t)
    case DTYPE_O:testn(int, 4, 1)
    case DTYPE_F:testf(float, DTYPE_F, DTYPE_NATIVE_FLOAT)
    case DTYPE_FS:testf(float, DTYPE_FS, DTYPE_NATIVE_FLOAT)
    case DTYPE_G:testf(double, DTYPE_G, DTYPE_NATIVE_DOUBLE)
    case DTYPE_D:testf(double, DTYPE_D, DTYPE_NATIVE_DOUBLE)
    case DTYPE_FT:testf(double, DTYPE_FT, DTYPE_NATIVE_DOUBLE)
    case DTYPE_FC:case DTYPE_FSC:if (op != OP_EQ && op != OP_NE) {
      status = TdiINVDTYDSC;
      break;
    }
    testn(int, 2, 0)
    case DTYPE_GC:case DTYPE_DC:case DTYPE_FTC:if (op != OP_EQ && op != OP_NE) {
      status = TdiINVDTYDSC;
      break;
    }
    testn(int, 4, 0)
    default:status = TdiINVDTYDSC;
  }
  return status;
}

int Tdi3Eq(struct descriptor *in1_ptr, struct descriptor *in2_ptr, struct descriptor *out_ptr)
{
  return Tdi3_Eq(in1_ptr, in2_ptr, out_ptr, OP_EQ);
}

int Tdi3Ge(struct descriptor *in1_ptr, struct descriptor *in2_ptr, struct descriptor *out_ptr)
{
  return Tdi3_Eq(in1_ptr, in2_ptr, out_ptr, OP_GE);
}

int Tdi3Gt(struct descriptor *in1_ptr, struct descriptor *in2_ptr, struct descriptor *out_ptr)
{
  return Tdi3_Eq(in1_ptr, in2_ptr, out_ptr, OP_GT);
}

int Tdi3Le(struct descriptor *in1_ptr, struct descriptor *in2_ptr, struct descriptor *out_ptr)
{
  return Tdi3_Eq(in1_ptr, in2_ptr, out_ptr, OP_LE);
}

int Tdi3Lt(struct descriptor *in1_ptr, struct descriptor *in2_ptr, struct descriptor *out_ptr)
{
  return Tdi3_Eq(in1_ptr, in2_ptr, out_ptr, OP_LT);
}

int Tdi3Ne(struct descriptor *in1_ptr, struct descriptor *in2_ptr, struct descriptor *out_ptr)
{
  int status = Tdi3_Eq(in1_ptr, in2_ptr, out_ptr, OP_EQ);
  if STATUS_OK
    status = Tdi3Not(out_ptr, out_ptr);
  return status;
}
