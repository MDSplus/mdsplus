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
#include <mdsobjects.h>
#include <string>
#include <cstring>

#include "testutils/CommaInit.h"
#include "testutils/vector_test.h"

#include "testing.h"

#include "MdsDataTest.h"


// NOTE: This test is actually not very usefull as it does not stress the
// numeric conversion to its limits, so it should not thows particular
// exceptions. Nevertheless it comes in help to see how the numeric conversion
// is tested and to easily access to possible errors on conversion or leaks.


using namespace MDSplus;
using namespace testing;



////////////////////////////////////////////////////////////////////////////////
//  SIGNED  ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



void test_Int8() {
    signed char value = 123;

    Data * data = new Int8(value);
    MdsDataTest::test_data_numerics(data,value);
    MdsDataTest::test_data_string(data,"123B");
    deleteData(data);
}

void test_Int8Array() {

    std::vector<char> value;
    value << (char)1,2,3,123;

    Data * data = new Int8Array(&value[0],value.size());
    MdsDataTest::test_data_numerics(data,value);
    MdsDataTest::test_data_string(data,"Byte([1,2,3,123])");

    deleteData(data);
}

void test_Int16() {
    signed short value = 123;

    Data * data = new Int16(value);
    MdsDataTest::test_data_numerics(data,value);
    MdsDataTest::test_data_string(data,"123W");
    deleteData(data);
}

void test_Int16Array() {

    std::vector<short> value;
    value << (short)1,2,3,123;

    Data * data = new Int16Array(&value[0],value.size());
    MdsDataTest::test_data_numerics(data,value);
    MdsDataTest::test_data_string(data,"Word([1,2,3,123])");

    deleteData(data);
}

void test_Int32() {
    signed int value = 123;

    Data * data = new Int32(value);
    MdsDataTest::test_data_numerics(data,value);
    MdsDataTest::test_data_string(data,"123");
    deleteData(data);
}

void test_Int32Array() {

    std::vector<int> value;
    value << (int)1,2,3,123;

    Data * data = new Int32Array(&value[0],value.size());
    MdsDataTest::test_data_numerics(data,value);
    MdsDataTest::test_data_string(data,"[1,2,3,123]");

    deleteData(data);
}

void test_Int64() {
    int64_t value = 123;

    Data * data = new Int64(value);
    MdsDataTest::test_data_numerics(data,value);
    MdsDataTest::test_data_string(data,"123Q");
    deleteData(data);
}

void test_Int64Array() {

    std::vector<int64_t> value;
    value << (int64_t)1,2,3,123;

    Data * data = new Int64Array(&value[0],value.size());
    MdsDataTest::test_data_numerics(data,value);
    MdsDataTest::test_data_string(data,"[1Q,2Q,3Q,123Q]");

    deleteData(data);
}


////////////////////////////////////////////////////////////////////////////////
//  UNSIGED  ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void test_Uint8() {
    unsigned char value = 123;

    Data * data = new Uint8(value);
    MdsDataTest::test_data_numerics(data,value);
    MdsDataTest::test_data_string(data,"123BU");
    deleteData(data);
}

void test_Uint8Array() {

    std::vector<unsigned char> value;
    value << (unsigned char)1,2,3,123;

    Data * data = new Uint8Array(&value[0],value.size());
    MdsDataTest::test_data_numerics(data,value);
    MdsDataTest::test_data_string(data,"Byte_Unsigned([1,2,3,123])");

    deleteData(data);
}

void test_Uint16() {
    unsigned short value = 123;

    Data * data = new Uint16(value);
    MdsDataTest::test_data_numerics(data,value);
    MdsDataTest::test_data_string(data,"123WU");
    deleteData(data);
}

void test_Uint16Array() {

    std::vector<unsigned short> value;
    value << (unsigned short)1,2,3,123;

    Data * data = new Uint16Array(&value[0],value.size());
    MdsDataTest::test_data_numerics(data,value);
    MdsDataTest::test_data_string(data,"Word_Unsigned([1,2,3,123])");

    deleteData(data);
}

void test_Uint32() {
    unsigned int value = 123;

    Data * data = new Uint32(value);
    MdsDataTest::test_data_numerics(data,value);
    MdsDataTest::test_data_string(data,"123LU");
    deleteData(data);
}

void test_Uint32Array() {

    std::vector<unsigned int> value;
    value << (unsigned int)1,2,3,123;

    Data * data = new Uint32Array(&value[0],value.size());
    MdsDataTest::test_data_numerics(data,value);
    MdsDataTest::test_data_string(data,"Long_Unsigned([1LU,2LU,3LU,123LU])");

    deleteData(data);
}

void test_Uint64() {
    uint64_t value = 123;

    Data * data = new Uint64(value);
    MdsDataTest::test_data_numerics(data,value);
    MdsDataTest::test_data_string(data,"123QU");
    deleteData(data);
}

void test_Uint64Array() {

    std::vector<uint64_t> value;
    value << (uint64_t)1,2,3,123;

    Data * data = new Uint64Array(&value[0],value.size());
    MdsDataTest::test_data_numerics(data,value);
    MdsDataTest::test_data_string(data,"[1QU,2QU,3QU,123QU]");

    deleteData(data);
}

////////////////////////////////////////////////////////////////////////////////
//  FLOATS  ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void test_Float32() {

    float value = 123.2368;

    Data * data = new Float32(value);
    MdsDataTest::test_data_numerics(data,value);
    MdsDataTest::test_data_string(data,"123.237");

    deleteData(data);
}
void test_Float32Array() {

    std::vector<float> value;
    value << (float)1,2,3,123.2368;

    Data * data = new Float32Array(&value[0],value.size());
    MdsDataTest::test_data_numerics(data,value);
    MdsDataTest::test_data_string(data,"[1.,2.,3.,123.237]");

    deleteData(data);
}
void test_Float64() {

    double value = 123.2368;

    Data * data = new Float64(value);
    MdsDataTest::test_data_numerics(data,value);
    MdsDataTest::test_data_string(data,"123.2368D0");

    deleteData(data);
}
void test_Float64Array() {

    std::vector<double> value;
    value << (double)1,2,3,123.2368;

    Data * data = new Float64Array(&value[0],value.size());
    MdsDataTest::test_data_numerics(data,value);
    MdsDataTest::test_data_string(data,"[1D0,2D0,3D0,123.2368D0]");

    deleteData(data);
}


////////////////////////////////////////////////////////////////////////////////
//  COMPLEX  ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void test_Complex32() {

    std::complex<float> value(1.23, 123.2368);
    Data * data = new Complex32(value.real(),value.imag());
    MdsDataTest::test_data_numerics(data,value);
    MdsDataTest::test_data_string(data,"Cmplx(1.23,123.237)");

    deleteData(data);
}
void test_Complex32Array() {
    typedef std::complex<float> Cf;

    TEST1( sizeof(Cf) == sizeof(float)*2 );
    std::vector<Cf> array;
    array << Cf(1.,2.), Cf(3, 4), Cf(5,6);
    float data_array[6] = {1.,2.,3.,4.,5.,6.};

    Data * data = new Complex32Array(data_array,3);
    MdsDataTest::test_data_numerics(data,array);
    MdsDataTest::test_data_string(data,"[Cmplx(1.,2.),Cmplx(3.,4.),Cmplx(5.,6.)]");

    deleteData(data);
}

void test_Complex64() {

    std::complex<double> value(1.23, 123.2368);
    Data * data = new Complex64(value.real(),value.imag());
    MdsDataTest::test_data_numerics(data,value);
    MdsDataTest::test_data_string(data,"Cmplx(1.23,123.237)");

    deleteData(data);
}
void test_Complex64Array() {
    typedef std::complex<double> Cf;

    TEST1( sizeof(Cf) == sizeof(double)*2 );
    std::vector<Cf> array;
    array << Cf(1.,2.), Cf(3, 4), Cf(5,6);
    double data_array[6] = {1.,2.,3.,4.,5.,6.};

    Data * data = new Complex64Array(data_array,3);
    MdsDataTest::test_data_numerics(data,array);
    MdsDataTest::test_data_string(data,"[Cmplx(1.,2.),Cmplx(3.,4.),Cmplx(5.,6.)]");

    deleteData(data);
}





////////////////////////////////////////////////////////////////////////////////
//  MAIN  //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


int main(int argc UNUSED_ARGUMENT, char *argv[] UNUSED_ARGUMENT)
{
    BEGIN_TESTING(Data);

    test_Int8();
    test_Int8Array();
    test_Int16();
    test_Int16Array();
    test_Int32();
    test_Int32Array();
    test_Int64();
    test_Int64Array();

    test_Uint8();
    test_Uint8Array();
    test_Uint16();
    test_Uint16Array();
    test_Uint32();
    test_Uint32Array();
    test_Uint64();
    test_Uint64Array();

    test_Float32();
    test_Float32Array();
    test_Float64();
    test_Float64Array();

    test_Complex32();
    test_Complex32Array();
    test_Complex32();
    test_Complex32Array();


    END_TESTING;
}
