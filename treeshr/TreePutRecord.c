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

        Call sequence: TreePutRecord(int nid, struct descriptor *, int
utility_update)

------------------------------------------------------------------------------
   Copyright (c) 1988
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

        Description:

+-----------------------------------------------------------------------------*/
#include <mdsplus/mdsconfig.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <io.h>
#include <windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#include <libroutines.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <mdsshr_messages.h>
#include <mdstypes.h>
#include <ncidef.h>
#include <strroutines.h>
#include <treeshr.h>
#include <usagedef.h>

#include "treeshrp.h"
#include "treethreadstatic.h"

#ifdef min
#undef min
#endif
#define min(a, b) (((a) < (b)) ? (a) : (b))

static int check_usage(PINO_DATABASE *dblist, NID *nid_ptr, NCI *nci);
int tree_fixup_nid(NID *nid, void **dbid_tree, struct descriptor *path);
static int FixupPath();
static int put_datafile(TREE_INFO *info, int nodenum, NCI *nci_ptr,
                        struct descriptor_xd *data_dsc_ptr, NCI *previous_nci,
                        int *ncilocked);
static int update_datafile(TREE_INFO *info, int nodenum, NCI *nci_ptr,
                           struct descriptor_xd *data_dsc_ptr, int *ncilocked);
extern int PutRecordRemote();

extern void **TreeCtx();

int64_t TreeTimeInserted()
{
  int64_t ans;
  LibTimeToVMSTime(0, &ans);
  return ans;
}

int TreePutRecord(int nid, struct descriptor *descriptor_ptr,
                  int utility_update)
{
  return _TreePutRecord(*TreeCtx(), nid, descriptor_ptr, utility_update);
}

int _TreePutRecord(void *dbid, int nid, struct descriptor *descriptor_ptr,
                   int utility_update)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID *nid_ptr = (NID *)&nid;
  int status;
  int open_status;
  TREE_INFO *info_ptr;
  int nidx;
  unsigned int old_record_length = 0;
  static int saved_uic = 0;
  static int saved_uic32 = 0;
  int shot_open;
  EXTENDED_ATTRIBUTES attributes;
  int extended = 0;
  int64_t extended_offset;
  int compress_utility = utility_update == 2;
#ifndef _WIN32
  if (!saved_uic)
  {
    saved_uic = getuid();
    saved_uic32 = (saved_uic & 0xFFFF0000) != 0;
    if (!saved_uic32)
    {
      saved_uic = (getgid() << 16) | (saved_uic);
    }
  }
 #endif
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  if (dblist->open_readonly)
    return TreeREADONLY;
  if (dblist->remote)
    return PutRecordRemote(dblist, nid, descriptor_ptr, utility_update);
  shot_open = (dblist->shotid != -1);
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr)
  {
    int stv;
    NCI local_nci, old_nci;
    int64_t saved_viewdate;
    TreeCallHookFun("TreeNidHook", "PutData", info_ptr->treenam, info_ptr->shot,
                    nid, NULL);
    TreeCallHookFun("TreeNidDataHook", "PutDataFull", info_ptr->treenam,
                    info_ptr->shot, nid, descriptor_ptr, NULL);
    status = TreeCallHook(PutData, info_ptr, nid);
    if (status && STATUS_NOT_OK)
      return status;
    TreeGetViewDate(&saved_viewdate);
    int locked_nci = 0;
    RETURN_IF_NOT_OK(
        tree_get_and_lock_nci(info_ptr, nidx, &local_nci, &locked_nci));
    TreeSetViewDate(&saved_viewdate);
    memcpy(&old_nci, &local_nci, sizeof(local_nci));
    if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
      open_status = TreeOpenDatafileW(info_ptr, &stv, 0);
    else
      open_status = 1;
    if (local_nci.flags2 & NciM_EXTENDED_NCI)
    {
      int ext_status;
      extended_offset = RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa);
      ext_status =
          TreeGetExtendedAttributes(info_ptr, extended_offset, &attributes);
      if (!(ext_status & 1))
      {
        local_nci.flags2 &= ~NciM_EXTENDED_NCI;
      }
      else
      {
        extended = 1;
      }
    }
    if (STATUS_OK)
    {
      if (utility_update)
      {
        TREETHREADSTATIC_INIT;
        local_nci.flags = TREE_TEMPNCI.flags;
        bitassign(0, local_nci.flags, NciM_VERSIONS);
        bitassign((TREE_TEMPNCI.flags2 & NciM_32BIT_UID_NCI), local_nci.flags2, NciM_32BIT_UID_NCI);
        local_nci.owner_identifier = TREE_TEMPNCI.owner_identifier;
        local_nci.time_inserted = TREE_TEMPNCI.time_inserted;
      }
      else
      {
        bitassign(dblist->setup_info, local_nci.flags, NciM_SETUP_INFORMATION);
        bitassign(saved_uic32, local_nci.flags2, NciM_32BIT_UID_NCI);
        local_nci.owner_identifier = saved_uic;
        /* VMS time = unixtime * 10,000,000 + 0x7c95674beb4000q */
        local_nci.time_inserted = TreeTimeInserted();
      }
      if (!(open_status & 1))
      {
        local_nci.DATA_INFO.ERROR_INFO.stv = stv;
        bitassign_c(1, local_nci.flags2, NciM_ERROR_ON_PUT);
        local_nci.DATA_INFO.ERROR_INFO.error_status = open_status;
        local_nci.length = 0;
        tree_put_nci(info_ptr, nidx, &local_nci, &locked_nci);
        return open_status;
      }
      else
      {
        NCI *nci = info_ptr->data_file->asy_nci->nci;
        *nci = local_nci;
        if (nci->length != 0 &&
            ((shot_open && info_ptr->header->versions_in_pulse) ||
             (!shot_open && info_ptr->header->versions_in_model)))
          bitassign(1, nci->flags, NciM_VERSIONS);
        if (!utility_update)
        {
          old_record_length =
              (nci->flags2 & NciM_DATA_IN_ATT_BLOCK ||
               (nci->flags & NciM_VERSIONS))
                  ? 0
                  : nci->DATA_INFO.DATA_LOCATION.record_length;
          if ((nci->flags & NciM_WRITE_ONCE) && nci->length)
            status = TreeNOOVERWRITE;
          if ((STATUS_OK) &&
              (shot_open && (nci->flags & NciM_NO_WRITE_SHOT)))
            status = TreeNOWRITESHOT;
          if ((STATUS_OK) &&
              (!shot_open && (nci->flags & NciM_NO_WRITE_MODEL)))
            status = TreeNOWRITEMODEL;
        }
        if (STATUS_OK)
        {
          unsigned char tree = (unsigned char)nid_ptr->tree;
          int compressible;
          int data_in_altbuf;
          TREETHREADSTATIC_INIT;
          void *dbid_tree[2] = {(void *)dbid, (void *)&tree};
          TREE_NIDREF = TREE_PATHREF = FALSE;
          status = MdsSerializeDscOutZ(
              descriptor_ptr, info_ptr->data_file->data, tree_fixup_nid,
              dbid_tree, FixupPath, 0,
              ((compress_utility || (nci->flags & NciM_COMPRESS_ON_PUT)) &&
                  !(nci->flags & NciM_DO_NOT_COMPRESS)) ? local_nci.compression_method : -1,
              &compressible, &nci->length,
              &nci->DATA_INFO.DATA_LOCATION.record_length, &nci->dtype,
              &nci->class,
              (nci->flags & NciM_VERSIONS || extended)
                  ? 0
                  : sizeof(nci->DATA_INFO.DATA_IN_RECORD.data),
              nci->DATA_INFO.DATA_IN_RECORD.data, &data_in_altbuf);
          bitassign(TREE_PATHREF, nci->flags, NciM_PATH_REFERENCE);
          bitassign(TREE_NIDREF, nci->flags, NciM_NID_REFERENCE);
          bitassign(compressible, nci->flags, NciM_COMPRESSIBLE);
          bitassign_c(data_in_altbuf, nci->flags2, NciM_DATA_IN_ATT_BLOCK);
        }
        if ((STATUS_OK) && nci->length && (!utility_update))
          status = check_usage(dblist, nid_ptr, nci);
        if (STATUS_OK)
        {
          if (nci->flags2 & NciM_DATA_IN_ATT_BLOCK &&
              !(nci->flags & NciM_VERSIONS) && !extended)
          {
            bitassign_c(0, nci->flags2, NciM_ERROR_ON_PUT);
            status = tree_put_nci(info_ptr, nidx, nci, &locked_nci);
          }
          else
          {
            if (extended)
            {
              status = tree_put_dsc(
                  dblist, info_ptr, nid, descriptor_ptr,
                  &attributes.facility_offset[STANDARD_RECORD_FACILITY],
                  &attributes.facility_length[STANDARD_RECORD_FACILITY],
                  (compress_utility || (nci->flags & NciM_COMPRESS_ON_PUT)) &&
                      !(nci->flags & NciM_DO_NOT_COMPRESS));
              if (STATUS_OK)
              {
                attributes.facility_offset[SEGMENTED_RECORD_FACILITY] = -1;
                attributes.facility_length[SEGMENTED_RECORD_FACILITY] = 0;
                status = TreePutExtendedAttributes(info_ptr, &attributes,
                                                   &extended_offset);
              }
              if (STATUS_OK)
              {
                bitassign(0, nci->flags, NciM_SEGMENTED);
                tree_put_nci(info_ptr, nidx, nci, &locked_nci);
              }
              else
                tree_unlock_nci(info_ptr, 0, nidx, &locked_nci);
            }
            else if ((nci->DATA_INFO.DATA_LOCATION.record_length !=
                      old_record_length) ||
                     (nci->DATA_INFO.DATA_LOCATION.record_length >=
                      DATAF_C_MAX_RECORD_SIZE) ||
                     utility_update || (nci->flags & NciM_VERSIONS) ||
                     (nci->flags2 & NciM_ERROR_ON_PUT))
              status =
                  put_datafile(info_ptr, nidx, nci, info_ptr->data_file->data,
                               &old_nci, &locked_nci);
            else
              status =
                  update_datafile(info_ptr, nidx, nci,
                                  info_ptr->data_file->data, &locked_nci);
          }
        }
        else
          tree_unlock_nci(info_ptr, 0, nidx, &locked_nci);
      }
    }
    else
      tree_unlock_nci(info_ptr, 0, nidx, &locked_nci);
  }
  else
    status = TreeINVTREE;
  return status;
}

static int check_usage(PINO_DATABASE *dblist, NID *nid_ptr, NCI *nci)
{

#define is_expression                                             \
  ((nci->dtype == DTYPE_FUNCTION) || (nci->dtype == DTYPE_NID) || \
   (nci->dtype == DTYPE_PATH) || (nci->dtype == DTYPE_IDENT) ||   \
   (nci->dtype == DTYPE_CALL))

#define check(boolean) (boolean) ? TreeSUCCESS : TreeINVDTPUSG;

#define is_numeric                                            \
  (((nci->dtype >= DTYPE_BU) && (nci->dtype <= DTYPE_DC)) ||  \
   ((nci->dtype >= DTYPE_OU) && (nci->dtype <= DTYPE_HC)) ||  \
   ((nci->dtype >= DTYPE_FS) && (nci->dtype <= DTYPE_FTC)) || \
   (nci->dtype == DTYPE_DSC))

  NODE *node_ptr;
  int status;
  node_ptr = nid_to_node(dblist, nid_ptr);
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
    status =
        check(is_numeric || (nci->dtype == DTYPE_PARAM) ||
              (nci->dtype == DTYPE_RANGE) || (nci->dtype == DTYPE_WITH_UNITS) ||
              (nci->dtype == DTYPE_WITH_ERROR) ||
              (nci->dtype == DTYPE_OPAQUE) || is_expression);
    break;
  case TreeUSAGE_SIGNAL:
    status =
        check(is_numeric || (nci->dtype == DTYPE_SIGNAL) ||
              (nci->dtype == DTYPE_DIMENSION) || (nci->dtype == DTYPE_PARAM) ||
              (nci->dtype == DTYPE_RANGE) || (nci->dtype == DTYPE_WITH_UNITS) ||
              (nci->dtype == DTYPE_WITH_ERROR) ||
              (nci->dtype == DTYPE_OPAQUE) || is_expression);
    break;
  case TreeUSAGE_TASK:
    status =
        check((nci->dtype == DTYPE_PROGRAM) || (nci->dtype == DTYPE_ROUTINE) ||
              (nci->dtype == DTYPE_PROCEDURE) || (nci->dtype == DTYPE_METHOD) ||
              is_expression);
    break;
  case TreeUSAGE_TEXT:
    status = check((nci->dtype == DTYPE_T) || (nci->dtype == DTYPE_PARAM) ||
                   (nci->dtype == DTYPE_WITH_UNITS) ||
                   (nci->dtype == DTYPE_OPAQUE) || (nci->dtype == DTYPE_DSC) ||
                   is_expression);
    break;
  case TreeUSAGE_WINDOW:
    status = check((nci->dtype == DTYPE_WINDOW) || is_expression);
    break;
  case TreeUSAGE_AXIS:
    status =
        check(is_numeric || (nci->dtype == DTYPE_SLOPE) ||
              (nci->dtype == DTYPE_RANGE) || (nci->dtype == DTYPE_WITH_UNITS) ||
              (nci->dtype == DTYPE_DIMENSION) || is_expression);
    break;
  default:
    status = TreeSUCCESS;
    break;
  }
  return status;
}

int tree_fixup_nid(NID *nid, void **dbid_tree, struct descriptor *path)
{
  unsigned char *tree = (unsigned char *)dbid_tree[1];
  TREETHREADSTATIC_INIT;
  if (nid->tree != *tree)
  {
    char *path_c = _TreeGetPath(dbid_tree[0], *(int *)nid);
    if (path_c)
    {
      struct descriptor path_d = {0, DTYPE_T, CLASS_S, 0};
      path_d.length = (unsigned short)strlen(path_c);
      path_d.pointer = path_c;
      StrCopyDx(path, &path_d);
      TreeFree(path_c);
    }
    TREE_PATHREF = TRUE;
    return TRUE;
  }
  else
  {
    TREE_NIDREF = TRUE;
    return FALSE;
  }
}

static int FixupPath()
{
  TREETHREADSTATIC_INIT;
  TREE_PATHREF = 1;
  return 0;
}
static inline char *
tree_to_datafile(const char *tree,
                 const int tmpfile)
{ // replace .tree with .characteristics or
  // .characteristics# if tmpfile
  const size_t baselen = strlen(tree) - sizeof("tree") + 1;
  const size_t namelen =
      baselen + (tmpfile ? sizeof("datafile#") - 1 : sizeof("datafile") - 1);
  char *const filename = memcpy(malloc(namelen + 1), tree, baselen);
  memcpy(filename + baselen, "datafile#", namelen - baselen);
  filename[namelen] = '\0';
  return filename;
}
int TreeOpenDatafileW(TREE_INFO *info, int *stv_ptr, int tmpfile)
{
  WRLOCKINFO(info);
  int status = _TreeOpenDatafileW(info, stv_ptr, tmpfile);
  UNLOCKINFO(info);
  return status;
}
int _TreeOpenDatafileW(TREE_INFO *info, int *stv_ptr, int tmpfile)
{
  int status = TreeSUCCESS;
  if (info->header->readonly)
    return TreeREADONLY_TREE;
  DATA_FILE *df_ptr = info->data_file;
  *stv_ptr = 0;
  if (df_ptr == 0)
  {
    df_ptr = TreeGetVmDatafile();
    status = (df_ptr == NULL) ? TreeFAILURE : TreeSUCCESS;
  }
  if (STATUS_OK)
  {
    char *filename = tree_to_datafile(info->filespec, tmpfile);
    int old_fd = df_ptr->get;
    df_ptr->get = MDS_IO_OPEN(
        filename, tmpfile ? O_RDWR | O_CREAT | O_TRUNC | O_EXCL : O_RDONLY,
        0664);
    status = (df_ptr->get == -1) ? TreeFAILURE : TreeSUCCESS;
    if (df_ptr->get == -1)
      df_ptr->get = old_fd;
    else if (old_fd > 0)
      MDS_IO_CLOSE(old_fd);
    if (STATUS_OK)
    {
      old_fd = df_ptr->put;
      df_ptr->put = MDS_IO_OPEN(filename, O_RDWR, 0);
      status = (df_ptr->put == -1) ? TreeFAILURE : TreeSUCCESS;
      if (df_ptr->put == -1)
        df_ptr->put = old_fd;
      else if (old_fd > 0)
        MDS_IO_CLOSE(old_fd);
      if (STATUS_OK)
        df_ptr->open_for_write = 1;
    }
    free(filename);
  }
  else
  {
    free(df_ptr);
    df_ptr = NULL;
  }
  info->data_file = df_ptr;
  if (STATUS_OK)
  {
    TreeCallHookFun("TreeHook", "OpenDataFileWrite", info->treenam,
                    info->shot, NULL);
    TreeCallHook(OpenDataFileWrite, info, 0);
  }
  return status;
}

static int put_datafile(TREE_INFO *info, int nodenum, NCI *nci_ptr,
                        struct descriptor_xd *data_dsc_ptr, NCI *old_nci_ptr,
                        int *ncilocked)
{
  int status = TreeSUCCESS;
  unsigned int bytes_to_put =
      data_dsc_ptr->l_length > 0
          ? nci_ptr->DATA_INFO.DATA_LOCATION.record_length
          : 0;
  int blen = bytes_to_put + (bytes_to_put + DATAF_C_MAX_RECORD_SIZE + 1) /
                                (DATAF_C_MAX_RECORD_SIZE + 2) *
                                sizeof(RECORD_HEADER);
  static int nonvms_compatible = -1;
  char *buffer = NULL;
  char *bptr;
  int64_t eof;
  unsigned char rfa[6];
  if (blen > 0)
  {
    bitassign_c(1, nci_ptr->flags2, NciM_DATA_CONTIGUOUS);
    if (nonvms_compatible == -1)
      nonvms_compatible = getenv("NONVMS_COMPATIBLE") != NULL;
    if (nonvms_compatible)
    {
      bitassign_c(1, nci_ptr->flags2, NciM_NON_VMS);
      buffer = (char *)data_dsc_ptr->pointer->pointer;
      bptr = buffer + nci_ptr->DATA_INFO.DATA_LOCATION.record_length;
      status = TreeLockDatafile(info, 0, 0);
      eof = MDS_IO_LSEEK(info->data_file->put, 0, SEEK_END);
    }
    else
    {
      buffer = (char *)malloc(blen);
      bptr = buffer;
      loadint32(&info->data_file->record_header->node_number, &nodenum);
      bitassign_c(0, nci_ptr->flags2, NciM_NON_VMS);
      memset(&info->data_file->record_header->rfa, 0, sizeof(RFA));
      status = TreeLockDatafile(info, 0, 0);
      if (STATUS_OK)
      {
        eof = MDS_IO_LSEEK(info->data_file->put, 0, SEEK_END);
        while (bytes_to_put)
        {
          const int bytes_this_time =
              min(DATAF_C_MAX_RECORD_SIZE + 2, bytes_to_put);
          bytes_to_put -= bytes_this_time;
          uint16_t rlength = bytes_this_time + 10;
          loadint16(&info->data_file->record_header->rlength, &rlength);
          memcpy(bptr, (void *)info->data_file->record_header,
                 sizeof(RECORD_HEADER));
          bptr += sizeof(RECORD_HEADER);
          memcpy(bptr,
                 (void *)(((char *)data_dsc_ptr->pointer->pointer) +
                          bytes_to_put),
                 bytes_this_time);
          bptr += bytes_this_time;
          SeekToRfa(eof, rfa);
          memcpy(&info->data_file->record_header->rfa, rfa,
                 sizeof(info->data_file->record_header->rfa));
          if (bytes_to_put)
            eof += sizeof(RECORD_HEADER) + bytes_this_time;
        }
      }
    }
  }
  else
  {
    if (nci_ptr->flags & NciM_VERSIONS)
      status = TreeLockDatafile(info, 0, 0);
    bptr = buffer;
    eof = -1;
  }
  if (STATUS_OK)
  {
    if (bptr == buffer || MDS_IO_WRITE(info->data_file->put, (void *)buffer,
                                       bptr - buffer) == (bptr - buffer))
    {
      bitassign_c(0, nci_ptr->flags2, NciM_ERROR_ON_PUT);
      if (bptr > buffer)
      {
        SeekToRfa(eof, rfa);
        memcpy(nci_ptr->DATA_INFO.DATA_LOCATION.rfa, rfa,
               sizeof(nci_ptr->DATA_INFO.DATA_LOCATION.rfa));
      }
      if (nci_ptr->flags & NciM_VERSIONS)
      {
        char nci_bytes[42];
        int64_t seek_end;
        TreeSerializeNciOut(old_nci_ptr, nci_bytes);
        seek_end = MDS_IO_LSEEK(info->data_file->put, 0, SEEK_END);
        if (seek_end != -1 &&
            MDS_IO_WRITE(info->data_file->put, nci_bytes,
                         sizeof(nci_bytes)) != sizeof(nci_bytes))
        {
          fprintf(stderr, "ERROR: put_datafile writing old nci incomplete\n");
          status = TreeFAILURE;
        }
      }
    }
    else
    {
      bitassign_c(1, nci_ptr->flags2, NciM_ERROR_ON_PUT);
      nci_ptr->DATA_INFO.ERROR_INFO.error_status = TreeFAILURE;
      nci_ptr->length = 0;
    }
    TreeUnLockDatafile(info, 0, 0);
    if (STATUS_OK)
      // unless put_datafile writing old nci incomplete
      status = tree_put_nci(info, nodenum, nci_ptr, ncilocked);
  }
  if (buffer && (!nonvms_compatible))
    free(buffer);
  return status;
}

static int update_datafile(TREE_INFO *info, int nodenum, NCI *nci_ptr,
                           struct descriptor_xd *data_dsc_ptr, int *ncilocked)
{
  int status = TreeSUCCESS;
  unsigned int bytes_to_put = nci_ptr->DATA_INFO.DATA_LOCATION.record_length;
  loadint32(&info->data_file->record_header->node_number, &nodenum);
  memset(&info->data_file->record_header->rfa, 0, sizeof(RFA));
  while (bytes_to_put && STATUS_OK)
  {
    const int bytes_this_time = min(DATAF_C_MAX_RECORD_SIZE + 2, bytes_to_put);
    const int64_t rfa_l = RfaToSeek(nci_ptr->DATA_INFO.DATA_LOCATION.rfa);
    uint16_t rlength = bytes_this_time + 10;
    loadint16(&info->data_file->record_header->rlength, &rlength);
    status = TreeLockDatafile(info, 0, rfa_l);
    if (STATUS_OK)
    {
      MDS_IO_LSEEK(info->data_file->put, rfa_l, SEEK_SET);
      if (MDS_IO_WRITE(info->data_file->put,
                       (void *)info->data_file->record_header,
                       sizeof(RECORD_HEADER)) == sizeof(RECORD_HEADER))
      {
        bytes_to_put -= bytes_this_time;
        status =
            (MDS_IO_WRITE(info->data_file->put,
                          (void *)(((char *)data_dsc_ptr->pointer->pointer) +
                                   bytes_to_put),
                          bytes_this_time) == bytes_this_time)
                ? TreeSUCCESS
                : TreeFAILURE;
      }
      else
        status = TreeFAILURE;
      TreeUnLockDatafile(info, 0, rfa_l);
    }
  }
  bitassign(0, nci_ptr->flags, NciM_SEGMENTED);
  if (STATUS_OK)
    bitassign_c(0, nci_ptr->flags2, NciM_ERROR_ON_PUT);
  else
  {
    bitassign_c(1, nci_ptr->flags2, NciM_ERROR_ON_PUT);
    nci_ptr->DATA_INFO.ERROR_INFO.error_status = status;
    nci_ptr->length = 0;
  }
  const int stat = tree_put_nci(info, nodenum, nci_ptr, ncilocked);
  return STATUS_OK ? stat : status;
}

/*-----------------------------------------------------------------
        Recursively compact all descriptors and adjust pointers.
        NIDs converted to PATHs for TREE$COPY_TO_RECORD.
        Eliminates DSC descriptors. Need DSC for classes A and APD?
-----------------------------------------------------------------*/
int TreeSetTemplateNci(NCI *nci)
{
  TREETHREADSTATIC_INIT;
  TREE_TEMPNCI = *nci;
  return TreeSUCCESS;
}

int TreeLockDatafile(TREE_INFO *info, int readonly, int64_t offset)
{
  INIT_STATUS;
  int deleted = 1;
  if (!info->header->readonly)
  {
    while (deleted && STATUS_OK)
    {
      status =
          MDS_IO_LOCK(readonly ? info->data_file->get : info->data_file->put,
                      offset, offset >= 0 ? 12 : (DATAF_C_MAX_RECORD_SIZE * 3),
                      readonly ? MDS_IO_LOCK_RD : MDS_IO_LOCK_WRT, &deleted);
      if (deleted && STATUS_OK)
        status = TreeReopenDatafile(info);
    }
  }
  return status;
}

int TreeUnLockDatafile(TREE_INFO *info, int readonly, int64_t offset)
{
  INIT_STATUS;
  if (!info->header->readonly)
    status = MDS_IO_LOCK(
        readonly ? info->data_file->get : info->data_file->put, offset,
        offset >= 0 ? 12 : (DATAF_C_MAX_RECORD_SIZE * 3), MDS_IO_LOCK_NONE, 0);
  return status;
}
