/* 
 * Curve fitting routines for DT200 CPCI digitizers.
 * adapted from B5910 code.
 *
 * Josh Stillerman 2/21/02
 */

#include <stdlib.h>
#ifdef VMS
    extern void math$csakm();
    extern float math$csval();
#else
    extern void CSAKM();
    extern float CSVAL();
#endif

int LinFit(int *num_knots, float *knots_x, float *knots_y, int *num_v, float *x, float *y)
{
  float     slope;
  float     intercept;
  int       i;
  int       knot_idx = 1;
  float     high_x = 0.0;
  if (*num_knots > 1)
  {
    slope = (knots_x[1] == knots_x[0]) ? 0 : (knots_y[1] - knots_y[0]) / (knots_x[1] - knots_x[0]);
    intercept = knots_y[1] - slope * knots_x[1];
    high_x = knots_x[1];
  }
  else if (*num_knots == 1)
  {
    slope = 0.0;
    intercept = knots_y[0];
  }
  else
  {
    slope = 0.0;
    intercept = 0.0;
  }
  if (*num_v > 0)
  {
    for (i = 0; i < *num_v; i++)
    {
      while ((x[i] > high_x) && (knot_idx < (*num_knots-1)))
      {
        knot_idx++;
        slope = (knots_x[knot_idx] == knots_x[knot_idx-1]) ? 0 :
                    (knots_y[knot_idx] - knots_y[knot_idx-1]) / (knots_x[knot_idx] - knots_x[knot_idx-1]);
        intercept = knots_y[knot_idx] - slope * knots_x[knot_idx];
        high_x = knots_x[knot_idx];
      }
      y[i] = slope * x[i] + intercept;
    }
  }
  else
  {
    float     min_x = x[0];
    float     max_x = x[-(*num_v)-1];
    int j = 0;
    while ((min_x > high_x) && (knot_idx < (*num_knots-1)))
    {
      knot_idx++;
      slope = (knots_x[knot_idx] == knots_x[knot_idx-1]) ? 0 :
                    (knots_y[knot_idx] - knots_y[knot_idx-1]) / (knots_x[knot_idx] - knots_x[knot_idx-1]);
      intercept = knots_y[knot_idx] - slope * knots_x[knot_idx];
      high_x = knots_x[knot_idx];
    }
    y[j++] = slope * min_x + intercept;
    while ((max_x > high_x) && (knot_idx < (*num_knots-1)))
    {
      y[j] = knots_y[knot_idx];
      x[j++] = knots_x[knot_idx];
      knot_idx++;
      slope = (knots_x[knot_idx] == knots_x[knot_idx-1]) ? 0 :
                    (knots_y[knot_idx] - knots_y[knot_idx-1]) / (knots_x[knot_idx] - knots_x[knot_idx-1]);
      intercept = knots_y[knot_idx] - slope * knots_x[knot_idx];
      high_x = knots_x[knot_idx];
    }
    y[j] = slope * max_x + intercept;
    x[j++] = max_x;
    *num_v = j;
  }
  return 1;
}
int SplineFit(int *num_knots, float *knots_x, float *knots_y, int *num_v, float *x, float *y)
{
  int  i;
  if (*num_knots > 2)
  {
    float    *fbreak = (float *)calloc(*num_knots, sizeof(float));
    float    *cscoef = (float *)calloc(*num_knots * 4, sizeof(float));
#ifdef VMS
    math$csakm(num_knots, knots_x, knots_y, fbreak, cscoef);
#else
    CSAKM(num_knots, knots_x, knots_y, fbreak, cscoef);
#endif    
    for (i = 0; i < *num_v; i++)
#ifdef VMS
      y[i] = math$csval(&x[i], num_knots, fbreak, cscoef);
#else
      y[i] = CSVAL(&x[i], num_knots, fbreak, cscoef);
#endif
    free((char *)fbreak);
    free((char *)cscoef);
    return 1;
  }
  else
    return LinFit(num_knots, knots_x, knots_y, num_v, x, y);
}
