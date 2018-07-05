/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <dcl.h>
#include <mdsdcl_messages.h>
#include "dcl_p.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include "mdsdclthreadsafe.h"

extern int mdsdcl_do_command(char const *command);

/*! Main mdsdcl command line interpretter program. mdsdcl prompts
the user for input and executes commands defined in xml command definition
tables. The default command table is $MDSPLUS_DIR/xml/mdsdcl_commands.xml
which define a basic set of commands. Additional command definitions can
be added using the "SET COMMAND table-name" command.

 \param argc [in] The number of options provided on the command line.
 \param argv [in] An array of command line option strings.
*/

static void flushOut(char *output)
{
  fprintf(stdout, "%s", output);
  fflush(stdout);
  output[0] = 0;
}

static void flushError(char *error)
{
  fprintf(stderr, "%s", error);
  fflush(stderr);
  error[0] = 0;
}

void handle_signals(int signo __attribute__ ((unused)))
{
}

int main(int argc, char const *argv[])
{
  char *history_file = NULL;
  char *command = NULL;
  int notDone = 1;
  int status = 0;
  int i;
  char *output = NULL;
  char *error = NULL;
  char *prompt = NULL;

  /* See if a -prep option is provided as the first command option. */

  mdsdclSetOutputRtn(flushOut);
  mdsdclSetErrorRtn(flushError);

  mdsdclAddCommands("mdsdcl_commands", &error);
  if (error) {
    fprintf(stderr, "%s", error);
    fflush(stderr), free(error);
    exit(1);
  }

  if ((argc > 2) && (strcmp("-prep", argv[1]) == 0)) {
    char *prep_cmd = strdup(argv[2]);
    int inquote = 0;
    size_t k;

    /* Replace option hyphens with slashes to convert the prep to a
       SET COMMAND command. Utilties such as mdstcl, mdsccl, etc use
       this to load the appropriate command definitions, set the prompt
       string and the history file. */

    for (k = 0; k < strlen(prep_cmd); k++) {
      if (prep_cmd[k] == '"')
	inquote = !inquote;
      else if ((!inquote) && (prep_cmd[k] == '-'))
	prep_cmd[k] = '/';
    }

    /* Execute the prep command */

    status = mdsdcl_do_command(prep_cmd);
    free(prep_cmd);

    /* Remove the prep command from the option list */

    for (i = 3; i < argc; i++)
      argv[i - 2] = argv[i];
    argc -= 2;
  }

  /* If other options on command line */

  if (argc > 1) {

    /* Concatenate rest of line into a mdsdcl command string */
    char *cmd = strdup(argv[1]);
    for (i = 2; i < argc; i++) {
      cmd = strcat(realloc(cmd, strlen(cmd) + strlen(argv[i]) + 2), " ");
      strcat(cmd, argv[i]);
    }

    /* Attempt to execute the command */

    status = mdsdcl_do_command(cmd);
    add_history(cmd);
    if (cmd)
      free(cmd);
    goto done;

  }

  /* Get the command prompt */

  prompt = mdsdclGetPrompt();

  signal(SIGINT, handle_signals);

  /* While more commands to be entered */

  while (notDone) {
    char *cmd;

    /* If prompt not defined get the prompt */

    if (prompt == NULL)
      prompt = mdsdclGetPrompt();

#ifdef HAVE_RL_SET_SIGNALS
    rl_catch_signals = 1;
    rl_set_signals();
#endif
    /* Read in a command */
    cmd = readline(prompt);

    /* If not EOF */

    if (cmd) {

      /* If command continued from previous line or command need more input,
         append line to previous command portion */
      if (command) {
	if (strlen(cmd) > 0) {
	  command = (char *)realloc(command, strlen(command) + strlen(cmd) + 1);
	  strcat(command, cmd);
	  free(cmd);
	} else {
	  free(cmd);
	  free(command);
	  command = 0;
	  free(prompt);
	  prompt = 0;
	  continue;
	}
      } else
	command = cmd;

      /* If line ends in hyphen it is a continuation. Go get rest of line */
      if ( strlen(command)>1 ) if (command[strlen(command) - 1] == '-') {
	command[strlen(command) - 1] = '\0';
	if (prompt)
	  free(prompt);
	prompt = 0;
	prompt = strdup("Continue: ");
	continue;
      }

      /* If not an empty command line */

      if (strlen(command) > 0) {
	char *prompt_more = 0;
	add_history(command);
	if (output) {
	  free(output);
	  output = 0;
	}
	status = mdsdcl_do_command_extra_args(command, &prompt_more, &error, &output, 0, 0);
	if (prompt_more != NULL) {
	  HIST_ENTRY *hist;
	  hist = remove_history(where_history());
	  if (hist) {
	    if (hist->line)
	      free((void *)hist->line);
	    free(hist);
	  }

	  command = strcat(realloc(command, strlen(command) + 2), " ");
	  if (prompt)
	    free(prompt);
	  prompt = strcpy(malloc(strlen(prompt_more) + 10), "_");
	  strcat(prompt, prompt_more);
	  strcat(prompt, ": ");
	  free(prompt_more);
	  continue;
	}
	if (error != NULL) {
	  fprintf(stderr, "%s", error);
	  fflush(stderr);
	  free(error);
	  error = 0;
	}
	if (prompt)
	  free(prompt);
	prompt = 0;
	if (status == MdsdclEXIT) {
	  free(command);
          status=0;
	  goto done;
	}
      }
      free(command);
      command = 0;
    } else {
      notDone = 0;
      printf("\n");
    }
  }
 done:
  if (output)
    free(output);
  if (prompt)
    free(prompt);
  if (error)
    free(error);
  history_file = mdsdclGetHistoryFile();
  if (history_file)
    write_history(history_file);
  free(history_file);
  return status;
}
