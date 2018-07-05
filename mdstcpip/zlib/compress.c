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
/* compress.c -- compress a memory buffer
 * Copyright (C) 1995-1998 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */

/* @(#) $Id$ */

#include "zlib.h"

/* ===========================================================================
     Compresses the source buffer into the destination buffer. The level
   parameter has the same meaning as in deflateInit.  sourceLen is the byte
   length of the source buffer. Upon entry, destLen is the total size of the
   destination buffer, which must be at least 0.1% larger than sourceLen plus
   12 bytes. Upon exit, destLen is the actual size of the compressed buffer.

     compress2 returns Z_OK if success, Z_MEM_ERROR if there was not enough
   memory, Z_BUF_ERROR if there was not enough room in the output buffer,
   Z_STREAM_ERROR if the level parameter is invalid.
*/
int ZEXPORT compress2(dest, destLen, source, sourceLen, level)
Bytef *dest;
uLongf *destLen;
const Bytef *source;
uLong sourceLen;
int level;
{
  z_stream stream;
  int err;

  stream.next_in = (Bytef *) source;
  stream.avail_in = (uInt) sourceLen;
#ifdef MAXSEG_64K
  /* Check for source > 64K on 16-bit machine: */
  if ((uLong) stream.avail_in != sourceLen)
    return Z_BUF_ERROR;
#endif
  stream.next_out = dest;
  stream.avail_out = (uInt) * destLen;
  if ((uLong) stream.avail_out != *destLen)
    return Z_BUF_ERROR;

  stream.zalloc = (alloc_func) 0;
  stream.zfree = (free_func) 0;
  stream.opaque = (voidpf) 0;

  err = deflateInit(&stream, level);
  if (err != Z_OK)
    return err;

  err = deflate(&stream, Z_FINISH);
  if (err != Z_STREAM_END) {
    deflateEnd(&stream);
    return err == Z_OK ? Z_BUF_ERROR : err;
  }
  *destLen = stream.total_out;

  err = deflateEnd(&stream);
  return err;
}

/* ===========================================================================
 */
int ZEXPORT compress(dest, destLen, source, sourceLen)
Bytef *dest;
uLongf *destLen;
const Bytef *source;
uLong sourceLen;
{
  return compress2(dest, destLen, source, sourceLen, Z_DEFAULT_COMPRESSION);
}
