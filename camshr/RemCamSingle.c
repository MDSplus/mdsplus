#include <ipdesc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <config.h>

struct descriptor {
  unsigned short length;
  char dtype;
  char class;
  void *pointer;
};

short RemCamLastIosb[4];

int RemoteServerId()
{
  static int socket = 0;
  if (socket == 0) {
    char *server = getenv("camac_server");
    if (server == 0) {
      printf("Set the camac_server environment variable to your camac server address\n");
    } else {
      socket = ConnectToMds(server);
      if (socket < 0)
	socket = 0;
    }
  }
  return socket;
}

static int CamSingle(char *routine, char *name, int a, int f, void *data, int mem, short *iosb);

#define MakeSingle(locnam,remnam) \
int locnam(char *name, int a, int f, void *data, int mem, short *iosb) \
{ \
  return CamSingle(#remnam,name,a,f,data,mem,iosb); \
}

MakeSingle(RemCamPiow, Piow)
    MakeSingle(RemCamPioQrepw, PioQrepw)

static void getiosb(int serverid, short *iosb)
{
  int status;
  struct descrip ans_d = { 0, 0, {0}, 0, 0};
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
  struct descrip ans_d = { 0, 0, {0}, 0, 0};
  status = MdsValue(serverid, "_data", &ans_d, NULL);
  if (status & 1 && (ans_d.dtype == DTYPE_USHORT || ans_d.dtype == DTYPE_LONG) && ans_d.ptr)
    memcpy(data, ans_d.ptr, (ans_d.dtype == DTYPE_USHORT) ? 2 : 4);
  if (ans_d.ptr)
    free(ans_d.ptr);
}

static int CamSingle(char *routine, char *name, int a, int f, void *data, int mem, short *iosb)
{
  int serverid = RemoteServerId();
  int status = 0;
  int writeData;
  if (serverid) {
    struct descrip data_d = { 8, 0, {0}, 0, 0};
    struct descrip ans_d = { 0, 0, {0}, 0, 0};
    char cmd[512];

    writeData = (!(f & 0x08)) && (f > 8);
    sprintf(cmd, "CamSingle('%s','%s',%d,%d,%s,%d,_iosb)", routine, name, a, f,
	    (writeData) ? "_data=$" : "_data", mem);
    if (writeData) {
      data_d.dtype = mem < 24 ? DTYPE_SHORT : DTYPE_LONG;
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
