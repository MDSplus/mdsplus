#include <libroutines.h>
#include <treeshr_hooks.h>
#include <ncidef.h>
#include "treeshrp.h"
#include <mds_stdarg.h>

int TreeCallHook(TreeshrHookType htype, TREE_INFO *info, int nid)
{
  static DESCRIPTOR(image,"TreeShrHooks");
  static DESCRIPTOR(rtnname,"Notify");
  static int (*Notify)(TreeshrHookType,char *, int,int) = 0;
  int status = 1;
#ifndef HAVE_VXWORKS_H
  if (Notify != (int (*)(TreeshrHookType,char *,int,int))-1)
  {
    if (Notify == 0)
      status = LibFindImageSymbol(&image,&rtnname,&Notify);
    if (status & 1)
      status = (*Notify)(htype, info->treenam, info->shot,nid);
    else
    {
      Notify = (int (*)(TreeshrHookType,char *,int,int))-1;
      status = 1;
    }
  }
#endif
  return status;
}
