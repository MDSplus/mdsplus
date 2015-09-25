
#include <assert.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
//  ASSERT  ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//# define assert(expr)							\
//    ((expr)								\
//    ? __ASSERT_VOID_CAST (0)						\
//    : __assert_fail (__STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION))


//#define assert(expr)  \
//    ((expr)								\
//     ? __mark_point (__STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION) \
//     : __assert_fail (__STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION))


//#define assert_msg(expr, ...) \
//  (expr) ? \
//     _mark_point(__FILE__, __LINE__) : \
//     __assert_fail(expr, __FILE__, __LINE__, __ASSERT_FUNCTION)


// Assertion fail declaration as found in assert.h //
extern void __assert_fail (const char *__assertion, const char *__file,
                           unsigned int __line, const char *__function)
__THROW __attribute__ ((__noreturn__));



////////////////////////////////////////////////////////////////////////////////
//  TEST  //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



#define TEST_FORK(value) __test_setfork(value);


#define TEST_ASSERT(expr) \
    ((expr)								\
    ? __mark_point (__STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION) \
    : __assert_fail (__STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION))

#define TEST1(expr)  \
    ((expr)								\
    ? __mark_point (__STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION) \
    : __assert_fail (__STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION))

#define TEST0(expr) \
    ((expr)								\
    ? __assert_fail (__STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION) \
    : __mark_point (__STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION)) 
    

#define BEGIN_TESTING(description) \
    __test_init(__STRING(description),__FILE__,__LINE__); \
    if( __setup_parent() ) { /* do something more after parent */ ;} \
    else { if(__setup_child())
    
#define END_TESTING } __test_end();


#if defined __cplusplus


#define TEST_STD_EXCEPTION(val, string) try { val; } catch (std::exception &e) { TEST0( strcmp(e.what(), string) ); }
#define TEST_MDS_EXCEPTION(val, string) try { val; } catch (std::exception &e) { TEST0( strcmp(e.what(), string) ); }
#define TEST_EXCEPTION(val, ExceptionClass) { bool correct_exception_caught = false; \
    try { val; } catch (ExceptionClass &e) { correct_exception_caught=true; } catch (...) {} \
    TEST1(correct_exception_caught); }

#endif


#if defined __cplusplus 
extern "C" {
#endif

extern void __test_setfork(int value);

extern void __test_init(const char *test_name, const char *file, const int line);

extern void __test_end();

extern int  __setup_parent();

extern int  __setup_child();

extern void __test_assert_fail(const char *file, int line, const char *expr, ...);

extern void __mark_point(const char *__assertion, const char *__file, unsigned int __line, const char *__function);

extern void __test_exit() __attribute__ ((__noreturn__));


#if defined __cplusplus 
}
#endif
