#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <mdsdcldef.h>
#include        <sys/time.h>
#include        <sys/resource.h>
#include        <mdsdescrip.h>
#include        <malloc.h>
#include        <unistd.h>
#include        <clidef.h>

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

static char *PROMPT = 0;
static char *DEF_FILE = 0;

void mdsdclSetPrompt(char *prompt) {
  if (PROMPT)
    free(PROMPT);
  PROMPT = strdup(prompt);
}

char *mdsdclGetPrompt() {
  if (PROMPT == NULL)
    PROMPT=strdup("DCL> ");
  return strdup(PROMPT);
}

void mdsdclSetDefFile(char *deffile) {
  if (DEF_FILE)
    free(DEF_FILE);
  DEF_FILE = strdup(deffile);
}

char *mdsdclGetDefFile() {
  if (DEF_FILE == NULL)
    DEF_FILE == strdup(".dcl");
  return strdup(DEF_FILE);
}


	/****************************************************************
	 * mdsdcl_exit:
	 ****************************************************************/
int mdsdcl_exit(void *ctx)
{
  exit(0);
  return 1;
}

	/****************************************************************
	 * mdsdcl_init_timer:
	 * mdsdcl_show_timer:


	 ****************************************************************/

static struct rusage TIMER_START_USAGE;
static struct rusage TIMER_NOW_USAGE;
static struct timeval TIMER_START_TIME;
static struct timeval TIMER_NOW_TIME;

int mdsdcl_init_timer(void *ctx)
{
  gettimeofday(&TIMER_START_TIME, 0);
  getrusage(RUSAGE_SELF, &TIMER_START_USAGE);
  return (1);
}

int mdsdcl_show_timer(void *ctx)
{
  int emsec, umsec, smsec;      /* milliseconds                 */

  gettimeofday(&TIMER_NOW_TIME, 0);
  getrusage(RUSAGE_SELF, &TIMER_NOW_USAGE);
  if (TIMER_NOW_TIME.tv_usec < TIMER_START_TIME.tv_usec)
    --TIMER_NOW_TIME.tv_sec, TIMER_NOW_TIME.tv_usec += 1000000;
  if (TIMER_NOW_USAGE.ru_utime.tv_usec < TIMER_START_USAGE.ru_utime.tv_usec)
    --TIMER_NOW_USAGE.ru_utime.tv_sec, TIMER_NOW_USAGE.ru_utime.tv_usec += 1000000;
  if (TIMER_NOW_USAGE.ru_stime.tv_usec < TIMER_START_USAGE.ru_stime.tv_usec)
    --TIMER_NOW_USAGE.ru_stime.tv_sec, TIMER_NOW_USAGE.ru_stime.tv_usec += 1000000;
  emsec = (TIMER_NOW_TIME.tv_usec - TIMER_START_TIME.tv_usec) / 10000;
  umsec = (TIMER_NOW_USAGE.ru_utime.tv_usec - TIMER_START_USAGE.ru_utime.tv_usec) / 10000;
  smsec = (TIMER_NOW_USAGE.ru_stime.tv_usec - TIMER_START_USAGE.ru_stime.tv_usec) / 10000;
  fprintf(stderr, "elapsed=%ld.%02d user=%ld.%02d sys=%ld.%02d sf=%ld hf=%ld\n",
          TIMER_NOW_TIME.tv_sec - TIMER_START_TIME.tv_sec, emsec,
          TIMER_NOW_USAGE.ru_utime.tv_sec - TIMER_START_USAGE.ru_utime.tv_sec, umsec,
          TIMER_NOW_USAGE.ru_stime.tv_sec - TIMER_START_USAGE.ru_stime.tv_sec, smsec,
          TIMER_NOW_USAGE.ru_minflt - TIMER_START_USAGE.ru_minflt, TIMER_NOW_USAGE.ru_majflt - TIMER_START_USAGE.ru_majflt);
  return (1);
}

	/**************************************************************
	 * mdsdcl_set_prompt:
	 **************************************************************/
int mdsdcl_set_prompt( void *ctx )
{
  char *prompt=0;
  int sts = cli_get_value(ctx, "PROMPT", &prompt);
  mdsdclSetPrompt(prompt);
  free(prompt);
  return sts;
}

	/****************************************************************
	 * mdsdcl_type:
	 ****************************************************************/
int mdsdcl_type( void *ctx)
{
  char *text = 0;
  int sts = cli_get_value(ctx, "P1", &text);
  printf("%s\n", (sts & 1) ? text : " ");
  fflush(stdout);
  free(text);
  return (sts);
}

	/****************************************************************
         * mdsdcl_set_hyphen:
         ****************************************************************/

static int MDSDCL_HYPHEN=0;
int mdsdcl_set_hyphen( void *ctx )
{
  MDSDCL_HYPHEN = cli_present(ctx, "HYPHEN") & 1;
  return 1;
}

	/****************************************************************
	 * mdsdcl_set_verify:
	 ****************************************************************/

static int MDSDCL_VERIFY=0;

int mdsdcl_set_verify( void *ctx )
{
  MDSDCL_VERIFY = cli_present(ctx,"VERIFY") & 1;
  return (1);
}

	/****************************************************************
	 * mdsdcl_define_symbol:
	 ****************************************************************/
int mdsdcl_define_symbol( void *ctx)
{
  int k;
  int sts;
  char *p;
  char *name = 0;
  char *value = 0;

  sts = cli_get_value(ctx, "SYMBOL", &name);
  if (~sts & 1)
    return (sts);

  sts = cli_get_value(ctx, "VALUE", &value);
  if (~sts & 1)
    fprintf(stderr,"*ERR* getting value for symbol");

  k = strlen(name) + strlen(value);
  p = malloc(k + 2);
  sprintf(p, "%s=%s", name, value);
  sts = putenv(p);
  if (sts) {
    perror("error from setenv");
    sts = MDSDCL_STS_ERROR;
  } else
    sts=1;

  return (sts);
}

extern int LibSpawn();


	/**************************************************************
	 * mdsdcl_spawn:
	 **************************************************************/
int mdsdcl_spawn( void *ctx )
{
  int notifyFlag;
  int waitFlag;
  int sts;
  char *cmd = 0;
  struct descriptor cmd_dsc = {0, DTYPE_T, CLASS_S, 0};
  cli_get_value(ctx, "COMMAND", &cmd);
  notifyFlag = cli_present(ctx, "NOTIFY") & 1;
  waitFlag = cli_present(ctx, "WAIT") & 1;
  cmd_dsc.length=strlen(cmd);
  cmd_dsc.pointer=cmd;
  sts = LibSpawn(&cmd_dsc, waitFlag, notifyFlag);
  if (cmd)
    free(cmd);
  return (sts == 0);
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



int mdsdcl_set_command(	void *ctx )
{
  int sts;
  char *table = 0;

		/*------------------------------------------------------
		 * Get tablename and find its address in shared library ...
		 *-----------------------------------------------------*/
  sts = cli_get_value(ctx, "TABLE", &table);
  if (sts & 1) {
    char *error=0;
    int status = mdsdclAddCommands(table, &error);
    free(table);
    if (error) {
      status = 0;
      MdsMsg(status, error);
      return status;
    }
  }

		/*------------------------------------------------------
		 * Check for other qualifiers ...
		 *-----------------------------------------------------*/
  if (cli_present(ctx, "PROMPT") & 1) {
    char *prompt = 0;
    cli_get_value(ctx, "PROMPT", &prompt);
    mdsdclSetPrompt(prompt);
    free(prompt);
  }

  if (cli_present(ctx, "DEF_FILE") & 1) {
    char *def_file = 0;
    cli_get_value(ctx, "DEF_FILE", &def_file);
    mdsdclSetDefFile(def_file);
    free(def_file);
  }

  return (3);
}
	/**************************************************************
	 * mdsdcl_wait:
	 **************************************************************/
int mdsdcl_wait(void *ctx)
{				/* Return:  status                        */
  int k;
  int sts;
  int day, hr, min, sec;
  unsigned int nsec;
  float fsec;
  char *deltatime = 0;

  sts = cli_get_value(ctx, "DELTA_TIME", &deltatime);
  if (~sts & 1)
    return (sts);

  k = sscanf(deltatime, "%d %d:%d:%f", &day, &hr, &min, &fsec);
  free(deltatime);
  if (k != 4) {
    MdsMsg(0, "--> invalid time string");
    return (CLI_STS_BADLINE);
  }
  if (hr < 0)
    hr = -hr;
  sec = (int)fsec;
  nsec = sec + 60 * (min + 60 * (hr + 24 * day));
  sleep(nsec);
  return (sts);
}

int mdsdcl_help(void *ctx) {
  return 1;
}
