#include <mdsdescrip.h>
#include <libroutines.h>
#include <strroutines.h>
#include <librtl_messages.h>
#include <string.h>
#include <stdlib.h>
#include <mdsshr.h>

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

int Dsc2Rec(struct descriptor_xd *inp, struct descriptor_xd *out_dsc_ptr)
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
