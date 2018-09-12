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
#include <STATICdef.h>
#include <string.h>
#include <stdlib.h>
#include <mdsdescrip.h>
#include <treeshr.h>
#include <mdsshr.h>
#include <ncidef.h>
#include "treeshrp.h"

extern void **TreeCtx();

typedef struct move_s{
char *from_c;
char *to_c;
char *from_d;
char *to_d;
} move_t;
static void freemove(void* move_p) {
  if (((move_t*)move_p)->from_c) free(((move_t*)move_p)->from_c);
  if (((move_t*)move_p)->to_c  ) free(((move_t*)move_p)->to_c  );
  if (((move_t*)move_p)->from_d) free(((move_t*)move_p)->from_d);
  if (((move_t*)move_p)->to_d  ) free(((move_t*)move_p)->to_d  );
}
static void treeclose(void* dbid_p) {
  _TreeClose(dbid_p, 0, 0);
  if (*(void**)dbid_p)
    free(*(void**)dbid_p);
}
STATIC_ROUTINE int RewriteDatafile(char *tree, int shot, int compress)
{
  int status, stat1;
  void *dbid1 = 0, *dbid2 = 0;
  char *tree_list = strcpy(malloc(strlen(tree) + 4), tree);
  strcat(tree_list, ",\"\"");
  status = _TreeOpen(&dbid1, tree_list, shot, 1);
  free(tree_list);
  if STATUS_OK {
    move_t move = {0};
    pthread_cleanup_push(freemove,&move);
    pthread_cleanup_push(treeclose,&dbid1);
    int stv;
    PINO_DATABASE *dblist1 = (PINO_DATABASE *) dbid1;
    TREE_INFO *info1 = dblist1->tree_info;
    status = TreeOpenNciW(dblist1->tree_info, 0);
    if STATUS_OK {
      TreeCallHook(dblist1, OpenNCIFileWrite, 0);
      status = TreeOpenDatafileW(dblist1->tree_info, &stv, 0);
      if STATUS_OK {
	TreeCallHook(dblist1, OpenDataFileWrite, 0);
	status = _TreeOpenEdit(&dbid2, tree, shot);
	if STATUS_OK {
	  pthread_cleanup_push(treeclose,&dbid2);
	  PINO_DATABASE *dblist2 = (PINO_DATABASE *) dbid2;
	  TREE_INFO *info2 = dblist2->tree_info;
	  status = TreeOpenNciW(dblist2->tree_info, 1);
	  if STATUS_OK {
	    dblist2->tree_info->edit->first_in_mem = dblist2->tree_info->header->nodes;
	    status = TreeOpenDatafileW(dblist2->tree_info, &stv, 1);
	    if STATUS_OK {
	      int i;
	      for (i = 0; i < info1->header->nodes; i++) {
		EMPTYXD(xd);
		EMPTYXD(mtxd);
		int first = 1;
		struct nci_list {
		  NCI nci;
		  struct nci_list *next;
		} *list = memset(malloc(sizeof(struct nci_list)), 0, sizeof(struct nci_list));
		TreeGetNciW(info1, i, &list->nci, 0);
		while (list->nci.flags & NciM_VERSIONS) {
		  struct nci_list *old_list = list;
		  list = malloc(sizeof(struct nci_list));
		  list->next = old_list;
		  TreeGetVersionNci(info1, &old_list->nci, &list->nci);
		}
		while (list) {
		  int64_t now = -1;
		  unsigned int oldlength;
		  struct nci_list *old_list = list;
		  TreeSetViewDate(&list->nci.time_inserted);
		  if (first) {
		    oldlength = list->nci.length;
		    list->nci.length = 0;
		    TreePutNci(info2, i, &list->nci, 1);
		    list->nci.length = oldlength;
		    first = 0;
		  }
		  if (list->nci.flags2 & NciM_EXTENDED_NCI) {
		    TreeCopyExtended(dbid1, dbid2, i, &list->nci, compress);
		  } else {
		    stat1 = _TreeGetRecord(dbid1, i, &xd);
		    TreeSetViewDate(&now);
		    if IS_OK(stat1) {
		      TreeSetTemplateNci(&list->nci);
		      stat1 = _TreePutRecord(dbid2, i, (struct descriptor *)&xd, compress ? 2 : 1);
		    } else if (stat1 == TreeBADRECORD || stat1 == TreeINVDFFCLASS) {
		      fprintf(stderr, "TreeBADRECORD, Clearing nid %d\n", i);
		      stat1 = _TreePutRecord(dbid2, i, (struct descriptor *)&mtxd, compress ? 2 : 1);
		    }
		    MdsFree1Dx(&xd, NULL);
		  }
		  list = list->next;
		  free(old_list);
		}
	      }
	      move.from_c = strcpy(malloc(strlen(info1->filespec) + 13), info1->filespec);
	      strcpy(move.from_c + strlen(info1->filespec) - 4, "characteristics#");
	      move.to_c = strcpy(malloc(strlen(info1->filespec) + 12), info1->filespec);
	      strcpy(move.to_c + strlen(info1->filespec) - 4, "characteristics");
	      move.from_d = strcpy(malloc(strlen(info1->filespec) + 6), info1->filespec);
	      strcpy(move.from_d + strlen(info1->filespec) - 4, "datafile#");
	      move.to_d = strcpy(malloc(strlen(info1->filespec) + 5), info1->filespec);
	      strcpy(move.to_d + strlen(info1->filespec) - 4, "datafile");
	    }
	  }
          pthread_cleanup_pop(1);//treeclose(&dbid2)
	}
      }
    }
    pthread_cleanup_pop(1);//treeclose(&dbid1)
    if STATUS_OK {
      status = MDS_IO_REMOVE(move.to_c) == 0 ? TreeNORMAL : TreeDELFAIL;
      if STATUS_OK
	status = MDS_IO_REMOVE(move.to_d) == 0 ? TreeNORMAL : TreeDELFAIL;
      if STATUS_OK
	status = ((MDS_IO_RENAME(move.from_c, move.to_c) == 0)
		  && (MDS_IO_RENAME(move.from_d, move.to_d) == 0)) ? TreeNORMAL : TreeRENFAIL;
    }
    pthread_cleanup_pop(1);//freemove(&move)
  }
  return status;
}

int TreeCleanDatafile(char *tree, int shot)
{
  return RewriteDatafile(tree, shot, 0);
}

int TreeCompressDatafile(char *tree, int shot)
{
  return RewriteDatafile(tree, shot, 1);
}

int _TreeCleanDatafile(void **dbid __attribute__ ((unused)), char *tree, int shot)
{
  return RewriteDatafile(tree, shot, 0);
}

int _TreeCompressDatafile(void **dbid __attribute__ ((unused)), char *tree, int shot)
{
  return RewriteDatafile(tree, shot, 1);
}
