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
#include <STATICdef.h>
#include <ncidef.h>
#include "treeshrp.h"
#include <mdsshr.h>
#include <mdsdescrip.h>
#include <treeshr.h>
#include <tdishr_messages.h>
#include <libroutines.h>
#include <strroutines.h>
#include <mds_stdarg.h>
#include <string.h>
#include <mdsshr_messages.h>
#include <ctype.h>

#define  count(num) va_start(incrmtr, method_ptr); \
                     for (num=2; (num < 256) && (va_arg(incrmtr, struct descriptor *) != MdsEND_ARG);  num++)

#define arglist_nargs(nargs) (void *)((char *)0+(nargs));
extern void **TreeCtx();

int TreeDoMethod(struct descriptor *nid_dsc, struct descriptor *method_ptr, ...)
{
  int i;
  int nargs;
  void *arglist[256];
  va_list incrmtr;
  count(nargs);
  arglist[0] = arglist_nargs(nargs + 2);
  arglist[1] = *TreeCtx();
  arglist[2] = nid_dsc;
  arglist[3] = method_ptr;
  va_start(incrmtr, method_ptr);
  for (i = 3; i <= nargs; i++)
    arglist[i + 1] = va_arg(incrmtr, struct descriptor *);
  va_end(incrmtr);
  arglist[nargs + 2] = MdsEND_ARG;
  return (int)((char *)LibCallg(arglist, _TreeDoMethod) - (char *)0);
}

int TreeDoFun(struct descriptor *funname, int nargs, struct descriptor **args,
	      struct descriptor_xd *out_ptr)
{
  int status;
  static void (*TdiEvaluate) () = 0;
  int i;
  short OpcExtFunction = 162;
  STATIC_CONSTANT DESCRIPTOR(tdishr, "TdiShr");
  STATIC_CONSTANT DESCRIPTOR(tdievaluate, "TdiEvaluate");
  DESCRIPTOR_FUNCTION(fun, &OpcExtFunction, 255);
  void *call_arglist[] = { (void *)3, (void *)&fun, (void *)out_ptr, MdsEND_ARG };
  fun.ndesc = nargs + 2;
  fun.arguments[0] = 0;
  fun.arguments[1] = funname;
  for (i = 0; i < nargs; i++)
    fun.arguments[2 + i] = args[i];
  if ((status = TdiEvaluate ? 1 : LibFindImageSymbol(&tdishr, &tdievaluate, &TdiEvaluate)) & 1)
    status = (int)((char *)LibCallg(call_arglist, TdiEvaluate) - (char *)0);
  return status;
}

int _TreeDoMethod(void *dbid, struct descriptor *nid_dsc, struct descriptor *method_ptr, ...)
{
  va_list incrmtr;
  int status = TreeNOMETHOD;
  static short conglomerate_elt;
  static unsigned char data_type;
  static int head_nid;
  NCI_ITM itmlst[] = { {1, NciDTYPE, &data_type, 0},
  {2, NciCONGLOMERATE_ELT, (unsigned char *)&conglomerate_elt, 0},
  {4, NciCONGLOMERATE_NIDS, (unsigned char *)&head_nid, 0},
  {0, NciEND_OF_LIST, 0, 0}
  };
  void (*addr) ();
  static int (*TdiExecute) () = 0;
  STATIC_CONSTANT DESCRIPTOR(close, "$)");
  STATIC_CONSTANT DESCRIPTOR(arg, "$,");
  STATIC_CONSTANT DESCRIPTOR(tdishr, "TdiShr");
  STATIC_CONSTANT DESCRIPTOR(tdiexecute, "TdiExecute");
  static struct descriptor_xd xd = { 0, 0, CLASS_XD, 0, 0 };
  static DESCRIPTOR(underunder, "__");
  static struct descriptor_d method = { 0, DTYPE_T, CLASS_D, 0 };
  static struct descriptor_conglom *conglom_ptr;
  int nargs;
  void *arglist[256];
  count(nargs);
  arglist[0] = arglist_nargs(nargs);

  if (nid_dsc->dtype != DTYPE_NID || (!nid_dsc->pointer))
    return TreeNOMETHOD;
  head_nid = 0;
  status = _TreeGetNci(dbid, *(int *)nid_dsc->pointer, itmlst);
  if (!(status & 1))
    return status;

  if (conglomerate_elt || (data_type == DTYPE_CONGLOM)) {
    int i;
    arglist[1] = nid_dsc;
    arglist[2] = method_ptr;
    va_start(incrmtr, method_ptr);
    for (i = 2; i < nargs; i++)
      arglist[i + 1] = va_arg(incrmtr, struct descriptor *);
    va_end(incrmtr);
    status = _TreeGetRecord(dbid, head_nid ? head_nid : *((int *)nid_dsc->pointer), &xd);
    if (!(status & 1))
      return status;
    conglom_ptr = (struct descriptor_conglom *)xd.pointer;
    if (conglom_ptr->dtype != DTYPE_CONGLOM)
      return TreeNOT_CONGLOM;
    if (conglom_ptr->image && conglom_ptr->image->length == strlen("__python__")
	&& strncmp(conglom_ptr->image->pointer, "__python__", strlen("__python__")) == 0) {
      void *dbid = *TreeCtx();
      /**** Try python class ***/
      struct descriptor_d exp = { 0, DTYPE_T, CLASS_D, 0 };
      STATIC_CONSTANT DESCRIPTOR(open, "PyDoMethod(");
      StrCopyDx((struct descriptor *)&exp, (struct descriptor *)&open);
      if (nargs == 4 && method_ptr->length == strlen("DW_SETUP")
	  && strncmp(method_ptr->pointer, "DW_SETUP", strlen("DW_SETUP")) == 0) {
	arglist[3] = arglist[4];
	nargs--;
      }
      for (i = 1; i < nargs - 1; i++)
	StrAppend(&exp, (struct descriptor *)&arg);
      StrAppend(&exp, (struct descriptor *)&close);
      if (TdiExecute == 0)
	status = LibFindImageSymbol(&tdishr, &tdiexecute, &TdiExecute);
      if (status & 1) {
	EMPTYXD(list_xd);
	struct descriptor_xd *ans_xd=arglist[nargs];
	DESCRIPTOR_LONG(stat_d,&status);
	static DESCRIPTOR(dollar_d,"$");
	arglist[nargs]=&list_xd;
	for (i = nargs; i > 0; i--)
	  arglist[i + 1] = arglist[i];
	nargs += 2;
	arglist[0] = arglist_nargs(nargs);
	arglist[1] = &exp;
	arglist[nargs] = MdsEND_ARG;
	status = (int)((char *)LibCallg(arglist, TdiExecute) - (char *)0);
        if (status & 1) {
	  struct descriptor_a *list = (struct descriptor_a *)list_xd.pointer;
	  if (list && list->dtype == DTYPE_LIST && list->arsize == (sizeof(void *)*2)) {
	    struct descriptor *statd = ((struct descriptor **)list->pointer)[0];
	    struct descriptor *ansd  = ((struct descriptor **)list->pointer)[1];
	    if (ansd) {
	      (*TdiExecute)(&dollar_d, ansd,ans_xd, MdsEND_ARG);
	    } else {
	      (*TdiExecute)(&dollar_d,statd,ans_xd, MdsEND_ARG);
	    }
	  }
	  else {
	    (*TdiExecute)(&dollar_d,&stat_d,ans_xd, MdsEND_ARG);
	    status = 0;
	  }
	  MdsFree1Dx(&list_xd,0);
	}
      }
      StrFree1Dx(&exp);
      *TreeCtx() = dbid;
      return status;
    }
    StrConcat((struct descriptor *)&method, conglom_ptr->model, (struct descriptor *)&underunder,
	      method_ptr MDS_END_ARG);
    for (i = 0; i < method.length; i++)
      method.pointer[i] = tolower(method.pointer[i]);
    if (conglom_ptr->image && conglom_ptr->image->dtype == DTYPE_T)
      status = LibFindImageSymbol(conglom_ptr->image, &method, &addr);
    else
      status = 0;
    if (status & 1) {
      void *old_dbid = *TreeCtx();
      *TreeCtx() = dbid;
      status = (int)((char *)LibCallg(arglist, addr) - (char *)0);
      *TreeCtx() = old_dbid;
      if (arglist[nargs]) {
	struct descriptor *ans = (struct descriptor *)arglist[nargs];
	if (ans->class == CLASS_XD) {
	  DESCRIPTOR_LONG(status_d, 0);
	  status_d.pointer = (char *)&status;
	  MdsCopyDxXd(&status_d, (struct descriptor_xd *)ans);
	} else if ((ans->dtype == DTYPE_L) && (ans->length == 4) && (ans->pointer)) {
	  *(int *)ans->pointer = status;
	}
      }
    } else {
      /**** Try tdi fun ***/
      status =
	  TreeDoFun((struct descriptor *)&method, nargs - 1, (struct descriptor **)&arglist[1],
		    (struct descriptor_xd *)arglist[nargs]);
      if (status == TdiUNKNOWN_VAR)
	status = TreeNOMETHOD;
    }
  } else
    status = TreeNOMETHOD;
  /*
     if (!(status&1)) lib$signal(status, 0);
   */
  return status;
}

