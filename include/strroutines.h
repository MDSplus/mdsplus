#ifndef STRROUTINES_H

#define STRROUTINES_H

#include <config.h>
#include <mdsdescrip.h>
EXPORT extern int StrAppend();
EXPORT extern int StrCaseBlindCompare();
EXPORT extern int StrCompare();
EXPORT extern int StrConcat(struct descriptor *out, struct descriptor *first, ...);
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
