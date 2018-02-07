#pragma once

#ifdef WORDS_BIGENDIAN
 typedef struct int128_s{
   uint64_t high;
   uint64_t low;
 } int128_t;
 const int128_t int128_one = {1,0};
#else
 typedef struct int128_s{
   uint64_t low;
   uint64_t high;
 } int128_t;
 const int128_t int128_one = {0,1};
#endif
const int128_t int128_zero = {0,0};
#define HI_INT 0xFFFFFFFF00000000LL
#define LO_INT 0x00000000FFFFFFFFLL

static inline void int128_minus(const int128_t *a, int128_t *ans){
  ans->high = ~(a->high);
  ans->low  = ~(a->low);
  ans->low++;
  if (ans->low == 0)
    ans->high++;
}

static inline int int128_add(const int128_t *a, const int128_t *b, int128_t *ans){
  int128_t aa;memcpy(&aa,a,sizeof(int128_t));
  ans->low  = a->low  + b->low;
  ans->high = a->high + b->high;
  if (ans->low < aa.low)
    ans->high++;
  return ans->high < aa.high;
}

static inline int int128_sub(const int128_t* a, const int128_t* b, int128_t *ans){
  int128_t aa;memcpy(&aa,a,sizeof(int128_t));
  ans->low  = a->low  - b->low;
  ans->high = a->high - b->high;
  if (ans->low > aa.low)
    ans->high--;
  return ans->high > aa.high;
}

static inline int int128_mul(const int128_t* x, const int128_t* y, int128_t *ans){
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
  return  (acc >> 32LL) | (carry << 32LL);
}

/*
int int128_div(const int128_t x, const int128_t d, int128_t *r)
{
  int s;
  int128_t d1, p2, rv;

//printf("divide %.16llX %016llX / %.16llX %016llX\n", x.high, x.low, d.high, d.low);

  // check for divide by zero
  if ((d.low == 0) && (d.high == 0)) {
    rv.low = x.low / d.low; // This will cause runtime error
  }

  s = 1;
  if (x < ((s128_o) 0)) {
    // notice that MININT will be unchanged, this is used below.
    s = - s;
    x = - x;
  }
  if (d < ((s128_o) 0)) {
    s = - s;
    d = - d;
  }

  if (d == ((s128_o) 1)) {
    // This includes the overflow case MININT/-1
    rv = x;
    x = 0;
  } else if (x < ((s128_o) d)) {
    // x < d, so quotient is 0 and x is remainder
    rv = 0;
  } else {
    rv = 0;

    // calculate biggest power of 2 times d that's <= x
    p2 = 1; d1 = d;
    x = x - d1;
    while(x >= d1) {
      x = x - d1;
      d1 = d1 + d1;
      p2 = p2 + p2;
    }
    x = x + d1;

    while(p2.low != 0 || p2.high != 0) {
      if (x >= d1) {
        x = x - d1;
        rv = rv + p2;
      }
      p2 = s128_shr(p2);
      d1 = s128_shr(d1);
    }
  }

  if (s < 0) rv.high = - rv.high;
  if (r)     *r = x;
  retrun rv;
}
*/
