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

extern int StrAppend();
extern int StrCaseBlindCompare();
extern int StrCompare();
extern int StrConcat();
extern int StrCopyDx();
extern int StrCopyR();
extern int StrElement();
extern int StrFindFirstInSet();
extern int StrFindFirstNotInSet();
extern int StrFree1Dx();
extern int StrGet1Dx();
extern int StrLenExtr();
extern int StrMatchWild();
extern int StrPosition();
extern int StrReplace();
extern int StrRight();
extern int StrTranslate();
extern int StrTrim();
extern int StrUpcase();

#endif

