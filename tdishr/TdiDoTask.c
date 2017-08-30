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
#define DEF_FREED
#define DEF_FREEXD
#include <STATICdef.h>
#include <stdlib.h>
#include <mdsdescrip.h>
#include "tdirefstandard.h"
#include <libroutines.h>
#include <strroutines.h>
#include <tdishr_messages.h>
#include <pthread_port.h>
#include <errno.h>
#ifdef __VMS
#include <starlet.h>
typedef struct {
  int lo;
  unsigned int hi;
} quadw;
#endif
#include <mdsshr.h>
#include <treeshr.h>



extern int TdiTaskOf();
extern int TdiGetFloat();
extern int TdiGetLong();
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
  INIT_STATUS;
  int dtype, ndesc, j;
  void **arglist[256];
  ndesc = ptask->ndesc;
  while (ndesc > 3 && ptask->arguments[ndesc - 4] == 0)
    ndesc--;
  dtype = ptask->length ? *(unsigned char *)ptask->pointer : DTYPE_L;
  switch (ptask->dtype) {
  case DTYPE_METHOD:{
      struct descriptor_method *pmethod = (struct descriptor_method *)ptask;
      *(int*)&arglist[0] = ndesc + 1;
      /*** skip timeout,method,object ***/
      for (j = 3; j < ndesc; ++j)
	arglist[j] = (void *)pmethod->arguments[j - 3];
      arglist[ndesc] = (void *)out_ptr;
      arglist[ndesc + 1] = MdsEND_ARG;
      int nid;
      DESCRIPTOR_NID(nid_dsc, 0);
      nid_dsc.pointer = (char *)&nid;
      arglist[1] = (void*)&nid_dsc;
      struct descriptor_d method_d = { 0, DTYPE_T, CLASS_D, 0 };
      FREED_ON_EXIT(&method_d);
      arglist[2] = (void*)&method_d;
      status = TdiData(pmethod->method, &method_d MDS_END_ARG);
      if STATUS_OK
	status = TdiGetNid(pmethod->object, &nid);
      status = (int)((char *)LibCallg(arglist, TreeDoMethod) - (char *)0);
      FREED_NOW(&method_d);
      status = TdiPutLong(&status, out_ptr);
      break;
    }
    /*case DTYPE_PROCEDURE :    break; */
  case DTYPE_PROGRAM:
    {
      struct descriptor_program *prog_task = (struct descriptor_program *)ptask;
      if (prog_task->program && prog_task->program->dtype == DTYPE_T)
        status = LibSpawn(prog_task->program, 1, 0) ? MDSplusERROR : MDSplusSUCCESS;
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

#if !defined(__VMS) && !defined(_WIN32)
typedef struct _WorkerArgs{
  Condition                 *pcond;
  int                       *pstatus;
  struct descriptor_routine *ptask;
} WorkerArgs;

static void WorkerExit(void *WorkerRunning){
  CONDITION_RESET(((Condition*)WorkerRunning));
}

static void WorkerThread(void *args){
  pthread_cleanup_push(WorkerExit, (void*)((WorkerArgs*)args)->pcond);
  CONDITION_SET(((WorkerArgs*)args)->pcond);
  EMPTYXD(out_xd);
  FREEXD_ON_EXIT(&out_xd);
  int status = Doit(((WorkerArgs*)args)->ptask,&out_xd);
  *((WorkerArgs*)args)->pstatus = STATUS_OK ? *(int*)out_xd.pointer->pointer : status;
  FREEXD_NOW(&out_xd);
  pthread_cleanup_pop(1);
  pthread_exit(0);
}

STATIC_ROUTINE int StartWorker(struct descriptor_routine *ptask, struct descriptor_xd *out_ptr, const float timeout){
  INIT_STATUS, t_status = MDSplusERROR;
  pthread_t Worker;
  Condition WorkerRunning = CONDITION_INITIALIZER;
  WorkerArgs args = { &WorkerRunning, &t_status, ptask };
  _CONDITION_LOCK(&WorkerRunning);
  CREATE_DETACHED_THREAD(Worker, *8, WorkerThread,(void*)&args);
  if (c_status) {
    perror("Error creating pthread");
    status = MDSplusFATAL;
  } else {
    _CONDITION_WAIT_SET(&WorkerRunning);
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    uint64_t ns = tp.tv_nsec + (uint64_t)(timeout*1E9);
    tp.tv_nsec = ns % 1000000000;
    tp.tv_sec += (time_t)(ns/1000000000);
    int err = pthread_cond_timedwait(&WorkerRunning.cond,&WorkerRunning.mutex,&tp);
    if (err) {
      pthread_cancel(Worker);
      status = err==ETIMEDOUT ? TdiTIMEOUT : MDSplusERROR;
    } else // only populate out_ptr if task finished in time
      status = TdiPutLong(&t_status, out_ptr);
    if (WorkerRunning.value)
      _CONDITION_WAIT(&WorkerRunning);
  }
  _CONDITION_UNLOCK(&WorkerRunning);
  CONDITION_DESTROY(&WorkerRunning);
  return status;
}
#endif

int Tdi1DoTask(int opcode __attribute__ ((unused)),
	       int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  EMPTYXD(task_xd);
  FREEXD_ON_EXIT(&task_xd);
  struct descriptor_routine *ptask;
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

  float timeout = (float)0.;
  if STATUS_OK
    status = TdiGetFloat(ptask->time_out, &timeout);
  if STATUS_NOT_OK goto cleanup;
#ifdef __VMS
  /***** get timeout *****/
  quadw dt = { 0, 0 };
  DESCRIPTOR_FLOAT(timeout_dsc, 0);
  struct descriptor dt_dsc = { sizeof(dt), DTYPE_Q, CLASS_S, 0 };
  timeout_dsc.pointer = (char *)&timeout;
  dt_dsc.pointer = (char *)&dt;
  if (timeout > 0.) {
    STATIC_CONSTANT int zero = 0;
    STATIC_CONSTANT DESCRIPTOR_LONG(zero_dsc, &zero);
    timeout = (float)(1.E7 * timeout);			/*** 100 ns steps ***/
    if STATUS_OK
      status = TdiConvert(&timeout_dsc, &dt_dsc MDS_END_ARG);
    if STATUS_OK
      status = TdiSubtract(&zero_dsc, &dt_dsc, &dt_dsc MDS_END_ARG);
  }
  if STATUS_NOT_OK goto cleanup;
  if (dt.lo || dt.hi)
    status = sys$setimr(0, &dt, TASK_AST, &dt, 0);
  if STATUS_NOT_OK goto cleanup;
  status = Doit(ptask, out_ptr);
  if (dt.lo || dt.hi) {
    sys$cantim(&dt, 0);
    sys$canwak(0, 0);			/*** just in case LIB$WAIT called ***/
  }
#else
 #ifndef _WIN32
  if (timeout > 0.) {
    status = StartWorker(ptask, out_ptr, timeout);
  } else
 #endif
    status = Doit(ptask, out_ptr);
#endif
 cleanup: ;
  FREEXD_NOW(&task_xd);
  return status;
}
