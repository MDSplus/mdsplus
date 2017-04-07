#include "treeshrp.h"
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

Segmented records use a special extended attributes record stored at the file offset (nci rfa field)
which describes the location of segment index and segment information metadata stored in the
datafile. There is an EXTENDED_NCI bit in the nci flags2 field which indicates that the node contains
extended nci information. The EXTENDED_NCI capability supports different types of "facilities" of which
3 are currently supported: STANDARD_RECORD, SEGMENTED_RECORD, NAMED_ATTRIBUTES. The NAMED_ATTRIBUTE
facility provides a means for adding extra node characteristics by defining name/value pairs. The use
of the STANDARD_RECORD facility is currently only used if the node also has named attributes.

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

#ifndef _WIN32
static int saved_uic = 0;
#define SAVED_UIC \
if (!saved_uic) \
  saved_uic = (getgid() << 16) | getuid();
#else
#define SAVED_UIC
const int saved_uic = 0;
#endif

#ifdef WORDS_BIGENDIAN

#define ALLOCATE_BUFFER(SIZE,BUFFER) \
char *BUFFER = malloc(SIZE);
#define _CHECK_ENDIAN_TRANFER(BUFFER_IN,SIZE,LENGTH,DTYPE,BUFFER_OUT,COPY) {\
if (LENGTH > 1 && DTYPE+0 != DTYPE_T && DTYPE+0 != DTYPE_IDENT && DTYPE+0 != DTYPE_PATH) { \
  char *bptr; \
  int i; \
  switch (LENGTH) { \
  case 2: \
    for (i = 0, bptr = (char*)BUFFER_OUT; i < (int)SIZE / LENGTH; i++, bptr += sizeof(short)) \
      LoadShort(((short *)BUFFER_IN)[i], bptr); \
    break; \
  case 4: \
    for (i = 0, bptr = (char*)BUFFER_OUT; i < (int)SIZE / LENGTH; i++, bptr += sizeof(int)) \
      LoadInt(((int *)BUFFER_IN)[i], bptr); \
    break; \
  case 8: \
    for (i = 0, bptr = (char*)BUFFER_OUT; i < (int)SIZE / LENGTH; i++, bptr += sizeof(int64_t)) \
      LoadQuad(((int64_t *) BUFFER_IN)[i], bptr); \
    break; \
  } \
} else {COPY;}}
#define CHECK_ENDIAN_TRANSFER(BUFFER_IN,SIZE,LENGTH,DTYPE,BUFFER_OUT) _CHECK_ENDIAN_TRANFER(BUFFER_IN,SIZE,LENGTH,DTYPE,BUFFER_OUT,memcpy(BUFFER_OUT,BUFFER_IN,SIZE))
#define CHECK_ENDIAN(BUFFER,SIZE,LENGTH,DTYPE) _CHECK_ENDIAN_TRANFER(BUFFER,SIZE,LENGTH,DTYPE,BUFFER,)
#define FREE_BUFFER(BUFFER) free(BUFFER);

#else

#define ALLOCATE_BUFFER(SIZE,BUFFER) char *BUFFER;
#define CHECK_ENDIAN(POINTER,SIZE,LENGTH,DTYPE) {}
#define CHECK_ENDIAN_TRANSFER(BUFFER_IN,SIZE,LENGTH,DTYPE,BUFFER_OUT) BUFFER_OUT = (char*)BUFFER_IN;
#define FREE_BUFFER(BUFFER) {}

#endif




typedef ARRAY_COEFF(char, 8) A_COEFF_TYPE;
static int PutNamedAttributesIndex(TREE_INFO * info_ptr, NAMED_ATTRIBUTES_INDEX * index, int64_t * offset);
static int PutSegmentHeader(TREE_INFO * info_ptr, SEGMENT_HEADER * hdr, int64_t * offset);
static int PutSegmentIndex(TREE_INFO * info_ptr, SEGMENT_INDEX * idx, int64_t * offset);
static int PutInitialValue(TREE_INFO * info_ptr, int *dims, struct descriptor_a *array, int64_t * offset);
static int PutDimensionValue(TREE_INFO * info_ptr, int64_t * timestamps, int rows_filled, int ndims, int *dims, int64_t * offset);
static int GetSegmentHeader(TREE_INFO * info_ptr, int64_t offset, SEGMENT_HEADER * hdr);
static int GetSegmentIndex(TREE_INFO * info_ptr, int64_t offset, SEGMENT_INDEX * idx);
static int GetNamedAttributesIndex(TREE_INFO * info_ptr, int64_t offset, NAMED_ATTRIBUTES_INDEX * index);

static int __TreeBeginSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end,
                              struct descriptor *dimension,
                              struct descriptor_a *initialValue, int idx, int rows_filled);
int _TreeBeginSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end,
                      struct descriptor *dimension, struct descriptor_a *initialValue, int idx){
  return __TreeBeginSegment(dbid, nid, start, end, dimension, initialValue, idx, 0);
}

int TreeBeginSegment(int nid, struct descriptor *start, struct descriptor *end,
                     struct descriptor *dimension, struct descriptor_a *initialValue, int idx){
  return _TreeBeginSegment(*TreeCtx(), nid, start, end, dimension, initialValue, idx);
}

int _TreeMakeSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end,
                     struct descriptor *dimension,
                     struct descriptor_a *initialValue, int idx, int rows_filled){
  return __TreeBeginSegment(dbid, nid, start, end, dimension, initialValue, idx, rows_filled);
}

int TreeMakeSegment(int nid, struct descriptor *start, struct descriptor *end,
                    struct descriptor *dimension,
                    struct descriptor_a *initialValue, int idx, int rows_filled){
  return _TreeMakeSegment(*TreeCtx(), nid, start, end, dimension, initialValue, idx, rows_filled);
}

static int __TreeBeginTimestampedSegment(void *dbid, int nid, int64_t * timestamps,
                                         struct descriptor_a *initialValue, int idx,
                                         int rows_filled);
int _TreeBeginTimestampedSegment(void *dbid, int nid, struct descriptor_a *initialValue, int idx){
  return __TreeBeginTimestampedSegment(dbid, nid, 0, initialValue, idx, 0);
}

int TreeBeginTimestampedSegment(int nid, struct descriptor_a *initialValue, int idx){
  return _TreeBeginTimestampedSegment(*TreeCtx(), nid, initialValue, idx);
}

int _TreeMakeTimestampedSegment(void *dbid, int nid, int64_t * timestamps,
                                struct descriptor_a *initialValue, int idx, int rows_filled){
  return __TreeBeginTimestampedSegment(dbid, nid, timestamps, initialValue, idx, rows_filled);
}

int TreeMakeTimestampedSegment(int nid, int64_t * timestamps, struct descriptor_a *initialValue,
                               int idx, int rows_filled){
  return _TreeMakeTimestampedSegment(*TreeCtx(), nid, timestamps, initialValue, idx, rows_filled);
}

int _TreeUpdateSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end,
                       struct descriptor *dimension, int idx);

int TreeUpdateSegment(int nid, struct descriptor *start, struct descriptor *end,
                      struct descriptor *dimension, int idx){
  return _TreeUpdateSegment(*TreeCtx(), nid, start, end, dimension, idx);
}

int _TreePutSegment(void *dbid, int nid, int startIdx, struct descriptor_a *data);
int TreePutSegment(int nid, int startIdx, struct descriptor_a *data){
  return _TreePutSegment(*TreeCtx(), nid, startIdx, data);
}

int _TreeGetSegmentTimesXd(void *dbid, int nid, int *nsegs, struct descriptor_xd *start_list, struct descriptor_xd *end_list);
int TreeGetSegmentTimesXd(int nid, int *nsegs, struct descriptor_xd *start_list, struct descriptor_xd *end_list){
  return _TreeGetSegmentTimesXd(*TreeCtx(), nid, nsegs, start_list, end_list);
}

int _TreeGetSegmentTimes(void *dbid, int nid, int *nsegs, uint64_t ** times);
int TreeGetSegmentTimes(int nid, int *nsegs, uint64_t ** times){
  return _TreeGetSegmentTimes(*TreeCtx(), nid, nsegs, times);
}

int _TreeGetNumSegments(void *dbid, int nid, int *num);
int TreeGetNumSegments(int nid, int *num){
  return _TreeGetNumSegments(*TreeCtx(), nid, num);
}

int TreeGetSegment(int nid, int idx, struct descriptor_xd *segment, struct descriptor_xd *dim){
  return _TreeGetSegment(*TreeCtx(), nid, idx, segment, dim);
}

int _TreeGetSegmentLimits(void *dbid, int nid, int idx, struct descriptor_xd *retStart,
                          struct descriptor_xd *retEnd);
int TreeGetSegmentLimits(int nid, int idx, struct descriptor_xd *retStart,
                         struct descriptor_xd *retEnd){
  return _TreeGetSegmentLimits(*TreeCtx(), nid, idx, retStart, retEnd);
}

int _TreeGetSegmentInfo(void *dbid, int nid, int idx, char *dtype, char *dimct, int *dims,
                        int *next_row);

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

int TreeGetSegments(int nid, struct descriptor *start, struct descriptor *end,
                    struct descriptor_xd *out){
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


#define NODE_PTR \
node_ptr = nid_to_node(dblist, nid_ptr); \
if (!node_ptr) \
  return TreeNNF;
//if (node_ptr->usage != TreeUSAGE_SIGNAL)
//  return  TreeINVDTPUSG;

#define SEGMENTREMOTE \
if (dblist->remote) { \
  fprintf(stderr,"Segmented records are not supported using thick client mode\n"); \
  return TreeFAILURE; \
}
//"
#define INFO_PTR \
nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx); \
if (!info_ptr) \
  return TreeNNF;

#define OPEN_DATAFILE_WRITE(CHECKINPUT) \
OPEN_TREE_READ; \
if (dblist->open_readonly) \
  return TreeREADONLY; \
int shot_open = (dblist->shotid != -1); \
NODE_PTR; \
SEGMENTREMOTE; \
int stv; \
NCI local_nci; \
EXTENDED_ATTRIBUTES attributes; \
INFO_PTR; \
status = TreeCallHook(PutData, info_ptr, nid); \
if (status && STATUS_NOT_OK) \
  return status; \
int64_t saved_viewdate; \
TreeGetViewDate(&saved_viewdate); \
status = TreeGetNciLw(info_ptr, nidx, &local_nci); \
if STATUS_NOT_OK return status; \
if (STATUS_OK && (shot_open && (local_nci.flags & NciM_NO_WRITE_SHOT))) \
  RETURN(UNLOCK_NCI,TreeNOWRITESHOT); \
if (STATUS_OK && (!shot_open && (local_nci.flags & NciM_NO_WRITE_MODEL))) \
  RETURN(UNLOCK_NCI,TreeNOWRITEMODEL); \
CHECKINPUT; \
if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1){ \
  status = TreeOpenDatafileW(info_ptr, &stv, 0); \
  if STATUS_NOT_OK \
    RETURN(UNLOCK_NCI,status) \
}

#define UNLOCK_NCI TreeUnLockNci(info_ptr, 0, nidx);
#define RETURN(UNLOCK,STATUS) {\
UNLOCK; \
return STATUS; \
}

#define UPDATE_FINISH status = PutSegmentIndex(info_ptr, &segment_index, &index_offset);

#define begin_finish \
status = PutSegmentIndex(info_ptr, &segment_index, &segment_header.index_offset); \
status = PutSegmentHeader(info_ptr, &segment_header, &attributes.facility_offset[SEGMENTED_RECORD_FACILITY]); \
if (update_attributes) { \
  status = TreePutExtendedAttributes(info_ptr, &attributes, &attributes_offset); \
  SeekToRfa(attributes_offset, local_nci.DATA_INFO.DATA_LOCATION.rfa); \
  local_nci.flags2 |= NciM_EXTENDED_NCI; \
} \
if (((int64_t) local_nci.length + (int64_t) add_length) < (2 ^ 31)) \
  local_nci.length += add_length; \
else \
  local_nci.length = (2 ^ 31); \
local_nci.flags=local_nci.flags | NciM_SEGMENTED; \
TreePutNci(info_ptr, nidx, &local_nci, 0);
return status;
}

#define BEGIN_LOCAL_NCI \
local_nci.flags2 &= ~NciM_DATA_IN_ATT_BLOCK; \
local_nci.dtype = initialValue->dtype; \
local_nci.class = CLASS_R; \
local_nci.time_inserted = TreeTimeInserted(); \
SAVED_UIC \
local_nci.owner_identifier = saved_uic;
/* See if node is currently using the Extended Nci feature and if so get the current contents of the attributes
 * index. If not, make an empty index and flag that a new index needs to be written.
 */
#define IF_NO_EXTENDED_NCI \
if (!(local_nci.flags2 & NciM_EXTENDED_NCI) \
 || IS_NOT_OK(TreeGetExtendedAttributes(info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes)))

#define CHECK_EXTENDED_NCI(UNLOCK) IF_NO_EXTENDED_NCI RETURN(UNLOCK,TreeFAILURE);

#define BEGIN_EXTENDED_NCI \
int update_attributes = 0; \
int64_t attributes_offset = -1; \
IF_NO_EXTENDED_NCI { \
  memset(&attributes, -1, sizeof(attributes)); \
  update_attributes = 1; \
} else { \
  attributes_offset = RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa); \
  if (attributes.facility_offset[STANDARD_RECORD_FACILITY] != -1) { \
    attributes.facility_offset[STANDARD_RECORD_FACILITY] = -1; \
    attributes.facility_length[STANDARD_RECORD_FACILITY] = 0; \
    update_attributes = 1; \
  } \
}

/* See if the node currently has an segment header record.
 * If not, make an empty segment header and flag that a new one needs to be written.
 */
#define IF_NO_SEGMENT_HEADER \
SEGMENT_HEADER segment_header; \
if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY] == -1 \
 || IS_NOT_OK(GetSegmentHeader(info_ptr, attributes.facility_offset[SEGMENTED_RECORD_FACILITY],&segment_header)))

#define CHECK_SEGMENT_HEADER(UNLOCK) IF_NO_SEGMENT_HEADER RETURN(UNLOCK,TreeFAILURE)

#define BEGIN_SEGMENT_HEADER \
IF_NO_SEGMENT_HEADER { \
  memset(&segment_header, 0, sizeof(segment_header)); \
  attributes.facility_offset[SEGMENTED_RECORD_FACILITY] = -1; \
  segment_header.index_offset = -1; \
  segment_header.idx = -1; \
  update_attributes = 1; \
} else if (initialValue->dtype != segment_header.dtype || \
          (initialValue->class == CLASS_A && \
          (initialValue->dimct != segment_header.dimct || \
          (initialValue->dimct > 1 \
           && memcmp(segment_header.dims, a_coeff->m, (initialValue->dimct - 1) * sizeof(int)))))) \
  RETURN(UNLOCK_NCI,TreeFAILURE);


/* See if the node currently has an segment_index record.
 * If not, make an empty segment index and flag that a new one needs to be written.
 */
 #define IF_NO_SEGMENT_INDEX \
SEGMENT_INDEX segment_index; \
if ((segment_header.index_offset == -1) \
 || IS_NOT_OK(GetSegmentIndex(info_ptr, segment_header.index_offset, &segment_index)))

#define CHECK_SEGMENT_INDEX(UNLOCK) IF_NO_SEGMENT_INDEX RETURN(UNLOCK,TreeFAILURE)

#define BEGIN_SEGMENT_INDEX \
IF_NO_SEGMENT_INDEX { \
  segment_header.index_offset = -1; \
  memset(&segment_index, -1, sizeof(segment_index)); \
  segment_index.first_idx = 0; \
}

inline static int checkSegmentInfo(UNLOCK) {
SEGMENT_INFO *sinfo;
if (idx < 0 || idx > segment_header.idx)
  RETURN(UNLOCK,TreeFAILURE);
int64_t index_offset;
for (index_offset = segment_header.index_offset;
     STATUS_OK && idx < segment_index.first_idx && segment_index.previous_offset > 0;
     index_offset = segment_index.previous_offset)
  status = GetSegmentIndex(info_ptr, segment_index.previous_offset, &segment_index);
if (STATUS_NOT_OK || (idx < segment_index.first_idx))
  RETURN(UNLOCK,TreeFAILURE);
sinfo = &segment_index.segment[idx % SEGMENTS_PER_INDEX];

#define BEGIN_SINFO(CHECKCOMPRESS)
SEGMENT_INFO *sinfo;
int add_length = 0;
if (idx == -1) {
  segment_header.idx++;
  idx = segment_header.idx;
  add_length = (initialValue->class == CLASS_A) ? initialValue->arsize : 0;
} else if (idx < -1 || idx > segment_header.idx)
  RETURN(UNLOCK_NCI,TreeBUFFEROVF)
else {
  /* TODO: Add support for updating an existing segment. add_length=new_length-old_length. */
  /* Potentially use same storage area if old array is same size. */
  printf("this is not yet supported\n");
  RETURN(UNLOCK_NCI,TreeFAILURE);
}
segment_header.data_offset = -1;
segment_header.dim_offset = -1;
segment_header.dtype = initialValue->dtype;
segment_header.dimct = (initialValue->class == CLASS_A) ? initialValue->dimct : 0;
segment_header.length = (initialValue->class == CLASS_A) ? initialValue->length : 0;
int previous_length = -1;
if (segment_header.idx > 0 && segment_header.length != 0) {
  int d;
  previous_length = segment_header.length;
  for (d = 0; d < segment_header.dimct; d++)
  previous_length *= segment_header.dims[d];
} else previous_length = -1;
if (initialValue->class == CLASS_A) {
  if (initialValue->dimct == 1)
    segment_header.dims[0] = initialValue->arsize / initialValue->length;
  else
    memcpy(segment_header.dims, a_coeff->m, initialValue->dimct * sizeof(int));
}
segment_header.next_row = rows_filled;
/* If not the first segment, see if we can reuse the previous segment storage space and compress the previous segment. */
if (((segment_header.idx % SEGMENTS_PER_INDEX) > 0) &&
    (previous_length == add_length) && compress) {
  int deleted;
  EMPTYXD(xd_data);
  EMPTYXD(xd_dim);
  sinfo = &segment_index.segment[(idx % SEGMENTS_PER_INDEX) - 1];
  TreeUnLockNci(info_ptr, 0, nidx);
  status = _TreeGetSegment(dbid, nid, idx - 1, &xd_data, &xd_dim);
  TreeLockNci(info_ptr, 0, nidx, &deleted);
  if STATUS_OK
    CHECKCOMPRESS;
  MdsFree1Dx(&xd_data, 0);
  MdsFree1Dx(&xd_dim, 0);
}
if (idx >= segment_index.first_idx + SEGMENTS_PER_INDEX) {
  memset(&segment_index, -1, sizeof(segment_index));
  segment_index.previous_offset = segment_header.index_offset;
  segment_header.index_offset = -1;
    segment_index.first_idx = idx;
  }
  sinfo = &segment_index.segment[idx % SEGMENTS_PER_INDEX];
}

void putData(SEGMENT_INFO *sinfo, SEGMENT_HEADER segment_header){
  sinfo->data_offset = segment_header.data_offset;
  sinfo->rows = segment_header.dims[segment_header.dimct - 1];
}

int putDataInitValue(SEGMENT_INFO *sinfo, ) {
  if (initialValue->dtype == DTYPE_OPAQUE) {
    int length;
    status = TreePutDsc(info_ptr, nid, (struct descriptor *)initialValue, &sinfo->data_offset, &length, compress);
    segment_header.data_offset = sinfo->data_offset;
    add_length = length;
    sinfo->rows = length | 0x80000000;
  } else
    status = PutInitialValue(info_ptr, segment_header.dims, initialValue, &segment_header.data_offset);
  if (initialValue->dtype != DTYPE_OPAQUE) {
    sinfo->data_offset = segment_header.data_offset;
    sinfo->rows = segment_header.dims[segment_header.dimct - 1];
  }
}

#define PUTLIMITBYDSC(limit,sinfo_limit,sinfo_limit_offset,sinfo_limit_length) \
for (dsc = limit; dsc && dsc->pointer && dsc->dtype == DTYPE_DSC; \
  dsc = (struct descriptor *)dsc->pointer); \
if (dsc && dsc->pointer && (dsc->dtype == DTYPE_Q || dsc->dtype == DTYPE_QU)) { \
  sinfo_limit = *(int64_t *) dsc->pointer; \
  sinfo_limit_offset = -1; \
  sinfo_limit_length = 0; \
} else if (limit) { \
  sinfo_limit = -1; \
  status = TreePutDsc(info_ptr, nid, limit, &sinfo_limit_offset, &sinfo_limit_length, compress); \
}
#define PUTDIM_DIM \
struct descriptor *dsc; \
PUTLIMITBYDSC(start,sinfo->start,sinfo->start_offset,sinfo->start_length); \
PUTLIMITBYDSC(end  ,sinfo->end  ,sinfo->end_offset  ,sinfo->end_length  ); \
if (dimension) \
  status = TreePutDsc(info_ptr, nid, dimension, &sinfo->dimension_offset, &sinfo->dimension_length, compress);

#define PUTDIM_TS \
status = PutDimensionValue(info_ptr, timestamps, rows_filled, initialValue->dimct, segment_header.dims, &segment_header.dim_offset); \
sinfo->start = rows_filled > 0 ? timestamps[0] : 0; \
sinfo->end = rows_filled > 0 ? timestamps[rows_filled - 1] : 0; \
sinfo->dimension_offset = segment_header.dim_offset; \
sinfo->dimension_length = 0;

#define CHECKCOMPRESS_DIM { \
int length; \
segment_header.data_offset = sinfo->data_offset; \
status = TreePutDsc(info_ptr, nid, xd_data.pointer, &sinfo->data_offset, &length, compress); \
/*** flag compressed segment by setting high bit in the rows field. ***/ \
sinfo->rows = length | 0x80000000; \
}

#define CHECKCOMPRESS_TS { \
  int length; \
  A_COEFF_TYPE *data_a = (A_COEFF_TYPE *) xd_data.pointer; \
  A_COEFF_TYPE *dim_a = (A_COEFF_TYPE *) xd_dim.pointer; \
  int rows = (initialValue->dimct == 1) ? \
             initialValue->arsize / initialValue->length : \
             ((A_COEFF_TYPE *) initialValue)->m[initialValue->dimct - 1]; \
  if (data_a && data_a->class == CLASS_A && data_a->pointer \
      && data_a->arsize >= initialValue->arsize && dim_a && dim_a->class == CLASS_A \
      && dim_a->pointer && dim_a->arsize >= (rows * sizeof(int64_t)) && dim_a->dimct == 1 \
      && dim_a->length == sizeof(int64_t) && dim_a->dtype == DTYPE_Q) { \
    segment_header.data_offset = sinfo->data_offset; \
    segment_header.dim_offset = sinfo->dimension_offset; \
    status = TreePutDsc(info_ptr, nid, xd_data.pointer, &sinfo->data_offset, &length, compress); \
    status = TreePutDsc(info_ptr, nid, xd_dim.pointer, &sinfo->dimension_offset,&sinfo->dimension_length, compress); \
    sinfo->start = ((int64_t *) dim_a->pointer)[0]; \
    sinfo->end = ((int64_t *) dim_a->pointer)[(dim_a->arsize / dim_a->length) - 1]; \
     /*** flag compressed segment by setting high bit in the rows field. ***/ \
    sinfo->rows = length | 0x80000000; \
  } else { \
    if (!data_a) \
      printf("data_a null\n"); \
    else if (data_a->class != CLASS_A) \
      printf("data_a is not CLASS_A, class=%d\n", data_a->class); \
    else if (!data_a->pointer) \
      printf("data_a's pointer is null\n"); \
    else if (data_a->arsize < initialValue->arsize) \
      printf("data_a->arsize (%d) < initialValue->arsize (%d)\n", data_a->arsize, \
           initialValue->arsize); \
    else if (!dim_a) \
      printf("dim_a null\n"); \
    else if (dim_a->class != CLASS_A) \
      printf("dim_a is not CLASS_A, class=%d\n", dim_a->class); \
    else if (!dim_a->pointer) \
      printf("dim_a's pointer is null\n"); \
    else if (dim_a->arsize < (rows * sizeof(int64_t))) \
      printf("dim_a->arsize (%d) < (rows (%d) * sizeof(int64_t))", dim_a->arsize, rows); \
    else if (dim_a->dimct != 1) \
      printf("dim_a->dimct (%d) != 1\n", dim_a->dimct); \
    else if (dim_a->length != sizeof(int64_t)) \
      printf("dim_a->length (%d) != sizeof(int64_t)\n", dim_a->length); \
    else if (dim_a->dtype != DTYPE_Q) \
      printf("dim_a->dtype (%d) != DTYPE_Q\n", dim_a->dtype); \
  } \
}
//"

#define COMPRESS \
int compress = (local_nci.flags & NciM_COMPRESS_ON_PUT) \
            && (local_nci.flags & NciM_COMPRESS_SEGMENTS) \
            &&!(local_nci.flags & NciM_DO_NOT_COMPRESS);

#define BEGIN_CHECKINPUT \
A_COEFF_TYPE *a_coeff = (A_COEFF_TYPE *) initialValue; \
while (initialValue && initialValue->dtype == DTYPE_DSC) \
initialValue = (struct descriptor_a *)initialValue->pointer; \
if (initialValue == NULL || \
   !((initialValue->class == CLASS_R && initialValue->dtype == DTYPE_OPAQUE) || \
    (initialValue->class == CLASS_A && (initialValue->dimct > 0 || initialValue->dimct <= 8))) ) \
  return TreeINVDTPUSG;

#define WRITESEGMENT(CHECKINPUT,SETUP_LOCAL_NCI,SETUP_EXTENDED_NCI,SETUP_SEGMENT_HEADER,SETUP_SEGMENT_INDEX,SETUP_SINFO,PUTDATA,PUTDIM,FINISH) \
OPEN_DATAFILE_WRITE(CHECKINPUT); \
COMPRESS; \
SETUP_LOCAL_NCI; \
SETUP_EXTENDED_NCI; \
SETUP_SEGMENT_HEADER; \
SETUP_SEGMENT_INDEX; \
SETUP_SINFO; \
PUTDATA; \
PUTDIM; \
FINISH; \
RETURN(UNLOCK_NCI,status);

#define PUTSEG_CHECKDATA \
DESCRIPTOR_A(data_a, 0, 0, 0, 0); \
while (data && data->dtype == DTYPE_DSC) \
  data = (struct descriptor_a *)data->pointer; \
if (data->class == CLASS_S) { \
  data_a.pointer = data->pointer; \
  data_a.length = data->length; \
  data_a.class = CLASS_A; \
  data_a.dtype = data->dtype; \
  data_a.arsize = data->length; \
  data_a.dimct = 1; \
  data = (struct descriptor_a *)&data_a; \
} \
A_COEFF_TYPE *a_coeff = (A_COEFF_TYPE *) data; \
if (data == NULL || data->class != CLASS_A || data->dimct < 1 || data->dimct > 8) \
  return TreeINVDTPUSG;

#define PUTSEG_PUTDATA \
ALLOCATE_BUFFER(bytes_to_insert,buffer) \
CHECK_ENDIAN_TRANSFER(data->pointer,bytes_to_insert,segment_header.length,data->dtype,buffer) \
TreeLockDatafile(info_ptr, 0, offset); \
MDS_IO_LSEEK(info_ptr->data_file->put, offset, SEEK_SET); \
status = (MDS_IO_WRITE(info_ptr->data_file->put, buffer, bytes_to_insert) == (ssize_t)bytes_to_insert) ? TreeSUCCESS : TreeFAILURE; \
FREE_BUFFER(buffer); \
TreeUnLockDatafile(info_ptr, 0, offset);

#define CHECK_DATA_DIMCT(UNLOCK) \
if (data->dtype != segment_header.dtype \
|| (data->dimct != segment_header.dimct && data->dimct != segment_header.dimct - 1) \
|| (data->dimct > 1 && memcmp(segment_header.dims, a_coeff->m, (data->dimct - 1) * sizeof(int)))) \
  RETURN(UNLOCK,TreeFAILURE)

#define CHECK_STARTIDX(UNLOCK) \
if (startIdx == -1) \
  startIdx = segment_header.next_row; \
else if (startIdx < -1 || startIdx > segment_header.dims[segment_header.dimct - 1]) { \
  UNLOCK; \
  return TreeBUFFEROVF; \
}

#define OPEN_TREE_READ \
INIT_TREESUCCESS; \
PINO_DATABASE *dblist = (PINO_DATABASE *) dbid; \
NID *nid_ptr = (NID *) & nid; \
TREE_INFO *info_ptr; \
int nidx; \
NODE *node_ptr; \
if (!(IS_OPEN(dblist))) \
  return TreeNOT_OPEN;

#define OPEN_DATAFILE_READ \
OPEN_TREE_READ; \
NODE_PTR; \
SEGMENTREMOTE; \
INFO_PTR; \
NCI local_nci; \
int64_t saved_viewdate; \
EXTENDED_ATTRIBUTES attributes; \
TreeGetViewDate(&saved_viewdate); \
status = TreeGetNciW(info_ptr, nidx, &local_nci, 0); \
if STATUS_NOT_OK \
  return status; \
if (info_ptr->data_file == 0){ \
  status = TreeOpenDatafileR(info_ptr); \
  if STATUS_NOT_OK \
    return status; \
}

#define OPEN_HEADER_READ \
OPEN_DATAFILE_READ \
IF_NO_EXTENDED_NCI   return TreeFAILURE; \
IF_NO_SEGMENT_HEADER return TreeFAILURE;

#define OPEN_INDEX_READ \
OPEN_HEADER_READ; \
IF_NO_SEGMENT_INDEX return TreeFAILURE;

#define GETSEGMENTTIMES \
OPEN_INDEX_READ \
int numsegs = segment_header.idx + 1; \
int idx; \
SEGMENT_INFO *sinfo; \
*nsegs = numsegs;

#define READPROPERTY(OFFSET,BUFFER,LENGTH) \
int deleted = B_TRUE; \
while STATUS_OK { \
  status = (MDS_IO_READ_X(info_ptr->data_file->get,OFFSET,BUFFER,LENGTH, &deleted) == LENGTH) ? TreeSUCCESS : TreeFAILURE; \
  if (STATUS_OK && deleted) \
    status = TreeReopenDatafile(info_ptr); \
  else break; \
}

#define READPROPERTY_SAFE(OFFSET,BUFFER,LENGTH) \
if (offset > -1) { \
  status = 1; \
  READPROPERTY(OFFSET,BUFFER,LENGTH) \
} else \
  status = TreeFAILURE

#define GETSEGMENTTIME_LOOP(startval,endval,GETLIMIT,start_xd,COPYSTART,end_xd,COPYEND) {\
  int index_idx = idx % SEGMENTS_PER_INDEX; \
  sinfo = &segment_index.segment[index_idx]; \
  if (sinfo->dimension_offset != -1 && sinfo->dimension_length == 0) { \
    /* It's a timestamped segment */ \
    if (sinfo->rows < 0 || !(sinfo->start == 0 && sinfo->end == 0)) { \
      /* Valid start and end in segment info */ \
      startval = sinfo->start; \
      endval = sinfo->end; \
    } else { \
      /* Current segment so use timestmps in segment */ \
      int length = sizeof(int64_t) * sinfo->rows; \
      char *buffer = malloc(length), *bptr; \
      int64_t timestamp; \
      READPROPERTY(sinfo->dimension_offset, buffer, length); \
      if STATUS_OK { \
        startval = swapquad(buffer); \
        for (bptr = buffer + length - sizeof(int64_t) ; (bptr > buffer) && (timestamp = swapquad(bptr))==0 ; bptr -= sizeof(int64_t)) ;\
        endval = bptr > buffer ? timestamp : 0; \
      } else { \
        startval = 0; \
        endval = 0; \
      } \
      free(buffer); \
    } \
    COPYSTART; \
    COPYEND; \
  } else { \
    GETLIMIT(startval,sinfo->start,sinfo->start_offset,sinfo->start_length, start_xd, COPYSTART) \
    GETLIMIT(endval  ,sinfo->end,  sinfo->end_offset,  sinfo->end_length,   end_xd  , COPYEND  ) \
  } \
  if (index_idx == 0 && idx > 0) \
    status = GetSegmentIndex(info_ptr, segment_index.previous_offset, &segment_index); \
}

#define GETLIMIT_ARRAY(limitval,sinfo_limit,sinfo_limit_offset,sinfo_limit_length,limit_xd,COPYLIMIT) \
if (sinfo_limit != -1) \
  limitval = sinfo_limit; \
else if (sinfo_limit_offset > 0 && sinfo_limit_length > 0) { \
  EMPTYXD(xd); \
  status = TreeGetDsc(info_ptr, nid, sinfo_limit_offset, sinfo_limit_length, &xd); \
  if (STATUS_OK && xd.pointer && xd.pointer->length == 8) \
    limitval = *(uint64_t *) xd.pointer->pointer; \
  else \
    limitval = 0; \
  MdsFree1Dx(&xd, 0); \
} else \
  limitval = 0;

#define GETLIMIT_XD(limitval,sinfo_limit,sinfo_limit_offset,sinfo_limit_length,limit_xd,COPYLIMIT) \
if (sinfo_limit != -1) { \
  limitval = sinfo_limit; \
  COPYLIMIT; \
} else if (sinfo_limit_offset > 0 && sinfo_limit_length > 0) \
  status = TreeGetDsc(info_ptr, nid, sinfo_limit_offset, sinfo_limit_length, limit_xd);

static int __TreeBeginSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end,
                              struct descriptor *dimension,
                              struct descriptor_a *initialValue, int idx, int rows_filled){
//WRITESEGMENT(CHECKINPUT,SETUP_LOCAL_NCI,SETUP_EXTENDED_NCI,SETUP_SEGMENT_HEADER,SETUP_SEGMENT_INDEX,SETUP_SINFO,PUTDATA,FINISH)
  WRITESEGMENT(BEGIN_CHECKINPUT,
               BEGIN_LOCAL_NCI,
               BEGIN_EXTENDED_NCI,
               BEGIN_SEGMENT_HEADER,
               BEGIN_SEGMENT_INDEX,
               BEGIN_SINFO(CHECKCOMPRESS_DIM),
               PUTDATA_INITVALUE,
               PUTDIM_DIM,
               BEGIN_FINISH);
}

static int __TreeBeginTimestampedSegment(void *dbid, int nid, int64_t * timestamps,
                                         struct descriptor_a *initialValue, int idx,
                                         int rows_filled){
//WRITESEGMENT(CHECKINPUT,SETUP_LOCAL_NCI,SETUP_EXTENDED_NCI,SETUP_SEGMENT_HEADER,SETUP_SEGMENT_INDEX,SETUP_SINFO,PUTDATA,PUTDIM,FINISH)
  WRITESEGMENT(BEGIN_CHECKINPUT,
               BEGIN_LOCAL_NCI,
               BEGIN_EXTENDED_NCI,
               BEGIN_SEGMENT_HEADER,
               BEGIN_SEGMENT_INDEX,
               BEGIN_SINFO(CHECKCOMPRESS_TS),
               PUTDATA_INITVALUE,
               PUTDIM_TS,
               BEGIN_FINISH);
}

int _TreeUpdateSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end,
                       struct descriptor *dimension, int idx)
{
//WRITESEGMENT(CHECKINPUT,SETUP_LOCAL_NCI,SETUP_EXTENDED_NCI,SETUP_SEGMENT_HEADER,SETUP_SEGMENT_INDEX,SETUP_SINFO,PUTDATA,FINISH)
  WRITESEGMENT(,
               ,
               CHECK_EXTENDED_NCI(UNLOCK_NCI),
               CHECK_SEGMENT_HEADER(UNLOCK_NCI),
               CHECK_SEGMENT_INDEX(UNLOCK_NCI),
               CHECK_SINFO(UNLOCK_NCI),
               PUTDATA,
               PUTDIM_DIM,
               UPDATE_FINISH);
}

int _TreePutSegment(void *dbid, int nid, int startIdx, struct descriptor_a *data)
{
  OPEN_DATAFILE_WRITE(PUTSEG_CHECKDATA);
  CHECK_EXTENDED_NCI(UNLOCK_NCI);
  CHECK_SEGMENT_HEADER(UNLOCK_NCI);
  CHECK_DATA_DIMCT(UNLOCK_NCI);
  CHECK_STARTIDX(UNLOCK_NCI);
  int rows_to_insert;
  int bytes_per_row,i;
  for (bytes_per_row = segment_header.length, i = 0; i < segment_header.dimct - 1;
       bytes_per_row *= segment_header.dims[i], i++) ;
  if (data->dimct < segment_header.dimct) {
    rows_to_insert = 1;
  } else {
    rows_to_insert = (data->dimct == 1) ? data->arsize / data->length : a_coeff->m[a_coeff->dimct - 1];
  }
  int rows_in_segment = segment_header.dims[segment_header.dimct - 1];
  unsigned int bytes_to_insert = ((rows_to_insert > (rows_in_segment - startIdx)) ? (rows_in_segment - startIdx) : rows_to_insert) * bytes_per_row;
  if (bytes_to_insert < data->arsize)
    RETURN(UNLOCK_NCI,TreeBUFFEROVF);
  int64_t offset = segment_header.data_offset + startIdx * bytes_per_row;
  PUTSEG_PUTDATA;
  if (startIdx == segment_header.next_row)
    segment_header.next_row += bytes_to_insert / bytes_per_row;
  if STATUS_OK
    status = PutSegmentHeader(info_ptr, &segment_header, &attributes.facility_offset[SEGMENTED_RECORD_FACILITY]);
  RETURN(UNLOCK_NCI,status);
}

int _TreeGetSegmentTimesXd(void *dbid, int nid, int *nsegs, struct descriptor_xd *start_list, struct descriptor_xd *end_list){
  GETSEGMENTTIMES;
  DESCRIPTOR_APD(start_apd, DTYPE_LIST, malloc(numsegs * sizeof(void *)), numsegs);
  DESCRIPTOR_APD(end_apd, DTYPE_LIST, malloc(numsegs * sizeof(void *)), numsegs);
  struct descriptor_xd **start_xds = (struct descriptor_xd **)start_apd.pointer;
  struct descriptor_xd **end_xds = (struct descriptor_xd **)end_apd.pointer;
  EMPTYXD(xd);
  int64_t startval;
  int64_t endval;
  struct descriptor startdsc = {sizeof(int64_t), DTYPE_Q, CLASS_S, (char *)&startval};
  struct descriptor enddsc = {sizeof(int64_t), DTYPE_Q, CLASS_S, (char *)&endval};
  for (idx = numsegs - 1; STATUS_OK && idx >= 0; idx--) {
    start_xds[idx]= memcpy(malloc(sizeof(struct descriptor_xd)),&xd, sizeof(struct descriptor_xd));
    end_xds[idx]  = memcpy(malloc(sizeof(struct descriptor_xd)),&xd, sizeof(struct descriptor_xd));
    struct descriptor_xd *start_xd = ((struct descriptor_xd **)start_apd.pointer)[idx];
    struct descriptor_xd *end_xd   = ((struct descriptor_xd **)end_apd.pointer)[idx];
    GETSEGMENTTIME_LOOP(startval,endval,GETLIMIT_XD,start_xd,MdsCopyDxXd(&startdsc,start_xd),end_xd,MdsCopyDxXd(&enddsc,end_xd));
  }
  MdsCopyDxXd((struct descriptor *)&start_apd,start_list);
  MdsCopyDxXd((struct descriptor *)&end_apd,end_list);
  for (idx=0;idx<numsegs;idx++) {
    MdsFree1Dx(start_xds[idx],0);
    free(start_xds[idx]);
    MdsFree1Dx(end_xds[idx],0);
    free(end_xds[idx]);
  }
  free(start_apd.pointer);
  free(end_apd.pointer);
  return status;
}

int _TreeGetSegmentTimes(void *dbid, int nid, int *nsegs, uint64_t ** times){
  *times = NULL;
  GETSEGMENTTIMES;
  uint64_t *ans = (uint64_t *) malloc(numsegs * 2 * sizeof(uint64_t));
  *times = ans;
  memset(ans, 0, numsegs * 2 * sizeof(uint64_t));
  for (idx = numsegs - 1; STATUS_OK && idx >= 0; idx--)
    GETSEGMENTTIME_LOOP(ans[idx * 2],ans[idx * 2 + 1],GETLIMIT_ARRAY,,,,);
  return status;
}

int _TreeGetNumSegments(void *dbid, int nid, int *num){
  *num = 0;
  OPEN_DATAFILE_READ;
  IF_NO_EXTENDED_NCI   return status;
  IF_NO_SEGMENT_HEADER return status;
  *num = segment_header.idx + 1;
  return status;
}

static int ReadSegment(TREE_INFO * info_ptr, int nid, SEGMENT_HEADER * segment_header,
                       SEGMENT_INFO * sinfo, struct descriptor_xd *segment,
                       struct descriptor_xd *dim)
{
  INIT_TREESUCCESS;
  if (sinfo->data_offset != -1) {
    int compressed_segment = 0;
    DESCRIPTOR_A(dim2, 8, DTYPE_Q, 0, 0);
    DESCRIPTOR_A_COEFF(ans, 0, 0, 0, 8, 0);
    ans.pointer = 0;
    ans.dtype = segment_header->dtype;
    ans.length = segment_header->length;
    ans.dimct = segment_header->dimct;
    memcpy(ans.m, segment_header->dims, sizeof(segment_header->dims));
    ans.m[segment_header->dimct - 1] = sinfo->rows;
    ans.arsize = ans.length;
    int i;
    for (i = 0; i < ans.dimct; i++)
      ans.arsize *= ans.m[i];
    if (sinfo->rows < 0) {
      EMPTYXD(compressed_segment_xd);
      int data_length = sinfo->rows & 0x7fffffff;
      compressed_segment = 1;
      status = TreeGetDsc(info_ptr, nid, sinfo->data_offset, data_length, &compressed_segment_xd);
      if STATUS_OK {
        status = MdsDecompress((struct descriptor_r *)compressed_segment_xd.pointer, segment);
        MdsFree1Dx(&compressed_segment_xd, 0);
      }
    } else {
      ans.pointer = malloc(ans.arsize);
      READPROPERTY(sinfo->data_offset, ans.pointer, (ssize_t)ans.arsize)
    }
    if STATUS_OK {
      if (!compressed_segment)
        CHECK_ENDIAN(ans.pointer,ans.arsize,ans.length,ans.dtype);
      if (sinfo->dimension_offset != -1 && sinfo->dimension_length == 0) {
        int64_t *tp;
        dim2.arsize = sinfo->rows * sizeof(int64_t);
        dim2.pointer = malloc(dim2.arsize);
        READPROPERTY(sinfo->dimension_offset, dim2.pointer, (ssize_t)dim2.arsize);
        CHECK_ENDIAN(dim2.pointer,dim2.arsize,8,);
        if (!compressed_segment) {
          /**** Remove trailing null timestamps from dimension and data ****/
          for (tp = (int64_t *) (dim2.pointer + dim2.arsize - dim2.length);
               (tp >= (int64_t *) dim2.pointer) && (*tp == 0); tp--) {
            ans.m[segment_header->dimct - 1]--;
            dim2.arsize -= sizeof(int64_t);
          }
          ans.arsize = ans.length;
          for (i = 0; i < ans.dimct; i++)
            ans.arsize *= ans.m[i];
          /**** end remove null timestamps ***/
        }
        MdsCopyDxXd((struct descriptor *)&dim2, dim);
        free(dim2.pointer);
      } else {
        TreeGetDsc(info_ptr, nid, sinfo->dimension_offset, sinfo->dimension_length, dim);
      }
      if (!compressed_segment) {
        MdsCopyDxXd((struct descriptor *)&ans, segment);
      }
    } else {
      status = TreeFAILURE;
    }
    if (!compressed_segment)
      free(ans.pointer);
  } else {
    status = TreeFAILURE;
  }
  return status;
}

static int getSegmentLimits(TREE_INFO * info_ptr, int nid,
                            SEGMENT_INFO * sinfo,
                            struct descriptor_xd *retStart, struct descriptor_xd *retEnd){
  INIT_TREESUCCESS;
  struct descriptor q_d = { 8, DTYPE_Q, CLASS_S, 0 };
  if (sinfo->dimension_offset != -1 && sinfo->dimension_length == 0) {
                                                                     /*** timestamped segments ****/
    if (sinfo->rows < 0 || !(sinfo->start == 0 && sinfo->end == 0)) {
      q_d.pointer = (char *)&sinfo->start;
      MdsCopyDxXd(&q_d, retStart);
      q_d.pointer = (char *)&sinfo->end;
      MdsCopyDxXd(&q_d, retEnd);
    } else {
      int length = sizeof(int64_t) * sinfo->rows;
      char *buffer = malloc(length);
      int64_t timestamp;
      char *bptr;
      READPROPERTY(sinfo->dimension_offset, buffer, length);
      if STATUS_OK {
        q_d.pointer = (char *)&timestamp;
        timestamp = swapquad(buffer);
        MdsCopyDxXd(&q_d, retStart);
        for (bptr = buffer + length - sizeof(int64_t);
             bptr > buffer && ((timestamp = swapquad(bptr)) == 0); bptr -= sizeof(int64_t)) ;
        if (bptr > buffer) {
          MdsCopyDxXd(&q_d, retEnd);
        } else {
          MdsFree1Dx(retEnd, 0);
        }
      } else {
        MdsFree1Dx(retStart, 0);
        MdsFree1Dx(retEnd, 0);
      }
      free(buffer);
    }
  } else {
    if (sinfo->start != -1) {
      q_d.pointer = (char *)&sinfo->start;
      MdsCopyDxXd(&q_d, retStart);
    } else if (sinfo->start_length > 0 && sinfo->start_offset > 0) {
      status = TreeGetDsc(info_ptr, nid, sinfo->start_offset, sinfo->start_length, retStart);
    } else
      status = MdsFree1Dx(retStart, 0);
    if (sinfo->end != -1) {
      q_d.pointer = (char *)&sinfo->end;
      MdsCopyDxXd(&q_d, retEnd);
    } else if (sinfo->end_length > 0 && sinfo->end_offset > 0) {
      status = TreeGetDsc(info_ptr, nid, sinfo->end_offset, sinfo->end_length, retEnd);
    } else
      status = MdsFree1Dx(retEnd, 0);
  }
  return status;
}

int _TreeSetXNci(void *dbid, int nid, const char *xnciname, struct descriptor *value){
  OPEN_DATAFILE_WRITE(if (!xnciname || strlen(xnciname) < 1 || strlen(xnciname) > NAMED_ATTRIBUTE_NAME_SIZE)return TreeFAILURE;);
  COMPRESS;
  int64_t attributes_offset = -1;
  int update_attributes = 0;
  int found_index = -1;
  int64_t index_offset = -1;
  int value_length;
  int64_t value_offset;
  NAMED_ATTRIBUTES_INDEX index, current_index;
  status = TreePutDsc(info_ptr, nid, value, &value_offset, &value_length, compress);
  if STATUS_NOT_OK
    return status;
  /*** See if node is currently using the Extended Nci feature and if so get the current contents of the attributes
       index. If not, make an empty index and flag that a new index needs to be written.***/
  IF_NO_EXTENDED_NCI {
    memset(&attributes, -1, sizeof(attributes));
    update_attributes = 1;
    if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) && local_nci.length > 0) {
      if (local_nci.flags2 & NciM_DATA_IN_ATT_BLOCK) {
        EMPTYXD(dsc);
        struct descriptor *dptr;
        unsigned char dsc_dtype = DTYPE_DSC;
        int dlen = local_nci.length - 8;
        unsigned int ddlen = dlen + sizeof(struct descriptor);
        status = MdsGet1Dx(&ddlen, &dsc_dtype, &dsc, 0);
        dptr = dsc.pointer;
        dptr->length = dlen;
        dptr->dtype = local_nci.dtype;
        dptr->class = CLASS_S;
        dptr->pointer = (char *)dptr + sizeof(struct descriptor);
        memcpy(dptr->pointer, local_nci.DATA_INFO.DATA_IN_RECORD.data, dptr->length);
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
        TreePutDsc(info_ptr, nid, dptr, &attributes.facility_offset[STANDARD_RECORD_FACILITY],
                   &attributes.facility_length[STANDARD_RECORD_FACILITY], compress);
        local_nci.flags2 &= ~NciM_DATA_IN_ATT_BLOCK;
      } else {
        EMPTYXD(xd);
        int retsize;
        int nodenum;
        int length = local_nci.DATA_INFO.DATA_LOCATION.record_length;
        if (length > 0) {
          char *data = malloc(length);
          status =
              TreeGetDatafile(info_ptr, local_nci.DATA_INFO.DATA_LOCATION.rfa, &length, data,
                              &retsize, &nodenum, local_nci.flags2);
          if STATUS_NOT_OK
            status = TreeBADRECORD;
          else if (!(local_nci.flags2 & NciM_NON_VMS)
                   && ((retsize != length) || (nodenum != nidx)))
            status = TreeBADRECORD;
          else
            status = (MdsSerializeDscIn(data, &xd) & 1) ? TreeNORMAL : TreeBADRECORD;
          free(data);
          if STATUS_OK {
            status =
                TreePutDsc(info_ptr, nid, (struct descriptor *)&xd,
                           &attributes.facility_offset[STANDARD_RECORD_FACILITY],
                           &attributes.facility_length[STANDARD_RECORD_FACILITY], compress);
          }
          MdsFree1Dx(&xd, 0);
        }
        if (length <= 0 || STATUS_NOT_OK) {
          attributes.facility_offset[STANDARD_RECORD_FACILITY] = 0;
          attributes.facility_length[STANDARD_RECORD_FACILITY] = 0;
          local_nci.length = 0;
          local_nci.DATA_INFO.DATA_LOCATION.record_length = 0;
        }
      }
    }
  } else
    attributes_offset = RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa);
  /* See if the node currently has an named attributes header record.
   * If not, make an empty named attributes header and flag that a new one needs to be written.
   */
  if (attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY] == -1
   || IS_NOT_OK(GetNamedAttributesIndex(info_ptr, attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY], &index))) {
    memset(&index, 0, sizeof(index));
    attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY] = -1;
    index_offset = -1;
    index.previous_offset = -1;
    update_attributes = 1;
  } else
    index_offset = attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY];
  current_index = index;
  /*** See if the node currently has a value for this attribute. ***/
  while (index_offset != -1 && found_index == -1) {
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
      if ((GetNamedAttributesIndex(info_ptr, index.previous_offset, &index) & 1) == 0)
        break;
      index_offset = new_offset;
    } else
      break;
  }
  /*** If name exists just replace the value else find an empty slot ***/
  if (found_index != -1) {
    index.attribute[found_index].offset = value_offset;
    index.attribute[found_index].length = value_length;
  } else {
    int i;
    index = current_index;
    for (i = 0; i < NAMED_ATTRIBUTES_PER_INDEX; i++) {
      if (index.attribute[i].name[0] == 0) {
        strcpy(index.attribute[i].name, xnciname);
        index.attribute[i].offset = value_offset;
        index.attribute[i].length = value_length;
        index_offset = attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY];
        break;
      }
    }
    if (i == NAMED_ATTRIBUTES_PER_INDEX) {
      memset(&index, 0, sizeof(index));
      index.previous_offset = attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY];
      update_attributes = 1;
      strcpy(index.attribute[0].name, xnciname);
      index.attribute[0].offset = value_offset;
      index.attribute[0].length = value_length;
      index_offset = -1;
    }
  }
  status = PutNamedAttributesIndex(info_ptr, &index, &index_offset);
  if (STATUS_OK && update_attributes) {
    attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY] = index_offset;
    status = TreePutExtendedAttributes(info_ptr, &attributes, &attributes_offset);
    SeekToRfa(attributes_offset, local_nci.DATA_INFO.DATA_LOCATION.rfa);
    local_nci.flags2 |= NciM_EXTENDED_NCI;
    TreePutNci(info_ptr, nidx, &local_nci, 0);
  }
  RETURN(UNLOCK_NCI,status);
}

int _TreeGetXNci(void *dbid, int nid, const char *xnciname, struct descriptor_xd *value)
{
  if (!xnciname || strlen(xnciname) < 1 || strlen(xnciname) > NAMED_ATTRIBUTE_NAME_SIZE)
    return TreeFAILURE;
OPEN_DATAFILE_READ;
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
    if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) ||
        ((TreeGetExtendedAttributes
          (info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1) == 0)) {
      status = TreeFAILURE;
    } else if (attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY] == -1
           ||  IS_NOT_OK(GetNamedAttributesIndex(info_ptr, attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY],&index))) {
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
          if ((GetNamedAttributesIndex(info_ptr, index.previous_offset, &index) & 1) == 0) {
            break;
          }
        } else
          break;
      }
      if (found_index != -1) {
        status =
            TreeGetDsc(info_ptr, nid, index.attribute[found_index].offset,
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

int TreePutDsc(TREE_INFO * info_ptr, int nid_in, struct descriptor *dsc, int64_t * offset, int *length, int compress)
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
    TreeLockDatafile(info_ptr, 0, 0);
    *offset = MDS_IO_LSEEK(info_ptr->data_file->put, 0, SEEK_END);
    status = MDS_IO_WRITE(info_ptr->data_file->put, ap->pointer, ap->arsize)
          == (ssize_t)ap->arsize ? TreeSUCCESS : TreeFAILURE;
    TreeUnLockDatafile(info_ptr, 0, 0);
    *length = ap->arsize;
    MdsFree1Dx(&xd, 0);
  }
  return status;
}

int TreeGetDsc(TREE_INFO * info_ptr, int nid, int64_t offset, int length, struct descriptor_xd *dsc){
  INIT_TREESUCCESS;
  char *buffer = malloc(length);
  READPROPERTY_SAFE(offset, buffer, length);
  if STATUS_OK
    status = MdsSerializeDscIn(buffer, dsc);
  if (dsc->pointer)
    status = TreeMakeNidsLocal(dsc->pointer, nid);
  free(buffer);
  return status;
}

static int GetCompressedSegmentRows(TREE_INFO * info_ptr, int64_t offset, int *rows){
  INIT_TREESUCCESS;
  int length = 60;
  unsigned char buffer[60];
  READPROPERTY_SAFE(offset, buffer, length);
  if STATUS_OK {
    if (buffer[3] == CLASS_CA || buffer[3] == CLASS_A) {
      unsigned char dimct = buffer[11];
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

static int PutSegmentHeader(TREE_INFO * info_ptr, SEGMENT_HEADER * hdr, int64_t * offset){
  INIT_TREESUCCESS;
  int64_t loffset;
  int j;
  char *next_row_fix = getenv("NEXT_ROW_FIX");
  char buffer[2 * sizeof(char) + 1 * sizeof(short) + 10 * sizeof(int) +
                       3 * sizeof(int64_t)], *bptr = buffer;
  if (*offset == -1) {
    TreeLockDatafile(info_ptr, 0, 0);
    *offset = MDS_IO_LSEEK(info_ptr->data_file->put, 0, SEEK_END);
    loffset = 0;
  } else {
    TreeLockDatafile(info_ptr, 0, *offset);
    loffset = *offset;
    MDS_IO_LSEEK(info_ptr->data_file->put, *offset, SEEK_SET);
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
  status = MDS_IO_WRITE(info_ptr->data_file->put, buffer, sizeof(buffer))
        == sizeof(buffer) ? TreeSUCCESS : TreeFAILURE;
  TreeUnLockDatafile(info_ptr, 0, loffset);
  return status;
}

static int PutSegmentIndex(TREE_INFO * info_ptr, SEGMENT_INDEX * idx, int64_t * offset){
  INIT_TREESUCCESS;
  int i;
  int64_t loffset;
  char buffer[sizeof(int64_t) + sizeof(int) +
                       SEGMENTS_PER_INDEX * (6 * sizeof(int64_t) + 4 * sizeof(int))], *bptr =
      buffer;
  if (*offset == -1) {
    TreeLockDatafile(info_ptr, 0, 0);
    loffset = 0;
    *offset = MDS_IO_LSEEK(info_ptr->data_file->put, 0, SEEK_END);
  } else {
    TreeLockDatafile(info_ptr, 0, *offset);
    loffset = *offset;
    MDS_IO_LSEEK(info_ptr->data_file->put, *offset, SEEK_SET);
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
  status = MDS_IO_WRITE(info_ptr->data_file->put, buffer, sizeof(buffer))
        == sizeof(buffer) ? TreeSUCCESS : TreeFAILURE;
  TreeUnLockDatafile(info_ptr, 0, loffset);
  return status;
}

int TreePutExtendedAttributes(TREE_INFO * info_ptr, EXTENDED_ATTRIBUTES * att, int64_t * offset){
  INIT_TREESUCCESS;
  int i;
  int64_t loffset;
  char buffer[sizeof(int64_t) + FACILITIES_PER_EA * (sizeof(int64_t) + sizeof(int))],
      *bptr = buffer;
  if (*offset == -1) {
    TreeLockDatafile(info_ptr, 0, 0);
    loffset = 0;
    *offset = MDS_IO_LSEEK(info_ptr->data_file->put, 0, SEEK_END);
  } else {
    TreeLockDatafile(info_ptr, 0, *offset);
    loffset = *offset;
    MDS_IO_LSEEK(info_ptr->data_file->put, *offset, SEEK_SET);
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
  status = MDS_IO_WRITE(info_ptr->data_file->put, buffer, sizeof(buffer))
        == sizeof(buffer) ? TreeSUCCESS : TreeFAILURE;
  TreeUnLockDatafile(info_ptr, 0, loffset);
  return status;
}

static int PutNamedAttributesIndex(TREE_INFO * info_ptr, NAMED_ATTRIBUTES_INDEX * index,
                                   int64_t * offset){
  INIT_TREESUCCESS;
  int i;
  int64_t loffset;
  char buffer[sizeof(int64_t) + NAMED_ATTRIBUTES_PER_INDEX * (NAMED_ATTRIBUTE_NAME_SIZE + sizeof(int64_t) + sizeof(int))], *bptr = buffer;
  if (*offset == -1) {
    TreeLockDatafile(info_ptr, 0, 0);
    loffset = 0;
    *offset = MDS_IO_LSEEK(info_ptr->data_file->put, 0, SEEK_END);
  } else {
    TreeLockDatafile(info_ptr, 0, *offset);
    loffset = *offset;
    MDS_IO_LSEEK(info_ptr->data_file->put, *offset, SEEK_SET);
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
  status = MDS_IO_WRITE(info_ptr->data_file->put, buffer, sizeof(buffer))
        == sizeof(buffer) ? TreeSUCCESS : TreeFAILURE;
  TreeUnLockDatafile(info_ptr, 0, loffset);
  return status;
}

static int PutInitialValue(TREE_INFO * info_ptr, int *dims, struct descriptor_a *array,
                           int64_t * offset)
{
  int status;
  int length = array->length;
  int i;
  int64_t loffset;
  if (*offset == -1) {
    TreeLockDatafile(info_ptr, 0, 0);
    loffset = 0;
    *offset = MDS_IO_LSEEK(info_ptr->data_file->put, 0, SEEK_END);
  } else {
    TreeLockDatafile(info_ptr, 0, *offset);
    loffset = *offset;
    MDS_IO_LSEEK(info_ptr->data_file->put, *offset, SEEK_SET);
  }
  for (i = 0; i < array->dimct; i++)
    length = length * dims[i];
  ALLOCATE_BUFFER(length,buffer)
  CHECK_ENDIAN_TRANSFER(array->pointer,length,array->length,array->dtype,buffer)
  status = (MDS_IO_WRITE(info_ptr->data_file->put, buffer, length) == length) ? TreeSUCCESS : TreeFAILURE;
  FREE_BUFFER(buffer);
  TreeUnLockDatafile(info_ptr, 0, loffset);
  return status;
}

static int PutDimensionValue(TREE_INFO * info_ptr, int64_t * timestamps, int rows_filled, int ndims,
                             int *dims, int64_t * offset)
{
  INIT_TREESUCCESS;
  int length = sizeof(int64_t);
  int64_t loffset;
  unsigned char *buffer = 0;
  if (*offset == -1) {
    TreeLockDatafile(info_ptr, 0, 0);
    loffset = 0;
    *offset = MDS_IO_LSEEK(info_ptr->data_file->put, 0, SEEK_END);
  } else {
    TreeLockDatafile(info_ptr, 0, *offset);
    loffset = *offset;
    MDS_IO_LSEEK(info_ptr->data_file->put, *offset, SEEK_SET);
  }
  length = length * dims[ndims - 1] - (sizeof(int64_t) * rows_filled);
  if (length > 0) {
    buffer = malloc(length);
    memset(buffer, 0, length);
  }
  if (rows_filled > 0)
    status = MDS_IO_WRITE(info_ptr->data_file->put, timestamps, rows_filled * sizeof(int64_t))
          == (int)(rows_filled * sizeof(int64_t)) ? TreeSUCCESS : TreeFAILURE;
  if (STATUS_OK && length > 0)
    status = MDS_IO_WRITE(info_ptr->data_file->put, buffer, length) == length ? TreeSUCCESS : TreeFAILURE;
  if (buffer)
    free(buffer);
  TreeUnLockDatafile(info_ptr, 0, loffset);
  return status;
}

static int GetSegmentHeader(TREE_INFO * info_ptr, int64_t offset, SEGMENT_HEADER * hdr){
  INIT_TREESUCCESS;
  char buffer[2 * sizeof(char) + 1 * sizeof(short) + 10 * sizeof(int) + 3 * sizeof(int64_t)], *bptr;
  READPROPERTY_SAFE(offset, buffer, sizeof(buffer));
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

static int GetSegmentIndex(TREE_INFO * info_ptr, int64_t offset, SEGMENT_INDEX * idx){
  INIT_TREESUCCESS;
  char buffer[sizeof(int64_t) + sizeof(int) + SEGMENTS_PER_INDEX * (6 * sizeof(int64_t) + 4 * sizeof(int))], *bptr;
  READPROPERTY_SAFE(offset, buffer, sizeof(buffer));
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

int TreeGetExtendedAttributes(TREE_INFO * info_ptr, int64_t offset, EXTENDED_ATTRIBUTES * att){
  INIT_TREESUCCESS;
  char buffer[sizeof(int64_t) + FACILITIES_PER_EA * (sizeof(int64_t) + sizeof(int))], *bptr;
  READPROPERTY_SAFE(offset, buffer, sizeof(buffer));
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

static int GetNamedAttributesIndex(TREE_INFO * info_ptr, int64_t offset, NAMED_ATTRIBUTES_INDEX * index){
  INIT_TREESUCCESS;
  char buffer[sizeof(int64_t) + NAMED_ATTRIBUTES_PER_INDEX * (sizeof(int64_t) + sizeof(int) + NAMED_ATTRIBUTE_NAME_SIZE)], *bptr;
  READPROPERTY_SAFE(offset, buffer, sizeof(buffer));
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

#define PUTTSEG_CHECKDATA \
  DESCRIPTOR_A(data_a, 0, 0, 0, 0); \
  while (data && data->dtype == DTYPE_DSC) \
    data = (struct descriptor_a *)data->pointer; \
  if (data && data->class == CLASS_S) { \
    data_a.pointer = data->pointer; \
    data_a.length = data->length; \
    data_a.class = CLASS_A; \
    data_a.dtype = data->dtype; \
    data_a.arsize = data->length; \
    data_a.dimct = 1; \
    data = (struct descriptor_a *)&data_a; \
  } \
  A_COEFF_TYPE *a_coeff = (A_COEFF_TYPE *) data; \
  if (data == NULL || data->class != CLASS_A || data->dimct < 1 || data->dimct > 8) \
    return TreeINVDTPUSG;

int _TreePutTimestampedSegment(void *dbid, int nid, int64_t * timestamp, struct descriptor_a *data){
  OPEN_DATAFILE_WRITE(PUTTSEG_CHECKDATA);
  int startIdx;
  int rows_to_insert = -1;
  int bytes_per_row;
  int rows_in_segment;
  int bytes_to_insert;
  int64_t offset;
  int i;
  IF_NO_EXTENDED_NCI   RETURN(UNLOCK_NCI,TreeNOSEGMENTS);
  IF_NO_SEGMENT_HEADER RETURN(UNLOCK_NCI,TreeNOSEGMENTS);
  if (data->dtype != segment_header.dtype)
    RETURN(UNLOCK_NCI,TreeINVDTYPE);
  if ((a_coeff->dimct == 1)
   &&!(a_coeff->dimct == segment_header.dimct)
   &&!(a_coeff->dimct == segment_header.dimct - 1))
    RETURN(UNLOCK_NCI,TreeINVSHAPE);
  if (a_coeff->dimct > 1 && memcmp(segment_header.dims, a_coeff->m,(segment_header.dimct - 1) * sizeof(int)))
    RETURN(UNLOCK_NCI,TreeINVSHAPE);
  if (a_coeff->dimct == 1 && a_coeff->arsize / a_coeff->length != 1
   && (unsigned int)segment_header.dims[0] < a_coeff->arsize / a_coeff->length)
    RETURN(UNLOCK_NCI,TreeINVSHAPE);
  startIdx = segment_header.next_row;
  for (bytes_per_row = segment_header.length, i = 0; i < segment_header.dimct - 1;
       bytes_per_row *= segment_header.dims[i], i++) ;
  if (data->dimct < segment_header.dimct)
    rows_to_insert = 1;
  else
    rows_to_insert = (data->dimct == 1) ? data->arsize / data->length : a_coeff->m[a_coeff->dimct - 1];
  if (rows_to_insert <= 0)
    RETURN(UNLOCK_NCI,TreeINVSHAPE)
  rows_in_segment = segment_header.dims[segment_header.dimct - 1];
  bytes_to_insert = ((rows_to_insert > (rows_in_segment - startIdx)) ? (rows_in_segment - startIdx) : rows_to_insert) * bytes_per_row;
  if (bytes_to_insert < 1)
    RETURN(UNLOCK_NCI,TreeBUFFEROVF)
  offset = segment_header.data_offset + startIdx * bytes_per_row;
  ALLOCATE_BUFFER(bytes_to_insert,buffer);
  CHECK_ENDIAN_TRANSFER(data->pointer,bytes_to_insert,segment_header.length,data->dtype,buffer);
  TreeLockDatafile(info_ptr, 0, offset);
  MDS_IO_LSEEK(info_ptr->data_file->put, offset, SEEK_SET);
  status =(MDS_IO_WRITE(info_ptr->data_file->put, buffer, bytes_to_insert) == bytes_to_insert) ? TreeSUCCESS : TreeFAILURE;
  FREE_BUFFER(buffer);
  MDS_IO_LSEEK(info_ptr->data_file->put, segment_header.dim_offset + startIdx * sizeof(int64_t), SEEK_SET);
  ALLOCATE_BUFFER(rows_to_insert,times);
  CHECK_ENDIAN_TRANSFER(timestamp,rows_to_insert,8,,times);
  status = (MDS_IO_WRITE(info_ptr->data_file->put, times, sizeof(int64_t) * rows_to_insert) == (int)(sizeof(int64_t) * rows_to_insert)) ? TreeSUCCESS : TreeFAILURE;
  FREE_BUFFER(times);
  TreeUnLockDatafile(info_ptr, 0, offset);
  segment_header.next_row = startIdx + bytes_to_insert / bytes_per_row;
  if STATUS_OK
    status = PutSegmentHeader(info_ptr, &segment_header, &attributes.facility_offset[SEGMENTED_RECORD_FACILITY]);
  RETURN(UNLOCK_NCI,status);
}

static int CopyStandardRecord(TREE_INFO * info_in, TREE_INFO * info_out, int nid, int64_t * offset,
                              int *length, int compress){
  EMPTYXD(xd);
  int status;
  status = TreeGetDsc(info_in, nid, *offset, *length, &xd);
  if STATUS_OK
    status = TreePutDsc(info_out, nid, (struct descriptor *)&xd, offset, length, compress);
  MdsFree1Dx(&xd, 0);
  if STATUS_NOT_OK {
    *offset = -1;
    *length = 0;
  }
  return status;
}

static int CopyNamedAttributes(TREE_INFO * info_in, TREE_INFO * info_out, int nid, int64_t * offset,
                               int *length, int compress){
  EMPTYXD(xd);
  NAMED_ATTRIBUTES_INDEX index;
  int status = GetNamedAttributesIndex(info_in, *offset, &index);
  if STATUS_OK {
    int i;
    *length=0;
    if (index.previous_offset != -1)
      CopyNamedAttributes(info_in, info_out, nid, &index.previous_offset, 0, compress);
    for (i = 0; i < NAMED_ATTRIBUTES_PER_INDEX; i++) {
      if (index.attribute[i].name[0] != '\0' && index.attribute[i].offset != -1) {
        status = TreeGetDsc(info_in, nid, index.attribute[i].offset, index.attribute[i].length, &xd);
        if STATUS_OK {
          status = TreePutDsc(info_out, nid, (struct descriptor *)&xd,
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
    status = PutNamedAttributesIndex(info_out, &index, offset);
  } else
    *offset = -1;
  return status;
}

static int DataCopy(TREE_INFO * info_ptr, TREE_INFO * info_out, int64_t offset_in, int length_in,
                    int64_t * offset_out)
{
  INIT_TREESUCCESS;
  if (offset_in != -1 && length_in >= 0) {
    char *data = malloc(length_in);
    READPROPERTY(offset_in, data, length_in)
    if (STATUS_OK) {
      *offset_out = MDS_IO_LSEEK(info_out->data_file->put, 0, SEEK_END);
      status = MDS_IO_WRITE(info_out->data_file->put, data, length_in) == length_in;
    }
    free(data);
    if STATUS_NOT_OK
      *offset_out = -1;
  } else
    *offset_out = -1;
  return status;
}

static int CopySegment(TREE_INFO *info_in, TREE_INFO *info_out, int nid, SEGMENT_HEADER *header, SEGMENT_INFO *sinfo, int compress) {
  INIT_TREESUCCESS;
  if (compress) {
    int length;
    EMPTYXD(data_xd);
    EMPTYXD(dim_xd);
    status = ReadSegment(info_in, nid, header, sinfo, &data_xd, &dim_xd);
    if STATUS_OK {
      status = TreePutDsc(info_out, nid, data_xd.pointer, &sinfo->data_offset, &length, compress);
      if STATUS_OK {
        sinfo->rows = length + 0x80000000;
        status = TreePutDsc(info_out, nid, dim_xd.pointer, &sinfo->dimension_offset, &sinfo->dimension_length, compress);
        MdsFree1Dx(&dim_xd,0);
      }
      MdsFree1Dx(&data_xd,0);
    }
  } else {
    int length;
    if (sinfo->data_offset != -1 && sinfo->rows < 0) {
      length = sinfo->rows & 0x7fffffff;
      status = DataCopy(info_in, info_out, sinfo->data_offset, length, &sinfo->data_offset);
      if STATUS_OK
        status = DataCopy(info_in, info_out, sinfo->dimension_offset, sinfo->dimension_length,
                          &sinfo->dimension_offset);
    } else {
      int rowlen = header->length, i;
      for (i = 0; i < header->dimct - 1; i++)
        rowlen = rowlen * header->dims[i];
      if (sinfo->dimension_length == 0)
        length = sinfo->rows * sizeof(int64_t);
      else
        length = sinfo->dimension_length;
      status = DataCopy(info_in, info_out, sinfo->dimension_offset, length, &sinfo->dimension_offset);
      if STATUS_OK {
        length = rowlen * sinfo->rows;
        status = DataCopy(info_in, info_out, sinfo->data_offset, length, &sinfo->data_offset);
      }
    }
  }
  if (STATUS_OK && (sinfo->start_offset > 0)) {
    status = DataCopy(info_in, info_out, sinfo->start_offset, sinfo->start_length, &sinfo->start_offset);
    if(STATUS_OK && sinfo->end_offset > 0)
        status = DataCopy(info_in, info_out, sinfo->end_offset, sinfo->end_length, &sinfo->end_offset);
  }
  return status;
}

static int CopySegmentIndex(TREE_INFO * info_in, TREE_INFO * info_out, int nid, SEGMENT_HEADER * header,
                            int64_t * index_offset, int64_t * data_offset, int64_t * dim_offset, int compress){
  SEGMENT_INDEX segment_index;
  int i;
  int status = GetSegmentIndex(info_in, *index_offset, &segment_index);
  if STATUS_OK {
    if (segment_index.previous_offset != -1) {
      status =
          CopySegmentIndex(info_in, info_out, nid, header, &segment_index.previous_offset, data_offset,
                           dim_offset, compress);
    }
    for (i = 0; (i < SEGMENTS_PER_INDEX) && STATUS_OK; i++) {
      SEGMENT_INFO *sinfo = &segment_index.segment[i];
      status = CopySegment(info_in, info_out, nid, header, sinfo, compress);
    }
    *index_offset = -1;
    status = PutSegmentIndex(info_out, &segment_index, index_offset);
  }
  return status;
}

static int CopySegmentedRecords(TREE_INFO * info_in, TREE_INFO * info_out, int nid, int64_t * offset,
                                int *length, int compress){
  SEGMENT_HEADER header;
  INIT_STATUS_AS GetSegmentHeader(info_in, *offset, &header);
  if STATUS_OK {
    *length=0;
    status =
        CopySegmentIndex(info_in, info_out, nid, &header, &header.index_offset, &header.data_offset,
                         &header.dim_offset, compress);
    *offset = -1;
    status = PutSegmentHeader(info_out, &header, offset);
  }
  return status;
}

int TreeCopyExtended(PINO_DATABASE * dbid_in, PINO_DATABASE * dbid_out, int nid, NCI * nci, int compress){
  EXTENDED_ATTRIBUTES attributes;
  TREE_INFO *info_in = dbid_in->tree_info, *info_out = dbid_out->tree_info;
  int64_t now = -1;
  int64_t offset = -1;
  INIT_STATUS_AS TreeGetExtendedAttributes(info_in, RfaToSeek(nci->DATA_INFO.DATA_LOCATION.rfa), &attributes);
  if STATUS_OK {
    if (attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY] != -1)
      CopyNamedAttributes(info_in, info_out, nid,
                          &attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY],
                          &attributes.facility_length[NAMED_ATTRIBUTES_FACILITY], compress);
    if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY] != -1)
      CopySegmentedRecords(info_in, info_out, nid,
                           &attributes.facility_offset[SEGMENTED_RECORD_FACILITY],
                           &attributes.facility_length[SEGMENTED_RECORD_FACILITY], compress);
    if (attributes.facility_offset[STANDARD_RECORD_FACILITY] != -1)
      CopyStandardRecord(info_in, info_out, nid,
                         &attributes.facility_offset[STANDARD_RECORD_FACILITY],
                         &attributes.facility_length[STANDARD_RECORD_FACILITY], compress);
    status = TreePutExtendedAttributes(info_out, &attributes, &offset);
    if STATUS_OK {
      SeekToRfa(offset, nci->DATA_INFO.DATA_LOCATION.rfa);
      status = TreePutNci(info_out, nid, nci, 0);
    }
    TreeSetViewDate(&now);
  }
  return status;
}

static EMPTYXD(TREE_START_CONTEXT);
static EMPTYXD(TREE_END_CONTEXT);
static EMPTYXD(TREE_DELTA_CONTEXT);
int TreeSetTimeContext(struct descriptor *start, struct descriptor *end, struct descriptor *delta)
{
  INIT_STATUS_AS MdsCopyDxXd(start, &TREE_START_CONTEXT);
  if STATUS_OK {
    status = MdsCopyDxXd(end, &TREE_END_CONTEXT);
    if STATUS_OK
      status = MdsCopyDxXd(delta, &TREE_DELTA_CONTEXT);
  }
  return status;
}

int TreeResetTimeContext()
{
  EMPTYXD(emptyXd);
  INIT_STATUS_AS MdsCopyDxXd((struct descriptor *)&emptyXd, &TREE_START_CONTEXT);
  if STATUS_OK {
    status = MdsCopyDxXd((struct descriptor *)&emptyXd, &TREE_END_CONTEXT);
    if STATUS_OK
      status = MdsCopyDxXd((struct descriptor *)&emptyXd, &TREE_DELTA_CONTEXT);
  }
  return status;
}

int _TreeGetSegmentedRecord(void *dbid, int nid, struct descriptor_xd *data)
{
  INIT_TREESUCCESS;
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
  return (*addr) (dbid, nid, TREE_START_CONTEXT.pointer, TREE_END_CONTEXT.pointer, TREE_DELTA_CONTEXT.pointer, data);
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

/* GETFROMSEGMENT
 * opens tree->extended_nci->segment_handler->segemnt_index
 * and finds segment idx.
 * idx can be number in the range of 0-(number of segments-1) or -1.
 * If -1 is specified return the last segment started for the segment.
 * - no clean up required. -
 */
#define GETFROMSEGMENT \
OPEN_INDEX_READ; \
if (idx == -1) idx = segment_header.idx; \
while (STATUS_OK && idx < segment_index.first_idx && segment_index.previous_offset > 0) \
  status = GetSegmentIndex(info_ptr, segment_index.previous_offset, &segment_index); \
if STATUS_NOT_OK \
  return status; \
if (idx < segment_index.first_idx || idx > segment_index.first_idx + SEGMENTS_PER_INDEX) \
  return TreeFAILURE; \
SEGMENT_INFO *sinfo = &segment_index.segment[idx % SEGMENTS_PER_INDEX];

int _TreeGetSegmentLimits(void *dbid, int nid, int idx, struct descriptor_xd *retStart,
                          struct descriptor_xd *retEnd){
  GETFROMSEGMENT;
  return getSegmentLimits(info_ptr, nid, sinfo, retStart, retEnd);
}

int _TreeGetSegment(void *dbid, int nid, int idx, struct descriptor_xd *segment,
                    struct descriptor_xd *dim){
  GETFROMSEGMENT;
  return ReadSegment(info_ptr, nid, &segment_header, sinfo, segment, dim);
}

int _TreeGetSegmentInfo(void *dbid, int nid, int idx, char *dtype, char *dimct, int *dims, int *next_row){
  GETFROMSEGMENT;
  if (sinfo->data_offset == -1)
    return TreeFAILURE;
  *dtype = segment_header.dtype;
  *dimct = segment_header.dimct;
  memcpy(dims, segment_header.dims, sizeof(segment_header.dims));
  if (idx == segment_header.idx)
    *next_row = segment_header.next_row;
  else if (sinfo->rows < 1)
    return GetCompressedSegmentRows(info_ptr, sinfo->data_offset, next_row);
  else
    *next_row = sinfo->rows;
  return status;
}

static int isSegmentInRange(TREE_INFO * info_ptr, int nid,
                          struct descriptor *start,
                          struct descriptor *end,
                          SEGMENT_INFO * sinfo){
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
      status = getSegmentLimits(info_ptr, nid, sinfo, &segstart, &segend);
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
  OPEN_HEADER_READ;
  SEGMENT_INDEX segment_index;
  int numsegs = segment_header.idx + 1;
  int segfound = B_FALSE;
  int apd_idx = 0;
  struct descriptor **dptr = malloc(sizeof(struct descriptor *) * numsegs * 2);
  DESCRIPTOR_APD(apd, DTYPE_LIST, dptr, numsegs * 2);
  memset(dptr, 0, sizeof(struct descriptor *) * numsegs * 2);
  status = GetSegmentIndex(info_ptr, segment_header.index_offset, &segment_index);
  int idx;
  for (idx = numsegs; STATUS_OK && idx > 0; idx--) {
    int segidx = idx - 1;
    while (STATUS_OK && segidx < segment_index.first_idx && segment_index.previous_offset > 0)
      status = GetSegmentIndex(info_ptr, segment_index.previous_offset, &segment_index);
    if STATUS_NOT_OK
      break;
    else {
      SEGMENT_INFO *sinfo = &segment_index.segment[segidx % SEGMENTS_PER_INDEX];
      if (isSegmentInRange(info_ptr, nid, start, end, sinfo)) {
        EMPTYXD(segment);
        EMPTYXD(dim);
        segfound = B_TRUE;
        status = ReadSegment(info_ptr, nid, &segment_header, sinfo, &segment, &dim);
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
  for (idx = 0; idx < apd_idx; idx++) {
    if (apd.pointer[idx] != NULL) {
      MdsFree1Dx((struct descriptor_xd *)apd.pointer[idx], 0);
      free(apd.pointer[idx]);
    }
  }
  if (apd.pointer)
    free(apd.pointer);
  return status;
}
