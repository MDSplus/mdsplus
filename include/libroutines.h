#ifndef __LIBROUTINES_H__
#define __LIBROUTINES_H__

#include <config.h>

/// @{
/// defined in MdsShr.so

EXPORT extern int LibAddx();
EXPORT extern void *LibCallg();
EXPORT extern int LibConvertDateString();
EXPORT extern int LibCreateVmZone();
EXPORT extern int LibDeleteVmZone();
EXPORT extern int LibEmul();
EXPORT extern int LibEstablish();
EXPORT extern int LibFindFile();
EXPORT extern int LibFindFileEnd();
EXPORT extern int LibFindFileRecurseCaseBlind();
EXPORT extern int LibFindImageSymbol();
EXPORT extern int LibFindImageSymbol_C();
EXPORT extern char *LibFindImageSymbolErrString();
EXPORT extern char *LibFindImageSymbolErrStr();
EXPORT extern int LibFindVmZone();
EXPORT extern int LibFreeVm();
EXPORT extern int LibGetVm();
EXPORT extern int libgetvm_();
EXPORT extern int libfreevm_();
EXPORT extern int LibInsertTree();
EXPORT extern int LibLookupTree();
EXPORT extern int LibResetVmZone();
EXPORT extern int LibSFree1Dd();
EXPORT extern int LibShowVmZone();
EXPORT extern int LibSigToRet();
EXPORT extern int LibSpawn();
EXPORT extern int LibSubx();
EXPORT extern int LibSysAscTim();
EXPORT extern int LibTraverseTree();
EXPORT extern int LibWait(float const *);
EXPORT extern int LibTimeToVMSTime();
EXPORT extern int libffs(int *position, int *size, char *base, int *find_position);
EXPORT extern int LibFindImageSymbol_C(const char *image, const char *routine, void **rtn);

/// @}

#endif
