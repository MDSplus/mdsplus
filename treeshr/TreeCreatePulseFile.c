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
#include <usagedef.h>

extern char *MaskReplace();

STATIC_ROUTINE int _CopyFile(char *src, char *dst, int lock_it);
#include <fcntl.h>
#ifdef _WIN32
#include <windows.h>
#else
#ifndef O_BINARY
#define O_BINARY 0
#endif
#ifndef O_RANDOM
#define O_RANDOM 0
#endif
#endif

extern void **TreeCtx();

int TreeCreatePulseFile(int shotid, int numnids_in, int *nids_in)
{
  return _TreeCreatePulseFile(*TreeCtx(), shotid, numnids_in, nids_in);
}

int _TreeCreatePulseFile(void *dbid, int shotid, int numnids_in, int *nids_in)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  int status = 1;
  int retstatus;
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
    nids[0] = 0;
    for (num = 1;
	 num < 256
	 && (_TreeFindNodeWild(dbid, "***", &nids[num], &ctx, (1 << TreeUSAGE_SUBTREE)) & 1);
	 num++) ;
    TreeFindNodeEnd(&ctx);
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
  if (status & 1) {
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
      if (status & 1 && !(skip))
	status = TreeCreateTreeFiles(name, shot, source_shot);
      if (!(status & 1) && (i == 0))
	retstatus = status;
    }
  }
  return retstatus;
}

int TreeCreateTreeFiles(char *tree, int shot, int source_shot)
{
  size_t len = strlen(tree);
  char tree_lower[13];
  char pathname[32];
  char *path;
  char *pathin;
  size_t pathlen;
  char name[32];
  size_t i;
  int status = 1;
  int itype;
  char *types[] = { ".tree", ".characteristics", ".datafile" };
  for (i = 0; i < len && i < 12; i++)
    tree_lower[i] = tolower(tree[i]);
  tree_lower[i] = 0;
  strcpy(pathname, tree_lower);
  strcat(pathname, TREE_PATH_SUFFIX);
  pathin = TranslateLogical(pathname);
  if (pathin) {
    pathlen = strlen(pathin);
    for (itype = 0; itype < 3 && (status & 1); itype++) {
      char *srcfile = 0;
      char *dstfile = 0;
      char *type = types[itype];
      char *part;
      path = MaskReplace(pathin, tree_lower, source_shot);
      pathlen = strlen(path);
      if (source_shot > 999)
	sprintf(name, "%s_%d", tree_lower, source_shot);
      else if (source_shot > 0)
	sprintf(name, "%s_%03d", tree_lower, source_shot);
      else if (source_shot == -1)
	sprintf(name, "%s_model", tree_lower);
      else
	return TreeINVSHOT;
      for (i = 0, part = path; i < pathlen + 1; i++) {
	if (*part == ' ')
	  part++;
	else if ((path[i] == ';' || path[i] == 0) && strlen(part)) {
	  path[i] = 0;
	  srcfile = strcpy(malloc(strlen(part) + strlen(name) + strlen(type) + 2), part);
	  if (srcfile[strlen(srcfile) - 1] == '+') {
	    srcfile[strlen(srcfile) - 1] = '\0';
	  } else {
	    if (strcmp(srcfile + strlen(srcfile) - 1, TREE_PATH_DELIM))
	      strcat(srcfile, TREE_PATH_DELIM);
	    strcat(srcfile, name);
	  }
	  strcat(srcfile, type);
	  if (MDS_IO_EXISTS(srcfile))
	    break;
	  else {
	    free(srcfile);
	    srcfile = 0;
	    part = &path[i + 1];
	  }
	}
      }
      free(path);
      if (srcfile) {
	path = MaskReplace(pathin, tree_lower, shot);
	pathlen = strlen(path);
	if (shot > 999)
	  sprintf(name, "%s_%d", tree_lower, shot);
	else if (shot > 0)
	  sprintf(name, "%s_%03d", tree_lower, shot);
	else if (shot == -1)
	  sprintf(name, "%s_model", tree_lower);
	else
	  return TreeINVSHOT;

	for (i = 0, part = path; i < pathlen + 1; i++) {
	  if (*part == ' ')
	    part++;
	  else if ((path[i] == ';' || path[i] == 0) && strlen(part)) {
	    path[i] = 0;
	    dstfile = strcpy(malloc(strlen(part) + strlen(name) + strlen(type) + 2), part);
	    if (dstfile[strlen(dstfile) - 1] == '+') {
	      char *delim = TREE_PATH_DELIM;
	      int j = (int)(strlen(dstfile) - 1);
	      dstfile[j] = '\0';
	      for (j--; j >= 0 && dstfile[j] != delim[0]; j--) ;
	      if (j >= 0) {
		dstfile[j] = 0;
		if (MDS_IO_EXISTS(dstfile)) {
		  dstfile[j] = delim[0];
		  strcat(dstfile, type);
		  break;
		}
	      }
	    } else {
	      if (strcmp(dstfile + strlen(dstfile) - 1, TREE_PATH_DELIM) == 0)
		*(dstfile + strlen(dstfile) - 1) = 0;
	      if (MDS_IO_EXISTS(dstfile)) {
		strcat(dstfile, TREE_PATH_DELIM);
		strcat(dstfile, name);
		strcat(dstfile, type);
		break;
	      }
	    }
	    free(dstfile);
	    dstfile = 0;
	    part = &path[i + 1];
	  }
	}
	free(path);
	if (dstfile) {
	  status = _CopyFile(srcfile, dstfile, itype != 0);
	  free(dstfile);
	} else
	  status = TreeINVPATH;
	free(srcfile);
      } else
	status = TreeTREENF;
    }
    TranslateLogicalFree(pathin);
  } else
    status = TreeINVPATH;
  return status;
}

#define MAX_CHUNK 1024*1024*256
#define MIN(a,b) ((a) < (b))?(a):(b)
#define MAX(a,b) ((a) > (b))?(a):(b)

STATIC_ROUTINE int _CopyFile(char *src, char *dst, int lock_it)
{
  int status;

  int src_fd = MDS_IO_OPEN(src, O_RDONLY | O_BINARY | O_RANDOM, 0);
  if (src_fd != -1) {
    ssize_t src_len = MDS_IO_LSEEK(src_fd, 0, SEEK_END);
    int dst_fd = MDS_IO_OPEN(dst, O_RDWR | O_CREAT | O_TRUNC, 0664);
    if ((dst_fd != -1) && (src_len != -1)) {
      MDS_IO_LSEEK(src_fd, 0, SEEK_SET);
      if (lock_it)
	MDS_IO_LOCK(src_fd, 0, (int)src_len, MDS_IO_LOCK_RD, 0);
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
	MDS_IO_LOCK(src_fd, 0, (int)src_len, MDS_IO_LOCK_NONE, 0);
      MDS_IO_CLOSE(dst_fd);
    } else
      status = TreeFCREATE;
    MDS_IO_CLOSE(src_fd);
  } else
    status = TreeFOPENR;
  return status;
}
