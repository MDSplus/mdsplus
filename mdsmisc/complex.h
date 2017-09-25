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

typedef struct {
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
