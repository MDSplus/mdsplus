#pragma once
#include <string.h>
#include <inttypes.h>
#include <math.h>

#ifdef WORDS_BIGENDIAN
 typedef struct int128_s{
    int64_t high;
   uint64_t low;
 } int128_t;
 typedef struct uint128_s{
   uint64_t high;
   uint64_t low;
 } uint128_t;
 #define int128_one  { 0, 1 };
 #define int128_max  { 0x7fffffffffffffffLL,-1 };
 #define int128_min  { 0x8000000000000000LL, 0 };
#else
 typedef struct int128_s{
   uint64_t low;
    int64_t high;
 } int128_t;
 typedef struct uint128_s{
   uint64_t low;
   uint64_t high;
 } uint128_t;
 #define int128_one  { 1, 0 };
 #define int128_max  {-1, 0x7fffffffffffffffLL };
 #define int128_min  { 0, 0x8000000000000000LL };
#endif
#define int128_zero { 0, 0 };
#define uint128_max {-1,-1 };
#define uint128_min { 0, 0 };

static inline int uint128_gt(const uint128_t *a, const uint128_t *b){
  if (a->high==b->high)
    return a->low > b->low;
  return a->high > b->high;
}

static inline int int128_gt(const int128_t *a, const int128_t *b){
  if (a->high==b->high)
    return a->low > b->low;
  return a->high > b->high;
}

static inline int uint128_lt(const uint128_t *a, const uint128_t *b){
  if (a->high==b->high)
    return a->low < b->low;
  return a->high < b->high;
}

static inline int int128_lt(const int128_t *a, const int128_t *b){
  if (a->high==b->high)
    return a->low < b->low;
  return a->high < b->high;
}

static inline void int128_minus(const int128_t *a, int128_t *ans){
  ans->high = ~(a->high);
  ans->low  = ~(a->low);
  ans->low++;
  if (ans->low == 0)
    ans->high++;
}

static inline int int128_abs(const int128_t* x, int128_t* r){
  if (x->high>=0) {
    if (x!=r) memcpy(r,x,sizeof(int128_t));
    return 0;
  }
  int128_minus(x,r);
  return 1;
}

static inline int uint128_add(const uint128_t *a, const uint128_t *b, uint128_t *ans){
  uint128_t aa;memcpy(&aa,a,sizeof(uint128_t));
  ans->low  = a->low  + b->low;
  ans->high = a->high + b->high;
  if (ans->low < aa.low)
    ans->high++;
  return ans->high < aa.high;
}

static inline int int128_add(const int128_t *a, const int128_t *b, int128_t *ans){
  int128_t aa;memcpy(&aa,a,sizeof(uint128_t));
  ans->low  = a->low  + b->low;
  ans->high = a->high + b->high;
  if (ans->low < aa.low)
    ans->high++;
  return ans->high < aa.high;
}

static inline int uint128_sub(const uint128_t* a, const uint128_t* b, uint128_t *ans) {
  uint128_t aa;memcpy(&aa,a,sizeof(uint128_t));
  ans->low  = a->low  - b->low;
  ans->high = a->high - b->high;
  if (ans->low > aa.low)
    ans->high--;
  return ans->high > aa.high;
}

static inline int int128_sub(const int128_t* a, const int128_t* b, int128_t *ans){
  int128_t aa;memcpy(&aa,a,sizeof(int128_t));
  ans->low  = a->low  - b->low;
  ans->high = a->high - b->high;
  if (ans->low > aa.low)
    ans->high--;
  return ans->high > aa.high;
}

static inline int uint128_mul(const uint128_t* x, const uint128_t* y, uint128_t *ans){
  #define HI_INT 0xFFFFFFFF00000000LL
  #define LO_INT 0x00000000FFFFFFFFLL
  /* as by 128-bit integer arithmetic for C++, by Robert Munafo */
  uint64_t acc, ac2, carry, o1, o2;
  uint64_t a, b, c, d, e, f, g, h;

/************************
 x      a  b  c  d
 y      e  f  g  h
-------------------------
        -o2-  -o1-
 ************************/

  d =  x->low  & LO_INT;
  c = (x->low  & HI_INT) >> 32LL;
  b =  x->high & LO_INT;
  a = (x->high & HI_INT) >> 32LL;

  h =  y->low  & LO_INT;
  g = (y->low  & HI_INT) >> 32LL;
  f =  y->high & LO_INT;
  e = (y->high & HI_INT) >> 32LL;

  acc = d * h;
  o1  = acc & LO_INT;
  acc >>= 32LL;
  carry = 0;
  ac2 = acc + c * h; if (ac2 < acc) { carry++; }
  acc = ac2 + d * g; if (acc < ac2) { carry++; }
  ans->low = o1 | (acc << 32LL);
  ac2 = (acc >> 32LL) | (carry << 32LL); carry = 0;

  acc = ac2 + b * h; if (acc < ac2) { carry++; }
  ac2 = acc + c * g; if (ac2 < acc) { carry++; }
  acc = ac2 + d * f; if (acc < ac2) { carry++; }
  o2  = acc & LO_INT;
  ac2 = (acc >> 32LL) | (carry << 32LL);

  acc = ac2 + a * h;
  ac2 = acc + b * g;
  acc = ac2 + c * f;
  ac2 = acc + d * e;
  ans->high = (ac2 << 32LL) | o2;
  return ((acc >> 32LL) | (carry << 32LL))==0; // 1 if no overflow
}

static inline int int128_mul(const int128_t* x, const int128_t* d, int128_t* r){
  uint128_t ux,ud;
  int mns = (int128_abs(x,(int128_t*)&ux) ^ int128_abs(d,(int128_t*)&ud));
  uint128_mul(&ux,&ud,(uint128_t*)r);
  if (mns) int128_minus(r,r);
  return 1;
}

static inline int uint128_div(const uint128_t* x, const uint128_t* d, uint128_t* r){
  const double hifac = ((double)((uint64_t)-1)+1);
  memset(r,0,sizeof(int128_t));
  if ((d->low == 0) && (d->high == 0))
    return 1;
  double dd = (double)d->low + hifac*(double)d->high;
  uint128_t t,a;
  memcpy(&a,x,sizeof(int128_t));
  double dr;
  while ((dr = ((double)a.low + hifac*(double)a.high) / dd)>=1) {
    t.low  = (uint64_t)fmod(dr,hifac);
    t.high =  (int64_t)(dr/hifac);
    uint128_add(&t,r,r);
    uint128_mul(d,&t,&t);
    uint128_sub(&a,&t,&a);
  } // 'a' is the remainder
  return (a.low == 0) && (a.high == 0); // 1 if no remainder
}

static inline int int128_div(const int128_t* x, const int128_t* d, int128_t* r){
  uint128_t ux,ud;
  int mns = (int128_abs(x,(int128_t*)&ux) ^ int128_abs(d,(int128_t*)&ud));
  uint128_div(&ux,&ud,(uint128_t*)r);
  if (mns) int128_minus(r,r);
  return 1;
}
