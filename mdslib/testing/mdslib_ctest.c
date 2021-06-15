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
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <mdslib.h>
#include <treeshr.h>
#include <socket_port.h>
#define BUFFLEN 10

int status;
int null = 0;
int returnlength = 0;
int dtype_short = DTYPE_SHORT;
int dtype_long = DTYPE_LONG;
int dtype_float = DTYPE_FLOAT;
int dtype_cstring = DTYPE_CSTRING;

#ifdef _WIN32
#define PATHSEP '\\'
#else
#define PATHSEP '/'
#endif

#define TREE "TEST"
#define SHOT 7357
#define SHOTS "7357"
#define TEST(test)                                                             \
  {                                                                            \
    int s = test;                                                              \
    if ((s & 1) == 0)                                                          \
    {                                                                          \
      fprintf(stderr, "%s:%d : " #test " = %d => ERROR\n", __FILE__, __LINE__, \
              s);                                                              \
      exit(1);                                                                 \
    }                                                                          \
  }

int testOpen(char *tree, int shot)
{
  return MdsOpen(tree, &shot);
}
int testClose(char *tree, int shot) { return MdsClose(tree, &shot); }

int testScalarString(char *expression, char *expected)
{
  int length = strlen(expected);
  int lenalloc = length + 32;
  char *string = calloc(lenalloc, 1);
  int dsc = descr(&dtype_cstring, string, &null, &lenalloc);
  returnlength = 0;
  status = MdsValue(expression, &dsc, &null, &returnlength);
  if (STATUS_OK)
  {
    fprintf(stderr, "testScalarString(%.*s -- %s  %d)\n", returnlength, string, expected, returnlength);
    status =
        (returnlength == length) && (strncmp(string, expected, length) == 0);
  }
  free(string);
  return (status);
}

int testSetDefault(char *node) { return (MdsSetDefault(node)); }

int testNull(char *expression)
{
  char *buf = malloc(BUFFLEN);
  int bufflen = BUFFLEN;
  int dsc = descr(&dtype_cstring, buf, &null, &bufflen);
  status = MdsValue(expression, &dsc, &null, &returnlength);
  return ((STATUS_OK) == 0 && (returnlength == 0));
}

int testPut1Dsc(char *node, char *expression, int dsc)
{
  return (MdsPut(node, expression, &dsc, &null));
}

int testPut2Dsc(char *node, char *expression, int dsc1, int dsc2)
{
  return (MdsPut(node, expression, &dsc1, &dsc2, &null));
}

int testClearNode(char *node) { return (MdsPut(node, "", &null)); }

/******** MAJOR TEST SECTIONS ********/

void TestTreeOpenClose()
{
  TEST(testOpen(TREE, SHOT));
  TEST(testOpen("FOOFOOFOO", 0xDEAD) ^ 1);
  TEST(testClose("FOOFOOFOO", 0xDEAD) == TreeNOT_OPEN);
  TEST(testSetDefault("\\" TREE "::TOP:A"));
  TEST(testScalarString("$DEFAULT", "\\" TREE "::TOP:A"));
  TEST(testScalarString("$EXPT", TREE));
  TEST(testClose(TREE, SHOT));
  TEST(testNull("$EXPT"));
}

void TestTdi()
{
  int status;
  float result[10], result1;
  int dsc, dsc1, dsc2, i;
  float arg1 = 1.234567;
  float arg2 = 2.345678;
  int sresult = 10;
  char *machine = getenv("MACHINE");

  for (i = 0; i < 10; i++)
    result[i] = 0;

  /**** should segfault with no return length argument!!!! need to provide NULL
   * *****/
  dsc = descr(&dtype_float, &result1, &null);
  status = MdsValue("1.", &dsc, &null, 0);
  dsc = descr(&dtype_float, result, &sresult, &null);
  status = MdsValue("2. : 20. : 2.", &dsc, &null, &returnlength);
  status = (status && (returnlength == 10));
  if (STATUS_OK)
  {
    for (i = 0; i < returnlength; i++)
      status = status && (result[i] == 2. * (i + 1));
  }
  TEST(status);

  dsc1 = descr(&dtype_float, &arg1, &null);
  dsc2 = descr(&dtype_float, &arg2, &null);
  dsc = descr(&dtype_float, &result1, &null);
  status = MdsValue("$ * $", &dsc1, &dsc2, &dsc, &null, &returnlength);
  status = status && (returnlength == 1);
  if (STATUS_OK)
    status = status && (sqrt(result1 * result1) - (arg1 * arg2) < 1.e-7);
  TEST(status);

  machine = machine ? machine : "";
  TEST(testScalarString("MACHINE()", machine));
}

void TestArray1D()
{
  int i;
  int dsc;
  int size = 100;
  float *array = malloc(size * sizeof(float));
  float *compare = malloc(size * sizeof(float));

  TEST(testOpen(TREE, SHOT));

  for (i = 0; i < size; i++)
    array[i] = i * 10;
  dsc = descr(&dtype_float, array, &size, &null);
  TEST(testPut1Dsc("\\TOP:A", "$", dsc));

  dsc = descr(&dtype_float, compare, &size, &null);
  status = MdsValue("\\TOP:A", &dsc, &null, &returnlength);
  if (STATUS_OK)
  {
    status = (returnlength == size);
    if (STATUS_OK)
    {
      int i;
      for (i = 0; i < size; i++)
        status = status && (array[i] == compare[i]);
    }
  }
  TEST(status);

  TEST(testClearNode("\\TOP:A"));

  free(array);
  free(compare);
}

void TestArray2D()
{
  int dsc;
  int sx = 2;
  int sy = 13;
  int sxx = 4;
  int syy = 20;
  int i;
  float array[2][13] = {
      {0., 31., 28., 31., 30., 31., 30., 31., 31., 30., 31., 30., 31.},
      {0., 31., 29., 31., 30., 31., 30., 31., 31., 30., 31., 30., 31.}};
  float compare[2][13];
  float compareBigger[4][20];
  for (i = 0; i < sx; i++)
  {
    int j;
    for (j = 0; j < sy; j++)
      compare[i][j] = 0;
  }
  for (i = 0; i < sxx; i++)
  {
    int j;
    for (j = 0; j < syy; j++)
      compareBigger[i][j] = 0;
  }

  TEST(testOpen(TREE, SHOT));

  dsc = descr(&dtype_float, array, &sx, &sy, &null);
  TEST(testPut1Dsc("\\TOP:A", "BUILD_SIGNAL($,*,*,*)", dsc));

  dsc = descr(&dtype_float, compare, &sx, &sy, &null);
  status = MdsValue("\\TOP:A", &dsc, &null, &returnlength);
  if (STATUS_OK)
  {
    status = (returnlength == sx * sy);
    if (STATUS_OK)
    {
      int i;
      for (i = 0; i < sx; i++)
      {
        int j;
        for (j = 0; j < sy; j++)
        {
          status = status && (array[i][j] == compare[i][j]);
        }
      }
    }
  }
  TEST(status);
  dsc = descr(&dtype_float, compareBigger, &sxx, &syy, &null);
  status = MdsValue("\\TOP:A", &dsc, &null, &returnlength);
  if (STATUS_OK)
  {
    status = (returnlength == sx * sy);
    if (STATUS_OK)
    {
      int i;
      for (i = 0; i < sx; i++)
      {
        int j;
        for (j = 0; j < sy; j++)
        {
          status = status && (array[i][j] == compare[i][j]);
        }
      }
    }
  }
  TEST(status);

  TEST(testClearNode("\\TOP:A"));
}

int main(int argc, char *argv[])
{
  (void)argv;
  int returnlength = 0;
  int status = 0;
  int dsc = descr(&dtype_long, &status, &null);
  TEST(MdsValue("setenv('test_path=.')", &dsc, &null, &returnlength));
  if (argc > 1)
  {
    SOCKET socket;
    printf("Connecting to: local://0\n");
    socket = MdsConnect("local://0");
    TEST((socket != INVALID_SOCKET));
  }
  TEST(
      MdsValue("TreeOpenNew('" TREE "'," SHOTS ")", &dsc, &null, &returnlength))
  TEST(MdsValue("{_=-1;TreeAddNode('A',_,'ANY');}", &dsc, &null, &returnlength))
  TEST(MdsValue("TreeWrite('" TREE "'," SHOTS ")", &dsc, &null, &returnlength))
  TEST(MdsValue("TreeClose('" TREE "'," SHOTS ")", &dsc, &null, &returnlength))
  TestTreeOpenClose();
  TestTdi();
  TestArray1D();
  TestArray2D();
  printf("TEST OK\n");
  exit(0);
}
