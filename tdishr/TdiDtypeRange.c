/*	Tdi1DtypeRange.C
	Create array of stepped values of most data types. Default delta is 1.
		vector = from : to : [delta]
	Example, 3..11..2) returns [3, 5, 7, 9, 11].

	The begin, end, or delta may be vectors.
	In which case they are matched.
	The result is a simple vector concatenation.
	Example, 3..[4,5] is [3,4,3,4,5].

	Ken Klare, LANL CTR-7	(c)1989,1990
	NEED faster code for longs and maybe floats.
*/

struct descriptor *TdiRANGE_PTRS[3] = {0,0,0};

extern unsigned short OpcValue;

#include "tdinelements.h"
#include "tdirefcat.h"
#include "tdirefstandard.h"
#include <tdimessages.h>
#include <stdlib.h>
#include <mdsshr.h>

static DESCRIPTOR_A(arr0, 1, DTYPE_B, 0, 0);
static int minus_one_value = -1;
static struct descriptor minus_one = {sizeof(int), DTYPE_L, CLASS_S, (char *)&minus_one_value};

extern struct descriptor *TdiItoXSpecial;

extern int MdsFree1Dx();
extern int TdiItoX();
extern int TdiXtoI();
extern int TdiCvtArgs();
extern int TdiGetArgs();
extern int TdiDivide();
extern int TdiSubtract();
extern int TdiDim();
extern int TdiLong();
extern int Tdi2Range();
extern int Tdi3Ramp();
extern int Tdi3Multiply();
extern int Tdi3Add();
extern int TdiMasterData();

extern struct descriptor *TdiItoXSpecial;

#ifdef __DECC
#pragma member_alignment save
#pragma nomember_alignment
#endif
typedef ARRAY(char) array;
#ifdef __DECC
#pragma member_alignment restore
#endif


TdiRefStandard(Tdi1DtypeRange)
unsigned short len;
unsigned char dtype;
int				cmode = -1, j, nseg=0, nnew = narg;
int				*pl, tot=0;
struct descriptor		*new[3];
struct descriptor_xd	nelem = EMPTY_XD, limits = EMPTY_XD;
struct descriptor		dx0, dx1;
array				arr = *(array *)&arr0;
struct descriptor_xd	sig[3], uni[3], dat[3];
struct TdiCatStruct		cats[4];

	new[0] = list[0];
        new[1] = list[1];
	new[2] = narg>2 ? list[2] : 0;
	/**********************************************
	* Specials for X_TO_I axis-to-index conversion.
	* No axis increment implies all points between.
	* *:*:$VALUE is done internally in X_TO_I.
	* x:y:$VALUE is I_TO_X(X_TO_I(x),X_TO_I(y)) with
	*	missing x or y taken as first or last.
	* x:y:z is normal range with missing computed.
	**********************************************/
	if (new[2]
	&& new[2]->dtype == DTYPE_FUNCTION
	&& *new[2]->pointer == OpcValue) {
	DESCRIPTOR_RANGE(range, 0, 0, 0);
                range.begin = &dx0;
                range.ending = &dx1;
		if (!TdiRANGE_PTRS[2]) return TdiNULL_PTR;
		if (new[0] == 0 && new[1] == 0) return TdiItoX(TdiRANGE_PTRS[2], out_ptr MDS_END_ARG);
		status = TdiXtoI(TdiRANGE_PTRS[2], TdiItoXSpecial, &limits MDS_END_ARG);
		if (status & 1) {
			dx0 = *limits.pointer;
			dx0.class = CLASS_S;
			dx1 = dx0;
			dx1.pointer += dx1.length;

			dat[0] = dat[1] = EMPTY_XD;
			if (new[0]) {
				status = TdiXtoI(TdiRANGE_PTRS[2], new[0], &dat[0] MDS_END_ARG);
				range.begin = dat[0].pointer;
			}
		}
		if (new[1] && status & 1) {
			status = TdiXtoI(TdiRANGE_PTRS[2], new[1], &dat[1] MDS_END_ARG);
			range.ending = dat[1].pointer;
		}
		if (status & 1) status = TdiItoX(TdiRANGE_PTRS[2], &range, out_ptr MDS_END_ARG);
		MdsFree1Dx(&dat[1], NULL);
		MdsFree1Dx(&dat[0], NULL);
		MdsFree1Dx(&limits, NULL);
		return status;
	}
	if (new[0] == 0) new[0] = TdiRANGE_PTRS[0];
	if (new[1] == 0) new[1] = TdiRANGE_PTRS[1];
	if (new[2] == 0) nnew = 2;

	/******************************************
	Fetch signals and data and data's category.
	******************************************/
	if (status & 1) status = TdiGetArgs(opcode, nnew, new, sig, uni, dat, cats);
	if (status & 1) for (j = nnew; --j >= 0;)
		if (dat[j].pointer->dtype == DTYPE_MISSING) status = TdiNULL_PTR;

	/******************************************
	Adjust category needed to match data types.
	Do any conversions to match types.
	******************************************/
	if (status & 1) status = Tdi2Range(nnew, uni, dat, cats, 0);
	if (status & 1) status = TdiCvtArgs(nnew, dat, cats);
	dtype = cats[nnew].out_dtype;
	len = cats[nnew].digits;

	/********************************************
	The number of elements in each segment.
	Expression: LONG(MAX((end-begin)/delta+1,0)).
	Total number of elements is sum of above.
	WARNING 3$ routines require type match.
	********************************************/
	if (status & 1) status = TdiSubtract(&dat[1], dat[0].pointer, &nelem MDS_END_ARG);
	if (new[2] && status & 1) status = TdiDivide(&nelem, dat[2].pointer, &nelem MDS_END_ARG);
	if (status & 1) status = TdiDim(&nelem, &minus_one, &nelem MDS_END_ARG);
	if (status & 1 && nelem.pointer->dtype != DTYPE_L) status = TdiLong(&nelem, &nelem MDS_END_ARG);
	if (status & 1) N_ELEMENTS(nelem.pointer, nseg);
	if (status & 1) for (j = nseg, pl = (int *)nelem.pointer->pointer, tot = 0; --j >= 0;) tot += *pl++;

	/**************************
	Output array size is known.
	Get the array and stuff it.
	**************************/
	arr.arsize = tot;
	if (status & 1) status = MdsGet1DxA((struct descriptor_a *)&arr, &len, &dtype, out_ptr);
	if (status & 1) {
	DESCRIPTOR_A(x_dsc, 0, 0, 0, 0);
	struct descriptor begin = {0, 0, CLASS_S, 0};
	struct descriptor delta = {0, 0, CLASS_S, 0};
	int incb = dat[0].pointer->class == CLASS_A;
	int incs = new[2] && dat[2].pointer->class == CLASS_A;

		x_dsc.length = len;
                x_dsc.dtype = dtype;
                x_dsc.pointer = out_ptr->pointer->pointer;
                begin.length = len;
                begin.dtype = dtype;
                begin.pointer = dat[0].pointer->pointer;
                delta.length = len;
                delta.dtype = dtype;
                delta.pointer = new[2] ? dat[2].pointer->pointer : 0;
		for (j = nseg, pl = (int *)nelem.pointer->pointer; --j >= 0;) {
			x_dsc.arsize = *pl * len;
			if (status & 1) status = Tdi3Ramp(&x_dsc);
			if (new[2] && status & 1) status = Tdi3Multiply(&x_dsc, &delta, &x_dsc);
			if (status & 1) status = Tdi3Add(&x_dsc, &begin, &x_dsc);
			if (incb) begin.pointer += len;
			if (incs) delta.pointer += len;
			x_dsc.pointer += x_dsc.arsize;
			++pl;
		}
	}

	MdsFree1Dx(&nelem, NULL);
	MdsFree1Dx(&limits, NULL);

	if (status & 1) status = TdiMasterData(0, sig, uni, &cmode, out_ptr);

	for (j = nnew; --j >= 0; ) {
		if (sig[j].pointer) MdsFree1Dx(&sig[j], NULL);
		if (uni[j].pointer) MdsFree1Dx(&uni[j], NULL);
		if (dat[j].pointer) MdsFree1Dx(&dat[j], NULL);
	}
	return status;
}
