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
#include <stdio.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#if defined(__APPLE__) || defined(__MACH__)
#define _MACOSX
#endif
#include <mds_stdarg.h>
#include <mdsdescrip.h>
#define MAXEXPR 16384
#include <ctype.h>
#include <libroutines.h>
#include <mdsshr.h>
#include <tdishr.h>

static char *history_file = NULL;

#define COM
#define OPC(name, NAME, ...) #NAME,
const char *builtins[] = {
#include <opcbuiltins.h>
};
#undef OPC
#undef COM

static char **character_names = NULL;
char *character_name_generator(const char *text, int state)
{
  // default to filepath completion if we are doing strings
#ifndef _MACOSX
  if (rl_completion_quote_character)
    return NULL;
#endif
  if (!character_names)
  {
    void *ctx = NULL;
    uint16_t i;
    char req[] = "MDS_PATH:*.*";
    mdsdsc_t rqd = {sizeof(req), DTYPE_T, CLASS_S, req};
    mdsdsc_t ans = {0, DTYPE_T, CLASS_D, NULL};
    size_t maxlen = 512, curlen = sizeof(builtins) / sizeof(void *);
    character_names = malloc(sizeof(void *) * maxlen);
    memcpy(character_names, builtins, sizeof(builtins));
    int status;
    for (;;)
    {
      status = LibFindFileRecurseCaseBlind(&rqd, &ans, &ctx);
      if (STATUS_NOT_OK || !ctx || ans.length < 4)
        break;
      ans.pointer = realloc(ans.pointer, ans.length + 1);
      ans.pointer[ans.length] = '\0';
      char *c, *p = ans.pointer;
      for (; (c = strchr(p, '/')); p = c + 1)
        ;
#ifdef _WIN32
      for (; (c = strchr(p, '\\')); p = c + 1)
        ;
#endif
      size_t mlen = strlen(p);
      for (i = 0; i < mlen; i++)
        p[i] = tolower(p[i]);
      if (strcmp(&p[mlen - 3], ".py") == 0)
        mlen -= 3;
      else if (strcmp(&p[mlen - 4], ".fun") == 0)
        mlen -= 4;
      else
        continue;
      p[mlen] = '\0';
      if (curlen >= maxlen)
      {
        maxlen *= 2;
        character_names = realloc(character_names, sizeof(void *) * maxlen);
      }
      character_names[curlen++] = strdup(p);
    }
    character_names = realloc(character_names, sizeof(void *) * (curlen + 1));
    character_names[curlen] = NULL;
    LibFindFileEnd(&ctx);
    free(ans.pointer);
  }
  static int list_index, len;
  char *name;
  if (!state)
  {
    list_index = 0;
    len = strlen(text);
  }
  // use case sensitivity to distinguish between builtins and tdi.funs
  while ((name = character_names[list_index++]))
    if (strncmp(name, text, len) == 0)
    {
      if (name[0] == '$') // constants like $PI
        return strdup(name);
      size_t len = strlen(name);
      char *match = memcpy(malloc(len + 2), name, len);
      match[len] = '(';
      match[len + 1] = '\0';
      return match;
    }
  return NULL;
}

char **character_name_completion(const char *text,
                                 int start __attribute__((__unused__)),
                                 int end __attribute__((__unused__)))
{
#ifndef _MACOSX
  rl_attempted_completion_over = !rl_completion_quote_character;
#endif
  return rl_completion_matches(text, character_name_generator);
}

static void keyboard_interrupt(int signo __attribute__((__unused__)))
{
  fprintf(stderr, "KeyboardInterrupt\n");
}
#ifdef _WIN32
static inline void set_readline_handlers()
{
  signal(SIGINT, SIG_IGN);
}
static inline void set_execute_handlers()
{
  signal(SIGINT, keyboard_interrupt);
}
#else // _WIN32
static struct sigaction act;
#ifdef HAVE_RL_SET_SIGNALS
static inline void set_readline_handlers()
{
  rl_catch_signals = 1;
  rl_set_signals();
}
static inline void set_execute_handlers() { rl_clear_signals(); }
#else // HAVE_RL_SET_SIGNALS
#ifdef _MACOSX
#define _rl_sigint SIG_IGN
#else  // _MACOSX
static void _rl_sigint(int signo __attribute__((__unused__)))
{
  // reset readline line buffer and state before printing new prompt
  rl_free_line_state();
  rl_cleanup_after_signal();
  RL_UNSETSTATE(RL_STATE_ISEARCH | RL_STATE_NSEARCH | RL_STATE_VIMOTION |
                RL_STATE_NUMERICARG | RL_STATE_MULTIKEY);
  rl_line_buffer[rl_point = rl_end = rl_mark = 0] = 0;
  printf("\n");
}
#endif // !_MACOSX
static inline void set_readline_handlers()
{
  act.sa_handler = _rl_sigint;
  sigaction(SIGINT, &act, NULL);
}
static inline void set_execute_handlers()
{
  act.sa_handler = keyboard_interrupt;
  sigaction(SIGINT, &act, NULL);
}
#endif // !HAVE_RL_SET_SIGNALS
#endif // !_WIN32

static char *getExpression(FILE *f_in)
{
  char buf[MAXEXPR] = {0};
  char *ans = NULL;
  size_t alen = 0;
  int append;
  if (f_in == NULL)
    set_readline_handlers();
  do
  {
    char *next;
    size_t nlen;
    if (f_in == NULL)
    {
      next = readline("");
      if (next)
        nlen = strlen(next);
      else
        nlen = 0;
    }
    else
    {
      next = fgets(buf, MAXEXPR, f_in);
      if (next)
      {
        nlen = strlen(next);
        if (next[nlen - 1] == '\n')
          next[--nlen] = '\0';
        next = strdup(next);
      }
      else
        nlen = 0;
    }
    if (nlen <= 0)
    {
      if (ans)
      {
        free(next);
        next = ans;
      }
      else
        ans = next;
      break;
    }
    append = next[nlen - 1] == '\\';
    if (append)
      next[--nlen] = '\0';
    if (ans)
    {
      if (nlen > 0)
      {
        ans = realloc(ans, alen + nlen + 1);
        memcpy(ans + alen, next, nlen + 1);
        alen = alen + nlen;
      }
      free(next);
      next = ans;
    }
    else
    {
      ans = next;
      alen = nlen;
    }
  } while (append);
  return ans;
}

static void print_help(int error)
{
  FILE *file = error ? stderr : stdout;
  fprintf(
      file,
      "tditest help:\n"
      "  -h, --help		: print this help and exit\n"
      "  -q, --quiet		: suppress output of errors and results\n"
      "  -s, --server <server>	: evaluate expression on server "
      "(experimental)\n"
      "  -v, --verbose		: print this help and exit\n");
  exit(error);
}

static inline char *get_arg(const int argc, char **const args, const int inext,
                            const char *const arg)
{
  if (argc <= inext)
  {
    fprintf(stderr, "Error %s requires a value but none given\n", arg);
    print_help(2);
  }
  return args[inext];
}
int main(int argc, char **argv)
{
  FILE *f_in = NULL;
  FILE *f_out = NULL;
  int status = 1;
  rl_attempted_completion_function = character_name_completion;
  rl_completer_quote_characters = "\"'";
  rl_completer_word_break_characters = " ,;[{(+-*\\/^<>=:&|!?~";
  mdsdsc_t expr_dsc = {0, DTYPE_T, CLASS_S, 0};
  EMPTYXD(ans);
  char *buf, *command;
  char error_out_c[64], clear_errors_c[64];
  DESCRIPTOR(mdsconnect, "MDSCONNECT($)");
  DESCRIPTOR(mdsvalue, "MDSVALUE('DECOMPILE(`EXECUTE($))',$)");
  char *server = NULL, *script = NULL, *output = NULL;
  int quiet = FALSE;
  int verbose = FALSE;
  int i;
  for (i = 1; argc > i; i++)
  {
    const char *arg = argv[i];
    if (*arg == '-')
    {
      arg++;
      if (*arg == '-') // --long word
      {
        arg++;
        if (strcmp(argv[i], "help") == 0)
          print_help(0);
        else if (strcmp(argv[i], "output") == 0)
          output = get_arg(argc, argv, ++i, arg - 2);
        else if (strcmp(argv[i], "quiet") == 0)
          quiet = TRUE;
        else if (strcmp(argv[i], "verbose") == 0)
          verbose = TRUE;
        else if (strcmp(argv[i], "server") == 0)
          server = get_arg(argc, argv, ++i, arg - 2);
        else
        {
          fprintf(stderr, "Unknown parameter --%s:\n", arg);
          print_help(1);
        }
      }
      else
      {
        for (; *arg; arg++) // -short letter
        {
          switch (*arg)
          {
          case 'h':
            print_help(0);
            break;
          case 'o':
            output = get_arg(argc, argv, ++i, "-o");
            break;
          case 'q':
            quiet = TRUE;
            break;
          case 'v':
            verbose = TRUE;
            break;
          case 's':
            server = get_arg(argc, argv, ++i, "-s");
            break;
          default:
          {
            fprintf(stderr, "Unknown parameter -%c:\n", *arg);
            print_help(1);
          }
          }
        }
      }
    }
    else
    { // input
      script = argv[i];
      break;
    }
  }
  mdsdsc_t output_unit = {0, 0, CLASS_S, 0};
  output_unit.length = sizeof(void *);
  output_unit.dtype = DTYPE_POINTER;
  output_unit.pointer = (char *)&f_out;
  if (output)
  {
    f_out = fopen(output, "w");
    if (!f_out)
    {
      printf("Error opening log file '%s'\n", output);
      exit(1);
    }
    if (verbose)
      fprintf(stderr, "Writing output to file '%s'\n", output);
  }
  else
    f_out = stdout;
  if (server)
  {
    DESCRIPTOR_FROM_CSTRING(server_dsc, server);
    set_execute_handlers();
    status = TdiExecute((mdsdsc_t *)&mdsconnect, (mdsdsc_t *)&server_dsc,
                        &ans MDS_END_ARG);
    if (STATUS_OK)
      status = *(int *)ans.pointer->pointer;
    if (STATUS_NOT_OK)
    {
      fprintf(stderr, "Error connecting to server '%s'\n", server);
      exit(1);
    }
    if (verbose)
      fprintf(stderr, "Connected to server '%s'\n", server);
    set_readline_handlers();
    strcpy(error_out_c, "WRITE($,MDSVALUE('DEBUG(13)'))");
    strcpy(clear_errors_c, "WRITE($,$),MDSVALUE('DEBUG(4)')");
  }
  else
  {
    strcpy(error_out_c, "WRITE($,DEBUG(13))");
    strcpy(clear_errors_c, "WRITE($,DECOMPILE($)),DEBUG(4)");
  }
  DESCRIPTOR_FROM_CSTRING(error_out, error_out_c);
  DESCRIPTOR_FROM_CSTRING(clear_errors, clear_errors_c);
  if (script)
  {
    f_in = fopen(script, "r");
    if (!f_in)
    {
      if (verbose)
        printf("Error opening input file '%s'\n", script);
      exit(1);
    }
    if (verbose)
      fprintf(stderr, "Executing script '%s'\n", script);
  }
  else
  {
#ifdef _WIN32
    char *home = getenv("USERPROFILE");
#define S_SEP_S "%s\\%s"
#else
    char *home = getenv("HOME");
#define S_SEP_S "%s/%s"
#endif
    if (home)
    {
      char *history = ".tditest";
      history_file = malloc(strlen(history) + strlen(home) + 2);
      sprintf(history_file, S_SEP_S, home, history);
      if (verbose)
        fprintf(stderr, "Reading from history file '%s'\n", history_file);
      read_history(history_file);
    }
  }
  // initialize SIGINT handler
#ifdef _WIN32
  signal(SIGINT, keyboard_interrupt);
#else
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  act.sa_handler = keyboard_interrupt;
  sigaction(SIGINT, &act, NULL);
#endif
  if (argc > i && !server)
  {
    const int i0 = i;
    char buf[16];
    set_execute_handlers();
    mdsdsc_t argdsc = {0, DTYPE_T, CLASS_S, 0};
    expr_dsc.pointer = buf;
    for (; argc > i; i++)
    {
      expr_dsc.length = sprintf(expr_dsc.pointer, "_$%d=$;*", i - i0);
      argdsc.pointer = argv[i];
      argdsc.length = strlen(argv[i]);
      status = TdiExecute((mdsdsc_t *)&expr_dsc, &argdsc, &ans MDS_END_ARG);
    }
  }
  set_readline_handlers();
  while ((buf = command = getExpression(f_in)) &&
         strcasecmp(command, "exit") != 0 && strcasecmp(command, "quit") != 0)
  {
    int comment = command[0] == '#';
    if (!comment)
    {
      if (f_in)
      {
        if (command[0] == '@')
          command++;
        else if (!quiet)
        {
          fprintf(f_out, "%s\n", command);
          fflush(f_out);
        }
      }
      set_execute_handlers();
      expr_dsc.length = strlen(command);
      expr_dsc.pointer = command;
      if (server)
      {
        status = TdiExecute((mdsdsc_t *)&mdsvalue, (mdsdsc_t *)&expr_dsc,
                            &ans MDS_END_ARG);
        if (STATUS_OK && !ans.pointer)
          status = 0;
      }
      else
        status = TdiExecute((mdsdsc_t *)&expr_dsc, &ans MDS_END_ARG);
      if (!quiet)
      {
        if (STATUS_OK)
          TdiExecute((mdsdsc_t *)&clear_errors, &output_unit, &ans,
                     &ans MDS_END_ARG);
        else
          TdiExecute((mdsdsc_t *)&error_out, &output_unit, &ans MDS_END_ARG);
        fflush(f_out);
      }
      set_readline_handlers();
      add_history(command);
    }
    free(buf);
  }
  MdsFree1Dx(&ans, NULL);
  if (history_file)
  {
    if (verbose)
      fprintf(stderr, "Writing to history file '%s'\n", history_file);
    write_history(history_file);
    free(history_file);
  }
  return STATUS_NOT_OK;
}
