#include <mdsdescrip.h>


struct descriptor *fformat(double *f, int *width, int *precision)
{
  static char c[1024];
  static struct descriptor d = {0, DTYPE_T, CLASS_S, c};
  d.length = sprintf(c,"%*.*f",*width,*precision,*f);
  return &d;
}


  
