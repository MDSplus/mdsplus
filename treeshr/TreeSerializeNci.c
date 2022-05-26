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
#include "treeshrp.h"
#include <mdsplus/mdsconfig.h>
#include <ncidef.h>
#include <string.h>

void TreeSerializeNciOut(const NCI *in, char *out)
{
  PACKED_NCI *ptr = (PACKED_NCI *)out;
  memset(out, 0, sizeof(PACKED_NCI));
  ptr->flags = in->flags;
  ptr->flags2 = in->flags2;
  ptr->spare = in->spare;
  ptr->time_inserted = in->time_inserted;
  ptr->owner_identifier = in->owner_identifier;
  ptr->class = in->class;
  ptr->dtype = in->dtype;
  ptr->length = in->length;
  ptr->compression_method = in->compression_method;

  if (in->flags2 & NciM_DATA_IN_ATT_BLOCK)
  {
    ptr->DATA_INFO.DATA_IN_RECORD.element_length = in->DATA_INFO.DATA_IN_RECORD.element_length;
    memcpy(ptr->DATA_INFO.DATA_IN_RECORD.data, in->DATA_INFO.DATA_IN_RECORD.data, sizeof(in->DATA_INFO.DATA_IN_RECORD.data));
  }
  else if (in->flags2 & NciM_ERROR_ON_PUT)
  {
    ptr->DATA_INFO.ERROR_INFO.error_status = in->DATA_INFO.ERROR_INFO.error_status;
    ptr->DATA_INFO.ERROR_INFO.stv = in->DATA_INFO.ERROR_INFO.stv;
  }
  else
  {
    ptr->DATA_INFO.DATA_LOCATION.file_level = in->DATA_INFO.DATA_LOCATION.file_level ;
    ptr->DATA_INFO.DATA_LOCATION.file_version = in->DATA_INFO.DATA_LOCATION.file_version ;
    memcpy(ptr->DATA_INFO.DATA_LOCATION.rfa, in->DATA_INFO.DATA_LOCATION.rfa, sizeof(in->DATA_INFO.DATA_LOCATION.rfa));
    ptr->DATA_INFO.DATA_LOCATION.record_length = in->DATA_INFO.DATA_LOCATION.record_length ;
  }
}

void TreeSerializeNciIn(const char *in, NCI *out)
{
  PACKED_NCI *ptr = (PACKED_NCI *)in; 
  out->flags = ptr->flags;
  out->flags2 = ptr->flags2;
  out->spare = ptr->spare;
  out->time_inserted = ptr->time_inserted;
  out->owner_identifier = ptr->owner_identifier;
  out->class = ptr->class;
  out->dtype = ptr->dtype;
  out->length = ptr->length;
  out->compression_method = ptr->compression_method;
  out->status = ptr->status;
  if (out->flags2 & NciM_DATA_IN_ATT_BLOCK)
  {
    out->DATA_INFO.DATA_IN_RECORD.element_length = ptr->DATA_INFO.DATA_IN_RECORD.element_length;
    memcpy(out->DATA_INFO.DATA_IN_RECORD.data, ptr->DATA_INFO.DATA_IN_RECORD.data, sizeof(ptr->DATA_INFO.DATA_IN_RECORD.data));
  }
  else if (out->flags2 & NciM_ERROR_ON_PUT)
  {
    out->DATA_INFO.ERROR_INFO.error_status = ptr->DATA_INFO.ERROR_INFO.error_status;
    out->DATA_INFO.ERROR_INFO.stv = ptr->DATA_INFO.ERROR_INFO.stv;
  }
  else
  {
    out->DATA_INFO.DATA_LOCATION.file_level = ptr->DATA_INFO.DATA_LOCATION.file_level;
    out->DATA_INFO.DATA_LOCATION.file_version = ptr->DATA_INFO.DATA_LOCATION.file_version;
    memcpy(out->DATA_INFO.DATA_LOCATION.rfa, ptr->DATA_INFO.DATA_LOCATION.rfa, sizeof(ptr->DATA_INFO.DATA_LOCATION.rfa));
    out->DATA_INFO.DATA_LOCATION.record_length = ptr->DATA_INFO.DATA_LOCATION.record_length;
  }
}
