#include <treeshr.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define Dummy(name,status) \
int name()\
{\
  int stat = status;\
  if (!(stat & 1))\
    printf("%s not currently implemented\n",#name);\
  return stat;\
}

#define DummyArgs(name,ret,args,status) \
ret name args \
{\
  printf("%s not currently implemented\n",#name);\
  return status;\
}

Dummy(TreeCallHook,TreeNORMAL)
Dummy(TreeEstablishRundownEvent,TreeNORMAL)
Dummy(TreeWait,TreeNORMAL)
