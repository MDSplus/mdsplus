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
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <mdsshr_messages.h>
#include <mdstypes.h>
#include <_ncidef.h>
#include <stdint.h>
#include <string.h>

#define align(bytes, size) ((((bytes) + (size)-1) / (size)) * (size))
#define set_length(ans) swap(short, &in_ptr[0], ans)
#define dtype() (*(unsigned char *)&in_ptr[2])
#define class()(*(unsigned char *)&in_ptr[3])
#define set_offset(ans) swap(int, &in_ptr[4], ans)
#define set_l_length(ans) swap(int, &in_ptr[8], ans)
#define ndesc() (*(unsigned char *)&in_ptr[8])
#define set_dscptrs(ans, j) swap(int, &in_ptr[12 + j * 4], ans)
#define scale() (*(char *)&in_ptr[8])
#define digits() (*(unsigned char *)&in_ptr[9])
#define binscale() ((*((unsigned char *)&in_ptr[10]) & 0x08) != 0)
#define redim() ((*((unsigned char *)&in_ptr[10]) & 0x10) != 0)
#define column() ((*((unsigned char *)&in_ptr[10]) & 0x20) != 0)
#define coeff() ((*((unsigned char *)&in_ptr[10]) & 0x40) != 0)
#define bounds() ((*((unsigned char *)&in_ptr[10]) & 0x80) != 0)
#define dimct() (*(unsigned char *)&in_ptr[11])
#define set_arsize(ans) swap(int, &in_ptr[12], ans)
#define set_a0(ans) swap(int, &in_ptr[16], ans)
#define m() (&in_ptr[20])
#define LoadChar(in, outp) (outp)[0] = ((char *)&in)[0]
#define FixLength(a) \
  if (a.length == 0) \
  MdsFixDscLength((mdsdsc_t *)&a)
#define N_ELEMENTS(a) (sizeof(a) / sizeof(a[0]))
void MdsFixDscLength(mdsdsc_t *in);

#ifdef WORDS_BIGENDIAN
#define LoadShort(in, outp)     \
  (outp)[0] = ((char *)&in)[1]; \
  (outp)[1] = ((char *)&in)[0]
#define LoadInt(in, outp)       \
  (outp)[0] = ((char *)&in)[3]; \
  (outp)[1] = ((char *)&in)[2]; \
  (outp)[2] = ((char *)&in)[1]; \
  (outp)[3] = ((char *)&in)[0]
#define LoadQuad(in, outp)      \
  (outp)[0] = ((char *)&in)[7]; \
  (outp)[1] = ((char *)&in)[6]; \
  (outp)[2] = ((char *)&in)[5]; \
  (outp)[3] = ((char *)&in)[4]; \
  (outp)[4] = ((char *)&in)[3]; \
  (outp)[5] = ((char *)&in)[2]; \
  (outp)[6] = ((char *)&in)[1]; \
  (outp)[7] = ((char *)&in)[0]
#else
#define LoadShort(in, outp)     \
  (outp)[0] = ((char *)&in)[0]; \
  (outp)[1] = ((char *)&in)[1]
#define LoadInt(in, outp)       \
  (outp)[0] = ((char *)&in)[0]; \
  (outp)[1] = ((char *)&in)[1]; \
  (outp)[2] = ((char *)&in)[2]; \
  (outp)[3] = ((char *)&in)[3]
#define LoadQuad(in, outp)      \
  (outp)[0] = ((char *)&in)[0]; \
  (outp)[1] = ((char *)&in)[1]; \
  (outp)[2] = ((char *)&in)[2]; \
  (outp)[3] = ((char *)&in)[3]; \
  (outp)[4] = ((char *)&in)[4]; \
  (outp)[5] = ((char *)&in)[5]; \
  (outp)[6] = ((char *)&in)[6]; \
  (outp)[7] = ((char *)&in)[7]
#endif
#define set_aflags(ptr, in)                                                 \
  ptr[10] = (char)((inp->aflags.binscale << 3) | (inp->aflags.redim << 4) | \
                   (inp->aflags.column << 5) | (inp->aflags.coeff << 6) |   \
                   (inp->aflags.bounds << 7))
#define offset(ptr) *(uint32_t *)&ptr

union __bswap {
  char b[8];
  uint32_t int_;
  unsigned short short_;
  uint64_t int64_t_;
};

#if defined(WORDS_BIGENDIAN)

#define swap(dtype, ptr, ans)                    \
  {                                              \
    char const *p = ptr;                         \
    union __bswap bswap;                         \
    uint32_t __i;                                \
    for (__i = 0; __i < sizeof(dtype); __i++)    \
      bswap.b[sizeof(dtype) - __i - 1] = p[__i]; \
    ans = bswap.dtype##_;                        \
  }

#else

#define swap(dtype, ptr, ans)                 \
  {                                           \
    char const *p = ptr;                      \
    union __bswap bswap;                      \
    uint32_t __i;                             \
    for (__i = 0; __i < sizeof(dtype); __i++) \
      bswap.b[__i] = p[__i];                  \
    ans = bswap.dtype##_;                     \
  }

#endif

static int copy_rec_dx(char const *in_ptr, mdsdsc_xd_t *out_dsc_ptr,
                       l_length_t *b_out, l_length_t *b_in)
{
  int status = 1;
  uint32_t bytes_out = 0, bytes_in = 0, i, j, size_out, size_in;
  if (in_ptr && (in_ptr[0] || in_ptr[1] || in_ptr[2] || in_ptr[3]))
  {
    switch (class())
    {
    case CLASS_S:
    case CLASS_D:
    {
      mdsdsc_t in = {0};
      mdsdsc_t *po = (mdsdsc_t *)out_dsc_ptr;
      set_length(in.length);
      in.dtype = dtype();
      FixLength(in);
      in.class = CLASS_S;
      if (po)
      {
        *po = in;
        po->pointer = (void *)(po + 1);
        memcpy(po->pointer, in_ptr + 8, in.length);
#if defined(WORDS_BIGENDIAN)
        if (po->length > 1 && po->dtype != DTYPE_T &&
            po->dtype != DTYPE_IDENT && po->dtype != DTYPE_PATH)
        {
          switch (po->length)
          {
          case 2:
            swap(int16_t, po->pointer, *(int16_t *)po->pointer);
            break;
          case 4:
            swap(int32_t, po->pointer, *(int32_t *)po->pointer);
            break;
          case 8:
            swap(int64_t, po->pointer, *(int64_t *)po->pointer);
            break;
          }
        }
#endif
      }
      bytes_out = (uint32_t)align(sizeof(mdsdsc_t) + in.length, sizeof(void *));
      bytes_in = 8u + in.length;
    }
    break;

    case CLASS_XS:
    case CLASS_XD:
    {
      mdsdsc_xs_t in = {0};
      mdsdsc_xs_t *po = (mdsdsc_xs_t *)out_dsc_ptr;
      in.length = 0;
      in.dtype = dtype();
      in.class = CLASS_XS;
      set_l_length(in.l_length);
      if (po)
      {
        *po = in;
        po->pointer = (mdsdsc_t *)(po + 1);
        memcpy(po->pointer, in_ptr + 12, in.l_length);
      }
      bytes_out =
          (uint32_t)align(sizeof(mdsdsc_xs_t) + in.l_length, sizeof(void *));
      bytes_in = 12u + in.l_length;
    }
    break;

    case CLASS_R:
    {
      mdsdsc_r_t pi_tmp = {0};
      mdsdsc_r_t *pi = &pi_tmp;
      mdsdsc_r_t *po = (mdsdsc_r_t *)out_dsc_ptr;
      set_length(pi_tmp.length);
      pi_tmp.dtype = dtype();
      FixLength(pi_tmp);
      pi_tmp.class = CLASS_R;
      pi_tmp.ndesc = ndesc();
      pi_tmp.dscptrs[0] = 0;
      bytes_out = (uint32_t)sizeof(mdsdsc_r_t) +
                  (pi->ndesc - 1u) * (uint32_t)sizeof(mdsdsc_t *);
      bytes_in = 12u + pi->ndesc * 4u;
      if (po)
      {

        memset(po, 0, bytes_out);
        *po = *pi;
        if (pi->length > 0)
        {
          po->pointer = (unsigned char *)po + bytes_out;
          memcpy(po->pointer, &in_ptr[12 + pi->ndesc * 4], pi->length);
#if defined(WORDS_BIGENDIAN)
          if (po->dtype == DTYPE_FUNCTION && po->length == 2)
          {
            swap(short, (char *)po->pointer, *(short *)po->pointer);
          }
#endif
        }
      }
      bytes_out = (uint32_t)align(bytes_out + pi->length, sizeof(void *));
      bytes_in += pi->length;

      /******************************
      Each descriptor must be copied.
      ******************************/
      for (j = 0; j < pi->ndesc && STATUS_OK; ++j)
      {
        uint32_t offset;
        set_dscptrs(offset, j);
        if (offset)
        {
          status = copy_rec_dx(in_ptr + bytes_in,
                               po ? (mdsdsc_xd_t *)((char *)po + bytes_out) : 0,
                               &size_out, &size_in);
          if (po)
            po->dscptrs[j] =
                size_out ? (mdsdsc_t *)((char *)po + bytes_out) : 0;
          bytes_out = align(bytes_out + size_out, (uint32_t)sizeof(void *));
          bytes_in += size_in;
        }
      }
    }
    break;
    case CLASS_A:
    {
      uint32_t dsc_size, align_size;
      array_coeff a_tmp = {0};
      array_coeff *pi = &a_tmp;
      array_coeff *po = (array_coeff *)out_dsc_ptr;
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
      bytes_in =
          16u +
          (pi->aflags.coeff
               ? (uint32_t)sizeof(int) + (uint32_t)sizeof(int) * pi->dimct
               : 0u) +
          (pi->aflags.bounds ? (uint32_t)sizeof(int) * pi->dimct * 2u : 0u);
      dsc_size =
          (uint32_t)sizeof(mdsdsc_a_t) +
          (pi->aflags.coeff
               ? (uint32_t)sizeof(char *) + (uint32_t)sizeof(int) * pi->dimct
               : 0u) +
          (pi->aflags.bounds ? (uint32_t)sizeof(int) * pi->dimct * 2u : 0u);
      align_size = (pi->dtype == DTYPE_T) ? 1 : pi->length;
      bytes_out = dsc_size + pi->arsize + align_size;
      if (po)
      {
        memcpy(po, pi, sizeof(mdsdsc_a_t));
        for (i = 0; i < pi->dimct; i++)
        {
          if (pi->aflags.coeff)
          {
            swap(int, m() + sizeof(int) * i, po->m[i]);
            if (pi->aflags.bounds)
            {
              swap(int, m() + (pi->dimct + 2 * i) * sizeof(int),
                   po->m[pi->dimct + 2 * i]);
              swap(int, m() + (pi->dimct + 2 * i + 1) * sizeof(int),
                   po->m[pi->dimct + 2 * i + 1]);
            }
          }
        }
        po->pointer = (void *)align((intptr_t)po + dsc_size, align_size);
        memcpy(po->pointer, &in_ptr[bytes_in], pi->arsize);
        if (pi->aflags.coeff)
        {
          uint32_t offset, a0;
          set_offset(offset);
          set_a0(a0);
          po->a0 = po->pointer + (a0 - offset);
        }
#if defined(WORDS_BIGENDIAN)
        if (po->dtype != DTYPE_T && po->dtype != DTYPE_IDENT &&
            po->dtype != DTYPE_PATH)
        {
          int i;
          switch (po->length)
          {
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
            for (i = 0, ptr = (int *)po->pointer; i < (int)po->arsize;
                 i += sizeof(*ptr), ptr++)
              swap(int, (char *)ptr, *ptr);
          }
          break;
          case 8:
          {
            int64_t *ptr;
            for (i = 0, ptr = (int64_t *)po->pointer; i < (int)po->arsize;
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
      array_coeff *po = (array_coeff *)out_dsc_ptr;
      //      mdsdsc_t **pdi = (mdsdsc_t **)pi->pointer;
      mdsdsc_t **pdo = 0;
      uint32_t num_dsc;
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
      if (a_tmp.length != sizeof(void *))
      {
        a_tmp.arsize = a_tmp.arsize / a_tmp.length * (uint32_t)sizeof(void *);
        a_tmp.length = sizeof(void *);
      }
      bytes_in =
          16u +
          (pi->aflags.coeff
               ? (uint32_t)sizeof(int) + (uint32_t)sizeof(int) * pi->dimct
               : 0u) +
          (pi->aflags.bounds ? (uint32_t)sizeof(int) * pi->dimct * 2u : 0u);
      bytes_out =
          (uint32_t)sizeof(mdsdsc_a_t) +
          (pi->aflags.coeff
               ? (uint32_t)sizeof(char *) + (uint32_t)sizeof(int) * pi->dimct
               : 0u) +
          (pi->aflags.bounds ? (uint32_t)sizeof(int) * pi->dimct * 2u : 0u);
      num_dsc = pi->arsize / pi->length;
      if (po)
      {
        memcpy(po, pi, sizeof(mdsdsc_a_t));
        for (i = 0; i < pi->dimct; i++)
        {
          if (pi->aflags.coeff)
          {
            swap(int, m() + i * sizeof(int), po->m[i]);
            if (pi->aflags.bounds)
            {
              swap(int, m() + (pi->dimct + 2 * i) * sizeof(int),
                   po->m[pi->dimct + 2 * i]);
              swap(int, m() + (pi->dimct + 2 * i + 1) * sizeof(int),
                   po->m[pi->dimct + 2 * i + 1]);
            }
          }
        }
        po->pointer = (char *)po + bytes_out;
        if (pi->aflags.coeff)
        {
          uint32_t offset;
          uint32_t a0;
          set_offset(offset);
          set_a0(a0);
          po->a0 = po->pointer + (a0 - offset);
        }
        pdo = (mdsdsc_t **)po->pointer;
        po->length = sizeof(mdsdsc_t *);
        po->arsize = num_dsc * (uint32_t)sizeof(mdsdsc_t *);
      }
      bytes_out += pi->arsize;
      bytes_in += num_dsc * (uint32_t)sizeof(int);
      /******************************
      Each descriptor must be copied.
      ******************************/
      for (j = 0; j < num_dsc && STATUS_OK; ++j)
      {
        status = copy_rec_dx(in_ptr + bytes_in,
                             po ? (mdsdsc_xd_t *)((char *)po + bytes_out) : 0,
                             &size_out, &size_in);
        if (po)
          *pdo++ = size_out ? (mdsdsc_t *)((char *)po + bytes_out) : 0;
        bytes_out += size_out;
        bytes_in += size_in;
      }
    }
    break;

    case CLASS_CA:
    {
      array_coeff a_tmp = {0};
      array_coeff *pi = &a_tmp;
      array_coeff *po = (array_coeff *)out_dsc_ptr;
      // mdsdsc_t **pdi = (mdsdsc_t **)pi->pointer;
      // mdsdsc_t **pdo = 0;
      uint32_t offset;
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
      bytes_in =
          16u +
          (pi->aflags.coeff
               ? (uint32_t)sizeof(int) + (uint32_t)sizeof(int) * pi->dimct
               : 0u) +
          (pi->aflags.bounds ? (uint32_t)sizeof(int) * pi->dimct * 2u : 0u);
      bytes_out = align(
          (uint32_t)sizeof(mdsdsc_a_t) +
              (pi->aflags.coeff ? (uint32_t)sizeof(char *) +
                                      (uint32_t)sizeof(int) * pi->dimct
                                : 0u) +
              (pi->aflags.bounds ? (uint32_t)sizeof(int) * pi->dimct * 2u : 0u),
          (uint32_t)sizeof(void *));
      set_offset(offset);
      if (po)
      {
        memcpy(po, pi, sizeof(mdsdsc_a_t));
        if (pi->aflags.coeff)
        {
          uint32_t a0;
          set_a0(a0);
          po->a0 = (char *)(intptr_t)a0;
        }
        for (i = 0; i < pi->dimct; i++)
        {
          if (pi->aflags.coeff)
          {
            swap(int, m() + i * sizeof(int), po->m[i]);
            if (pi->aflags.bounds)
            {
              swap(int, m() + (pi->dimct + 2 * i) * sizeof(int),
                   po->m[pi->dimct + 2 * i]);
              swap(int, m() + (pi->dimct + 2 * i + 1) * sizeof(int),
                   po->m[pi->dimct + 2 * i + 1]);
            }
          }
        }
        // do not use align!!
        po->pointer = offset ? ((char *)po + bytes_out) : 0;

        if (po->pointer && pi->aflags.bounds)
        {
          po->a0 = (char *)(intptr_t) - (po->m[po->dimct] * po->length);
        }
      }

      /***************************
      Null pointer for shape only.
      ***************************/
      if (offset)
      {
        status = copy_rec_dx(in_ptr + bytes_in,
                             po ? (mdsdsc_xd_t *)((char *)po + bytes_out) : 0,
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
  }
  else
  {
    mdsdsc_t *po = (mdsdsc_t *)out_dsc_ptr;
    static mdsdsc_t empty = {0, DTYPE_DSC, CLASS_S, 0};
    if (po)
    {
      memcpy(po, &empty, sizeof(empty));
    }
    bytes_out = align(sizeof(empty), sizeof(void *));
    bytes_in = sizeof(int);
  }
  *b_out = bytes_out;
  *b_in = bytes_in;
  return status;
}

EXPORT int MdsSerializeDscIn(char const *in, mdsdsc_xd_t *out)
{
  l_length_t size_out, size_in;
  static const dtype_t dsc_dtype = DTYPE_DSC;
  int status = copy_rec_dx(in, 0, &size_out, &size_in);
  if (STATUS_OK && size_out)
  {
    status = MdsGet1Dx(&size_out, &dsc_dtype, out, 0);
    if (STATUS_OK)
      status = copy_rec_dx(in, (mdsdsc_xd_t *)out->pointer, &size_out, &size_in);
  }
  else
    MdsFree1Dx(out, 0);
  return status;
}

static int copy_dx_rec(const mdsdsc_t *in_ptr, char *out_ptr,
                       l_length_t *b_out, l_length_t *b_in)
{
  int status = MDSplusSUCCESS;
  unsigned bytes_out = 0, bytes_in = 0, j, size_out, size_in, num_dsc;
  if (in_ptr)
    switch (in_ptr->class)
    {
    case CLASS_S:
    case CLASS_D:
    {
      if (out_ptr)
      {
        char *inp = ((char *)in_ptr) + offset(in_ptr->pointer);
        int dscsize = 8;
        LoadShort(in_ptr->length, out_ptr);
        LoadChar(in_ptr->dtype, out_ptr + 2);
        LoadChar(in_ptr->class, out_ptr + 3);
        LoadInt(dscsize, out_ptr + 4);
        out_ptr += 8;
#ifdef WORDS_BIGENDIAN
        if (in_ptr->dtype != DTYPE_T && in_ptr->dtype != DTYPE_IDENT &&
            in_ptr->dtype != DTYPE_PATH)
        {
          switch (in_ptr->length)
          {
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
        }
        else
#endif
          memcpy(out_ptr, inp, in_ptr->length);
        out_ptr += in_ptr->length;
      }
      bytes_out = 8u + in_ptr->length;
      bytes_in = (uint32_t)sizeof(mdsdsc_t) + in_ptr->length;
    }
    break;

    case CLASS_XS:
    case CLASS_XD:
    {
      mdsdsc_xs_t *inp = (mdsdsc_xs_t *)in_ptr;
      if (out_ptr)
      {
        int dscsize = 12;
        LoadShort(in_ptr->length, out_ptr);
        LoadChar(in_ptr->dtype, out_ptr + 2);
        LoadChar(in_ptr->class, out_ptr + 3);
        LoadInt(dscsize, out_ptr + 4);
        LoadInt(inp->l_length, out_ptr + 8);
        out_ptr += 12;
        memcpy(out_ptr, ((char *)in_ptr) + offset(in_ptr->pointer),
               inp->l_length);
        out_ptr += inp->l_length;
      }
      bytes_in = (uint32_t)sizeof(mdsdsc_xs_t) + inp->l_length;
      bytes_out = 12u + inp->l_length;
    }
    break;

    case CLASS_R:
    {
      mdsdsc_r_t *inp = (mdsdsc_r_t *)in_ptr;
      char *begin = out_ptr;
      char *dscptrs = NULL;
      if (out_ptr)
      {
        int dscsize = inp->length ? 12 + inp->ndesc * 4 : 0;
        LoadShort(inp->length, out_ptr);
        LoadChar(inp->dtype, out_ptr + 2);
        LoadChar(inp->class, out_ptr + 3);
        LoadInt(dscsize, out_ptr + 4);
        LoadChar(inp->ndesc, out_ptr + 8);
        dscptrs = out_ptr + 12;
        memset(dscptrs, 0, inp->ndesc * 4u);
        out_ptr += 12u + inp->ndesc * 4u;
        if (inp->length)
        {
          memcpy(out_ptr, ((char *)inp) + offset(inp->pointer), inp->length);
#if defined(WORDS_BIGENDIAN)
          if (inp->dtype == DTYPE_FUNCTION && inp->length == 2)
          {
            short value;
            swap(short, (char *)out_ptr, value);
            memcpy(out_ptr, &value, 2);
          }
#endif
        }
        out_ptr += inp->length;
      }
      bytes_out = 12u + inp->ndesc * 4u + inp->length;
      bytes_in = (uint32_t)sizeof(mdsdsc_r_t) +
                 (inp->ndesc - 1u) * (uint32_t)sizeof(mdsdsc_t *) + inp->length;
      for (j = 0; j < inp->ndesc; j++)
      {
        if (inp->dscptrs[j])
        {
          status =
              copy_dx_rec((mdsdsc_t *)(((char *)inp) + offset(inp->dscptrs[j])),
                          out_ptr, &size_out, &size_in);
          if (out_ptr)
          {
            uint32_t poffset = (uint32_t)(out_ptr - begin);
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
      array_coeff *inp = (array_coeff *)in_ptr;
      if (out_ptr)
      {
        char *inp2 = ((char *)inp) + offset(inp->pointer);
        uint32_t dscsize =
            16u +
            (inp->aflags.coeff
                 ? (uint32_t)sizeof(int) + (uint32_t)sizeof(int) * inp->dimct
                 : 0u) +
            (inp->aflags.bounds ? (uint32_t)sizeof(int) * (inp->dimct * 2u)
                                : 0u);
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
        if (inp->aflags.coeff)
        {
          uint32_t a0 = dscsize + (offset(inp->a0) - offset(inp->pointer));
          LoadInt(a0, out_ptr);
          out_ptr += 4;
          for (j = 0; j < inp->dimct; j++)
          {
            LoadInt(inp->m[j], out_ptr);
            out_ptr += 4;
          }
          if (inp->aflags.bounds)
          {
            for (j = 0; j < inp->dimct; j++)
            {
              LoadInt(inp->m[inp->dimct + 2 * j], out_ptr);
              out_ptr += 4;
              LoadInt(inp->m[inp->dimct + 2 * j + 1], out_ptr);
              out_ptr += 4;
            }
          }
        }
#ifdef WORDS_BIGENDIAN
        if (in_ptr->dtype != DTYPE_T && in_ptr->dtype != DTYPE_IDENT &&
            in_ptr->dtype != DTYPE_PATH)
        {
          uint32_t i;
          switch (in_ptr->length)
          {
          case 2:
            for (i = 0; i < inp->arsize; i += sizeof(short))
            {
              LoadShort(inp2[i], out_ptr + i);
            }
            break;
          case 4:
            for (i = 0; i < inp->arsize; i += sizeof(int))
            {
              LoadInt(inp2[i], out_ptr + i);
            }
            break;
          case 8:
            for (i = 0; i < inp->arsize; i += in_ptr->length)
            {
              LoadQuad(inp2[i], out_ptr + i);
            }
            break;
          default:
            memcpy(out_ptr, inp2, inp->arsize);
          }
        }
        else
#endif
          memcpy(out_ptr, inp2, inp->arsize);
        out_ptr += inp->arsize;
      }
      bytes_out =
          16u +
          (inp->aflags.coeff
               ? (uint32_t)sizeof(int) + (uint32_t)sizeof(int) * inp->dimct
               : 0u) +
          (inp->aflags.bounds ? (uint32_t)sizeof(int) * inp->dimct * 2u : 0u) +
          inp->arsize;
      bytes_in =
          (uint32_t)sizeof(mdsdsc_a_t) +
          (inp->aflags.coeff
               ? (uint32_t)sizeof(char *) + (uint32_t)sizeof(int) * inp->dimct
               : 0u) +
          (inp->aflags.bounds ? (uint32_t)sizeof(int) * inp->dimct * 2u : 0u) +
          inp->arsize;
    }
    break;

    /**************************************
    For CA and APD, a0 is the offset.
    **************************************/
    case CLASS_APD:
    {
      array_coeff *inp = (array_coeff *)in_ptr;
      mdsdsc_t **dsc = (mdsdsc_t **)(((char *)in_ptr) + offset(inp->pointer));
      char *begin = out_ptr;
      char *dscptr = NULL;
      num_dsc = inp->arsize / inp->length;
      if (out_ptr)
      {
        uint32_t dscsize =
            16u +
            (inp->aflags.coeff
                 ? (uint32_t)sizeof(int) + (uint32_t)sizeof(int) * inp->dimct
                 : 0u) +
            (inp->aflags.bounds ? (uint32_t)sizeof(int) * inp->dimct * 2u : 0u);
        short length = sizeof(int);
        uint32_t arsize = (uint32_t)sizeof(int) * num_dsc;
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
        if (inp->aflags.coeff)
        {
          uint32_t a0 = dscsize + (offset(inp->a0) - offset(inp->pointer));
          LoadInt(a0, out_ptr);
          out_ptr += 4;
          for (j = 0; j < inp->dimct; j++)
          {
            LoadInt(inp->m[j], out_ptr);
            out_ptr += 4;
          }
          if (inp->aflags.bounds)
          {
            for (j = 0; j < inp->dimct; j++)
            {
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
      bytes_out = 16u +
                  (inp->aflags.coeff ? (uint32_t)sizeof(char *) +
                                           (uint32_t)sizeof(int) * inp->dimct
                                     : 0u) +
                  (inp->aflags.bounds ? (uint32_t)sizeof(int) * inp->dimct * 2u
                                      : 0u + num_dsc * 4u);
      bytes_in =
          (uint32_t)sizeof(mdsdsc_a_t) +
          (inp->aflags.coeff
               ? (uint32_t)sizeof(int) + (uint32_t)sizeof(int) * inp->dimct
               : 0u) +
          (inp->aflags.bounds ? (uint32_t)sizeof(int) * inp->dimct * 2u : 0u) +
          inp->arsize;
      /******************************
      Each descriptor must be copied.
      ******************************/
      for (j = 0; j < num_dsc; j++, bytes_in += 4)
      {
        if (offset(dsc[j]))
        {
          status = copy_dx_rec((mdsdsc_t *)(((char *)in_ptr) + offset(dsc[j])),
                               out_ptr, &size_out, &size_in);
          if (out_ptr)
          {
            uint32_t poffset = (uint32_t)(out_ptr - begin);
            LoadInt(poffset, dscptr + (j * 4));
            out_ptr += size_out;
          }
          bytes_out += size_out;
          bytes_in += size_in;
        }
        else
        {
          if (out_ptr)
          {
            uint32_t poffset = 0;
            LoadInt(poffset, dscptr + (j * 4));
            out_ptr += (uint32_t)sizeof(int);
          }
          bytes_out += (uint32_t)sizeof(int);
          bytes_in += (uint32_t)sizeof(void *);
        }
      }
    }
    break;

    case CLASS_CA:
    {
      array_coeff *inp = (array_coeff *)in_ptr;
      if (out_ptr)
      {
        uint32_t dscsize =
            inp->pointer ? 16u +
                               (inp->aflags.coeff
                                    ? (uint32_t)sizeof(int) +
                                          (uint32_t)sizeof(int) * inp->dimct
                                    : 0u) +
                               (inp->aflags.bounds
                                    ? (uint32_t)sizeof(int) * inp->dimct * 2u
                                    : 0u)
                         : 0u;
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
        if (inp->aflags.coeff)
        {
          uint32_t a0 = (uint32_t)(uintptr_t)inp->a0;
          LoadInt(a0, out_ptr);
          out_ptr += 4;
          for (j = 0; j < inp->dimct; j++)
          {
            LoadInt(inp->m[j], out_ptr);
            out_ptr += 4;
          }
          if (inp->aflags.bounds)
          {
            for (j = 0; j < inp->dimct; j++)
            {
              LoadInt(inp->m[inp->dimct + 2 * j], out_ptr);
              out_ptr += 4;
              LoadInt(inp->m[inp->dimct + 2 * j + 1], out_ptr);
              out_ptr += 4;
            }
          }
        }
      }
      bytes_out =
          16u +
          (inp->aflags.coeff
               ? (uint32_t)sizeof(int) + (uint32_t)sizeof(int) * inp->dimct
               : 0u) +
          (inp->aflags.bounds ? (uint32_t)sizeof(int) * inp->dimct * 2u : 0u);
      bytes_in =
          (uint32_t)sizeof(mdsdsc_a_t) +
          (inp->aflags.coeff
               ? (uint32_t)sizeof(char *) + (uint32_t)sizeof(int) * inp->dimct
               : 0u) +
          (inp->aflags.bounds ? (uint32_t)sizeof(int) * inp->dimct * 2u : 0u);
      /***************************
      Null pointer for shape only.
      ***************************/
      if (inp->pointer)
      {
        status =
            copy_dx_rec((mdsdsc_t *)(((char *)in_ptr) + offset(inp->pointer)),
                        out_ptr, &size_out, &size_in);
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

static int Dsc2Rec(const mdsdsc_t *inp, mdsdsc_xd_t *out_ptr,
                   arsize_t *reclen)
{
  arsize_t size_out, size_in;
  static const dtype_t b_dtype = DTYPE_B;
  int status = copy_dx_rec(inp, 0, &size_out, &size_in);
  if (STATUS_OK && size_out)
  {
    length_t nlen = 1;
    array out_template = {1, DTYPE_B, CLASS_A, 0, 0, 0, {0, 1, 1, 0, 0}, 1, 0};
    out_template.arsize = *reclen = size_out;
    status = MdsGet1DxA((mdsdsc_a_t *)&out_template, &nlen, &b_dtype, out_ptr);
    if (STATUS_OK)
    {
      memset(out_ptr->pointer->pointer, 0, size_out);
      status = copy_dx_rec((mdsdsc_t *)inp, out_ptr->pointer->pointer,
                           &size_out, &size_in);
    }
  }
  else
    MdsFree1Dx(out_ptr, NULL);
  return status;
}

static int PointerToOffset(mdsdsc_t *dsc_ptr, l_length_t *length)
{
  int status = 1;
  if ((dsc_ptr->dtype == DTYPE_DSC) && (dsc_ptr->class != CLASS_A) &&
      (dsc_ptr->class != CLASS_APD))
    status = PointerToOffset((mdsdsc_t *)dsc_ptr->pointer, length);
  if (STATUS_OK)
  {
    switch (dsc_ptr->class)
    {
    case CLASS_S:
    case CLASS_D:
      *length += (uint32_t)sizeof(mdsdsc_t) + dsc_ptr->length;
      dsc_ptr->pointer = dsc_ptr->pointer - (uintptr_t)dsc_ptr;
      break;
    case CLASS_XD:
    case CLASS_XS:
      *length +=
          (uint32_t)sizeof(mdsdsc_xd_t) + ((mdsdsc_xd_t *)dsc_ptr)->l_length;
      dsc_ptr->pointer =
          (void *)((intptr_t)dsc_ptr->pointer - (intptr_t)dsc_ptr);
      break;
    case CLASS_R:
    {
      mdsdsc_r_t *r_ptr = (mdsdsc_r_t *)dsc_ptr;
      int i;
      *length += (uint32_t)sizeof(*r_ptr) +
                 (r_ptr->ndesc - 1u) * (uint32_t)sizeof(mdsdsc_t *) +
                 r_ptr->length;
      if (r_ptr->length != 0)
        r_ptr->pointer = (void *)((intptr_t)r_ptr->pointer - (intptr_t)r_ptr);
      for (i = 0; STATUS_OK && (i < r_ptr->ndesc); i++)
        if (r_ptr->dscptrs[i] != 0)
        {
          status = PointerToOffset(r_ptr->dscptrs[i], length);
          r_ptr->dscptrs[i] =
              (mdsdsc_t *)((intptr_t)r_ptr->dscptrs[i] - (intptr_t)r_ptr);
        }
    }
    break;
    case CLASS_A:
    {
      mdsdsc_a_t *a_ptr = (mdsdsc_a_t *)dsc_ptr;
      *length +=
          (uint32_t)sizeof(mdsdsc_a_t) +
          (a_ptr->aflags.coeff ? (uint32_t)sizeof(int) * (a_ptr->dimct + 1u)
                               : 0u) +
          (a_ptr->aflags.bounds ? (uint32_t)sizeof(int) * a_ptr->dimct * 2u
                                : 0u) +
          a_ptr->arsize;
      a_ptr->pointer = (void *)((intptr_t)a_ptr->pointer - (intptr_t)a_ptr);
      if (a_ptr->aflags.coeff)
      {
        uint32_t *a0_ptr =
            (uint32_t *)((uintptr_t)a_ptr + sizeof(mdsdsc_a_t));
        *a0_ptr = *a0_ptr - (uint32_t)(uintptr_t)a_ptr;
      }
    }
    break;
    case CLASS_APD:
    {
      mdsdsc_a_t *a_ptr = (mdsdsc_a_t *)dsc_ptr;
      uint32_t i, elts = a_ptr->arsize / a_ptr->length;
      *length +=
          (uint32_t)sizeof(mdsdsc_a_t) +
          (a_ptr->aflags.coeff ? (uint32_t)sizeof(int) * (a_ptr->dimct + 1u)
                               : 0u) +
          (a_ptr->aflags.bounds ? (uint32_t)sizeof(int) * (a_ptr->dimct * 2u)
                                : 0u) +
          a_ptr->arsize;
      for (i = 0; (i < elts) && STATUS_OK; i++)
      {
        mdsdsc_t **dsc_ptr = (mdsdsc_t **)a_ptr->pointer + i;
        if (dsc_ptr && *dsc_ptr)
        {
          status = PointerToOffset(*dsc_ptr, length);
          *dsc_ptr = (mdsdsc_t *)((uintptr_t)*dsc_ptr - (uintptr_t)a_ptr);
        }
        else
        {
          status = 1;
          *dsc_ptr = 0;
        }
      }
      if (STATUS_OK)
      {
        a_ptr->pointer =
            (void *)((uintptr_t)a_ptr->pointer - (uintptr_t)a_ptr);
        if (a_ptr->aflags.coeff)
        {
          uint32_t *a0_ptr =
              (uint32_t *)((uintptr_t)a_ptr + sizeof(mdsdsc_a_t));
          *a0_ptr = *a0_ptr - (uint32_t)(uintptr_t)a_ptr;
        }
      }
    }
    break;
    case CLASS_CA:
      if (dsc_ptr->pointer)
      {
        uint32_t dummy_length;
        mdsdsc_a_t *a_ptr = (mdsdsc_a_t *)dsc_ptr;
        *length +=
            (uint32_t)sizeof(mdsdsc_a_t) +
            (a_ptr->aflags.coeff ? (uint32_t)sizeof(int) * (a_ptr->dimct + 1u)
                                 : 0u) +
            (a_ptr->aflags.bounds ? (uint32_t)sizeof(int) * a_ptr->dimct * 2u
                                  : 0u) +
            a_ptr->arsize;
        status = PointerToOffset((mdsdsc_t *)dsc_ptr->pointer, &dummy_length);
        a_ptr->pointer = (void *)((intptr_t)a_ptr->pointer - (intptr_t)a_ptr);
      }
      break;
    default:
      status = LibINVSTRDES;
      break;
    }
  }
  return status;
}

EXPORT int MdsSerializeDscOutZ(
    mdsdsc_t const *in, mdsdsc_xd_t *out, int (*fixupNid)(), void *fixupNidArg,
    int (*fixupPath)(), void *fixupPathArg, int compress, int *compressible_out,
    l_length_t *length_out, l_length_t *reclen_out, dtype_t *dtype_out,
    class_t *class_out, int altbuflen, void *altbuf, int *data_in_altbuf_out)
{
  int status;
  mdsdsc_t *out_ptr;
  mdsdsc_xd_t tempxd;
  int compressible = 0;
  l_length_t length = 0;
  l_length_t reclen = 0;
  dtype_t dtype = 0;
  class_t class = 0;
  int data_in_altbuf = 0;
  status =
      MdsCopyDxXdZ(in, out, 0, fixupNid, fixupNidArg, fixupPath, fixupPathArg);
  if (status == MdsCOMPRESSIBLE)
  {
    // changed the meaning of the compress arg from boolean to:
    // -1 -don't compress
    // 0  - use standard delta compression
    // 1.. - use the compression method with this index
    if (compress != -1)
    {
      DEFINE_COMPRESSION_METHODS

      tempxd = *out;
      out->l_length = 0;
      out->pointer = 0;
      if ((unsigned int)compress >= NUM_COMPRESSION_METHODS)
          compress = 0;
      if (compress) 
      {
          DESCRIPTOR_FROM_CSTRING(image, compression_methods[compress].image);
          DESCRIPTOR_FROM_CSTRING(method, compression_methods[compress].method);
          status = MdsCompress(&image, &method, tempxd.pointer, out);
      }
      else
      {
          status = MdsCompress(NULL, NULL, tempxd.pointer, out);
      }
      MdsFree1Dx(&tempxd, NULL);
      compressible = 0;
    }
    else
      compressible = 1;
  }
  else
    compressible = 0;
  if (STATUS_OK)
  {
    if (out->pointer && out->dtype == DTYPE_DSC)
    {
      out_ptr = out->pointer;
      dtype = (dtype_t)out_ptr->dtype;
      if ((out_ptr->class == CLASS_S || out_ptr->class == CLASS_D) &&
          out_ptr->length < altbuflen)
      {
        data_in_altbuf = 1;
        class = CLASS_S;
        length = out_ptr->length + 8u;
#ifdef WORDS_BIGENDIAN
        if (dtype != DTYPE_T && dtype != DTYPE_IDENT && dtype != DTYPE_PATH)
        {
          char *outp = (char *)altbuf;
          char *inp = out_ptr->pointer;
          switch (out_ptr->length)
          {
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
        }
        else
#endif
          memcpy(altbuf, out_ptr->pointer, out_ptr->length);
      }
      else
      {
        data_in_altbuf = 0;
        class = (out_ptr->class == CLASS_XD) ? CLASS_XS : out_ptr->class;
        length = 0;
        status = PointerToOffset(out->pointer, &length);
        if (STATUS_OK)
        {
          tempxd = *out;
          out->l_length = 0;
          out->pointer = 0;
          Dsc2Rec(tempxd.pointer, out, &reclen);
          MdsFree1Dx(&tempxd, NULL);
        }
      }
    }
    else
    {
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

EXPORT int MdsSerializeDscOut(mdsdsc_t const *in, mdsdsc_xd_t *out)
{
  return MdsSerializeDscOutZ(in, out, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0);
}
