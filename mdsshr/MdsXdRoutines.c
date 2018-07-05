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
#include <mdsshr_messages.h>
#include <mdsshr.h>
#include "mdsshrp.h"
#include <STATICdef.h>

#define LibVM_FIRST_FIT      1
#define LibVM_BOUNDARY_TAGS  1
#define LibVM_EXTEND_AREA    32
#define LibVM_TAIL_LARGE     128
#define compression_threshold 128
#define _MOVC3(a,b,c) memcpy(c,b,(size_t)(a))

inline static size_t _sizeAligned(size_t bytes) {
  size_t b_mod = bytes % sizeof(void *);
  return (b_mod == 0) ? bytes : (bytes + sizeof(void *) - b_mod);
}

inline static char *_align(char *ptr, size_t offset, size_t size) {
  uintptr_t p_mod = (uintptr_t)(ptr + offset) % size;
  return (p_mod == 0) ? ptr + offset: (ptr + offset + size - p_mod);
}

void MdsFixDscLength(struct descriptor *in);
//static void _checkAlign(struct descriptor *in);

EXPORT int MdsGet1Dx(unsigned int const *length_ptr, unsigned char const *dtype_ptr, struct descriptor_xd *dsc_ptr,
	      void **zone)
{
  int status;
  if (dsc_ptr->class == CLASS_XD) {
    if (*length_ptr != dsc_ptr->l_length) {
      if (dsc_ptr->l_length)
	status =
	  LibFreeVm(&dsc_ptr->l_length, (void *)&dsc_ptr->pointer, zone);
      else
	status = 1;
      if STATUS_OK
	status =
	  LibGetVm((unsigned int *)length_ptr, (void *)&dsc_ptr->pointer, zone);
    } else
      status = 1;
    if STATUS_OK {
      dsc_ptr->length = 0;
      dsc_ptr->l_length = *length_ptr;
      dsc_ptr->class = CLASS_XD;
      dsc_ptr->dtype = *dtype_ptr;
    }
  } else
    status = LibINVSTRDES;
  return status;
}

EXPORT int MdsFree1Dx(struct descriptor_xd *dsc_ptr, void **zone)
{
  int status;
  if (dsc_ptr->class == CLASS_XD) {
    if (dsc_ptr->pointer)
      status =
	LibFreeVm(&dsc_ptr->l_length, (void *)&dsc_ptr->pointer, zone);
    else
      status = 1;
    if STATUS_OK {
      dsc_ptr->pointer = 0;
      dsc_ptr->l_length = 0;
    }
  } else if (dsc_ptr->class == CLASS_D)
    status = LibSFree1Dd((struct descriptor *)dsc_ptr);
  else
    status = LibINVSTRDES;
  return status;
}

typedef struct _bounds {
  int l;
  int u;
} BOUNDS;

STATIC_ROUTINE struct descriptor *FixedArray();

/*-----------------------------------------------------------------
	Recursively compact all descriptors and adjust pointers.
	NIDs converted to PATHs for TREE$COPY_TO_RECORD.
	Eliminates DSC descriptors. Need DSC for classes A and APD?
-----------------------------------------------------------------*/
STATIC_ROUTINE int copy_dx(struct descriptor_xd const *in_dsc_ptr,
			   struct descriptor_xd *out_dsc_ptr,
			   unsigned int *bytes_used_ptr,
			   int (*fixup_nid) (),
			   void *fixup_nid_arg,
			   int (*fixup_path) (), void *fixup_path_arg, int *compressible)
{ int status = 1;
  unsigned int bytes = 0, j, size;
  struct descriptor *in_ptr = (struct descriptor *)in_dsc_ptr;
  unsigned int align_size;
  while (in_ptr && in_ptr->dtype == DTYPE_DSC && in_ptr->class != CLASS_APD)
    in_ptr = (struct descriptor *)in_ptr->pointer;
  if (in_ptr)
    switch (in_ptr->class) {
    case CLASS_S:
    case CLASS_D:
      {
	struct descriptor in;
	struct descriptor *po = (struct descriptor *)out_dsc_ptr;
	struct descriptor_d path = { 0, DTYPE_T, CLASS_D, 0 };
	in = *(struct descriptor *)in_ptr;
	in.class = CLASS_S;
	if (in.dtype == DTYPE_NID && fixup_nid && (*fixup_nid) (in.pointer, fixup_nid_arg, &path)) {
	  in.length = path.length;
	  in.dtype = DTYPE_PATH;
	  in.pointer = path.pointer;
	} else if (in.dtype == DTYPE_PATH && fixup_path
		   && (*fixup_path) (&in, fixup_path_arg, &path)) {
	  in.length = path.length;
	  in.pointer = path.pointer;
	}
	align_size = ((in.dtype == DTYPE_T || in.dtype == DTYPE_PATH) && in.length) ? 1 : in.length;
	if (po) {
	  *po = in;
	  po->class = CLASS_S;
	  po->pointer = in.length ? _align((char *)po, sizeof(struct descriptor), align_size) : 0;
	  if (in.length)
	    _MOVC3(in.length, (char *)in.pointer, (char *)po->pointer);
	}
	if (path.pointer)
	  StrFree1Dx(&path);
	bytes = (unsigned int)sizeof(struct descriptor_s) + in.length + align_size;
      }
      break;

    case CLASS_XS:
    case CLASS_XD:
      {
	struct descriptor_xs in;
	struct descriptor_xs *po = (struct descriptor_xs *)out_dsc_ptr;
	struct descriptor_d path = { 0, DTYPE_T, CLASS_D, 0 };
	in = *(struct descriptor_xs *)in_ptr;
	if (in.dtype == DTYPE_NID && fixup_nid && (*fixup_nid) (in.pointer, fixup_nid_arg, &path)) {
	  in.l_length = path.length;
	  in.dtype = DTYPE_PATH;
	  in.pointer = (struct descriptor *)path.pointer;
	} else if (in.dtype == DTYPE_PATH && fixup_path
		   && (*fixup_path) (&in, fixup_path_arg, &path)) {
	  in.l_length = path.length;
	  in.pointer = (struct descriptor *)path.pointer;
	}
	if (po) {
	  *po = in;
	  po->class = CLASS_XS;
	  po->pointer = (struct descriptor *)po + sizeof(in);
	  _MOVC3(in.l_length, in.pointer, po->pointer);
	}
	if (path.pointer)
	  StrFree1Dx(&path);
	bytes = (unsigned int)_sizeAligned(sizeof(struct descriptor_xs) + in.l_length);
      }
      break;

    case CLASS_R:
      {
	struct descriptor_r *pi = (struct descriptor_r *)in_ptr;
	struct descriptor_r *po = (struct descriptor_r *)out_dsc_ptr;
	bytes = (unsigned int)sizeof(struct descriptor_r) + (pi->ndesc - 1u) * (unsigned int)sizeof(struct descriptor *);
	if (po) {
	  _MOVC3(bytes, (char *)pi, (char *)po);
	  if (pi->length > 0) {
	    po->pointer = (unsigned char *)po + bytes;
	    _MOVC3(pi->length, (char *)pi->pointer, (char *)po->pointer);
	  }
	}
	bytes = (unsigned int)_sizeAligned(bytes + pi->length);
      /******************************
      Each descriptor must be copied.
      ******************************/
	for (j = 0; j < pi->ndesc && STATUS_OK; ++j)
	  if (pi->dscptrs[j]) {
	    status = copy_dx((struct descriptor_xd *)pi->dscptrs[j],
			     po ? (struct descriptor_xd *)((char *)po + bytes) : 0,
			     &size, fixup_nid, fixup_nid_arg,
			     fixup_path, fixup_path_arg, compressible);
	    if (po)
	      po->dscptrs[j] = size ? (struct descriptor *)((char *)po + bytes) : 0;
	    bytes = (unsigned int)_sizeAligned(bytes + size);
	  }
      }
      break;

    case CLASS_NCA:
      {
	array_coeff *pi = (array_coeff *) FixedArray(in_ptr);
	array_coeff *po = (array_coeff *) out_dsc_ptr;
	bytes = (unsigned int)sizeof(struct descriptor_a)
	    + (pi->aflags.coeff  ? (unsigned int)sizeof(int) * (pi->dimct + 1u) : 0u)
	    + (pi->aflags.bounds ? (unsigned int)sizeof(int) *  pi->dimct * 2u  : 0u);
	if (po) {
	  _MOVC3(bytes, (char *)pi, (char *)po);
	  po->pointer = (char *)po + bytes;
	  _MOVC3(pi->arsize, pi->pointer, po->pointer);
	  if (pi->aflags.coeff)
	    po->a0 = po->pointer + (pi->a0 - pi->pointer);
	}
	bytes = (unsigned int)_sizeAligned(bytes + pi->arsize);
	if (pi->arsize > compression_threshold)
	  *compressible = 1;
	free(pi);
      }
      break;

    case CLASS_A:
      {
	unsigned int dscsize, align_size;
	array_coeff *pi = (array_coeff *) in_ptr;
	array_coeff *po = (array_coeff *) out_dsc_ptr;
	dscsize = (unsigned int)sizeof(struct descriptor_a)
	    + (pi->aflags.coeff  ? (unsigned int)sizeof(char *) + (unsigned int)sizeof(int) * pi->dimct : 0u)
	    + (pi->aflags.bounds ? (unsigned int)sizeof(int) * pi->dimct * 2u : 0u);
	if (pi->length == 0)
	  MdsFixDscLength((struct descriptor *)pi);
	align_size = (pi->dtype == DTYPE_T || pi->length == 0) ? 1 : pi->length;
	bytes = dscsize + pi->arsize + align_size;
	if (po) {
	  _MOVC3(dscsize, (char *)pi, (char *)po);
	  po->pointer = _align((char *)po, dscsize, align_size);
	  if (pi->arsize > 0 && pi->pointer != NULL )
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
	struct descriptor_a *pi = (struct descriptor_a *)in_ptr;
	struct descriptor_a *po = (struct descriptor_a *)out_dsc_ptr;
	struct descriptor **pdi = (struct descriptor **)pi->pointer;
	struct descriptor **pdo = 0;
	unsigned int num_dsc = pi->arsize / pi->length;
	bytes = (unsigned int)sizeof(struct descriptor_a)
	    + (pi->aflags.coeff ? (unsigned int)sizeof(char *) + (unsigned int)sizeof(int) * pi->dimct : 0u)
	    + (pi->aflags.bounds ? (unsigned int)sizeof(int) * pi->dimct * 2u : 0u);
	if (po) {
	  _MOVC3(bytes, (char *)pi, (char *)po);
	  pdo = (struct descriptor **)(po->pointer = (char *)po + bytes);
	}
	bytes = (unsigned int)_sizeAligned(bytes + pi->arsize);

      /******************************
      Each descriptor must be copied.
      ******************************/
	for (j = 0; j < num_dsc && STATUS_OK; ++j) {
	  status = copy_dx((struct descriptor_xd *)*pdi++,
			   po ? (struct descriptor_xd *)((char *)po + bytes) : 0,
			   &size, fixup_nid, fixup_nid_arg, fixup_path, fixup_path_arg,
			   compressible);
	  if (po)
	    *pdo++ = size ? (struct descriptor *)((char *)po + bytes) : 0;
	  bytes = (unsigned int)_sizeAligned(bytes + size);
	}
      }
      break;

    case CLASS_CA:
      {
	struct descriptor_a *pi = (struct descriptor_a *)in_ptr;
	struct descriptor_a *po = (struct descriptor_a *)out_dsc_ptr;
	bytes = (unsigned int)_sizeAligned(sizeof(struct descriptor_a)
		+ (pi->aflags.coeff  ? (unsigned int)sizeof(char *) + (unsigned int)sizeof(int) * pi->dimct : 0u)
		+ (pi->aflags.bounds ? (unsigned int)sizeof(int) * pi->dimct * 2u : 0u));
	if (po) {
	  _MOVC3(bytes, (char *)pi, (char *)po);
	  if (pi->pointer)
	    po->pointer = _align((char *)po, bytes, sizeof(void *));
	  else
	    po->pointer = NULL;
	}

      /***************************
      Null pointer for shape only.
      ***************************/
	if (pi->pointer) {
	  status = copy_dx((struct descriptor_xd *)pi->pointer,
			   po ? (struct descriptor_xd *)(po->pointer) : 0,
			   &size, fixup_nid, fixup_nid_arg, fixup_path, fixup_path_arg,
			   compressible);
	  bytes = (unsigned int)_sizeAligned(bytes + size);
	}
      }
      break;

    default:
      status = LibINVSTRDES;
      break;
    }
  *bytes_used_ptr = bytes;
  /*  if (out_dsc_ptr) {
    _checkAlign((struct descriptor *)out_dsc_ptr);
    }*/
  return status;
}

/*----------------------------------------------------------------
	The outer routine checks size, allocates, and calls again.
	Compressible flag is set for big arrays.
*/

EXPORT int MdsCopyDxXdZ(const struct descriptor *in_dsc_ptr, struct descriptor_xd *out_dsc_ptr, void **zone,
		 int (*fixup_nid) (), void *fixup_nid_arg, int (*fixup_path) (),
		 void *fixup_path_arg)
{
  unsigned int size;
  int status;
  STATIC_CONSTANT unsigned char dsc_dtype = DTYPE_DSC;
/************************************************
* Get the total size of the thing to copy so that
* a contiguous descriptor can be allocated.
* If something to copy then do it, else clear.
************************************************/
  int compressible = 0;
  status =
      copy_dx((struct descriptor_xd *)in_dsc_ptr, 0, &size, fixup_nid, fixup_nid_arg, fixup_path,
	      fixup_path_arg, &compressible);
  if (STATUS_OK && size) {
    status = MdsGet1Dx(&size, (unsigned char *)&dsc_dtype, out_dsc_ptr, zone);
    if STATUS_OK
      status = copy_dx((struct descriptor_xd *)in_dsc_ptr,
		       (struct descriptor_xd *)out_dsc_ptr->pointer,
		       &size, fixup_nid, fixup_nid_arg, fixup_path, fixup_path_arg, &compressible);
    if (STATUS_OK && compressible)
      status = MdsCOMPRESSIBLE;
  } else
    MdsFree1Dx(out_dsc_ptr, zone);
  return status;
}

STATIC_ROUTINE struct descriptor *FixedArray(struct descriptor *in)
{

  array_coeff *a = (array_coeff *) in;
  unsigned int dsize = (unsigned int)sizeof(struct descriptor_a) + 4u + 12u * a->dimct;
  BOUNDS *bounds = (BOUNDS *) & a->m[a->dimct];
  array_coeff *answer = (array_coeff *) memcpy(malloc(dsize), a, dsize);
  answer->class = CLASS_A;
  answer->aflags.column = 1;
  answer->aflags.coeff = 1;
  answer->aflags.bounds = 1;
  int i;
  for (i = 0; i < a->dimct - 1; i++)
    answer->m[i] = a->m[i + 1] / a->m[i];
  answer->m[i] = (unsigned int)(bounds[i].u - bounds[i].l) + 1u;
  return (struct descriptor *)answer;
}

EXPORT int MdsCopyDxXd(struct descriptor const *in, struct descriptor_xd *out)
{
  return MdsCopyDxXdZ(in, out, NULL, NULL, NULL, NULL, NULL);
}

void MdsFixDscLength(struct descriptor *in)
{
  switch (in->dtype) {
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
static void _checkAlign(struct descriptor *in)
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
	printf("Unaligned! dtype=%d, class=%d, length=%d, pointer=%p (aligned=%p)\n",
	       in->dtype, in->class, in->length, in->pointer, ptr);
	exit(1);
      }
    }
  }
}
*/
