
#include "mdsip_connections.h"
#include <stdlib.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
//  MdsSetCompression  /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


int MdsSetCompression(int id, int level)
{
  int old_level;
  if (level < 0)
    level = 0;
  else if (level > 9)
    level = 9;
  old_level = SetCompressionLevel(level);
  if (id != -1) {
    char expression[128];
    struct descrip ans = {0};
    sprintf(expression, "MdsSetCompression(%d)", level);
    MdsValue(id, expression, &ans, NULL);
    if (ans.ptr != 0)
      free(ans.ptr);
  }
  return old_level;
}
