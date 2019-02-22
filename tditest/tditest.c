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


static char *getExpression(FILE *f_in) {
  char line_in[MAXEXPR] = {0};
  char  *ans = NULL;
  size_t alen = 0;
  int append;
  do {
    char  *next;
    size_t nlen;
    if ( f_in == NULL ) {
      next = readline("");
      nlen = strlen(next);
    } else {
      next = fgets(line_in, MAXEXPR-1, f_in);
      if (next) {
	nlen = strlen(next);
	if (next[nlen-1]=='\n')
	  next[--nlen]='\0';
	next = strdup(next);
      } else nlen = 0;
    }
    if (nlen<=0) {
      if (next) free(next);
      next = ans;
      break;
    }
    append = next[nlen-1] == '\\';
    if (append) next[--nlen]='\0';
    if (ans) {
      if (nlen>0) {
	ans = realloc(ans,alen+nlen+1);
	memcpy(ans+alen,next,nlen+1);
	alen = alen + nlen;
      }
      free(next);
      next = ans;
    } else {
      ans = next;
      alen = nlen;
    }
  } while (append);
  return ans;
}

int main(int argc, char **argv)
{
  FILE *f_in = NULL;
  FILE *f_out= NULL;
  int status = 1;
  struct descriptor expr_dsc = { 0, DTYPE_T, CLASS_S, 0};
  EMPTYXD(ans);
  char *command=NULL;
  DESCRIPTOR(error_out, "WRITE($,DEBUG(13))");
  DESCRIPTOR(clear_errors, "WRITE($,DECOMPILE($)),DEBUG(4)");
  if (argc > 1) {
    f_in = fopen(argv[1], "r");
    if (!f_in) {
      printf("Error opening input file /%s/\n", argv[1]);
      exit(1);
    }
  } else {
#ifdef _WIN32
    char *home = getenv("USERPROFILE");
#define S_SEP_S "%s\\%s"
#else
    char *home = getenv("HOME");
#define S_SEP_S "%s/%s"
#endif
    if (home) {
      char *history = ".tditest";
      history_file = malloc(strlen(history) + strlen(home) + 2);
      sprintf(history_file, S_SEP_S, home, history);
      read_history(history_file);
    }
  }
  mdsdsc_t output_unit = {0, 0, CLASS_S, 0};
  if (argc > 2) {
    f_out = fopen(argv[2],"w");
    if (!f_out) {
      printf("Error opening input file /%s/\n", argv[2]);
      exit(1);
    }
    output_unit.length  = sizeof(void*);
    output_unit.dtype   = DTYPE_POINTER;
    output_unit.pointer = (char*)&f_out;
  } else
    f_out = stdout;
  while ((command=getExpression(f_in))
      && strcasecmp(command,"exit") != 0
      && strcasecmp(command,"quit") != 0   ) {
    int comment = command[0] == '!';
    if (!comment) {
      if (f_in) {
        fprintf(f_out,"%s\n",command);
        fflush(f_out);
      }
    }
    if (!comment) {
      expr_dsc.length = strlen(command);
      expr_dsc.pointer = command;
      status = TdiExecute((struct descriptor *)&expr_dsc, &ans MDS_END_ARG);
      add_history(command);
      if (status&1)
	TdiExecute((struct descriptor *)&clear_errors, &output_unit, &ans, &ans MDS_END_ARG);
      else
	TdiExecute((struct descriptor *)&error_out, &output_unit, &ans MDS_END_ARG);
      fflush(f_out);
    }
    free(command);
  }
  if (command) free(command);
  if (history_file) {
    write_history(history_file);
    free(history_file);
  }
  return !(status&1);
}
