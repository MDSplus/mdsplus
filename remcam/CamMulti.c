#include <ipdesc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct descriptor { unsigned short length;
                    char dtype;
                    char class;
                    void *pointer;
                  };

extern short RemCamLastIosb[4];
extern int RemoteServerId();

static int CamMulti(char *routine, struct descriptor *name, int *a, int *f, int *count, void *data, int *mem, short *iosb);

#define MakeMulti(locnam,remnam) \
int locnam(struct descriptor *name, int *a, int *f, int *count, void *data, int *mem, short *iosb) \
{ \
  return CamMulti(#remnam,name,a,f,count,data,mem,iosb); \
}

MakeMulti(CamFQrepw,cam$fqrepw)
MakeMulti(CamFQstopw,cam$fqstopw)
MakeMulti(CamFStopw,cam$fstopw)
MakeMulti(CamQrepw,cam$qrepw)
MakeMulti(CamQscanw,cam$qscanw)
MakeMulti(CamQstopw,cam$qstopw)
MakeMulti(CamStopw,cam$stopw)

static int CamMulti(char *routine, struct descriptor *name, int *a, int *f, int *count, void *data, int *mem, short *iosb)
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
    sprintf(cmd,"RemCamMulti('%s','%s',%d,%d,%d,%s,%d,_iosb)",routine,name_c,*a,*f,*count,*f < 8 ? "_data" : "$",*mem);
    if (*f < 8)
    {
      status = MdsValue(serverid,cmd,&ans_d,0);
    }
    else
    {
      data_d.dtype = *mem < 24 ? DTYPE_SHORT : DTYPE_LONG;
      data_d.dims[0] = *count;
      data_d.ptr = data;
      status = MdsValue(serverid,cmd,&data_d,&ans_d,0);
    }      
    if (status & 1)
    {
      int i;
      if (ans_d.dtype == DTYPE_LONG)
      {
        status = *(int *)ans_d.ptr;
        for (i=0;i<4;i++)
        {
          iosb[i] = (short)(((int *)ans_d.ptr)[i+1]);
        }
        for (i=5;i<ans_d.dims[0];i++)
          ((int *)data)[i-5] = ((int *)ans_d.ptr)[i];
      }
      else if (ans_d.dtype == DTYPE_SHORT)
      {
        status = (int)*(unsigned short *)ans_d.ptr;
        for (i=0;i<4;i++)
        {
          RemCamLastIosb[i] = iosb[i] = ((short *)ans_d.ptr)[i+1];
        }
        for (i=5;i<ans_d.dims[0];i++)
          ((short *)data)[i-5] = ((short *)ans_d.ptr)[i];
      }
    }
  }
  return status;
}
