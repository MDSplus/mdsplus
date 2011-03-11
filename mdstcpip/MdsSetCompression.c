
#include "mdsip_connections.h"
#include <stdlib.h>
#include <stdio.h>

int MdsSetCompression(int id, int level)
{
  int old_level;
  int status;
  if (level < 0)
    level = 0;
  else if (level > 9)
    level = 9;
  old_level = SetCompressionLevel(level);
  if (id != -1) 
  {
    char expression[128];
    struct descrip ans;
    sprintf(expression,"MdsSetCompression(%d)",level);
    status = MdsValue(id,expression,&ans,0);
    if (ans.ptr != 0) free(ans.ptr);
  }
  return old_level;
}

