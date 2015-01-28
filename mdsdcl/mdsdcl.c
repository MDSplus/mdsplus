#include <dcl.h>
#include "dcl_p.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <readline/readline.h>

extern int mdsdcl_do_command(char const *command);

main(int argc, char const *argv[])
{
  char *command = 0;
  int notDone = 1;
  int status;
  int i;
  char *output = 0;
  char *error = 0;
  char *prompt = 0;
  if ((argc > 2) && (strcmp("-prep", argv[1]) == 0)) {
    char *prep_cmd = strdup(argv[2]);
    int inquote = 0;
    for (i = 0; i < strlen(prep_cmd); i++) {
      if (prep_cmd[i] == '"')
	inquote = !inquote;
      else if ((!inquote) && (prep_cmd[i] == '-'))
	prep_cmd[i] = '/';
    }
    status = mdsdcl_do_command(prep_cmd);
    free(prep_cmd);
    if (!(status & 1)) {
      fprintf(stderr, "Error: %s\n", MdsGetMsg(status));
      return -1;
    }
    for (i = 3; i < argc; i++)
      argv[i - 2] = argv[i];
    argc -= 2;
  }
  if (argc > 1) {
    char *cmd = strdup(argv[1]);
    for (i = 2; i < argc; i++) {
      cmd = strcat(realloc(cmd, strlen(cmd) + strlen(argv[i]) + 2), " ");
      strcat(cmd, argv[i]);
    }
    status = mdsdcl_do_command(cmd);
    if (!(status & 1)) {
      fprintf(stderr, "Error: %s\n", MdsGetMsg(status));
      return -1;
    }
  }

  prompt = mdsdclGetPrompt();

  while (notDone) {
    char *cmd;
    if (prompt == NULL)
      prompt = mdsdclGetPrompt();
    cmd = readline(prompt);
    if (cmd) {
      if (command) {
	command = (char *)realloc(command, strlen(command) + strlen(cmd) + 1);
	strcat(command, cmd);
	free(cmd);
      } else
	command = cmd;
      if (command[strlen(command) - 1] == '-') {
	command[strlen(command) - 1] = '\0';
	if (prompt)
	  free(prompt);
	prompt = 0;
	prompt = strdup("Continue: ");
	continue;
      }
      if (mdsdclVerify() && strlen(command) > 0)
	  fprintf(stderr,"%s%s\n", prompt, command);
      if (prompt)
	free(prompt);
      prompt = 0;
      if (strlen(command) > 0) {
	char *prompt_more = 0;
	status = mdsdcl_do_command_extra_args(command, &prompt_more, &output, &error);
	if (prompt_more != NULL) {
	  command = strcat(realloc(command, strlen(command) + 2), " ");
	  prompt = strcpy(malloc(strlen(prompt_more) + 10), "_");
	  strcat(prompt, prompt_more);
	  strcat(prompt, ": ");
	  free(prompt_more);
	  continue;
	}
	if (error != NULL) {
	  fprintf(stderr, "%s", error);
	  free(error);
	  error = 0;
	}
	if (output != NULL) {
	  fprintf(stdout, "%s", output);
	  free(output);
	  output = 0;
	}
	add_history(command);
	if (status == MDSDCL_STS_EXIT) {
	  exit(0);
	}
      }
      free(command);
      command = 0;
    } else {
      notDone = 0;
      printf("\n");
    }
  }
}
