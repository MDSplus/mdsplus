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
/*      TDI$UNDEF.C unwritten routines

        Ken Klare, LANL P-4     (c)1989,1990,1992
*/

#include <tdishr_messages.h>
#include <STATICdef.h>


#define UNDEF(x) int x() {return TdiNO_OPC;}
/*** PERMANENT ***/
UNDEF(Tdi1Else)
    UNDEF(Tdi2undef)
    UNDEF(Tdi3undef)

/*** TEMPORARY ***/
    UNDEF(Tdi1Matrix)
    UNDEF(Tdi1Shift)

    UNDEF(Tdi1Backspace)
    UNDEF(Tdi1Convolve)
    UNDEF(Tdi1DateAndTime)
    UNDEF(Tdi1Decode)
    UNDEF(Tdi1Element)
    UNDEF(Tdi1Encode)
    UNDEF(Tdi1Endfile)
    UNDEF(Tdi1Fft)
    UNDEF(Tdi1Fit)
    UNDEF(Tdi1Inquire)
    UNDEF(Tdi1Interpol)
    UNDEF(Tdi1Intersect)
    UNDEF(Tdi1Inverse)
    UNDEF(Tdi1Laminate)
    UNDEF(Tdi1Matmul)
    UNDEF(Tdi1OnError)
    UNDEF(Tdi1Project)
    UNDEF(Tdi1Promote)
    UNDEF(Tdi1RandomSeed)
    /*
       UNDEF(Tdi1Read)
     */
    UNDEF(Tdi1Rebin)
    UNDEF(Tdi1Reshape)
    UNDEF(Tdi1Rewind)
    UNDEF(Tdi1Seek)
    UNDEF(Tdi1String)
    UNDEF(Tdi1Transfer)
    UNDEF(Tdi1Unpack)
    UNDEF(Tdi1Where)

    UNDEF(Tdi3Ibits)
    UNDEF(Tdi3Cshift)
    UNDEF(Tdi3Derivative)
    UNDEF(Tdi3Eoshift)
    UNDEF(Tdi3Integral)
    UNDEF(Tdi3Inverse)
    UNDEF(Tdi3Ishftc)
    UNDEF(Tdi3Matmul)
    UNDEF(Tdi3MatRot)
    UNDEF(Tdi3MatRotInt)
    UNDEF(Tdi3Median)
    UNDEF(Tdi3Modulo)
    UNDEF(Tdi3Nearest)
    UNDEF(Tdi3RcDroop)
    UNDEF(Tdi3Sign)
    UNDEF(Tdi3Smooth)
    UNDEF(Tdi3Solve)
    UNDEF(Tdi3SystemClock)
    UNDEF(Tdi3Transpose)
    UNDEF(Tdi3TransposeMul)
    UNDEF(Tdi3Rms)
    UNDEF(Tdi3StdDev)
