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
#include "complex.h"

complex AddC(complex c1, complex c2)
{
    complex ris;
    ris.re = c1.re + c2.re;
    ris.im = c1.im + c2.im;
    return ris;
}

complex SubC(complex c1, complex c2)
{
    complex ris;
    ris.re = c1.re - c2.re;
    ris.im = c1.im - c2.im;
    return ris;
}


complex MulC(complex c1, complex c2)
{
    complex ris;

//This ckeck is required to avoid floating point underflow!!
    if(fabs(c1.re) < 1E-30) c1.re = 0;
    if(fabs(c2.re) < 1E-30) c2.re = 0;
    if(fabs(c1.im) < 1E-30) c1.im = 0;
    if(fabs(c2.im) < 1E-30) c2.im = 0;
    
    
    ris.re = c1.re * c2.re - c1.im * c2.im;
    ris.im = c1.re * c2.im + c2.re * c1.im;
    return ris;
}

complex DivC(complex c1, complex c2)
{
    complex ris;
    double den;
    den = c2.re * c2.re + c2.im * c2.im;
    if(den == 0)
    {
	printf("DivC: division by Zero\n");
	exit(1);
    }
    ris.re = (c1.re * c2.re + c1.im * c2.im)/den;;
    ris.im = (c1.im * c2.re - c1.re * c2.im)/den;
    return ris;
}


complex ExpC(complex c)
{
    complex ris;
    ris.re = exp(c.re) * cos(c.im);
    ris.im = exp(c.re) * sin(c.im);
    return ris;
}

double Mod2(complex c)
{
    return c.re * c.re + c.im * c.im;
}
