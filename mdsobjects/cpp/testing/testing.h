

#include <cmath>
#include <limits>
#include <stdio.h>

//#include "boost/preprocessor/stringize.hpp"
#define PP_STRINGIZE_I(text) #text

#define BEGIN_TESTING(name)                \
static int _fail = 0;                      \
printf("..:: Testing " #name " ::..\n");

#define END_TESTING return _fail;

#define TEST1(val) _fail += (val)==0
#define TEST0(val) _fail += (val)!=0

#define PRINT_TEST(val) printf("testing: (" #val ") = %i\n",(val))
#define TEST1_P(val) PRINT_TEST(val); _fail += (val)==0
#define TEST0_P(val) PRINT_TEST(val); _fail += (val)!=0


namespace testing {

template < typename T >
bool areSame(T a, T b) {
    return std::fabs(a - b) < std::numeric_limits<T>::epsilon();
}

} // testing

// using namespace testing;
