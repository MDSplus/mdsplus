#include <libroutines.h>
#include <treeshr_hooks.h>
#include <ncidef.h>
#include "treeshrp.h"

int TreeCallHook(TreeshrHookType htype, TREE_INFO *info)
{
#ifdef HAVE_VXWORKS_H
  return 1;
}
#else
  static DESCRIPTOR(image,"TreeShrHooks");
  static DESCRIPTOR(rtnname,"Notify");
  static int (*rtn)(TreeshrHookType,char *, int) = 0;
  int status = 1;
  if (rtn == (int (*)(TreeshrHookType,char *,int))-1) 
    return 0;
  else if (rtn == 0)
    status = LibFindImageSymbol(&image,&rtnname,&rtn);
  if (status & 1)
    return (*rtn)(htype, info->treenam, info->shot);
  else
    rtn = (int (*)(TreeshrHookType,char *,int))-1;
  return status;
}
#endif
