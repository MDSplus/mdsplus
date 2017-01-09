/*      Tdi1DoTask.C
        A routine to process various tree operations.
        status = TDI$DO_TASK(&task_dsc)

        The task may be:
*not    BUILD_METHOD    (timeout, method, object, ...)
*not    BUILD_PROCEDURE (timeout, language, procedure, ...)
*not    BUILD_PROGRAM   (timeout, program)
        BUILD_ROUTINE   (timeout, image, routine, ...)

        The timeout in seconds, if present, determines the allowed task time.

        Ken Klare, LANL P-4     (c)1992
*/
typedef struct {
  int lo;
  unsigned int hi;
} quadw;

#include <STATICdef.h>
#include <stdlib.h>
#include <mdsdescrip.h>
#include "tdirefstandard.h"
#include <libroutines.h>
#include <strroutines.h>
#include <tdishr_messages.h>
#ifdef __VMS
#include <starlet.h>
#endif
#include <mdsshr.h>
#include <treeshr.h>



extern int TdiTaskOf();
extern int TdiGetFloat();
extern int TdiConvert();
extern int TdiSubtract();
extern int TdiData();
extern int TdiGetNid();
extern int TdiPutLong();
extern int TdiCall();

/****   SYS$SETIMR([efn], &daytim, [&astadr], [reqidt], [flags])
        SYS$CANTIM([reqidt], [acmode])
        efn     event flag number
        daytim  +absolute, - relative time
        astadr  service routine on expiration
        reqidt  unique id
        flags   1 for CPU time
        acmode  access mode
****/

#ifdef __VMS
STATIC_ROUTINE void TASK_AST(int astpar, int r0, int r1, int *pc, int psl)
{
  lib$signal(TdiTIMEOUT, 0);
}
#endif

STATIC_ROUTINE int Doit(struct descriptor_routine *ptask, struct descriptor_xd *out_ptr)
{
  int dtype, ndesc, j, status;
  void **arglist[256];
  ndesc = ptask->ndesc;
  while (ndesc > 3 && ptask->arguments[ndesc - 4] == 0)
    ndesc--;
  dtype = ptask->length ? *(unsigned char *)ptask->pointer : DTYPE_L;
  switch (ptask->dtype) {
  case DTYPE_METHOD:{
      struct descriptor_method *pmethod = (struct descriptor_method *)ptask;
      int nid;
      DESCRIPTOR_NID(nid_dsc, 0);
      struct descriptor_d method_d = { 0, DTYPE_T, CLASS_D, 0 };
      nid_dsc.pointer = (char *)&nid;
      status = TdiData(pmethod->method, &method_d MDS_END_ARG);
      if STATUS_OK
	status = TdiGetNid(pmethod->object, &nid);
      *(int*)&arglist[0] = ndesc + 1;
      arglist[1] = (void*)&nid_dsc;
      arglist[2] = (void*)&method_d;

      /*** skip timeout,method,object ***/
      for (j = 3; j < ndesc; ++j)
	arglist[j] = (void *)pmethod->arguments[j - 3];
      arglist[ndesc] = (void *)out_ptr;
      arglist[ndesc + 1] = MdsEND_ARG;
      void* result = LibCallg(arglist, TreeDoMethod);
      status = TdiPutLong((int*)&result, out_ptr);
      StrFree1Dx(&method_d);
      break;
    }
    /*case DTYPE_PROCEDURE :    break; */
  case DTYPE_PROGRAM:
    {
      struct descriptor_program *prog_task = (struct descriptor_program *)ptask;
      if (prog_task->program && prog_task->program->dtype == DTYPE_T)
	status = LibSpawn(prog_task->program, 1, 0) == 0;
      status = TdiPutLong(&status, out_ptr);

    }
    break;
  case DTYPE_ROUTINE:
    status = TdiCall(dtype, ndesc - 1, &ptask->image, out_ptr);
    break;
  default:
    status = TdiINVDTYDSC;
    break;
  }
  return status;
}

int Tdi1DoTask(int opcode __attribute__ ((unused)),
	       int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  quadw dt = { 0, 0 };
  float timeout = (float)0.;
  DESCRIPTOR_FLOAT(timeout_dsc, 0);
  struct descriptor dt_dsc = { sizeof(dt), DTYPE_Q, CLASS_S, 0 };
  struct descriptor_xd task_xd = EMPTY_XD;
  struct descriptor_routine *ptask;
  timeout_dsc.pointer = (char *)&timeout;
  dt_dsc.pointer = (char *)&dt;
  status = TdiTaskOf(list[0], &task_xd MDS_END_ARG);
  if STATUS_NOT_OK
    goto cleanup;
  ptask = (struct descriptor_routine *)task_xd.pointer;
  if (!ptask)
    return TdiNULL_PTR;
  switch (ptask->dtype) {
  case DTYPE_L:
  case DTYPE_LU:
    {
      status = *(int *)ptask->pointer;
      status = TdiPutLong(&status, out_ptr);
      goto cleanup;
    }
  case DTYPE_W:
  case DTYPE_WU:
    {
      status = (int)*(short *)ptask->pointer;
      status = TdiPutLong(&status, out_ptr);
      goto cleanup;
    }
  case DTYPE_B:
  case DTYPE_BU:
    {
      status = (int)*(char *)ptask->pointer;
      status = TdiPutLong(&status, out_ptr);
      goto cleanup;
    }
  case DTYPE_PROGRAM:
  case DTYPE_METHOD:
  case DTYPE_ROUTINE:
    break;
  default:
    status = TdiINVDTYDSC;
    status = TdiPutLong(&status, out_ptr);
    goto cleanup;
  }

	/***** get timeout *****/
  if STATUS_OK
    status = TdiGetFloat(ptask->time_out, &timeout);
  if (timeout > 0.) {
    STATIC_CONSTANT int zero = 0;
    STATIC_CONSTANT DESCRIPTOR_LONG(zero_dsc, &zero);
    timeout = (float)(1.E7 * timeout);			/*** 100 ns steps ***/
    if STATUS_OK
      status = TdiConvert(&timeout_dsc, &dt_dsc MDS_END_ARG);
    if STATUS_OK
      status = TdiSubtract(&zero_dsc, &dt_dsc, &dt_dsc MDS_END_ARG);
  }

  if STATUS_NOT_OK
    goto cleanup;
#ifdef __VMS
  if (dt.lo || dt.hi)
    status = sys$setimr(0, &dt, TASK_AST, &dt, 0);
#endif
  if STATUS_NOT_OK
    goto cleanup;
  status = Doit(ptask, out_ptr);
#ifdef __VMS
  if (dt.lo || dt.hi) {
    sys$cantim(&dt, 0);
    sys$canwak(0, 0);			/*** just in case LIB$WAIT called ***/
  }
#endif

 cleanup:

  MdsFree1Dx(&task_xd, NULL);
  return status;
}
