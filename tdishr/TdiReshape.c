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

/* implements Reshape, Squeeze, and Flatten */

#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <tdishr.h>
#include <mdsshr.h>

#include <string.h>

int Tdi1Reshape(opcode_t opcode, int narg, struct descriptor *list[],
                struct descriptor_xd *out_ptr)
{
  if (narg < 1)
    return TdiMISS_ARG;
  int status;
  RETURN_IF_NOT_OK(TdiData(list[0], out_ptr MDS_END_ARG));
  if (out_ptr->pointer->class != CLASS_A &&
      out_ptr->pointer->class != CLASS_CA &&
      out_ptr->pointer->class != CLASS_APD)
    return TdiINVCLADSC;
  array_coeff *arr = (array_coeff *)out_ptr->pointer;
  int i;
  switch (opcode)
  {
  default:
    return TdiNO_OPC;
  case OPC_RESHAPE:
  {
    uint32_t shape[MAX_DIMS] = { 0 };
    mdsdsc_a_t dsc_shape = {
      .length = sizeof(uint32_t),
      .class = CLASS_A,
      .dtype = DTYPE_LU,
      .arsize = sizeof(shape),
      .pointer = (char *)shape,
    };

    RETURN_IF_NOT_OK(TdiConvert((mdsdsc_a_t *)list[1], (mdsdsc_a_t *)&dsc_shape));

    size_t dimct = 0;
    size_t original_count = (arr->arsize / arr->length);
    size_t new_count = 1;
    for (size_t i = 0; i < MAX_DIMS; ++i) {
      if (shape[i] == 0) {
        break;
      }

      ++dimct;
      new_count *= shape[i];
    }

    if (original_count != new_count) {
      return TdiMISMATCH;
    }

    if (dimct <= arr->dimct) {
      arr->dimct = dimct;
      for (size_t i = 0; i < dimct; ++i) {
        arr->m[i] = shape[i];
      }
    }
    else {
      // Need to resize the array descriptor to fit the new dims

      array_coeff new_array = {
        .length = arr->length,
        .dtype = arr->dtype,
        .class = arr->class,
        .pointer = arr->pointer,
        .scale = arr->scale,
        .digits = arr->digits,
        .aflags = arr->aflags,
        .dimct = dimct,
        .arsize = arr->arsize,
      };

      new_array.aflags.coeff = 1;
      new_array.dimct = dimct;
      new_array.a0 = new_array.pointer;
      for (size_t i = 0; i < dimct; ++i) {
        new_array.m[i] = shape[i];
      }

      mdsdsc_xd_t tmp_out_ptr = MDSDSC_XD_INITIALIZER;
      MdsCopyDxXd((mdsdsc_t *)&new_array, &tmp_out_ptr);

      // Use the new array, not the original one
      MdsFree1Dx(out_ptr, NULL);
      memcpy(out_ptr, &tmp_out_ptr, sizeof(mdsdsc_xd_t));
    }

    break;
  }
  case OPC_FLATTEN:
  {
    if (arr->dimct > 1)
    {
      for (i = 1; i < arr->dimct; i++)
      {
        arr->m[0] *= arr->m[i];
        arr->m[i] = 0;
      }
      arr->dimct = arr->m[0] > 0 ? 1 : 0;
      arr->aflags.bounds = 0; // discard bounds
    }
    break;
  }
  case OPC_SQUEEZE:
  {
    int j = 0;
    // calculate old bounds location
    bound_t *bounds =
        arr->aflags.bounds ? (bound_t *)&arr->m[arr->dimct] : NULL;
    for (i = 0; i < arr->dimct; i++)
    {
      if (arr->m[i] == 1)
      {
        j++;
      }
      else
      {
        arr->m[i - j] = arr->m[i];
        if (bounds)
        {
          bounds[i - j] = bounds[i];
        }
      }
    }
    arr->dimct = arr->dimct - j;
    if (bounds)
    { // move bounds
      const int end = arr->dimct * (1 + sizeof(bound_t) / sizeof(int32_t));
      for (i = arr->dimct; i < end; i++)
      {
        arr->m[i] = arr->m[i + j];
      }
    }
    break;
  }
  }
  return status;
}