#include <mdsshr.h>
#include "treeshrp.h"
#include <treeshr.h>
#include <mdsdescrip.h>
#include <librtl_messages.h>
#include <string.h>
#include <stdlib.h>

extern int Rec2Dsc();
extern int GetNciW();

static int OpenDatafileR(TREE_INFO *info);
static int MakeNidsLocal(struct descriptor *dsc_ptr, unsigned char tree);
static DATA_FILE *GetVmDatafile();
static int GetDatafile(TREE_INFO *info_ptr, unsigned short *rfa, int *buffer_size, char *record, int *retsize,int *nodenum);

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
                int ddlen = nci.length - 8 + sizeof(struct descriptor);
                status = MdsGet1Dx(&ddlen, &dsc_dtype, dsc,0);
		dptr = dsc->pointer;
		dptr->length = nci.length - 8;
		dptr->dtype = nci.dtype;
		dptr->class = CLASS_S;
		dptr->pointer = (char *) dptr + sizeof(struct descriptor);
                memcpy(dptr->pointer,nci.DATA_INFO.DATA_IN_RECORD.data,dptr->length);
	      }
	      else
	      {
                int length = nci.DATA_INFO.DATA_LOCATION.record_length;
                char *data = malloc(length);
		status = GetDatafile(info, nci.DATA_INFO.DATA_LOCATION.rfa,&length,data,&retsize,&nodenum);
		if ((status & 1) && ((retsize != length) || (nodenum != nidx)))
		  status = TreeBADRECORD;
                else
	          status = Rec2Dsc(data,dsc);
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
  df_ptr = GetVmDatafile(info);
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
	  nid_ptr->tree = tree;
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

static DATA_FILE *GetVmDatafile()
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
