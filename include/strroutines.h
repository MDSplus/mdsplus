#pragma once

#include <mdsdescrip.h>

extern int StrAppend(struct descriptor_d *out, struct descriptor *tail);
extern int StrCaseBlindCompare(struct descriptor *one, struct descriptor *two);
extern int StrCompare(struct descriptor *str1, struct descriptor *str2);
extern int StrConcat(struct descriptor *out, ...);
extern int StrCopyDx(struct descriptor *out, const struct descriptor *in);
extern int StrCopyR(struct descriptor *dest, const unsigned short *len, char *source);
extern int StrElement(struct descriptor *dest, int *num, struct descriptor *delim, struct descriptor *src);
extern int StrFindFirstInSet(struct descriptor *source, struct descriptor *set);
extern int StrFindFirstNotInSet(struct descriptor *source, struct descriptor *set);
extern int StrFree1Dx(struct descriptor_d *inout);
extern int StrGet1Dx(const unsigned short *len, struct descriptor_d *out);
extern int StrLenExtr(struct descriptor *dest, struct descriptor *source, int *start_in, int *len_in);
extern unsigned int StrMatchWild(struct descriptor *candidate, struct descriptor *pattern);
extern int StrPosition(struct descriptor *source, struct descriptor *substring, int *start);
extern int StrReplace(struct descriptor *dest, struct descriptor *src, int *start_idx, int *end_idx,
		      struct descriptor *rep);
extern int StrRight(struct descriptor *out, struct descriptor *in, unsigned short *start);
extern int StrTranslate(struct descriptor *dest, struct descriptor *src, struct descriptor *tran,
			struct descriptor *match);
extern int StrTrim(struct descriptor *out, struct descriptor *in, unsigned short *lenout);
extern int StrUpcase(struct descriptor *out, struct descriptor *in);

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
static void __attribute__((unused)) free_d(void *ptr){
  StrFree1Dx((struct descriptor_d*)ptr);
}
#define FREED_ON_EXIT(ptr) pthread_cleanup_push(free_d, ptr)
#define FREED_IF(ptr,c)    pthread_cleanup_pop(c)
#define FREED_NOW(ptr)     FREED_IF(ptr,1)
#define FREED_CANCEL(ptr)  FREED_IF(ptr,0)
#define INIT_AS_AND_FREED_ON_EXIT(var,value) struct descriptor_d var=value;FREED_ON_EXIT(&var)
#define INIT_AND_FREED_ON_EXIT(var,dtype)    INIT_AS_AND_FREED_ON_EXIT(var, ((struct descriptor_d){ 0, dtype, CLASS_D, NULL}))
#endif
