#pragma once
#include <mdstypes.h>
#include <mdsdescrip.h>

#ifdef COM
#undef COM
#endif
#define COM

#define OPC(name,builtin,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11) extern int Tdi##name ( struct descriptor *first, ... );
#include <opcbuiltins.h>
#undef OPC
#define OPC(name,builtin,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11) extern unsigned short Opc##name;
#include <opcbuiltins.h>
#undef OPC
#undef COM

extern int TdiGetLong(struct descriptor *indsc, int *out_int);
extern int TdiGetFloat(struct descriptor *index, float *out_float);
extern int CvtConvertFloat(void *invalue, uint32_t indtype, void *outvalue, uint32_t outdtype, uint32_t options);
