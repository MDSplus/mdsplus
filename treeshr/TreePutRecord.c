/*------------------------------------------------------------------------------

		Name: TreePutRecord

		Type:   C function

		Author:	Thomas W. Fredian
			MIT Plasma Fusion Center

		Date:   23-MAY-1988

		Purpose: Put record in TREE datafile

------------------------------------------------------------------------------

	Call sequence: TreePutRecord(int nid, struct descriptor *, int utility_update)

------------------------------------------------------------------------------
   Copyright (c) 1988
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:


+-----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <ncidef.h>
#include "treeshrp.h"
#include <treeshr.h>
#include <usagedef.h>
#include <ncidef.h>
#include <string.h>
#include <time.h>
#include <librtl_messages.h>
#include <strroutines.h>
#include <libroutines.h>
#include <fcntl.h>
#ifndef O_BINARY
#define O_BINARY 0
#endif
#ifndef O_RANDOM
#define O_RANDOM 0
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#include <io.h>
#endif

#ifdef HAVE_VXWORKS_H
static int timezone = 0;
#endif

static char *cvsrev = "@(#)$RCSfile$ $Revision$ $Date$";

#ifdef min
#undef min
#endif
#define min(a,b) (((a) < (b)) ? (a) : (b))

static int CheckUsage(PINO_DATABASE *dblist, NID *nid_ptr, NCI *nci);
static int FixupNid(NID *nid, unsigned char *tree, struct descriptor *path);
static int FixupPath();
static int PutDatafile(TREE_INFO *info, int nodenum, NCI *nci_ptr, struct descriptor_xd *data_dsc_ptr);
static int UpdateDatafile(TREE_INFO *info, int nodenum, NCI *nci_ptr, struct descriptor_xd *data_dsc_ptr);
static int compress_utility;
static char nid_reference;
static char path_reference;

static int Dsc2Rec(struct descriptor *inp, struct descriptor_xd *out_dsc_ptr);

extern int PutRecordRemote();

extern void *DBID;
int       TreePutRecord(int nid, struct descriptor *descriptor_ptr, int utility_update) {
  return _TreePutRecord(DBID,nid,descriptor_ptr,utility_update);}

int       _TreePutRecord(void *dbid, int nid, struct descriptor *descriptor_ptr, int utility_update)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID       *nid_ptr = (NID *)&nid;
  int       status;
  int       open_status;
  TREE_INFO *info_ptr;
  int       nidx;
  unsigned int old_record_length;
  static int saved_uic = 0;
  int       id = 0;
  int       length = 0;
  int       shot_open;
  compress_utility = utility_update == 2;
#if !defined(HAVE_WINDOWS_H) && !defined(vxWorks)
  if (!saved_uic)
    saved_uic = (getgid() << 16) | getuid();
#endif
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  if (dblist->open_readonly)
    return TreeREADONLY;
  if (dblist->remote)
    return PutRecordRemote(dblist,nid,descriptor_ptr,utility_update);
  shot_open = (dblist->shotid != -1);
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr)
  {
    int       stv;
    NCI       local_nci;
    if (info_ptr->reopen)
      TreeCloseFiles(info_ptr);
    if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
      open_status = TreeOpenDatafileW(info_ptr, &stv, 0);
    else
      open_status = 1;
    status = TreeGetNciLw(info_ptr, nidx, &local_nci);
    if (status & 1)
    {
      if (utility_update)
      {
	length = local_nci.length = 0;
	local_nci.DATA_INFO.DATA_LOCATION.record_length = 0;
      }
      else
      {
        unsigned int m1;
        unsigned int m2 = 10000000;
        _int64 addin = LONG_LONG_CONSTANT(0x7c95674beb4000);
        _int64 zero = 0;
        _int64 temp = 0;
        bitassign(dblist->setup_info, local_nci.flags, NciM_SETUP_INFORMATION);
	local_nci.owner_identifier = saved_uic;
	/* VMS time = unixtime * 10,000,000 + 0x7c95674beb4000q */
#ifndef vxWorks
        tzset();
#endif
        m1 = (unsigned int)time(NULL) - timezone;
	LibEmul(&m1,&m2,&zero,&temp);
        local_nci.time_inserted = temp + addin;
      }
      if (!(open_status & 1))
      {
	local_nci.DATA_INFO.ERROR_INFO.stv = stv;
        bitassign_c(1,local_nci.flags2,NciM_ERROR_ON_PUT);
	local_nci.DATA_INFO.ERROR_INFO.error_status = open_status;
	length = local_nci.length = 0;
	TreePutNci(info_ptr, nidx, &local_nci, 1);
	return open_status;
      }
      else
      {
	NCI      *nci = info_ptr->data_file->asy_nci->nci;
	*nci = local_nci;
	if (!utility_update)
	{
	  old_record_length = (nci->flags2 & NciM_DATA_IN_ATT_BLOCK) ? 0 : 
                        nci->DATA_INFO.DATA_LOCATION.record_length;
	  if ((nci->flags & NciM_WRITE_ONCE) && nci->length)
	    status = TreeNOOVERWRITE;
	  if ((status & 1) && (shot_open && (nci->flags & NciM_NO_WRITE_SHOT)))
	    status = TreeNOWRITESHOT;
	  if ((status & 1) && (!shot_open && (nci->flags & NciM_NO_WRITE_MODEL)))
	    status = TreeNOWRITEMODEL;
	}
	if (status & 1)
	{
          unsigned char tree = (unsigned char)nid_ptr->tree;
          int compressible;
          int data_in_altbuf;
          nid_reference = 0;
          path_reference = 0;
	  status = MdsSerializeDscOutZ(descriptor_ptr, info_ptr->data_file->data,FixupNid,&tree,FixupPath,0,
            (compress_utility || (nci->flags & NciM_COMPRESS_ON_PUT)) && !(nci->flags & NciM_DO_NOT_COMPRESS),
            &compressible,&nci->length,&nci->DATA_INFO.DATA_LOCATION.record_length,&nci->dtype,&nci->class,
            sizeof(nci->DATA_INFO.DATA_IN_RECORD.data),nci->DATA_INFO.DATA_IN_RECORD.data,&data_in_altbuf);
          bitassign(path_reference,nci->flags,NciM_PATH_REFERENCE);
          bitassign(nid_reference,nci->flags,NciM_NID_REFERENCE);
          bitassign(compressible,nci->flags,NciM_COMPRESSIBLE);
          bitassign_c(data_in_altbuf,nci->flags2,NciM_DATA_IN_ATT_BLOCK);
        }
	if ((status & 1) && nci->length && (!utility_update))
	  status = CheckUsage(dblist, nid_ptr, nci);
	if (status & 1)
	{
	  if (nci->flags2 & NciM_DATA_IN_ATT_BLOCK)
	  {
	    bitassign_c(0,nci->flags2,NciM_ERROR_ON_PUT);
	    status = TreePutNci(info_ptr, nidx, nci, 1);
	  }
	  else
	  {
	    if ((nci->DATA_INFO.DATA_LOCATION.record_length != old_record_length) ||
		(nci->DATA_INFO.DATA_LOCATION.record_length >= DATAF_C_MAX_RECORD_SIZE) ||
		utility_update ||
		(nci->flags2 & NciM_ERROR_ON_PUT))
	      status = PutDatafile(info_ptr, nidx, nci, info_ptr->data_file->data);
	    else
	      status = UpdateDatafile(info_ptr, nidx, nci, info_ptr->data_file->data);
	  }
	}
      }
    }
  }
  else
    status = TreeINVTREE;
  return status;
}

static int CheckUsage(PINO_DATABASE *dblist, NID *nid_ptr, NCI *nci)
{

#define is_expression ((nci->dtype == DTYPE_FUNCTION) ||\
                       (nci->dtype == DTYPE_NID) ||\
                       (nci->dtype == DTYPE_PATH) ||\
                       (nci->dtype == DTYPE_IDENT) ||\
                       (nci->dtype == DTYPE_CALL))

#define check(boolean) (boolean) ? TreeNORMAL : TreeINVDTPUSG;

#define is_numeric ( ((nci->dtype >= DTYPE_BU) &&\
                      (nci->dtype <= DTYPE_DC)) ||\
                     ((nci->dtype >= DTYPE_OU) &&\
                      (nci->dtype <= DTYPE_HC)) ||\
                     ((nci->dtype >= DTYPE_FS) &&\
                      (nci->dtype <= DTYPE_FTC))  )


  NODE     *node_ptr;
  int       status;
  nid_to_node(dblist, nid_ptr, node_ptr);
  if (!node_ptr)
    return TreeNNF;
  switch (node_ptr->usage)
  {
   case TreeUSAGE_STRUCTURE:
   case TreeUSAGE_SUBTREE:
    status = TreeINVDTPUSG;
    break;
   case TreeUSAGE_ACTION:
    status = check((nci->dtype == DTYPE_ACTION) || is_expression);
    break;
   case TreeUSAGE_COMPOUND_DATA:
   case TreeUSAGE_DEVICE:
    status = check(nci->dtype == DTYPE_CONGLOM);
    break;
   case TreeUSAGE_DISPATCH:
    status = check((nci->dtype == DTYPE_DISPATCH) || is_expression);
    break;
   case TreeUSAGE_NUMERIC:
    status = check(is_numeric ||
		   (nci->dtype == DTYPE_PARAM) ||
		   (nci->dtype == DTYPE_RANGE) ||
		   (nci->dtype == DTYPE_WITH_UNITS) || is_expression);
    break;
   case TreeUSAGE_SIGNAL:
    status = check(is_numeric ||
		   (nci->dtype == DTYPE_SIGNAL) ||
		   (nci->dtype == DTYPE_DIMENSION) ||
		   (nci->dtype == DTYPE_PARAM) ||
		   (nci->dtype == DTYPE_RANGE) ||
		   (nci->dtype == DTYPE_WITH_UNITS) || is_expression);
    break;
   case TreeUSAGE_TASK:
    status = check((nci->dtype == DTYPE_PROGRAM) ||
		   (nci->dtype == DTYPE_ROUTINE) ||
		   (nci->dtype == DTYPE_PROCEDURE) ||
		   (nci->dtype == DTYPE_METHOD) || is_expression);
    break;
   case TreeUSAGE_TEXT:
    status = check((nci->dtype == DTYPE_T) ||
		   (nci->dtype == DTYPE_PARAM) ||
		   (nci->dtype == DTYPE_WITH_UNITS) || is_expression);
    break;
   case TreeUSAGE_WINDOW:
    status = check((nci->dtype == DTYPE_WINDOW) || is_expression);
    break;
   case TreeUSAGE_AXIS:
    status = check(is_numeric ||
		   (nci->dtype == DTYPE_SLOPE) ||
		   (nci->dtype == DTYPE_RANGE) ||
		   (nci->dtype == DTYPE_WITH_UNITS) || is_expression);
    break;
   default:
    status = TreeNORMAL;
    break;
  }
  return status;
}

static int FixupNid(NID *nid, unsigned char *tree, struct descriptor *path)
{
  int       status = 0;
  if (nid->tree != *tree)
  {
    char *path_c = TreeGetPath(*(int *)nid);
    if (path_c)
      {
        struct descriptor path_d = {0, DTYPE_T, CLASS_S, 0};
        path_d.length = strlen(path_c);
        path_d.pointer = path_c;
        StrCopyDx(path,&path_d);
        TreeFree(path_c);
      }
    
    path_reference = 1;
  }
  else
    nid_reference = 1;
  return status;
}

static int FixupPath()
{
  path_reference = 1;
  return 0;
}

int TreeOpenDatafileW(TREE_INFO *info, int *stv_ptr, int tmpfile)
{
  int       status = TreeNORMAL;
  DATA_FILE *df_ptr = info->data_file;
  *stv_ptr = 0;
  if (df_ptr == 0)
  {
    df_ptr = TreeGetVmDatafile();
    status = (df_ptr == NULL) ? TreeFAILURE : TreeNORMAL;
  }
  if (status & 1)
  {
    size_t len = strlen(info->filespec)-4;
    char *filename = strncpy(malloc(len+20),info->filespec,len);
    filename[len]='\0';
    strcat(filename,tmpfile ? "datafile#" : "datafile");
    df_ptr->get = open(filename,(tmpfile ? O_RDWR | O_CREAT | O_TRUNC : O_RDONLY) | O_BINARY | O_RANDOM, 0664);
    status = (df_ptr->get == -1) ? TreeFAILURE : TreeNORMAL;
    if (df_ptr->get == -1)
      df_ptr->get = 0;
    if (status & 1)
    {
      df_ptr->put = open(filename,O_RDWR | O_BINARY | O_RANDOM, 0);
      status = (df_ptr->put == -1) ? TreeFAILURE : TreeNORMAL;
      if (df_ptr->put == -1)
        df_ptr->put = 0;
      if (status & 1)
 	  df_ptr->open_for_write = 1;
    }
  }
  else
  {
    free(df_ptr);
    df_ptr = NULL;
  }
  info->data_file = df_ptr;
  if (status & 1)
    TreeCallHook(OpenDataFileWrite, info);
  return status;
}


static int PutDatafile(TREE_INFO *info, int nodenum, NCI *nci_ptr, struct descriptor_xd *data_dsc_ptr)
{
  int       status = TreeNORMAL;
  int       bytes_to_put = nci_ptr->DATA_INFO.DATA_LOCATION.record_length;

  LoadInt(nodenum, (char *)&info->data_file->record_header->node_number);
  memset(&info->data_file->record_header->rfa,0,sizeof(RFA));
  while (bytes_to_put && (status & 1))
  {
    int bytes_this_time = min(DATAF_C_MAX_RECORD_SIZE + 2, bytes_to_put);
    int eof;
    unsigned char rfa[6];
    status = TreeLockDatafile(info, 0, -1);
    if (status & 1)
    {
      unsigned short rlength = bytes_this_time + 10;
      eof = lseek(info->data_file->put,0,SEEK_END);
      bytes_to_put -= bytes_this_time;
      LoadShort(rlength,(char *)&info->data_file->record_header->rlength);
      status = (write(info->data_file->put,(void *) info->data_file->record_header,sizeof(RECORD_HEADER)) == sizeof(RECORD_HEADER))
                    ? TreeNORMAL : TreeFAILURE;
      status = (write(info->data_file->put,(void *) (((char *)data_dsc_ptr->pointer->pointer) + bytes_to_put), bytes_this_time) == bytes_this_time)
                    ? TreeNORMAL : TreeFAILURE;
      if (!bytes_to_put)
      {
        if (status & 1)
        {
          bitassign(0,nci_ptr->flags2,NciM_ERROR_ON_PUT);
          SeekToRfa(eof,rfa);
          memcpy(nci_ptr->DATA_INFO.DATA_LOCATION.rfa,rfa,sizeof(nci_ptr->DATA_INFO.DATA_LOCATION.rfa));
        }
        else
        {
          bitassign(1,nci_ptr->flags2,NciM_ERROR_ON_PUT);
          nci_ptr->DATA_INFO.ERROR_INFO.error_status = status;
          nci_ptr->length = 0;
        }
        TreePutNci(info, nodenum, nci_ptr, 1);
      }
      else
      {
        SeekToRfa(eof,rfa);
        memcpy(&info->data_file->record_header->rfa,rfa,sizeof(info->data_file->record_header->rfa));
      }
      TreeUnLockDatafile(info, 0, eof);
    }
    else
      TreeUnLockNci(info, 0, nodenum);
  }
  return status;
}

static int UpdateDatafile(TREE_INFO *info, int nodenum, NCI *nci_ptr, struct descriptor_xd *data_dsc_ptr)
{
  int       status = TreeNORMAL;
  int       bytes_to_put = nci_ptr->DATA_INFO.DATA_LOCATION.record_length;
  info->data_file->record_header->node_number = swapint((char *)&nodenum);
  memset(&info->data_file->record_header->rfa,0,sizeof(RFA));
  while (bytes_to_put && (status & 1))
  {
    int bytes_this_time = min(DATAF_C_MAX_RECORD_SIZE + 2, bytes_to_put);
    int rfa_l = RfaToSeek(nci_ptr->DATA_INFO.DATA_LOCATION.rfa);
    status = TreeLockDatafile(info, 0, rfa_l);
    if (status & 1)
    {
      bytes_to_put -= bytes_this_time;
      info->data_file->record_header->rlength = (unsigned short)(bytes_this_time + 10);
      info->data_file->record_header->rlength = swapshort((char *)&info->data_file->record_header->rlength);
      lseek(info->data_file->put,rfa_l,SEEK_SET);
      status = (write(info->data_file->put,(void *) info->data_file->record_header,sizeof(RECORD_HEADER)) == sizeof(RECORD_HEADER))
                   ? TreeNORMAL : TreeFAILURE;
      status = (write(info->data_file->put,(void *) (((char *)data_dsc_ptr->pointer->pointer) + bytes_to_put), bytes_this_time) == bytes_this_time) ? TreeNORMAL : TreeFAILURE;
      if (!bytes_to_put)
      {
        if (status & 1)
        {
          bitassign(0,nci_ptr->flags2,NciM_ERROR_ON_PUT);
        }
        else
        {
          bitassign(1,nci_ptr->flags2,NciM_ERROR_ON_PUT);
          nci_ptr->DATA_INFO.ERROR_INFO.error_status = status;
          nci_ptr->length = 0;
        }
        TreePutNci(info, nodenum, nci_ptr, 1);
      }
      TreeUnLockDatafile(info,0,rfa_l);
    }
    else
      TreeUnLockNci(info, 0, nodenum);
 }
  return status;
}

/*-----------------------------------------------------------------
	Recursively compact all descriptors and adjust pointers.
	NIDs converted to PATHs for TREE$COPY_TO_RECORD.
	Eliminates DSC descriptors. Need DSC for classes A and APD?
-----------------------------------------------------------------*/

#ifdef _WIN32

static int LockStart;
static int LockSize;
int TreeLockDatafile(TREE_INFO *info, int readonly, int offset)
{
	LockStart = offset >= 0 ? offset : _lseek(info->data_file->put,0,SEEK_END);
	LockSize = offset >= 0 ? 12 : 0x7fffffff;
	return LockFile((HANDLE)_get_osfhandle(readonly ? info->data_file->get : info->data_file->put), LockStart, 0,
	   LockSize, 0) == 0 ? TreeFAILURE : TreeSUCCESS;
}
int TreeUnLockDatafile(TREE_INFO *info, int readonly, int offset)
{
  return UnlockFile((HANDLE)_get_osfhandle(readonly ? info->data_file->get : info->data_file->put), LockStart, 0,
	   LockSize, 0) == 0 ? TreeFAILURE : TreeSUCCESS;
}
#else
#ifdef vxWorks 
int TreeLockDatafile(TREE_INFO *info, int readonly, int nodenum)
{ return TreeSUCCESS; }
int TreeUnLockDatafile(TREE_INFO *info, int readonly, int nodenum)
{ return TreeSUCCESS; }
#else
int TreeLockDatafile(TREE_INFO *info, int readonly, int offset)
{
  struct flock flock_info;
  flock_info.l_type = readonly ? F_RDLCK : F_WRLCK;
  flock_info.l_whence = offset >= 0 ? SEEK_SET : SEEK_END;
  flock_info.l_start = offset >= 0 ? offset : 0;
  flock_info.l_len = offset >= 0 ? 12 : (DATAF_C_MAX_RECORD_SIZE * 3);
  return (fcntl(readonly ? info->data_file->get : info->data_file->put,F_SETLKW, &flock_info) != -1) ? 
          TreeSUCCESS : TreeLOCK_FAILURE;
}

int TreeUnLockDatafile(TREE_INFO *info, int readonly, int offset)
{
  struct flock flock_info;
  flock_info.l_type = F_UNLCK;
  flock_info.l_whence = SEEK_SET;
  flock_info.l_start = 0;
  flock_info.l_len = 0;
  return (fcntl(readonly ? info->data_file->get : info->data_file->put,F_SETLKW, &flock_info) != -1) ? 
          TreeSUCCESS : TreeLOCK_FAILURE;
}
#endif
#endif

