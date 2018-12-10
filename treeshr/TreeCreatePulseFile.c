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

		Name:   TreeCreatePulseFile

		Type:   C function

		Author:	Thomas W. Fredian

		Date:   14-SEP-1989

		Purpose: Create pulse file from Model

------------------------------------------------------------------------------

	Call sequence:

int TreeCreatePulseFile(int shotid,int numnids, int *nids)

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------*/
#include "treeshrp.h"		/* must be first or off_t is defined wrong */
#include <STATICdef.h>
#include <stdlib.h>
#include <string.h>
#include <mdsdescrip.h>
#include <sys/stat.h>
#include <mdsshr.h>
#include <ncidef.h>
#include <treeshr.h>
#include <ctype.h>

#ifdef DEBUG
# define DBG(...) fprintf(stderr, __VA_ARGS__)
#else
# define DBG(...) {}
#endif

extern char *MaskReplace();

STATIC_ROUTINE int _CopyFile(int src_fd, int dst_fd, int lock_it);

extern void **TreeCtx();

int TreeCreatePulseFile(int shotid, int numnids_in, int *nids_in)
{
  return _TreeCreatePulseFile(*TreeCtx(), shotid, numnids_in, nids_in);
}

int _TreeCreatePulseFile(void *dbid, int shotid, int numnids_in, int *nids_in)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  INIT_STATUS, retstatus = MDSplusERROR;
  int num;
  int nids[256];
  int i;
  int j;
  int shot;
  int source_shot;

/* Make sure tree is open */

  if ((status = _TreeIsOpen(dblist)) != TreeOPEN)
    return status;

  source_shot = dblist->shotid;
  if (numnids_in == 0) {
    void *ctx = 0;
    for (num = 0; num < 256 && _TreeFindTagWild(dbid, "TOP", &nids[num], &ctx); num++);
    TreeFindTagEnd(&ctx);
    /*
    nids[0] = 0;
    for (num = 1;
	 num < 256 && (_TreeFindNodeWild(dbid, "***", &nids[num], &ctx, (1 << TreeUSAGE_SUBTREE)) & 1);
	 num++) ;
    TreeFindNodeEnd(&ctx);
    */
  } else {
    num = 0;
    for (i = 0; i < numnids_in; i++) {
      for (j = 0; j < num; j++)
	if (nids[j] == nids_in[i])
	  break;
      if (j == num)
	nids[num++] = nids_in[i];
    }
  }
  if (dblist->remote)
    return CreatePulseFileRemote(dblist, shotid, nids, num);
  if (shotid)
    shot = shotid;
  else
    shot = TreeGetCurrentShotId(dblist->experiment);

  retstatus = status;
  if STATUS_OK {
    for (i = 0; i < num && (retstatus & 1); i++) {
      int skip = 0;
      char name[13];
      if (nids[i]) {
	int flags;
	NCI_ITM itmlst[] = { {sizeof(name) - 1, NciNODE_NAME, 0, 0}
	, {4, NciGET_FLAGS, &flags, 0}
	, {0, NciEND_OF_LIST, 0, 0}
	};
	itmlst[0].pointer = name;
	status = _TreeGetNci(dbid, nids[i], itmlst);
	if (numnids_in == 0)
	  skip = (flags & NciM_INCLUDE_IN_PULSE) == 0;
	name[12] = 0;
	for (j = 11; j > 0; j--)
	  if (name[j] == 0x20)
	    name[j] = '\0';
      } else {
	strcpy(name, dblist->experiment);
      }
      if (STATUS_OK && !(skip))
	status = TreeCreateTreeFiles(name, shot, source_shot);
      if (STATUS_NOT_OK && (i == 0))
	retstatus = status;
    }
  }
  return retstatus;
}

int TreeCreateTreeFiles(char *tree, int shot, int source_shot){
  if (!source_shot || (source_shot < -1)) return TreeINVSHOT;
  if (!shot        || (shot        < -1)) return TreeINVSHOT;
  int status,i;
  int src[3],dst[3];
  INIT_AND_FREE_ON_EXIT(char*,tmp);
  status = TreeSUCCESS;
  for (i = 0 ; i < 3 ; i++) {
    if STATUS_OK {
      status = MDS_IO_OPEN_ONE(NULL,tree,source_shot,i+TREE_TREEFILE_TYPE,0,0,&tmp,&src[i]);
      if (tmp) {
	if STATUS_OK DBG("%s ->\n",tmp);
	free(tmp);
	tmp = NULL;
      }
    } else  src[i] = -1;
  }
  for (i = 0 ; i < 3 ; i++) {
    if STATUS_OK {
      status = MDS_IO_OPEN_ONE(NULL,tree,shot,i+TREE_TREEFILE_TYPE,1,0,&tmp,&dst[i]);
      if (tmp) {
	if STATUS_OK DBG("%s <-\n",tmp);
	free(tmp);
	tmp = NULL;
      }
    } else dst[i] = -1;
  }
  FREE_NOW(filespec);
  for (i = 0 ; i < 3 ; i++) {
    if STATUS_OK status = _CopyFile(src[i],dst[i],i>0);
    if (src[i]>=0) MDS_IO_CLOSE(src[i]);
    if (dst[i]>=0) MDS_IO_CLOSE(dst[i]);
  }
  return status;
}

#define MAX_CHUNK 1024*1024*256
#define MIN(a,b) ((a) < (b))?(a):(b)
#define MAX(a,b) ((a) > (b))?(a):(b)

STATIC_ROUTINE int _CopyFile(int src_fd, int dst_fd, int lock_it)
{
  INIT_STATUS_ERROR;
  if (src_fd != -1) {
    ssize_t src_len = MDS_IO_LSEEK(src_fd, 0, SEEK_END);
    if ((dst_fd != -1) && (src_len != -1)) {
      MDS_IO_LSEEK(src_fd, 0, SEEK_SET);
      if (lock_it)
	MDS_IO_LOCK(src_fd, 0, (size_t)src_len, MDS_IO_LOCK_RD, 0);
      if (src_len > 0) {
	size_t chunk_size = (size_t) (MIN(MAX_CHUNK, src_len));
	void *buff = malloc(chunk_size);
	size_t bytes_to_go = (size_t) src_len;
	while (bytes_to_go > 0) {
	  size_t io_size = MIN(bytes_to_go, chunk_size);
	  ssize_t bytes_read = MDS_IO_READ(src_fd, buff, io_size);
	  if (bytes_read == (ssize_t)io_size) {
	    ssize_t bytes_written = MDS_IO_WRITE(dst_fd, buff, io_size);
	    if (bytes_written != (ssize_t)io_size)
	      break;
	  }
	  bytes_to_go -= io_size;
	}
	if (buff)
	  free(buff);
	if (bytes_to_go == 0)
	  status = TreeSUCCESS;
      } else
	status = TreeSUCCESS;
      if (lock_it)
	MDS_IO_LOCK(src_fd, 0, (size_t)src_len, MDS_IO_LOCK_NONE, 0);
    } else
      status = TreeFCREATE;
  } else
    status = TreeFOPENR;
  return status;
}
