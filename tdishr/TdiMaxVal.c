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
/*------------------------------------------------------------------------------

                Name:   Tdi3MaxVal

                Type:   C function

                Author: MARK LONDON

                Date:   5-OCT-1993

                Purpose:

 Scalar/vector results from simple operations on one VMS data type and class.
 Works along the dimension.

form:   MAXLOC(array,[mask]) offset vector of size = rank of input
extern int Tdi3MaxLoc
extern int Tdi3MinLoc

form:   MAXVAL(array,[dim],[mask]) value scalar if no dim, vector if dim
extern int Tdi3MaxVal
extern int Tdi3MinVal
extern int Tdi3Mean
extern int Tdi3Product
extern int Tdi3Sum
extern int Tdi3Accumulate
------------------------------------------------------------------------------

        Call sequence:

int Tdi3xxxxx(struct descriptor *in, struct descriptor *mask,
               struct descriptor *out,int cnt_dim, int cnt_bef, int cnt_aft,
               int stp_dim, int stp_bef, int stp_aft)

------------------------------------------------------------------------------
   Copyright (c) 1993
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

        Description:


------------------------------------------------------------------------------*/

extern int Tdi3Lt();
extern int Tdi3Gt();
extern int Tdi3Divide();
#include <STATICdef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <mdsdescrip.h>
#include <string.h>
#include <tdishr_messages.h>
#include <int128.h>

#define out(typ) fprintf(stderr,"%3s: (%3d,%3d,%3d)\n",#typ,a->cnt_##typ,a->stp_##typ,a->stpm_##typ)

extern int CvtConvertFloat();

const int roprand = 0x8000;

#if DTYPE_NATIVE_DOUBLE == DTYPE_D
#define DHUGE 1.7E38
#elif DTYPE_NATIVE_DOUBLE == DTYPE_G
#define DHUGE 8.9E307
#else
#define DHUGE 1.7E308
#endif

int TdiLtO(unsigned int *in1, unsigned int *in2, int is_signed){
  if (is_signed)
    return int128_lt(( int128_t*)in1,( int128_t*)in2);
  return  uint128_lt((uint128_t*)in1,(uint128_t*)in2);
}
int TdiGtO(unsigned int *in1, unsigned int *in2, int is_signed){
  if (is_signed)
    return int128_gt(( int128_t*)in1,( int128_t*)in2);
  return  uint128_gt((uint128_t*)in1,(uint128_t*)in2);
}

typedef struct _args_t{
char*  inp;
char*  outp;
char*  maskp;
int    length;
int    stp_dim;
int    stp_bef;
int    stp_aft;
int    stpm_dim;
int    stpm_bef;
int    stpm_aft;
int    cnt_dim;
int    cnt_bef;
int    cnt_aft;
} args_t;

static inline int setupArgs(struct descriptor *in, struct descriptor *mask,
	struct descriptor *out, int stp_dim, int stp_bef, int stp_aft, args_t* a){
  switch (out->class)
  {
    case CLASS_S:
    case CLASS_D:
    case CLASS_A: break;
    default: return C_ERROR;
  }
  switch (in->class)
  {
    case CLASS_S:
    case CLASS_D:
    case CLASS_A: break;
    default: return C_ERROR;
  }
  switch (mask->class)
  {
    case CLASS_S:
    case CLASS_D: break;
    case CLASS_A:
	a->stpm_dim=stp_dim*mask->length,
	a->stpm_bef=stp_bef*mask->length,
	a->stpm_aft=stp_aft*mask->length;
	break;
    default: return C_ERROR;
  }
  return C_OK;
}

#define SETUP_ARGS(args)   args_t args = {\
  in->pointer,out->pointer,mask->pointer,in->length,\
  stp_dim*in->length,stp_bef*in->length,stp_aft*in->length,\
  0,0,0,\
  cnt_dim,cnt_bef,cnt_aft};\
  if (setupArgs(in,mask,out,stp_dim,stp_bef,stp_aft,&args))\
    return TdiINVCLADSC;

/********************************************
 define comparison functions:
 int8_gt, int8_lt, uint8_gt, uint8_lt, ...
 int64_gt, int64_lt, uint64_gt, uint64_lt, ...
 ********************************************/
static int gt(const double in1,const double in2){return in1>in2;}
static int lt(const double in1,const double in2){return in1<in2;}
static double add(const double in1,const double in2){return in1 + in2;}
static double mul(const double in1,const double in2){return in1 * in2;}
#define OP(type)\
static int type##_lt(const char* in1,const char* in2){return *(type##_t*)in1<*(type##_t*)in2;}\
static int type##_gt(const char* in1,const char* in2){return *(type##_t*)in1>*(type##_t*)in2;}\
static void type##_add(const char* in1,const char* in2, char* out){\
  type##_t* res = (type##_t*)out;\
  *res = (*(type##_t*)in1) + (*(type##_t*)in2);\
}\
static void type##_mul(const char* in1,const char* in2, char* out){\
  type##_t* res = (type##_t*)out;\
  *res = (*(type##_t*)in1) * (*(type##_t*)in2);\
}
#define AVG(type)\
static void type##_avgadd(const char* in, char* inout){\
  int64_t* sum = (int64_t*)inout;\
  *sum+= (*(type##_t*)in);\
}\
static void type##_avgdiv(const char* in, const int count, char* out){\
  type##_t* res = (type##_t*)out;\
  *res = (type##_t)((*(int64_t*)in)/count);\
}
#define BITOP(bit)   OP(int##bit); OP(uint##bit)
#define BITAVG(bit) AVG(int##bit);AVG(uint##bit)
BITOP(8);BITOP(16);BITOP(32);BITOP(64);
BITAVG(8);BITAVG(16);BITAVG(32);

static inline void int64_avgadd(const char* in, char* buf){
  int128_t a = {.low=*(uint64_t*)in,.high=*(int64_t*)in<0 ? -1 : 0 };
  int128_add(&a,(int128_t*)buf,(int128_t*)buf);
}
static inline void int64_avgdiv(const char* in, const int count, char* out){
  int128_t cnt = {.low=count,.high=0};
  int128_div((int128_t*)in,&cnt,&cnt);
  memcpy(out,&cnt.low,sizeof(int64_t));
}
static inline void uint64_avgadd(const char* in, char* buf){
  uint128_t a = {.low=*(uint64_t*)in,.high=0};
  uint128_add(&a,(uint128_t*)buf,(uint128_t*)buf);
}
static inline void uint64_avgdiv(const char* in, const int count, char* out){
  uint128_t cnt = {.low=count,.high=0};
  uint128_div((uint128_t*)in,&cnt,&cnt);
  memcpy(out,&cnt.low,sizeof(uint64_t));
}

/* missing versions for octaword */
static inline void int128_avgadd(const char* in, char* buf){
  int128_add((int128_t*)in,(int128_t*)buf,(int128_t*)buf);
}
static inline void int128_avgdiv(const char* in, const int count, char* out){
  int128_t cnt = {.low=count,.high=0};
  int128_div((int128_t*)in,&cnt,(int128_t*)out);
}
static inline void uint128_avgadd(const char* in, char* buf){
  uint128_add((uint128_t*)in,(uint128_t*)buf,(uint128_t*)buf);
}
static inline void uint128_avgdiv(const char* in, const int count, char* out){
  uint128_t cnt = {.low=count,.high=0};
  uint128_div((uint128_t*)in,&cnt,(uint128_t*)out);
}

static inline void operateIloc(void* start,int testit(const char*,const char*),args_t*a) {
  char* result = malloc(a->length);
  int* outp= (int*)a->outp;
  int ja, jb, jd;
  char *pid, *pib, *pia;
  char *pmd, *pmb, *pma;
  for (ja=0;    pia=a->inp,pma=a->maskp,ja < a->cnt_aft; ja++, pia+=a->stp_aft, pma+=a->stpm_aft) {                  // LOOP_AFTER
    for (jb=0,  pib= pia,  pmb= pma;    jb++ < a->cnt_bef; pib += a->stp_bef, pmb += a->stpm_bef, outp++) {          // LOOP_BEFORE_LOC
      *outp = -1;
      memcpy(result,start, a->length);
      for (jd=0,pid= pib,  pmd= pmb;    jd < a->cnt_dim; jd++, pid+=a->stp_dim, pmd+=a->stpm_dim) {                  // LOOP_DIM
        if (*pmd & 1 && testit(pid,result)){
          *outp = jd;
          memcpy(result,pid, a->length);
        }
      }
    }
  }
  free(result);
}
#define OperateIloc(type,start,testit) do{type strt=start;operateIloc(&strt,testit,&args);}while(0)
static inline void OperateFloc(char dtype, double start, int operator(const double,const double),args_t* a) {
  int* outp= (int*)a->outp;
  int ja, jb, jd;
  char *pid, *pib, *pia;
  char *pmd, *pmb, *pma;
  for (ja=0;    pia=a->inp,pma=a->maskp,ja < a->cnt_aft; ja++, pia+=a->stp_aft, pma+=a->stpm_aft) {                  // LOOP_AFTER
    for (jb=0,  pib= pia,  pmb= pma;    jb++ < a->cnt_bef; pib += a->stp_bef, pmb += a->stpm_bef, outp++) {          // LOOP_BEFORE_LOC
      *outp = -1;
      double result = start;
      for (jd=0,pid= pib,  pmd= pmb;    jd < a->cnt_dim; jd++, pid+=a->stp_dim, pmd+=a->stpm_dim) {                  // LOOP_DIM
        if (*pmd & 1) {
          double val;
          if (CvtConvertFloat(pid,dtype,&val,DTYPE_NATIVE_DOUBLE,0) && operator(val,result)) {
            result = val;
            *outp  = jd;
          }
        }
      }
    }
  }
}
static inline void OperateTloc(int testit(),args_t* a) {
  char testval;
  struct descriptor s_d={a->length,DTYPE_T,CLASS_S,0};
  struct descriptor o_d={1,DTYPE_B,CLASS_S,&testval};
  int* outp= (int*)a->outp;
  int ja, jb, jd;
  char *pid, *pib, *pia;
  char *pmd, *pmb, *pma;
  for (ja=0;    pia=a->inp,pma=a->maskp,ja < a->cnt_aft; ja++, pia+=a->stp_aft, pma+=a->stpm_aft) {                  // LOOP_AFTER
    for (jb=0,  pib= pia,  pmb= pma;    jb++ < a->cnt_bef; pib += a->stp_bef, pmb += a->stpm_bef, outp++) {          // LOOP_BEFORE_LOC
      struct descriptor result={a->length,DTYPE_T,CLASS_S,pib};
      *outp = -1;
      for (jd=0,pid= pib,  pmd= pmb;    jd < a->cnt_dim; jd++, pid+=a->stp_dim, pmd+=a->stpm_dim) {                  // LOOP_DIM
        if (*pmd & 1) {
           s_d.pointer=pid;
           testit(&s_d,&result,&o_d);
           if (testval) {
             result.pointer=pid;
             *outp = jd;
           }
        }
      }
    }
  }
}

int Tdi3MaxLoc(struct descriptor *in, struct descriptor *mask,
	       struct descriptor *out, int cnt_dim, int cnt_bef, int cnt_aft,
	       int stp_dim, int stp_bef, int stp_aft){
  SETUP_ARGS(args);
  switch (in->dtype) {
  case DTYPE_T:  OperateTloc(Tdi3Gt,&args);break;
  case DTYPE_B:  OperateIloc(   int8_t,       -128,   int8_gt);break;
  case DTYPE_BU: OperateIloc(  uint8_t,          0,  uint8_gt);break;
  case DTYPE_W:  OperateIloc(  int16_t,     -32768,  int16_gt);break;
  case DTYPE_WU: OperateIloc( uint16_t,          0, uint16_gt);break;
  case DTYPE_L:  OperateIloc(  int32_t, 0x80000000,  int32_gt);break;
  case DTYPE_LU: OperateIloc( uint32_t,          0, uint32_gt);break;
  case DTYPE_Q:  OperateIloc(  int64_t,  int64_min,  int64_gt);break;
  case DTYPE_QU: OperateIloc( uint64_t,          0, uint64_gt);break;
  case DTYPE_O:  OperateIloc( int128_t, int128_min,(void*)int128_gt);break;
  case DTYPE_OU: OperateIloc(uint128_t,uint128_min,(void*)uint128_gt);break;
  case DTYPE_F:  OperateFloc(DTYPE_F, -DHUGE, gt,&args);break;
  case DTYPE_FS: OperateFloc(DTYPE_FS,-DHUGE, gt,&args);break;
  case DTYPE_G:  OperateFloc(DTYPE_G, -DHUGE, gt,&args);break;
  case DTYPE_D:  OperateFloc(DTYPE_D, -DHUGE, gt,&args);break;
  case DTYPE_FT: OperateFloc(DTYPE_FT,-DHUGE, gt,&args);break;
  default:return TdiINVDTYDSC;
  }
  return 1;
}

int Tdi3MinLoc(struct descriptor *in, struct descriptor *mask,
	       struct descriptor *out, int cnt_dim, int cnt_bef, int cnt_aft,
	       int stp_dim, int stp_bef, int stp_aft){
  SETUP_ARGS(args);
  switch (in->dtype) {
  case DTYPE_T:  {OperateTloc(Tdi3Lt,&args);break;}
  case DTYPE_B:  {OperateIloc(   int8_t,        127,   int8_lt);break;}
  case DTYPE_BU: {OperateIloc(  uint8_t,         -1,  uint8_lt);break;}
  case DTYPE_W:  {OperateIloc(  int16_t,      32767,  int16_lt);break;}
  case DTYPE_WU: {OperateIloc( uint16_t,         -1, uint16_lt);break;}
  case DTYPE_L:  {OperateIloc(  int32_t, 0x7fffffff,  int32_lt);break;}
  case DTYPE_LU: {OperateIloc( uint32_t,         -1, uint32_lt);break;}
  case DTYPE_Q:  {OperateIloc(  int64_t,  int64_max,  int64_lt);break;}
  case DTYPE_QU: {OperateIloc( uint64_t,         -1, uint64_lt);break;}
  case DTYPE_O:  {OperateIloc( int128_t, int128_max,(void*) int128_lt);break;}
  case DTYPE_OU: {OperateIloc(uint128_t,uint128_max,(void*)uint128_lt);break;}
  case DTYPE_F:  {OperateFloc(DTYPE_F,  DHUGE, lt,&args);break;}
  case DTYPE_FS: {OperateFloc(DTYPE_FS, DHUGE, lt,&args);break;}
  case DTYPE_G:  {OperateFloc(DTYPE_G,  DHUGE, lt,&args);break;}
  case DTYPE_D:  {OperateFloc(DTYPE_D,  DHUGE, lt,&args);break;}
  case DTYPE_FT: {OperateFloc(DTYPE_FT, DHUGE, lt,&args);break;}
  default:return TdiINVDTYDSC;
  }
  return 1;
}

static inline void operateIval(void* start,int testit(const char*,const char*),args_t*a) {
  char* result = malloc(a->length);
  char *outp = a->outp;
  int ja, jb, jd;
  char *pid, *pib, *pia;
  char *pmd, *pmb, *pma;
  for (ja=0;    pia=a->inp,pma=a->maskp,ja < a->cnt_aft; ja++, pia+=a->stp_aft, pma+=a->stpm_aft) {                  // LOOP_AFTER
    for (jb=0,  pib= pia,  pmb= pma;    jb < a->cnt_bef; jb++, pib+=a->stp_bef, pmb+=a->stpm_bef, outp+=a->length) { // LOOP_BEFORE_VAL
      memcpy(result,start, a->length);
      for (jd=0,pid= pib,  pmd= pmb;    jd < a->cnt_dim; jd++, pid+=a->stp_dim, pmd+=a->stpm_dim) {                  // LOOP_DIM
        if (*pmd & 1 && testit(pid,result))
          memcpy(result,pid, a->length);
      }
      memcpy(outp, result, a->length);
    }
  }
  free(result);
}
#define OperateIval(type,start,testit) do{type strt=start;operateIval(&strt,testit,&args);}while(0)
static inline void OperateFval(char dtype, double start, int operator(const double,const double),args_t* a) {
  char *outp = a->outp;
  int ja, jb, jd;
  char *pid, *pib, *pia;
  char *pmd, *pmb, *pma;
  for (ja=0;    pia=a->inp,pma=a->maskp,ja < a->cnt_aft; ja++, pia+=a->stp_aft, pma+=a->stpm_aft) {                  // LOOP_AFTER
    for (jb=0,  pib= pia,  pmb= pma;    jb < a->cnt_bef; jb++, pib+=a->stp_bef, pmb+=a->stpm_bef, outp+=a->length) { // LOOP_BEFORE_VAL
      double result = start;
      for (jd=0,pid= pib,  pmd= pmb;    jd < a->cnt_dim; jd++, pid+=a->stp_dim, pmd+=a->stpm_dim) {                  // LOOP_DIM
        if (*pmd & 1) {
          double val;
          if (CvtConvertFloat(pid,dtype,&val,DTYPE_NATIVE_DOUBLE,0) && operator(val,result))
            result = val;
        }
      }
      CvtConvertFloat(&result,DTYPE_NATIVE_DOUBLE,outp,dtype,0);
    }
  }
}
static inline void OperateTval(int testit(),args_t* a) {
  char testval;
  struct descriptor s_d={a->length,DTYPE_T,CLASS_S,0};
  struct descriptor o_d={1,DTYPE_B,CLASS_S,&testval};
  struct descriptor result={a->length,DTYPE_T,CLASS_S,0};
  char *outp = a->outp;
  int ja, jb, jd;
  char *pid, *pib, *pia;
  char *pmd, *pmb, *pma;
  for (ja=0;    pia=a->inp,pma=a->maskp,ja < a->cnt_aft; ja++, pia+=a->stp_aft, pma+=a->stpm_aft) {                  // LOOP_AFTER
    for (jb=0,  pib= pia,  pmb= pma;    jb < a->cnt_bef; jb++, pib+=a->stp_bef, pmb+=a->stpm_bef, outp+=a->length) { // LOOP_BEFORE_VAL
      result.pointer = pib;
      for (jd=0,pid= pib,  pmd= pmb;    jd < a->cnt_dim; jd++, pid+=a->stp_dim, pmd+=a->stpm_dim) {                  // LOOP_DIM
        if (*pmd & 1) {
           s_d.pointer=pid;
           testit(&s_d,&result,&o_d);
           if (testval)
             result.pointer=pid;
        }
      }
      memmove(outp,result.pointer,a->length);
    }
  }
}

int Tdi3MaxVal(struct descriptor *in, struct descriptor *mask,
	       struct descriptor *out, int cnt_dim, int cnt_bef, int cnt_aft,
	       int stp_dim, int stp_bef, int stp_aft){
  SETUP_ARGS(args);
  switch (in->dtype) {
  case DTYPE_T:  OperateTval(Tdi3Gt,&args);break;
  case DTYPE_B:  OperateIval(   int8_t,       -128,   int8_gt);break;
  case DTYPE_BU: OperateIval(  uint8_t,          0,  uint8_gt);break;
  case DTYPE_W:  OperateIval(  int16_t,     -32768,  int16_gt);break;
  case DTYPE_WU: OperateIval( uint16_t,          0, uint16_gt);break;
  case DTYPE_L:  OperateIval(  int32_t, 0x80000000,  int32_gt);break;
  case DTYPE_LU: OperateIval( uint32_t,          0, uint32_gt);break;
  case DTYPE_Q:  OperateIval(  int64_t,  int64_min,  int64_gt);break;
  case DTYPE_QU: OperateIval( uint64_t,          0, uint64_gt);break;
  case DTYPE_O:  OperateIval( int128_t, int128_min,(void*) int128_gt);break;
  case DTYPE_OU: OperateIval(uint128_t,uint128_min,(void*)uint128_gt);break;
  case DTYPE_F:  OperateFval(DTYPE_F, -DHUGE, gt,&args);break;
  case DTYPE_FS: OperateFval(DTYPE_FS,-DHUGE, gt,&args);break;
  case DTYPE_G:  OperateFval(DTYPE_G, -DHUGE, gt,&args);break;
  case DTYPE_D:  OperateFval(DTYPE_D, -DHUGE, gt,&args);break;
  case DTYPE_FT: OperateFval(DTYPE_FT,-DHUGE, gt,&args);break;
  default:return TdiINVDTYDSC;
  }
  return 1;
}

int Tdi3MinVal(struct descriptor *in, struct descriptor *mask,
	       struct descriptor *out, int cnt_dim, int cnt_bef, int cnt_aft,
	       int stp_dim, int stp_bef, int stp_aft){
  SETUP_ARGS(args);
  switch (in->dtype) {
  case DTYPE_T:  OperateTval(Tdi3Lt,&args);break;
  case DTYPE_B:  OperateIval(   int8_t,        127,   int8_lt);break;
  case DTYPE_BU: OperateIval(  uint8_t,         -1,  uint8_lt);break;
  case DTYPE_W:  OperateIval(  int16_t,      32767,  int16_lt);break;
  case DTYPE_WU: OperateIval( uint16_t,         -1, uint16_lt);break;
  case DTYPE_L:  OperateIval(  int32_t, 0x7fffffff,  int32_lt);break;
  case DTYPE_LU: OperateIval( uint32_t,         -1, uint32_lt);break;
  case DTYPE_Q:  OperateIval(  int64_t,  int64_max,  int64_lt);break;
  case DTYPE_QU: OperateIval( uint64_t,         -1, uint64_lt);break;
  case DTYPE_O:  OperateIval( int128_t, int128_max,(void*) int128_lt);break;
  case DTYPE_OU: OperateIval(uint128_t,uint128_max,(void*)uint128_lt);break;
  case DTYPE_F:  OperateFval(DTYPE_F,  DHUGE, lt,&args);break;
  case DTYPE_FS: OperateFval(DTYPE_FS, DHUGE, lt,&args);break;
  case DTYPE_G:  OperateFval(DTYPE_G,  DHUGE, lt,&args);break;
  case DTYPE_D:  OperateFval(DTYPE_D,  DHUGE, lt,&args);break;
  case DTYPE_FT: OperateFval(DTYPE_FT, DHUGE, lt,&args);break;
  default:return TdiINVDTYDSC;
  }
  return 1;
}

/*loops for mean*/
static inline void OperateImean(size_t buflen,void avgadd(const char*, char*),void avgdiv(const char*, const int, char*),args_t*a) {
  char *outp = a->outp;
  int ja, jb, jd;
  char *pid, *pib, *pia;
  char *pmd, *pmb, *pma;
  char* buf = malloc(buflen);
  for (ja=0;    pia=a->inp,pma=a->maskp,ja < a->cnt_aft; ja++, pia+=a->stp_aft, pma+=a->stpm_aft) {                  // LOOP_AFTER
    for (jb=0,  pib= pia,  pmb= pma;    jb < a->cnt_bef; jb++, pib+=a->stp_bef, pmb+=a->stpm_bef, outp+=a->length) { // LOOP_BEFORE_VAL
      int count = 0;
      memset(buf,0,buflen);
      for (jd=0,pid= pib,  pmd= pmb;    jd < a->cnt_dim; jd++, pid+=a->stp_dim, pmd+=a->stpm_dim) {                  // LOOP_DIM
        if (*pmd & 1) {
          avgadd(pid,buf);
          count++;
        }
      }
      if (count)
        avgdiv(buf,count,outp);
      else
        memset(outp,0,a->length);
    }
  }
  free(buf);
}
static inline void OperateFmean(char dtype, args_t*a) {
  char *outp = a->outp;
  int ja, jb, jd;
  char *pid, *pib, *pia;
  char *pmd, *pmb, *pma;
  for (ja=0;    pia=a->inp,pma=a->maskp,ja < a->cnt_aft; ja++, pia+=a->stp_aft, pma+=a->stpm_aft) {                  // LOOP_AFTER
    for (jb=0,  pib= pia,  pmb= pma;    jb < a->cnt_bef; jb++, pib+=a->stp_bef, pmb+=a->stpm_bef, outp+=a->length) { // LOOP_BEFORE_VAL
      int count = 0;
      double result = 0;
      for (jd=0,pid= pib,  pmd= pmb;    jd < a->cnt_dim; jd++, pid+=a->stp_dim, pmd+=a->stpm_dim) {                  // LOOP_DIM
        if (*pmd & 1) {
          double val;
          if (CvtConvertFloat(pid,dtype,&val,DTYPE_NATIVE_DOUBLE,0)) {
            result+= val;count++;
          } else {
            CvtConvertFloat(&roprand,DTYPE_F,outp,dtype,0);
            break;
          }
        }
      }
      result/=count;
      CvtConvertFloat(&result,DTYPE_NATIVE_DOUBLE,outp,dtype,0);
    }
  }
}
static inline void OperateCmean(char dtype, args_t*a) {
  char *outp = a->outp;
  int ja, jb, jd;
  char *pid, *pib, *pia;
  char *pmd, *pmb, *pma;
  for (ja=0;    pia=a->inp,pma=a->maskp,ja < a->cnt_aft; ja++, pia+=a->stp_aft, pma+=a->stpm_aft) {                  // LOOP_AFTER
    for (jb=0,  pib= pia,  pmb= pma;    jb < a->cnt_bef; jb++, pib+=a->stp_bef, pmb+=a->stpm_bef, outp+=a->length) { // LOOP_BEFORE_VAL
      int count = 0;
      double resultr=0, resulti=0;
      for (jd=0,pid= pib,  pmd= pmb;    jd < a->cnt_dim; jd++, pid+=a->stp_dim, pmd+=a->stpm_dim) {                  // LOOP_DIM
        if (*pmd & 1) {
          double valr,vali;
          if (CvtConvertFloat(pid            ,dtype,&valr,DTYPE_NATIVE_DOUBLE,0)&&
              CvtConvertFloat(pid+a->length/2,dtype,&vali,DTYPE_NATIVE_DOUBLE,0)) {
            resultr+=valr; resulti+=vali; count++;
          } else {
            CvtConvertFloat(&roprand,DTYPE_F,outp            ,dtype,0);
            CvtConvertFloat(&roprand,DTYPE_F,outp+a->length/2,dtype,0);
            break;
          }
        }
      }
      resultr/=count; resulti/=count;
      CvtConvertFloat(&resultr,DTYPE_NATIVE_DOUBLE,outp            ,dtype,0);
      CvtConvertFloat(&resulti,DTYPE_NATIVE_DOUBLE,outp+a->length/2,dtype,0);
    }
  }
}

int Tdi3Mean(struct descriptor *in, struct descriptor *mask,
	     struct descriptor *out, int cnt_dim, int cnt_bef, int cnt_aft,
	     int stp_dim, int stp_bef, int stp_aft){
  SETUP_ARGS(args);
  switch (in->dtype) {
  case DTYPE_B:  OperateImean( 8,   int8_avgadd,   int8_avgdiv,&args);break;
  case DTYPE_BU: OperateImean( 8,  uint8_avgadd,  uint8_avgdiv,&args);break;
  case DTYPE_W:  OperateImean( 8,  int16_avgadd,  int16_avgdiv,&args);break;
  case DTYPE_WU: OperateImean( 8, uint16_avgadd, uint16_avgdiv,&args);break;
  case DTYPE_L:  OperateImean( 8,  int32_avgadd,  int32_avgdiv,&args);break;
  case DTYPE_LU: OperateImean( 8, uint32_avgadd, uint32_avgdiv,&args);break;
  case DTYPE_Q:  OperateImean(16,  int64_avgadd,  int64_avgdiv,&args);break;//TODO: use octaword as buffer
  case DTYPE_QU: OperateImean(16, uint64_avgadd, uint64_avgdiv,&args);break;
  case DTYPE_O:  OperateImean(16, int128_avgadd,(void*) int128_avgdiv,&args);break;
  case DTYPE_OU: OperateImean(16,uint128_avgadd,(void*)uint128_avgdiv,&args);break;
  case DTYPE_F:  OperateFmean(DTYPE_F    ,&args);break;
  case DTYPE_FS: OperateFmean(DTYPE_FS   ,&args);break;
  case DTYPE_G:  OperateFmean(DTYPE_G    ,&args);break;
  case DTYPE_D:  OperateFmean(DTYPE_D    ,&args);break;
  case DTYPE_FT: OperateFmean(DTYPE_FT   ,&args);break;
  case DTYPE_FC: OperateCmean(DTYPE_F    ,&args);break;
  case DTYPE_FSC:OperateCmean(DTYPE_FS   ,&args);break;
  case DTYPE_GC: OperateCmean(DTYPE_G    ,&args);break;
  case DTYPE_DC: OperateCmean(DTYPE_D    ,&args);break;
  case DTYPE_FTC:OperateCmean(DTYPE_FT   ,&args);break;
  default:return TdiINVDTYDSC;
  }
  return 1;
}

/*loops for sum and product*/
static inline void OperateIfun(char init, void fun(const char*, const char*, char*),args_t*a) {
  char *outp = a->outp;
  int ja, jb, jd;
  char *pid, *pib, *pia;
  char *pmd, *pmb, *pma;
  for (ja=0;    pia=a->inp,pma=a->maskp,ja < a->cnt_aft; ja++, pia+=a->stp_aft, pma+=a->stpm_aft) {                  // LOOP_AFTER
    for (jb=0,  pib= pia,  pmb= pma;    jb < a->cnt_bef; jb++, pib+=a->stp_bef, pmb+=a->stpm_bef, outp+=a->length) { // LOOP_BEFORE_VAL
      memset(outp,0,a->length); outp[0]=init;
      for (jd=0,pid= pib,  pmd= pmb;    jd < a->cnt_dim; jd++, pid+=a->stp_dim, pmd+=a->stpm_dim) {                  // LOOP_DIM
        if (*pmd & 1)
          fun(pid,outp,outp);
      }
    }
  }
}
static inline void OperateFfun(double init, char dtype, double fun(const double,const double),args_t*a) {
  char *outp = a->outp;
  int ja, jb, jd;
  char *pid, *pib, *pia;
  char *pmd, *pmb, *pma;
  for (ja=0;    pia=a->inp,pma=a->maskp,ja < a->cnt_aft; ja++, pia+=a->stp_aft, pma+=a->stpm_aft) {                  // LOOP_AFTER
    for (jb=0,  pib= pia,  pmb= pma;    jb < a->cnt_bef; jb++, pib+=a->stp_bef, pmb+=a->stpm_bef, outp+=a->length) { // LOOP_BEFORE_VAL
      double result = init;
      for (jd=0,pid= pib,  pmd= pmb;    jd < a->cnt_dim; jd++, pid+=a->stp_dim, pmd+=a->stpm_dim) {                  // LOOP_DIM
        if (*pmd & 1) {
          double val;
          if (CvtConvertFloat(pid,dtype,&val,DTYPE_NATIVE_DOUBLE,0)) {
            result = fun(val,result);
          } else {
            CvtConvertFloat(&roprand,DTYPE_F,outp,dtype,0);
            break;
          }
        }
      }
      CvtConvertFloat(&result,DTYPE_NATIVE_DOUBLE,outp,dtype,0);
    }
  }
}
static inline void OperateCfun(double init, char dtype, double fun(const double,const double), args_t*a) {
  char *outp = a->outp;
  int ja, jb, jd;
  char *pid, *pib, *pia;
  char *pmd, *pmb, *pma;
  for (ja=0;    pia=a->inp,pma=a->maskp,ja < a->cnt_aft; ja++, pia+=a->stp_aft, pma+=a->stpm_aft) {                  // LOOP_AFTER
    for (jb=0,  pib= pia,  pmb= pma;    jb < a->cnt_bef; jb++, pib+=a->stp_bef, pmb+=a->stpm_bef, outp+=a->length) { // LOOP_BEFORE_VAL
      double resultr = init,resulti = init;
      for (jd=0,pid= pib,  pmd= pmb;    jd < a->cnt_dim; jd++, pid+=a->stp_dim, pmd+=a->stpm_dim) {                  // LOOP_DIM
        if (*pmd & 1) {
          double valr,vali;
          if (CvtConvertFloat(pid            ,dtype,&valr,DTYPE_NATIVE_DOUBLE,0)&&
              CvtConvertFloat(pid+a->length/2,dtype,&vali,DTYPE_NATIVE_DOUBLE,0)) {
            resultr=fun(valr,resultr);resulti=fun(vali,resulti);
          } else {
            CvtConvertFloat(&roprand,DTYPE_F,outp            ,dtype,0);
            CvtConvertFloat(&roprand,DTYPE_F,outp+a->length/2,dtype,0);
            break;
          }
        }
      }
      CvtConvertFloat(&resultr,DTYPE_NATIVE_DOUBLE,outp            ,dtype,0);
      CvtConvertFloat(&resulti,DTYPE_NATIVE_DOUBLE,outp+a->length/2,dtype,0);
    }
  }
}

int Tdi3Product(struct descriptor *in, struct descriptor *mask,
		struct descriptor *out, int cnt_dim, int cnt_bef, int cnt_aft,
		int stp_dim, int stp_bef, int stp_aft)
{
  SETUP_ARGS(args);
  switch (in->dtype) {
  case DTYPE_B:  OperateIfun(1,    int8_mul,&args);break;
  case DTYPE_BU: OperateIfun(1,   uint8_mul,&args);break;
  case DTYPE_W:  OperateIfun(1,   int16_mul,&args);break;
  case DTYPE_WU: OperateIfun(1,  uint16_mul,&args);break;
  case DTYPE_L:  OperateIfun(1,   int32_mul,&args);break;
  case DTYPE_LU: OperateIfun(1,  uint32_mul,&args);break;
  case DTYPE_Q:  OperateIfun(1,   int64_mul,&args);break;
  case DTYPE_QU: OperateIfun(1,  uint64_mul,&args);break;
  case DTYPE_O:  OperateIfun(1,(void*) int128_mul,&args);break;
  case DTYPE_OU: OperateIfun(1,(void*)uint128_mul,&args);break;
  case DTYPE_F:  OperateFfun(1,DTYPE_F ,mul,&args);break;
  case DTYPE_FS: OperateFfun(1,DTYPE_FS,mul,&args);break;
  case DTYPE_G:  OperateFfun(1,DTYPE_G ,mul,&args);break;
  case DTYPE_D:  OperateFfun(1,DTYPE_D ,mul,&args);break;
  case DTYPE_FT: OperateFfun(1,DTYPE_FT,mul,&args);break;
  case DTYPE_FC: OperateCfun(1,DTYPE_F ,mul,&args);break;
  case DTYPE_FSC:OperateCfun(1,DTYPE_FS,mul,&args);break;
  case DTYPE_GC: OperateCfun(1,DTYPE_G ,mul,&args);break;
  case DTYPE_DC: OperateCfun(1,DTYPE_D ,mul,&args);break;
  case DTYPE_FTC:OperateCfun(1,DTYPE_FT,mul,&args);break;
  default:return TdiINVDTYDSC;
  }
  return 1;
}

int Tdi3Sum(struct descriptor *in, struct descriptor *mask,
	    struct descriptor *out, int cnt_dim, int cnt_bef, int cnt_aft,
	    int stp_dim, int stp_bef, int stp_aft)
{
  SETUP_ARGS(args);
  switch (in->dtype) {
  case DTYPE_B:  OperateIfun(0,    int8_add,&args);break;
  case DTYPE_BU: OperateIfun(0,   uint8_add,&args);break;
  case DTYPE_W:  OperateIfun(0,   int16_add,&args);break;
  case DTYPE_WU: OperateIfun(0,  uint16_add,&args);break;
  case DTYPE_L:  OperateIfun(0,   int32_add,&args);break;
  case DTYPE_LU: OperateIfun(0,  uint32_add,&args);break;
  case DTYPE_Q:  OperateIfun(0,   int64_add,&args);break;
  case DTYPE_QU: OperateIfun(0,  uint64_add,&args);break;
  case DTYPE_O:  OperateIfun(0,(void*) int128_add,&args);break;
  case DTYPE_OU: OperateIfun(0,(void*)uint128_add,&args);break;
  case DTYPE_F:  OperateFfun(0,DTYPE_F ,add,&args);break;
  case DTYPE_FS: OperateFfun(0,DTYPE_FS,add,&args);break;
  case DTYPE_G:  OperateFfun(0,DTYPE_G ,add,&args);break;
  case DTYPE_D:  OperateFfun(0,DTYPE_D ,add,&args);break;
  case DTYPE_FT: OperateFfun(0,DTYPE_FT,add,&args);break;
  case DTYPE_FC: OperateCfun(0,DTYPE_F ,add,&args);break;
  case DTYPE_FSC:OperateCfun(0,DTYPE_FS,add,&args);break;
  case DTYPE_GC: OperateCfun(0,DTYPE_G ,add,&args);break;
  case DTYPE_DC: OperateCfun(0,DTYPE_D ,add,&args);break;
  case DTYPE_FTC:OperateCfun(0,DTYPE_FT,add,&args);break;
  default:return TdiINVDTYDSC;
  }
  return 1;
}

static inline void OperateIaccum(void add(const char*,const char*, char*),args_t* a) {
  int ja, jb, jd;
  char *poa, *pob, *pod;
  char *pia, *pib, *pid;
  char *pma, *pmb, *pmd;
  char *result = malloc(a->length);
  for (ja=0,poa=a->outp,pia=a->inp,pma=a->maskp; ja++ < a->cnt_aft; poa += a->stp_aft, pia += a->stp_aft, pma += a->stpm_aft) { // LOOP_AFTER_ARRAY
    for (jb=0,pob=poa,  pib=pia,   pmb=pma;      jb++ < a->cnt_bef; pob += a->stp_bef, pib += a->stp_bef, pmb += a->stpm_bef) { // LOOP_BEFORE_ARRAY
      memset(result,0,a->length);
      for (jd=0,pod=pob,pid=pib,   pmd=pmb;      jd++ < a->cnt_dim; pod += a->stp_dim, pid += a->stp_dim, pmd += a->stpm_dim) { // LOOP_DIM_ARRAY
        if (*pmd & 1) {
          add(pid,result,result);
          memcpy(pod,result,a->length);
        }
      }
    }
  }
  free(result);
}
static inline void OperateFaccum(char dtype,args_t* a){
  int ja, jb, jd;
  char *poa, *pob, *pod;
  char *pia, *pib, *pid;
  char *pma, *pmb, *pmd;
  for (ja=0,poa=a->outp,pia=a->inp,pma=a->maskp; ja++ < a->cnt_aft; poa += a->stp_aft, pia += a->stp_aft, pma += a->stpm_aft) { // LOOP_AFTER_ARRAY
    for (jb=0,pob=poa,  pib=pia,   pmb=pma;      jb++ < a->cnt_bef; pob += a->stp_bef, pib += a->stp_bef, pmb += a->stpm_bef) { // LOOP_BEFORE_ARRAY
      double result = 0;
      for (jd=0,pod=pob,pid=pib,   pmd=pmb;      jd++ < a->cnt_dim; pod += a->stp_dim, pid += a->stp_dim, pmd += a->stpm_dim) { // LOOP_DIM_ARRAY
        if (*pmd & 1){
          double val;
          if (CvtConvertFloat(pid,dtype,&val,DTYPE_NATIVE_DOUBLE,0)) {
            result += val;
            CvtConvertFloat(&result,DTYPE_NATIVE_DOUBLE,pod,dtype,0);
          } else CvtConvertFloat(&roprand,DTYPE_F,pod,dtype,0);
        }
      }
    }
  }
}

static inline void OperateCaccum(char dtype,args_t* a){
  int ja, jb, jd;
  char *poa, *pob, *pod;
  char *pia, *pib, *pid;
  char *pma, *pmb, *pmd;
  for (ja=0,poa=a->outp,pia=a->inp,pma=a->maskp; ja++ < a->cnt_aft; poa += a->stp_aft, pia += a->stp_aft, pma += a->stpm_aft) { // LOOP_AFTER_ARRAY
    for (jb=0,pob=poa,  pib=pia,   pmb=pma;      jb++ < a->cnt_bef; pob += a->stp_bef, pib += a->stp_bef, pmb += a->stpm_bef) { // LOOP_BEFORE_ARRAY
      double result = 0, resulti = 0;
      for (jd=0,pod=pob,pid=pib,   pmd=pmb;      jd++ < a->cnt_dim; pod += a->stp_dim, pid += a->stp_dim, pmd += a->stpm_dim) { // LOOP_DIM_ARRAY
        if (*pmd & 1){
          double val, vali;
          if (CvtConvertFloat(pid,dtype,&val,DTYPE_NATIVE_DOUBLE,0) &&
              CvtConvertFloat(pid+a->length,dtype,&vali,DTYPE_NATIVE_DOUBLE,0)) {
            result += val; resulti += vali;
            CvtConvertFloat(&result,DTYPE_NATIVE_DOUBLE,pod,dtype,0);
            CvtConvertFloat(&resulti,DTYPE_NATIVE_DOUBLE,pod+a->length/2,dtype,0);
          } else {
            CvtConvertFloat(&roprand,DTYPE_F,pod,dtype,0);
            CvtConvertFloat(&roprand,DTYPE_F,pod+a->length/2,dtype,0);
          }
        }
      }
    }
  }
}

int Tdi3Accumulate(struct descriptor *in, struct descriptor *mask,
		   struct descriptor *out, int cnt_dim, int cnt_bef, int cnt_aft,
		   int stp_dim, int stp_bef, int stp_aft)
{
  SETUP_ARGS(args);
  switch (in->dtype) {
    case DTYPE_B:  OperateIaccum(   int8_add,&args);break;
    case DTYPE_BU: OperateIaccum(  uint8_add,&args);break;
    case DTYPE_W:  OperateIaccum(  int16_add,&args);break;
    case DTYPE_WU: OperateIaccum( uint16_add,&args);break;
    case DTYPE_L:  OperateIaccum(  int32_add,&args);break;
    case DTYPE_LU: OperateIaccum( uint32_add,&args);break;
    case DTYPE_Q:  OperateIaccum(  int64_add,&args);break;
    case DTYPE_QU: OperateIaccum( uint64_add,&args);break;
    case DTYPE_O:  OperateIaccum((void*) int128_add,&args);break;
    case DTYPE_OU: OperateIaccum((void*)uint128_add,&args);break;
    case DTYPE_F:  OperateFaccum(DTYPE_F    ,&args);break;
    case DTYPE_FS: OperateFaccum(DTYPE_FS   ,&args);break;
    case DTYPE_G:  OperateFaccum(DTYPE_G    ,&args);break;
    case DTYPE_D:  OperateFaccum(DTYPE_D    ,&args);break;
    case DTYPE_FT: OperateFaccum(DTYPE_FT   ,&args);break;
    case DTYPE_FC: OperateCaccum(DTYPE_F    ,&args);break;
    case DTYPE_FSC:OperateCaccum(DTYPE_FS   ,&args);break;
    case DTYPE_GC: OperateCaccum(DTYPE_G    ,&args);break;
    case DTYPE_DC: OperateCaccum(DTYPE_D    ,&args);break;
    case DTYPE_FTC:OperateCaccum(DTYPE_FT   ,&args);break;
    default:return TdiINVDTYDSC;
  }
  return 1;
}
