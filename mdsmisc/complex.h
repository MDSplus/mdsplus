/*  DEC/CMS REPLACEMENT HISTORY, Element COMPLEX.H */
/*  *10   13-MAR-1995 17:32:08 MDSPLUS "New Version" */
/*  *9    16-NOV-1994 12:44:18 MDSPLUS "OSF/1 compatibile" */
/*  *8    15-NOV-1994 16:32:14 MDSPLUS "Ported to OSF1" */
/*  *7     7-NOV-1994 17:29:08 MDSPLUS "Proceede" */
/*  *6     7-NOV-1994 12:53:03 MDSPLUS "Proceede" */
/*  *5     7-NOV-1994 11:45:40 MDSPLUS "Proceede" */
/*  *4    24-OCT-1994 19:18:46 MDSPLUS "Proceede" */
/*  *3    19-OCT-1994 18:23:27 MDSPLUS "Proceede" */
/*  *2    18-OCT-1994 16:55:07 MDSPLUS "Proceede" */
/*  *1    18-OCT-1994 16:44:16 MDSPLUS "Include file for compelx operations" */
/*  DEC/CMS REPLACEMENT HISTORY, Element COMPLEX.H */
#ifndef COMPLEX_H
#define COMPLEX_H
#define PI 3.1415926535898

typedef struct
{
  double re;
  double im;
} Complex;

Complex MulC(Complex, Complex);
Complex AddC(Complex, Complex);
Complex DivC(Complex, Complex);
Complex SubC(Complex, Complex);
Complex ExpC(Complex);
double Mod2(Complex);

#endif
