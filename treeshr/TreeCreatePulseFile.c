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
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include "treeshrp.h"
#include <ncidef.h>

#define __tolower(c) (((c) >= 'A' && (c) <= 'Z') ? (c) | 0x20 : (c))

extern void *DBID;

int       TreeCreatePulseFile(int shotid, int numnids_in, int *nids_in)
{
  return _TreeCreatePulseFile(DBID, shotid, numnids_in, nids_in);
}

int       _TreeCreatePulseFile(void *dbid, int shotid, int numnids_in, int *nids_in)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  int       status = 1;
  int       retstatus;
  int       num;
  int       nids[256];
  int       i;
  int       j;
  int       shot;
  int source_shot;

/* Make sure tree is open */

  if ((status = _TreeIsOpen(dblist)) != TreeOPEN)
    return status;

  source_shot = dblist->shotid;
  if (numnids_in == 0)
  {
    void       *ctx = 0;
    for (num = 0; num < 256 && TreeFindTagWild("TOP", &nids[num], &ctx); num++);
  }
  else
  {
    num = 0;
    for (i = 0; i < numnids_in; i++)
    {
      for (j = 0; j < num; j++)
	if (nids[j] == nids_in[i])
	  break;
      if (j == num)
	nids[num++] = nids_in[i];
    }
  }

  if (shotid)
    shot = shotid;
  else
    shot = MdsGetCurrentShotId(dblist->experiment);

  retstatus = status;
  if (status & 1)
  {
    for (i = 0; i < num; i++)
    {
      char name[13];
      if (nids[i])
      {
        NCI_ITM itmlst[] = {{sizeof(name)-1, NciNODE_NAME, 0, 0}, {0, NciEND_OF_LIST, 0, 0}};
        itmlst[0].pointer = name;
        status = TreeGetNci(nids[i], itmlst);
        name[12] = 0;
        for (j=11;j>0;j--) if (name[j] == 0x20) name[j] = '\0';
      }
      else
      {
        strcpy(name,dblist->experiment);
      }
      if (status & 1)
        status = TreeCreateTreeFiles(name, shot, source_shot);
      if (!(status & 1))
        retstatus = status;
    }
  }
  return retstatus;
}

static int  TreeCreateTreeFiles(char *tree, int shot, int source_shot)
{
  int len = strlen(tree);
  char tree_lower[13];
  char pathname[32];
  char *path;
  char *pathin;
  int pathlen;
  char name[32];
  int i;
  int status = 1;
  int itype;
  char *types[] = {".tree",".characteristics",".datafile"};
  for (i=0;i<len && i < 12;i++)
    tree_lower[i] = __tolower(tree[i]);
  tree_lower[i]=0;
  strcpy(pathname,tree_lower);
  strcat(pathname,TREE_PATH_SUFFIX);
#if defined(__VMS)
  pathin = pathname;
#elif defined(_MSC_VER)
  pathin = GetRegistryPath(pathname);
#else
  pathin = getenv(pathname);
#endif
  if (pathin)
  {
    pathlen = strlen(pathin);
    path = malloc(pathlen+1);
    for (itype=0;itype<3 && (status & 1);itype++)
    {
      struct stat stat_info;
      char *srcfile = 0;
      char *dstfile = 0;
      char *type = types[itype];
      char *part;
      strcpy(path,pathin);
      if (source_shot < 0)
        sprintf(name,"%s_model",tree_lower);
      else if (source_shot < 1000)
        sprintf(name,"%s_%03d",tree_lower,source_shot);
      else
        sprintf(name,"%s_%d",tree_lower,source_shot);
      for (i=0,part=path;i<pathlen+1;i++)
      {
        if (*part == ' ') 
          part++;
        else if ((path[i] == ' ' || path[i] == ';' || path[i] == 0) && strlen(part))
        {
	  path[i] = 0;
          srcfile = strcpy(malloc(strlen(part)+strlen(name)+strlen(type)+2),part);
	  if (strcmp(srcfile+strlen(srcfile)-1,TREE_PATH_DELIM))
	    strcat(srcfile,TREE_PATH_DELIM);
	  strcat(srcfile,name);
	  strcat(srcfile,type);
          if (stat(srcfile,&stat_info) == 0)
            break;
          else
	  {
            free(srcfile);
            srcfile = 0;
	    part = &path[i+1];
          }
        }
      }
      if (srcfile)
      {
        strcpy(path,pathin);
        if (shot < 0)
          sprintf(name,"%s_model",tree_lower);
        else if (shot < 1000)
          sprintf(name,"%s_%03d",tree_lower,shot);
        else
          sprintf(name,"%s_%d",tree_lower,shot);
        for (i=0,part=path;i<pathlen+1;i++)
        {
          if (*part == ' ') 
            part++;
          else if ((path[i] == ' ' || path[i] == ';' || path[i] == 0) && strlen(part))
          {
	    path[i] = 0;
            dstfile = strcpy(malloc(strlen(part)+strlen(name)+strlen(type)+2),part);
	    if (strcmp(dstfile+strlen(dstfile)-1,TREE_PATH_DELIM))
	      strcat(dstfile,TREE_PATH_DELIM);
            if (stat(dstfile,&stat_info) == 0)
	    {
	      strcat(dstfile,name);
	      strcat(dstfile,type);
              break;
            }
            else
	    {
              free(dstfile);
              dstfile = 0;
	      part = &path[i+1];
            }
          }
        }
        if (dstfile)
	{
          status = CopyFile(srcfile,dstfile);
          free(dstfile);
        }
        else
          status = 0;
        free(srcfile);
      }
    }
    free(path);
  }
  return status;
}

static int CopyFile(char *src, char *dst)
{
  char cmd[1024];
  sprintf(cmd,"cp %s %s",src,dst);
  system(cmd);
  return 1;
}
