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
#endif
#include <stdlib.h>
#ifndef HAVE_VXWORKS_H
#include <pthread.h>
#endif
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <STATICdef.h>

struct vmlist { struct vmlist *next;
                int len;
                char pre[20];
                char vm[1];
              };

STATIC_THREADSAFE struct vmlist *VM = 0;
STATIC_THREADSAFE int NumMalloc = 0;
STATIC_THREADSAFE int NumFree = 0;
STATIC_THREADSAFE int Allocated = 0;
STATIC_CONSTANT char PRE[]  = "abcdefghijklmnopqrst";
STATIC_CONSTANT char POST[] = "tsrqponmlkjihgfedcba";

STATIC_ROUTINE void CheckList();

void *MdsMALLOC(int len)
{
  struct vmlist *p = malloc(len+sizeof(struct vmlist)+sizeof(POST)-1);
  if (len <= 0)
  {
    MDSprintf("malloc called with len <= 0\n");
    raise(SIGUSR1);
  }
  p->len = len;
  memcpy(p->pre,PRE,sizeof(PRE)-1);
  memcpy(p->vm+len,POST,sizeof(POST)-1);
  pthread_lock_global_np();
  p->next = VM;
  VM = p;
  NumMalloc++;
  Allocated += len;
  CheckList();
  pthread_unlock_global_np();
  return p->vm;
}

void MdsFREE(void *ptr)
{
  struct vmlist *n,*p;
  pthread_lock_global_np();
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
  pthread_unlock_global_np();
}

void *MdsREALLOC(void *ptr, int len)
{
  void *newmem = MdsMALLOC(len);
  struct vmlist *n;
  if (ptr)
  {
    pthread_lock_global_np();
    CheckList();
    for (n=VM; n && n->vm != ptr; n=n->next);
    pthread_unlock_global_np();
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
  pthread_lock_global_np();
  MDSprintf("malloc %d times, free %d times, %d still allocated in %d chunks\n",
    NumMalloc,NumFree,Allocated,NumMalloc-NumFree);
  if (full)
  {
    for (n=VM; n; n=n->next)
    {
      int i;
      char *p = malloc(n->len+1);
      for (i=0;i<n->len;i++)
      p[i] = (n->vm[i] < 32 || n->vm[i] > 122) ? '.' : n->vm[i];
      p[n->len] = 0;
      MDSprintf("%d -------- %.100s\n",n->len,p);
      free(p);
    }
  } 
  pthread_unlock_global_np();
}    

void *MdsCALLOC(int num, int len)
{
  return memset(MdsMALLOC(num * len),0,num*len);
}
