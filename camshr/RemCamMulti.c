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
#include <ipdesc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mdsplus/mdsconfig.h>

#ifndef min
#define min(a,b) ((a) < (b)) ? (a) : (b)
#endif

struct descriptor {
  unsigned short length;
  char dtype;
  char class;
  void *pointer;
};

extern short RemCamLastIosb[4];
extern int RemoteServerId();

static int CamMulti(char *routine, char *name, int a, int f, int count, void *data, int mem,
		    unsigned short *iosb);

#define MakeMulti(locnam,remnam) \
int locnam(char *name, int a, int f, int count, void *data, int mem, unsigned short *iosb) \
{ \
  return CamMulti(#remnam,name,a,f,count,data,mem,iosb); \
}

MakeMulti(RemCamFQrepw, FQrepw)
    MakeMulti(RemCamFQstopw, FQstopw)
    MakeMulti(RemCamFStopw, FStopw)
    MakeMulti(RemCamQrepw, Qrepw)
    MakeMulti(RemCamQscanw, Qscanw)
    MakeMulti(RemCamQstopw, Qstopw)
    MakeMulti(RemCamStopw, Stopw)

static int DoCamMulti(char *routine, char *name, int a, int f, int count, void *data, int mem,
		      short *iosb);

static int CamMulti(char *routine, char *name, int a, int f, int count, void *data, int mem,
		    unsigned short *iosb)
{
  int status = 1;

  iosb = (iosb) ? iosb : (unsigned short *)&RemCamLastIosb;

  status = DoCamMulti(routine, name, a, f, count, data, mem, (short *)&RemCamLastIosb);
  if (iosb)
    memcpy(iosb, &RemCamLastIosb, sizeof(RemCamLastIosb));
  return status;
}

static void getiosb(int serverid, short *iosb)
{
  int status;
  struct descrip ans_d = { 0, 0, {0}, 0, 0 };
  status = MdsValue(serverid, "_iosb", &ans_d, NULL);
  if (status & 1 && ans_d.dtype == DTYPE_USHORT && ans_d.ndims == 1 && ans_d.dims[0] == 4) {
    memcpy(RemCamLastIosb, ans_d.ptr, 8);
    if (iosb)
      memcpy(iosb, ans_d.ptr, 8);
  }
  if (ans_d.ptr)
    free(ans_d.ptr);
}

static void getdata(int serverid, void *data)
{
  int status;
  struct descrip ans_d = { 0, 0, {0}, 0, 0 };
  status = MdsValue(serverid, "_data", &ans_d, NULL);
  if (status & 1 && (ans_d.dtype == DTYPE_USHORT || ans_d.dtype == DTYPE_LONG) && ans_d.ptr)
    memcpy(data, ans_d.ptr, ((ans_d.dtype == DTYPE_USHORT) ? 2 : 4) * ans_d.dims[0]);
  if (ans_d.ptr)
    free(ans_d.ptr);
}

static int DoCamMulti(char *routine, char *name, int a, int f, int count, void *data, int mem,
		      short *iosb)
{
  int serverid = RemoteServerId();
  int status = 0;
  int writeData;
  if (serverid) {
    struct descrip data_d = { 8, 1, {0}, 0, 0};
    struct descrip ans_d = { 0, 0, {0}, 0 , 0};
    char cmd[512];
    writeData = (!(f & 0x08)) && (f > 8);
    sprintf(cmd, "CamMulti('%s','%s',%d,%d,%d,%s,%d,_iosb)", routine, name, a, f, count,
	    writeData ? "_data=$" : "_data", mem);
    if (writeData) {
      data_d.dtype = mem < 24 ? DTYPE_SHORT : DTYPE_LONG;
      data_d.dims[0] = count;
      data_d.ptr = data;
      status = MdsValue(serverid, cmd, &data_d, &ans_d, NULL);
    } else {
      status = MdsValue(serverid, cmd, &ans_d, NULL);
    }
    if (status & 1 && ans_d.dtype == DTYPE_LONG && ans_d.ptr) {
      memcpy(&status, ans_d.ptr, 4);
      free(ans_d.ptr);
      ans_d.ptr = 0;
      if (data && f < 8)
	getdata(serverid, data);
      getiosb(serverid, iosb);
    }
  }
  return status;
}

int RemCamSetMAXBUF(char *name, int new)
{
  int serverid = RemoteServerId();
  int status = -1;
  if (serverid) {
    struct descrip ans_d = { 0, 0, {0}, 0, 0};
    char cmd[512];
    sprintf(cmd, "CamSetMAXBUF('%s',%d)", name, new);
    status = MdsValue(serverid, cmd, &ans_d, NULL);
    if (status & 1 && ans_d.dtype == DTYPE_LONG && ans_d.ptr) {
      memcpy(&status, ans_d.ptr, 4);
      free(ans_d.ptr);
      ans_d.ptr = 0;
    } else
      status = -1;
  }
  return status;
}

int RemCamGetMAXBUF(char *name)
{
  int serverid = RemoteServerId();
  int status = -1;
  if (serverid) {
    struct descrip ans_d = { 0, 0, {0}, 0, 0 };
    char cmd[512];
    sprintf(cmd, "CamGetMAXBUF('%s')", name);
    status = MdsValue(serverid, cmd, &ans_d, NULL);
    if (status & 1 && ans_d.dtype == DTYPE_LONG && ans_d.ptr) {
      memcpy(&status, ans_d.ptr, 4);
      free(ans_d.ptr);
      ans_d.ptr = 0;
    } else
      status = -1;
  }
  return status;
}
