#include <mdsdescrip.h>
#include <string.h>
#include <stdlib.h>
#include <mdsshr.h>
#include <mds_stdarg.h>
#include <status.h>
#define MAX_ARGUMENTS 2
extern int TdiCompile();
extern int TdiEvaluate();
extern int TdiExecute();
#ifndef va_count
#define  va_count(narg) va_start(ap, expr); \
                        for (narg=1; (narg < 256) && (va_arg(ap, struct descriptor *) != MdsEND_ARG); narg++)
#endif				/* va_count */

int ReadInt(char *expr, ...)
{
  INIT_STATUS;
  static struct descriptor expr_dsc = { 0, DTYPE_T, CLASS_S, 0 };
  int ans;
  struct descriptor *dsc_ptrs[MAX_ARGUMENTS];
  int numargs;
  static struct descriptor_xd ans_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
  va_list ap;
  va_count(numargs);

  if ((numargs - 1) > MAX_ARGUMENTS)
    return 0;
  if (numargs > 1) {
    int i;
    va_start(ap, expr);
    for (i = 0; i < numargs - 1; i++)
      dsc_ptrs[i] = va_arg(ap, struct descriptor *);
    va_end(ap);
  }
  expr_dsc.length = strlen(expr);
  expr_dsc.pointer = expr;
  switch (numargs) {
  case 1:
    status = TdiExecute(&expr_dsc, &ans_xd MDS_END_ARG);
    break;
  case 2:
    status = TdiExecute(&expr_dsc, dsc_ptrs[0], &ans_xd MDS_END_ARG);
    break;
  case 3:
    status = TdiExecute(&expr_dsc, dsc_ptrs[0], dsc_ptrs[1], &ans_xd MDS_END_ARG);
    break;
  }
  if STATUS_OK {
    /*    status = TdiEvaluate(&ans_xd, &ans_xd); */
    if STATUS_OK {
      struct descriptor *d_ptr;
      for (d_ptr = (struct descriptor *)&ans_xd;
	   d_ptr->dtype == DTYPE_DSC; d_ptr = (struct descriptor *)d_ptr->pointer) ;
      if (d_ptr->dtype == DTYPE_L) {
	/*
	   ans = (int)malloc(sizeof(int));
	 */
	ans = *(int *)d_ptr->pointer;
      } else if (d_ptr->dtype == DTYPE_NID) {
	/*
	   ans = (int)malloc(sizeof(int));
	 */
	ans = *(int *)d_ptr->pointer;
      } else if (d_ptr->dtype == DTYPE_WU) {
	/*
	   ans = (int)malloc(sizeof(int));
	 */
	ans = *(unsigned short *)d_ptr->pointer;
      } else if (d_ptr->dtype == DTYPE_W) {
	/*
	   ans = (int)malloc(sizeof(int));
	 */
	ans = *(short *)d_ptr->pointer;
      } else if (d_ptr->dtype == DTYPE_BU) {
	/*
	   ans = (int)malloc(sizeof(int));
	 */
	ans = *(char *)d_ptr->pointer;
      } else
	ans = 0;
    } else
      ans = 0;
    MdsFree1Dx(&ans_xd, 0);
  } else
    ans = 0;
  return ans;
}
