
#include <string>
#include <cstring>
#include <cmath>
#include <climits>

#include <mdsobjects.h>

#include "testing.h"
#include "testutils/testutils.h"
#include "testutils/Singleton.h"

#include <mdsplus/numeric_cast.hpp>

// NOTE: This is a special test for the numeric_cast function that was added to
// cope with the possible conversion exceptions. The numeric_cast_test function
// is not finished yet and aims to automatize the test


using namespace  MDSplus;
using namespace  testing;

// INTEGER //
template < typename T >
typename enable_if< numeric_limits<T>::is_integer, void >::type
numeric_cast_test() {       
    unsigned long long int max = numeric_limits<T>::highest();
    long long int min = numeric_limits<T>::lowest();    
    
    long double max_f = numeric_limits<T>::highest();
    long double min_f = numeric_limits<T>::lowest();

    TEST1( numeric_cast<T>(0) == 0 );
    TEST1( numeric_cast<T>(1) == 1 );
    TEST1( numeric_cast<T>(max) == numeric_limits<T>::highest() );
    TEST1( numeric_cast<T>(min) == numeric_limits<T>::lowest() );

    TEST_EXCEPTION( numeric_cast<T>(max+1), std::overflow_error );
    TEST_EXCEPTION( numeric_cast<T>(min-1), std::underflow_error );

    if( numeric_limits<T>::digits < numeric_limits<long double>::digits ) {
        // IS A COERCION so we can check next of max_f //
        TEST_EXCEPTION( numeric_cast<T>(max_f+1), std::overflow_error );
        TEST_EXCEPTION( numeric_cast<T>(min_f-1), std::underflow_error );
    }
}

// FLOAT //
template < typename T >
typename enable_if< !numeric_limits<T>::is_integer, void >::type
numeric_cast_test() {

}


/// this may trigger an exception where overflow should be thrown by numeric_cast
int main(int argc, char *argv[])
{
    BEGIN_TESTING(ScalarCast);

    // TEST IF ISNAN WORKS //
    TEST0(std::isnan(0.0));
    TEST0(std::isnan(1.0/0.0));
    TEST0(std::isnan(-1.0/0.0));
    TEST1(std::isnan(sqrt(-1.0)));

    // TEST IF ISINF IS PRESENT //
    //    TEST1(std::isinf(nume ))

    
    TEST1( numeric_cast<char>(0) == 0 );
    TEST1( numeric_cast<char>(1) == 1 );
    TEST1( numeric_cast<char>(CHAR_MAX) == CHAR_MAX );
    TEST1( numeric_cast<char>(CHAR_MIN) == CHAR_MIN );
    TEST_EXCEPTION( numeric_cast<char>(CHAR_MAX+1), std::overflow_error );
    TEST_EXCEPTION( numeric_cast<char>(CHAR_MIN-1), std::underflow_error );

    TEST1( numeric_cast<unsigned char>(0) == 0 );
    TEST1( numeric_cast<unsigned char>(1) == 1 );
    TEST1( numeric_cast<unsigned char>(UCHAR_MAX) == UCHAR_MAX );
    TEST_EXCEPTION( numeric_cast<unsigned char>(UCHAR_MAX + 1), std::overflow_error );
    TEST_EXCEPTION( numeric_cast<unsigned char>(-1), std::underflow_error );

    TEST1( numeric_cast<int>(0) == 0 );
    TEST1( numeric_cast<int>(1) == 1 );
    TEST1( numeric_cast<int>(INT_MAX) == INT_MAX );
    TEST1( numeric_cast<int>(INT_MIN) == INT_MIN );
    TEST_EXCEPTION( numeric_cast<int>((int64_t)INT_MAX+1), std::overflow_error );
    TEST_EXCEPTION( numeric_cast<int>((int64_t)INT_MIN-1), std::underflow_error );

    TEST1( numeric_cast<unsigned int>(0) == 0 );
    TEST1( numeric_cast<unsigned int>(1) == 1 );
    TEST1( numeric_cast<unsigned int>(UINT_MAX) == UINT_MAX );
    TEST_EXCEPTION( numeric_cast<unsigned int>((int64_t)UINT_MAX + 1), std::overflow_error );
    TEST_EXCEPTION( numeric_cast<unsigned int>(-1), std::underflow_error );


    TEST1( numeric_cast<long>(0) == 0 );
    TEST1( numeric_cast<long>(1) == 1 );
    TEST1( numeric_cast<long>(LONG_MAX) == LONG_MAX );
    TEST1( numeric_cast<long>(LONG_MIN) == LONG_MIN );
    TEST_EXCEPTION( numeric_cast<long>(numeric_limits<float>::highest()), std::overflow_error );
    TEST_EXCEPTION( numeric_cast<long>(numeric_limits<float>::lowest()), std::underflow_error );

    TEST_EXCEPTION( numeric_cast<float>(numeric_limits<double>::highest()), std::overflow_error );
    TEST_EXCEPTION( numeric_cast<float>(numeric_limits<double>::lowest()), std::underflow_error );

    
    
    // TODO: this is experimental //////////////////////////////////////////////
    //
    numeric_cast_test<bool>();
    numeric_cast_test<char>();
    numeric_cast_test<unsigned char>();
    numeric_cast_test<int>();
    numeric_cast_test<unsigned int>();
    //
    // /////////////////////////////////////////////////////////////////////////


    END_TESTING;
}

