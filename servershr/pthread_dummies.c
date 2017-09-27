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
#include <windows.h>
#include <process.h>
#include <stdio.h>
#define ETIMEDOUT 42

int pthread_mutex_init(HANDLE * mutex);
void pthread_mutex_lock(HANDLE * mutex);
void pthread_mutex_unlock(HANDLE * mutex);

void pthread_detach(HANDLE * thread)
{
  return;
}

int pthread_cond_init(HANDLE * cond)
{
  *cond = CreateEvent(NULL, TRUE, FALSE, NULL);
  return (*cond == NULL);
}

BOOL pthread_cond_destroy(HANDLE * cond)
{
  return CloseHandle(*cond);
}

int pthread_cond_signal(HANDLE * cond)
{
  BOOL status;
#ifdef ___DEBUG_IT
  printf("signalling event %p\n", *cond);
#endif
  status = SetEvent(*cond);
#ifdef ___DEBUG_IT
  printf("SetEvent on %p completed with status = %d\n", *cond, status);
#endif
  if (status)
    status = ResetEvent(*cond);
#ifdef ___DEBUG_IT
  printf("ResetEvent on %p completed with status = %d\n", *cond, status);
#endif
  return status == 0;
}

int pthread_cond_wait(HANDLE * cond)
{
  int status;
#ifdef ___DEBUG_IT
  printf("waiting for condition %p\n", *cond);
#endif
  status = WaitForSingleObject(*cond, INFINITE);
#ifdef ___DEBUG_IT
  printf("got condition %p\n", *cond);
#endif
  return (status == WAIT_FAILED);
}

void pthread_cond_timedwait(HANDLE * cond, int msec)
{
  WaitForSingleObject(*cond, msec);

}

int pthread_mutex_init(HANDLE * mutex)
{
  *mutex = CreateMutex(0, FALSE, NULL);
  return (*mutex == NULL);
}

BOOL pthread_mutex_destroy(HANDLE * mutex)
{
  return CloseHandle(*mutex);
}

static HANDLE global_mutex = NULL;
void pthread_unlock_global_np()
{
  if (global_mutex == NULL)
    pthread_mutex_init(&global_mutex);
  pthread_mutex_unlock(&global_mutex);

}

void pthread_lock_global_np()
{
  if (global_mutex == NULL)
    pthread_mutex_init(&global_mutex);
  pthread_mutex_lock(&global_mutex);
}

int pthread_exit(int status)
{
  return status;
}

int pthread_create(unsigned long *thread, void *dummy, void (*rtn) (void *), void *rtn_param)
{
  *thread = _beginthread(rtn, 0, rtn_param);
  return *thread == 0;
}

void pthread_cleanup_pop()
{
}

void pthread_cleanup_push()
{
}

void pthread_mutex_lock(HANDLE * mutex)
{
#ifdef ___DEBUG_IT
  printf("Trying to lock mutex %p\n", *mutex);
#endif
  WaitForSingleObject(*mutex, INFINITE);
#ifdef ___DEBUG_IT
  printf("Locked mutex %p\n", *mutex);
#endif
}

void pthread_mutex_unlock(HANDLE * mutex)
{
  int status;
#ifdef ___DEBUG_IT
  printf("Unlocking mutex %p\n", *mutex);
#endif
  status = ReleaseMutex(*mutex);
#ifdef ___DEBUG_IT
  printf("Unlocked mutex %p with status=%d\n", *mutex, status);
#endif
}

void pthread_cancel(HANDLE thread)
{
  printf("Abort not supported");
}
