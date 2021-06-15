/*  DEC/CMS REPLACEMENT HISTORY, Element FILTER.H */
/*  *20   13-MAR-1995 17:32:27 MDSPLUS "New Version" */
/*  *19   16-NOV-1994 12:44:27 MDSPLUS "OSF/1 compatibile" */
/*  *18   15-NOV-1994 16:32:23 MDSPLUS "Ported to OSF1" */
/*  *17   11-NOV-1994 15:40:11 MDSPLUS "Proceede" */
/*  *16   11-NOV-1994 15:36:35 MDSPLUS "Proceede" */
/*  *15   11-NOV-1994 15:35:48 MDSPLUS "Proceede" */
/*  *14   11-NOV-1994 13:39:43 MDSPLUS "Proceede" */
/*  *13   10-NOV-1994 16:46:55 MDSPLUS "Proceede" */
/*  *12   10-NOV-1994 16:45:52 MDSPLUS "Proceede" */
/*  *11   10-NOV-1994 16:43:15 MDSPLUS "Proceede" */
/*  *10   10-NOV-1994 16:42:08 MDSPLUS "Proceede" */
/*  *9     7-NOV-1994 17:28:31 MDSPLUS "Proceede" */
/*  *8     7-NOV-1994 13:44:22 MDSPLUS "Proceede" */
/*  *7     7-NOV-1994 11:53:45 MDSPLUS "Proceede" */
/*  *6     2-NOV-1994 18:03:39 MDSPLUS "Proceede" */
/*  *5    28-OCT-1994 17:38:40 MDSPLUS "Proceede" */
/*  *4    26-OCT-1994 15:04:29 MDSPLUS "Proceede" */
/*  *3    25-OCT-1994 17:19:13 MDSPLUS "Proceede" */
/*  *2    19-OCT-1994 18:42:40 MDSPLUS "Proceede" */
/*  *1    18-OCT-1994 16:54:48 MDSPLUS "Include file for filter operations" */
/*  DEC/CMS REPLACEMENT HISTORY, Element FILTER.H */
#include "complex.h"

typedef struct
{
  int num_degree, den_degree;
  double *num, *den;
} FilterUnit;

typedef struct
{
  int num_parallels;
  FilterUnit *units;
} Filter;

#define MAX_FILTER_BUF 64
#define MAX_FILTER_UNITS 16

typedef struct
{
  double oldX[MAX_FILTER_BUF];
  double oldY[MAX_FILTER_UNITS][MAX_FILTER_BUF];
  int idx;
} RunTimeFilter;

/* Public Function prototypes */
Filter *ButtwInvar(float *fp, float *fs, float *ap, float *as, float *fc,
                   int *out_n);
Filter *ButtwBilinear(float *fp, float *fs, float *ap, float *as, float *fc,
                      int *out_n);
Filter *ChebInvar(float *fp, float *fs, float *ap, float *as, float *fc,
                  int *out_n);
Filter *ChebBilinear(float *fp, float *fs, float *ap, float *as, float *fc,
                     int *out_n);
Filter *BessInvar(float *fp, float *fs, float *ap, float *as, float *fc,
                  float *delay, int *out_n);

Filter *FirRectangular(float *fc, float *s_f, int *n);
Filter *FirBartlett(float *fc, float *s_f, int *n);
Filter *FirHanning(float *fc, float *s_f, int *n);
Filter *FirHamming(float *fc, float *s_f, int *n);
Filter *FirBlackmann(float *fc, float *s_f, int *n);

void DoFilter(Filter *filter, float *in, float *out, int *n_samples);
void DoFilterResample(Filter *filter, float *in, float *out, int *n_samples,
                      int *start_idx, int *delta_idx, int *max_out_idx);
void DoFilterResampleVME(Filter *filter, short *in, float *out, int *n_samples,
                         int *start_idx, int *delta_idx, int *max_out_idx,
                         int step_raw);
void TestFilter(Filter *filter, float fc, int n_points, float *module,
                float *phase);
void FreeFilter(Filter *filter);

/* Public Function prototypes internally used */
Filter *Invariant(double fp, double fs, double ap, double as, double fc,
                  int *out_n, Complex *(*FindPoles)());
Filter *Bilinear(double fp, double fs, double ap, double as, double fc,
                 int *out_n, Complex *(*FindPoles)());
Complex *FindFactors(Complex *poles, double fc, int n, double gain);

/* Real time support */
// Prepare the description of a butterworth
Filter *prepareFilter(float cutFreq, float samplingFreq, int numPoles);
// Initialize then data structures whichb are required for real-time filtering
void initializeRunTimeFilter(RunTimeFilter *rtf);
/* Perform step filtering */
double getFiltered(double in, Filter *flt, RunTimeFilter *rtf);
