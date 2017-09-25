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
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <mdsdescrip.h>
#include <tdishr_messages.h>
#include <math.h>
#include <STATICdef.h>

#define MAXTYPE (DTYPE_FTC + 1)



extern void CvtConvertFloat();
extern int IsRoprand();

#define TWO_32 (double)4294967296.
#ifdef min
#undef min
#endif
#define min(a,b) (((a) < (b)) ? (a) : (b))
#ifdef max
#undef max
#endif
#define max(a,b) (((a) < (b)) ? (b) : (a))

/****** Identity conversions handled before big switch *********/
#define BU_BU(lena,pa,lenb,pb,numb)
#define WU_WU(lena,pa,lenb,pb,numb)
#define LU_LU(lena,pa,lenb,pb,numb)
#define QU_QU(lena,pa,lenb,pb,numb)
#define OU_OU(lena,pa,lenb,pb,numb)
#define B_B(lena,pa,lenb,pb,numb)
#define W_W(lena,pa,lenb,pb,numb)
#define L_L(lena,pa,lenb,pb,numb)
#define Q_Q(lena,pa,lenb,pb,numb)
#define O_O(lena,pa,lenb,pb,numb)
#define F_F(lena,pa,lenb,pb,numb)
#define FS_FS(lena,pa,lenb,pb,numb)
#define D_D(lena,pa,lenb,pb,numb)
#define G_G(lena,pa,lenb,pb,numb)
#define FT_FT(lena,pa,lenb,pb,numb)
#define FC_FC(lena,pa,lenb,pb,numb)
#define FSC_FSC(lena,pa,lenb,pb,numb)
#define DC_DC(lena,pa,lenb,pb,numb)
#define GC_GC(lena,pa,lenb,pb,numb)
#define FTC_FTC(lena,pa,lenb,pb,numb)
/************* Straight binary conversions *************************/
#define CONVERT_BINARY(ti,to,pa,pb,numb) { ti *ip = (ti *)pa; to *op = (to *)pb; while (numb-- > 0) *op++ = (to)*ip++; status = MDSplusSUCCESS;}
#define BU_WU(lena,pa,lenb,pb,numb) CONVERT_BINARY(unsigned char,unsigned short,pa,pb,numb)
#define BU_LU(lena,pa,lenb,pb,numb) CONVERT_BINARY(unsigned char,unsigned int,pa,pb,numb)
#define BU_B(lena,pa,lenb,pb,numb) CONVERT_BINARY(unsigned char,char,pa,pb,numb)
#define BU_W(lena,pa,lenb,pb,numb) CONVERT_BINARY(unsigned char,short,pa,pb,numb)
#define BU_L(lena,pa,lenb,pb,numb) CONVERT_BINARY(unsigned char,int,pa,pb,numb)
#define WU_BU(lena,pa,lenb,pb,numb) CONVERT_BINARY(unsigned short,unsigned char,pa,pb,numb)
#define WU_LU(lena,pa,lenb,pb,numb) CONVERT_BINARY(unsigned short,unsigned int,pa,pb,numb)
#define WU_B(lena,pa,lenb,pb,numb) CONVERT_BINARY(unsigned short,char,pa,pb,numb)
#define WU_W(lena,pa,lenb,pb,numb) CONVERT_BINARY(unsigned short,short,pa,pb,numb)
#define WU_L(lena,pa,lenb,pb,numb) CONVERT_BINARY(unsigned short,int,pa,pb,numb)
#define LU_BU(lena,pa,lenb,pb,numb) CONVERT_BINARY(unsigned int,unsigned char,pa,pb,numb)
#define LU_WU(lena,pa,lenb,pb,numb) CONVERT_BINARY(unsigned int,unsigned short,pa,pb,numb)
#define LU_B(lena,pa,lenb,pb,numb) CONVERT_BINARY(unsigned int,char,pa,pb,numb)
#define LU_W(lena,pa,lenb,pb,numb) CONVERT_BINARY(unsigned int,short,pa,pb,numb)
#define LU_L(lena,pa,lenb,pb,numb) CONVERT_BINARY(unsigned int,int,pa,pb,numb)
#define B_BU(lena,pa,lenb,pb,numb) CONVERT_BINARY(char,unsigned char,pa,pb,numb)
#define B_WU(lena,pa,lenb,pb,numb) CONVERT_BINARY(char,unsigned short,pa,pb,numb)
#define B_LU(lena,pa,lenb,pb,numb) CONVERT_BINARY(char,unsigned int,pa,pb,numb)
#define B_W(lena,pa,lenb,pb,numb) CONVERT_BINARY(char,short,pa,pb,numb)
#define B_L(lena,pa,lenb,pb,numb) CONVERT_BINARY(char,int,pa,pb,numb)
#define W_BU(lena,pa,lenb,pb,numb) CONVERT_BINARY(short,unsigned char,pa,pb,numb)
#define W_WU(lena,pa,lenb,pb,numb) CONVERT_BINARY(short,unsigned short,pa,pb,numb)
#define W_LU(lena,pa,lenb,pb,numb) CONVERT_BINARY(short,unsigned int,pa,pb,numb)
#define W_B(lena,pa,lenb,pb,numb) CONVERT_BINARY(short,char,pa,pb,numb)
#define W_L(lena,pa,lenb,pb,numb) CONVERT_BINARY(short,int,pa,pb,numb)
#define L_BU(lena,pa,lenb,pb,numb) CONVERT_BINARY(int,unsigned char,pa,pb,numb)
#define L_WU(lena,pa,lenb,pb,numb) CONVERT_BINARY(int,unsigned short,pa,pb,numb)
#define L_LU(lena,pa,lenb,pb,numb) CONVERT_BINARY(int,unsigned int,pa,pb,numb)
#define L_B(lena,pa,lenb,pb,numb) CONVERT_BINARY(int,char,pa,pb,numb)
#define L_W(lena,pa,lenb,pb,numb) CONVERT_BINARY(int,short,pa,pb,numb)
/************* Extended binary conversions *************************/

#ifdef WORDS_BIGENDIAN

#define CONVERT_BINARY_ZETEND(ti,pa,pb,numb,nints) { int i; ti *ip = (ti *)pa; unsigned int *op = (unsigned int *)pb; \
                   while (numb-- > 0) {for(i=1;i<nints;i++) *op++ = (unsigned int)0; *op++ = (unsigned int)*ip++; }\
                   status = MDSplusSUCCESS;}
#define CONVERT_BINARY_SETEND(ti,pa,pb,numb,nints) { int i; ti *ip = (ti *)pa; int *op = (int *)pb; \
                   while (numb-- > 0) {int extend = (*ip < 0) ? -1 : 0; for(i=1;i<nints;i++) *op++ = extend; *op++ = (int)*ip++; }\
                   status = MDSplusSUCCESS;}
#define CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb) {int i; \
                   while(numb-- > 0) {for (i=0;i<lenb;i++) *pb++ = pa[i+lena-lenb]; pa += lena;}\
                   status = MDSplusSUCCESS;}
#define CONVERT_BINARY_LARGER_ZEXTEND(pa,pb,numb,lena,lenb) {int i; \
               while(numb-- > 0) {for (i=0;i<lena;i++) pb[lenb - i - 1] = pa[lena - i - 1];\
               for (i=lena; i<lenb; i++) pb[lenb - i - 1] = (char)0; pb += lenb; pa += lena;}\
               status = MDSplusSUCCESS;}
#define CONVERT_BINARY_LARGER_SEXTEND(pa,pb,numb,lena,lenb) {int i;\
               while(numb-- > 0) {for (i=0;i<lena;i++) pb[lenb - i - 1] = pa[lena - i - 1];\
               for (i=lena; i<lenb; i++) pb[lenb - i - 1] = (char)((pa[0] < 0) ? -1 : 0); pb += lenb; pa += lena;}\
               status = MDSplusSUCCESS;}
#else
#define CONVERT_BINARY_ZETEND(ti,pa,pb,numb,nints) { int i; ti *ip = (ti *)pa; unsigned int *op = (unsigned int *)pb; \
                   while (numb-- > 0) {*op++ = (unsigned int)*ip++; for(i=1;i<nints;i++) *op++ = (unsigned int)0;}\
                   status = MDSplusSUCCESS;}
#define CONVERT_BINARY_SETEND(ti,pa,pb,numb,nints) { int i; ti *ip = (ti *)pa; int *op = (int *)pb; \
                   while (numb-- > 0) {int extend = (*ip < 0) ? -1 : 0; *op++ = (int)*ip++; for(i=1;i<nints;i++) *op++ = extend;}\
                   status = MDSplusSUCCESS;}
#define CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb) {int i; \
                   while(numb-- > 0) {for (i=0;i<lenb;i++) *pb++ = pa[i]; pa += lena;}\
                   status = MDSplusSUCCESS;}
#define CONVERT_BINARY_LARGER_ZEXTEND(pa,pb,numb,lena,lenb) {int i; \
               while(numb-- > 0) {for (i=0;i<lena;i++) *pb++ = *pa++;\
               for (i=lena; i<lenb; i++) *pb++ = (char)0;}\
               status = MDSplusSUCCESS;}
#define CONVERT_BINARY_LARGER_SEXTEND(pa,pb,numb,lena,lenb) {int i;\
               while(numb-- > 0) {for (i=0;i<lena;i++) *pb++ = *pa++; \
               for (i=lena; i<lenb; i++) *pb++ = (char)((pa[-1] < 0) ? -1 : 0);}\
               status = MDSplusSUCCESS;}
#endif

#define BU_QU(lena,pa,lenb,pb,numb) CONVERT_BINARY_ZETEND(unsigned char,pa,pb,numb,2)
#define BU_OU(lena,pa,lenb,pb,numb) CONVERT_BINARY_ZETEND(unsigned char,pa,pb,numb,4)
#define WU_QU(lena,pa,lenb,pb,numb) CONVERT_BINARY_ZETEND(unsigned short,pa,pb,numb,2)
#define WU_OU(lena,pa,lenb,pb,numb) CONVERT_BINARY_ZETEND(unsigned short,pa,pb,numb,4)
#define LU_QU(lena,pa,lenb,pb,numb) CONVERT_BINARY_ZETEND(unsigned int,pa,pb,numb,2)
#define LU_OU(lena,pa,lenb,pb,numb) CONVERT_BINARY_ZETEND(unsigned int,pa,pb,numb,4)
#define B_QU(lena,pa,lenb,pb,numb) CONVERT_BINARY_SETEND(char,pa,pb,numb,2)
#define B_OU(lena,pa,lenb,pb,numb) CONVERT_BINARY_SETEND(char,pa,pb,numb,4)
#define W_QU(lena,pa,lenb,pb,numb) CONVERT_BINARY_SETEND(short,pa,pb,numb,2)
#define W_OU(lena,pa,lenb,pb,numb) CONVERT_BINARY_SETEND(short,pa,pb,numb,4)
#define L_QU(lena,pa,lenb,pb,numb) CONVERT_BINARY_SETEND(int,pa,pb,numb,2)
#define L_OU(lena,pa,lenb,pb,numb) CONVERT_BINARY_SETEND(int,pa,pb,numb,4)
#define BU_Q(lena,pa,lenb,pb,numb) CONVERT_BINARY_ZETEND(unsigned char,pa,pb,numb,2)
#define BU_O(lena,pa,lenb,pb,numb) CONVERT_BINARY_ZETEND(unsigned char,pa,pb,numb,4)
#define WU_Q(lena,pa,lenb,pb,numb) CONVERT_BINARY_ZETEND(unsigned short,pa,pb,numb,2)
#define WU_O(lena,pa,lenb,pb,numb) CONVERT_BINARY_ZETEND(unsigned short,pa,pb,numb,4)
#define LU_Q(lena,pa,lenb,pb,numb) CONVERT_BINARY_ZETEND(unsigned int,pa,pb,numb,2)
#define LU_O(lena,pa,lenb,pb,numb) CONVERT_BINARY_ZETEND(unsigned int,pa,pb,numb,4)
#define B_Q(lena,pa,lenb,pb,numb) CONVERT_BINARY_SETEND(char,pa,pb,numb,2)
#define B_O(lena,pa,lenb,pb,numb) CONVERT_BINARY_SETEND(char,pa,pb,numb,4)
#define W_Q(lena,pa,lenb,pb,numb) CONVERT_BINARY_SETEND(short,pa,pb,numb,2)
#define W_O(lena,pa,lenb,pb,numb) CONVERT_BINARY_SETEND(short,pa,pb,numb,4)
#define L_Q(lena,pa,lenb,pb,numb) CONVERT_BINARY_SETEND(int,pa,pb,numb,2)
#define L_O(lena,pa,lenb,pb,numb) CONVERT_BINARY_SETEND(int,pa,pb,numb,4)
#define QU_BU(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define QU_WU(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define QU_LU(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define QU_B(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define QU_W(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define QU_L(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define OU_BU(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define OU_WU(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define OU_LU(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define OU_B(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define OU_W(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define OU_L(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define Q_BU(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define Q_WU(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define Q_LU(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define Q_B(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define Q_W(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define Q_L(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define O_BU(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define O_WU(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define O_LU(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define O_B(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define O_W(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define O_L(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define QU_OU(lena,pa,lenb,pb,numb) CONVERT_BINARY_LARGER_ZEXTEND(pa,pb,numb,lena,lenb)
#define QU_Q(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define QU_O(lena,pa,lenb,pb,numb) CONVERT_BINARY_LARGER_ZEXTEND(pa,pb,numb,lena,lenb)
#define OU_QU(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define OU_Q(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define OU_O(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define Q_QU(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define Q_OU(lena,pa,lenb,pb,numb) CONVERT_BINARY_LARGER_SEXTEND(pa,pb,numb,lena,lenb)
#define Q_O(lena,pa,lenb,pb,numb) CONVERT_BINARY_LARGER_SEXTEND(pa,pb,numb,lena,lenb)
#define O_QU(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define O_OU(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
#define O_Q(lena,pa,lenb,pb,numb) CONVERT_BINARY_SMALLER(pa,pb,numb,lena,lenb)
/*********** Binary to Floating Point ****************************/
#define CONVERT_FLOAT(pb,numb,otype) if (otype != DTYPE_NATIVE_FLOAT) {int i=numb; float *op = (float *)pb; \
     while (i-- > 0) {float tmp = *op; CvtConvertFloat(&tmp, DTYPE_NATIVE_FLOAT, op++, otype, 0);}}

#define CONVERT_DOUBLE(pb,numb,otype) if (otype != DTYPE_NATIVE_DOUBLE) {int i=numb; double *op = (double *)pb; \
     while (i-- > 0) {double tmp = *op; CvtConvertFloat(&tmp, DTYPE_NATIVE_DOUBLE, op++, otype, 0);}}

#define BINARY_TO_FLOAT(ti,pa,pb,numb,otype)  \
  {int i=numb; ti *ip=(ti*)pa; float *op=(float*)pb; while (i-- > 0) *op++ =(float)*ip++; status = MDSplusSUCCESS; CONVERT_FLOAT(pb,numb,otype)}

#define BINARY_TO_DOUBLE(ti,pa,pb,numb,otype)  \
  {int i=numb; ti *ip=(ti*)pa; double *op=(double*)pb; while (i-- > 0) *op++ =(double)*ip++; status = MDSplusSUCCESS;\
   CONVERT_DOUBLE(pb,numb,otype)}

#define BINARY_TO_FLOATC(ti,pa,pb,numb,otype)  \
  {int i=numb; ti *ip=(ti*)pa; float *op=(float*)pb; while (i-- > 0) {*op++ =(float)*ip++; *op++ = (float)0.0;} status = MDSplusSUCCESS; \
   CONVERT_FLOAT(pb,numb*2,otype)}

#define BINARY_TO_DOUBLEC(ti,pa,pb,numb,otype)  \
  {int i=numb; ti *ip=(ti*)pa; double *op=(double*)pb; while (i-- > 0) {*op++ =(double)*ip++; *op++ = (double)0.0;} status = MDSplusSUCCESS; \
   CONVERT_DOUBLE(pb,numb*2,otype)}

#define BU_F(lena,pa,lenb,pb,numb) BINARY_TO_FLOAT(unsigned char, pa, pb, numb, DTYPE_F)
#define BU_D(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLE(unsigned char, pa, pb, numb, DTYPE_D)
#define BU_G(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLE(unsigned char, pa, pb, numb, DTYPE_G)
#define BU_FC(lena,pa,lenb,pb,numb) BINARY_TO_FLOATC(unsigned char, pa, pb, numb, DTYPE_F)
#define BU_DC(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLEC(unsigned char, pa, pb, numb, DTYPE_D)
#define BU_GC(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLEC(unsigned char, pa, pb, numb, DTYPE_G)
#define BU_FS(lena,pa,lenb,pb,numb) BINARY_TO_FLOAT(unsigned char, pa, pb, numb, DTYPE_FS)
#define BU_FT(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLE(unsigned char, pa, pb, numb, DTYPE_FT)
#define BU_FSC(lena,pa,lenb,pb,numb) BINARY_TO_FLOATC(unsigned char, pa, pb, numb, DTYPE_FSC)
#define BU_FTC(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLEC(unsigned char, pa, pb, numb, DTYPE_FTC)
#define WU_F(lena,pa,lenb,pb,numb) BINARY_TO_FLOAT(unsigned short, pa, pb, numb, DTYPE_F)
#define WU_D(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLE(unsigned short, pa, pb, numb, DTYPE_D)
#define WU_G(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLE(unsigned short, pa, pb, numb, DTYPE_G)
#define WU_FC(lena,pa,lenb,pb,numb) BINARY_TO_FLOATC(unsigned short, pa, pb, numb, DTYPE_F)
#define WU_DC(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLEC(unsigned short, pa, pb, numb, DTYPE_D)
#define WU_GC(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLEC(unsigned short, pa, pb, numb, DTYPE_G)
#define WU_FS(lena,pa,lenb,pb,numb) BINARY_TO_FLOAT(unsigned short, pa, pb, numb, DTYPE_FS)
#define WU_FT(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLE(unsigned short, pa, pb, numb, DTYPE_FT)
#define WU_FSC(lena,pa,lenb,pb,numb) BINARY_TO_FLOATC(unsigned short, pa, pb, numb, DTYPE_FSC)
#define WU_FTC(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLEC(unsigned short, pa, pb, numb, DTYPE_FTC)
#define LU_F(lena,pa,lenb,pb,numb) BINARY_TO_FLOAT(unsigned int, pa, pb, numb, DTYPE_F)
#define LU_D(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLE(unsigned int, pa, pb, numb, DTYPE_D)
#define LU_G(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLE(unsigned int, pa, pb, numb, DTYPE_G)
#define LU_FC(lena,pa,lenb,pb,numb) BINARY_TO_FLOATC(unsigned int, pa, pb, numb, DTYPE_F)
#define LU_DC(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLEC(unsigned int, pa, pb, numb, DTYPE_D)
#define LU_GC(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLEC(unsigned int, pa, pb, numb, DTYPE_G)
#define LU_FS(lena,pa,lenb,pb,numb) BINARY_TO_FLOAT(unsigned int, pa, pb, numb, DTYPE_FS)
#define LU_FT(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLE(unsigned int, pa, pb, numb, DTYPE_FT)
#define LU_FSC(lena,pa,lenb,pb,numb) BINARY_TO_FLOATC(unsigned int, pa, pb, numb, DTYPE_FSC)
#define LU_FTC(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLEC(unsigned int, pa, pb, numb, DTYPE_FTC)
#define B_F(lena,pa,lenb,pb,numb) BINARY_TO_FLOAT(char, pa, pb, numb, DTYPE_F)
#define B_D(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLE(char, pa, pb, numb, DTYPE_D)
#define B_G(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLE(char, pa, pb, numb, DTYPE_G)
#define B_FC(lena,pa,lenb,pb,numb) BINARY_TO_FLOATC(char, pa, pb, numb, DTYPE_F)
#define B_DC(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLEC(char, pa, pb, numb, DTYPE_D)
#define B_GC(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLEC(char, pa, pb, numb, DTYPE_G)
#define B_FS(lena,pa,lenb,pb,numb) BINARY_TO_FLOAT(char, pa, pb, numb, DTYPE_FS)
#define B_FT(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLE(char, pa, pb, numb, DTYPE_FT)
#define B_FSC(lena,pa,lenb,pb,numb) BINARY_TO_FLOATC(char, pa, pb, numb, DTYPE_FSC)
#define B_FTC(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLEC(char, pa, pb, numb, DTYPE_FTC)
#define W_F(lena,pa,lenb,pb,numb) BINARY_TO_FLOAT(short, pa, pb, numb, DTYPE_F)
#define W_D(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLE(short, pa, pb, numb, DTYPE_D)
#define W_G(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLE(short, pa, pb, numb, DTYPE_G)
#define W_FC(lena,pa,lenb,pb,numb) BINARY_TO_FLOATC(short, pa, pb, numb, DTYPE_F)
#define W_DC(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLEC(short, pa, pb, numb, DTYPE_D)
#define W_GC(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLEC(short, pa, pb, numb, DTYPE_G)
#define W_FS(lena,pa,lenb,pb,numb) BINARY_TO_FLOAT(short, pa, pb, numb, DTYPE_FS)
#define W_FT(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLE(short, pa, pb, numb, DTYPE_FT)
#define W_FSC(lena,pa,lenb,pb,numb) BINARY_TO_FLOATC(short, pa, pb, numb, DTYPE_FSC)
#define W_FTC(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLEC(short, pa, pb, numb, DTYPE_FTC)
#define L_F(lena,pa,lenb,pb,numb) BINARY_TO_FLOAT(int, pa, pb, numb, DTYPE_F)
#define L_D(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLE(int, pa, pb, numb, DTYPE_D)
#define L_G(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLE(int, pa, pb, numb, DTYPE_G)
#define L_FC(lena,pa,lenb,pb,numb) BINARY_TO_FLOATC(int, pa, pb, numb, DTYPE_F)
#define L_DC(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLEC(int, pa, pb, numb, DTYPE_D)
#define L_GC(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLEC(int, pa, pb, numb, DTYPE_G)
#define L_FS(lena,pa,lenb,pb,numb) BINARY_TO_FLOAT(int, pa, pb, numb, DTYPE_FS)
#define L_FT(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLE(int, pa, pb, numb, DTYPE_FT)
#define L_FSC(lena,pa,lenb,pb,numb) BINARY_TO_FLOATC(int, pa, pb, numb, DTYPE_FSC)
#define L_FTC(lena,pa,lenb,pb,numb) BINARY_TO_DOUBLEC(int, pa, pb, numb, DTYPE_FTC)
/**************** Long Binary to Floating Point *******************************/
#define LBINARY_TO_FLOAT(pa,pb,numb,size,is_signed,otype)  \
  {int i=numb; unsigned int *ip=(unsigned int*)pa; float *op=(float*)pb;\
   while (i-- > 0) {*op++ =(float)WideIntToDouble(ip,size,is_signed); ip += size;} status = MDSplusSUCCESS; CONVERT_FLOAT(pb,numb,otype)}

#define LBINARY_TO_DOUBLE(pa,pb,numb,size,is_signed,otype)  \
  {int i=numb; unsigned int *ip=(unsigned int*)pa; double *op=(double*)pb;\
   while (i-- > 0) {*op++ =(double)WideIntToDouble(ip,size,is_signed); ip += size;} status = MDSplusSUCCESS; CONVERT_DOUBLE(pb,numb,otype)}

#define LBINARY_TO_FLOATC(pa,pb,numb,size,is_signed,otype)  \
  {int i=numb; unsigned int *ip=(unsigned int*)pa; float *op=(float*)pb;\
   while (i-- > 0) {*op++ =(float)WideIntToDouble(ip,size,is_signed); *op++ = (float)0.0; ip += size;} status = MDSplusSUCCESS; \
   CONVERT_FLOAT(pb,numb*2,otype)}

#define LBINARY_TO_DOUBLEC(pa,pb,numb,size,is_signed,otype)  \
  {int i=numb; unsigned int *ip=(unsigned int*)pa; double *op=(double*)pb;\
   while (i-- > 0) {*op++ =(double)WideIntToDouble(ip,size,is_signed); *op++ = (double)0.0; ip += size;} status = MDSplusSUCCESS; \
   CONVERT_DOUBLE(pb,numb*2,otype)}

#define QU_F(lena,pa,lenb,pb,numb) LBINARY_TO_FLOAT(pa,pb,numb,2,0,DTYPE_F)
#define QU_FS(lena,pa,lenb,pb,numb) LBINARY_TO_FLOAT(pa,pb,numb,2,0,DTYPE_FS)
#define QU_D(lena,pa,lenb,pb,numb) LBINARY_TO_DOUBLE(pa,pb,numb,2,0,DTYPE_D)
#define QU_G(lena,pa,lenb,pb,numb) LBINARY_TO_DOUBLE(pa,pb,numb,2,0,DTYPE_G)
#define QU_FT(lena,pa,lenb,pb,numb) LBINARY_TO_DOUBLE(pa,pb,numb,2,0,DTYPE_FT)
#define QU_FC(lena,pa,lenb,pb,numb) LBINARY_TO_FLOATC(pa,pb,numb,2,0,DTYPE_F)
#define QU_FSC(lena,pa,lenb,pb,numb) LBINARY_TO_FLOATC(pa,pb,numb,2,0,DTYPE_FS)
#define QU_DC(lena,pa,lenb,pb,numb) LBINARY_TO_DOUBLEC(pa,pb,numb,2,0,DTYPE_D)
#define QU_GC(lena,pa,lenb,pb,numb) LBINARY_TO_DOUBLEC(pa,pb,numb,2,0,DTYPE_G)
#define QU_FTC(lena,pa,lenb,pb,numb) LBINARY_TO_DOUBLEC(pa,pb,numb,2,0,DTYPE_FT)
#define OU_F(lena,pa,lenb,pb,numb) LBINARY_TO_FLOAT(pa,pb,numb,4,0,DTYPE_F)
#define OU_FS(lena,pa,lenb,pb,numb) LBINARY_TO_FLOAT(pa,pb,numb,4,0,DTYPE_FS)
#define OU_D(lena,pa,lenb,pb,numb) LBINARY_TO_DOUBLE(pa,pb,numb,4,0,DTYPE_D)
#define OU_G(lena,pa,lenb,pb,numb) LBINARY_TO_DOUBLE(pa,pb,numb,4,0,DTYPE_G)
#define OU_FT(lena,pa,lenb,pb,numb) LBINARY_TO_DOUBLE(pa,pb,numb,4,0,DTYPE_FT)
#define OU_FC(lena,pa,lenb,pb,numb) LBINARY_TO_FLOATC(pa,pb,numb,4,0,DTYPE_F)
#define OU_FSC(lena,pa,lenb,pb,numb) LBINARY_TO_FLOATC(pa,pb,numb,4,0,DTYPE_FS)
#define OU_DC(lena,pa,lenb,pb,numb) LBINARY_TO_DOUBLEC(pa,pb,numb,4,0,DTYPE_D)
#define OU_GC(lena,pa,lenb,pb,numb) LBINARY_TO_DOUBLEC(pa,pb,numb,4,0,DTYPE_G)
#define OU_FTC(lena,pa,lenb,pb,numb) LBINARY_TO_DOUBLEC(pa,pb,numb,4,0,DTYPE_FT)
#define Q_F(lena,pa,lenb,pb,numb) LBINARY_TO_FLOAT(pa,pb,numb,2,1,DTYPE_F)
#define Q_FS(lena,pa,lenb,pb,numb) LBINARY_TO_FLOAT(pa,pb,numb,2,1,DTYPE_FS)
#define Q_D(lena,pa,lenb,pb,numb) LBINARY_TO_DOUBLE(pa,pb,numb,2,1,DTYPE_D)
#define Q_G(lena,pa,lenb,pb,numb) LBINARY_TO_DOUBLE(pa,pb,numb,2,1,DTYPE_G)
#define Q_FT(lena,pa,lenb,pb,numb) LBINARY_TO_DOUBLE(pa,pb,numb,2,1,DTYPE_FT)
#define Q_FC(lena,pa,lenb,pb,numb) LBINARY_TO_FLOATC(pa,pb,numb,2,1,DTYPE_F)
#define Q_FSC(lena,pa,lenb,pb,numb) LBINARY_TO_FLOATC(pa,pb,numb,2,1,DTYPE_FS)
#define Q_DC(lena,pa,lenb,pb,numb) LBINARY_TO_DOUBLEC(pa,pb,numb,2,1,DTYPE_D)
#define Q_GC(lena,pa,lenb,pb,numb) LBINARY_TO_DOUBLEC(pa,pb,numb,2,1,DTYPE_G)
#define Q_FTC(lena,pa,lenb,pb,numb) LBINARY_TO_DOUBLEC(pa,pb,numb,2,1,DTYPE_FT)
#define O_F(lena,pa,lenb,pb,numb) LBINARY_TO_FLOAT(pa,pb,numb,4,1,DTYPE_F)
#define O_FS(lena,pa,lenb,pb,numb) LBINARY_TO_FLOAT(pa,pb,numb,4,1,DTYPE_FS)
#define O_D(lena,pa,lenb,pb,numb) LBINARY_TO_DOUBLE(pa,pb,numb,4,1,DTYPE_D)
#define O_G(lena,pa,lenb,pb,numb) LBINARY_TO_DOUBLE(pa,pb,numb,4,1,DTYPE_G)
#define O_FT(lena,pa,lenb,pb,numb) LBINARY_TO_DOUBLE(pa,pb,numb,4,1,DTYPE_FT)
#define O_FC(lena,pa,lenb,pb,numb) LBINARY_TO_FLOATC(pa,pb,numb,4,1,DTYPE_F)
#define O_FSC(lena,pa,lenb,pb,numb) LBINARY_TO_FLOATC(pa,pb,numb,4,1,DTYPE_FS)
#define O_DC(lena,pa,lenb,pb,numb) LBINARY_TO_DOUBLEC(pa,pb,numb,4,1,DTYPE_D)
#define O_GC(lena,pa,lenb,pb,numb) LBINARY_TO_DOUBLEC(pa,pb,numb,4,1,DTYPE_G)
#define O_FTC(lena,pa,lenb,pb,numb) LBINARY_TO_DOUBLEC(pa,pb,numb,4,1,DTYPE_FT)
/*************** Float to Binary ****************************************/
#define FLOAT_TO_BINARY(itype,pa,pb,numb,to,mino,maxo)  \
  {int i=numb; float *ip=(float*)pa; to *op=(to*)pb; \
   while (i-- > 0) { float tmp; \
     if (itype != DTYPE_NATIVE_FLOAT) CvtConvertFloat(ip++, itype, &tmp, DTYPE_NATIVE_FLOAT,0); else tmp = *ip++; \
     *op++ = (to)max((float)((to)mino),min((float)((to)maxo),tmp));} status = MDSplusSUCCESS;}

#define DOUBLE_TO_BINARY(itype,pa,pb,numb,to,mino,maxo)  \
  {int i=numb; double *ip=(double*)pa; to *op=(to*)pb; \
   while (i-- > 0) { double tmp; \
     if (itype != DTYPE_NATIVE_DOUBLE) CvtConvertFloat(ip++, itype, &tmp, DTYPE_NATIVE_DOUBLE,0); else tmp = *ip++; \
     *op++ = (to)max((double)((to)mino),min((double)((to)maxo),tmp));} status = MDSplusSUCCESS;}

#define FLOATC_TO_BINARY(itype,pa,pb,numb,to,mino,maxo)  \
  {int i=numb; float *ip=(float*)pa; to *op=(to*)pb; \
   while (i-- > 0) { float tmp; \
     if (itype != DTYPE_NATIVE_FLOAT) CvtConvertFloat(ip++, itype, &tmp, DTYPE_NATIVE_FLOAT,0); else tmp = *ip++; \
     *op++ = (to)max((float)((to)mino),min((float)((to)maxo),tmp)); ip++;} status = MDSplusSUCCESS;}

#define DOUBLEC_TO_BINARY(itype,pa,pb,numb,to,mino,maxo)  \
  {int i=numb; double *ip=(double*)pa; to *op=(to*)pb; \
   while (i-- > 0) { double tmp; \
     if (itype != DTYPE_NATIVE_DOUBLE) CvtConvertFloat(ip++, itype, &tmp, DTYPE_NATIVE_DOUBLE,0); else tmp = *ip++; \
     *op++ = (to)max((double)((to)mino),min((double)((to)maxo),tmp)); ip++;} status = MDSplusSUCCESS;}

#define F_BU(lena,pa,lenb,pb,numb) FLOAT_TO_BINARY(DTYPE_F,pa,pb,numb,unsigned char,0,0xff)
#define F_WU(lena,pa,lenb,pb,numb) FLOAT_TO_BINARY(DTYPE_F,pa,pb,numb,unsigned short,0,0xffff)
#define F_LU(lena,pa,lenb,pb,numb) FLOAT_TO_BINARY(DTYPE_F,pa,pb,numb,unsigned int,0,0xffffffff)
#define F_B(lena,pa,lenb,pb,numb)  FLOAT_TO_BINARY(DTYPE_F,pa,pb,numb,char,0x80,0x7f)
#define F_W(lena,pa,lenb,pb,numb)  FLOAT_TO_BINARY(DTYPE_F,pa,pb,numb,short,0x8000,0x7fff)
#define F_L(lena,pa,lenb,pb,numb)  FLOAT_TO_BINARY(DTYPE_F,pa,pb,numb,int,0x80000000,0x7fffffff)
#define FS_BU(lena,pa,lenb,pb,numb) FLOAT_TO_BINARY(DTYPE_FS,pa,pb,numb,unsigned char,0,0xff)
#define FS_WU(lena,pa,lenb,pb,numb) FLOAT_TO_BINARY(DTYPE_FS,pa,pb,numb,unsigned short,0,0xffff)
#define FS_LU(lena,pa,lenb,pb,numb) FLOAT_TO_BINARY(DTYPE_FS,pa,pb,numb,unsigned int,0,0xffffffff)
#define FS_B(lena,pa,lenb,pb,numb)  FLOAT_TO_BINARY(DTYPE_FS,pa,pb,numb,char,0x80,0x7f)
#define FS_W(lena,pa,lenb,pb,numb)  FLOAT_TO_BINARY(DTYPE_FS,pa,pb,numb,short,0x8000,0x7fff)
#define FS_L(lena,pa,lenb,pb,numb)  FLOAT_TO_BINARY(DTYPE_FS,pa,pb,numb,int,0x80000000,0x7fffffff)
#define D_BU(lena,pa,lenb,pb,numb) DOUBLE_TO_BINARY(DTYPE_D,pa,pb,numb,unsigned char,0,0xff)
#define D_WU(lena,pa,lenb,pb,numb) DOUBLE_TO_BINARY(DTYPE_D,pa,pb,numb,unsigned short,0,0xffff)
#define D_LU(lena,pa,lenb,pb,numb) DOUBLE_TO_BINARY(DTYPE_D,pa,pb,numb,unsigned int,0,0xffffffff)
#define D_B(lena,pa,lenb,pb,numb)  DOUBLE_TO_BINARY(DTYPE_D,pa,pb,numb,char,0x80,0x7f)
#define D_W(lena,pa,lenb,pb,numb)  DOUBLE_TO_BINARY(DTYPE_D,pa,pb,numb,short,0x8000,0x7fff)
#define D_L(lena,pa,lenb,pb,numb)  DOUBLE_TO_BINARY(DTYPE_D,pa,pb,numb,int,0x80000000,0x7fffffff)
#define G_BU(lena,pa,lenb,pb,numb) DOUBLE_TO_BINARY(DTYPE_G,pa,pb,numb,unsigned char,0,0xff)
#define G_WU(lena,pa,lenb,pb,numb) DOUBLE_TO_BINARY(DTYPE_G,pa,pb,numb,unsigned short,0,0xffff)
#define G_LU(lena,pa,lenb,pb,numb) DOUBLE_TO_BINARY(DTYPE_G,pa,pb,numb,unsigned int,0,0xffffffff)
#define G_B(lena,pa,lenb,pb,numb)  DOUBLE_TO_BINARY(DTYPE_G,pa,pb,numb,char,0x80,0x7f)
#define G_W(lena,pa,lenb,pb,numb)  DOUBLE_TO_BINARY(DTYPE_G,pa,pb,numb,short,0x8000,0x7fff)
#define G_L(lena,pa,lenb,pb,numb)  DOUBLE_TO_BINARY(DTYPE_G,pa,pb,numb,int,0x80000000,0x7fffffff)
#define FT_BU(lena,pa,lenb,pb,numb) DOUBLE_TO_BINARY(DTYPE_FT,pa,pb,numb,unsigned char,0,0xff)
#define FT_WU(lena,pa,lenb,pb,numb) DOUBLE_TO_BINARY(DTYPE_FT,pa,pb,numb,unsigned short,0,0xffff)
#define FT_LU(lena,pa,lenb,pb,numb) DOUBLE_TO_BINARY(DTYPE_FT,pa,pb,numb,unsigned int,0,0xffffffff)
#define FT_B(lena,pa,lenb,pb,numb)  DOUBLE_TO_BINARY(DTYPE_FT,pa,pb,numb,char,0x80,0x7f)
#define FT_W(lena,pa,lenb,pb,numb)  DOUBLE_TO_BINARY(DTYPE_FT,pa,pb,numb,short,0x8000,0x7fff)
#define FT_L(lena,pa,lenb,pb,numb)  DOUBLE_TO_BINARY(DTYPE_FT,pa,pb,numb,int,0x80000000,0x7fffffff)
#define FC_BU(lena,pa,lenb,pb,numb) FLOATC_TO_BINARY(DTYPE_F,pa,pb,numb,unsigned char,0,0xff)
#define FC_WU(lena,pa,lenb,pb,numb) FLOATC_TO_BINARY(DTYPE_F,pa,pb,numb,unsigned short,0,0xffff)
#define FC_LU(lena,pa,lenb,pb,numb) FLOATC_TO_BINARY(DTYPE_F,pa,pb,numb,unsigned int,0,0xffffffff)
#define FC_B(lena,pa,lenb,pb,numb)  FLOATC_TO_BINARY(DTYPE_F,pa,pb,numb,char,0x80,0x7f)
#define FC_W(lena,pa,lenb,pb,numb)  FLOATC_TO_BINARY(DTYPE_F,pa,pb,numb,short,0x8000,0x7fff)
#define FC_L(lena,pa,lenb,pb,numb)  FLOATC_TO_BINARY(DTYPE_F,pa,pb,numb,int,0x80000000,0x7fffffff)
#define FSC_BU(lena,pa,lenb,pb,numb) FLOATC_TO_BINARY(DTYPE_FS,pa,pb,numb,unsigned char,0,0xff)
#define FSC_WU(lena,pa,lenb,pb,numb) FLOATC_TO_BINARY(DTYPE_FS,pa,pb,numb,unsigned short,0,0xffff)
#define FSC_LU(lena,pa,lenb,pb,numb) FLOATC_TO_BINARY(DTYPE_FS,pa,pb,numb,unsigned int,0,0xffffffff)
#define FSC_B(lena,pa,lenb,pb,numb)  FLOATC_TO_BINARY(DTYPE_FS,pa,pb,numb,char,0x80,0x7f)
#define FSC_W(lena,pa,lenb,pb,numb)  FLOATC_TO_BINARY(DTYPE_FS,pa,pb,numb,short,0x8000,0x7fff)
#define FSC_L(lena,pa,lenb,pb,numb)  FLOATC_TO_BINARY(DTYPE_FS,pa,pb,numb,int,0x80000000,0x7fffffff)
#define DC_BU(lena,pa,lenb,pb,numb) DOUBLEC_TO_BINARY(DTYPE_D,pa,pb,numb,unsigned char,0,0xff)
#define DC_WU(lena,pa,lenb,pb,numb) DOUBLEC_TO_BINARY(DTYPE_D,pa,pb,numb,unsigned short,0,0xffff)
#define DC_LU(lena,pa,lenb,pb,numb) DOUBLEC_TO_BINARY(DTYPE_D,pa,pb,numb,unsigned int,0,0xffffffff)
#define DC_B(lena,pa,lenb,pb,numb)  DOUBLEC_TO_BINARY(DTYPE_D,pa,pb,numb,char,0x80,0x7f)
#define DC_W(lena,pa,lenb,pb,numb)  DOUBLEC_TO_BINARY(DTYPE_D,pa,pb,numb,short,0x8000,0x7fff)
#define DC_L(lena,pa,lenb,pb,numb)  DOUBLEC_TO_BINARY(DTYPE_D,pa,pb,numb,int,0x80000000,0x7fffffff)
#define GC_BU(lena,pa,lenb,pb,numb) DOUBLEC_TO_BINARY(DTYPE_G,pa,pb,numb,unsigned char,0,0xff)
#define GC_WU(lena,pa,lenb,pb,numb) DOUBLEC_TO_BINARY(DTYPE_G,pa,pb,numb,unsigned short,0,0xffff)
#define GC_LU(lena,pa,lenb,pb,numb) DOUBLEC_TO_BINARY(DTYPE_G,pa,pb,numb,unsigned int,0,0xffffffff)
#define GC_B(lena,pa,lenb,pb,numb)  DOUBLEC_TO_BINARY(DTYPE_G,pa,pb,numb,char,0x80,0x7f)
#define GC_W(lena,pa,lenb,pb,numb)  DOUBLEC_TO_BINARY(DTYPE_G,pa,pb,numb,short,0x8000,0x7fff)
#define GC_L(lena,pa,lenb,pb,numb)  DOUBLEC_TO_BINARY(DTYPE_G,pa,pb,numb,int,0x80000000,0x7fffffff)
#define FTC_BU(lena,pa,lenb,pb,numb) DOUBLEC_TO_BINARY(DTYPE_FT,pa,pb,numb,unsigned char,0,0xff)
#define FTC_WU(lena,pa,lenb,pb,numb) DOUBLEC_TO_BINARY(DTYPE_FT,pa,pb,numb,unsigned short,0,0xffff)
#define FTC_LU(lena,pa,lenb,pb,numb) DOUBLEC_TO_BINARY(DTYPE_FT,pa,pb,numb,unsigned int,0,0xffffffff)
#define FTC_B(lena,pa,lenb,pb,numb)  DOUBLEC_TO_BINARY(DTYPE_FT,pa,pb,numb,char,0x80,0x7f)
#define FTC_W(lena,pa,lenb,pb,numb)  DOUBLEC_TO_BINARY(DTYPE_FT,pa,pb,numb,short,0x8000,0x7fff)
#define FTC_L(lena,pa,lenb,pb,numb)  DOUBLEC_TO_BINARY(DTYPE_FT,pa,pb,numb,int,0x80000000,0x7fffffff)

/*********** Float to Long Binary ********************/
#define FLOAT_TO_LBINARY(itype,pa,pb,numb,size)  \
  {int i=numb; float *ip=(float*)pa; unsigned int *op=(unsigned int *)pb; \
   while (i-- > 0) { double tmp; \
     if (itype != DTYPE_NATIVE_FLOAT) CvtConvertFloat(ip++, itype, &tmp, DTYPE_NATIVE_DOUBLE,0); else tmp = (double)*ip++; \
     DoubleToWideInt(&tmp, size, op); op += size;} status = MDSplusSUCCESS;}

#define DOUBLE_TO_LBINARY(itype,pa,pb,numb,size)  \
  {int i=numb; double *ip=(double*)pa; unsigned int *op=(unsigned int *)pb; \
   while (i-- > 0) { double tmp; \
     if (itype != DTYPE_NATIVE_DOUBLE) CvtConvertFloat(ip++, itype, &tmp, DTYPE_NATIVE_DOUBLE,0); else tmp = *ip++; \
     DoubleToWideInt(&tmp, size, op); op += size;} status = MDSplusSUCCESS;}

#define FLOATC_TO_LBINARY(itype,pa,pb,numb,size)  \
  {int i=numb; float *ip=(float*)pa; unsigned int *op=(unsigned int *)pb; \
   while (i-- > 0) { double tmp; \
     if (itype != DTYPE_NATIVE_FLOAT) CvtConvertFloat(ip++, itype, &tmp, DTYPE_NATIVE_DOUBLE,0); else tmp = (double)*ip++; \
     DoubleToWideInt(&tmp, size, op); op += size; ip++;} status = MDSplusSUCCESS;}

#define DOUBLEC_TO_LBINARY(itype,pa,pb,numb,size)  \
  {int i=numb; double *ip=(double*)pa; unsigned int *op=(unsigned int *)pb; \
   while (i-- > 0) { double tmp; \
     if (itype != DTYPE_NATIVE_DOUBLE) CvtConvertFloat(ip++, itype, &tmp, DTYPE_NATIVE_DOUBLE,0); else tmp = *ip++; \
     DoubleToWideInt(&tmp, size, op); op += size; ip++;} status = MDSplusSUCCESS;}

#define F_QU(lena,pa,lenb,pb,numb)  FLOAT_TO_LBINARY(DTYPE_F,pa,pb,numb,2)
#define F_OU(lena,pa,lenb,pb,numb)  FLOAT_TO_LBINARY(DTYPE_F,pa,pb,numb,4)
#define F_Q(lena,pa,lenb,pb,numb)   FLOAT_TO_LBINARY(DTYPE_F,pa,pb,numb,2)
#define F_O(lena,pa,lenb,pb,numb)   FLOAT_TO_LBINARY(DTYPE_F,pa,pb,numb,4)
#define FS_QU(lena,pa,lenb,pb,numb) FLOAT_TO_LBINARY(DTYPE_FS,pa,pb,numb,2)
#define FS_OU(lena,pa,lenb,pb,numb) FLOAT_TO_LBINARY(DTYPE_FS,pa,pb,numb,4)
#define FS_Q(lena,pa,lenb,pb,numb)  FLOAT_TO_LBINARY(DTYPE_FS,pa,pb,numb,2)
#define FS_O(lena,pa,lenb,pb,numb)  FLOAT_TO_LBINARY(DTYPE_FS,pa,pb,numb,4)
#define D_QU(lena,pa,lenb,pb,numb)  DOUBLE_TO_LBINARY(DTYPE_D,pa,pb,numb,2)
#define D_OU(lena,pa,lenb,pb,numb)  DOUBLE_TO_LBINARY(DTYPE_D,pa,pb,numb,4)
#define D_Q(lena,pa,lenb,pb,numb)   DOUBLE_TO_LBINARY(DTYPE_D,pa,pb,numb,2)
#define D_O(lena,pa,lenb,pb,numb)   DOUBLE_TO_LBINARY(DTYPE_D,pa,pb,numb,4)
#define G_QU(lena,pa,lenb,pb,numb)  DOUBLE_TO_LBINARY(DTYPE_G,pa,pb,numb,2)
#define G_OU(lena,pa,lenb,pb,numb)  DOUBLE_TO_LBINARY(DTYPE_G,pa,pb,numb,4)
#define G_Q(lena,pa,lenb,pb,numb)   DOUBLE_TO_LBINARY(DTYPE_G,pa,pb,numb,2)
#define G_O(lena,pa,lenb,pb,numb)   DOUBLE_TO_LBINARY(DTYPE_G,pa,pb,numb,4)
#define FT_QU(lena,pa,lenb,pb,numb) DOUBLE_TO_LBINARY(DTYPE_FT,pa,pb,numb,2)
#define FT_OU(lena,pa,lenb,pb,numb) DOUBLE_TO_LBINARY(DTYPE_FT,pa,pb,numb,4)
#define FT_Q(lena,pa,lenb,pb,numb)  DOUBLE_TO_LBINARY(DTYPE_FT,pa,pb,numb,2)
#define FT_O(lena,pa,lenb,pb,numb)  DOUBLE_TO_LBINARY(DTYPE_FT,pa,pb,numb,4)
#define FC_QU(lena,pa,lenb,pb,numb)  FLOATC_TO_LBINARY(DTYPE_F,pa,pb,numb,2)
#define FC_OU(lena,pa,lenb,pb,numb)  FLOATC_TO_LBINARY(DTYPE_F,pa,pb,numb,4)
#define FC_Q(lena,pa,lenb,pb,numb)   FLOATC_TO_LBINARY(DTYPE_F,pa,pb,numb,2)
#define FC_O(lena,pa,lenb,pb,numb)   FLOATC_TO_LBINARY(DTYPE_F,pa,pb,numb,4)
#define FSC_QU(lena,pa,lenb,pb,numb) FLOATC_TO_LBINARY(DTYPE_FS,pa,pb,numb,2)
#define FSC_OU(lena,pa,lenb,pb,numb) FLOATC_TO_LBINARY(DTYPE_FS,pa,pb,numb,4)
#define FSC_Q(lena,pa,lenb,pb,numb)  FLOATC_TO_LBINARY(DTYPE_FS,pa,pb,numb,2)
#define FSC_O(lena,pa,lenb,pb,numb)  FLOATC_TO_LBINARY(DTYPE_FS,pa,pb,numb,4)
#define DC_QU(lena,pa,lenb,pb,numb)  DOUBLEC_TO_LBINARY(DTYPE_D,pa,pb,numb,2)
#define DC_OU(lena,pa,lenb,pb,numb)  DOUBLEC_TO_LBINARY(DTYPE_D,pa,pb,numb,4)
#define DC_Q(lena,pa,lenb,pb,numb)   DOUBLEC_TO_LBINARY(DTYPE_D,pa,pb,numb,2)
#define DC_O(lena,pa,lenb,pb,numb)   DOUBLEC_TO_LBINARY(DTYPE_D,pa,pb,numb,4)
#define GC_QU(lena,pa,lenb,pb,numb)  DOUBLEC_TO_LBINARY(DTYPE_G,pa,pb,numb,2)
#define GC_OU(lena,pa,lenb,pb,numb)  DOUBLEC_TO_LBINARY(DTYPE_G,pa,pb,numb,4)
#define GC_Q(lena,pa,lenb,pb,numb)   DOUBLEC_TO_LBINARY(DTYPE_G,pa,pb,numb,2)
#define GC_O(lena,pa,lenb,pb,numb)   DOUBLEC_TO_LBINARY(DTYPE_G,pa,pb,numb,4)
#define FTC_QU(lena,pa,lenb,pb,numb) DOUBLEC_TO_LBINARY(DTYPE_FT,pa,pb,numb,2)
#define FTC_OU(lena,pa,lenb,pb,numb) DOUBLEC_TO_LBINARY(DTYPE_FT,pa,pb,numb,4)
#define FTC_Q(lena,pa,lenb,pb,numb)  DOUBLEC_TO_LBINARY(DTYPE_FT,pa,pb,numb,2)
#define FTC_O(lena,pa,lenb,pb,numb)  DOUBLEC_TO_LBINARY(DTYPE_FT,pa,pb,numb,4)

/************ Float to Float *************************/
typedef union {
  float f;
  char i;
} binary_float;
typedef union {
  double f;
  char i;
} binary_double;

#define FLOAT_TO_FLOAT(itype,it,pa,otype,ot,pb,numb)  \
  {int i=numb; \
   binary_##it *ip=(binary_##it*)pa; \
   binary_##ot *op=(binary_##ot*)pb; \
   while (i-- > 0) { \
     binary_##it tmp = *ip++; \
     CvtConvertFloat(&tmp, itype, op++, otype,0);} status = MDSplusSUCCESS;}

#define FLOAT_TO_COMPLEX(itype,it,pa,otype,ot,pb,numb)  \
  {int i=numb; binary_##it *ip=(binary_##it*)pa; binary_##ot *op=(binary_##ot*)pb; \
   binary_##ot zero = {0.0};\
   while (i-- > 0) { if (itype == otype) *op++ = *(binary_##ot *)ip++; else { \
          binary_##it tmp = *ip++; CvtConvertFloat(&tmp, itype, op++, otype,0);} \
   *op++ = zero;\
   } status = MDSplusSUCCESS;}

#define COMPLEX_TO_FLOAT(itype,it,pa,otype,ot,pb,numb)  \
  {int i=numb; binary_##it *ip=(binary_##it*)pa; binary_##ot *op=(binary_##ot*)pb; \
   while (i-- > 0) { if (itype == otype) *op++ = *(binary_##ot *)ip++; else { \
          binary_##it tmp = *ip++; CvtConvertFloat(&tmp, itype, op++, otype,0);} ip++;} status = MDSplusSUCCESS;}

#define COMPLEX_TO_COMPLEX(itype,it,pa,otype,ot,pb,numb) FLOAT_TO_FLOAT(itype,it,pa,otype,ot,pb,numb*2)

#define F_FS(lena,pa,lenb,pb,numb)   FLOAT_TO_FLOAT(DTYPE_F,float,pa,DTYPE_FS,float,pb,numb)
#define F_D(lena,pa,lenb,pb,numb)    FLOAT_TO_FLOAT(DTYPE_F,float,pa,DTYPE_D,double,pb,numb)
#define F_G(lena,pa,lenb,pb,numb)    FLOAT_TO_FLOAT(DTYPE_F,float,pa,DTYPE_G,double,pb,numb)
#define F_FT(lena,pa,lenb,pb,numb)   FLOAT_TO_FLOAT(DTYPE_F,float,pa,DTYPE_FT,double,pb,numb)
#define F_FC(lena,pa,lenb,pb,numb)   FLOAT_TO_COMPLEX(DTYPE_F,float,pa,DTYPE_F,float,pb,numb)
#define F_FSC(lena,pa,lenb,pb,numb)  FLOAT_TO_COMPLEX(DTYPE_F,float,pa,DTYPE_FS,float,pb,numb)
#define F_DC(lena,pa,lenb,pb,numb)   FLOAT_TO_COMPLEX(DTYPE_F,float,pa,DTYPE_D,double,pb,numb)
#define F_GC(lena,pa,lenb,pb,numb)   FLOAT_TO_COMPLEX(DTYPE_F,float,pa,DTYPE_G,double,pb,numb)
#define F_FTC(lena,pa,lenb,pb,numb)  FLOAT_TO_COMPLEX(DTYPE_F,float,pa,DTYPE_FT,double,pb,numb)
#define FS_F(lena,pa,lenb,pb,numb)   FLOAT_TO_FLOAT(DTYPE_FS,float,pa,DTYPE_F,float,pb,numb)
#define FS_D(lena,pa,lenb,pb,numb)   FLOAT_TO_FLOAT(DTYPE_FS,float,pa,DTYPE_D,double,pb,numb)
#define FS_G(lena,pa,lenb,pb,numb)   FLOAT_TO_FLOAT(DTYPE_FS,float,pa,DTYPE_G,double,pb,numb)
#define FS_FT(lena,pa,lenb,pb,numb)  FLOAT_TO_FLOAT(DTYPE_FS,float,pa,DTYPE_FT,double,pb,numb)
#define FS_FC(lena,pa,lenb,pb,numb)  FLOAT_TO_COMPLEX(DTYPE_FS,float,pa,DTYPE_F,float,pb,numb)
#define FS_FSC(lena,pa,lenb,pb,numb) FLOAT_TO_COMPLEX(DTYPE_FS,float,pa,DTYPE_FS,float,pb,numb)
#define FS_DC(lena,pa,lenb,pb,numb)  FLOAT_TO_COMPLEX(DTYPE_FS,float,pa,DTYPE_D,double,pb,numb)
#define FS_GC(lena,pa,lenb,pb,numb)  FLOAT_TO_COMPLEX(DTYPE_FS,float,pa,DTYPE_G,double,pb,numb)
#define FS_FTC(lena,pa,lenb,pb,numb) FLOAT_TO_COMPLEX(DTYPE_FS,float,pa,DTYPE_FT,double,pb,numb)
#define D_F(lena,pa,lenb,pb,numb)    FLOAT_TO_FLOAT(DTYPE_D,double,pa,DTYPE_F,float,pb,numb)
#define D_FS(lena,pa,lenb,pb,numb)   FLOAT_TO_FLOAT(DTYPE_D,double,pa,DTYPE_FS,float,pb,numb)
#define D_G(lena,pa,lenb,pb,numb)    FLOAT_TO_FLOAT(DTYPE_D,double,pa,DTYPE_G,double,pb,numb)
#define D_FT(lena,pa,lenb,pb,numb)   FLOAT_TO_FLOAT(DTYPE_D,double,pa,DTYPE_FT,double,pb,numb)
#define D_FC(lena,pa,lenb,pb,numb)   FLOAT_TO_COMPLEX(DTYPE_D,double,pa,DTYPE_F,float,pb,numb)
#define D_FSC(lena,pa,lenb,pb,numb)  FLOAT_TO_COMPLEX(DTYPE_D,double,pa,DTYPE_FS,float,pb,numb)
#define D_DC(lena,pa,lenb,pb,numb)   FLOAT_TO_COMPLEX(DTYPE_D,double,pa,DTYPE_D,double,pb,numb)
#define D_GC(lena,pa,lenb,pb,numb)   FLOAT_TO_COMPLEX(DTYPE_D,double,pa,DTYPE_G,double,pb,numb)
#define D_FTC(lena,pa,lenb,pb,numb)  FLOAT_TO_COMPLEX(DTYPE_D,double,pa,DTYPE_FT,double,pb,numb)
#define G_F(lena,pa,lenb,pb,numb)    FLOAT_TO_FLOAT(DTYPE_G,double,pa,DTYPE_F,float,pb,numb)
#define G_FS(lena,pa,lenb,pb,numb)   FLOAT_TO_FLOAT(DTYPE_G,double,pa,DTYPE_FS,float,pb,numb)
#define G_D(lena,pa,lenb,pb,numb)    FLOAT_TO_FLOAT(DTYPE_G,double,pa,DTYPE_D,double,pb,numb)
#define G_FT(lena,pa,lenb,pb,numb)   FLOAT_TO_FLOAT(DTYPE_G,double,pa,DTYPE_FT,double,pb,numb)
#define G_FC(lena,pa,lenb,pb,numb)   FLOAT_TO_COMPLEX(DTYPE_G,double,pa,DTYPE_F,float,pb,numb)
#define G_FSC(lena,pa,lenb,pb,numb)  FLOAT_TO_COMPLEX(DTYPE_G,double,pa,DTYPE_FS,float,pb,numb)
#define G_DC(lena,pa,lenb,pb,numb)   FLOAT_TO_COMPLEX(DTYPE_G,double,pa,DTYPE_D,double,pb,numb)
#define G_GC(lena,pa,lenb,pb,numb)   FLOAT_TO_COMPLEX(DTYPE_G,double,pa,DTYPE_G,double,pb,numb)
#define G_FTC(lena,pa,lenb,pb,numb)  FLOAT_TO_COMPLEX(DTYPE_G,double,pa,DTYPE_FT,double,pb,numb)
#define FT_F(lena,pa,lenb,pb,numb)   FLOAT_TO_FLOAT(DTYPE_FT,double,pa,DTYPE_F,float,pb,numb)
#define FT_FS(lena,pa,lenb,pb,numb)  FLOAT_TO_FLOAT(DTYPE_FT,double,pa,DTYPE_FS,float,pb,numb)
#define FT_D(lena,pa,lenb,pb,numb)   FLOAT_TO_FLOAT(DTYPE_FT,double,pa,DTYPE_D,double,pb,numb)
#define FT_G(lena,pa,lenb,pb,numb)   FLOAT_TO_FLOAT(DTYPE_FT,double,pa,DTYPE_G,double,pb,numb)
#define FT_FC(lena,pa,lenb,pb,numb)  FLOAT_TO_COMPLEX(DTYPE_FT,double,pa,DTYPE_F,float,pb,numb)
#define FT_FSC(lena,pa,lenb,pb,numb) FLOAT_TO_COMPLEX(DTYPE_FT,double,pa,DTYPE_FS,float,pb,numb)
#define FT_DC(lena,pa,lenb,pb,numb)  FLOAT_TO_COMPLEX(DTYPE_FT,double,pa,DTYPE_D,double,pb,numb)
#define FT_GC(lena,pa,lenb,pb,numb)  FLOAT_TO_COMPLEX(DTYPE_FT,double,pa,DTYPE_G,double,pb,numb)
#define FT_FTC(lena,pa,lenb,pb,numb) FLOAT_TO_COMPLEX(DTYPE_FT,double,pa,DTYPE_FT,double,pb,numb)
#define FC_F(lena,pa,lenb,pb,numb)   COMPLEX_TO_FLOAT(DTYPE_F,float,pa,DTYPE_F,float,pb,numb)
#define FC_FS(lena,pa,lenb,pb,numb)  COMPLEX_TO_FLOAT(DTYPE_F,float,pa,DTYPE_FS,float,pb,numb)
#define FC_D(lena,pa,lenb,pb,numb)   COMPLEX_TO_FLOAT(DTYPE_F,float,pa,DTYPE_D,double,pb,numb)
#define FC_G(lena,pa,lenb,pb,numb)   COMPLEX_TO_FLOAT(DTYPE_F,float,pa,DTYPE_G,double,pb,numb)
#define FC_FT(lena,pa,lenb,pb,numb)  COMPLEX_TO_FLOAT(DTYPE_F,float,pa,DTYPE_FT,double,pb,numb)
#define FC_FSC(lena,pa,lenb,pb,numb) COMPLEX_TO_COMPLEX(DTYPE_F,float,pa,DTYPE_FS,float,pb,numb)
#define FC_DC(lena,pa,lenb,pb,numb)  COMPLEX_TO_COMPLEX(DTYPE_F,float,pa,DTYPE_D,double,pb,numb)
#define FC_GC(lena,pa,lenb,pb,numb)  COMPLEX_TO_COMPLEX(DTYPE_F,float,pa,DTYPE_G,double,pb,numb)
#define FC_FTC(lena,pa,lenb,pb,numb) COMPLEX_TO_COMPLEX(DTYPE_F,float,pa,DTYPE_FT,double,pb,numb)
#define FSC_F(lena,pa,lenb,pb,numb)  COMPLEX_TO_FLOAT(DTYPE_FS,float,pa,DTYPE_F,float,pb,numb)
#define FSC_FS(lena,pa,lenb,pb,numb) COMPLEX_TO_FLOAT(DTYPE_FS,float,pa,DTYPE_FS,float,pb,numb)
#define FSC_D(lena,pa,lenb,pb,numb)  COMPLEX_TO_FLOAT(DTYPE_FS,float,pa,DTYPE_D,double,pb,numb)
#define FSC_G(lena,pa,lenb,pb,numb)  COMPLEX_TO_FLOAT(DTYPE_FS,float,pa,DTYPE_G,float,pb,numb)
#define FSC_FT(lena,pa,lenb,pb,numb) COMPLEX_TO_FLOAT(DTYPE_FS,float,pa,DTYPE_FT,double,pb,numb)
#define FSC_FC(lena,pa,lenb,pb,numb) COMPLEX_TO_COMPLEX(DTYPE_FS,float,pa,DTYPE_F,float,pb,numb)
#define FSC_DC(lena,pa,lenb,pb,numb) COMPLEX_TO_COMPLEX(DTYPE_FS,float,pa,DTYPE_D,double,pb,numb)
#define FSC_GC(lena,pa,lenb,pb,numb) COMPLEX_TO_COMPLEX(DTYPE_FS,float,pa,DTYPE_G,double,pb,numb)
#define FSC_FTC(lena,pa,lenb,pb,numb) COMPLEX_TO_COMPLEX(DTYPE_FS,float,pa,DTYPE_FT,double,pb,numb)
#define DC_F(lena,pa,lenb,pb,numb)   COMPLEX_TO_FLOAT(DTYPE_D,double,pa,DTYPE_F,float,pb,numb)
#define DC_FS(lena,pa,lenb,pb,numb)  COMPLEX_TO_FLOAT(DTYPE_D,double,pa,DTYPE_FS,float,pb,numb)
#define DC_D(lena,pa,lenb,pb,numb)   COMPLEX_TO_FLOAT(DTYPE_D,double,pa,DTYPE_D,double,pb,numb)
#define DC_G(lena,pa,lenb,pb,numb)   COMPLEX_TO_FLOAT(DTYPE_D,double,pa,DTYPE_G,double,pb,numb)
#define DC_FT(lena,pa,lenb,pb,numb)  COMPLEX_TO_FLOAT(DTYPE_D,double,pa,DTYPE_FT,double,pb,numb)
#define DC_FC(lena,pa,lenb,pb,numb)  COMPLEX_TO_COMPLEX(DTYPE_D,double,pa,DTYPE_F,float,pb,numb)
#define DC_FSC(lena,pa,lenb,pb,numb) COMPLEX_TO_COMPLEX(DTYPE_D,double,pa,DTYPE_FS,float,pb,numb)
#define DC_GC(lena,pa,lenb,pb,numb)  COMPLEX_TO_COMPLEX(DTYPE_D,double,pa,DTYPE_G,double,pb,numb)
#define DC_FTC(lena,pa,lenb,pb,numb) COMPLEX_TO_COMPLEX(DTYPE_D,double,pa,DTYPE_FT,double,pb,numb)
#define GC_F(lena,pa,lenb,pb,numb)   COMPLEX_TO_FLOAT(DTYPE_G,double,pa,DTYPE_F,float,pb,numb)
#define GC_FS(lena,pa,lenb,pb,numb)  COMPLEX_TO_FLOAT(DTYPE_G,double,pa,DTYPE_FS,float,pb,numb)
#define GC_D(lena,pa,lenb,pb,numb)   COMPLEX_TO_FLOAT(DTYPE_G,double,pa,DTYPE_D,double,pb,numb)
#define GC_G(lena,pa,lenb,pb,numb)   COMPLEX_TO_FLOAT(DTYPE_G,double,pa,DTYPE_G,double,pb,numb)
#define GC_FT(lena,pa,lenb,pb,numb)  COMPLEX_TO_FLOAT(DTYPE_G,double,pa,DTYPE_FT,double,pb,numb)
#define GC_FC(lena,pa,lenb,pb,numb)  COMPLEX_TO_COMPLEX(DTYPE_G,double,pa,DTYPE_F,float,pb,numb)
#define GC_FSC(lena,pa,lenb,pb,numb) COMPLEX_TO_COMPLEX(DTYPE_G,double,pa,DTYPE_FS,float,pb,numb)
#define GC_DC(lena,pa,lenb,pb,numb)  COMPLEX_TO_COMPLEX(DTYPE_G,double,pa,DTYPE_D,double,pb,numb)
#define GC_FTC(lena,pa,lenb,pb,numb) COMPLEX_TO_COMPLEX(DTYPE_G,double,pa,DTYPE_FT,double,pb,numb)
#define FTC_F(lena,pa,lenb,pb,numb)  COMPLEX_TO_FLOAT(DTYPE_FT,double,pa,DTYPE_F,float,pb,numb)
#define FTC_FS(lena,pa,lenb,pb,numb) COMPLEX_TO_FLOAT(DTYPE_FT,double,pa,DTYPE_FS,float,pb,numb)
#define FTC_D(lena,pa,lenb,pb,numb)  COMPLEX_TO_FLOAT(DTYPE_FT,double,pa,DTYPE_D,double,pb,numb)
#define FTC_G(lena,pa,lenb,pb,numb)  COMPLEX_TO_FLOAT(DTYPE_FT,double,pa,DTYPE_G,double,pb,numb)
#define FTC_FT(lena,pa,lenb,pb,numb) COMPLEX_TO_FLOAT(DTYPE_FT,double,pa,DTYPE_FT,double,pb,numb)
#define FTC_FC(lena,pa,lenb,pb,numb) COMPLEX_TO_COMPLEX(DTYPE_FT,double,pa,DTYPE_F,float,pb,numb)
#define FTC_FSC(lena,pa,lenb,pb,numb) COMPLEX_TO_COMPLEX(DTYPE_FT,double,pa,DTYPE_FS,float,pb,numb)
#define FTC_DC(lena,pa,lenb,pb,numb) COMPLEX_TO_COMPLEX(DTYPE_FT,double,pa,DTYPE_D,double,pb,numb)
#define FTC_GC(lena,pa,lenb,pb,numb) COMPLEX_TO_COMPLEX(DTYPE_FT,double,pa,DTYPE_G,double,pb,numb)
/************* Text *****************/
#define TO_TEXT(pa,it,pb,numb,cvt) \
  {it *ip = (it*)pa; char *op = (char *)pb; int i=numb; while(i-- > 0) {\
   char text[64]; int nfill; int n=cvt;  nfill = lenb - n; \
   strncpy((nfill <= 0) ? op : op+nfill, (nfill <= 0) ? text-nfill : text, (size_t)((nfill <= 0) ? lenb : n)); \
   if (nfill > 0) memset(op,32,(size_t)nfill); op += lenb;} status = MDSplusSUCCESS;}

/******************* CODE *********************/
void DoubleToWideInt();
double WideIntToDouble();

static void mem_shift(char *in, int offset, int num)
{
  int i;
  char *p1 = in, *p2 = in + offset;
  for (i = 0; i < num; i++)
    *p1++ = *p2++;
}

STATIC_ROUTINE void FLOAT_TO_TEXT(int itype, char *pa, char *pb, int numb, int lenb, char sym)
{
  float *ip = (float *)pa;
  char *op = (char *)pb;
  int i = numb;
  while (i-- > 0) {
    char text[64];
    int nfill;
    int n;
    if (IsRoprand(itype, ip)) {
      n = sprintf(text, "$ROPRAND");
      ip++;
    } else {
      char *pe;
      int width;
      int prec;
      double tmp;
      CvtConvertFloat(ip++, itype, &tmp, DTYPE_NATIVE_DOUBLE, 0);
      width = lenb < 13 ? lenb : 13;
      prec = width - 7;
      if (prec < 0)
	prec = 0;
      n = sprintf(text, "%#*.*G", width, prec, tmp);
      pe = strchr(text, 'E');
      if (pe)
	*pe = sym;
      else {
	if (text[0] == ' ')
	  mem_shift(text, 1, n - 1);
	if (text[0] == ' ')
	  mem_shift(text, 1, n - 2);
	text[n - 2] = sym;
	text[n - 1] = '0';
      }
    }
    nfill = lenb - n;
    strncpy((nfill <= 0) ? op : op + nfill,
	    (nfill <= 0) ? text - nfill : text, (nfill <= 0) ? lenb : n);
    if (nfill > 0)
      memset(op, 32, nfill);
    op += lenb;
  }
}

STATIC_ROUTINE void DOUBLE_TO_TEXT(int itype, char *pa, char *pb, int numb, int lenb, char sym)
{
  double *ip = (double *)pa;
  char *op = (char *)pb;
  int i = numb;
  while (i-- > 0) {
    char text[64];
    int nfill;
    int n;
    if (IsRoprand(itype, ip)) {
      n = sprintf(text, "$ROPRAND");
      ip++;
    } else {
      char *pe;
      int width;
      int prec;
      double tmp;
      if (itype != DTYPE_NATIVE_DOUBLE)
	CvtConvertFloat(ip++, itype, &tmp, DTYPE_NATIVE_DOUBLE, 0);
      else
	tmp = *ip++;
      width = lenb < 23 ? lenb : 23;
      prec = width - 7;
      if (prec < 0)
	prec = 0;
      n = sprintf(text, "%#*.*G", width, prec, tmp);
      pe = strchr(text, 'E');
      if (pe)
	*pe = sym;
      else {
	int i;
	if (text[0] == ' ')
	  for (i = 0; i < (n - 1); i++)
	    text[i] = text[i + 1];	/* memcpy(text,text+1,n-1); */
	if (text[0] == ' ')
	  for (i = 0; i < (n - 2); i++)
	    text[i] = text[i + 1];	/* memcpy(text,text+1,n-2); */
	text[n - 2] = sym;
	text[n - 1] = '0';
      }
    }
    nfill = lenb - n;
    strncpy((nfill <= 0) ? op : op + nfill,
	    (nfill <= 0) ? text - nfill : text, (nfill <= 0) ? lenb : n);
    if (nfill > 0)
      memset(op, 32, nfill);
    op += lenb;
  }
}

STATIC_ROUTINE void FLOATC_TO_TEXT(int itype, char *pa, char *pb, int numb, int lenb, char sym)
{
  float *ip = (float *)pa;
  char *op = (char *)pb;
  int i = numb;
  while (i-- > 0) {
    char text[64];
    int nfill;
    int n;
    int part;
    for (part = 0; part < 2; part++) {
      int len = (lenb - 1) / 2;
      char *opl = op;
      if ((len * 2 + 1) != lenb)
	*opl++ = ' ';
      opl[len] = ',';
      if (IsRoprand(itype, ip)) {
	n = sprintf(text, "$ROPRAND");
	ip++;
      } else {
	int width;
	int prec;
	float tmp;
	char *pe;
	if (itype != DTYPE_NATIVE_FLOAT)
	  CvtConvertFloat(ip++, itype, &tmp, DTYPE_NATIVE_FLOAT, 0);
	else
	  tmp = *ip++;
	width = len < 13 ? len : 13;
	prec = width - 7;
	if (prec < 0)
	  prec = 0;
	n = sprintf(text, "%#*.*G", width, prec, tmp);
	pe = strchr(text, 'E');
	if (pe)
	  *pe = sym;
	else {
	  strcpy(text, text + 2);
	  text[n - 2] = sym;
	  text[n - 1] = '0';
	}
      }
      nfill = len - n;
      strncpy((nfill <= 0) ? opl : opl + nfill,
	      (nfill <= 0) ? text - nfill : text, (nfill <= 0) ? len : n);
      if (nfill > 0)
	memset(opl, 32, nfill);
    }
    op += lenb;
  }
}

STATIC_ROUTINE void DOUBLEC_TO_TEXT(int itype, char *pa, char *pb, int numb, int lenb, char sym)
{
  double *ip = (double *)pa;
  char *op = (char *)pb;
  int i = numb;
  while (i-- > 0) {
    char text[64];
    int nfill;
    int n;
    int part;
    for (part = 0; part < 2; part++) {
      int len = (lenb - 1) / 2;
      char *opl = op;
      if ((len * 2 + 1) != lenb)
	*opl++ = ' ';
      opl[len] = ',';
      if (IsRoprand(itype, ip)) {
	n = sprintf(text, "$ROPRAND");
	ip++;
      } else {
	int width;
	int prec;
	double tmp;
	char *pe;
	if (itype != DTYPE_NATIVE_DOUBLE)
	  CvtConvertFloat(ip++, itype, &tmp, DTYPE_NATIVE_DOUBLE, 0);
	else
	  tmp = *ip++;
	width = len < 23 ? len : 23;
	prec = width - 7;
	if (prec < 0)
	  prec = 0;
	n = sprintf(text, "%#*.*G", width, prec, tmp);
	pe = strchr(text, 'E');
	if (pe)
	  *pe = sym;
	else {
	  strcpy(text, text + 2);
	  text[n - 2] = sym;
	  text[n - 1] = '0';
	}
      }
      nfill = len - n;
      strncpy((nfill <= 0) ? opl : opl + nfill,
	      (nfill <= 0) ? text - nfill : text, (nfill <= 0) ? len : n);
      if (nfill > 0)
	memset(opl, 32, nfill);
    }
    op += lenb;
  }
}

#define BU_T(lena,pa,lenb,pb,numb)  TO_TEXT(pa,unsigned char,pb,numb,sprintf(text,"%u",(unsigned int)*ip++))
#define WU_T(lena,pa,lenb,pb,numb)  TO_TEXT(pa,unsigned short,pb,numb,sprintf(text,"%u",(unsigned int)*ip++))
#define LU_T(lena,pa,lenb,pb,numb)  TO_TEXT(pa,unsigned int,pb,numb,sprintf(text,"%u",(unsigned int)*ip++))
#define QU_T(lena,pa,lenb,pb,numb)  TO_TEXT(pa,uint64_t,pb,numb,sprintf(text,"%"PRIu64,(uint64_t)*ip++))
#define B_T(lena,pa,lenb,pb,numb)   TO_TEXT(pa,char,pb,numb,sprintf(text,"%d",(int)*ip++))
#define W_T(lena,pa,lenb,pb,numb)   TO_TEXT(pa,short,pb,numb,sprintf(text,"%d",(int)*ip++))
#define L_T(lena,pa,lenb,pb,numb)   TO_TEXT(pa,int,pb,numb,sprintf(text,"%d",(int)*ip++))
#define Q_T(lena,pa,lenb,pb,numb)   TO_TEXT(pa,int64_t,pb,numb,sprintf(text,"%"PRId64,(int64_t)*ip++))
#define OU_T(lena,pa,lenb,pb,numb)  TO_TEXT(pa,unsigned int,pb,numb,sprintf(text,"%#x%08x%08x%08x",ip[3],ip[2],ip[1],ip[0]);ip +=4)
#define O_T(lena,pa,lenb,pb,numb)   TO_TEXT(pa,unsigned int,pb,numb,sprintf(text,"%#x%08x%08x%08x",ip[3],ip[2],ip[1],ip[0]);ip +=4)
#if DTYPE_F == DTYPE_NATIVE_FLOAT
#define F_T(lena,pa,lenb,pb,numb)   FLOAT_TO_TEXT(DTYPE_F,pa,pb,numb,lenb,'E'); status=1
#define FS_T(lena,pa,lenb,pb,numb)  FLOAT_TO_TEXT(DTYPE_FS,pa,pb,numb,lenb,'S'); status=1
#define D_T(lena,pa,lenb,pb,numb)   DOUBLE_TO_TEXT(DTYPE_D,pa,pb,numb,lenb,'D'); status=1
#define FT_T(lena,pa,lenb,pb,numb)  DOUBLE_TO_TEXT(DTYPE_FT,pa,pb,numb,lenb,'T'); status=1
#define FC_T(lena,pa,lenb,pb,numb)  FLOATC_TO_TEXT(DTYPE_F,pa,pb,numb,lenb,'E'); status=1
#define FSC_T(lena,pa,lenb,pb,numb) FLOATC_TO_TEXT(DTYPE_FS,pa,pb,numb,lenb,'S'); status=1
#define DC_T(lena,pa,lenb,pb,numb)  DOUBLEC_TO_TEXT(DTYPE_D,pa,pb,numb,lenb,'D'); status=1
#define FTC_T(lena,pa,lenb,pb,numb) DOUBLEC_TO_TEXT(DTYPE_FT,pa,pb,numb,lenb,'T'); status=1
#else
#define F_T(lena,pa,lenb,pb,numb)   FLOAT_TO_TEXT(DTYPE_F,pa,pb,numb,lenb,'F'); status=1
#define FS_T(lena,pa,lenb,pb,numb)  FLOAT_TO_TEXT(DTYPE_FS,pa,pb,numb,lenb,'E'); status=1
#define D_T(lena,pa,lenb,pb,numb)   DOUBLE_TO_TEXT(DTYPE_D,pa,pb,numb,lenb,'V'); status=1
#define FT_T(lena,pa,lenb,pb,numb)  DOUBLE_TO_TEXT(DTYPE_FT,pa,pb,numb,lenb,'D'); status=1
#define FC_T(lena,pa,lenb,pb,numb)  FLOATC_TO_TEXT(DTYPE_F,pa,pb,numb,lenb,'F'); status=1
#define FSC_T(lena,pa,lenb,pb,numb) FLOATC_TO_TEXT(DTYPE_FS,pa,pb,numb,lenb,'E'); status=1
#define DC_T(lena,pa,lenb,pb,numb)  DOUBLEC_TO_TEXT(DTYPE_D,pa,pb,numb,lenb,'V'); status=1
#define FTC_T(lena,pa,lenb,pb,numb) DOUBLEC_TO_TEXT(DTYPE_FT,pa,pb,numb,lenb,'D'); status=1
#endif
#define G_T(lena,pa,lenb,pb,numb)   DOUBLE_TO_TEXT(DTYPE_G,pa,pb,numb,lenb,'G'); status=1
#define GC_T(lena,pa,lenb,pb,numb)  DOUBLEC_TO_TEXT(DTYPE_G,pa,pb,numb,lenb,'G'); status=1
/******* Text to Text ***************/
#define TEXT_TO_TEXT(pa,lena,pb,lenb,numb) \
  {char *ip = (char*)pa; char *op = (char *)pb; int i=numb; while(i-- > 0) {\
   int nfill; nfill = lenb - lena; \
   if (ip) \
      memcpy(op, ip, (nfill <= 0) ? lenb : lena); \
   if (nfill > 0) memset(op+lena,32,nfill); op += lenb; ip += lena;} status = MDSplusSUCCESS;}

#define T_T(lena,pa,lenb,pb,numb) TEXT_TO_TEXT(pa,lena,pb,lenb,numb)

/******* No Text to numeric conversions handled here *********/

#define T_BU(lena,pa,lenb,pb,numb)
#define T_WU(lena,pa,lenb,pb,numb)
#define T_LU(lena,pa,lenb,pb,numb)
#define T_QU(lena,pa,lenb,pb,numb)
#define T_OU(lena,pa,lenb,pb,numb)
#define T_B(lena,pa,lenb,pb,numb)
#define T_W(lena,pa,lenb,pb,numb)
#define T_L(lena,pa,lenb,pb,numb)
#define T_Q(lena,pa,lenb,pb,numb)
#define T_O(lena,pa,lenb,pb,numb)
#define T_F(lena,pa,lenb,pb,numb)
#define T_D(lena,pa,lenb,pb,numb)
#define T_G(lena,pa,lenb,pb,numb)
#define T_FC(lena,pa,lenb,pb,numb)
#define T_DC(lena,pa,lenb,pb,numb)
#define T_GC(lena,pa,lenb,pb,numb)
#define T_FS(lena,pa,lenb,pb,numb)
#define T_FT(lena,pa,lenb,pb,numb)
#define T_FSC(lena,pa,lenb,pb,numb)
#define T_FTC(lena,pa,lenb,pb,numb)

#define defcase(a,b) case MAXTYPE*DTYPE_##a+DTYPE_##b : a##_##b(lena,pa,lenb,pb,numb); break;
#define defset(a) \
        defcase(a,BU) \
        defcase(a,WU) \
        defcase(a,LU) \
        defcase(a,QU) \
        defcase(a,OU) \
        defcase(a,B) \
        defcase(a,W) \
        defcase(a,L) \
        defcase(a,Q) \
        defcase(a,O) \
        defcase(a,F) \
        defcase(a,D) \
        defcase(a,G) \
        defcase(a,FC) \
        defcase(a,DC) \
        defcase(a,GC) \
        defcase(a,T) \
        defcase(a,FS) \
        defcase(a,FT) \
        defcase(a,FSC) \
        defcase(a,FTC)

EXPORT int TdiConvert(struct descriptor_a *pdin, struct descriptor_a *pdout)
{
  int lena = pdin->length;
  int dtypea = pdin->dtype;
  char *pa = pdin->pointer;
  int classa = pdin->class == CLASS_A;
  int numa = classa ? (int)pdin->arsize / max(1, lena) : 1L;
  int lenb = pdout->length;
  int dtypeb = pdout->dtype;
  int classb = pdout->class == CLASS_A;
  char *pb = pdout->pointer;
  register int numb = classb ? (int)pdout->arsize / max(lenb, 1) : 1L;
  int n;
  int status = TdiINVDTYDSC;
 /** no output **/
  if (numb <= 0 || lenb <= 0)
    return 1;

 /** set zero or blanks **/
  if (numa <= 0 || dtypea == DTYPE_Z) {
    memset((void *)pb, (int)((dtypeb == DTYPE_T) ? ' ' : 0), numb * lenb);
    return 1;
  }

 /** incomplete fill (was an error in olden times) **/
  if (classa && (n = numb - numa) > 0) {
    memset((void *)(pb + numa * lenb), (int)((dtypeb == DTYPE_T) ? ' ' : 0), n * lenb);
    numb = numa;
  }

 /** identical types just copies, even unknowns **/
 /** vector=>either is int copy, scalar=>either is many copies **/
  if (dtypea == dtypeb && lena == lenb) {
 same:if (classa)
      memmove((void *)pb, (void *)pa, numb * lenb);
    else
      switch (lenb) {
      case 1:
	for (; --numb >= 0; pb += 1)
	  *pb = *pa;
      case 2:
	for (; --numb >= 0; pb += 2)
	  *(short *)pb = *(short *)pa;
      case 4:
	for (; --numb >= 0; pb += 4)
	  *(int *)pb = *(int *)pa;
      default:
	for (; --numb >= 0; pb += lenb)
	  memmove((void *)pb, (void *)pa, lenb);
      }
    status = MDSplusSUCCESS;
  } else if (dtypeb >= MAXTYPE)
    status = TdiINVDTYDSC;
 /** scalar => vector convert once as scalar and copy as needed **/
  else if (!classa && classb) {
    struct descriptor scalarb;
    scalarb = *(struct descriptor *)pdout;
    scalarb.class = CLASS_S;
    TdiConvert(pdin, (struct descriptor_a *)&scalarb);
    pa = pb;
    pb += lenb;
    --numb;
    goto same;
  } else {
	/** big branch **/
    n = MAXTYPE * dtypea + dtypeb;
    switch (n) {
      defset(BU)
	  defset(WU)
	  defset(LU)
	  defset(QU)
	  defset(OU)
	  defset(B)
	  defset(W)
	  defset(L)
	  defset(Q)
	  defset(O)
	  defset(F)
	  defset(D)
	  defset(G)
	  defset(FC)
	  defset(DC)
	  defset(GC)
	  defset(T)
	  defset(FS)
	  defset(FT)
	  defset(FSC)
	  defset(FTC)
    default:
      return TdiINVDTYDSC;
    }
  }
  return status;
}

double WideIntToDouble(unsigned int *bin_in, int size, int is_signed)
{
  int i;
  double factor;
  double ans = 0.0;
  unsigned int bin[16];
  int negative;

#ifdef WORDS_BIGENDIAN
  for (i = 0; i < size; i++)
    bin[i] = bin_in[size - i - 1];
#else
  memcpy(bin, bin_in, size * sizeof(int));
#endif

  negative = is_signed && (bin[size - 1] & 0x80000000);
  for (i = 0, factor = 1.; i < size; i++, factor = (i < 4) ? factor * TWO_32 : 0)
    ans += ((negative ? ~bin[i] : bin[i]) * factor);
  if (negative)
    ans = -1 - ans;
  return ans;
}

void DoubleToWideInt(double *in, int size, unsigned int *out)
{
  int negative = *in < 0;
  int i;
  double factor;
  double tmp;
  for (i = size - 1, tmp = negative ? -1 - *in : *in, factor =
       pow(2.0, 32. * (size - 1)); i >= 0; tmp -= out[i--] * factor, factor /= TWO_32)
    out[i] = (unsigned int)((tmp / factor) + .49999999999);
  if (negative)
    for (i = 0; i < size; i++)
      out[i] = ~out[i];

#ifdef WORDS_BIGENDIAN
  {
    unsigned int tmp[16];
    for (i = 0; i < size; i++)
      tmp[i] = out[i];
    for (i = 0; i < size; i++)
      out[i] = tmp[size - i - 1];
  }
#endif

}
