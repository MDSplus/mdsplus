/*------------------------------------------------------------------------------

		Name:   TREE$DO_METHOD

		Type:   C function

		Author:	Josh Stillerman

		Date:   27-APR-1989

		Purpose: Do method on nid

------------------------------------------------------------------------------

	Call sequence:

int TreeDoMethod( nid_dsc, method_dsc [,args]...)

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:


------------------------------------------------------------------------------*/

#include <mdsdescrip.h>
#include <treeshr.h>
#include "treeshrp.h"
#include <ncidef.h>
#include <libroutines.h>
#include <strroutines.h>
#include <mds_stdarg.h>

#define  count(num) va_start(incrmtr, nid_dsc); \
                     for (num=1; (num < 256) && (va_arg(incrmtr, struct descriptor *) != MdsEND_ARG);  num++)
  
extern void *DBID;

int TreeDoMethod(struct descriptor *nid_dsc, struct descriptor *method_ptr, ...)
{
  int i;
  int nargs;
  void *arglist[256];
  va_list   incrmtr;
  count(nargs);
  arglist[0] = (void *)(nargs + 1);
  arglist[1] = DBID;
  arglist[2] = nid_dsc;
  arglist[3] = method_ptr;
  va_start(incrmtr, method_ptr);
  for (i = 3; i <= nargs ; i++)
    arglist[i + 1] = va_arg(incrmtr, struct descriptor *);
  va_end(incrmtr);
  return LibCallg(arglist,_TreeDoMethod);
}

int _TreeDoMethod(void *dbid, struct descriptor *nid_dsc, struct descriptor *method_ptr, ...)
{
  va_list   incrmtr;
  int       status = TreeNOMETHOD;
  static short conglomerate_elt;
  static unsigned char data_type;
  static int head_nid;
  NCI_ITM itmlst[] = {{1, NciDTYPE, &data_type, 0},
		      {2, NciCONGLOMERATE_ELT, (unsigned char *)&conglomerate_elt, 0},
		      {4, NciCONGLOMERATE_NIDS, (unsigned char *)&head_nid, 0},
		      {0, NciEND_OF_LIST, 0, 0}};
  void       (*addr)();
  static struct descriptor_xd xd = {0, 0, CLASS_XD, 0, 0};
  static    DESCRIPTOR(underunder, "__");
  static struct descriptor_d method = {0, DTYPE_T, CLASS_D, 0};
  static struct descriptor_conglom *conglom_ptr;
  int nargs;
  void *arglist[256];
  count(nargs);
  arglist[0] = (void *)nargs;

  if (nid_dsc->dtype != DTYPE_NID || (!nid_dsc->pointer))
    return TreeNOMETHOD;
  head_nid = 0;
  status = _TreeGetNci(dbid, *(int *)nid_dsc->pointer, itmlst);
  if (!(status & 1))
    return status;

  if (conglomerate_elt || (data_type == DTYPE_CONGLOM))
  {
    int       i;
    arglist[1] = nid_dsc;
    arglist[2] = method_ptr;
    va_start(incrmtr, method_ptr);
    for (i = 2; i < nargs; i++)
      arglist[i + 1] = va_arg(incrmtr, struct descriptor *);
    va_end(incrmtr);
    status = _TreeGetRecord(dbid, head_nid ? head_nid : *((int *) nid_dsc->pointer), &xd);
    if (!(status & 1))
      return status;
    conglom_ptr = (struct descriptor_conglom *) xd.pointer;
    if (conglom_ptr->dtype != DTYPE_CONGLOM)
      return TreeNOT_CONGLOM;
    StrConcat(&method, conglom_ptr->model, &underunder, method_ptr MDS_END_ARG);
    /*
    lib$establish(TREE$DO_METHOD_HANDLER);
    */
    status = LibFindImageSymbol(conglom_ptr->image, &method, &addr);
    /*
    lib$revert();
    */
    if (status & 1)
    {
      void *old_dbid = DBID;
      DBID = dbid;
      status = LibCallg(arglist, addr);
      DBID = old_dbid;
    }
    else
    {
      /**** Try tdi fun ***/
      struct descriptor exp = {0, DTYPE_T, CLASS_D, 0};
      static DESCRIPTOR(open,"(");
      static DESCRIPTOR(close,"$)");
      static DESCRIPTOR(arg,"$,");
      static DESCRIPTOR(tdishr,"TdiShr");
      static DESCRIPTOR(tdiexecute,"TdiExecute");
      StrCopyDx(&exp, &method);
      StrAppend(&exp,&open);
      for (i=2;i<nargs;i++) StrAppend(&exp,&arg);
      StrAppend(&exp,&close);
      status = LibFindImageSymbol(&tdishr,&tdiexecute,&addr);
      if (status & 1)
      {
        for (i=nargs;i>0;i--) arglist[i+1] = arglist[i];
        nargs += 2;
        arglist[0] = (void *)nargs;
        arglist[1] = &exp;
        arglist[nargs] = MdsEND_ARG;
        status = LibCallg(arglist,addr);
      }
      StrFree1Dx(&exp);
      /*      status = TreeNOMETHOD; */
    }
  }
  else
    status = TreeNOMETHOD;
  /*
  if (!(status&1)) lib$signal(status, 0);
  */
  return status;
}
