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
#if defined(__APPLE__) || defined(__MACH__)
#define _MACOSX;
#endif
#include <mdsdescrip.h>
#include <mds_stdarg.h>
#define MAXEXPR 16384
#include <tdishr.h>
#include <mdsshr.h>
#include <libroutines.h>
#include <ctype.h>

static char *history_file=NULL;

#define COM
#define OPC(name,NAME,...) #NAME,
const char* builtins[] = {
#include <opcbuiltins.h>
};
#undef OPC
#undef COM

static char ** character_names = NULL;
char *character_name_generator(const char *text, int state){
  // default to filepath completion if we are doing strings
#ifndef _MACOSX
  if (rl_completion_quote_character) return NULL;
#endif
  if (!character_names) {
    void* ctx = NULL;
    uint16_t i;
    char req[] = "MDS_PATH:*.*";
    mdsdsc_t rqd = { sizeof(req), DTYPE_T, CLASS_S, req };
    mdsdsc_t ans = { 0 , DTYPE_T, CLASS_D, NULL};
    size_t maxlen = 512, curlen = sizeof(builtins)/sizeof(void*);
    character_names = malloc(sizeof(void*)*maxlen);
    memcpy(character_names,builtins,sizeof(builtins));
    int status;
    for (;;) {
      status = LibFindFileRecurseCaseBlind(&rqd,&ans,&ctx);
      if (STATUS_NOT_OK || !ctx || ans.length<4) break;
      ans.pointer = realloc(ans.pointer, ans.length+1);ans.pointer[ans.length] = '\0';
      char *c,*p = ans.pointer;
      for (;(c=strchr(p,'/'));p=c+1);
#ifdef _WIN32
      for (;(c=strchr(p,'\\'));p=c+1);
#endif
      size_t mlen = strlen(p);
      for (i=0 ; i<mlen ; i++)
        p[i] = tolower(p[i]);
      if      (strcmp(&p[mlen-3],".py" )==0) mlen-= 3;
      else if (strcmp(&p[mlen-4],".fun")==0) mlen-= 4;
      else continue;
      p[mlen] = '\0';
      if (curlen>=maxlen) {
        maxlen *= 2;
        character_names = realloc(character_names,sizeof(void*)*maxlen);
      }
      character_names[curlen++] = strdup(p);
    }
    character_names = realloc(character_names,sizeof(void*)*(curlen+1));
    character_names[curlen] = NULL;
    LibFindFileEnd(&ctx);
    if (ans.pointer)
      free(ans.pointer);
  }
  static int list_index, len;
  char *name;
  if (!state) {
    list_index = 0;
    len = strlen(text);
  }
  // use case sensitivity to distinguish between builtins and tdi.funs
  while ((name = character_names[list_index++]))
    if (strncmp(name, text, len) == 0) {
      if (name[0] == '$') // constants like $PI
        return strdup(name);
      size_t len = strlen(name);
      char *match = memcpy(malloc(len+2),name,len);
      match[len]='(';match[len+1]='\0';
      return match;
    }
  return NULL;
}

char **character_name_completion(const char *text,
    int start __attribute__((__unused__)), int end __attribute__((__unused__))){
#ifndef _MACOSX
  rl_attempted_completion_over = !rl_completion_quote_character;
#endif
  return rl_completion_matches(text, character_name_generator);
}

static char *getExpression(FILE *f_in) {
  char buf[MAXEXPR] = {0};
  char  *ans = NULL;
  size_t alen = 0;
  int append;
  do {
    char  *next;
    size_t nlen;
    if ( f_in == NULL ) {
      next = readline("");
      if (next)
	nlen = strlen(next);
      else nlen = 0;
    } else {
      next = fgets(buf, MAXEXPR, f_in);
      if (next) {
	nlen = strlen(next);
	if (next[nlen-1]=='\n')
	  next[--nlen]='\0';
	next = strdup(next);
      } else nlen = 0;
    }
    if (nlen<=0) {
      if (ans) {
        if (next)
          free(next);
        next = ans;
      } else
        ans = next;
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
  rl_attempted_completion_function = character_name_completion;
  rl_completer_quote_characters = "\"'";
  rl_completer_word_break_characters = " ,;[{(+-*\\/^<>=:&|!?~";
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
  free(command);
  MdsFree1Dx(&ans,NULL);
  if (history_file) {
    write_history(history_file);
    free(history_file);
  }
  return !(status&1);
}
