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
#include <mdslib.h>
#include <treeshr.h>
#include <math.h>

#define BUFFLEN 10

long status;
int null = 0;
int returnlength = 0;
int dtype_short = DTYPE_SHORT;
int dtype_long = DTYPE_LONG;
int dtype_float = DTYPE_FLOAT;
int dtype_cstring = DTYPE_CSTRING;

void report(int test)
{
  if (test & 1) {
    printf(" ... ok\n");
  } else {
    printf(" ... ERROR\n");
  }
}

long testOpen(char *tree, int shot)
{
  printf("Opening tree %s shot %d", tree, shot);
  status = MdsOpen(tree, &shot);
  return (status);
}

long testClose(char *tree, int shot)
{
  printf("Closing tree %s shot %d", tree, shot);
  status = MdsClose(tree, &shot);
  return (status);
}

long testScalarString(char *expression, char *expected, int length)
{
  char *string = malloc(length + 1);
  int dsc = descr(&dtype_cstring, string, &null, &length);
  printf("Checking to see if '%s' is '%s'", expression, expected);
  status = MdsValue(expression, &dsc, &null, &returnlength);
  /*printf("result: %s\n",string); */
  if (status & 1)
    status = (returnlength == length) && (strncmp(string, expected, length) == 0);
  free(string);
  return (status);
}

long testSetDefault(char *node)
{
  printf("Setting default to %s", node);
  return (MdsSetDefault(node));
}

long testNull(char *expression)
{
  char *buf = malloc(BUFFLEN);
  int bufflen = BUFFLEN;
  int dsc = descr(&dtype_cstring, buf, &null, &bufflen);
  printf("Checking to see if '%s' is NULL", expression);
  status = MdsValue(expression, &dsc, &null, &returnlength);
  return ((status & 1) == 0 && (returnlength == 0));
}

long testPut1Dsc(char *node, char *expression, int dsc)
{
  printf("Writing 1D array expression '%s' to  node %s", expression, node);
  return (MdsPut(node, expression, &dsc, &null));
}

long testPut2Dsc(char *node, char *expression, int dsc1, int dsc2)
{
  printf("Writing 2D array expression '%s' to  node %s\n", expression, node);
  return (MdsPut(node, expression, &dsc1, &dsc2, &null));
}

long testClearNode(char *node)
{
  printf("Clearing node %s", node);
  return (MdsPut(node, "", &null));
}

/******** MAJOR TEST SECTIONS ********/

void TestTreeOpenClose()
{
  printf("\n*** TREE OPENING AND CLOSING TESTS ***\n");
  report(testOpen("SUBTREE", -1));
  report(testOpen("FOOFOOFOO", 12345) == TreeFILE_NOT_FOUND);
  report(testOpen("MAIN", -1));
  report(testSetDefault("\\MAIN::TOP.CHILD"));
  report(testScalarString("$DEFAULT", "\\MAIN::TOP.CHILD", 16));
  report(testClose("FOOFOOFOO", 12345) == TreeNOT_OPEN);
  report(testScalarString("$EXPT", "MAIN", 4));
  report(testClose("SUBTREE", -1));
  report(testScalarString("$EXPT", "MAIN", 4));
  report(testClose("MAIN", -1));
  report(testNull("$EXPT"));
}

void TestTdi()
{
  long status;
  float result[10], result1;
  int dsc, dsc1, dsc2, i;
  float arg1 = 1.234567;
  float arg2 = 2.345678;
  int sresult = 10;
  char *machine = getenv("MACHINE");

  printf("\n*** TDI TESTS ***\n");

  for (i = 0; i < 10; i++)
    result[i] = 0;

/**** should segfault with no return length argument!!!! need to provide NULL *****/
  dsc = descr(&dtype_float, &result1, &null);
  status = MdsValue("1.", &dsc, &null, 0);
  printf("status: %ld\n", status);

  printf("Range creation");
  dsc = descr(&dtype_float, result, &sresult, &null);
  status = MdsValue("2. : 20. : 2.", &dsc, &null, &returnlength);
  status = (status && (returnlength == 10));
  if (status & 1) {
    for (i = 0; i < returnlength; i++)
      status = status && (result[i] == 2. * (i + 1));
  }
  report(status);

  printf("Multiplication of two parameters");
  dsc1 = descr(&dtype_float, &arg1, &null);
  dsc2 = descr(&dtype_float, &arg2, &null);
  dsc = descr(&dtype_float, &result1, &null);
  status = MdsValue("$ * $", &dsc1, &dsc2, &dsc, &null, &returnlength);
  status = status && (returnlength == 1);
  if (status & 1)
    status = status && (sqrt(result1 * result1) - (arg1 * arg2) < 1.e-7);
  report(status);

  machine = machine ? machine : "";
  report(testScalarString("MACHINE()", machine, strlen(machine)));
}

void TestArray1D()
{
  int i;
  int dsc;
  int size = 100;
  float *array = malloc(size * sizeof(float));
  float *compare = malloc(size * sizeof(float));

  printf("\n*** WRITING AND READING 1D ARRAY TO/FROM TREE ***\n");
  report(testOpen("MAIN", -1));

  for (i = 0; i < size; i++)
    array[i] = i * 10;
  dsc = descr(&dtype_float, array, &size, &null);
  report(testPut1Dsc("\\TOP:NUMERIC", "$", dsc));

  printf("Testing data in \\TOP:NUMERIC");
  dsc = descr(&dtype_float, compare, &size, &null);
  status = MdsValue("\\TOP:NUMERIC", &dsc, &null, &returnlength);
  if (status & 1) {
    status = (returnlength == size);
    if (status & 1) {
      int i;
      for (i = 0; i < size; i++)
	status = status && (array[i] == compare[i]);
    }
  }
  report(status);

  report(testClearNode("\\TOP:NUMERIC"));

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
    {0., 31., 29., 31., 30., 31., 30., 31., 31., 30., 31., 30., 31.}
  };
  float compare[2][13];
  float compareBigger[4][20];
  for (i = 0; i < sx; i++) {
    int j;
    for (j = 0; j < sy; j++)
      compare[i][j] = 0;
  }
  for (i = 0; i < sxx; i++) {
    int j;
    for (j = 0; j < syy; j++)
      compareBigger[i][j] = 0;
  }

  printf("\n*** WRITING AND READING 2D SIGNAL ***\n");
  report(testOpen("MAIN", -1));

  dsc = descr(&dtype_float, array, &sx, &sy, &null);
  report(testPut1Dsc("\\TOP:SIGNAL", "BUILD_SIGNAL($,*,*,*)", dsc));

  printf("Testing data in \\TOP:SIGNAL");
  dsc = descr(&dtype_float, compare, &sx, &sy, &null);
  status = MdsValue("\\TOP:SIGNAL", &dsc, &null, &returnlength);
  if (status & 1) {
    status = (returnlength == sx * sy);
    if (status & 1) {
      int i;
      for (i = 0; i < sx; i++) {
	int j;
	for (j = 0; j < sy; j++) {
	  /*printf("i: %d, j: %d, data: %f \n",i,j,compare[i][j]); */
	  status = status && (array[i][j] == compare[i][j]);
	}
      }
    }
  }
  report(status);

  printf("Testing reading 2D data from \\TOP:SIGNAL into larger array");
  dsc = descr(&dtype_float, compareBigger, &sxx, &syy, &null);
  status = MdsValue("\\TOP:SIGNAL", &dsc, &null, &returnlength);
  if (status & 1) {
    status = (returnlength == sx * sy);
    if (status & 1) {
      int i;
      for (i = 0; i < sx; i++) {
	int j;
	for (j = 0; j < sy; j++) {
	  /*printf("i: %d, j: %d, data: %f \n",i,j,compare[i][j]); */
	  status = status && (array[i][j] == compare[i][j]);
	}
      }
    }
  }
  report(status);

  report(testClearNode("\\TOP:SIGNAL"));

}

int main(int argc, char *argv[])
{
  if (argc > 1) {
    SOCKET socket;
    printf("*** Connecting to: %s", argv[1]);
    socket = MdsConnect(argv[1]);
    report((socket != INVALID_SOCKET));
  }

  TestTreeOpenClose();
  TestTdi();
  TestArray1D();
  TestArray2D();
  printf("\n");
  exit(0);
}
