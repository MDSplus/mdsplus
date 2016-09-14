#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <libroutines.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "idl_gen.h"

static char *GetNextCmd(char **cmds);

static int idl_initialized = 0;

static int (*execute) (char *cmd);

static void InitIdl()
{
  //int zero;
  if (!idl_initialized) {
    DESCRIPTOR(image, "IDL");
    DESCRIPTOR(execute_d, "execute");
    int status;
    status = LibFindImageSymbol(&image, &execute_d, &execute);
    if (status & 1)
      status = (*execute) ("print,'IDL Activated'") == 0;
    if (status & 1)
      idl_initialized = 1;
    else
      printf("Error activating IDL");
  }
}

EXPORT int idl___execute(struct descriptor *niddsc __attribute__ ((unused)), InExecuteStruct * setup)
{
  static char *initialize[] = { "close,/all", "!ERROR = 0", "retall" };
  int status = IDL$_ERROR;
  char *cmd;
  char *cmds;
  int i;

  if (idl_initialized == 0) {
    InitIdl();
  }

  if (idl_initialized == 0) {
    return status;
  }
  for (i = 0; i < ((int)(sizeof(initialize) / sizeof(initialize[0]))); i++)
    (*execute) (initialize[i]);

  for (cmds = setup->commands; status && (cmd = GetNextCmd(&cmds));)
    status = (*execute) (cmd) == 0;

  if (status == 0)
    status = IDL$_ERROR;
  return status;
}

static char *GetNextCmd(char **cmds)
{
  char *ptr = *cmds;
  char *ans;
  int chars = 0;
  for (; *ptr && *ptr == '\n'; ptr++, chars++) ;
  if (*ptr) {
    ans = ptr;
    for (; *ptr && *ptr != '\n'; ptr++, chars++) ;
    if (*ptr == '\n')
      chars++;
    *ptr = 0;
  } else
    ans = 0;
  *cmds += chars;
  return ans;
}
