/*	TdiGetArgs.C
	Fetches signal, data, and category of each input.
	Guesses input conversion and output category.
	Empty XD if not a signal.
	Empty XD or null units if not with_units.
	Permits BUILD_UNITS(BUILD_SIGNAL(data),units) or BUILD_SIGNAL(BUILD_UNITS(data,units)) and others.
	Internal routine called by
		Tdi1Build	Tdi1Same	Tdi1Scalar	Tdi1Trans	Tdi1Trim
		Tdi1RANGE	Tdi1SetRange	Tdi1Vector

	Ken Klare, LANL CTR-7	(c)1989,1990
*/
#include <mdsdescrip.h>
#include <tdimessages.h>
#include <stdlib.h>
#include <mdsshr.h>

extern int MdsFree1Dx();
extern struct descriptor_xd *TdiSELF_PTR;

#include "tdirefcat.h"
#include "tdireffunction.h"
#include "tdirefstandard.h"

extern int TdiGetData(  );
extern int TdiData(  );
extern int TdiUnits(  );
extern int Tdi2Keep();
extern int Tdi2Long2();
extern int Tdi2Any();
extern int Tdi2Cmplx();

int				TdiGetSignalUnitsData(
struct descriptor		*in_ptr,
struct descriptor_xd	*signal_ptr,
struct descriptor_xd	*units_ptr,
struct descriptor_xd	*data_ptr)
{
static unsigned char omitsu[] = {DTYPE_SIGNAL,DTYPE_WITH_UNITS,0};
static unsigned char omits[] = {DTYPE_SIGNAL,0};
static unsigned char omitu[] = {DTYPE_WITH_UNITS,0};
struct descriptor_xd	tmp, *keep;
int				status;

	MdsFree1Dx(signal_ptr, NULL);
	status = TdiGetData(omitsu, in_ptr, data_ptr);
	if (status & 1) switch (data_ptr->pointer->dtype) {
	case DTYPE_SIGNAL :
		*signal_ptr = *data_ptr;
		*data_ptr = EMPTY_XD;
		keep = TdiSELF_PTR;
		TdiSELF_PTR = (struct descriptor_xd *)signal_ptr->pointer;
		status = TdiGetData(omitu, 
            ((struct descriptor_signal *)signal_ptr->pointer)->data, 
            data_ptr);
		if (status & 1) switch (data_ptr->pointer->dtype) {
		case DTYPE_WITH_UNITS :
			tmp = *data_ptr;
			*data_ptr = EMPTY_XD;
			status = TdiData(((struct descriptor_with_units *)tmp.pointer)->units, units_ptr 
					MDS_END_ARG);
			if (status & 1) status =
				TdiData(((struct descriptor_with_units *)tmp.pointer)->data, 
                                         data_ptr MDS_END_ARG);
			MdsFree1Dx(&tmp, NULL);
			break;
		default :
			MdsFree1Dx(units_ptr, NULL);
			break;
		}
		TdiSELF_PTR = keep;
		break;
	case DTYPE_WITH_UNITS :
		tmp = *data_ptr;
		*data_ptr = EMPTY_XD;
		status = TdiUnits(tmp.pointer, units_ptr MDS_END_ARG);
		if (status & 1) status = TdiGetData(omits, tmp.pointer, data_ptr);
		if (status & 1) switch(data_ptr->pointer->dtype) {
		case DTYPE_SIGNAL :
			*signal_ptr = *data_ptr;
			*data_ptr = EMPTY_XD;
			status = TdiData(signal_ptr->pointer, data_ptr MDS_END_ARG);
			break;
		default :
			break;
		}
		MdsFree1Dx(&tmp, NULL);
		break;
	default :
		MdsFree1Dx(units_ptr, NULL);
		break;
	}
	return status;
}

void UseNativeFloat(struct TdiCatStruct *cat)
{
  unsigned char k;
  for (k=0;k<TdiCAT_MAX;k++)
    if ( ((TdiREF_CAT[k].cat & ~(0x800)) == (cat->out_cat & ~(0x800))) && 
         (k == DTYPE_FLOAT || 
          k == DTYPE_DOUBLE ||
          k == DTYPE_FLOAT_COMPLEX ||
          k == DTYPE_DOUBLE_COMPLEX))
    {
      cat->out_dtype = k;
      cat->out_cat = TdiREF_CAT[k].cat;
    }
}

/*-------------------------------------------------------------------*/

int				TdiGetArgs(
int				opcode,
int				narg,
struct descriptor		*list[],
struct descriptor_xd	sig[],
struct descriptor_xd	uni[],
struct descriptor_xd	dat[],
struct TdiCatStruct		cats[])
{
struct TdiCatStruct		*cptr;
struct TdiFunctionStruct	*fun_ptr = (struct TdiFunctionStruct *)&TdiRefFunction[opcode];
int				nc=0, j, status = 1;
unsigned short			i1 = TdiREF_CAT[fun_ptr->i1].cat,
				i2 = TdiREF_CAT[fun_ptr->i2].cat,
				o1 = TdiREF_CAT[fun_ptr->o1].cat,
				o2 = TdiREF_CAT[fun_ptr->o2].cat;
unsigned char nd=0,jd;
int use_native = (fun_ptr->f2 != Tdi2Keep && fun_ptr->f2 != Tdi2Long2 && fun_ptr->f2 != Tdi2Any && fun_ptr->f2 != Tdi2Cmplx);
	/***************************
	Get signal, units, and data.
	***************************/
	for (j = 0; j < narg; ++j) {
		sig[j] = uni[j] = dat[j] = EMPTY_XD;
		if (status & 1) status = TdiGetSignalUnitsData(list[j], &sig[j], &uni[j], &dat[j]);
	}

	/******************************
	Find category of data type.
	Adjust out in f2 routine.
	Make in into out in CVT_ARGS.
	******************************/
	if (status & 1) for (nd = 0, nc = 0, j = 0; j < narg; ++j) {
	struct descriptor	*dat_ptr = dat[j].pointer;

		cptr = &cats[j];
		cptr->out_dtype = cptr->in_dtype = jd = dat_ptr->dtype;
		if (jd > nd && (nd = jd) >= TdiCAT_MAX) {status = TdiINVDTYDSC; break;}
		cptr->out_cat = (unsigned short)(((cptr->in_cat = TdiREF_CAT[jd].cat) | i1) & i2);
		if (jd != DTYPE_MISSING) nc |= cptr->out_cat;
		if ((cptr->digits = TdiREF_CAT[jd].digits) == 0) cptr->digits = dat_ptr->length;
	}

        for (j=0;j<narg;j++)
        {
          if (fun_ptr->i1 == fun_ptr->i2)
          {
            cats[j].out_dtype = fun_ptr->i2;
            cats[j].out_cat = i2;
          }
          else if (cats[j].out_cat & 0x400 && use_native)
            UseNativeFloat(&cats[j]);
        }

	/***********************************************
	Output cat and dtype are guesses, checked later.
	***********************************************/
	if (status & 1) {
	  cptr = &cats[narg];
	  cptr->in_dtype = nd;
	  cptr->out_dtype = nd;
	  cptr->in_cat = TdiREF_CAT[nd].cat;
	  cptr->out_cat = (unsigned short)((nc | o1) & o2);
	  cptr->digits = TdiREF_CAT[nd].digits;
          if (fun_ptr->o1 == fun_ptr->o2)
          {
            cptr->out_dtype = cptr->in_dtype = fun_ptr->o2;
            cptr->out_cat = cptr->in_cat = o2;
          }
          else if (cptr->out_cat & 0x400 && use_native)
            UseNativeFloat(cptr);
	}
	return status;
}
