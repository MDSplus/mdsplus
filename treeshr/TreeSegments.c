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
#define DEF_FREEXD
#include "treethreadsafe.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <treeshr.h>
#include <usagedef.h>
#include <libroutines.h>
#include <mdsshr.h>
#include <mdsshr_messages.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <ctype.h>
#include <mds_stdarg.h>
#include <STATICdef.h>
extern void **TreeCtx();

/*** Segmented Record Support  ************************

Segmented records use a special extended attr record stored at the file offset (nci rfa field)
which describes the location of segment index and segment information metadata stored in the
datafile. There is an EXTENDED_NCI bit in the nci flags2 field which indicates that the node contains
extended nci information. The EXTENDED_NCI capability supports different types of "facilities" of which
3 are currently supported: STANDARD_RECORD, SEGMENTED_RECORD, NAMED_ATTRIBUTES. The NAMED_ATTRIBUTE
facility provides a means for adding extra node characteristics by defining name/value pairs. The use
of the STANDARD_RECORD facility is currently only used if the node also has named vars->attr.

When writing a segment in a node for the first time, an extended attribute record is stored for the node
which contains a file offset where the segment header information is stored. The segment header contains
information such as the datatype, the dimensions of rows in the segments, the length of each data element,
the current segment index, the next row to be stored in that segment and file offsets for a segment
index, and offsets for the data and dimension of the segment currently in progress (timestamped segments).

The segment index is a linked list segment indexes each containing information of up to 128 segments. The
index offset in the segment header points to the most recently stored index page. Each index page contains
information about the segments including the segment start and end, the segments dimension and the offset
of the data for the segment.

Since the structures used to index and define segments was designed prior to the addition of new features
such as timestamped segments and compress segments, the fields in the segment descriptions are used in various
ways to indicate different segment properties.

Below are the special settings that are used to indicate characteristics of a segment:

segment_info.rows:
   Normally indicates the number of rows in the segment. If rows is negative it indicates that the
   segment is stored as a serialized record located in the file a data_offset with length equal to the
   lower 31 bits of the rows field. This special indicator is used when segments are compressed or when
   segments are DTYPE_OPAQUE (i.e. image storage).

segment_info.dimension_length
   For normal segments the segment has its own dimension stored as a serialized dimension and the dimension
   offset is where the dimension data is stored and the dimension length is the length of that data.
   For timestamped segments, the dimension length is set to 0 indicating that the dimension is a list of
   64-bit timestamps, one for each row, located at the dimension offset in the file.

segment_info.xxxx_offset
   These fields are used to indicate file offsets where particular data can be found. They are set to -1 to
   indicate that there is no data for xxxx stored.

********************************************************/
#define INIT_TREESUCCESS INIT_STATUS_AS TreeSUCCESS
typedef struct common_vars {
PINO_DATABASE*		dblist;
NID*			nid_ptr;
TREE_INFO*		tinfo;
NODE*			node_ptr;
int64_t			saved_viewdate;
int			nidx;
int			stv;
NCI			local_nci;	// was pointer
int			nci_locked;
EXTENDED_ATTRIBUTES	attr;		// was pointer
int                     attr_update;    // was pointer
int64_t                 attr_offset;    // was pointer
SEGMENT_HEADER		shead;
SEGMENT_INDEX		sindex;         // was pointer
int64_t                 index_offset;
SEGMENT_INFO*           sinfo;
int                     compress;
// write
int                     idx;
int                     rows_filled;
uint32_t                add_length;
} vars_t;

#define INIT_VARS \
int status; \
vars_t _vars = {0}; \
vars_t* vars = &_vars; \
_vars.dblist = (PINO_DATABASE *) dbid; \
_vars.nid_ptr = (NID *) & nid;

#define INIT_WRITE_VARS INIT_VARS _vars.idx = idx; _vars.rows_filled = rows_filled;

static void unlock_nci(void* vars_in) {
  vars_t*vars = (vars_t*)vars_in;
  if (vars->nci_locked) {
    TreeUnLockNci(vars->tinfo, 0, vars->nidx);
    vars->nci_locked = 0;
  }
}
static void lock_nci(void* vars_in) {
  vars_t*vars = (vars_t*)vars_in;
  //  fprintf(stderr,"lock_nci-------------------------------------------------------------------------------------------------\n");
  if (!vars->nci_locked) {
    TreeLockNci(vars->tinfo, 0, vars->nidx, NULL);
    vars->nci_locked = 1;
  }
}

#ifdef WORDS_BIGENDIAN
 #define ALLOCATE_BUFFER(SIZE,BUFFER) char *BUFFER = malloc(SIZE);
 inline static void endianTransfer(const char* buffer_in, const size_t size, const int length, char* buffer_out) {
    char *bptr;
    int i;
    switch (length) {
    case 2:
      for (i = 0, bptr = (char*)buffer_out; i < (int)size / length; i++, bptr += sizeof(short))
        LoadShort(((short *)buffer_in)[i], bptr);
      break;
    case 4:
      for (i = 0, bptr = (char*)buffer_out; i < (int)size / length; i++, bptr += sizeof(int))
        LoadInt(((int *)buffer_in)[i], bptr);
      break;
    case 8:
      for (i = 0, bptr = (char*)buffer_out; i < (int)size / length; i++, bptr += sizeof(int64_t))
        LoadQuad(((int64_t *) buffer_in)[i], bptr);
      break;
    }
 }
 #define CHECK_ENDIAN(POINTER,SIZE,LENGTH,DTYPE){
  if (LENGTH > 1 && DTYPE+0 != DTYPE_T && DTYPE+0 != DTYPE_IDENT && DTYPE+0 != DTYPE_PATH)
    endianTransfer((char*)BUFFER,SIZE,LENGTH,(char*)BUFFER);
 }
 #define CHECK_ENDIAN_TRANSFER(BUFFER_IN,SIZE,LENGTH,DTYPE,BUFFER_OUT){
  if (LENGTH > 1 && DTYPE+0 != DTYPE_T && DTYPE+0 != DTYPE_IDENT && DTYPE+0 != DTYPE_PATH)
    endianTransfer((char*)BUFFER_IN,SIZE,LENGTH,BUFFER_OUT);
  else memcpy(BUFFER_OUT,BUFFER_IN,SIZE);
 }
 #define FREE_BUFFER(BUFFER) free(BUFFER);
#else
 #define ALLOCATE_BUFFER(SIZE,BUFFER) char *BUFFER;
 #define CHECK_ENDIAN(POINTER,SIZE,LENGTH,DTYPE) {}
 #define CHECK_ENDIAN_TRANSFER(BUFFER_IN,SIZE,LENGTH,DTYPE,BUFFER_OUT) BUFFER_OUT = (char*)BUFFER_IN;
 #define FREE_BUFFER(BUFFER) {}
#endif



typedef ARRAY_COEFF(char, 8) A_COEFF_TYPE;
static int PutNamedAttributesIndex(TREE_INFO * tinfo, NAMED_ATTRIBUTES_INDEX * index, int64_t * offset);
static int PutSegmentHeader(TREE_INFO * tinfo, SEGMENT_HEADER * hdr, int64_t * offset);
static int PutSegmentIndex(TREE_INFO * tinfo, SEGMENT_INDEX * idx, int64_t * offset);
static int PutInitialValue(TREE_INFO * tinfo, int *dims, struct descriptor_a *array, int64_t * offset);
static int PutDimensionValue(TREE_INFO * tinfo, int64_t * timestamps, int rows_filled, int ndims, int *dims, int64_t * offset);
static int GetSegmentHeader(TREE_INFO * tinfo, int64_t offset, SEGMENT_HEADER * hdr);
static int GetSegmentIndex(TREE_INFO * tinfo, int64_t offset, SEGMENT_INDEX * idx);
static int GetNamedAttributesIndex(TREE_INFO * tinfo, int64_t offset, NAMED_ATTRIBUTES_INDEX * index);

int _TreeMakeSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end, struct descriptor *dimension, struct descriptor_a *initialValue, int idx, int rows_filled);
int TreeMakeSegment(int nid, struct descriptor *start, struct descriptor *end, struct descriptor *dimension, struct descriptor_a *initialValue, int idx, int rows_filled){
  return _TreeMakeSegment(*TreeCtx(), nid, start, end, dimension, initialValue, idx, rows_filled);
}
int _TreeBeginSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end, struct descriptor *dimension, struct descriptor_a *initialValue, int idx){
  return _TreeMakeSegment(dbid, nid, start, end, dimension, initialValue, idx, 0);
}
int TreeBeginSegment(int nid, struct descriptor *start, struct descriptor *end, struct descriptor *dimension, struct descriptor_a *initialValue, int idx){
  return _TreeBeginSegment(*TreeCtx(), nid, start, end, dimension, initialValue, idx);
}

int _TreeMakeTimestampedSegment(void *dbid, int nid, int64_t * timestamps, struct descriptor_a *initialValue, int idx, int rows_filled);
int TreeMakeTimestampedSegment(int nid, int64_t * timestamps, struct descriptor_a *initialValue, int idx, int rows_filled){
  return _TreeMakeTimestampedSegment(*TreeCtx(), nid, timestamps, initialValue, idx, rows_filled);
}
int _TreeBeginTimestampedSegment(void *dbid, int nid, struct descriptor_a *initialValue, int idx){
  return _TreeMakeTimestampedSegment(dbid, nid, 0, initialValue, idx, 0);
}
int TreeBeginTimestampedSegment(int nid, struct descriptor_a *initialValue, int idx){
  return _TreeBeginTimestampedSegment(*TreeCtx(), nid, initialValue, idx);
}

int _TreeUpdateSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end,
                       struct descriptor *dimension, int idx);
int TreeUpdateSegment(int nid, struct descriptor *start, struct descriptor *end,
                      struct descriptor *dimension, int idx){
  return _TreeUpdateSegment(*TreeCtx(), nid, start, end, dimension, idx);
}

int _TreePutSegment(void *dbid, int nid, const int startIdx, struct descriptor_a *data);
int TreePutSegment(const int nid, const int startIdx, struct descriptor_a *data){
  return _TreePutSegment(*TreeCtx(), nid, startIdx, data);
}

int _TreeGetSegmentTimesXd(void *dbid, int nid, int *nsegs, struct descriptor_xd *start_list, struct descriptor_xd *end_list);
int TreeGetSegmentTimesXd(int nid, int *nsegs, struct descriptor_xd *start_list, struct descriptor_xd *end_list){
  return _TreeGetSegmentTimesXd(*TreeCtx(), nid, nsegs, start_list, end_list);
}

int _TreeGetSegmentTimes(void *dbid, int nid, int *nsegs, int64_t ** times);
int TreeGetSegmentTimes(int nid, int *nsegs, int64_t ** times){
  return _TreeGetSegmentTimes(*TreeCtx(), nid, nsegs, times);
}

int _TreeGetNumSegments(void *dbid, int nid, int *num);
int TreeGetNumSegments(int nid, int *num){
  return _TreeGetNumSegments(*TreeCtx(), nid, num);
}

int TreeGetSegment(int nid, int idx, struct descriptor_xd *segment, struct descriptor_xd *dim){
  return _TreeGetSegment(*TreeCtx(), nid, idx, segment, dim);
}

int _TreeGetSegmentLimits(void *dbid, int nid, int idx, struct descriptor_xd *retStart, struct descriptor_xd *retEnd);
int TreeGetSegmentLimits(int nid, int idx, struct descriptor_xd *retStart, struct descriptor_xd *retEnd){
  return _TreeGetSegmentLimits(*TreeCtx(), nid, idx, retStart, retEnd);
}

int _TreeGetSegmentInfo(void *dbid, int nid, int idx, char *dtype, char *dimct, int *dims, int *next_row);
int TreeGetSegmentInfo(int nid, int idx, char *dtype, char *dimct, int *dims, int *next_row){
  return _TreeGetSegmentInfo(*TreeCtx(), nid, idx, dtype, dimct, dims, next_row);
}

int _TreeSetXNci(void *dbid, int nid, const char *xnciname, struct descriptor *value);
int TreeSetXNci(int nid, const char *xnciname, struct descriptor *value){
  return _TreeSetXNci(*TreeCtx(), nid, xnciname, value);
}

int _TreeGetXNci(void *dbid, int nid, const char *xnciname, struct descriptor_xd *value);
int TreeGetXNci(int nid, const char *xnciname, struct descriptor_xd *value){
  return _TreeGetXNci(*TreeCtx(), nid, xnciname, value);
}

int TreeGetSegments(int nid, struct descriptor *start, struct descriptor *end, struct descriptor_xd *out){
  return _TreeGetSegments(*TreeCtx(), nid, start, end, out);
}

int TreeGetSegmentedRecord(int nid, struct descriptor_xd *data){
  return _TreeGetSegmentedRecord(*TreeCtx(), nid, data);
}

int _TreePutRow(void *dbid, int nid, int bufsize, int64_t * timestamp, struct descriptor_a *data);
int TreePutRow(int nid, int bufsize, int64_t * timestamp, struct descriptor_a *data){
  return _TreePutRow(*TreeCtx(), nid, bufsize, timestamp, data);
}

int _TreePutTimestampedSegment(void *dbid, int nid, int64_t * timestamp, struct descriptor_a *data);
int TreePutTimestampedSegment(int nid, int64_t * timestamp, struct descriptor_a *data){
  return _TreePutTimestampedSegment(*TreeCtx(), nid, timestamp, data);
}

int _TreeGetSegmentScale(void *dbid, int nid, struct descriptor_xd *value);
int TreeGetSegmentScale(int nid, struct descriptor_xd *value) {
  return _TreeGetSegmentScale(*TreeCtx(), nid, value);
}

int _TreeSetSegmentScale(void *dbid, int nid, struct descriptor *value);
int TreeSetSegmentScale(int nid, struct descriptor *value) {
  return _TreeSetSegmentScale(*TreeCtx(), nid, value);
}

inline static int load_node_ptr(vars_t* vars) {
  vars->node_ptr = nid_to_node(vars->dblist, vars->nid_ptr);
  if (!vars->node_ptr)
    return TreeNNF;
// if (vars->node_ptr->usage != TreeUSAGE_SIGNAL) return TreeINVDTPUSG;
  return TreeSUCCESS;
}

inline static int check_segment_remote(vars_t* vars) {
  if (vars->dblist->remote) {
    printf("Segmented records are not supported using thick client mode\n");
    return TreeFAILURE;
  }
  return TreeSUCCESS;
}

inline static int load_info_ptr(vars_t* vars) {
  vars->nidx = nid_to_tree_idx(vars->dblist, vars->nid_ptr, &vars->tinfo);
  if (!vars->tinfo) return TreeNNF;
  return TreeSUCCESS;
}

#define shot_open (vars->dblist->shotid != -1)

inline static int open_datafile_write0(vars_t* vars) {
  INIT_TREESUCCESS;
  if (vars->dblist->open_readonly)
    return TreeREADONLY;
  RETURN_IF_NOT_OK(load_node_ptr(vars));
  RETURN_IF_NOT_OK(check_segment_remote(vars));
  RETURN_IF_NOT_OK(load_info_ptr(vars));
  status = TreeCallHook(PutData, vars->tinfo, *(int*)vars->nid_ptr);
  if (status && STATUS_NOT_OK)
    return status;
  TreeGetViewDate(&vars->saved_viewdate);
  status = TreeGetNciLw(vars->tinfo, vars->nidx, &vars->local_nci);
  if STATUS_NOT_OK return status;
  vars->nci_locked = 1;
  if (vars->dblist->shotid == -1) {
    if (vars->local_nci.flags & NciM_NO_WRITE_MODEL)
        return TreeNOWRITEMODEL;
  } else {
    if (vars->local_nci.flags & NciM_NO_WRITE_SHOT)
	return TreeNOWRITESHOT;
  }
  if (STATUS_OK && (vars->local_nci.flags & NciM_WRITE_ONCE)) {
    if (vars->local_nci.length)
      return TreeNOOVERWRITE;
    vars->local_nci.flags &= ~NciM_WRITE_ONCE;
  }
  return status;
}

inline static int open_datafile_write1(vars_t* vars){
  if (vars->tinfo->data_file ? (!vars->tinfo->data_file->open_for_write) : 1)
     return TreeOpenDatafileW(vars->tinfo, &vars->stv, 0);
  return TreeSUCCESS;
}


inline static int begin_finish(vars_t* vars){
  int status = PutSegmentIndex(vars->tinfo,&vars->sindex,&vars->shead.index_offset);
  if STATUS_NOT_OK return status;
  status = PutSegmentHeader(vars->tinfo, &vars->shead, &vars->attr.facility_offset[SEGMENTED_RECORD_FACILITY]);
  if (vars->attr_update) {
    status = TreePutExtendedAttributes(vars->tinfo, &vars->attr, &vars->attr_offset);
    SeekToRfa(vars->attr_offset, vars->local_nci.DATA_INFO.DATA_LOCATION.rfa);
    vars->local_nci.flags2 |= NciM_EXTENDED_NCI;
  }
  if STATUS_NOT_OK return status;
  if (((int64_t) vars->local_nci.length + (int64_t) vars->add_length) < (int64_t)0xffffffffU)
    vars->local_nci.length += vars->add_length;
  else
    vars->local_nci.length = 0xffffffffU;
  vars->local_nci.flags=vars->local_nci.flags | NciM_SEGMENTED;
  return TreePutNci(vars->tinfo, vars->nidx, &vars->local_nci, 0);
}

#ifndef _WIN32
static int saved_uic = 0;
static void init_saved_uic() {
  saved_uic = (getgid() << 16) | getuid();
}
#endif

inline static void begin_local_nci(vars_t* vars, const struct descriptor_a *initialValue){
  vars->local_nci.flags2 &= ~NciM_DATA_IN_ATT_BLOCK;
  vars->local_nci.dtype = initialValue->dtype;
  vars->local_nci.class = CLASS_R;
  vars->local_nci.time_inserted = TreeTimeInserted();
#ifndef _WIN32
  RUN_FUNCTION_ONCE(init_saved_uic);
#else
  const int saved_uic = 0;
#endif
  vars->local_nci.owner_identifier = saved_uic;
}

/* See if node is currently using the Extended Nci feature and if so get the current contents of the attr
 * index. If not, make an empty index and flag that a new index needs to be written.
 */
#define IF_NO_EXTENDED_NCI \
if (!(vars->local_nci.flags2 & NciM_EXTENDED_NCI) \
 || IS_NOT_OK(TreeGetExtendedAttributes(vars->tinfo, RfaToSeek(vars->local_nci.DATA_INFO.DATA_LOCATION.rfa), &vars->attr)))


inline static void begin_extended_nci(vars_t* vars){
  IF_NO_EXTENDED_NCI {
    memset(&vars->attr, -1, sizeof(vars->attr));
    vars->attr_update = 1;
    vars->attr_offset = -1;
  } else {
    vars->attr_offset = RfaToSeek(vars->local_nci.DATA_INFO.DATA_LOCATION.rfa);
    if (vars->attr.facility_offset[STANDARD_RECORD_FACILITY] != -1) {
      vars->attr.facility_offset[STANDARD_RECORD_FACILITY] = -1;
      vars->attr.facility_length[STANDARD_RECORD_FACILITY] = 0;
      vars->attr_update = 1;
    }
  }
}

/* See if the node currently has an segment header record.
 * If not, make an empty segment header and flag that a new one needs to be written.
 */
#define IF_NO_SEGMENT_HEADER \
if (vars->attr.facility_offset[SEGMENTED_RECORD_FACILITY] == -1 \
 || IS_NOT_OK(GetSegmentHeader(vars->tinfo, vars->attr.facility_offset[SEGMENTED_RECORD_FACILITY],&vars->shead)))


inline static int begin_segment_header(vars_t* vars,struct descriptor_a *initialValue){
  IF_NO_SEGMENT_HEADER {
    memset(&vars->shead, 0, sizeof(vars->shead));
    vars->attr.facility_offset[SEGMENTED_RECORD_FACILITY] = -1;
    vars->shead.index_offset = -1;
    vars->shead.idx = -1;
    vars->attr_update = 1;
  } else if (initialValue->dtype != vars->shead.dtype ||
            (initialValue->class == CLASS_A &&
            (initialValue->dimct != vars->shead.dimct ||
            (initialValue->dimct > 1
             && memcmp(vars->shead.dims, ((A_COEFF_TYPE *)initialValue)->m, (initialValue->dimct - 1) * sizeof(int))))))
    return TreeFAILURE;
  return TreeSUCCESS;
}


/* See if the node currently has an *sindex record.
 * If not, make an empty segment index and flag that a new one needs to be written.
 */
#define IF_NO_SEGMENT_INDEX \
if ((vars->shead.index_offset == -1) \
 || IS_NOT_OK(GetSegmentIndex(vars->tinfo, vars->shead.index_offset, &vars->sindex)))

inline static void begin_segment_index(vars_t* vars) {
  IF_NO_SEGMENT_INDEX {
    vars->shead.index_offset = -1;
    memset(&vars->sindex, -1, sizeof(vars->sindex));
    vars->sindex.first_idx = 0;
  }
}

inline static int check_sinfo(vars_t*vars) {
  INIT_TREESUCCESS;
  if (vars->idx < 0 || vars->idx > vars->shead.idx)
    return TreeFAILURE;
  for (vars->index_offset = vars->shead.index_offset;
       STATUS_OK && vars->idx < vars->sindex.first_idx && vars->sindex.previous_offset > 0;) {
    vars->index_offset = vars->sindex.previous_offset;
    status = GetSegmentIndex(vars->tinfo, vars->sindex.previous_offset, &vars->sindex);
  }
  if (STATUS_NOT_OK || (vars->idx < vars->sindex.first_idx))
    return TreeFAILURE;
  vars->sinfo = &vars->sindex.segment[vars->idx % SEGMENTS_PER_INDEX];
  return status;
}

inline static int begin_sinfo(vars_t*vars,struct descriptor_a *initialValue,int checkcompress()) {
  INIT_TREESUCCESS;
  vars->add_length = 0;
  if (vars->idx == -1) {
    vars->shead.idx++;
    vars->idx = vars->shead.idx;
    vars->add_length = (initialValue->class == CLASS_A) ? initialValue->arsize : 0;
  } else if (vars->idx < -1 || vars->idx > vars->shead.idx)
    return TreeBUFFEROVF;
  else {
    /* TODO: Add support for updating an existing segment. vars->add_length=new_length-old_length. */
    /* Potentially use same storage area if old array is same size. */
    printf("this is not yet supported\n");
    return TreeFAILURE;
  }
  vars->shead.data_offset = -1;
  vars->shead.dim_offset = -1;
  vars->shead.dtype = initialValue->dtype;
  vars->shead.dimct = (initialValue->class == CLASS_A) ? initialValue->dimct : 0;
  vars->shead.length = (initialValue->class == CLASS_A) ? initialValue->length : 0;
  int previous_length = -1;
  if (vars->shead.idx > 0 && vars->shead.length != 0) {
    int d;
    previous_length = vars->shead.length;
    for (d = 0; d < vars->shead.dimct; d++)
    previous_length *= vars->shead.dims[d];
  } else previous_length = -1;
  if (initialValue->class == CLASS_A) {
    if (initialValue->dimct == 1)
      vars->shead.dims[0] = initialValue->arsize / initialValue->length;
    else
      memcpy(vars->shead.dims, ((A_COEFF_TYPE *)initialValue)->m, initialValue->dimct * sizeof(int));
  }
  vars->shead.next_row = vars->rows_filled;
  /* If not the first segment, see if we can reuse the previous segment storage space and compress the previous segment. */
  if (((vars->shead.idx % SEGMENTS_PER_INDEX) > 0) &&
      (previous_length == (int64_t)vars->add_length) && vars->compress) {
    EMPTYXD(xd_data);
    EMPTYXD(xd_dim);
    vars->sinfo = &vars->sindex.segment[(vars->idx % SEGMENTS_PER_INDEX) - 1];
    unlock_nci(vars);
    status = _TreeGetSegment(vars->dblist, *(int*)vars->nid_ptr, vars->idx - 1, &xd_data, &xd_dim);
    lock_nci(vars);
    if STATUS_OK
      status = checkcompress(vars,xd_data,xd_dim,initialValue);
    MdsFree1Dx(&xd_data, 0);
    MdsFree1Dx(&xd_dim, 0);
  }
  if (vars->idx >= vars->sindex.first_idx + SEGMENTS_PER_INDEX) {
    memset(&vars->sindex, -1, sizeof(vars->sindex));
    vars->sindex.previous_offset = vars->shead.index_offset;
    vars->shead.index_offset = -1;
    vars->sindex.first_idx = vars->idx;
  }
  vars->sinfo = &vars->sindex.segment[vars->idx % SEGMENTS_PER_INDEX];
  return status;
}

inline static int putdata_initialvalue(vars_t*vars,struct descriptor_a* initialValue) {
  int status;
  if (initialValue->dtype == DTYPE_OPAQUE) {
    int length;
    status = TreePutDsc(vars->tinfo, *(int*)vars->nid_ptr, (struct descriptor *)initialValue, &vars->sinfo->data_offset, &length, vars->compress);
    vars->shead.data_offset = vars->sinfo->data_offset;
    vars->add_length = length;
    vars->sinfo->rows = length | 0x80000000;
  } else
    status = PutInitialValue(vars->tinfo, vars->shead.dims, initialValue, &vars->shead.data_offset);
  if (initialValue->dtype != DTYPE_OPAQUE) {
    vars->sinfo->data_offset = vars->shead.data_offset;
    vars->sinfo->rows = vars->shead.dims[vars->shead.dimct - 1];
  }
  return status;
}

inline static int put_limit_by_dsc(vars_t*vars,struct descriptor *limit_in, int64_t* limit_out, int64_t* offset, int* length) {
  struct descriptor *dsc;
  for (dsc = limit_in; dsc && dsc->pointer && dsc->dtype == DTYPE_DSC;
    dsc = (struct descriptor *)dsc->pointer);
  if (dsc && dsc->pointer && (dsc->dtype == DTYPE_Q || dsc->dtype == DTYPE_QU)) {
    *limit_out = *(int64_t *) dsc->pointer;
    *offset = -1;
    *length = 0;
    return TreeSUCCESS;
  }
  if (limit_in) {
    *limit_out = -1;
    return TreePutDsc(vars->tinfo, *(int*)vars->nid_ptr, limit_in, offset, length, vars->compress);
  }
  return TreeSUCCESS;
}

inline static int putdim_dim(vars_t*vars,struct descriptor *start, struct descriptor *end, struct descriptor *dimension) {
  INIT_TREESUCCESS;
  RETURN_IF_NOT_OK(put_limit_by_dsc(vars,start,&vars->sinfo->start,&vars->sinfo->start_offset,&vars->sinfo->start_length));
  RETURN_IF_NOT_OK(put_limit_by_dsc(vars,end  ,&vars->sinfo->end  ,&vars->sinfo->end_offset  ,&vars->sinfo->end_length  ));
  if (dimension)
    status = TreePutDsc(vars->tinfo, *(int*)vars->nid_ptr, dimension, &vars->sinfo->dimension_offset, &vars->sinfo->dimension_length, vars->compress);
  return status;
}

inline static int putdim_ts(vars_t* vars, int64_t * timestamps,struct descriptor_a *initialValue) {
  int status = PutDimensionValue(vars->tinfo, timestamps, vars->rows_filled, initialValue->dimct, vars->shead.dims, &vars->shead.dim_offset);
  vars->sinfo->start = vars->rows_filled > 0 ? timestamps[0] : 0;
  vars->sinfo->end   = vars->rows_filled > 0 ? timestamps[vars->rows_filled - 1] : 0;
  vars->sinfo->dimension_offset = vars->shead.dim_offset;
  vars->sinfo->dimension_length = 0;
  return status;
}

inline static int check_compress_dim(vars_t* vars,
          struct descriptor_xd *xd_data_ptr,struct descriptor_xd *xd_dim_ptr __attribute__((unused)),struct descriptor_a *initialValue __attribute__((unused))) {
  int length = 0;
  vars->shead.data_offset = vars->sinfo->data_offset;
  int status = TreePutDsc(vars->tinfo, *(int*)vars->nid_ptr, xd_data_ptr->pointer, &vars->sinfo->data_offset, &length, vars->compress);
  /*** flag compressed segment by setting high bit in the rows field. ***/
  vars->sinfo->rows = length | 0x80000000;
  return status;
}

inline static int check_compress_ts(vars_t*vars, struct descriptor_xd *xd_data_ptr,struct descriptor_xd *xd_dim_ptr,struct descriptor_a *initialValue) {
  int length;
  A_COEFF_TYPE *data_a = (A_COEFF_TYPE *) xd_data_ptr->pointer;
  A_COEFF_TYPE *dim_a = (A_COEFF_TYPE *) xd_dim_ptr->pointer;
  int rows = (initialValue->dimct == 1) ?
             initialValue->arsize / initialValue->length :
             ((A_COEFF_TYPE *) initialValue)->m[initialValue->dimct - 1];
  if (data_a && data_a->class == CLASS_A && data_a->pointer
      && data_a->arsize >= initialValue->arsize && dim_a && dim_a->class == CLASS_A
      && dim_a->pointer && dim_a->arsize >= (rows * sizeof(int64_t)) && dim_a->dimct == 1
      && dim_a->length == sizeof(int64_t) && dim_a->dtype == DTYPE_Q) {
    vars->shead.data_offset = vars->sinfo->data_offset;
    vars->shead.dim_offset = vars->sinfo->dimension_offset;
    int status = TreePutDsc(vars->tinfo, *(int*)vars->nid_ptr, xd_data_ptr->pointer, &vars->sinfo->data_offset, &length, vars->compress);
    int statdim= TreePutDsc(vars->tinfo, *(int*)vars->nid_ptr, xd_dim_ptr->pointer, &vars->sinfo->dimension_offset,&vars->sinfo->dimension_length, vars->compress);
    vars->sinfo->start = ((int64_t *) dim_a->pointer)[0];
    vars->sinfo->end = ((int64_t *) dim_a->pointer)[(dim_a->arsize / dim_a->length) - 1];
     /*** flag compressed segment by setting high bit in the rows field. ***/
    vars->sinfo->rows = length | 0x80000000;
    return STATUS_OK ? statdim : status;
  } else {
    if (!data_a)
      printf("data_a null\n");
    else if (data_a->class != CLASS_A)
      printf("data_a is not CLASS_A, class=%d\n", data_a->class);
    else if (!data_a->pointer)
      printf("data_a's pointer is null\n");
    else if (data_a->arsize < initialValue->arsize)
      printf("data_a->arsize (%d) < initialValue->arsize (%d)\n", data_a->arsize,
           initialValue->arsize);
    else if (!dim_a)
      printf("dim_a null\n");
    else if (dim_a->class != CLASS_A)
      printf("dim_a is not CLASS_A, class=%d\n", dim_a->class);
    else if (!dim_a->pointer)
      printf("dim_a's pointer is null\n");
    else if (dim_a->arsize < (rows * sizeof(int64_t)))
      printf("dim_a->arsize (%d) < (rows (%d) * sizeof(int64_t))", dim_a->arsize, rows);
    else if (dim_a->dimct != 1)
      printf("dim_a->dimct (%d) != 1\n", dim_a->dimct);
    else if (dim_a->length != sizeof(int64_t))
      printf("dim_a->length (%d) != sizeof(int64_t)\n", dim_a->length);
    else if (dim_a->dtype != DTYPE_Q)
      printf("dim_a->dtype (%d) != DTYPE_Q\n", dim_a->dtype);
    return TreeFAILURE;
  }
}

inline static void set_compress(vars_t* vars){
vars->compress =(vars->local_nci.flags & NciM_COMPRESS_ON_PUT)
             && (vars->local_nci.flags & NciM_COMPRESS_SEGMENTS)
             &&!(vars->local_nci.flags & NciM_DO_NOT_COMPRESS);
}

inline static int check_input(struct descriptor_a **data_p){
  while (*data_p && (*data_p)->dtype == DTYPE_DSC)
    *data_p = (struct descriptor_a *)(*data_p)->pointer;
  if (*data_p == NULL
   || !(((*data_p)->class == CLASS_R && (*data_p)->dtype == DTYPE_OPAQUE)
     || ((*data_p)->class == CLASS_A && ((*data_p)->dimct > 0 || (*data_p)->dimct <= 8))) )
    return TreeINVDTPUSG;
  return TreeSUCCESS;
}

inline static int open_datafile_read(vars_t* vars) {
  if (!(IS_OPEN(vars->dblist))) return TreeNOT_OPEN;
  int status;
  RETURN_IF_NOT_OK(load_node_ptr(vars));
  RETURN_IF_NOT_OK(check_segment_remote(vars));
  RETURN_IF_NOT_OK(load_info_ptr(vars));
  TreeGetViewDate(&vars->saved_viewdate);
  RETURN_IF_NOT_OK(TreeGetNciW(vars->tinfo, vars->nidx, &vars->local_nci, 0));
  if (vars->tinfo->data_file == 0)
    status = TreeOpenDatafileR(vars->tinfo);
  return status;
}

inline static int open_header_read(vars_t*vars){
  int status;
  RETURN_IF_NOT_OK(open_datafile_read(vars));
  IF_NO_EXTENDED_NCI   return TreeNOSEGMENTS;
  IF_NO_SEGMENT_HEADER return TreeNOSEGMENTS;
  return status;
}

inline static int open_index_read(vars_t*vars){
  int status;
  RETURN_IF_NOT_OK(open_header_read(vars));
  IF_NO_SEGMENT_INDEX return TreeFAILURE;
  return status;
}

inline static int ReadProperty(TREE_INFO *tinfo, const int64_t offset, char *buffer, const int length){
  INIT_TREESUCCESS;
  int deleted = B_TRUE;
  while STATUS_OK {
    status = (MDS_IO_READ_X(tinfo->data_file->get,offset,buffer,length, &deleted) == length) ? TreeSUCCESS : TreeFAILURE;
    if (STATUS_OK && deleted)
      status = TreeReopenDatafile(tinfo);
    else break;
  }
  return status;
}

inline static int ReadProperty_safe(TREE_INFO *tinfo, const int64_t offset,char *buffer,const int length) {
  if (offset > -1) return ReadProperty(tinfo,offset,buffer,length);
  return TreeFAILURE;
}

#define GOTO_END_ON_ERROR(statement) do{if IS_NOT_OK((status = statement)) goto end;}while(0)

#define CLEANUP_NCI_PUSH pthread_cleanup_push(unlock_nci,(void*)vars)
#define CLEANUP_NCI_POP  pthread_cleanup_pop(vars->nci_locked)
//#define CLEANUP_NCI_PUSH
//#define CLEANUP_NCI_POP  unlock_nci(vars)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclobbered"

int _TreeMakeSegment(void *dbid, int nid,
        struct descriptor *start, struct descriptor *end, struct descriptor *dimension,
        struct descriptor_a *initValIn, int idx, int rows_filled){
  INIT_WRITE_VARS;
  pthread_cleanup_push(unlock_nci,(void*)vars);
  struct descriptor_a* initialValue = initValIn;
  GOTO_END_ON_ERROR(open_datafile_write0(vars));
  GOTO_END_ON_ERROR(check_input(&initialValue));
  GOTO_END_ON_ERROR(open_datafile_write1(vars));
  set_compress(vars);
  begin_local_nci(vars, initialValue);
  begin_extended_nci(vars);
  GOTO_END_ON_ERROR(begin_segment_header(vars,initialValue));
  begin_segment_index(vars);
  GOTO_END_ON_ERROR(begin_sinfo(vars,initialValue,check_compress_dim));
  GOTO_END_ON_ERROR(putdata_initialvalue(vars,initialValue));
  GOTO_END_ON_ERROR(putdim_dim(vars,start,end,dimension));
  status = begin_finish(vars);
end: ;
  pthread_cleanup_pop(vars->nci_locked);
  return status;
}

int _TreeMakeTimestampedSegment(void *dbid, int nid,
        int64_t * timestamps,
        struct descriptor_a *initValIn, int idx, int rows_filled){
  INIT_WRITE_VARS;
  CLEANUP_NCI_PUSH;
  struct descriptor_a* initialValue = initValIn;
  GOTO_END_ON_ERROR(open_datafile_write0(vars));
  GOTO_END_ON_ERROR(check_input(&initialValue));
  GOTO_END_ON_ERROR(open_datafile_write1(vars));
  set_compress(vars);
  begin_local_nci(vars, initialValue);
  begin_extended_nci(vars);
  GOTO_END_ON_ERROR(begin_segment_header(vars,initialValue));
  begin_segment_index(vars);
  GOTO_END_ON_ERROR(begin_sinfo(vars,initialValue,check_compress_ts));
  GOTO_END_ON_ERROR(putdata_initialvalue(vars,initialValue));
  GOTO_END_ON_ERROR(putdim_ts(vars,timestamps,initialValue));
  status = begin_finish(vars);
end: ;
  CLEANUP_NCI_POP;
  unlock_nci(vars);
  return status;
}

int _TreeUpdateSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end,
                       struct descriptor *dimension, int idx)
{
  INIT_VARS;vars->idx=idx;
  CLEANUP_NCI_PUSH;
  GOTO_END_ON_ERROR(open_datafile_write0(vars));
  GOTO_END_ON_ERROR(open_datafile_write1(vars));
  set_compress(vars);
  IF_NO_EXTENDED_NCI   {status = TreeNOSEGMENTS;goto end;}
  IF_NO_SEGMENT_HEADER {status = TreeNOSEGMENTS;goto end;}
  IF_NO_SEGMENT_INDEX  {status = TreeFAILURE;goto end;}
  GOTO_END_ON_ERROR(check_sinfo(vars));
  GOTO_END_ON_ERROR(putdim_dim(vars,start,end,dimension));
  status = PutSegmentIndex(vars->tinfo, &vars->sindex, &vars->index_offset);
end: ;
  CLEANUP_NCI_POP;
  return status;
}

int _TreePutSegment(void *dbid, int nid, const int startIdx, struct descriptor_a *data) {
  A_COEFF_TYPE *a_coeff = (A_COEFF_TYPE *) data;
  INIT_VARS;
  CLEANUP_NCI_PUSH;
  DESCRIPTOR_A(data_a, 0, 0, 0, 0);/*CHECK_DATA*/{
    while (data && data->dtype == DTYPE_DSC)
      data = (struct descriptor_a *)data->pointer;
    if (data->class == CLASS_S) {
      data_a.pointer = data->pointer;
      data_a.length = data->length;
      data_a.class = CLASS_A;
      data_a.dtype = data->dtype;
      data_a.arsize = data->length;
      data_a.dimct = 1;
      data = (struct descriptor_a *)&data_a;
    }
    if (data == NULL || data->class != CLASS_A || data->dimct < 1 || data->dimct > 8)
      {status = TreeINVDTPUSG; goto end;}
  }
  GOTO_END_ON_ERROR(open_datafile_write0(vars));
  GOTO_END_ON_ERROR(open_datafile_write1(vars));
  IF_NO_EXTENDED_NCI   {status = TreeNOSEGMENTS;goto end;}
  IF_NO_SEGMENT_HEADER {status = TreeNOSEGMENTS;goto end;}
  /*CHECK_DATA_DIMCT*/{
    if (data->dtype != vars->shead.dtype
    || (data->dimct != vars->shead.dimct && data->dimct != vars->shead.dimct - 1)
    || (data->dimct > 1 && memcmp(vars->shead.dims, a_coeff->m, (data->dimct - 1) * sizeof(int))))
      {status = TreeFAILURE;goto end;}
  }
  int start_idx;
  /*CHECK_STARTIDX*/{
    if (startIdx == -1)
      start_idx = vars->shead.next_row;
    else if (startIdx < -1 || startIdx > vars->shead.dims[vars->shead.dimct - 1])
      {status = TreeBUFFEROVF;goto end;}
    else start_idx = startIdx;
  }
  int rows_to_insert;
  int bytes_per_row,i;
  for (bytes_per_row = vars->shead.length, i = 0; i < vars->shead.dimct - 1;
       bytes_per_row *= vars->shead.dims[i], i++) ;
  if (data->dimct < vars->shead.dimct) {
    rows_to_insert = 1;
  } else {
    rows_to_insert = (data->dimct == 1) ? data->arsize / data->length : a_coeff->m[a_coeff->dimct - 1];
  }
  int rows_in_segment = vars->shead.dims[vars->shead.dimct - 1];
  unsigned int bytes_to_insert = ((rows_to_insert > (rows_in_segment - start_idx)) ? (rows_in_segment - start_idx) : rows_to_insert) * bytes_per_row;
  if (bytes_to_insert < data->arsize)
    {status = TreeBUFFEROVF;goto end;}
  int64_t offset = vars->shead.data_offset + start_idx * bytes_per_row;
  /*PUTSEG_PUTDATA*/{
    ALLOCATE_BUFFER(bytes_to_insert,buffer)
    CHECK_ENDIAN_TRANSFER(data->pointer,bytes_to_insert,vars->shead.length,data->dtype,buffer);
    TreeLockDatafile(vars->tinfo, 0, offset);
    MDS_IO_LSEEK(vars->tinfo->data_file->put, offset, SEEK_SET);
    status = (MDS_IO_WRITE(vars->tinfo->data_file->put, buffer, bytes_to_insert) == (ssize_t)bytes_to_insert) ? TreeSUCCESS : TreeFAILURE;
    FREE_BUFFER(buffer);
    TreeUnLockDatafile(vars->tinfo, 0, offset);
  }
  if (start_idx == vars->shead.next_row)
    vars->shead.next_row += bytes_to_insert / bytes_per_row;
  if STATUS_OK
    status = PutSegmentHeader(vars->tinfo, &vars->shead, &vars->attr.facility_offset[SEGMENTED_RECORD_FACILITY]);
end: ;
  CLEANUP_NCI_POP;
  return status;
}

int _TreePutTimestampedSegment(void *dbid, int nid, int64_t * timestamp, struct descriptor_a *data_in){
  INIT_VARS;
  CLEANUP_NCI_PUSH;
  struct descriptor_a *data = data_in;
  GOTO_END_ON_ERROR(open_datafile_write0(vars));
  DESCRIPTOR_A(data_a, 0, 0, 0, 0);
  while (data && data->dtype == DTYPE_DSC)
    data = (struct descriptor_a *)data->pointer;
  if (data && data->class == CLASS_S) {
    data_a.pointer = data->pointer;
    data_a.length = data->length;
    data_a.class = CLASS_A;
    data_a.dtype = data->dtype;
    data_a.arsize = data->length;
    data_a.dimct = 1;
    data = (struct descriptor_a *)&data_a;
  }
  A_COEFF_TYPE *a_coeff = (A_COEFF_TYPE *) data;
  if (data == NULL || data->class != CLASS_A || data->dimct < 1 || data->dimct > 8)
    {status = TreeINVDTYPE; goto end;}
  GOTO_END_ON_ERROR(open_datafile_write1(vars));
  int start_idx;
  int rows_to_insert = -1;
  int bytes_per_row;
  int rows_in_segment;
  int bytes_to_insert;
  int64_t offset;
  int i;
  IF_NO_EXTENDED_NCI   {status = TreeNOSEGMENTS;goto end;};
  IF_NO_SEGMENT_HEADER {status = TreeNOSEGMENTS;goto end;};
  if (data->dtype != vars->shead.dtype)
    {status = TreeINVDTYPE; goto end;}
  if ((a_coeff->dimct == 1)
   &&!(a_coeff->dimct == vars->shead.dimct)
   &&!(a_coeff->dimct == vars->shead.dimct - 1))
    {status = TreeINVSHAPE; goto end;}
  if (a_coeff->dimct > 1 && memcmp(vars->shead.dims, a_coeff->m,(vars->shead.dimct - 1) * sizeof(int)))
    {status = TreeINVSHAPE; goto end;}
  if (a_coeff->dimct == 1 && a_coeff->arsize / a_coeff->length != 1
   && (unsigned int)vars->shead.dims[0] < a_coeff->arsize / a_coeff->length)
    {status = TreeINVSHAPE; goto end;}
  start_idx = vars->shead.next_row;
  for (bytes_per_row = vars->shead.length, i = 0; i < vars->shead.dimct - 1;
       bytes_per_row *= vars->shead.dims[i], i++) ;
  if (data->dimct < vars->shead.dimct)
    rows_to_insert = 1;
  else
    rows_to_insert = (data->dimct == 1) ? data->arsize / data->length : a_coeff->m[a_coeff->dimct - 1];
  if (rows_to_insert <= 0)
    {status = TreeINVSHAPE; goto end;}
  rows_in_segment = vars->shead.dims[vars->shead.dimct - 1];
  bytes_to_insert = ((rows_to_insert > (rows_in_segment - start_idx)) ? (rows_in_segment - start_idx) : rows_to_insert) * bytes_per_row;
  if (bytes_to_insert < 1)
    {status = TreeBUFFEROVF; goto end;}
  offset = vars->shead.data_offset + start_idx * bytes_per_row;
  ALLOCATE_BUFFER(bytes_to_insert,buffer);
  CHECK_ENDIAN_TRANSFER(data->pointer,bytes_to_insert,vars->shead.length,data->dtype,buffer);
  TreeLockDatafile(vars->tinfo, 0, offset);
  MDS_IO_LSEEK(vars->tinfo->data_file->put, offset, SEEK_SET);
  status =(MDS_IO_WRITE(vars->tinfo->data_file->put, buffer, bytes_to_insert) == bytes_to_insert) ? TreeSUCCESS : TreeFAILURE;
  FREE_BUFFER(buffer);
  MDS_IO_LSEEK(vars->tinfo->data_file->put, vars->shead.dim_offset + start_idx * sizeof(int64_t), SEEK_SET);
  ALLOCATE_BUFFER(rows_to_insert,times);
  CHECK_ENDIAN_TRANSFER(timestamp,rows_to_insert,8,,times);
  status = (MDS_IO_WRITE(vars->tinfo->data_file->put, times, sizeof(int64_t) * rows_to_insert) == (int)(sizeof(int64_t) * rows_to_insert)) ? TreeSUCCESS : TreeFAILURE;
  FREE_BUFFER(times);
  TreeUnLockDatafile(vars->tinfo, 0, offset);
  vars->shead.next_row = start_idx + bytes_to_insert / bytes_per_row;
  if STATUS_OK
    status = PutSegmentHeader(vars->tinfo, &vars->shead, &vars->attr.facility_offset[SEGMENTED_RECORD_FACILITY]);
end: ;
  CLEANUP_NCI_POP;
  return status;
}

#pragma GCC diagnostic pop

///// GET SEGMENT TIMES /////

inline static void getlimit_array(vars_t* vars, int64_t* limitval, const int64_t sinfo_limit, const int64_t sinfo_limit_offset, const int sinfo_limit_length) {
  if (sinfo_limit != -1)
    *limitval = sinfo_limit;
  else if (sinfo_limit_offset > 0 && sinfo_limit_length > 0) {
    EMPTYXD(xd);
    if (TreeGetDsc(vars->tinfo, *(int*)vars->nid_ptr, sinfo_limit_offset, sinfo_limit_length, &xd)&1 && xd.pointer && xd.pointer->length == 8)
      *limitval = *(int64_t *) xd.pointer->pointer;
    else
      *limitval = 0;
    MdsFree1Dx(&xd, 0);
  } else
    *limitval = 0;
}

inline static void getlimit_xd(vars_t* vars, int64_t* limitval,const int64_t sinfo_limit, const int64_t sinfo_limit_offset, const int sinfo_limit_length,
	struct descriptor_xd* limit_xd,struct descriptor* limitdsc) {
  if (sinfo_limit != -1) {
    *limitval = sinfo_limit;
    MdsCopyDxXd(limitdsc,limit_xd);
  }
  if (sinfo_limit_offset > 0 && sinfo_limit_length > 0)
    TreeGetDsc(vars->tinfo, *(int*)vars->nid_ptr, sinfo_limit_offset, sinfo_limit_length, limit_xd);
}

inline static int get_filled_rows_ts(SEGMENT_HEADER* shead,SEGMENT_INFO* sinfo, const int idx, const int64_t *buffer){
  if (shead->idx==idx)
    return shead->next_row<0 ? sinfo->rows : shead->next_row;
  else {
    /* Removes trailing null timestamps from dimension and data
     * if at least more than one zero timestamps
     * or last timestamp does not continue monotonic increase of time vector
     */
    int filled_rows = sinfo->rows;
    if (filled_rows>1 && swapquad(&buffer[filled_rows-2])>=0)// if second last is not <0
      for ( ; filled_rows>0 && 0==buffer[filled_rows-1] ; filled_rows--);
    return filled_rows;
  }
}

inline static int get_segment_times_loop(vars_t* vars, int64_t* startval, int64_t* endval,
	struct descriptor_xd* start_xd,struct descriptor* startdsc,
	struct descriptor_xd* end_xd,  struct descriptor* enddsc) {
  INIT_TREESUCCESS;
  int index_idx = vars->idx % SEGMENTS_PER_INDEX;
  vars->sinfo = &vars->sindex.segment[index_idx];
  if (vars->sinfo->dimension_offset != -1 && vars->sinfo->dimension_length == 0) {
    /* It's a timestamped segment */
    if (vars->sinfo->rows < 0 || !(vars->sinfo->start == 0 && vars->sinfo->end == 0)) {
      /* Valid start and end in segment tinfo */
      *startval = vars->sinfo->start;
      *endval = vars->sinfo->end;
    } else {
      /* Current segment so use timestamps in segment */
      int length = sizeof(int64_t) * vars->sinfo->rows;
      char *buffer = NULL;
      FREE_ON_EXIT(buffer);
      buffer = malloc(length);
      status = ReadProperty(vars->tinfo,vars->sinfo->dimension_offset, buffer, length);
      if STATUS_OK {
        *startval = swapquad(buffer);
        int filled_rows = get_filled_rows_ts(&vars->shead,vars->sinfo,vars->idx,(int64_t*)buffer);
        *endval = filled_rows ? swapquad(buffer+(filled_rows-1)*sizeof(int64_t)) : 0;
      } else {
        *startval = 0;
        *endval = 0;
      }
      FREE_NOW(buffer);
    }
    if (start_xd) {
      MdsCopyDxXd(startdsc,start_xd);
      MdsCopyDxXd(enddsc,end_xd);
    }
  } else if (start_xd) {
    getlimit_xd(vars,startval,vars->sinfo->start,vars->sinfo->start_offset,vars->sinfo->start_length, start_xd, startdsc);
    getlimit_xd(vars,endval  ,vars->sinfo->end,  vars->sinfo->end_offset,  vars->sinfo->end_length,   end_xd  , enddsc  );
  } else {
    getlimit_array(vars,startval,vars->sinfo->start,vars->sinfo->start_offset,vars->sinfo->start_length);
    getlimit_array(vars,endval  ,vars->sinfo->end,  vars->sinfo->end_offset,  vars->sinfo->end_length);
  }
  if (index_idx == 0 && vars->idx > 0)
    return GetSegmentIndex(vars->tinfo, vars->sindex.previous_offset, &vars->sindex);
  return TreeSUCCESS;
}

int _TreeGetSegmentTimesXd(void *dbid, int nid, int *nsegs, struct descriptor_xd *start_list, struct descriptor_xd *end_list){
  INIT_VARS;
  RETURN_IF_NOT_OK(open_index_read(vars));
  int numsegs = vars->shead.idx + 1;
  *nsegs = numsegs;
  DESCRIPTOR_APD(start_apd, DTYPE_LIST, malloc(numsegs * sizeof(void *)), numsegs);
  DESCRIPTOR_APD(end_apd,   DTYPE_LIST, malloc(numsegs * sizeof(void *)), numsegs);
  FREE_ON_EXIT(start_apd.pointer);
  FREE_ON_EXIT(end_apd.pointer);
  status = TreeSUCCESS;
  struct descriptor_xd **start_xds = (struct descriptor_xd **)start_apd.pointer;
  struct descriptor_xd **end_xds = (struct descriptor_xd **)end_apd.pointer;
  EMPTYXD(xd);
  int64_t startval;
  int64_t endval;
  struct descriptor startdsc = {sizeof(int64_t), DTYPE_Q, CLASS_S, (char *)&startval};
  struct descriptor enddsc = {sizeof(int64_t), DTYPE_Q, CLASS_S, (char *)&endval};
  for (vars->idx=numsegs ; STATUS_OK && vars->idx-->0 ;) {
    start_xds[vars->idx]= memcpy(malloc(sizeof(struct descriptor_xd)),&xd, sizeof(struct descriptor_xd));
    end_xds[vars->idx]  = memcpy(malloc(sizeof(struct descriptor_xd)),&xd, sizeof(struct descriptor_xd));
    struct descriptor_xd *start_xd = ((struct descriptor_xd **)start_apd.pointer)[vars->idx];
    struct descriptor_xd *end_xd   = ((struct descriptor_xd **)end_apd.pointer)[vars->idx];
    get_segment_times_loop(vars,&startval,&endval,start_xd,&startdsc,end_xd,&enddsc);
  }
  MdsCopyDxXd((struct descriptor *)&start_apd,start_list);
  MdsCopyDxXd((struct descriptor *)&end_apd,end_list);
  for (vars->idx=0; vars->idx<numsegs ; vars->idx++) {
    MdsFree1Dx(start_xds[vars->idx],0);
    free(start_xds[vars->idx]);
    MdsFree1Dx(end_xds[vars->idx],0);
    free(end_xds[vars->idx]);
  }
  FREE_NOW(end_apd.pointer);
  FREE_NOW(start_apd.pointer);
  return status;
}



int _TreeGetSegmentTimes(void *dbid, int nid, int *nsegs, int64_t ** times){
  *times = NULL;
  INIT_VARS;
  RETURN_IF_NOT_OK(open_index_read(vars));
  int numsegs = vars->shead.idx + 1;
  *nsegs = numsegs;
  int64_t *ans = (int64_t *) malloc(numsegs * 2 * sizeof(int64_t));
  *times = ans;
  memset(ans, 0, numsegs * 2 * sizeof(int64_t));
  for (vars->idx=numsegs; STATUS_OK && vars->idx-->0;)
    get_segment_times_loop(vars,&ans[vars->idx * 2],&ans[vars->idx * 2 + 1],0,0,0,0);
  return status;
}

int _TreeGetNumSegments(void *dbid, int nid, int *num){
  *num = 0;
  INIT_VARS;
  RETURN_IF_NOT_OK(open_datafile_read(vars));
  IF_NO_EXTENDED_NCI   return status; //return num=0
  IF_NO_SEGMENT_HEADER return status; //return num=0
  *num = _vars.shead.idx + 1;
  return status;
}

static int ReadSegment(TREE_INFO* tinfo, int nid, SEGMENT_HEADER* shead, SEGMENT_INFO* sinfo,
                       int idx, struct descriptor_xd *segment, struct descriptor_xd *dim){
  INIT_TREESUCCESS;
  if (sinfo->data_offset != -1) {
    int i,compressed_segment = sinfo->rows < 0;
    DESCRIPTOR_A(dim2, 8, DTYPE_Q, 0, 0);
    DESCRIPTOR_A_COEFF(ans, 0, 0, 0, 8, 0);
    void *ans_ptr = NULL, *dim_ptr;
    if (segment){
      ans.pointer = 0;
      ans.dtype = shead->dtype;
      ans.length = shead->length;
      ans.dimct = shead->dimct;
      memcpy(ans.m, shead->dims, sizeof(shead->dims));
      ans.m[shead->dimct - 1] = sinfo->rows;
      ans.arsize = ans.length;
      for (i = 0; i < ans.dimct; i++)
        ans.arsize *= ans.m[i];
      if (compressed_segment) {
        EMPTYXD(compressed_segment_xd);
        int data_length = sinfo->rows & 0x7fffffff;
        status = TreeGetDsc(tinfo, nid, sinfo->data_offset, data_length, &compressed_segment_xd);
        if STATUS_OK {
          status = MdsDecompress((struct descriptor_r *)compressed_segment_xd.pointer, segment);
          MdsFree1Dx(&compressed_segment_xd, 0);
        }
      } else {
        ans_ptr = ans.pointer = malloc(ans.arsize);
        status = ReadProperty(tinfo,sinfo->data_offset, ans.pointer, (ssize_t)ans.arsize);
      }
      if (STATUS_OK && !compressed_segment)
        CHECK_ENDIAN(ans.pointer,ans.arsize,ans.length,ans.dtype);
    }
    if STATUS_OK {
      if (sinfo->dimension_offset != -1 && sinfo->dimension_length == 0) {
        dim2.arsize = sinfo->rows * sizeof(int64_t);
        dim_ptr = dim2.pointer = malloc(dim2.arsize);
        status = ReadProperty(tinfo,sinfo->dimension_offset, dim2.pointer, (ssize_t)dim2.arsize);
        CHECK_ENDIAN(dim2.pointer,dim2.arsize,8,);
        if (!compressed_segment) {
          int filled_rows = get_filled_rows_ts(shead,sinfo,idx,(int64_t*)dim2.pointer);
          dim2.arsize = filled_rows * sizeof(int64_t);
          if (segment) {
            ans.m[shead->dimct - 1] = filled_rows;
            ans.arsize = ans.length;
            for (i = 0; i < ans.dimct; i++)
              ans.arsize *= ans.m[i];
          }
        }
        if (dim2.arsize==0){
          if (segment) ans.pointer = NULL;
          dim2.pointer = NULL;
        }
        MdsCopyDxXd((struct descriptor *)&dim2, dim);
        free(dim_ptr);
      } else {
	if (sinfo->dimension_length != -1) {
          TreeGetDsc(tinfo, nid, sinfo->dimension_offset, sinfo->dimension_length, dim);
	}
      }
      if (ans_ptr)
        MdsCopyDxXd((struct descriptor *)&ans, segment);
    } else {
      status = TreeFAILURE;
    }
    if (ans_ptr)
      free(ans_ptr);
  } else {
    status = TreeFAILURE;
  }
  return status;
}

static int getSegmentLimits(vars_t* vars,
                            struct descriptor_xd *retStart, struct descriptor_xd *retEnd){
  INIT_TREESUCCESS;
  struct descriptor q_d = { 8, DTYPE_Q, CLASS_S, 0 };
  if (vars->sinfo->dimension_offset != -1 && vars->sinfo->dimension_length == 0) {
    /*** timestamped segments ****/
    if (vars->sinfo->rows < 0 || !(vars->sinfo->start == 0 && vars->sinfo->end == 0)) {
      q_d.pointer = (char *)&vars->sinfo->start;
      MdsCopyDxXd(&q_d, retStart);
      q_d.pointer = (char *)&vars->sinfo->end;
      MdsCopyDxXd(&q_d, retEnd);
    } else {
      int length = sizeof(int64_t) * vars->sinfo->rows;
      char *buffer = NULL;
      FREE_ON_EXIT(buffer);
      buffer = malloc(length);
      int64_t timestamp;
      status = ReadProperty(vars->tinfo,vars->sinfo->dimension_offset, buffer, length);
      if STATUS_OK {
        q_d.pointer = (char *)&timestamp;
        timestamp = swapquad(buffer);
        MdsCopyDxXd(&q_d, retStart);
        int filled_rows = get_filled_rows_ts(&vars->shead,vars->sinfo,vars->idx,(int64_t*)buffer);
        if (filled_rows > 0) {
          timestamp = swapquad(buffer + (filled_rows-1) * sizeof(int64_t));
          MdsCopyDxXd(&q_d, retEnd);
        } else {
          MdsFree1Dx(retEnd, 0);
        }
      } else {
        MdsFree1Dx(retStart, 0);
        MdsFree1Dx(retEnd, 0);
      }
      FREE_NOW(buffer);
    }
  } else {
    if (vars->sinfo->start != -1) {
      q_d.pointer = (char *)&vars->sinfo->start;
      MdsCopyDxXd(&q_d, retStart);
    } else if (vars->sinfo->start_length > 0 && vars->sinfo->start_offset > 0) {
      status = TreeGetDsc(vars->tinfo, *(int*)vars->nid_ptr, vars->sinfo->start_offset, vars->sinfo->start_length, retStart);
    } else
      status = MdsFree1Dx(retStart, 0);
    if (vars->sinfo->end != -1) {
      q_d.pointer = (char *)&vars->sinfo->end;
      MdsCopyDxXd(&q_d, retEnd);
    } else if (vars->sinfo->end_length > 0 && vars->sinfo->end_offset > 0) {
      status = TreeGetDsc(vars->tinfo, *(int*)vars->nid_ptr, vars->sinfo->end_offset, vars->sinfo->end_length, retEnd);
    } else
      status = MdsFree1Dx(retEnd, 0);
  }
  return status;
}

int _TreeSetXNci(void *dbid, int nid, const char *xnciname, struct descriptor *value){
  if (!xnciname || strlen(xnciname) < 1 || strlen(xnciname) > NAMED_ATTRIBUTE_NAME_SIZE)
    return TreeFAILURE;
  INIT_VARS;
  CLEANUP_NCI_PUSH;
  GOTO_END_ON_ERROR(open_datafile_write0(vars));
  GOTO_END_ON_ERROR(open_datafile_write1(vars));
  set_compress(vars);
  vars->index_offset = -1;
  int value_length = 0;
  int64_t value_offset = -1;
  NAMED_ATTRIBUTES_INDEX index, current_index;
  if (value) // NULL means delete
    GOTO_END_ON_ERROR(TreePutDsc(vars->tinfo, *(int*)vars->nid_ptr, value, &value_offset, &value_length, vars->compress));
  /*** See if node is currently using the Extended Nci feature and if so get the current contents of the attr
       index. If not, make an empty index and flag that a new index needs to be written.***/
  IF_NO_EXTENDED_NCI {
    if (!value) goto end; // has not xnci; nothing to delete
    vars->attr_offset=-1;
    memset(&vars->attr, -1, sizeof(vars->attr));
    vars->attr_update = 1;
    if (((vars->local_nci.flags2 & NciM_EXTENDED_NCI) == 0) && vars->local_nci.length > 0) {
      if (vars->local_nci.flags2 & NciM_DATA_IN_ATT_BLOCK) {
        EMPTYXD(dsc);
        struct descriptor *dptr;
        unsigned char dsc_dtype = DTYPE_DSC;
        int dlen = vars->local_nci.length - 8;
        unsigned int ddlen = dlen + sizeof(struct descriptor);
        status = MdsGet1Dx(&ddlen, &dsc_dtype, &dsc, 0);
        dptr = dsc.pointer;
        dptr->length = dlen;
        dptr->dtype = vars->local_nci.dtype;
        dptr->class = CLASS_S;
        dptr->pointer = (char *)dptr + sizeof(struct descriptor);
        memcpy(dptr->pointer, vars->local_nci.DATA_INFO.DATA_IN_RECORD.data, dptr->length);
        if (dptr->dtype != DTYPE_T) {
          switch (dptr->length) {
          case 2:
            *(short *)dptr->pointer = swapshort(dptr->pointer);
            break;
          case 4:
            *(int *)dptr->pointer = swapint(dptr->pointer);
            break;
          case 8:
            *(int64_t *) dptr->pointer = swapquad(dptr->pointer);
            break;
          }
        }
        TreePutDsc(vars->tinfo, *(int*)vars->nid_ptr, dptr, &vars->attr.facility_offset[STANDARD_RECORD_FACILITY],
                   &vars->attr.facility_length[STANDARD_RECORD_FACILITY], vars->compress);
        vars->local_nci.flags2 &= ~NciM_DATA_IN_ATT_BLOCK;
      } else {
        EMPTYXD(xd);
        int retsize;
        int nodenum;
        int length = vars->local_nci.DATA_INFO.DATA_LOCATION.record_length;
        if (length > 0) {
          char *data = NULL;
          FREE_ON_EXIT(data);
          data = malloc(length);
          status =
              TreeGetDatafile(vars->tinfo, vars->local_nci.DATA_INFO.DATA_LOCATION.rfa, &length, data,
                              &retsize, &nodenum, vars->local_nci.flags2);
          if STATUS_NOT_OK
            status = TreeBADRECORD;
          else if (!(vars->local_nci.flags2 & NciM_NON_VMS)
                   && ((retsize != length) || (nodenum != vars->nidx)))
            status = TreeBADRECORD;
          else
            status = (MdsSerializeDscIn(data, &xd) & 1) ? TreeNORMAL : TreeBADRECORD;
          FREE_NOW(data);
          if STATUS_OK {
            status =
                TreePutDsc(vars->tinfo, *(int*)vars->nid_ptr, (struct descriptor *)&xd,
                           &vars->attr.facility_offset[STANDARD_RECORD_FACILITY],
                           &vars->attr.facility_length[STANDARD_RECORD_FACILITY], vars->compress);
          }
          MdsFree1Dx(&xd, 0);
        }
        if (length <= 0 || STATUS_NOT_OK) {
          vars->attr.facility_offset[STANDARD_RECORD_FACILITY] = 0;
          vars->attr.facility_length[STANDARD_RECORD_FACILITY] = 0;
          vars->local_nci.length = 0;
          vars->local_nci.DATA_INFO.DATA_LOCATION.record_length = 0;
        }
      }
    }
  } else
    vars->attr_offset = RfaToSeek(vars->local_nci.DATA_INFO.DATA_LOCATION.rfa);
  /* See if the node currently has an named attr header record.
   * If not, make an empty named attr header and flag that a new one needs to be written.
   */
  if (vars->attr.facility_offset[NAMED_ATTRIBUTES_FACILITY] == -1
   || IS_NOT_OK(GetNamedAttributesIndex(vars->tinfo, vars->attr.facility_offset[NAMED_ATTRIBUTES_FACILITY], &index))) {
    memset(&index, 0, sizeof(index));
    vars->attr.facility_offset[NAMED_ATTRIBUTES_FACILITY] = -1;
    vars->index_offset = -1;
    index.previous_offset = -1;
    vars->attr_update = 1;
  } else
    vars->index_offset = vars->attr.facility_offset[NAMED_ATTRIBUTES_FACILITY];
  current_index = index;
  /*** See if the node currently has a value for this attribute. ***/
  int found_index = -1;
  while (vars->index_offset != -1 && found_index == -1) {
    int i;
    size_t j;
    for (i = 0; i < NAMED_ATTRIBUTES_PER_INDEX; i++) {
      size_t len = strlen(xnciname);
      for (j = 0; j < len; j++)
        if (tolower(xnciname[j]) != tolower(index.attribute[i].name[j]))
          break;
      if (j == len && index.attribute[i].name[j] == 0)
        break;
    }
    if (i < NAMED_ATTRIBUTES_PER_INDEX)
      found_index = i;
    else if (index.previous_offset != -1) {
      int64_t new_offset = index.previous_offset;
      if ((GetNamedAttributesIndex(vars->tinfo, index.previous_offset, &index) & 1) == 0)
        break;
      vars->index_offset = new_offset;
    } else
      break;
  }
  /*** If name exists just replace the value else find an empty slot ***/
  if (found_index != -1) {
    index.attribute[found_index].offset = value_offset;
    index.attribute[found_index].length = value_length;
  } else {
    if (!value) goto end; // nothing to delete
    int i;
    index = current_index;
    for (i = 0; i < NAMED_ATTRIBUTES_PER_INDEX; i++) {
      if (index.attribute[i].name[0] == 0) {
        strcpy(index.attribute[i].name, xnciname);
        index.attribute[i].offset = value_offset;
        index.attribute[i].length = value_length;
        vars->index_offset = vars->attr.facility_offset[NAMED_ATTRIBUTES_FACILITY];
        break;
      }
    }
    if (i == NAMED_ATTRIBUTES_PER_INDEX) {
      memset(&index, 0, sizeof(index));
      index.previous_offset = vars->attr.facility_offset[NAMED_ATTRIBUTES_FACILITY];
      vars->attr_update = 1;
      strcpy(index.attribute[0].name, xnciname);
      index.attribute[0].offset = value_offset;
      index.attribute[0].length = value_length;
      vars->index_offset = -1;
    }
  }
  status = PutNamedAttributesIndex(vars->tinfo, &index, &vars->index_offset);
  if (STATUS_OK && vars->attr_update) {
    vars->attr.facility_offset[NAMED_ATTRIBUTES_FACILITY] = vars->index_offset;
    status = TreePutExtendedAttributes(vars->tinfo, &vars->attr, &vars->attr_offset);
    SeekToRfa(vars->attr_offset, vars->local_nci.DATA_INFO.DATA_LOCATION.rfa);
    vars->local_nci.flags2 |= NciM_EXTENDED_NCI;
    TreePutNci(vars->tinfo, vars->nidx, &vars->local_nci, 0);
  }
end: ;
  CLEANUP_NCI_POP;
  return status;
}

int _TreeGetXNci(void *dbid, int nid, const char *xnciname, struct descriptor_xd *value)
{
  if (!xnciname || strlen(xnciname) < 1 || strlen(xnciname) > NAMED_ATTRIBUTE_NAME_SIZE)
    return TreeFAILURE;
  INIT_VARS;
  RETURN_IF_NOT_OK(open_datafile_read(vars));
    NAMED_ATTRIBUTES_INDEX index;
    char *attnames = "attributenames";
    int getnames = 0;
    struct _namelist {
      char name[NAMED_ATTRIBUTE_NAME_SIZE + 1];
      struct _namelist *next;
    } *namelist = 0;
    size_t longestattname = 0;
    int numnames = 0;
    size_t i;
    size_t len = strlen(xnciname);
    if (len == strlen(attnames)) {
      for (i = 0; i < len; i++) {
        if (tolower(xnciname[i]) != attnames[i])
          break;
      }
      if (i == len)
        getnames = 1;
    }
    if (((vars->local_nci.flags2 & NciM_EXTENDED_NCI) == 0) ||
        ((TreeGetExtendedAttributes(vars->tinfo, RfaToSeek(vars->local_nci.DATA_INFO.DATA_LOCATION.rfa), &vars->attr) & 1) == 0)) {
      status = TreeFAILURE;
    } else if (vars->attr.facility_offset[NAMED_ATTRIBUTES_FACILITY] == -1
           ||  IS_NOT_OK(GetNamedAttributesIndex(vars->tinfo, vars->attr.facility_offset[NAMED_ATTRIBUTES_FACILITY],&index))) {
      status = TreeFAILURE;
    } else {
      int found_index = -1;
      while (found_index == -1) {
        unsigned int i, j;
        for (i = 0; i < NAMED_ATTRIBUTES_PER_INDEX; i++) {
          if (getnames == 1) {
            if (index.attribute[i].name[0] != 0) {
              size_t len = strlen(index.attribute[i].name);
              struct _namelist *p = malloc(sizeof(struct _namelist));
              memcpy(p->name, index.attribute[i].name, sizeof(p->name));
              p->next = namelist;
              namelist = p;
              longestattname = (len > longestattname) ? len : longestattname;
              numnames++;
            }
          } else {
            for (j = 0; j < len; j++) {
              if (tolower(xnciname[j]) != tolower(index.attribute[i].name[j]))
                break;
            }
            if (j == len && index.attribute[i].name[j] == 0)
              break;
          }
        }
        if (i < NAMED_ATTRIBUTES_PER_INDEX)
          found_index = i;
        else if (index.previous_offset != -1) {
          if ((GetNamedAttributesIndex(vars->tinfo, index.previous_offset, &index) & 1) == 0) {
            break;
          }
        } else
          break;
      }
      if (found_index != -1) {
        status =
            TreeGetDsc(vars->tinfo, *(int*)vars->nid_ptr, index.attribute[found_index].offset,
                       index.attribute[found_index].length, value);
      } else if (getnames == 1) {
        if (namelist == 0) {
          status = TreeFAILURE;
        } else {
          char *names = malloc(longestattname * numnames);
          DESCRIPTOR_A(name_array, (short)longestattname, DTYPE_T, names,
                       (unsigned int)(longestattname * numnames));
          struct _namelist *p, *pnext;
          char *np;
          for (p = namelist, np = names; p; p = pnext, np += longestattname) {
            size_t i;
            pnext = p->next;
            memcpy(np, p->name, longestattname);
            for (i = 1; i < longestattname; i++)
              if (np[i] == '\0')
                np[i] = ' ';
            free(p);
          }
          MdsCopyDxXd((struct descriptor *)&name_array, value);
          free(names);
        }
      } else
        status = TreeFAILURE;
    }
  return status;
}

int TreeSetRetrievalQuota(int quota __attribute__ ((unused)))
{
  printf("TreeSetRetrievalQuota is not implemented\n");
  return 0;
}

extern int TreeFixupNid();

int TreePutDsc(TREE_INFO * tinfo, int nid_in, struct descriptor *dsc, int64_t * offset, int *length, int compress)
{
  EMPTYXD(xd);
  int compressible;
  unsigned int dlen;
  unsigned int reclen;
  unsigned char dtype, class;
  int data_in_altbuf;
  NID *nid = (NID *) & nid_in;
  unsigned char tree = nid->tree;
  int status = MdsSerializeDscOutZ(dsc, &xd, TreeFixupNid, &tree, 0, 0, compress,
                                   &compressible, &dlen, &reclen, &dtype, &class, 0, 0,
                                   &data_in_altbuf);
  if (STATUS_OK && xd.pointer && xd.pointer->class == CLASS_A && xd.pointer->pointer) {
    struct descriptor_a *ap = (struct descriptor_a *)xd.pointer;
    TreeLockDatafile(tinfo, 0, 0);
    *offset = MDS_IO_LSEEK(tinfo->data_file->put, 0, SEEK_END);
    status = MDS_IO_WRITE(tinfo->data_file->put, ap->pointer, ap->arsize)
          == (ssize_t)ap->arsize ? TreeSUCCESS : TreeFAILURE;
    TreeUnLockDatafile(tinfo, 0, 0);
    *length = ap->arsize;
    MdsFree1Dx(&xd, 0);
  }
  return status;
}

int TreeGetDsc(TREE_INFO * tinfo, const int nid, int64_t offset, int length, struct descriptor_xd *dsc){
  INIT_TREESUCCESS;
  char *buffer = malloc(length);
  status = ReadProperty_safe(tinfo,offset, buffer, length);
  if STATUS_OK
    status = MdsSerializeDscIn(buffer, dsc);
  if (dsc->pointer)
    status = TreeMakeNidsLocal(dsc->pointer, nid);
  free(buffer);
  return status;
}

static int GetCompressedSegmentRows(TREE_INFO * tinfo, const int64_t offset, int *rows){
  INIT_TREESUCCESS;
  int length = 60;
  char buffer[60];
  status = ReadProperty_safe(tinfo,offset, buffer, length);
  if STATUS_OK {
    if ((unsigned char)buffer[3] == CLASS_CA || (unsigned char)buffer[3] == CLASS_A) {
      char dimct = buffer[11];
      if (dimct == 1) {
        int arsize = swapint(buffer + 12);
        *rows = arsize / swapshort(buffer);
      } else
        *rows = swapint(buffer + 16 + dimct * 4);
      return TreeSUCCESS;
    } else
      return TreeFAILURE;
  }
  return status;
}

static int PutSegmentHeader(TREE_INFO * tinfo, SEGMENT_HEADER * hdr, int64_t * offset){
  INIT_TREESUCCESS;
  int64_t loffset;
  int j;
  char *next_row_fix = getenv("NEXT_ROW_FIX");
  char buffer[2 * sizeof(char) + 1 * sizeof(short) + 10 * sizeof(int) +
                       3 * sizeof(int64_t)], *bptr = buffer;
  if (*offset == -1) {
    TreeLockDatafile(tinfo, 0, 0);
    *offset = MDS_IO_LSEEK(tinfo->data_file->put, 0, SEEK_END);
    loffset = 0;
  } else {
    TreeLockDatafile(tinfo, 0, *offset);
    loffset = *offset;
    MDS_IO_LSEEK(tinfo->data_file->put, *offset, SEEK_SET);
  }
  *bptr = hdr->dtype;
  bptr++;
  *bptr = hdr->dimct;
  bptr++;
  for (j = 0; j < 8; j++) {
    LoadInt(hdr->dims[j], bptr);
    bptr += sizeof(int);
  }
  LoadShort(hdr->length, bptr);
  bptr += sizeof(short);
  LoadInt(hdr->idx, bptr);
  bptr += sizeof(int);
  if (next_row_fix != 0) {
    int fix = atoi(next_row_fix);
    if (fix > 0) {
      if (fix <= hdr->next_row) {
        hdr->next_row -= fix;
        printf("next row adjusted down %d, now %d\n", fix, hdr->next_row);
      } else
        printf("next row not adjusted, requested=%d, next_row=%d\n", fix, hdr->next_row);
    }
  }
  LoadInt(hdr->next_row, bptr);
  bptr += sizeof(int);
  LoadQuad(hdr->index_offset, bptr);
  bptr += sizeof(int64_t);
  LoadQuad(hdr->data_offset, bptr);
  bptr += sizeof(int64_t);
  LoadQuad(hdr->dim_offset, bptr);
  bptr += sizeof(int64_t);
  status = MDS_IO_WRITE(tinfo->data_file->put, buffer, sizeof(buffer))
        == sizeof(buffer) ? TreeSUCCESS : TreeFAILURE;
  TreeUnLockDatafile(tinfo, 0, loffset);
  return status;
}

static int PutSegmentIndex(TREE_INFO * tinfo, SEGMENT_INDEX * idx, int64_t * offset){
  INIT_TREESUCCESS;
  int i;
  int64_t loffset;
  char buffer[sizeof(int64_t) + sizeof(int) +
                       SEGMENTS_PER_INDEX * (6 * sizeof(int64_t) + 4 * sizeof(int))], *bptr =
      buffer;
  if (*offset == -1) {
    TreeLockDatafile(tinfo, 0, 0);
    loffset = 0;
    *offset = MDS_IO_LSEEK(tinfo->data_file->put, 0, SEEK_END);
  } else {
    TreeLockDatafile(tinfo, 0, *offset);
    loffset = *offset;
    MDS_IO_LSEEK(tinfo->data_file->put, *offset, SEEK_SET);
  }
  LoadQuad(idx->previous_offset, bptr);
  bptr += sizeof(int64_t);
  LoadInt(idx->first_idx, bptr);
  bptr += sizeof(int);
  for (i = 0; i < SEGMENTS_PER_INDEX; i++) {
    LoadQuad(idx->segment[i].start, bptr);
    bptr += sizeof(int64_t);
    LoadQuad(idx->segment[i].end, bptr);
    bptr += sizeof(int64_t);
    LoadQuad(idx->segment[i].start_offset, bptr);
    bptr += sizeof(int64_t);
    LoadInt(idx->segment[i].start_length, bptr);
    bptr += sizeof(int);
    LoadQuad(idx->segment[i].end_offset, bptr);
    bptr += sizeof(int64_t);
    LoadInt(idx->segment[i].end_length, bptr);
    bptr += sizeof(int);
    LoadQuad(idx->segment[i].dimension_offset, bptr);
    bptr += sizeof(int64_t);
    LoadInt(idx->segment[i].dimension_length, bptr);
    bptr += sizeof(int);
    LoadQuad(idx->segment[i].data_offset, bptr);
    bptr += sizeof(int64_t);
    LoadInt(idx->segment[i].rows, bptr);
    bptr += sizeof(int);
  }
  status = MDS_IO_WRITE(tinfo->data_file->put, buffer, sizeof(buffer))
        == sizeof(buffer) ? TreeSUCCESS : TreeFAILURE;
  TreeUnLockDatafile(tinfo, 0, loffset);
  return status;
}

int TreePutExtendedAttributes(TREE_INFO * tinfo, EXTENDED_ATTRIBUTES * att, int64_t * offset){
  INIT_TREESUCCESS;
  int i;
  int64_t loffset;
  char buffer[sizeof(int64_t) + FACILITIES_PER_EA * (sizeof(int64_t) + sizeof(int))],
      *bptr = buffer;
  if (*offset == -1) {
    TreeLockDatafile(tinfo, 0, 0);
    loffset = 0;
    *offset = MDS_IO_LSEEK(tinfo->data_file->put, 0, SEEK_END);
  } else {
    TreeLockDatafile(tinfo, 0, *offset);
    loffset = *offset;
    MDS_IO_LSEEK(tinfo->data_file->put, *offset, SEEK_SET);
  }
  LoadQuad(att->next_ea_offset, bptr);
  bptr += sizeof(int64_t);
  for (i = 0; i < FACILITIES_PER_EA; i++) {
    LoadQuad(att->facility_offset[i], bptr);
    bptr += sizeof(int64_t);
  }
  for (i = 0; i < FACILITIES_PER_EA; i++) {
    LoadInt(att->facility_length[i], bptr);
    bptr += sizeof(int);
  }
  status = MDS_IO_WRITE(tinfo->data_file->put, buffer, sizeof(buffer))
        == sizeof(buffer) ? TreeSUCCESS : TreeFAILURE;
  TreeUnLockDatafile(tinfo, 0, loffset);
  return status;
}

static int PutNamedAttributesIndex(TREE_INFO * tinfo, NAMED_ATTRIBUTES_INDEX * index,
                                   int64_t * offset){
  INIT_TREESUCCESS;
  int i;
  int64_t loffset;
  char buffer[sizeof(int64_t) + NAMED_ATTRIBUTES_PER_INDEX * (NAMED_ATTRIBUTE_NAME_SIZE + sizeof(int64_t) + sizeof(int))], *bptr = buffer;
  if (*offset == -1) {
    TreeLockDatafile(tinfo, 0, 0);
    loffset = 0;
    *offset = MDS_IO_LSEEK(tinfo->data_file->put, 0, SEEK_END);
  } else {
    TreeLockDatafile(tinfo, 0, *offset);
    loffset = *offset;
    MDS_IO_LSEEK(tinfo->data_file->put, *offset, SEEK_SET);
  }
  LoadQuad(index->previous_offset, bptr);
  bptr += sizeof(int64_t);
  for (i = 0; i < NAMED_ATTRIBUTES_PER_INDEX; i++) {
    memcpy(bptr, index->attribute[i].name, NAMED_ATTRIBUTE_NAME_SIZE);
    bptr += NAMED_ATTRIBUTE_NAME_SIZE;
    LoadQuad(index->attribute[i].offset, bptr);
    bptr += sizeof(int64_t);
    LoadInt(index->attribute[i].length, bptr);
    bptr += sizeof(int);
  }
  status = MDS_IO_WRITE(tinfo->data_file->put, buffer, sizeof(buffer))
        == sizeof(buffer) ? TreeSUCCESS : TreeFAILURE;
  TreeUnLockDatafile(tinfo, 0, loffset);
  return status;
}

static int PutInitialValue(TREE_INFO * tinfo, int *dims, struct descriptor_a *array,
                           int64_t * offset)
{
  int status;
  int length = array->length;
  int i;
  int64_t loffset;
  if (*offset == -1) {
    TreeLockDatafile(tinfo, 0, 0);
    loffset = 0;
    *offset = MDS_IO_LSEEK(tinfo->data_file->put, 0, SEEK_END);
  } else {
    TreeLockDatafile(tinfo, 0, *offset);
    loffset = *offset;
    MDS_IO_LSEEK(tinfo->data_file->put, *offset, SEEK_SET);
  }
  for (i = 0; i < array->dimct; i++)
    length = length * dims[i];
  ALLOCATE_BUFFER(length,buffer)
  CHECK_ENDIAN_TRANSFER(array->pointer,length,array->length,array->dtype,buffer)
  status = (MDS_IO_WRITE(tinfo->data_file->put, buffer, length) == length) ? TreeSUCCESS : TreeFAILURE;
  FREE_BUFFER(buffer);
  TreeUnLockDatafile(tinfo, 0, loffset);
  return status;
}

static int PutDimensionValue(TREE_INFO * tinfo, int64_t * timestamps, int rows_filled, int ndims,
                             int *dims, int64_t * offset)
{
  INIT_TREESUCCESS;
  int length = sizeof(int64_t);
  int64_t loffset;
  unsigned char *buffer = 0;
  if (*offset == -1) {
    TreeLockDatafile(tinfo, 0, 0);
    loffset = 0;
    *offset = MDS_IO_LSEEK(tinfo->data_file->put, 0, SEEK_END);
  } else {
    TreeLockDatafile(tinfo, 0, *offset);
    loffset = *offset;
    MDS_IO_LSEEK(tinfo->data_file->put, *offset, SEEK_SET);
  }
  length = length * dims[ndims - 1] - (sizeof(int64_t) * rows_filled);
  if (length > 0) {
    buffer = malloc(length);
    memset(buffer, 0, length);
  }
  if (rows_filled > 0)
    status = MDS_IO_WRITE(tinfo->data_file->put, timestamps, rows_filled * sizeof(int64_t))
          == (int)(rows_filled * sizeof(int64_t)) ? TreeSUCCESS : TreeFAILURE;
  if (STATUS_OK && length > 0)
    status = MDS_IO_WRITE(tinfo->data_file->put, buffer, length) == length ? TreeSUCCESS : TreeFAILURE;
  if (buffer)
    free(buffer);
  TreeUnLockDatafile(tinfo, 0, loffset);
  return status;
}

static int GetSegmentHeader(TREE_INFO * tinfo, const int64_t offset, SEGMENT_HEADER * hdr){
  INIT_TREESUCCESS;
  char buffer[2 * sizeof(char) + 1 * sizeof(short) + 10 * sizeof(int) + 3 * sizeof(int64_t)], *bptr;
  status = ReadProperty_safe(tinfo,offset, buffer, sizeof(buffer));
  if (status == TreeSUCCESS) {
    bptr = buffer;
    hdr->dtype = *bptr++;
    hdr->dimct = *bptr++;
    int i;
    for (i = 0; i < 8; i++, bptr += sizeof(int))
      hdr->dims[i] = swapint(bptr);
    hdr->length = swapshort(bptr);
    bptr += sizeof(short);
    hdr->idx = swapint(bptr);
    bptr += sizeof(int);
    hdr->next_row = swapint(bptr);
    bptr += sizeof(int);
    hdr->index_offset = swapquad(bptr);
    bptr += sizeof(int64_t);
    hdr->data_offset = swapquad(bptr);
    bptr += sizeof(int64_t);
    hdr->dim_offset = swapquad(bptr);
    bptr += sizeof(int64_t);
  }
  return status;
}

static int GetSegmentIndex(TREE_INFO * tinfo, const int64_t offset, SEGMENT_INDEX * idx){
  INIT_TREESUCCESS;
  char buffer[sizeof(int64_t) + sizeof(int) + SEGMENTS_PER_INDEX * (6 * sizeof(int64_t) + 4 * sizeof(int))], *bptr;
  status = ReadProperty_safe(tinfo, offset, buffer, sizeof(buffer));
  if (status == TreeSUCCESS) {
    bptr = buffer;
    idx->previous_offset = swapquad(bptr);
    bptr += sizeof(int64_t);
    idx->first_idx = swapint(bptr);
    bptr += sizeof(int);
    int i;
    for (i = 0; i < SEGMENTS_PER_INDEX; i++) {
      idx->segment[i].start = swapquad(bptr);
      bptr += sizeof(int64_t);
      idx->segment[i].end = swapquad(bptr);
      bptr += sizeof(int64_t);
      idx->segment[i].start_offset = swapquad(bptr);
      bptr += sizeof(int64_t);
      idx->segment[i].start_length = swapint(bptr);
      bptr += sizeof(int);
      idx->segment[i].end_offset = swapquad(bptr);
      bptr += sizeof(int64_t);
      idx->segment[i].end_length = swapint(bptr);
      bptr += sizeof(int);
      idx->segment[i].dimension_offset = swapquad(bptr);
      bptr += sizeof(int64_t);
      idx->segment[i].dimension_length = swapint(bptr);
      bptr += sizeof(int);
      idx->segment[i].data_offset = swapquad(bptr);
      bptr += sizeof(int64_t);
      idx->segment[i].rows = swapint(bptr);
      bptr += sizeof(int);
    }
  }
  return status;
}

int TreeGetExtendedAttributes(TREE_INFO * tinfo, const int64_t offset, EXTENDED_ATTRIBUTES * att){
  INIT_TREESUCCESS;
  char buffer[sizeof(int64_t) + FACILITIES_PER_EA * (sizeof(int64_t) + sizeof(int))], *bptr;
  status = ReadProperty_safe(tinfo,offset, buffer, sizeof(buffer));
  if (status == TreeSUCCESS) {
    bptr = buffer;
    att->next_ea_offset = swapquad(bptr);
    bptr += sizeof(int64_t);
    int i;
    for (i = 0; i < FACILITIES_PER_EA; i++) {
      att->facility_offset[i] = swapquad(bptr);
      bptr += sizeof(int64_t);
    }
    for (i = 0; i < FACILITIES_PER_EA; i++) {
      att->facility_length[i] = swapint(bptr);
      bptr += sizeof(int);
    }
  }
  return status;
}

static int GetNamedAttributesIndex(TREE_INFO * tinfo, const int64_t offset, NAMED_ATTRIBUTES_INDEX * index){
  INIT_TREESUCCESS;
  char buffer[sizeof(int64_t) + NAMED_ATTRIBUTES_PER_INDEX * (sizeof(int64_t) + sizeof(int) + NAMED_ATTRIBUTE_NAME_SIZE)], *bptr;
  status = ReadProperty_safe(tinfo,offset, buffer, sizeof(buffer));
  if (status == TreeSUCCESS) {
    bptr = buffer;
    index->previous_offset = swapquad(bptr);
    bptr += sizeof(int64_t);
    int i;
    for (i = 0; i < NAMED_ATTRIBUTES_PER_INDEX; i++) {
      memcpy(index->attribute[i].name, bptr, NAMED_ATTRIBUTE_NAME_SIZE);
      bptr += NAMED_ATTRIBUTE_NAME_SIZE;
      index->attribute[i].offset = swapquad(bptr);
      bptr += sizeof(int64_t);
      index->attribute[i].length = swapint(bptr);
      bptr += sizeof(int);
    }
  }
  return status;
}

static int CopyStandardRecord(TREE_INFO * tinfo_in, TREE_INFO * tinfo_out, int nid, int64_t * offset,
                              int *length, int compress){
  int status;
  INIT_AND_FREEXD_ON_EXIT(xd);
  status = TreeGetDsc(tinfo_in, nid, *offset, *length, &xd);
  if STATUS_OK
    status = TreePutDsc(tinfo_out, nid, (struct descriptor *)&xd, offset, length, compress);
  FREEXD_NOW(xd);
  if STATUS_NOT_OK {
    *offset = -1;
    *length = 0;
  }
  return status;
}

static int CopyNamedAttributes(TREE_INFO * tinfo_in, TREE_INFO * tinfo_out, int nid, int64_t * offset,
                               int *length, int compress){
  EMPTYXD(xd);
  NAMED_ATTRIBUTES_INDEX index;
  int status = GetNamedAttributesIndex(tinfo_in, *offset, &index);
  if STATUS_OK {
    int i;
    *length=0;
    if (index.previous_offset != -1)
      CopyNamedAttributes(tinfo_in, tinfo_out, nid, &index.previous_offset, 0, compress);
    for (i = 0; i < NAMED_ATTRIBUTES_PER_INDEX; i++) {
      if (index.attribute[i].name[0] != '\0' && index.attribute[i].offset != -1) {
        status = TreeGetDsc(tinfo_in, nid, index.attribute[i].offset, index.attribute[i].length, &xd);
        if STATUS_OK {
          status = TreePutDsc(tinfo_out, nid, (struct descriptor *)&xd,
                              &index.attribute[i].offset, &index.attribute[i].length, compress);
          if STATUS_NOT_OK {
            memset(index.attribute[i].name, 0, sizeof(index.attribute[i].name));
            index.attribute[i].offset = -1;
          }
          MdsFree1Dx(&xd, 0);
        }
      }
    }
    *offset = -1;
    status = PutNamedAttributesIndex(tinfo_out, &index, offset);
  } else
    *offset = -1;
  return status;
}

static int DataCopy(TREE_INFO * tinfo, TREE_INFO * tinfo_out, int64_t offset_in, int length_in,
                    int64_t * offset_out){
  INIT_TREESUCCESS;
  if (offset_in != -1 && length_in >= 0) {
    char *data = malloc(length_in);
    status = ReadProperty(tinfo,offset_in, data, length_in);
    if (STATUS_OK) {
      *offset_out = MDS_IO_LSEEK(tinfo_out->data_file->put, 0, SEEK_END);
      status = MDS_IO_WRITE(tinfo_out->data_file->put, data, length_in) == length_in;
    }
    free(data);
    if STATUS_NOT_OK
      *offset_out = -1;
  } else
    *offset_out = -1;
  return status;
}

static int CopySegment(TREE_INFO *tinfo_in, TREE_INFO *tinfo_out, int nid, SEGMENT_HEADER *header, SEGMENT_INFO *sinfo, int idx, int compress) {
  INIT_TREESUCCESS;
  if (compress) {
    int length;
    EMPTYXD(data_xd);
    EMPTYXD(dim_xd);
    status = ReadSegment(tinfo_in, nid, header, sinfo, idx, &data_xd, &dim_xd);
    if STATUS_OK {
      status = TreePutDsc(tinfo_out, nid, data_xd.pointer, &sinfo->data_offset, &length, compress);
      if STATUS_OK {
        sinfo->rows = length | 0x80000000;
        status = TreePutDsc(tinfo_out, nid, dim_xd.pointer, &sinfo->dimension_offset, &sinfo->dimension_length, compress);
        MdsFree1Dx(&dim_xd,0);
      }
      MdsFree1Dx(&data_xd,0);
    }
  } else {
    int length;
    if (sinfo->data_offset != -1 && sinfo->rows < 0) {
      length = sinfo->rows & 0x7fffffff;
      status = DataCopy(tinfo_in, tinfo_out, sinfo->data_offset, length, &sinfo->data_offset);
      if STATUS_OK
        status = DataCopy(tinfo_in, tinfo_out, sinfo->dimension_offset, sinfo->dimension_length,
                          &sinfo->dimension_offset);
    } else {
      int rowlen = header->length, i;
      for (i = 0; i < header->dimct - 1; i++)
        rowlen = rowlen * header->dims[i];
      if (sinfo->dimension_length == 0)
        length = sinfo->rows * sizeof(int64_t);
      else
        length = sinfo->dimension_length;
      status = DataCopy(tinfo_in, tinfo_out, sinfo->dimension_offset, length, &sinfo->dimension_offset);
      if STATUS_OK {
        length = rowlen * sinfo->rows;
        status = DataCopy(tinfo_in, tinfo_out, sinfo->data_offset, length, &sinfo->data_offset);
      }
    }
  }
  if (STATUS_OK && (sinfo->start_offset > 0)) {
    status = DataCopy(tinfo_in, tinfo_out, sinfo->start_offset, sinfo->start_length, &sinfo->start_offset);
    if(STATUS_OK && sinfo->end_offset > 0)
        status = DataCopy(tinfo_in, tinfo_out, sinfo->end_offset, sinfo->end_length, &sinfo->end_offset);
  }
  return status;
}

typedef struct indexlist {
  struct indexlist* next;
  int64_t           offset;
} indexlist_t;
static int CopySegmentIndex(TREE_INFO * tinfo_in, TREE_INFO * tinfo_out, int nid, SEGMENT_HEADER * shead,
        int64_t * index_offset,
        int64_t * data_offset __attribute__((unused)), int64_t * dim_offset __attribute__((unused)),
        int compress){
  INIT_TREESUCCESS;
  indexlist_t *tlist, *list = NULL;
  int64_t offset = *index_offset;
  while (offset>=0 && STATUS_OK) {
    tlist = malloc(sizeof(indexlist_t));
    tlist->next   = list;
    tlist->offset = offset;
    list          = tlist;
    status = ReadProperty(tinfo_in, offset, (char*)&offset, sizeof(offset));
#ifdef WORDS_BIGENDIAN
    offset = swapquad((char*)&offset);
#endif
    if (list->offset==offset)
      break;
  }
  *index_offset = -1;
  while (list) {
    if STATUS_OK {
      SEGMENT_INDEX sindex;
      status = GetSegmentIndex(tinfo_in, list->offset, &sindex);
      if STATUS_OK {
	int i;
	for (i = 0; (i < SEGMENTS_PER_INDEX) && STATUS_OK; i++) {
	  SEGMENT_INFO* sinfo = &sindex.segment[i];
	  status = CopySegment(tinfo_in, tinfo_out, nid, shead, sinfo, i,compress);
	}
	sindex.previous_offset = *index_offset;	*index_offset = -1; // append
	status = PutSegmentIndex(tinfo_out, &sindex, index_offset);
      }
    }
    tlist = list;
    list  = list->next;
    free(tlist);
  }
  return status;
}

static int CopySegmentedRecords(TREE_INFO * tinfo_in, TREE_INFO * tinfo_out, int nid, int64_t * offset,
                                int *length, int compress){
  SEGMENT_HEADER header;
  INIT_STATUS_AS GetSegmentHeader(tinfo_in, *offset, &header);
  if STATUS_OK {
    *length=0;
    status =
        CopySegmentIndex(tinfo_in, tinfo_out, nid, &header, &header.index_offset, &header.data_offset,
                         &header.dim_offset, compress);
    *offset = -1;
    status = PutSegmentHeader(tinfo_out, &header, offset);
  }
  return status;
}

int TreeCopyExtended(PINO_DATABASE * dbid_in, PINO_DATABASE * dbid_out, int nid, NCI * nci, int compress){
  EXTENDED_ATTRIBUTES attr;
  TREE_INFO *tinfo_in = dbid_in->tree_info, *tinfo_out = dbid_out->tree_info;
  int64_t now = -1;
  int64_t offset = -1;
  INIT_STATUS_AS TreeGetExtendedAttributes(tinfo_in, RfaToSeek(nci->DATA_INFO.DATA_LOCATION.rfa), &attr);
  if STATUS_OK {
    if (attr.facility_offset[NAMED_ATTRIBUTES_FACILITY] != -1)
      CopyNamedAttributes(tinfo_in, tinfo_out, nid,
                          &attr.facility_offset[NAMED_ATTRIBUTES_FACILITY],
                          &attr.facility_length[NAMED_ATTRIBUTES_FACILITY], compress);
    if (attr.facility_offset[SEGMENTED_RECORD_FACILITY] != -1)
      CopySegmentedRecords(tinfo_in, tinfo_out, nid,
                           &attr.facility_offset[SEGMENTED_RECORD_FACILITY],
                           &attr.facility_length[SEGMENTED_RECORD_FACILITY], compress);
    if (attr.facility_offset[STANDARD_RECORD_FACILITY] != -1)
      CopyStandardRecord(tinfo_in, tinfo_out, nid,
                         &attr.facility_offset[STANDARD_RECORD_FACILITY],
                         &attr.facility_length[STANDARD_RECORD_FACILITY], compress);
    status = TreePutExtendedAttributes(tinfo_out, &attr, &offset);
    if STATUS_OK {
      SeekToRfa(offset, nci->DATA_INFO.DATA_LOCATION.rfa);
      status = TreePutNci(tinfo_out, nid, nci, 0);
    }
    TreeSetViewDate(&now);
  }
  return status;
}


/*****************************************
 TimeContext sticks with current db (tree)
 *****************************************/
int _TreeSetTimeContext(void *dbid, struct descriptor *start, struct descriptor *end, struct descriptor *delta){
  timecontext_t* tc = &((PINO_DATABASE*)dbid)->timecontext;
  INIT_STATUS_AS MdsCopyDxXd(start, &tc->start);
  if STATUS_OK {
    status = MdsCopyDxXd(end, &tc->end);
    if STATUS_OK
      status = MdsCopyDxXd(delta, &tc->delta);
  }
  return status;
}

int TreeSetTimeContext(struct descriptor *start, struct descriptor *end, struct descriptor *delta){
  return _TreeSetTimeContext(*TreeCtx(), start, end, delta);
}

int _TreeGetTimeContext(void *dbid, struct descriptor_xd *start, struct descriptor_xd *end, struct descriptor_xd *delta){
  timecontext_t* tc = &((PINO_DATABASE*)dbid)->timecontext;
  INIT_STATUS_AS 1;
  if (start) RETURN_IF_NOT_OK(MdsCopyDxXd(tc->start.pointer,start));
  if (end  ) RETURN_IF_NOT_OK(MdsCopyDxXd(tc->end.pointer  ,end  ));
  if (delta) RETURN_IF_NOT_OK(MdsCopyDxXd(tc->delta.pointer,delta));
  return status;
}
int TreeGetTimeContext(struct descriptor_xd *start, struct descriptor_xd *end, struct descriptor_xd *delta){
  return _TreeGetTimeContext(*TreeCtx(), start, end, delta);
}

static int getOpaqueList(void *dbid, int nid, struct descriptor_xd *out) {
  INIT_VARS;
  int isOpList=0;
  EMPTYXD(segdata);
  EMPTYXD(segdim);
  status = _TreeGetSegment(dbid, nid, 0, &segdata, &segdim);
  isOpList = segdata.pointer && (segdata.pointer->dtype == DTYPE_OPAQUE);
  MdsFree1Dx(&segdata, 0);
  MdsFree1Dx(&segdim, 0);
  if (isOpList) {
    RETURN_IF_NOT_OK(open_header_read(vars));
    int numsegs = vars->shead.idx + 1;
    int apd_idx = 0;
    struct descriptor **dptr = malloc(sizeof(struct descriptor *) * numsegs);
    DESCRIPTOR_APD(apd, DTYPE_LIST, dptr, numsegs);
    memset(dptr, 0, sizeof(struct descriptor *) * numsegs);
    status = GetSegmentIndex(vars->tinfo, vars->shead.index_offset, &vars->sindex);
    int idx;
    for (idx = numsegs; STATUS_OK && idx > 0; idx--) {
      int segidx = idx - 1;
      while (STATUS_OK && segidx < vars->sindex.first_idx && vars->sindex.previous_offset > 0)
        status = GetSegmentIndex(vars->tinfo, vars->sindex.previous_offset, &vars->sindex);
      if STATUS_NOT_OK
        break;
      else {
        vars->sinfo = &vars->sindex.segment[segidx % SEGMENTS_PER_INDEX];
        EMPTYXD(segment);
        EMPTYXD(dim);
        status = ReadSegment(vars->tinfo, *(int*)vars->nid_ptr, &vars->shead, vars->sinfo, idx, &segment, &dim);
        if STATUS_OK {
          apd.pointer[apd_idx] = malloc(sizeof(struct descriptor_xd));
          memcpy(apd.pointer[apd_idx++], &segment, sizeof(struct descriptor_xd));
        } else {
          MdsFree1Dx(&segment, 0);
          MdsFree1Dx(&dim, 0);
        }
      }
    }
    if STATUS_OK {
	status = MdsCopyDxXd((struct descriptor *)&apd, out);
    }
    for (idx = 0; idx < apd_idx; idx++) {
      if (apd.pointer[idx] != NULL) {
	MdsFree1Dx((struct descriptor_xd *)apd.pointer[idx], 0);
	free(apd.pointer[idx]);
      }
    }
    if (apd.pointer)
      free(apd.pointer);
  } else if STATUS_OK {
    status = 0;
  }
  return status;
}

int _TreeGetSegmentedRecord(void *dbid, int nid, struct descriptor_xd *data)
{
  INIT_TREESUCCESS;
  int opstatus = getOpaqueList(dbid, nid, data );
  if IS_OK(opstatus)
    return opstatus;
  static int activated = 0;
  static int (*addr) (void *, int, struct descriptor *, struct descriptor *, struct descriptor *, struct descriptor_xd *);
  if (!activated) {
    static DESCRIPTOR(library, "XTreeShr");
    static DESCRIPTOR(routine, "_XTreeGetTimedRecord");
    status = LibFindImageSymbol(&library, &routine, &addr);
    if STATUS_OK
      activated = 1;
    else {
      fprintf(stderr, "Error activating XTreeShr library. Cannot access segmented records.\n");
      return status;
    }
  }
  timecontext_t* tc = &((PINO_DATABASE*)dbid)->timecontext;
  return (*addr) (dbid, nid, tc->start.pointer, tc->end.pointer, tc->delta.pointer, data);
}

int _TreePutRow(void *dbid, int nid, int bufsize, int64_t * timestamp, struct descriptor_a *data){
  DESCRIPTOR_A_COEFF(initValue, data->length, data->dtype, 0, 8, 0);
  INIT_STATUS_AS _TreePutTimestampedSegment(dbid, nid, timestamp, data);
  if (status == TreeNOSEGMENTS || status == TreeBUFFEROVF) {
    status = TreeSUCCESS;
    while (data && data->dtype == DTYPE_DSC)
      data = (struct descriptor_a *)data->pointer;
    if (data) {
      if (data->class == CLASS_A) {
        int i;
        initValue.arsize = data->arsize * bufsize;
        initValue.pointer = initValue.a0 = malloc(initValue.arsize);
        memset(initValue.pointer, 0, initValue.arsize);
        initValue.dimct = data->dimct + 1;
        if (data->dimct == 1) {
          initValue.arsize = data->arsize * bufsize;
          initValue.m[0] = data->arsize / data->length;
          initValue.m[1] = bufsize;
        } else {
          A_COEFF_TYPE *data_c = (A_COEFF_TYPE *) data;
          for (i = 0; i < data->dimct; i++)
            initValue.m[i] = data_c->m[i];
          initValue.m[data->dimct] = bufsize;
        }
      } else if (data->class == CLASS_S || data->class == CLASS_D) {
        initValue.arsize = data->length * bufsize;
        initValue.pointer = initValue.a0 = malloc(initValue.arsize);
        memset(initValue.pointer, 0, initValue.arsize);
        initValue.dimct = 1;
        initValue.m[0] = bufsize;
      } else
        status = TreeFAILURE;
    } else
      status = TreeFAILURE;
    if STATUS_OK {
      status = _TreeBeginTimestampedSegment(dbid, nid, (struct descriptor_a *)&initValue, -1);
      free(initValue.pointer);
      if STATUS_OK
        status = _TreePutTimestampedSegment(dbid, nid, timestamp, data);
    }
  }
  return status;
}

/* RETURN_IF_NOT_OK(get_segment(vars,&idx))
 * opens tree->extended_nci->segment_handler->segemnt_index
 * and finds segment idx.
 * idx can be number in the range of 0-(number of segments-1) or -1.
 * If -1 is specified return the last segment started for the segment.
 * - no clean up required. -
 */
inline static int get_segment(vars_t* vars) {
  INIT_TREESUCCESS;
  RETURN_IF_NOT_OK(open_index_read(vars));
  if (vars->idx == -1) vars->idx = vars->shead.idx;
  while (STATUS_OK && vars->idx < vars->sindex.first_idx && vars->sindex.previous_offset > 0)
    status = GetSegmentIndex(vars->tinfo, vars->sindex.previous_offset, &vars->sindex);
  if STATUS_NOT_OK
    return status;
  if (vars->idx < vars->sindex.first_idx || vars->idx > vars->sindex.first_idx + SEGMENTS_PER_INDEX)
    return TreeFAILURE;
  vars->sinfo = &vars->sindex.segment[vars->idx % SEGMENTS_PER_INDEX];
  return status;
}

int _TreeGetSegmentLimits(void *dbid, int nid, int idx, struct descriptor_xd *retStart,
                          struct descriptor_xd *retEnd){
  INIT_VARS;vars->idx=idx;
  RETURN_IF_NOT_OK(get_segment(vars));
  return getSegmentLimits(vars, retStart, retEnd);
}

int _TreeGetSegment(void *dbid, int nid, int idx, struct descriptor_xd *segment,
                    struct descriptor_xd *dim){
  INIT_VARS;vars->idx=idx;
  RETURN_IF_NOT_OK(get_segment(vars));
  return ReadSegment(vars->tinfo, nid, &vars->shead, vars->sinfo, vars->idx, segment, dim);
}

int _TreeGetSegmentInfo(void *dbid, int nid, int idx, char *dtype, char *dimct, int *dims, int *next_row){
  INIT_VARS;vars->idx=idx;
  RETURN_IF_NOT_OK(get_segment(vars));
  if (vars->sinfo->data_offset == -1)
    return TreeFAILURE;
  *dtype = vars->shead.dtype;
  *dimct = vars->shead.dimct;
  memcpy(dims, vars->shead.dims, sizeof(vars->shead.dims));
  if (vars->idx == vars->shead.idx)
    *next_row = vars->shead.next_row;
  else if (vars->sinfo->rows < 1)
    return GetCompressedSegmentRows(vars->tinfo, vars->sinfo->data_offset, next_row);
  else
    *next_row = vars->sinfo->rows;
  return status;
}

static int isSegmentInRange(vars_t* vars,
                          struct descriptor *start,
                          struct descriptor *end){
  int ans = B_FALSE;
  if ((start && start->pointer) || (end && end->pointer)) {
    INIT_TREESUCCESS;
    STATIC_CONSTANT DESCRIPTOR(tdishr, "TdiShr");
    STATIC_CONSTANT DESCRIPTOR(tdiexecute, "TdiExecute");
    STATIC_THREADSAFE int (*addr) () = NULL;
    if (!addr)
      status = LibFindImageSymbol(&tdishr, &tdiexecute, &addr);
    if STATUS_OK {
      EMPTYXD(segstart);
      EMPTYXD(segend);
      DESCRIPTOR_LONG(ans_d, &ans);
      status = getSegmentLimits(vars, &segstart, &segend);
      if STATUS_OK {
        if ((start && start->pointer) && (end && end->pointer)) {
          STATIC_CONSTANT DESCRIPTOR(expression, "($ <= $) && ($ >= $)");
          void *arglist[8] = { (void *)7 };
          arglist[1] = &expression;
          arglist[2] = start;
          arglist[3] = &segend;
          arglist[4] = end;
          arglist[5] = &segstart;
          arglist[6] = &ans_d;
          arglist[7] = MdsEND_ARG;
          status = (int)((char *)LibCallg(arglist, addr) - (char *)0);
        } else {
          if (start && start->pointer) {
            STATIC_CONSTANT DESCRIPTOR(expression, "($ <= $)");
            void *arglist[6] = { (void *)5 };
            arglist[1] = &expression;
            arglist[2] = start;
            arglist[3] = &segend;
            arglist[4] = &ans_d;
            arglist[5] = MdsEND_ARG;
            status = (int)((char *)LibCallg(arglist, addr) - (char *)0);
          } else {
            STATIC_CONSTANT DESCRIPTOR(expression, "($ >= $)");
            void *arglist[6] = { (void *)5 };
            arglist[1] = &expression;
            arglist[2] = end;
            arglist[3] = &segstart;
            arglist[4] = &ans_d;
            arglist[5] = MdsEND_ARG;
            status = (int)((char *)LibCallg(arglist, addr) - (char *)0);
          }
        }
      }
      MdsFree1Dx(&segstart, 0);
      MdsFree1Dx(&segend, 0);
    }
  } else
    ans = B_TRUE;
  return ans;
}

int _TreeGetSegments(void *dbid, int nid, struct descriptor *start, struct descriptor *end,
                     struct descriptor_xd *out){
  /* Get all the segments in an apd which contain data between the start and end times specified
   */
  INIT_VARS;
  RETURN_IF_NOT_OK(open_header_read(vars));
  int numsegs = vars->shead.idx + 1;
  int segfound = B_FALSE;
  int apd_idx = 0;
  struct descriptor **dptr = malloc(sizeof(struct descriptor *) * numsegs * 2);
  DESCRIPTOR_APD(apd, DTYPE_LIST, dptr, numsegs * 2);
  memset(dptr, 0, sizeof(struct descriptor *) * numsegs * 2);
  status = GetSegmentIndex(vars->tinfo, vars->shead.index_offset, &vars->sindex);
  for (vars->idx = numsegs; STATUS_OK && vars->idx > 0; vars->idx--) {
    int segidx = vars->idx - 1;
    while (STATUS_OK && segidx < vars->sindex.first_idx && vars->sindex.previous_offset > 0)
      status = GetSegmentIndex(vars->tinfo, vars->sindex.previous_offset, &vars->sindex);
    if STATUS_NOT_OK
      break;
    else {
      vars->sinfo = &vars->sindex.segment[segidx % SEGMENTS_PER_INDEX];
      if (isSegmentInRange(vars, start, end)) {
        EMPTYXD(segment);
        EMPTYXD(dim);
        segfound = B_TRUE;
        status = ReadSegment(vars->tinfo, nid, &vars->shead, vars->sinfo, vars->idx, &segment, &dim);
        if STATUS_OK {
          apd.pointer[apd_idx] = malloc(sizeof(struct descriptor_xd));
          memcpy(apd.pointer[apd_idx++], &segment, sizeof(struct descriptor_xd));
          apd.pointer[apd_idx] = malloc(sizeof(struct descriptor_xd));
          memcpy(apd.pointer[apd_idx++], &dim, sizeof(struct descriptor_xd));
        } else {
          MdsFree1Dx(&segment, 0);
          MdsFree1Dx(&dim, 0);
        }
      } else if (segfound)
          break;
    }
  }
  if STATUS_OK {
    apd.arsize = apd_idx * sizeof(struct descriptor *);
    int i;
    for (i = 0; i < apd_idx / 2 / 2; i++) {
      struct descriptor *data = apd.pointer[i * 2], *dim = apd.pointer[i * 2 + 1];
      apd.pointer[i * 2] = apd.pointer[apd_idx - i * 2 - 2];
      apd.pointer[i * 2 + 1] = apd.pointer[apd_idx - i * 2 - 1];
      apd.pointer[apd_idx - i * 2 - 2] = data;
      apd.pointer[apd_idx - i * 2 - 1] = dim;
    }
    status = MdsCopyDxXd((struct descriptor *)&apd, out);
  }
  for (vars->idx = 0; vars->idx < apd_idx; vars->idx++) {
    if (apd.pointer[vars->idx] != NULL) {
      MdsFree1Dx((struct descriptor_xd *)apd.pointer[vars->idx], 0);
      free(apd.pointer[vars->idx]);
    }
  }
  if (apd.pointer)
    free(apd.pointer);
  return status;
}

#define SEGMENT_SCALE_NAME "SegmentScale"
int _TreeSetSegmentScale(void *dbid, int nid, struct descriptor *value) {
  return _TreeSetXNci(dbid, nid, SEGMENT_SCALE_NAME, value);
}
int _TreeGetSegmentScale(void *dbid, int nid, struct descriptor_xd *value) {
  return _TreeGetXNci(dbid, nid, SEGMENT_SCALE_NAME, value);
}
