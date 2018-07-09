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
#include <mdsshr.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <strroutines.h>
#include <libroutines.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <mdsplus/mdsconfig.h>
#include <signal.h>
#include <unistd.h>
#include <ctype.h>
#ifndef _WIN32
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <sys/wait.h>
#endif

#include "mdsip_connections.h"

static void CompressString(struct descriptor *in, int upcase)
{
  unsigned short len;
  static unsigned short two = 2;
  StrTrim(in, in, &len);
  if (upcase)
    StrUpcase(in, in);
  while (in->length && (in->pointer[0] == ' ' || in->pointer[0] == '	'))
    StrRight(in, in, &two);
}

#ifdef _WIN32
static int BecomeUser(char *remuser __attribute__ ((unused)), struct descriptor *local_user __attribute__ ((unused))){
  return 1;
}
#else
static int BecomeUser(char *remuser, struct descriptor *local_user)
{
  int ok = 1;
  CompressString(local_user, 0);
  if (local_user->length == 4 && strncmp(local_user->pointer, "SELF", 4) == 0)
    ok = 1;
  else if (local_user->length) {
    char *luser = MdsDescrToCstring(local_user);
    int map_to_local = strcmp(luser, "MAP_TO_LOCAL") == 0;
    char *user = map_to_local ? remuser : luser;
    int status = -1;
    struct passwd *pwd;
    if (map_to_local && remuser == 0) {
      MdsFree(luser);
      return 0;
    }
    pwd = user ? getpwnam(user) : 0;
    if (!pwd && remuser == user) {
      size_t i;
      for (i = 0; i < strlen(user); i++)
	user[i] = tolower(user[i]);
      pwd = getpwnam(user);
    }
    if (pwd) {
      int homelen = strlen(pwd->pw_dir);
      char *cmd = strcpy(malloc(homelen + 10), "HOME=");
      initgroups(pwd->pw_name, pwd->pw_gid);
      status = setgid(pwd->pw_gid);
      status = setuid(pwd->pw_uid);
      if (status)
	printf("Cannot setuid - run server as root!");
      strcat(cmd, pwd->pw_dir);
      putenv(cmd);
      /* DO NOT FREE CMD --- putenv requires it to stay allocated */
    } else
      printf("Invalid mapping, cannot find user %s\n", user);
    MdsFree(luser);
    ok = (status == 0) ? 1 : 0;
  }
  return ok;
}
#endif

int CheckClient(char *username, int num, char **matchString)
{
  int ok = 0;
  char *hostfile = GetHostfile();
  if (strncmp(hostfile, "TDI", 3)) {
    FILE *f = fopen(hostfile, "r");
    static int zero = 0, one = 1;
    static unsigned short two = 2;
    if (f) {
      static char line_c[1024];
      static struct descriptor line_d = { 0, DTYPE_T, CLASS_S, line_c };
      static struct descriptor local_user = { 0, DTYPE_T, CLASS_D, 0 };
      static struct descriptor access_id = { 0, DTYPE_T, CLASS_D, 0 };
      static DESCRIPTOR(delimiter, "|");
      while (ok == 0 && fgets(line_c, 1023, f)) {
	if (line_c[0] != '#') {
	  int i;
	  line_d.length = (unsigned short)(strlen(line_c) - 1);
	  StrElement(&access_id, &zero, (struct descriptor *)&delimiter, (struct descriptor *)&line_d);
	  StrElement(&local_user, &one, (struct descriptor *)&delimiter, (struct descriptor *)&line_d);
	  CompressString(&access_id, 1);
	  CompressString(&local_user, 0);
	  if (access_id.length) {
	    for (i = 0; i < num; i++) {
	      struct descriptor match_d =
		  { (unsigned short)strlen(matchString[i]), DTYPE_T, CLASS_S, matchString[i] };
	      struct descriptor_d match = { 0, DTYPE_T, CLASS_D, 0 };
	      StrUpcase((struct descriptor *)&match, &match_d);
	      if (access_id.pointer[0] != '!') {
		if (match.length == strlen("multi") &&
		    strncmp(match.pointer, "MULTI", strlen("multi")) == 0 &&
		    access_id.length == strlen("multi") &&
		    strncmp(access_id.pointer, "MULTI", strlen("multi")) == 0)
		  BecomeUser(0, &local_user);
		else {
		  if (StrMatchWild((struct descriptor *)&match, &access_id) & 1)
		    ok = GetMulti()? 1 : BecomeUser(username, &local_user);
		}
	      } else {
		StrRight((struct descriptor *)&access_id, (struct descriptor *)&access_id, &two);
		if (StrMatchWild((struct descriptor *)&match, &access_id) & 1)
		  ok = 2;
	      }
	      StrFree1Dx(&match);
	    }
	  }
	}
      }
      fclose(f);
    } else {
      perror("CheckClient");
      printf("Unable to open hostfile one: %s\n", GetHostfile());
      exit(1);
    }
  } else {
    int i;
    int status;
    struct descriptor cmd_d = { 0, DTYPE_T, CLASS_S, 0 };
    struct descriptor_d ans_d = { 0, DTYPE_T, CLASS_D, 0 };
    size_t cmdlen = strlen(hostfile) + 10 + strlen(username);
    char *cmd;
    static int (*TdiExecute) (struct descriptor * cmd, struct descriptor_d * ans, void *endarg) = 0;
    if (TdiExecute == 0) {
      DESCRIPTOR(TdiShr, "TdiShr");
      DESCRIPTOR(tdiexec, "TdiExecute");
      status = LibFindImageSymbol(&TdiShr, &tdiexec, &TdiExecute);
      if (!(status & 1)) {
	fprintf(stderr, "Error activating TdiShr");
	return 0;
      }
    }
    for (i = 0; i < num; i++)
      cmdlen += (strlen(matchString[i]) + 3);
    cmd = (char *)malloc(cmdlen);
    sprintf(cmd, "%s(\"%s\",", hostfile + 3, username);
    for (i = 0; i < num; i++)
      sprintf(cmd + strlen(cmd), "\"%s\",", matchString[i]);
    cmd[strlen(cmd) - 1] = ')';
    cmd_d.pointer = cmd;
    cmd_d.length = (unsigned short)strlen(cmd);
    status = (*TdiExecute) (&cmd_d, &ans_d MDS_END_ARG);
    if (status & 1) {
      ok = 1;
      if (ans_d.pointer && ans_d.length > 0) {
	if (GetMulti())
	  ok = BecomeUser(username, (struct descriptor *)&ans_d);
	StrFree1Dx(&ans_d);
      }
    } else
      ok = 0;
    free(cmd);
  }
  return ok;
}
