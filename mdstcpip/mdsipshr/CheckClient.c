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

#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifndef _WIN32
#include <grp.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

#include <libroutines.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <strroutines.h>
#include <tdishr_messages.h>
#include <treeshr.h>

#include "../mdsip_connections.h"

extern int TdiExecute();

static inline int filter_string(char **const str_ptr, const int upcase)
{
#define str (*str_ptr)
  while (*str && isspace(*str))
    str++;
  char *c = str + strlen(str) - 1;
  while (isspace(*c))
    c--;
  *++c = '\0'; // terminate
  if (upcase)
  {
    for (c = str; *c; c++)
      *c = toupper(*c);
  }
  return c - str; // is strlen
}

#ifdef _WIN32
#define become_user(remote_user, local_user) ACCESS_GRANTED
#else
static int become_user(const char *remote_user,
                       const char *local_user)
{ // both args may be NULL
  if (!local_user || !*local_user)
    return ACCESS_NOMATCH; // NULL or empty
  if (strcmp(local_user, "SELF") == 0)
    return ACCESS_GRANTED; // NOP
  const int map_to_local = strcmp(local_user, "MAP_TO_LOCAL") == 0;
  if (map_to_local && !(remote_user || *remote_user))
    return ACCESS_DENIED; // cannot map to invalid remote user
  const int is_root =
      remote_user &&
      strcmp(remote_user, "root") == 0; // if map_to_local map root to nobody
  const char *user;
  if (map_to_local)
  {
    user = is_root ? "nobody" : remote_user;
  }
  else if (strcmp(local_user, "SANDBOX") == 0)
  {
    MdsEnableSandbox();
    user = "nobody";
  }
  else
    user = local_user;
  struct passwd *pwd = user ? getpwnam(user) : 0;
  // On macOS Ventura, searching for "unknown" in password list 
  // returns default "_unknown" user, which should be ignored.
#ifdef __APPLE__
  if (pwd && (strcmp("_unknown", pwd->pw_name) == 0)) {
    pwd = 0;
  }
#endif
  if (!pwd && user == remote_user)
  {
    int i;
    char *lowuser = malloc(i = strlen(remote_user) + 1);
    while (i-- > 0)
      lowuser[i] = tolower(remote_user[i]);
    pwd = getpwnam(lowuser);
    free(lowuser);
#ifdef __APPLE__
    if (pwd && (strcmp("_unknown", pwd->pw_name) == 0)) {
      pwd = 0;
    }
#endif
  }
  int access;
  if (pwd)
  {
    initgroups(pwd->pw_name, pwd->pw_gid);
    if (setgid(pwd->pw_gid) || setuid(pwd->pw_uid))
    {
      fprintf(stderr, "Cannot set gid/uid - run server as root!\n");
      access = ACCESS_DENIED;
    }
    else
    {
      if (setenv("HOME", pwd->pw_dir, TRUE))
      {
        fprintf(stderr, "Failed to set HOME for user \"%s\"\n", user);
        access = ACCESS_DENIED;
      }
      else
        access = ACCESS_GRANTED;
    }
  }
  else
    access = ACCESS_NOMATCH;
  return access;
}
#endif

static inline size_t bytes_added(ssize_t result_of_sprintf)
{
  return (result_of_sprintf > 0) ? (size_t)result_of_sprintf : 0;
}

int CheckClient(const char *const username, int num,
                char *const *const matchString)
{
  int access = ACCESS_NOMATCH;
  char *hostfile = GetHostfile();
  if (strncmp(hostfile, "TDI", 3))
  {
    FILE *f = fopen(hostfile, "r");
    if (f)
    {
      struct match
      {
        char *c;
        mdsdsc_t d;
      } *matchS = calloc(num, sizeof(struct match));
      char line_c[1024], *access_id, *local_user;
      int i, deny;
      while (access == ACCESS_NOMATCH && fgets(line_c, 1023, f))
      {
        if (line_c[0] != '#')
        {
          access_id = line_c;
          local_user = strchr(line_c, '|');
          if (local_user)
          {
            *local_user++ = '\0';
            filter_string(&local_user, FALSE);
          }
          if (filter_string(&access_id, TRUE))
          { // not empty
            if ((deny = access_id[0] == '!'))
            {
              access_id++; // drop '!'
              if (!filter_string(&access_id, FALSE))
              {
                if (username)
                  access = ACCESS_DENIED; // deny
                else
                  continue; // looking for multi
              }
            }
            DESCRIPTOR_FROM_CSTRING(access_d, access_id);
            for (i = 0; i < num && access == ACCESS_NOMATCH; i++)
            {
              struct match *const match = &matchS[i];
              if (!match->c)
              {
                match->c = strdup(matchString[i]);
                match->d.dtype = DTYPE_T;
                match->d.class = CLASS_S;
                match->d.pointer = match->c;
                match->d.length =
                    (length_t)filter_string(&match->d.pointer, TRUE);
              }
              if (deny)
              {
                if (IS_OK(StrMatchWild(&match->d, &access_d)))
                  access = ACCESS_DENIED;
              }
              else
              {
                if (strcmp(match->d.pointer, "MULTI") == 0 &&
                    strcmp(access_id, "MULTI") == 0)
                  access = become_user(NULL, local_user);
                else if (IS_OK(StrMatchWild(&match->d, &access_d)))
                  access = GetMulti() ? ACCESS_GRANTED
                                      : become_user(username, local_user);
              }
            }
          }
        }
      }
      for (i = 0; i < num && matchS[i].c; i++)
        free(matchS[i].c);
      free(matchS);
      fclose(f);
    }
    else
    {
      perror("CheckClient");
      fprintf(stderr, "Unable to open hostfile: %s\n", GetHostfile());
      access = ACCESS_DENIED;
    }
  }
  else
  {
    // hostfile starts with TDI
    size_t cmdlen = strlen(hostfile) + 5;
    if (username)
    {
      if (strchr(username, '"'))
      {
        /* " in username would cause a syntax error
         * or could lead to unsafe code execution
         */
        fprintf(stderr, "CheckClient: invalid username '%s'.", username);
        return ACCESS_DENIED;
      }
      cmdlen += strlen(username) + 1;
    }
    int i;
    for (i = 0; i < num; i++)
      cmdlen += strlen(matchString[i]) + 3;
    char *const cmd = (char *)malloc(cmdlen);
    if (cmd == NULL)
      return ACCESS_DENIED;
    size_t len = bytes_added(sprintf(cmd, "%s(", hostfile + 3));
    if (username)
      len += bytes_added(sprintf(cmd + len, "\"%s\",", username));
    else
      len += bytes_added(sprintf(cmd + len, "*,"));
    for (i = 0; i < num; i++)
      len += bytes_added(sprintf(cmd + len, "\"%s\",", matchString[i]));
    cmd[len - 1] = ')'; // replace trailing , with closing )
    mdsdsc_t cmd_d = {(length_t)len, DTYPE_T, CLASS_S, cmd};
    mdsdsc_t ans_d = {0, DTYPE_T, CLASS_D, 0};
    int status = TdiExecute(&cmd_d, &ans_d MDS_END_ARG);
    free(cmd);
    if (STATUS_OK)
    {
      access = ACCESS_GRANTED;
      if (ans_d.pointer && ans_d.length > 0)
      {
        if (!GetMulti())
        {
          ans_d.pointer = realloc(ans_d.pointer, ans_d.length + 1);
          ans_d.pointer[ans_d.length] = '\0';
          access = become_user(username, ans_d.pointer);
        }
        StrFree1Dx((mdsdsc_d_t *)&ans_d);
      }
    }
    else if (status == TdiUNKNOWN_VAR)
    {
      fprintf(stderr, "CheckClient: Failed to load tdi function \"%s\"\n",
              hostfile + 3);
      access = ACCESS_DENIED;
    }
    else
      access = ACCESS_DENIED;
  }
  return access;
}
