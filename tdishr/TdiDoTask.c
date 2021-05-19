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
#include <errno.h>
#include <signal.h>
#include <stdlib.h>

#include "tdirefstandard.h"
#include <condition.h>
#include <libroutines.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <strroutines.h>
#include <tdishr.h>
#include <treeshr.h>

extern int TdiGetFloat();
extern int TdiGetLong();
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

static int Doit(struct descriptor_routine *ptask,
                struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  int dtype, ndesc, j;
  void **arglist[256];
  ndesc = ptask->ndesc;
  while (ndesc > 3 && ptask->arguments[ndesc - 4] == 0)
    ndesc--;
  dtype = ptask->length ? *(unsigned char *)ptask->pointer : DTYPE_L;
  switch (ptask->dtype)
  {
  case DTYPE_METHOD:
  {
    struct descriptor_method *pmethod = (struct descriptor_method *)ptask;
    *(int *)&arglist[0] = ndesc + 1;
    /*** skip timeout,method,object ***/
    for (j = 3; j < ndesc; ++j)
      arglist[j] = (void *)pmethod->arguments[j - 3];
    arglist[ndesc] = (void *)out_ptr;
    arglist[ndesc + 1] = MdsEND_ARG;
    int nid;
    DESCRIPTOR_NID(nid_dsc, 0);
    nid_dsc.pointer = (char *)&nid;
    arglist[1] = (void *)&nid_dsc;
    struct descriptor_d method_d = {0, DTYPE_T, CLASS_D, 0};
    FREED_ON_EXIT(&method_d);
    arglist[2] = (void *)&method_d;
    status = TdiData(pmethod->method, &method_d MDS_END_ARG);
    if (STATUS_OK)
      status = TdiGetNid(pmethod->object, &nid);
    status = (int)(intptr_t)LibCallg(arglist, TreeDoMethod);
    FREED_NOW(&method_d);
    status = TdiPutLong(&status, out_ptr);
    break;
  }
    /*case DTYPE_PROCEDURE :    break; */
  case DTYPE_PROGRAM:
  {
    struct descriptor_program *prog_task = (struct descriptor_program *)ptask;
    if (prog_task->program && prog_task->program->dtype == DTYPE_T)
      status =
          LibSpawn(prog_task->program, 1, 0) ? MDSplusERROR : MDSplusSUCCESS;
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

/* The following implementation of a cancelable worker is used in:
 * tdishr/TdiDoTask.c
 * mdstcpip/ProcessMessage.c
 * Problems with the implementation are likely to be fixed in all locations.
 */
typedef struct
{
  void **ctx;
  int status;
#ifndef _WIN32
  Condition *const condition;
#endif
  struct descriptor_xd *const task_xd;
} worker_args_t;

typedef struct
{
  worker_args_t *const wa;
  struct descriptor_xd *const xdp;
  void *pc;
} worker_cleanup_t;

static void WorkerCleanup(void *args)
{
  worker_cleanup_t *const wc = (worker_cleanup_t *)args;
  if (wc->pc)
    TreeCtxPop(wc->pc);
#ifndef _WIN32
  CONDITION_SET(wc->wa->condition);
#endif
  free_xd(wc->xdp);
  free_xd(wc->wa->task_xd);
}

static int WorkerThread(void *args)
{
  EMPTYXD(xd);
  worker_cleanup_t wc = {(worker_args_t *)args, &xd, NULL};
  pthread_cleanup_push(WorkerCleanup, (void *)&wc);
  wc.pc = TreeCtxPush(wc.wa->ctx);
  struct descriptor_routine *ptask =
      (struct descriptor_routine *)wc.wa->task_xd->pointer;
  wc.wa->status = Doit(ptask, wc.xdp);
  if (IS_OK(wc.wa->status))
    wc.wa->status = *(int *)xd.pointer->pointer;
  pthread_cleanup_pop(1);
  return wc.wa->status;
}

static int StartWorker(struct descriptor_xd *task_xd,
                       struct descriptor_xd *out_ptr, const float timeout)
{
  INIT_STATUS;
#ifdef _WIN32
  worker_args_t wa = {
#else
  Condition WorkerRunning = CONDITION_INITIALIZER;
  worker_args_t wa = {.condition = &WorkerRunning,
#endif
    .ctx = TreeCtx(),
    .status = -1,
    .task_xd = task_xd
  };
#ifdef _WIN32
  HANDLE hWorker = CreateThread(NULL, DEFAULT_STACKSIZE * 16,
                                (void *)WorkerThread, &wa, 0, NULL);
  if (!hWorker)
  {
    errno = GetLastError();
    perror("ERROR CreateThread");
    status = MDSplusFATAL;
    goto end;
  }
  int canceled = B_FALSE;
  if (WaitForSingleObject(hWorker, (int)(timeout * 1000)) == WAIT_TIMEOUT)
  {
    fflush(stdout);
    fprintf(stderr, "Timeout, terminating Worker\n");
    TerminateThread(hWorker, 2);
    canceled = B_TRUE;
  }
  WaitForSingleObject(hWorker, INFINITE);
  if (canceled)
    status = TdiTIMEOUT;
#else
  pthread_t Worker = 0;
  _CONDITION_LOCK(wa.condition);
  CREATE_THREAD(Worker, *16, WorkerThread, &wa);
  if (c_status)
  {
    perror("ERROR pthread_create");
    _CONDITION_UNLOCK(wa.condition);
    pthread_cond_destroy(&WorkerRunning.cond);
    pthread_mutex_destroy(&WorkerRunning.mutex);
    status = MDSplusFATAL;
    goto end;
  }
  struct timespec tp;
  clock_gettime(CLOCK_REALTIME, &tp);
  uint64_t ns = tp.tv_nsec + (uint64_t)(timeout * 1E9);
  tp.tv_nsec = ns % 1000000000;
  tp.tv_sec += (time_t)(ns / 1000000000);
  int err =
      pthread_cond_timedwait(&WorkerRunning.cond, &WorkerRunning.mutex, &tp);
  if (err)
  {
    fflush(stdout);
    fprintf(stderr, "Timeout, terminating Worker ..");
#ifdef WIN32
    if (pthread_cancel(Worker))
#endif
      pthread_kill(Worker, SIGINT);
    _CONDITION_WAIT_1SEC(wa.condition);
    fflush(stdout);
    if (WorkerRunning.value)
    {
      fprintf(stderr, " ok\n");
    }
    else
    {
      fprintf(stderr, " failed - sending SIGCHLD\n");
      pthread_kill(Worker, SIGCHLD);
    }
  }
  _CONDITION_UNLOCK(&WorkerRunning);
  void *result;
  pthread_join(Worker, &result);
  pthread_cond_destroy(&WorkerRunning.cond);
  pthread_mutex_destroy(&WorkerRunning.mutex);
  if (err && result == PTHREAD_CANCELED)
    status = err == ETIMEDOUT ? TdiTIMEOUT : MDSplusFATAL;
#endif
  else // only populate out_ptr if task finished in time
    status = TdiPutLong(&wa.status, out_ptr);
end:;
  return status;
}

int Tdi1DoTask(opcode_t opcode __attribute__((unused)),
               int narg __attribute__((unused)), struct descriptor *list[],
               struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  EMPTYXD(task_xd);
  int freetask;
  FREEXD_ON_EXIT(&task_xd);
  freetask = 1;
  struct descriptor_routine *ptask;
  status = TdiTaskOf(list[0], &task_xd MDS_END_ARG);
  if (STATUS_NOT_OK)
    goto cleanup;
  ptask = (struct descriptor_routine *)task_xd.pointer;
  if (!ptask)
  {
    status = TdiNULL_PTR;
    goto cleanup;
  }
  switch (ptask->dtype)
  {
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
  if (STATUS_OK)
    status = TdiGetFloat(ptask->time_out, &timeout);
  if (STATUS_NOT_OK)
    goto cleanup;
  if (timeout > 0.)
  {
    freetask = 0;
    status = StartWorker(&task_xd, out_ptr, timeout);
  }
  else
    status = Doit(ptask, out_ptr);
cleanup:;
  FREEXD_IF(&task_xd, freetask);
  return status;
}
