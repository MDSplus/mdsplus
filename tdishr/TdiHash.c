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
/*      TdiHash.C
	Use hashing method to lookup a function name with fewest string compares.
	The choice of size will vary but should be a few times larger than the number of names.
	For 441 entries,  I found 991 gave .11 failure rate on first try.
	Near 800 .23
	Near 700 .4
	Near 600 .7
	Near 500 2 (3 string compares on the average)
	        index = TdiHash(string) to find the index.
*/
#include "tdirefcat.h"
#include "tdireffunction.h"
#include <pthread_port.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

//#define MAIN
#ifdef MAIN
  #define MAIN_MAX 32767
  int TdiHASH_MAX;
  static short TdiREF_HASH[MAIN_MAX];
#else
  #define TdiHASH_MAX 10354
  static short TdiREF_HASH[TdiHASH_MAX];
#endif

int hash(const int len, const char *str) {
  // djb2 by dan bernstein (http://www.cse.yorku.ca/~oz/hash.html)
  unsigned int hash = 5381;
  if (len<0) {// len unknown
    const char *c;
    for (c = str; *c ; c++)
      hash = ((hash << 5) + hash) + (unsigned)toupper(*c); /* hash * 33 + c */
  } else {
    const char *c = str, *e = str+len;
    for (; c<e ; c++)
      hash = ((hash << 5) + hash) + (unsigned)toupper(*c); /* hash * 33 + c */
  }
  return (signed)(hash%TdiHASH_MAX);
}

#ifndef MAIN // optimized _ only if TdiHASH_MAX such that hash has no collisions
static void hash_all() {
 short i;
  for (i = 0 ; i < TdiHASH_MAX ; i++)
    TdiREF_HASH[i] = -1;
 for (i = 0; i < TdiFUNCTION_MAX; i++)
   TdiREF_HASH[hash(-1, TdiRefFunction[i].name)] = i;
}
int tdiHash(const int len, const char *const pstring) {
  static pthread_once_t once = PTHREAD_ONCE_INIT;
  pthread_once(&once,hash_all);
  const int jf = TdiREF_HASH[hash(len, pstring)];
  if (jf<0)			return -1; // pstring's hash did not match a buildin's
  const char *rf = TdiRefFunction[jf].name;
  const char *ps=pstring, *pe=pstring+len;
  for (; ps<pe ; ps++, rf++) {
    if (!*rf)			return -1; // pstring is too long
    if (*rf != toupper(*ps))	return -1; // pstirng is different
  }
  if (*rf)			return -1; // pstring is too short
  return jf;
}
#else
static int hash_all() {
  int count = 0;
  int jf, jh;
  struct TdiFunctionStruct *pf = (struct TdiFunctionStruct *)TdiRefFunction;
  for (jh = TdiHASH_MAX; --jh >= 0;)
    TdiREF_HASH[jh] = -1;
  for (jf = 0; jf < TdiFUNCTION_MAX; ++jf, pf++) {
    if (pf->name) {
      jh = hash(-1, pf->name);
      TdiREF_HASH[jh] = (short)jf;
      while (TdiREF_HASH[jh] >= 0) {
	count++;
	if (++jh >= TdiHASH_MAX)
	  jh = 0;
      }
      TdiREF_HASH[jh] = (short)jf;
    } else
      printf("Error in function table, function index = %d\n", jf);
  }
  return count;
}
int tdiHash(const int len, const char *const pstring) {
  // used in TdiLex and TdiChar
  int i, jh, jf;
  jh = hash(len, pstring);
  while ((jf = TdiREF_HASH[jh]) >= 0) {
    for (i = 0 ; i<len && TdiRefFunction[jf].name[i] ; i++) {
      if (TdiRefFunction[jf].name[i] != toupper(pstring[i]))
        goto next;
    }
    if (i==len && !TdiRefFunction[jf].name[len])
      break;
next: ;
    if (++jh >= TdiHASH_MAX)
      jh = 0;
  }
  return jf;
}
/*      test program for hashing
	Also change the TdiHASH_MAX to a variable.
*/
int main() {
  int count, jf, k;
  int max = 0x7fffffff, max_val = max;
  printf(" size  depth\n");
  for (TdiHASH_MAX = TdiFUNCTION_MAX; TdiHASH_MAX < MAIN_MAX; TdiHASH_MAX++) {
    count = hash_all();
    if (count<max) {
      printf("%5d  %d\n", max_val = TdiHASH_MAX, max = count);
      if (count==0) break;
    }
  }
  TdiHASH_MAX = max_val;
  count = hash_all();
  for (jf = 0; jf < TdiFUNCTION_MAX; ++jf)
    if (jf != (k = TdiHash(strlen(TdiRefFunction[jf].name), TdiRefFunction[jf].name)))
      printf("%d  %s  %d\n", jf, TdiRefFunction[jf].name, k);
  printf("%5d  %g\n", TdiHASH_MAX, (float)max / (float)TdiHASH_MAX);
  return 0;
}
#endif
