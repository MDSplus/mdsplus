/*      Tdi1Cull.C
        CULL removes bad indices and EXTEND replaces them with the limit values.
                subscripts = CULL(dimension or array, [dim], [subscripts])
                subscripts = EXTEND(dimension or array, [dim], [subscripts])
                same = CULL or EXTEND(*, *, [subscripts])

        See also Tdi1ItoX.C for index to axis conversion and vice versa.
        See also Tdi1Subscript.C for subscripting and mapping.

        Ken Klare, LANL P-4     (c)1990,1991,1992
        KK      21-Oct-1992     remove text mismatches, NOT FOR EXTEND
*/
#include <stdlib.h>
#include <string.h>
#include "tdirefcat.h"
#include "tdirefstandard.h"
#include "tdinelements.h"
#include "tdithreadsafe.h"
#include <tdishr_messages.h>
#include <mdsshr.h>
#include <STATICdef.h>



#define _MOVC3(a,b,c) memmove(c,b,a)

extern unsigned short OpcValue;

extern struct descriptor *TdiItoXSpecial;

extern int TdiGetData();
extern int TdiUnits();
extern int TdiData();
extern int TdiCvtArgs();
extern int TdiGe();
extern int TdiLe();
extern int TdiMasterData();
extern int TdiGetArgs();
extern int TdiIsIn();
extern int TdiItoX();
extern int TdiGetLong();
extern int Tdi2Range();

STATIC_CONSTANT DESCRIPTOR_FUNCTION_0(value, &OpcValue);
STATIC_CONSTANT DESCRIPTOR_RANGE(EMPTY_RANGEE, 0, 0, (struct descriptor *)&value);
typedef struct {
  int x[2];
} quadw;

/**********************************************
        Redo culled array or scalar.
*/
int TdiRecull(struct descriptor_xd *out_ptr)
{
  int status = 1;
  struct descriptor_a *px = (struct descriptor_a *)out_ptr->pointer;

	/****************************
        Culled scalar is null vector.
        ****************************/
  if (px->class != CLASS_A) {
    DESCRIPTOR_A(arr0, 0, 0, 0, 0);
    arr0.length = px->length;
    arr0.dtype = px->dtype;
    status = MdsCopyDxXd((struct descriptor *)&arr0, out_ptr);
  } else {
    px->dimct = 1;
    px->aflags.coeff = 0;
    px->aflags.bounds = 0;
  }
  return status;
}

/*---------------------------------------------
        Remove out-of-bounds integer-limited elements.
*/
int TdiIcull(int left, int right, struct descriptor_a *px)
{
  int *pi = (int *)px->pointer, *po = pi;
  int n, status = 1;

  N_ELEMENTS(px, n);
  if (status & 1)
    for (; --n >= 0; ++pi)
      if (*pi >= left && *pi <= right)
	*po++ = *pi;
  if (pi != po) {
		/********************************
                Scalars must be nulled elsewhere.
                ********************************/
    if (px->class == CLASS_A)
      px->arsize = (char *)po - px->pointer;
    status = -1;
  }
  return status;
}

/*---------------------------------------------
        Replace out-of-bounds integer-limited elements.
*/
int TdiIextend(int left, int right, struct descriptor_a *px)
{
  int *pi = (int *)px->pointer;
  int n, status = 1;

  N_ELEMENTS(px, n);
  if (status & 1)
    for (; --n >= 0; ++pi) {
      if (*pi < left)
	*pi = left;
      else if (*pi > right)
	*pi = right;
    }
  return status;
}

/*---------------------------------------------
        Remove elements not satisfying mask.
*/
STATIC_ROUTINE int rcull(struct descriptor *pnew __attribute__ ((unused)),
			 struct descriptor_a *pmask, struct descriptor_a *px)
{
  char *pm = pmask->pointer, *pi = px->pointer, *po = pi;
  int len = px->length, n, status = 1;
  char *cptr;
  short *sptr;
  int *lptr;
  quadw *qptr;

  N_ELEMENTS(px, n);
  if (status & 1)
    switch (len) {
    case 1:
      for (cptr = (char *)po; --n >= 0; pi += 1)
	if (*pm++)
	  *cptr++ = *((char *)pi);
      po = (char *)cptr;
      break;
    case 2:
      for (sptr = (short *)po; --n >= 0; pi += 2)
	if (*pm++)
	  *sptr++ = *((short *)pi);
      po = (char *)sptr;
      break;
    case 4:
      for (lptr = (int *)po; --n >= 0; pi += 4)
	if (*pm++)
	  *lptr++ = *((int *)pi);
      po = (char *)lptr;
      break;
    case 8:
      for (qptr = (quadw *) po; --n >= 0; pi += 8)
	if (*pm++)
	  *qptr++ = *((quadw *) pi);
      po = (char *)qptr;
      break;
    default:
      for (; --n >= 0; pi += len)
	if (*pm++)
	  _MOVC3(len, pi, po), po += len;
      break;
    }
  if (pi != po) {
    status = -1;
		/********************************
                Scalars must be nulled elsewhere.
                ********************************/
    if (px->class == CLASS_A) {
      px->arsize = (char *)po - px->pointer;
      px->dimct = 1;
      px->aflags.coeff = 0;
      px->aflags.bounds = 0;
    }
  }
  return status;
}

/*---------------------------------------------
        Replace elements not in mask.
*/
STATIC_ROUTINE int rextend(struct descriptor *pnew,
			   struct descriptor_a *pmask, struct descriptor_a *px)
{
  char *pn = pnew->pointer, *pm = pmask->pointer, *pi = px->pointer;
  int len = px->length, n, status = 1;

  N_ELEMENTS(px, n);
  if (status & 1)
    switch (len) {
    case 1:
      for (; --n >= 0; pi += 1)
	if (!*pm++)
	  *(char *)pi = *((char *)pn);
      break;
    case 2:
      for (; --n >= 0; pi += 2)
	if (!*pm++)
	  *(short *)pi = *((short *)pn);
      break;
    case 4:
      for (; --n >= 0; pi += 4)
	if (!*pm++)
	  *(int *)pi = *((int *)pn);
      break;
    case 8:
      for (; --n >= 0; pi += 8)
	if (!*pm++)
	  *(quadw *) pi = *((quadw *) pn);
      break;
    default:
      for (; --n >= 0; pi += len)
	if (!*pm++)
	  _MOVC3(len, pn, pi);
      break;
    }
  return status;
}

/**********************************************
        Going to find out who is naughty and nice.
*/
STATIC_ROUTINE int work(int
			rroutine(struct descriptor *, struct descriptor_a *,
				 struct descriptor_a *), int opcode, int narg,
			struct descriptor *list[3], struct descriptor_xd *out_ptr)
{
  struct descriptor_xd in = EMPTY_XD, tmp = EMPTY_XD, units = EMPTY_XD;
  struct descriptor *keep[3];
  int cmode = -1, dim, status = 1, s1 = 1;
  struct descriptor_range *new[3];
  struct descriptor_with_units *pwu;
  struct descriptor_signal *psig = 0;
  struct descriptor_range fake_range;
  struct descriptor dx0, dx1;
  struct descriptor_xd sig[3], uni[3], dat[3];
  struct TdiCatStruct cats[4];
  STATIC_CONSTANT unsigned char omits[] = { DTYPE_DIMENSION, DTYPE_SIGNAL, DTYPE_DIMENSION, 0 };
  STATIC_CONSTANT unsigned char omitd[] = { DTYPE_WITH_UNITS, DTYPE_DIMENSION, 0 };
  keep[0] = TdiThreadStatic()->TdiRANGE_PTRS[0];
  keep[1] = TdiThreadStatic()->TdiRANGE_PTRS[1];
  keep[2] = TdiThreadStatic()->TdiRANGE_PTRS[2];
  status = TdiGetData(omits, list[0], &in);
  if (status & 1 && in.pointer->dtype == DTYPE_WITH_UNITS) {
    status = TdiUnits(in.pointer, &units MDS_END_ARG);
    if (status & 1)
      status = TdiGetData(&omits[1], &in, &in);
  }
  if (status & 1 && narg > 1 && list[1])
    status = TdiGetLong(list[1], &dim);
  else
    dim = 0;

	/*****************************
        Convert signal to a dimension.
        *****************************/
  if (status & 1 && in.pointer->dtype == DTYPE_SIGNAL) {
    tmp = in;
    in = EMPTY_XD;
    psig = (struct descriptor_signal *)tmp.pointer;
    if (dim >= psig->ndesc - 2)
      status = TdiBAD_INDEX;
    else
      status = TdiGetData(omitd, psig->dimensions[dim], &in);
    if (status & 1 && in.pointer->dtype == DTYPE_WITH_UNITS) {
      status = TdiUnits(in.pointer, &units MDS_END_ARG);
      if (status & 1)
	status = TdiGetData(&omitd[1], &in, &in);
    } else
      MdsFree1Dx(&units, NULL);
    dim = 0;
  }
	/*************************
        Get dimension information.
        *************************/
  if (status & 1)
    switch (in.pointer->dtype) {
    case DTYPE_MISSING:
      if (narg > 2 && list[2])
	status = MdsCopyDxXd(list[2], out_ptr);
      else
	MdsFree1Dx(out_ptr, NULL);
      break;
    case DTYPE_T:
      if (!(narg > 2 && list[2])) {
	if (status & 1)
	  status = TdiData(&in, out_ptr MDS_END_ARG);
	break;
      }
      MdsFree1Dx(out_ptr, NULL);
      status = TdiGetArgs(opcode, 1, &list[2], sig, uni, dat, cats);
      if (status & 1 && units.pointer == 0)
	status = MdsCopyDxXd((struct descriptor *)&uni[0], &units);
      if (status & 1)
	status = TdiIsIn(dat[0].pointer, &in, &tmp MDS_END_ARG);
      if (status & 1)
	status =
	    rroutine(dat[0].pointer, (struct descriptor_a *)tmp.pointer,
		     (struct descriptor_a *)dat[0].pointer);
      if (status == -1)
	status = TdiRecull(&dat[0]);
      MdsFree1Dx(&sig[0], NULL);
      MdsFree1Dx(&uni[0], NULL);
      if (status & 1)
	*out_ptr = dat[0];
      break;
    default:
      if (!(narg > 2 && list[2])) {
	if (status & 1)
	  status = TdiData(&in, out_ptr MDS_END_ARG);
	break;
      }
		/********************************
                Special conversion to get limits.
                Could use some tests to speed up.
                ********************************/
      MdsFree1Dx(out_ptr, NULL);
      status = TdiItoX(in.pointer, TdiItoXSpecial, &tmp MDS_END_ARG);
      if (status & 1) {
	pwu = (struct descriptor_with_units *)tmp.pointer;
	if (pwu->dtype == DTYPE_WITH_UNITS)
	  dx0 = *pwu->data;
	else
	  dx0 = *tmp.pointer;
	dx0.class = CLASS_S;
	dx1 = dx0;
	dx1.pointer += dx1.length;
	new[0] = (struct descriptor_range *)&dx0;
	new[1] = (struct descriptor_range *)&dx1;
	new[2] = (struct descriptor_range *)list[2];
	while (new[2] && new[2]->class == CLASS_XD)
	  new[2] = (struct descriptor_range *)new[2]->pointer;
	TdiThreadStatic()->TdiRANGE_PTRS[0] = &dx0;
	TdiThreadStatic()->TdiRANGE_PTRS[1] = &dx1;
	TdiThreadStatic()->TdiRANGE_PTRS[2] = 0;
	if (in.pointer->dtype == DTYPE_DIMENSION) {
	  TdiThreadStatic()->TdiRANGE_PTRS[2] = in.pointer;
		/************************************************************
                Dimensions conversion with missing increment uses all values.
                ************************************************************/
	  if (new[2]->dtype == DTYPE_RANGE
	      && (new[2]->ndesc == 2 || (new[2]->ndesc == 3 && new[2]->deltaval == 0))) {
	    fake_range = EMPTY_RANGEE;
	    fake_range.begin = new[2]->begin;
	    fake_range.ending = new[2]->ending;
	    new[2] = &fake_range;
	  }
	}
	status = TdiGetArgs(opcode, 3, new, sig, uni, dat, cats);
	TdiThreadStatic()->TdiRANGE_PTRS[0] = keep[0];
	TdiThreadStatic()->TdiRANGE_PTRS[1] = keep[1];
	TdiThreadStatic()->TdiRANGE_PTRS[2] = keep[2];
      }
      if (status & 1)
	status = Tdi2Range(3, uni, dat, cats, 0);
      if (status & 1 && units.pointer == 0)
	status = MdsCopyDxXd((struct descriptor *)&uni[0], &units);
      if (status & 1)
	status = TdiCvtArgs(3, dat, cats);
      if (status & 1)
	status = TdiGe(dat[2].pointer, dat[0].pointer, &tmp MDS_END_ARG);
      if (status & 1)
	s1 = status =
	    rroutine(dat[0].pointer, (struct descriptor_a *)tmp.pointer,
		     (struct descriptor_a *)dat[2].pointer);
      if (status & 1)
	status = TdiLe(dat[2].pointer, dat[1].pointer, &tmp MDS_END_ARG);
      if (status & 1)
	s1 |= status =
	    rroutine(dat[1].pointer, (struct descriptor_a *)tmp.pointer,
		     (struct descriptor_a *)dat[2].pointer);
      if (status & 1 && s1 == -1)
	status = TdiRecull(&dat[2]);
      MdsFree1Dx(&sig[0], NULL);
      MdsFree1Dx(&sig[1], NULL);
      MdsFree1Dx(&sig[2], NULL);
      MdsFree1Dx(&uni[0], NULL);
      MdsFree1Dx(&uni[1], NULL);
      MdsFree1Dx(&uni[2], NULL);
      MdsFree1Dx(&dat[0], NULL);
      MdsFree1Dx(&dat[1], NULL);
      if (status & 1)
	*out_ptr = dat[2];
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  MdsFree1Dx(&in, NULL);
  if (status & 1 && units.pointer)
    status = TdiMasterData(0, sig, &units, &cmode, out_ptr);
  MdsFree1Dx(&units, NULL);
  return status;
}

/***********************************************/
int Tdi1Cull(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  return work(rcull, opcode, narg, list, out_ptr);
}

/***********************************************/
int Tdi1Extend(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  return work(rextend, opcode, narg, list, out_ptr);
}
