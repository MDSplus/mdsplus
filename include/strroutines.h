#ifdef __VMS
#include <str$routines.h>
#define StrAppend str$append
#define StrCaseBlindCompare str$case_blind_compare
#define StrCompare str$compare
#define StrConcat str$concat
#define StrCopyDx str$copy_dx
#define StrCopyR str$copy_r
#define StrElement str$element
#define StrFindFirstInSet str$find_first_in_set
#define StrFindFirstNotInSet str$find_first_not_in_set
#define StrFree1Dx str$free1_dx
#define StrGet1Dx str$get1_dx
#define StrLenExtr str$len_extr
#define StrMatchWild str$match_wild
#define StrPosition str$position
#define StrReplace str$replace
#define StrRight str$right
#define StrTranslate str$translate
#define StrTrim str$trim
#define StrUpcase str$upcase

#else
#include <config.h>
EXPORT extern int StrAppend();
EXPORT extern int StrCaseBlindCompare();
EXPORT extern int StrCompare();
EXPORT extern int StrConcat();
EXPORT extern int StrCopyDx();
EXPORT extern int StrCopyR();
EXPORT extern int StrElement();
EXPORT extern int StrFindFirstInSet();
EXPORT extern int StrFindFirstNotInSet();
EXPORT extern int StrFree1Dx();
EXPORT extern int StrGet1Dx();
EXPORT extern int StrLenExtr();
EXPORT extern unsigned int StrMatchWild();
EXPORT extern int StrPosition();
EXPORT extern int StrReplace();
EXPORT extern int StrRight();
EXPORT extern int StrTranslate();
EXPORT extern int StrTrim();
EXPORT extern int StrUpcase();

#endif

