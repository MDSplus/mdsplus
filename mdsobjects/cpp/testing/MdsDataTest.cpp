#include <mdsobjects.h>
#include <string>
#include <cstring>


#include "testing.h"


using namespace MDSplus;
using namespace testing;


//virtual int * getShape(int *numDim);

//virtual Data *getDimensionAt(int dimIdx);
//virtual int getSize() {return 1;}




template < typename T >
void test_data_numerics(Data *data, const T value) {
    TEST1( data->getByte() == static_cast<char>(value) );
    TEST1( data->getShort() == static_cast<short>(value) );
    TEST1( data->getInt() == static_cast<int>(value) );
    TEST1( data->getLong() == static_cast<int64_t>(value) );

    TEST1( data->getByteUnsigned() == static_cast<unsigned char>(value) );
    TEST1( data->getShortUnsigned() == static_cast<unsigned short>(value) );
    TEST1( data->getIntUnsigned() == static_cast<unsigned int>(value) );
    TEST1( data->getLongUnsigned() == static_cast<uint64_t>(value) );

    TEST1( data->getFloat() == static_cast<float>(value) );
    TEST1( data->getDouble() == static_cast<double>(value) );

    try { data->getComplex(); } catch (MdsException &e) {
        TEST0( strcmp(e.what(),"getComplex() not supported for non Compelx data types") );
    }
}


template < typename T >
void test_data_numerics(Data *data, const std::complex<T> value) {
    TEST1( data->getByte() == static_cast<char>(value.real()) );
    TEST1( data->getShort() == static_cast<short>(value.real()) );
    TEST1( data->getInt() == static_cast<int>(value.real()) );
    TEST1( data->getLong() == static_cast<int64_t>(value.real()) );

    TEST1( data->getByteUnsigned() == static_cast<unsigned char>(value.real()) );
    TEST1( data->getShortUnsigned() == static_cast<unsigned short>(value.real()) );
    TEST1( data->getIntUnsigned() == static_cast<unsigned int>(value.real()) );
    TEST1( data->getLongUnsigned() == static_cast<uint64_t>(value.real()) );

    TEST1( data->getFloat() == static_cast<float>(value.real()) );
    TEST1( data->getDouble() == static_cast<double>(value.real()) );

    std::complex<double> value_d(value.real(),value.imag());
    TEST1( data->getComplex() == value_d );
}



// note: http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#69
template <typename T >
void test_data_numerics(Data *data, const std::vector<T> &array) {
    TEST1( data->getByteArray() == array );
    TEST1( data->getShortArray() == array );
    TEST1( data->getIntArray() == array );
    TEST1( data->getLongArray() == array );

    TEST1( data->getByteUnsignedArray() == array );
    TEST1( data->getShortUnsignedArray() == array );
    TEST1( data->getIntUnsignedArray() == array );
    TEST1( data->getLongUnsignedArray() == array );

    TEST1( data->getFloatArray() == array );
    TEST1( data->getDoubleArray() == array );

    // test //
    try { data->getComplexArray(); } catch (MdsException &e) {
        TEST0( strcmp(e.what(),"getComplexArray() not supported for non Complex data types") );
    }
}


template <typename T >
void test_data_numerics(Data *data, const std::vector<std::complex<T> > &array) {

    // TODO:
    TEST_MDS_EXCEPTION( data->getByteArray(), "getByteArray() not supported for Complex data type" );
    TEST_MDS_EXCEPTION( data->getShortArray(), "getShortArray() not supported for Complex data type" );
    TEST_MDS_EXCEPTION( data->getIntArray(), "getIntArray() not supported for Complex data type" );
    TEST_MDS_EXCEPTION( data->getLongArray(), "getLongArray() not supported for Complex data type" );

    TEST_MDS_EXCEPTION( data->getByteUnsignedArray(), "getByteUnsignedArray() not supported for Complex data type" );
    TEST_MDS_EXCEPTION( data->getShortUnsignedArray(), "getShortUnsignedArray() not supported for Complex data type" );
    TEST_MDS_EXCEPTION( data->getIntUnsignedArray(), "getIntUnsignedArray() not supported for Complex data type" );
    TEST_MDS_EXCEPTION( data->getLongUnsignedArray(), "getLongUnsignedArray() not supported for Complex data type" );

    TEST_MDS_EXCEPTION( data->getFloatArray(), "getFloatArray() not supported for Complex data type" );
    TEST_MDS_EXCEPTION( data->getDoubleArray(), "getDoubleArray() not supported for Complex data type" );


    // cast T to double //
    std::vector<std::complex<double> > array_d;
    for(size_t i=0; i<array.size(); ++i) {
        const std::complex<T> &el = array[i];
        array_d.push_back( std::complex<double>(el.real(),el.imag()) );
    }
    TEST1( data->getComplexArray() == array_d );
}


void print_type_encoded(Data *data) {
    char * str = data->getString();
    std::cout << "data in tdi string: " << str << "\n";
    delete [] str;
}

void test_data_string(Data *data, const char *str) {
    char * data_str = data->getString();
    int data_getString_differs_from_test = strcmp(data_str,str);
    TEST0( data_getString_differs_from_test );
    if(data_getString_differs_from_test) std::cout << "tdi decompile: " << data_str << "\n"
                                        << "test string:   " << str << "\n";
    delete[] data_str;
}




////////////////////////////////////////////////////////////////////////////////
//  SIGNED  ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



void test_Int8() {
    signed char value = 123;

    Data * data = new Int8(value);    
    test_data_numerics(data,value);
    test_data_string(data,"123B");
    deleteData(data);
}

void test_Int8Array() {

    std::vector<char> value;
    value << (char)1,2,3,123;

    Data * data = new Int8Array(&value[0],value.size());           
    test_data_numerics(data,value);
    test_data_string(data,"Byte([1,2,3,123])");

    deleteData(data);
}

void test_Int16() {
    signed short value = 123;

    Data * data = new Int16(value);
    test_data_numerics(data,value);
    test_data_string(data,"123W");
    deleteData(data);
}

void test_Int16Array() {

    std::vector<short> value;
    value << (short)1,2,3,123;

    Data * data = new Int16Array(&value[0],value.size());
    test_data_numerics(data,value);
    test_data_string(data,"Word([1,2,3,123])");

    deleteData(data);
}

void test_Int32() {
    signed int value = 123;

    Data * data = new Int32(value);
    test_data_numerics(data,value);
    test_data_string(data,"123");
    deleteData(data);
}

void test_Int32Array() {

    std::vector<int> value;
    value << (int)1,2,3,123;

    Data * data = new Int32Array(&value[0],value.size());

    std::vector<int> v2 = data->getIntArray();

    test_data_numerics(data,value);
    test_data_string(data,"[1,2,3,123]");

    deleteData(data);
}

void test_Int64() {
    int64_t value = 123;

    Data * data = new Int64(value);
    test_data_numerics(data,value);
    test_data_string(data,"0X7bQ");
    deleteData(data);
}

void test_Int64Array() {

    std::vector<int64_t> value;
    value << (int64_t)1,2,3,123;

    Data * data = new Int64Array(&value[0],value.size());
    test_data_numerics(data,value);
    test_data_string(data,"[0X1Q,0X2Q,0X3Q,0X7bQ]");

    deleteData(data);
}


////////////////////////////////////////////////////////////////////////////////
//  UNSIGED  ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void test_Uint8() {
    unsigned char value = 123;

    Data * data = new Uint8(value);
    test_data_numerics(data,value);
    test_data_string(data,"123BU");
    deleteData(data);
}

void test_Uint8Array() {

    std::vector<unsigned char> value;
    value << (unsigned char)1,2,3,123;

    Data * data = new Uint8Array(&value[0],value.size());
    test_data_numerics(data,value);
    test_data_string(data,"Byte_Unsigned([1,2,3,123])");

    deleteData(data);
}

void test_Uint16() {
    unsigned short value = 123;

    Data * data = new Uint16(value);
    test_data_numerics(data,value);
    test_data_string(data,"123WU");
    deleteData(data);
}

void test_Uint16Array() {

    std::vector<unsigned short> value;
    value << (unsigned short)1,2,3,123;

    Data * data = new Uint16Array(&value[0],value.size());
    test_data_numerics(data,value);
    test_data_string(data,"Word_Unsigned([1,2,3,123])");

    deleteData(data);
}

void test_Uint32() {
    unsigned int value = 123;

    Data * data = new Uint32(value);
    test_data_numerics(data,value);
    test_data_string(data,"123LU");
    deleteData(data);
}

void test_Uint32Array() {

    std::vector<unsigned int> value;
    value << (unsigned int)1,2,3,123;

    Data * data = new Uint32Array(&value[0],value.size());
    test_data_numerics(data,value);
    test_data_string(data,"Long_Unsigned([1LU,2LU,3LU,123LU])");

    deleteData(data);
}

void test_Uint64() {
    uint64_t value = 123;

    Data * data = new Uint64(value);
    test_data_numerics(data,value);
    test_data_string(data,"0X7bQU");
    deleteData(data);
}

void test_Uint64Array() {

    std::vector<uint64_t> value;
    value << (uint64_t)1,2,3,123;

    Data * data = new Uint64Array(&value[0],value.size());
    test_data_numerics(data,value);
    test_data_string(data,"[0X1QU,0X2QU,0X3QU,0X7bQU]");

    deleteData(data);
}

////////////////////////////////////////////////////////////////////////////////
//  FLOATS  ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


bool test_Float32() {

    float value = 123.2368;

    Data * data = new Float32(value);
    test_data_numerics(data,value);
    test_data_string(data,"123.237");

    deleteData(data);
}
void test_Float32Array() {

    std::vector<float> value;
    value << (float)1,2,3,123.2368;

    Data * data = new Float32Array(&value[0],value.size());
    test_data_numerics(data,value);
    test_data_string(data,"[1.,2.,3.,123.237]");

    deleteData(data);
}

bool test_Float64() {

    double value = 123.2368;

    Data * data = new Float64(value);
    test_data_numerics(data,value);
    test_data_string(data,"123.2368D0");

    deleteData(data);
}
void test_Float64Array() {

    std::vector<double> value;
    value << (double)1,2,3,123.2368;

    Data * data = new Float64Array(&value[0],value.size());
    test_data_numerics(data,value);
    test_data_string(data,"[1D0,2D0,3D0,123.2368D0]");

    deleteData(data);
}


////////////////////////////////////////////////////////////////////////////////
//  COMPLEX  ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void test_Complex32() {

    std::complex<float> value(1.23, 123.2368);
    Data * data = new Complex32(value.real(),value.imag());
    test_data_numerics(data,value);
    test_data_string(data,"Cmplx(1.23,123.237)");

    deleteData(data);
}
void test_Complex32Array() {
    typedef std::complex<float> Cf;

    TEST1( sizeof(Cf) == sizeof(float)*2 );
    std::vector<Cf> array;
    array << Cf(1.,2.), Cf(3, 4), Cf(5,6);

    Data * data = new Complex32Array(&array.at(0).real(),3);
    test_data_numerics(data,array);
    test_data_string(data,"[Cmplx(1.,2.),Cmplx(3.,4.),Cmplx(5.,6.)]");

    deleteData(data);
}

void test_Complex64() {

    std::complex<double> value(1.23, 123.2368);
    Data * data = new Complex64(value.real(),value.imag());
    test_data_numerics(data,value);
    test_data_string(data,"Cmplx(1.23,123.237)");

    deleteData(data);
}
void test_Complex64Array() {
    typedef std::complex<double> Cf;

    TEST1( sizeof(Cf) == sizeof(double)*2 );
    std::vector<Cf> array;
    array << Cf(1.,2.), Cf(3, 4), Cf(5,6);

    Data * data = new Complex64Array(&array.at(0).real(),3);
    test_data_numerics(data,array);
    test_data_string(data,"[Cmplx(1.,2.),Cmplx(3.,4.),Cmplx(5.,6.)]");

    deleteData(data);
}




int main(int argc, char *argv[])
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

    std::cout << "\n\n";
    if(Singleton<TestResults>::get_instance().fails() == 0)
    { std::cout << " SUCCESS !! \n"; }
    else { std::cout << " SOME FAILS OCCURRED !! \n"; }

    END_TESTING;
}
