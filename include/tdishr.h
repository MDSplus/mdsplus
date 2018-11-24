#pragma once
#include <mdstypes.h>
#include <mdsdescrip.h>
#include <status.h>
#include <tdishr_messages.h>

#ifdef COM
#undef COM
#endif
#define COM


#define COM
#define OPC(name,NAME, ...) OPC_##NAME,
typedef enum opcode_e {
#include <opcbuiltins.h>
  OPC_INVALID=-1
} opcode_t;
#undef OPC

# define OPC(name, ...) extern const unsigned short Opc##name;
# include <opcbuiltins.h>
#undef OPC

#define OPC(name, ...) extern int Tdi##name(struct descriptor* first, ...);
#include <opcbuiltins.h>
#undef OPC

#undef COM

extern int TdiGetLong(struct descriptor *indsc, int *out_int);
extern int TdiGetFloat(struct descriptor *index, float *out_float);
extern int TdiConvert();
extern int TdiIntrinsic(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr);
extern int CvtConvertFloat(void *invalue, uint32_t indtype, void *outvalue, uint32_t outdtype, uint32_t options);
