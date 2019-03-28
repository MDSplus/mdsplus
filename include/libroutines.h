#ifndef __LIBROUTINES_H__
#define __LIBROUTINES_H__

#include <time.h>
#include <mdsdescrip.h>
#include <mdstypes.h>

#ifndef LIBRTL_SRC
#define ZoneList void
#define LibTreeNode void
#endif

#include <inttypes.h>
//#ifdef WINDOWS_H
//#include <stdint.h>
//#endif


/// @{
/// defined in MdsShr.so

extern void *LibCallg();
extern int LibConvertDateString(const char *asc_time, int64_t * qtime);
extern int LibCreateVmZone(ZoneList **const zone);
extern time_t LibCvtTim(int *time_in, double *t);
extern int LibDeleteVmZone(ZoneList **const zone);
extern int LibFindFile(mdsdsc_t *filespec, mdsdsc_t *result, void **ctx);
extern int LibFindFileCaseBlind(mdsdsc_t *filespec, mdsdsc_t *result, void **ctx);
extern int LibFindFileEnd(void **ctx);
extern int LibFindFileRecurseCaseBlind(mdsdsc_t *filespec, mdsdsc_t *result, void **ctx);
extern int LibFindImageSymbol();
extern int LibFindImageSymbol_C();
extern char *LibFindImageSymbolErrString();
extern int LibFreeVm (const uint32_t *const len, void **const vm, ZoneList **const zone);
extern int libfreevm_(const uint32_t *const len, void **const vm, ZoneList **const zone);
extern int libfreevm (const uint32_t *const len, void **const vm, ZoneList **const zone);
extern int LibGetVm (const uint32_t *const len, void **const vm, ZoneList **const zone);
extern int libgetvm_(const uint32_t *const len, void **const vm, ZoneList **const zone);
extern int Libgetvm (const uint32_t *const len, void **const vm, ZoneList **const zone);
extern int LibInsertTree(LibTreeNode **treehead, void *symbol_ptr, int *control_flags,
			 int (*compare_rtn) (), int (*alloc_rtn) (), LibTreeNode **blockaddr,
			 void *user_data);
extern int LibLookupTree(LibTreeNode **treehead, void *symbolstring, int (*compare_rtn) (),
		  LibTreeNode **blockaddr);
extern int LibResetVmZone(ZoneList ** zone);
extern int LibSpawn(struct descriptor *cmd, int waitFlag, int notifyFlag);
extern int LibSysAscTim(unsigned short *len, struct descriptor *str, int *time_in);
extern int LibTraverseTree(LibTreeNode **treehead, int (*user_rtn) (), void *user_data);
extern int LibWait(const float *secs);
extern int LibTimeToVMSTime(const time_t * time_in, int64_t * time_out);
extern int libffs(int *position, int *size, char *base, int *find_position);
extern uint32_t LibGetHostAddr(char *host);

/// @}

#endif
