/*      Tdi1MATRIX
        Some matrix operations.
                square_matrix = DIAGONAL([vector],[fill])

        NEED signality for DIAGONAL, maybe.
        Ken Klare, LANL CTR-7   (c)1990
*/

#include <STATICdef.h>
#define _MOVC3(a,b,c) memcpy(c,b,a)
#include "STATICdef.h"
#include "tdirefstandard.h"
#include "tdirefcat.h"
#include "tdinelements.h"
#include <tdishr_messages.h>
#include <stdlib.h>
#include <mdsshr.h>
#include <string.h>



extern int TdiGetArgs();
extern int TdiData();
extern int TdiConvert();
extern int TdiMasterData();

STATIC_CONSTANT struct descriptor missing = { 0, DTYPE_MISSING, CLASS_S, 0 };

typedef struct {
  int q[2];
} quadw;
STATIC_ROUTINE int copy(int len, int n, char *x, int incx, char *y, int incy)
{
  switch (len) {
  case 1:
    for (; --n >= 0; x += incx, y += incy)
      *(char *)y = *(char *)x;
    break;
  case 2:
    for (; --n >= 0; x += incx, y += incy)
      *(short *)y = *(short *)x;
    break;
  case 4:
    for (; --n >= 0; x += incx, y += incy)
      *(int *)y = *(int *)x;
    break;
  case 8:
    for (; --n >= 0; x += incx, y += incy)
      *(quadw *) y = *(quadw *) x;
    break;
  default:
    for (; --n >= 0; x += incx, y += incy)
      _MOVC3(len, x, y);
    break;
  }
  return 1;
}

int Tdi1Diagonal(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  int cmode = -1, nside = 0;
  struct descriptor_xd fill = EMPTY_XD;
  struct descriptor *fillptr;
  struct descriptor_a *pv, *po;
  DESCRIPTOR_A_COEFF(proto, 1, DTYPE_BU, 0, 2, 0);
  struct descriptor_xd sig[1], uni[1], dat[1];
  struct TdiCatStruct cats[2];

  status = TdiGetArgs(opcode, 1, list, sig, uni, dat, cats);
  pv = (struct descriptor_a *)dat[0].pointer;
  if STATUS_OK {
    N_ELEMENTS(pv, nside);
  }
  if STATUS_OK {
    proto.m[0] = nside;
    proto.m[1] = nside;
    proto.length = pv->length;
    proto.arsize = pv->length * nside * nside;
    if (narg > 1 && list[1]) {
      status = TdiData(list[1], &fill MDS_END_ARG);
      fillptr = fill.pointer;
    } else
      fillptr = (struct descriptor *)&missing;
  }
  if STATUS_OK
    status = MdsGet1DxA((struct descriptor_a *)&proto, &pv->length, &pv->dtype, out_ptr);
  po = (struct descriptor_a *)out_ptr->pointer;
  if STATUS_OK
    status = TdiConvert(fillptr, po MDS_END_ARG);
  if STATUS_OK
    status = copy(pv->length, nside, pv->pointer, pv->length,
		  po->pointer, pv->length * (nside + 1));
  if STATUS_OK
    status = TdiMasterData(0, sig, uni, &cmode, out_ptr);
  MdsFree1Dx(&fill, NULL);
  MdsFree1Dx(&dat[0], NULL);
  MdsFree1Dx(&uni[0], NULL);
  MdsFree1Dx(&sig[0], NULL);
  return status;
}
