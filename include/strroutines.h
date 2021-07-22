#ifndef _STRROUTINES_H
#define _STRROUTINES_H

#include <mdsdescrip.h>

extern int StrAppend(mdsdsc_d_t *const out, const mdsdsc_t *const tail);
extern int StrCaseBlindCompare(const mdsdsc_t *const str1,
                               const mdsdsc_t *const str2);
extern int StrCompare(const mdsdsc_t *const str1, const mdsdsc_t *const str2);
extern int StrConcat(mdsdsc_t *const out, ...);
extern int StrCopyDx(mdsdsc_t *const out, const mdsdsc_t *const in);
extern int StrLenExtr(mdsdsc_t *const out, const mdsdsc_t *const in,
                      const int *const start_in, const int *const len_in);
extern int StrReplace(mdsdsc_t *const out, const mdsdsc_t *const in,
                      const int *const start_idx, const int *const end_idx,
                      const mdsdsc_t *const rep);
extern int StrRight(mdsdsc_t *const out, const mdsdsc_t *const in,
                    const length_t *const start);
extern int StrTranslate(mdsdsc_t *const out, const mdsdsc_t *const in,
                        const mdsdsc_t *const tran, mdsdsc_t *const match);
extern int StrTrim(mdsdsc_t *const out, const mdsdsc_t *const in,
                   length_t *const lenout);
extern int StrUpcase(mdsdsc_t *const out, const mdsdsc_t *const in);
extern int StrCopyR(mdsdsc_t *const out, const length_t *const len,
                    char *const in);
extern int StrElement(mdsdsc_t *const out, const int *const num,
                      const mdsdsc_t *const delim, const mdsdsc_t *const in);
extern int StrFindFirstInSet(const mdsdsc_t *const source,
                             const mdsdsc_t *const set);
extern int StrFindFirstNotInSet(const mdsdsc_t *const source,
                                const mdsdsc_t *const set);
extern int StrFree1Dx(mdsdsc_d_t *const inout);
extern int StrGet1Dx(const length_t *const len, mdsdsc_d_t *const out);
extern int StrMatchWild(const mdsdsc_t *const candidate,
                        const mdsdsc_t *const pattern);
extern int StrPosition(const mdsdsc_t *const source,
                       const mdsdsc_t *const substring, const int *const start);

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
static void __attribute__((unused)) free_d(void *ptr)
{
  StrFree1Dx((mdsdsc_d_t *)ptr);
}
#define FREED_ON_EXIT(ptr) pthread_cleanup_push(free_d, ptr)
#define FREED_IF(ptr, c) pthread_cleanup_pop(c)
#define FREED_NOW(ptr) FREED_IF(ptr, 1)
#define FREED_CANCEL(ptr) FREED_IF(ptr, 0)
#define INIT_AS_AND_FREED_ON_EXIT(var, value) \
  mdsdsc_d_t var = value;                     \
  FREED_ON_EXIT(&var)
#define INIT_AND_FREED_ON_EXIT(var, dtype) \
  INIT_AS_AND_FREED_ON_EXIT(var, ((mdsdsc_d_t){0, dtype, CLASS_D, NULL}))
#endif
#endif