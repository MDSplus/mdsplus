//static char *cvsrev = "@(#)$RCSfile$ $Revision$ $Date$";
#include <mdsdescrip.h>
#include <string.h>
#include <stdlib.h>
#include <mdsshr.h>
#include <mds_stdarg.h>
#include <status.h>
#ifndef va_count
#define  va_count(narg) va_start(ap, expr); \
                        for (narg=1; (narg < 256) && (va_arg(ap, struct descriptor *) != MdsEND_ARG); narg++)
#endif				/* va_count */

#define MAX_ARGUMENTS 2
extern int TdiData();
extern int TdiCompile();
extern int TdiExecute();
char *ReadString(char *expr, ...)
{
  INIT_STATUS;
  static struct descriptor expr_dsc = { 0, DTYPE_T, CLASS_S, 0 };
  char *ans;
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
    /*    status = TdiData(&ans_xd, &ans_xd); */
    if STATUS_OK {
      struct descriptor *d_ptr;
      for (d_ptr = (struct descriptor *)&ans_xd;
	   d_ptr->dtype == DTYPE_DSC; d_ptr = (struct descriptor *)d_ptr->pointer) ;
      if (d_ptr->dtype == DTYPE_T) {
	ans = (char *)malloc((d_ptr->length + 1) * sizeof(char));
	strncpy(ans, d_ptr->pointer, d_ptr->length);
	ans[d_ptr->length] = 0;
      } else
	ans = 0;
    } else
      ans = 0;
    MdsFree1Dx(&ans_xd, 0);
  } else
    ans = 0;
  return ans;
}
