/*	Tdi1Trim.C
	Generic transformation with single string output.

	Ken Klare, LANL P-4	(c)1989,1990,1991
*/
#include "tdinelements.h"
#include "tdirefcat.h"
#include "tdireffunction.h"
#include "tdirefstandard.h"
#include <strroutines.h>
#include <string.h>
#include <tdimessages.h>
#include <stdlib.h>
#include <mdsshr.h>
#include <mds_stdarg.h>

extern int MdsFree1Dx();
extern int MdsCopyDxXd();
extern int TdiGetLong();
extern int TdiGetArgs();
extern int TdiCvtArgs();
extern int TdiMasterData();

TdiRefStandard(Tdi1Trim)
struct descriptor_xd	sig[1], uni[1], dat[1];
struct TdiCatStruct		cats[2];
struct TdiFunctionStruct	*fun_ptr = (struct TdiFunctionStruct *)&TdiRefFunction[opcode];
int				j, cmode = -1;

	uni[0] = EMPTY_XD;
	status = TdiGetArgs(opcode, narg, list, sig, uni, dat, cats);
	if (status & 1) status = TdiCvtArgs(narg, dat, cats);
	if (status & 1 && narg > 0) {
		N_ELEMENTS(dat[0].pointer, j);
		if (status & 1 && j != 1) status = TdiINVCLADSC;
	}
	/***********************
	Go off and do something.
	***********************/
	if (status & 1) status = (*fun_ptr->f3)(dat[0].pointer, out_ptr);
	if (status & 1) status = TdiMasterData(narg, sig, uni, &cmode, out_ptr);
	if (narg > 0) {
		if (sig[0].pointer) MdsFree1Dx(&sig[0], NULL);
		if (uni[0].pointer) MdsFree1Dx(&uni[0], NULL);
		if (dat[0].pointer) MdsFree1Dx(&dat[0], NULL);
	}
	return status;
}
/*--------------------------------------------------------------
	F90 transformation, string trailing blanks removed.
*/
int				Tdi3Trim(
struct descriptor		*in_ptr,
struct descriptor_xd	*out_ptr)
{
int	n = in_ptr->length;
char	*t_ptr = in_ptr->pointer + n;

	for (; n > 0; --n) if (*--t_ptr != ' ' && *t_ptr != '\t') break;
	in_ptr->length = (unsigned short)n;
	return MdsCopyDxXd(in_ptr, out_ptr);
}
/*--------------------------------------------------------------
	Convert opcode to builtin name.
*/
int				Tdi3OpcodeBuiltin(
struct descriptor		*in_ptr,
struct descriptor_xd	*out_ptr)
{
unsigned int	ind = 0xffffffff;
int		status;
static unsigned char dtype = (unsigned char)DTYPE_T;
	status = TdiGetLong(in_ptr, &ind);
	if (status & 1 && ind < (unsigned int)TdiFUNCTION_MAX) {
	char *name_ptr = TdiRefFunction[ind].name;
	struct descriptor str2 = {0,DTYPE_T,CLASS_S,0};
                str2.length = (unsigned short)strlen(name_ptr);
                str2.pointer = name_ptr;
		status = MdsGet1DxS(&str2.length, &dtype, out_ptr);
		if (status & 1) status = StrCopyDx(out_ptr->pointer, &str2);
	}
	else if (status & 1) status = TdiINV_OPC;
	return status;
}
/*--------------------------------------------------------------
	Convert opcode to string name.
*/
int				Tdi3OpcodeString(
struct descriptor		*in_ptr,
struct descriptor_xd	*out_ptr)
{
static DESCRIPTOR(str1,"OPC""$");
unsigned int	ind = 0xffffffff;
int		status;
static unsigned char dtype = (unsigned char)DTYPE_T;
	status = TdiGetLong(in_ptr, &ind);
	if (status & 1 && ind < (unsigned int)TdiFUNCTION_MAX) {
	char *name_ptr = TdiRefFunction[ind].name;
	struct descriptor str2 = {0,DTYPE_T,CLASS_S,0};
	unsigned short total;
                str2.length = (unsigned short)strlen(name_ptr);
                str2.pointer = name_ptr;
	        total = (unsigned short)(str1.length + str2.length);
		status = MdsGet1DxS(&total, &dtype, out_ptr);
		if (status & 1) status = StrConcat(out_ptr->pointer, &str1, &str2 MDS_END_ARG);
	}
	else if (status & 1) status = TdiINV_OPC;
	return status;
}
