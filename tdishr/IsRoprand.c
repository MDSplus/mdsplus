/*  CMS REPLACEMENT HISTORY, Element ISROPRAND.C */
/*  *2    28-AUG-1996 10:45:43 TWF "Cleanup" */
/*  *1    27-AUG-1996 07:51:38 TWF "Test for reserved oprand" */
/*  CMS REPLACEMENT HISTORY, Element ISROPRAND.C */
#include <mdsdescrip.h>

#define f_float_exp(val) ((*(int *)val >> 7) & 0xff)
#define f_float_sign(val) ((*(int *)val >> 15) &0x1)
#define IsRoprandF(val) ((f_float_exp(val) == 0) && (f_float_sign(val) == 1))
#define s_float_exp(val) ((*(int *)val >> 23) & 0xff)
#define IsRoprandS(val) (s_float_exp(val) == 255)
#define IsRoprandD(val) IsRoprandF(val)
#define g_float_exp(val) ((*(int *)val >> 4) & 0x7ff)
#define g_float_sign(val) ((*(int *)val >> 15) &0x1)
#define IsRoprandG(val) ((g_float_exp(val) == 0) && (g_float_sign(val) == 1))
#define t_float_exp(val) ((((int *)val)[1] >> 4) & 0x7ff)
#define IsRoprandT(val) (t_float_exp(val) == 2047)

int IsRoprand(int dtype,void *value)
{
  int ans = 0;
  switch (dtype)
  {
    case DTYPE_F  :  ans = IsRoprandF(value); break;
    case DTYPE_D  :  ans = IsRoprandD(value); break;
    case DTYPE_G  :  ans = IsRoprandG(value); break;
    case DTYPE_FS :  ans = IsRoprandS(value); break;
    case DTYPE_T  :  ans = IsRoprandT(value); break;
  }
  return ans;
}
