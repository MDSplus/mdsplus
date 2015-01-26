#include "dcl.h"
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <readline/readline.h>

extern int mdsdcl_do_command(char const *command);

main(int argc, char const *argv[])
{
  char *command = 0;
  int notDone=1;
  while (notDone) {
    int status;
    char *prompt=mdsdclGetPrompt();
    char *cmd = readline(prompt);
    free(prompt);
    if (cmd) {
      if (command) {
	command = (char *)realloc(command, strlen(cmd) + 1);
	strcat(command, cmd);
	free(cmd);
      } else
	command = cmd;
      if (command[strlen(command) - 1] == '-') {
	command[strlen(command) - 1] = '\0';
	break;
      }
      if (strlen(command) > 0) {
	status = mdsdcl_do_command(command);
	if (status ==  CLI_STS_IVVERB)
	  printf("ERROR: No such command\n");
	add_history(command);
      }
      free(command);
      command = 0;
    }
    else {
      notDone=0;
      printf("\n");
    }
  }
}
