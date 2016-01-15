/*  DEC/CMS REPLACEMENT HISTORY, Element COMPLEX.C */
/*  *10   15-MAR-1995 11:20:02 MDSPLUS "Compatibility" */
/*  *9    14-MAR-1995 16:59:10 MDSPLUS "ANSI C" */
/*  *8    13-MAR-1995 17:32:04 MDSPLUS "New Version" */
/*  *7    16-NOV-1994 12:44:16 MDSPLUS "OSF/1 compatibile" */
/*  *6    15-NOV-1994 16:32:09 MDSPLUS "Ported to OSF1" */
/*  *5    15-NOV-1994 15:15:21 MDSPLUS "Move to OSF1" */
/*  *4    15-NOV-1994 15:14:59 MDSPLUS "Move to OSF1" */
/*  *3    24-OCT-1994 19:18:22 MDSPLUS "Proceede" */
/*  *2    19-OCT-1994 18:24:08 MDSPLUS "Proceede" */
/*  *1    18-OCT-1994 16:53:53 MDSPLUS "Complex operations" */
/*  DEC/CMS REPLACEMENT HISTORY, Element COMPLEX.C */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "complex.h"
#include <config.h>

EXPORT Complex AddC(Complex c1, Complex c2)
{
  //This ckeck is required to avoid floating point underflow!!
  if (fabs(c1.re) < 1E-30)
    c1.re = 0;
  if (fabs(c2.re) < 1E-30)
    c2.re = 0;
  if (fabs(c1.im) < 1E-30)
    c1.im = 0;
  if (fabs(c2.im) < 1E-30)
    c2.im = 0;
  Complex ris;
  ris.re = c1.re + c2.re;
  ris.im = c1.im + c2.im;
  return ris;
}

EXPORT Complex SubC(Complex c1, Complex c2)
{
  Complex ris;
//This ckeck is required to avoid floating point underflow!!
  if (fabs(c1.re) < 1E-30)
    c1.re = 0;
  if (fabs(c2.re) < 1E-30)
    c2.re = 0;
  if (fabs(c1.im) < 1E-30)
    c1.im = 0;
  if (fabs(c2.im) < 1E-30)
    c2.im = 0;
  ris.re = c1.re - c2.re;
  ris.im = c1.im - c2.im;
  return ris;
}

EXPORT Complex MulC(Complex c1, Complex c2)
{
  Complex ris;

//This ckeck is required to avoid floating point underflow!!
  if (fabs(c1.re) < 1E-30)
    c1.re = 0;
  if (fabs(c2.re) < 1E-30)
    c2.re = 0;
  if (fabs(c1.im) < 1E-30)
    c1.im = 0;
  if (fabs(c2.im) < 1E-30)
    c2.im = 0;

  ris.re = c1.re * c2.re - c1.im * c2.im;
  ris.im = c1.re * c2.im + c2.re * c1.im;
  return ris;
}

EXPORT Complex DivC(Complex c1, Complex c2)
{
  Complex ris;
  double den;
  //This ckeck is required to avoid floating point underflow!!
  if (fabs(c1.re) < 1E-30)
    c1.re = 0;
  if (fabs(c2.re) < 1E-30)
    c2.re = 0;
  if (fabs(c1.im) < 1E-30)
    c1.im = 0;
  if (fabs(c2.im) < 1E-30)
    c2.im = 0;
  den = c2.re * c2.re + c2.im * c2.im;
  if (den == 0) {
    printf("DivC: division by Zero\n");
    exit(1);
  }
  ris.re = (c1.re * c2.re + c1.im * c2.im) / den;;
  ris.im = (c1.im * c2.re - c1.re * c2.im) / den;
  return ris;
}

EXPORT Complex ExpC(Complex c)
{
  Complex ris;
  //This ckeck is required to avoid floating point underflow!!
  if (fabs(c.re) < 1E-30)
    c.re = 0;
  if (fabs(c.im) < 1E-30)
    c.im = 0;
  ris.re = exp(c.re) * cos(c.im);
  ris.im = exp(c.re) * sin(c.im);
  return ris;
}

EXPORT double Mod2(Complex c)
{
  //This ckeck is required to avoid floating point underflow!!
  if (fabs(c.re) < 1E-30)
    c.re = 0;
  if (fabs(c.im) < 1E-30)
    c.im = 0;
  return c.re * c.re + c.im * c.im;
}
