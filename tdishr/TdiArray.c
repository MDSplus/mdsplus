/*	Tdi1Array.C
	Generic routine to create shaped and molded scalar or array.
		ARRAY	([size], [mold])
		RAMP	([size], [mold])
		RANDOM	([size], [mold])
		ZERO	([size], [mold])
	Size is missing for a scalar.
	Size is scalar for simple array.
	Size is vector for array with multipliers.
	Type and length given by mold, if present, otherwise defaulted.
	Unlikely example, ARRAY([COMPLEX(2,11),[3,4,5],6.7],1d0)
	returns an array of doubles of dimensions [2,3,4,5,6].

	Limitation: number of dimensions must not exceed MAXDIM.
	Limitation: product of dimensions must not exceed virtual-memory paging space.
	Ken Klare, LANL CTR-7	(c)1989,1990
*/
#define MAXDIM 8
#include <tdimessages.h>
#include <mdsshr.h>
#include <stdlib.h>
#include "tdinelements.h"
#include "tdirefcat.h"
#include "tdireffunction.h"
#include "tdirefstandard.h"

int Tdi_RandomSeed = 1234567;

#ifdef __DECC
#pragma member_alignment save
#pragma nomember_alignment
#endif
typedef ARRAY_COEFF(char, MAXDIM) array_coeff;
typedef ARRAY(int) array_int;
#ifdef __DECC
#pragma member_alignment restore
#endif

extern int TdiData();
extern int TdiConvert();
extern int Tdi3Add();
extern int MdsFree1Dx();
extern int CvtConvertFloat();

TdiRefStandard(Tdi1Array)
static DESCRIPTOR_A_COEFF(	arr0,sizeof(int),DTYPE_L,0,MAXDIM,0);
static DESCRIPTOR_A(		cvt0,sizeof(int),DTYPE_L,0,0);
array_coeff arr = *(array_coeff *)&arr0;
array_int cvt = *(array_int *)&cvt0;
struct TdiFunctionStruct	*fun_ptr = (struct TdiFunctionStruct *)&TdiRefFunction[opcode];
struct descriptor_xd	tmp = EMPTY_XD;
unsigned short length;
unsigned char dtype;
int				j, ndim=0;

	/****************************************
	Get dimensions given and make into longs.
	****************************************/
	if (narg <= 0 || list[0] == 0) arr.class = CLASS_S;
	else {
		status = TdiData(list[0], &tmp MDS_END_ARG);
		if (status & 1) N_ELEMENTS(tmp.pointer, ndim);
		if (status & 1) {
			arr.dimct = (unsigned char)ndim;
			arr.aflags.coeff = (unsigned char)(tmp.pointer->class == CLASS_A);
			arr.a0 = 0;
			if (ndim > MAXDIM) status = TdiNDIM_OVER;
			else {
				cvt.pointer = (int *)&arr.m[0];
				cvt.arsize = sizeof(int) * ndim;
				status = TdiConvert(tmp.pointer, &cvt MDS_END_ARG);
			}
		}
	}

	/*****************************
	Data type from opcode or mold.
	*****************************/
	if (narg <= 1 || list[1] == 0) {
		dtype = fun_ptr->o1;
		length = TdiREF_CAT[dtype].length;
	}
	else switch (list[1]->class) {
	case CLASS_S :
	case CLASS_D :
	case CLASS_A :
	case CLASS_CA :
		dtype = list[1]->dtype;
		length = list[1]->length;
		break;
	default :
		if (status & 1) status = TdiData(list[1], &tmp MDS_END_ARG);
		if (status & 1) {
			dtype = tmp.pointer->dtype;
			length = tmp.pointer->length;
		}
		break;
	}
	MdsFree1Dx(&tmp, NULL);

	/*****************************
	Size is product of dimensions.
	*****************************/
	if (status & 1) {
		if (arr.class == CLASS_A) {
 			for (arr.arsize = sizeof(int), j = ndim; --j >= 0;) arr.arsize *= arr.m[j];
			status = MdsGet1DxA((struct descriptor_a *)&arr, &length, &dtype, out_ptr);
		}
		else	status = MdsGet1DxS(&length, &dtype, out_ptr);
	}

	/*********************
	Put some data into it.
	*********************/
	if (status & 1) status = (*fun_ptr->f3)(out_ptr->pointer);
	return status;
}
/*---------------------------------------------------------------------
	Create un-initialized array.
		var = ARRAY([size-vector], [mold-type])
*/
int		Tdi3Array(
int		*out_ptr)
{
	return 1;
}
/*---------------------------------------------------------------------
	Create a ramp of integers starting from zero.
*/
int			Tdi3Ramp(
struct descriptor	*out_ptr)
{
static int	i0 = 0, i1 = 1;
static struct descriptor con0 = {sizeof(int),DTYPE_L,CLASS_S,(char *)&i0};
static struct descriptor con1 = {sizeof(int),DTYPE_L,CLASS_S,(char *)&i1};
int	status = 1, n;
int i;

#define LoadRamp(type) { type *ptr = (type *)out_ptr->pointer; for (i=0;i<n;i++) ptr[i] = (type)i; break;}
#define LoadRampF(type,dtype,native) { type *ptr = (type *)out_ptr->pointer; type tmp; \
                                       for (i=0;i<n;i++) {tmp = (type)i; CvtConvertFloat(&tmp,native,&ptr[i],dtype,0);} break;}

	N_ELEMENTS(out_ptr, n);
	switch (out_ptr->dtype) {

	case DTYPE_B  : case DTYPE_BU :	LoadRamp(char)
	case DTYPE_W  : case DTYPE_WU :	LoadRamp(short)
	case DTYPE_L  : case DTYPE_LU :	LoadRamp(int)
	case DTYPE_F  : LoadRampF(float,DTYPE_F,DTYPE_FLOAT) 
	case DTYPE_FS : LoadRampF(float,DTYPE_FS,DTYPE_FLOAT) 
	case DTYPE_D  : LoadRampF(double,DTYPE_D,DTYPE_DOUBLE)
	case DTYPE_G  : LoadRampF(double,DTYPE_G,DTYPE_DOUBLE)
	case DTYPE_FT : LoadRampF(double,DTYPE_FT,DTYPE_DOUBLE)

	/**********************************************************
	WARNING this depends on order of operations in ADD routine.
	Make a zero and a one. Add 1 to this starter, but offset.
	**********************************************************/
	default : {
		struct descriptor new = *out_ptr;
			new.class = CLASS_S;
			if (n > 0) status = TdiConvert(&con0, &new MDS_END_ARG);
			if (n > 1) {
			int step = new.length;
				new.pointer += step;
				if (status & 1) status = TdiConvert(&con1, &new MDS_END_ARG);
				if (n > 2) {
				struct descriptor_a modify = *(struct descriptor_a *)out_ptr;
					modify.pointer += step;
					modify.arsize -= step;
					if (status & 1) status = Tdi3Add(out_ptr, &new, &modify);
				}
			}
		}
		break;
	}
	return status;
}
/*---------------------------------------------------------------------
	Create PSEUDO-RANDOM numbers. F8X returns one f-float per call.
	Integer range is full sized, reals range from 0.0 to 1.0.
		value = RANDOM([size-vector], [type-mold])

	Limitation: This method is for 32-bit machine only.
	Limitation: Low-order bits are not random.
*/
#define ran (Tdi_RandomSeed = Tdi_RandomSeed * 69069 + 1)

int			Tdi3Random(
struct descriptor_a	*out_ptr)
{
double	half = .5;
double norm = half/2147483648.;
int	status = 1, n, length = out_ptr->length;
int	i;

	N_ELEMENTS(out_ptr, n);
	switch (out_ptr->dtype) {
	default : status = TdiINVDTYDSC; break;

#define LoadRandom(type,value) { type *ptr = (type *)out_ptr->pointer; for (i=0;i<n;i++) ptr[i] = (type)(value);}
#define LoadRandomFloat(dtype,type,value) { type *ptr = (type *)out_ptr->pointer; \
                     for (i=0;i<n;i++) {double val = value; CvtConvertFloat(&val,DTYPE_DOUBLE,&ptr[i],dtype,0);}}

	/*********************
	WARNING falls through.
	*********************/
	case DTYPE_O : case DTYPE_OU : n += n;
	case DTYPE_Q : case DTYPE_QU : n += n;
	case DTYPE_L : case DTYPE_LU : LoadRandom(int,ran); break;
	case DTYPE_W : case DTYPE_WU : LoadRandom(short,ran >> 16) break;
	case DTYPE_B : case DTYPE_BU : LoadRandom(char,ran >> 24) break;

	/*********************
	WARNING falls through.
	*********************/
	case DTYPE_FC : n += n;
	case DTYPE_F :  LoadRandomFloat(DTYPE_F, float, ran * norm + half); break;

	case DTYPE_FSC : n += n;
        case DTYPE_FS : LoadRandomFloat(DTYPE_FS, float, ran * norm + half); break;

	case DTYPE_DC : n += n;
	case DTYPE_D : LoadRandomFloat(DTYPE_D, double, (ran * norm + ran) * norm + half); break;

	case DTYPE_GC : n += n;
	case DTYPE_G : LoadRandomFloat(DTYPE_G, double, (ran * norm + ran) * norm + half); break;

	case DTYPE_FTC : n += n;
	case DTYPE_FT : LoadRandomFloat(DTYPE_FT, double, (ran * norm + ran) * norm + half); break;

	}
	return status;
}
/*---------------------------------------------------------------------
	Create an array of zeroes.
*/
int			Tdi3Zero(
struct descriptor_a	*out_ptr)
{
static int	i0 = 0;
static struct descriptor con0 = {sizeof(int),DTYPE_L,CLASS_S,(char *)&i0};
int	status;

	status = TdiConvert(&con0, out_ptr MDS_END_ARG);
	return status;
}
