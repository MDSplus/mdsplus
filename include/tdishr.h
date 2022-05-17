#ifndef _TDISHR_H
#define _TDISHR_H
#include <mdsdescrip.h>
#include <mdstypes.h>
#include <status.h>
#include <tdishr_messages.h>

#define COM

#define OPC(name, ...) extern const opcode_t Opc##name;
#include <opcbuiltins.h>
#undef OPC

#define OPC(name, ...) extern int Tdi##name(struct descriptor *first, ...);
#include <opcbuiltins.h>
#undef OPC

#define OPC(name, ...) \
  extern int _Tdi##name(void **ctx, struct descriptor *first, ...);
#include <opcbuiltins.h>
#undef OPC

#undef COM

extern int TdiGetLong(struct descriptor *indsc, int *out_int);
extern int TdiGetFloat(struct descriptor *index, float *out_float);
extern int TdiConvert();
extern int TdiIntrinsic(opcode_t opcode, int narg, struct descriptor *list[],
                        struct descriptor_xd *out_ptr);
extern int _TdiIntrinsic(void **ctx, opcode_t opcode, int narg,
                         struct descriptor *list[],
                         struct descriptor_xd *out_ptr);
extern int CvtConvertFloat(void *invalue, uint32_t indtype, void *outvalue,
                           uint32_t outdtype, uint32_t options);
#endif