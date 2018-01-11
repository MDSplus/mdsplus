#ifndef TDISHRP_H
#define TDISHRP_H

#include "tdirefcat.h"

int CvtConvertFloat(void const *input_value, const int input_type, void *output_value,
		    const int output_type);
int TdiAddOctaword(unsigned const int *a, unsigned const int *b, unsigned int *ans);
int TdiAddQuadword(unsigned const int *a, unsigned const int *b, unsigned int *ans);
int TdiBinary(struct descriptor *in1_ptr, struct descriptor *in2_ptr, struct descriptor *out_ptr,
	      unsigned int *out_count);
int TdiConvert(struct descriptor *pdin, struct descriptor_a *pdout);
int TdiCvtArgs(int narg, struct descriptor_xd dat[1], struct TdiCatStruct cats[1]);
int TdiGetArgs(void *dbid, int opcode, int narg, struct descriptor *list[],
	       struct descriptor_xd sig[], struct descriptor_xd uni[], struct descriptor_xd dat[],
	       struct TdiCatStruct cats[]);
int TdiGetFloat(struct descriptor *in_ptr, float *val_ptr);
int TdiGetRecord(int nid, struct descriptor_xd *out);
int TdiGetShape(int narg, struct descriptor_xd dat[1], unsigned short length, unsigned char dtype,
		int *cmode_ptr, struct descriptor_xd *out_ptr);
int TdiHash(int len, char *pstring);
int TdiMasterData(int nsig, struct descriptor_xd sig[1], struct descriptor_xd uni[1],
		  int *cmode_ptr, struct descriptor_xd *out_ptr);
int Tdi3Multiply(struct descriptor *in1, struct descriptor *in2, struct descriptor *out);
int TdiSubtractOctaword(unsigned int *a, unsigned int *b, unsigned int *ans);
int TdiSubtractQuadword(unsigned int *a, unsigned int *b, unsigned int *ans);
int TdiUnary(struct descriptor *in_ptr, struct descriptor *out_ptr, unsigned int *out_count);
void UseNativeFloat(struct TdiCatStruct *cat);
void TdiResetGetRecord();
#endif
