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
/*  CMS REPLACEMENT HISTORY, Element MDSXDROUTINES.C */
/*  *12    4-SEP-1996 08:04:48 TWF "Remove libvmdef.h" */
/*  *11    5-AUG-1996 14:38:17 TWF "Fix copy_dx for unpacked descriptors" */
/*  *10    2-AUG-1996 12:28:09 TWF "Problem with long on VMS" */
/*  *9     2-AUG-1996 08:16:24 TWF "Use int instead of long" */
/*  *8    27-JUN-1996 15:43:11 TWF "Port to Unix/Windows" */
/*  *7    12-JUN-1996 10:56:40 TWF "Port to Unix/Windows" */
/*  *6    12-JUN-1996 10:56:04 TWF "Port to Unix/Windows" */
/*  *5    12-JUN-1996 10:53:52 TWF "Port to Unix/Windows" */
/*  *4    24-OCT-1995 11:32:35 TWF "Fix copy of record without any descriptors"
 */
/*  *3    19-OCT-1995 15:06:51 TWF "Add mdsxdroutines.c" */
/*  *2    19-OCT-1995 14:57:44 TWF "Add mdsxdroutines.c" */
/*  *1    19-OCT-1995 13:38:16 TWF "XD handling" */
/*  CMS REPLACEMENT HISTORY, Element MDSXDROUTINES.C */
#include "mdsshrp.h"
#include <libroutines.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <mdsshr_messages.h>
#include <stdlib.h>
#include <string.h>
#include <strroutines.h>

#define LibVM_FIRST_FIT 1
#define LibVM_BOUNDARY_TAGS 1
#define LibVM_EXTEND_AREA 32
#define LibVM_TAIL_LARGE 128
#define compression_threshold 128

inline static size_t _sizeAligned(const size_t bytes)
{
  size_t b_mod = bytes % sizeof(void *);
  return (b_mod == 0) ? bytes : (bytes + sizeof(void *) - b_mod);
}

inline static char *_align(char *const ptr, const size_t offset,
                           const size_t size)
{
  uintptr_t p_mod = (uintptr_t)(ptr + offset) % size;
  return (p_mod == 0) ? ptr + offset : (ptr + offset + size - p_mod);
}

// static void _checkAlign(mdsdsc_t *in);

EXPORT int MdsGet1Dx(const l_length_t *const length_ptr,
                     const dtype_t *const dtype_ptr, mdsdsc_xd_t *const dsc_ptr,
                     void **const zone)
{
  int status;
  if (dsc_ptr->class == CLASS_XD)
  {
    if (*length_ptr != dsc_ptr->l_length)
    {
      if (dsc_ptr->l_length)
        status = LibFreeVm(&dsc_ptr->l_length, (void *)&dsc_ptr->pointer, zone);
      else
        status = 1;
      if (STATUS_OK)
        status =
            LibGetVm((uint32_t *)length_ptr, (void *)&dsc_ptr->pointer, zone);
    }
    else
      status = 1;
    if (STATUS_OK)
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

EXPORT int MdsFree1Dx(mdsdsc_xd_t *const dsc_ptr, void **const zone)
{
  int status;
  if (dsc_ptr->class == CLASS_XD)
  {
    if (dsc_ptr->pointer)
      status = LibFreeVm(&dsc_ptr->l_length, (void *)&dsc_ptr->pointer, zone);
    else
      status = 1;
    if (STATUS_OK)
    {
      dsc_ptr->pointer = 0;
      dsc_ptr->l_length = 0;
    }
  }
  else if (dsc_ptr->class == CLASS_D)
    status = StrFree1Dx((mdsdsc_d_t *)dsc_ptr);
  else
    status = LibINVSTRDES;
  return status;
}

static mdsdsc_t *FixedArray();
void MdsFixDscLength(mdsdsc_t *const in);

/*-----------------------------------------------------------------
        Recursively compact all descriptors and adjust pointers.
        NIDs converted to PATHs for TREE$COPY_TO_RECORD.
        Eliminates DSC descriptors. Need DSC for classes A and APD?
-----------------------------------------------------------------*/
static int copy_dx(const mdsdsc_xd_t *const in_dsc_ptr,
                   mdsdsc_xd_t *const out_dsc_ptr,
                   uint32_t *const bytes_used_ptr, int (*const fixup_nid)(),
                   void *const fixup_nid_arg, int (*const fixup_path)(),
                   void *const fixup_path_arg, int *const compressible)
{
  int status = 1;
  uint32_t bytes = 0, j, size;
  mdsdsc_t *in_ptr = (mdsdsc_t *)in_dsc_ptr;
  uint32_t align_size;
  while (in_ptr && in_ptr->dtype == DTYPE_DSC && in_ptr->class != CLASS_APD)
    in_ptr = (mdsdsc_t *)in_ptr->pointer;
  if (in_ptr)
    switch (in_ptr->class)
    {
    case CLASS_S:
    case CLASS_D:
    {
      mdsdsc_t in;
      mdsdsc_t *po = (mdsdsc_t *)out_dsc_ptr;
      mdsdsc_d_t path = {0, DTYPE_T, CLASS_D, 0};
      in = *(mdsdsc_t *)in_ptr;
      in.class = CLASS_S;
      if (in.dtype == DTYPE_NID && fixup_nid &&
          (*fixup_nid)(in.pointer, fixup_nid_arg, &path))
      {
        in.length = path.length;
        in.dtype = DTYPE_PATH;
        in.pointer = path.pointer;
      }
      else if (in.dtype == DTYPE_PATH && fixup_path &&
               (*fixup_path)(&in, fixup_path_arg, &path))
      {
        in.length = path.length;
        in.pointer = path.pointer;
      }
      align_size =
          ((in.dtype == DTYPE_T || in.dtype == DTYPE_PATH) && in.length)
              ? 1
              : in.length;
      if (po)
      {
        *po = in;
        po->class = CLASS_S;
        if (in.length)
        {
          po->pointer = _align((char *)po, sizeof(mdsdsc_t), align_size);
          memcpy(po->pointer, in.pointer, in.length);
        }
        else
          po->pointer = NULL;
      }
      if (path.pointer)
        StrFree1Dx(&path);
      bytes = (uint32_t)sizeof(mdsdsc_s_t) + in.length + align_size;
    }
    break;

    case CLASS_XS:
    case CLASS_XD:
    {
      mdsdsc_xs_t in;
      mdsdsc_xs_t *po = (mdsdsc_xs_t *)out_dsc_ptr;
      mdsdsc_d_t path = {0, DTYPE_T, CLASS_D, 0};
      in = *(mdsdsc_xs_t *)in_ptr;
      if (in.dtype == DTYPE_NID && fixup_nid &&
          (*fixup_nid)(in.pointer, fixup_nid_arg, &path))
      {
        in.l_length = path.length;
        in.dtype = DTYPE_PATH;
        in.pointer = (mdsdsc_t *)path.pointer;
      }
      else if (in.dtype == DTYPE_PATH && fixup_path &&
               (*fixup_path)(&in, fixup_path_arg, &path))
      {
        in.l_length = path.length;
        in.pointer = (mdsdsc_t *)path.pointer;
      }
      if (po)
      {
        *po = in;
        po->class = CLASS_XS;
        po->pointer = (mdsdsc_t *)po + sizeof(in);
        memcpy(po->pointer, in.pointer, in.l_length);
      }
      if (path.pointer)
        StrFree1Dx(&path);
      bytes = (uint32_t)_sizeAligned(sizeof(mdsdsc_xs_t) + in.l_length);
    }
    break;

    case CLASS_R:
    {
      mdsdsc_r_t *pi = (mdsdsc_r_t *)in_ptr;
      mdsdsc_r_t *po = (mdsdsc_r_t *)out_dsc_ptr;
      bytes = (uint32_t)sizeof(mdsdsc_r_t) +
              (pi->ndesc - 1u) * (uint32_t)sizeof(mdsdsc_t *);
      if (po)
      {
        memcpy((char *)po, (char *)pi, bytes);
        if (pi->length > 0)
        {
          po->pointer = (unsigned char *)po + bytes;
          memcpy((char *)po->pointer, (char *)pi->pointer, pi->length);
        }
      }
      bytes = (uint32_t)_sizeAligned(bytes + pi->length);
      /******************************
      Each descriptor must be copied.
      ******************************/
      for (j = 0; j < pi->ndesc && STATUS_OK; ++j)
        if (pi->dscptrs[j])
        {
          status = copy_dx((mdsdsc_xd_t *)pi->dscptrs[j],
                           po ? (mdsdsc_xd_t *)((char *)po + bytes) : 0, &size,
                           fixup_nid, fixup_nid_arg, fixup_path, fixup_path_arg,
                           compressible);
          if (po)
            po->dscptrs[j] = size ? (mdsdsc_t *)((char *)po + bytes) : 0;
          bytes = (uint32_t)_sizeAligned(bytes + size);
        }
    }
    break;

    case CLASS_NCA:
    {
      array_coeff *pi = (array_coeff *)FixedArray(in_ptr);
      array_coeff *po = (array_coeff *)out_dsc_ptr;
      bytes =
          (uint32_t)sizeof(mdsdsc_a_t) +
          (pi->aflags.coeff ? (uint32_t)sizeof(int) * (pi->dimct + 1u) : 0u) +
          (pi->aflags.bounds ? (uint32_t)sizeof(int) * pi->dimct * 2u : 0u);
      if (po)
      {
        memcpy((char *)po, (char *)pi, bytes);
        po->pointer = (char *)po + bytes;
        memcpy(po->pointer, pi->pointer, pi->arsize);
        if (pi->aflags.coeff)
          po->a0 = po->pointer + (pi->a0 - pi->pointer);
      }
      bytes = (uint32_t)_sizeAligned(bytes + pi->arsize);
      if (pi->arsize > compression_threshold)
        *compressible = 1;
      free(pi);
    }
    break;

    case CLASS_A:
    {
      uint32_t dscsize, align_size;
      array_coeff *pi = (array_coeff *)in_ptr;
      array_coeff *po = (array_coeff *)out_dsc_ptr;
      dscsize =
          (uint32_t)sizeof(mdsdsc_a_t) +
          (pi->aflags.coeff
               ? (uint32_t)sizeof(char *) + (uint32_t)sizeof(int) * pi->dimct
               : 0u) +
          (pi->aflags.bounds ? (uint32_t)sizeof(int) * pi->dimct * 2u : 0u);
      if (pi->length == 0)
        MdsFixDscLength((mdsdsc_t *)pi);
      align_size = (pi->dtype == DTYPE_T || pi->length == 0) ? 1 : pi->length;
      bytes = dscsize + pi->arsize + align_size;
      if (po)
      {
        memcpy((char *)po, (char *)pi, dscsize);
        po->pointer = _align((char *)po, dscsize, align_size);
        if (pi->arsize > 0 && pi->pointer != NULL)
          memcpy(po->pointer, pi->pointer, pi->arsize);
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
      mdsdsc_a_t *pi = (mdsdsc_a_t *)in_ptr;
      mdsdsc_a_t *po = (mdsdsc_a_t *)out_dsc_ptr;
      mdsdsc_t **pdi = (mdsdsc_t **)pi->pointer;
      mdsdsc_t **pdo = 0;
      uint32_t num_dsc = pi->arsize / pi->length;
      bytes = (uint32_t)sizeof(mdsdsc_a_t) +
              (pi->aflags.coeff ? (uint32_t)sizeof(char *) +
                                      (uint32_t)sizeof(int) * pi->dimct
                                : 0u) +
              (pi->aflags.bounds ? (uint32_t)sizeof(int) * pi->dimct * 2u : 0u);
      if (po)
      {
        memcpy((char *)po, (char *)pi, bytes);
        pdo = (mdsdsc_t **)(po->pointer = (char *)po + bytes);
      }
      bytes = (uint32_t)_sizeAligned(bytes + pi->arsize);

      /******************************
      Each descriptor must be copied.
      ******************************/
      for (j = 0; j < num_dsc && STATUS_OK; ++j)
      {
        status = copy_dx((mdsdsc_xd_t *)*pdi++,
                         po ? (mdsdsc_xd_t *)((char *)po + bytes) : 0, &size,
                         fixup_nid, fixup_nid_arg, fixup_path, fixup_path_arg,
                         compressible);
        if (po)
          *pdo++ = size ? (mdsdsc_t *)((char *)po + bytes) : 0;
        bytes = (uint32_t)_sizeAligned(bytes + size);
      }
    }
    break;

    case CLASS_CA:
    {
      mdsdsc_a_t *pi = (mdsdsc_a_t *)in_ptr;
      mdsdsc_a_t *po = (mdsdsc_a_t *)out_dsc_ptr;
      bytes = (uint32_t)_sizeAligned(
          sizeof(mdsdsc_a_t) +
          (pi->aflags.coeff
               ? (uint32_t)sizeof(char *) + (uint32_t)sizeof(int) * pi->dimct
               : 0u) +
          (pi->aflags.bounds ? (uint32_t)sizeof(int) * pi->dimct * 2u : 0u));
      if (po)
      {
        memcpy((char *)po, (char *)pi, bytes);
        if (pi->pointer)
          po->pointer = _align((char *)po, bytes, sizeof(void *));
        else
          po->pointer = NULL;
      }

      /***************************
      Null pointer for shape only.
      ***************************/
      if (pi->pointer)
      {
        status =
            copy_dx((mdsdsc_xd_t *)pi->pointer,
                    po ? (mdsdsc_xd_t *)(po->pointer) : 0, &size, fixup_nid,
                    fixup_nid_arg, fixup_path, fixup_path_arg, compressible);
        bytes = (uint32_t)_sizeAligned(bytes + size);
      }
    }
    break;

    default:
      status = LibINVSTRDES;
      break;
    }
  *bytes_used_ptr = bytes;
  /*  if (out_dsc_ptr) {
    _checkAlign((mdsdsc_t *)out_dsc_ptr);
    }*/
  return status;
}

/*----------------------------------------------------------------
        The outer routine checks size, allocates, and calls again.
        Compressible flag is set for big arrays.
*/

EXPORT int MdsCopyDxXdZ(const mdsdsc_t *const in_dsc_ptr,
                        mdsdsc_xd_t *const out_dsc_ptr, void **const zone,
                        int (*const fixup_nid)(), void *const fixup_nid_arg,
                        int (*const fixup_path)(), void *const fixup_path_arg)
{
  uint32_t size;
  static dtype_t dsc_dtype = DTYPE_DSC;
  /************************************************
   * Get the total size of the thing to copy so that
   * a contiguous descriptor can be allocated.
   * If something to copy then do it, else clear.
   ************************************************/
  int compressible = 0;
  int status =
      copy_dx((mdsdsc_xd_t *)in_dsc_ptr, 0, &size, fixup_nid, fixup_nid_arg,
              fixup_path, fixup_path_arg, &compressible);
  if (STATUS_OK && size)
  {
    status = MdsGet1Dx(&size, &dsc_dtype, out_dsc_ptr, zone);
    if (STATUS_OK)
      status = copy_dx((mdsdsc_xd_t *)in_dsc_ptr,
                       (mdsdsc_xd_t *)out_dsc_ptr->pointer, &size, fixup_nid,
                       fixup_nid_arg, fixup_path, fixup_path_arg, &compressible);
    if (STATUS_OK && compressible)
      status = MdsCOMPRESSIBLE;
  }
  else
    MdsFree1Dx(out_dsc_ptr, zone);
  return status;
}

static mdsdsc_t *FixedArray(const mdsdsc_t *const in)
{

  array_coeff *a = (array_coeff *)in;
  uint32_t dsize = (uint32_t)sizeof(mdsdsc_a_t) + 4u + 12u * a->dimct;
  bound_t *bounds = (bound_t *)&a->m[a->dimct];
  array_coeff *answer = (array_coeff *)memcpy(malloc(dsize), a, dsize);
  answer->class = CLASS_A;
  answer->aflags.column = 1;
  answer->aflags.coeff = 1;
  answer->aflags.bounds = 1;
  int i;
  for (i = 0; i < a->dimct - 1; i++)
    answer->m[i] = a->m[i + 1] / a->m[i];
  answer->m[i] = (uint32_t)(bounds[i].u - bounds[i].l) + 1u;
  return (mdsdsc_t *)answer;
}

EXPORT int MdsCopyDxXd(const mdsdsc_t *const in, mdsdsc_xd_t *const out)
{
  return MdsCopyDxXdZ(in, out, NULL, NULL, NULL, NULL, NULL);
}

void MdsFixDscLength(mdsdsc_t *const in)
{
  switch (in->dtype)
  {
  default:
    break;
  case DTYPE_B:
  case DTYPE_BU:
    in->length = 1;
    break;
  case DTYPE_W:
  case DTYPE_WU:
    in->length = 2;
    break;
  case DTYPE_L:
  case DTYPE_LU:
  case DTYPE_F:
  case DTYPE_FS:
    in->length = 4;
    break;
  case DTYPE_D:
  case DTYPE_G:
  case DTYPE_FT:
  case DTYPE_Q:
  case DTYPE_QU:
    in->length = 8;
    break;
  case DTYPE_O:
  case DTYPE_OU:
    in->length = 16;
    break;
  }
}
/*
static void _checkAlign(mdsdsc_t *in)
{
  if (in->pointer) {
    size_t aligned_with = 1;
    switch (in->dtype) {
    case DTYPE_B:
    case DTYPE_BU:
    case DTYPE_T:
    case DTYPE_PATH:
      aligned_with = 1;
      break;
    case DTYPE_W:
    case DTYPE_WU:
      aligned_with = 2;
      break;
    case DTYPE_L:
    case DTYPE_LU:
    case DTYPE_F:
    case DTYPE_FS:
    case DTYPE_NID:
      aligned_with = 4;
      break;
    case DTYPE_D:
    case DTYPE_G:
    case DTYPE_FT:
    case DTYPE_Q:
    case DTYPE_QU:
      aligned_with = 8;
      break;
    case DTYPE_O:
    case DTYPE_OU:
      aligned_with = 16;
    break;
    }
    if (aligned_with != 1) {
      char *ptr = _align(in->pointer, 0, aligned_with);
      if ( ptr != in->pointer ) {
        printf("Unaligned! dtype=%d, class=%d, length=%d, pointer=%p
(aligned=%p)\n", in->dtype, in->class, in->length, in->pointer, ptr); exit(1);
      }
    }
  }
}
*/
