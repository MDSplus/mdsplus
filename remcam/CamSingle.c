#include <ipdesc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct descriptor { unsigned short length;
                    char dtype;
                    char class;
                    void *pointer;
                  };

short RemCamLastIosb[4];

int RemoteServerId()
{
  static int socket = 0;
  if (socket == 0)
  {
    char *server = getenv("camac_server");
    if (server == 0)
    {
      printf("Set the camac_server environment variable to your camac server address\n");
    }
    else
    {
      socket = ConnectToMds(server);
      if (socket < 0)
        socket = 0;
    }
  }
  return socket;
}
 
static int CamSingle(char *routine, struct descriptor *name, int *a, int *f, void *data, int *mem, short *iosb);

#define MakeSingle(locnam,remnam) \
int locnam(struct descriptor *name, int *a, int *f, void *data, int *mem, short *iosb) \
{ \
  return CamSingle(#remnam,name,a,f,data,mem,iosb); \
}

MakeSingle(CamPiow,cam$piow)
MakeSingle(CamPioQrepw,cam$pioqrepw)

static int CamSingle(char *routine, struct descriptor *name, int *a, int *f, void *data, int *mem, short *iosb)
{
  int serverid = RemoteServerId();
  int status = 0;
  if (serverid)
  {
    struct descrip data_d = {8,1,{0,0,0,0,0,0,0},0};
    struct descrip ans_d = {0,0,{0,0,0,0,0,0,0},0};
    char name_c[512];
    char cmd[512];
    strncpy(name_c,name->pointer,name->length);
    name_c[name->length] = '\0';
    sprintf(cmd,"RemCamSingle('%s','%s',%d,%d,%s,%d,_iosb)",routine,name_c,*a,*f,*f < 8 ? "_data" : "$",*mem);
    if (*f < 8)
    {
      status = MdsValue(serverid,cmd,&ans_d,0);
    }
    else
    {
      data_d.dtype = *mem < 24 ? DTYPE_SHORT : DTYPE_LONG;
      data_d.ptr = data;
      status = MdsValue(serverid,cmd,&data_d,&ans_d,0);
    }      
    if (status & 1)
    {
      int i;
      status = *(int *)ans_d.ptr;
      for (i=0;i<4;i++)
      {
        RemCamLastIosb[i] = iosb[i] = (short)(((int *)ans_d.ptr)[i+1]);
      }
      for (i=5;i<ans_d.dims[0];i++)
        if (*mem < 24)
          ((short *)data)[i-5] = (short)((int *)ans_d.ptr)[i];
        else
          ((int *)data)[i-5] = ((int *)ans_d.ptr)[i];
    }
  }
  return status;
}
