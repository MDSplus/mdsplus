#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <treeshr.h>
#include <usagedef.h>
#include <libroutines.h>
#include <mdsshr.h>
#include "treeshrp.h"
extern void *DBID;
#ifdef __tolower
#undef __tolower
#endif
#define __tolower(c) (((c) >= 'A' && (c) <= 'Z') ? (c) | 0x20 : (c))

typedef ARRAY_COEFF(char,8) A_COEFF_TYPE;
static int PutNamedAttributesIndex(TREE_INFO *info, NAMED_ATTRIBUTES_INDEX *index, _int64 *offset);
int TreePutDsc(TREE_INFO *info_ptr, int nid, struct descriptor *dsc, _int64 *offset, int *length);
static int PutSegmentHeader(TREE_INFO *info_ptr, SEGMENT_HEADER *hdr, _int64 *offset);
static int PutSegmentIndex(TREE_INFO *info_ptr, SEGMENT_INDEX   *idx, _int64 *offset);
int TreePutExtendedAttributes(TREE_INFO *info_ptr, EXTENDED_ATTRIBUTES *att, _int64 *offset);
static int PutInitialValue(TREE_INFO *info_ptr, int *dims, struct descriptor_a *array, _int64 *offset);
static int PutDimensionValue(TREE_INFO *info_ptr, int ndims, int *dims, _int64 *offset);
static int GetSegmentHeader(TREE_INFO *info_ptr, _int64 offset,SEGMENT_HEADER *hdr);
static int GetSegmentIndex(TREE_INFO *info_ptr, _int64 offset, SEGMENT_INDEX *idx);
int TreeGetExtendedAttributes(TREE_INFO *info_ptr, _int64 offset, EXTENDED_ATTRIBUTES *att);
static int GetNamedAttributesIndex(TREE_INFO *info_ptr, _int64 offset, NAMED_ATTRIBUTES_INDEX *index);
int TreeGetDsc(TREE_INFO *info,_int64 offset, int length, struct descriptor_xd *dsc);
extern _int64 TreeTimeInserted();
static int _TreeBeginSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end,
			     struct descriptor *dimension,
			     struct descriptor_a *initialValue, int idx);

int TreeBeginSegment(int nid, struct descriptor *start, struct descriptor *end, 
		     struct descriptor *dimension,
		     struct descriptor_a *initialValue, int idx) {
  return  _TreeBeginSegment(DBID, nid, start, end, dimension, initialValue, idx);
}

int _TreeBeginSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end, 
		      struct descriptor *dimension,
		      struct descriptor_a *initialValue, int idx) {
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
    SEGMENT_HEADER segment_header;
    SEGMENT_INDEX segment_index;
    SEGMENT_INFO *sinfo;
    status = TreeCallHook(PutData,info_ptr,nid);
    if (status && !(status & 1))
      return status;
    if (info_ptr->reopen)
      TreeCloseFiles(info_ptr);
    if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
      open_status = TreeOpenDatafileW(info_ptr, &stv, 0);
    else
      open_status = 1;
    if (!(open_status & 1))
      return open_status;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciLw(info_ptr, nidx, &local_nci);
    local_nci.flags2 &= ~NciM_DATA_IN_ATT_BLOCK;
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
    segment_header.next_row=0;
    if (initialValue->dimct == 1)
      segment_header.dims[0]=initialValue->arsize/initialValue->length;
    else {
      memcpy(segment_header.dims,a_coeff->m,initialValue->dimct*sizeof(int));
    }
    status = PutInitialValue(info_ptr,segment_header.dims,initialValue,&segment_header.data_offset);
    if (idx >= segment_index.first_idx+SEGMENTS_PER_INDEX) {
      memset(&segment_index,0,sizeof(segment_index));
      segment_index.previous_offset=segment_header.index_offset;
      segment_header.index_offset=0;
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
  }
  return status;
}


static int _TreeUpdateSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end, struct descriptor *dimension,
			     int idx);

int TreeUpdateSegment(int nid, struct descriptor *start, struct descriptor *end, struct descriptor *dimension,
		      int idx) {
  return _TreeUpdateSegment(DBID, nid, start, end, dimension, idx);
}

static int _TreeUpdateSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end, struct descriptor *dimension,
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
    if (info_ptr->reopen)
      TreeCloseFiles(info_ptr);
    if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
      open_status = TreeOpenDatafileW(info_ptr, &stv, 0);
    else
      open_status = 1;
    if (!(open_status & 1))
      return open_status;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciLw(info_ptr, nidx, &local_nci);
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
  return _TreePutSegment(DBID, nid, startIdx, data);
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
    int bytes_to_insert;
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
    if (info_ptr->reopen)
      TreeCloseFiles(info_ptr);
    if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
      open_status = TreeOpenDatafileW(info_ptr, &stv, 0);
    else
      open_status = 1;
    if (!(open_status & 1))
      return open_status;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciLw(info_ptr, nidx, &local_nci);
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
    offset=segment_header.data_offset+startIdx*bytes_per_row;
#ifdef WORDS_BIGENDIAN
    buffer=memcpy(malloc(bytes_to_insert),data->pointer,bytes_to_insert);
    if (segment_header.length > 1 && array->dtype != DTYPE_T && array->dtype != DTYPE_IDENT && array->dtype != DTYPE_PATH) {
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
      for (i=0,bptr=buffer;i<byptes_to_insert/segment_header.length;i++,bptr+=sizeof(_int64))
	LoadQuad(((_int64 *)data->pointer)[i],bptr);
      break;
    }
    TreeLockDatafile(info_ptr,0,offset);
    MDS_IO_LSEEK(info_ptr->data_file->put,offset,SEEK_SET);
    status = (MDS_IO_WRITE(info->data_file->put,buffer,bytes_to_insert) == length) ? TreeSUCCESS : TreeFAILURE;
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
  return  _TreeGetNumSegments(DBID, nid, num);
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
  //  if (node_ptr->usage != TreeUSAGE_SIGNAL)
  //  return  TreeFAILURE;
  if (dblist->remote) {
    printf("Segmented records are not supported using thick client mode\n");
    return 0;
  }
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr)
  {
    int       stv;
    NCI       local_nci;
    _int64    saved_viewdate;
    EXTENDED_ATTRIBUTES attributes;
    SEGMENT_HEADER segment_header;
    if (info_ptr->reopen)
      TreeCloseFiles(info_ptr);
    if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
      open_status = TreeOpenDatafileR(info_ptr, &stv, 0);
    else
      open_status = 1;
    if (!(open_status & 1))
      return open_status;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciW(info_ptr, nidx, &local_nci,0);
    if (!(status & 1))
      return status;
    if (((local_nci.flags2 & NciM_EXTENDED_NCI) == 0) || 
	((TreeGetExtendedAttributes(info_ptr, RfaToSeek(local_nci.DATA_INFO.DATA_LOCATION.rfa), &attributes) & 1)==0)) {
      *num=0;
    } else if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY]==0 ||
       ((GetSegmentHeader(info_ptr, attributes.facility_offset[SEGMENTED_RECORD_FACILITY],&segment_header)&1)==0)) {
      *num=0;
    }
    else
      *num=segment_header.idx+1;
    TreeUnLockNci(info_ptr,0,nidx);
  }
  return status;
}

int _TreeGetSegment(void *dbid, int nid, int idx, struct descriptor_xd *segment, struct descriptor_xd *dim);
int TreeGetSegment(int nid, int idx, struct descriptor_xd *segment, struct descriptor_xd *dim) {
  return _TreeGetSegment(DBID, nid, idx, segment, dim);
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
    int       stv;
    NCI       local_nci;
    _int64    saved_viewdate;
    EXTENDED_ATTRIBUTES attributes;
    SEGMENT_HEADER segment_header;
    if (info_ptr->reopen)
      TreeCloseFiles(info_ptr);
    if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
      open_status = TreeOpenDatafileR(info_ptr, &stv, 0);
    else
      open_status = 1;
    if (!(open_status & 1))
      return open_status;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciW(info_ptr, nidx, &local_nci,0);
    if (!(status & 1))
      return status;
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
        if (sinfo->data_offset > -1) {
	  int i;
	  DESCRIPTOR_A(dim2,8,DTYPE_Q,0,0);
	  DESCRIPTOR_A_COEFF(ans,0,0,0,8,0);
	  ans.dtype=segment_header.dtype;
	  ans.length=segment_header.length;
	  ans.dimct=segment_header.dimct;
	  memcpy(ans.m,segment_header.dims,sizeof(segment_header.dims));
	  ans.m[segment_header.dimct-1]=sinfo->rows;
	  ans.arsize=ans.length;
	  for (i=0;i<ans.dimct; i++) ans.arsize *= ans.m[i];
	  ans.pointer = malloc(ans.arsize);
	  status = TreeLockDatafile(info_ptr, 1, sinfo->data_offset);
	  MDS_IO_LSEEK(info_ptr->data_file->get,sinfo->data_offset,SEEK_SET);
	  status = (MDS_IO_READ(info_ptr->data_file->get,ans.pointer,ans.arsize) == ans.arsize) ? TreeSUCCESS : TreeFAILURE;
	  status = TreeUnLockDatafile(info_ptr, 1, sinfo->data_offset);
	  if (status & 1) {
#ifdef WORDS_BIGENDIAN
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
		  *(_int64)bptr = swapquad(bptr);
		break;
	      }
	    }
#endif
	    if (sinfo->dimension_offset != -1 && sinfo->dimension_length==0) {
	      _int64 *tp;
	      status = TreeLockDatafile(info_ptr, 1, sinfo->dimension_offset);
	      dim2.arsize=sinfo->rows * sizeof(_int64);
	      dim2.pointer=malloc(dim2.arsize);
	      MDS_IO_LSEEK(info_ptr->data_file->get,sinfo->dimension_offset,SEEK_SET);
	      status = (MDS_IO_READ(info_ptr->data_file->get,dim2.pointer,dim2.arsize) == dim2.arsize) ? TreeSUCCESS : TreeFAILURE;
	      status = TreeUnLockDatafile(info_ptr, 1, sinfo->dimension_offset);
#ifdef WORDS_BIGENDIAN
	      for (i=0,bptr=dim2.pointer;i<dim2.arsize/dim2.length;i++,bptr+=sizeof(_int64))
		*(_int64)bptr = swapquad(bptr);
#endif
              for (tp=(_int64 *)(dim2.pointer+dim2.arsize-dim2.length); (tp >= (_int64 *)dim2.pointer) && (*tp==0);tp--) {
		ans.m[segment_header.dimct-1]--;
		dim2.arsize-=sizeof(_int64);
	      }
	      ans.arsize=ans.length;
	      for (i=0;i<ans.dimct; i++) ans.arsize *= ans.m[i];
	      MdsCopyDxXd((struct descriptor *)&dim2,dim);
	      free(dim2.pointer);
	    } else {
	      TreeGetDsc(info_ptr,sinfo->dimension_offset,sinfo->dimension_length,dim);
	    }
	    MdsCopyDxXd((struct descriptor *)&ans,segment);
	  } else {
	    status = TreeFAILURE;
	  }
	  free(ans.pointer);
	}
        else {
	  status = TreeFAILURE;
	}
      } else {
	status = TreeFAILURE;
      }
    }
    TreeUnLockNci(info_ptr,0,nidx);
  }
  return status;
}

int _TreeGetSegmentLimits(void *dbid, int nid, int idx, struct descriptor_xd *retStart, struct descriptor_xd *retEnd);
int TreeGetSegmentLimits(int nid, int idx, struct descriptor_xd *retStart, struct descriptor_xd *retEnd) {
  return _TreeGetSegmentLimits(DBID, nid, idx, retStart, retEnd);
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
    int       stv;
    NCI       local_nci;
    _int64    saved_viewdate;
    EXTENDED_ATTRIBUTES attributes;
    SEGMENT_HEADER segment_header;
    if (info_ptr->reopen)
      TreeCloseFiles(info_ptr);
    if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
      open_status = TreeOpenDatafileR(info_ptr, &stv, 0);
    else
      open_status = 1;
    if (!(open_status & 1))
      return open_status;
    TreeGetViewDate(&saved_viewdate);
    status = TreeGetNciW(info_ptr, nidx, &local_nci,0);
    if (!(status & 1))
      return status;
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
	  int length =sizeof(_int64) * sinfo->rows;
	  char *buffer=malloc(length),*bptr;
	  _int64 timestamp;
	  TreeLockDatafile(info_ptr,1,sinfo->dimension_offset);
	  MDS_IO_LSEEK(info_ptr->data_file->get,sinfo->dimension_offset,SEEK_SET);
	  status = (MDS_IO_READ(info_ptr->data_file->get,buffer,length) == length) ? TreeSUCCESS : TreeFAILURE;
	  TreeUnLockDatafile(info_ptr,1,sinfo->dimension_offset);
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
    TreeUnLockNci(info_ptr,0,nidx);
  }
  return status;
}

int _TreeSetXNci(void *dbid, int nid, char *xnciname, struct descriptor *value);

int TreeSetXNci(int nid, char *xnciname, struct descriptor *value) {
  return _TreeSetXNci(DBID, nid, xnciname, value);
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
    if (info_ptr->reopen)
      TreeCloseFiles(info_ptr);
    if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
      open_status = TreeOpenDatafileW(info_ptr, &stv, 0);
    else
      open_status = 1;
    if (!(open_status & 1))
      return open_status;
    TreeGetViewDate(&saved_viewdate);
    status=TreePutDsc(info_ptr,nid,value,&value_offset,&value_length);
    if (!(status & 1))
      return status;
    status = TreeGetNciLw(info_ptr, nidx, &local_nci);
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
  return _TreeGetXNci(DBID, nid, xnciname, value);
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
    int       stv;
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
    int i;
    int len=strlen(xnciname);
    if (info_ptr->reopen)
      TreeCloseFiles(info_ptr);
    if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
      open_status = TreeOpenDatafileR(info_ptr, &stv, 0);
    else
      open_status = 1;
    if (!(open_status & 1))
      return open_status;
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
	int i,j;
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
    TreeUnLockNci(info_ptr,0,nidx);
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
    status = (MDS_IO_WRITE(info->data_file->put,ap->pointer,ap->arsize) == ap->arsize) ? TreeSUCCESS : TreeFAILURE;
    TreeUnLockDatafile(info,0,0);
    *length=ap->arsize;
    MdsFree1Dx(&xd,0);
  }
  return status;
}

int TreeGetDsc(TREE_INFO *info,_int64 offset, int length, struct descriptor_xd *dsc) {
  char *buffer = malloc(length);
  int status;
  status = TreeLockDatafile(info, 1, offset);
  MDS_IO_LSEEK(info->data_file->get,offset,SEEK_SET);
  status = (MDS_IO_READ(info->data_file->get,buffer,length) == length) ? TreeSUCCESS : TreeFAILURE;
  status = TreeUnLockDatafile(info, 1, offset);
  if (status & 1) {
    status = MdsSerializeDscIn(buffer,dsc);
  }
  free(buffer);
  return status;
}

static int PutSegmentHeader(TREE_INFO *info, SEGMENT_HEADER *hdr, _int64 *offset) {
  int status;
  _int64 loffset;
  int j;
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

static int PutDimensionValue(TREE_INFO *info,int ndims, int *dims, _int64 *offset) {
  int status;
  int length=sizeof(_int64);
  _int64 loffset;
  unsigned char *buffer;
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
  length=length*dims[ndims-1];
  buffer=malloc(length);
  memset(buffer,0,length);
  status = (MDS_IO_WRITE(info->data_file->put,buffer,length) == length) ? TreeSUCCESS : TreeFAILURE;
  free(buffer);
  TreeUnLockDatafile(info,0,loffset);
  return status;
}

static int GetSegmentHeader(TREE_INFO *info, _int64 offset,SEGMENT_HEADER *hdr) {
  int status = TreeFAILURE;
  int i;
  unsigned char buffer[2*sizeof(char)+1*sizeof(short)+10*sizeof(int)+3*sizeof(_int64)],*bptr;
  if (offset > -1) {
    status = TreeLockDatafile(info, 1, offset);
    MDS_IO_LSEEK(info->data_file->get,offset,SEEK_SET);
    status = (MDS_IO_READ(info->data_file->get,buffer,sizeof(buffer)) == sizeof(buffer)) ? TreeSUCCESS : TreeFAILURE;
    TreeUnLockDatafile(info, 1, offset);
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
    status = TreeLockDatafile(info, 1, offset);
    MDS_IO_LSEEK(info->data_file->get,offset,SEEK_SET);
    status = (MDS_IO_READ(info->data_file->get,buffer,sizeof(buffer)) == sizeof(buffer)) ? TreeSUCCESS : TreeFAILURE;
    TreeUnLockDatafile(info, 1, offset);
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
    status = TreeLockDatafile(info, 1, offset);
    MDS_IO_LSEEK(info->data_file->get,offset,SEEK_SET);
    status = (MDS_IO_READ(info->data_file->get,buffer,sizeof(buffer)) == sizeof(buffer)) ? TreeSUCCESS : TreeFAILURE;
    TreeUnLockDatafile(info, 1, offset);
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
    status = TreeLockDatafile(info, 1, offset);
    MDS_IO_LSEEK(info->data_file->get,offset,SEEK_SET);
    status = (MDS_IO_READ(info->data_file->get,buffer,sizeof(buffer)) == sizeof(buffer)) ? TreeSUCCESS : TreeFAILURE;
    TreeUnLockDatafile(info, 1, offset);
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

 int _TreeBeginTimestampedSegment(void *dbid, int nid, struct descriptor_a *initialValue, int idx);

 int TreeBeginTimestampedSegment(int nid, struct descriptor_a *initialValue, int idx) {
   return _TreeBeginTimestampedSegment(DBID, nid, initialValue, idx);
 }


 int _TreeBeginTimestampedSegment(void *dbid, int nid, struct descriptor_a *initialValue, int idx) {
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
     SEGMENT_HEADER segment_header;
     SEGMENT_INDEX segment_index;
     SEGMENT_INFO *sinfo;
     status = TreeCallHook(PutData,info_ptr,nid);
     if (status && !(status & 1))
       return status;
     if (info_ptr->reopen)
       TreeCloseFiles(info_ptr);
     if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
       open_status = TreeOpenDatafileW(info_ptr, &stv, 0);
     else
       open_status = 1;
     if (!(open_status & 1))
       return open_status;
     TreeGetViewDate(&saved_viewdate);
     status = TreeGetNciLw(info_ptr, nidx, &local_nci);
     local_nci.flags2 &= ~NciM_DATA_IN_ATT_BLOCK;
     local_nci.dtype=initialValue->dtype;
     local_nci.class=CLASS_A;
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
     segment_header.next_row=0;
     segment_header.data_offset=-1;
     segment_header.dim_offset=-1;
     if (initialValue->dimct == 1)
       segment_header.dims[0]=initialValue->arsize/initialValue->length;
     else {
       memcpy(segment_header.dims,a_coeff->m,initialValue->dimct*sizeof(int));
     }
     status = PutInitialValue(info_ptr,segment_header.dims,initialValue,&segment_header.data_offset);
     status = PutDimensionValue(info_ptr,initialValue->dimct, segment_header.dims,&segment_header.dim_offset);
     if (idx >= segment_index.first_idx+SEGMENTS_PER_INDEX) {
       memset(&segment_index,0,sizeof(segment_index));
       segment_index.previous_offset=segment_header.index_offset;
       segment_header.index_offset=-1;
       segment_index.first_idx=idx;
     }
     sinfo=&segment_index.segment[idx % SEGMENTS_PER_INDEX];
     sinfo->start=0;
     sinfo->end=0;
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
   return _TreePutTimestampedSegment(DBID, nid, timestamp, data);
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
       int rows_to_insert;
       int bytes_per_row;
       int rows_in_segment;
       int bytes_to_insert;
#ifdef WORDS_BIGENDIAN
       unsigned char *buffer,*bptr;
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
       if (info_ptr->reopen)
	 TreeCloseFiles(info_ptr);
       if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
	 open_status = TreeOpenDatafileW(info_ptr, &stv, 0);
       else
	 open_status = 1;
       if (!(open_status & 1))
	 return open_status;
       TreeGetViewDate(&saved_viewdate);
       status = TreeGetNciLw(info_ptr, nidx, &local_nci);
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
       if (attributes.facility_offset[SEGMENTED_RECORD_FACILITY]==-1) {
	 status = TreeFAILURE;
       } else if (((status = GetSegmentHeader(info_ptr, attributes.facility_offset[SEGMENTED_RECORD_FACILITY],&segment_header))&1)==0) {
	 status=status;
       } else if (data->dtype != segment_header.dtype) {
	 status = TreeFAILURE;
       } else if (!(data->dimct == 1 && segment_header.dimct == 1) && (data->dimct != segment_header.dimct-1)) {
	 status = TreeFAILURE;
       } else if (a_coeff->dimct > 1 && memcmp(segment_header.dims,a_coeff->m,(segment_header.dimct-1)*sizeof(int)) != 0) {
	 status = TreeFAILURE;
       } else if (a_coeff->dimct == 1 && a_coeff->arsize/a_coeff->length != 1 && segment_header.dims[0] != a_coeff->arsize/a_coeff->length) {
	 status = TreeFAILURE;
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
       if (rows_to_insert != 1) {
	 TreeUnLockNci(info_ptr,0,nidx);
	 return TreeFAILURE;
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
       if (segment_header.length > 1 && array->dtype != DTYPE_T && array->dtype != DTYPE_IDENT && array->dtype != DTYPE_PATH) {
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
	   for (i=0,bptr=buffer;i<byptes_to_insert/segment_header.length;i++,bptr+=sizeof(_int64))
	     LoadQuad(((_int64 *)data->pointer)[i],bptr);
	   break;
	 }
       }
       TreeLockDatafile(info_ptr,0,offset);
       MDS_IO_LSEEK(info_ptr->data_file->put,offset,SEEK_SET);
       status = (MDS_IO_WRITE(info->data_file->put,buffer,bytes_to_insert) == length) ? TreeSUCCESS : TreeFAILURE;
       MDS_IO_LSEEK(info_ptr->data_file->put,segment_header.dim_offset+startIdx*sizeof(_int64),SEEK_SET);
       LoadQuad(*timestamp,tstamp);
       status = (MDS_IO_WRITE(info->data_file->put,tstamp,sizeof(_int64)) == sizeof(_int64)) ? TreeSUCCESS : TreeFAILURE;
       free(buffer);
#else
       TreeLockDatafile(info_ptr,0,offset);
       MDS_IO_LSEEK(info_ptr->data_file->put,offset,SEEK_SET);
       status = (MDS_IO_WRITE(info_ptr->data_file->put,data->pointer,bytes_to_insert) == bytes_to_insert) ? TreeSUCCESS : TreeFAILURE;
       MDS_IO_LSEEK(info_ptr->data_file->put,segment_header.dim_offset+startIdx*sizeof(_int64),SEEK_SET);
       status = (MDS_IO_WRITE(info_ptr->data_file->put,timestamp,sizeof(_int64)) == sizeof(_int64)) ? TreeSUCCESS : TreeFAILURE;
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
   if (offset1 != -1) {
     char *data=malloc(length);
     status = TreeUnLockDatafile(info1, 1, offset1);
     MDS_IO_LSEEK(info1->data_file->get,offset1,SEEK_SET);
     status = MDS_IO_READ(info1->data_file->get,data,length) == length;
     status = TreeUnLockDatafile(info1, 1, offset1);
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

 int TreeGetSegmentedRecord(int nid, struct descriptor_xd *data) {
   static int activated=0;
   static int (*addr)(int, struct descriptor *, struct descriptor *, struct descriptor *, struct descriptor_xd *);
   int status=42;
   if (!activated) {
     static DESCRIPTOR(library,"XTreeShr");
     static DESCRIPTOR(routine,"XTreeGetTimedRecord");
     status = LibFindImageSymbol(&library,&routine,&addr);
     if (status & 1) {
       activated=1;
     } else {
       fprintf(stderr,"Error activating XTreeShr library. Cannot access segmented records.\n");
       return status;
     }
   }
   status = (*addr)(nid, TREE_START_CONTEXT.pointer, TREE_END_CONTEXT.pointer, TREE_DELTA_CONTEXT.pointer, data);
   return status;
 }
