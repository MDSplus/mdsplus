#include <stdio.h>
#include <string.h>
#include  <stdlib.h>
#include <config.h>

#ifdef HAVE_WINDOWS_H
int MDSEventCan()
{
  int stat = 0;
  if (!(stat & 1))
    printf("%s not currently implemented\n","MDSEventCan");
  return stat;
}
int MDSEventAst()
{
  int stat = 0;
  if (!(stat & 1))
    printf("%s not currently implemented\n","MDSEventAst");
  return stat;
}
int MDSEvent()
{
  int stat = 0;
  if (!(stat & 1))
    printf("%s not currently implemented\n","MDSEvent");
  return stat;
}
#endif
