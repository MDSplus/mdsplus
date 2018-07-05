/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <string.h>
#include <mdsdescrip.h>
#include <mdstypes.h>
#include <mdsshr.h>
#include <mdsshr_messages.h>
#include <STATICdef.h>

#define align(bytes,size) ((((bytes) + (size) - 1)/(size)) * (size))
#define set_length(ans)  swap(short,&in_ptr[0],ans)
#define dtype()     (*(unsigned char  *)&in_ptr[2])
#define class()     (*(unsigned char  *)&in_ptr[3])
#define set_offset(ans)   swap(int,&in_ptr[4],ans)
#define set_l_length(ans) swap(int,&in_ptr[8],ans)
#define ndesc()     (*(unsigned char  *)&in_ptr[8])
#define set_dscptrs(ans,j) swap(int,&in_ptr[12+j*4],ans)
#define scale()     (*(char  *)&in_ptr[8])
#define digits()    (*(unsigned char  *)&in_ptr[9])
#define binscale()  ((*((unsigned char  *)&in_ptr[10]) & 0x08) != 0)
#define redim()     ((*((unsigned char  *)&in_ptr[10]) & 0x10) != 0)
#define column()    ((*((unsigned char  *)&in_ptr[10]) & 0x20) != 0)
#define coeff()     ((*((unsigned char  *)&in_ptr[10]) & 0x40) != 0)
#define bounds()    ((*((unsigned char  *)&in_ptr[10]) & 0x80) != 0)
#define dimct()     (*(unsigned char  *)&in_ptr[11])
#define set_arsize(ans)    swap(int,&in_ptr[12],ans)
#define set_a0(ans)       swap(int,&in_ptr[16],ans)
#define m()         (&in_ptr[20])
#define LoadChar(in,outp)  (outp)[0] = ((char *)&in)[0]
#define FixLength(a) if (a.length == 0) MdsFixDscLength((struct descriptor *)&a)
void MdsFixDscLength(struct descriptor *in);


#ifdef WORDS_BIGENDIAN
#define LoadShort(in,outp) (outp)[0] = ((char *)&in)[1]; (outp)[1] = ((char *)&in)[0]
#define LoadInt(in,outp)   (outp)[0] = ((char *)&in)[3]; (outp)[1] = ((char *)&in)[2]; \
                           (outp)[2] = ((char *)&in)[1]; (outp)[3] = ((char *)&in)[0]
#define LoadQuad(in,outp)  (outp)[0] = ((char *)&in)[7]; (outp)[1] = ((char *)&in)[6]; \
                           (outp)[2] = ((char *)&in)[5]; (outp)[3] = ((char *)&in)[4]; \
                           (outp)[4] = ((char *)&in)[3]; (outp)[5] = ((char *)&in)[2]; \
                           (outp)[6] = ((char *)&in)[1]; (outp)[7] = ((char *)&in)[0]
#else
#define LoadShort(in,outp) (outp)[0] = ((char *)&in)[0]; (outp)[1] = ((char *)&in)[1]
#define LoadInt(in,outp)   (outp)[0] = ((char *)&in)[0]; (outp)[1] = ((char *)&in)[1]; \
                           (outp)[2] = ((char *)&in)[2]; (outp)[3] = ((char *)&in)[3]
#define LoadQuad(in,outp)  (outp)[0] = ((char *)&in)[0]; (outp)[1] = ((char *)&in)[1]; \
                           (outp)[2] = ((char *)&in)[2]; (outp)[3] = ((char *)&in)[3]; \
                           (outp)[4] = ((char *)&in)[4]; (outp)[5] = ((char *)&in)[5]; \
                           (outp)[6] = ((char *)&in)[6]; (outp)[7] = ((char *)&in)[7]
#endif
#define set_aflags(ptr,in)  ptr[10] = (char)((inp->aflags.binscale << 3)  | (inp->aflags.redim << 4) \
                                    | (inp->aflags.column << 5) | (inp->aflags.coeff << 6) | (inp->aflags.bounds << 7))
#define offset(ptr)       *(unsigned int *)&ptr

union __bswap {
  char b[8];
  unsigned int int_;
  unsigned short short_;
  uint64_t int64_t_;
};

#if defined(WORDS_BIGENDIAN)

#define swap(dtype,ptr,ans) \
{ char const * p = ptr;\
  union __bswap bswap;\
  unsigned int __i;\
  for (__i=0;__i<sizeof(dtype);__i++) bswap.b[sizeof(dtype)-__i-1] = p[__i];\
  ans = bswap.dtype##_;\
}

#else

#define swap(dtype,ptr,ans) \
{ char const * p = ptr;\
  union __bswap bswap;\
  unsigned int __i;\
  for (__i=0;__i<sizeof(dtype);__i++) bswap.b[__i] = p[__i];\
  ans = bswap.dtype##_;\
}

#endif

STATIC_ROUTINE int copy_rec_dx(char const *in_ptr, struct descriptor_xd *out_dsc_ptr,
			       unsigned int *b_out, unsigned int *b_in)
{
  int status = 1;
  unsigned int bytes_out = 0, bytes_in = 0, i, j, size_out, size_in;
  if (in_ptr && (in_ptr[0] || in_ptr[1] || in_ptr[2] || in_ptr[3])) {
    switch (class()) {
    case CLASS_S:
    case CLASS_D:
      {
	struct descriptor in = {0};
	struct descriptor *po = (struct descriptor *)out_dsc_ptr;
	set_length(in.length);
	in.dtype = dtype();
	FixLength(in);
	in.class = CLASS_S;
	if (po) {
	  *po = in;
	  po->pointer = (char *)(po + 1);
	  memcpy(po->pointer, in_ptr + 8, in.length);
#if defined(WORDS_BIGENDIAN)
	  if (po->length > 1 && po->dtype != DTYPE_T && po->dtype != DTYPE_IDENT
	      && po->dtype != DTYPE_PATH) {
	    switch (po->length) {
	    case 2:
	      swap(short, po->pointer, *(short *)po->pointer) break;
	    case 4:
	      swap(int, po->pointer, *(int *)po->pointer) break;
	    case 8:
	      swap(int64_t, po->pointer, *(int64_t *) po->pointer);
	      break;
	    }
	  }
#endif
	}
	bytes_out = (unsigned int)align(sizeof(struct descriptor) + in.length, sizeof(void *));
	bytes_in = 8u + in.length;
      }
      break;

    case CLASS_XS:
    case CLASS_XD:
      {
	struct descriptor_xs in = {0};
	struct descriptor_xs *po = (struct descriptor_xs *)out_dsc_ptr;
	in.length = 0;
	in.dtype = dtype();
	in.class = CLASS_XS;
	set_l_length(in.l_length);
	if (po) {
	  *po = in;
	  po->pointer = (struct descriptor *)(po + 1);
	  memcpy(po->pointer, in_ptr + 12, in.l_length);
	}
	bytes_out = (unsigned int)align(sizeof(struct descriptor_xs) + in.l_length, sizeof(void *));
	bytes_in = 12u + in.l_length;
      }
      break;

    case CLASS_R:
      {
	struct descriptor_r pi_tmp = {0};
	struct descriptor_r *pi = &pi_tmp;
	struct descriptor_r *po = (struct descriptor_r *)out_dsc_ptr;
	set_length(pi_tmp.length);
	pi_tmp.dtype = dtype();
	FixLength(pi_tmp);
	pi_tmp.class = CLASS_R;
	pi_tmp.ndesc = ndesc();
	pi_tmp.dscptrs[0] = 0;
	bytes_out = (unsigned int)sizeof(struct descriptor_r) + (pi->ndesc - 1u) * (unsigned int)sizeof(struct descriptor *);
	bytes_in = 12u + pi->ndesc * 4u;
	if (po) {

	  memset(po, 0, bytes_out);
	  *po = *pi;
	  if (pi->length > 0) {
	    po->pointer = (unsigned char *)po + bytes_out;
	    memcpy(po->pointer, &in_ptr[12 + pi->ndesc * 4], pi->length);
#if defined(WORDS_BIGENDIAN)
	    if (po->dtype == DTYPE_FUNCTION && po->length == 2) {
	      swap(short, (char *)po->pointer, *(short *)po->pointer);
	    }
#endif
	  }
	}
	bytes_out = (unsigned int)align(bytes_out + pi->length, sizeof(void *));
	bytes_in += pi->length;

      /******************************
      Each descriptor must be copied.
      ******************************/
	for (j = 0; j < pi->ndesc && status & 1; ++j) {
	  unsigned int offset;
	  set_dscptrs(offset, j);
	  if (offset) {
	    status =
		copy_rec_dx(in_ptr + bytes_in,
			    po ? (struct descriptor_xd *)((char *)po + bytes_out) : 0, &size_out,
			    &size_in);
	    if (po)
	      po->dscptrs[j] = size_out ? (struct descriptor *)((char *)po + bytes_out) : 0;
	    bytes_out = align(bytes_out + size_out, (unsigned int)sizeof(void *));
	    bytes_in += size_in;
	  }
	}
      }
      break;
    case CLASS_A:
      {
	unsigned int dsc_size, align_size;
	array_coeff a_tmp = {0};
	array_coeff *pi = &a_tmp;
	array_coeff *po = (array_coeff *) out_dsc_ptr;
	set_length(a_tmp.length);
	a_tmp.dtype = dtype();
	FixLength(a_tmp);
	a_tmp.class = class();
	a_tmp.scale = scale();
	a_tmp.digits = digits();
	a_tmp.aflags.binscale = binscale();
	a_tmp.aflags.redim = redim();
	a_tmp.aflags.column = column();
	a_tmp.aflags.coeff = coeff();
	a_tmp.aflags.bounds = bounds();
	a_tmp.dimct = dimct();
	set_arsize(a_tmp.arsize);
	bytes_in = 16u
            + (pi->aflags.coeff  ? (unsigned int)sizeof(int) + (unsigned int)sizeof(int) * pi->dimct : 0u)
	    + (pi->aflags.bounds ? (unsigned int)sizeof(int) * pi->dimct * 2u : 0u);
	dsc_size = (unsigned int)sizeof(struct descriptor_a)
	    + (pi->aflags.coeff  ? (unsigned int)sizeof(char *) + (unsigned int)sizeof(int) * pi->dimct : 0u)
	    + (pi->aflags.bounds ? (unsigned int)sizeof(int) * pi->dimct * 2u : 0u);
	align_size = (pi->dtype == DTYPE_T) ? 1 : pi->length;
	bytes_out = dsc_size + pi->arsize + align_size;
	if (po) {
	  memcpy(po, pi, sizeof(struct descriptor_a));
	  for (i = 0; i < pi->dimct; i++) {
	    if (pi->aflags.coeff) {
	      swap(int, m() + sizeof(int) * i, po->m[i]);
	      if (pi->aflags.bounds) {
		swap(int, m() + (pi->dimct + 2 * i) * sizeof(int), po->m[pi->dimct + 2 * i]);
		swap(int, m() + (pi->dimct + 2 * i + 1) * sizeof(int),
		     po->m[pi->dimct + 2 * i + 1]);
	      }
	    }
	  }
	  po->pointer =
	      (char *)po + align((char *)po - (char *)0 + dsc_size,
				 align_size) - ((char *)po - (char *)0);
	  memcpy(po->pointer, &in_ptr[bytes_in], pi->arsize);
	  if (pi->aflags.coeff) {
	    unsigned int offset, a0;
	    set_offset(offset);
	    set_a0(a0);
	    po->a0 = po->pointer + (a0 - offset);
	  }
#if defined(WORDS_BIGENDIAN)
	  if (po->dtype != DTYPE_T && po->dtype != DTYPE_IDENT && po->dtype != DTYPE_PATH) {
	    int i;
	    switch (po->length) {
	    case 2:
	      {
		short *ptr;
		for (i = 0, ptr = (short *)po->pointer; i < (int)po->arsize;
		     i += sizeof(*ptr), ptr++)
		  swap(short, (char *)ptr, *ptr);
	      }
	      break;
	    case 4:
	      {
		int *ptr;
		for (i = 0, ptr = (int *)po->pointer; i < (int)po->arsize; i += sizeof(*ptr), ptr++)
		  swap(int, (char *)ptr, *ptr);
	      }
	      break;
	    case 8:
	      {
		int64_t *ptr;
		for (i = 0, ptr = (int64_t *) po->pointer; i < (int)po->arsize;
		     i += sizeof(*ptr), ptr++)
		  swap(int64_t, (char *)ptr, *ptr);
	      }
	      break;
	    }
	  }
#endif
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
	array_coeff a_tmp = {0};
	array_coeff *pi = &a_tmp;
	array_coeff *po = (array_coeff *) out_dsc_ptr;
	//      struct descriptor **pdi = (struct descriptor **)pi->pointer;
	struct descriptor **pdo = 0;
	unsigned int num_dsc;
	set_length(a_tmp.length);
	a_tmp.dtype = dtype();
	FixLength(a_tmp);
	a_tmp.class = class();
	a_tmp.scale = scale();
	a_tmp.digits = digits();
	a_tmp.aflags.binscale = binscale();
	a_tmp.aflags.redim = redim();
	a_tmp.aflags.column = column();
	a_tmp.aflags.coeff = coeff();
	a_tmp.aflags.bounds = bounds();
	a_tmp.dimct = dimct();
	set_arsize(a_tmp.arsize);
	if (a_tmp.length != sizeof(void *)) {
	  a_tmp.arsize = a_tmp.arsize / a_tmp.length * (unsigned int)sizeof(void *);
	  a_tmp.length = sizeof(void *);
	}
	bytes_in = 16u
            + (pi->aflags.coeff  ? (unsigned int)sizeof(int) + (unsigned int)sizeof(int) * pi->dimct : 0u)
	    + (pi->aflags.bounds ? (unsigned int)sizeof(int) * pi->dimct * 2u : 0u);
	bytes_out = (unsigned int)sizeof(struct descriptor_a)
	    + (pi->aflags.coeff  ? (unsigned int)sizeof(char *) + (unsigned int)sizeof(int) * pi->dimct : 0u)
	    + (pi->aflags.bounds ? (unsigned int)sizeof(int) * pi->dimct * 2u : 0u);
	num_dsc = pi->arsize / pi->length;
	if (po) {
	  memcpy(po, pi, sizeof(struct descriptor_a));
	  for (i = 0; i < pi->dimct; i++) {
	    if (pi->aflags.coeff) {
	      swap(int, m() + i * sizeof(int), po->m[i]);
	      if (pi->aflags.bounds) {
		swap(int, m() + (pi->dimct + 2 * i) * sizeof(int), po->m[pi->dimct + 2 * i]);
		swap(int, m() + (pi->dimct + 2 * i + 1) * sizeof(int),
		     po->m[pi->dimct + 2 * i + 1]);
	      }
	    }
	  }
	  po->pointer = (char *)po + bytes_out;
	  if (pi->aflags.coeff) {
	    unsigned int offset;
	    unsigned int a0;
	    set_offset(offset);
	    set_a0(a0);
	    po->a0 = po->pointer + (a0 - offset);
	  }
	  pdo = (struct descriptor **)po->pointer;
	  po->length = sizeof(struct descriptor *);
	  po->arsize = num_dsc * (unsigned int)sizeof(struct descriptor *);
	}
	bytes_out += pi->arsize;
	bytes_in += num_dsc * (unsigned int)sizeof(int);
      /******************************
      Each descriptor must be copied.
      ******************************/
	for (j = 0; j < num_dsc && status & 1; ++j) {
	  status =
	      copy_rec_dx(in_ptr + bytes_in,
			  po ? (struct descriptor_xd *)((char *)po + bytes_out) : 0, &size_out,
			  &size_in);
	  if (po)
	    *pdo++ = size_out ? (struct descriptor *)((char *)po + bytes_out) : 0;
	  bytes_out += size_out;
	  bytes_in += size_in;
	}
      }
      break;

    case CLASS_CA:
      {
	array_coeff a_tmp = {0};
	array_coeff *pi = &a_tmp;
	array_coeff *po = (array_coeff *) out_dsc_ptr;
	//struct descriptor **pdi = (struct descriptor **)pi->pointer;
	//struct descriptor **pdo = 0;
	unsigned int offset;
	set_length(a_tmp.length);
	a_tmp.dtype = dtype();
	FixLength(a_tmp);
	a_tmp.class = class();
	a_tmp.scale = scale();
	a_tmp.digits = digits();
	a_tmp.aflags.binscale = binscale();
	a_tmp.aflags.redim = redim();
	a_tmp.aflags.column = column();
	a_tmp.aflags.coeff = coeff();
	a_tmp.aflags.bounds = bounds();
	a_tmp.dimct = dimct();
	set_arsize(a_tmp.arsize);
	bytes_in = 16u
            + (pi->aflags.coeff  ? (unsigned int)sizeof(int) + (unsigned int)sizeof(int) * pi->dimct : 0u)
	    + (pi->aflags.bounds ? (unsigned int)sizeof(int) * pi->dimct * 2u : 0u);
	bytes_out = align((unsigned int)sizeof(struct descriptor_a)
			  + (pi->aflags.coeff  ? (unsigned int)sizeof(char *) + (unsigned int)sizeof(int) * pi->dimct : 0u)
			  + (pi->aflags.bounds ? (unsigned int)sizeof(int) * pi->dimct * 2u : 0u),
			  (unsigned int)sizeof(void *));
	set_offset(offset);
	if (po) {
	  memcpy(po, pi, sizeof(struct descriptor_a));
	  if (pi->aflags.coeff) {
	    unsigned int a0;
	    set_a0(a0);
	    po->a0 = (char *)0 + a0;
	  }
	  for (i = 0; i < pi->dimct; i++) {
	    if (pi->aflags.coeff) {
	      swap(int, m() + i * sizeof(int), po->m[i]);
	      if (pi->aflags.bounds) {
		swap(int, m() + (pi->dimct + 2 * i) * sizeof(int), po->m[pi->dimct + 2 * i]);
		swap(int, m() + (pi->dimct + 2 * i + 1) * sizeof(int),
		     po->m[pi->dimct + 2 * i + 1]);
	      }
	    }
	  }
/*          po->pointer = offset ?
              (char *)po + align( ((char *)po - (char *)0) + bytes_out, sizeof(void *)) - ((char *)po - (char *)0) : 0;*/
//do not use align!!
	  po->pointer = offset ? ((char *)po + bytes_out) : 0;

	  if (po->pointer && pi->aflags.bounds) {
	    po->a0 = (char *)0 - (po->m[po->dimct] * po->length);
	  }
	}

      /***************************
      Null pointer for shape only.
      ***************************/
	if (offset) {
	  status =
	      copy_rec_dx(in_ptr + bytes_in,
			  po ? (struct descriptor_xd *)((char *)po + bytes_out) : 0, &size_out,
			  &size_in);
	  bytes_out += size_out;
	  bytes_in += size_in;
	}
      }
      break;

    default:
      status = LibINVSTRDES;
      break;
    }
  } else {
    struct descriptor *po = (struct descriptor *)out_dsc_ptr;
    static struct descriptor empty = {0,DTYPE_DSC,CLASS_S,0};
    if (po) {
      memcpy(po, &empty, sizeof(empty));
    }
    bytes_out = align(sizeof(empty), sizeof(void *));
    bytes_in = sizeof(int);
  }
  *b_out = bytes_out;
  *b_in = bytes_in;
  return status;
}

EXPORT int MdsSerializeDscIn(char const *in, struct descriptor_xd *out)
{
  unsigned int size_out;
  unsigned int size_in;
  int status;
  STATIC_CONSTANT const unsigned char dsc_dtype = DTYPE_DSC;
  status = copy_rec_dx(in, 0, &size_out, &size_in);
  if (STATUS_OK && size_out) {
    status = MdsGet1Dx(&size_out, (unsigned char *)&dsc_dtype, out, 0);
    if STATUS_OK
      status = copy_rec_dx(in, (struct descriptor_xd *)out->pointer, &size_out, &size_in);
  } else
    MdsFree1Dx(out, 0);
  return status;
}

STATIC_ROUTINE int copy_dx_rec(struct descriptor *in_ptr, char *out_ptr, unsigned int *b_out,
			       unsigned int *b_in)
{ int status = MDSplusSUCCESS;
  unsigned bytes_out = 0, bytes_in = 0, j, size_out, size_in, num_dsc;
  if (in_ptr)
    switch (in_ptr->class) {
    case CLASS_S:
    case CLASS_D:
      {
	if (out_ptr) {
	  char *inp = ((char *)in_ptr) + offset(in_ptr->pointer);
	  int dscsize = 8;
	  LoadShort(in_ptr->length, out_ptr);
	  LoadChar(in_ptr->dtype, out_ptr + 2);
	  LoadChar(in_ptr->class, out_ptr + 3);
	  LoadInt(dscsize, out_ptr + 4);
	  out_ptr += 8;
#ifdef WORDS_BIGENDIAN
	  if (in_ptr->dtype != DTYPE_T && in_ptr->dtype != DTYPE_IDENT
	      && in_ptr->dtype != DTYPE_PATH) {
	    switch (in_ptr->length) {
	    case 2:
	      LoadShort(inp[0], out_ptr);
	      break;
	    case 4:
	      LoadInt(inp[0], out_ptr);
	      break;
	    case 8:
	      LoadQuad(inp[0], out_ptr);
	      break;
	    default:
	      memcpy(out_ptr, inp, in_ptr->length);
	      break;
	    }
	  } else
#endif
	    memcpy(out_ptr, inp, in_ptr->length);
	  out_ptr += in_ptr->length;
	}
	bytes_out = 8u + in_ptr->length;
	bytes_in = (unsigned int)sizeof(struct descriptor) + in_ptr->length;
      }
      break;

    case CLASS_XS:
    case CLASS_XD:
      {
	struct descriptor_xs *inp = (struct descriptor_xs *)in_ptr;
	if (out_ptr) {
	  int dscsize = 12;
	  LoadShort(in_ptr->length, out_ptr);
	  LoadChar(in_ptr->dtype, out_ptr + 2);
	  LoadChar(in_ptr->class, out_ptr + 3);
	  LoadInt(dscsize, out_ptr + 4);
	  LoadInt(inp->l_length, out_ptr + 8);
	  out_ptr += 12;
	  memcpy(out_ptr, ((char *)in_ptr) + offset(in_ptr->pointer), inp->l_length);
	  out_ptr += inp->l_length;
	}
	bytes_in = (unsigned int)sizeof(struct descriptor_xs) + inp->l_length;
	bytes_out = 12u + inp->l_length;
      }
      break;

    case CLASS_R:
      {
	struct descriptor_r *inp = (struct descriptor_r *)in_ptr;
	char *begin = out_ptr;
	char *dscptrs = NULL;
	if (out_ptr) {
	  int dscsize = inp->length ? 12 + inp->ndesc * 4 : 0;
	  LoadShort(inp->length, out_ptr);
	  LoadChar(inp->dtype, out_ptr + 2);
	  LoadChar(inp->class, out_ptr + 3);
	  LoadInt(dscsize, out_ptr + 4);
	  LoadChar(inp->ndesc, out_ptr + 8);
	  dscptrs = out_ptr + 12;
	  memset(dscptrs, 0, inp->ndesc * 4u);
	  out_ptr += 12u + inp->ndesc * 4u;
	  if (inp->length) {
	    memcpy(out_ptr, ((char *)inp) + offset(inp->pointer), inp->length);
#if defined(WORDS_BIGENDIAN)
	    if (inp->dtype == DTYPE_FUNCTION && inp->length == 2) {
	      short value;
	      swap(short, (char *)out_ptr, value);
	      memcpy(out_ptr, &value, 2);
	    }
#endif
	  }
	  out_ptr += inp->length;
	}
	bytes_out = 12u + inp->ndesc * 4u + inp->length;
	bytes_in = (unsigned int)sizeof(struct descriptor_r) + (inp->ndesc - 1u) * (unsigned int)sizeof(struct descriptor *) + inp->length;
	for (j = 0; j < inp->ndesc; j++) {
	  if (inp->dscptrs[j]) {
	    status =
		copy_dx_rec((struct descriptor *)(((char *)inp) + offset(inp->dscptrs[j])), out_ptr,
			    &size_out, &size_in);
	    if (out_ptr) {
	      unsigned int poffset = (unsigned int)(out_ptr - begin);
	      LoadInt(poffset, dscptrs + (j * 4));
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
	array_coeff *inp = (array_coeff *) in_ptr;
	if (out_ptr) {
	  char *inp2 = ((char *)inp) + offset(inp->pointer);
	  unsigned int dscsize = 16u
              + (inp->aflags.coeff  ? (unsigned int)sizeof(int) + (unsigned int)sizeof(int) * inp->dimct : 0u)
	      + (inp->aflags.bounds ? (unsigned int)sizeof(int) * (inp->dimct * 2u) : 0u);
	  LoadShort(inp->length, out_ptr);
	  LoadChar(inp->dtype, out_ptr + 2);
	  LoadChar(inp->class, out_ptr + 3);
	  LoadInt(dscsize, out_ptr + 4);
	  LoadChar(inp->scale, out_ptr + 8);
	  LoadChar(inp->digits, out_ptr + 9);
	  set_aflags(out_ptr, in);
	  LoadChar(inp->dimct, out_ptr + 11);
	  LoadInt(inp->arsize, out_ptr + 12);
	  out_ptr += 16;
	  if (inp->aflags.coeff) {
	    unsigned int a0 = dscsize + (offset(inp->a0) - offset(inp->pointer));
	    LoadInt(a0, out_ptr);
	    out_ptr += 4;
	    for (j = 0; j < inp->dimct; j++) {
	      LoadInt(inp->m[j], out_ptr);
	      out_ptr += 4;
	    }
	    if (inp->aflags.bounds) {
	      for (j = 0; j < inp->dimct; j++) {
		LoadInt(inp->m[inp->dimct + 2 * j], out_ptr);
		out_ptr += 4;
		LoadInt(inp->m[inp->dimct + 2 * j + 1], out_ptr);
		out_ptr += 4;
	      }
	    }
	  }
#ifdef WORDS_BIGENDIAN
	  if (in_ptr->dtype != DTYPE_T && in_ptr->dtype != DTYPE_IDENT
	      && in_ptr->dtype != DTYPE_PATH) {
	    unsigned int i;
	    switch (in_ptr->length) {
	    case 2:
	      for (i = 0; i < inp->arsize; i += sizeof(short)) {
		LoadShort(inp2[i], out_ptr + i);
	      }
	      break;
	    case 4:
	      for (i = 0; i < inp->arsize; i += sizeof(int)) {
		LoadInt(inp2[i], out_ptr + i);
	      } break;
	    case 8:
	      for (i = 0; i < inp->arsize; i += in_ptr->length) {
		LoadQuad(inp2[i], out_ptr + i);
	      }
	      break;
	    default:
	      memcpy(out_ptr, inp2, inp->arsize);
	    }
	  } else
#endif
	    memcpy(out_ptr, inp2, inp->arsize);
	  out_ptr += inp->arsize;
	}
	bytes_out = 16u
            + (inp->aflags.coeff  ? (unsigned int)sizeof(int) + (unsigned int)sizeof(int) * inp->dimct : 0u)
	    + (inp->aflags.bounds ? (unsigned int)sizeof(int) * inp->dimct * 2u : 0u) + inp->arsize;
	bytes_in = (unsigned int)sizeof(struct descriptor_a)
	    + (inp->aflags.coeff  ? (unsigned int)sizeof(char *) + (unsigned int)sizeof(int) * inp->dimct : 0u)
	    + (inp->aflags.bounds ? (unsigned int)sizeof(int) * inp->dimct * 2u : 0u) + inp->arsize;
      }
      break;

    /**************************************
    For CA and APD, a0 is the offset.
    **************************************/
    case CLASS_APD:
      {
	array_coeff *inp = (array_coeff *) in_ptr;
	struct descriptor **dsc = (struct descriptor **)(((char *)in_ptr) + offset(inp->pointer));
	char *begin = out_ptr;
	char *dscptr = NULL;
	num_dsc = inp->arsize / inp->length;
	if (out_ptr) {
	  unsigned int dscsize = 16u
              + (inp->aflags.coeff  ? (unsigned int)sizeof(int) + (unsigned int)sizeof(int) * inp->dimct : 0u)
	      + (inp->aflags.bounds ? (unsigned int)sizeof(int) * inp->dimct * 2u : 0u);
	  short length = sizeof(int);
	  unsigned int arsize = (unsigned int)sizeof(int) * num_dsc;
	  LoadShort(length, out_ptr);
	  LoadChar(inp->dtype, out_ptr + 2);
	  LoadChar(inp->class, out_ptr + 3);
	  LoadInt(dscsize, out_ptr + 4);
	  LoadChar(inp->scale, out_ptr + 8);
	  LoadChar(inp->digits, out_ptr + 9);
	  set_aflags(out_ptr, in);
	  LoadChar(inp->dimct, out_ptr + 11);
	  LoadInt(arsize, out_ptr + 12);
	  out_ptr += 16;
	  if (inp->aflags.coeff) {
	    unsigned int a0 = dscsize + (offset(inp->a0) - offset(inp->pointer));
	    LoadInt(a0, out_ptr);
	    out_ptr += 4;
	    for (j = 0; j < inp->dimct; j++) {
	      LoadInt(inp->m[j], out_ptr);
	      out_ptr += 4;
	    }
	    if (inp->aflags.bounds) {
	      for (j = 0; j < inp->dimct; j++) {
		LoadInt(inp->m[inp->dimct + 2 * j], out_ptr);
		out_ptr += 4;
		LoadInt(inp->m[inp->dimct + 2 * j + 1], out_ptr);
		out_ptr += 4;
	      }
	    }
	  }
	  dscptr = out_ptr;
	  out_ptr += num_dsc * 4;
	  memset(dscptr, 0, num_dsc * 4);
	}
	bytes_out = 16u
            + (inp->aflags.coeff  ? (unsigned int)sizeof(char *) + (unsigned int)sizeof(int) * inp->dimct : 0u)
	    + (inp->aflags.bounds ? (unsigned int)sizeof(int) * inp->dimct * 2u : 0u + num_dsc * 4u);
	bytes_in = (unsigned int)sizeof(struct descriptor_a)
	    + (inp->aflags.coeff  ? (unsigned int)sizeof(int) + (unsigned int)sizeof(int) * inp->dimct : 0u)
	    + (inp->aflags.bounds ? (unsigned int)sizeof(int) * inp->dimct * 2u : 0u) + inp->arsize;
      /******************************
      Each descriptor must be copied.
      ******************************/
	for (j = 0; j < num_dsc; j++, bytes_in += 4) {
	  if (offset(dsc[j])) {
	    status =
		copy_dx_rec((struct descriptor *)(((char *)in_ptr) + offset(dsc[j])), out_ptr,
			    &size_out, &size_in);
	    if (out_ptr) {
	      unsigned int poffset = (unsigned int)(out_ptr - begin);
	      LoadInt(poffset, dscptr + (j * 4));
	      out_ptr += size_out;
	    }
	    bytes_out += size_out;
	    bytes_in += size_in;
	  } else {
	    if (out_ptr) {
	      unsigned int poffset = 0;
	      LoadInt(poffset, dscptr + (j * 4));
	      out_ptr += (unsigned int)sizeof(int);
	    }
	    bytes_out += (unsigned int)sizeof(int);
	    bytes_in += (unsigned int)sizeof(void *);
	  }
	}
      }
      break;

    case CLASS_CA:
      {
	array_coeff *inp = (array_coeff *) in_ptr;
	if (out_ptr) {
	  unsigned int dscsize = inp->pointer ? 16u
              + (inp->aflags.coeff  ? (unsigned int)sizeof(int) + (unsigned int)sizeof(int) * inp->dimct : 0u)
	      + (inp->aflags.bounds ? (unsigned int)sizeof(int) * inp->dimct * 2u : 0u) : 0u;
	  LoadShort(inp->length, out_ptr);
	  LoadChar(inp->dtype, out_ptr + 2);
	  LoadChar(inp->class, out_ptr + 3);
	  LoadInt(dscsize, out_ptr + 4);
	  LoadChar(inp->scale, out_ptr + 8);
	  LoadChar(inp->digits, out_ptr + 9);
	  set_aflags(out_ptr, in);
	  LoadChar(inp->dimct, out_ptr + 11);
	  LoadInt(inp->arsize, out_ptr + 12);
	  out_ptr += 16;
	  if (inp->aflags.coeff) {
	    unsigned int a0 = (unsigned int)((char *)inp->a0 - (char *)0);
	    LoadInt(a0, out_ptr);
	    out_ptr += 4;
	    for (j = 0; j < inp->dimct; j++) {
	      LoadInt(inp->m[j], out_ptr);
	      out_ptr += 4;
	    }
	    if (inp->aflags.bounds) {
	      for (j = 0; j < inp->dimct; j++) {
		LoadInt(inp->m[inp->dimct + 2 * j], out_ptr);
		out_ptr += 4;
		LoadInt(inp->m[inp->dimct + 2 * j + 1], out_ptr);
		out_ptr += 4;
	      }
	    }
	  }
	}
	bytes_out = 16u
            + (inp->aflags.coeff  ? (unsigned int)sizeof(int) + (unsigned int)sizeof(int) * inp->dimct : 0u)
	    + (inp->aflags.bounds ? (unsigned int)sizeof(int) * inp->dimct * 2u : 0u);
	bytes_in = (unsigned int)sizeof(struct descriptor_a)
	    + (inp->aflags.coeff  ? (unsigned int)sizeof(char *) + (unsigned int)sizeof(int) * inp->dimct : 0u)
	    + (inp->aflags.bounds ? (unsigned int)sizeof(int) * inp->dimct * 2u : 0u);
      /***************************
      Null pointer for shape only.
      ***************************/
	if (inp->pointer) {
	  status =
	      copy_dx_rec((struct descriptor *)(((char *)in_ptr) + offset(inp->pointer)), out_ptr,
			  &size_out, &size_in);
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
  *b_in = bytes_in;
  return status;
}

STATIC_ROUTINE int Dsc2Rec(struct descriptor const *inp, struct descriptor_xd *out_dsc_ptr,
			   unsigned int *reclen)
{
  unsigned int size_out;
  unsigned int size_in;
  int status;
  STATIC_CONSTANT const unsigned char dsc_dtype = DTYPE_B;
  status = copy_dx_rec((struct descriptor *)inp, 0, &size_out, &size_in);
  if (status & 1 && size_out) {
    unsigned short nlen = 1;
    array out_template = { 1, DTYPE_B, CLASS_A, 0, 0, 0, {0, 1, 1, 0, 0}, 1, 0 };
    out_template.arsize = *reclen = size_out;
    status =
	MdsGet1DxA((struct descriptor_a *)&out_template, &nlen, (unsigned char *)&dsc_dtype,
		   out_dsc_ptr);
    if (status & 1) {
      memset(out_dsc_ptr->pointer->pointer, 0, size_out);
      status =
	  copy_dx_rec((struct descriptor *)inp, out_dsc_ptr->pointer->pointer, &size_out, &size_in);
    }
  } else
    MdsFree1Dx(out_dsc_ptr, NULL);
  return status;
}

STATIC_CONSTANT int PointerToOffset(struct descriptor *dsc_ptr, unsigned int *length)
{
  int status = 1;
  if ((dsc_ptr->dtype == DTYPE_DSC) && (dsc_ptr->class != CLASS_A) && (dsc_ptr->class != CLASS_APD))
    status = PointerToOffset((struct descriptor *)dsc_ptr->pointer, length);
  if (status & 1) {
    switch (dsc_ptr->class) {
    case CLASS_S:
    case CLASS_D:
      *length += (unsigned int)sizeof(struct descriptor) + dsc_ptr->length;
      dsc_ptr->pointer = dsc_ptr->pointer - ((char *)dsc_ptr - (char *)0);
      break;
    case CLASS_XD:
    case CLASS_XS:
      *length += (unsigned int)sizeof(struct descriptor_xd) + ((struct descriptor_xd *)dsc_ptr)->l_length;
      dsc_ptr->pointer = dsc_ptr->pointer - ((char *)dsc_ptr - (char *)0);
      break;
    case CLASS_R:
      {
	struct descriptor_r *r_ptr = (struct descriptor_r *)dsc_ptr;
	int i;
	*length += (unsigned int)sizeof(*r_ptr) + (r_ptr->ndesc - 1u) * (unsigned int)sizeof(struct descriptor *)
	    + r_ptr->length;
	if (r_ptr->length != 0)
	  r_ptr->pointer = r_ptr->pointer - ((char *)r_ptr - (char *)0);
	for (i = 0; (status & 1) && (i < r_ptr->ndesc); i++)
	  if (r_ptr->dscptrs[i] != 0) {
	    status = PointerToOffset(r_ptr->dscptrs[i], length);
	    r_ptr->dscptrs[i] =
		(struct descriptor *)((char *)r_ptr->dscptrs[i] - ((char *)r_ptr - (char *)0));
	  }
      }
      break;
    case CLASS_A:
      {
	struct descriptor_a *a_ptr = (struct descriptor_a *)dsc_ptr;
	*length += (unsigned int)sizeof(struct descriptor_a)
	    + (a_ptr->aflags.coeff  ? (unsigned int)sizeof(int) * (a_ptr->dimct + 1u) : 0u)
	    + (a_ptr->aflags.bounds ? (unsigned int)sizeof(int) *  a_ptr->dimct * 2u  : 0u)
	    + a_ptr->arsize;
	a_ptr->pointer = a_ptr->pointer - ((char *)a_ptr - (char *)0);
	if (a_ptr->aflags.coeff) {
	  unsigned int *a0_ptr = (unsigned int *)((char *)a_ptr + sizeof(struct descriptor_a));
	  *a0_ptr = *a0_ptr - (unsigned int)((char *)a_ptr - (char *)0);
	}
      }
      break;
    case CLASS_APD:
      {
	struct descriptor_a *a_ptr = (struct descriptor_a *)dsc_ptr;
	unsigned int i,elts = a_ptr->arsize / a_ptr->length;
	*length += (unsigned int)sizeof(struct descriptor_a)
	    + (a_ptr->aflags.coeff  ? (unsigned int)sizeof(int) * (a_ptr->dimct + 1u) : 0u)
	    + (a_ptr->aflags.bounds ? (unsigned int)sizeof(int) *  a_ptr->dimct * 2u  : 0u)
	    + a_ptr->arsize;
	for (i = 0; (i < elts) && STATUS_OK; i++) {
	  struct descriptor **dsc_ptr = (struct descriptor **)a_ptr->pointer + i;
	  if (dsc_ptr && *dsc_ptr) {
	    status = PointerToOffset(*dsc_ptr, length);
	    *dsc_ptr = (struct descriptor *)((char *)*dsc_ptr - ((char *)a_ptr - (char *)0));
	  } else {
	    status = 1;
	    *dsc_ptr = 0;
	  }
	}
	if STATUS_OK {
	  a_ptr->pointer = a_ptr->pointer - ((char *)a_ptr - (char *)0);
	  if (a_ptr->aflags.coeff) {
	    unsigned int *a0_ptr = (unsigned int *)((char *)a_ptr + sizeof(struct descriptor_a));
	    *a0_ptr = *a0_ptr - (unsigned int)((char *)a_ptr - (char *)0);
	  }
        }
      }
      break;
    case CLASS_CA:
      if (dsc_ptr->pointer) {
	unsigned int dummy_length;
	struct descriptor_a *a_ptr = (struct descriptor_a *)dsc_ptr;
	*length += (unsigned int)sizeof(struct descriptor_a)
	    + (a_ptr->aflags.coeff  ? (unsigned int)sizeof(int) * (a_ptr->dimct + 1u) : 0u)
	    + (a_ptr->aflags.bounds ? (unsigned int)sizeof(int) *  a_ptr->dimct * 2u  : 0u)
	    + a_ptr->arsize;
	status = PointerToOffset((struct descriptor *)dsc_ptr->pointer, &dummy_length);
	a_ptr->pointer = a_ptr->pointer - ((char *)a_ptr - (char *)0);
      }
      break;
    default:
      status = LibINVSTRDES;
      break;
    }
  }
  return status;
}

EXPORT int MdsSerializeDscOutZ(struct descriptor const *in,
			struct descriptor_xd *out,
			int (*fixupNid) (),
			void *fixupNidArg,
			int (*fixupPath) (),
			void *fixupPathArg,
			int compress,
			int *compressible_out,
			unsigned int *length_out,
			unsigned int *reclen_out,
			unsigned char *dtype_out,
			unsigned char *class_out,
			int altbuflen, void *altbuf, int *data_in_altbuf_out)
{
  int status;
  struct descriptor *out_ptr;
  struct descriptor_xd tempxd;
  int compressible = 0;
  unsigned int length = 0;
  unsigned int reclen = 0;
  unsigned char dtype = 0;
  unsigned char class = 0;
  int data_in_altbuf = 0;
  status = MdsCopyDxXdZ(in, out, 0, fixupNid, fixupNidArg, fixupPath, fixupPathArg);
  if (status == MdsCOMPRESSIBLE) {
    if (compress) {
      tempxd = *out;
      out->l_length = 0;
      out->pointer = 0;
      status = MdsCompress(0, 0, tempxd.pointer, out);
      MdsFree1Dx(&tempxd, NULL);
      compressible = 0;
    } else
      compressible = 1;
  } else
    compressible = 0;
  if (status & 1) {
    if (out->pointer && out->dtype == DTYPE_DSC) {
      out_ptr = out->pointer;
      dtype = out_ptr->dtype;
      if ((out_ptr->class == CLASS_S || out_ptr->class == CLASS_D) && out_ptr->length < altbuflen) {
	data_in_altbuf = 1;
	class = CLASS_S;
	length = out_ptr->length + 8u;
#ifdef WORDS_BIGENDIAN
	if (dtype != DTYPE_T && dtype != DTYPE_IDENT && dtype != DTYPE_PATH) {
	  char *outp = (char *)altbuf;
	  char *inp = out_ptr->pointer;
	  switch (out_ptr->length) {
	  case 2:
	    LoadShort(inp[0], outp);
	    break;
	  case 4:
	    LoadInt(inp[0], outp);
	    break;
	  case 8:
	    LoadQuad(inp[0], outp);
	    break;
	  default:
	    memcpy(outp, inp, out_ptr->length);
	  }
	} else
#endif
	  memcpy(altbuf, out_ptr->pointer, out_ptr->length);
      } else {
	data_in_altbuf = 0;
	class = (out_ptr->class == CLASS_XD) ? CLASS_XS : out_ptr->class;
	length = 0;
	status = PointerToOffset(out->pointer, &length);
	if (status & 1) {
	  tempxd = *out;
	  out->l_length = 0;
	  out->pointer = 0;
	  Dsc2Rec(tempxd.pointer, out, &reclen);
	  MdsFree1Dx(&tempxd, NULL);
	}
      }
    } else {
      length = 0;
      dtype = 0;
      class = 0;
      reclen = 0;
      data_in_altbuf = 1;
    }
  }
  if (compressible_out)
    *compressible_out = compressible;
  if (length_out)
    *length_out = length;
  if (reclen_out && (data_in_altbuf != 1))
    *reclen_out = reclen;
  if (dtype_out)
    *dtype_out = dtype;
  if (class_out)
    *class_out = class;
  if (data_in_altbuf_out)
    *data_in_altbuf_out = data_in_altbuf;
  return status;
}

EXPORT int MdsSerializeDscOut(struct descriptor const *in, struct descriptor_xd *out)
{
  return MdsSerializeDscOutZ(in, out, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}
