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
#include "treeshrp.h" /* must be first of off_t wrong */
#include <ctype.h>
#include <fcntl.h>
#include <libroutines.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <ncidef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <strroutines.h>
#include <treeshr.h>
extern int TreeGetCurrentShotIdRemote();
extern int TreeSetCurrentShotIdRemote();
extern char *MaskReplace();

extern char *TreePath();

static char * PathToFileName(const char * experiment, char * path)
{
  static char filename[1024];
  char * tmp = MaskReplace(path, experiment, 0);
  strcpy(filename, tmp);
  free(tmp);
  
  // (slwalsh, jas, fsantoro) NOTE: We have no idea if this is even used anymore, possibly remove?
  if (filename[strlen(filename) - 1] == '+')
  {
    size_t i;
    for (i = strlen(filename); (i > 0) && (filename[i - 1] != TREE_PATH_DELIM[0]); i--)
      ;
    if (i > 0)
      filename[i] = 0;
  }
  else
  {
    if (filename[strlen(filename) - 1] != TREE_PATH_DELIM[0])
      strcat(filename, TREE_PATH_DELIM);
  }
  strcat(filename, "shotid.sys");
  
  return filename;
}

int ReadShotId(char * filename, int * shot)
{
  int status = TreeFAILURE;

  int fd = MDS_IO_OPEN(filename, O_RDONLY, 0);
  if (fd != -1) {
    status = MDS_IO_READ(fd, shot, sizeof(*shot)) == sizeof(*shot);
    MDS_IO_CLOSE(fd);
  #ifdef WORDS_BIGENDIAN
    if (STATUS_OK)
    {
      int lshot = shot;
      int i;
      char *optr = (char *)shot;
      char *iptr = (char *)lshot;
      for (i = 0; i < 4; i++)
        optr[i] = iptr[3 - i];
    }
  #endif
  }
  
  return status;
}

int WriteShotId(char * filename, int shot, int mode)
{
  int status = TreeFAILURE;

  int fd = MDS_IO_OPEN(filename, mode, 0664);
  if (fd != -1) {
    int lshot = shot;
  #ifdef WORDS_BIGENDIAN
    int i;
    char *optr = (char *)&lshot;
    char *iptr = (char *)&shot;
    for (i = 0; i < 4; i++)
      optr[i] = iptr[3 - i];
  #endif
    status = MDS_IO_WRITE(fd, &lshot, sizeof(shot)) == sizeof(shot);
    MDS_IO_CLOSE(fd);
  }
  
  return status;
}

int TreeGetCurrentShotId(char const *experiment)
{
  int shot = 0;
  int status = TreeFAILURE;
  char experiment_lower[16] = {0};
  size_t slen;
  char * pathlist = TreePath(experiment, experiment_lower);
  char * filename;
  char * saveptr = NULL;
  char * path = strtok_r(pathlist, TREE_PATH_LIST_DELIM, &saveptr);
  while (path) {
    slen = strlen(path);
    bool thick = (slen > 2) && (path[slen - 1] == ':') && (path[slen - 2] == ':');
    if (thick)
    {
      path[slen - 2] = 0;
      status = TreeGetCurrentShotIdRemote(experiment_lower, path, &shot);
    }
    else
    {
      filename = PathToFileName(experiment_lower, path);
      
      if (MDS_IO_EXISTS(filename)) {
        status = ReadShotId(filename, &shot);
      }
    }
    
    if (STATUS_OK) {
      break;
    }
    
    path = strtok_r(NULL, TREE_PATH_LIST_DELIM, &saveptr);
  }
  free(pathlist);
  return STATUS_OK ? shot : 0;
}

int TreeSetCurrentShotId(char const *experiment, int shot)
{
  int status = TreeFAILURE;
  char experiment_lower[16] = {0};
  size_t slen;
  char * pathlist = TreePath(experiment, experiment_lower);
  char * filename;
  char * saveptr = NULL;
  char * path = strtok_r(pathlist, TREE_PATH_LIST_DELIM, &saveptr);
  while (path) {
    slen = strlen(path);
    bool thick = (slen > 2) && (path[slen - 1] == ':') && (path[slen - 2] == ':');
    if (thick)
    {
      path[slen - 2] = 0;
      status = TreeSetCurrentShotIdRemote(experiment_lower, path, shot);
    }
    else
    {
      filename = PathToFileName(experiment_lower, path);
      
      if (MDS_IO_EXISTS(filename)) {
        status = WriteShotId(filename, shot, O_WRONLY);
      }
    }
    
    if (STATUS_OK) {
      break;
    }
    
    path = strtok_r(NULL, TREE_PATH_LIST_DELIM, &saveptr);
  }
  free(pathlist);
  
  // (slwalsh) NOTE: This will potentially create a shotid.sys on a remote host even if 
  // there is one further down the path, recommend only putting thick client at the end
  // of tree paths.
  if (STATUS_NOT_OK) {
    pathlist = TreePath(experiment, experiment_lower);
    saveptr = NULL;
    path = strtok_r(pathlist, TREE_PATH_LIST_DELIM, &saveptr);
    while (path) {
      status = TreeFAILURE;
      
      slen = strlen(path);
      bool thick = (slen > 2) && (path[slen - 1] == ':') && (path[slen - 2] == ':');
      if (!thick) {
        filename = PathToFileName(experiment_lower, path);

        status = WriteShotId(filename, shot, O_RDWR | O_CREAT | O_TRUNC);
      }
      
      if (STATUS_OK) {
        break;
      }

      path = strtok_r(NULL, TREE_PATH_LIST_DELIM, &saveptr);
    }
    free(pathlist);
  }

  return status;
}
