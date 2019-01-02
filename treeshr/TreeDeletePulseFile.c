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
#include "treeshrp.h"		/* must be first or off_t wrong */
#include <STATICdef.h>
#include <fcntl.h>
#include <STATICdef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <ncidef.h>
#include <sys/stat.h>
#include <ctype.h>

#define DEBUG
#ifdef DEBUG
# define DBG(...) fprintf(stderr, __VA_ARGS__)
#else
# define DBG(...) {}
#endif

extern char *MaskReplace();

extern void **TreeCtx();
STATIC_ROUTINE int TreeDeleteTreeFiles(char *tree, int shot);

int TreeDeletePulseFile(int shotid, int allfiles)
{
  return _TreeDeletePulseFile(*TreeCtx(), shotid, allfiles);
}

int _TreeDeletePulseFile(void *dbid, int shotid, int allfiles __attribute__ ((unused)))
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  int status = 1;
  int retstatus = 1;
  int num;
  int nids[256];
  int j;
  int shot;
  void *dbid_tmp = 0;

/* Make sure tree is open */

  if ((status = _TreeIsOpen(dblist)) != TreeOPEN)
    return status;

  status = _TreeOpen(&dbid_tmp, dblist->experiment, shotid, 0);
  if (status & 1) {
    void *ctx = 0;
    shot = ((PINO_DATABASE *) dbid_tmp)->shotid;
    for (num = 0; num < 256 && _TreeFindTagWild(dbid_tmp, "TOP", &nids[num], &ctx); num++) {
      char name[13];
      if (nids[num]) {
	NCI_ITM itmlst[] = { {sizeof(name) - 1, NciNODE_NAME, 0, 0}
	, {0, NciEND_OF_LIST, 0, 0}
	};
	itmlst[0].pointer = name;
	status = _TreeGetNci(dbid_tmp, nids[num], itmlst);
	name[12] = 0;
	for (j = 11; j > 0; j--)
	  if (name[j] == 0x20)
	    name[j] = '\0';
      } else {
	strcpy(name, dblist->experiment);
      }
      if STATUS_OK
	status = TreeDeleteTreeFiles(name, shot);
      if STATUS_NOT_OK
	retstatus = status;
    }
    _TreeClose(&dbid_tmp, dblist->experiment, shotid);
    free(dbid_tmp);
    TreeFindTagEnd(&ctx);
  }
  return retstatus;
}

static int TreeDeleteTreeFiles(char *tree, int shot){
  if (!shot        || (shot        < -1)) return TreeINVSHOT;
  int status,i;
  int src[3];
  char* tmp[3] = {0};
  status = TreeSUCCESS;
  for (i = 0 ; i < 3 ; i++) {
    if STATUS_OK {
      status = MDS_IO_OPEN_ONE(NULL,tree,shot,i+TREE_TREEFILE_TYPE,0,0,&tmp[i],NULL,&src[i]);
      if (STATUS_OK && tmp[i]) DBG("%s -x\n",tmp[i]);
    } else  src[i] = -1;
  }
  int retstatus = 0;
  for (i = 0 ; i < 3 ; i++) {
    if (src[i]>=0) MDS_IO_CLOSE(src[i]);
    if STATUS_OK retstatus = MDS_IO_REMOVE(tmp[i]);
    if (tmp[i]) free(tmp[i]);
  }
  return retstatus ? TreeFAILURE : status;
}
