#include <stdio.h>
#include <export.h>
#include <signal.h>

static int init()
{
   static int done=0;
   int zero=0;
   if (!done)
   {
     IDL_Init(IDL_INIT_NOTTYEDIT | IDL_INIT_QUIET ,&zero,0);
     done=1;
   }
   return 1;
}
int execute(char *command)
{
  int status;
  sigset_t newsigset;
  sigemptyset(&newsigset);
  sigaddset(&newsigset,SIGALRM);
  sigprocmask(SIG_BLOCK, &newsigset, NULL);
  init();
  status = IDL_ExecuteStr(command);
  sigprocmask(SIG_BLOCK, &newsigset, NULL);
  return status;
}

