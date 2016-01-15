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

STATIC_THREADSAFE int lock_initialized = 0;
STATIC_THREADSAFE pthread_mutex_t lock;
STATIC_THREADSAFE short TdiREF_HASH[1024];	/*TdiHASH_MAX]; */
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
    //__builtin_smul_overflow(hash, 11, &hash);
    //__builtin_sadd_overflow(hash, (upcase(ps) - ' '), &hash);
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

	/**************
        Self-initialize
        **************/
  LockTdiMutex(&lock, &lock_initialized);
  jh = TdiHashOne(len, pstring);
  if (TdiREF_HASH[0] == 0)
    TdiHashAll();

  while ((jf = TdiREF_HASH[jh]) >= 0) {
    int i;
    char *name = TdiRefFunction[jf].name;
    for (i = 0; (i < len) && (name[i] != '\0') && (upcase(pstring + i) == name[i]); i++) ;
    if (i == len && name[i] == '\0')
      break;
/*
                if (strncmp(pstring, TdiRefFunction[jf].name, len) == 0) {
                        k = strlen(TdiRefFunction[jf].name);
                        if (k == len || *(pstring+k) == '\0') break;
                }
*/
    if (++jh >= TdiHASH_MAX)
      jh = 0;
  }
  UnlockTdiMutex(&lock);
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
