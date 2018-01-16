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
#include "treeshrp.h"		/* must be first or off_t wrong */
#include <mdsplus/mdsconfig.h>
#include <stdio.h>
#include <stdlib.h>
#include <mdstypes.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <ncidef.h>
#include "treethreadsafe.h"
#include <treeshr.h>
#include <usagedef.h>
#include <ncidef.h>
#include <string.h>
#include <time.h>
#include <mdsshr_messages.h>
#include <strroutines.h>
#include <libroutines.h>
#include <fcntl.h>

#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

#ifdef min
#undef min
#endif
#define min(a,b) (((a) < (b)) ? (a) : (b))

static int CheckUsage(PINO_DATABASE * dblist, NID * nid_ptr, NCI * nci);
int TreeFixupNid(NID * nid, unsigned char *tree, struct descriptor *path);
static int FixupPath();
static int PutDatafile(TREE_INFO * info, int nodenum, NCI * nci_ptr,
		       struct descriptor_xd *data_dsc_ptr, NCI * previous_nci);
static int UpdateDatafile(TREE_INFO * info, int nodenum, NCI * nci_ptr,
			  struct descriptor_xd *data_dsc_ptr);
extern int PutRecordRemote();

extern void **TreeCtx();

int64_t TreeTimeInserted()
{
  int64_t ans;
  LibTimeToVMSTime(0, &ans);
  return ans;
}

int TreePutRecord(int nid, struct descriptor *descriptor_ptr, int utility_update)
{
  return _TreePutRecord(*TreeCtx(), nid, descriptor_ptr, utility_update);
}

int _TreePutRecord(void *dbid, int nid, struct descriptor *descriptor_ptr, int utility_update)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid_ptr = (NID *) & nid;
  int status;
  int open_status;
  TREE_INFO *info_ptr;
  int nidx;
  unsigned int old_record_length = 0;
  static int saved_uic = 0;
  int shot_open;
  EXTENDED_ATTRIBUTES attributes;
  int extended = 0;
  int64_t extended_offset;
  int compress_utility = utility_update == 2;
  int unlock_nci_needed = 0;
#ifndef _WIN32
  if (!saved_uic)
    saved_uic = (getgid() << 16) | getuid();
#endif
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  if (dblist->open_readonly)
    return TreeREADONLY;
  if (dblist->remote)
    return PutRecordRemote(dblist, nid, descriptor_ptr, utility_update);
  shot_open = (dblist->shotid != -1);
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr) {
    int stv;
    NCI local_nci, old_nci;
    int64_t saved_viewdate;
    status = TreeCallHook(PutData, info_ptr, nid);
    if (status && !(status & 1))
      return status;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciLw(info_ptr, nidx, &local_nci);
    if (status & 1)
      unlock_nci_needed = 1;
    TreeSetViewDate(&saved_viewdate);
    memcpy(&old_nci, &local_nci, sizeof(local_nci));
    if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
      open_status = TreeOpenDatafileW(info_ptr, &stv, 0);
    else
      open_status = 1;
    if (local_nci.flags2 & NciM_EXTENDED_NCI) {
      int ext_status;
      extended_offset = RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa);
      ext_status = TreeGetExtendedAttributes(info_ptr, extended_offset, &attributes);
      if (!(ext_status & 1)) {
	local_nci.flags2 &= ~NciM_EXTENDED_NCI;
      } else {
	extended = 1;
      }
    }
    if STATUS_OK {
      if (utility_update) {
        NCI *nci = &TreeGetThreadStatic()->TemplateNci;
	local_nci.flags = nci->flags;
	bitassign(0, local_nci.flags, NciM_VERSIONS);
	local_nci.owner_identifier = nci->owner_identifier;
	local_nci.time_inserted = nci->time_inserted;
      } else {
	bitassign(dblist->setup_info, local_nci.flags, NciM_SETUP_INFORMATION);
	local_nci.owner_identifier = saved_uic;
	/* VMS time = unixtime * 10,000,000 + 0x7c95674beb4000q */
	local_nci.time_inserted = TreeTimeInserted();
      }
      if (!(open_status & 1)) {
	local_nci.DATA_INFO.ERROR_INFO.stv = stv;
	bitassign_c(1, local_nci.flags2, NciM_ERROR_ON_PUT);
	local_nci.DATA_INFO.ERROR_INFO.error_status = open_status;
	local_nci.length = 0;
	TreePutNci(info_ptr, nidx, &local_nci, 1);
	status = open_status;
	goto done;
      } else {
	NCI *nci = info_ptr->data_file->asy_nci->nci;
	*nci = local_nci;
	if (nci->length != 0 && ((shot_open && info_ptr->header->versions_in_pulse) ||
				 (!shot_open && info_ptr->header->versions_in_model)))
	  bitassign(1, nci->flags, NciM_VERSIONS);
	if (!utility_update) {
	  old_record_length = (nci->flags2 & NciM_DATA_IN_ATT_BLOCK
			       || (nci->flags & NciM_VERSIONS)) ? 0 : nci->DATA_INFO.DATA_LOCATION.
	      record_length;
	  if ((nci->flags & NciM_WRITE_ONCE) && nci->length)
	    status = TreeNOOVERWRITE;
	  if ((status & 1) && (shot_open && (nci->flags & NciM_NO_WRITE_SHOT)))
	    status = TreeNOWRITESHOT;
	  if ((status & 1) && (!shot_open && (nci->flags & NciM_NO_WRITE_MODEL)))
	    status = TreeNOWRITEMODEL;
	}
	if (status & 1) {
	  unsigned char tree = (unsigned char)nid_ptr->tree;
	  int compressible;
	  int data_in_altbuf;
	  TreeGetThreadStatic()->nid_reference = 0;
	  TreeGetThreadStatic()->path_reference = 0;
	  status =
	      MdsSerializeDscOutZ(descriptor_ptr, info_ptr->data_file->data, TreeFixupNid, &tree,
				  FixupPath, 0, (compress_utility
						 || (nci->flags & NciM_COMPRESS_ON_PUT))
				  && !(nci->flags & NciM_DO_NOT_COMPRESS), &compressible,
				  &nci->length, &nci->DATA_INFO.DATA_LOCATION.record_length,
				  &nci->dtype, &nci->class, (nci->flags & NciM_VERSIONS
							     || extended) ? 0 :
				  sizeof(nci->DATA_INFO.DATA_IN_RECORD.data),
				  nci->DATA_INFO.DATA_IN_RECORD.data, &data_in_altbuf);
	  bitassign(TreeGetThreadStatic()->path_reference, nci->flags, NciM_PATH_REFERENCE);
	  bitassign(TreeGetThreadStatic()->nid_reference, nci->flags, NciM_NID_REFERENCE);
	  bitassign(compressible, nci->flags, NciM_COMPRESSIBLE);
	  bitassign_c(data_in_altbuf, nci->flags2, NciM_DATA_IN_ATT_BLOCK);
	}
	if ((status & 1) && nci->length && (!utility_update))
	  status = CheckUsage(dblist, nid_ptr, nci);
	if (status & 1) {
	  if (nci->flags2 & NciM_DATA_IN_ATT_BLOCK && !(nci->flags & NciM_VERSIONS) && !extended) {
	    bitassign_c(0, nci->flags2, NciM_ERROR_ON_PUT);
	    status = TreePutNci(info_ptr, nidx, nci, 1);
	  } else {
	    if (extended) {
	      status =
		  TreePutDsc(info_ptr, nid, descriptor_ptr,
			     &attributes.facility_offset[STANDARD_RECORD_FACILITY],
			     &attributes.facility_length[STANDARD_RECORD_FACILITY], (compress_utility
						 || (nci->flags & NciM_COMPRESS_ON_PUT))
				  && !(nci->flags & NciM_DO_NOT_COMPRESS));
	      if (status & 1) {
		attributes.facility_offset[SEGMENTED_RECORD_FACILITY] = -1;
		attributes.facility_length[SEGMENTED_RECORD_FACILITY] = 0;
		status = TreePutExtendedAttributes(info_ptr, &attributes, &extended_offset);
	      }
	      if (status & 1) {
		bitassign(0,nci->flags,NciM_SEGMENTED);
		TreePutNci(info_ptr, nidx, nci, 1);
	      }
	    } else if ((nci->DATA_INFO.DATA_LOCATION.record_length != old_record_length) ||
		       (nci->DATA_INFO.DATA_LOCATION.record_length >= DATAF_C_MAX_RECORD_SIZE) ||
		       utility_update ||
		       (nci->flags & NciM_VERSIONS) || (nci->flags2 & NciM_ERROR_ON_PUT))
	      status = PutDatafile(info_ptr, nidx, nci, info_ptr->data_file->data, &old_nci);
	    else
	      status = UpdateDatafile(info_ptr, nidx, nci, info_ptr->data_file->data);
	  }
	}
      }
    }
  } else
    status = TreeINVTREE;
 done:
  if (unlock_nci_needed)
    TreeUnLockNci(info_ptr, 0, nidx);
  return status;
}

static int CheckUsage(PINO_DATABASE * dblist, NID * nid_ptr, NCI * nci)
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
                      (nci->dtype <= DTYPE_FTC)) || (nci->dtype == DTYPE_DSC) )

  NODE *node_ptr;
  int status;
  node_ptr = nid_to_node(dblist, nid_ptr);
  if (!node_ptr)
    return TreeNNF;
  switch (node_ptr->usage) {
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
		   (nci->dtype == DTYPE_WITH_UNITS) ||
		   (nci->dtype == DTYPE_WITH_ERROR) ||
		   (nci->dtype == DTYPE_OPAQUE) || is_expression);
    break;
  case TreeUSAGE_SIGNAL:
    status = check(is_numeric ||
		   (nci->dtype == DTYPE_SIGNAL) ||
		   (nci->dtype == DTYPE_DIMENSION) ||
		   (nci->dtype == DTYPE_PARAM) ||
		   (nci->dtype == DTYPE_RANGE) ||
		   (nci->dtype == DTYPE_WITH_UNITS) ||
		   (nci->dtype == DTYPE_WITH_ERROR) ||
		   (nci->dtype == DTYPE_OPAQUE) || is_expression);
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
		   (nci->dtype == DTYPE_WITH_UNITS) ||
		   (nci->dtype == DTYPE_OPAQUE) || (nci->dtype == DTYPE_DSC) || is_expression);
    break;
  case TreeUSAGE_WINDOW:
    status = check((nci->dtype == DTYPE_WINDOW) || is_expression);
    break;
  case TreeUSAGE_AXIS:
    status = check(is_numeric ||
		   (nci->dtype == DTYPE_SLOPE) ||
		   (nci->dtype == DTYPE_RANGE) ||
		   (nci->dtype == DTYPE_WITH_UNITS) ||
		   (nci->dtype == DTYPE_DIMENSION) || is_expression);
    break;
  default:
    status = TreeNORMAL;
    break;
  }
  return status;
}

int TreeFixupNid(NID * nid, unsigned char *tree, struct descriptor *path)
{
  int status = 0;
  if (nid->tree != *tree) {
    char *path_c = TreeGetPath(*(int *)nid);
    if (path_c) {
      struct descriptor path_d = { 0, DTYPE_T, CLASS_S, 0 };
      path_d.length = (unsigned short)strlen(path_c);
      path_d.pointer = path_c;
      StrCopyDx(path, &path_d);
      TreeFree(path_c);
    }

    TreeGetThreadStatic()->path_reference = 1;
    status = 1;
  } else
    TreeGetThreadStatic()->nid_reference = 1;
  return status;
}

static int FixupPath()
{
  TreeGetThreadStatic()->path_reference = 1;
  return 0;
}

int TreeOpenDatafileW(TREE_INFO * info, int *stv_ptr, int tmpfile){
  WRLOCKINFO(info);
  int status = _TreeOpenDatafileW(info, stv_ptr, tmpfile);
  UNLOCKINFO(info);
  return status;
}
int _TreeOpenDatafileW(TREE_INFO * info, int *stv_ptr, int tmpfile)
{
  int status = TreeNORMAL;
  if (info->header->readonly)
    return TreeREADONLY_TREE;
  DATA_FILE *df_ptr = info->data_file;
  *stv_ptr = 0;
  if (df_ptr == 0) {
    df_ptr = TreeGetVmDatafile();
    status = (df_ptr == NULL) ? TreeFAILURE : TreeNORMAL;
  }
  if (status & 1) {
    int old_get = df_ptr->get;
    size_t len = strlen(info->filespec) - 4;
    size_t const filename_length = len + 20;
    char *filename = (char *)alloca(filename_length);
    strncpy(filename, info->filespec, len);
    filename[len] = '\0';
    strcat(filename, tmpfile ? "datafile#" : "datafile");
    df_ptr->get = MDS_IO_OPEN(filename, tmpfile ? O_RDWR | O_CREAT | O_TRUNC | O_EXCL: O_RDONLY, 0664);
    status = (df_ptr->get == -1) ? TreeFAILURE : TreeNORMAL;
    if (df_ptr->get == -1)
      df_ptr->get = old_get;
    else if (df_ptr->get > 0)
      MDS_IO_CLOSE(old_get);
    if (status & 1) {
      df_ptr->put = MDS_IO_OPEN(filename, O_RDWR, 0);
      status = (df_ptr->put == -1) ? TreeFAILURE : TreeNORMAL;
      if (df_ptr->put == -1)
	df_ptr->put = 0;
      if (status & 1)
	df_ptr->open_for_write = 1;
    }
  } else {
    free(df_ptr);
    df_ptr = NULL;
  }
  info->data_file = df_ptr;
  if (status & 1)
    TreeCallHook(OpenDataFileWrite, info, 0);
  return status;
}

#define BUFFERED_IO
#ifdef BUFFERED_IO
static int PutDatafile(TREE_INFO * info, int nodenum, NCI * nci_ptr,
		       struct descriptor_xd *data_dsc_ptr, NCI * old_nci_ptr)
{
  int status = TreeNORMAL;
  unsigned int bytes_to_put =
      data_dsc_ptr->l_length > 0 ? nci_ptr->DATA_INFO.DATA_LOCATION.record_length : 0;
  int blen =
      bytes_to_put + (bytes_to_put + DATAF_C_MAX_RECORD_SIZE + 1) / (DATAF_C_MAX_RECORD_SIZE +
								     2) * sizeof(RECORD_HEADER);
  static int nonvms_compatible = -1;
  char *buffer = 0;
  char *bptr;
  int64_t eof;
  int locked = 0;
  unsigned char rfa[6];
  if (blen > 0) {
    bitassign_c(1, nci_ptr->flags2, NciM_DATA_CONTIGUOUS);
    if (nonvms_compatible == -1)
      nonvms_compatible = getenv("NONVMS_COMPATIBLE") != NULL;
    if (nonvms_compatible) {
      bitassign_c(1, nci_ptr->flags2, NciM_NON_VMS);
      status = TreeLockDatafile(info, 0, 0);
      locked = 1;
      eof = MDS_IO_LSEEK(info->data_file->put, 0, SEEK_END);
      buffer = (char *)data_dsc_ptr->pointer->pointer;
      bptr = buffer + nci_ptr->DATA_INFO.DATA_LOCATION.record_length;
    } else {
      buffer = (char *)malloc(blen);
      bptr = buffer;
      LoadInt(nodenum, (char *)&info->data_file->record_header->node_number);
      bitassign_c(0, nci_ptr->flags2, NciM_NON_VMS);
      memset(&info->data_file->record_header->rfa, 0, sizeof(RFA));
      while (bytes_to_put && (status & 1)) {
	int bytes_this_time = min(DATAF_C_MAX_RECORD_SIZE + 2, bytes_to_put);
	bytes_to_put -= bytes_this_time;
	bptr += sizeof(RECORD_HEADER);
	memcpy(bptr, (void *)(((char *)data_dsc_ptr->pointer->pointer) + bytes_to_put),
	       bytes_this_time);
	bptr += bytes_this_time;
      }
      bptr = buffer;
      status = TreeLockDatafile(info, 0, 0);
      locked = 1;
      eof = MDS_IO_LSEEK(info->data_file->put, 0, SEEK_END);
      bytes_to_put = nci_ptr->DATA_INFO.DATA_LOCATION.record_length;
      while (bytes_to_put && (status & 1)) {
	int bytes_this_time = min(DATAF_C_MAX_RECORD_SIZE + 2, bytes_to_put);
	unsigned short rlength = bytes_this_time + 10;
	bytes_to_put -= bytes_this_time;
	LoadShort(rlength, (char *)&info->data_file->record_header->rlength);
	memcpy(bptr, (void *)info->data_file->record_header, sizeof(RECORD_HEADER));
	bptr += sizeof(RECORD_HEADER);
	bptr += bytes_this_time;
	SeekToRfa(eof, rfa);
	memcpy(&info->data_file->record_header->rfa, rfa,
	       sizeof(info->data_file->record_header->rfa));
	if (bytes_to_put)
	  eof += sizeof(RECORD_HEADER) + bytes_this_time;
      }
    }
    if (status & 1) {
      status =
	  (MDS_IO_WRITE(info->data_file->put, (void *)buffer, bptr - buffer) == (bptr - buffer))
	  ? TreeNORMAL : TreeFAILURE;
      if (status & 1) {
	bitassign_c(0, nci_ptr->flags2, NciM_ERROR_ON_PUT);
	SeekToRfa(eof, rfa);
	memcpy(nci_ptr->DATA_INFO.DATA_LOCATION.rfa, rfa,
	       sizeof(nci_ptr->DATA_INFO.DATA_LOCATION.rfa));
      } else {
	bitassign_c(1, nci_ptr->flags2, NciM_ERROR_ON_PUT);
	nci_ptr->DATA_INFO.ERROR_INFO.error_status = status;
	nci_ptr->length = 0;
      }
    }
  }
  if (nci_ptr->flags & NciM_VERSIONS) {
    char nci_bytes[42];
    int64_t seek_end;
    TreeSerializeNciOut(old_nci_ptr, nci_bytes);
    seek_end = MDS_IO_LSEEK(info->data_file->put, 0, SEEK_END);
    if (seek_end != -1) {
      status =
	  (MDS_IO_WRITE(info->data_file->put, nci_bytes, sizeof(nci_bytes)) ==
	   sizeof(nci_bytes)) ? TreeNORMAL : TreeFAILURE;
    } else
      status = TreeFAILURE;
  }
  TreePutNci(info, nodenum, nci_ptr, 1);
  if (locked)
    TreeUnLockDatafile(info, 0, 0);
  if (buffer && (!nonvms_compatible))
    free(buffer);
  return status;
}

#else
static int PutDatafile(TREE_INFO * info, int nodenum, NCI * nci_ptr,
		       struct descriptor_xd *data_dsc_ptr)
{
  int status = TreeNORMAL;
  int bytes_to_put = nci_ptr->DATA_INFO.DATA_LOCATION.record_length;

  LoadInt(nodenum, (char *)&info->data_file->record_header->node_number);
  memset(&info->data_file->record_header->rfa, 0, sizeof(RFA));
  while (bytes_to_put && (status & 1)) {
    int bytes_this_time = min(DATAF_C_MAX_RECORD_SIZE + 2, bytes_to_put);
    int64_t eof;
    unsigned char rfa[6];
    status = TreeLockDatafile(info, 0, 0);
    if STATUS_OK {
      unsigned short rlength = bytes_this_time + 10;
      eof = MDS_IO_LSEEK(info->data_file->put, 0, SEEK_END);
      bytes_to_put -= bytes_this_time;
      LoadShort(rlength, (char *)&info->data_file->record_header->rlength);
      status =
	  (MDS_IO_WRITE
	   (info->data_file->put, (void *)info->data_file->record_header,
	    sizeof(RECORD_HEADER)) == sizeof(RECORD_HEADER))
	  ? TreeNORMAL : TreeFAILURE;
      status =
	  (MDS_IO_WRITE
	   (info->data_file->put, (void *)(((char *)data_dsc_ptr->pointer->pointer) + bytes_to_put),
	    bytes_this_time) == bytes_this_time)
	  ? TreeNORMAL : TreeFAILURE;
      if (!bytes_to_put) {
	bitassign(0,nci_ptr->flags,NciM_SEGMENTED);
	if STATUS_OK {
	  bitassign(0, nci_ptr->flags2, NciM_ERROR_ON_PUT);
	  SeekToRfa(eof, rfa);
	  memcpy(nci_ptr->DATA_INFO.DATA_LOCATION.rfa, rfa,
		 sizeof(nci_ptr->DATA_INFO.DATA_LOCATION.rfa));
	} else {
	  bitassign(1, nci_ptr->flags2, NciM_ERROR_ON_PUT);
	  nci_ptr->DATA_INFO.ERROR_INFO.error_status = status;
	  nci_ptr->length = 0;
	}
	TreePutNci(info, nodenum, nci_ptr, 1);
      } else {
	SeekToRfa(eof, rfa);
	memcpy(&info->data_file->record_header->rfa, rfa,
	       sizeof(info->data_file->record_header->rfa));
      }
      TreeUnLockDatafile(info, 0, 0);
    } else
      TreeUnLockNci(info, 0, nodenum);
  }
  return status;
}
#endif

static int UpdateDatafile(TREE_INFO * info, int nodenum, NCI * nci_ptr,
			  struct descriptor_xd *data_dsc_ptr)
{
  int status = TreeNORMAL;
  unsigned int bytes_to_put = nci_ptr->DATA_INFO.DATA_LOCATION.record_length;
  LoadInt(nodenum, (char *)&info->data_file->record_header->node_number);
  memset(&info->data_file->record_header->rfa, 0, sizeof(RFA));
  while (bytes_to_put && (status & 1)) {
    int bytes_this_time = min(DATAF_C_MAX_RECORD_SIZE + 2, bytes_to_put);
    int64_t rfa_l = RfaToSeek(nci_ptr->DATA_INFO.DATA_LOCATION.rfa);
    status = TreeLockDatafile(info, 0, rfa_l);
    if (status & 1) {
      bytes_to_put -= bytes_this_time;
      info->data_file->record_header->rlength = (unsigned short)(bytes_this_time + 10);
      info->data_file->record_header->rlength =
	  swapshort((char *)&info->data_file->record_header->rlength);
      MDS_IO_LSEEK(info->data_file->put, rfa_l, SEEK_SET);
      status =
	  (MDS_IO_WRITE
	   (info->data_file->put, (void *)info->data_file->record_header,
	    sizeof(RECORD_HEADER)) == sizeof(RECORD_HEADER))
	  ? TreeNORMAL : TreeFAILURE;
      status =
	  (MDS_IO_WRITE
	   (info->data_file->put, (void *)(((char *)data_dsc_ptr->pointer->pointer) + bytes_to_put),
	    bytes_this_time) == bytes_this_time) ? TreeNORMAL : TreeFAILURE;
      if (!bytes_to_put) {
	bitassign(0,nci_ptr->flags,NciM_SEGMENTED);
	if (status & 1) {
	  bitassign_c(0, nci_ptr->flags2, NciM_ERROR_ON_PUT);
	} else {
	  bitassign_c(1, nci_ptr->flags2, NciM_ERROR_ON_PUT);
	  nci_ptr->DATA_INFO.ERROR_INFO.error_status = status;
	  nci_ptr->length = 0;
	}
	TreePutNci(info, nodenum, nci_ptr, 1);
      }
      TreeUnLockDatafile(info, 0, rfa_l);
    } else
      TreeUnLockNci(info, 0, nodenum);
  }
  return status;
}

/*-----------------------------------------------------------------
	Recursively compact all descriptors and adjust pointers.
	NIDs converted to PATHs for TREE$COPY_TO_RECORD.
	Eliminates DSC descriptors. Need DSC for classes A and APD?
-----------------------------------------------------------------*/
int TreeSetTemplateNci(NCI * nci){
  TreeGetThreadStatic()->TemplateNci = *nci;
  return TreeSUCCESS;
}

int TreeLockDatafile(TREE_INFO * info, int readonly, int64_t offset){
  INIT_STATUS;
  int deleted = 1;
  if (! info->header->readonly) {
    while (deleted && status & 1) {
      status = MDS_IO_LOCK(readonly ? info->data_file->get : info->data_file->put,
			   offset, offset >= 0 ? 12 : (DATAF_C_MAX_RECORD_SIZE * 3),
			   readonly ? MDS_IO_LOCK_RD : MDS_IO_LOCK_WRT, &deleted);
      if (deleted && STATUS_OK)
	status = TreeReopenDatafile(info);
    }
  }
  return status;
}

int TreeUnLockDatafile(TREE_INFO * info, int readonly, int64_t offset){
  INIT_STATUS;
  if (! info->header->readonly )
    status = MDS_IO_LOCK(readonly ? info->data_file->get : info->data_file->put,
			 offset, offset >= 0 ? 12 : (DATAF_C_MAX_RECORD_SIZE * 3), MDS_IO_LOCK_NONE, 0);
  return status;
}

#ifdef OLD_LOCK_CODE
#ifdef _WIN32

static int LockStart;
static int LockSize;
int TreeLockDatafile(TREE_INFO * info, int readonly, int64_t offset)
{
  LockStart = offset >= 0 ? offset : MDS_IO_LSEEK(info->data_file->put, 0, SEEK_END);
  LockSize = offset >= 0 ? 12 : 0x7fffffff;
  return LockFile((HANDLE) _get_osfhandle(readonly ? info->data_file->get : info->data_file->put),
		  LockStart, 0, LockSize, 0) == 0 ? TreeFAILURE : TreeSUCCESS;
}

int TreeUnLockDatafile(TREE_INFO * info, int readonly, int64_t offset)
{
  return UnlockFile((HANDLE) _get_osfhandle(readonly ? info->data_file->get : info->data_file->put),
		    LockStart, 0, LockSize, 0) == 0 ? TreeFAILURE : TreeSUCCESS;
}
#else
int TreeLockDatafile(TREE_INFO * info, int readonly, int64_t offset)
{
  struct flock flock_info;
  flock_info.l_type = readonly ? F_RDLCK : F_WRLCK;
  flock_info.l_whence = offset >= 0 ? SEEK_SET : SEEK_END;
  flock_info.l_start = offset >= 0 ? offset : 0;
  flock_info.l_len = offset >= 0 ? 12 : (DATAF_C_MAX_RECORD_SIZE * 3);
  return (fcntl(readonly ? info->data_file->get : info->data_file->put, F_SETLKW, &flock_info) !=
	  -1) ? TreeSUCCESS : TreeLOCK_FAILURE;
}

int TreeUnLockDatafile(TREE_INFO * info, int readonly, int64_t offset)
{
  struct flock flock_info;
  flock_info.l_type = F_UNLCK;
  flock_info.l_whence = SEEK_SET;
  flock_info.l_start = 0;
  flock_info.l_len = 0;
  return (fcntl(readonly ? info->data_file->get : info->data_file->put, F_SETLKW, &flock_info) !=
	  -1) ? TreeSUCCESS : TreeLOCK_FAILURE;
}
#endif
#endif
