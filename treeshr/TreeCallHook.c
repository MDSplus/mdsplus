#include <libroutines.h>
#include <treeshr_hooks.h>
#include <ncidef.h>
#include "treeshrp.h"
#include <mds_stdarg.h>

int TreeCallHook(TreeshrHookType htype, TREE_INFO *info, int nid)
{
#ifdef HAVE_VXWORKS_H
  return 1;
}
#else
  static DESCRIPTOR(image,"TreeShrHooks");
  static DESCRIPTOR(rtnname,"Notify");
  static int (*Notify)(TreeshrHookType,char *, int,int) = 0;
  int status = 1;
  if (Notify == (int (*)(TreeshrHookType,char *,int,int))-1) 
    return 0;
  else if (Notify == 0)
    status = LibFindImageSymbol(&image,&rtnname,&Notify);
  if (status & 1)
    return (*Notify)(htype, info->treenam, info->shot,nid);
  else
    status = 1;
/*
  else
  {
    static DESCRIPTOR(image,"TdiShr");
    static DESCRIPTOR(rtnname,"TdiExecute");
    static int (*TdiExecute)(char *,...);
    status = LibFindImageSymbol(&image,&rtnname,&TdiExecute);
    if (status & 1)
    {
      DESCRIPTOR(expression,"long(TreeHook($,$,$,$))");
      int hookt = (int)htype;
      struct descriptor tree = {0, DTYPE_T, CLASS_S, 0};
      DESCRIPTOR_LONG(hooktype,0);
      DESCRIPTOR_LONG(shot,0);
      DESCRIPTOR_LONG(nid_d,0);
      EMPTYXD(ans);
      tree.length = strlen(info->treenam);
      tree.pointer = info->treenam;
      hooktype.pointer = (char *)&hookt;
      shot.pointer = (char *)&info->shot;
      nid_d.pointer = (char *)&nid;
      status = (*TdiExecute)(&expression,&hooktype,&tree,&shot,&nid_d,&ans MDS_END_ARG);
      if (status & 1)
      {
        status = ans.pointer ? (ans.pointer->pointer ? *(int *)ans.pointer->pointer : 0) : 0;
      }
      else
        Notify = (int (*)(TreeshrHookType,char *,int,int))-1;
    }
    else
      Notify = (int (*)(TreeshrHookType,char *,int,int))-1;
  }
*/
  return status;
}
#endif
