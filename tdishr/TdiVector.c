/*	Tdi1Vector.C
	Converts a list of scalars and arrays into a rank-1 vector or higher array.
	Signality is lost.
	Null vectors are ignored, except lamination is not done.
	If all dimensions match exactly, the result has one more dimension.
	[[1],,[3]] [1,2,3] and [1,[2,3]] are simple vectors.
	[[1,2,3],[4,5,6]] has lamination shape [3,2].

	Ken Klare, LANL P-4	(c)1989,1990,1991
*/
#define MAXDIM 8

#include "tdirefcat.h"
#include "tdirefstandard.h"
#include "tdinelements.h"
#include <libroutines.h>
#include <tdimessages.h>
#include <stdlib.h>
#include <mdsshr.h>

extern int TdiConvert();
extern int TdiCvtArgs();
extern int TdiGetArgs();
extern int TdiMasterData();
extern int Tdi2Vector();
extern int MdsCopyDxXd();
extern int MdsFree1Dx();

#ifdef __DECC
#pragma member_alignment save
#pragma nomember_alignment
#endif
typedef ARRAY_COEFF(char,MAXDIM+1) array_coeff_max;
typedef ARRAY_COEFF(char,1) array_coeff_1;
#ifdef __DECC
#pragma member_alignment restore
#endif


TdiRefStandard(Tdi1Vector)
static DESCRIPTOR_A(		miss,0,DTYPE_MISSING,0,0);
static DESCRIPTOR_A_COEFF(	arr0,1,DTYPE_BU,0,MAXDIM+1,0);
array_coeff_max                 arr = *(array_coeff_max *)&arr0;
struct descriptor_xd	(*psig)[], (*puni)[]=0, (*pdat)[]=0;
struct TdiCatStruct		(*pcats)[]=0;
int	cmode = -1, j, n, (*pnelem)[]=0, jd, mind = MAXDIM, maxd = 0, nmiss = 0;
int	virt = (sizeof(struct descriptor_xd)*3 + sizeof(int *))*narg + sizeof(struct TdiCatStruct)*(narg+1);

	if (narg == 0) return MdsCopyDxXd((struct descriptor *)&miss, out_ptr);
	/************************************
	Dynamic number of descriptors.
	Memory for sig[narg], uni[narg],
	dat[narg], nelem[narg], cats[narg+1].
	************************************/
        
	status = (psig = malloc(virt)) != NULL;
	if (status & 1) {
		puni =  (struct descriptor_xd (*)[])&(*psig)[narg];
		pdat = (struct descriptor_xd (*)[])&(*puni)[narg];
		pnelem = (int (*)[])&(*pdat)[narg];
		pcats = (struct TdiCatStruct (*)[])&(*pnelem)[narg]; /* narg+1 of these */
	}

	/******************************************
	Fetch signals and data and data's category.
	******************************************/
	if (status & 1) status = TdiGetArgs(opcode, narg, list, (*psig), (*puni), (*pdat), (*pcats));

	/*****************************************
	Save and accumulate lengths of all inputs.
	*****************************************/
	if (status & 1) for (j = narg; --j >= 0;) {
	array_coeff_1 *pnew = (array_coeff_1 *)(*pdat)[j].pointer;
		if ((*pcats)[j].digits > (*pcats)[narg].digits) (*pcats)[narg].digits = (*pcats)[j].digits;
		if (pnew->dtype == DTYPE_MISSING) {n = 0; nmiss++;}
		else {
			N_ELEMENTS(pnew, n);
			if (pnew->class == CLASS_A) jd = pnew->aflags.coeff ? pnew->dimct : 1;
			else jd = 0;
			if (jd < mind) mind = jd;
			if (jd > maxd) maxd = jd;
		}
		arr.arsize += (*pnelem)[j] = n;
	}

	/********************************
	If all dimensions match then add one.
	If all match but highest, expand it.
	Otherwise, use a simple vector.
	Shape: [[3],[3]] is [3,2].
	Shape: [[3,1],[3,4]] is [3,5].
	Shape: [[3],[3,4]] is [3,5].
	********************************/
	if (!(status & 1)) ;
	else if (mind > 0 && mind >= maxd-1 && mind < MAXDIM && nmiss == 0) {
		n = 0;
		for (j = 0; j < narg; ++j) {		
		array_coeff_1 *pnew = (array_coeff_1 *)(*pdat)[j].pointer;
			if (pnew->aflags.coeff) {
				jd = pnew->dimct;
				n += (jd == maxd) ? pnew->m[maxd-1] : 1;
				if (j == 0) for (; --jd >= 0;) arr.m[jd] = pnew->m[jd];
				else for (; --jd >= 0;) if (arr.m[jd] != pnew->m[jd]) mind = jd;
				if (mind < maxd-1) goto simple;
			}
			else {
				n++;
				if (j == 0) arr.m[0] = (*pnelem)[j];
				else if (arr.m[0] != (*pnelem)[j]) goto simple;
			}
		}
		arr.dimct = (unsigned char)(mind + 1);
		arr.aflags.coeff = 1;
		if (mind < maxd) arr.m[maxd-1] = n;
		else		arr.m[maxd] = narg;
	}
	else {
simple:		arr.dimct = 1;
		arr.aflags.coeff = 0;
	}

	if (status & 1) status = Tdi2Vector(narg, (*puni), (*pdat), (*pcats), 0);

	/*********************
	Find type conversions.
	*********************/
	if (status & 1) status = TdiCvtArgs(0, (*pdat), &(*pcats)[narg]);

	/***************************
	Get an array to hold it all.
	Size is 1 so arsize = nelem.
	***************************/
	if (status & 1) status = MdsGet1DxA((struct descriptor_a *)&arr, &(*pcats)[narg].digits, &(*pcats)[narg].out_dtype, 
                      out_ptr);

	/*********************************
	Accumulate all arrays and scalars.
	Recycle arr as temporary pointer.
	Class and flags are the same.
	*********************************/
	if (status & 1) {
	struct descriptor *pout = out_ptr->pointer;
		arr.length	= pout->length;
		arr.dtype		= pout->dtype;
		arr.pointer	= pout->pointer;
		for (j = 0; j < narg && status & 1; ++j) {
			arr.arsize = (*pnelem)[j] * arr.length;
			status = TdiConvert((*pdat)[j].pointer, &arr MDS_END_ARG);
			arr.pointer += arr.arsize;
		}
	}

	/*************************
	Remove signal, keep units.
	*************************/
	if (status & 1) status = TdiMasterData(0, (*psig), (*puni), &cmode, out_ptr);

	/********************
	Free all temporaries.
	********************/
	for (j = narg; --j >= 0; ) {
		if ((*psig)[j].pointer) MdsFree1Dx(&(*psig)[j], NULL);
		if ((*puni)[j].pointer) MdsFree1Dx(&(*puni)[j], NULL);
		if ((*pdat)[j].pointer) MdsFree1Dx(&(*pdat)[j], NULL);
	}
	if (psig != NULL)
	  free(psig);
	return status;
}
