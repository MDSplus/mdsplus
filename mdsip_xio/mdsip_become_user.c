#include "mdsip.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>

int mdsip_become_user(mdsip_client_t *c)
{
  int  status = -1;
  if (c->num_groups == -1)
  {
    char *user = strcmp(c->local_user,"MAP_TO_LOCAL") ? c->local_user : c->remote_user;
    struct passwd *pwd;
    if (user)
    {
      int num_groups=0;
      char *grouplist;
      user=strcpy((char *)malloc(strlen(user)+1),user);
      if ((grouplist = strchr(user,':')) != NULL)
      {
	struct group *group_info;
	char *group;
	grouplist[0]=0;
	grouplist++;
	while ((grouplist != NULL) && (grouplist[0] != 0) && (num_groups < NGROUPS))
	{
	  char *group_end;
          group=grouplist;
	  if ((group_end=strchr(grouplist,',')) != NULL)
	  {
	    group_end[0]=0;
	    grouplist=group_end+1;
	  }
	  else
	    grouplist=0;
      	  if ((group_info = getgrnam(group)) != NULL)
	  {
	    c->gids[num_groups++]=group_info->gr_gid;
	  }
	  else
	  {
	    fprintf(stderr,"%s, Error initializing groups for %s, group %s is unknown\n",mdsip_current_time(),user,group);
	  }
	}
      }
      pwd = user ? getpwnam(user) : NULL;
      if (pwd)
      {
	int homelen = strlen(pwd->pw_dir); 
	char *cmd = strcpy(malloc(homelen+10),"HOME=");
	char *proxy = getenv("X509_USER_PROXY");
        status = 0;
	if (proxy != NULL)
	  chown(proxy,pwd->pw_uid,pwd->pw_gid);
        c->uid = pwd->pw_uid;
        c->gid = pwd->pw_gid;
	if (num_groups > 0)
	{
          c->num_groups=num_groups;
        }
     	else
	{  
	  initgroups(user,pwd->pw_gid);
          c->num_groups = getgroups(NGROUPS,c->gids);
        }
	strcat(cmd,pwd->pw_dir);
	putenv(cmd);
	/* DO NOT FREE CMD --- putenv requires it to stay allocated */
      }
      else
      {
        fprintf(stderr,"%s, Invalid user mapping, %s is mapped to %s and user %s is unknown on local system\n",
		mdsip_current_time(),c->remote_user ? c->remote_user : "",c->local_user ? c->local_user  : "",
		user ? user : "");
      }
      free(user);
    }
  }
  else
    status = 0;
  if (status == 0)
  {
    setgroups(c->num_groups,c->gids);
    setgid(c->gid);
    status = setuid(c->uid);
    if (status)
      fprintf(stderr,"%s, Cannot setuid - run server as root!",mdsip_current_time());
  }
  fflush(stderr);
  return status == 0;
}

