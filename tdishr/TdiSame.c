/*      Tdi1Same.C
        Does operations that preserve the input shape and signality.
        It calls: status = name(&in1, ..., &out)
        1-argument routines include: AINT, NOT, SIN.
        2-argument routines include: ADD, AND, LOGICAL_AND, EQ. Indirectly from MAX and MIN
        3-argument routines include: BITS, SHFTC.
        1 or 2 arguments: CHAR D_COMPLEX ... STRING INT REAL.
        1, 2, or 3 arguments: CMPLX.
        Internal generic routine for elemental functions.

        Ken Klare, LANL CTR-7   (c)1989,1990
*/
#include <STATICdef.h>
#include <status.h>
#include <stdlib.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <string.h>
#include "tdirefcat.h"
#include "tdireffunction.h"
#include "tdirefstandard.h"
#include <libroutines.h>



extern int Tdi3undef();
extern int TdiGetArgs();
extern int TdiCvtArgs();
extern int TdiGetShape();
extern int TdiMasterData();
extern int TdiFaultHandler();

int Tdi1Same(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd sig[3], uni[3], dat[3];
  struct TdiCatStruct cats[4];
  struct TdiFunctionStruct *fun_ptr = (struct TdiFunctionStruct *)&TdiRefFunction[opcode];
  int cmode = -1, j, (*routine) () = fun_ptr->f3;

  dat[1].pointer = 0;
  dat[2].pointer = 0;

	/******************************************
        Fetch signals and data and data's category.
        ******************************************/
  memset(sig, 0, sizeof(sig));
  memset(uni, 0, sizeof(uni));
  memset(dat, 0, sizeof(dat));
  memset(cats, 0, sizeof(cats));
  status = TdiGetArgs(opcode, narg, list, sig, uni, dat, cats);

	/******************************************
        Adjust category needed to match data types.
        ******************************************/
  if STATUS_OK
    status = (*fun_ptr->f2) (narg, uni, dat, cats, &routine, fun_ptr->o1, fun_ptr->o2);

	/******************************
        Do the needed type conversions.
        ******************************/
  if STATUS_OK
    status = TdiCvtArgs(narg, dat, cats);

	/******************
        Find correct shape.
        ******************/
  if STATUS_OK
    status = TdiGetShape(narg, dat, cats[narg].digits, cats[narg].out_dtype, &cmode, out_ptr);

	/********************************
        Act on data, linear arguments.
        No action for simple conversions.
        Default is for MIN/MAX pairwise.
        ********************************/
  if STATUS_OK {
    if (routine == &Tdi3undef || routine == 0) {
      MdsFree1Dx(out_ptr, NULL);
      *out_ptr = dat[0];
      dat[0] = EMPTY_XD;
    } else {
      switch (fun_ptr->m2) {
      case 1:
	status = (*routine) (dat[0].pointer, out_ptr->pointer);
	break;
      default:
      case 2:
	status = (*routine) (dat[0].pointer, dat[1].pointer, out_ptr->pointer);
	break;
      case 3:
	status = (*routine) (dat[0].pointer, dat[1].pointer, dat[2].pointer, out_ptr->pointer);
	break;
      }
    }
  }
    /********************
      Embed data in signal.
     ********************/
  if STATUS_OK
    status = TdiMasterData(narg, sig, uni, &cmode, out_ptr);

	/********************
        Free all temporaries.
        ********************/
  for (j = narg; --j >= 0;) {
    if (sig[j].pointer)
      MdsFree1Dx(&sig[j], NULL);
    if (uni[j].pointer)
      MdsFree1Dx(&uni[j], NULL);
    if (dat[j].pointer)
      MdsFree1Dx(&dat[j], NULL);
  }
  return status;
}
