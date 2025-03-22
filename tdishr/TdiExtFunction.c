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
/*      Tdi1ExtFunction.C
        Call a routine in an image.
        An EXTERNAL FUNCTION has pointers to
        1       descriptor of image logical name (may be null for TDISHR
   entries, default SYS$SHARE:.EXE) 2       descriptor of routine name (for
   TDISHR image these must be TDI$xxxx.) 3...    descriptors of (ndesc-2)
   arguments You must check number of arguments. Limit is 253 arguments. result
   = image->entry(in1, ...) TDI defined functions may be invoked: name (
   [PRIVATE vbl|PUBLIC vbl|expr],...) These execute functions previously defined
   by FUN [PUBLIC|PRIVATE] name ( [OPTIONAL] [IN|INOUT|OUT] vbl,...) stmt If
   unknown, then try to find file, compile it, and if the name matches, execute
   it.

        (1) image==0 && known function ? do DO_FUN(entry).
        (2) FIND_IMAGE_SYMBOL(image or "MDS$FUNCTIONS", entry) found ? do
   symbol. (3) FOPEN image->routine as routine with related specification
   image.FUN or MDS$PATH:.FUN, read file, compile, if compiled is a function and
   has same name as routine, DO_FUN(entry). (4) three strikes and you are out.

        Ken Klare, LANL P-4     (c)1989,1990,1991
        NEED we chase logical names if not in first image?
*/
#include "tdirefstandard.h"
#include <libroutines.h>
#include <mds_stdarg.h>
#include <mdsshr.h>
#include <pthread_port.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strroutines.h>
#include <tdishr_messages.h>

extern int TdiData(mdsdsc_t *, ...);
extern int TdiDoFun();

static const struct descriptor_d EMPTY_D = {0, DTYPE_T, CLASS_D, 0};

int TdiFindImageSymbol(struct descriptor_d *image, struct descriptor_d *entry,
                       int (**symbol)())
{
  return LibFindImageSymbol(image, entry, symbol);
}

extern int TdiFindSymbol();

int Tdi1ExtFunction(opcode_t opcode __attribute__((unused)), int narg,
                    struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  int status;
  struct descriptor_d image = EMPTY_D, entry = EMPTY_D;
  FREED_ON_EXIT(&image);
  FREED_ON_EXIT(&entry);
  status = MDSplusSUCCESS;
  if (list[0])
    status = TdiData(list[0], &image MDS_END_ARG);
  if (STATUS_OK)
    status = TdiData(list[1], &entry MDS_END_ARG);
  if (STATUS_NOT_OK)
    goto done;
  /**************************
  Quickly do known TDI functions.
  **************************/
  if (image.length == 0)
  {
    status =
        StrUpcase((struct descriptor *)&entry, (struct descriptor *)&entry);
    if (STATUS_OK)
      status = TdiDoFun(&entry, narg - 2, &list[2], out_ptr);

  } else {
    printf("\n");
    printf("** The EXT_FUNCTION(<library>, <function>, ...) feature has been removed.  **\n");
    printf("** Please rewrite using the BUILD_CALL (aka ->) function. **\n\n");
    status = MDSplusERROR;
  }

done:;
  FREED_NOW(&entry);
  FREED_NOW(&image);
  return status;
}
