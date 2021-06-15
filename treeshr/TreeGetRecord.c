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
#include <mdsplus/mdsconfig.h>
#include <mdsplus/mdsplus.h>

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include <mdsdescrip.h>
#include <mdsshr.h>
#include <mdsshr_messages.h>
#include <ncidef.h>
#include <treeshr.h>

#include "treeshrp.h"
#include "treethreadstatic.h"

#define align(bytes, size) ((((bytes) + (size)-1) / (size)) * (size))

static int64_t ViewDate = -1;

extern void **TreeCtx();

#define min(a, b) (((a) < (b)) ? (a) : (b))
int TreeGetRecord(int nid_in, mdsdsc_xd_t *dsc);
int _TreeGetRecord(void *dbid, int nid_in, mdsdsc_xd_t *dsc)
{
  int status;
  CTX_PUSH(&dbid);
  status = TreeGetRecord(nid_in, dsc);
  CTX_POP(&dbid);
  return status;
}

static inline int read_descriptor(TREE_INFO *info, int length, NCI *nci, int nidx, mdsdsc_xd_t *dsc)
{
  int status = 0;
  INIT_AS_AND_FREE_ON_EXIT(char *, data, malloc(length));
  int nodenum;
  int retsize;
  status = TreeGetDatafile(
      info, nci->DATA_INFO.DATA_LOCATION.rfa, &length,
      data, &retsize, &nodenum, nci->flags2);
  if (STATUS_NOT_OK)
    status = TreeBADRECORD;
  else if (!(nci->flags2 & NciM_NON_VMS) &&
           ((retsize != length) || (nodenum != nidx)))
    status = TreeBADRECORD;
  else
    status = (MdsSerializeDscIn(data, dsc) & 1)
                 ? TreeSUCCESS
                 : TreeBADRECORD;
  FREE_NOW(data);
  return status;
}

int TreeGetRecord(int nid_in, mdsdsc_xd_t *dsc)
{
  void *dbid = *TreeCtx();
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID *nid = (NID *)&nid_in;
  struct descriptor *dptr;
  int status;
  NCI nci;
  MdsFree1Dx(dsc, NULL);
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  if (dblist->remote)
    return GetRecordRemote(dblist, nid_in, dsc);
  TREE_INFO *info;
  int nidx = nid_to_tree_idx(dblist, nid, &info);
  if (info)
  {
    TreeCallHookFun("TreeNidHook", "GetNci", info->treenam, info->shot, nid,
                    NULL);
    status = TreeCallHook(GetNci, info, nid_in);
    if (status && STATUS_NOT_OK)
      return 0;
    status = TreeOpenDatafileR(info);
    if (STATUS_OK)
    {
      int locked = 0;
      status = tree_get_nci(info, nidx, &nci, 0, &locked);
      if (STATUS_OK)
      {
        if (nci.length)
        {
          TreeCallHookFun("TreeNidHook", "GetData", info->treenam,
                          info->shot, nid, NULL);
          status = TreeCallHook(GetData, info, nid_in);
          if (status && STATUS_NOT_OK)
            return 0;
          switch (nci.class)
          {
          case CLASS_APD:
          case CLASS_CA:
          case CLASS_R:
          case CLASS_A:
            if (nci.flags2 & NciM_DATA_IN_ATT_BLOCK)
            {
              status = TreeINVDFFCLASS;
              break;
            }
            MDS_ATTR_FALLTHROUGH
          case CLASS_S:
          case CLASS_XS:
            if (STATUS_OK)
            {
              if (nci.flags2 & NciM_EXTENDED_NCI)
              {
                EXTENDED_ATTRIBUTES attributes;
                status = TreeGetExtendedAttributes(
                    info, RfaToSeek(nci.DATA_INFO.DATA_LOCATION.rfa),
                    &attributes);
                if (STATUS_OK &&
                    attributes
                            .facility_offset[STANDARD_RECORD_FACILITY] !=
                        -1)
                {
                  status = tree_get_dsc(
                      info, nid->tree,
                      attributes
                          .facility_offset[STANDARD_RECORD_FACILITY],
                      attributes
                          .facility_length[STANDARD_RECORD_FACILITY],
                      dsc);
                }
                else if (STATUS_OK &&
                         attributes.facility_offset
                                 [SEGMENTED_RECORD_FACILITY] != -1)
                {
                  status = _TreeGetSegmentedRecord(dbid, nid_in, dsc);
                }
                else
                  status = TreeBADRECORD;
              }
              else
              {
                if (nci.flags2 & NciM_DATA_IN_ATT_BLOCK)
                {
                  dtype_t dsc_dtype = DTYPE_DSC;
                  length_t dlen = nci.length - 8;
                  l_length_t ddlen = dlen + sizeof(struct descriptor);
                  status = MdsGet1Dx(&ddlen, &dsc_dtype, dsc, 0);
                  dptr = dsc->pointer;
                  dptr->length = dlen;
                  dptr->dtype = nci.dtype;
                  dptr->class = CLASS_S;
                  dptr->pointer =
                      (char *)dptr + sizeof(struct descriptor);
                  memcpy(dptr->pointer, nci.DATA_INFO.DATA_IN_RECORD.data,
                         dptr->length);
                  if (dptr->dtype != DTYPE_T)
                  {
                    switch (dptr->length)
                    {
                    case 2:
                      *(int16_t *)dptr->pointer =
                          swapint16(dptr->pointer);
                      break;
                    case 4:
                      *(int32_t *)dptr->pointer =
                          swapint32(dptr->pointer);
                      break;
                    case 8:
                      *(int64_t *)dptr->pointer =
                          swapint64(dptr->pointer);
                      break;
                    }
                  }
                }
                else
                {
                  int length = nci.DATA_INFO.DATA_LOCATION.record_length;
                  if (length > 0)
                  {
                    status = read_descriptor(info, length, &nci, nidx, dsc);
                  }
                  else
                    status = TreeBADRECORD;
                }
              }
            }
            break;
          default:
            status = TreeINVDFFCLASS;
            break;
          }
        }
        else
          status = TreeNODATA;
      }
    }
  }
  else
    status = TreeINVTREE;
  if (STATUS_OK)
    status = TreeMakeNidsLocal((struct descriptor *)dsc, nid_in);
  return status;
}

int TreeOpenDatafileR(TREE_INFO *info)
{
  int status = 1;
  WRLOCKINFO(info);
  if (!info->data_file)
    status = _TreeOpenDatafileR(info);
  UNLOCKINFO(info);
  return status;
}
static inline char *
tree_to_datafile(const char *tree)
{ // replace .tree with .characteristics
  const size_t baselen = strlen(tree) - sizeof("tree") + 1;
  const size_t namelen0 = baselen + sizeof("datafile");
  char *const filename = memcpy(malloc(namelen0), tree, baselen);
  memcpy(filename + baselen, "datafile", namelen0 - baselen);
  return filename;
}
int _TreeOpenDatafileR(TREE_INFO *info)
{
  INIT_STATUS_AS TreeFAILURE;
  if (!info->data_file)
    info->data_file = TreeGetVmDatafile(info);
  if (info->data_file)
  {
    if (!info->data_file->get)
    {
      char *filename = tree_to_datafile(info->filespec);
      const int lun = MDS_IO_OPEN(filename, O_RDONLY, 0);
      free(filename);
      status = (lun == -1) ? TreeFOPENR : TreeSUCCESS;
      info->data_file->get = (lun != -1) ? lun : 0;
    }
  }
  return status;
}

typedef RECORD(1) record_one;
typedef ARRAY(struct descriptor *) array_dsc;

int TreeMakeNidsLocal(struct descriptor *dsc_ptr, int nid)
{
  int status = 1;
  unsigned char tree = ((NID *)&nid)->tree;
  if (dsc_ptr == NULL)
    status = 1;
  else
    switch (dsc_ptr->class)
    {

    case CLASS_D:
    case CLASS_S:
    case CLASS_XD:
    case CLASS_XS:
      if ((dsc_ptr->dtype != DTYPE_DSC) && (dsc_ptr->dtype != DTYPE_NID))
        status = 1;
      else if (dsc_ptr->dtype == DTYPE_DSC)
        status = TreeMakeNidsLocal((struct descriptor *)dsc_ptr->pointer, nid);
      else if (dsc_ptr->dtype == DTYPE_NID)
      {
        NID *nid_ptr = (NID *)dsc_ptr->pointer;
        if ((nid_ptr->node == 0) && (nid_ptr->tree == 0))
          status = 1;
        else
        {
          nid_ptr->node = *(int *)nid_ptr & 0xffffff;
          nid_ptr->tree = tree;
        }
      }
      break;

    case CLASS_A:
      if ((dsc_ptr->dtype != DTYPE_DSC) && (dsc_ptr->dtype != DTYPE_NID))
        status = 1;
      else
        status = TreeUNSUPARRDTYPE;
      break;

    case CLASS_R:
    {
      record_one *rptr = (record_one *)dsc_ptr;
      int i;
      for (i = 0, status = 1; STATUS_OK && (i < rptr->ndesc); i++)
        status = TreeMakeNidsLocal(rptr->dscptrs[i], nid);
    }
    break;

    case CLASS_CA:
      status = 1;
      break;

    case CLASS_APD:
    {
      array_dsc *aptr = (array_dsc *)dsc_ptr;
      int n_elts = aptr->arsize / aptr->length;
      int i;
      for (status = 1, i = 0; STATUS_OK && (i < n_elts); i++)
        status =
            TreeMakeNidsLocal((struct descriptor *)*(aptr->pointer + i), nid);
    }
    break;

    default:
      status = LibINVSTRDES;
    }

  return status;
}

DATA_FILE *TreeGetVmDatafile()
{
  DATA_FILE *datafile_ptr;
  static const mdsdsc_xd_t empty_xd = {0, 0, CLASS_XD, 0, 0};
  int length = align(sizeof(DATA_FILE), sizeof(void *)) +
               align(sizeof(RECORD_HEADER), sizeof(void *)) +
               align(sizeof(ASY_NCI), sizeof(void *)) +
               align(sizeof(NCI), sizeof(void *)) +
               align(sizeof(mdsdsc_xd_t), sizeof(void *));
  datafile_ptr = malloc(length * 2);
  if (datafile_ptr != NULL)
  {
    char *ptr = (char *)datafile_ptr;
    memset(datafile_ptr, 0, length);
    datafile_ptr->record_header =
        (RECORD_HEADER *)(ptr += align(sizeof(DATA_FILE), sizeof(void *)));
    datafile_ptr->asy_nci =
        (ASY_NCI *)(ptr += align(sizeof(RECORD_HEADER), sizeof(void *)));
    datafile_ptr->data =
        (mdsdsc_xd_t *)(ptr += align(sizeof(ASY_NCI), sizeof(void *)));
    *datafile_ptr->data = empty_xd;
    datafile_ptr->asy_nci->nci =
        (NCI *)(ptr += align(sizeof(mdsdsc_xd_t), sizeof(void *)));
  }
  return datafile_ptr;
}

inline static int read_datablob(TREE_INFO *info, int *nodenum, const int64_t rfa_in, char *const record, uint32_t const buffer_space)
{
  int status;
  const int blen = buffer_space //
                   + (buffer_space + DATAF_C_MAX_RECORD_SIZE + 1) /
                         (DATAF_C_MAX_RECORD_SIZE + 2) *
                         sizeof(RECORD_HEADER);
  INIT_AS_AND_FREE_ON_EXIT(char *, buffer, malloc(blen));
  char *bptr = record + buffer_space;
#define RECORD_ALIGN (DATAF_C_MAX_RECORD_SIZE + 2 + sizeof(RECORD_HEADER))
  uint32_t partlen = ((blen - 1) % RECORD_ALIGN) + 1;
  int64_t rfa_l = rfa_in - blen + partlen;
  int deleted = 1;
  do
  {
    status = (MDS_IO_READ_X(info->data_file->get, rfa_l, (void *)buffer,
                            blen, &deleted) == blen)
                 ? TreeSUCCESS
                 : TreeDFREAD;
    if (STATUS_OK && deleted)
      status = TreeReopenDatafile(info);
    else
      break;
  } while (STATUS_OK);
  loadint32(nodenum, &((RECORD_HEADER *)buffer)->node_number);
  char *bptr_in;
  uint32_t bytes_remaining = buffer_space;
  for (bptr_in = buffer; bytes_remaining > 0;)
  {
    const int bytes_this_time = min(DATAF_C_MAX_RECORD_SIZE + 2, bytes_remaining);
    bptr_in += sizeof(RECORD_HEADER);
    memcpy(bptr - bytes_this_time, bptr_in, bytes_this_time);
    bptr_in += bytes_this_time;
    bptr -= bytes_this_time;
    bytes_remaining -= bytes_this_time;
  }
  FREE_NOW(buffer);
  return status;
}

EXPORT int TreeGetDatafile(TREE_INFO *info, unsigned char *rfa_in,
                           int *buffer_size, char *const record, int *retsize,
                           int *nodenum, unsigned char flags)
{
  int status = 1;
  int first = 1;
  unsigned char rfa[6];
  *retsize = 0;
  memcpy(rfa, rfa_in, sizeof(rfa));
  if (!(flags & NciM_DATA_CONTIGUOUS))
  {
    char *bptr = (char *)record;
    uint32_t buffer_space = (uint32_t)*buffer_size;
    while ((rfa[0] || rfa[1] || rfa[2] || rfa[3] || rfa[4] || rfa[5]) &&
           buffer_space && STATUS_OK)
    {
      RECORD_HEADER hdr;
      int64_t rfa_l = RfaToSeek(rfa);
      int deleted = 1;
      do
      {
        status = (MDS_IO_READ_X(info->data_file->get, rfa_l, (void *)&hdr, 12,
                                &deleted) == 12)
                     ? TreeSUCCESS
                     : TreeDFREAD;
        if (STATUS_OK && deleted)
          status = TreeReopenDatafile(info);
        else
          break;
      } while (STATUS_OK);
      if (STATUS_OK)
      {
        uint32_t partlen = (uint32_t)(swapint16(&hdr.rlength) - 10);
        partlen = min(partlen, buffer_space);
        int nidx = swapint32(&hdr.node_number);
        if (first)
          *nodenum = nidx;
        else if (*nodenum != nidx)
        {
          status = 0;
          break;
        }
        deleted = 1;
        do
        {
          status = ((uint32_t)MDS_IO_READ_X(info->data_file->get,
                                            rfa_l + 12, (void *)bptr,
                                            partlen, &deleted) == partlen)
                       ? TreeSUCCESS
                       : TreeDFREAD;
          if (STATUS_OK && deleted)
            status = TreeReopenDatafile(info);
          else
            break;
        } while (STATUS_OK);
        if (STATUS_OK)
        {
          bptr += partlen;
          buffer_space -= partlen;
          *retsize = *retsize + partlen;
          memcpy(rfa, &hdr.rfa, sizeof(rfa));
        }
      }
    }
  }
  else
  {
    int64_t rfa_l = RfaToSeek(rfa);
    if (flags & NciM_NON_VMS)
    {
      int deleted = 1;
      do
      {
        status = (MDS_IO_READ_X(info->data_file->get, rfa_l, (void *)record,
                                *buffer_size, &deleted) == *buffer_size)
                     ? TreeSUCCESS
                     : TreeDFREAD;
        if (STATUS_OK && deleted)
          status = TreeReopenDatafile(info);
        else
          break;
      } while (STATUS_OK);
    }
    else
    {
      status = read_datablob(info, nodenum, rfa_l, record, *buffer_size);
    }
    *retsize = *buffer_size;
  }
  return status;
}

static pthread_mutex_t viewdate_lock = PTHREAD_MUTEX_INITIALIZER;
int TreeSetViewDate(int64_t *date)
{
  TREETHREADSTATIC_INIT;
  if (TREE_PRIVATECTX)
    TREE_VIEWDATE = *date;
  else
  {
    pthread_mutex_lock(&viewdate_lock);
    ViewDate = *date;
    pthread_mutex_unlock(&viewdate_lock);
  }
  return TreeSUCCESS;
}

int TreeGetViewDate(int64_t *date)
{
  TREETHREADSTATIC_INIT;
  if (TREE_PRIVATECTX)
    *date = TREE_VIEWDATE;
  else
  {
    pthread_mutex_lock(&viewdate_lock);
    *date = ViewDate;
    pthread_mutex_unlock(&viewdate_lock);
  }
  return TreeSUCCESS;
}

int TreeGetVersionNci(TREE_INFO *info, NCI *nci, NCI *v_nci)
{
  char nci_bytes[42];
  int status = TreeOpenDatafileR(info);
  if (STATUS_OK)
  {

    int64_t rfa_l = RfaToSeek(nci->DATA_INFO.DATA_LOCATION.rfa);
    rfa_l += ((nci->DATA_INFO.DATA_LOCATION.record_length ==
               (DATAF_C_MAX_RECORD_SIZE + 2))
                  ? (DATAF_C_MAX_RECORD_SIZE + 2)
                  : (nci->DATA_INFO.DATA_LOCATION.record_length %
                     (DATAF_C_MAX_RECORD_SIZE + 2)));
    rfa_l += sizeof(RECORD_HEADER);
    int deleted = 1;
    do
    {
      status = (MDS_IO_READ_X(info->data_file->get, rfa_l, (void *)nci_bytes,
                              42, &deleted) == 42)
                   ? TreeSUCCESS
                   : TreeDFREAD;
      if (STATUS_OK && deleted)
        status = TreeReopenDatafile(info);
      else
        break;
    } while (STATUS_OK);
    if (STATUS_OK)
    {
      TreeSerializeNciIn(nci_bytes, v_nci);
    }
  }
  return status;
}
