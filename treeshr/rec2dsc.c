#include <mdsdescrip.h>
#include <libroutines.h>
#include <strroutines.h>
#include <librtl_messages.h>
#include <string.h>
#include <stdlib.h>
#include <mdsshr.h>

#ifdef __VMS
#pragma member_alignment save
#pragma nomember_alignment
#endif
typedef ARRAY_COEFF(char, 1) array_coef;
typedef ARRAY_COEFF(float, 12) ARRAY;
typedef struct _bounds { int l; int u; } BOUNDS;
#ifdef __VMS
#pragma member_alignment restore
#endif

#define length_of(ptr)    ((unsigned short *)&ptr[0])
#define dtype_of(ptr)     ((unsigned char  *)&ptr[2])
#define class_of(ptr)     ((unsigned char  *)&ptr[3])
#define pointer_of(ptr)   ((unsigned int   *)&ptr[4])
#define l_length_of(ptr)  ((unsigned int   *)&ptr[8])
#define ndesc_of(ptr)     ((unsigned char  *)&ptr[8])
#define dscptrs_of(ptr,j) ((unsigned int   *)&ptr[12+j*4])
#define scale_of(ptr)     ((unsigned char  *)&ptr[8])
#define digits_of(ptr)	  ((unsigned char  *)&ptr[9])
#define binscale_of(ptr)  ((*((unsigned char  *)&ptr[10]) & 0x08) != 0)
#define redim_of(ptr)     ((*((unsigned char  *)&ptr[10]) & 0x10) != 0)
#define column_of(ptr)    ((*((unsigned char  *)&ptr[10]) & 0x20) != 0)
#define coeff_of(ptr)     ((*((unsigned char  *)&ptr[10]) & 0x40) != 0)
#define bounds_of(ptr)    ((*((unsigned char  *)&ptr[10]) & 0x80) != 0)

#define dimct_of(ptr)     ((unsigned char  *)&ptr[11])
#define arsize_of(ptr)    ((unsigned int   *)&ptr[12])
#define a0_of(ptr)        ((unsigned int   *)&ptr[16])
#define m_of(ptr)         ((unsigned int   *)&ptr[20])

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
    switch (*class_of(in_ptr))
    {
     case CLASS_S:
     case CLASS_D:
      {
	struct descriptor in;
	struct descriptor *po = (struct descriptor *) out_dsc_ptr;
        in.length = *length_of(in_ptr);
        in.dtype = *dtype_of(in_ptr);
	in.class = CLASS_S;
	if (po)
	{
	  *po = in;
	  po->pointer = (char *) (po + 1);
          memcpy(po->pointer,in_ptr+8,in.length);
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
        in.dtype = *dtype_of(in_ptr);
        in.class = CLASS_XS;
        in.l_length = *l_length_of(in_ptr);
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
        pi_tmp.length = *length_of(in_ptr);
        pi_tmp.dtype = *dtype_of(in_ptr);
        pi_tmp.class = CLASS_R;
        pi_tmp.ndesc = *ndesc_of(in_ptr);
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
	  }
	}
	bytes_out += pi->length;
	bytes_in += pi->length;

      /******************************
      Each descriptor must be copied.
      ******************************/
	for (j = 0; j < pi->ndesc && status & 1; ++j)
          if (*dscptrs_of(in_ptr,j))
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
        array_coef a_tmp;
	array_coef *pi = &a_tmp;
	array_coef *po = (array_coef *) out_dsc_ptr;
        a_tmp.length = *length_of(in_ptr);
        a_tmp.dtype = *dtype_of(in_ptr);
        a_tmp.class = *class_of(in_ptr);
        a_tmp.scale = *scale_of(in_ptr);
        a_tmp.digits = *digits_of(in_ptr);
		a_tmp.aflags.binscale = binscale_of(in_ptr);
		a_tmp.aflags.redim = redim_of(in_ptr);
		a_tmp.aflags.column = column_of(in_ptr);
		a_tmp.aflags.coeff = coeff_of(in_ptr);
		a_tmp.aflags.bounds = bounds_of(in_ptr);
        a_tmp.dimct = *dimct_of(in_ptr);
        a_tmp.arsize = *arsize_of(in_ptr);
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
              po->m[i] = m_of(in_ptr)[i];
              if (pi->aflags.bounds)
              {
                po->m[pi->dimct + 2 * i] = m_of(in_ptr)[pi->dimct + 2 * i];
                po->m[pi->dimct + 2 * i + 1] = m_of(in_ptr)[pi->dimct + 2 * i + 1];
              }
            }
          }
	  po->pointer = (char *) po + bytes_out;
          memcpy(po->pointer,&in_ptr[bytes_in],pi->arsize);
	  if (pi->aflags.coeff)
	    po->a0 = po->pointer + (*a0_of(in_ptr) - *pointer_of(in_ptr));
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
        array_coef a_tmp;
	array_coef *pi = &a_tmp;
	array_coef *po = (array_coef *) out_dsc_ptr;
	struct descriptor **pdi = (struct descriptor **) pi->pointer;
	struct descriptor **pdo = 0;
	unsigned int       num_dsc;
        a_tmp.length = *length_of(in_ptr);
        a_tmp.dtype = *dtype_of(in_ptr);
        a_tmp.class = *class_of(in_ptr);
        a_tmp.scale = *scale_of(in_ptr);
        a_tmp.digits = *digits_of(in_ptr);
 		a_tmp.aflags.binscale = binscale_of(in_ptr);
		a_tmp.aflags.redim = redim_of(in_ptr);
		a_tmp.aflags.column = column_of(in_ptr);
		a_tmp.aflags.coeff = coeff_of(in_ptr);
		a_tmp.aflags.bounds = bounds_of(in_ptr);
        a_tmp.dimct = *dimct_of(in_ptr);
        a_tmp.arsize = *arsize_of(in_ptr);
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
			  po->m[i] = m_of(in_ptr)[i];
              if (pi->aflags.bounds)
              {
                po->m[pi->dimct + 2 * i] = m_of(in_ptr)[pi->dimct + 2 * i];
                po->m[pi->dimct + 2 * i + 1] = m_of(in_ptr)[pi->dimct + 2 * i + 1];
              }
            }
          }
	  po->pointer = (char *) po + bytes_out;
          for (i=0,pdo=(struct descriptor **)po->pointer;i<(int)num_dsc;i++)
            pdo[i] = (struct descriptor *)*(int *)&in_ptr[bytes_in];
	  if (pi->aflags.coeff)
	    po->a0 = po->pointer + (*a0_of(in_ptr) - *pointer_of(in_ptr));
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
        array_coef a_tmp;
	array_coef *pi = &a_tmp;
	array_coef *po = (array_coef *) out_dsc_ptr;
	struct descriptor **pdi = (struct descriptor **) pi->pointer;
	struct descriptor **pdo = 0;
        a_tmp.length = *length_of(in_ptr);
        a_tmp.dtype = *dtype_of(in_ptr);
        a_tmp.class = *class_of(in_ptr);
        a_tmp.scale = *scale_of(in_ptr);
        a_tmp.digits = *digits_of(in_ptr);
 		a_tmp.aflags.binscale = binscale_of(in_ptr);
		a_tmp.aflags.redim = redim_of(in_ptr);
		a_tmp.aflags.column = column_of(in_ptr);
		a_tmp.aflags.coeff = coeff_of(in_ptr);
		a_tmp.aflags.bounds = bounds_of(in_ptr);
        a_tmp.dimct = *dimct_of(in_ptr);
        a_tmp.arsize = *arsize_of(in_ptr);
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
              po->m[i] = m_of(in_ptr)[i];
              if (pi->aflags.bounds)
              {
                po->m[pi->dimct + 2 * i] = m_of(in_ptr)[pi->dimct + 2 * i];
                po->m[pi->dimct + 2 * i + 1] = m_of(in_ptr)[pi->dimct + 2 * i + 1];
              }
            }
          }
          po->pointer = (*pointer_of(in_ptr)) ? (char *) po + bytes_out : NULL;
	}

      /***************************
      Null pointer for shape only.
      ***************************/
	if (*pointer_of(in_ptr))
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

int Rec2Dsc(char *in, struct descriptor_xd *out_dsc_ptr)
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
