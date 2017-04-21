#include <STATICdef.h>
#include <string.h>
#include <stdlib.h>
#include <mdsdescrip.h>
#include <treeshr.h>
#include <mdsshr.h>
#include <ncidef.h>
#include "treeshrp.h"

extern void **TreeCtx();

STATIC_ROUTINE int RewriteDatafile(char *tree, int shot, int compress)
{
  int status;
  void *dbid1 = 0, *dbid2 = 0;
  char *from_c = NULL;
  char *to_c = NULL;
  char *from_d = NULL;
  char *to_d = NULL;
  int lstatus;
  char *tree_list = strcpy(malloc(strlen(tree) + 4), tree);
  strcat(tree_list, ",\"\"");
  status = _TreeOpen(&dbid1, tree_list, shot, 1);
  free(tree_list);
  if (status & 1) {
    int stv;
    PINO_DATABASE *dblist1 = (PINO_DATABASE *) dbid1;
    TREE_INFO *info1 = dblist1->tree_info;
    status = TreeOpenNciW(dblist1->tree_info, 0);
    if (status & 1) {
      status = TreeOpenDatafileW(dblist1->tree_info, &stv, 0);
      if (status & 1) {
	status = _TreeOpenEdit(&dbid2, tree, shot);
	if (status & 1) {
	  PINO_DATABASE *dblist2 = (PINO_DATABASE *) dbid2;
	  TREE_INFO *info2 = dblist2->tree_info;
	  status = TreeOpenNciW(dblist2->tree_info, 1);
	  if (status & 1) {
	    dblist2->tree_info->edit->first_in_mem = dblist2->tree_info->header->nodes;
	    status = TreeOpenDatafileW(dblist2->tree_info, &stv, 1);
	    if (status & 1) {
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
		  int oldlength;
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
		    lstatus = _TreeGetRecord(dbid1, i, &xd);
		    TreeSetViewDate(&now);
		    if (lstatus & 1) {
		      TreeSetTemplateNci(&list->nci);
		      lstatus =
			  _TreePutRecord(dbid2, i, (struct descriptor *)&xd, compress ? 2 : 1);
		    } else if (lstatus == TreeBADRECORD || lstatus == TreeINVDFFCLASS) {
		      fprintf(stderr, "TreeBADRECORD, Clearing nid %d\n", i);
		      lstatus =
			  _TreePutRecord(dbid2, i, (struct descriptor *)&mtxd, compress ? 2 : 1);
		    }
		    MdsFree1Dx(&xd, NULL);
		  }
		  list = list->next;
		  free(old_list);
		}
	      }
	      from_c = strcpy(malloc(strlen(info1->filespec) + 20), info1->filespec);
	      strcpy(from_c + strlen(info1->filespec) - 4, "characteristics#");
	      to_c = strcpy(malloc(strlen(info1->filespec) + 20), info1->filespec);
	      strcpy(to_c + strlen(info1->filespec) - 4, "characteristics");
	      from_d = strcpy(malloc(strlen(info1->filespec) + 20), from_c);
	      strcpy(from_d + strlen(info1->filespec) - 4, "datafile#");
	      to_d = strcpy(malloc(strlen(info1->filespec) + 20), to_c);
	      strcpy(to_d + strlen(info1->filespec) - 4, "datafile");
	    }
	  }
	  _TreeClose(&dbid2, 0, 0);
	  if (dbid2)
	    free(dbid2);
	}
      }
    }
    _TreeClose(&dbid1, 0, 0);
    if (dbid1)
      free(dbid1);
    if (status & 1) {
      status = MDS_IO_REMOVE(to_c) == 0 ? TreeNORMAL : TreeDELFAIL;
      if (status & 1)
	status = MDS_IO_REMOVE(to_d) == 0 ? TreeNORMAL : TreeDELFAIL;
      if (status & 1)
	status = ((MDS_IO_RENAME(from_c, to_c) == 0)
		  && (MDS_IO_RENAME(from_d, to_d) == 0)) ? TreeNORMAL : TreeRENFAIL;
    }
    if (from_c)
      free(from_c);
    if (to_c)
      free(to_c);
    if (from_d)
      free(from_d);
    if (to_d)
      free(to_d);
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
