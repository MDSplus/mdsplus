#pragma once
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>

//#define DEBUG

#define uint64_max 0xffffffffffffffffLL
#define uint64_min 0x0000000000000000LL
#define  int64_max 0x7fffffffffffffffLL
#define  int64_min 0x8000000000000000LL
#ifdef WORDS_BIGENDIAN
 typedef struct int128_s{
    int64_t high;
   uint64_t low;
 } int128_t;
 typedef struct uint128_s{
   uint64_t high;
   uint64_t low;
 } uint128_t;
#else
 typedef struct int128_s{
   uint64_t low;
    int64_t high;
 } int128_t;
 typedef struct uint128_s{
   uint64_t low;
   uint64_t high;
 } uint128_t;
#endif
#define  int128_one  { .high=0,         .low=1          };
#define  int128_zero { .high=0,         .low=0          };
#define  int128_max  { .high=int64_max, .low=uint64_max };
#define  int128_min  { .high=int64_min, .low=uint64_max };
#define uint128_max  { .high=uint64_max,.low=uint64_max };
#define uint128_min  { .high=uint64_min,.low=uint64_min };

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

static inline void uint128_lshft(const uint128_t* x, const int n, uint128_t* r){
  int nn = n%128; if (nn<0) nn+=128;
  if (nn>63) {
    r->high = x->low<<(nn-64);               r->low = 0;
  } else {
    r->high = x->high<<nn | x->low>>(64-nn); r->low = x->low<<nn;
  }
}

static inline void uint128_rshft(const uint128_t* x, const int n, uint128_t* r){
  int nn = n%128; if (nn<0) nn+=128;
  if (nn>63) {
    r->low = x->high>>(nn-64);              r->high = 0;
  } else {
    r->low = x->low>>nn | x->high<<(64-nn); r->high = x->high>>nn;
  }
}

static inline void int128_lshft(const int128_t* x, const int n, int128_t* r){
  int nn = n%128; if (nn<0) nn+=128;
  if (nn>63) {
    r->high = x->low<<(nn-64);               r->low = 0;
  } else {
    r->high = x->high<<nn | x->low>>(64-nn); r->low = x->low<<nn;
  }
}

static inline void int128_rshft(const int128_t* x, const int n, int128_t* r){
  int nn = n%128; if (nn<0) nn+=128;
  if (nn>63) {
    r->low = x->high>>(nn-64);              r->high = 0;
  } else {
    r->low = x->low>>nn | x->high<<(64-nn); r->high = x->high>>nn;
  }
}

static inline void uint128_ishft(const uint128_t* x, const int n, uint128_t* r){
  if (n<0)
    return uint128_rshft(x,-n,r);
  return uint128_lshft(x,n,r);
}
#define int128_ishft(x,n,r) uint128_ishft((uint128_t*)x,n,(uint128_t*)r)

#define INT128_BUFLEN 128/3+2
#define INT128_BUF(buf) char buf[INT128_BUFLEN]
static char* uint128_deco(const uint128_t* in, char* p){
  uint128_t n;
  int i;
  memset(p, '0', INT128_BUFLEN - 1);
  p[INT128_BUFLEN - 1] = '\0';
  memcpy(&n, in, sizeof(n));
  for (i = 0; i < 128; i++){
    int j, carry;
    carry = (n.high > int64_max);
    // Shift n[] left, doubling it
    n.high= (n.high << 1) + (n.low > int64_max);
    n.low = (n.low  << 1);
    // Add s[] to itself in decimal, doubling it
    for (j=INT128_BUFLEN-1 ; j-->0 ;) {
      p[j] += p[j] - '0' + carry;
      carry = (p[j] > '9');
      if (carry) p[j] -= 10;
    }
  }
  char *max = &p[INT128_BUFLEN-2];
  for(;(p[0] == '0') && (p < max); p++);
  return p;
}

static inline char* int128_deco(const int128_t* in, char* p){
 int128_t a;
 int s = int128_abs(in,&a);
 p = uint128_deco((uint128_t*)&a, p);
 if (s) (--p)[0]='-';
 return p;
}


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

#define HI_INT 0xFFFFFFFF00000000LL
#define LO_INT 0x00000000FFFFFFFFLL
static inline int uint128_mul(const uint128_t* x, const uint128_t* y, uint128_t *ans){
  /* as by 128-bit integer arithmetic for C++, by Robert Munafo */
  uint64_t a[4], b[4], A[4];
  a[0] = (x->high & HI_INT) >> 32LL;
  a[1] =  x->high & LO_INT;
  a[2] = (x->low  & HI_INT) >> 32LL;
  a[3] =  x->low  & LO_INT;

  b[0] = (y->high & HI_INT) >> 32LL;
  b[1] =  y->high & LO_INT;
  b[2] = (y->low  & HI_INT) >> 32LL;
  b[3] =  y->low  & LO_INT;
  int i,j;
  uint64_t carry,acc = 0;
  for (j=4 ; j-->0 ;) {
    carry = 0;
    for (i=4; i-->j;) {
      uint64_t ac2 = acc + a[i] * b[j-i+3];
      if (ac2 < acc) carry++;
      acc = ac2;
    }
    A[j] = acc & LO_INT;
    acc = (acc >> 32LL) | (carry << 32LL);
  }
  ans->high = A[0] << 32LL | A[1];
  ans->low  = A[2] << 32LL | A[3];
  return carry==0 && (acc&HI_INT)==0;
}

static inline int int128_mul(const int128_t* x, const int128_t* d, int128_t* ans){
  uint128_t ux,ud;
  int mns = (int128_abs(x,(int128_t*)&ux) ^ int128_abs(d,(int128_t*)&ud));
  uint128_mul(&ux,&ud,(uint128_t*)ans);
  if (mns) int128_minus(ans,ans);
  return 1;
}

static inline int uint128_div(const uint128_t* x, const uint128_t* y, uint128_t* ans){
  if (y->low==0 && y->high==0) {
    ans->low=0;ans->high=0;
    return 1;
  }
  if (y->low==1 && y->high==0) {
    memcpy(ans,x,sizeof(uint128_t));
    return 1;
  }
  if (uint128_lt(x,y)) {
    ans->low=0;ans->high=0;
    return 0;
  }
  int p = 0;
  uint128_t n;memcpy(&n,x,sizeof(uint128_t));
  uint128_t d;memcpy(&d,y,sizeof(uint128_t));
  uint128_sub(&n,&d,&n);
  while (!uint128_lt(&n,&d)) {
    uint128_sub(&n,&d,&n);
    //shift left (*2)
    d.high=d.high<<1 | d.low>>63; d.low=d.low<<1;
    p++;
  }
  uint128_add(&n,&d,&n);
  memset(ans,0,sizeof(uint128_t));
  for (;p>=64;p--) {
    if (!uint128_lt(&n,&d)) {
      uint128_sub(&n,&d,&n);
      ans->high |= 1LL<<(p-64);
    }
    //shift right (/2)
    d.low=d.low>>1|d.high<<63, d.high=d.high>>1;
  }
  for (;p>=0;p--) {
    if (!uint128_lt(&n,&d)) {
      uint128_sub(&n,&d,&n);
      ans->low |= 1LL<<p;
    }
    //shift right (/2)
    d.low=d.low>>1|d.high<<63, d.high=d.high>>1;
  }
#ifdef DEBUG
  fprintf(stderr,"  0x%016lx %016lxou\n",x->high,x->low);
  fprintf(stderr,"/ 0x%016lx %016lxou\n",y->high,y->low);
  fprintf(stderr,"= 0x%016lx %016lxou\n",ans->high,ans->low);
  fprintf(stderr,"R 0x%016lx %016lxou\n\n",n.high,n.low);
#endif
  return n.low==0 && n.high==0;
}

static inline int int128_div(const int128_t* x, const int128_t* d, int128_t* ans){
  uint128_t ux,ud;
  int mns = (int128_abs(x,(int128_t*)&ux) ^ int128_abs(d,(int128_t*)&ud));
  uint128_div(&ux,&ud,(uint128_t*)ans);
  if (mns) int128_minus(ans,ans);
  return 1;
}
