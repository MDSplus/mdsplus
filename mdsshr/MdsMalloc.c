#define _GNU_SOURCE /* glibc2 needs this */
#if defined(__sparc__)
#include "/usr/include/sys/types.h"
#elif !defined(HAVE_WINDOWS_H)
#include <sys/types.h>
#endif
#ifdef malloc
#undef malloc
#undef free
#undef realloc
#undef calloc
#endif
#include <stdlib.h>
#ifndef HAVE_VXWORKS_H
#include <pthread.h>
#endif
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <STATICdef.h>
#include <mdsshr.h>
#include "mdsshrthreadsafe.h"

#ifdef HAVE_WINDOWS_H
static unsigned long *mutex;
#else
static pthread_mutex_t mutex;
#endif
static int mutexInitialized=0;

struct vmlist { struct vmlist *next;
                unsigned int len;
                char pre[20];
                char vm[1];
              };

STATIC_THREADSAFE struct vmlist *VM = 0;
STATIC_THREADSAFE int NumMalloc = 0;
STATIC_THREADSAFE int NumFree = 0;
STATIC_THREADSAFE unsigned int Allocated = 0;
STATIC_CONSTANT char PRE[]  = "abcdefghijklmnopqrst";
STATIC_CONSTANT char POST[] = "tsrqponmlkjihgfedcba";

STATIC_ROUTINE void CheckList();

void *MdsMALLOC(unsigned int len)
{
  size_t length=len+sizeof(struct vmlist)+sizeof(POST)-1;
  struct vmlist *p = malloc(length);
  if (len <= 0)
  {
    MDSprintf("malloc called with len <= 0\n");
    raise(SIGUSR1);
  }
  if (p == NULL)
  {
    raise(SIGUSR1);
  }
  p->len = len;
  memcpy(p->pre,PRE,sizeof(PRE)-1);
  memcpy(p->vm+len,POST,sizeof(POST)-1);
  LockMdsShrMutex(&mutex,&mutexInitialized);
  p->next = VM;
  VM = p;
  NumMalloc++;
  Allocated += len;
  CheckList();
  UnlockMdsShrMutex(&mutex);
  return p->vm;
}

void MdsFREE(void *ptr)
{
  struct vmlist *n,*p;
  LockMdsShrMutex(&mutex,&mutexInitialized);
  CheckList();
  NumFree++;
  for (p = 0, n=VM; n && n->vm != ptr; p=n,n=n->next);
  if (n)
  {
    Allocated -= n->len;
    if (p)
      p->next = n->next;
    else
      VM = n->next;
    free(n);
  }
  else
  {
    MDSprintf("free called for unknown memory\n");
    raise(SIGUSR1);
  }
  UnlockMdsShrMutex(&mutex);
}

void *MdsREALLOC(void *ptr, unsigned int len)
{
  void *newmem = MdsMALLOC(len);
  struct vmlist *n;
  if (ptr)
  {
    LockMdsShrMutex(&mutex,&mutexInitialized);
    CheckList();
    for (n=VM; n && n->vm != ptr; n=n->next);
    UnlockMdsShrMutex(&mutex);
    if (n)
    {
      memcpy(newmem,ptr,n->len > len ? len : n->len);
      MdsFREE(ptr);
    }
    else
    {
      MDSprintf("realloc called for unknown memory\n");
      raise(SIGUSR1);
    }
  }
  return newmem;
}

STATIC_ROUTINE void CheckList()
{
  struct vmlist *n;
  for (n=VM; n; n=n->next)
  {
    if (memcmp(n->pre,PRE,sizeof(PRE)-1)!=0)
    {
      MDSprintf("Corrupted memory found\n");
      raise(SIGUSR1);
    }
    if (memcmp(n->vm+n->len,POST,sizeof(PRE)-1)!=0)
    {
      MDSprintf("Corrupted memory found\n");
      raise(SIGUSR1);
    }
  }
}

void MdsShowVM(int full)
{
  struct vmlist *n;
  LockMdsShrMutex(&mutex,&mutexInitialized);
  MDSprintf("malloc %d times, free %d times, %d still allocated in %d chunks\n",
    NumMalloc,NumFree,Allocated,NumMalloc-NumFree);
  if (full)
  {
    for (n=VM; n; n=n->next)
    {
      unsigned int i;
      char *p = malloc(n->len+1);
      for (i=0;i<n->len;i++)
      p[i] = (n->vm[i] < 32 || n->vm[i] > 122) ? '.' : n->vm[i];
      p[n->len] = 0;
      MDSprintf("%d -------- %.100s\n",n->len,p);
      free(p);
    }
  } 
  UnlockMdsShrMutex(&mutex);
}    

void *MdsCALLOC(int num, unsigned int len)
{
  return memset(MdsMALLOC(num * len),0,num*len);
}
