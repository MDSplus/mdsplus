#include <mdsdescrip.h>
#include <stdio.h>
#include <stdlib.h>

static int ParseQualifiers();
static int CheckErrors();
typedef struct { unsigned short condition;
                 unsigned short bytcnt;
                 unsigned int   devstat;
               } IOSB;

static struct descriptor Name = {0, DTYPE_T, CLASS_D, 0};
static struct descriptor DefName = {0, DTYPE_T, CLASS_D, 0};
static int A = 0;
static int F = 0;
static int Mem = 24;
static int DefMem = 24;
static void   *D = 0;
static int Count; 
static IOSB iosb;
static int LastStatus;
static int Xrequired = 1;
static int Qrequired = 0;
static struct descriptor value = {0, DTYPE_T, CLASS_D, 0};

#define not_implemented(a) \
    int a(){printf("\n==>  %s : This command is not implemented on this platform\n\n",#a);return 0;}

#define multi_io(ccl_name,cam_name) int ccl_name()\
{\
  int status = ParseQualifiers();\
  if (status & 1) status = cam_name(Name.pointer,A,F,Count,D,Mem,&iosb);\
  return CheckErrors(status,&iosb);\
}

#define single_io(ccl_name,cam_name) int ccl_name()\
{\
  int status = ParseQualifiers();\
  if (status & 1) status = cam_name(Name.pointer,A,F,D,Mem,&iosb);\
  return CheckErrors(status,&iosb);\
}

not_implemented(ccl_dclst)
not_implemented(ccl_execute)
not_implemented(ccl_finish)
not_implemented(ccl_lamwait)
not_implemented(ccl_lpio)
not_implemented(ccl_lqscan)
not_implemented(ccl_lstop)
single_io(ccl_pio,CamPiow)
not_implemented(ccl_plot_data)
multi_io(ccl_qrep,CamQrepw)
multi_io(ccl_qscan,CamQscanw)
multi_io(ccl_qstop,CamQstopw)
not_implemented(ccl_rdata)
not_implemented(ccl_set_memory)
int ccl_set_module();
int ccl_set_xandq();
int ccl_show_data();
int ccl_show_module();
int ccl_show_status();
multi_io(ccl_stop,CamStopw)
not_implemented(ccl_wdata)

static int ParseQualifiers()
{
  int memsize;
  int *d32;
  short *d16;
  int binary,hex,octal;
  int i;
  str_copy_dx(&Name, (cli_get_value("module",&value) & 1) ? &value : &DefName);
  if (Name.pointer == 0)
  {
    printf("No module selected\n");
    return 0;
  }
  A = (cli_get_value("address",&value) & 1) ? atoi(value.pointer) : 0;
  if (A < 0 || A > 15)
  {
    printf("BADADDRESS: bad value for /ADDRESS. Use number from 0 to 15\n");
    return 0;
  }
  F = (cli_get_value("function",&value) & 1) ? atoi(value.pointer) : 0;
  if (F < 0 || F > 31)
  {
    printf("BADFUNCTION: bad value for /FUNCTION. Use number from 0 to 31\n");
    return 0;
  }
  Count = (cli_get_value("count",&value) & 1) ? atoi(value.pointer) : 1;
  if (Count < 1)
  {
    printf("BADCOUNT: bad value for /COUNT. Use number greater than 0.\n");
    return 0;
  }
  Mem = (cli_get_value("memory",&value) & 1) ? atoi(value.pointer) : DefMem;
  if (Mem != 16 && Mem != 24)
  {
    printf("BADMEM: bad value for /MEMORY. Use 16 or 24.\n");
    return 0;
  }
  if ((Count * ((Mem == 24) ? 4 : 2)) > 65535)
  {
    printf("Count too large, max=32767 for 16 bit, 16383 for 24 bit\n");
    return 0;
  }
  if (D) free(D);
  D = malloc(Count * ((Mem == 24) ? 4 : 2));
  d16 = (short *)D;
  d32 = (int *)D;
  binary = cli_present("binary") & 1;
  hex = cli_present("hex") & 1;
  octal = cli_present("octal") & 1;
  for (i=0;i<Count;i++)
  {
    int val = 0;
    if (cli_get_value("data",&value) & 1)
    {
      if (binary)
      {
        int j;
        for (j=0;j<value.length;j++)
	{
          if (value.pointer[value.length-j-1] != '0' || value.pointer[j] != '1')
	  {
            printf("BADDATA: bad value for /DATA with /BINARY. Number must be specified in binary.\n");
            return 0;
          }
          val = val | ((value.pointer[value.length-j-1] - '0') << j);
        }
      }
      else if (hex)
      {
        sscanf(value.pointer,"%x",&val);
      }
      else if (octal)
      {
        sscanf(value.pointer,"%o",&val);
      }
      else
      {
        sscanf(value.pointer,"%d",&val);
      }
    }
    if (Mem == 24)
      d32[i] = val;
    else
      d16[i] = (short)val;
  }
  return 1;
}

static int CheckErrors(int status,IOSB *iosb)
{
  LastStatus = status;
  if (status & 1)
  {
    if (Xrequired)
    {
      int x = CamX(iosb) & 1;
      if (x && (Xrequired == -1))
        printf("CAMX: got X=1, expecting X=0\n");
      else if (!x && (Xrequired == 1))
        printf("CAMNOX: got X=0, expecting X=1\n");
    }
    if (Qrequired)
    {        
      int q = CamQ(iosb) & 1;
      if (q && (Qrequired == -1))
        printf("CAMQ: got Q=1, expecting Q=0\n");
      else if (!q && (Qrequired == 1))
        printf("CAMNOQ: got Q=0, expecting Q=1\n");
    }
  }
  else
  {
    MDSfprintf(stderr,"Error detected in CAMAC call, %s\n",MdsGetMsg(status));
  }
  return 1;
}

int ccl_set_xandq()
{
  if (cli_get_value("x",&value) & 1)
  {
    if (value.pointer[0] == 'N' || value.pointer[0] == 'n')
      Xrequired = -1;
    else if (value.pointer[0] == 'Y' || value.pointer[0] == 'y')
      Xrequired = 1;
    else
      Xrequired = 0;
  }
  if (cli_get_value("q",&value) & 1)
  {
    if (value.pointer[0] == 'N' || value.pointer[0] == 'n')
      Qrequired = -1;
    else if (value.pointer[0] == 'Y' || value.pointer[0] == 'y')
      Qrequired = 1;
    else
      Qrequired = 0;
  }
  return 1;
}

int ccl_set_module()
{
  return cli_get_value("name",&DefName);
}

int ccl_show_module()
{
  printf("Module set to %s\n",DefName.pointer ? DefName.pointer : "<undefined>");
  return 1;
}

int ccl_show_status()
{
  printf("Last status = 0x%x, iosb status = 0x%x, bytcnt = %d, %s, %s\n",
	 LastStatus, iosb.condition, iosb.bytcnt,(CamX(&iosb) & 1) ? "X=1" : "X=0", (CamQ(&iosb) & 1) ? "Q=1" : "Q=0");
  return 1;
}

int ccl_show_data()
{
  int *d32 = (int *)D;
  short *d16 = (short *)D;
  int binary = cli_present("binary") & 1;
  int octal = cli_present("octal") & 1;
  int hex = cli_present("hex") & 1;
  int first=1;
  int last=1;
  int i;
  int chars;
  int maxsmps = iosb.bytcnt / ((Mem == 24) ? 4 : 2);
  char *format;
  char outline[256];
  first =  (cli_get_value("START",&value) & 1) ? atoi(value.pointer) : 1;
  if (first < 1)
    first = 1;
  if (first > maxsmps)
    first = maxsmps;
  last =  (cli_get_value("END",&value) & 1) ? atoi(value.pointer) : first + 99;
  if (last < first)
    last = first;
  if (last > maxsmps)
    last = maxsmps;
  for (i=first,chars=0;i<=last;i++)
  {
    if (chars == 0) 
      chars = sprintf(outline,"%06d ",i); 
    if (octal)
      format = (Mem == 24) ? " %#011o" : " %#06o";
    else if (hex || binary)
      format = (Mem == 24) ? " %#011x" : " %#06x";
    else
      format = (Mem == 24) ? " %11d"  : " %6d";
    chars += sprintf(&outline[chars],format,d32[i-1]);
    if (chars > 72)
    {
      printf("%s\n",outline);
      chars = 0;
    }
  }
  if (chars > 0)
    printf("%s\n",outline);
  return 1;
}
