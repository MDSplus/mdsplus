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
typedef ARRAY_COEFF(char, 8) A_COEFF_TYPE;
static int PutNamedAttributesIndex(TREE_INFO * info, NAMED_ATTRIBUTES_INDEX * index,
				   int64_t * offset);
static int PutSegmentHeader(TREE_INFO * info_ptr, SEGMENT_HEADER * hdr, int64_t * offset);
static int PutSegmentIndex(TREE_INFO * info_ptr, SEGMENT_INDEX * idx, int64_t * offset);
static int PutInitialValue(TREE_INFO * info_ptr, int *dims, struct descriptor_a *array,
			   int64_t * offset);
static int PutDimensionValue(TREE_INFO * info_ptr, int64_t * timestamps, int rows_filled, int ndims,
			     int *dims, int64_t * offset);
static int GetSegmentHeader(TREE_INFO * info_ptr, int64_t offset, SEGMENT_HEADER * hdr);
static int GetSegmentIndex(TREE_INFO * info_ptr, int64_t offset, SEGMENT_INDEX * idx);
static int GetNamedAttributesIndex(TREE_INFO * info_ptr, int64_t offset,
				   NAMED_ATTRIBUTES_INDEX * index);

static int __TreeBeginSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end,
			      struct descriptor *dimension, struct descriptor_a *initialValue,
			      int idx, int rows_filled);
int _TreeBeginSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end,
		      struct descriptor *dimension, struct descriptor_a *initialValue, int idx)
{
  return __TreeBeginSegment(dbid, nid, start, end, dimension, initialValue, idx, 0);
}

int TreeBeginSegment(int nid, struct descriptor *start, struct descriptor *end,
		     struct descriptor *dimension, struct descriptor_a *initialValue, int idx)
{
  return _TreeBeginSegment(*TreeCtx(), nid, start, end, dimension, initialValue, idx);
}

int _TreeMakeSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end,
		     struct descriptor *dimension,
		     struct descriptor_a *initialValue, int idx, int rows_filled)
{
  return __TreeBeginSegment(dbid, nid, start, end, dimension, initialValue, idx, rows_filled);
}

int TreeMakeSegment(int nid, struct descriptor *start, struct descriptor *end,
		    struct descriptor *dimension,
		    struct descriptor_a *initialValue, int idx, int rows_filled)
{
  return _TreeMakeSegment(*TreeCtx(), nid, start, end, dimension, initialValue, idx, rows_filled);
}

int TreeGetSegmentTimesXd(int nid, int *nsegs, struct descriptor_xd *start_list, struct descriptor_xd *end_list)
{
  return _TreeGetSegmentTimesXd(*TreeCtx(), nid, nsegs, start_list, end_list);
}

int _TreeGetSegmentTimesXd(void *dbid, int nid, int *nsegs, struct descriptor_xd *start_list, struct descriptor_xd *end_list)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid_ptr = (NID *) & nid;
  int status = TreeFAILURE;
  int open_status;
  TREE_INFO *info_ptr;
  int nidx;
  NODE *node_ptr;
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  node_ptr = nid_to_node(dblist, nid_ptr);
  if (!node_ptr)
    return TreeNNF;
  if (dblist->remote) {
    printf("Segmented records are not supported using thick client mode\n");
    return 0;
  }
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr) {
    NCI local_nci;
    int64_t saved_viewdate;
    EXTENDED_ATTRIBUTES attributes;
    SEGMENT_HEADER segment_header;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciW(info_ptr, nidx, &local_nci, 0);
    if (!(status & 1))
      return status;
    if (info_ptr->data_file == 0)
      open_status = TreeOpenDatafileR(info_ptr);
    else
      open_status = 1;
    if (!(open_status & 1)) {
      return open_status;
    }
    if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) ||
	((TreeGetExtendedAttributes
	  (info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1) == 0)) {
      status = TreeFAILURE;
    } else if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY] == 0 ||
	       ((GetSegmentHeader
		 (info_ptr, attributes.facility_offset[SEGMENTED_RECORD_FACILITY],
		  &segment_header) & 1) == 0)) {
      status = TreeFAILURE;
    } else {
      SEGMENT_INDEX index;
      int numsegs = segment_header.idx + 1;
      int idx;
      DESCRIPTOR_APD(start_apd, DTYPE_LIST, malloc(numsegs * sizeof(void *)), numsegs);
      DESCRIPTOR_APD(end_apd, DTYPE_LIST, malloc(numsegs * sizeof(void *)), numsegs);
      struct descriptor_xd **start_xds = (struct descriptor_xd **)start_apd.pointer;
      struct descriptor_xd **end_xds = (struct descriptor_xd **)end_apd.pointer;
      *nsegs = numsegs;
      status = GetSegmentIndex(info_ptr, segment_header.index_offset, &index);
      for (idx = numsegs - 1; (status & 1) && idx >= 0; idx--) {
	EMPTYXD(xd);
	start_xds[idx]=memcpy(malloc(sizeof(struct descriptor_xd)),&xd,
			      sizeof(struct descriptor_xd));
	end_xds[idx]=memcpy(malloc(sizeof(struct descriptor_xd)),&xd,
			    sizeof(struct descriptor_xd));
	struct descriptor_xd *start_xd = ((struct descriptor_xd **)start_apd.pointer)[idx];
	struct descriptor_xd *end_xd = ((struct descriptor_xd **)end_apd.pointer)[idx];
	int index_idx = idx % SEGMENTS_PER_INDEX;
	SEGMENT_INFO *sinfo = &index.segment[index_idx];
	if (sinfo->dimension_offset != -1 && sinfo->dimension_length == 0) {
	  /* It's a timestamped segment */
	  int64_t startval;
	  int64_t endval;
	  struct descriptor startdsc = {sizeof(int64_t), DTYPE_Q, CLASS_S, (char *)&startval};
	  struct descriptor enddsc = {sizeof(int64_t), DTYPE_Q, CLASS_S, (char *)&endval};
	  if (sinfo->rows < 0 || !(sinfo->start == 0 && sinfo->end == 0)) {
	    /* Valid start and end in segment info */
	    startval = sinfo->start;
	    endval = sinfo->end;
	  } else {
	    /* Current segment so use timestmps in segment */
	    int length = sizeof(int64_t) * sinfo->rows;
	    char *buffer = malloc(length), *bptr;
	    int64_t timestamp;
	    int deleted = 1;
	    while (status & 1 && deleted) {
	      status =
		  (MDS_IO_READ_X
		   (info_ptr->data_file->get, sinfo->dimension_offset, buffer, length,
		    &deleted) == length) ? TreeSUCCESS : TreeFAILURE;
	      if (status & 1 && deleted)
		status = TreeReopenDatafile(info_ptr);
	    }
	    if (status & 1) {
	      startval = swapquad(buffer);
	      for (bptr = buffer + length - sizeof(int64_t);
		   bptr > buffer && ((timestamp = swapquad(bptr)) == 0); bptr -= sizeof(int64_t)) ;
	      if (bptr > buffer) {
		endval = timestamp;
	      } else {
		endval = 0;
	      }
	    } else {
	      startval = 0;
	      endval = 0;
	    }
	    free(buffer);
	  }
	  MdsCopyDxXd(&startdsc,start_xd);
	  MdsCopyDxXd(&enddsc,end_xd);
	} else {
	  if (sinfo->start != -1) {
	    struct descriptor startdsc = {sizeof(int64_t), DTYPE_Q, CLASS_S, (char *)&sinfo->start};
	    MdsCopyDxXd(&startdsc,start_xd);
	  } else if (sinfo->start_length > 0 && sinfo->start_offset > 0) {
	    status = TreeGetDsc(info_ptr, nid, sinfo->start_offset, sinfo->start_length, start_xd);
	  }
	  if (sinfo->end != -1) {
	    struct descriptor enddsc = {sizeof(int64_t), DTYPE_Q, CLASS_S, (char *)&sinfo->end};
	    MdsCopyDxXd(&enddsc,end_xd);
	  } else if (sinfo->end_length > 0 && sinfo->end_offset > 0) {
	    status = TreeGetDsc(info_ptr, nid, sinfo->end_offset, sinfo->end_length, end_xd);
	  }
	}
	if (index_idx == 0 && idx > 0) {
	  status = GetSegmentIndex(info_ptr, index.previous_offset, &index);
	}
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
    }
  }
  return status;
}

int TreeGetSegmentTimes(int nid, int *nsegs, uint64_t ** times)
{
  return _TreeGetSegmentTimes(*TreeCtx(), nid, nsegs, times);
}

int _TreeGetSegmentTimes(void *dbid, int nid, int *nsegs, uint64_t ** times)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid_ptr = (NID *) & nid;
  int status = TreeFAILURE;
  int open_status;
  TREE_INFO *info_ptr;
  int nidx;
  NODE *node_ptr;
  *times = NULL;
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  node_ptr = nid_to_node(dblist, nid_ptr);
  if (!node_ptr)
    return TreeNNF;
  if (dblist->remote) {
    printf("Segmented records are not supported using thick client mode\n");
    return 0;
  }
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr) {
    NCI local_nci;
    int64_t saved_viewdate;
    EXTENDED_ATTRIBUTES attributes;
    SEGMENT_HEADER segment_header;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciW(info_ptr, nidx, &local_nci, 0);
    if (!(status & 1))
      return status;
    if (info_ptr->data_file == 0)
      open_status = TreeOpenDatafileR(info_ptr);
    else
      open_status = 1;
    if (!(open_status & 1)) {
      return open_status;
    }
    if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) ||
	((TreeGetExtendedAttributes
	  (info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1) == 0)) {
      status = TreeFAILURE;
    } else if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY] == 0 ||
	       ((GetSegmentHeader
		 (info_ptr, attributes.facility_offset[SEGMENTED_RECORD_FACILITY],
		  &segment_header) & 1) == 0)) {
      status = TreeFAILURE;
    } else {
      SEGMENT_INDEX index;
      int numsegs = segment_header.idx + 1;
      int idx;
      uint64_t *ans = (uint64_t *) malloc(numsegs * 2 * sizeof(uint64_t));
      *nsegs = numsegs;
      *times = ans;
      memset(ans, 0, numsegs * 2 * sizeof(uint64_t));
      status = GetSegmentIndex(info_ptr, segment_header.index_offset, &index);
      for (idx = numsegs - 1; (status & 1) && idx >= 0; idx--) {
	int index_idx = idx % SEGMENTS_PER_INDEX;
	SEGMENT_INFO *sinfo = &index.segment[index_idx];
	if (sinfo->dimension_offset != -1 && sinfo->dimension_length == 0) {
	  /* It's a timestamped segment */
	  if (sinfo->rows < 0 || !(sinfo->start == 0 && sinfo->end == 0)) {
	    /* Valid start and end in segment info */
	    ans[idx * 2] = sinfo->start;
	    ans[idx * 2 + 1] = sinfo->end;
	  } else {
	    /* Current segment so use timestmps in segment */
	    int length = sizeof(int64_t) * sinfo->rows;
	    char *buffer = malloc(length), *bptr;
	    int64_t timestamp;
	    int deleted = 1;
	    while (status & 1 && deleted) {
	      status =
		  (MDS_IO_READ_X
		   (info_ptr->data_file->get, sinfo->dimension_offset, buffer, length,
		    &deleted) == length) ? TreeSUCCESS : TreeFAILURE;
	      if (status & 1 && deleted)
		status = TreeReopenDatafile(info_ptr);
	    }
	    if (status & 1) {
	      ans[idx * 2] = swapquad(buffer);
	      for (bptr = buffer + length - sizeof(int64_t);
		   bptr > buffer && ((timestamp = swapquad(bptr)) == 0); bptr -= sizeof(int64_t)) ;
	      if (bptr > buffer) {
		ans[idx * 2 + 1] = timestamp;
	      } else {
		ans[idx * 2 + 1] = 0;
	      }
	    } else {
	      ans[idx * 2] = 0;
	      ans[idx * 2 + 1] = 0;
	    }
	    free(buffer);
	  }
	} else {
	  if (sinfo->start != -1) {
	    ans[idx * 2] = sinfo->start;
	  } else if (sinfo->start_length > 0 && sinfo->start_offset > 0) {
	    EMPTYXD(xd);
	    status = TreeGetDsc(info_ptr, nid, sinfo->start_offset, sinfo->start_length, &xd);
	    if (status & 1 && xd.pointer && xd.pointer->length == 8) {
	      ans[idx * 2] = *(uint64_t *) xd.pointer->pointer;
	    } else {
	      ans[idx * 2] = 0;
	    }
	    MdsFree1Dx(&xd, 0);
	  } else
	    ans[idx * 2] = 0;
	  if (sinfo->end != -1) {
	    ans[idx * 2 + 1] = sinfo->end;
	  } else if (sinfo->end_length > 0 && sinfo->end_offset > 0) {
	    EMPTYXD(xd);
	    status = TreeGetDsc(info_ptr, nid, sinfo->end_offset, sinfo->end_length, &xd);
	    if (status & 1 && xd.pointer && xd.pointer->length == 8) {
	      ans[idx * 2 + 1] = *(uint64_t *) xd.pointer->pointer;
	    } else {
	      ans[idx * 2 + 1] = 0;
	    }
	    MdsFree1Dx(&xd, 0);
	  } else
	    ans[idx * 2 + 1] = 0;
	}
	if (index_idx == 0 && idx > 0) {
	  status = GetSegmentIndex(info_ptr, index.previous_offset, &index);
	}
      }
    }
  }
  return status;
}

static int __TreeBeginSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end,
			      struct descriptor *dimension,
			      struct descriptor_a *initialValue, int idx, int rows_filled)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid_ptr = (NID *) & nid;
  int status;
  int open_status;
  TREE_INFO *info_ptr;
  int nidx;
  int shot_open;
  NODE *node_ptr;
  A_COEFF_TYPE *a_coeff = (A_COEFF_TYPE *) initialValue;
  struct descriptor *dsc;
  // compress_utility = utility_update == 2;
#ifndef _WIN32
  static int saved_uic = 0;
  if (!saved_uic)
    saved_uic = (getgid() << 16) | getuid();
#endif
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  if (dblist->open_readonly)
    return TreeREADONLY;

  node_ptr = nid_to_node(dblist, nid_ptr);
  if (!node_ptr)
    return TreeNNF;
  //  if (node_ptr->usage != TreeUSAGE_SIGNAL)
  //  return  TreeINVDTPUSG;

  while (initialValue && initialValue->dtype == DTYPE_DSC)
    initialValue = (struct descriptor_a *)initialValue->pointer;
  if (initialValue == NULL ||
      !((initialValue->class == CLASS_R && initialValue->dtype == DTYPE_OPAQUE) ||
	(initialValue->class == CLASS_A && (initialValue->dimct > 0 || initialValue->dimct <= 8)))
      )
    return TreeINVDTPUSG;
  if (dblist->remote) {
    printf("Segmented records are not supported using thick client mode\n");
    return 0;
  }
  shot_open = (dblist->shotid != -1);
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr) {
    int stv;
    NCI local_nci;
    int64_t saved_viewdate;
    EXTENDED_ATTRIBUTES attributes;
    int64_t attributes_offset = -1;
    int update_attributes = 0;
    int add_length = 0;
    int previous_length = -1;
    int d;
    SEGMENT_HEADER segment_header;
    SEGMENT_INDEX segment_index;
    SEGMENT_INFO *sinfo;
    int compress;
    status = TreeCallHook(PutData, info_ptr, nid);
    if (status && !(status & 1))
      return status;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciLw(info_ptr, nidx, &local_nci);
    if (!(status & 1))
      return status;
    if ((status & 1) && (shot_open && (local_nci.flags & NciM_NO_WRITE_SHOT)))
      status = TreeNOWRITESHOT;
    if ((status & 1) && (!shot_open && (local_nci.flags & NciM_NO_WRITE_MODEL)))
      status = TreeNOWRITEMODEL;
    if (!(status & 1)) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return status;
    }
    compress =	(local_nci.flags & NciM_COMPRESS_ON_PUT) &&
	(local_nci.flags & NciM_COMPRESS_SEGMENTS) &&
      !(local_nci.flags & NciM_DO_NOT_COMPRESS);
    if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
      open_status = TreeOpenDatafileW(info_ptr, &stv, 0);
    else
      open_status = 1;
    if (!(open_status & 1)) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return open_status;
    }
    local_nci.flags2 &= ~NciM_DATA_IN_ATT_BLOCK;
    local_nci.class = CLASS_R;
    local_nci.dtype = initialValue->dtype;
    local_nci.time_inserted = TreeTimeInserted();
#ifdef _WIN32
    local_nci.owner_identifier = 0;
#else
    local_nci.owner_identifier = saved_uic;
#endif
    /*** See if node is currently using the Extended Nci feature and if so get the current contents of the attributes
         index. If not, make an empty index and flag that a new index needs to be written.
    ****/
    if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) ||
	((TreeGetExtendedAttributes
	  (info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1) == 0)) {
      memset(&attributes, -1, sizeof(attributes));
      update_attributes = 1;
    } else {
      attributes_offset = RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa);
      if (attributes.facility_offset[STANDARD_RECORD_FACILITY] != -1) {
	attributes.facility_offset[STANDARD_RECORD_FACILITY] = -1;
	attributes.facility_length[STANDARD_RECORD_FACILITY] = 0;
	update_attributes = 1;
      }
    }
    /*** See if the node currently has an segment header record. If not, make an empty segment header and flag that
	 a new one needs to be written.
    ***/
    if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY] == -1 ||
	((GetSegmentHeader
	  (info_ptr, attributes.facility_offset[SEGMENTED_RECORD_FACILITY],
	   &segment_header) & 1) == 0)) {
      memset(&segment_header, 0, sizeof(segment_header));
      attributes.facility_offset[SEGMENTED_RECORD_FACILITY] = -1;
      segment_header.index_offset = -1;
      segment_header.idx = -1;
      update_attributes = 1;
    } else if (initialValue->dtype != segment_header.dtype ||
	       (initialValue->class == CLASS_A &&
		(initialValue->dimct != segment_header.dimct ||
		 (initialValue->dimct > 1
		  && memcmp(segment_header.dims, a_coeff->m,
			    (initialValue->dimct - 1) * sizeof(int)) != 0)))) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return TreeFAILURE;
    }
    /*** See if the node currently has an segment_index record. If not, make an empty segment index and
	 flag that a new one needs to be written.
    ***/
    if ((segment_header.index_offset == -1) ||
	((GetSegmentIndex(info_ptr, segment_header.index_offset, &segment_index) & 1) == 0)) {
      segment_header.index_offset = -1;
      memset(&segment_index, -1, sizeof(segment_index));
      segment_index.first_idx = 0;
    }
    if (idx == -1) {
      segment_header.idx++;
      idx = segment_header.idx;
      add_length = (initialValue->class == CLASS_A) ? initialValue->arsize : 0;
    } else if (idx < -1 || idx > segment_header.idx) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return TreeBUFFEROVF;
    } else {
      printf("this is not yet supported\n");
      return TreeFAILURE;
      /***************
Add support for updating an existing segment. add_length=new_length-old_length. potentially use same storage area if
old array is same size.
      *******/
      add_length = 0;
    }
    segment_header.data_offset = -1;
    segment_header.dim_offset = -1;
    segment_header.dtype = initialValue->dtype;
    segment_header.dimct = (initialValue->class == CLASS_A) ? initialValue->dimct : 0;
    segment_header.length = (initialValue->class == CLASS_A) ? initialValue->length : 0;
    previous_length = -1;
    if (segment_header.idx > 0 && segment_header.length != 0) {
      previous_length = segment_header.length;
      for (d = 0; d < segment_header.dimct; d++)
	previous_length *= segment_header.dims[d];
    }
    if (initialValue->class == CLASS_A) {
      if (initialValue->dimct == 1)
	segment_header.dims[0] = initialValue->arsize / initialValue->length;
      else {
	memcpy(segment_header.dims, a_coeff->m, initialValue->dimct * sizeof(int));
      }
    }
    //    rows_filled=(rows_filled < 0) ? 0 : 
    //  ((rows_filled > segment_header.dims[initialValue->dimct]) ? segment_header.dims[initialValue->dimct] : rows_filled);
    segment_header.next_row = rows_filled;
    /*****
	  If not the first segment, see if we can reuse the previous segment storage space and compress the previous segment.
    ****/
    if (((segment_header.idx % SEGMENTS_PER_INDEX) > 0) &&
	(previous_length == add_length) && compress) {
      int deleted;
      EMPTYXD(xd_data);
      EMPTYXD(xd_dim);
      sinfo = &segment_index.segment[(idx % SEGMENTS_PER_INDEX) - 1];
      TreeUnLockNci(info_ptr, 0, nidx);
      status = _TreeGetSegment(dbid, nid, idx - 1, &xd_data, &xd_dim);
      TreeLockNci(info_ptr, 0, nidx, &deleted);
      if (status & 1) {
	int length;
	segment_header.data_offset = sinfo->data_offset;
	status = TreePutDsc(info_ptr, nid, xd_data.pointer, &sinfo->data_offset, &length, compress);
	/*** flag compressed segment by setting high bit in the rows field. ***/
	sinfo->rows = length | 0x80000000;
      }
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
    if (initialValue->dtype == DTYPE_OPAQUE) {
      int length;
      status =
	  TreePutDsc(info_ptr, nid, (struct descriptor *)initialValue, &sinfo->data_offset,
		     &length, compress);
      segment_header.data_offset = sinfo->data_offset;
      add_length = length;
      sinfo->rows = length | 0x80000000;
    } else
      status =
	  PutInitialValue(info_ptr, segment_header.dims, initialValue, &segment_header.data_offset);
    for (dsc = start; dsc != 0 && dsc->pointer != 0 && dsc->dtype == DTYPE_DSC;
	 dsc = (struct descriptor *)dsc->pointer) ;
    if (dsc != 0 && dsc->pointer != 0 && (dsc->dtype == DTYPE_Q || dsc->dtype == DTYPE_QU)) {
      sinfo->start = *(int64_t *) dsc->pointer;
      sinfo->start_offset = -1;
      sinfo->start_length = 0;
    } else if (start != 0) {
      sinfo->start = -1;
      status = TreePutDsc(info_ptr, nid, start, &sinfo->start_offset, &sinfo->start_length, compress);
    }
    for (dsc = end; dsc != 0 && dsc->pointer != 0 && dsc->dtype == DTYPE_DSC;
	 dsc = (struct descriptor *)dsc->pointer) ;
    if (dsc != 0 && (dsc->dtype == DTYPE_Q || dsc->dtype == DTYPE_QU)) {
      sinfo->end = *(int64_t *) dsc->pointer;
      sinfo->end_offset = -1;
      sinfo->end_length = 0;
    } else if (end != 0) {
      sinfo->end = -1;
      status = TreePutDsc(info_ptr, nid, end, &sinfo->end_offset, &sinfo->end_length, compress);
    }
    if (dimension != 0) {
      status =
	TreePutDsc(info_ptr, nid, dimension, &sinfo->dimension_offset, &sinfo->dimension_length, compress);
    }
    if (initialValue->dtype != DTYPE_OPAQUE) {
      sinfo->data_offset = segment_header.data_offset;
      sinfo->rows = segment_header.dims[segment_header.dimct - 1];
    }
    status = PutSegmentIndex(info_ptr, &segment_index, &segment_header.index_offset);
    status =
	PutSegmentHeader(info_ptr, &segment_header,
			 &attributes.facility_offset[SEGMENTED_RECORD_FACILITY]);
    if (update_attributes) {
      status = TreePutExtendedAttributes(info_ptr, &attributes, &attributes_offset);
      SeekToRfa(attributes_offset, local_nci.DATA_INFO.DATA_LOCATION.rfa);
      local_nci.flags2 |= NciM_EXTENDED_NCI;
    }
    if (((int64_t) local_nci.length + (int64_t) add_length) < (2 ^ 31))
      local_nci.length += add_length;
    else
      local_nci.length = (2 ^ 31);
    local_nci.flags=local_nci.flags | NciM_SEGMENTED;
    TreePutNci(info_ptr, nidx, &local_nci, 0);
    TreeUnLockNci(info_ptr, 0, nidx);
  } else status = TreeNNF;
  return status;
}

int _TreeUpdateSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end,
		       struct descriptor *dimension, int idx);

int TreeUpdateSegment(int nid, struct descriptor *start, struct descriptor *end,
		      struct descriptor *dimension, int idx)
{
  return _TreeUpdateSegment(*TreeCtx(), nid, start, end, dimension, idx);
}

int _TreeUpdateSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end,
		       struct descriptor *dimension, int idx)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid_ptr = (NID *) & nid;
  int status;
  int open_status;
  TREE_INFO *info_ptr;
  int nidx;
  int shot_open;
  NODE *node_ptr;
  struct descriptor *dsc;
  //  compress_utility = utility_update == 2;
#ifndef _WIN32
  static int saved_uic = 0;
  if (!saved_uic)
    saved_uic = (getgid() << 16) | getuid();
#endif
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  if (dblist->open_readonly)
    return TreeREADONLY;

  node_ptr = nid_to_node(dblist, nid_ptr);
  if (!node_ptr)
    return TreeNNF;
  //  if (node_ptr->usage != TreeUSAGE_SIGNAL)
  //  return  TreeINVDTPUSG;

  if (dblist->remote) {
    printf("Segmented records are not supported using thick client mode\n");
    return 0;
  }
  shot_open = (dblist->shotid != -1);
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr) {
    int stv;
    NCI local_nci;
    int64_t saved_viewdate;
    int64_t index_offset;
    EXTENDED_ATTRIBUTES attributes;
    SEGMENT_HEADER segment_header;
    SEGMENT_INDEX segment_index;
    SEGMENT_INFO *sinfo;
    int compress;
    status = TreeCallHook(PutData, info_ptr, nid);
    if (status && !(status & 1))
      return status;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciLw(info_ptr, nidx, &local_nci);
    if (!(status & 1))
      return status;
    if ((status & 1) && (shot_open && (local_nci.flags & NciM_NO_WRITE_SHOT)))
      status = TreeNOWRITESHOT;
    if ((status & 1) && (!shot_open && (local_nci.flags & NciM_NO_WRITE_MODEL)))
      status = TreeNOWRITEMODEL;
    if (!(status & 1)) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return status;
    }
    if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
      open_status = TreeOpenDatafileW(info_ptr, &stv, 0);
    else
      open_status = 1;
    if (!(open_status & 1)) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return open_status;
    }
    /*** See if node is currently using the Extended Nci feature and if so get the current contents of the attributes
         index. If not, make an empty index and flag that a new index needs to be written.
    ****/
    if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) ||
	((TreeGetExtendedAttributes
	  (info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1) == 0)) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return TreeFAILURE;
    }
    /*** See if the node currently has an segment header record. If not, make an empty segment header and flag that
	 a new one needs to be written.
    ***/
    if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY] == -1 ||
	((GetSegmentHeader
	  (info_ptr, attributes.facility_offset[SEGMENTED_RECORD_FACILITY],
	   &segment_header) & 1) == 0)) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return TreeFAILURE;
    }
    /*** See if the node currently has an segment_index record. If not, make an empty segment index and
	 flag that a new one needs to be written.
    ***/
    if ((segment_header.index_offset == -1) ||
	((GetSegmentIndex(info_ptr, segment_header.index_offset, &segment_index) & 1) == 0)) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return TreeFAILURE;
    }
    if (idx < 0 || idx > segment_header.idx) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return TreeBUFFEROVF;
    }
    compress =	(local_nci.flags & NciM_COMPRESS_ON_PUT) &&
	(local_nci.flags & NciM_COMPRESS_SEGMENTS) &&
      !(local_nci.flags & NciM_DO_NOT_COMPRESS);
    for (index_offset = segment_header.index_offset;
	 (status & 1) != 0 && idx < segment_index.first_idx && segment_index.previous_offset > 0;
	 index_offset = segment_index.previous_offset)
      status = GetSegmentIndex(info_ptr, segment_index.previous_offset, &segment_index);
    if (!(status & 1) || (idx < segment_index.first_idx)) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return TreeFAILURE;
    }
    sinfo = &segment_index.segment[idx % SEGMENTS_PER_INDEX];
    for (dsc = start; dsc != 0 && dsc->pointer != 0 && dsc->dtype == DTYPE_DSC;
	 dsc = (struct descriptor *)dsc->pointer) ;
    if (dsc != 0 && dsc->pointer != 0 && (dsc->dtype == DTYPE_Q || dsc->dtype == DTYPE_QU)) {
      sinfo->start = *(int64_t *) dsc->pointer;
      sinfo->start_offset = -1;
      sinfo->start_length = 0;
    } else if (start != 0) {
      sinfo->start = -1;
      status = TreePutDsc(info_ptr, nid, start, &sinfo->start_offset, &sinfo->start_length, compress);
    }
    for (dsc = end; dsc != 0 && dsc->pointer != 0 && dsc->dtype == DTYPE_DSC;
	 dsc = (struct descriptor *)dsc->pointer) ;
    if (dsc != 0 && (dsc->dtype == DTYPE_Q || dsc->dtype == DTYPE_QU)) {
      sinfo->end = *(int64_t *) dsc->pointer;
      sinfo->end_offset = -1;
      sinfo->end_length = 0;
    } else if (end != 0) {
      sinfo->end = -1;
      status = TreePutDsc(info_ptr, nid, end, &sinfo->end_offset, &sinfo->end_length, compress);
    }
    if (dimension != 0) {
      status =
	TreePutDsc(info_ptr, nid, dimension, &sinfo->dimension_offset, &sinfo->dimension_length, compress);
    }
    sinfo->data_offset = segment_header.data_offset;
    sinfo->rows = segment_header.dims[segment_header.dimct - 1];
    status = PutSegmentIndex(info_ptr, &segment_index, &index_offset);
    TreeUnLockNci(info_ptr, 0, nidx);
  }
  else status=TreeNNF;
  return status;
}

int _TreePutSegment(void *dbid, int nid, int startIdx, struct descriptor_a *data);
int TreePutSegment(int nid, int startIdx, struct descriptor_a *data)
{
  return _TreePutSegment(*TreeCtx(), nid, startIdx, data);
}

int _TreePutSegment(void *dbid, int nid, int startIdx, struct descriptor_a *data)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid_ptr = (NID *) & nid;
  int status;
  int open_status;
  TREE_INFO *info_ptr;
  int nidx;
  int shot_open;
  NODE *node_ptr;
  DESCRIPTOR_A(data_a, 0, 0, 0, 0);
  A_COEFF_TYPE *a_coeff;
  //  compress_utility = utility_update == 2;
#ifndef _WIN32
  static int saved_uic = 0;
  if (!saved_uic)
    saved_uic = (getgid() << 16) | getuid();
#endif
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  if (dblist->open_readonly)
    return TreeREADONLY;

  node_ptr = nid_to_node(dblist, nid_ptr);
  if (!node_ptr)
    return TreeNNF;
  //  if (node_ptr->usage != TreeUSAGE_SIGNAL)
  //  return  TreeINVDTPUSG;

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
  a_coeff = (A_COEFF_TYPE *) data;
  if (data == NULL || data->class != CLASS_A || data->dimct < 1 || data->dimct > 8)
    return TreeINVDTPUSG;
  if (dblist->remote) {
    printf("Segmented records are not supported using thick client mode\n");
    return 0;
  }
  shot_open = (dblist->shotid != -1);
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr) {
    int stv;
    int rows_to_insert;
    int bytes_per_row;
    int rows_in_segment;
    unsigned int bytes_to_insert;
#ifdef WORDS_BIGENDIAN
    char *buffer, *bptr;
#endif
    int64_t offset;
    NCI local_nci;
    int64_t saved_viewdate;
    EXTENDED_ATTRIBUTES attributes;
    int i;
    SEGMENT_HEADER segment_header;
    status = TreeCallHook(PutData, info_ptr, nid);
    if (status && !(status & 1))
      return status;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciLw(info_ptr, nidx, &local_nci);
    if (!(status & 1))
      return status;
    if ((status & 1) && (shot_open && (local_nci.flags & NciM_NO_WRITE_SHOT)))
      status = TreeNOWRITESHOT;
    if ((status & 1) && (!shot_open && (local_nci.flags & NciM_NO_WRITE_MODEL)))
      status = TreeNOWRITEMODEL;
    if (!(status & 1)) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return status;
    }
    if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
      open_status = TreeOpenDatafileW(info_ptr, &stv, 0);
    else
      open_status = 1;
    if (!(open_status & 1)) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return open_status;
    }
    /*** See if node is currently using the Extended Nci feature and if so get the current contents of the attributes
         index. If not, make an empty index and flag that a new index needs to be written.
    ****/
    if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) ||
	((TreeGetExtendedAttributes
	  (info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1) == 0)) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return TreeFAILURE;
    }
    /*** See if the node currently has an segment header record. If not, make an empty segment header and flag that
	 a new one needs to be written.
    ***/
    if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY] == -1 ||
	((GetSegmentHeader
	  (info_ptr, attributes.facility_offset[SEGMENTED_RECORD_FACILITY],
	   &segment_header) & 1) == 0)) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return TreeFAILURE;
    } else if (data->dtype != segment_header.dtype
	       || (data->dimct != segment_header.dimct && data->dimct != segment_header.dimct - 1)
	       || (data->dimct > 1
		   && memcmp(segment_header.dims, a_coeff->m,
			     (data->dimct - 1) * sizeof(int)) != 0)) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return TreeFAILURE;
    }
    if (startIdx == -1) {
      startIdx = segment_header.next_row;
    } else if (startIdx < -1 || startIdx > segment_header.dims[segment_header.dimct - 1]) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return TreeBUFFEROVF;
    }
    for (bytes_per_row = segment_header.length, i = 0; i < segment_header.dimct - 1;
	 bytes_per_row *= segment_header.dims[i], i++) ;
    if (data->dimct < segment_header.dimct) {
      rows_to_insert = 1;
    } else {
      rows_to_insert =
	  (data->dimct == 1) ? data->arsize / data->length : a_coeff->m[a_coeff->dimct - 1];
    }
    rows_in_segment = segment_header.dims[segment_header.dimct - 1];
    bytes_to_insert =
	((rows_to_insert >
	  (rows_in_segment - startIdx)) ? (rows_in_segment -
					   startIdx) : rows_to_insert) * bytes_per_row;
    if (bytes_to_insert < data->arsize) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return TreeBUFFEROVF;
    }
    offset = segment_header.data_offset + startIdx * bytes_per_row;
#ifdef WORDS_BIGENDIAN
    buffer = memcpy(malloc(bytes_to_insert), data->pointer, bytes_to_insert);
    if (segment_header.length > 1 && data->dtype != DTYPE_T && data->dtype != DTYPE_IDENT
	&& data->dtype != DTYPE_PATH) {
      switch (segment_header.length) {
      case 2:
	for (i = 0, bptr = buffer; i < bytes_to_insert / segment_header.length;
	     i++, bptr += sizeof(short))
	  LoadShort(((short *)data->pointer)[i], bptr);
	break;
      case 4:
	for (i = 0, bptr = buffer; i < bytes_to_insert / segment_header.length;
	     i++, bptr += sizeof(int))
	  LoadInt(((int *)data->pointer)[i], bptr);
	break;
      case 8:
	for (i = 0, bptr = buffer; i < bytes_to_insert / segment_header.length;
	     i++, bptr += sizeof(int64_t))
	  LoadQuad(((int64_t *) data->pointer)[i], bptr);
	break;
      }
    }
    TreeLockDatafile(info_ptr, 0, offset);
    MDS_IO_LSEEK(info_ptr->data_file->put, offset, SEEK_SET);
    status =
	(MDS_IO_WRITE(info_ptr->data_file->put, buffer, bytes_to_insert) ==
	 (ssize_t)bytes_to_insert) ? TreeSUCCESS : TreeFAILURE;
    free(buffer);
#else
    TreeLockDatafile(info_ptr, 0, offset);
    MDS_IO_LSEEK(info_ptr->data_file->put, offset, SEEK_SET);
    status =
	(MDS_IO_WRITE(info_ptr->data_file->put, data->pointer, bytes_to_insert) ==
	 (ssize_t)bytes_to_insert) ? TreeSUCCESS : TreeFAILURE;
    TreeUnLockDatafile(info_ptr, 0, offset);
#endif
    if (startIdx == segment_header.next_row) {
      segment_header.next_row += bytes_to_insert / bytes_per_row;
    }
    if (status & 1)
      status =
	  PutSegmentHeader(info_ptr, &segment_header,
			   &attributes.facility_offset[SEGMENTED_RECORD_FACILITY]);
    TreeUnLockNci(info_ptr, 0, nidx);
  } else status = TreeNNF;
  return status;
}

int _TreeGetNumSegments(void *dbid, int nid, int *num);

int TreeGetNumSegments(int nid, int *num)
{
  return _TreeGetNumSegments(*TreeCtx(), nid, num);
}

int _TreeGetNumSegments(void *dbid, int nid, int *num)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid_ptr = (NID *) & nid;
  int status = TreeFAILURE;
  int open_status;
  TREE_INFO *info_ptr;
  int nidx;
  NODE *node_ptr;
  *num = 0;
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  node_ptr = nid_to_node(dblist, nid_ptr);
  *num = 0;
  if (!node_ptr)
    return TreeNNF;
  if (dblist->remote) {
    printf("Segmented records are not supported using thick client mode\n");
    return 0;
  }
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr) {
    NCI local_nci;
    int64_t saved_viewdate;
    EXTENDED_ATTRIBUTES attributes;
    SEGMENT_HEADER segment_header;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciW(info_ptr, nidx, &local_nci, 0);
    if (!(status & 1))
      return status;
    if (info_ptr->data_file == 0)
      open_status = TreeOpenDatafileR(info_ptr);
    else
      open_status = 1;
    if (!(open_status & 1)) {
      return open_status;
    }
    if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) ||
	((TreeGetExtendedAttributes
	  (info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1) == 0)) {
      *num = 0;
    } else if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY] == 0 ||
	       ((GetSegmentHeader
		 (info_ptr, attributes.facility_offset[SEGMENTED_RECORD_FACILITY],
		  &segment_header) & 1) == 0)) {
      *num = 0;
    } else
      *num = segment_header.idx + 1;
  }
  return status;
}

static int ReadSegment(TREE_INFO * info_ptr, int nid, SEGMENT_HEADER * segment_header,
		       SEGMENT_INFO * sinfo, struct descriptor_xd *segment,
		       struct descriptor_xd *dim)
{
  int status = 1;
  if (sinfo->data_offset != -1) {
    int i;
    int deleted = 1;
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
    for (i = 0; i < ans.dimct; i++)
      ans.arsize *= ans.m[i];
    if (sinfo->rows < 0) {
      EMPTYXD(compressed_segment_xd);
      int data_length = sinfo->rows & 0x7fffffff;
      compressed_segment = 1;
      status = TreeGetDsc(info_ptr, nid, sinfo->data_offset, data_length, &compressed_segment_xd);
      if (status & 1) {
	status = MdsDecompress((struct descriptor_r *)compressed_segment_xd.pointer, segment);
	MdsFree1Dx(&compressed_segment_xd, 0);
      }
    } else {
      ans.pointer = malloc(ans.arsize);
      while (status & 1 && deleted) {
	status =
	    (MDS_IO_READ_X
	     (info_ptr->data_file->get, sinfo->data_offset, ans.pointer, ans.arsize,
	      &deleted) == (ssize_t) ans.arsize) ? TreeSUCCESS : TreeFAILURE;
	if (status & 1 && deleted)
	  status = TreeReopenDatafile(info_ptr);
      }
    }
    if (status & 1) {
#ifdef WORDS_BIGENDIAN
      if (!compressed_segment) {
	char *bptr;
	if (ans.length > 1 && ans.dtype != DTYPE_T && ans.dtype != DTYPE_IDENT
	    && ans.dtype != DTYPE_PATH) {
	  switch (ans.length) {
	  case 2:
	    for (i = 0, bptr = ans.pointer; i < ans.arsize / ans.length; i++, bptr += sizeof(short))
	      *(short *)bptr = swapshort(bptr);
	    break;
	  case 4:
	    for (i = 0, bptr = ans.pointer; i < ans.arsize / ans.length; i++, bptr += sizeof(int))
	      *(int *)bptr = swapint(bptr);
	    break;
	  case 8:
	    for (i = 0, bptr = ans.pointer; i < ans.arsize / ans.length;
		 i++, bptr += sizeof(int64_t))
	      *(int64_t *) bptr = swapquad(bptr);
	    break;
	  }
	}
      }
#endif
      if (sinfo->dimension_offset != -1 && sinfo->dimension_length == 0) {
	int64_t *tp;
	int deleted = 1;
	//	char *bptr;
	dim2.arsize = sinfo->rows * sizeof(int64_t);
	dim2.pointer = malloc(dim2.arsize);
	while (status & 1 && deleted) {
	  status =
	      (MDS_IO_READ_X
	       (info_ptr->data_file->get, sinfo->dimension_offset, dim2.pointer, dim2.arsize,
		&deleted) == (ssize_t) dim2.arsize) ? TreeSUCCESS : TreeFAILURE;
	  if (status & 1 && deleted)
	    status = TreeReopenDatafile(info_ptr);
	}
#ifdef WORDS_BIGENDIAN
	for (i = 0, bptr = dim2.pointer; i < dim2.arsize / dim2.length;
	     i++, bptr += sizeof(int64_t))
	  *(int64_t *) bptr = swapquad(bptr);
#endif
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

int TreeGetSegment(int nid, int idx, struct descriptor_xd *segment, struct descriptor_xd *dim)
{
  return _TreeGetSegment(*TreeCtx(), nid, idx, segment, dim);
}

int _TreeGetSegment(void *dbid, int nid, int idx, struct descriptor_xd *segment,
		    struct descriptor_xd *dim)
{
  /*** Get a segment of data from a node. idx can be number in the range of 0-(number of segments-1) or -1. If
       -1 is specified return the last segment started for the segment. ***/
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid_ptr = (NID *) & nid;
  int status = TreeFAILURE;
  int open_status;
  TREE_INFO *info_ptr;
  int nidx;
  NODE *node_ptr;
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  node_ptr = nid_to_node(dblist, nid_ptr);
  if (!node_ptr)
    return TreeNNF;
  if (dblist->remote) {
    printf("Segmented records are not supported using thick client mode\n");
    return 0;
  }
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr) {
    NCI local_nci;
    int64_t saved_viewdate;
    EXTENDED_ATTRIBUTES attributes;
    SEGMENT_HEADER segment_header;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciW(info_ptr, nidx, &local_nci, 0);
    if (!(status & 1))
      return status;
    if (info_ptr->data_file == 0)
      open_status = TreeOpenDatafileR(info_ptr);
    else
      open_status = 1;
    if (!(open_status & 1)) {
      return open_status;
    }
    if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) ||
	((TreeGetExtendedAttributes
	  (info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1) == 0)) {
      status = TreeFAILURE;
    } else if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY] == 0 ||
	       ((GetSegmentHeader
		 (info_ptr, attributes.facility_offset[SEGMENTED_RECORD_FACILITY],
		  &segment_header) & 1) == 0)) {
      status = TreeFAILURE;
    } else {
      SEGMENT_INDEX index;
      if (idx == -1)
	idx = segment_header.idx;
      status = GetSegmentIndex(info_ptr, segment_header.index_offset, &index);
      while ((status & 1) != 0 && idx < index.first_idx && index.previous_offset > 0)
	status = GetSegmentIndex(info_ptr, index.previous_offset, &index);
      if ((status & 1) != 0 && idx >= index.first_idx && idx < index.first_idx + SEGMENTS_PER_INDEX) {
	SEGMENT_INFO *sinfo = &index.segment[idx % SEGMENTS_PER_INDEX];
	status = ReadSegment(info_ptr, nid, &segment_header, sinfo, segment, dim);
      } else {
	status = TreeFAILURE;
      }
    }
  }
  return status;
}

static int getSegmentLimits(TREE_INFO * info_ptr, int nid,
			    SEGMENT_INFO * sinfo,
			    struct descriptor_xd *retStart, struct descriptor_xd *retEnd)
{
  int status = 1;
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
      int deleted = 1;
      char *bptr;
      while (status & 1 && deleted) {
	status =
	    (MDS_IO_READ_X
	     (info_ptr->data_file->get, sinfo->dimension_offset, buffer, length,
	      &deleted) == length) ? TreeSUCCESS : TreeFAILURE;
	if (status & 1 && deleted)
	  status = TreeReopenDatafile(info_ptr);
      }
      if (status & 1) {
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

int _TreeGetSegmentLimits(void *dbid, int nid, int idx, struct descriptor_xd *retStart,
			  struct descriptor_xd *retEnd);
int TreeGetSegmentLimits(int nid, int idx, struct descriptor_xd *retStart,
			 struct descriptor_xd *retEnd)
{
  return _TreeGetSegmentLimits(*TreeCtx(), nid, idx, retStart, retEnd);
}

int _TreeGetSegmentLimits(void *dbid, int nid, int idx, struct descriptor_xd *retStart,
			  struct descriptor_xd *retEnd)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid_ptr = (NID *) & nid;
  int status = TreeFAILURE;
  int open_status;
  TREE_INFO *info_ptr;
  int nidx;
  NODE *node_ptr;
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  node_ptr = nid_to_node(dblist, nid_ptr);
  if (!node_ptr)
    return TreeNNF;
  if (dblist->remote) {
    printf("Segmented records are not supported using thick client mode\n");
    return 0;
  }
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr) {
    NCI local_nci;
    int64_t saved_viewdate;
    EXTENDED_ATTRIBUTES attributes;
    SEGMENT_HEADER segment_header;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciW(info_ptr, nidx, &local_nci, 0);
    if (!(status & 1))
      return status;
    if (info_ptr->data_file == 0)
      open_status = TreeOpenDatafileR(info_ptr);
    else
      open_status = 1;
    if (!(open_status & 1)) {
      return open_status;
    }
    if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) ||
	((TreeGetExtendedAttributes
	  (info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1) == 0)) {
      status = TreeFAILURE;
    } else if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY] == 0 ||
	       ((GetSegmentHeader
		 (info_ptr, attributes.facility_offset[SEGMENTED_RECORD_FACILITY],
		  &segment_header) & 1) == 0)) {
      status = TreeFAILURE;
    } else {
      SEGMENT_INDEX index;
      status = GetSegmentIndex(info_ptr, segment_header.index_offset, &index);
      while ((status & 1) != 0 && idx < index.first_idx && index.previous_offset > 0)
	status = GetSegmentIndex(info_ptr, index.previous_offset, &index);
      if ((status & 1) != 0 && idx >= index.first_idx && idx < index.first_idx + SEGMENTS_PER_INDEX) {
	SEGMENT_INFO *sinfo = &index.segment[idx % SEGMENTS_PER_INDEX];
	status = getSegmentLimits(info_ptr, nid, sinfo, retStart, retEnd);
      } else {
	status = TreeFAILURE;
      }
    }
  }
  return status;
}

int _TreeSetXNci(void *dbid, int nid, const char *xnciname, struct descriptor *value);

int TreeSetXNci(int nid, const char *xnciname, struct descriptor *value)
{
  return _TreeSetXNci(*TreeCtx(), nid, xnciname, value);
}

int _TreeSetXNci(void *dbid, int nid, const char *xnciname, struct descriptor *value)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid_ptr = (NID *) & nid;
  int status = TreeFAILURE;
  int open_status;
  TREE_INFO *info_ptr;
  int nidx;
  int shot_open;
  NODE *node_ptr;
  //  compress_utility = utility_update == 2;
#ifndef _WIN32
  static int saved_uic = 0;
  if (!saved_uic)
    saved_uic = (getgid() << 16) | getuid();
#endif
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  if (dblist->open_readonly)
    return TreeREADONLY;
  if (!xnciname || strlen(xnciname) < 1 || strlen(xnciname) > NAMED_ATTRIBUTE_NAME_SIZE)
    return TreeFAILURE;
  node_ptr = nid_to_node(dblist, nid_ptr);
  if (!node_ptr)
    return TreeNNF;
  //  if (node_ptr->usage != TreeUSAGE_SIGNAL)
  //  return  TreeINVDTPUSG;

  if (dblist->remote) {
    printf("Extended attributes are not supported using thick client mode\n");
    return 0;
  }
  shot_open = (dblist->shotid != -1);
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr) {
    int stv;
    NCI local_nci;
    int64_t saved_viewdate;
    EXTENDED_ATTRIBUTES attributes;
    int64_t attributes_offset = -1;
    int update_attributes = 0;
    int found_index = -1;
    int64_t index_offset = -1;
    int value_length;
    int64_t value_offset;
    NAMED_ATTRIBUTES_INDEX index, current_index;
    int compress=0;
    status = TreeGetNciLw(info_ptr, nidx, &local_nci);
    compress = 	(local_nci.flags & NciM_COMPRESS_ON_PUT) &&
      !(local_nci.flags & NciM_DO_NOT_COMPRESS);
    if (!(status & 1))
      return status;
    if ((status & 1) && (shot_open && (local_nci.flags & NciM_NO_WRITE_SHOT)))
      status = TreeNOWRITESHOT;
    if ((status & 1) && (!shot_open && (local_nci.flags & NciM_NO_WRITE_MODEL)))
      status = TreeNOWRITEMODEL;
    if (!(status & 1)) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return status;
    }
    if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
      open_status = TreeOpenDatafileW(info_ptr, &stv, 0);
    else
      open_status = 1;
    if (!(open_status & 1)) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return open_status;
    }
    TreeGetViewDate(&saved_viewdate);
    status = TreePutDsc(info_ptr, nid, value, &value_offset, &value_length, compress);
    if (!(status & 1))
      return status;
    /*** See if node is currently using the Extended Nci feature and if so get the current contents of the attributes
         index. If not, make an empty index and flag that a new index needs to be written.
    ****/
    if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) ||
	((TreeGetExtendedAttributes
	  (info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1) == 0)) {
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
	    if (!(status & 1))
	      status = TreeBADRECORD;
	    else if (!(local_nci.flags2 & NciM_NON_VMS)
		     && ((retsize != length) || (nodenum != nidx)))
	      status = TreeBADRECORD;
	    else
	      status = (MdsSerializeDscIn(data, &xd) & 1) ? TreeNORMAL : TreeBADRECORD;
	    free(data);
	    if (status & 1) {
	      status =
		  TreePutDsc(info_ptr, nid, (struct descriptor *)&xd,
			     &attributes.facility_offset[STANDARD_RECORD_FACILITY],
			     &attributes.facility_length[STANDARD_RECORD_FACILITY], compress);
	    }
	    MdsFree1Dx(&xd, 0);
	  }
	  if (length <= 0 || !(status & 1)) {
	    attributes.facility_offset[STANDARD_RECORD_FACILITY] = 0;
	    attributes.facility_length[STANDARD_RECORD_FACILITY] = 0;
	    local_nci.length = 0;
	    local_nci.DATA_INFO.DATA_LOCATION.record_length = 0;
	  }
	}
      }
    } else {
      attributes_offset = RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa);
    }
    /*** See if the node currently has an named attributes header record. If not, make an empty named attributes header and flag that
	 a new one needs to be written.
    ***/
    if (attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY] == -1 ||
	((GetNamedAttributesIndex
	  (info_ptr, attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY], &index) & 1) == 0)) {
      memset(&index, 0, sizeof(index));
      attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY] = -1;
      index_offset = -1;
      index.previous_offset = -1;
      update_attributes = 1;
    } else {
      index_offset = attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY];
    }
    current_index = index;
    /*** See if the node currently has a value for this attribute.
    ***/
    while (index_offset != -1 && found_index == -1) {
      int i;
      size_t j;
      for (i = 0; i < NAMED_ATTRIBUTES_PER_INDEX; i++) {
	size_t len = strlen(xnciname);
	for (j = 0; j < len; j++) {
	  if (tolower(xnciname[j]) != tolower(index.attribute[i].name[j]))
	    break;
	}
	if (j == len && index.attribute[i].name[j] == 0)
	  break;
      }
      if (i < NAMED_ATTRIBUTES_PER_INDEX)
	found_index = i;
      else if (index.previous_offset != -1) {
	int64_t new_offset = index.previous_offset;
	if ((GetNamedAttributesIndex(info_ptr, index.previous_offset, &index) & 1) == 0) {
	  break;
	} else
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
    if ((status & 1) && update_attributes) {
      attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY] = index_offset;
      status = TreePutExtendedAttributes(info_ptr, &attributes, &attributes_offset);
      SeekToRfa(attributes_offset, local_nci.DATA_INFO.DATA_LOCATION.rfa);
      local_nci.flags2 |= NciM_EXTENDED_NCI;
      TreePutNci(info_ptr, nidx, &local_nci, 0);
    } 
    TreeUnLockNci(info_ptr, 0, nidx);
  }
  return status;
}

int _TreeGetXNci(void *dbid, int nid, const char *xnciname, struct descriptor_xd *value);

int TreeGetXNci(int nid, const char *xnciname, struct descriptor_xd *value)
{
  return _TreeGetXNci(*TreeCtx(), nid, xnciname, value);
}

int _TreeGetXNci(void *dbid, int nid, const char *xnciname, struct descriptor_xd *value)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid_ptr = (NID *) & nid;
  int status = TreeFAILURE;
  int open_status;
  TREE_INFO *info_ptr;
  int nidx;
  NODE *node_ptr;
  if (!xnciname || strlen(xnciname) < 1 || strlen(xnciname) > NAMED_ATTRIBUTE_NAME_SIZE)
    return TreeFAILURE;
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  node_ptr = nid_to_node(dblist, nid_ptr);
  if (!node_ptr)
    return TreeNNF;
  //  if (node_ptr->usage != TreeUSAGE_SIGNAL)
  //  return  TreeFAILURE;
  if (dblist->remote) {
    printf("Segmented records are not supported using thick client mode\n");
    return 0;
  }
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr) {
    NCI local_nci;
    int64_t saved_viewdate;
    EXTENDED_ATTRIBUTES attributes;
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
      if (i == len) {
	getnames = 1;
      }
    }
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciW(info_ptr, nidx, &local_nci, 0);
    if (!(status & 1))
      return status;
    if (info_ptr->data_file == 0)
      open_status = TreeOpenDatafileR(info_ptr);
    else
      open_status = 1;
    if (!(open_status & 1)) {
      return open_status;
    }
    if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) ||
	((TreeGetExtendedAttributes
	  (info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1) == 0)) {
      status = TreeFAILURE;
    } else if (attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY] == -1 ||
	       ((GetNamedAttributesIndex
		 (info_ptr, attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY],
		  &index) & 1) == 0)) {
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
  }
  return status;
}

int TreeSetRetrievalQuota(int quota __attribute__ ((unused)))
{
  printf("TreeSetRetrievalQuota is not implemented\n");
  return 0;
}

extern int TreeFixupNid();

int TreePutDsc(TREE_INFO * info, int nid_in, struct descriptor *dsc, int64_t * offset, int *length, int compress)
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
  if (status & 1 && xd.pointer != 0 && xd.pointer->class == CLASS_A && xd.pointer->pointer != 0) {
    struct descriptor_a *ap = (struct descriptor_a *)xd.pointer;
    TreeLockDatafile(info, 0, 0);
    *offset = MDS_IO_LSEEK(info->data_file->put, 0, SEEK_END);
    status =
	(MDS_IO_WRITE(info->data_file->put, ap->pointer, ap->arsize) ==
	 (ssize_t) ap->arsize) ? TreeSUCCESS : TreeFAILURE;
    TreeUnLockDatafile(info, 0, 0);
    *length = ap->arsize;
    MdsFree1Dx(&xd, 0);
  }
  return status;
}

int TreeGetDsc(TREE_INFO * info, int nid, int64_t offset, int length, struct descriptor_xd *dsc)
{
  char *buffer = malloc(length);
  int status = 1;
  int deleted = 1;
  while (status & 1 && deleted) {
    status =
	(MDS_IO_READ_X(info->data_file->get, offset, buffer, length, &deleted) ==
	 length) ? TreeSUCCESS : TreeFAILURE;
    if (status & 1 && deleted)
      status = TreeReopenDatafile(info);
  }
  if (status & 1) {
    status = MdsSerializeDscIn(buffer, dsc);
  }
  if (dsc->pointer)
    status = TreeMakeNidsLocal(dsc->pointer, nid);
  free(buffer);
  return status;
}

static int GetCompressedSegmentRows(TREE_INFO * info, int64_t offset, int *rows)
{
  int length = 60;
  unsigned char buffer[60];
  int status = 1;
  int deleted = 1;
  while (status & 1 && deleted) {
    status =
	(MDS_IO_READ_X(info->data_file->get, offset, buffer, length, &deleted) ==
	 length) ? TreeSUCCESS : TreeFAILURE;
    if (status & 1 && deleted)
      status = TreeReopenDatafile(info);
  }
  if (status & 1) {
    if (buffer[3] == CLASS_CA || buffer[3] == CLASS_A) {
      unsigned char dimct = buffer[11];
      if (dimct == 1) {
	int arsize = swapint(buffer + 12);
	*rows = arsize / swapshort(buffer);
      } else {
	*rows = swapint(buffer + 16 + dimct * 4);
      }
      return 1;
    } else
      return 0;
  }
  return status;
}

static int PutSegmentHeader(TREE_INFO * info, SEGMENT_HEADER * hdr, int64_t * offset)
{
  int status;
  int64_t loffset;
  int j;
  char *next_row_fix = getenv("NEXT_ROW_FIX");
  char buffer[2 * sizeof(char) + 1 * sizeof(short) + 10 * sizeof(int) +
		       3 * sizeof(int64_t)], *bptr = buffer;
  if (*offset == -1) {
    TreeLockDatafile(info, 0, 0);
    *offset = MDS_IO_LSEEK(info->data_file->put, 0, SEEK_END);
    loffset = 0;
  } else {
    TreeLockDatafile(info, 0, *offset);
    loffset = *offset;
    MDS_IO_LSEEK(info->data_file->put, *offset, SEEK_SET);
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
      } else {
	printf("next row not adjusted, requested=%d, next_row=%d\n", fix, hdr->next_row);
      }
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
  status =
      (MDS_IO_WRITE(info->data_file->put, buffer, sizeof(buffer)) ==
       sizeof(buffer)) ? TreeSUCCESS : TreeFAILURE;
  TreeUnLockDatafile(info, 0, loffset);
  return status;
}

static int PutSegmentIndex(TREE_INFO * info, SEGMENT_INDEX * idx, int64_t * offset)
{
  int status;
  int i;
  int64_t loffset;
  char buffer[sizeof(int64_t) + sizeof(int) +
		       SEGMENTS_PER_INDEX * (6 * sizeof(int64_t) + 4 * sizeof(int))], *bptr =
      buffer;
  if (*offset == -1) {
    TreeLockDatafile(info, 0, 0);
    loffset = 0;
    *offset = MDS_IO_LSEEK(info->data_file->put, 0, SEEK_END);
  } else {
    TreeLockDatafile(info, 0, *offset);
    loffset = *offset;
    MDS_IO_LSEEK(info->data_file->put, *offset, SEEK_SET);
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
  status =
      (MDS_IO_WRITE(info->data_file->put, buffer, sizeof(buffer)) ==
       sizeof(buffer)) ? TreeSUCCESS : TreeFAILURE;
  TreeUnLockDatafile(info, 0, loffset);
  return status;
}

int TreePutExtendedAttributes(TREE_INFO * info, EXTENDED_ATTRIBUTES * att, int64_t * offset)
{
  int status;
  int i;
  int64_t loffset;
  char buffer[sizeof(int64_t) + FACILITIES_PER_EA * (sizeof(int64_t) + sizeof(int))],
      *bptr = buffer;
  if (*offset == -1) {
    TreeLockDatafile(info, 0, 0);
    loffset = 0;
    *offset = MDS_IO_LSEEK(info->data_file->put, 0, SEEK_END);
  } else {
    TreeLockDatafile(info, 0, *offset);
    loffset = *offset;
    MDS_IO_LSEEK(info->data_file->put, *offset, SEEK_SET);
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
  status =
      (MDS_IO_WRITE(info->data_file->put, buffer, sizeof(buffer)) ==
       sizeof(buffer)) ? TreeSUCCESS : TreeFAILURE;
  TreeUnLockDatafile(info, 0, loffset);
  return status;
}

static int PutNamedAttributesIndex(TREE_INFO * info, NAMED_ATTRIBUTES_INDEX * index,
				   int64_t * offset)
{
  int status;
  int i;
  int64_t loffset;
  char buffer[sizeof(int64_t) +
		       NAMED_ATTRIBUTES_PER_INDEX * (NAMED_ATTRIBUTE_NAME_SIZE + sizeof(int64_t) +
						     sizeof(int))], *bptr = buffer;
  if (*offset == -1) {
    TreeLockDatafile(info, 0, 0);
    loffset = 0;
    *offset = MDS_IO_LSEEK(info->data_file->put, 0, SEEK_END);
  } else {
    TreeLockDatafile(info, 0, *offset);
    loffset = *offset;
    MDS_IO_LSEEK(info->data_file->put, *offset, SEEK_SET);
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
  status =
      (MDS_IO_WRITE(info->data_file->put, buffer, sizeof(buffer)) ==
       sizeof(buffer)) ? TreeSUCCESS : TreeFAILURE;
  TreeUnLockDatafile(info, 0, loffset);
  return status;
}

static int PutInitialValue(TREE_INFO * info, int *dims, struct descriptor_a *array,
			   int64_t * offset)
{
  int status;
  int length = array->length;
  int i;
  int64_t loffset;
#ifdef WORDS_BIGENDIAN
  char *buffer, *bptr;
#endif
  if (*offset == -1) {
    TreeLockDatafile(info, 0, 0);
    loffset = 0;
    *offset = MDS_IO_LSEEK(info->data_file->put, 0, SEEK_END);
  } else {
    TreeLockDatafile(info, 0, *offset);
    loffset = *offset;
    MDS_IO_LSEEK(info->data_file->put, *offset, SEEK_SET);
  }
  for (i = 0; i < array->dimct; i++)
    length = length * dims[i];
#ifdef WORDS_BIGENDIAN
  buffer = memcpy(malloc(length), array->pointer, length);
  if (array->length > 1 && array->dtype != DTYPE_T && array->dtype != DTYPE_IDENT
      && array->dtype != DTYPE_PATH) {
    switch (array->length) {
    case 2:
      for (i = 0, bptr = buffer; i < length / array->length; i++, bptr += sizeof(short))
	LoadShort(((short *)array->pointer)[i], bptr);
      break;
    case 4:
      for (i = 0, bptr = buffer; i < length / array->length; i++, bptr += sizeof(int))
	LoadInt(((int *)array->pointer)[i], bptr);
      break;
    case 8:
      for (i = 0, bptr = buffer; i < length / array->length; i++, bptr += sizeof(int64_t))
	LoadQuad(((int64_t *) array->pointer)[i], bptr);
      break;
    }
  }
  status =
      (MDS_IO_WRITE(info->data_file->put, buffer, length) == length) ? TreeSUCCESS : TreeFAILURE;
  free(buffer);
#else
  status =
      (MDS_IO_WRITE(info->data_file->put, array->pointer, length) ==
       length) ? TreeSUCCESS : TreeFAILURE;
#endif
  TreeUnLockDatafile(info, 0, loffset);
  return status;
}

static int PutDimensionValue(TREE_INFO * info, int64_t * timestamps, int rows_filled, int ndims,
			     int *dims, int64_t * offset)
{
  int status = 1;
  int length = sizeof(int64_t);
  int64_t loffset;
  unsigned char *buffer = 0;
  if (*offset == -1) {
    TreeLockDatafile(info, 0, 0);
    loffset = 0;
    *offset = MDS_IO_LSEEK(info->data_file->put, 0, SEEK_END);
  } else {
    TreeLockDatafile(info, 0, *offset);
    loffset = *offset;
    MDS_IO_LSEEK(info->data_file->put, *offset, SEEK_SET);
  }
  length = length * dims[ndims - 1] - (sizeof(int64_t) * rows_filled);
  if (length > 0) {
    buffer = malloc(length);
    memset(buffer, 0, length);
  }
  if (rows_filled > 0) {
    status =
      (MDS_IO_WRITE(info->data_file->put, timestamps, rows_filled * sizeof(int64_t)) ==
       (int)(rows_filled * sizeof(int64_t))) ? TreeSUCCESS : TreeFAILURE;
  }
  if (status & 1 && length > 0) {
    status =
	(MDS_IO_WRITE(info->data_file->put, buffer, length) == length) ? TreeSUCCESS : TreeFAILURE;
  }
  if (buffer)
    free(buffer);
  TreeUnLockDatafile(info, 0, loffset);
  return status;
}

static int GetSegmentHeader(TREE_INFO * info, int64_t offset, SEGMENT_HEADER * hdr)
{
  int status = TreeFAILURE;
  int i;
  char buffer[2 * sizeof(char) + 1 * sizeof(short) + 10 * sizeof(int) +
		       3 * sizeof(int64_t)], *bptr;
  if (offset > -1) {
    int deleted = 1;
    status = 1;
    while (status & 1 && deleted) {
      status =
	  (MDS_IO_READ_X(info->data_file->get, offset, buffer, sizeof(buffer), &deleted) ==
	   sizeof(buffer)) ? TreeSUCCESS : TreeFAILURE;
      if (status & 1 && deleted)
	status = TreeReopenDatafile(info);
    }
  }
  if (status == TreeSUCCESS) {
    bptr = buffer;
    hdr->dtype = *bptr++;
    hdr->dimct = *bptr++;
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

static int GetSegmentIndex(TREE_INFO * info, int64_t offset, SEGMENT_INDEX * idx)
{
  int status = TreeFAILURE;
  int i;
  char buffer[sizeof(int64_t) + sizeof(int) +
		       SEGMENTS_PER_INDEX * (6 * sizeof(int64_t) + 4 * sizeof(int))], *bptr;
  if (offset > -1) {
    int deleted = 1;
    status = 1;
    while (status & 1 && deleted) {
      status =
	  (MDS_IO_READ_X(info->data_file->get, offset, buffer, sizeof(buffer), &deleted) ==
	   sizeof(buffer)) ? TreeSUCCESS : TreeFAILURE;
      if (status & 1 && deleted)
	status = TreeReopenDatafile(info);
    }
  }
  if (status == TreeSUCCESS) {
    bptr = buffer;
    idx->previous_offset = swapquad(bptr);
    bptr += sizeof(int64_t);
    idx->first_idx = swapint(bptr);
    bptr += sizeof(int);
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

int TreeGetExtendedAttributes(TREE_INFO * info, int64_t offset, EXTENDED_ATTRIBUTES * att)
{
  int status = TreeFAILURE;
  int i;
  char buffer[sizeof(int64_t) + FACILITIES_PER_EA * (sizeof(int64_t) + sizeof(int))],
      *bptr;
  if (offset > -1) {
    int deleted = 1;
    status = 1;
    while (status & 1 && deleted) {
      status =
	  (MDS_IO_READ_X(info->data_file->get, offset, buffer, sizeof(buffer), &deleted) ==
	   sizeof(buffer)) ? TreeSUCCESS : TreeFAILURE;
      if (status & 1 && deleted)
	status = TreeReopenDatafile(info);
    }
  }
  if (status == TreeSUCCESS) {
    bptr = buffer;
    att->next_ea_offset = swapquad(bptr);
    bptr += sizeof(int64_t);
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

static int GetNamedAttributesIndex(TREE_INFO * info, int64_t offset, NAMED_ATTRIBUTES_INDEX * index)
{
  int status = TreeFAILURE;
  int i;
  char buffer[sizeof(int64_t) +
		       NAMED_ATTRIBUTES_PER_INDEX * (sizeof(int64_t) + sizeof(int) +
						     NAMED_ATTRIBUTE_NAME_SIZE)], *bptr;
  if (offset > -1) {
    int deleted = 1;
    status = 1;
    while (status & 1 && deleted) {
      status =
	  (MDS_IO_READ_X(info->data_file->get, offset, buffer, sizeof(buffer), &deleted) ==
	   sizeof(buffer)) ? TreeSUCCESS : TreeFAILURE;
      if (status & 1 && deleted)
	status = TreeReopenDatafile(info);
    }
  }
  if (status == TreeSUCCESS) {
    bptr = buffer;
    index->previous_offset = swapquad(bptr);
    bptr += sizeof(int64_t);
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

static int __TreeBeginTimestampedSegment(void *dbid, int nid, int64_t * timestamps,
					 struct descriptor_a *initialValue, int idx,
					 int rows_filled);

int _TreeBeginTimestampedSegment(void *dbid, int nid, struct descriptor_a *initialValue, int idx)
{
  return __TreeBeginTimestampedSegment(dbid, nid, 0, initialValue, idx, 0);
}

int TreeBeginTimestampedSegment(int nid, struct descriptor_a *initialValue, int idx)
{
  return _TreeBeginTimestampedSegment(*TreeCtx(), nid, initialValue, idx);
}

int _TreeMakeTimestampedSegment(void *dbid, int nid, int64_t * timestamps,
				struct descriptor_a *initialValue, int idx, int rows_filled)
{
  return __TreeBeginTimestampedSegment(dbid, nid, timestamps, initialValue, idx, rows_filled);
}

int TreeMakeTimestampedSegment(int nid, int64_t * timestamps, struct descriptor_a *initialValue,
			       int idx, int rows_filled)
{
  return _TreeMakeTimestampedSegment(*TreeCtx(), nid, timestamps, initialValue, idx, rows_filled);
}

static int __TreeBeginTimestampedSegment(void *dbid, int nid, int64_t * timestamps,
					 struct descriptor_a *initialValue, int idx,
					 int rows_filled)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid_ptr = (NID *) & nid;
  int status;
  int open_status;
  TREE_INFO *info_ptr;
  int nidx;
  int shot_open;
  NODE *node_ptr;
  A_COEFF_TYPE *a_coeff = (A_COEFF_TYPE *) initialValue;
#ifndef _WIN32
  static int saved_uic = 0;
  if (!saved_uic)
    saved_uic = (getgid() << 16) | getuid();
#endif
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  if (dblist->open_readonly)
    return TreeREADONLY;

  node_ptr = nid_to_node(dblist, nid_ptr);
  if (!node_ptr)
    return TreeNNF;
  //  if (node_ptr->usage != TreeUSAGE_SIGNAL)
  //  return  TreeINVDTPUSG;

  while (initialValue && initialValue->dtype == DTYPE_DSC)
    initialValue = (struct descriptor_a *)initialValue->pointer;
  if (initialValue == NULL || initialValue->class != CLASS_A || initialValue->dimct < 1
      || initialValue->dimct > 8)
    return TreeINVDTPUSG;
  if (dblist->remote) {
    printf("Segmented records are not supported using thick client mode\n");
    return 0;
  }
  shot_open = (dblist->shotid != -1);
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr) {
    int stv;
    NCI local_nci;
    int64_t saved_viewdate;
    EXTENDED_ATTRIBUTES attributes;
    int64_t attributes_offset = -1;
    int update_attributes = 0;
    int add_length = 0;
    int previous_length = -1;
    int d;
    int compress;
    SEGMENT_HEADER segment_header;
    SEGMENT_INDEX segment_index;
    SEGMENT_INFO *sinfo;
    status = TreeCallHook(PutData, info_ptr, nid);
    if (status && !(status & 1))
      return status;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciLw(info_ptr, nidx, &local_nci);
    compress =	(local_nci.flags & NciM_COMPRESS_ON_PUT) &&
      (local_nci.flags & NciM_COMPRESS_SEGMENTS) &&
      !(local_nci.flags & NciM_DO_NOT_COMPRESS);
    if (!(status & 1))
      return status;
    if ((status & 1) && (shot_open && (local_nci.flags & NciM_NO_WRITE_SHOT)))
      status = TreeNOWRITESHOT;
    if ((status & 1) && (!shot_open && (local_nci.flags & NciM_NO_WRITE_MODEL)))
      status = TreeNOWRITEMODEL;
    if (!(status & 1)) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return status;
    }
    if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
      open_status = TreeOpenDatafileW(info_ptr, &stv, 0);
    else
      open_status = 1;
    if (!(open_status & 1)) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return open_status;
    }
    local_nci.flags2 &= ~NciM_DATA_IN_ATT_BLOCK;
    local_nci.dtype = initialValue->dtype;
    local_nci.class = CLASS_R;
    local_nci.time_inserted = TreeTimeInserted();
#ifdef _WIN32
    local_nci.owner_identifier = 0;
#else
    local_nci.owner_identifier = saved_uic;
#endif
     /*** See if node is currently using the Extended Nci feature and if so get the current contents of the attributes
	  index. If not, make an empty index and flag that a new index needs to be written.
     ****/
    if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) ||
	((TreeGetExtendedAttributes
	  (info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1) == 0)) {
      memset(&attributes, -1, sizeof(attributes));
      update_attributes = 1;
    } else {
      attributes_offset = RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa);
      if (attributes.facility_offset[STANDARD_RECORD_FACILITY] != -1) {
	attributes.facility_offset[STANDARD_RECORD_FACILITY] = -1;
	attributes.facility_length[STANDARD_RECORD_FACILITY] = 0;
	update_attributes = 1;
      }
    }
     /*** See if the node currently has an segment header record. If not, make an empty segment header and flag that
	  a new one needs to be written.
     ***/
    if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY] == -1 ||
	((GetSegmentHeader
	  (info_ptr, attributes.facility_offset[SEGMENTED_RECORD_FACILITY],
	   &segment_header) & 1) == 0)) {
      memset(&segment_header, 0, sizeof(segment_header));
      attributes.facility_offset[SEGMENTED_RECORD_FACILITY] = -1;
      segment_header.index_offset = -1;
      segment_header.data_offset = -1;
      segment_header.dim_offset = -1;
      segment_header.idx = -1;
      update_attributes = 1;
    } else if (initialValue->dtype != segment_header.dtype
	       || initialValue->dimct != segment_header.dimct || (initialValue->dimct > 1
								  && memcmp(segment_header.dims,
									    a_coeff->m,
									    (initialValue->dimct -
									     1) * sizeof(int)) !=
								  0)) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return TreeFAILURE;
    }
     /*** See if the node currently has an segment_index record. If not, make an empty segment index and
	  flag that a new one needs to be written.
     ***/
    if ((segment_header.index_offset == -1) ||
	((GetSegmentIndex(info_ptr, segment_header.index_offset, &segment_index) & 1) == 0)) {
      segment_header.index_offset = -1;
      memset(&segment_index, -1, sizeof(segment_index));
      segment_index.first_idx = 0;
    }
    if (idx == -1) {
      segment_header.idx++;
      idx = segment_header.idx;
      add_length = initialValue->arsize;
    } else if (idx < -1 || idx > segment_header.idx) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return TreeBUFFEROVF;
    } else {
      TreeUnLockNci(info_ptr, 0, nidx);
      printf("this is not yet supported\n");
      return TreeFAILURE;
       /***************
Add support for updating an existing segment. add_length=new_length-old_length. potentially use same storage area if
old array is same size.
       *******/
      add_length = 0;
    }
    segment_header.dtype = initialValue->dtype;
    segment_header.dimct = initialValue->dimct;
    segment_header.length = initialValue->length;
    segment_header.next_row = rows_filled;
    segment_header.data_offset = -1;
    segment_header.dim_offset = -1;
    if (segment_header.idx > 0) {
      previous_length = segment_header.length;
      for (d = 0; d < segment_header.dimct; d++)
	previous_length *= segment_header.dims[d];
    }
    if (initialValue->dimct == 1)
      segment_header.dims[0] = initialValue->arsize / initialValue->length;
    else {
      memcpy(segment_header.dims, a_coeff->m, initialValue->dimct * sizeof(int));
    }
     /*****
	  If not the first segment, see if we can reuse the previous segment storage space and compress the previous segment.
     ****/
    if ((segment_header.idx % SEGMENTS_PER_INDEX) > 0 && previous_length == add_length
	&& (local_nci.flags & NciM_COMPRESS_ON_PUT && local_nci.flags & NciM_COMPRESS_SEGMENTS)) {
      int deleted;
      EMPTYXD(xd_data);
      EMPTYXD(xd_dim);
      sinfo = &segment_index.segment[(idx % SEGMENTS_PER_INDEX) - 1];
      TreeUnLockNci(info_ptr, 0, nidx);
      status = _TreeGetSegment(dbid, nid, idx - 1, &xd_data, &xd_dim);
      TreeLockNci(info_ptr, 0, nidx, &deleted);
      if (status & 1) {
	int length;
	int rows;
	A_COEFF_TYPE *data_a = (A_COEFF_TYPE *) xd_data.pointer;
	A_COEFF_TYPE *dim_a = (A_COEFF_TYPE *) xd_dim.pointer;
	rows =
	    (initialValue->dimct ==
	     1) ? initialValue->arsize /
	    initialValue->length : ((A_COEFF_TYPE *) initialValue)->m[initialValue->dimct - 1];
	if (data_a && data_a->class == CLASS_A && data_a->pointer
	    && data_a->arsize >= initialValue->arsize && dim_a && dim_a->class == CLASS_A
	    && dim_a->pointer && dim_a->arsize >= (rows * sizeof(int64_t)) && dim_a->dimct == 1
	    && dim_a->length == sizeof(int64_t) && dim_a->dtype == DTYPE_Q) {
	  segment_header.data_offset = sinfo->data_offset;
	  segment_header.dim_offset = sinfo->dimension_offset;
	  status = TreePutDsc(info_ptr, nid, xd_data.pointer, &sinfo->data_offset, &length, compress);
	  status =
	      TreePutDsc(info_ptr, nid, xd_dim.pointer, &sinfo->dimension_offset,
			 &sinfo->dimension_length, compress);
	  sinfo->start = ((int64_t *) dim_a->pointer)[0];
	  sinfo->end = ((int64_t *) dim_a->pointer)[(dim_a->arsize / dim_a->length) - 1];
	   /*** flag compressed segment by setting high bit in the rows field. ***/
	  sinfo->rows = length | 0x80000000;
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
	}
      }
      MdsFree1Dx(&xd_data, 0);
      MdsFree1Dx(&xd_dim, 0);
    }
    status =
	PutInitialValue(info_ptr, segment_header.dims, initialValue, &segment_header.data_offset);
    status =
	PutDimensionValue(info_ptr, timestamps, rows_filled, initialValue->dimct,
			  segment_header.dims, &segment_header.dim_offset);
    if (idx >= segment_index.first_idx + SEGMENTS_PER_INDEX) {
      memset(&segment_index, -1, sizeof(segment_index));
      segment_index.previous_offset = segment_header.index_offset;
      segment_header.index_offset = -1;
      segment_index.first_idx = idx;
    }
    sinfo = &segment_index.segment[idx % SEGMENTS_PER_INDEX];
    sinfo->start = rows_filled > 0 ? timestamps[0] : 0;
    sinfo->end = rows_filled > 0 ? timestamps[rows_filled - 1] : 0;
    sinfo->data_offset = segment_header.data_offset;
    sinfo->dimension_offset = segment_header.dim_offset;
    sinfo->dimension_length = 0;
    sinfo->rows = segment_header.dims[segment_header.dimct - 1];
    status = PutSegmentIndex(info_ptr, &segment_index, &segment_header.index_offset);
    status =
	PutSegmentHeader(info_ptr, &segment_header,
			 &attributes.facility_offset[SEGMENTED_RECORD_FACILITY]);
    if (update_attributes) {
      status = TreePutExtendedAttributes(info_ptr, &attributes, &attributes_offset);
      SeekToRfa(attributes_offset, local_nci.DATA_INFO.DATA_LOCATION.rfa);
      local_nci.flags2 |= NciM_EXTENDED_NCI;
    }
    local_nci.length += add_length;
    local_nci.flags=local_nci.flags | NciM_SEGMENTED;
    TreePutNci(info_ptr, nidx, &local_nci, 0);
  } else status = TreeNNF;
  TreeUnLockNci(info_ptr, 0, nidx);
  return status;
}

int _TreePutTimestampedSegment(void *dbid, int nid, int64_t * timestamp, struct descriptor_a *data);

int TreePutTimestampedSegment(int nid, int64_t * timestamp, struct descriptor_a *data)
{
  return _TreePutTimestampedSegment(*TreeCtx(), nid, timestamp, data);
}

int _TreePutTimestampedSegment(void *dbid, int nid, int64_t * timestamp, struct descriptor_a *data)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid_ptr = (NID *) & nid;
  int status;
  int open_status;
  TREE_INFO *info_ptr;
  int nidx;
  int shot_open;
  NODE *node_ptr;
  DESCRIPTOR_A(data_a, 0, 0, 0, 0);
  A_COEFF_TYPE *a_coeff;
  //  compress_utility = utility_update == 2;
#ifndef _WIN32
  static int saved_uic = 0;
  if (!saved_uic)
    saved_uic = (getgid() << 16) | getuid();
#endif
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  if (dblist->open_readonly)
    return TreeREADONLY;

  node_ptr = nid_to_node(dblist, nid_ptr);
  if (!node_ptr)
    return TreeNNF;
  //  if (node_ptr->usage != TreeUSAGE_SIGNAL)
  //  return  TreeINVDTPUSG;

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
  a_coeff = (A_COEFF_TYPE *) data;
  if (data == NULL || data->class != CLASS_A || data->dimct < 1 || data->dimct > 8)
    return TreeINVDTPUSG;
  if (dblist->remote) {
    printf("Segmented records are not supported using thick client mode\n");
    return 0;
  }
  shot_open = (dblist->shotid != -1);
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr) {
    int stv;
    int rows_to_insert = -1;
    int bytes_per_row;
    int rows_in_segment;
    int bytes_to_insert;
#ifdef WORDS_BIGENDIAN
    char *buffer, *bptr, *times;
#endif
    int64_t offset;
    NCI local_nci;
    int64_t saved_viewdate;
    EXTENDED_ATTRIBUTES attributes;
    int i;
    int startIdx;
    SEGMENT_HEADER segment_header;
    status = TreeCallHook(PutData, info_ptr, nid);
    if (status && !(status & 1))
      return status;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciLw(info_ptr, nidx, &local_nci);
    if (!(status & 1))
      return status;
    if ((status & 1) && (shot_open && (local_nci.flags & NciM_NO_WRITE_SHOT)))
      status = TreeNOWRITESHOT;
    if ((status & 1) && (!shot_open && (local_nci.flags & NciM_NO_WRITE_MODEL)))
      status = TreeNOWRITEMODEL;
    if (!(status & 1)) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return status;
    }
    if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
      open_status = TreeOpenDatafileW(info_ptr, &stv, 0);
    else
      open_status = 1;
    if (!(open_status & 1)) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return open_status;
    }
       /*** See if node is currently using the Extended Nci feature and if so get the current contents of the attributes
	    index. If not, make an empty index and flag that a new index needs to be written.
       ****/
    if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) ||
	((TreeGetExtendedAttributes
	  (info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1) == 0)) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return TreeNOSEGMENTS;
    }
    if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY] == -1) {
      status = TreeNOSEGMENTS;
    } else
	if (((status =
	      GetSegmentHeader(info_ptr, attributes.facility_offset[SEGMENTED_RECORD_FACILITY],
			       &segment_header)) & 1) == 0) {
      status = status;
    } else if (data->dtype != segment_header.dtype) {
      status = TreeINVDTYPE;
    } else if (a_coeff->dimct == 1 && !((a_coeff->dimct == segment_header.dimct)
					|| (a_coeff->dimct == segment_header.dimct - 1))) {
      status = TreeINVSHAPE;
    } else if (a_coeff->dimct > 1
	       && memcmp(segment_header.dims, a_coeff->m,
			 (segment_header.dimct - 1) * sizeof(int)) != 0) {
      status = TreeINVSHAPE;
    } else if (a_coeff->dimct == 1 && a_coeff->arsize / a_coeff->length != 1
	       && (unsigned int)segment_header.dims[0] < a_coeff->arsize / a_coeff->length) {
      status = TreeINVSHAPE;
    }
    if (!(status & 1)) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return status;
    }
    startIdx = segment_header.next_row;
    for (bytes_per_row = segment_header.length, i = 0; i < segment_header.dimct - 1;
	 bytes_per_row *= segment_header.dims[i], i++) ;
    if (data->dimct < segment_header.dimct) {
      rows_to_insert = 1;
    } else {
      rows_to_insert =
	  (data->dimct == 1) ? data->arsize / data->length : a_coeff->m[a_coeff->dimct - 1];
    }
    if (rows_to_insert <= 0) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return TreeINVSHAPE;
    }
    rows_in_segment = segment_header.dims[segment_header.dimct - 1];
    bytes_to_insert =
	((rows_to_insert >
	  (rows_in_segment - startIdx)) ? (rows_in_segment -
					   startIdx) : rows_to_insert) * bytes_per_row;
    if (bytes_to_insert < 1) {
      TreeUnLockNci(info_ptr, 0, nidx);
      return TreeBUFFEROVF;
    }
    offset = segment_header.data_offset + startIdx * bytes_per_row;
#ifdef WORDS_BIGENDIAN
    buffer = memcpy(malloc(bytes_to_insert), data->pointer, bytes_to_insert);
    times = malloc(sizeof(int64_t) * rows_to_insert);
    if (segment_header.length > 1 && data->dtype != DTYPE_T && data->dtype != DTYPE_IDENT
	&& data->dtype != DTYPE_PATH) {
      switch (segment_header.length) {
      case 2:
	for (i = 0, bptr = buffer; i < bytes_to_insert / segment_header.length;
	     i++, bptr += sizeof(short))
	  LoadShort(((short *)data->pointer)[i], bptr);
	break;
      case 4:
	for (i = 0, bptr = buffer; i < bytes_to_insert / segment_header.length;
	     i++, bptr += sizeof(int))
	  LoadInt(((int *)data->pointer)[i], bptr);
	break;
      case 8:
	for (i = 0, bptr = buffer; i < bytes_to_insert / segment_header.length;
	     i++, bptr += sizeof(int64_t))
	  LoadQuad(((int64_t *) data->pointer)[i], bptr);
	break;
      }
    }
    TreeLockDatafile(info_ptr, 0, offset);
    MDS_IO_LSEEK(info_ptr->data_file->put, offset, SEEK_SET);
    status =
	(MDS_IO_WRITE(info_ptr->data_file->put, buffer, bytes_to_insert) ==
	 bytes_to_insert) ? TreeSUCCESS : TreeFAILURE;
    MDS_IO_LSEEK(info_ptr->data_file->put, segment_header.dim_offset + startIdx * sizeof(int64_t),
		 SEEK_SET);
    for (i = 0, bptr = times; i < rows_to_insert; i++, bptr += sizeof(int64_t))
      LoadQuad(timestamp[i], bptr);
    status =
	(MDS_IO_WRITE(info_ptr->data_file->put, times, sizeof(int64_t) * rows_to_insert) ==
	 (sizeof(int64_t) * rows_to_insert)) ? TreeSUCCESS : TreeFAILURE;
    free(times);
    free(buffer);
#else
    TreeLockDatafile(info_ptr, 0, offset);
    MDS_IO_LSEEK(info_ptr->data_file->put, offset, SEEK_SET);
    status =
	(MDS_IO_WRITE(info_ptr->data_file->put, data->pointer, bytes_to_insert) ==
	 bytes_to_insert) ? TreeSUCCESS : TreeFAILURE;
    MDS_IO_LSEEK(info_ptr->data_file->put, segment_header.dim_offset + startIdx * sizeof(int64_t),
		 SEEK_SET);
    status =
	(MDS_IO_WRITE(info_ptr->data_file->put, timestamp, sizeof(int64_t) * rows_to_insert) ==
	 (int)(sizeof(int64_t) * rows_to_insert)) ? TreeSUCCESS : TreeFAILURE;
#endif
    TreeUnLockDatafile(info_ptr, 0, offset);
    segment_header.next_row = startIdx + bytes_to_insert / bytes_per_row;
    if (status & 1)
      status =
	  PutSegmentHeader(info_ptr, &segment_header,
			   &attributes.facility_offset[SEGMENTED_RECORD_FACILITY]);
    TreeUnLockNci(info_ptr, 0, nidx);
  } else status = TreeNNF;
  return status;
}

static int CopyStandardRecord(TREE_INFO * info_in, TREE_INFO * info_out, int nid, int64_t * offset,
			      int *length, int compress)
{
  EMPTYXD(xd);
  int status;
  status = TreeGetDsc(info_in, nid, *offset, *length, &xd);
  if (status & 1) {
    status = TreePutDsc(info_out, nid, (struct descriptor *)&xd, offset, length, compress);
  }
  MdsFree1Dx(&xd, 0);
  if (!(status & 1)) {
    *offset = -1;
    *length = 0;
  }
  return status;
}

static int CopyNamedAttributes(TREE_INFO * info_in, TREE_INFO * info_out, int nid, int64_t * offset,
			       int *length, int compress)
{
  EMPTYXD(xd);
  NAMED_ATTRIBUTES_INDEX index;
  int status = GetNamedAttributesIndex(info_in, *offset, &index);
  if (status & 1) {
    int i;
    *length=0;
    if (index.previous_offset != -1) {
      CopyNamedAttributes(info_in, info_out, nid, &index.previous_offset, 0, compress);
    }
    for (i = 0; i < NAMED_ATTRIBUTES_PER_INDEX; i++) {
      if (index.attribute[i].name[0] != '\0' && index.attribute[i].offset != -1) {
	status = TreeGetDsc(info_in, nid, index.attribute[i].offset, index.attribute[i].length, &xd);
	if (status & 1) {
	  status = TreePutDsc(info_out, nid, (struct descriptor *)&xd,
			      &index.attribute[i].offset, &index.attribute[i].length, compress);
	  if (!(status & 1)) {
	    memset(index.attribute[i].name, 0, sizeof(index.attribute[i].name));
	    index.attribute[i].offset = -1;
	  }
	  MdsFree1Dx(&xd, 0);
	}
      }
    }
    *offset = -1;
    status = PutNamedAttributesIndex(info_out, &index, offset);
  } else {
    *offset = -1;
  }
  return status;
}

static int DataCopy(TREE_INFO * info_in, TREE_INFO * info_out, int64_t offset_in, int length_in,
		    int64_t * offset_out)
{
  int status = 1;
  if (offset_in != -1 && length_in >= 0) {
    char *data = malloc(length_in);
    int deleted = 1;
    while (status & 1 && deleted) {
      status = MDS_IO_READ_X(info_in->data_file->get, offset_in, data, length_in, &deleted) == length_in;
      if (status & 1 && deleted)
	status = TreeReopenDatafile(info_in);
    }
    if (status) {
      *offset_out = MDS_IO_LSEEK(info_out->data_file->put, 0, SEEK_END);
      status = MDS_IO_WRITE(info_out->data_file->put, data, length_in) == length_in;
    }
    free(data);
    if (!(status & 1)) {
      *offset_out = -1;
    }
  } else {
    *offset_out = -1;
  }
  return status;
}

static int CopySegment(TREE_INFO *info_in, TREE_INFO *info_out, int nid, SEGMENT_HEADER *header, SEGMENT_INFO *sinfo, int compress) {
  int status = 1;
  if (compress) {
    int length;
    EMPTYXD(data_xd);
    EMPTYXD(dim_xd);
    status = ReadSegment(info_in, nid, header, sinfo, &data_xd, &dim_xd);
    if (status & 1) {
      status = TreePutDsc(info_out, nid, data_xd.pointer, &sinfo->data_offset, &length, compress);
      if (status & 1) {
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
      if (status & 1)
	status = DataCopy(info_in, info_out, sinfo->dimension_offset, sinfo->dimension_length,
			  &sinfo->dimension_offset);
    } else {
      int rowlen = header->length, i;
      for (i = 0; i < header->dimct - 1; i++) {
	rowlen = rowlen * header->dims[i];
      }
      if (sinfo->dimension_length == 0) {
	length = sinfo->rows * sizeof(int64_t);
      } else {
	length = sinfo->dimension_length;
      }
      status = DataCopy(info_in, info_out, sinfo->dimension_offset, length, &sinfo->dimension_offset);
      if (status & 1) {
        length = rowlen * sinfo->rows;
        status = DataCopy(info_in, info_out, sinfo->data_offset, length, &sinfo->data_offset);
      }
    }
  }
  if ((status & 1) && (sinfo->start_offset > 0)) {
    status = DataCopy(info_in, info_out, sinfo->start_offset, sinfo->start_length, &sinfo->start_offset);
    if (status & 1) {
      if (sinfo->end_offset > 0) {
	status = DataCopy(info_in, info_out, sinfo->end_offset, sinfo->end_length, &sinfo->end_offset);
      }
    }
  }
  return status;
}
		       
static int CopySegmentIndex(TREE_INFO * info_in, TREE_INFO * info_out, int nid, SEGMENT_HEADER * header,
			    int64_t * index_offset, int64_t * data_offset, int64_t * dim_offset, int compress)
{
  SEGMENT_INDEX index;
  int i;
  int status = GetSegmentIndex(info_in, *index_offset, &index);
  if (status & 1) {
    if (index.previous_offset != -1) {
      status =
	  CopySegmentIndex(info_in, info_out, nid, header, &index.previous_offset, data_offset,
			   dim_offset, compress);
    }
    for (i = 0; (i < SEGMENTS_PER_INDEX) && (status & 1); i++) {
      SEGMENT_INFO *sinfo = &index.segment[i];
      status = CopySegment(info_in, info_out, nid, header, sinfo, compress);
    }
    *index_offset = -1;
    status = PutSegmentIndex(info_out, &index, index_offset);
  }
  return status;
}

static int CopySegmentedRecords(TREE_INFO * info_in, TREE_INFO * info_out, int nid, int64_t * offset,
				int *length, int compress)
{
  SEGMENT_HEADER header;
  int status = GetSegmentHeader(info_in, *offset, &header);
  if (status & 1) {
    *length=0;
    status =
	CopySegmentIndex(info_in, info_out, nid, &header, &header.index_offset, &header.data_offset,
			 &header.dim_offset, compress);
    *offset = -1;
    status = PutSegmentHeader(info_out, &header, offset);
  }
  return status;
}

int TreeCopyExtended(PINO_DATABASE * dbid_in, PINO_DATABASE * dbid_out, int nid, NCI * nci, int compress)
{
  EXTENDED_ATTRIBUTES attributes;
  TREE_INFO *info_in = dbid_in->tree_info, *info_out = dbid_out->tree_info;
  int status;
  int64_t now = -1;
  int64_t offset = -1;
  status =
      TreeGetExtendedAttributes(info_in, RfaToSeek(nci->DATA_INFO.DATA_LOCATION.rfa), &attributes);
  if (status & 1) {
    if (attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY] != -1) {
      CopyNamedAttributes(info_in, info_out, nid,
			  &attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY],
			  &attributes.facility_length[NAMED_ATTRIBUTES_FACILITY], compress);
    }
    if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY] != -1) {
      CopySegmentedRecords(info_in, info_out, nid,
			   &attributes.facility_offset[SEGMENTED_RECORD_FACILITY],
			   &attributes.facility_length[SEGMENTED_RECORD_FACILITY], compress);
    }
    if (attributes.facility_offset[STANDARD_RECORD_FACILITY] != -1) {
      CopyStandardRecord(info_in, info_out, nid,
			 &attributes.facility_offset[STANDARD_RECORD_FACILITY],
			 &attributes.facility_length[STANDARD_RECORD_FACILITY], compress);
    }
    status = TreePutExtendedAttributes(info_out, &attributes, &offset);
    if (status & 1) {
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
  int status = MdsCopyDxXd(start, &TREE_START_CONTEXT);
  if (status & 1) {
    status = MdsCopyDxXd(end, &TREE_END_CONTEXT);
    if (status & 1) {
      status = MdsCopyDxXd(delta, &TREE_DELTA_CONTEXT);
    }
  }
  return status;
}

int TreeResetTimeContext()
{
  EMPTYXD(emptyXd);
  int status = MdsCopyDxXd((struct descriptor *)&emptyXd, &TREE_START_CONTEXT);
  if (status & 1) {
    status = MdsCopyDxXd((struct descriptor *)&emptyXd, &TREE_END_CONTEXT);
    if (status & 1) {
      status = MdsCopyDxXd((struct descriptor *)&emptyXd, &TREE_DELTA_CONTEXT);
    }
  }
  return status;
}

int TreeGetSegmentedRecord(int nid, struct descriptor_xd *data)
{
  return _TreeGetSegmentedRecord(*TreeCtx(), nid, data);
}

int _TreeGetSegmentedRecord(void *dbid, int nid, struct descriptor_xd *data)
{
  static int activated = 0;
  static int (*addr) (void *, int, struct descriptor *, struct descriptor *, struct descriptor *,
		      struct descriptor_xd *);
  int status = 42;
  if (!activated) {
    static DESCRIPTOR(library, "XTreeShr");
    static DESCRIPTOR(routine, "_XTreeGetTimedRecord");
    status = LibFindImageSymbol(&library, &routine, &addr);
    if (status & 1) {
      activated = 1;
    } else {
      fprintf(stderr, "Error activating XTreeShr library. Cannot access segmented records.\n");
      return status;
    }
  }
  status =
      (*addr) (dbid, nid, TREE_START_CONTEXT.pointer, TREE_END_CONTEXT.pointer,
	       TREE_DELTA_CONTEXT.pointer, data);
  return status;
}

int _TreePutRow(void *dbid, int nid, int bufsize, int64_t * timestamp, struct descriptor_a *data);

int TreePutRow(int nid, int bufsize, int64_t * timestamp, struct descriptor_a *data)
{
  return _TreePutRow(*TreeCtx(), nid, bufsize, timestamp, data);
}

int _TreePutRow(void *dbid, int nid, int bufsize, int64_t * timestamp, struct descriptor_a *data)
{
  DESCRIPTOR_A_COEFF(initValue, data->length, data->dtype, 0, 8, 0);
  int status = _TreePutTimestampedSegment(dbid, nid, timestamp, data);
  if (status == TreeNOSEGMENTS || status == TreeBUFFEROVF) {
    status = 1;
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
	  for (i = 0; i < data->dimct; i++) {
	    initValue.m[i] = data_c->m[i];
	  }
	  initValue.m[data->dimct] = bufsize;
	}
      } else if (data->class == CLASS_S || data->class == CLASS_D) {
	initValue.arsize = data->length * bufsize;
	initValue.pointer = initValue.a0 = malloc(initValue.arsize);
	memset(initValue.pointer, 0, initValue.arsize);
	initValue.dimct = 1;
	initValue.m[0] = bufsize;
      } else {
	status = TreeFAILURE;
      }
    } else {
      status = TreeFAILURE;
    }
    if (status & 1) {
      status = _TreeBeginTimestampedSegment(dbid, nid, (struct descriptor_a *)&initValue, -1);
      free(initValue.pointer);
      if (status && 1) {
	status = _TreePutTimestampedSegment(dbid, nid, timestamp, data);
      }
    }
  }
  return status;
}

int _TreeGetSegmentInfo(void *dbid, int nid, int idx, char *dtype, char *dimct, int *dims,
			int *next_row);

int TreeGetSegmentInfo(int nid, int idx, char *dtype, char *dimct, int *dims, int *next_row)
{
  return _TreeGetSegmentInfo(*TreeCtx(), nid, idx, dtype, dimct, dims, next_row);
}

int _TreeGetSegmentInfo(void *dbid, int nid, int idx, char *dtype, char *dimct, int *dims,
			int *next_row)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid_ptr = (NID *) & nid;
  int status = TreeFAILURE;
  int open_status;
  TREE_INFO *info_ptr;
  int nidx;
  NODE *node_ptr;
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  node_ptr = nid_to_node(dblist, nid_ptr);
  if (!node_ptr)
    return TreeNNF;
  if (dblist->remote) {
    printf("Segmented records are not supported using thick client mode\n");
    return 0;
  }
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr) {
    NCI local_nci;
    int64_t saved_viewdate;
    EXTENDED_ATTRIBUTES attributes;
    SEGMENT_HEADER segment_header;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciW(info_ptr, nidx, &local_nci, 0);
    if (!(status & 1))
      return status;
    if (info_ptr->data_file == 0)
      open_status = TreeOpenDatafileR(info_ptr);
    else
      open_status = 1;
    if (!(open_status & 1)) {
      return open_status;
    }
    if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) ||
	((TreeGetExtendedAttributes
	  (info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1) == 0)) {
      status = TreeFAILURE;
    } else if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY] == 0 ||
	       ((GetSegmentHeader
		 (info_ptr, attributes.facility_offset[SEGMENTED_RECORD_FACILITY],
		  &segment_header) & 1) == 0)) {
      status = TreeFAILURE;
    } else {
      SEGMENT_INDEX index;
      if (idx == -1)
	idx = segment_header.idx;
      status = GetSegmentIndex(info_ptr, segment_header.index_offset, &index);
      while ((status & 1) != 0 && idx < index.first_idx && index.previous_offset > 0)
	status = GetSegmentIndex(info_ptr, index.previous_offset, &index);
      if ((status & 1) != 0 && idx >= index.first_idx && idx < index.first_idx + SEGMENTS_PER_INDEX) {
	SEGMENT_INFO *sinfo = &index.segment[idx % SEGMENTS_PER_INDEX];
	if (sinfo->data_offset == -1) {
	  status = TreeFAILURE;
	} else {
	  *dtype = segment_header.dtype;
	  *dimct = segment_header.dimct;
	  memcpy(dims, segment_header.dims, sizeof(segment_header.dims));
	  if (idx == segment_header.idx)
	    *next_row = segment_header.next_row;
	  else if (sinfo->rows < 1) {
	    status = GetCompressedSegmentRows(info_ptr, sinfo->data_offset, next_row);
	  } else
	    *next_row = sinfo->rows;
	  //          dims[segment_header.dimct-1]=*next_row;
	}
      } else {
	status = TreeFAILURE;
      }
    }
  }
  return status;
}

static int segmentInRange(TREE_INFO * info_ptr, int nid,
			  struct descriptor *start,
			  struct descriptor *end,
			  SEGMENT_INFO * sinfo)
{
  int ans = 0;
  if ((start && start->pointer) || (end && end->pointer)) {
    int status = 1;
    STATIC_CONSTANT DESCRIPTOR(tdishr, "TdiShr");
    STATIC_CONSTANT DESCRIPTOR(tdiexecute, "TdiExecute");
    STATIC_THREADSAFE int (*addr) () = 0;
    if (addr == 0)
      status = LibFindImageSymbol(&tdishr, &tdiexecute, &addr);
    if (status & 1) {
      EMPTYXD(segstart);
      EMPTYXD(segend);
      DESCRIPTOR_LONG(ans_d, &ans);
      status = getSegmentLimits(info_ptr, nid, sinfo, &segstart, &segend);
      if (status & 1) {
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
  } else {
    ans = 1;
  }
  return ans;
}

int TreeGetSegments(int nid, struct descriptor *start, struct descriptor *end,
		    struct descriptor_xd *out)
{
  return _TreeGetSegments(*TreeCtx(), nid, start, end, out);
}

int _TreeGetSegments(void *dbid, int nid, struct descriptor *start, struct descriptor *end,
		     struct descriptor_xd *out)
{
  /*** Get all the segments in an apd which contain data between the start and end times specified ***/
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid_ptr = (NID *) & nid;
  int status = TreeFAILURE;
  int open_status;
  TREE_INFO *info_ptr;
  int nidx;
  NODE *node_ptr;
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  node_ptr = nid_to_node(dblist, nid_ptr);
  if (!node_ptr)
    return TreeNNF;
  if (dblist->remote) {
    printf("Segmented records are not supported using thick client mode\n");
    return 0;
  }
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr) {
    NCI local_nci;
    int64_t saved_viewdate;
    EXTENDED_ATTRIBUTES attributes;
    SEGMENT_HEADER segment_header;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciW(info_ptr, nidx, &local_nci, 0);
    if (!(status & 1))
      return status;
    if (info_ptr->data_file == 0)
      open_status = TreeOpenDatafileR(info_ptr);
    else
      open_status = 1;
    if (!(open_status & 1)) {
      return open_status;
    }
    if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) ||
	((TreeGetExtendedAttributes
	  (info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1) == 0)) {
      status = TreeFAILURE;
    } else if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY] == 0 ||
	       ((GetSegmentHeader
		 (info_ptr, attributes.facility_offset[SEGMENTED_RECORD_FACILITY],
		  &segment_header) & 1) == 0)) {
      status = TreeFAILURE;
    } else {
      SEGMENT_INDEX index;
      int numsegs = segment_header.idx + 1;
      struct descriptor **dptr = malloc(sizeof(struct descriptor *) * numsegs * 2);
      int idx;
      int segfound = 0;
      int apd_idx = 0;
      DESCRIPTOR_APD(apd, DTYPE_LIST, dptr, numsegs * 2);
      memset(dptr, 0, sizeof(struct descriptor *) * numsegs * 2);
      status = GetSegmentIndex(info_ptr, segment_header.index_offset, &index);
      for (idx = numsegs; (status & 1) && idx > 0; idx--) {
	int segidx = idx - 1;
	while ((status & 1) && segidx < index.first_idx && index.previous_offset > 0) {
	  status = GetSegmentIndex(info_ptr, index.previous_offset, &index);
	}
	if (!(status & 1))
	  break;
	else {
	  SEGMENT_INFO *sinfo = &index.segment[segidx % SEGMENTS_PER_INDEX];
	  if (segmentInRange(info_ptr, nid, start, end, sinfo)) {
	    EMPTYXD(segment);
	    EMPTYXD(dim);
	    segfound = 1;
	    status = ReadSegment(info_ptr, nid, &segment_header, sinfo, &segment, &dim);
	    if (status & 1) {
	      apd.pointer[apd_idx] = malloc(sizeof(struct descriptor_xd));
	      memcpy(apd.pointer[apd_idx++], &segment, sizeof(struct descriptor_xd));
	      apd.pointer[apd_idx] = malloc(sizeof(struct descriptor_xd));
	      memcpy(apd.pointer[apd_idx++], &dim, sizeof(struct descriptor_xd));
	    } else {
	      MdsFree1Dx(&segment, 0);
	      MdsFree1Dx(&dim, 0);
	    }
	  } else {
	    if (segfound)
	      break;
	  }
	}
      }
      if (status & 1) {
	int i;
	apd.arsize = apd_idx * sizeof(struct descriptor *);
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
      if (apd.pointer) {
	free(apd.pointer);
      }
    }
  }
  return status;
}
