#include <mdsdescrip.h>
#include <librtl_messages.h>

#define align(bytes,size) ((((bytes) + (size) - 1)/(size)) * (size))
#define length_of(ptr)    ((unsigned short)swapshort(&ptr[0]))
#define dtype_of(ptr)     (*(unsigned char  *)&ptr[2])
#define class_of(ptr)     (*(unsigned char  *)&ptr[3])
#define pointer_of(ptr)   ((unsigned int)swapint(&ptr[4]))
#define l_length_of(ptr)  ((unsigned int)swapint(&ptr[8]))
#define ndesc_of(ptr)     (*(unsigned char  *)&ptr[8])
#define dscptrs_of(ptr,j) ((unsigned int)swapint(&ptr[12+j*4]))
#define scale_of(ptr)     (*(unsigned char  *)&ptr[8])
#define digits_of(ptr)	  (*(unsigned char  *)&ptr[9])
#define binscale_of(ptr)  ((*((unsigned char  *)&ptr[10]) & 0x08) != 0)
#define redim_of(ptr)     ((*((unsigned char  *)&ptr[10]) & 0x10) != 0)
#define column_of(ptr)    ((*((unsigned char  *)&ptr[10]) & 0x20) != 0)
#define coeff_of(ptr)     ((*((unsigned char  *)&ptr[10]) & 0x40) != 0)
#define bounds_of(ptr)    ((*((unsigned char  *)&ptr[10]) & 0x80) != 0)
#define dimct_of(ptr)     (*(unsigned char  *)&ptr[11])
#define arsize_of(ptr)    ((unsigned int)swapint(&ptr[12]))
#define a0_of(ptr)        ((unsigned int)swapint(&ptr[16]))
#define m_of(ptr)         (&ptr[20])

#if defined(_big_endian)

static int swapint(char *in_c)
{
  int out;
  char *out_c = (char *)&out;
  int i;
  for (i=0;i<4;i++) out_c[3-i] = in_c[i];
  return out;
}

static int swapshort(char *in_c)
{
  short out;
  char *out_c = (char *)&out;
  int i;
  for (i=0;i<2;i++) out_c[1-i] = in_c[i];
  return out;
}

#else

static int swapint(char *in_c)
{
  int out;
  char *out_c = (char *)&out;
  int i;
  for (i=0;i<4;i++) out_c[i] = in_c[i];
  return out;
}

static int swapshort(char *in_c)
{
  short out;
  char *out_c = (char *)&out;
  int i;
  for (i=0;i<2;i++) out_c[i] = in_c[i];
  return out;
}

#endif

static int copy_rec_dx( char *in_ptr, struct descriptor_xd *out_dsc_ptr, 
                        unsigned int *b_out, unsigned int *b_in)
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
	    case 2: *(short *)po->pointer = swapshort(po->pointer); break;
            case 4: *(int *)po->pointer = swapint(po->pointer); break;
            case 8: *(int *)po->pointer = swapint(po->pointer); 
	            ((int *)po->pointer)[1] = swapint(po->pointer + sizeof(int)); break;
	    }
	  }
	}
	bytes_out = align(sizeof(struct descriptor) + in.length,sizeof(void *));
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
	bytes_out = align(sizeof(struct descriptor_xs) + in.l_length,sizeof(void *));
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
	pi_tmp.dscptrs[0] = 0;
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
                *(short *)po->pointer = swapshort((char *)po->pointer);
              }
	  }
	}
	bytes_out = align(bytes_out + pi->length,sizeof(void *));
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
	    bytes_out = align(bytes_out + size_out, sizeof(void *));
            bytes_in += size_in;
	  }

     }
     break;
     case CLASS_A:
     {
        int dsc_size;
        int align_size;
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
	dsc_size = sizeof(struct descriptor_a)
		+ (pi->aflags.coeff ? sizeof(char *) + sizeof(int) * pi->dimct : 0)
		+ (pi->aflags.bounds ? sizeof(int) * (pi->dimct * 2) : 0);
        align_size = (pi->dtype == DTYPE_T) ? 1 : pi->length;
	bytes_out = dsc_size + pi->arsize + align_size;
	if (po)
	{
          memcpy(po,pi,sizeof(struct descriptor_a));
          for (i=0;i<pi->dimct;i++)
          {
            if (pi->aflags.coeff)
            {
              po->m[i] = swapint(m_of(in_ptr)+sizeof(int) * i);
              if (pi->aflags.bounds)
              {
                po->m[pi->dimct + 2 * i] = swapint(m_of(in_ptr) + (pi->dimct + 2 * i) * sizeof(int));
                po->m[pi->dimct + 2 * i + 1] = swapint(m_of(in_ptr) + (pi->dimct + 2 * i + 1) * sizeof(int));
              }
            }
          }
	  po->pointer = (char *)po + (align(((unsigned int)((char *)po + dsc_size)),align_size) - (unsigned int)po);
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
		for (i=0,ptr=(short *)po->pointer;i< (int)po->arsize;i += sizeof(*ptr),ptr++) *ptr = swapshort((char *)ptr);
	      }
	      break;
	    case 4:
	    case 8:
	      { int *ptr;
		for (i=0,ptr=(int *)po->pointer;i<(int)po->arsize;i += sizeof(*ptr),ptr++) *ptr = swapint((char *)ptr);
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
			  po->m[i] = swapint(m_of(in_ptr) + i * sizeof(int));
              if (pi->aflags.bounds)
              {
                po->m[pi->dimct + 2 * i] = swapint(m_of(in_ptr) + (pi->dimct + 2 * i) * sizeof(int));
                po->m[pi->dimct + 2 * i + 1] = swapint(m_of(in_ptr) + (pi->dimct + 2 * i + 1) * sizeof(int));
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
	bytes_out = align(sizeof(struct descriptor_a)
		+ (pi->aflags.coeff ? sizeof(char *) + sizeof(int) * pi->dimct : 0)
		+ (pi->aflags.bounds ? sizeof(int) * (pi->dimct * 2) : 0),sizeof(void *));
	if (po)
	{
          memcpy(po,pi,sizeof(struct descriptor_a));
          for (i=0;i<pi->dimct;i++)
          {
            if (pi->aflags.coeff)
            {
              po->m[i] = swapint(m_of(in_ptr) + i * sizeof(int));
              if (pi->aflags.bounds)
              {
                po->m[pi->dimct + 2 * i] = swapint(m_of(in_ptr) + (pi->dimct + 2 * i) * sizeof(int));
                po->m[pi->dimct + 2 * i + 1] = swapint(m_of(in_ptr) + (pi->dimct + 2 * i + 1) * sizeof(int));
              }
            }
          }
          po->pointer = (pointer_of(in_ptr)) ? 
              (char *)po + (align(((unsigned int)((char *)po + bytes_out)),sizeof(void *)) - (unsigned int)po) : 0;
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

int MdsSerializeDscIn(char *in, struct descriptor_xd *out)
{
  unsigned int size_out;
  unsigned int size_in;
  int       status;
  static const unsigned char dsc_dtype = DTYPE_DSC;
  status = copy_rec_dx(in, 0, &size_out, &size_in);
  if (status & 1 && size_out)
  {
    status = MdsGet1Dx(&size_out, (unsigned char *) &dsc_dtype, out, 0);
    if (status & 1)
      status = copy_rec_dx(in, (struct descriptor_xd *)out->pointer, &size_out, &size_in);
  }
  else
    MdsFree1Dx(out, 0);
  return status;
}

