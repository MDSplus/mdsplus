#include <mdsdescrip.h>
#include <treeshr.h>
#include "treeshrp.h"
#include <string.h>
#include <stdlib.h>

extern void *DBID;


static int RewriteDatafile(void **dbid, char *tree, int shot, int compress)
{
  int status;
  void *dbid1 = 0, *dbid2 = 0;
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
              char *from;
              char *to;
              int i;
              int lstatus;
              for (i=0;i<info1->header->nodes;i++)
              {
                EMPTYXD(xd);
                NCI nci;
                TreeGetNciW(info1, i, &nci);
                TreePutNci(info2, i, &nci, 1);
                lstatus = _TreeGetRecord(dbid1, i, &xd);
                lstatus = _TreePutRecord(dbid2, i, (struct descriptor *)&xd, compress ? 2 : 1);
              }
              from = strcpy(malloc(strlen(info1->filespec)+20),info1->filespec);
              strcpy(from+strlen(info1->filespec)-4,"characteristics#");
              to = strcpy(malloc(strlen(info1->filespec)+20),info1->filespec);
              strcpy(to+strlen(info1->filespec)-4,"characteristics");
              lstatus = rename(from,to);
              strcpy(from+strlen(info1->filespec)-4,"datafile#");
              strcpy(to+strlen(info1->filespec)-4,"datafile");
              lstatus = rename(from,to);
              free(from);
              free(to);
            }
          }
          _TreeClose(&dbid2, 0, 0);
        }
      }
    }
    _TreeClose(&dbid1, 0, 0);
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
