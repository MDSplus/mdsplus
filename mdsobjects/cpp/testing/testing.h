
#ifndef MDS_UNITTEST_TESTING_H
#define MDS_UNITTEST_TESTING_H


#include "testutils/testutils.h"
#include "testutils/Singleton.h"

namespace testing {

template < typename T >
bool areSame(T a, T b) {
    return std::fabs(a - b) < std::numeric_limits<T>::epsilon();
}


class TestResults {
public:
    TestResults() : m_fails(0) {}
    int & fails() { return m_fails; }
private:
    int m_fails;
};


} // testing



#define BEGIN_TESTING(name)                \
printf("..:: Testing " #name " ::..\n");

#define END_TESTING return testing::Singleton<testing::TestResults>::get_instance().fails();

#define PRINT_TEST(val) printf("testing: (" #val ") = %i\n",(val))

#define PRINT_TEST0(fail,val) if(fail) printf("error in test: (" #val ") \n")
#define PRINT_TEST1(fail,val) if(fail) printf("error in test: (" #val ") \n")

#define TEST1(val) {int _fail = (val)==0; PRINT_TEST1(_fail,val); testing::Singleton<testing::TestResults>::get_instance().fails() += _fail;}
#define TEST0(val) {int _fail = (val)!=0; PRINT_TEST0(_fail,val); testing::Singleton<testing::TestResults>::get_instance().fails() += _fail;}

#define TEST_STD_EXCEPTION(val, string) try { val; } catch (std::exception &e) { TEST0( strcmp(e.what(), string) ); }
#define TEST_MDS_EXCEPTION(val, string) try { val; } catch (std::exception &e) { TEST0( strcmp(e.what(), string) ); }
#define TEST_EXCEPTION(val, ExceptionClass) { bool correct_exception_caught = false; \
    try { val; } catch (ExceptionClass &e) { correct_exception_caught=true; } catch (...) {} \
    TEST1(correct_exception_caught); }

#endif // MDS_UNITTEST_TESTING_H
