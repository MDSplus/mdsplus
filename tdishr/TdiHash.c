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
#include <STATICdef.h>
#include "tdithreadsafe.h"
#define TdiHASH_MAX 991

STATIC_THREADSAFE short TdiREF_HASH[TdiHASH_MAX];
#include "tdireffunction.h"
#include <string.h>
#include <stdio.h>



#define upcase(ptr) ((*(char *)(ptr) >= 'a' && *(char *)(ptr) <= 'z') ? *(char *)(ptr) & 0xdf : *(char *)(ptr))

STATIC_ROUTINE int TdiHashOne(int len, char *pstring)
{
  int hash = 0, n = len;
  char *ps = pstring;
  while (--n >= 0 && *ps) {
    hash = (int)(((uint64_t)hash) * 11 + (upcase(ps) - ' '));
    ps++;
  }
  hash %= TdiHASH_MAX;
  if (hash < 0)
    hash += TdiHASH_MAX;
  return hash;
}

STATIC_ROUTINE int TdiHashAll()
{
  int jf, jh;
  struct TdiFunctionStruct *pf = (struct TdiFunctionStruct *)TdiRefFunction;
  int count = 0;
  for (jh = TdiHASH_MAX; --jh >= 0;)
    TdiREF_HASH[jh] = -1;
  for (jf = 0; jf < TdiFUNCTION_MAX; ++jf, pf++) {
    if (pf->name) {
      int i;
      int len = strlen(pf->name);
      char temp[128];
      strcpy(temp, pf->name);
      for (i = 0; i < len; i++)
	if (temp[i] >= 'a' && temp[i] <= 'z')
	  temp[i] = (char)(temp[i] & 0xDF);
      jh = TdiHashOne(63, temp);
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

int TdiHash(int len, char *pstring)
{
  int jh, jf;
  static pthread_mutex_t lock;
  pthread_mutex_lock(&lock);
  pthread_cleanup_push((void*)pthread_mutex_unlock,&lock);
  jh = TdiHashOne(len, pstring);
  if (TdiREF_HASH[0] == 0)
    TdiHashAll();
  while ((jf = TdiREF_HASH[jh]) >= 0) {
    int i;
    char *name = TdiRefFunction[jf].name;
    for (i = 0; (i < len) && (name[i] != '\0') && (upcase(pstring + i) == name[i]); i++) ;
    if (i == len && name[i] == '\0')
      break;
    if (++jh >= TdiHASH_MAX)
      jh = 0;
  }
  pthread_cleanup_pop(1);
  return jf;
}

/*      test program for hashing
        Also change the TdiHASH_MAX to a variable.
int     main() {
int     count, jf, k;
        printf(" size  depth\n");
        for (TdiHASH_MAX = 1025; --TdiHASH_MAX > TdiFUNCTION_MAX;) {
                count = TdiHashAll();
                printf("%5d  %g\n", TdiHASH_MAX, (float)count / (float)TdiHASH_MAX);
                TdiREF_HASH[0] = 0;
        }
        TdiHASH_MAX = 991;
        for (jf = 0; jf < TdiFUNCTION_MAX; ++jf)
                if (jf != (k = TdiHash(99, TdiRefFunction[jf].name)))
                        printf("%d  %s  %d\n", jf, TdiRefFunction[jf], k);
}
*/
