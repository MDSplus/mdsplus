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
  char *ptr = out;
  memset(out, 0, 42);
  putint32(&ptr, &in->flags);
  putint8(&ptr, &in->flags2);
  putint8(&ptr, &in->spare);
  putint64(&ptr, &in->time_inserted);
  putint32(&ptr, &in->owner_identifier);
  putint8(&ptr, &in->class);
  putint8(&ptr, &in->dtype);
  putint32(&ptr, &in->length);
  putint8(&ptr, &in->compression_method);
  putint32(&ptr, &in->status);
  if (in->flags2 & NciM_DATA_IN_ATT_BLOCK)
  {
    putint8(&ptr, &in->DATA_INFO.DATA_IN_RECORD.element_length);
    putchars(&ptr, &in->DATA_INFO.DATA_IN_RECORD.data, 11);
  }
  else if (in->flags2 & NciM_ERROR_ON_PUT)
  {
    putint32(&ptr, &in->DATA_INFO.ERROR_INFO.error_status);
    putint32(&ptr, &in->DATA_INFO.ERROR_INFO.stv);
  }
  else
  {
    putint8(&ptr, &in->DATA_INFO.DATA_LOCATION.file_level);
    putint8(&ptr, &in->DATA_INFO.DATA_LOCATION.file_version);
    putchars(&ptr, &in->DATA_INFO.DATA_LOCATION.rfa, 6);
    putint32(&ptr, &in->DATA_INFO.DATA_LOCATION.record_length);
  }
}

void TreeSerializeNciIn(const char *in, NCI *out)
{
  char *ptr = (char *)in;
  getint32(&ptr, &out->flags);
  getint8(&ptr, &out->flags2);
  getint8(&ptr, &out->spare);
  getint64(&ptr, &out->time_inserted);
  getint32(&ptr, &out->owner_identifier);
  getint8(&ptr, &out->class);
  getint8(&ptr, &out->dtype);
  getint32(&ptr, &out->length);
  getint8(&ptr, &out->compression_method);
  getint32(&ptr, &out->status);
  if (out->flags2 & NciM_DATA_IN_ATT_BLOCK)
  {
    getint8(&ptr, &out->DATA_INFO.DATA_IN_RECORD.element_length);
    getchars(&ptr, &out->DATA_INFO.DATA_IN_RECORD.data, 11);
  }
  else if (out->flags2 & NciM_ERROR_ON_PUT)
  {
    getint32(&ptr, &out->DATA_INFO.ERROR_INFO.error_status);
    getint32(&ptr, &out->DATA_INFO.ERROR_INFO.stv);
  }
  else
  {
    getint8(&ptr, &out->DATA_INFO.DATA_LOCATION.file_level);
    getint8(&ptr, &out->DATA_INFO.DATA_LOCATION.file_version);
    getchars(&ptr, &out->DATA_INFO.DATA_LOCATION.rfa, 6);
    getint32(&ptr, &out->DATA_INFO.DATA_LOCATION.record_length);
  }
}
