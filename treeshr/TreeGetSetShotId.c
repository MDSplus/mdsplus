/*------------------------------------------------------------------------------

		Name:   TreeGetCurrentShotId

		Type:   C function

		Author:	Thomas W. Fredian

		Date:   11-OCT-1989

		Purpose: Get current shot number

------------------------------------------------------------------------------

	Call sequence:

int TreeGetCurrentShotId(experiment,shot)

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
------------------------------------------------------------------------------*/
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <libroutines.h>
#include <strroutines.h>
#include <stdlib.h>
#include <sys/stat.h>

extern void TranslateLogicalFree();
extern int TreeGetCurrentShotIdRemote();
extern int TreeSetCurrentShotIdRemote();
#ifndef index
extern char *index();
#endif

extern char *TranslateLogical();

static char *cvsrev = "@(#)$RCSfile$ $Revision$ $Date$";

#define _ToLower(c) (((c) >= 'A' && (c) <= 'Z') ? (c) | 0x20 : (c))

static char *GetFileName(char *experiment,char **ctx)
{
  char *ans = 0;
  static char pathname[550];
  static char *path;
  FILE *file = 0;
  char *semi = 0;
  char *part;
  if (*ctx == NULL)
  {
    if (path != NULL)
      TranslateLogicalFree(path);
    strcpy(pathname,experiment);
    strcat(pathname,"_path");
    path = (char *)TranslateLogical(pathname);
    part = path;
  }
  else if (*ctx == pathname)
    return NULL;
  else
    part = *ctx;
  if (part != NULL)
  {
    if ((semi = (char *)index(part, ';')) != 0)
      *semi = '\0';
    strncpy(pathname,part,500);
    if (semi == 0) 
      *ctx = pathname;
    else
      *ctx = part + strlen(part) + 1;
#ifdef _WINDOWS
    strcat(pathname,"\\");
#else
    strcat(pathname,"/");
#endif
    strcat(pathname,"shotid.sys");
    MaskReplace(pathname,experiment,0);
    ans = pathname;
  }
  return ans;
}

static FILE *CreateShotIdFile(char *experiment)
{
  FILE *file = NULL;
  char *ctx = 0;
  char *filename;
  while ((file == NULL) && (filename = GetFileName(experiment,&ctx)))
    file = fopen(filename,"w+b");
  return file;
}

static FILE *OpenShotIdFile(char *experiment,char *mode)
{
  FILE *file = NULL;
  char *ctx = 0;
  char *filename;
  int found = 0;
  struct stat statbuf;
  while ((filename = GetFileName(experiment,&ctx)) && !(found=(stat(filename,&statbuf) == 0)));
  return (found ? fopen(filename,mode) : CreateShotIdFile(experiment));
}


int       TreeGetCurrentShotId(char *experiment)
{
  int shot = 0;
  int status = 0;
  char *path = 0;
  char *exp = strcpy(malloc(strlen(experiment)+6),experiment);
  int i;
  int slen;
  for (i=0;exp[i] != '\0';i++)
    exp[i] = _ToLower(exp[i]);
  strcat(exp,"_path");
  path = TranslateLogical(exp);
  exp[strlen(experiment)]='\0';
  if (path && ((slen = strlen(path)) > 2) && (path[slen-1] == ':') && (path[slen-2] == ':'))
    status = TreeGetCurrentShotIdRemote(exp, path, &shot);
  else
  {
    FILE *file = OpenShotIdFile(exp,"rb");
    if (file)
    {
      status = fread(&shot,sizeof(shot),1,file) == 1;
      fclose(file);
#ifdef WORDS_BIGENDIAN
      if (status & 1)
      {
        int lshot = shot;
        int i;
        char *optr = (char *)&shot;
        char *iptr = (char *)&lshot;
        for (i=0;i<4;i++) optr[i] = iptr[3-i];
      }
#endif
    }
  }
  if (path)
    TranslateLogicalFree(path);
  free(exp);
  return (status & 1) ? shot : 0;
}

int       TreeSetCurrentShotId(char *experiment, int shot)
{
  int status = 0;
  char *path = 0;
  char *exp = strcpy(malloc(strlen(experiment)+6),experiment);
  int slen;
  int i;
  for (i=0;exp[i] != '\0';i++)
    exp[i] = _ToLower(exp[i]);
  strcat(exp,"_path");
  path = TranslateLogical(exp);
  if (path && ((slen = strlen(path)) > 2) && (path[slen-1] == ':') && (path[slen-2] == ':'))
    status = TreeSetCurrentShotIdRemote(experiment, path, shot);
  else
  {
    FILE *file = OpenShotIdFile(experiment,"r+b");
    if (file)
    {
      int lshot = shot;
#ifdef WORDS_BIGENDIAN
      int i;
      char *optr = (char *)&lshot;
      char *iptr = (char *)&shot;
      for (i=0;i<4;i++) optr[i] = iptr[3-i];
#endif
      status = fwrite(&lshot,sizeof(shot),1,file) == 1;
      fclose(file);
    }
  }
  if (path)
    TranslateLogicalFree(path);
  free(exp);
  return status;
}

