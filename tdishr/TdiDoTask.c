/*	Tdi1DoTask.C
	A routine to process various tree operations.
	status = TDI$DO_TASK(&task_dsc)

	The task may be:
*not	BUILD_METHOD	(timeout, method, object, ...)
*not	BUILD_PROCEDURE	(timeout, language, procedure, ...)
*not	BUILD_PROGRAM	(timeout, program)
	BUILD_ROUTINE	(timeout, image, routine, ...)

	The timeout in seconds, if present, determines the allowed task time.

	Ken Klare, LANL P-4	(c)1992
*/
typedef struct {int lo; unsigned int hi;} quadw;

#include <mdsdescrip.h>
#include <mdsdescrip.h>
#include "tdirefstandard.h"
#include <libroutines.h>
#include <strroutines.h>
#include <tdimessages.h>
#ifdef __VMS
#include <starlet.h>
#endif
#include <stdlib.h>
#include <mdsshr.h>
#include <treeshr.h>

extern int MdsFree1Dx();
extern int TdiTaskOf();
extern int TdiGetFloat();
extern int TdiConvert();
extern int TdiSubtract();
extern int TdiData();
extern int TdiGetNid();
extern int TdiPutLong();
extern int TdiCall();

/****	SYS$SETIMR([efn], &daytim, [&astadr], [reqidt], [flags])
	SYS$CANTIM([reqidt], [acmode])
	efn	event flag number
	daytim	+absolute, - relative time
	astadr	service routine on expiration
	reqidt	unique id
	flags	1 for CPU time
	acmode	access mode
****/

#ifdef __VMS
static void TASK_AST(int astpar, int r0, int r1, int *pc, int psl) {
	lib$signal(TdiTIMEOUT,0);
}
#endif

static int Doit(struct descriptor_routine	*ptask, struct descriptor_xd *out_ptr)
{
  int dtype, ndesc, j, status;
  int *arglist[256];
  LibEstablish(LibSigToRet);
  ndesc = ptask->ndesc;
  dtype = ptask->length ? *(unsigned char *)ptask->pointer : DTYPE_L;
  switch (ptask->dtype) {
    case DTYPE_METHOD : {
      struct descriptor_method	*pmethod = (struct descriptor_method *)ptask;
      int	nid;
      DESCRIPTOR_NID(nid_dsc, 0);
      struct descriptor_d method_d = {0,DTYPE_T,CLASS_D,0};
      nid_dsc.pointer = (char *)&nid;
      status = TdiData(pmethod->method, &method_d MDS_END_ARG);
      if (status & 1) status = TdiGetNid(pmethod->object, &nid);
      arglist[0] = (int *)(ndesc - 1);
      arglist[1] = (int *)&nid_dsc;
      arglist[2] = (int *)&method_d;
      /*** skip timeout,method,object ***/
      for (j = 3; j < ndesc; ++j) arglist[j] = (int *)pmethod->arguments[j-3];
      if (status & 1) {
	status = LibCallg(arglist, TreeDoMethod);
	status = TdiPutLong(&status, out_ptr);
      }
      StrFree1Dx(&method_d);
      break; }
    /*case DTYPE_PROCEDURE :	break;*/
    /*case DTYPE_PROGRAM :	break;*/
    case DTYPE_ROUTINE :
      status = TdiCall(dtype, ndesc-1, &ptask->image, out_ptr);
      break;
    default :	status = TdiINVDTYDSC;	break;
  }
  return status;
}


TdiRefStandard(Tdi1DoTask)
quadw	dt = {0,0};
float	timeout = (float)0.;
DESCRIPTOR_FLOAT(timeout_dsc, 0);
struct descriptor	dt_dsc = {sizeof(dt),DTYPE_Q,CLASS_S,0};
struct descriptor_xd	task_xd = EMPTY_XD, image_xd = EMPTY_XD;
struct descriptor_routine	*ptask;
        timeout_dsc.pointer = (char *)&timeout;
        dt_dsc.pointer = (char *)&dt;
	status = TdiTaskOf(list[0], &task_xd MDS_END_ARG);
	ptask = (struct descriptor_routine *)task_xd.pointer;
	if (!ptask) return TdiNULL_PTR;
        if (ptask->dtype == DTYPE_L)
        {
           status = *(int *)task_xd.pointer->pointer;
	   status = TdiPutLong(&status, out_ptr);
           goto cleanup;
        }
	/***** get timeout *****/
	if (status & 1) status=TdiGetFloat(ptask->time_out, &timeout);
	if (timeout > 0.) {
                static int zero = 0;
                static DESCRIPTOR_LONG(zero_dsc,&zero);
		timeout = (float)(1.E7 * timeout);	/*** 100 ns steps ***/
		if (status & 1) status = TdiConvert(&timeout_dsc, &dt_dsc MDS_END_ARG);
                if (status & 1) status = TdiSubtract(&zero_dsc,&dt_dsc,&dt_dsc MDS_END_ARG);
	}

	if (!(status & 1)) goto cleanup;
#ifdef __VMS
	if (dt.lo || dt.hi) status = sys$setimr(0, &dt, TASK_AST, &dt, 0);
#endif
	if (!(status & 1)) goto cleanup;
        status = Doit(ptask,out_ptr);
#ifdef __VMS
	if (dt.lo || dt.hi) {
                sys$cantim(&dt, 0);
		sys$canwak(0, 0);	/*** just in case LIB$WAIT called ***/
	}
#endif

cleanup:

        MdsFree1Dx(&task_xd, NULL);
	return status;
}
