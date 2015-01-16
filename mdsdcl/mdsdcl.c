#include "dcl.h"
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <readline/readline.h>

extern int mdsdcl_do_command(char const *command);

main(int argc, char const *argv[])
{
  char *command = 0;
  while (1) {
    int status;
    char *cmd = readline("TCL> ");
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
      status = mdsdcl_do_command(command);
      if (status ==  CLI_STS_IVVERB)
	printf("ERROR: No such command\n");
      free(command);
      command = 0;
    }
  }
}
