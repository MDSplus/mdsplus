/*  CMS REPLACEMENT HISTORY, Element ISROPRAND.C */
/*  *2    28-AUG-1996 10:45:43 TWF "Cleanup" */
/*  *1    27-AUG-1996 07:51:38 TWF "Test for reserved oprand" */
/*  CMS REPLACEMENT HISTORY, Element ISROPRAND.C */
#include <mdsdescrip.h>

typedef struct {unsigned hi : 7; unsigned exp :  8; unsigned sign : 1; unsigned low : 16;} f_float;
typedef struct {unsigned hi : 7; unsigned exp :  8; unsigned sign : 1; unsigned low : 16; unsigned int low2;} d_float;
typedef struct {unsigned hi : 4; unsigned exp : 11; unsigned sign : 1; unsigned low : 16; unsigned int low2;} g_float;
typedef struct {		 unsigned exp : 15; unsigned sign : 1; unsigned low : 16; unsigned int low2[3];} h_float;
typedef struct {unsigned low:16; unsigned hi : 7;   unsigned exp:8;    unsigned sign :1;} s_float;
typedef struct {unsigned int low2; unsigned low:16; unsigned hi : 4; unsigned exp:11; unsigned sign :1;} t_float;

#define IsRoprandF(val) ((((f_float *)val)->exp == 0) && (((f_float *)val)->sign == 1))
#define IsRoprandS(val) (((s_float *)val)->exp == 255)
#define IsRoprandD(val) ((((d_float *)val)->exp == 0) && (((d_float *)val)->sign == 1))
#define IsRoprandG(val) ((((g_float *)val)->exp == 0) && (((g_float *)val)->sign == 1))
#define IsRoprandT(val) (((t_float *)val)->exp == 2047)

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
