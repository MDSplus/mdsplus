#include <STATICdef.h>
#include <string.h>
#include <stdlib.h>
#include <mdsdescrip.h>
#include <treeshr.h>
#include "treeshrp.h"

extern void *DBID;


STATIC_ROUTINE int RewriteDatafile(void **dbid, char *tree, int shot, int compress)
{
  int status;
  void *dbid1 = 0, *dbid2 = 0;
  char *from_c = NULL;
  char *to_c = NULL;
  char *from_d = NULL;
  char *to_d = NULL;
  int lstatus;
  status = _TreeOpenEdit(&dbid1, tree, shot);
  if (status & 1)
  {
    int stv;
    PINO_DATABASE *dblist1 = (PINO_DATABASE *)dbid1;
    TREE_INFO *info1 = dblist1->tree_info;
    status = TreeOpenNciW(dblist1->tree_info, 0);
    if (status & 1)
    {
      status = TreeOpenDatafileW(dblist1->tree_info, &stv, 0);
      if (status & 1)
      {
        status = _TreeOpenEdit(&dbid2, tree, shot);
        if (status & 1)
        {
          PINO_DATABASE *dblist2 = (PINO_DATABASE *)dbid2;
          TREE_INFO *info2 = dblist2->tree_info;
          status = TreeOpenNciW(dblist2->tree_info, 1);
          if (status & 1)
          {
            dblist2->tree_info->edit->first_in_mem = dblist1->tree_info->edit->first_in_mem;
            status = TreeOpenDatafileW(dblist2->tree_info, &stv, 1);
            if (status & 1)
            {
              int i;
              for (i=0;i<info1->header->nodes;i++)
              {
                EMPTYXD(xd);
                EMPTYXD(mtxd);
                NCI nci;
                TreeGetNciW(info1, i, &nci);
                TreePutNci(info2, i, &nci, 1);
                lstatus = _TreeGetRecord(dbid1, i, &xd);
                if (lstatus & 1)
                  lstatus = _TreePutRecord(dbid2, i, (struct descriptor *)&xd, compress ? 2 : 1);
                else if (lstatus == TreeBADRECORD || lstatus == TreeINVDFFCLASS)
		{
                  fprintf(stderr,"TreeBADRECORD, Clearing nid %d\n",i);
                  lstatus = _TreePutRecord(dbid2, i, (struct descriptor *)&mtxd, compress ? 2 : 1);
                }
                MdsFree1Dx(&xd,NULL);
                
              }
              from_c = strcpy(malloc(strlen(info1->filespec)+20),info1->filespec);
              strcpy(from_c+strlen(info1->filespec)-4,"characteristics#");
              to_c = strcpy(malloc(strlen(info1->filespec)+20),info1->filespec);
              strcpy(to_c+strlen(info1->filespec)-4,"characteristics");
			  from_d = strcpy(malloc(strlen(info1->filespec)+20),from_c);
              strcpy(from_d+strlen(info1->filespec)-4,"datafile#");
			  to_d = strcpy(malloc(strlen(info1->filespec)+20),to_c);
              strcpy(to_d+strlen(info1->filespec)-4,"datafile");
            }
          }
          _TreeClose(&dbid2, 0, 0);
        }
      }
    }
    _TreeClose(&dbid1, 0, 0);
    if (status & 1)
    {
      status = remove(to_c) == 0 ? TreeNORMAL : TreeFAILURE;
      if (status & 1)
	status = remove(to_d) == 0 ? TreeNORMAL : TreeFAILURE;
      if (status & 1)
      status = ((rename(from_c,to_c) == 0) && (rename(from_d,to_d) == 0)) ? TreeNORMAL : TreeFAILURE;
    }
    if (from_c) free(from_c);
    if (to_c) free(to_c);
    if (from_d) free(from_d);
    if (to_d) free(to_d);
  }
  return status;
}

int TreeCleanDatafile(char *tree, int shot)
{
  return RewriteDatafile(&DBID, tree, shot, 0);
}

int TreeCompressDatafile(char *tree, int shot)
{
  return RewriteDatafile(&DBID, tree, shot, 1);
}

int _TreeCleanDatafile(void **dbid, char *tree, int shot)
{
  return RewriteDatafile(dbid, tree, shot, 0);
}

int _TreeCompressDatafile(void **dbid, char *tree, int shot)
{
  return RewriteDatafile(dbid, tree, shot, 1);
}
