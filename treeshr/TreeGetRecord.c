#include <mdsdescrip.h>
#include <mdsshr.h>
#include <ncidef.h>
#include "treeshrp.h"
#include <treeshr.h>
#include <librtl_messages.h>
#include <string.h>
#include <stdlib.h>


static int OpenDatafileR(TREE_INFO *info);
static int MakeNidsLocal(struct descriptor *dsc_ptr, unsigned char tree);
static int GetDatafile(TREE_INFO *info_ptr, unsigned short *rfa, int *buffer_size, char *record, int *retsize,int *nodenum);
static int Rec2Dsc(char *in, struct descriptor_xd *out_dsc_ptr);

extern void *DBID;

#define min(a,b) (((a) < (b)) ? (a) : (b))

int TreeGetRecord(int nid_in, struct descriptor_xd *dsc) {return _TreeGetRecord(DBID,nid_in,dsc);}

int _TreeGetRecord(void *dbid, int nid_in, struct descriptor_xd *dsc)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *)dbid;
  NID *nid = (NID *)&nid_in;
  struct descriptor *dptr;
  int       status;
  NCI nci;
  TREE_INFO *info;
  int       nidx;
  int       retsize;
  int       nodenum;
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  if (dblist->remote)
	  return GetRecordRemote(dblist, nid_in, dsc);
  nid_to_tree_nidx(dblist, nid, info, nidx);
  if (info)
  {
    if (info->reopen)
      TreeCloseFiles(info);
    if (!info->data_file)
      status = OpenDatafileR(info);
    else
      status = 1;
    if (status & 1)
    {
      status = GetNciW(info, nidx, &nci);
      if (status & 1)
      {
	if (nci.length)
	{
	  switch (nci.class)
	  {
	   case CLASS_APD:
	   case CLASS_CA:
	   case CLASS_R:
	   case CLASS_A: if (nci.data_in_att_block)
                         {
	                   status = TreeINVDFFCLASS;
                           break;
                         }
	   case CLASS_S:
	   case CLASS_XS:
	    if (status & 1)
	      if (nci.data_in_att_block)
	      {
                unsigned char dsc_dtype = DTYPE_DSC;
                int dlen = nci.length - 8;
                unsigned int ddlen = nci.length - 8 + sizeof(struct descriptor);
                status = MdsGet1Dx(&ddlen, &dsc_dtype, dsc,0);
		dptr = dsc->pointer;
		dptr->length = nci.length - 8;
		dptr->dtype = nci.dtype;
		dptr->class = CLASS_S;
		dptr->pointer = (char *) dptr + sizeof(struct descriptor);
                memcpy(dptr->pointer,nci.DATA_INFO.DATA_IN_RECORD.data,dptr->length);
                if (dptr->dtype != DTYPE_T)
		{
		  switch (dptr->length)
		  {
		  case 2: *(short *)dptr->pointer = swapshort(*(short *)dptr->pointer); break;
		  case 4: *(int *)dptr->pointer = swapint(*(int *)dptr->pointer); break;
		  case 8: *(int *)dptr->pointer = swapint(*(int *)dptr->pointer); break;
		          ((int *)dptr->pointer)[1] = swapint(((int *)dptr->pointer)[1]); break;
		  }
  	        }
	      }
	      else
	      {
                int length = nci.DATA_INFO.DATA_LOCATION.record_length;
                char *data = malloc(length);
		status = GetDatafile(info, nci.DATA_INFO.DATA_LOCATION.rfa,&length,data,&retsize,&nodenum);
		if ((status & 1) && ((retsize != length) || (nodenum != nidx)))
		  status = TreeBADRECORD;
                else
	          status = (Rec2Dsc(data,dsc) & 1) ? TreeNORMAL : TreeBADRECORD;
                free(data);
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
  if (status & 1)
    status = MakeNidsLocal((struct descriptor *)dsc, (unsigned char)nid->tree);
  return status;
}

static int OpenDatafileR(TREE_INFO *info)
{
  int       status;
  static const    DESCRIPTOR(dataf_buffering, "DATAF");
  static const char *datafile_name = ".DATAFILE";
  DATA_FILE *df_ptr = info->data_file;
  if (df_ptr)
    return 1;
  df_ptr = TreeGetVmDatafile(info);
  if (df_ptr != NULL)
  {

#ifdef __VMS
  /********************************************
    Open the file for Read only access with
    a namblock.  If there is a problem free the
    memory and return.
  *********************************************/
    RECORD_HEADER header;

    *df_ptr->fab = cc$rms_fab;
    df_ptr->fab->fab$l_nam = df_ptr->nam;
    df_ptr->fab->fab$l_dna = info->filespec;
    df_ptr->fab->fab$b_dns = strlen(info->filespec);
    df_ptr->fab->fab$l_fna = datafile_name;
    df_ptr->fab->fab$b_fns = strlen(datafile_name);
    df_ptr->fab->fab$b_shr = FAB$M_SHRGET | FAB$M_SHRPUT | FAB$M_MSE | FAB$M_SHRUPD;
    df_ptr->fab->fab$b_fac = FAB$M_GET;
    *df_ptr->nam = cc$rms_nam;
    status = sys$open(df_ptr->fab, 0, 0);
    if (status & 1)
    {
    /**********************************************
     Set up the RAB for buffered reads and writes
     and CONNECT it.
    **********************************************/

      *df_ptr->getrab = cc$rms_rab;
      df_ptr->getrab->rab$l_fab = df_ptr->fab;
      df_ptr->getrab->rab$l_rhb = (char *) &header;
      TREE$GET_BUFFERING((struct descriptor *)&dataf_buffering, df_ptr->getrab);
      status = sys$connect(df_ptr->getrab, 0, 0);
      if (status & 1)
	df_ptr->open_for_write = 0;
      else
      {
	sys$close(df_ptr->fab, 0, 0);
	free(df_ptr);
        df_ptr = NULL;
      }
    }
    else
    {
      free(df_ptr);
      df_ptr = NULL;
    }
#else
    size_t len = strlen(info->filespec)-4;
    char *filename = strncpy(malloc(len+9),info->filespec,len);
    filename[len]='\0';
    strcat(filename,"datafile");
    df_ptr->get = fopen(filename,"rb");
    status = (df_ptr->get == NULL) ? TreeFAILURE : TreeNORMAL;
#endif
  }
  info->data_file = df_ptr;
  return status;
}

#ifdef __VMS
#pragma member_alignment save
#pragma nomember_alignment
#endif

typedef RECORD(1) record_one;
typedef ARRAY(struct descriptor *) array_dsc;

#ifdef __VMS
#pragma member_alignment restore
#endif

static int MakeNidsLocal(struct descriptor *dsc_ptr, unsigned char tree)
{
  int       status = 1;
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
	status = MakeNidsLocal((struct descriptor *) dsc_ptr->pointer, tree);
      else if (dsc_ptr->dtype == DTYPE_NID)
      {
	NID      *nid_ptr = (NID *) dsc_ptr->pointer;
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
	record_one *rptr = (record_one *) dsc_ptr;
	int       i;
	for (i = 0, status = 1; status && (i < rptr->ndesc); i++)
	  status = MakeNidsLocal(rptr->dscptrs[i], tree);
      }
      break;

     case CLASS_CA:
      status = MakeNidsLocal((struct descriptor *) dsc_ptr->pointer, tree);
      break;

     case CLASS_APD:
      {
	array_dsc * aptr = (array_dsc *) dsc_ptr;
	int       n_elts = aptr->arsize / aptr->length;
	int       i;
	for (status = 1, i = 0; ((status & 1) && (i < n_elts)); i++)
	  status = MakeNidsLocal((struct descriptor *) * (aptr->pointer + i), tree);
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
  static const struct descriptor_xd empty_xd = {0, 0, CLASS_XD, 0, 0};
#ifdef __VMS
  static const int vm_size = sizeof(DATA_FILE) +
		       sizeof(struct NAM) +
		       sizeof(struct RAB) +
		       sizeof(struct RAB) +
		       sizeof(struct RAB) +
		       sizeof(struct FAB) +
		       sizeof(RECORD_HEADER) +
		       sizeof(ASY_NCI) +
		       sizeof(struct descriptor_xd) +
		       sizeof(NCI);
  int       status;
  char     *ptr;
/********************************************
 For efficiency and cleanliness reasons the
 datafile structure and all the structures
 it points to will be allocated in one
 block of virtual memory. The use of ptr
 in the following code avoids type casting
 of each structure for the pointer arithmetic.
**********************************************/

  if ((status = lib$get_vm(&vm_size, &datafile_ptr)) & 1)
  {
    ptr = (char *) datafile_ptr;
    datafile_ptr->nam = (struct NAM *) (ptr += sizeof(DATA_FILE));
    datafile_ptr->getrab = (struct RAB *) (ptr += sizeof(struct NAM));
    datafile_ptr->putrab = (struct RAB *) (ptr += sizeof(struct RAB));
    datafile_ptr->updaterab = (struct RAB *) (ptr += sizeof(struct RAB));
    datafile_ptr->fab = (struct FAB *) (ptr += sizeof(struct RAB));
    datafile_ptr->record_header = (RECORD_HEADER *) (ptr += sizeof(struct FAB));
    datafile_ptr->asy_nci = (ASY_NCI *) (ptr += sizeof(RECORD_HEADER));
    datafile_ptr->data = (struct descriptor_xd *) (ptr += sizeof(ASY_NCI));
    *datafile_ptr->data = empty_xd;
    datafile_ptr->asy_nci->nci = (NCI *) (ptr += sizeof(struct descriptor_xd));
  }
  else
    datafile_ptr = NULL;
#else
  int length = sizeof(DATA_FILE)+sizeof(RECORD_HEADER)+sizeof(ASY_NCI)+sizeof(NCI)+sizeof(struct descriptor_xd);
  datafile_ptr = malloc(length);
  if (datafile_ptr != NULL)
  {
    char *ptr = (char *)datafile_ptr;
    memset(datafile_ptr,0,length);
    datafile_ptr->record_header = (RECORD_HEADER *) (ptr += sizeof(DATA_FILE));
    datafile_ptr->asy_nci = (ASY_NCI *) (ptr += sizeof(RECORD_HEADER));
    datafile_ptr->data = (struct descriptor_xd *) (ptr += sizeof(ASY_NCI));
    *datafile_ptr->data = empty_xd;
    datafile_ptr->asy_nci->nci = (NCI *) (ptr += sizeof(struct descriptor_xd));
  }
#endif
  return datafile_ptr;
}

static int GetDatafile(TREE_INFO *info, unsigned short *rfa, int *buffer_size, char *record, int *retsize,int *nodenum)
{
  int status = 1;
  int buffer_space = *buffer_size;
  int       first = 1;
#ifdef __VMS
  struct RAB *rab_ptr = info_ptr->tree_info$a_data_file_ptr->$a_getrab;
  RFA      *rab_rfa = (RFA *) rab_ptr->rab$w_rfa;
  RECORD_HEADER *header = (RECORD_HEADER *) rab_ptr->rab$l_rhb;
/**************************************
 Records are retrieved by their record
 file address (RFA). The maximum record
 size is 32765 bytes minus the size of
 the VFC header portion. Longer records
 are segmented into multiple records
 and the RFA field of the header points
 to the next segment.

 This routine will return RMS$_RFA if
 the RFA supplied is all zeros or if
 the node number changes during the
 reading of a segmented record.

 It will return RMS$_RTB if there are
 more segments and the user buffer
 if full.

 Otherwise it will return whatever status
 is returned by the SYS$GET call.
***************************************/

  *retsize = 0;
  rab_ptr->rab$b_rac = RAB$C_RFA;
  *rab_rfa = *rfa;
  status = RMS$_RFA;
  while ((rab_ptr->rab$w_rfa[0] || rab_ptr->rab$w_rfa[1] || rab_ptr->rab$w_rfa[2]) && buffer_space)
  {
    rab_ptr->rab$w_usz = min(DATAF$C_MAX_RECORD_SIZE, buffer_space);
    rab_ptr->rab$l_ubf = record + *retsize;
    rab_ptr->rab$w_rsz = 0;
    rab_ptr->rab$l_rop |= RAB$M_NLK;
    status = sys$get(rab_ptr);
    *retsize += rab_ptr->rab$w_rsz;
    if (status & 1)
    {
      buffer_space -= rab_ptr->rab$w_rsz;
      *rab_rfa = header->$b_rfa;
      if (first)
      {
	*nodenum = header->$l_node_number;
	first = 0;
      }
      else if (*nodenum != header->$l_node_number)
      {
	*retsize -= rab_ptr->rab$w_rsz;
	return RMS$_RFA;
      }
    }
    else
      return status;
  }
  if (rab_ptr->rab$w_rfa[0] || rab_ptr->rab$w_rfa[1] || rab_ptr->rab$w_rfa[2])
    return RMS$_RTB;
  else
    return status;
#else
  char *bptr = (char *)record;
  *retsize = 0;
  while ((rfa[0] || rfa[1] || rfa[2]) && buffer_space && (status & 1))
  {
    RECORD_HEADER hdr;
    int rfa_l = RfaToSeek(rfa);
	fseek(info->data_file->get,rfa_l,SEEK_SET);
    if ((fread((void *)&hdr,12,1,info->data_file->get) == 1))
    {
     unsigned int partlen = min(32755, buffer_space);
     hdr.node_number = swapint(hdr.node_number);
     *(int *)&hdr.rfa = swapint(*(int *)&hdr.rfa);
     ((short *)&hdr.rfa)[2] = swapshort(((short *)&hdr.rfa)[2]);
     if (first)
        *nodenum = hdr.node_number;
      else if (*nodenum != hdr.node_number)
      {
        status = 0;
        break;
      }
      if ((fread((void *)bptr,partlen,1,info->data_file->get) == 1))
      {
        bptr += partlen;
        buffer_space -= partlen;
		*retsize = *retsize + partlen;
        rfa = (unsigned short *)&hdr.rfa;
      }
      else
        status = 0;
    }
    else
      status = 0;
  }
  return status;
#endif
}

#define length_of(ptr)    ((unsigned short)swapshort(*(short *)&ptr[0]))
#define dtype_of(ptr)     (*(unsigned char  *)&ptr[2])
#define class_of(ptr)     (*(unsigned char  *)&ptr[3])
#define pointer_of(ptr)   ((unsigned int)swapint(*(int *)&ptr[4]))
#define l_length_of(ptr)  ((unsigned int)swapint(*(int *)&ptr[8]))
#define ndesc_of(ptr)     (*(unsigned char  *)&ptr[8])
#define dscptrs_of(ptr,j) ((unsigned int)swapint(*(int *)&ptr[12+j*4]))
#define scale_of(ptr)     (*(unsigned char  *)&ptr[8])
#define digits_of(ptr)	  (*(unsigned char  *)&ptr[9])
#define binscale_of(ptr)  ((*((unsigned char  *)&ptr[10]) & 0x08) != 0)
#define redim_of(ptr)     ((*((unsigned char  *)&ptr[10]) & 0x10) != 0)
#define column_of(ptr)    ((*((unsigned char  *)&ptr[10]) & 0x20) != 0)
#define coeff_of(ptr)     ((*((unsigned char  *)&ptr[10]) & 0x40) != 0)
#define bounds_of(ptr)    ((*((unsigned char  *)&ptr[10]) & 0x80) != 0)

#define dimct_of(ptr)     (*(unsigned char  *)&ptr[11])
#define arsize_of(ptr)    ((unsigned int)swapint(*(int *)&ptr[12]))
#define a0_of(ptr)        ((unsigned int)swapint(*(int *)&ptr[16]))
#define m_of(ptr)         ((int *)&ptr[20])

/*-----------------------------------------------------------------
	Recursively compact all descriptors and adjust pointers.
	NIDs converted to PATHs for TREE$COPY_TO_RECORD.
	Eliminates DSC descriptors. Need DSC for classes A and APD?
-----------------------------------------------------------------*/

static int copy_rec_dx( char *in_ptr,struct descriptor_xd *out_dsc_ptr,unsigned int *b_out, unsigned int *b_in)
{
  unsigned int status = 1,
              bytes_out = 0,
              bytes_in = 0,
              i,j,
              size_out,
              size_in;
  if (in_ptr)
    switch (class_of(in_ptr))
    {
     case CLASS_S:
     case CLASS_D:
      {
	struct descriptor in;
	struct descriptor *po = (struct descriptor *) out_dsc_ptr;
        in.length = length_of(in_ptr);
        in.dtype = dtype_of(in_ptr);
	in.class = CLASS_S;
	if (po)
	{
	  *po = in;
	  po->pointer = (char *) (po + 1);
          memcpy(po->pointer,in_ptr+8,in.length);
          if (po->length > 1 && po->dtype != DTYPE_T)
	  {
	    switch (po->length)
	    {
	    case 2: *(short *)po->pointer = swapshort(*(short *)po->pointer); break;
            case 4: *(int *)po->pointer = swapint(*(int *)po->pointer); break;
            case 8: *(int *)po->pointer = swapint(*(int *)po->pointer); break;
	            ((int *)po->pointer)[1] = swapint(((int *)po->pointer)[1]); break;
	    }
	  }
	}
	bytes_out = sizeof(struct descriptor) + in.length;
        bytes_in = 8 + in.length;
      }
      break;

     case CLASS_XS:
     case CLASS_XD:
      {
	struct descriptor_xs in;
	struct descriptor_xs *po = (struct descriptor_xs *) out_dsc_ptr;
        in.length = 0;
        in.dtype = dtype_of(in_ptr);
        in.class = CLASS_XS;
        in.l_length = l_length_of(in_ptr);
	if (po)
	{
	  *po = in;
	  po->pointer = (struct descriptor *) (po + 1);
          memcpy(po->pointer,in_ptr+12,in.length);
	}
	bytes_out = sizeof(struct descriptor_xs) + in.l_length;
	bytes_in = 12 + in.l_length;
      }
      break;

     case CLASS_R:
      {
        struct descriptor_r pi_tmp;
	struct descriptor_r *pi = &pi_tmp;
	struct descriptor_r *po = (struct descriptor_r *) out_dsc_ptr;
        pi_tmp.length = length_of(in_ptr);
        pi_tmp.dtype = dtype_of(in_ptr);
        pi_tmp.class = CLASS_R;
        pi_tmp.ndesc = ndesc_of(in_ptr);
		pi_tmp.dscptrs[0] = NULL;
	bytes_out = sizeof(struct descriptor_r) + (int)(pi->ndesc - 1) * sizeof(struct descriptor *);
        bytes_in = 12 + (int)(pi->ndesc) * 4;
	if (po)
	{
	  memset(po,0,bytes_out);
          *po = *pi;
	  if (pi->length > 0)
	  {
	    po->pointer = (unsigned char *) po + bytes_out;
            memcpy(po->pointer,&in_ptr[12+pi->ndesc*4],pi->length);
            if (po->dtype == DTYPE_FUNCTION && po->length == 2)
	      {
                *(short *)po->pointer = swapshort(*(short *)po->pointer);
              }
	  }
	}
	bytes_out += pi->length;
	bytes_in += pi->length;

      /******************************
      Each descriptor must be copied.
      ******************************/
	for (j = 0; j < pi->ndesc && status & 1; ++j)
          if (dscptrs_of(in_ptr,j))
	  {
	    status = copy_rec_dx(in_ptr + bytes_in, po ? (struct descriptor_xd *) ((char *) po + bytes_out) : 0, &size_out, &size_in);
	    if (po)
	      po->dscptrs[j] = size_out ? (struct descriptor *) ((char *) po + bytes_out) : 0;
	    bytes_out += size_out;
            bytes_in += size_in;
	  }
      }
      break;
     case CLASS_A:
      {
        array_coeff a_tmp;
	array_coeff *pi = &a_tmp;
	array_coeff *po = (array_coeff *) out_dsc_ptr;
        a_tmp.length = length_of(in_ptr);
        a_tmp.dtype = dtype_of(in_ptr);
        a_tmp.class = class_of(in_ptr);
        a_tmp.scale = scale_of(in_ptr);
        a_tmp.digits = digits_of(in_ptr);
		a_tmp.aflags.binscale = binscale_of(in_ptr);
		a_tmp.aflags.redim = redim_of(in_ptr);
		a_tmp.aflags.column = column_of(in_ptr);
		a_tmp.aflags.coeff = coeff_of(in_ptr);
		a_tmp.aflags.bounds = bounds_of(in_ptr);
        a_tmp.dimct = dimct_of(in_ptr);
        a_tmp.arsize = arsize_of(in_ptr);
	bytes_in = 16
		+ (pi->aflags.coeff ? sizeof(int) + sizeof(int) * pi->dimct : 0)
		+ (pi->aflags.bounds ? sizeof(int) * (pi->dimct * 2) : 0);
	bytes_out = sizeof(struct descriptor_a)
		+ (pi->aflags.coeff ? sizeof(char *) + sizeof(int) * pi->dimct : 0)
		+ (pi->aflags.bounds ? sizeof(int) * (pi->dimct * 2) : 0);
	if (po)
	{
          memcpy(po,pi,sizeof(struct descriptor_a));
          for (i=0;i<pi->dimct;i++)
          {
            if (pi->aflags.coeff)
            {
              po->m[i] = swapint(m_of(in_ptr)[i]);
              if (pi->aflags.bounds)
              {
                po->m[pi->dimct + 2 * i] = swapint(m_of(in_ptr)[pi->dimct + 2 * i]);
                po->m[pi->dimct + 2 * i + 1] = swapint(m_of(in_ptr)[pi->dimct + 2 * i + 1]);
              }
            }
          }
	  po->pointer = (char *) po + bytes_out;
          memcpy(po->pointer,&in_ptr[bytes_in],pi->arsize);
	  if (pi->aflags.coeff)
	    po->a0 = po->pointer + (a0_of(in_ptr) - pointer_of(in_ptr));
          if (po->dtype != DTYPE_T)
	  {
            int i;
	    switch (po->length)
	    {
	    case 2: 
	      { short *ptr;
		for (i=0,ptr=(short *)po->pointer;i<po->arsize;i += sizeof(*ptr),ptr++) *ptr = swapshort(*ptr);
	      }
	      break;
	    case 4:
	    case 8:
	      { int *ptr;
		for (i=0,ptr=(int *)po->pointer;i<po->arsize;i += sizeof(*ptr),ptr++) *ptr = swapint(*ptr);
	      }
	      break;
	    }
	  }
	}
	bytes_out += pi->arsize;
        bytes_in += pi->arsize;
      }
      break;

    /**************************************
    For CA and APD, a0 is the offset.
    **************************************/
     case CLASS_APD:
      {
        array_coeff a_tmp;
	array_coeff *pi = &a_tmp;
	array_coeff *po = (array_coeff *) out_dsc_ptr;
	struct descriptor **pdi = (struct descriptor **) pi->pointer;
	struct descriptor **pdo = 0;
	unsigned int       num_dsc;
        a_tmp.length = length_of(in_ptr);
        a_tmp.dtype = dtype_of(in_ptr);
        a_tmp.class = class_of(in_ptr);
        a_tmp.scale = scale_of(in_ptr);
        a_tmp.digits = digits_of(in_ptr);
 		a_tmp.aflags.binscale = binscale_of(in_ptr);
		a_tmp.aflags.redim = redim_of(in_ptr);
		a_tmp.aflags.column = column_of(in_ptr);
		a_tmp.aflags.coeff = coeff_of(in_ptr);
		a_tmp.aflags.bounds = bounds_of(in_ptr);
        a_tmp.dimct = dimct_of(in_ptr);
        a_tmp.arsize = arsize_of(in_ptr);
	bytes_in = 16
		+ (pi->aflags.coeff ? sizeof(int) + sizeof(int) * pi->dimct : 0)
		+ (pi->aflags.bounds ? sizeof(int) * (pi->dimct * 2) : 0);
	bytes_out = sizeof(struct descriptor_a)
		+ (pi->aflags.coeff ? sizeof(char *) + sizeof(int) * pi->dimct : 0)
		+ (pi->aflags.bounds ? sizeof(int) * (pi->dimct * 2) : 0);
	num_dsc = pi->arsize / pi->length;
	if (po)
	{
          memcpy(po,pi,sizeof(struct descriptor_a));
          for (i=0;i<pi->dimct;i++)
          {
            if (pi->aflags.coeff)
            {
			  po->m[i] = swapint(m_of(in_ptr)[i]);
              if (pi->aflags.bounds)
              {
                po->m[pi->dimct + 2 * i] = swapint(m_of(in_ptr)[pi->dimct + 2 * i]);
                po->m[pi->dimct + 2 * i + 1] = swapint(m_of(in_ptr)[pi->dimct + 2 * i + 1]);
              }
            }
          }
	  po->pointer = (char *) po + bytes_out;
          for (i=0,pdo=(struct descriptor **)po->pointer;i<(int)num_dsc;i++)
            pdo[i] = (struct descriptor *)*(int *)&in_ptr[bytes_in];
	  if (pi->aflags.coeff)
	    po->a0 = po->pointer + (a0_of(in_ptr) - pointer_of(in_ptr));
	  pdo = (struct descriptor **) po->pointer;
          po->length = sizeof(struct descriptor *);
          po->arsize = num_dsc * sizeof(struct descriptor *);
	}
	bytes_out += po->arsize;
        bytes_in += pi->arsize;
      /******************************
      Each descriptor must be copied.
      ******************************/
	for (j = 0; j < num_dsc && status & 1; ++j)
	{
          status = copy_rec_dx(in_ptr + bytes_in, po ? (struct descriptor_xd *) ((char *) po + bytes_out) : 0, &size_out, &size_in);
	  if (po)
	    *pdo++ = size_out ? (struct descriptor *) ((char *) po + bytes_out) : 0;
	  bytes_out += size_out;
          bytes_in += size_in;
	}
      }
      break;

     case CLASS_CA:
      {
        array_coeff a_tmp;
	array_coeff *pi = &a_tmp;
	array_coeff *po = (array_coeff *) out_dsc_ptr;
	struct descriptor **pdi = (struct descriptor **) pi->pointer;
	struct descriptor **pdo = 0;
        a_tmp.length = length_of(in_ptr);
        a_tmp.dtype = dtype_of(in_ptr);
        a_tmp.class = class_of(in_ptr);
        a_tmp.scale = scale_of(in_ptr);
        a_tmp.digits = digits_of(in_ptr);
 		a_tmp.aflags.binscale = binscale_of(in_ptr);
		a_tmp.aflags.redim = redim_of(in_ptr);
		a_tmp.aflags.column = column_of(in_ptr);
		a_tmp.aflags.coeff = coeff_of(in_ptr);
		a_tmp.aflags.bounds = bounds_of(in_ptr);
        a_tmp.dimct = dimct_of(in_ptr);
        a_tmp.arsize = arsize_of(in_ptr);
	bytes_in = 16
		+ (pi->aflags.coeff ? sizeof(int) + sizeof(int) * pi->dimct : 0)
		+ (pi->aflags.bounds ? sizeof(int) * (pi->dimct * 2) : 0);
	bytes_out = sizeof(struct descriptor_a)
		+ (pi->aflags.coeff ? sizeof(char *) + sizeof(int) * pi->dimct : 0)
		+ (pi->aflags.bounds ? sizeof(int) * (pi->dimct * 2) : 0);
	if (po)
	{
          memcpy(po,pi,sizeof(struct descriptor_a));
          for (i=0;i<pi->dimct;i++)
          {
            if (pi->aflags.coeff)
            {
              po->m[i] = swapint(m_of(in_ptr)[i]);
              if (pi->aflags.bounds)
              {
                po->m[pi->dimct + 2 * i] = swapint(m_of(in_ptr)[pi->dimct + 2 * i]);
                po->m[pi->dimct + 2 * i + 1] = swapint(m_of(in_ptr)[pi->dimct + 2 * i + 1]);
              }
            }
          }
          po->pointer = (pointer_of(in_ptr)) ? (char *) po + bytes_out : NULL;
	}

      /***************************
      Null pointer for shape only.
      ***************************/
	if (pointer_of(in_ptr))
	{
          status = copy_rec_dx(in_ptr + bytes_in, po ? (struct descriptor_xd *) ((char *) po + bytes_out) : 0, &size_out, &size_in);
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

static int Rec2Dsc(char *in, struct descriptor_xd *out_dsc_ptr)
{
  unsigned int size_out;
  unsigned int size_in;
  int       status;
  static const unsigned char dsc_dtype = DTYPE_DSC;
  status = copy_rec_dx(in, 0, &size_out, &size_in);
  if (status & 1 && size_out)
  {
    status = MdsGet1Dx(&size_out, (unsigned char *) &dsc_dtype, out_dsc_ptr, NULL);
    if (status & 1)
      status = copy_rec_dx(in, (struct descriptor_xd *)out_dsc_ptr->pointer, &size_out, &size_in);
  }
  else
    MdsFree1Dx(out_dsc_ptr, NULL);
  return status;
}
