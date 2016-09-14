/*      Tdi1DecompileDependency
        Decompile a dependency tree

        Author: Manduchi Gabriele
                Istituto Gas Ionizzati del CNR - Padova (Italy)

        Date:   3-MAY-1990
        Revised: 18-May-1990 Ken Klare, CTR-7 LANL (c)1990 using precedence method

        Precedence:
*/
#define P_WEAK  0		/*weakest */
#define P_OR    4		/*OR(|) */
#define P_AND   8		/*AND(&) */
#define P_UNARY 12		/*unary: NEGATE(!) IGNORE_UNDEFINED(?) IGNORE_STATUS(~) */
#define P_TIGHT 16		/*strongest: path <event> and parentheses */

#include <STATICdef.h>
#include <string.h>
#include <mdsdescrip.h>
#include "tdirefstandard.h"
#include <strroutines.h>
#include <tdishr_messages.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <mds_stdarg.h>



STATIC_CONSTANT DESCRIPTOR(AND, " & ");
STATIC_CONSTANT DESCRIPTOR(OR, " | ");
STATIC_CONSTANT DESCRIPTOR(NEGATE, "!");
STATIC_CONSTANT DESCRIPTOR(IGNORE_UNDEFINED, "?");
STATIC_CONSTANT DESCRIPTOR(IGNORE_STATUS, "~");

STATIC_CONSTANT DESCRIPTOR(LEFT_ANGLE, "<");
STATIC_CONSTANT DESCRIPTOR(RIGHT_ANGLE, ">");
STATIC_CONSTANT DESCRIPTOR(LEFT_PAREN, "(");
STATIC_CONSTANT DESCRIPTOR(RIGHT_PAREN, ")");

STATIC_ROUTINE int DependencyGet(int prec, struct descriptor_r *pin, struct descriptor_d *pout)
{
  int now = 0, status = 1;
  struct descriptor *pwhich = 0;

  switch (pin->dtype) {
  case DTYPE_EVENT:
    status =
	StrConcat((struct descriptor *)pout, (struct descriptor *)pout,
		  &LEFT_ANGLE, pin, &RIGHT_ANGLE MDS_END_ARG);
    break;
  case DTYPE_NID:
    {
      char *path = TreeGetMinimumPath(0, *(int *)pin->pointer);
      if (path != NULL) {
	DESCRIPTOR_FROM_CSTRING(path_d, path);
	status = StrAppend(pout, &path_d);
	TreeFree(path);
      } else
	status = TreeFAILURE;
    }
    break;
  case DTYPE_PATH:
    status = StrAppend(pout, (struct descriptor *)pin);
    break;
  case DTYPE_DEPENDENCY:
    switch (*(short *)pin->pointer) {
    case TreeDEPENDENCY_AND:
      now = P_AND;
      pwhich = (struct descriptor *)&AND;
      break;
    case TreeDEPENDENCY_OR:
      now = P_OR;
      pwhich = (struct descriptor *)&OR;
      break;
    default:
      status = TdiINV_OPC;
      break;
    }
    if (status & 1 && now < prec)
      status = StrAppend(pout, (struct descriptor *)&LEFT_PAREN);
    if (status & 1)
      status = DependencyGet(now - 1, (struct descriptor_r *)pin->dscptrs[0], pout);
    if (status & 1)
      status = StrAppend(pout, pwhich);
    if (status & 1)
      status = DependencyGet(now + 1, (struct descriptor_r *)pin->dscptrs[1], pout);
    if (status & 1 && now < prec)
      status = StrAppend(pout, (struct descriptor *)&RIGHT_PAREN);
    break;
  case DTYPE_CONDITION:
    switch (*((short *)pin->pointer)) {
    case TreeNEGATE_CONDITION:
      pwhich = (struct descriptor *)&NEGATE;
      break;
    case TreeIGNORE_UNDEFINED:
      pwhich = (struct descriptor *)&IGNORE_UNDEFINED;
      break;
    case TreeIGNORE_STATUS:
      pwhich = (struct descriptor *)&IGNORE_STATUS;
      break;
    default:
      status = TdiINV_OPC;
      break;
    }
    if (status & 1)
      status = StrAppend(pout, pwhich);
    if (status & 1)
      status = DependencyGet(P_UNARY, (struct descriptor_r *)pin->dscptrs[0], pout);
    break;
  default:
    status = TdiINVDTYDSC;
    break;
  }
  return status;
}

/*------------------------------------------------------------------*/
int Tdi1DecompileDependency(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)),
			    struct descriptor *list[],
			    struct descriptor_xd *out_ptr)
{
  int status = 1;
  struct descriptor_d answer = { 0, DTYPE_T, CLASS_D, 0 };
  struct descriptor *pdep = list[0];

  while (pdep && pdep->dtype == DTYPE_DSC)
    pdep = (struct descriptor *)pdep->pointer;
  if (pdep)
    status = DependencyGet(P_WEAK, (struct descriptor_r *)pdep, &answer);
  if (status & 1)
    status = MdsCopyDxXd((struct descriptor *)&answer, out_ptr);
  StrFree1Dx(&answer);
  return status;
}
