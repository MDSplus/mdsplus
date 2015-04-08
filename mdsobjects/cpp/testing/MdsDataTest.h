#ifndef MDSDATATEST_H
#define MDSDATATEST_H

#include <mdsobjects.h>
#include <string>

#include "testing.h"

using namespace MDSplus;

namespace testing {


////////////////////////////////////////////////////////////////////////////////
//  TEST STRING CONVERSION  ////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


inline void print_type_encoded(Data *data) {
    char * str = data->getString();
    std::cout << "data in tdi string: " << str << "\n";
    delete [] str;
}

inline std::string mdsdata_to_string(Data *data) {
    char * str = data->getString();
    std::string out(str);
    delete [] str;
    return out;
}




////////////////////////////////////////////////////////////////////////////////
//  NUMERIC CONVERSION  ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// we assume vector std allocator to be contiguous //
// see note: http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#69

#define MDS_ARRAY_FUNCPT(type,name) \
    ((type *(Data::*)(int*))&Data::name)


class MdsDataTest {
public:

    template < typename _Fn >
    struct ArrayFnptr {
        typedef typename detail::FunctionTypes<_Fn>::ReturnType ReturnType;
        typedef typename detail::remove_pointer<ReturnType>::type type;
        typedef typename std::vector<type> VectorType;
    };

    template < typename _Fn >
    static typename ArrayFnptr<_Fn>::VectorType
    get_vector(Data *data, _Fn fn) {
        typedef typename ArrayFnptr<_Fn>::type R;

        std::vector<R> out;
        int size;
        R * array = (data->*fn)(&size);
        for(size_t i = 0; i<size; ++i)
            out.push_back(array[i]);
        delete [] array;
        return out;
    }

    template < typename T >
    static void test_data_numerics(Data *data, const T value);

    template < typename T >
    static void test_data_numerics(Data *data, const std::complex<T> value);

    template <typename T >
    static void test_data_numerics(Data *data, const std::vector<T> &array);

    template <typename T >
    static void test_data_numerics(Data *data, const std::vector<std::complex<T> > &array);

    static void test_data_string(Data *data, const char *str) {
        char * data_str = data->getString();
        int data_getString_differs_from_test = strcmp(data_str,str);
        TEST0( data_getString_differs_from_test );
        if(data_getString_differs_from_test) std::cout << "tdi decompile: " << data_str << "\n"
                                            << "test string:   " << str << "\n";
        delete[] data_str;
    }    

};


template < typename T >
inline void MdsDataTest::test_data_numerics(Data *data, const T value) {
    TEST1( data->getSize() == 1 );
//    std::cout << "data->getByte() = " << (int)data->getByte() << " -vs- " << "static_char<char>(vlaue) = " << (int)static_cast<char>(value) << "\n";

    TEST1( data->getByte() == numeric_cast<char>(value) );
    TEST1( data->getShort() == numeric_cast<short>(value) );
    TEST1( data->getInt() == numeric_cast<int>(value) );
    TEST1( data->getLong() == numeric_cast<int64_t>(value) );

    TEST1( data->getByteUnsigned() == numeric_cast<unsigned char>(value) );
    TEST1( data->getShortUnsigned() == numeric_cast<unsigned short>(value) );
    TEST1( data->getIntUnsigned() == numeric_cast<unsigned int>(value) );
    TEST1( data->getLongUnsigned() == numeric_cast<uint64_t>(value) );

    TEST1( data->getFloat() == numeric_cast<float>(value) );
    TEST1( data->getDouble() == numeric_cast<double>(value) );

    try { data->getComplex(); } catch (MdsException &e) {
        TEST0( strcmp(e.what(),"getComplex() not supported for non Complex data types") );
    }
}

template < typename T >
inline void MdsDataTest::test_data_numerics(Data *data, const std::complex<T> value) {
    TEST1( data->getSize() == 1 );

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


template <typename T >
inline void MdsDataTest::test_data_numerics(Data *data, const std::vector<T> &array) {
    TEST1( data->getSize() == array.size() );

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

    TEST1( MdsDataTest::get_vector(data, MDS_ARRAY_FUNCPT(char,getByteArray)) == array );
    TEST1( MdsDataTest::get_vector(data, MDS_ARRAY_FUNCPT(short,getShortArray)) == array );
    TEST1( MdsDataTest::get_vector(data, MDS_ARRAY_FUNCPT(int,getIntArray)) == array );
    TEST1( MdsDataTest::get_vector(data, MDS_ARRAY_FUNCPT(int64_t,getLongArray)) == array );

    TEST1( MdsDataTest::get_vector(data, MDS_ARRAY_FUNCPT(unsigned char,getByteUnsignedArray)) == array );
    TEST1( MdsDataTest::get_vector(data, MDS_ARRAY_FUNCPT(unsigned short,getShortUnsignedArray)) == array );
    TEST1( MdsDataTest::get_vector(data, MDS_ARRAY_FUNCPT(unsigned int,getIntUnsignedArray)) == array );
    TEST1( MdsDataTest::get_vector(data, MDS_ARRAY_FUNCPT(uint64_t,getLongUnsignedArray)) == array );

    TEST1( MdsDataTest::get_vector(data, MDS_ARRAY_FUNCPT(float,getFloatArray)) == array );
    TEST1( MdsDataTest::get_vector(data, MDS_ARRAY_FUNCPT(double,getDoubleArray)) == array );

    // see complex specialization below //
    try { data->getComplexArray(); } catch (MdsException &e) {
        TEST0( strcmp(e.what(),"getComplexArray() not supported for non Complex data types") );
    }
}

template <typename T >
inline void MdsDataTest::test_data_numerics(Data *data, const std::vector<std::complex<T> > &array) {
    TEST1 ( data->getSize() == array.size() );

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

    // cast T to double as all complex in Mds are double //
    std::vector<std::complex<double> > array_d;
    for(size_t i=0; i<array.size(); ++i) {
        const std::complex<T> &el = array[i];
        array_d.push_back( std::complex<double>(el.real(),el.imag()) );
    }
    TEST1( data->getComplexArray() == array_d );
    TEST1( MdsDataTest::get_vector(data, MDS_ARRAY_FUNCPT(std::complex<double>,getComplexArray)) == array_d );
}




} // testing


#undef MDS_ARRAY_FUNCPT


#endif // MDSDATATEST_H

