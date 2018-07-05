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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <mdsdescrip.h>
#include <mds_stdarg.h>
#define MAXEXPR 16384
#include <tdishr.h>

static char *history_file=NULL;

static void tdiputs(char *line);

static char *getExpression(FILE *f_in, char *command) {
  char line_in[MAXEXPR] = {0};
  char *ans;
  if ( f_in == NULL ) {
    ans = readline("");
  } else {
    ans = fgets(line_in, MAXEXPR-1, f_in);
    if (ans) {
      if (ans[strlen(ans)-1]=='\n')
	ans[strlen(ans)-1]='\0';
      ans=strdup(ans);
    }
  }
  if (ans && strlen(ans) > 0 && ans[strlen(ans)-1] == '\\' ) {
    ans[strlen(ans)-1]='\0';
    ans = getExpression(f_in, ans);
  }
  if (command) {
    char *newcmd=strcpy(malloc(strlen(command)+strlen(ans)+1),command);
    strcat(newcmd,ans);
    free(command);
    free(ans);
    ans = newcmd;
  }
  return ans;
}

int main(int argc, char **argv)
{
  FILE *f_in = NULL;
  int status = 1;
  static struct descriptor expr_dsc = { 0, DTYPE_T, CLASS_S, 0};
  static EMPTYXD(ans);
  static EMPTYXD(output_unit);
  static DESCRIPTOR(out_unit_stdout, "PUBLIC _OUTPUT_UNIT=*");
  static DESCRIPTOR(out_unit_other, "PUBLIC _OUTPUT_UNIT=FOPEN($,'w')");
  static DESCRIPTOR(reset_output_unit, "PUBLIC _OUTPUT_UNIT=$");
  char *command=NULL;
  static DESCRIPTOR(error_out, "WRITE($,DEBUG(13))");
  static DESCRIPTOR(clear_errors, "WRITE($,DECOMPILE($)),DEBUG(4)");
  if (argc > 1) {
    f_in = fopen(argv[1], "r");
    if (f_in == (FILE *) 0) {
      printf("Error opening input file /%s/\n", argv[1]);
      return 1;
    }
  } else {
#ifdef _WIN32
    char *home = getenv("USERPROFILE");
    char *sep = "\\";
#else
    char *home = getenv("HOME");
    char *sep = "/";
#endif
    if (home) {
      char *history = ".tditest";
      history_file = malloc(strlen(history) + strlen(home) + strlen(history) + 1);
      sprintf(history_file, "%s%s%s", home, sep, history);
      read_history(history_file);
    }
  }
  if (argc > 2) {
    struct descriptor out_d = { 0, DTYPE_T, CLASS_S, 0 };
    out_d.length = (unsigned short)strlen(argv[2]);
    out_d.pointer = argv[2];
    TdiExecute((struct descriptor *)&out_unit_other, &out_d, &output_unit MDS_END_ARG);
  } else
    TdiExecute((struct descriptor *)&out_unit_stdout, &output_unit MDS_END_ARG);
  while ((command=getExpression(f_in,NULL)) != NULL &&
	 strcasecmp(command,"exit") != 0 &&
	 strcasecmp(command,"quit") != 0) {
    int comment = command[0] == '!';
    if (!comment) {
      TdiExecute((struct descriptor *)&reset_output_unit, &output_unit, &ans MDS_END_ARG);
      if (f_in) tdiputs(command);
    }
    if (!comment) {
      expr_dsc.length = strlen(command);
      expr_dsc.pointer = command;
      status = TdiExecute((struct descriptor *)&expr_dsc, &ans MDS_END_ARG);
      if (status&1) {
	add_history(command);
	TdiExecute((struct descriptor *)&clear_errors, &output_unit, &ans, &ans MDS_END_ARG);
      }
      else
	TdiExecute((struct descriptor *)&error_out, &output_unit, &ans MDS_END_ARG);
    }
    if (command) {
      free(command);
      command=NULL;
    }
  }
  if (command) {
    free(command);
  }
  if (history_file) {
    write_history(history_file);
    free(history_file);
  }
  return !(status&1);
}

static void tdiputs(char *line)
{
  static EMPTYXD(ans);
  static DESCRIPTOR(write_it, "WRITE(_OUTPUT_UNIT,$)");
  struct descriptor line_d = { 0, DTYPE_T, CLASS_S, 0 };
  line_d.length = (unsigned short)strlen(line);
  line_d.pointer = line;
  if (line[line_d.length - 1] == '\n')
    line_d.length--;
  TdiExecute((struct descriptor *)&write_it, &line_d, &ans MDS_END_ARG);
  fflush(stdout);
}
