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

long testOpen(int ttype, int conid, char *tree, int shot)
{
  printf("Opening tree %s shot %d", tree, shot);
  if (ttype < 3)
    status = MdsOpen(tree, &shot);
  else
    status = MdsOpenR(&conid, tree, &shot);
  return (status);
}

long testClose(int ttype, int conid, char *tree, int shot)
{
  printf("Closing tree %s shot %d", tree, shot);
  if (ttype < 3)
    status = MdsClose(tree, &shot);
  else
    status = MdsCloseR(&conid, tree, &shot);
  return (status);
}

long testScalarString(int ttype, int conid, char *expression, char *expected, int length)
{
  char *string = malloc(length + 1);
  int dsc = descr(&dtype_cstring, string, &null, &length);
  int dsc2 = descr2(&dtype_cstring, &null, &length);
  printf("Checking to see if '%s' is '%s'", expression, expected);
  switch (ttype) {
  case 1:
    status = MdsValue(expression, &dsc, &null, &returnlength);
    break;
  case 2:
    status = MdsValue2(expression, &dsc2, string, &null, &returnlength);
    break;
  case 3:
    status = MdsValueR(&conid, expression, &dsc, &null, &returnlength);
    break;
  case 4:
    status = MdsValue2R(&conid, expression, &dsc2, string, &null, &returnlength);
    break;
  }

  /*printf("result: %s\n",string); */
  if (status & 1)
    status = (returnlength == length) && (strncmp(string, expected, length) == 0);
  free(string);
  return (status);
}

long testSetDefault(int ttype, int conid, char *node)
{
  printf("Setting default to %s", node);
  if (ttype < 3)
    return (MdsSetDefault(node));
  else
    return (MdsSetDefaultR(&conid, node));
}

long testNull(int ttype, int conid, char *expression)
{
  char *buf = malloc(BUFFLEN);
  int bufflen = BUFFLEN;
  int dsc = descr(&dtype_cstring, buf, &null, &bufflen);
  int dsc2 = descr2(&dtype_cstring, &null, &bufflen);
  printf("Checking to see if '%s' is NULL", expression);
  switch (ttype) {
  case 1:
    status = MdsValue(expression, &dsc, &null, &returnlength);
    break;
  case 2:
    status = MdsValue2(expression, &dsc2, buf, &null, &returnlength);
    break;
  case 3:
    status = MdsValueR(&conid, expression, &dsc, &null, &returnlength);
    break;
  case 4:
    status = MdsValue2R(&conid, expression, &dsc, buf, &null, &returnlength);
    break;
  } 
  return ((status & 1) == 0 && (returnlength == 0));
}

long testPut1Dsc(int ttype, int conid, char *node, char *expression, int dsc, int dsc2, void *value)
{
  int status = 0;
  printf("Writing 1D array expression '%s' to  node %s", expression, node);
  switch (ttype) {
  case 1:
    status = MdsPut(node, expression, &dsc, &null);
    break;
  case 2:
    status = MdsPut2(node, expression, &dsc2, value, &null);
    break;
  case 3:
    status = MdsPutR(&conid, node, expression, &dsc, &null);
    break;
  case 4:
    status = MdsPut2R(&conid, node, expression, &dsc2, value, &null);
    break;
  }
  return status;
}

long testPut2Dsc(int ttype, int conid, char *node, char *expression,
		 int dsc_1, void *value_1, int dsc2_1,
		 int dsc_2, void *value_2, int dsc2_2)
{
  int status = 0;
  printf("Writing 2D array expression '%s' to  node %s\n", expression, node);
  switch (ttype) {
  case 1:
    status = MdsPut(node, expression, &dsc_1, &dsc_2, &null);
    break;
  case 2:
    status = MdsPut2(node, expression, &dsc2_1, value_1, &dsc2_2, value_2, &null);
    break;
  case 3:
    status = MdsPutR(&conid, node, expression, &dsc_1, &dsc_2, &null);
    break;
  case 4:
    status = MdsPut2R(&conid, node, expression, &dsc2_1, value_1, &dsc2_2, value_2, &null);
    break;
  }
  return status;
}

long testClearNode(int ttype, int conid, char *node)
{
  int status = 0;
  printf("Clearing node %s", node);
  switch (ttype) {
  case 1:
    status = MdsPut(node, "", &null);
    break;
  case 2:
    status = MdsPut2(node, "", &null);
    break;
  case 3:
    status = MdsPutR(&conid, node, "", &null);
    break;
  case 4:
    status =MdsPut2R(&conid, node, "", &null);
    break;
  }
  return status;
}

/******** MAJOR TEST SECTIONS ********/

void TestTreeOpenClose(int ttype, int conid)
{
  printf("\n*** TREE OPENING AND CLOSING TESTS ***\n");
  report(testOpen(ttype, conid, "SUBTREE", -1));
  report(testOpen(ttype, conid, "FOOFOOFOO", 12345) == TreeFILE_NOT_FOUND);
  report(testOpen(ttype, conid, "MAIN", -1));
  report(testSetDefault(ttype, conid, "\\MAIN::TOP.CHILD"));
  report(testScalarString(ttype, conid, "$DEFAULT", "\\MAIN::TOP.CHILD", 16));
  report(testClose(ttype, conid, "FOOFOOFOO", 12345) == TreeNOT_OPEN);
  report(testScalarString(ttype, conid, "$EXPT", "MAIN", 4));
  report(testClose(ttype, conid, "SUBTREE", -1));
  report(testScalarString(ttype, conid, "$EXPT", "MAIN", 4));
  report(testClose(ttype, conid, "MAIN", -1));
  report(testNull(ttype, conid, "$EXPT"));
}

void TestTdi(int ttype, int conid)
{
  long status=0;
  float result[10], result1;
  int dsc_1, dsc2_1, dsc_2, dsc2_2, i, dsc_r, dsc2_r;
  float arg1 = 1.234567;
  float arg2 = 2.345678;
  int sresult = 10;
  char *machine = getenv("MACHINE");

  printf("\n*** TDI TESTS ***\n");

  for (i = 0; i < 10; i++)
    result[i] = 0;

/**** should segfault with no return length argument!!!! need to provide NULL *****/
  dsc_1 = descr(&dtype_float, &result1, &null);
  dsc2_1 = descr2(&dtype_float, &null);
  switch (ttype) {
  case 1:
    status = MdsValue("1.", &dsc_1, &null, 0);
    break;
  case 2:
    status = MdsValue2("1.", &dsc2_1, &result1, &null, 0);
    break;
  case 3:
    status = MdsValueR(&conid, "1.", &dsc_1, &null, 0);
    break;
  case 4:
    status = MdsValue2R(&conid, "1.", &dsc2_1, &result1, &null, 0);
    break;
  }
  printf("status: %ld\n", status);

  printf("Range creation");
  dsc_r = descr(&dtype_float, result, &sresult, &null);
  dsc2_r = descr2(&dtype_float, &sresult, &null);
  switch (ttype) {
  case 1:
    status = MdsValue("2. : 20. : 2.", &dsc_r, &null, &returnlength);
    break;
  case 2:
    status = MdsValue2("2. : 20. : 2.", &dsc2_r, result, &null, &returnlength);
    break;
  case 3:
    status = MdsValueR(&conid, "2. : 20. : 2.", &dsc_r, &null, &returnlength);
    break;
  case 4:
    status = MdsValue2R(&conid, "2. : 20. : 2.", &dsc_r, result, &null, &returnlength);
    break;
  }
  status = (status && (returnlength == 10));
  if (status & 1) {
    for (i = 0; i < returnlength; i++)
      status = status && (result[i] == 2. * (i + 1));
  }
  report(status);

  printf("Multiplication of two parameters");
  dsc_1 = descr(&dtype_float, &arg1, &null);
  dsc2_1 = descr2(&dtype_float, &null);
  dsc_2 = descr(&dtype_float, &arg2, &null);
  dsc2_2 = descr2(&dtype_float, &null);
  dsc_r = descr(&dtype_float, &result1, &null);
  dsc2_r = descr2(&dtype_float, &null);
  switch (ttype) {
  case 1:
    status = MdsValue("$ * $", &dsc_1, &dsc_2, &dsc_r, &null, &returnlength);
    break;
  case 2:
    status = MdsValue2("$ * $", &dsc2_1, &arg1, &dsc2_2, &arg2, &dsc2_r, &result1, &null, &returnlength);
    break;
  case 3:
    status = MdsValueR(&conid, "$ * $", &dsc_1, &dsc_2, &dsc_r, &null, &returnlength);
    break;
  case 4:
    status = MdsValue2R(&conid, "$ * $", &dsc2_1, &arg1, &dsc2_2, &arg2, &dsc2_r, &result1,
			&null, &returnlength);
    break;
  }
  status = status && (returnlength == 1);
  if (status & 1)
    status = status && (sqrt(result1 * result1) - (arg1 * arg2) < 1.e-7);
  report(status);

  machine = machine ? machine : "";
  report(testScalarString(ttype, conid, "MACHINE()", machine, strlen(machine)));
}

void TestArray1D(int ttype, int conid)
{
  int i;
  int dsc, dsc2;
  int size = 100;
  float *array = malloc(size * sizeof(float));
  float *compare = malloc(size * sizeof(float));

  printf("\n*** WRITING AND READING 1D ARRAY TO/FROM TREE ***\n");
  report(testOpen(ttype, conid, "MAIN", -1));

  for (i = 0; i < size; i++)
    array[i] = i * 10;
  dsc = descr(&dtype_float, array, &size, &null);
  dsc2 = descr2(&dtype_float, &size, &null);
  report(testPut1Dsc(ttype, conid, "\\TOP:NUMERIC", "$", dsc, dsc2, array));

  printf("Testing data in \\TOP:NUMERIC ttype=%d", ttype);
  dsc = descr(&dtype_float, compare, &size, &null);
  dsc2 = descr2(&dtype_float, &size, &null);
  switch (ttype) {
  case 1:
    status = MdsValue("\\TOP:NUMERIC", &dsc, &null, &returnlength);
    break;
  case 2:
    status = MdsValue2("\\TOP:NUMERIC", &dsc2, compare, &null, &returnlength);
    break;
  case 3:
    status = MdsValueR(&conid,"\\TOP:NUMERIC", &dsc, &null, &returnlength);
    break;
  case 4:
    status = MdsValue2R(&conid, "\\TOP:NUMERIC", &dsc2, compare, &null, &returnlength);
    break;
  }
  if (status & 1) {
    status = (returnlength == size);
    if (status & 1) {
      int i;
      for (i = 0; i < size; i++)
	status = status && (array[i] == compare[i]);
    }
  }
  report(status);

  report(testClearNode(ttype, conid, "\\TOP:NUMERIC"));

  free(array);
  free(compare);
}

void TestArray2D(int ttype, int conid)
{
  int dsc,dsc2;
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
  report(testOpen(ttype, conid, "MAIN", -1));

  dsc = descr(&dtype_float, array, &sx, &sy, &null);
  dsc2 = descr2(&dtype_float, &sx, &sy, &null);
  report(testPut1Dsc(ttype, conid, "\\TOP:SIGNAL", "BUILD_SIGNAL($,*,*,*)", dsc, dsc2, array));

  printf("Testing data in \\TOP:SIGNAL ttype=%d",ttype);
  dsc = descr(&dtype_float, compare, &sx, &sy, &null);
  dsc2 = descr2(&dtype_float, &sx, &sy, &null);
  switch (ttype) {
  case 1:
    status = MdsValue("\\TOP:SIGNAL", &dsc, &null, &returnlength);
    break;
  case 2:
    status = MdsValue2("\\TOP:SIGNAL", &dsc2, compare, &null, &returnlength);
    break;
  case 3:
    status = MdsValueR(&conid, "\\TOP:SIGNAL", &dsc, &null, &returnlength);
    break;
  case 4:
    status = MdsValue2R(&conid, "\\TOP:SIGNAL", &dsc2, compare, &null, &returnlength);
    break;
  }
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

  printf("Testing reading 2D data from \\TOP:SIGNAL into larger array ttype=%d",ttype);
  dsc = descr(&dtype_float, compareBigger, &sxx, &syy, &null);
  dsc2 = descr2(&dtype_float, &sxx, &syy, &null);
  switch (ttype) {
  case 1:
    status = MdsValue("\\TOP:SIGNAL", &dsc, &null, &returnlength);
    break;
  case 2:
    status = MdsValue2("\\TOP:SIGNAL", &dsc2, compareBigger, &null, &returnlength);
    break;
  case 3:
    status = MdsValueR(&conid, "\\TOP:SIGNAL", &dsc, &null, &returnlength);
    break;
  case 4:
    status = MdsValue2R(&conid, "\\TOP:SIGNAL", &dsc2, compareBigger, &null, &returnlength);
    break;
  }
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

  report(testClearNode(ttype, conid, "\\TOP:SIGNAL"));

}

int main(int argc, char *argv[])
{
  SOCKET connection=-1;
  int pconnection=-1;
  if (argc > 1) {
    printf("*** Connecting to: %s", argv[1]);
    connection = MdsConnect(argv[1]);
    pconnection = MdsConnectR(argv[1]);
    report((connection != INVALID_SOCKET));
  }
  int ttype;

  for (ttype=1; ttype < 5; ttype++) {
    TestTreeOpenClose(ttype, pconnection);
    TestTdi(ttype, pconnection);
    TestArray1D(ttype, pconnection);
    TestArray2D(ttype, pconnection);
    TestTreeOpenClose(ttype, pconnection);
    TestTdi(ttype, pconnection);
    TestArray1D(ttype, pconnection);
    TestArray2D(ttype, pconnection);
    TestTreeOpenClose(ttype, pconnection);
  }
  MdsDisconnect();
  MdsDisconnectR(&pconnection);
  printf("\n");
  exit(0);
}
