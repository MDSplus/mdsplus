#include  <stdlib.h>
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

Dummy(MdsEventCan,0)
