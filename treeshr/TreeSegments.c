#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <treeshr.h>
#include <usagedef.h>
#include <libroutines.h>
#include <mdsshr.h>
#include "treeshrp.h"
extern void **TreeCtx();
#ifdef __tolower
#undef __tolower
#endif
#define __tolower(c) (((c) >= 'A' && (c) <= 'Z') ? (c) | 0x20 : (c))

typedef ARRAY_COEFF(char,8) A_COEFF_TYPE;
static int PutNamedAttributesIndex(TREE_INFO *info, NAMED_ATTRIBUTES_INDEX *index, _int64 *offset);
static int PutSegmentHeader(TREE_INFO *info_ptr, SEGMENT_HEADER *hdr, _int64 *offset);
static int PutSegmentIndex(TREE_INFO *info_ptr, SEGMENT_INDEX   *idx, _int64 *offset);
static int PutInitialValue(TREE_INFO *info_ptr, int *dims, struct descriptor_a *array, _int64 *offset);
static int PutDimensionValue(TREE_INFO *info_ptr, _int64 *timestamps, int rows_filled, int ndims,  int *dims, _int64 *offset);
static int GetSegmentHeader(TREE_INFO *info_ptr, _int64 offset,SEGMENT_HEADER *hdr);
static int GetSegmentIndex(TREE_INFO *info_ptr, _int64 offset, SEGMENT_INDEX *idx);
static int GetNamedAttributesIndex(TREE_INFO *info_ptr, _int64 offset, NAMED_ATTRIBUTES_INDEX *index);
static int __TreeBeginSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end, 
			      struct descriptor *dimension,struct descriptor_a *initialValue, int idx, int rows_filled);
int _TreeBeginSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end,
		      struct descriptor *dimension,struct descriptor_a *initialValue, int idx) {
  return __TreeBeginSegment(dbid,nid,start,end,dimension,initialValue,idx,0);
}

int TreeBeginSegment(int nid, struct descriptor *start, struct descriptor *end, 
		     struct descriptor *dimension,
		     struct descriptor_a *initialValue, int idx) {
  return  _TreeBeginSegment(*TreeCtx(), nid, start, end, dimension, initialValue, idx);
}

int _TreeMakeSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end, 
		      struct descriptor *dimension,
		      struct descriptor_a *initialValue, int idx, int rows_filled) {
  return __TreeBeginSegment(dbid,nid,start,end,dimension,initialValue,idx,rows_filled);
}

int TreeMakeSegment(int nid, struct descriptor *start, struct descriptor *end, 
		     struct descriptor *dimension,
		     struct descriptor_a *initialValue, int idx, int rows_filled) {
  return  _TreeMakeSegment(*TreeCtx(), nid, start, end, dimension, initialValue, idx,rows_filled);
}

static int __TreeBeginSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end, 
		      struct descriptor *dimension,
		      struct descriptor_a *initialValue, int idx, int rows_filled) {
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID       *nid_ptr = (NID *)&nid;
  int       status;
  int       open_status;
  TREE_INFO *info_ptr;
  int       nidx;
  static int saved_uic = 0;
  int       shot_open;
  NODE      *node_ptr;
  A_COEFF_TYPE *a_coeff = (A_COEFF_TYPE *)initialValue;
  struct descriptor *dsc;
  //  compress_utility = utility_update == 2;
#if !defined(HAVE_WINDOWS_H) && !defined(HAVE_VXWORKS_H)
  if (!saved_uic)
    saved_uic = (getgid() << 16) | getuid();
#endif
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  if (dblist->open_readonly)
    return TreeREADONLY;

  nid_to_node(dblist, nid_ptr, node_ptr);
  if (!node_ptr)
    return TreeNNF;
  //  if (node_ptr->usage != TreeUSAGE_SIGNAL)
  //  return  TreeINVDTPUSG;

  while (initialValue && initialValue->dtype == DTYPE_DSC)
    initialValue = (struct descriptor_a *)initialValue->pointer;
  if (initialValue == NULL || initialValue->class != CLASS_A || initialValue->dimct < 1 || initialValue->dimct > 8)
    return TreeINVDTPUSG;
  if (dblist->remote) {
    printf("Segmented records are not supported using thick client mode\n");
    return 0;
  }
  shot_open = (dblist->shotid != -1);
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr)
  {
    int       stv;
    NCI       local_nci;
    _int64    saved_viewdate;
    EXTENDED_ATTRIBUTES attributes;
    _int64 attributes_offset=-1;
    int    update_attributes=0;
    int add_length=0;
    int previous_length=-1;
    int d;
    SEGMENT_HEADER segment_header;
    SEGMENT_INDEX segment_index;
    SEGMENT_INFO *sinfo;
    status = TreeCallHook(PutData,info_ptr,nid);
    if (status && !(status & 1))
      return status;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciLw(info_ptr, nidx, &local_nci);
    if (!(status & 1))
      return status;
    if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
      open_status = TreeOpenDatafileW(info_ptr, &stv, 0);
    else
      open_status = 1;
    if (!(open_status & 1)){
      TreeUnLockNci(info_ptr,0,nidx);
      return open_status;
    }
    local_nci.flags2 &= ~NciM_DATA_IN_ATT_BLOCK;
    local_nci.class = CLASS_R;
    local_nci.dtype = initialValue->dtype;
    local_nci.time_inserted=TreeTimeInserted();
    local_nci.owner_identifier=saved_uic;
    /*** See if node is currently using the Extended Nci feature and if so get the current contents of the attributes
         index. If not, make an empty index and flag that a new index needs to be written.
    ****/
    if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) || 
	((TreeGetExtendedAttributes(info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1)==0)) {
      memset(&attributes,-1,sizeof(attributes));
      update_attributes=1;
    }
    else {
      attributes_offset=RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa);
      if (attributes.facility_offset[STANDARD_RECORD_FACILITY] != -1) {
	attributes.facility_offset[STANDARD_RECORD_FACILITY] = -1;
	attributes.facility_length[STANDARD_RECORD_FACILITY] = 0;
	update_attributes=1;
      }
    }
    /*** See if the node currently has an segment header record. If not, make an empty segment header and flag that
	 a new one needs to be written.
    ***/
    if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY]==-1 ||
       ((GetSegmentHeader(info_ptr, attributes.facility_offset[SEGMENTED_RECORD_FACILITY],&segment_header)&1)==0)) {
      memset(&segment_header,0,sizeof(segment_header));
      attributes.facility_offset[SEGMENTED_RECORD_FACILITY]=-1;
      segment_header.index_offset=-1;
      segment_header.idx = -1;
      update_attributes=1;
    } else if (initialValue->dtype != segment_header.dtype || initialValue->dimct != segment_header.dimct ||
	       (initialValue->dimct > 1 && memcmp(segment_header.dims,a_coeff->m,(initialValue->dimct-1)*sizeof(int)) != 0)) {
      TreeUnLockNci(info_ptr,0,nidx);
      return TreeFAILURE;
    }
    /*** See if the node currently has an segment_index record. If not, make an empty segment index and
	 flag that a new one needs to be written.
    ***/
    if ((segment_header.index_offset == -1) || 
	((GetSegmentIndex(info_ptr,segment_header.index_offset,&segment_index)&1)==0)) {
      segment_header.index_offset = -1;
      memset(&segment_index,-1,sizeof(segment_index));
      segment_index.first_idx=0;
    }
    if (idx == -1) {
      segment_header.idx++;
      idx=segment_header.idx;
      add_length=initialValue->arsize;
    } else if (idx < -1 || idx > segment_header.idx) {
      TreeUnLockNci(info_ptr,0,nidx);
      return TreeBUFFEROVF;
    } else {
      printf("this is not yet supported\n");
      return TreeFAILURE;
      /***************
Add support for updating an existing segment. add_length=new_length-old_length. potentially use same storage area if
old array is same size.
      *******/
      add_length=0;
    }
    segment_header.data_offset=-1;
    segment_header.dim_offset=-1;
    segment_header.dtype=initialValue->dtype;
    segment_header.dimct=initialValue->dimct;
    segment_header.length=initialValue->length;
    if (segment_header.idx > 0) {
      previous_length=segment_header.length;
      for (d=0;d<segment_header.dimct;d++)
	previous_length *= segment_header.dims[d];
    }
    if (initialValue->dimct == 1)
      segment_header.dims[0]=initialValue->arsize/initialValue->length;
    else {
      memcpy(segment_header.dims,a_coeff->m,initialValue->dimct*sizeof(int));
    }
    //    rows_filled=(rows_filled < 0) ? 0 : 
    //  ((rows_filled > segment_header.dims[initialValue->dimct]) ? segment_header.dims[initialValue->dimct] : rows_filled);
    segment_header.next_row=rows_filled;
    /*****
	  If not the first segment, see if we can reuse the previous segment storage space and compress the previous segment.
    ****/
    if ((segment_header.idx % SEGMENTS_PER_INDEX) > 0 && previous_length == add_length && (local_nci.flags & NciM_COMPRESS_ON_PUT && local_nci.flags & NciM_COMPRESS_SEGMENTS)) {
      int deleted;
      EMPTYXD(xd_data);
      EMPTYXD(xd_dim);
      sinfo = &segment_index.segment[(idx % SEGMENTS_PER_INDEX) - 1];
      TreeUnLockNci(info_ptr,0,nidx);
      status = _TreeGetSegment(dbid, nid, idx-1, &xd_data, &xd_dim);
      TreeLockNci(info_ptr,0,nidx,&deleted);
      if (status & 1) {
	int length;
	segment_header.data_offset=sinfo->data_offset;
	status = TreePutDsc(info_ptr,nid,xd_data.pointer,&sinfo->data_offset,&length);
	/*** flag compressed segment by setting high bit in the rows field. ***/
	sinfo->rows = length | 0x80000000;
      }
      MdsFree1Dx(&xd_data,0);
      MdsFree1Dx(&xd_dim,0);
    }
    status = PutInitialValue(info_ptr,segment_header.dims,initialValue,&segment_header.data_offset);
    if (idx >= segment_index.first_idx+SEGMENTS_PER_INDEX) {
      memset(&segment_index,-1,sizeof(segment_index));
      segment_index.previous_offset=segment_header.index_offset;
      segment_header.index_offset=-1;
      segment_index.first_idx=idx;
    }
    sinfo=&segment_index.segment[idx % SEGMENTS_PER_INDEX];
    for (dsc=start;dsc != 0 && dsc->pointer != 0 && dsc->dtype == DTYPE_DSC;dsc=(struct descriptor *)dsc->pointer);
    if (dsc != 0 && dsc->pointer != 0 && (dsc->dtype == DTYPE_Q || dsc->dtype == DTYPE_QU)) {
      sinfo->start=*(_int64 *)dsc->pointer;
      sinfo->start_offset=-1;
      sinfo->start_length=0;
    }
    else if (start != 0) {
      sinfo->start=-1;
      status = TreePutDsc(info_ptr,nid,start,&sinfo->start_offset,&sinfo->start_length);
    }
    for (dsc=end;dsc != 0 && dsc->pointer != 0 && dsc->dtype==DTYPE_DSC;dsc=(struct descriptor *)dsc->pointer);
    if (dsc != 0 && dsc->dtype == DTYPE_Q || dsc->dtype == DTYPE_QU) {
      sinfo->end=*(_int64 *)dsc->pointer;
      sinfo->end_offset=-1;
      sinfo->end_length=0;
    }
    else if (end != 0) {
      sinfo->end=-1;
      status = TreePutDsc(info_ptr,nid,end,&sinfo->end_offset,&sinfo->end_length);
    }
    if (dimension != 0) {
      status = TreePutDsc(info_ptr,nid,dimension,&sinfo->dimension_offset,&sinfo->dimension_length);
    }
    sinfo->data_offset=segment_header.data_offset;
    sinfo->rows=segment_header.dims[segment_header.dimct-1];
    status = PutSegmentIndex(info_ptr,&segment_index,&segment_header.index_offset);
    status = PutSegmentHeader(info_ptr,&segment_header,&attributes.facility_offset[SEGMENTED_RECORD_FACILITY]);
    if (update_attributes) {
      status = TreePutExtendedAttributes(info_ptr,&attributes,&attributes_offset);
      SeekToRfa(attributes_offset,local_nci.DATA_INFO.DATA_LOCATION.rfa);
      local_nci.flags2 |= NciM_EXTENDED_NCI;
    }
    local_nci.length += add_length;
    TreePutNci(info_ptr,nidx,&local_nci,0);
    TreeUnLockNci(info_ptr,0,nidx);
  }
  return status;
}


int _TreeUpdateSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end, struct descriptor *dimension,
			     int idx);

int TreeUpdateSegment(int nid, struct descriptor *start, struct descriptor *end, struct descriptor *dimension,
		      int idx) {
  return _TreeUpdateSegment(*TreeCtx(), nid, start, end, dimension, idx);
}

int _TreeUpdateSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end, struct descriptor *dimension,
		      int idx) {
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID       *nid_ptr = (NID *)&nid;
  int       status;
  int       open_status;
  TREE_INFO *info_ptr;
  int       nidx;
  static int saved_uic = 0;
  int       shot_open;
  NODE      *node_ptr;
  struct descriptor *dsc;
  //  compress_utility = utility_update == 2;
#if !defined(HAVE_WINDOWS_H) && !defined(HAVE_VXWORKS_H)
  if (!saved_uic)
    saved_uic = (getgid() << 16) | getuid();
#endif
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  if (dblist->open_readonly)
    return TreeREADONLY;

  nid_to_node(dblist, nid_ptr, node_ptr);
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
  if (info_ptr)
  {
    int       stv;
    NCI       local_nci;
    _int64    saved_viewdate;
    _int64 index_offset;
    EXTENDED_ATTRIBUTES attributes;
    _int64 attributes_offset=-1;
    SEGMENT_HEADER segment_header;
    SEGMENT_INDEX segment_index;
    SEGMENT_INFO *sinfo;
    status = TreeCallHook(PutData,info_ptr,nid);
    if (status && !(status & 1))
      return status;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciLw(info_ptr, nidx, &local_nci);
    if (!(status & 1))
      return status;
    if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
      open_status = TreeOpenDatafileW(info_ptr, &stv, 0);
    else
      open_status = 1;
    if (!(open_status & 1)) {
      TreeUnLockNci(info_ptr,0,nidx);
      return open_status;
    }
    /*** See if node is currently using the Extended Nci feature and if so get the current contents of the attributes
         index. If not, make an empty index and flag that a new index needs to be written.
    ****/
    if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) || 
	((TreeGetExtendedAttributes(info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1)==0)) {
      TreeUnLockNci(info_ptr,0,nidx);
      return TreeFAILURE;
    }
    else {
      attributes_offset=RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa);
    }
    /*** See if the node currently has an segment header record. If not, make an empty segment header and flag that
	 a new one needs to be written.
    ***/
    if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY]==-1 ||
       ((GetSegmentHeader(info_ptr, attributes.facility_offset[SEGMENTED_RECORD_FACILITY],&segment_header)&1)==0)) {
      TreeUnLockNci(info_ptr,0,nidx);
      return TreeFAILURE;
    }
    /*** See if the node currently has an segment_index record. If not, make an empty segment index and
	 flag that a new one needs to be written.
    ***/
    if ((segment_header.index_offset == -1) || 
	((GetSegmentIndex(info_ptr,segment_header.index_offset,&segment_index)&1)==0)) {
      TreeUnLockNci(info_ptr,0,nidx);
      return TreeFAILURE;
    }
    if (idx < 0 || idx > segment_header.idx) {
      TreeUnLockNci(info_ptr,0,nidx);
      return TreeBUFFEROVF;
    }
    for (index_offset=segment_header.index_offset;
	 (status &1) != 0 && idx < segment_index.first_idx && segment_index.previous_offset > 0;index_offset=segment_index.previous_offset)
      status = GetSegmentIndex(info_ptr, segment_index.previous_offset, &segment_index);
    if (!(status & 1) || (idx < segment_index.first_idx)) {
      TreeUnLockNci(info_ptr,0,nidx);
      return TreeFAILURE;
    }
    sinfo=&segment_index.segment[idx % SEGMENTS_PER_INDEX];
    for (dsc=start;dsc != 0 && dsc->pointer != 0 && dsc->dtype == DTYPE_DSC;dsc=(struct descriptor *)dsc->pointer);
    if (dsc != 0 && dsc->pointer != 0 && (dsc->dtype == DTYPE_Q || dsc->dtype == DTYPE_QU)) {
      sinfo->start=*(_int64 *)dsc->pointer;
      sinfo->start_offset=-1;
      sinfo->start_length=0;
    }
    else if (start != 0) {
      sinfo->start=-1;
      status = TreePutDsc(info_ptr,nid,start,&sinfo->start_offset,&sinfo->start_length);
    }
    for (dsc=end;dsc != 0 && dsc->pointer != 0 && dsc->dtype==DTYPE_DSC;dsc=(struct descriptor *)dsc->pointer);
    if (dsc != 0 && dsc->dtype == DTYPE_Q || dsc->dtype == DTYPE_QU) {
      sinfo->end=*(_int64 *)dsc->pointer;
      sinfo->end_offset=-1;
      sinfo->end_length=0;
    }
    else if (end != 0) {
      sinfo->end=-1;
      status = TreePutDsc(info_ptr,nid,end,&sinfo->end_offset,&sinfo->end_length);
    }
    if (dimension != 0) {
      status = TreePutDsc(info_ptr,nid,dimension,&sinfo->dimension_offset,&sinfo->dimension_length);
    }
    sinfo->data_offset=segment_header.data_offset;
    sinfo->rows=segment_header.dims[segment_header.dimct-1];
    status = PutSegmentIndex(info_ptr,&segment_index,&index_offset);
    TreeUnLockNci(info_ptr,0,nidx);
  }
  return status;
}

int _TreePutSegment(void *dbid, int nid, int startIdx, struct descriptor_a *data);
int TreePutSegment(int nid, int startIdx, struct descriptor_a *data) {
  return _TreePutSegment(*TreeCtx(), nid, startIdx, data);
}

int _TreePutSegment(void *dbid, int nid, int startIdx, struct descriptor_a *data) {
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID       *nid_ptr = (NID *)&nid;
  int       status;
  int       open_status;
  TREE_INFO *info_ptr;
  int       nidx;
  static int saved_uic = 0;
  int       shot_open;
  NODE      *node_ptr;
  DESCRIPTOR_A(data_a, 0, 0, 0, 0);
  A_COEFF_TYPE *a_coeff;
  //  compress_utility = utility_update == 2;
#if !defined(HAVE_WINDOWS_H) && !defined(HAVE_VXWORKS_H)
  if (!saved_uic)
    saved_uic = (getgid() << 16) | getuid();
#endif
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  if (dblist->open_readonly)
    return TreeREADONLY;

  nid_to_node(dblist, nid_ptr, node_ptr);
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
    data_a.dtype= data->dtype;
    data_a.arsize=data->length;
    data_a.dimct=1;
    data=(struct descriptor_a *)&data_a;
  }
  a_coeff = (A_COEFF_TYPE *)data;
  if (data == NULL || data->class != CLASS_A || data->dimct < 1 || data->dimct > 8)
    return TreeINVDTPUSG;
  if (dblist->remote) {
    printf("Segmented records are not supported using thick client mode\n");
    return 0;
  }
  shot_open = (dblist->shotid != -1);
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr)
  {
    int stv;
    int rows_to_insert;
    int bytes_per_row;
    int rows_in_segment;
    unsigned int bytes_to_insert;
#ifdef WORDS_BIGENDIAN
    unsigned char *buffer,*bptr;
#endif
    _int64 offset;
    NCI       local_nci;
    _int64    saved_viewdate;
    EXTENDED_ATTRIBUTES attributes;
    int i;
    SEGMENT_HEADER segment_header;
    status = TreeCallHook(PutData,info_ptr,nid);
    if (status && !(status & 1))
      return status;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciLw(info_ptr, nidx, &local_nci);
    if (!(status & 1))
      return status;
    if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
      open_status = TreeOpenDatafileW(info_ptr, &stv, 0);
    else
      open_status = 1;
    if (!(open_status & 1)) {
      TreeUnLockNci(info_ptr,0,nidx);
      return open_status;
    }
    /*** See if node is currently using the Extended Nci feature and if so get the current contents of the attributes
         index. If not, make an empty index and flag that a new index needs to be written.
    ****/
    if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) || 
	((TreeGetExtendedAttributes(info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1)==0)) {
      TreeUnLockNci(info_ptr,0,nidx);
      return TreeFAILURE;
    }
    /*** See if the node currently has an segment header record. If not, make an empty segment header and flag that
	 a new one needs to be written.
    ***/
    if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY]==-1 ||
       ((GetSegmentHeader(info_ptr, attributes.facility_offset[SEGMENTED_RECORD_FACILITY],&segment_header)&1)==0)) {
      TreeUnLockNci(info_ptr,0,nidx);
      return TreeFAILURE;
    } else if (data->dtype != segment_header.dtype || (data->dimct != segment_header.dimct && data->dimct != segment_header.dimct-1) ||
	       (data->dimct > 1 && memcmp(segment_header.dims,a_coeff->m,(data->dimct-1)*sizeof(int)) != 0)) {
      TreeUnLockNci(info_ptr,0,nidx);
      return TreeFAILURE;
    }
    if (startIdx == -1) {
      startIdx=segment_header.next_row;
    } else if (startIdx < -1 || startIdx > segment_header.dims[segment_header.dimct-1]) {
      TreeUnLockNci(info_ptr,0,nidx);
      return TreeBUFFEROVF;
    }
    for (bytes_per_row=segment_header.length,i=0;i<segment_header.dimct-1;bytes_per_row *= segment_header.dims[i],i++);
    if (data->dimct < segment_header.dimct) {
      rows_to_insert = 1;
    } else {
      rows_to_insert = (data->dimct == 1) ? data->arsize/data->length : a_coeff->m[a_coeff->dimct-1];
    }
    rows_in_segment = segment_header.dims[segment_header.dimct-1];
    bytes_to_insert=((rows_to_insert > (rows_in_segment - startIdx)) ? (rows_in_segment-startIdx) : rows_to_insert) * bytes_per_row;
    if (bytes_to_insert < data->arsize) {
      TreeUnLockNci(info_ptr,0,nidx);
      return TreeBUFFEROVF;
    }  
    offset=segment_header.data_offset+startIdx*bytes_per_row;
#ifdef WORDS_BIGENDIAN
    buffer=memcpy(malloc(bytes_to_insert),data->pointer,bytes_to_insert);
    if (segment_header.length > 1 && data->dtype != DTYPE_T && data->dtype != DTYPE_IDENT && data->dtype != DTYPE_PATH) {
      switch (segment_header.length) {
      case 2: 
	for (i=0,bptr=buffer;i<bytes_to_insert/segment_header.length;i++,bptr+=sizeof(short))
	  LoadShort(((short *)data->pointer)[i],bptr);
	break;
      case 4:
	for (i=0,bptr=buffer;i<bytes_to_insert/segment_header.length;i++,bptr+=sizeof(int))
	  LoadInt(((int *)data->pointer)[i],bptr);
	break;
      case 8:
	for (i=0,bptr=buffer;i<bytes_to_insert/segment_header.length;i++,bptr+=sizeof(_int64))
	  LoadQuad(((_int64 *)data->pointer)[i],bptr);
	break;
      }
    }
    TreeLockDatafile(info_ptr,0,offset);
    MDS_IO_LSEEK(info_ptr->data_file->put,offset,SEEK_SET);
    status = (MDS_IO_WRITE(info_ptr->data_file->put,buffer,bytes_to_insert) == bytes_to_insert) ? TreeSUCCESS : TreeFAILURE;
    free(buffer);
#else
    TreeLockDatafile(info_ptr,0,offset);
    MDS_IO_LSEEK(info_ptr->data_file->put,offset,SEEK_SET);
    status = (MDS_IO_WRITE(info_ptr->data_file->put,data->pointer,bytes_to_insert) == bytes_to_insert) ? TreeSUCCESS : TreeFAILURE;
    TreeUnLockDatafile(info_ptr,0,offset);
#endif
    if (startIdx==segment_header.next_row) {
       segment_header.next_row+=bytes_to_insert/bytes_per_row;
    }
    if (status & 1)
      status = PutSegmentHeader(info_ptr,&segment_header,&attributes.facility_offset[SEGMENTED_RECORD_FACILITY]);
    TreeUnLockNci(info_ptr,0,nidx);
  }
  return status;
}

int _TreeGetNumSegments(void *dbid, int nid, int *num);

int TreeGetNumSegments(int nid, int *num) {
  return  _TreeGetNumSegments(*TreeCtx(), nid, num);
}

int _TreeGetNumSegments(void *dbid, int nid, int *num) {
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID       *nid_ptr = (NID *)&nid;
  int       status=TreeFAILURE;
  int       open_status;
  TREE_INFO *info_ptr;
  int       nidx;
  NODE      *node_ptr;
  *num=0;
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  nid_to_node(dblist, nid_ptr, node_ptr);
  *num=0;
  if (!node_ptr)
    return TreeNNF;
  if (dblist->remote) {
    printf("Segmented records are not supported using thick client mode\n");
    return 0;
  }
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr)
  {
    NCI       local_nci;
    _int64    saved_viewdate;
    EXTENDED_ATTRIBUTES attributes;
    SEGMENT_HEADER segment_header;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciW(info_ptr, nidx, &local_nci,0);
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
	((TreeGetExtendedAttributes(info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1)==0)) {
      *num=0;
    } else if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY]==0 ||
       ((GetSegmentHeader(info_ptr, attributes.facility_offset[SEGMENTED_RECORD_FACILITY],&segment_header)&1)==0)) {
      *num=0;
    }
    else
      *num=segment_header.idx+1;
  }
  return status;
}

int TreeGetSegment(int nid, int idx, struct descriptor_xd *segment, struct descriptor_xd *dim) {
  return _TreeGetSegment(*TreeCtx(), nid, idx, segment, dim);
}

int _TreeGetSegment(void *dbid, int nid, int idx, struct descriptor_xd *segment, struct descriptor_xd *dim) {
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID       *nid_ptr = (NID *)&nid;
  int       status=TreeFAILURE;
  int       open_status;
  TREE_INFO *info_ptr;
  int       nidx;
  NODE      *node_ptr;
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  nid_to_node(dblist, nid_ptr, node_ptr);
  if (!node_ptr)
    return TreeNNF;
  if (dblist->remote) {
    printf("Segmented records are not supported using thick client mode\n");
    return 0;
  }
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr)
  {
    NCI       local_nci;
    _int64    saved_viewdate;
    EXTENDED_ATTRIBUTES attributes;
    SEGMENT_HEADER segment_header;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciW(info_ptr, nidx, &local_nci,0);
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
        ((TreeGetExtendedAttributes(info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1)==0)) {
      status = TreeFAILURE;
    } else if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY]==0 ||
       ((GetSegmentHeader(info_ptr, attributes.facility_offset[SEGMENTED_RECORD_FACILITY],&segment_header)&1)==0)) {
      status = TreeFAILURE;
    }
    else {
      SEGMENT_INDEX index;
      if (idx == -1)
	idx=segment_header.idx;
      status = GetSegmentIndex(info_ptr, segment_header.index_offset, &index);
      while ((status &1) != 0 && idx < index.first_idx && index.previous_offset > 0)
        status = GetSegmentIndex(info_ptr, index.previous_offset, &index);
      if ((status&1) != 0 && idx >= index.first_idx && idx < index.first_idx + SEGMENTS_PER_INDEX) {
        SEGMENT_INFO *sinfo=&index.segment[idx % SEGMENTS_PER_INDEX];
        if (sinfo->data_offset != -1) {
          int i;
          int deleted=1;
	  int compressed_segment=0;
          DESCRIPTOR_A(dim2,8,DTYPE_Q,0,0);
          DESCRIPTOR_A_COEFF(ans,0,0,0,8,0);
          ans.pointer = 0;
          ans.dtype=segment_header.dtype;
          ans.length=segment_header.length;
          ans.dimct=segment_header.dimct;
          memcpy(ans.m,segment_header.dims,sizeof(segment_header.dims));
          ans.m[segment_header.dimct-1]=sinfo->rows;
          ans.arsize=ans.length;
          for (i=0;i<ans.dimct; i++) ans.arsize *= ans.m[i];
	  if (sinfo->rows < 0) {
	    EMPTYXD(compressed_segment_xd);
	    int data_length=sinfo->rows & 0x7fffffff;
	    compressed_segment=1;
	    status = TreeGetDsc(info_ptr,sinfo->data_offset,data_length,&compressed_segment_xd);
            if (status & 1) {
	      status = MdsDecompress((struct descriptor_r *)compressed_segment_xd.pointer,segment);
	      MdsFree1Dx(&compressed_segment_xd,0);
	    }
	  } else {
	    ans.pointer = malloc(ans.arsize);
	    while (status & 1 && deleted) {
	      status = (MDS_IO_READ_X(info_ptr->data_file->get,sinfo->data_offset,ans.pointer,ans.arsize,&deleted) == (ssize_t)ans.arsize) ? TreeSUCCESS : TreeFAILURE;
	      if (status & 1 && deleted) 
		status= TreeReopenDatafile(info_ptr);
	    }
	  }
          if (status & 1) {
#ifdef WORDS_BIGENDIAN
            if (!compressed_segment) {
	      char *bptr;
	      if (ans.length > 1 && ans.dtype != DTYPE_T && ans.dtype != DTYPE_IDENT && ans.dtype != DTYPE_PATH) {
		switch (ans.length) {
		case 2: 
		  for (i=0,bptr=ans.pointer;i<ans.arsize/ans.length;i++,bptr+=sizeof(short))
		    *(short *)bptr = swapshort(bptr);
		  break;
		case 4:
		  for (i=0,bptr=ans.pointer;i<ans.arsize/ans.length;i++,bptr+=sizeof(int))
		    *(int *)bptr = swapint(bptr);
		  break;
		case 8:
		  for (i=0,bptr=ans.pointer;i<ans.arsize/ans.length;i++,bptr+=sizeof(_int64))
		    *(_int64 *)bptr = swapquad(bptr);
		  break;
		}
	      }
            }
#endif
            if (sinfo->dimension_offset != -1 && sinfo->dimension_length==0) {
              _int64 *tp;
              int deleted=1;
              dim2.arsize=sinfo->rows * sizeof(_int64);
              dim2.pointer=malloc(dim2.arsize);
              while (status & 1 && deleted) {
                status = (MDS_IO_READ_X(info_ptr->data_file->get,sinfo->dimension_offset,dim2.pointer,dim2.arsize,&deleted) == (ssize_t)dim2.arsize) ? TreeSUCCESS : TreeFAILURE;
                if (status & 1 && deleted) 
                  status = TreeReopenDatafile(info_ptr);
              }
#ifdef WORDS_BIGENDIAN
              for (i=0,bptr=dim2.pointer;i<dim2.arsize/dim2.length;i++,bptr+=sizeof(_int64))
                *(_int64 *)bptr = swapquad(bptr);
#endif
	      if (!compressed_segment) {
		/**** Remove trailing null timestamps from dimension and data ****/
		for (tp=(_int64 *)(dim2.pointer+dim2.arsize-dim2.length); (tp >= (_int64 *)dim2.pointer) && (*tp==0);tp--) {
		  ans.m[segment_header.dimct-1]--;
		  dim2.arsize-=sizeof(_int64);
		}
		ans.arsize=ans.length;
		for (i=0;i<ans.dimct; i++) ans.arsize *= ans.m[i];
		/**** end remove null timestamps ***/
	      }
              MdsCopyDxXd((struct descriptor *)&dim2,dim);
              free(dim2.pointer);
            } else {
              TreeGetDsc(info_ptr,sinfo->dimension_offset,sinfo->dimension_length,dim);
            }
	    if (!compressed_segment) {
	      MdsCopyDxXd((struct descriptor *)&ans,segment);
	    }
          } else {
            status = TreeFAILURE;
          }
	  if (!compressed_segment) 
	    free(ans.pointer);
        }
        else {
          status = TreeFAILURE;
        }
      } else {
        status = TreeFAILURE;
      }
    }
  }
  return status;
}

int _TreeGetSegmentLimits(void *dbid, int nid, int idx, struct descriptor_xd *retStart, struct descriptor_xd *retEnd);
int TreeGetSegmentLimits(int nid, int idx, struct descriptor_xd *retStart, struct descriptor_xd *retEnd) {
  return _TreeGetSegmentLimits(*TreeCtx(), nid, idx, retStart, retEnd);
}


int _TreeGetSegmentLimits(void *dbid, int nid, int idx, struct descriptor_xd *retStart, struct descriptor_xd *retEnd) {
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID       *nid_ptr = (NID *)&nid;
  int       status=TreeFAILURE;
  int       open_status;
  TREE_INFO *info_ptr;
  int       nidx;
  NODE      *node_ptr;
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  nid_to_node(dblist, nid_ptr, node_ptr);
  if (!node_ptr)
    return TreeNNF;
  if (dblist->remote) {
    printf("Segmented records are not supported using thick client mode\n");
    return 0;
  }
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr)
  {
    NCI       local_nci;
    _int64    saved_viewdate;
    EXTENDED_ATTRIBUTES attributes;
    SEGMENT_HEADER segment_header;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciW(info_ptr, nidx, &local_nci,0);
    if (!(status & 1))
      return status;
    if (info_ptr->data_file==0)
      open_status = TreeOpenDatafileR(info_ptr);
    else
      open_status = 1;
    if (!(open_status & 1)) {
      return open_status;
    }
    if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) || 
	((TreeGetExtendedAttributes(info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1)==0)) {
      status = TreeFAILURE;
    } else if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY]==0 ||
       ((GetSegmentHeader(info_ptr, attributes.facility_offset[SEGMENTED_RECORD_FACILITY],&segment_header)&1)==0)) {
      status = TreeFAILURE;
    }
    else {
      SEGMENT_INDEX index;
      status = GetSegmentIndex(info_ptr, segment_header.index_offset, &index);
      while ((status &1) != 0 && idx < index.first_idx && index.previous_offset > 0)
	status = GetSegmentIndex(info_ptr, index.previous_offset, &index);
      if ((status&1) != 0 && idx >= index.first_idx && idx < index.first_idx + SEGMENTS_PER_INDEX) {
	SEGMENT_INFO *sinfo=&index.segment[idx % SEGMENTS_PER_INDEX];
	struct descriptor q_d = {8,DTYPE_Q,CLASS_S,0};
	if (sinfo->dimension_offset != -1 && sinfo->dimension_length==0) {
          if (sinfo->rows < 0) {
	    q_d.pointer=(char *)&sinfo->start;
	    MdsCopyDxXd(&q_d,retStart);
            q_d.pointer=(char *)&sinfo->end;
	    MdsCopyDxXd(&q_d,retEnd);
          } else {
	    int length =sizeof(_int64) * sinfo->rows;
	    char *buffer=malloc(length),*bptr;
	    _int64 timestamp;
	    int deleted=1;
	    while (status & 1 && deleted) {
	      status = (MDS_IO_READ_X(info_ptr->data_file->get,sinfo->dimension_offset,buffer,length,&deleted) == length) ? TreeSUCCESS : TreeFAILURE;
	      if (status & 1 && deleted) 
		status = TreeReopenDatafile(info_ptr);
	    }
	    if (status & 1) {
	      q_d.pointer=(char *)&timestamp;
	      timestamp=swapquad(buffer);
	      MdsCopyDxXd(&q_d,retStart);
	      for (bptr=buffer+length-sizeof(_int64); bptr > buffer  && ((timestamp=swapquad(bptr)) == 0); bptr -= sizeof(_int64));
	      if (bptr > buffer) {
		MdsCopyDxXd(&q_d,retEnd);
	      } else {
		MdsFree1Dx(retEnd,0);
	      } 
	    } else {
	      MdsFree1Dx(retStart,0);
	      MdsFree1Dx(retEnd,0);
	    }
	    free(buffer);
	  }
	} else {
	  if (sinfo->start != -1) {
	    q_d.pointer = (char *)&sinfo->start;
	    MdsCopyDxXd(&q_d,retStart);
	  } else if (sinfo->start_length > 0 && sinfo->start_offset > 0) {
	    status = TreeGetDsc(info_ptr,sinfo->start_offset,sinfo->start_length,retStart);
	  } else
	    status = MdsFree1Dx(retStart,0);
	  if (sinfo->end != -1) {
	    q_d.pointer = (char *)&sinfo->end;
	    MdsCopyDxXd(&q_d,retEnd);
	  } else if (sinfo->end_length > 0 && sinfo->end_offset > 0) {
	    status = TreeGetDsc(info_ptr,sinfo->end_offset,sinfo->end_length,retEnd);
	  } else
	    status = MdsFree1Dx(retEnd,0);
	}
      } else {
	status = TreeFAILURE;
      }
    }
  }
  return status;
}

int _TreeSetXNci(void *dbid, int nid, char *xnciname, struct descriptor *value);

int TreeSetXNci(int nid, char *xnciname, struct descriptor *value) {
  return _TreeSetXNci(*TreeCtx(), nid, xnciname, value);
}

int _TreeSetXNci(void *dbid, int nid, char *xnciname, struct descriptor *value) {
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID       *nid_ptr = (NID *)&nid;
  int       status=TreeFAILURE;
  int       open_status;
  TREE_INFO *info_ptr;
  int       nidx;
  static int saved_uic = 0;
  int       shot_open;
  NODE      *node_ptr;
  //  compress_utility = utility_update == 2;
#if !defined(HAVE_WINDOWS_H) && !defined(HAVE_VXWORKS_H)
  if (!saved_uic)
    saved_uic = (getgid() << 16) | getuid();
#endif
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  if (dblist->open_readonly)
    return TreeREADONLY;
  if (!xnciname || strlen(xnciname) < 1 || strlen(xnciname) > NAMED_ATTRIBUTE_NAME_SIZE )
    return TreeFAILURE;
  nid_to_node(dblist, nid_ptr, node_ptr);
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
  if (info_ptr)
  {
    int       stv;
    NCI       local_nci;
    _int64    saved_viewdate;
    EXTENDED_ATTRIBUTES attributes;
    _int64 attributes_offset=-1;
    int    update_attributes=0;
    int found_index=-1;
    _int64 index_offset=-1;
    int value_length;
    _int64 value_offset;
    NAMED_ATTRIBUTES_INDEX index,current_index;
    status = TreeGetNciLw(info_ptr, nidx, &local_nci);
    if (!(status & 1))
      return status;
    if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
      open_status = TreeOpenDatafileW(info_ptr, &stv, 0);
    else
      open_status = 1;
    if (!(open_status & 1)) {
      TreeUnLockNci(info_ptr,0,nidx);
      return open_status;
    }
    TreeGetViewDate(&saved_viewdate);
    status=TreePutDsc(info_ptr,nid,value,&value_offset,&value_length);
    if (!(status & 1))
      return status;
    /*** See if node is currently using the Extended Nci feature and if so get the current contents of the attributes
         index. If not, make an empty index and flag that a new index needs to be written.
    ****/
    if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) || 
	((TreeGetExtendedAttributes(info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1)==0)) {
      memset(&attributes,-1,sizeof(attributes));
      update_attributes=1;
      if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) && local_nci.length > 0) {
	if (local_nci.flags2 & NciM_DATA_IN_ATT_BLOCK) {
	  EMPTYXD(dsc);
	  struct descriptor *dptr;
	  unsigned char dsc_dtype = DTYPE_DSC;
	  int dlen = local_nci.length - 8;
	  unsigned int ddlen = dlen + sizeof(struct descriptor);
	  status = MdsGet1Dx(&ddlen, &dsc_dtype, &dsc,0);
	  dptr = dsc.pointer;
	  dptr->length = dlen;
	  dptr->dtype = local_nci.dtype;
	  dptr->class = CLASS_S;
	  dptr->pointer = (char *) dptr + sizeof(struct descriptor);
	  memcpy(dptr->pointer,local_nci.DATA_INFO.DATA_IN_RECORD.data,dptr->length);
	  if (dptr->dtype != DTYPE_T) {
	    switch (dptr->length)
	      {
	      case 2: *(short *)dptr->pointer = swapshort(dptr->pointer); break;
	      case 4: *(int *)dptr->pointer = swapint(dptr->pointer); break;
	      case 8: *(_int64 *)dptr->pointer = swapquad(dptr->pointer); break;
	      }
	  }
	  TreePutDsc(info_ptr,nid,dptr,&attributes.facility_offset[STANDARD_RECORD_FACILITY],
		 &attributes.facility_length[STANDARD_RECORD_FACILITY]);
	  local_nci.flags2 &= ~NciM_DATA_IN_ATT_BLOCK;
	} else {
	  EMPTYXD(xd);
	  int retsize;
	  int nodenum;
	  int length = local_nci.DATA_INFO.DATA_LOCATION.record_length;
	  if (length > 0)
	  {
	    char *data = malloc(length);
	    status = TreeGetDatafile(info_ptr, local_nci.DATA_INFO.DATA_LOCATION.rfa,&length,data,&retsize,&nodenum,local_nci.flags2);
	    if (!(status & 1))
	      status = TreeBADRECORD;
	    else if (!(local_nci.flags2 & NciM_NON_VMS) && ((retsize != length) || (nodenum != nidx)))
	      status = TreeBADRECORD;
	    else
	      status = (MdsSerializeDscIn(data,&xd) & 1) ? TreeNORMAL : TreeBADRECORD;
	    free(data);
	    if (status & 1) {
	      status = TreePutDsc(info_ptr, nid,(struct descriptor *)&xd, &attributes.facility_offset[STANDARD_RECORD_FACILITY], 
				  &attributes.facility_length[STANDARD_RECORD_FACILITY]);
	    }
	  }
	  if (length <= 0 || !(status & 1)) {
	    attributes.facility_offset[STANDARD_RECORD_FACILITY]= 0;
	    attributes.facility_length[STANDARD_RECORD_FACILITY]= 0;
	    local_nci.length=0;
	    local_nci.DATA_INFO.DATA_LOCATION.record_length=0;
	  }
	}
      }
    }
    else {
      attributes_offset=RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa);
    }
    /*** See if the node currently has an named attributes header record. If not, make an empty named attributes header and flag that
	 a new one needs to be written.
    ***/
    if (attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY]==-1 ||
       ((GetNamedAttributesIndex(info_ptr, attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY],&index)&1)==0)) {
      memset(&index,0,sizeof(index));
      attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY]=-1;
      index_offset=-1;
      index.previous_offset=-1;
      update_attributes=1;
    } else {
      index_offset=attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY];
    }
    current_index=index;
    /*** See if the node currently has a value for this attribute.
    ***/
    while(index_offset != -1 && found_index==-1) {
      int i,j;
      for (i=0;i<NAMED_ATTRIBUTES_PER_INDEX;i++) {
	int len=strlen(xnciname);
	for (j=0;j<len;j++) {
	  if (__tolower(xnciname[j]) !=  __tolower(index.attribute[i].name[j]))
	    break;
	}
	if (j==len && index.attribute[i].name[j]==0)
	  break;
      }
      if (i < NAMED_ATTRIBUTES_PER_INDEX)
	found_index=i;
      else if (index.previous_offset != -1) {
	_int64 new_offset=index.previous_offset;
	if ((GetNamedAttributesIndex(info_ptr,index.previous_offset,&index)&1)==0) {
	  break;
	}
	else
	  index_offset=new_offset;
      }
      else
	break;
    }
    /*** If name exists just replace the value else find an empty slot ***/
    if (found_index != -1) {
      index.attribute[found_index].offset=value_offset;
      index.attribute[found_index].length=value_length;
    } else {
      int i;
      index=current_index;
      for (i=0;i<NAMED_ATTRIBUTES_PER_INDEX;i++) {
	if (index.attribute[i].name[0]==0) {
	  strcpy(index.attribute[i].name,xnciname);
	  index.attribute[i].offset=value_offset;
	  index.attribute[i].length=value_length;
	  index_offset=attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY];
	  break;
	}
      }
      if (i == NAMED_ATTRIBUTES_PER_INDEX) {
	memset(&index,0,sizeof(index));
	index.previous_offset=attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY];
	update_attributes=1;
	strcpy(index.attribute[0].name,xnciname);
	index.attribute[0].offset=value_offset;
	index.attribute[0].length=value_length;
	index_offset=-1;
      }
    }
    status = PutNamedAttributesIndex(info_ptr,&index,&index_offset);
    if ((status & 1) && update_attributes) {
      attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY]=index_offset;
      status = TreePutExtendedAttributes(info_ptr,&attributes,&attributes_offset);
      SeekToRfa(attributes_offset,local_nci.DATA_INFO.DATA_LOCATION.rfa);
      local_nci.flags2 |= NciM_EXTENDED_NCI;
      TreePutNci(info_ptr,nidx,&local_nci,0);
    }
    else {
      TreeUnLockNci(info_ptr,0,nidx);
    }
  }      
  return status;
}

int _TreeGetXNci(void *dbid, int nid, char *xnciname, struct descriptor_xd *value);

int TreeGetXNci(int nid, char *xnciname, struct descriptor_xd *value) {
  return _TreeGetXNci(*TreeCtx(), nid, xnciname, value);
}

int _TreeGetXNci(void *dbid, int nid, char *xnciname, struct descriptor_xd *value) {
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID       *nid_ptr = (NID *)&nid;
  int       status=TreeFAILURE;
  int       open_status;
  TREE_INFO *info_ptr;
  int       nidx;
  NODE      *node_ptr;
  if (!xnciname || strlen(xnciname) < 1 || strlen(xnciname) > NAMED_ATTRIBUTE_NAME_SIZE )
    return TreeFAILURE;
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  nid_to_node(dblist, nid_ptr, node_ptr);
  if (!node_ptr)
    return TreeNNF;
  //  if (node_ptr->usage != TreeUSAGE_SIGNAL)
  //  return  TreeFAILURE;
  if (dblist->remote) {
    printf("Segmented records are not supported using thick client mode\n");
    return 0;
  }
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr)
  {
    NCI       local_nci;
    _int64    saved_viewdate;
    EXTENDED_ATTRIBUTES attributes;
    NAMED_ATTRIBUTES_INDEX index;
    char *attnames="attributenames";
    int getnames=0;
    struct _namelist { 
      char name[NAMED_ATTRIBUTE_NAME_SIZE+1];
      struct _namelist *next;
    } *namelist = 0;
    int longestattname=0;
    int numnames=0;
    unsigned int i;
    unsigned int len=strlen(xnciname);
    if (len == strlen(attnames)) {
      for (i=0;i<len;i++) {
	if (__tolower(xnciname[i]) != attnames[i])
	  break;
      }
      if (i==len) {
	getnames=1;
      }
    }
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciW(info_ptr, nidx, &local_nci,0);
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
	((TreeGetExtendedAttributes(info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1)==0)) {
      status = TreeFAILURE;
    } else if (attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY]== -1 ||
       ((GetNamedAttributesIndex(info_ptr, attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY],&index)&1)==0)) {
      status = TreeFAILURE;
    }
    else {
      int found_index=-1;
      while(found_index==-1) {
	unsigned int i,j;
	for (i=0;i<NAMED_ATTRIBUTES_PER_INDEX;i++) {
	  if (getnames==1) {
	    if (index.attribute[i].name[0]!=0) {
	      int len=strlen(index.attribute[i].name);
	      struct _namelist *p=malloc(sizeof(struct _namelist));
	      memcpy(p->name,index.attribute[i].name,sizeof(p->name));
	      p->next=namelist;
	      namelist=p;
	      longestattname=(len > longestattname) ? len : longestattname;
	      numnames++;
	    }
	  }
	  else {
	    for (j=0;j<len;j++) {
	      if (__tolower(xnciname[j]) !=  __tolower(index.attribute[i].name[j]))
		break;
	    }
	    if (j==len && index.attribute[i].name[j]==0)
	      break;
	  }
	}
	if (i < NAMED_ATTRIBUTES_PER_INDEX)
	  found_index=i;
	else if (index.previous_offset != -1) {
	  if ((GetNamedAttributesIndex(info_ptr,index.previous_offset,&index)&1)==0) {
	    break;
	  }
	}
	else
	  break;
      }
      if (found_index != -1) {
	status = TreeGetDsc(info_ptr,index.attribute[found_index].offset,index.attribute[found_index].length,value);
      } else if (getnames==1) {
	if (namelist==0) {
	  status = TreeFAILURE;
	} else {
	  char *names=malloc(longestattname*numnames);
	  DESCRIPTOR_A(name_array,longestattname,DTYPE_T,names,longestattname*numnames);
	  struct _namelist *p;
	  char *np;
	  for (p=namelist,np=names;p;p=p->next,np+=longestattname) {
	    int i;
	    memcpy(np,p->name,longestattname);
	    for (i=1;i<longestattname;i++) if (np[i]=='\0') np[i]=' ';
	  }
	  MdsCopyDxXd((struct descriptor *)&name_array,value);
	  free(names);
	}
      } else
	status = TreeFAILURE;
    }
  }
  return status;
}

int TreeSetRetrievalQuota(int quota) {
  printf("TreeSetRetrievalQuota is not implemented\n");
  return 0;
}

 extern int TreeFixupNid();

int TreePutDsc(TREE_INFO *info, int nid_in, struct descriptor *dsc, _int64 *offset, int *length) {
  EMPTYXD(xd);
  int compressible;
  int dlen;
  int reclen;
  char dtype,class;
  int data_in_altbuf;
  NID *nid = (NID *)&nid_in;
  unsigned char tree=nid->tree;
  int status = MdsSerializeDscOutZ(dsc, &xd, TreeFixupNid,&tree,0,0,1,
            &compressible,&dlen,&reclen,&dtype,&class,0,0,&data_in_altbuf);
  if (status & 1 && xd.pointer != 0 && xd.pointer->class == CLASS_A && xd.pointer->pointer != 0) {
    struct descriptor_a *ap=(struct descriptor_a *)xd.pointer;
    TreeLockDatafile(info,0,0);
    *offset=MDS_IO_LSEEK(info->data_file->put,0,SEEK_END);
    status = (MDS_IO_WRITE(info->data_file->put,ap->pointer,ap->arsize) == (ssize_t)ap->arsize) ? TreeSUCCESS : TreeFAILURE;
    TreeUnLockDatafile(info,0,0);
    *length=ap->arsize;
    MdsFree1Dx(&xd,0);
  }
  return status;
}

int TreeGetDsc(TREE_INFO *info,_int64 offset, int length, struct descriptor_xd *dsc) {
  char *buffer = malloc(length);
  int status=1;
  int deleted=1;
  while (status & 1 && deleted) {
    status = (MDS_IO_READ_X(info->data_file->get,offset,buffer,length,&deleted) == length) ? TreeSUCCESS : TreeFAILURE;
    if (status & 1 && deleted) 
      status = TreeReopenDatafile(info);
  }
  if (status & 1) {
    status = MdsSerializeDscIn(buffer,dsc);
  }
  free(buffer);
  return status;
}

static int GetCompressedSegmentRows(TREE_INFO *info,_int64 offset, int *rows) {
  int length = 60;
  unsigned char buffer[60];
  int status=1;
  int deleted=1;
  while (status & 1 && deleted) {
    status = (MDS_IO_READ_X(info->data_file->get,offset,buffer,length,&deleted) == length) ? TreeSUCCESS : TreeFAILURE;
    if (status & 1 && deleted) 
      status = TreeReopenDatafile(info);
  }
  if (status & 1) {
    if (buffer[3]==CLASS_CA || buffer[3]==CLASS_A) {
      unsigned char dimct=buffer[11];
      if (dimct==1) {
        int arsize=swapint(buffer+12);
        *rows=arsize/swapshort(buffer);
      } else {
        *rows=swapint(buffer+16+dimct*4);
      }
      return 1;
    }
    else
      return 0;
  }
  return status;
}


static int PutSegmentHeader(TREE_INFO *info, SEGMENT_HEADER *hdr, _int64 *offset) {
  int status;
  _int64 loffset;
  int j;
  char *next_row_fix=getenv("NEXT_ROW_FIX");
  unsigned char buffer[2*sizeof(char)+1*sizeof(short)+10*sizeof(int)+3*sizeof(_int64)],*bptr=buffer;
  if (*offset == -1) {
    TreeLockDatafile(info,0,0);
    *offset=MDS_IO_LSEEK(info->data_file->put,0,SEEK_END);
    loffset=0;
  } else {
    TreeLockDatafile(info,0,*offset);
    loffset=*offset;
    MDS_IO_LSEEK(info->data_file->put,*offset,SEEK_SET);
  }
  *bptr = hdr->dtype; bptr++;
  *bptr = hdr->dimct; bptr++;
  for (j=0;j<8;j++) {
    LoadInt(hdr->dims[j],bptr); bptr+=sizeof(int);
  }
  LoadShort(hdr->length,bptr); bptr+=sizeof(short);
  LoadInt(hdr->idx,bptr); bptr+=sizeof(int);
  if (next_row_fix != 0) {
    int fix=atoi(next_row_fix);
    if (fix > 0) {
      if (fix <= hdr->next_row) {
	hdr->next_row -= fix;
	printf("next row adjusted down %d, now %d\n",fix,hdr->next_row);
      } else {
	printf("next row not adjusted, requested=%d, next_row=%d\n",fix,hdr->next_row);
      }
    }
  }
  LoadInt(hdr->next_row,bptr); bptr+=sizeof(int);
  LoadQuad(hdr->index_offset,bptr); bptr+=sizeof(_int64);
  LoadQuad(hdr->data_offset,bptr); bptr+=sizeof(_int64);
  LoadQuad(hdr->dim_offset,bptr); bptr+=sizeof(_int64);
  status = (MDS_IO_WRITE(info->data_file->put,buffer,sizeof(buffer)) == sizeof(buffer)) ? TreeSUCCESS : TreeFAILURE;
  TreeUnLockDatafile(info,0,loffset);
  return status;
}

static int PutSegmentIndex(TREE_INFO *info, SEGMENT_INDEX   *idx, _int64 *offset) {
  int status;
  int i;
  _int64 loffset;
  unsigned char buffer[sizeof(_int64)+sizeof(int)+SEGMENTS_PER_INDEX*(6*sizeof(_int64)+4*sizeof(int))],*bptr=buffer;
  if (*offset == -1) {
    TreeLockDatafile(info,0,0);
    loffset=0;
    *offset=MDS_IO_LSEEK(info->data_file->put,0,SEEK_END);
  }
  else {
    TreeLockDatafile(info,0,*offset);
    loffset=*offset;
    MDS_IO_LSEEK(info->data_file->put,*offset,SEEK_SET);
  }
  LoadQuad(idx->previous_offset,bptr);bptr+=sizeof(_int64);
  LoadInt(idx->first_idx,bptr);bptr+=sizeof(int);
  for (i=0;i<SEGMENTS_PER_INDEX;i++) {
    LoadQuad(idx->segment[i].start,bptr); bptr+=sizeof(_int64);
    LoadQuad(idx->segment[i].end,bptr); bptr+=sizeof(_int64);
    LoadQuad(idx->segment[i].start_offset,bptr); bptr+=sizeof(_int64);
    LoadInt(idx->segment[i].start_length,bptr); bptr+=sizeof(int);
    LoadQuad(idx->segment[i].end_offset,bptr); bptr+=sizeof(_int64);
    LoadInt(idx->segment[i].end_length,bptr); bptr+=sizeof(int);
    LoadQuad(idx->segment[i].dimension_offset,bptr); bptr+=sizeof(_int64);
    LoadInt(idx->segment[i].dimension_length,bptr); bptr+=sizeof(int);
    LoadQuad(idx->segment[i].data_offset,bptr); bptr+=sizeof(_int64);
    LoadInt(idx->segment[i].rows,bptr); bptr+=sizeof(int);
  }
  status = (MDS_IO_WRITE(info->data_file->put,buffer,sizeof(buffer)) == sizeof(buffer)) ? TreeSUCCESS : TreeFAILURE;
  TreeUnLockDatafile(info,0,loffset);
  return status;
}

int TreePutExtendedAttributes(TREE_INFO *info, EXTENDED_ATTRIBUTES *att, _int64 *offset) {
  int status;
  int i;
  _int64 loffset;
  unsigned char buffer[sizeof(_int64)+FACILITIES_PER_EA*(sizeof(_int64)+sizeof(int))],*bptr=buffer;
  if (*offset == -1) {
    TreeLockDatafile(info,0,0);
    loffset=0;
    *offset=MDS_IO_LSEEK(info->data_file->put,0,SEEK_END);
  }
  else {
    TreeLockDatafile(info,0,*offset);
    loffset=*offset;
    MDS_IO_LSEEK(info->data_file->put,*offset,SEEK_SET);
  }
  LoadQuad(att->next_ea_offset,bptr); bptr+=sizeof(_int64);
  for (i=0;i<FACILITIES_PER_EA;i++) {
    LoadQuad(att->facility_offset[i],bptr); bptr+=sizeof(_int64);
  }
  for (i=0;i<FACILITIES_PER_EA;i++) {
    LoadInt(att->facility_length[i],bptr); bptr+=sizeof(int);
  }
  status = (MDS_IO_WRITE(info->data_file->put,buffer,sizeof(buffer)) == sizeof(buffer)) ? TreeSUCCESS : TreeFAILURE;
  TreeUnLockDatafile(info,0,loffset);
  return status;
}

static int PutNamedAttributesIndex(TREE_INFO *info, NAMED_ATTRIBUTES_INDEX *index, _int64 *offset) {
  int status;
  int i;
  _int64 loffset;
  unsigned char buffer[sizeof(_int64)+NAMED_ATTRIBUTES_PER_INDEX*(NAMED_ATTRIBUTE_NAME_SIZE+sizeof(_int64)+sizeof(int))],*bptr=buffer;
  if (*offset == -1) {
    TreeLockDatafile(info,0,0);
    loffset=0;
    *offset=MDS_IO_LSEEK(info->data_file->put,0,SEEK_END);
  }
  else {
    TreeLockDatafile(info,0,*offset);
    loffset=*offset;
    MDS_IO_LSEEK(info->data_file->put,*offset,SEEK_SET);
  }
  LoadQuad(index->previous_offset,bptr); bptr+=sizeof(_int64);
  for (i=0;i<NAMED_ATTRIBUTES_PER_INDEX;i++) {
    memcpy(bptr,index->attribute[i].name,NAMED_ATTRIBUTE_NAME_SIZE); bptr+=NAMED_ATTRIBUTE_NAME_SIZE;
    LoadQuad(index->attribute[i].offset,bptr); bptr+=sizeof(_int64);
    LoadInt(index->attribute[i].length,bptr); bptr+=sizeof(int);
  }
  status = (MDS_IO_WRITE(info->data_file->put,buffer,sizeof(buffer)) == sizeof(buffer)) ? TreeSUCCESS : TreeFAILURE;
  TreeUnLockDatafile(info,0,loffset);
  return status;
}

static int PutInitialValue(TREE_INFO *info,int *dims, struct descriptor_a *array, _int64 *offset) {
  int status;
  int length=array->length;
  int i;
  _int64 loffset;
#ifdef WORDS_BIGENDIAN
  unsigned char *buffer,*bptr;
#endif
  if (*offset == -1) {
    TreeLockDatafile(info,0,0);
    loffset=0;
    *offset=MDS_IO_LSEEK(info->data_file->put,0,SEEK_END);
  }
  else {
    TreeLockDatafile(info,0,*offset);
    loffset=*offset;
    MDS_IO_LSEEK(info->data_file->put,*offset,SEEK_SET);
  }
  for (i=0;i<array->dimct;i++) length=length*dims[i];
#ifdef WORDS_BIGENDIAN
  buffer=memcpy(malloc(length),array->pointer,length);
  if (array->length > 1 && array->dtype != DTYPE_T && array->dtype != DTYPE_IDENT && array->dtype != DTYPE_PATH) {
    switch (array->length) {
    case 2: 
      for (i=0,bptr=buffer;i<length/array->length;i++,bptr+=sizeof(short))
	LoadShort(((short *)array->pointer)[i],bptr);
      break;
    case 4:
      for (i=0,bptr=buffer;i<length/array->length;i++,bptr+=sizeof(int))
	LoadInt(((int *)array->pointer)[i],bptr);
      break;
    case 8:
      for (i=0,bptr=buffer;i<length/array->length;i++,bptr+=sizeof(_int64))
	LoadQuad(((_int64 *)array->pointer)[i],bptr);
      break;
    }
  }
  status = (MDS_IO_WRITE(info->data_file->put,buffer,length) == length) ? TreeSUCCESS : TreeFAILURE;
  free(buffer);
#else
  status = (MDS_IO_WRITE(info->data_file->put,array->pointer,length) == length) ? TreeSUCCESS : TreeFAILURE;
#endif
  TreeUnLockDatafile(info,0,loffset);
  return status;
}

static int PutDimensionValue(TREE_INFO *info, _int64 *timestamps, int rows_filled, int ndims, int *dims, _int64 *offset) {
  int status = 1;
  int length=sizeof(_int64);
  _int64 loffset;
  unsigned char *buffer = 0;
  if (*offset == -1) {
    TreeLockDatafile(info,0,0);
    loffset=0;
    *offset=MDS_IO_LSEEK(info->data_file->put,0,SEEK_END);
  }
  else {
    TreeLockDatafile(info,0,*offset);
    loffset=*offset;
    MDS_IO_LSEEK(info->data_file->put,*offset,SEEK_SET);
  }
  length=length*dims[ndims-1] - (sizeof(_int64)*rows_filled);
  if (length > 0) {
    buffer=malloc(length);
    memset(buffer,0,length);
  }
  if (rows_filled > 0) {
    status = (MDS_IO_WRITE(info->data_file->put,timestamps,rows_filled*sizeof(_int64)) == (rows_filled * sizeof(_int64))) ? TreeSUCCESS : TreeFAILURE;
  }
  if (status & 1 && length > 0) {
    status = (MDS_IO_WRITE(info->data_file->put,buffer,length) == length) ? TreeSUCCESS : TreeFAILURE;
  }
  if (buffer)
    free(buffer);
  TreeUnLockDatafile(info,0,loffset);
  return status;
}

static int GetSegmentHeader(TREE_INFO *info, _int64 offset,SEGMENT_HEADER *hdr) {
  int status = TreeFAILURE;
  int i;
  unsigned char buffer[2*sizeof(char)+1*sizeof(short)+10*sizeof(int)+3*sizeof(_int64)],*bptr;
  if (offset > -1) {
    int deleted=1;
    status=1;
    while (status & 1 && deleted) {
      status = (MDS_IO_READ_X(info->data_file->get,offset,buffer,sizeof(buffer),&deleted) == sizeof(buffer)) ? TreeSUCCESS : TreeFAILURE;
      if (status & 1 && deleted) 
        status = TreeReopenDatafile(info);
    }
  }
  if (status == TreeSUCCESS) {
    bptr=buffer;
    hdr->dtype = *bptr++;
    hdr->dimct = *bptr++;
    for (i=0;i<8;i++,bptr+=sizeof(int))
      hdr->dims[i]=swapint(bptr);
    hdr->length = swapshort(bptr); bptr+=sizeof(short);
    hdr->idx = swapint(bptr); bptr+=sizeof(int);
    hdr->next_row = swapint(bptr); bptr+=sizeof(int);
    hdr->index_offset=swapquad(bptr); bptr+=sizeof(_int64);
    hdr->data_offset=swapquad(bptr); bptr+=sizeof(_int64);
    hdr->dim_offset=swapquad(bptr); bptr+=sizeof(_int64);
  }
  return status;
}

static int GetSegmentIndex(TREE_INFO *info, _int64 offset, SEGMENT_INDEX *idx) {
  int status = TreeFAILURE;
  int i;
  unsigned char buffer[sizeof(_int64)+sizeof(int)+SEGMENTS_PER_INDEX*(6*sizeof(_int64)+4*sizeof(int))],*bptr;
  if (offset > -1) {
    int deleted=1;
    status=1;
    while (status & 1 && deleted) {
      status = (MDS_IO_READ_X(info->data_file->get,offset,buffer,sizeof(buffer),&deleted) == sizeof(buffer)) ? TreeSUCCESS : TreeFAILURE;
      if (status & 1 && deleted) 
	status = TreeReopenDatafile(info);
    }
  }
  if (status == TreeSUCCESS) {
    bptr=buffer;
    idx->previous_offset=swapquad(bptr); bptr+=sizeof(_int64);
    idx->first_idx=swapint(bptr); bptr+=sizeof(int);
    for (i=0;i<SEGMENTS_PER_INDEX;i++) {
      idx->segment[i].start=swapquad(bptr); bptr+=sizeof(_int64);
      idx->segment[i].end=swapquad(bptr); bptr+=sizeof(_int64);
      idx->segment[i].start_offset=swapquad(bptr); bptr+=sizeof(_int64);
      idx->segment[i].start_length=swapint(bptr); bptr+=sizeof(int);
      idx->segment[i].end_offset=swapquad(bptr); bptr+=sizeof(_int64);
      idx->segment[i].end_length=swapint(bptr); bptr+=sizeof(int);
      idx->segment[i].dimension_offset=swapquad(bptr); bptr+=sizeof(_int64);
      idx->segment[i].dimension_length=swapint(bptr); bptr+=sizeof(int);
      idx->segment[i].data_offset=swapquad(bptr); bptr+=sizeof(_int64);
      idx->segment[i].rows=swapint(bptr); bptr+=sizeof(int);
    }
  }
  return status;
}

int TreeGetExtendedAttributes(TREE_INFO *info, _int64 offset, EXTENDED_ATTRIBUTES *att) {
  int status = TreeFAILURE;
  int i;
  unsigned char buffer[sizeof(_int64)+FACILITIES_PER_EA*(sizeof(_int64)+sizeof(int))],*bptr;
  if (offset > -1) {
    int deleted=1;
    status=1;
    while (status & 1 && deleted) {
      status = (MDS_IO_READ_X(info->data_file->get,offset,buffer,sizeof(buffer),&deleted) == sizeof(buffer)) ? TreeSUCCESS : TreeFAILURE;
      if (status & 1 && deleted) 
	status = TreeReopenDatafile(info);
    }
  }
  if (status == TreeSUCCESS) {
    bptr=buffer;
    att->next_ea_offset=swapquad(bptr); bptr+=sizeof(_int64);
    for (i=0;i<FACILITIES_PER_EA;i++) {
      att->facility_offset[i]=swapquad(bptr); bptr+=sizeof(_int64);
    }
    for (i=0;i<FACILITIES_PER_EA;i++) {
      att->facility_length[i]=swapint(bptr); bptr+=sizeof(int);
    }
  }
  return status;
 }

static int GetNamedAttributesIndex(TREE_INFO *info, _int64 offset, NAMED_ATTRIBUTES_INDEX *index) {
  int status = TreeFAILURE;
  int i;
  unsigned char buffer[sizeof(_int64)+NAMED_ATTRIBUTES_PER_INDEX*(sizeof(_int64)+sizeof(int)+NAMED_ATTRIBUTE_NAME_SIZE)],*bptr;
  if (offset > -1) {
    int deleted=1;
    status=1;
    while (status & 1 && deleted) {
      status = (MDS_IO_READ_X(info->data_file->get,offset,buffer,sizeof(buffer),&deleted) == sizeof(buffer)) ? TreeSUCCESS : TreeFAILURE;
      if (status & 1 && deleted) 
	status = TreeReopenDatafile(info);
    } 
  }
  if (status == TreeSUCCESS) {
    bptr=buffer;
    index->previous_offset=swapquad(bptr); bptr+=sizeof(_int64);
    for (i=0;i<NAMED_ATTRIBUTES_PER_INDEX;i++) {
      memcpy(index->attribute[i].name,bptr,NAMED_ATTRIBUTE_NAME_SIZE); bptr+=NAMED_ATTRIBUTE_NAME_SIZE;
      index->attribute[i].offset=swapquad(bptr); bptr+=sizeof(_int64);
      index->attribute[i].length=swapint(bptr); bptr+=sizeof(int);
    }
  }
  return status;
 }

static int __TreeBeginTimestampedSegment(void *dbid, int nid, _int64 *timestamps, struct descriptor_a *initialValue, int idx, int rows_filled);

int _TreeBeginTimestampedSegment(void *dbid, int nid, struct descriptor_a *initialValue, int idx) {
  return __TreeBeginTimestampedSegment(dbid, nid, 0, initialValue, idx, 0);
}

 int TreeBeginTimestampedSegment(int nid, struct descriptor_a *initialValue, int idx) {
   return _TreeBeginTimestampedSegment(*TreeCtx(), nid, initialValue, idx);
 }

int _TreeMakeTimestampedSegment(void *dbid, int nid, _int64 *timestamps, struct descriptor_a *initialValue, int idx, int rows_filled) {
  return __TreeBeginTimestampedSegment(dbid, nid, timestamps, initialValue, idx, rows_filled);
}

int TreeMakeTimestampedSegment(int nid, _int64 *timestamps, struct descriptor_a *initialValue, int idx, int rows_filled) {
   return _TreeMakeTimestampedSegment(*TreeCtx(), nid, timestamps, initialValue, idx, rows_filled);
 }

static int __TreeBeginTimestampedSegment(void *dbid, int nid, _int64 *timestamps, struct descriptor_a *initialValue, int idx, int rows_filled) {
   PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
   NID       *nid_ptr = (NID *)&nid;
   int       status;
   int       open_status;
   TREE_INFO *info_ptr;
   int       nidx;
   static int saved_uic = 0;
   int       shot_open;
   NODE      *node_ptr;
   A_COEFF_TYPE *a_coeff = (A_COEFF_TYPE *)initialValue;
#if !defined(HAVE_WINDOWS_H) && !defined(HAVE_VXWORKS_H)
   if (!saved_uic)
     saved_uic = (getgid() << 16) | getuid();
#endif
   if (!(IS_OPEN(dblist)))
     return TreeNOT_OPEN;
   if (dblist->open_readonly)
     return TreeREADONLY;
   
   nid_to_node(dblist, nid_ptr, node_ptr);
   if (!node_ptr)
     return TreeNNF;
   //  if (node_ptr->usage != TreeUSAGE_SIGNAL)
   //  return  TreeINVDTPUSG;
   
   while (initialValue && initialValue->dtype == DTYPE_DSC)
     initialValue = (struct descriptor_a *)initialValue->pointer;
   if (initialValue == NULL || initialValue->class != CLASS_A || initialValue->dimct < 1 || initialValue->dimct > 8)
     return TreeINVDTPUSG;
   if (dblist->remote) {
     printf("Segmented records are not supported using thick client mode\n");
     return 0;
   }
   shot_open = (dblist->shotid != -1);
   nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
   if (info_ptr)    {
     int       stv;
     NCI       local_nci;
     _int64    saved_viewdate;
     EXTENDED_ATTRIBUTES attributes;
     _int64 attributes_offset=-1;
     int    update_attributes=0;
     int add_length=0;
     int previous_length=-1;
     int d;
     SEGMENT_HEADER segment_header;
     SEGMENT_INDEX segment_index;
     SEGMENT_INFO *sinfo;
     status = TreeCallHook(PutData,info_ptr,nid);
     if (status && !(status & 1))
       return status;
     TreeGetViewDate(&saved_viewdate);
     status = TreeGetNciLw(info_ptr, nidx, &local_nci);
     if (!(status & 1))
       return status;
     if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
       open_status = TreeOpenDatafileW(info_ptr, &stv, 0);
     else
       open_status = 1;
     if (!(open_status & 1)) {
       TreeUnLockNci(info_ptr,0,nidx);
       return open_status;
     }
     local_nci.flags2 &= ~NciM_DATA_IN_ATT_BLOCK;
     local_nci.dtype=initialValue->dtype;
     local_nci.class=CLASS_R;
     local_nci.time_inserted=TreeTimeInserted();
     local_nci.owner_identifier=saved_uic;
     /*** See if node is currently using the Extended Nci feature and if so get the current contents of the attributes
	  index. If not, make an empty index and flag that a new index needs to be written.
     ****/
     if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) || 
	 ((TreeGetExtendedAttributes(info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1)==0)) {
       memset(&attributes,-1,sizeof(attributes));
       update_attributes=1;
     }
     else {
       attributes_offset=RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa);
       if (attributes.facility_offset[STANDARD_RECORD_FACILITY] != -1) {
	 attributes.facility_offset[STANDARD_RECORD_FACILITY] = -1;
	 attributes.facility_length[STANDARD_RECORD_FACILITY] = 0;
	 update_attributes=1;
       }
     }
     /*** See if the node currently has an segment header record. If not, make an empty segment header and flag that
	  a new one needs to be written.
     ***/
     if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY]==-1 ||
	 ((GetSegmentHeader(info_ptr, attributes.facility_offset[SEGMENTED_RECORD_FACILITY],&segment_header)&1)==0)) {
       memset(&segment_header,0,sizeof(segment_header));
       attributes.facility_offset[SEGMENTED_RECORD_FACILITY]=-1;
       segment_header.index_offset=-1;
       segment_header.data_offset=-1;
       segment_header.dim_offset=-1;
       segment_header.idx = -1;
       update_attributes=1;
     } else if (initialValue->dtype != segment_header.dtype || initialValue->dimct != segment_header.dimct ||
		(initialValue->dimct > 1 && memcmp(segment_header.dims,a_coeff->m,(initialValue->dimct-1)*sizeof(int)) != 0)) {
       TreeUnLockNci(info_ptr,0,nidx);
       return TreeFAILURE;
     }
     /*** See if the node currently has an segment_index record. If not, make an empty segment index and
	  flag that a new one needs to be written.
     ***/
     if ((segment_header.index_offset == -1) || 
	 ((GetSegmentIndex(info_ptr,segment_header.index_offset,&segment_index)&1)==0)) {
       segment_header.index_offset = -1;
       memset(&segment_index,-1,sizeof(segment_index));
       segment_index.first_idx=0;
     }
     if (idx == -1) {
       segment_header.idx++;
       idx=segment_header.idx;
       add_length=initialValue->arsize;
     } else if (idx < -1 || idx > segment_header.idx) {
       TreeUnLockNci(info_ptr,0,nidx);
       return TreeBUFFEROVF;
     } else {
       printf("this is not yet supported\n");
       return TreeFAILURE;
       /***************
Add support for updating an existing segment. add_length=new_length-old_length. potentially use same storage area if
old array is same size.
       *******/
       add_length=0;
     }
     segment_header.dtype=initialValue->dtype;
     segment_header.dimct=initialValue->dimct;
     segment_header.length=initialValue->length;
     segment_header.next_row=rows_filled;
     segment_header.data_offset=-1;
     segment_header.dim_offset=-1;
     if (segment_header.idx > 0) {
       previous_length=segment_header.length;
       for (d=0;d<segment_header.dimct;d++)
	 previous_length *= segment_header.dims[d];
     }
     if (initialValue->dimct == 1)
       segment_header.dims[0]=initialValue->arsize/initialValue->length;
     else {
       memcpy(segment_header.dims,a_coeff->m,initialValue->dimct*sizeof(int));
     }
     /*****
	  If not the first segment, see if we can reuse the previous segment storage space and compress the previous segment.
     ****/
     if ((segment_header.idx % SEGMENTS_PER_INDEX) > 0 && previous_length == add_length && (local_nci.flags & NciM_COMPRESS_ON_PUT && local_nci.flags & NciM_COMPRESS_SEGMENTS)) {
       int deleted;
       EMPTYXD(xd_data);
       EMPTYXD(xd_dim);
       sinfo = &segment_index.segment[(idx % SEGMENTS_PER_INDEX) - 1];
       TreeUnLockNci(info_ptr,0,nidx);
       status = _TreeGetSegment(dbid, nid, idx-1, &xd_data, &xd_dim);
       TreeLockNci(info_ptr,0,nidx,&deleted);
       if (status & 1) {
	 int length;
         int rows;
	 A_COEFF_TYPE *data_a = (A_COEFF_TYPE *)xd_data.pointer;
	 A_COEFF_TYPE *dim_a = (A_COEFF_TYPE *)xd_dim.pointer;
         rows = (initialValue->dimct == 1) ? initialValue->arsize/initialValue->length : ((A_COEFF_TYPE *)initialValue)->m[initialValue->dimct-1];
	 if (data_a && data_a->class == CLASS_A && data_a->pointer && data_a->arsize >= initialValue->arsize &&
	     dim_a && dim_a->class == CLASS_A && dim_a->pointer && dim_a->arsize >= (rows * sizeof(_int64)) &&
	     dim_a->dimct == 1 && dim_a->length == sizeof(_int64) && dim_a->dtype == DTYPE_Q) {
	   segment_header.data_offset=sinfo->data_offset;
	   segment_header.dim_offset=sinfo->dimension_offset;
	   status = TreePutDsc(info_ptr,nid,xd_data.pointer,&sinfo->data_offset,&length);
	   status = TreePutDsc(info_ptr,nid,xd_dim.pointer,&sinfo->dimension_offset,&sinfo->dimension_length);
	   sinfo->start = ((_int64 *)dim_a->pointer)[0];
	   sinfo->end = ((_int64 *)dim_a->pointer)[(dim_a->arsize/dim_a->length)-1];
	   /*** flag compressed segment by setting high bit in the rows field. ***/
	   sinfo->rows = length | 0x80000000;
	 }
	 else {
	   if (!data_a)
	     printf("data_a null\n");
	   else if (data_a->class != CLASS_A)
	     printf("data_a is not CLASS_A, class=%d\n",data_a->class);
	   else if (!data_a->pointer)
	     printf("data_a's pointer is null\n");
	   else if (data_a->arsize < initialValue->arsize)
	     printf("data_a->arsize (%d) < initialValue->arsize (%d)\n",data_a->arsize,initialValue->arsize);
	   else if (!dim_a)
	     printf("dim_a null\n");
	   else if (dim_a->class != CLASS_A)
	     printf("dim_a is not CLASS_A, class=%d\n",dim_a->class);
	   else if (!dim_a->pointer)
	     printf("dim_a's pointer is null\n");
	   else if (dim_a->arsize < (rows * sizeof(_int64)))
	     printf("dim_a->arsize (%d) < (rows (%d) * sizeof(_int64))",dim_a->arsize,rows);
	   else if (dim_a->dimct != 1)
	     printf("dim_a->dimct (%d) != 1\n",dim_a->dimct);
	   else if (dim_a->length != sizeof(_int64))
	     printf("dim_a->length (%d) != sizeof(_int64)\n",dim_a->length);
	   else if (dim_a->dtype != DTYPE_Q)
	     printf("dim_a->dtype (%d) != DTYPE_Q\n",dim_a->dtype);
	 }
       }
       MdsFree1Dx(&xd_data,0);
       MdsFree1Dx(&xd_dim,0);
     }
     status = PutInitialValue(info_ptr,segment_header.dims,initialValue,&segment_header.data_offset);
     status = PutDimensionValue(info_ptr,timestamps, rows_filled, initialValue->dimct, segment_header.dims,&segment_header.dim_offset);
     if (idx >= segment_index.first_idx+SEGMENTS_PER_INDEX) {
       memset(&segment_index,-1,sizeof(segment_index));
       segment_index.previous_offset=segment_header.index_offset;
       segment_header.index_offset=-1;
       segment_index.first_idx=idx;
     }
     sinfo=&segment_index.segment[idx % SEGMENTS_PER_INDEX];
     sinfo->start=rows_filled > 0 ? timestamps[0] : 0;
     sinfo->end=rows_filled > 0 ? timestamps[rows_filled - 1] : 0;
     sinfo->data_offset=segment_header.data_offset;
     sinfo->dimension_offset=segment_header.dim_offset;
     sinfo->dimension_length=0;
     sinfo->rows=segment_header.dims[segment_header.dimct-1];
     status = PutSegmentIndex(info_ptr,&segment_index,&segment_header.index_offset);
     status = PutSegmentHeader(info_ptr,&segment_header,&attributes.facility_offset[SEGMENTED_RECORD_FACILITY]);
     if (update_attributes) {
       status = TreePutExtendedAttributes(info_ptr,&attributes,&attributes_offset);
       SeekToRfa(attributes_offset,local_nci.DATA_INFO.DATA_LOCATION.rfa);
       local_nci.flags2 |= NciM_EXTENDED_NCI;
     }
     local_nci.length += add_length;
     TreePutNci(info_ptr,nidx,&local_nci,0);
   }
   return status;
 }

 int _TreePutTimestampedSegment(void *dbid, int nid, _int64 *timestamp, struct descriptor_a *data);

 int TreePutTimestampedSegment(int nid, _int64 *timestamp, struct descriptor_a *data) {
   return _TreePutTimestampedSegment(*TreeCtx(), nid, timestamp, data);
 }


 int _TreePutTimestampedSegment(void *dbid, int nid, _int64 *timestamp, struct descriptor_a *data) {
   PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
   NID       *nid_ptr = (NID *)&nid;
   int       status;
   int       open_status;
   TREE_INFO *info_ptr;
   int       nidx;
   static int saved_uic = 0;
   int       shot_open;
   NODE      *node_ptr;
   DESCRIPTOR_A(data_a, 0, 0, 0, 0);
   A_COEFF_TYPE *a_coeff;
   //  compress_utility = utility_update == 2;
#if !defined(HAVE_WINDOWS_H) && !defined(HAVE_VXWORKS_H)
   if (!saved_uic)
     saved_uic = (getgid() << 16) | getuid();
#endif
   if (!(IS_OPEN(dblist)))
     return TreeNOT_OPEN;
   if (dblist->open_readonly)
     return TreeREADONLY;
   
   nid_to_node(dblist, nid_ptr, node_ptr);
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
     data_a.dtype= data->dtype;
     data_a.arsize=data->length;
     data_a.dimct=1;
     data=(struct descriptor_a *)&data_a;
   }
   a_coeff = (A_COEFF_TYPE *)data;
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
       int rows_to_insert=-1;
       int bytes_per_row;
       int rows_in_segment;
       int bytes_to_insert;
#ifdef WORDS_BIGENDIAN
       unsigned char *buffer,*bptr,*times;
#endif
       _int64 offset;
       NCI       local_nci;
       _int64    saved_viewdate;
       EXTENDED_ATTRIBUTES attributes;
       int i;
       int startIdx;
       SEGMENT_HEADER segment_header;
       status = TreeCallHook(PutData,info_ptr,nid);
       if (status && !(status & 1))
	 return status;
       TreeGetViewDate(&saved_viewdate);
       status = TreeGetNciLw(info_ptr, nidx, &local_nci);
       if (!(status & 1))
	 return status;
       if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
	 open_status = TreeOpenDatafileW(info_ptr, &stv, 0);
       else
	 open_status = 1;
       if (!(open_status & 1)) {
	 TreeUnLockNci(info_ptr,0,nidx);
	 return open_status;
       }
       /*** See if node is currently using the Extended Nci feature and if so get the current contents of the attributes
	    index. If not, make an empty index and flag that a new index needs to be written.
       ****/
       if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) || 
	   ((TreeGetExtendedAttributes(info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1)==0)) {
	 TreeUnLockNci(info_ptr,0,nidx);
	 return TreeNOSEGMENTS;
       }
       if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY]==-1) {
	 status = TreeNOSEGMENTS;
       } else if (((status = GetSegmentHeader(info_ptr, attributes.facility_offset[SEGMENTED_RECORD_FACILITY],&segment_header))&1)==0) {
	 status=status;
       } else if (data->dtype != segment_header.dtype) {
	 status = TreeINVDTYPE;
       } else if (a_coeff->dimct == 1 && !((a_coeff->dimct == segment_header.dimct) || (a_coeff->dimct == segment_header.dimct-1))) {
	 status = TreeINVSHAPE;
       } else if (a_coeff->dimct > 1 && memcmp(segment_header.dims,a_coeff->m,(segment_header.dimct-1)*sizeof(int)) != 0) {
	 status = TreeINVSHAPE;
       } else if (a_coeff->dimct == 1 && a_coeff->arsize/a_coeff->length != 1 && (unsigned int)segment_header.dims[0] != a_coeff->arsize/a_coeff->length) {
	 status = TreeINVSHAPE;
       }
       if (!(status & 1)) {
	 TreeUnLockNci(info_ptr,0,nidx);
	 return status;
       }
       startIdx=segment_header.next_row;
       for (bytes_per_row=segment_header.length,i=0;i<segment_header.dimct-1;bytes_per_row *= segment_header.dims[i],i++);
       if (data->dimct < segment_header.dimct) {
	 rows_to_insert = 1;
       } else {
	 rows_to_insert = (data->dimct == 1) ? data->arsize/data->length : a_coeff->m[a_coeff->dimct-1];
       }
       if (rows_to_insert <= 0) {
	 TreeUnLockNci(info_ptr,0,nidx);
	 return TreeINVSHAPE;
       }
       rows_in_segment = segment_header.dims[segment_header.dimct-1];
       bytes_to_insert=((rows_to_insert > (rows_in_segment - startIdx)) ? (rows_in_segment-startIdx) : rows_to_insert) * bytes_per_row;
       if (bytes_to_insert < 1) {
	 TreeUnLockNci(info_ptr,0, nidx);
	 return TreeBUFFEROVF;
       }
       offset=segment_header.data_offset+startIdx*bytes_per_row;
#ifdef WORDS_BIGENDIAN
       buffer=memcpy(malloc(bytes_to_insert),data->pointer,bytes_to_insert);
       times=malloc(sizeof(_int64)*rows_to_insert);
       if (segment_header.length > 1 && data->dtype != DTYPE_T && data->dtype != DTYPE_IDENT && data->dtype != DTYPE_PATH) {
	 switch (segment_header.length) {
	 case 2: 
	   for (i=0,bptr=buffer;i<bytes_to_insert/segment_header.length;i++,bptr+=sizeof(short))
	     LoadShort(((short *)data->pointer)[i],bptr);
	   break;
	 case 4:
	   for (i=0,bptr=buffer;i<bytes_to_insert/segment_header.length;i++,bptr+=sizeof(int))
	     LoadInt(((int *)data->pointer)[i],bptr);
	   break;
	 case 8:
	   for (i=0,bptr=buffer;i<bytes_to_insert/segment_header.length;i++,bptr+=sizeof(_int64))
	     LoadQuad(((_int64 *)data->pointer)[i],bptr);
	   break;
	 }
       }
       TreeLockDatafile(info_ptr,0,offset);
       MDS_IO_LSEEK(info_ptr->data_file->put,offset,SEEK_SET);
       status = (MDS_IO_WRITE(info_ptr->data_file->put,buffer,bytes_to_insert) == bytes_to_insert) ? TreeSUCCESS : TreeFAILURE;
       MDS_IO_LSEEK(info_ptr->data_file->put,segment_header.dim_offset+startIdx*sizeof(_int64),SEEK_SET);
       for (i=0,bptr=times;i<rows_to_insert;i++,bptr+=sizeof(_int64))
	 LoadQuad(timestamp[i],bptr);
       status = (MDS_IO_WRITE(info_ptr->data_file->put,times,sizeof(_int64)*rows_to_insert) == (sizeof(_int64) * rows_to_insert)) ? TreeSUCCESS : TreeFAILURE;
       free(times);
       free(buffer);
#else
       TreeLockDatafile(info_ptr,0,offset);
       MDS_IO_LSEEK(info_ptr->data_file->put,offset,SEEK_SET);
       status = (MDS_IO_WRITE(info_ptr->data_file->put,data->pointer,bytes_to_insert) == bytes_to_insert) ? TreeSUCCESS : TreeFAILURE;
       MDS_IO_LSEEK(info_ptr->data_file->put,segment_header.dim_offset+startIdx*sizeof(_int64),SEEK_SET);
       status = (MDS_IO_WRITE(info_ptr->data_file->put,timestamp,sizeof(_int64)*rows_to_insert) == (sizeof(_int64) * rows_to_insert)) ? TreeSUCCESS : TreeFAILURE;
#endif
       TreeUnLockDatafile(info_ptr,0,offset);
       segment_header.next_row=startIdx+bytes_to_insert/bytes_per_row;
       if (status & 1)
	 status = PutSegmentHeader(info_ptr,&segment_header,&attributes.facility_offset[SEGMENTED_RECORD_FACILITY]);
       TreeUnLockNci(info_ptr,0,nidx);
   }
   return status;
 }

 static int CopyStandardRecord(TREE_INFO *info1, TREE_INFO *info2, int nid, _int64 *offset, int *length) {
   EMPTYXD(xd);
   int status;
   status = TreeGetDsc(info1,*offset,*length,&xd);
   if (status & 1) {
     status = TreePutDsc(info2, nid, (struct descriptor *)&xd, offset, length);
   }
   if (!(status & 1)) {
     *offset=-1;
     *length=0;
   }
   return status;
 }

 static int CopyNamedAttributes(TREE_INFO *info1, TREE_INFO *info2, int nid, _int64 *offset, int *length) {
   EMPTYXD(xd);
   NAMED_ATTRIBUTES_INDEX index;
   int status = GetNamedAttributesIndex(info1,*offset,&index);
   if (status & 1) {
     int i;
     if (index.previous_offset != -1) {
       CopyNamedAttributes(info1, info2, nid, &index.previous_offset,0);
     }
     for (i=0;i<NAMED_ATTRIBUTES_PER_INDEX;i++) {
       if (index.attribute[i].name[0]!='\0' && index.attribute[i].offset != -1) {
	 status = TreeGetDsc(info1,index.attribute[i].offset,index.attribute[i].length,&xd);
	 if (status & 1) {
	   status = TreePutDsc(info2, nid, (struct descriptor *)&xd,
			       &index.attribute[i].offset,&index.attribute[i].length);
	   if (!(status & 1)) {
	     memset(index.attribute[i].name,0,sizeof(index.attribute[i].name));
	     index.attribute[i].offset=-1;
	   }
	 }
       }
     }
     *offset=-1;
     status = PutNamedAttributesIndex(info2,&index,offset);
   } else {
     *offset=-1;
   }
   return status;
 }

 static int DataCopy(TREE_INFO *info1, TREE_INFO *info2, _int64 offset1, int length, _int64 *offset2) {
   int status=1;
   if (offset1 != -1 && length >= 0) {
     char *data=malloc(length);
     int deleted=1;
     while (status & 1 && deleted) {
       status = MDS_IO_READ_X(info1->data_file->get,offset1,data,length,&deleted) == length;
       if (status & 1 && deleted)
         status = TreeReopenDatafile(info1);
     }
     if (status) {
       *offset2 = MDS_IO_LSEEK(info2->data_file->put,0,SEEK_END);
       status = MDS_IO_WRITE(info2->data_file->put,data,length) == length;
     }
     free(data);
     if (!(status & 1)) {
       *offset2=-1;
     }
   }
   else {
     *offset2=-1;
   }
   return status;
 }
   
 static int CopySegmentIndex(TREE_INFO *info1, TREE_INFO *info2, int nid, SEGMENT_HEADER *header, _int64 *index_offset,
			     _int64 *data_offset, _int64 *dim_offset) {
   SEGMENT_INDEX index;
   int i;
   int status = GetSegmentIndex(info1, *index_offset, &index);
   int rowlen=header->length;
   for (i=0;i<header->dimct-1;i++) {
     rowlen=rowlen*header->dims[i];
   }
   if (status & 1) {
     if (index.previous_offset != -1) {
       status = CopySegmentIndex(info1, info2, nid, header, &index.previous_offset, data_offset, dim_offset);
     }
     for (i=0;i<SEGMENTS_PER_INDEX;i++) {
       SEGMENT_INFO *sinfo=&index.segment[i];
       int length;
       if (sinfo->data_offset != -1 && sinfo->rows < 0) {
	 length = sinfo->rows & 0x7fffffff;
	 status = DataCopy(info1, info2, sinfo->data_offset,length,&sinfo->data_offset);
	 status = DataCopy(info1, info2, sinfo->dimension_offset,sinfo->dimension_length,&sinfo->dimension_offset);
       }
       else {
	 status = DataCopy(info1, info2, sinfo->start_offset,sinfo->start_length,&sinfo->start_offset);
	 status = DataCopy(info1, info2, sinfo->end_offset,sinfo->end_length,&sinfo->end_offset);
	 if (sinfo->dimension_length == 0) {
	   length = sinfo->rows * sizeof(_int64);
	 } else {
	   length = sinfo->dimension_length;
	 }
	 status = DataCopy(info1, info2, sinfo->dimension_offset,length,&sinfo->dimension_offset);
	 length=rowlen*sinfo->rows;
	 status = DataCopy(info1, info2, sinfo->data_offset, length, &sinfo->data_offset);
	 if (header->idx == (index.first_idx+i)) {
	   *data_offset=sinfo->data_offset;
	   if (sinfo->dimension_offset != -1 && sinfo->dimension_length == 0) {
	     *dim_offset=sinfo->dimension_offset;
	   } else {
	     *dim_offset=-1;
	   }
	 }
       }
     }
     *index_offset=-1;
     status = PutSegmentIndex(info2, &index, index_offset);
   }
   return status;
 }

 static int CopySegmentedRecords(TREE_INFO *info1, TREE_INFO *info2, int nid, _int64 *offset, int *length) {
   SEGMENT_HEADER header;
   int status=GetSegmentHeader(info1,*offset,&header);
   if (status & 1) {
     status = CopySegmentIndex(info1, info2, nid, &header, &header.index_offset,&header.data_offset, &header.dim_offset);
     *offset=-1;
     status = PutSegmentHeader(info2, &header, offset);
   }
   return status;
 }

 int TreeCopyExtended(PINO_DATABASE *dbid1, PINO_DATABASE *dbid2, int nid, NCI *nci) {
   EXTENDED_ATTRIBUTES attributes;
   TREE_INFO *info1=dbid1->tree_info, *info2=dbid2->tree_info;
   int status;
   _int64 now=-1;
   _int64 offset=-1;
   status = TreeGetExtendedAttributes(info1,RfaToSeek(nci->DATA_INFO.DATA_LOCATION.rfa),&attributes);
   if (status & 1) {
     if (attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY] != -1) {
       CopyNamedAttributes(info1,info2,nid,
			   &attributes.facility_offset[NAMED_ATTRIBUTES_FACILITY],
			   &attributes.facility_length[NAMED_ATTRIBUTES_FACILITY]);
     }
     if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY] != -1) {
       CopySegmentedRecords(info1,info2,nid,
			    &attributes.facility_offset[SEGMENTED_RECORD_FACILITY],
			    &attributes.facility_length[SEGMENTED_RECORD_FACILITY]);
     }
     if (attributes.facility_offset[STANDARD_RECORD_FACILITY] != -1) {
       CopyStandardRecord(info1,info2,nid,
			  &attributes.facility_offset[STANDARD_RECORD_FACILITY],
			  &attributes.facility_length[STANDARD_RECORD_FACILITY]);
     }
     status = TreePutExtendedAttributes(info2,&attributes,&offset);
     if (status & 1) {
       SeekToRfa(offset,nci->DATA_INFO.DATA_LOCATION.rfa);
       status = TreePutNci(info2,nid,nci,0);
     }
     TreeSetViewDate(&now);
   }
   return status;
 }

 static EMPTYXD(TREE_START_CONTEXT);
 static EMPTYXD(TREE_END_CONTEXT);
 static EMPTYXD(TREE_DELTA_CONTEXT);
 int TreeSetTimeContext(struct descriptor *start, struct descriptor *end, struct descriptor *delta) {
   int status = MdsCopyDxXd(start,&TREE_START_CONTEXT);
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



int TreeGetSegmentedRecord(int nid, struct descriptor_xd *data) {
  return _TreeGetSegmentedRecord(*TreeCtx(),nid,data);
}

 int _TreeGetSegmentedRecord(void *dbid, int nid, struct descriptor_xd *data) {
   static int activated=0;
   static int (*addr)(void *, int, struct descriptor *, struct descriptor *, struct descriptor *, struct descriptor_xd *);
   int status=42;
   if (!activated) {
     static DESCRIPTOR(library,"XTreeShr");
     static DESCRIPTOR(routine,"_XTreeGetTimedRecord");
     status = LibFindImageSymbol(&library,&routine,&addr);
     if (status & 1) {
       activated=1;
     } else {
       fprintf(stderr,"Error activating XTreeShr library. Cannot access segmented records.\n");
       return status;
     }
   }
   status = (*addr)(dbid, nid, TREE_START_CONTEXT.pointer, TREE_END_CONTEXT.pointer, TREE_DELTA_CONTEXT.pointer, data);
   return status;
 }

int _TreePutRow(void *dbid, int nid, int bufsize, _int64 *timestamp, struct descriptor_a *data);

 int TreePutRow(int nid, int bufsize, _int64 *timestamp, struct descriptor_a *data) {
   return _TreePutRow(*TreeCtx(), nid, bufsize, timestamp, data);
 }


 int _TreePutRow(void *dbid, int nid, int bufsize, _int64 *timestamp, struct descriptor_a *data) {
  DESCRIPTOR_A_COEFF(initValue, data->length, data->dtype, 0, 8, 0);
  int status = _TreePutTimestampedSegment(dbid, nid, timestamp, data);
   if (status==TreeNOSEGMENTS || status==TreeBUFFEROVF) {
     status = 1;
     while (data && data->dtype==DTYPE_DSC) data=(struct descriptor_a *)data->pointer;
     if (data) {
       if (data->class == CLASS_A) {
         int i;
         initValue.arsize=data->arsize*bufsize;
         initValue.pointer=initValue.a0=malloc(initValue.arsize);
         memset(initValue.pointer,0,initValue.arsize);
         initValue.dimct = data->dimct+1;
         if (data->dimct == 1) {
	   initValue.arsize=data->arsize*bufsize;
           initValue.m[0]=data->arsize/data->length;
           initValue.m[1]=bufsize;
         } else {
	   A_COEFF_TYPE *data_c = (A_COEFF_TYPE *)data;
           for (i=0;i<data->dimct;i++) {
             initValue.m[i]=data_c->m[i];
	   }
           initValue.m[data->dimct]=bufsize;
	 }
       } else if (data->class == CLASS_S || data->class == CLASS_D) {
         initValue.arsize=data->length*bufsize;
         initValue.pointer=initValue.a0=malloc(initValue.arsize);
         memset(initValue.pointer,0,initValue.arsize);
         initValue.dimct = 1;
         initValue.m[0]=bufsize;
       }
       else {
	 status = TreeFAILURE;
       }
     } else {
       status = TreeFAILURE;
     }
     if (status & 1) {
       status = _TreeBeginTimestampedSegment(dbid, nid, (struct descriptor_a *)&initValue, -1);
       free(initValue.pointer);
       if (status && 1) {
	 status = _TreePutTimestampedSegment(dbid,nid,timestamp,data);
       }
     }
   }
   return status;
 }

int _TreeGetSegmentInfo(void *dbid, int nid, int idx, char *dtype, char *dimct, int *dims, int *next_row);

int TreeGetSegmentInfo(int nid, int idx, char *dtype, char *dimct, int *dims, int *next_row) {
  return _TreeGetSegmentInfo(*TreeCtx(), nid, idx, dtype, dimct, dims, next_row);
}

int _TreeGetSegmentInfo(void *dbid, int nid, int idx, char *dtype, char *dimct, int *dims, int *next_row) {
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID       *nid_ptr = (NID *)&nid;
  int       status=TreeFAILURE;
  int       open_status;
  TREE_INFO *info_ptr;
  int       nidx;
  NODE      *node_ptr;
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  nid_to_node(dblist, nid_ptr, node_ptr);
  if (!node_ptr)
    return TreeNNF;
  if (dblist->remote) {
    printf("Segmented records are not supported using thick client mode\n");
    return 0;
  }
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr)
  {
    NCI       local_nci;
    _int64    saved_viewdate;
    EXTENDED_ATTRIBUTES attributes;
    SEGMENT_HEADER segment_header;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciW(info_ptr, nidx, &local_nci,0);
    if (!(status & 1))
      return status;
    if (info_ptr->data_file == 0)
      open_status = TreeOpenDatafileR(info_ptr);
    else
      open_status = 1;
    if (!(open_status & 1)) {
      TreeUnLockNci(info_ptr,0,nidx);  
      return open_status;
    } 
    if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) || 
	((TreeGetExtendedAttributes(info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1)==0)) {
      status = TreeFAILURE;
    } else if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY]==0 ||
       ((GetSegmentHeader(info_ptr, attributes.facility_offset[SEGMENTED_RECORD_FACILITY],&segment_header)&1)==0)) {
      status = TreeFAILURE;
    }
    else {
      SEGMENT_INDEX index;
      if (idx == -1)
        idx = segment_header.idx;
      status = GetSegmentIndex(info_ptr, segment_header.index_offset, &index);
      while ((status &1) != 0 && idx < index.first_idx && index.previous_offset > 0)
	status = GetSegmentIndex(info_ptr, index.previous_offset, &index);
      if ((status&1) != 0 && idx >= index.first_idx && idx < index.first_idx + SEGMENTS_PER_INDEX) {
	SEGMENT_INFO *sinfo=&index.segment[idx % SEGMENTS_PER_INDEX];
	if (sinfo->data_offset == -1) {
	  status = TreeFAILURE;
	}
	else {
	  *dtype = segment_header.dtype;
	  *dimct = segment_header.dimct;
	  memcpy(dims,segment_header.dims,sizeof(segment_header.dims));
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

