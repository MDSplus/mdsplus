/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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
#include <mdsplus/mdsconfig.h>

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
