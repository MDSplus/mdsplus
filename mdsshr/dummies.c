#include <stdio.h>
#include <string.h>
#include  <stdlib.h>

int MdsEventCan()
{
  int stat = 0;
  if (!(stat & 1))
    printf("%s not currently implemented\n","MdsEventCan");
  return stat;
}
