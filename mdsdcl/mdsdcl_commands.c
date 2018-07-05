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
#include <mdsplus/mdsconfig.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mdsdcl_messages.h>
#include <sys/time.h>
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#else
#include <time.h>
#endif
#include <mdsdescrip.h>
#include <unistd.h>
#include <mdsshr.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <pthread_port.h>
#include <dcl.h>
#include "dcl_p.h"
#include "mdsdclthreadsafe.h"

#ifdef _WIN32
#define setenv(name,value,overwrite) _putenv_s(name,value)
#endif

typedef struct dclMacroList {
  char *name;			/*!<  macro name */
  int lines;			/*!<  number of lines in macro */
  char **cmds;			/*!<  cmds in macro */
  struct dclMacroList *next;
} dclMacroList, *dclMacroListPtr;

static int STOP_ON_FAIL = 1;

/**********************************************************************
* mdsdcl_commands.c --
*
* mdsdcl command execution routines
*
* Includes:
*    mdsdcl_exit()
*    mdsdcl_init_timer()
*    mdsdcl_show_timer()
*    mdsdcl_set_prompt()
*    mdsdcl_type()
*    mdsdcl_set_hyphen()
*    mdsdcl_set_verify()
*    mdsdcl_define_symbol()
*    mdsdcl_spawn()
*    mdsdcl_set_command()
*    mdsdcl_wait()
*    mdsdcl_help()
*
* History:
*  05-Nov-1997  TRG  Create.
*
************************************************************************/

	/****************************************************************
	 * mdsdcl_exit:
	 ****************************************************************/
EXPORT int mdsdcl_exit(void *ctx __attribute__ ((unused)),
		       char *error __attribute__ ((unused)),
		       char *output __attribute__ ((unused)))
{
  return MdsdclEXIT;
}

	/****************************************************************
	 * mdsdcl_init_timer:
	 * mdsdcl_show_timer:

	 ****************************************************************/

#ifdef HAVE_SYS_RESOURCE_H
static struct rusage TIMER_START_USAGE;
#else
static clock_t cpu_start;
#endif
static struct timeval TIMER_START_TIME;

EXPORT int mdsdcl_init_timer(void *ctx __attribute__ ((unused)),
			     char *error __attribute__ ((unused)),
			     char *output __attribute__ ((unused)))
{
  gettimeofday(&TIMER_START_TIME, 0);
#ifdef HAVE_GETRUSAGE
  getrusage(RUSAGE_SELF, &TIMER_START_USAGE);
#else
  cpu_start = clock();
#endif
  return MdsdclSUCCESS;
}

EXPORT int mdsdcl_show_timer(void *ctx __attribute__ ((unused)), char **error, char **output __attribute__ ((unused)))
{
  struct timeval TIMER_NOW_TIME;
  long int esec = 0;
  long int emsec = 0;
#ifdef HAVE_SYS_RESOURCE_H
  time_t usec = 0, ssec = 0;
  suseconds_t umsec = 0, smsec = 0;
  long int sf = 0, hf = 0;
  struct rusage TIMER_NOW_USAGE;
#else
  double usec_d;
#endif

  gettimeofday(&TIMER_NOW_TIME, 0);
  if (TIMER_NOW_TIME.tv_usec < TIMER_START_TIME.tv_usec)
    --TIMER_NOW_TIME.tv_sec, TIMER_NOW_TIME.tv_usec += 1000000;
  esec = TIMER_NOW_TIME.tv_sec - TIMER_START_TIME.tv_sec;
  emsec = (TIMER_NOW_TIME.tv_usec - TIMER_START_TIME.tv_usec) / 10000;
  *error = malloc(100);
#ifdef HAVE_GETRUSAGE
  getrusage(RUSAGE_SELF, &TIMER_NOW_USAGE);
  if (TIMER_NOW_USAGE.ru_utime.tv_usec < TIMER_START_USAGE.ru_utime.tv_usec)
    --TIMER_NOW_USAGE.ru_utime.tv_sec, TIMER_NOW_USAGE.ru_utime.tv_usec += 1000000;
  usec = TIMER_NOW_USAGE.ru_utime.tv_sec - TIMER_START_USAGE.ru_utime.tv_sec;
  umsec = (TIMER_NOW_USAGE.ru_utime.tv_usec - TIMER_START_USAGE.ru_utime.tv_usec) / 10000;
  if (TIMER_NOW_USAGE.ru_stime.tv_usec < TIMER_START_USAGE.ru_stime.tv_usec)
    --TIMER_NOW_USAGE.ru_stime.tv_sec, TIMER_NOW_USAGE.ru_stime.tv_usec += 1000000;
  ssec = TIMER_NOW_USAGE.ru_stime.tv_sec - TIMER_START_USAGE.ru_stime.tv_sec;
  smsec = (TIMER_NOW_USAGE.ru_stime.tv_usec - TIMER_START_USAGE.ru_stime.tv_usec) / 10000;
  sf = TIMER_NOW_USAGE.ru_minflt - TIMER_START_USAGE.ru_minflt;
  hf = TIMER_NOW_USAGE.ru_majflt - TIMER_START_USAGE.ru_majflt;
  sprintf(*error, "elapsed=%ld.%02ld user=%ld.%02ld sys=%ld.%02ld sf=%ld hf=%ld\n",
	  esec, emsec, usec, (long int)umsec, ssec, (long int)smsec, sf, hf);
#else
  usec_d = (double)(clock() - cpu_start) / (double)CLOCKS_PER_SEC;
  sprintf(*error, "elapsed=%ld.%02ld cpu=%g\n", esec, emsec, usec_d);
#endif
  return MdsdclSUCCESS;
}

	/**************************************************************
	 * mdsdcl_set_prompt:
	 **************************************************************/
EXPORT int mdsdcl_set_prompt(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  char *prompt = 0;
  int status;
  cli_get_value(ctx, "PROMPT", &prompt);
  if (prompt) {
    mdsdclSetPrompt(prompt);
    status = MdsdclSUCCESS;
    free(prompt);
  } else {
    status = MdsdclERROR;
    *error = strdup("No prompt specified\n");
  }
  return status;
}

	/****************************************************************
	 * mdsdcl_type:
	 ****************************************************************/
EXPORT int mdsdcl_type(void *ctx, char **error __attribute__ ((unused)), char **output)
{
  char *text = 0;
  cli_get_value(ctx, "P1", &text);
  if (text) {
    text = strcat(realloc(text, strlen(text) + 2), "\n");
    *output = text;
  } else {
    *output = strdup("\n");
  }
  return MdsdclSUCCESS;
}

	/****************************************************************
         * mdsdcl_set_hyphen:
         ****************************************************************/

EXPORT int mdsdcl_set_hyphen(void *ctx __attribute__ ((unused)), char **error, char **output __attribute__ ((unused)))
{
  *error = strdup("SET HYPHEN obsolete, no longer supported\n");
  return MdsdclSUCCESS;
}

	/****************************************************************
	 * mdsdcl_set_verify:
	 ****************************************************************/

static int MDSDCL_VERIFY = 0;

int mdsdclVerify()
{
  return MDSDCL_VERIFY;
}

EXPORT int mdsdcl_set_verify(void *ctx, char **error __attribute__ ((unused)), char **output __attribute__ ((unused)))
{
  char *verify = 0;
  cli_get_value(ctx, "P1", &verify);
  MDSDCL_VERIFY = verify && (toupper(verify[0]) == 'V');
  if (verify)
    free(verify);
  return MdsdclSUCCESS;
}

	/****************************************************************
	 * mdsdcl_define_symbol:
	 ****************************************************************/
EXPORT int mdsdcl_define_symbol(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  char *name=NULL, *value=NULL;
  int status = cli_get_value(ctx, "SYMBOL", &name);
  if STATUS_NOT_OK {
    *error = malloc(100);
    sprintf(*error, "Error: problem getting symbol name");
    goto done;
  }
  status = cli_get_value(ctx, "VALUE", &value);
  if STATUS_NOT_OK {
    *error = malloc(100);
    sprintf(*error, "Error:  problem getting value for symbol");
    goto done;
  }
  status = setenv(name,value,1);
  if (status) {
    *error = malloc(100);
    sprintf(*error, "putenv returned %d. Environment variable not set.", status);
    perror("error from putenv");
    status = MdsdclERROR;
  } else
    status = MdsdclSUCCESS;
 done:
  if (name)
    free(name);
  if (value)
    free(value);
  return (status);
}

EXPORT int mdsdcl_env(void *ctx, char **error, char **output __attribute__ ((unused))){
  char *name, *value;
  int status = cli_get_value(ctx, "P1", &name);
  for (value=name ; value[0] && value[0]!='=' ; value++ );
  if (value[0]=='\0') {
      value = getenv(name);
      if (value){
         fprintf(stderr, "'%s'='%s'\n",name,value);
         //free(value);
      } else
         fprintf(stderr, "'%s'= not defined\n",name);
  } else {
    value[0] = '\0'; value++;
    fprintf(stderr, "'%s'='%s'\n",name,value);
    status = setenv(name,value,1);
    if (status) {
      *error = malloc(100);
      perror("error from putenv");
      sprintf(*error, "Attempting putenv(\"%s\")\n", value);
      status = MdsdclERROR;
    } else
      status = MdsdclSUCCESS;
  }
  if (name)
    free(name);
  return (status);
}

extern int LibSpawn();

	/**************************************************************
	 * mdsdcl_spawn:
	 **************************************************************/
EXPORT int mdsdcl_spawn(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int notifyFlag;
  int waitFlag;
  int status;
  char *cmd = 0;
  struct descriptor cmd_dsc = { 0, DTYPE_T, CLASS_S, 0 };
  cli_get_value(ctx, "COMMAND", &cmd);
  notifyFlag = cli_present(ctx, "NOTIFY") & 1;
  waitFlag = cli_present(ctx, "WAIT") & 1;
  if (cmd) {
    cmd_dsc.length = strlen(cmd);
    cmd_dsc.pointer = cmd;
  }
  status = LibSpawn(&cmd_dsc, waitFlag, notifyFlag);
  if (cmd)
    free(cmd);
  if (status) {
    *error = malloc(100);
    sprintf(*error, "Spawn returned: %d\n", status);
    return MdsdclERROR;
  }
  return  MdsdclSUCCESS;
}

static char *history_file = 0;

EXPORT char *mdsdclGetHistoryFile()
{
  return history_file;
}

/***********************************************************************
* MDSDCL_SET_COMMAND.C --
*
* Include new command table in mdsdcl.
*
* History:
*  13-Feb-1998  TRG  Allow user to specify only the facility (e.g. "tcl"),
*                     rather than the entire name "tcl_commands".
*  16-Jan-1998  TRG  Create.  Ported from orginal mds code.
*
************************************************************************/

	/****************************************************************
	 * mdsdcl_set_command:
	 ****************************************************************/

EXPORT int mdsdcl_set_command(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int status;
  char *table = 0;
  char *prompt = 0;
  char *def_file = 0;
  char *history = 0;
  cli_get_value(ctx, "TABLE", &table);
  if (table) {
    status = mdsdclAddCommands(table, error) == 0;
    free(table);
  } else {
    *error = strdup("Error: command table not specified\n");
    status = MdsdclERROR;
  }
  if STATUS_OK {
  /*------------------------------------------------------
   * Check for other qualifiers ...
   *-----------------------------------------------------*/
    cli_get_value(ctx, "PROMPT", &prompt);
    if (prompt) {
      mdsdclSetPrompt(prompt);
      free(prompt);
    }
    cli_get_value(ctx, "DEF_FILE", &def_file);
    if (def_file) {
      if (def_file[0] == '*') {
	char *tmp = strdup(def_file + 1);
	free(def_file);
	def_file = tmp;
      }
      mdsdclSetDefFile(def_file);
      free(def_file);
    }
    cli_get_value(ctx, "HISTORY", &history);
    if (history) {
#ifdef _WIN32
      char *home = getenv("USERPROFILE");
      char *sep = "\\";
#else
      char *home = getenv("HOME");
      char *sep = "/";
#endif
      if (history_file)
	free(history_file);
      if (home) {
	history_file = malloc(strlen(history) + strlen(home) + 100);
	sprintf(history_file, "%s%s%s", home, sep, history);
	free(history);
	read_history(history_file);
      }
    }
  }

  return status;
}

	/**************************************************************
	 * mdsdcl_wait:
	 **************************************************************/
EXPORT int mdsdcl_wait(void *ctx, char **error, char **output __attribute__ ((unused)))
{				/* Return:  status                        */
  char *deltatime = 0;
  int status = MdsdclSUCCESS;
  int seconds;
  cli_get_value(ctx, "DELTA_TIME", &deltatime);
  if (deltatime == NULL) {
    *error = strdup("No wait time specified\n");
    return MdsdclERROR;
  }
  seconds = mdsdclDeltatimeToSeconds(deltatime);
  if (seconds == -1) {
    *error = malloc(strlen(deltatime) + 100);
    sprintf(*error, "Error: Invalid wait time specified - %s\n"
	    "Specify: [ndays] [nhours:][nminutes:]nseconds\n", deltatime);
    status = MdsdclERROR;
  } else
    sleep(seconds);
  free(deltatime);
  return status;
}

EXPORT int mdsdcl_help(void *ctx, char **error, char **output)
{
  char *p1 = 0;
  int status;
  cli_get_value(ctx, "P1", &p1);
  status = mdsdcl_do_help(p1, error, output);
  if (p1)
    free(p1);
  return status;
}

static dclMacroListPtr MLIST = 0;

static dclMacroListPtr mdsdclNewMacro(char *name)
{
  dclMacroListPtr l, prev = 0;
  size_t i;
  for (i = 0; i < strlen(name); i++)
    name[i] = toupper(name[i]);
  for (l = MLIST; l; prev = l, l = l->next) {
    if (strcasecmp(name, l->name) == 0)
      break;
  }
  if (l) {
    int i;
    for (i = 0; i < l->lines; i++)
      free(l->cmds[i]);
    l->lines = 0;
  } else {
    l = malloc(sizeof(dclMacroList));
    l->name = name;
    l->lines = 0;
    l->cmds = malloc(sizeof(char *));
    l->next = 0;
    if (prev)
      prev->next = l;
    else
      MLIST = l;
  }
  return l;
}

EXPORT int mdsdcl_define(void *ctx, char **error, char **output __attribute__ ((unused)), char *(*getline) (), void *getlineinfo)
{
  char *name = 0;
  char *line;
  dclMacroListPtr macro;
  cli_get_value(ctx, "p1", &name); /*** do not free name as it is used in macro definition ***/
  if (name == NULL) {
    *error = strdup("No macro name specified\n");
    return MdsdclIVVERB;
  }
  macro = mdsdclNewMacro(name);
  while ((line = (getline ? getline(getlineinfo) : readline("DEFMAC> "))) && (strlen(line)) > 0) {
    macro->cmds = realloc(macro->cmds, sizeof(char *) * (macro->lines + 1));
    macro->cmds[macro->lines++] = line;
    if (getline == NULL)
      add_history(line);
    line = 0;
  }
  if (line)
    free(line);
  return MdsdclSUCCESS;
}

static void mdsdcl_print_macro(dclMacroListPtr l, int full, char **output)
{
  if (*output == 0)
    *output = strdup("");
  *output = strcat(realloc(*output, strlen(*output) + strlen(l->name) + 5), "\n");
  strcat(*output, l->name);
  strcat(*output, "\n");
  if (full == MdsdclPRESENT) {
    int i;
    for (i = 0; i < l->lines; i++) {
      *output = strcat(realloc(*output, strlen(*output) + strlen(l->cmds[i]) + 5), " ");
      strcat(*output, l->cmds[i]);
      strcat(*output, "\n");
    }
  }
}

EXPORT int mdsdcl_show_macro(void *ctx, char **error, char **output)
{
  char *name = 0;
  int full = cli_present(ctx, "FULL");
  dclMacroListPtr l;
  cli_get_value(ctx, "P2", &name);
  if (name && (strlen(name) > 0)) {
    for (l = MLIST; l; l = l->next) {
      if (strcasecmp(name, l->name) == 0)
	break;
    }
    if (l)
      mdsdcl_print_macro(l, full, output);
    else {
      *error = malloc(100 + strlen(name));
      sprintf(*error, "Error finding macro called %s\n", name);
    }
  } else {
    for (l = MLIST; l; l = l->next)
      mdsdcl_print_macro(l, full, output);
  }
  if (name)
    free(name);
  return MdsdclSUCCESS;
}

static void mdsdclSubstitute(char **cmd, char *p1, char *p2, char *p3, char *p4, char *p5, char *p6,
			     char *p7)
{
  char *ps[7] = { p1, p2, p3, p4, p5, p6, p7 };
  int p;
  for (p = 0; p < 7; p++) {
    char param[10];
    size_t i;
    sprintf(param, "'p%d'", p + 1);
    for (i = 0; (i + 3) < strlen(*cmd); i++) {
      if (strncasecmp((*cmd) + i, param, strlen(param)) == 0) {
	if (ps[p] && (strlen(ps[p]) > 0)) {
	  char *newcmd = malloc(strlen(*cmd) + strlen(ps[p]));
	  (*cmd)[i] = 0;
	  strcpy(newcmd, *cmd);
	  strcat(newcmd, ps[p]);
	  strcat(newcmd, (*cmd) + i + 4);
	  free(*cmd);
	  *cmd = newcmd;
	} else {
	  memmove(*cmd + i, (*cmd) + i + 4, strlen(*cmd) - (i + 4));
	  (*cmd)[strlen(*cmd) - 4] = 0;
	}
      }
    }
  }
}

typedef struct getNextLineInfo {
  int *idx;
  dclMacroListPtr m;
} getNextLineInfo, *getNextLineInfoPtr;

static char *getNextLine(getNextLineInfoPtr info)
{
  *info->idx = *info->idx + 1;
  if (*info->idx < info->m->lines)
    return strdup(info->m->cmds[*info->idx]);
  else
    return strdup("");
}

EXPORT int mdsdcl_do_macro(void *ctx, char **error, char **output)
{
  char *name = 0;
  char *defname = 0;
  char *times_s = 0;
  char *p1 = 0, *p2 = 0, *p3 = 0, *p4 = 0, *p5 = 0, *p6 = 0, *p7 = 0;
  int status = MdsdclSUCCESS;
  int indirect = cli_present(ctx, "INDIRECT") == MdsdclPRESENT;
  dclMacroListPtr l = 0;
  cli_get_value(ctx, "name", &name);
  cli_get_value(ctx, "repeat", &times_s);
  cli_get_value(ctx, "p1", &p1);
  cli_get_value(ctx, "p2", &p2);
  cli_get_value(ctx, "p3", &p3);
  cli_get_value(ctx, "p4", &p4);
  cli_get_value(ctx, "p5", &p5);
  cli_get_value(ctx, "p6", &p6);
  cli_get_value(ctx, "p7", &p7);
  //void *oldOutputRtn = mdsdclSetOutputRtn(0);
  if (indirect) {
    FILE *f = NULL;
    char line[4096];
    GET_THREADSTATIC_P;
    if (DEF_FILE
     &&	(strlen(DEF_FILE) > 0)
     &&!( (strlen(name) > strlen(DEF_FILE))
       && (strcmp(name + strlen(name) - strlen(DEF_FILE), DEF_FILE) == 0))) {
      defname = strdup(name);
      defname = strcat(realloc(defname, strlen(defname) + strlen(DEF_FILE) + 1), DEF_FILE);
      f = fopen(defname, "r");
    }
    if (f == NULL)
      f = fopen(name, "r");
    if (f == NULL) {
      if (defname) {
	*error = malloc(strlen(name) + strlen(defname) + 100);
	sprintf(*error, "Error: Unable to open either %s or %s\n", defname, name);
      } else {
	*error = malloc(strlen(name) + 100);
	sprintf(*error, "Error: Unable to open %s\n", name);
      }
    }
    if (f != NULL) {
      l = memset(malloc(sizeof(dclMacroList)), 0, sizeof(dclMacroList));
      while (fgets(line, sizeof(line), f)) {
	l->lines++;
	l->cmds = realloc(l->cmds, l->lines * sizeof(char *));
	if (line[strlen(line) - 1] == '\n')
	  line[strlen(line) - 1] = 0;
	l->cmds[l->lines - 1] = strdup(line);
      }
      fclose(f);
    }
  } else {
    for (l = MLIST; l; l = l->next) {
      if (strcasecmp(name, l->name) == 0)
	break;
    }
    if (l == NULL) {
      *error = malloc(strlen(name) + 100);
      sprintf(*error, "Error: Macro %s is not defined\n", name);
      status = MdsdclERROR;
    }
  }
  if (l != NULL) {
    int times = 1;
    int time;
    int failed = 0;
    if (times_s) {
      times = atoi(times_s);
      free(times_s);
    }
    for (time = 0; (failed == 0) && (time < times); time++) {
      int i;
      for (i = 0; (failed == 0) && (i < l->lines); i++) {
	char *m_output = 0;
	char *m_error = 0;
	getNextLineInfo info = { &i, l };
	char *cmd;
	if (strlen(l->cmds[i]) == 0)
	  continue;
	cmd = strdup(l->cmds[i]);
	mdsdclSubstitute(&cmd, p1, p2, p3, p4, p5, p6, p7);
	status = mdsdcl_do_command_extra_args(cmd, 0, &m_error, &m_output, getNextLine, &info);
	free(cmd);
	if (m_error) {
	  if ((*error) == NULL)
	    *error = strdup("");
	  *error = strcat(realloc(*error, strlen(*error) + strlen(m_error) + 1), m_error);
	  free(m_error);
	  m_error = 0;
	}
	if (m_output) {
	  if ((*output) == NULL)
	    *output = strdup("");
	  *output = strcat(realloc(*output, strlen(*output) + strlen(m_output) + 1), m_output);
	  free(m_output);
	  m_output = 0;
	}
	if (STATUS_NOT_OK && STOP_ON_FAIL)
	  failed = 1;
      }
    }
    if (indirect) {
      int i;
      for (i = 0; i < l->lines; i++)
	free(l->cmds[i]);
      free(l->cmds);
      free(l);
    }
  }
  if (name)
    free(name);
  if (defname)
    free(defname);
  if (p1)
    free(p1);
  if (p2)
    free(p2);
  if (p3)
    free(p3);
  if (p4)
    free(p4);
  if (p5)
    free(p5);
  if (p6)
    free(p6);
  if (p7)
    free(p7);
  //mdsdclSetOutputRtn(oldOutputRtn);
  return status;
}

EXPORT int mdsdcl_delete_macro(void *ctx, char **error, char **output __attribute__ ((unused)))
{
  int status = MdsdclSUCCESS;
  char *name = 0;
  int all = cli_present(ctx, "ALL");
  cli_get_value(ctx, "name", &name);
  if (all & 1) {
    dclMacroListPtr l;
    for (l = MLIST, MLIST = 0; l;) {
      int i;
      dclMacroListPtr next = l->next;
      free(l->name);
      for (i = 0; i < l->lines; i++)
	free(l->cmds[i]);
      free(l->cmds);
      free(l);
      l = next;
    }
  } else if (name) {
    dclMacroListPtr l, prev = 0;
    for (l = MLIST; l; prev = l, l = l->next) {
      if (strcasecmp(name, l->name) == 0)
	break;
    }
    if (l) {
      int i;
      free(l->name);
      for (i = 0; i < l->lines; i++)
	free(l->cmds[i]);
      free(l->cmds);
      if (prev)
	prev->next = l->next;
      else
	MLIST = l->next;
      free(l);
    } else {
      *error = malloc(strlen(name) + 100);
      sprintf(*error, "Error: Macro not found - %s\n", name);
      status = MdsdclERROR;
    }
  } else {
    *error = strdup("Error: Either specify macro name or use the /ALL qualifier.\n");
    status = MdsdclERROR;
  }
  if (name)
    free(name);
  return status;
}

EXPORT int mdsdcl_set_stoponfail(void *ctx, char **error __attribute__ ((unused)), char **output __attribute__ ((unused)))
{
  if (cli_present(ctx, "ON") & 1)
    STOP_ON_FAIL = 1;
  else if (cli_present(ctx, "OFF") & 1)
    STOP_ON_FAIL = 0;
  return MdsdclSUCCESS;
}
