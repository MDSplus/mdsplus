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

#include <mdsdescrip.h>
#include <mdsshr.h>
#include <ncidef.h>
#include "treeshrp.h"
#include <treeshr.h>
#include <usagedef.h>
#include <ncidef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <librtl_messages.h>
#include <strroutines.h>
#include <libroutines.h>

#define min(a,b) (((a) < (b)) ? (a) : (b))

static int CheckUsage(PINO_DATABASE *dblist, NID *nid_ptr, NCI *nci);
static int FixupNid(NID *nid, unsigned char *tree, struct descriptor *path);
static int FixupPath();
static int CopyToRecord(NCI *nci, struct descriptor *descriptor_ptr, struct descriptor_xd *io_dscr_ptr, unsigned char tree);
static int PointerToOffset(struct descriptor *dsc_ptr, unsigned int *length);
static int StartEvent(NID *nid_ptr, int utility_update);
static void EndEvent(int id, int length, int status, int stv);
static int AddQuadword(unsigned int *a, unsigned int *b, unsigned int *ans);
static int       OpenDatafileW(TREE_INFO *info, int *stv_ptr);
static int PutDatafile(TREE_INFO *info, int nodenum, NCI *nci_ptr, struct descriptor_xd *data_dsc_ptr);

static int compress_utility;
static char nid_reference;
static char path_reference;

static int Dsc2Rec(struct descriptor *inp, struct descriptor_xd *out_dsc_ptr);

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
  int       old_record_length;
  static int saved_uic = 0;
  int       id = 0;
  int       length = 0;
  int       shot_open;
  compress_utility = utility_update == 2;
#if !defined(_WINDOWS)
  if (!saved_uic)
    saved_uic = (getgid() << 16) | getuid();
#endif
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  if (dblist->open_readonly)
    return TreeREADONLY;
  shot_open = (dblist->shotid != -1);
  nid_to_tree_nidx(dblist, nid_ptr, info_ptr, nidx);
  if (info_ptr)
  {
    int       stv;
    NCI       local_nci;
    if (info_ptr->reopen)
      TreeCloseFiles(info_ptr);
    if (info_ptr->data_file ? (!info_ptr->data_file->open_for_write) : 1)
      open_status = OpenDatafileW(info_ptr, &stv);
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
        unsigned int m1[2] = {0,0};
        unsigned int m2[2] = {10000000,0};
        unsigned int zero[2] = {0,0};
        unsigned int addin[2] = {0x4beb4000,0x7c9567};
        unsigned int temp[2] = {0,0};
        unsigned int time_inserted[2];
        bitassign(dblist->setup_info, local_nci.flags, NciM_SETUP_INFORMATION);
	local_nci.owner_identifier = saved_uic;
	/* VMS time = unixtime * 10,000,000 + 0x7c95674beb4000q */
        tzset();
        m1[0] = (unsigned int)time(NULL) - timezone;
	LibEmul(m1,m2,zero,temp);
        AddQuadword(temp,addin,time_inserted);
        memcpy(local_nci.time_inserted,time_inserted,sizeof(time_inserted));
      }
      if (!(open_status & 1))
      {
	local_nci.DATA_INFO.ERROR_INFO.stv = stv;
	local_nci.error_on_put = 1;
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
	  old_record_length = nci->data_in_att_block ? 0 : nci->DATA_INFO.DATA_LOCATION.record_length;
	  if ((nci->flags & NciM_WRITE_ONCE) && nci->length)
	    status = TreeNOOVERWRITE;
	  if ((status & 1) && (shot_open && (nci->flags & NciM_NO_WRITE_SHOT)))
	    status = TreeNOWRITESHOT;
	  if ((status & 1) && (!shot_open && (nci->flags & NciM_NO_WRITE_MODEL)))
	    status = TreeNOWRITEMODEL;
	}
	if (status & 1)
	  status = CopyToRecord(nci, descriptor_ptr, info_ptr->data_file->data,
				(unsigned char)nid_ptr->tree);
	if ((status & 1) && nci->length && (!utility_update))
	  status = CheckUsage(dblist, nid_ptr, nci);
	if (status & 1)
	{
	  if (nci->data_in_att_block)
	  {
	    nci->error_on_put = 0;
	    status = TreePutNci(info_ptr, nidx, nci, 1);
	  }
	  else
	  {
	    if ((nci->DATA_INFO.DATA_LOCATION.record_length != old_record_length) ||
		(nci->DATA_INFO.DATA_LOCATION.record_length >= DATAF_C_MAX_RECORD_SIZE) ||
		utility_update ||
		nci->error_on_put)
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


static int CopyToRecord(NCI *nci,
			          struct descriptor *descriptor_ptr,
			          struct descriptor_xd *io_dscr_ptr,
			          unsigned char tree)
{
  int       status;
  short     data_len;
  struct descriptor *out_ptr;
  nid_reference = 0;
  path_reference = 0;
  status = MdsCopyDxXdZ(descriptor_ptr, io_dscr_ptr, 0, FixupNid, &tree, FixupPath, NULL);
  bitassign(path_reference,nci->flags,NciM_PATH_REFERENCE);
  bitassign(nid_reference,nci->flags,NciM_NID_REFERENCE);
  if (status == MdsCOMPRESSIBLE)
  {
    if ((compress_utility || (nci->flags & NciM_COMPRESS_ON_PUT)) &&
        (!(nci->flags & NciM_DO_NOT_COMPRESS)))
    {
      static    EMPTYXD(empty);
      static    EMPTYXD(temp);
      temp = *io_dscr_ptr;
      *io_dscr_ptr = empty;
      status = MdsCompress(0, 0, (struct descriptor *)&temp, io_dscr_ptr);
      MdsFree1Dx(&temp, NULL);
      bitassign(0,nci->flags,NciM_COMPRESSIBLE);
    }
    else
      bitassign(1,nci->flags,NciM_COMPRESSIBLE);
  }
  else
    bitassign(0,nci->flags,NciM_COMPRESSIBLE);
  if (status & 1)
  {
    if (io_dscr_ptr->pointer)
    {
      if (io_dscr_ptr->dtype == DTYPE_DSC)
      {
	out_ptr = io_dscr_ptr->pointer;
	switch (out_ptr->class)
	{
	 case CLASS_S:
	 case CLASS_D:
	  nci->length = io_dscr_ptr->l_length;
	  nci->dtype = out_ptr->dtype;
	  nci->class = CLASS_S;
	  if (nci->length - sizeof(struct descriptor) < sizeof(nci->DATA_INFO.DATA_IN_RECORD.data))
	  {
	    data_len = nci->length - sizeof(struct descriptor);
            nci->length = data_len + 8;
	    nci->data_in_att_block = 1;
            memcpy(nci->DATA_INFO.DATA_IN_RECORD.data, out_ptr->pointer, data_len);
	  }
	  else
	  {           
            nci->DATA_INFO.DATA_LOCATION.record_length = nci->length;
	    nci->length = 0;
	    status = PointerToOffset(io_dscr_ptr->pointer, (unsigned int *)&nci->length);
#if  !(defined(_WINDOWS))
            if (status & 1)
	    { struct descriptor_xd tempxd = *io_dscr_ptr;
              io_dscr_ptr->l_length = 0;
              io_dscr_ptr->pointer = 0;
	      Dsc2Rec(tempxd.pointer,io_dscr_ptr);
              MdsFree1Dx(&tempxd, NULL);
            }
#endif
	    nci->data_in_att_block = 0;
	  }
	  break;

	 case CLASS_R:
	 case CLASS_A:
	 case CLASS_XD:
	 case CLASS_XS:
	 case CLASS_APD:
	 case CLASS_CA:
	  nci->DATA_INFO.DATA_LOCATION.record_length = io_dscr_ptr->l_length;
	  nci->dtype = out_ptr->dtype;
	  nci->class = (out_ptr->class == CLASS_XD) ? CLASS_XS : out_ptr->class;
	  nci->data_in_att_block = 0;
	  nci->length = 0;
	  status = PointerToOffset(io_dscr_ptr->pointer, (unsigned int *)&nci->length);
#if  !(defined(_WINDOWS))
            if (status & 1)
	    { struct descriptor_xd tempxd = *io_dscr_ptr;
              io_dscr_ptr->l_length = 0;
              io_dscr_ptr->pointer = 0;
	      Dsc2Rec(tempxd.pointer,io_dscr_ptr);
              MdsFree1Dx(&tempxd, NULL);
            }
#endif
	  break;

	 default:
	  status = LibINVSTRDES;
	}
      }
      else
      {
	nci->length = io_dscr_ptr->l_length;
	nci->dtype = io_dscr_ptr->dtype;
	nci->class = (io_dscr_ptr->class == CLASS_XD) ? CLASS_XS : io_dscr_ptr->class;
	nci->data_in_att_block = 0;
	nci->DATA_INFO.DATA_LOCATION.record_length = nci->length;
      }
    }
    else
    {
      nci->length = 0;
      nci->dtype = 0;
      nci->class = 0;
      nci->DATA_INFO.DATA_LOCATION.record_length = 0;
      nci->data_in_att_block = 1;
    }
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

static int PointerToOffset(struct descriptor *dsc_ptr, unsigned int *length)
{
  int       status = 1;
  if ((dsc_ptr->dtype == DTYPE_DSC) && (dsc_ptr->class != CLASS_A))
    status = PointerToOffset((struct descriptor *) dsc_ptr->pointer, length);
  if (status & 1)
  {
    switch (dsc_ptr->class)
    {
     case CLASS_S:
     case CLASS_D:
      *length += sizeof(struct descriptor) + dsc_ptr->length;
      dsc_ptr->pointer = dsc_ptr->pointer - (int) dsc_ptr;
      break;
     case CLASS_XD:
     case CLASS_XS:
      *length += sizeof(struct descriptor_xd) + ((struct descriptor_xd *) dsc_ptr)->l_length;
      dsc_ptr->pointer = dsc_ptr->pointer - (int) dsc_ptr;
      break;
     case CLASS_R:
      {
	struct descriptor_r *r_ptr = (struct descriptor_r *) dsc_ptr;
	int       i;
	*length += sizeof(*r_ptr) + (r_ptr->ndesc - 1) * sizeof(struct descriptor *)
		   + r_ptr->length;
	if (r_ptr->length != 0)
	  r_ptr->pointer = r_ptr->pointer - (int) r_ptr;
	for (i = 0; (status & 1) && (i < r_ptr->ndesc); i++)
	  if (r_ptr->dscptrs[i] != 0)
	  {
	    status = PointerToOffset(r_ptr->dscptrs[i], length);
	    r_ptr->dscptrs[i] = (struct descriptor *) ((char *) r_ptr->dscptrs[i] - (int) r_ptr);
	  }
      }
      break;
     case CLASS_A:
      {
	struct descriptor_a *a_ptr = (struct descriptor_a *) dsc_ptr;
	*length += sizeof(struct descriptor_a)
		+ (a_ptr->aflags.coeff ? sizeof(int) * (a_ptr->dimct + 1) : 0)
		+ (a_ptr->aflags.bounds ? sizeof(int) * (a_ptr->dimct * 2) : 0)
		+ a_ptr->arsize;
	a_ptr->pointer = a_ptr->pointer - (int) a_ptr;
	if (a_ptr->aflags.coeff)
	{
	  int     *a0_ptr = (int *)((char *) a_ptr + sizeof(struct descriptor_a));
	  *a0_ptr = *a0_ptr - (int) a_ptr;
	}
      }
      break;
     case CLASS_APD:
      {
	int       i;
	struct descriptor_a *a_ptr = (struct descriptor_a *) dsc_ptr;
	int       elts = a_ptr->arsize / a_ptr->length;
	*length += sizeof(struct descriptor_a)
		+ (a_ptr->aflags.coeff ? sizeof(int) * (a_ptr->dimct + 1) : 0)
		+ (a_ptr->aflags.bounds ? sizeof(int) * (a_ptr->dimct * 2) : 0)
		+ a_ptr->arsize;
	for (i = 0; (i < elts) && (status & 1); i++)
	{
	  struct descriptor **dsc_ptr = (struct descriptor **) a_ptr->pointer + i;
	  if (dsc_ptr && *dsc_ptr)
	  {
	    status = PointerToOffset(*dsc_ptr, length);
	    *dsc_ptr = (struct descriptor *) ((char *) *dsc_ptr - (int) a_ptr);
	  }
	  else
	    status = 1;
	}
	if (status & 1)
	{
	  a_ptr->pointer = a_ptr->pointer - (int) a_ptr;
	  if (a_ptr->aflags.coeff)
	  {
	    char     *a0_ptr = (char *) a_ptr + sizeof(struct descriptor_a);
	    *a0_ptr = *a0_ptr - (int) a_ptr;
	  }
	}
      }
      break;
     case CLASS_CA:
      if (dsc_ptr->pointer)
      {
	unsigned int dummy_length;
	struct descriptor_a *a_ptr = (struct descriptor_a *) dsc_ptr;
	*length += sizeof(struct descriptor_a)
		+ (a_ptr->aflags.coeff ? sizeof(int) * (a_ptr->dimct + 1) : 0)
		+ (a_ptr->aflags.bounds ? sizeof(int) * (a_ptr->dimct * 2) : 0)
		+ a_ptr->arsize;
	status = PointerToOffset((struct descriptor *) dsc_ptr->pointer, &dummy_length);
	a_ptr->pointer = a_ptr->pointer - (int) a_ptr;
      }
      break;
     default:
      status = LibINVSTRDES;
      break;
    }
  }
  return status;
}

static int       OpenDatafileW(TREE_INFO *info, int *stv_ptr)
{
  int       status = 1;
  int       tries = 0;
  static char *datafile_name = ".datafile";
  DATA_FILE *df_ptr = info->data_file;
  static    DESCRIPTOR(dataf_buffering, "DATAF");
  static RECORD_HEADER header;
/*------------------------------------------------------------------------------

 Executable:
*/
  *stv_ptr = 0;
  if (df_ptr != 0)
  {
    df_ptr = TreeGetVmDatafile();
    status = (df_ptr == NULL) ? TreeFAILURE : TreeNORMAL;
  }
  if (status & 1)
  {
    size_t len = strlen(info->filespec)-4;
    char *filename = strncpy(malloc(len+9),info->filespec,len);
    filename[len]='\0';
    strcat(filename,"datafile");
    df_ptr->get = fopen(filename,"rb");
    status = (df_ptr->get == NULL) ? TreeFAILURE : TreeNORMAL;
    if (status & 1)
    {
      df_ptr->put = fopen(filename,"r+b");
      status = (df_ptr->put == NULL) ? TreeFAILURE : TreeNORMAL;
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
  int       status = 1;
  int       bytes_to_put = nci_ptr->DATA_INFO.DATA_LOCATION.record_length;
  info->data_file->record_header->node_number = nodenum;
  memset(&info->data_file->record_header->rfa,0,sizeof(RFA));
  while (bytes_to_put && (status & 1))
  {
    int bytes_this_time = min(DATAF_C_MAX_RECORD_SIZE + 2, bytes_to_put);
    int eof;
    int rfa[2];
    fseek(info->data_file->put,0,SEEK_END);
    eof = ftell(info->data_file->put);
    bytes_to_put -= bytes_this_time;
    info->data_file->record_header->rlength = (unsigned short)(bytes_this_time + 10);
    status = fwrite((char *) info->data_file->record_header,sizeof(RECORD_HEADER), 1, info->data_file->put) == 1;
    status = fwrite((char *) data_dsc_ptr->pointer + bytes_to_put, bytes_this_time, 1, info->data_file->put) == 1;
    if (!bytes_to_put)
    {
      if (status & 1)
      {
        nci_ptr->error_on_put = 0;
        SeekToRfa(eof,rfa);
        memcpy(nci_ptr->DATA_INFO.DATA_LOCATION.rfa,rfa,sizeof(nci_ptr->DATA_INFO.DATA_LOCATION.rfa));
      }
      else
      {
        nci_ptr->error_on_put = 1;
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
  }
  fflush(info->data_file->put);
  return status;
}

static int UpdateDatafile(TREE_INFO *info, int nodenum, NCI *nci_ptr, struct descriptor_xd *data_dsc_ptr)
{
  int       status = 1;
  int       bytes_to_put = nci_ptr->DATA_INFO.DATA_LOCATION.record_length;
  info->data_file->record_header->node_number = nodenum;
  memset(&info->data_file->record_header->rfa,0,sizeof(RFA));
  while (bytes_to_put && (status & 1))
  {
    int bytes_this_time = min(DATAF_C_MAX_RECORD_SIZE + 2, bytes_to_put);
    int rfa_l = RfaToSeek(nci_ptr->DATA_INFO.DATA_LOCATION.rfa);
    fseek(info->data_file->put,rfa_l,SEEK_SET);
    bytes_to_put -= bytes_this_time;
    info->data_file->record_header->rlength = (unsigned short)(bytes_this_time + 10);
    status = fwrite((char *) info->data_file->record_header,sizeof(RECORD_HEADER), 1, info->data_file->put) == 1;
    status = fwrite((char *) data_dsc_ptr->pointer + bytes_to_put, bytes_this_time, 1, info->data_file->put) == 1;
    if (!bytes_to_put)
    {
      if (status & 1)
      {
        nci_ptr->error_on_put = 0;
      }
      else
      {
        nci_ptr->error_on_put = 1;
        nci_ptr->DATA_INFO.ERROR_INFO.error_status = status;
        nci_ptr->length = 0;
      }
      TreePutNci(info, nodenum, nci_ptr, 1);
    }
  }
  fflush(info->data_file->put);
  return status;
}

#ifdef _big_endian
#define swapquad(in) {int stmp; int *iptr = (int *)in; stmp=iptr[0]; iptr[0]=iptr[1]; iptr[1]=stmp;}
#else
#define swapquad(in) 
#endif

static int AddQuadword(unsigned int *a, unsigned int *b, unsigned int *ans)
{
  int i;
  int carry=0;
  unsigned int la[2];
  unsigned int lb[2];
  la[0] = a[0];
  la[1] = a[1];
  lb[0] = b[0];
  lb[1] = b[1];
  swapquad(la);
  swapquad(lb);
  for (i=0; i<2; i++) {
    unsigned int _a = la[i];
    unsigned int _b = lb[i];
    ans[i] = _a + _b + carry;
    carry = (ans[i] <= _a) && ((_b != 0) || (carry != 0));
  }
  swapquad(ans);
  return !carry;
}

#define length_of(ptr)    ((unsigned short *)&ptr[0])
#define dtype_of(ptr)     ((unsigned char  *)&ptr[2])
#define class_of(ptr)     ((unsigned char  *)&ptr[3])
#define pointer_of(ptr)   ((int   *)&ptr[4])
#define l_length_of(ptr)  ((unsigned int   *)&ptr[8])
#define ndesc_of(ptr)     ((unsigned char  *)&ptr[8])
#define dscptrs_of(ptr,j) ((unsigned int   *)&ptr[12+j*4])
#define scale_of(ptr)     ((unsigned char  *)&ptr[8])
#define digits_of(ptr)	  ((unsigned char  *)&ptr[9])
#define set_aflags(ptr,in)  ptr[10] = (inp->aflags.binscale << 3)  | (inp->aflags.redim << 4) | (inp->aflags.column << 5) \
                                     | (inp->aflags.coeff << 6) | (inp->aflags.bounds << 7)
#define dimct_of(ptr)     ((unsigned char  *)&ptr[11])
#define arsize_of(ptr)    ((unsigned int   *)&ptr[12])
#define a0_of(ptr)        ((int   *)&ptr[16])
#define m_of(ptr)         ((unsigned int   *)&ptr[20])
#define offset(ptr)       *(unsigned int *)&ptr


/*-----------------------------------------------------------------
	Recursively compact all descriptors and adjust pointers.
	NIDs converted to PATHs for TREE$COPY_TO_RECORD.
	Eliminates DSC descriptors. Need DSC for classes A and APD?
-----------------------------------------------------------------*/

static int copy_dx_rec( struct descriptor *in_ptr,char *out_ptr,unsigned int *b_out, unsigned int *b_in)
{
  unsigned int status = 1,
              bytes_out = 0,
              bytes_in = 0,
              i,j,
              size_out,
              size_in,
              num_dsc;
  if (in_ptr)
    switch (in_ptr->class)
    {
     case CLASS_S:
     case CLASS_D:
      {
	if (out_ptr)
	{
          *length_of(out_ptr) = in_ptr->length;
          *dtype_of(out_ptr) = in_ptr->dtype;
          *class_of(out_ptr) = in_ptr->class;
          *pointer_of(out_ptr) = 8;
          out_ptr += 8;
          memcpy(out_ptr,((char *)in_ptr) + offset(in_ptr->pointer), in_ptr->length); 
          out_ptr += in_ptr->length;
	}
	bytes_out = 8 + in_ptr->length;
        bytes_in = sizeof(struct descriptor) + in_ptr->length;
      }
      break;

     case CLASS_XS:
     case CLASS_XD:
      {
	struct descriptor_xs *inp = (struct descriptor_xs *)in_ptr;
	if (out_ptr)
	{
          *length_of(out_ptr) = 0;
          *dtype_of(out_ptr) = inp->dtype;
          *class_of(out_ptr) = inp->class;
          *pointer_of(out_ptr) = 12;
          *l_length_of(out_ptr) = inp->l_length;
          out_ptr += 12; 
          memcpy(out_ptr, ((char *)in_ptr) + offset(in_ptr->pointer), inp->l_length);
          out_ptr += inp->l_length;
	}
	bytes_in = sizeof(struct descriptor_xs) + inp->l_length;
	bytes_out = 12 + inp->l_length;
      }
      break;

     case CLASS_R:
      {
        struct descriptor_r *inp = (struct descriptor_r *)in_ptr;
        char *begin = out_ptr;
        unsigned int *dscptrs = NULL;
        if (out_ptr)
	{
          *length_of(out_ptr) = inp->length;
          *dtype_of(out_ptr) = inp->dtype;
          *class_of(out_ptr) = inp->class;
          *pointer_of(out_ptr) = inp->length ? 12 + inp->ndesc * 4 : 0;
          *ndesc_of(out_ptr) = inp->ndesc;
          dscptrs = (unsigned int *)(out_ptr + 12);
          memset(dscptrs,0,inp->ndesc * 4);
          out_ptr += 12 + inp->ndesc * 4;
          if (inp->length)
            memcpy(out_ptr,((char *)inp)+offset(inp->pointer),inp->length);
          out_ptr += inp->length;
        }
        bytes_out = 12 + (int)(inp->ndesc) * 4 + inp->length;
        bytes_in = sizeof(struct descriptor_r) + (int)(inp->ndesc - 1) * sizeof(struct descriptor *) + inp->length;
        for (j = 0; j < inp->ndesc; j++)
	{
	  if (inp->dscptrs[j])
	  {
	    status = copy_dx_rec((struct descriptor *)(((char *)inp) + offset(inp->dscptrs[j])), out_ptr, &size_out, &size_in);
            if (out_ptr)
	    {
              dscptrs[j] = out_ptr - begin;
              out_ptr += size_out;
            }
            bytes_out += size_out;
            bytes_in += size_in;
          }
	}
      }
      break;

     case CLASS_A:
      {
        array_coeff *inp = (array_coeff *)in_ptr;
        if (out_ptr)
	{
          *length_of(out_ptr) = inp->length;
          *dtype_of(out_ptr) = inp->dtype;
          *class_of(out_ptr) = inp->class;
          *pointer_of(out_ptr) = 16 + (inp->aflags.coeff ? sizeof(int) + sizeof(int) * inp->dimct : 0)
		                    + (inp->aflags.bounds ? sizeof(int) * (inp->dimct * 2) : 0);
          *scale_of(out_ptr) = inp->scale;
          *digits_of(out_ptr) = inp->digits;
          set_aflags(out_ptr,in);
          *dimct_of(out_ptr) = inp->dimct;
          *arsize_of(out_ptr) = inp->arsize;
          out_ptr += 16;
          if (inp->aflags.coeff)
	  {
            *(unsigned int *)out_ptr = *pointer_of(out_ptr) + (offset(inp->a0) - offset(inp->pointer));
            out_ptr += 4;
            for (j=0;j<inp->dimct;j++)
	    {
              *(unsigned int *)out_ptr = inp->m[j];
              out_ptr += 4;
            }
            if (inp->aflags.bounds)
	    {
              for (j=0;j<inp->dimct;j++)
	      {
                *(unsigned int *)out_ptr = inp->m[inp->dimct + 2 * j];
                out_ptr += 4;
                *(unsigned int *)out_ptr = inp->m[inp->dimct + 2 * j + 1];
                out_ptr += 4;
              }
            }
          }
          memcpy(out_ptr,((char *)inp) + offset(inp->pointer),inp->arsize);
          out_ptr += inp->arsize;
        }
	bytes_out = 16
		+ (inp->aflags.coeff ? sizeof(int) + sizeof(int) * inp->dimct : 0)
		+ (inp->aflags.bounds ? sizeof(int) * (inp->dimct * 2) : 0) + inp->arsize;
	bytes_in = sizeof(struct descriptor_a)
		+ (inp->aflags.coeff ? sizeof(char *) + sizeof(int) * inp->dimct : 0)
		+ (inp->aflags.bounds ? sizeof(int) * (inp->dimct * 2) : 0) + inp->arsize;
      }
      break;

    /**************************************
    For CA and APD, a0 is the offset.
    **************************************/
     case CLASS_APD:
      {
        array_coeff *inp = (array_coeff *)in_ptr;
        struct descriptor **dsc = (struct descriptor **)  (((char *)in_ptr) + offset(inp->pointer));
        char *begin = out_ptr;
        int *dscptr;
	num_dsc = inp->arsize / inp->length;
        if (out_ptr)
	{
          *length_of(out_ptr) = 4;
          *dtype_of(out_ptr) = inp->dtype;
          *class_of(out_ptr) = inp->class;
          *pointer_of(out_ptr) = 16 + (inp->aflags.coeff ? sizeof(int) + sizeof(int) * inp->dimct : 0)
		                    + (inp->aflags.bounds ? sizeof(int) * (inp->dimct * 2) : 0);
          *scale_of(out_ptr) = inp->scale;
          *digits_of(out_ptr) = inp->digits;
          set_aflags(out_ptr,in);
          *dimct_of(out_ptr) = inp->dimct;
          *arsize_of(out_ptr) = num_dsc * 4;
          out_ptr += 16;
          if (inp->aflags.coeff)
	  {
            *(unsigned int *)out_ptr = *pointer_of(out_ptr) + (offset(inp->a0) - offset(inp->pointer));
            out_ptr += 4;
            for (j=0;j<inp->dimct;j++)
	    {
              *(unsigned int *)out_ptr = inp->m[j];
              out_ptr += 4;
            }
            if (inp->aflags.bounds)
	    {
              for (j=0;j<inp->dimct;j++)
	      {
                *(unsigned int *)out_ptr = inp->m[inp->dimct + 2 * j];
                out_ptr += 4;
                *(unsigned int *)out_ptr = inp->m[inp->dimct + 2 * j + 1];
                out_ptr += 4;
              }
            }
          }
          dscptr = (int *)out_ptr;
          out_ptr += num_dsc * 4;
          memset(dscptr, 0, num_dsc * 4);
        }
	bytes_in = 16
		+ (inp->aflags.coeff ? sizeof(int) + sizeof(int) * inp->dimct : 0)
		+ (inp->aflags.bounds ? sizeof(int) * (inp->dimct * 2) : 0) + inp->arsize;
	bytes_out = sizeof(struct descriptor_a)
		+ (inp->aflags.coeff ? sizeof(char *) + sizeof(int) * inp->dimct : 0)
		+ (inp->aflags.bounds ? sizeof(int) * (inp->dimct * 2) : 0 + num_dsc * 4);
      /******************************
      Each descriptor must be copied.
      ******************************/
	for (j = 0; j < num_dsc; j++, bytes_in += 4)
	{
          if (offset(dsc[j]))
	  {
            status = copy_dx_rec((struct descriptor *)(((char *)in_ptr) + offset(dsc[j])), out_ptr, &size_out, &size_in);
  	    if (out_ptr)
	    {
              dscptr[i] = out_ptr - begin;
	      out_ptr += size_out;
            }
	    bytes_out += size_out;
            bytes_in += size_in;
          }
	}
      }
      break;

     case CLASS_CA:
      {
        array_coeff *inp = (array_coeff *)in_ptr;
        if (out_ptr)
	{
          *length_of(out_ptr) = inp->length;
          *dtype_of(out_ptr) = inp->dtype;
          *class_of(out_ptr) = inp->class;
          *pointer_of(out_ptr) = inp->pointer ? 16 + (inp->aflags.coeff ? sizeof(int) + sizeof(int) * inp->dimct : 0)
		                    + (inp->aflags.bounds ? sizeof(int) * (inp->dimct * 2) : 0) : 0;
          *scale_of(out_ptr) = inp->scale;
          *digits_of(out_ptr) = inp->digits;
          set_aflags(out_ptr,in);
          *dimct_of(out_ptr) = inp->dimct;
          *arsize_of(out_ptr) = inp->arsize;
          out_ptr += 16;
          if (inp->aflags.coeff)
	  {
            *(unsigned int *)out_ptr = *pointer_of(out_ptr) + (offset(inp->a0) - offset(inp->pointer));
            out_ptr += 4;
            for (j=0;j<inp->dimct;j++)
	    {
              *(unsigned int *)out_ptr = inp->m[j];
              out_ptr += 4;
            }
            if (inp->aflags.bounds)
	    {
              for (j=0;j<inp->dimct;j++)
	      {
                *(unsigned int *)out_ptr = inp->m[inp->dimct + 2 * j];
                out_ptr += 4;
                *(unsigned int *)out_ptr = inp->m[inp->dimct + 2 * j + 1];
                out_ptr += 4;
              }
            }
          }
        }
	bytes_out = 16
		+ (inp->aflags.coeff ? sizeof(int) + sizeof(int) * inp->dimct : 0)
		+ (inp->aflags.bounds ? sizeof(int) * (inp->dimct * 2) : 0);
	bytes_in = sizeof(struct descriptor_a)
		+ (inp->aflags.coeff ? sizeof(char *) + sizeof(int) * inp->dimct : 0)
		+ (inp->aflags.bounds ? sizeof(int) * (inp->dimct * 2) : 0);
      /***************************
      Null pointer for shape only.
      ***************************/
	if (inp->pointer)
	{
          status = copy_dx_rec((struct descriptor *)(((char *)in_ptr) + offset(inp->pointer)), out_ptr, &size_out, &size_in);
	  bytes_out += size_out;
          bytes_in += size_in;
	}
      }
      break;

     default:
      status = LibINVSTRDES;
      break;
    }
  *b_out = bytes_out;
  *b_in  = bytes_in;
  return status;
}

static int Dsc2Rec(struct descriptor *inp, struct descriptor_xd *out_dsc_ptr)
{
  unsigned int size_out;
  unsigned int size_in;
  int       status;
  static const unsigned char dsc_dtype = DTYPE_DSC;
  status = copy_dx_rec((struct descriptor *)inp, 0, &size_out, &size_in);
  if (status & 1 && size_out)
  {
    status = MdsGet1Dx(&size_out, (unsigned char *) &dsc_dtype, out_dsc_ptr, NULL);
    if (status & 1)
      status = copy_dx_rec((struct descriptor *)inp, (char *)out_dsc_ptr->pointer, &size_out, &size_in);
  }
  else
    MdsFree1Dx(out_dsc_ptr, NULL);
  return status;
}
