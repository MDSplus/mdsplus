#include <stdlib.h>
#include <mdsplus/mdsconfig.h>
#include <string.h>
#include <stdio.h>
#include <mdsshr.h>
extern MDSplusVersionInfo MDSplusVersion;
EXPORT int mdsdcl_show_version(void *ctx __attribute__ ((unused)), char **error __attribute__ ((unused)), char **output)
{
  char *info = malloc(strlen(MDSplusVersion.MAJOR)+strlen(MDSplusVersion.MINOR)+strlen(MDSplusVersion.RELEASE)+
		      strlen(MDSplusVersion.BRANCH)+strlen(MDSplusVersion.RELEASE_TAG)+
		      strlen(MDSplusVersion.COMMIT)+1000);
  strcpy(info,"\n\n");
  if (strlen(MDSplusVersion.MAJOR) && strlen(MDSplusVersion.MINOR) && strlen(MDSplusVersion.MAJOR)) {
    sprintf(info+strlen(info),"MDSplus version: %s.%s.%s\n",MDSplusVersion.MAJOR,MDSplusVersion.MINOR,MDSplusVersion.RELEASE);
  }
  if (strlen(MDSplusVersion.BRANCH)) {
    sprintf(info+strlen(info),"Git branch: %s\n",MDSplusVersion.BRANCH);
    if (strlen(MDSplusVersion.RELEASE_TAG)==0 && strlen(MDSplusVersion.COMMIT)==0) {
      sprintf(info+strlen(info),"  View:\n    https://github.com/MDSplus/mdsplus/tree/%s\n",MDSplusVersion.BRANCH);
      sprintf(info+strlen(info),"  Download:\n    https://github.com/MDSplus/mdsplus/archive/%s.tar.gz\n",MDSplusVersion.BRANCH);
    }
  }
  if (strlen(MDSplusVersion.RELEASE_TAG)) {
    sprintf(info+strlen(info),"  Git tag: %s\n",MDSplusVersion.RELEASE_TAG);
    sprintf(info+strlen(info),"  View:\n    https://github.com/MDSplus/mdsplus/tree/%s\n",MDSplusVersion.RELEASE_TAG);
    sprintf(info+strlen(info),"  Download:\n    https://github.com/MDSplus/mdsplus/archive/%s.tar.gz\n",MDSplusVersion.RELEASE_TAG);
  }
  if (strlen(MDSplusVersion.COMMIT)) {
    sprintf(info+strlen(info),"  Git commit: %s\n",MDSplusVersion.COMMIT);
    if (strlen(MDSplusVersion.RELEASE_TAG)==0) {
      sprintf(info+strlen(info),"  View:\n    https://github.com/MDSplus/mdsplus/tree/%s\n",MDSplusVersion.COMMIT);
      sprintf(info+strlen(info),"  Download:\n    https://github.com/MDSplus/mdsplus/archive/%s.tar.gz\n",MDSplusVersion.COMMIT);
    }
  }
  if (strlen(MDSplusVersion.DATE)) {
    sprintf(info+strlen(info),"  Build date: %s\n",MDSplusVersion.DATE);
  }
  if (strlen(info) == 2) {
    strcat(info,"Manual build from sources, no version information available\n");
  }
  strcat(info,"\n\n");
  *output=strdup(info);
  free(info);
  return 1;
}
