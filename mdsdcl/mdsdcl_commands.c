#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        "dcl.h"
#include        <sys/time.h>
#include        <sys/resource.h>
#include        <mdsdescrip.h>
#include        <malloc.h>
#include        <unistd.h>
#include <readline/readline.h>

typedef struct dclMacroList {
  char *name;               /*!<  macro name */
  int lines;                /*!<  number of lines in macro */
  char **cmds;              /*!<  cmds in macro */
  struct dclMacroList *next;
} dclMacroList, *dclMacroListPtr;


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
    PROMPT=strdup("Command> ");
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
  int sts;
  char *p;
  char *name = 0;
  char *value = 0;
  char *env;

  sts = cli_get_value(ctx, "SYMBOL", &name);
  if (~sts & 1)
    goto done;

  sts = cli_get_value(ctx, "VALUE", &value);
  if (~sts & 1)
    fprintf(stderr,"*ERR* getting value for symbol");

  p = malloc(strlen(name)+strlen(value) + 2);
  sprintf(p, "%s=%s", name, value);
  sts = putenv(p);
  env = getenv(name);
  if (sts) {
    perror("error from putenv");
    sts = MDSDCL_STS_ERROR;
  } else
    sts=1;
 done:
  if (name)
    free(name);
  if (value)
    free(value);

  return (sts);
}

int mdsdcl_env(void *ctx) {
  int sts;
  char *value;
  sts = cli_get_value(ctx,"P1",&value);
  sts = putenv(value);
  if (sts) {
    perror("error from putenv");
    fprintf(stderr,"Attempting putenv(\"%s\")\n",value);
    sts = MDSDCL_STS_ERROR;
  } else
    sts = 1;
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
  if (cmd) {
    cmd_dsc.length=strlen(cmd);
    cmd_dsc.pointer=cmd;
  }
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
  char *p1=0;
  int sts;
  cli_get_value(ctx, "P1", &p1);
  sts=mdsdcl_do_help(p1);
  if (p1)
    free(p1);
  return 1;
}

static dclMacroListPtr MLIST=0;

static dclMacroListPtr mdsdclNewMacro(char *name) {
  dclMacroListPtr l,prev=0;
  int i;
  for (i=0;i<strlen(name);i++)
    name[i]=toupper(name[i]);
  for (l=MLIST; l; prev=l,l=l->next) {
    if (strcasecmp(name,l->name)==0)
      break;
  }
  if (l) {
    int i;
    for (i=0;i<l->lines;i++)
      free(l->cmds[i]);
    l->lines=0;
  } else {
    l=malloc(sizeof(dclMacroList));
    l->name=name;
    l->lines=0;
    l->cmds=malloc(sizeof(char *));
    l->next=0;
    if (prev)
      prev->next=l;
    else
      MLIST=l;
  }
  return l;
}
	   
int mdsdcl_define(void *ctx) {
  char *name=0;
  char *line;
  dclMacroListPtr macro;
  cli_get_value(ctx, "p1", &name); /*** do not free name as it is used in macro definition ***/
  if (name == NULL)
      return CLI_STS_IVVERB;
  macro = mdsdclNewMacro(name);
  while ((line=readline("DEFMAC> ")) && (strlen(line)) > 0) {
    macro->cmds=realloc(macro->cmds,sizeof(char *)*(macro->lines+1));
    macro->cmds[macro->lines++]=strdup(line);
  }
  return 1;
}


static void mdsdcl_print_macro(dclMacroListPtr l,int full) {
  printf("\n%s\n",l->name);
  if (full == CLI_STS_PRESENT) {
    int i;
    for (i=0; i<l->lines;i++) {
      printf("  %s\n",l->cmds[i]);
    }
  }
}

int mdsdcl_show_macro(void *ctx) {
  char *name=0;
  int full = cli_present(ctx, "FULL");
  dclMacroListPtr l;
  cli_get_value(ctx, "P2", &name);
  if (name && (strlen(name) > 0)) {
    for (l=MLIST;l;l=l->next) {
      if (strcasecmp(name,l->name)==0)
	break;
    }
    if (l)
      mdsdcl_print_macro(l,full);
  } else {
    for (l=MLIST;l;l=l->next)
      mdsdcl_print_macro(l,full);
  }
  if (name)
    free(name);
  return 1;
}

static void mdsdclSubstitute(char **cmd, char *p1, char *p2, char *p3, char *p4, char *p5, char *p6, char *p7) {
  int i;
  char *ps[7]={p1,p2,p3,p4,p5,p6,p7};
  int p;
  for (p=0;p<7;p++) {
    char param[10];
    int i;
    sprintf(param,"'p%d'",p+1);
    for (i=0;i<(strlen(*cmd)-3);i++) {
      if (strncasecmp((*cmd)+i,param,strlen(param))==0) {
	if (ps[p] && (strlen(ps[p])>0)) {
	  char *newcmd=malloc(strlen(*cmd)+strlen(ps[p]));
	  (*cmd)[i]=0;
	  strcpy(newcmd,*cmd);
	  strcat(newcmd,ps[p]);
	  strcat(newcmd,(*cmd)+i+4);
	  free(*cmd);
	  *cmd=newcmd;
	} else {
	  memcpy(*cmd+i,(*cmd)+i+4,strlen(*cmd)-(i+4));
	  (*cmd)[strlen(*cmd)-4]=0;
	}
      }
    }
  }
}	

int mdsdcl_do_macro(void *ctx) {
  char *name=0;
  char *times_s=0;
  char *p1=0,*p2=0,*p3=0,*p4=0,*p5=0,*p6=0,*p7=0;
  int sts=1;
  dclMacroListPtr l;
  cli_get_value(ctx, "name", &name);
  cli_get_value(ctx, "repeat", &times_s);
  cli_get_value(ctx, "p1", &p1);
  cli_get_value(ctx, "p2", &p2);
  cli_get_value(ctx, "p3", &p3);
  cli_get_value(ctx, "p4", &p4);
  cli_get_value(ctx, "p5", &p5);
  cli_get_value(ctx, "p6", &p6);
  cli_get_value(ctx, "p7", &p7);
  for (l=MLIST; l; l=l->next) {
    if (strcasecmp(name,l->name)==0)
      break;
  }
  if (l) {
    int times=1;
    int time;
    int failed=0;
    if (times_s) {
      times=atoi(times_s);
      free(times_s);
    }
    for (time=0;(failed==0) && (time<times);time++) {
      int i;
      for (i=0;i<l->lines;i++) {
	char *cmd=strdup(l->cmds[i]);
	mdsdclSubstitute(&cmd,p1,p2,p3,p4,p5,p6,p7);
	sts=mdsdcl_do_command(cmd);
	free(cmd);
	if (!(sts&1))
	  failed=1;
      }
    }
  } else {
    sts = CLI_STS_IVVERB;
  }
  if (name)
    free(name);
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
}

int mdsdcl_delete_macro(void *ctx) {
  char *name=0;
  cli_get_value(ctx, "name", &name);
  if (name) {
    dclMacroListPtr l,prev=0;
    for (l=MLIST;l;prev=l,l=l->next) {
      if (strcasecmp(name,l->name)==0)
	break;
    }
    if (l) {
      int i;
      for (i=0;i<l->lines;i++)
	free(l->cmds[i]);
      free(l->cmds);
      if (prev)
	prev->next=l->next;
      else
	MLIST=l->next;
      free(l);
    }
  }
  return 1;
}
