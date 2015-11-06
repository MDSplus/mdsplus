#pragma once

#include <assert.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#define __THROW
#else
#include <sys/cdefs.h>
#endif

#ifdef _WIN32
#define __STRING(x)	#x
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif


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



#ifdef _WIN32
#  if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#   define __ASSERT_FUNCTION	__func__
#  else
#   define __ASSERT_FUNCTION	((const char *) 0)
#  endif
#endif




#if defined __cplusplus 
extern "C" {
#endif

#ifdef _TESTING

// Assertion fail declaration as found in assert.h //
EXPORT void __assert_fail (const char *__assertion, const char *__file,
                           unsigned int __line, const char *__function)
__THROW __attribute__ ((__noreturn__));

EXPORT void __mark_point(const char *__assertion, const char *__file, 
                         unsigned int __line, const char *__function);

EXPORT void __test_setfork(const int value);
EXPORT void __test_init(const char *test_name, const char *file, const int line);
EXPORT void __test_end();
EXPORT int  __setup_parent();
EXPORT int  __setup_child();
EXPORT void __test_assert_fail(const char *file, int line, const char *expr, ...);
EXPORT void __test_exit() __attribute__ ((__noreturn__));
EXPORT void __test_abort(int code, const char *__msg, const char *__file,
                         unsigned int __line, const char *__function);
EXPORT void __test_timeout(double seconds);
#else // not _TESTING ( normal build )

void __test_setfork(const int value) { (void)value; }
void __test_init(const char *test_name, const char *file, const int line) {}
void __test_end() {}
int  __setup_parent() { return 0; }
int  __setup_child()  { return 0; }
void __test_assert_fail(const char *file, int line, const char *expr, ...) {}
void __test_exit() { exit(0); }
void __test_timeout(double seconds) { (void)seconds; }

void __test_abort(int code, const char *__msg, const char *__file,
                  unsigned int __line, const char *__function) 
{ 
    printf(" TEST: ABORT - "
           "  file: %s ,  function: %s, line: %d "
           "  message:  (%s) \n", 
           __file,__function,__line,__msg);
    exit(code); 
}

void __assert_fail (const char *__assertion, const char *__file,
                    unsigned int __line, const char *__function)
__THROW __attribute__ ((__noreturn__));

void __mark_point(const char *__assertion, const char *__file, 
                  unsigned int __line, const char *__function) 
{
    printf(" TEST: "
           "  file: %s ,  function: %s, line: %d "
           "  assertion:  (%s) \n", 
           __file,__function,__line,__assertion);               
}

#endif // _TESTING

#if defined __cplusplus 
}
#endif





////////////////////////////////////////////////////////////////////////////////
//  TEST  //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define TEST_DEFAULT_TIMEOUT 10 // seconds

#ifdef NDEBUG
#define TEST_FORK(value)      (__ASSERT_VOID_CAST (0))
#define TEST_ASSERT(expr)     (__ASSERT_VOID_CAST (0)) 
#define TEST1(expr)           (__ASSERT_VOID_CAST (0))
#define TEST0(expr)           (__ASSERT_VOID_CAST (0))
#define BEGIN_TESTING(description) {    
#define END_TESTING }
#define SKIP_TEST  __test_abort(77);
#define ABORT_TEST __test_abort(99);
#else

#define TEST_FORK(value)    __test_setfork(value);

#define TEST_ASSERT(expr) \
    ((expr)				  \
    ? __mark_point (__STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION) \
    : __assert_fail (__STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION))

#define TEST1(expr)  \
    ((expr)			 \
    ? __mark_point (__STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION) \
    : __assert_fail (__STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION))

#define TEST0(expr) \
    ((expr)			\
    ? __assert_fail (__STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION) \
    : __mark_point (__STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION)) 

#define BEGIN_TESTING(description) \
    __test_init(__STRING(description),__FILE__,__LINE__); \
    if( __setup_parent() ) { /* do something more after parent */ ;} \
    else { if(__setup_child()) { /* do something more after child */ ;}
    
#define END_TESTING } __test_end();

#define SKIP_TEST  __test_abort(77,"SKIP: ", __FILE__,__LINE__,__ASSERT_FUNCTION);
#define ABORT_TEST __test_abort(99,"ERROR: ",__FILE__,__LINE__,__ASSERT_FUNCTION);

#define TEST_TIMEOUT(seconds) (__test_timeout(seconds))

#endif // NDEBUG


#if defined __cplusplus
#define TEST_STD_EXCEPTION(val, string) try { val; } catch (std::exception &e) { TEST0( strcmp(e.what(), string) ); }
#define TEST_MDS_EXCEPTION(val, string) try { val; } catch (std::exception &e) { TEST0( strcmp(e.what(), string) ); }
#define TEST_EXCEPTION(val, ExceptionClass) { bool correct_exception_caught = false; \
    try { val; } catch (ExceptionClass &e) { correct_exception_caught=true; } catch (...) {} \
    TEST1(correct_exception_caught); }
#endif





