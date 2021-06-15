#ifndef MDSDATATEST_H
#define MDSDATATEST_H

#include <mdsobjects.h>
#include <string>

#include "testing.h"
#include "testutils/FunctionTypes.h"
#include "testutils/type_traits.h"

// NOTE:
// This class is intended as a specialized helper to inspect numeric data
// conversion handled by the Data derived numeric types.

// using namespace MDSplus;
namespace mds = MDSplus;

namespace testing
{

  ////////////////////////////////////////////////////////////////////////////////
  //  TEST STRING CONVERSION  ////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////

  inline void print_type_encoded(mds::Data *data)
  {
    char *str = data->getString();
    std::cout << "data in tdi string: " << str << "\n";
    delete[] str;
  }

  inline std::string mdsdata_to_string(mds::Data *data)
  {
    char *str = data->getString();
    std::string out(str);
    delete[] str;
    return out;
  }

  ////////////////////////////////////////////////////////////////////////////////
  //  MDS DATA TEST CLASS  ///////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////

  // we assume vector std allocator to be contiguous //
  // see note: http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#69
  //
  class MdsDataTest
  {

    template <typename _Fn>
    struct ArrayFnptr
    {
      typedef typename detail::FunctionTypes<_Fn>::ReturnType ReturnType;
      typedef typename detail::remove_pointer<ReturnType>::type type;
      typedef typename std::vector<type> VectorType;
    };

    template <typename Fn>
    static typename ArrayFnptr<Fn>::VectorType get_vector(mds::Data *data, Fn fn);

    template <typename Fn, typename T>
    static void test_numeric_cast_function(mds::Data *data, Fn fn, const T value);

  public:
    template <typename T>
    static void test_data_numerics(mds::Data *data, const T value);

    template <typename T>
    static void test_data_numerics(mds::Data *data, const std::complex<T> value);

    template <typename T>
    static void test_data_numerics(mds::Data *data, const std::vector<T> &array);

    template <typename T>
    static void test_data_numerics(mds::Data *data,
                                   const std::vector<std::complex<T> > &array);

    static void test_data_string(mds::Data *data, const char *str)
    {
      char *data_str = data->getString();
      int data_getString_differs_from_test = strcmp(data_str, str);
      TEST0(data_getString_differs_from_test);
      if (data_getString_differs_from_test)
        std::cout << "tdi decompile: " << data_str << "\n"
                  << "test string:   " << str << "\n";
      delete[] data_str;
    }
  };

  ///
  /// Takes a Data object and a getter of Array and returns a vector of the
  /// obtained elements.
  ///
  template <typename Fn>
  inline typename MdsDataTest::ArrayFnptr<Fn>::VectorType
  MdsDataTest::get_vector(mds::Data *data, Fn fn)
  {
    typedef typename ArrayFnptr<Fn>::type R;

    std::vector<R> out;
    int size;
    R *array = (data->*fn)(&size);
    for (size_t i = 0; i < (unsigned int)size; ++i)
      out.push_back(array[i]);
    delete[] array;
    return out;
  }

  ///
  /// If the numeric_cast throws an exception this catch and compare with the
  /// getter method of the Data object. Otherwise the casted values are checked.
  ///
  template <typename Fn, typename T>
  inline void MdsDataTest::test_numeric_cast_function(mds::Data *data, Fn fn,
                                                      const T value)
  {
    typedef typename detail::FunctionTypes<Fn>::ReturnType R;
    try
    {
      mds::numeric_cast<T>(value);
    }
    catch (std::exception &e)
    {
      TEST_STD_EXCEPTION((data->*fn)(), e.what());
    }
    try
    {
      // NOTE: seems that the optimizer makes a fail in 32bit if these are not
      // volatile variables ( failing case is: test_conversion_limits<Int32> )
      volatile R casted_val = mds::numeric_cast<R>(value);
      volatile R data_val = (data->*fn)();
      TEST1(casted_val == data_val);
    }
    catch (...)
    {
    } // do not throw //
  }

#define MDS_GETARRAY_FUNCPT(type, name) \
  ((type * (mds::Data::*)(int *)) & mds::Data::name)

#define MDS_GETNUMERIC_FUNCPT(type, name) \
  ((type(mds::Data::*)()) & mds::Data::name)

  ///
  /// All numeric conversion implemented for MDS numeric types are tested.
  ///
  /// The actual conversion is now handled by the templated
  /// test_numeric_cast_function to account the possible exception throw if the
  /// conversion is forbidden.
  ///
  template <typename T>
  inline void MdsDataTest::test_data_numerics(mds::Data *data, const T value)
  {
    TEST1(data->getSize() == 1);

    //        TEST1( data->getByte() == mds::numeric_cast<char>(value) );
    //        TEST1( data->getShort() == mds::numeric_cast<short>(value) );
    //        TEST1( data->getInt() == mds::numeric_cast<int>(value) );
    //        TEST1( data->getLong() == mds::numeric_cast<int64_t>(value) );
    //        TEST1( data->getByteUnsigned() == mds::numeric_cast<unsigned
    //        char>(value) ); TEST1( data->getShortUnsigned() ==
    //        mds::numeric_cast<unsigned short>(value) ); TEST1(
    //        data->getIntUnsigned() == mds::numeric_cast<unsigned int>(value) );
    //        TEST1( data->getLongUnsigned() == mds::numeric_cast<uint64_t>(value)
    //        ); TEST1( data->getFloat() == mds::numeric_cast<float>(value) );
    //        TEST1( data->getDouble() == mds::numeric_cast<double>(value) );

    test_numeric_cast_function(data, MDS_GETNUMERIC_FUNCPT(char, getByte), value);
    test_numeric_cast_function(data, MDS_GETNUMERIC_FUNCPT(short, getShort),
                               value);
    test_numeric_cast_function(data, MDS_GETNUMERIC_FUNCPT(int, getInt), value);
    test_numeric_cast_function(data, MDS_GETNUMERIC_FUNCPT(int64_t, getLong),
                               value);

    test_numeric_cast_function(
        data, MDS_GETNUMERIC_FUNCPT(unsigned char, getByteUnsigned), value);
    test_numeric_cast_function(
        data, MDS_GETNUMERIC_FUNCPT(unsigned short, getShortUnsigned), value);
    test_numeric_cast_function(
        data, MDS_GETNUMERIC_FUNCPT(unsigned int, getIntUnsigned), value);
    test_numeric_cast_function(
        data, MDS_GETNUMERIC_FUNCPT(uint64_t, getLongUnsigned), value);

    test_numeric_cast_function(data, MDS_GETNUMERIC_FUNCPT(float, getFloat),
                               value);
    test_numeric_cast_function(data, MDS_GETNUMERIC_FUNCPT(double, getDouble),
                               value);

    try
    {
      data->getComplex();
    }
    catch (mds::MdsException &e)
    {
      TEST0(strcmp(e.what(),
                   "getComplex() not supported for non Complex data types"));
    }
  }

  ///
  /// Overload for all complex value conversions .. that in the case of a scalar
  /// value is implemented to NOT throwing an exception but converting to the real
  /// part.
  ///
  template <typename T>
  inline void MdsDataTest::test_data_numerics(mds::Data *data,
                                              const std::complex<T> value)
  {
    TEST1(data->getSize() == 1);

    //    TEST1( data->getByte() == numeric_cast<char>(value.real()) );
    //    TEST1( data->getShort() == numeric_cast<short>(value.real()) );
    //    TEST1( data->getInt() == numeric_cast<int>(value.real()) );
    //    TEST1( data->getLong() == numeric_cast<int64_t>(value.real()) );
    //    TEST1( data->getByteUnsigned() == numeric_cast<unsigned
    //    char>(value.real()) ); TEST1( data->getShortUnsigned() ==
    //    numeric_cast<unsigned short>(value.real()) ); TEST1(
    //    data->getIntUnsigned() == numeric_cast<unsigned int>(value.real()) );
    //    TEST1( data->getLongUnsigned() == numeric_cast<uint64_t>(value.real())
    //    ); TEST1( data->getFloat() == numeric_cast<float>(value.real()) );
    //    TEST1( data->getDouble() == numeric_cast<double>(value.real()) );

    test_numeric_cast_function(data, MDS_GETNUMERIC_FUNCPT(char, getByte),
                               value.real());
    test_numeric_cast_function(data, MDS_GETNUMERIC_FUNCPT(short, getShort),
                               value.real());
    test_numeric_cast_function(data, MDS_GETNUMERIC_FUNCPT(int, getInt),
                               value.real());
    test_numeric_cast_function(data, MDS_GETNUMERIC_FUNCPT(int64_t, getLong),
                               value.real());

    test_numeric_cast_function(
        data, MDS_GETNUMERIC_FUNCPT(unsigned char, getByteUnsigned),
        value.real());
    test_numeric_cast_function(
        data, MDS_GETNUMERIC_FUNCPT(unsigned short, getShortUnsigned),
        value.real());
    test_numeric_cast_function(
        data, MDS_GETNUMERIC_FUNCPT(unsigned int, getIntUnsigned), value.real());
    test_numeric_cast_function(
        data, MDS_GETNUMERIC_FUNCPT(uint64_t, getLongUnsigned), value.real());

    test_numeric_cast_function(data, MDS_GETNUMERIC_FUNCPT(float, getFloat),
                               value.real());
    test_numeric_cast_function(data, MDS_GETNUMERIC_FUNCPT(double, getDouble),
                               value.real());

    std::complex<double> value_d(value.real(), value.imag());
    TEST1(data->getComplex() == value_d);
  }

  ///
  /// Test data numeric conversion for Array data. This function overload treats
  /// all data numeric types but the complex that should produce an exception.
  /// The first function tests the size of the produced array, than all the
  /// vector conversions and the array conversions.
  ///
  template <typename T>
  inline void MdsDataTest::test_data_numerics(mds::Data *data,
                                              const std::vector<T> &array)
  {
    TEST1((unsigned int)data->getSize() == array.size());

    TEST1(data->getByteArray() == array);
    TEST1(data->getShortArray() == array);
    TEST1(data->getIntArray() == array);
    TEST1(data->getLongArray() == array);

    TEST1(data->getByteUnsignedArray() == array);
    TEST1(data->getShortUnsignedArray() == array);
    TEST1(data->getIntUnsignedArray() == array);
    TEST1(data->getLongUnsignedArray() == array);

    TEST1(data->getFloatArray() == array);
    TEST1(data->getDoubleArray() == array);

    TEST1(MdsDataTest::get_vector(
              data, MDS_GETARRAY_FUNCPT(char, getByteArray)) == array);
    TEST1(MdsDataTest::get_vector(
              data, MDS_GETARRAY_FUNCPT(short, getShortArray)) == array);
    TEST1(MdsDataTest::get_vector(data, MDS_GETARRAY_FUNCPT(int, getIntArray)) ==
          array);
    TEST1(MdsDataTest::get_vector(
              data, MDS_GETARRAY_FUNCPT(int64_t, getLongArray)) == array);

    TEST1(MdsDataTest::get_vector(
              data, MDS_GETARRAY_FUNCPT(unsigned char, getByteUnsignedArray)) ==
          array);
    TEST1(MdsDataTest::get_vector(
              data, MDS_GETARRAY_FUNCPT(unsigned short, getShortUnsignedArray)) ==
          array);
    TEST1(MdsDataTest::get_vector(
              data, MDS_GETARRAY_FUNCPT(unsigned int, getIntUnsignedArray)) ==
          array);
    TEST1(MdsDataTest::get_vector(
              data, MDS_GETARRAY_FUNCPT(uint64_t, getLongUnsignedArray)) ==
          array);

    TEST1(MdsDataTest::get_vector(
              data, MDS_GETARRAY_FUNCPT(float, getFloatArray)) == array);
    TEST1(MdsDataTest::get_vector(
              data, MDS_GETARRAY_FUNCPT(double, getDoubleArray)) == array);

    // see complex specialization below //
    try
    {
      data->getComplexArray();
    }
    catch (mds::MdsException &e)
    {
      TEST0(strcmp(e.what(),
                   "getComplexArray() not supported for non Complex data types"));
    }
  }

  ///
  /// All complex array are hadled by this overload that should produce an
  /// exception when a conversionn to standard numeric type are performed.
  ///
  template <typename T>
  inline void
  MdsDataTest::test_data_numerics(mds::Data *data,
                                  const std::vector<std::complex<T> > &array)
  {
    TEST1((unsigned int)data->getSize() == array.size());

    TEST_MDS_EXCEPTION(data->getByteArray(),
                       "getByteArray() not supported for Complex data type");
    TEST_MDS_EXCEPTION(data->getShortArray(),
                       "getShortArray() not supported for Complex data type");
    TEST_MDS_EXCEPTION(data->getIntArray(),
                       "getIntArray() not supported for Complex data type");
    TEST_MDS_EXCEPTION(data->getLongArray(),
                       "getLongArray() not supported for Complex data type");

    TEST_MDS_EXCEPTION(
        data->getByteUnsignedArray(),
        "getByteUnsignedArray() not supported for Complex data type");
    TEST_MDS_EXCEPTION(
        data->getShortUnsignedArray(),
        "getShortUnsignedArray() not supported for Complex data type");
    TEST_MDS_EXCEPTION(
        data->getIntUnsignedArray(),
        "getIntUnsignedArray() not supported for Complex data type");
    TEST_MDS_EXCEPTION(
        data->getLongUnsignedArray(),
        "getLongUnsignedArray() not supported for Complex data type");

    TEST_MDS_EXCEPTION(data->getFloatArray(),
                       "getFloatArray() not supported for Complex data type");
    TEST_MDS_EXCEPTION(data->getDoubleArray(),
                       "getDoubleArray() not supported for Complex data type");

    // cast T to double as all complex in Mds are double //
    std::vector<std::complex<double> > array_d;
    for (size_t i = 0; i < array.size(); ++i)
    {
      const std::complex<T> &el = array[i];
      array_d.push_back(std::complex<double>(el.real(), el.imag()));
    }
    TEST1(data->getComplexArray() == array_d);
    TEST1(MdsDataTest::get_vector(
              data, MDS_GETARRAY_FUNCPT(std::complex<double>, getComplexArray)) ==
          array_d);
  }

#undef MDS_GETARRAY_FUNCPT
#undef MDS_GETNUMERIC_FUNCPT

} // namespace testing

#endif // MDSDATATEST_H
