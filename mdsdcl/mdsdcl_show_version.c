#include <stdlib.h>
#include <mdsplus/mdsconfig.h>
#include <string.h>
#include <stdio.h>
#include <mdsshr.h>
#include <mdsdcl_messages.h>
#include <dcl.h>

// typedef struct {
//   const char *MAJOR;
//   const char *MINOR;
//   const char *RELEASE;
//   const char *BRANCH;
//   const char *RELEASE_TAG;
//   const char *COMMIT;
//   const char *DATE;
//   const char *MDSVERSION;
// } MDSplusVersionInfo;


// typedef struct {
//   const char *GIT_TAG;
//   const char *GIT_BRANCH;
//   const char *GIT_REMOTE;
//   const char *GIT_REMOTE_URL;
//   const char *GIT_COMMIT;
//   const char *GIT_SRCDIR;
// } MDSplusGitVersionInfo;

extern MDSplusVersionInfo MDSplusVersion;
extern MDSplusGitVersionInfo MDSplusGitVersion;

EXPORT int mdsdcl_show_version(void *ctx __attribute__ ((unused)), char **error __attribute__ ((unused)), char **output)
{
  char *info = malloc(strlen(MDSplusVersion.MAJOR)+strlen(MDSplusVersion.MINOR)+strlen(MDSplusVersion.RELEASE)+
		      strlen(MDSplusVersion.BRANCH)+strlen(MDSplusVersion.RELEASE_TAG)+
		      strlen(MDSplusVersion.COMMIT)+1000);
  
  strcpy(info,"\n\n");
  if (strlen(MDSplusVersion.MAJOR) && strlen(MDSplusVersion.MINOR) && strlen(MDSplusVersion.MAJOR)) {
    sprintf(info+strlen(info),"MDSplus version: %s.%s.%s\n",MDSplusVersion.MAJOR,MDSplusVersion.MINOR,MDSplusVersion.RELEASE);
  }
  sprintf(info+strlen(info),"----------------------\n");
  if (strlen(MDSplusVersion.RELEASE_TAG)) {
    sprintf(info+strlen(info),"  Release:  %s\n",MDSplusVersion.RELEASE_TAG);
    sprintf(info+strlen(info),"  Browse:   https://github.com/MDSplus/mdsplus/tree/%s\n",MDSplusVersion.RELEASE_TAG);
    sprintf(info+strlen(info),"  Download: https://github.com/MDSplus/mdsplus/archive/%s.tar.gz\n",MDSplusVersion.RELEASE_TAG);
  }
  if (strlen(MDSplusVersion.DATE)) {
    sprintf(info+strlen(info),"  Build date: %s\n",MDSplusVersion.DATE);
  }
  if (strlen(info) == 2) {
    strcat(info,"no release information available\n");
  }
  strcat(info,"\n\n");
  *output=strdup(info);
  free(info);

  return 1;
}

EXPORT int mdsdcl_show_git_info(void *ctx, char **error, char **output)
{  
  int status;
  int   qfr[] = { 
    cli_present(ctx, "TAG"),
    cli_present(ctx, "BRANCH"),
    cli_present(ctx, "REMOTE"),
    cli_present(ctx, "REMOTE_URL"),
    cli_present(ctx, "COMMIT"),
    cli_present(ctx, "SRCDIR")
  };

  // if no qualifiers have been set set all
  int i,s=0;
  for(i=0; i<6; ++i) s+=(qfr[i]==MdsdclPRESENT);
  if(s==0) for(i=0; i<6; ++i) qfr[i]=MdsdclPRESENT;
  
  char *info = malloc(strlen(MDSplusGitVersion.GIT_TAG)+
                      strlen(MDSplusGitVersion.GIT_BRANCH)+
                      strlen(MDSplusGitVersion.GIT_REMOTE)+
                      strlen(MDSplusGitVersion.GIT_REMOTE_URL)+
                      strlen(MDSplusGitVersion.GIT_COMMIT)+
                      strlen(MDSplusGitVersion.GIT_SRCDIR)+
                      +1000);
  
  strcpy(info,"");
  if (s) {      
    if(qfr[0] == MdsdclPRESENT)  sprintf(info+strlen(info),"%s ",MDSplusGitVersion.GIT_TAG);
    if(qfr[1] == MdsdclPRESENT)  sprintf(info+strlen(info),"%s ",MDSplusGitVersion.GIT_BRANCH);
    if(qfr[2] == MdsdclPRESENT)  sprintf(info+strlen(info),"%s ",MDSplusGitVersion.GIT_REMOTE);
    if(qfr[3] == MdsdclPRESENT)  sprintf(info+strlen(info),"%s ",MDSplusGitVersion.GIT_REMOTE_URL);
    if(qfr[4] == MdsdclPRESENT)  sprintf(info+strlen(info),"%s ",MDSplusGitVersion.GIT_COMMIT);
    if(qfr[5] == MdsdclPRESENT)  sprintf(info+strlen(info),"%s ",MDSplusGitVersion.GIT_SRCDIR);
    sprintf(info+strlen(info),"\n");
  } else {    
    sprintf(info+strlen(info),"\n MDSplus code info:\n");
    sprintf(info+strlen(info),"----------------------\n");
    sprintf(info+strlen(info),"  Tag:    %s\n",MDSplusGitVersion.GIT_TAG);
    sprintf(info+strlen(info),"  srcdir: %s\n",MDSplusGitVersion.GIT_SRCDIR);
    sprintf(info+strlen(info),"  Commit: %s\n",MDSplusGitVersion.GIT_COMMIT);
    sprintf(info+strlen(info),"  Branch: %s\n",MDSplusGitVersion.GIT_BRANCH);
    sprintf(info+strlen(info),"  Remote: %s\n",MDSplusGitVersion.GIT_REMOTE);
    sprintf(info+strlen(info),"  Url:    %s\n",MDSplusGitVersion.GIT_REMOTE_URL);
    sprintf(info+strlen(info),"\n\n");
  }
  status = MdsdclSUCCESS;    
  *error = strdup("");
  *output=strdup(info);
  free(info);
  return status;
}


