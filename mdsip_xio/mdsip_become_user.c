#include "mdsip.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>

int mdsip_become_user(char *local_user, char *remote_user)
{
  int status = -1;
  char *user = strcmp(local_user,"MAP_TO_LOCAL") ? local_user : remote_user;
  struct passwd *pwd = user ? getpwnam(user) : NULL;
  if (pwd)
  {
    int homelen = strlen(pwd->pw_dir); 
    char *cmd = strcpy(malloc(homelen+10),"HOME=");
    char *proxy = getenv("X509_USER_PROXY");
    if (proxy != NULL)
      chown(proxy,pwd->pw_uid,pwd->pw_gid);
    initgroups(pwd->pw_name,pwd->pw_gid);
    status = setgid(pwd->pw_gid);
    status = setuid(pwd->pw_uid);
    if (status)
      fprintf(stderr,"%s, Cannot setuid - run server as root!",mdsip_current_time());
    strcat(cmd,pwd->pw_dir);
    putenv(cmd);
    /* DO NOT FREE CMD --- putenv requires it to stay allocated */
  }
  return status == 0;
}

