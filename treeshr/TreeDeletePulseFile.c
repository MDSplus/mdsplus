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

                Name:   TreeDeletePulseFile

                Type:   C function

                Author:	Thomas W. Fredian

                Date:   14-SEP-1989

                Purpose: Create pulse file from Model

------------------------------------------------------------------------------

        Call sequence:

int TreeDeletePulseFile(int shotid,int numnids, int *nids)

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

        Description:

------------------------------------------------------------------------------*/
#include "treeshrp.h" /* must be first or off_t wrong */
#include <ctype.h>
#include <fcntl.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <ncidef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <treeshr.h>

//#define DEBUG
#include <mdsmsg.h>

extern void **TreeCtx();

int TreeDeletePulseFile(int shotid, int allfiles)
{
  return _TreeDeletePulseFile(*TreeCtx(), shotid, allfiles);
}

static inline int TreeDeleteTreeFilesOne(char *tree, int shot, char *treepath)
{
  int status, i;
  int src[3];
  char *tmp[3] = {0};
  status = TreeSUCCESS;
  for (i = 0; i < 3; i++)
  {
    if (STATUS_OK)
    {
      status = MDS_IO_OPEN_ONE(treepath, tree, shot, i + TREE_TREEFILE_TYPE,
                               0, 0, &tmp[i], NULL, &src[i]);
      if (STATUS_OK && tmp[i])
        MDSDBG("%s -x\n", tmp[i]);
    }
    else
      src[i] = -1;
  }
  int retstatus = 0;
  for (i = 0; i < 3; i++)
  {
    if (src[i] >= 0)
      MDS_IO_CLOSE(src[i]);
    if (STATUS_OK)
      retstatus = MDS_IO_REMOVE(tmp[i]);
    free(tmp[i]);
  }
  return retstatus ? TreeFAILURE : status;
}

static inline int _TreeDeletePulseFiles(PINO_DATABASE *dblist, int shotid,
                                        char *treepath)
{
  void *dbid_tmp = NULL;
  int status = _TreeOpen(&dbid_tmp, dblist->experiment, shotid, 0);
  if (STATUS_NOT_OK)
    return status;
  void *ctx = 0;
  // push cleanup of ctx and dbid_tmp
  status = TreeSUCCESS;
  int shot = ((PINO_DATABASE *)dbid_tmp)->shotid;

  int nids[256], j, num;
  for (num = 0;
       num < 256 && _TreeFindTagWild(dbid_tmp, "TOP", &nids[num], &ctx);
       num++)
  {
    int sts;
    char name[13];
    if (nids[num])
    { // for subtree nodes, i.e. nid!=0
      NCI_ITM itmlst[] = {{sizeof(name) - 1, NciNODE_NAME, 0, 0},
                          {0, NciEND_OF_LIST, 0, 0}};
      itmlst[0].pointer = name;
      sts = _TreeGetNci(dbid_tmp, nids[num], itmlst);
      // cut at first space and/or terminate with \0
      for (j = 0; j < 12 && name[j] != ' '; j++)
        ;
      name[j] = '\0';
    }
    else
    {
      strcpy(name, dblist->experiment);
      sts = 1;
    }
    if (IS_OK(sts))
      sts = TreeDeleteTreeFilesOne(name, shot, treepath);
    if (IS_NOT_OK(sts))
    {
      status = sts;
      break;
    }
  }
  // pop cleanup of ctx and dbid_tmp
  {
    TreeFindTagEnd(&ctx);
    _TreeClose(&dbid_tmp, dblist->experiment, shotid);
    free(dbid_tmp);
  }
  return status;
}

int _TreeDeletePulseFile(void *dbid, int shotid,
                         int allfiles __attribute__((unused)))
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  /* Make sure tree is open */
  int status = _TreeIsOpen(dblist);
  if (status != TreeOPEN)
    return status;
  if (dblist->remote)
    return TreeUNSUPTHICKOP;
  INIT_AND_FREE_ON_EXIT(char *, treepath);
  TREE_INFO *info = dblist->tree_info;
  if (info && info->filespec && info->speclen > 2 &&
      info->filespec[info->speclen - 1] == ':' &&
      info->filespec[info->speclen - 2] == ':')
  {
    treepath = memcpy(malloc(info->speclen + 1), info->filespec, info->speclen);
    treepath[info->speclen] = '\0';
  }
  status = _TreeDeletePulseFiles(dblist, shotid, treepath);
  FREE_NOW(treepath);
  return status;
}
