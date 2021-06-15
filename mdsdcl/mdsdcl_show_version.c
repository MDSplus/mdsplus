#include <mdsplus/mdsconfig.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dcl.h>
#include <mdsdcl_messages.h>
#include <mdsversion.h>

#include "dcl_p.h"

extern MDSplusGitVersionInfo MDSplusGitVersion;
extern const mds_version_t MdsVersion;
extern const char *MdsRelease();

EXPORT int mdsdcl_show_version(void *ctx __attribute__((unused)),
                               char **error __attribute__((unused)),
                               char **output)
{
  char *info = *output = malloc(1024);
  info += sprintf(info, "\n\n");
  info += sprintf(info, "MDSplus version: %d.%d.%d\n", MdsVersion.MAJOR,
                  MdsVersion.MINOR, MdsVersion.MICRO);
  info += sprintf(info, "----------------------\n");
  const char *tag = MdsRelease();
  info += sprintf(info, "  Release:  %s\n", tag);
  info += sprintf(
      info, "  Browse:   https://github.com/MDSplus/mdsplus/tree/%s\n", tag);
  info += sprintf(
      info,
      "  Download: https://github.com/MDSplus/mdsplus/archive/%s.tar.gz\n",
      tag);
  info += sprintf(info, "\n\n");
  return 1;
}

EXPORT int mdsdcl_show_git_info(void *ctx, char **error, char **output)
{
  int status;
  int qfr[] = {cli_present(ctx, "TAG"), cli_present(ctx, "BRANCH"),
               cli_present(ctx, "REMOTE"), cli_present(ctx, "REMOTE_URL"),
               cli_present(ctx, "COMMIT"), cli_present(ctx, "SRCDIR")};

  // if no qualifiers have been set set all
  int i, s = 0;
  for (i = 0; i < 6; ++i)
    s += (qfr[i] == MdsdclPRESENT);
  if (s == 0)
    for (i = 0; i < 6; ++i)
      qfr[i] = MdsdclPRESENT;

  char *info = *output = malloc(strlen(MDSplusGitVersion.GIT_TAG) +
                                strlen(MDSplusGitVersion.GIT_BRANCH) +
                                strlen(MDSplusGitVersion.GIT_REMOTE) +
                                strlen(MDSplusGitVersion.GIT_REMOTE_URL) +
                                strlen(MDSplusGitVersion.GIT_COMMIT) +
                                strlen(MDSplusGitVersion.GIT_SRCDIR) + 1000);

  if (s)
  {
    if (qfr[0] == MdsdclPRESENT)
      info += sprintf(info, "%s ", MDSplusGitVersion.GIT_TAG);
    if (qfr[1] == MdsdclPRESENT)
      info += sprintf(info, "%s ", MDSplusGitVersion.GIT_BRANCH);
    if (qfr[2] == MdsdclPRESENT)
      info += sprintf(info, "%s ", MDSplusGitVersion.GIT_REMOTE);
    if (qfr[3] == MdsdclPRESENT)
      info += sprintf(info, "%s ", MDSplusGitVersion.GIT_REMOTE_URL);
    if (qfr[4] == MdsdclPRESENT)
      info += sprintf(info, "%s ", MDSplusGitVersion.GIT_COMMIT);
    if (qfr[5] == MdsdclPRESENT)
      info += sprintf(info, "%s ", MDSplusGitVersion.GIT_SRCDIR);
    info += sprintf(info, "\n");
  }
  else
  {
    info += sprintf(info, "\n MDSplus code info:\n");
    info += sprintf(info, "----------------------\n");
    info += sprintf(info, "  Tag:    %s\n", MDSplusGitVersion.GIT_TAG);
    info += sprintf(info, "  srcdir: %s\n", MDSplusGitVersion.GIT_SRCDIR);
    info += sprintf(info, "  Commit: %s\n", MDSplusGitVersion.GIT_COMMIT);
    info += sprintf(info, "  Branch: %s\n", MDSplusGitVersion.GIT_BRANCH);
    info += sprintf(info, "  Remote: %s\n", MDSplusGitVersion.GIT_REMOTE);
    info += sprintf(info, "  Url:    %s\n", MDSplusGitVersion.GIT_REMOTE_URL);
    info += sprintf(info, "\n\n");
  }
  status = MdsdclSUCCESS;
  *error = strdup("");
  return status;
}
