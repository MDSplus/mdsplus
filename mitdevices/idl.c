#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <libroutines.h>
#include <stdlib.h>
#include <string.h>
#include "idl.h"

static char *GetNextCmd (char **cmds);

static int idl_initialized = 0;

static int (*IDL_Execute)(int num, char *cmds[]);
static int (*IDL_ExecuteStr)(char *cmd);
static int (*IDL_Init) (int options, int *argc, char *argv[]);
#define IDL_INIT_NOTTYEDIT 32

static void InitIdl()
{
  int zero;
  if (!idl_initialized)
  {
    DESCRIPTOR(image,"idl");
    DESCRIPTOR(idl_execute,"IDL_Execute");
    DESCRIPTOR(idl_executestr,"IDL_ExecuteStr");
    DESCRIPTOR(idl_init,"IDL_Init");
    int status;
    status = LibFindImageSymbol(&image,&idl_init,&IDL_Init);
    if (status & 1)
    {
      (*IDL_Init)(IDL_INIT_NOTTYEDIT,&zero,0);
      LibFindImageSymbol(&image,&idl_execute,&IDL_Execute);
      LibFindImageSymbol(&image,&idl_executestr,&IDL_ExecuteStr);
      idl_initialized = 1;
    }
    else
      printf("Error activating IDL");
  }
}

int idl___execute(struct descriptor *niddsc, InExecuteStruct *setup)
{
  int ctx = 0;
  int first;
  static char *initialize[] = {"close,/all","!ERROR = 0","retall"};
  static int c_nids[IDL_K_CONG_NODES];
  int status = IDL$_ERROR;
  char *ptr;
  char *cmd;
  char *cmds;

  if (idl_initialized == 0) {
    InitIdl();
  }

  if (idl_initialized == 0) {
    return status;
  }

  status = (*IDL_Execute)(sizeof(initialize)/sizeof(initialize[0]), initialize) == 0;
  if (! status) return status;

  for (cmds = setup->commands; status && (cmd = GetNextCmd(&cmds));)
    status = (*IDL_ExecuteStr)(cmd) == 0;

  if (status==0) status = IDL$_ERROR;
  return status;
}

static char *GetNextCmd (char **cmds)
{
  char *ptr = *cmds;
  char *ans;
  int chars=0;
  for (; *ptr && *ptr=='\n'; ptr++, chars++);
  if (*ptr) {
    ans = ptr;
    for (; *ptr && *ptr!='\n'; ptr++, chars++);
    if (*ptr =='\n')
      chars++;
    *ptr = 0;
  }
  else
    ans = 0;
  *cmds += chars;
  return ans;
}
