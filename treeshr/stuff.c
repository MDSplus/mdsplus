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

Dummy(MdsGetCurrentShotid,0)
Dummy(TreeCallHook,1)
Dummy(TreeEstablishRundownEvent,1)
Dummy(TreeDoMethod,0)
Dummy(_TreeDoMethod,0)
Dummy(MdsEventCan,0)


Dummy(TreeWait,1)
void TreeFree(void *ptr) { free(ptr);}
