#ifndef _TESTING_H
#define _TESTING_H

#include <assert.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>

#include "mdsconfig.h"

#ifdef _WIN32
#define __THROW
#define __STRING(x) #x
#else
#include <sys/cdefs.h>
#ifndef __THROW
#define __THROW
#endif
#endif

#ifndef EXPORT
#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#elif HAVE_VISIBILITY
#define EXPORT __attribute__((__visibility__("default")))
#else
#define EXPORT
#endif
#endif

#ifndef ASSERT_LINE_TYPE
#define ASSERT_LINE_TYPE unsigned int
#endif

////////////////////////////////////////////////////////////////////////////////
//  ASSERT  ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
//# define assert(expr)							    ((expr)								    ? __ASSERT_VOID_CAST (0)						    : __assert_fail (__STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION))


//#define assert(expr)      ((expr)								     ? __mark_point (__STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION)      : __assert_fail (__STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION))


//#define assert_msg(expr, ...)   (expr) ?      _mark_point(__FILE__, __LINE__) :      __assert_fail(expr, __FILE__, __LINE__, __ASSERT_FUNCTION)
*/

#ifndef __ASSERT_FUNCTION
#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#define __ASSERT_FUNCTION __func__
#else
#define __ASSERT_FUNCTION ((const char *)0)
#endif
#endif

#if defined __cplusplus
// Assertion fail declaration as found in assert.h //
extern "C"
{
#endif

  EXPORT void __assert_fail(const char *__assertion, const char *__file,
                            ASSERT_LINE_TYPE __line,
                            const char *__function) __THROW
      __attribute__((__noreturn__));

#ifdef _TESTING

  EXPORT void __mark_point(const char *__assertion, const char *__file,
                           ASSERT_LINE_TYPE __line, const char *__function);

  EXPORT void __test_setfork(const int value);
  EXPORT void __test_init(const char *test_name, const char *file,
                          const int line);
  EXPORT void __test_end();
  EXPORT int __setup_parent();
  EXPORT int __setup_child();
  EXPORT void __test_assert_fail(const char *file, int line, const char *expr,
                                 ...);
  EXPORT void __test_exit() __attribute__((__noreturn__));
  EXPORT void __test_abort(int code, const char *__msg, const char *__file,
                           unsigned int __line, const char *__function);
  EXPORT void __test_timeout(double seconds);
#else // not _TESTING ( normal build )

void __test_setfork(const int value)
{
  (void)value;
}
int __setup_parent() { return 0; }
int __setup_child() { return 0; }
void __test_exit() { exit(0); }
void __test_timeout(double seconds) { (void)seconds; }
void __test_init(const char *test_name __attribute__((unused)),
                 const char *file __attribute__((unused)),
                 const int line __attribute__((unused))) {}
void __test_end() { atexit(__test_exit); }

void __test_abort(int code, const char *__msg, const char *__file,
                  unsigned int __line, const char *__function)
{
  printf(" TEST: ABORT - "
         "  file: %s ,  function: %s, line: %d "
         "  message:  (%s) \n",
         __file, __function, __line, __msg);
  fflush(stdout);
  _Exit(code);
}

void __test_assert_fail(const char *file, int line, const char *expr, ...)
{
  printf(" TEST: FAIL"
         "  file: %s ,  line: %d "
         "  assertion:  (%s) \n",
         file, line, expr);
  fflush(stdout);
  abort();
}

void __mark_point(const char *__assertion, const char *__file,
                  unsigned int __line, const char *__function)
{
  printf(" TEST: OK"
         "  file: %s ,  function: %s, line: %d "
         "  assertion:  (%s) \n",
         __file, __function, __line, __assertion);
  fflush(stdout);
}

#endif // _TESTING

#if defined __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////
//  TEST  //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define TEST_DEFAULT_TIMEOUT 10 // seconds

#define TEST_FORK(value) __test_setfork(value);

#define TEST1(expr)                                                          \
  ((expr)                                                                    \
       ? __mark_point(__STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION) \
       : __test_assert_fail(__FILE__, __LINE__, __STRING(expr),              \
                            __ASSERT_FUNCTION));

#define TEST0(expr)                                                          \
  (!(expr)                                                                   \
       ? __mark_point(__STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION) \
       : __test_assert_fail(__FILE__, __LINE__, __STRING(expr),              \
                            __ASSERT_FUNCTION));

#define TEST_ASSERT(expr) TEST1(expr)

#define BEGIN_TESTING(description)                        \
  __test_init(__STRING(description), __FILE__, __LINE__); \
  if (!__setup_parent())                                  \
  {                                                       \
    __setup_child();

#define END_TESTING \
  }                 \
  __test_end();

#define SKIP_TEST(msg) \
  __test_abort(77, msg, __FILE__, __LINE__, __ASSERT_FUNCTION);
#define ABORT_TEST(msg) \
  __test_abort(99, msg, __FILE__, __LINE__, __ASSERT_FUNCTION);

#define TEST_TIMEOUT(seconds) (__test_timeout(seconds))

#if defined __cplusplus
#if defined __GNUC__ && 800 <= __GNUC__ * 100 + __GNUC_MINOR__
_Pragma("GCC diagnostic ignored \"-Wcatch-value\"")
#endif
#define TEST_STD_EXCEPTION(val, string) \
  try                                   \
  {                                     \
    val;                                \
  }                                     \
  catch (std::exception & e)            \
  {                                     \
    TEST0(strcmp(e.what(), string));    \
  }
#define TEST_MDS_EXCEPTION(val, string) \
  try                                   \
  {                                     \
    val;                                \
  }                                     \
  catch (std::exception & e)            \
  {                                     \
    TEST0(strcmp(e.what(), string));    \
  }
#define TEST_EXCEPTION(val, ExceptionClass) \
  {                                         \
    bool correct_exception_caught = false;  \
    try                                     \
    {                                       \
      val;                                  \
    }                                       \
    catch (ExceptionClass & e)              \
    {                                       \
      correct_exception_caught = true;      \
    }                                       \
    catch (...)                             \
    {                                       \
    }                                       \
    TEST1(correct_exception_caught);        \
  }
#endif
#endif // ifndef _TESTING_H
