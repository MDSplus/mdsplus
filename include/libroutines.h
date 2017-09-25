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
#ifndef __LIBROUTINES_H__
#define __LIBROUTINES_H__

#include <time.h>
#include <mdsdescrip.h>
#include <mdstypes.h>

#ifndef LIBRTL_SRC
#define ZoneList void
#define LibTreeNode void
#define FindFileCtx void
#endif

/// @{
/// defined in MdsShr.so

extern void *LibCallg();
extern int LibConvertDateString(const char *asc_time, int64_t * qtime);
extern int LibCreateVmZone(ZoneList ** zone);
extern time_t LibCvtTim(int *time_in, double *t);
extern int LibDeleteVmZone(ZoneList ** zone);
extern int LibFindFile(struct descriptor *filespec, struct descriptor *result, FindFileCtx **ctx);
extern int LibFindFileEnd(FindFileCtx **ctx);
extern int LibFindFileRecurseCaseBlind(struct descriptor *filespec,
				       struct descriptor *result, FindFileCtx **ctx);
extern int LibFindImageSymbol();
extern int LibFindImageSymbol_C(const char *filename, const char *symbol, void **symbol_value);
extern char *LibFindImageSymbolErrString();
extern int LibFreeVm(unsigned int *len, void **vm, ZoneList ** zone);
extern int libfreevm_(unsigned int *len, void **vm, ZoneList ** zone);
extern int libfreevm(unsigned int *len, void **vm, ZoneList ** zone);
extern int LibGetVm(unsigned int *len, void **vm, ZoneList ** zone);
extern int libgetvm_(unsigned int *len, void **vm, ZoneList ** zone);
extern int Libgetvm(unsigned int *len, void **vm, ZoneList ** zone);
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

/// @}

#endif
