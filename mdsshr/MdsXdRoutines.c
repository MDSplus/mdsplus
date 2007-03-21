/*  CMS REPLACEMENT HISTORY, Element MDSXDROUTINES.C */
/*  *12    4-SEP-1996 08:04:48 TWF "Remove libvmdef.h" */
/*  *11    5-AUG-1996 14:38:17 TWF "Fix copy_dx for unpacked descriptors" */
/*  *10    2-AUG-1996 12:28:09 TWF "Problem with long on VMS" */
/*  *9     2-AUG-1996 08:16:24 TWF "Use int instead of long" */
/*  *8    27-JUN-1996 15:43:11 TWF "Port to Unix/Windows" */
/*  *7    12-JUN-1996 10:56:40 TWF "Port to Unix/Windows" */
/*  *6    12-JUN-1996 10:56:04 TWF "Port to Unix/Windows" */
/*  *5    12-JUN-1996 10:53:52 TWF "Port to Unix/Windows" */
/*  *4    24-OCT-1995 11:32:35 TWF "Fix copy of record without any descriptors" */
/*  *3    19-OCT-1995 15:06:51 TWF "Add mdsxdroutines.c" */
/*  *2    19-OCT-1995 14:57:44 TWF "Add mdsxdroutines.c" */
/*  *1    19-OCT-1995 13:38:16 TWF "XD handling" */
/*  CMS REPLACEMENT HISTORY, Element MDSXDROUTINES.C */
#include <string.h>
#include <stdlib.h>
#include <mdsdescrip.h>
#include <libroutines.h>
#include <strroutines.h>
#include <librtl_messages.h>
#include <mdsshr.h>
#include <STATICdef.h>

STATIC_CONSTANT char *cvsrev = "@(#)$RCSfile$ $Revision$ $Date$";
#define LibVM_FIRST_FIT      1
#define LibVM_BOUNDARY_TAGS  1
#define LibVM_EXTEND_AREA    32
#define LibVM_TAIL_LARGE     128
#define align(bytes,size) ((((unsigned long)(bytes) + (size) - 1)/(size)) * (size))
#define compression_threshold 128
#define _MOVC3(a,b,c) memcpy(c,b,a)

void MdsFixDscLength(struct descriptor *in);

STATIC_CONSTANT void *MdsVM_ZONE = 0;

int  MdsGet1Dx(unsigned int *length_ptr, unsigned char *dtype_ptr, struct descriptor_xd *dsc_ptr, void **zone)
{
  int       status;
  if (dsc_ptr->class == CLASS_XD)
  {
    if (*length_ptr != dsc_ptr->l_length)
    {
      if (dsc_ptr->l_length)
	status = LibFreeVm(&dsc_ptr->l_length, &dsc_ptr->pointer, zone ? zone : (MdsVM_ZONE ? &MdsVM_ZONE : 0));
      else
	status = 1;
      if (status & 1)
	status = LibGetVm(length_ptr, &dsc_ptr->pointer, zone ? zone : (MdsVM_ZONE ? &MdsVM_ZONE : 0));
    }
    else
      status = 1;
    if (status & 1)
    {
      dsc_ptr->length = 0;
      dsc_ptr->l_length = *length_ptr;
      dsc_ptr->class = CLASS_XD;
      dsc_ptr->dtype = *dtype_ptr;
    }
  }
  else
    status = LibINVSTRDES;
  return status;
}

int  MdsFree1Dx(struct descriptor_xd *dsc_ptr, void **zone)
{
  int       status;
  if (dsc_ptr->class == CLASS_XD)
  {
    if (dsc_ptr->pointer)
      status = LibFreeVm(&dsc_ptr->l_length, &dsc_ptr->pointer, zone ? zone : (MdsVM_ZONE ? &MdsVM_ZONE : 0));
    else
      status = 1;
    if (status & 1)
    {
      dsc_ptr->pointer = 0;
      dsc_ptr->l_length = 0;
    }
  }
  else if (dsc_ptr->class == CLASS_D)
    status = LibSFree1Dd(dsc_ptr);
  else
    status = LibINVSTRDES;
  return status;
}

typedef struct _bounds { int l; int u; } BOUNDS;

STATIC_ROUTINE struct descriptor *FixedArray();

/*-----------------------------------------------------------------
	Recursively compact all descriptors and adjust pointers.
	NIDs converted to PATHs for TREE$COPY_TO_RECORD.
	Eliminates DSC descriptors. Need DSC for classes A and APD?
-----------------------------------------------------------------*/
STATIC_ROUTINE int copy_dx(
		               struct descriptor_xd *in_dsc_ptr,
		               struct descriptor_xd *out_dsc_ptr,
		               unsigned int *bytes_used_ptr,
		               int (*fixup_nid) (),
		               void  *fixup_nid_arg,
		               int (*fixup_path) (),
		               void  *fixup_path_arg, int *compressible)
{
  unsigned int status = 1,
              bytes = 0,
              j,
              size;
  struct descriptor *in_ptr = (struct descriptor *) in_dsc_ptr;
  int align_size;
  while (in_ptr && in_ptr->dtype == DTYPE_DSC)
    in_ptr = (struct descriptor *) in_ptr->pointer;
  if (in_ptr)
    switch (in_ptr->class)
    {
     case CLASS_S:
     case CLASS_D:
      {
	struct descriptor in;
	struct descriptor *po = (struct descriptor *) out_dsc_ptr;
        struct descriptor_d path = {0, DTYPE_T, CLASS_D, 0};
	in = *(struct descriptor *) in_ptr;
	in.class = CLASS_S;
	if (in.dtype == DTYPE_NID && fixup_nid
	    && (*fixup_nid) (in.pointer, fixup_nid_arg, &path))
	{
	  in.length = path.length;
	  in.dtype = DTYPE_PATH;
	  in.pointer = path.pointer;
	}
	else if (in.dtype == DTYPE_PATH && fixup_path
		 && (*fixup_path) (&in, fixup_path_arg, &path))
	{
	  in.length = path.length;
	  in.pointer = path.pointer;
	}
        align_size = (in.dtype == DTYPE_T && in.length) ? 1 : in.length;
	if (po)
	{
	  *po = in;
	  po->class = CLASS_S;
	  po->pointer = in.length ? (char *)po + align((char *)po - (char *)0 + sizeof(struct descriptor),align_size)
                   - ((char *)po - (char *)0) : 0;
	  if (in.length)
            _MOVC3(in.length, (char *) in.pointer, (char *) po->pointer);
	}
	if (path.pointer)
	  StrFree1Dx(&path);
	bytes = sizeof(struct descriptor_s) + in.length + align_size;
      }
      break;

     case CLASS_XS:
     case CLASS_XD:
      {
	struct descriptor_xs in;
	struct descriptor_xs *po = (struct descriptor_xs *) out_dsc_ptr;
        struct descriptor_d path = {0, DTYPE_T, CLASS_D, 0};
	in = *(struct descriptor_xs *) in_ptr;
	if (in.dtype == DTYPE_NID && fixup_nid
	    && (*fixup_nid) (in.pointer, fixup_nid_arg, &path))
	{
	  in.l_length = path.length;
	  in.dtype = DTYPE_PATH;
	  in.pointer = (struct descriptor *) path.pointer;
	}
	else if (in.dtype == DTYPE_PATH && fixup_path
		 && (*fixup_path) (&in, fixup_path_arg, &path))
	{
	  in.l_length = path.length;
	  in.pointer = (struct descriptor *) path.pointer;
	}
	if (po)
	{
	  *po = in;
	  po->class = CLASS_XS;
	  po->pointer = (struct descriptor *) po + sizeof(in);
	  _MOVC3(in.l_length, in.pointer, po->pointer);
	}
	if (path.pointer)
	  StrFree1Dx(&path);
	bytes = align(sizeof(struct descriptor_xs) + in.l_length,sizeof(void *));
      }
      break;

     case CLASS_R:
      {
	struct descriptor_r *pi = (struct descriptor_r *) in_ptr;
	struct descriptor_r *po = (struct descriptor_r *) out_dsc_ptr;
	bytes = sizeof(struct descriptor_r) + (int)(pi->ndesc - 1) * sizeof(struct descriptor *);
	if (po)
	{
	  _MOVC3(bytes, (char *) pi, (char *) po);
	  if (pi->length > 0)
	  {
	    po->pointer = (unsigned char *) po + bytes;
	    _MOVC3(pi->length, (char *) pi->pointer, (char *) po->pointer);
	  }
	}
	bytes = align(bytes + pi->length, sizeof(void *));
      /******************************
      Each descriptor must be copied.
      ******************************/
	for (j = 0; j < pi->ndesc && status & 1; ++j)
	  if (pi->dscptrs[j])
	  {
	    status = copy_dx((struct descriptor_xd *) pi->dscptrs[j],
			     po ? (struct descriptor_xd *) ((char *) po + bytes) : 0,
			     &size, fixup_nid, fixup_nid_arg,
			     fixup_path, fixup_path_arg, compressible);
	    if (po)
	      po->dscptrs[j] = size ? (struct descriptor *) ((char *) po + bytes) : 0;
	    bytes = align(bytes + size, sizeof(void *));
	  }
      }
      break;

     case CLASS_NCA:
      {
	array_coeff *pi = (array_coeff *) FixedArray(in_ptr);
	array_coeff *po = (array_coeff *) out_dsc_ptr;
	bytes = sizeof(struct descriptor_a)
		+ (pi->aflags.coeff ? sizeof(int) * (pi->dimct + 1) : 0)
		+ (pi->aflags.bounds ? sizeof(int) * (pi->dimct * 2) : 0);
	if (po)
	{
	  _MOVC3(bytes, (char *) pi, (char *) po);
	  po->pointer = (char *) po + bytes;
	  _MOVC3(pi->arsize, pi->pointer, po->pointer);
	  if (pi->aflags.coeff)
	    po->a0 = po->pointer + (pi->a0 - pi->pointer);
	}
	bytes = align(bytes + pi->arsize, sizeof(void *));
	if (pi->arsize > compression_threshold)
	  *compressible = 1;
        free(pi);
      }
      break;

     case CLASS_A:
      {
        int dscsize;
        int align_size;
	array_coeff *pi = (array_coeff *) in_ptr;
	array_coeff *po = (array_coeff *) out_dsc_ptr;
	dscsize = sizeof(struct descriptor_a)
		+ (pi->aflags.coeff ? sizeof(char *) + sizeof(int) * pi->dimct : 0)
		+ (pi->aflags.bounds ? sizeof(int) * (pi->dimct * 2) : 0);
        if (pi->length == 0)
          MdsFixDscLength((struct descriptor *)pi);
        align_size = (pi->dtype == DTYPE_T || pi->length == 0) ? 1 : pi->length;
	bytes = dscsize + pi->arsize + align_size;
	if (po)
	{
	  _MOVC3(dscsize, (char *) pi, (char *) po);
	  po->pointer = (char *)po + align((char *)po -(char *)0 + dscsize,align_size) - ((char *)po - (char *)0);
	  _MOVC3(pi->arsize, pi->pointer, po->pointer);
	  if (pi->aflags.coeff)
	    po->a0 = po->pointer + (pi->a0 - pi->pointer);
	}
	if (pi->arsize > compression_threshold)
	  *compressible = 1;
      }
      break;

    /**************************************
    For CA and APD, a0 is the offset.
    **************************************/
     case CLASS_APD:
      {
	struct descriptor_a *pi = (struct descriptor_a *) in_ptr;
	struct descriptor_a *po = (struct descriptor_a *) out_dsc_ptr;
	struct descriptor **pdi = (struct descriptor **) pi->pointer;
	struct descriptor **pdo = 0;
	unsigned int       num_dsc = pi->arsize / pi->length;
	bytes = sizeof(struct descriptor_a)
		+ (pi->aflags.coeff ? sizeof(char *) + sizeof(int) * pi->dimct : 0)
		+ (pi->aflags.bounds ? sizeof(int) * (pi->dimct * 2) : 0);
	if (po)
	{
	  _MOVC3(bytes, (char *) pi, (char *) po);
	  pdo = (struct descriptor **) (po->pointer = (char *) po + bytes);
	}
	bytes = align(bytes + pi->arsize, sizeof(void *));

      /******************************
      Each descriptor must be copied.
      ******************************/
	for (j = 0; j < num_dsc && status & 1; ++j)
	{
	  status = copy_dx((struct descriptor_xd *) * pdi++,
			   po ? (struct descriptor_xd *) ((char *) po + bytes) : 0,
			   &size, fixup_nid, fixup_nid_arg, fixup_path, fixup_path_arg, compressible);
	  if (po)
	    *pdo++ = size ? (struct descriptor *) ((char *) po + bytes) : 0;
	  bytes = align(bytes + size, sizeof(void *));
	}
      }
      break;

     case CLASS_CA:
      {
	struct descriptor_a *pi = (struct descriptor_a *) in_ptr;
	struct descriptor_a *po = (struct descriptor_a *) out_dsc_ptr;
	bytes = align(sizeof(struct descriptor_a)
		+ (pi->aflags.coeff ? sizeof(char *) + sizeof(int) * pi->dimct : 0)
		+ (pi->aflags.bounds ? sizeof(int) * (pi->dimct * 2) : 0),sizeof(void *));
	if (po)
	{
	  _MOVC3(bytes, (char *) pi, (char *) po);
	  if (pi->pointer)
  	    po->pointer = (char *)po + align((char *)po - (char *)0 + bytes,sizeof(void *)) - ((char *)po - (char *)0);
          else
            po->pointer = NULL;
	}

      /***************************
      Null pointer for shape only.
      ***************************/
	if (pi->pointer)
	{
	  status = copy_dx((struct descriptor_xd *) pi->pointer,
			   po ? (struct descriptor_xd *) (po->pointer) : 0,
			   &size, fixup_nid, fixup_nid_arg, fixup_path, fixup_path_arg, compressible);
	  bytes = align(bytes + size, sizeof(void *));
	}
      }
      break;

     default:
      status = LibINVSTRDES;
      break;
    }
  *bytes_used_ptr = bytes;
  return status;
}
/*----------------------------------------------------------------
	The outer routine checks size, allocates, and calls again.
	Compressible flag is set for big arrays.
*/

int MdsCopyDxXdZ(struct descriptor *in_dsc_ptr, struct descriptor_xd *out_dsc_ptr, void **zone,
                  int (*fixup_nid) (), void *fixup_nid_arg, int (*fixup_path) (), void *fixup_path_arg)
{
  unsigned int size;
  int       status;
  STATIC_CONSTANT unsigned char dsc_dtype = DTYPE_DSC;
/************************************************
* Get the total size of the thing to copy so that
* a contiguous descriptor can be allocated.
* If something to copy then do it, else clear.
************************************************/
  int compressible = 0;
  status = copy_dx((struct descriptor_xd *) in_dsc_ptr, 0, &size, fixup_nid, fixup_nid_arg, fixup_path, fixup_path_arg,
                   &compressible);
  if (status & 1 && size)
  {
    status = MdsGet1Dx(&size, (unsigned char *) &dsc_dtype, out_dsc_ptr, zone);
    if (status & 1)
      status = copy_dx((struct descriptor_xd *) in_dsc_ptr,
		       (struct descriptor_xd *) out_dsc_ptr->pointer,
		       &size, fixup_nid, fixup_nid_arg, fixup_path, fixup_path_arg,&compressible);
    if (status & 1 && compressible)
      status = MdsCOMPRESSIBLE;
  }
  else
    MdsFree1Dx(out_dsc_ptr, zone);
  return status;
}

STATIC_ROUTINE struct descriptor *FixedArray(struct descriptor *in)
{

  array_coeff *a = (array_coeff *)in;
  int dsize = sizeof(struct descriptor_a)+4+12*a->dimct;
  int i;
  BOUNDS *bounds = (BOUNDS *)&a->m[a->dimct];
  array_coeff *answer = (array_coeff *)memcpy(malloc(dsize),a,dsize);
  answer->class = CLASS_A;
  answer->aflags.column = 1;
  answer->aflags.coeff = 1;
  answer->aflags.bounds = 1;
  for (i=0;i<a->dimct-1;i++)
    answer->m[i] = a->m[i+1]/a->m[i];
  answer->m[i] = bounds[i].u - bounds[i].l + 1;
  return (struct descriptor *)answer;
}

int MdsCopyDxXd(struct descriptor *in, struct descriptor_xd *out)
{
  return MdsCopyDxXdZ(in, out, NULL, NULL, NULL, NULL, NULL);
}

void MdsFixDscLength(struct descriptor *in)
{
  switch(in->dtype)
  {
    case DTYPE_B:
    case DTYPE_BU: in->length = 1; break;
    case DTYPE_W:
    case DTYPE_WU: in->length = 2; break;
    case DTYPE_L:
    case DTYPE_LU:
    case DTYPE_F:
    case DTYPE_FS: in->length = 4; break;
    case DTYPE_D:
    case DTYPE_G:
    case DTYPE_FT:
    case DTYPE_Q:
    case DTYPE_QU: in->length = 8; break;
    case DTYPE_O:
    case DTYPE_OU: in->length = 16; break;
  }
}
