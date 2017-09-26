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
#include <limits.h>
#include "mdslib.h"

#include <testing.h>

#define FLOAT_TEST 1.234567
#define DOUBLE_TEST 3.141592653589793
#define COMPLEX_TEST0 5.0
#define COMPLEX_TEST1 2.0
#define COMPLEX_DOUBLE_TEST0 2.71828182828459
#define COMPLEX_DOUBLE_TEST1 1.414213562373
#define CSTRING_TEST0 "this is a test"
#define CSTRING_TEST1 "THIS IS A TEST"

int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{
    
    long status;
    int dsc;
    
    int dtype_uchar = DTYPE_UCHAR;
    unsigned char vUChar = UCHAR_MAX;
    
    int dtype_ushort = DTYPE_USHORT;
    unsigned short vUShort = USHRT_MAX;
    
    int dtype_ulong = DTYPE_ULONG;
    unsigned long vULong = UINT_MAX;
    
    int dtype_ulonglong = DTYPE_ULONGLONG;
    unsigned long vULongLong[2] = { UINT_MAX, UINT_MAX };
    
    int dtype_char = DTYPE_CHAR;
    char vChar = CHAR_MAX;
    
    int dtype_short = DTYPE_SHORT;
    short vShort = SHRT_MAX;
    
    int dtype_long = DTYPE_LONG;
    long vLong = INT_MAX;
    
    int dtype_longlong = DTYPE_LONGLONG;
    long vLongLong[2] = { INT_MAX, INT_MAX };
    
    int dtype_float = DTYPE_FLOAT;
    float vFloat = FLOAT_TEST;
    
    int dtype_double = DTYPE_DOUBLE;
    double vDouble = DOUBLE_TEST;
    
    int dtype_complex = DTYPE_COMPLEX;
    float vComplex[2] = { COMPLEX_TEST0, COMPLEX_TEST1 };
    
    int dtype_complex_double = DTYPE_COMPLEX_DOUBLE;
    double vComplexDouble[2] = { COMPLEX_DOUBLE_TEST0, COMPLEX_DOUBLE_TEST1 };
    
    int dtype_cstring = DTYPE_CSTRING;
    char vCstring[] = CSTRING_TEST0;
    
    int null = 0;
    int len = 0;
    //    char *msg = "   ";
    
    BEGIN_TESTING(dtype);
    /**** Run tests ****/
    
    dsc = descr(&dtype_uchar, &vUChar, &null);
    status = MdsValue("$-1", &dsc, &dsc, &null, &len);
    TEST1(status  & 1);
    TEST1(vUChar == UCHAR_MAX - 1);  
    
    dsc = descr(&dtype_ushort, &vUShort, &null);
    status = MdsValue("$-1", &dsc, &dsc, &null, &len);
    TEST1(status  & 1);
    TEST1(vUShort == USHRT_MAX - 1);
    
    dsc = descr(&dtype_ulong, &vULong, &null);
    status = MdsValue("$-1", &dsc, &dsc, &null, &len);
    TEST1(status  & 1);
    TEST1(vULong == UINT_MAX - 1);
    
    dsc = descr(&dtype_ulonglong, vULongLong, &null);
    status = MdsValue("$-1", &dsc, &dsc, &null, &len);
    /* do the test right on big endian machines */
#if defined( _QUAD_HIGHWORD) && defined(_QUAD_LOWWORD)
    TEST1(status  & 1);
    TEST1((vULongLong[_QUAD_LOWWORD] == UINT_MAX - 1) &&
          (vULongLong[_QUAD_HIGHWORD] == UINT_MAX));
#else
    TEST1(status  & 1);
    TEST1((vULongLong[0] == UINT_MAX - 1) && (vULongLong[1] == UINT_MAX));
#endif
    
    dsc = descr(&dtype_char, &vChar, &null);
    status = MdsValue("$-1", &dsc, &dsc, &null, &len);
    TEST1(status  & 1);
    TEST1(vChar == CHAR_MAX - 1);
    
    dsc = descr(&dtype_short, &vShort, &null);
    status = MdsValue("$-1", &dsc, &dsc, &null, &len);
    TEST1(status  & 1);
    TEST1(vShort == SHRT_MAX - 1);
    
    dsc = descr(&dtype_long, &vLong, &null);
    status = MdsValue("$-1", &dsc, &dsc, &null, &len);
    TEST1(status  & 1);
    TEST1(vLong == INT_MAX - 1);
    
    dsc = descr(&dtype_longlong, vLongLong, &null);
    status = MdsValue("$-1", &dsc, &dsc, &null, &len);
#if defined( _QUAD_HIGHWORD) && defined(_QUAD_LOWWORD)
    TEST1(status  & 1);
    TEST1((vLongLong[_QUAD_LOWWORD] == INT_MAX - 1) &&
          (vLongLong[_QUAD_HIGHWORD] == INT_MAX));
#else
    TEST1(status  & 1);
    TEST1((vLongLong[0] == INT_MAX - 1) && (vLongLong[1] == INT_MAX));
#endif
    
    dsc = descr(&dtype_float, &vFloat, &null);
    status = MdsValue("$-1", &dsc, &dsc, &null, &len);
    TEST1(status  & 1);
    TEST1(vFloat - FLOAT_TEST + 1 < 1.e-7);
    
    dsc = descr(&dtype_double, &vDouble, &null);
    status = MdsValue("$-1", &dsc, &dsc, &null, &len);
    TEST1(status  & 1);
    TEST1(vDouble == DOUBLE_TEST - 1);
    
    dsc = descr(&dtype_complex, vComplex, &null);
    status = MdsValue("$-CMPLX(0,1)", &dsc, &dsc, &null, &len);
    TEST1(status  & 1);
    TEST1((vComplex[0] == COMPLEX_TEST0) && (vComplex[1] == COMPLEX_TEST1 - 1));
    
    dsc = descr(&dtype_complex_double, vComplexDouble, &null);
    status = MdsValue("$-CMPLX(0,1)", &dsc, &dsc, &null, &len);
    TEST1(status  & 1);
    TEST1((vComplexDouble[0] == COMPLEX_DOUBLE_TEST0) &&
            (vComplexDouble[1] == COMPLEX_DOUBLE_TEST1 - 1));
    
    len = 14;
    dsc = descr(&dtype_cstring, vCstring, &null, &len);
    status = MdsValue("UPCASE($)", &dsc, &dsc, &null, &len);
    TEST1(status  & 1);
    TEST1(strcmp(vCstring, CSTRING_TEST1) == 0);
        
    END_TESTING;
    return 0;
}
