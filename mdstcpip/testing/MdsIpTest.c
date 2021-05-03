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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mdsshr.h>
#include <mdsdescrip.h>
#include <status.h>
#include <ipdesc.h>
static int __test_passed = 0, __test_failed = 0;

#define TEST_FATAL(cond, ...)       \
  do                                \
  {                                 \
    if (cond)                       \
    {                               \
      fprintf(stderr, __VA_ARGS__); \
      return 1;                     \
    }                               \
  } while (0)
#define TEST_FAIL(...)                       \
  do                                         \
  {                                          \
    fprintf(stderr, "FAILED: " __VA_ARGS__); \
    __test_failed++;                         \
  } while (0)
#define TEST_PASS(...) \
  do                   \
  {                    \
    __test_passed++;   \
  } while (0)

static void TEST_TOTAL()
{
  if (__test_failed)
  {
    fprintf(stderr, "TOTAL: FAILED %d/%d\n", __test_failed, __test_failed + __test_passed);
    exit(1);
  }
  else
  {
    fprintf(stderr, "TOTAL: PASSED %d/%d\n", __test_passed, __test_failed + __test_passed);
    exit(0);
  }
}

#define TEST_TRUE(cond, ...) \
  if (!(cond))               \
  TEST_FAIL(__VA_ARGS__)
#define TEST_FALSE(cond, ...) \
  if ((cond))                 \
  TEST_FAIL(__VA_ARGS__)
#define TEST_OK(...) TEST_TRUE(status & 1, __VA_ARGS__)

static void test_value(int c, char *expr, dtype_t typ, int len, void *val)
{
  struct descrip ans = {0};
  int status = MdsValue(c, expr, &ans, NULL);
  if (STATUS_NOT_OK)
    TEST_FAIL("'%s' : STATUS = %d : %s\n", expr, status, MdsGetMsg(status));
  else if (ans.ndims != 0)
    TEST_FAIL("'%s' : ndims\n", expr);
  else if (ans.dtype != typ)
    TEST_FAIL("'%s' : DTYPE %d != %d\n", expr, ans.dtype, typ);
  else if (ans.length != len)
    TEST_FAIL("'%s' : LENGTH %d != %d\n", expr, ans.length, len);
  else if (memcmp(ans.ptr, val, len))
    TEST_FAIL("'%s' : VALUE\n", expr);
  else
    TEST_PASS();
  free(ans.ptr);
}
#define TEST_VALUE(expr, DT, type, val)             \
  do                                                \
  {                                                 \
    type v = val;                                   \
    test_value(c, expr, DTYPE_##DT, sizeof(v), &v); \
  } while (0)

extern int MdsIpGetDescriptor(int id, const char *expression, int nargs,
                              mdsdsc_t **arglist_in,
                              mdsdsc_xd_t *ans_ptr);
static void test_descr(int c, char *expr, int argn, mdsdsc_t **argv, class_t cls, dtype_t typ, int len, void *val)
{
  EMPTYXD(ans);
  int status = MdsIpGetDescriptor(c, expr, argn, argv, &ans);
  if (STATUS_NOT_OK)
    TEST_FAIL("'%s' : STATUS = %d : %s\n", expr, status, MdsGetMsg(status));
  else if (ans.pointer->dtype != typ)
    TEST_FAIL("'%s' : DTYPE %d != %d\n", expr, ans.pointer->dtype, typ);
  else if (ans.pointer->length != len)
    TEST_FAIL("'%s' : LENGTH %d != %d\n", expr, ans.pointer->length, len);
  else if (memcmp(ans.pointer->pointer, val, len))
    TEST_FAIL("'%s' : VALUE\n", expr);
  else if (ans.pointer->class != cls)
    TEST_FAIL("'%s' : CLASS\n", expr); // fails if server does not serialize
  else
    TEST_PASS();
  MdsFree1Dx(&ans, NULL);
}

#define TEST_DESCR(expr, DT, type, val)                               \
  do                                                                  \
  {                                                                   \
    type v = val;                                                     \
    test_descr(c, expr, 0, NULL, CLASS_S, DTYPE_##DT, sizeof(v), &v); \
  } while (0)

int main(int argc, char **argv)
{
  (void)test_value;
  (void)test_descr;
  atexit(TEST_TOTAL);
  int c = ConnectToMds((argc > 1) ? argv[1] : "thread://0");
  TEST_DESCR("-1", L, int, -1);
  TEST_FATAL(c == -1, "MdsConnection failed.\n");
  TEST_VALUE("-1", L, int, -1);
  TEST_VALUE("0xffffffffLU", LU, uint32_t, -1);
  TEST_VALUE("-1W", W, short, -1);
  TEST_VALUE("-1B", B, char, -1);
  return 0;
}
