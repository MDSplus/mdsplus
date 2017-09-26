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
/*      Tdi3Square.C
        Interludes for some simple stuff.

        Ken Klare, LANL CTR-7   (c)1989,1990
*/
#include <mdsdescrip.h>
#include <status.h>
#include <STATICdef.h>



extern int Tdi3Multiply();
extern int Tdi3Complex();
extern int Tdi3UnaryMinus();
extern int Tdi3Floor();

/*--------------------------------------------------------------
        Square a number by multiplying.
*/
int Tdi3Square(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  return Tdi3Multiply(in_ptr, in_ptr, out_ptr);
}

/*--------------------------------------------------------------
        F90 elemental, convert complex call to molded type.
*/
int Tdi3Cmplx(struct descriptor *x_ptr,
	      struct descriptor *y_ptr, struct descriptor *mold_ptr __attribute__ ((unused)), struct descriptor *out_ptr)
{
  return Tdi3Complex(x_ptr, y_ptr, out_ptr);
}

/*--------------------------------------------------------------
        F90 elemental, round to higher integral value. In CC it is CEIL.
*/
int Tdi3Ceiling(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;

  status = Tdi3UnaryMinus(in_ptr, out_ptr);
  if STATUS_OK
    status = Tdi3Floor(out_ptr, out_ptr);
  if STATUS_OK
    status = Tdi3UnaryMinus(out_ptr, out_ptr);
  return status;
}
