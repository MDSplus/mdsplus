#ifndef __LIBROUTINES_H__
#define __LIBROUTINES_H__

#include <mdsdescrip.h>
#include <mdstypes.h>
#include <time.h>

#ifndef LIBRTL_SRC
#define ZoneList void
#define LibTreeNode void
#endif

#include <inttypes.h>

/// @{
/// defined in MdsShr.so

extern void *LibCallg();
extern int LibCreateVmZone(ZoneList **const zone);
extern int LibDeleteVmZone(ZoneList **const zone);
extern int LibResetVmZone(ZoneList **const zone);
extern int LibFindFile(const mdsdsc_t *const filespec, mdsdsc_t *const result,
                       void **const ctx);
extern int LibFindFileCaseBlind(const mdsdsc_t *const filespec,
                                mdsdsc_t *const result, void **const ctx);
extern int LibFindFileEnd(void **const ctx);
extern int LibFindFileRecurseCaseBlind(const mdsdsc_t *const filespec,
                                       mdsdsc_t *const result,
                                       void **const ctx);
extern int LibFindImageSymbol();
extern int LibFindImageSymbol_C();
extern char *LibFindImageSymbolErrString();
extern int LibFreeVm(const uint32_t *const len, void **const vm,
                     ZoneList **const zone);
extern int libfreevm_(const uint32_t *const len, void **const vm,
                      ZoneList **const zone);
extern int libfreevm(const uint32_t *const len, void **const vm,
                     ZoneList **const zone);
extern int LibGetVm(const uint32_t *const len, void **const vm,
                    ZoneList **const zone);
extern int libgetvm_(const uint32_t *const len, void **const vm,
                     ZoneList **const zone);
extern int Libgetvm(const uint32_t *const len, void **const vm,
                    ZoneList **const zone);
extern int LibInsertTree(LibTreeNode **const treehead, void *const symbol_ptr,
                         int *const control_flags, int (*const compare_rtn)(),
                         int (*const alloc_rtn)(),
                         LibTreeNode **const blockaddr, void *const user_data);
extern int LibLookupTree(LibTreeNode **const treehead, void *const symbol_ptr,
                         int (*const compare_rtn)(),
                         LibTreeNode **const blockaddr);
extern int LibSpawn(const mdsdsc_t *const cmd, const int waitFlag,
                    const int notifyFlag);
extern time_t LibCvtTim(const int *const time_in, double *const t);
extern int LibConvertDateString(const char *asc_time, int64_t *const qtime);
extern int LibSysAscTim(length_t *const len, struct descriptor *const str,
                        const int *const time_in);
extern int LibTimeToVMSTime(const time_t *const time_in,
                            int64_t *const time_out);
extern int LibTraverseTree(LibTreeNode **treehead, int (*user_rtn)(),
                           void *user_data);
extern int LibWait(const float *const secs);
extern int libffs(const int *const position, const int *const size,
                  const char *const base, int *const find_position);
/// @}

#endif
