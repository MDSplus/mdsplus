#include "mdsip_connections.h"
#include <mdsshr.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <strroutines.h>
#include <libroutines.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <config.h>
#include <signal.h>
#ifndef HAVE_WINDOWS_H
#include <sys/types.h>
#include <pwd.h>
#include <sys/wait.h>
#endif

#ifdef HAVE_VXWORKS_H
int CheckClient(char *matchString) {
  return 1;
}
  
#else

static void CompressString(struct descriptor *in, int upcase)
{
  unsigned short len;
  static int two=2;
  StrTrim(in,in,&len);
  if (upcase)
    StrUpcase(in,in);
  while(in->length && (in->pointer[0] == ' ' || in->pointer[0] == '	'))
    StrRight(in,in,&two);
}
#ifdef HAVE_WINDOWS_H
static int BecomeUser(char *remuser, struct descriptor *local_user) {return 1;}
#else
static void ChildSignalHandler(int num) {
  sigset_t set, oldset;
  pid_t pid;
  int status, exitstatus;
  /* block other incoming SIGCHLD signals */
  sigemptyset(&set);
  sigaddset(&set, SIGCHLD);
  sigprocmask(SIG_BLOCK, &set, &oldset);
  /* wait for child */
  while((pid = waitpid((pid_t)-1, &status, WNOHANG)) > 0) {
    /* re-install the signal handler (some systems need this) */
    signal(SIGCHLD,ChildSignalHandler);
    /* and unblock it */
    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);
    sigprocmask(SIG_UNBLOCK, &set, &oldset);
  }
}
static int BecomeUser(char *remuser, struct descriptor *local_user) {
  int ok = 1;
  CompressString(local_user,0);
  if (local_user->length == 4 && strncmp(local_user->pointer,"SELF",4) == 0)
    ok = 1;
  else if (local_user->length)  {
    char *luser = MdsDescrToCstring(local_user);
    int map_to_local = strcmp(luser,"MAP_TO_LOCAL")==0;
    char *user = map_to_local ? remuser : luser; 
    int status = -1;
    struct passwd *pwd;
    if (map_to_local && remuser == 0) {
      MdsFree(luser);
      return 0;
    }
    pwd = user ? getpwnam(user) : 0;
    if (!pwd && remuser == user) {
      int i;
      for (i=0;i<strlen(user);i++)
        user[i] = __tolower(user[i]);
      pwd = getpwnam(user);
    }
    if (pwd) {
       int pid = 0;
       int homelen = strlen(pwd->pw_dir); 
       char *cmd = strcpy(malloc(homelen+10),"HOME=");
       char *mds_path = getenv("MDS_PATH");
       if (GetMode() == 'I') {
         signal(SIGCHLD,ChildSignalHandler);
         pid = fork();
       }
       else
         pid = getpid();
       if (!pid || GetMode() != 'I') {
#ifdef GLOBUS
         chown(getenv("X509_USER_PROXY"),pwd->pw_uid,pwd->pw_gid);
#endif
         initgroups(pwd->pw_name,pwd->pw_gid);
         status = setgid(pwd->pw_gid);
         status = setuid(pwd->pw_uid);
         if (status)
           printf("Cannot setuid - run server as root!");
         strcat(cmd,pwd->pw_dir);
         putenv(cmd);
       /* DO NOT FREE CMD --- putenv requires it to stay allocated */
       }
       else
         status = 0;
    }
    else
      printf("Invalid mapping, cannot find user %s\n",user);
    MdsFree(luser);
    ok = (status == 0) ? 1 : 0;
  }
  return ok;
}
#endif

int CheckClient(char *username, int num, char **matchString) {
  FILE *f = fopen(GetHostfile(),"r");
  int ok = 0;
  static int zero=0, one=1, two=2;
  if (f) {
    static char line_c[1024];
    static char match_c1[1024];
    static char match_c2[1024];
    static struct descriptor line_d = {0, DTYPE_T, CLASS_S, line_c};
    static struct descriptor local_user = {0,DTYPE_T,CLASS_D,0};
    static struct descriptor access_id = {0,DTYPE_T,CLASS_D,0};
    static DESCRIPTOR(delimiter,"|");
    while (ok==0 && fgets(line_c,1023,f)) {
      if (line_c[0] != '#') {
	int i;
        line_d.length = strlen(line_c) - 1;
	StrElement(&access_id,&zero,&delimiter,&line_d);
        StrElement(&local_user,&one,&delimiter,&line_d);
        CompressString(&access_id,1);
	CompressString(&local_user,0);
        if (access_id.length) {
	  for (i=0;i<num;i++) {
	    struct descriptor match_d = {strlen(matchString[i]), DTYPE_T, CLASS_S, matchString[i]};
	    struct descriptor match = {0, DTYPE_T, CLASS_D, 0};
	    StrUpcase(&match,&match_d);
	    if (access_id.pointer[0] != '!') {
	      if (match.length==strlen("multi") && 
		  strncmp(match.pointer,"MULTI",strlen("multi"))==0 &&
		  access_id.length==strlen("multi") &&
		  strncmp(access_id.pointer,"MULTI",strlen("multi"))==0)
		BecomeUser(0,&local_user);
	      else {
		if (StrMatchWild(&match,&access_id) & 1)
		  ok = GetMulti() ? 1 : BecomeUser(username,&local_user);
	      }
	    } else {
	      StrRight(&access_id,&access_id,&two);
	      if (StrMatchWild(&match,&access_id) & 1)
		ok = 2;
	    }
	    StrFree1Dx(&match);
	  }
	}
      }
    }
    fclose(f);
  }
  else
  {
    printf("Unable to open hostfile: %s\n",GetHostfile());
    exit(1);
  }
  return ok;
}

#endif

