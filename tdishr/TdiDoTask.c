/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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

STATIC_ROUTINE int Doit(struct descriptor_routine *ptask, struct descriptor_xd *out_ptr){
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

typedef struct _WorkerArgs{
  Condition                 *pcond;
  int                       *pstatus;
  struct descriptor_xd      *task_xd;
  void                      *dbid;
} WorkerArgs;


pthread_mutex_t worker_destroy = PTHREAD_MUTEX_INITIALIZER;
static void WorkerExit(void *args){
  free_xd(((WorkerArgs*)args)->task_xd);
  pthread_mutex_lock(&worker_destroy);pthread_cleanup_push((void*)pthread_mutex_unlock, &worker_destroy);
  CONDITION_RESET(((WorkerArgs*)args)->pcond);
  pthread_cleanup_pop(1);
}

static void WorkerThread(void *args){
  pthread_cleanup_push(WorkerExit, (void*)((WorkerArgs*)args));
  CONDITION_SET(((WorkerArgs*)args)->pcond);
  TreeUsePrivateCtx(1);
  void* old = TreeSwitchDbid(((WorkerArgs*)args)->dbid);
  pthread_cleanup_push((void*)TreeSwitchDbid,old);
  EMPTYXD(out_xd);
  FREEXD_ON_EXIT(&out_xd);
  struct descriptor_routine* ptask = (struct descriptor_routine *)((WorkerArgs*)args)->task_xd->pointer;
  int status = Doit(ptask,&out_xd);
  *((WorkerArgs*)args)->pstatus = STATUS_OK ? *(int*)out_xd.pointer->pointer : status;
  FREEXD_NOW(&out_xd);
  pthread_cleanup_pop(1);
  pthread_cleanup_pop(1);
  pthread_exit(0);
}

STATIC_ROUTINE int StartWorker(struct descriptor_xd *task_xd, struct descriptor_xd *out_ptr, const float timeout){
  INIT_STATUS, t_status = MDSplusERROR;
  pthread_t Worker;
  Condition WorkerRunning = CONDITION_INITIALIZER;
  WorkerArgs args = { &WorkerRunning, &t_status, task_xd, TreeDbid()};
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
  CONDITION_DESTROY(&WorkerRunning,&worker_destroy);
  return status;
}

int Tdi1DoTask(int opcode __attribute__ ((unused)),
	       int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  EMPTYXD(task_xd);
  int freetask;
  FREEXD_ON_EXIT(&task_xd);
  freetask = 1;
  struct descriptor_routine *ptask;
  status = TdiTaskOf(list[0], &task_xd MDS_END_ARG);
  if STATUS_NOT_OK
    goto cleanup;
  ptask = (struct descriptor_routine *)task_xd.pointer;
  if (!ptask) {
    status = TdiNULL_PTR;
    goto cleanup;
  }
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
  if (timeout > 0.) {
    freetask = 0;
    status = StartWorker(&task_xd, out_ptr, timeout);
  } else
    status = Doit(ptask, out_ptr);
 cleanup: ;
  FREEXD_IF(&task_xd,freetask);
  return status;
}
